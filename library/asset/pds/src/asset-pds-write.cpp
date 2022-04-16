#include <pulchritude-asset/pds.h>

#include <string>

namespace {

void addTab(
  PuleAssetPdsWriteInfo info,
  int32_t const tabLevel,
  std::string & out
) {
  if (!info.prettyPrint) {
    return;
  }
  // put out new line before tabbing, unless we are already on a new line
  if (out != "" && out.back() != '\n') {
    out += "\n";
  }

  for (int32_t it = 0; it < tabLevel; ++ it) {
    for (uint32_t itspace = 0; itspace < info.spacesPerTab; ++ itspace) {
      out += " ";
    }
  }
}

void pdsIterateWriteToStream(
  PuleAssetPdsWriteInfo const info,
  int32_t const tabLevel,
  std::string & out,
  bool firstRun = true
) {
  if (puleDsIsI64(info.head)) {
    out += std::to_string(puleDsAsI64(info.head));
    out += info.prettyPrint ? "," : ",";
  }
  else
  if (puleDsIsF64(info.head)) {
    out += std::to_string(puleDsAsF64(info.head));
    out += info.prettyPrint ? "," : ",";
  }
  else
  if (puleDsIsBool(info.head)) {
    out += std::to_string(puleDsAsBool(info.head));
    out += info.prettyPrint ? "," : ",";
  }
  else
  if (puleDsIsString(info.head)) {
    PuleStringView const stringView = puleDsAsString(info.head);
    out += '"';
    for (size_t it = 0; it < stringView.len; ++ it) {
      auto const ch = stringView.contents[it];
      if (ch == '\n' || ch == '\r') {
        out += "\\n";
      } else if (ch == '\t') {
        out += "\\t";
      } else {
        out += ch;
      }
    }
    out += '"';
    out += info.prettyPrint ? "," : ",";
  }
  else
  if (puleDsIsArray(info.head)) {
    PuleDsValueArray const array = puleDsAsArray(info.head);
    out += info.prettyPrint ? "[" : "[";
    size_t intsHit = 0;
    for (size_t it = 0; it < array.length; ++ it) {
      PuleAssetPdsWriteInfo childInfo = info;
      childInfo.head = array.values[it];

      // dependent on type, we might want to flatten the list out a bit
      if (puleDsIsI64(array.values[it]) || puleDsIsF64(array.values[it])) {
        intsHit = (intsHit+1)%8;
        if (it == 0 || intsHit % 8 == 0) {
          addTab(info, tabLevel+1, out);
        }
        // add space between commas
        if (info.prettyPrint && it > 0 && it % 8 != 0) {
          out += " ";
        }
      } else {
        intsHit = 0;
        addTab(info, tabLevel+1, out);
      }

      pdsIterateWriteToStream(childInfo, tabLevel+1, out, false);
    }
    addTab(info, tabLevel, out);
    out += info.prettyPrint ? "]," : "]";
  }
  else
  if (puleDsIsObject(info.head)) {
    PuleDsValueObject const object = puleDsAsObject(info.head);
    if (!firstRun) {
      out += info.prettyPrint ? "{" : "{";
    }
    for (size_t it = 0; it < object.length; ++ it) {
      addTab(info, tabLevel+1, out);
      // write label
      PuleStringView const stringView = object.labels[it];
      out += std::string_view(stringView.contents, stringView.len);
      out += info.prettyPrint ? ":" : ": ";

      // write value
      PuleAssetPdsWriteInfo childInfo = info;
      childInfo.head = object.values[it];
      pdsIterateWriteToStream(childInfo, tabLevel+1, out, false);
    }
    if (!firstRun) {
      addTab(info, tabLevel, out);
      out += info.prettyPrint ? "}," : "}";
    }
  }
  else {
    puleLogError("Could not get underlying type for: %d", info.head.id);
  }
}

} // namespace


