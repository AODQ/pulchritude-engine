#include <pulchritude-asset/pds.h>

#include <cstring>
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
    out += info.prettyPrint ? ",\n" : ",";
  }
  else
  if (puleDsIsArray(info.head)) {
    PuleDsValueArray const array = puleDsAsArray(info.head);
    out += info.prettyPrint ? "[\n" : "[";
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
    out += info.prettyPrint ? "],\n" : "],";
  }
  else
  if (puleDsIsObject(info.head)) {
    PuleDsValueObject const object = puleDsAsObject(info.head);
    if (!firstRun) {
      out += info.prettyPrint ? "{\n" : "{";
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
      out += info.prettyPrint ? "},\n" : "},";
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

  puleStreamWriteDestroy(stream);
  puleFileClose(file);
}

void puleAssetPdsWriteToStdout(PuleDsValue const head) {
  PuleStreamWrite stdoutWrite = puleStreamStdoutWrite();
  puleAssetPdsWriteToStream(
    stdoutWrite,
    PuleAssetPdsWriteInfo {
      .head = head,
      .prettyPrint = true,
      .spacesPerTab = 2,
      .initialTabLevel = 0,
    }
  );
  puleStreamWriteDestroy(stdoutWrite);
}

} // C
