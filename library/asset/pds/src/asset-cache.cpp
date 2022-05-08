// below is concept/future-work for improving disk I/O performance with
//   puleDsValue, albeit I think this might be better possibly served in a
//   different library

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