extern "C" {

void puleAssetPdsWriteToStream(
  PuleStreamWrite const stream,
  PuleAssetPdsWriteInfo const writeInfo
) {
  if (writeInfo.head.id == 0) {
    return;
  }

  // TODO dont use string just pass stream
  std::string out;
  ::pdsIterateWriteToStream(writeInfo, writeInfo.initialTabLevel-1, out);
  out += "\n";
  puleStreamWriteBytes(
    stream,
    reinterpret_cast<uint8_t const * >(out.c_str()),
    out.size()
  );
}

void puleAssetPdsWriteToFile(
  PuleDsValue const head,
  char const * const filename,
  PuleError * const error
) {
  PuleFile const file = (
    puleFileOpen(
      filename,
      PuleFileDataMode_text,
      PuleFileOpenMode_writeOverwrite,
      error
    )
  );
  if (puleErrorConsume(error) > 0) {
    PULE_error(PuleErrorAssetPds_decode, "failed to open file");
    return;
  }
  uint8_t streamStorage[512];
  PuleArrayViewMutable const streamStorageView = {
    .data = &streamStorage[0],
    .elementStride = sizeof(uint8_t),
    .elementCount = 512,
  };
  PuleStreamWrite const stream = puleFileStreamWrite(file, streamStorageView);

  puleAssetPdsWriteToStream(
    stream,
    PuleAssetPdsWriteInfo {
      .head = head,
      .prettyPrint = false, .spacesPerTab = 2, .initialTabLevel = 0,
    }
  );

  puleFileClose(file);
  puleStreamWriteDestroy(stream);
}

void puleAssetPdsWriteToStdout(PuleDsValue const head) {
  PuleStreamWrite stdoutWrite = puleStreamStdoutWrite();
  puleAssetPdsWriteToStream(
    stdoutWrite,
    PuleAssetPdsWriteInfo {
      .head = head,
      .prettyPrint = true,
      .spacesPerTab = 2,
      .initialTabLevel = 1,
    }
  );
  puleStreamWriteDestroy(stdoutWrite);
}

typedef struct {
  PuleStringView filepath;

  // allows PDS to be cached such that the PDS is only written-to-disk &
  //  freed automatically. You *must* still call pdsDestroy
  bool enableCaching; // PULE_attributeDefaultValue(false)
} PuleAssetPdsOpenFromDiskInfo;

namespace {

struct CachedPdsFile {
  PuleDsValue value;
  size_t hash;
  int32_t openFiles;
  float elapsedMsSinceLastFileClosed;
};

std::unordered_map<size_t, CachedPdsFile> cachedFiles;
// I don't iterate this, so this might actually be a good use of dequeue
//  --- I do iterate this when I open a file that was at 0 openFiles
//  --- but it's possible to accelerate it with iterator possibly anyway
std::vector<CachedPdsFile *> closableCachedFilesLinear;

bool fileManagerSystemActive = false;

struct CachedPdsFileAllocatorOverrideUserdata {
  size_t filepathHash;
  void (*originalDeallocateFn)(void * const, void * const);
  void * originalUserdata;
};

} // namespace

void example() {
  a = puleAssetPdsOpenFromDisk({...}, err);
  b = puleAssetPdsOpenFromDisk({...}, err);
  assert(b.id == a.id)
  // TODO figure out how to not destroy 'a' but decrement the 'ref-count'
  puleDsDestroy(a);
  PuleString strB = puleAssetPdsToString(b);
  PULE_assert(strB.contents != '\0' && strB.len > 1);
  puleDsDestroy(b);
  // pass = no memory leaks
}

extern "C" {

void puleAssetPdsFileCacheSystemUpdate(PuleTimeMs const deltatime) {
  if (closableCachedFilesLinear.size() == 0) {
    return;
  }
  // only check first file
  auto & fileCheck = closableCachedFilesLinear[0];
  fileCheck.elapsedMsSinceLastFileClosed += deltatime;
  if (fileCheck.elapsedMsSinceLastFileClosed >= 10.0f) {
    puleDsDestroy(fileCheck.value);
    cachedFiles.erase(fileCheck.hash);
    closableCachedFilesLinear.erase(closableCachedFilesLinear.begin());
  }
}

static void cachedPdsFileAllocatorOverrideDeallocate(
  void * const address,
  void * const userdata
) {
  auto const overrideAlloc = (
    reinterpret_cast<CachedPdsFileAllocatorOverrideUserdata *>(userdata)
  );

  // decrement cached filesystem
  if (cachedFiles
  PuleDsValue
  ::cachedFiles(

  // dealloc original memory
  overrideAlloc.originalDeallocateFn(address, overrideAlloc.originalUserdata);

  // deallocate userdata
  overrideAlloc.originalDeallocateFn(
    overrideAlloc,
    overrideAlloc.originalUserdata
  );
}

static PuleAllocator cachedPdsFileAllocatorOverride(
  size_t const filepathHash,
  PuleAllocator const originalAllocator
) {
  auto const newUserData = (
    puleAllocate(
      originalAllocator,
      sizeof(CachedPdsFileAllocatorOverrideUserdata)
    )
  );
  newUserData->filepathHash = filepathHash;
  newUserData->originalDeallocateFn = originalAllocator.deallocateFn;
  newUserData->originalUserdata = originaAllocator.userdata;
  auto const newAllocator = PuleAllocator {
    .allocateFn = originalAllocator.allocateFn,
    .reallocFn = originalAllocator.reallocFn,
    .deallocateFn = &cachedPdsFileAllocatorOverrideDeallocate,
    .userdata = newUserData,
  };
}

PuleDsValue puleAssetPdsOpenFromDisk(
  PuleAssetPdsOpenFromDiskInfo const info,
  PuleError * const error
) {
  // TODO need to resolve filepath to a single path to reduce false-negatives:
  //   - symbolic links are followed
  //   - redundant pathing "./a.pds" "../bin//a.pds" etc resolves to "a.pds"
  size_t const filepathHash = puleStringViewHash(info.filepath);
  auto const cachedValuePtr = ::cachedFiles.find(filepathHash);
  if (cachedValuePtr != ::cachedFiles.end()) {
    cachedValuePtr->second.openFiles += 1;
    return cacheValuePtr->second.value;
  }

  PuleDsValue const value = (
    puleAssetPdsOpenFromRvalStream(puleOpenFile(info.filepath..))
  );

  ::cachedFiles.emplace(filepathHash, value);

  return value;
}

}

} // C
