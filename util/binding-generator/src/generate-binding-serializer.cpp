#include "binding-generator.hpp"

// this writes out simple POD struct serialization by using
//   puleDsStructSerialize and puleDsStructDeserialize

static PuleFileStream createFile(GenerateBindingInfo & info) {
  static std::string prefix = "../../../../../library/";
  std::string path = (
    prefix + std::string(info.path.contents) + "/src/autogen-serializer.inl"
  );
  printf("path: %s\n", path.c_str());
  auto fileOut = (
    puleFileStreamWriteOpen(
      puleCStr(path.c_str()), PuleFileDataMode_text
    )
  );
  info.output = puleFileStreamWriter(fileOut);
  return fileOut;
}

std::string structLowered(auto & str) {
  std::string a = str.name;
  a[0] = std::tolower(a[0]);
  return a;
}

void writeStructFields(PuleStreamWrite const & out, BindingStruct const & bs) {
  auto const & write = puleStreamWriteStrFormat;
  write(
    out,
    "    static PuleDsStructField const %s_fields[] = {\n",
    structLowered(bs).c_str()
  );

  // TODO instead can use puleDtLabel function when it's implemented
  //      (requires auto-generated Label functions from enums)
  static std::unordered_map<std::string, std::string> strToType = {
    {"int32_t", "i32"},     {"int8_t", "i8"},       {"int16_t", "i16"},
    {"int64_t", "i64"},     {"ptrdiff_t", "i64"},   {"uint32_t", "u32"},
    {"uint8_t", "u8"},      {"uint16_t", "u16"},    {"uint64_t", "u64"},
    {"size_t", "u64"},      {"char", "i8"},         {"bool", "bool"},
    {"float", "f32"},       {"double", "f64"},      {"PuleF32v2", "f32v2"},
    {"PuleF32v3", "f32v3"}, {"PuleF32v4", "f32v4"}, {"PuleF32m44", "f32m44"},
    {"PuleF64v2", "f64v2"}, {"PuleF64v3", "f64v3"}, {"PuleF64v4", "f64v4"},
  };

  for (auto const & field : bs.fields) {
    auto & type = field.type;
    PULE_assert(type.type == BindingTypeType::identifier);

    if (type.name == "std.vector") {
      // TODO not possible yet, maybe ever?
      PULE_assert(false && "std.vector serialization not supported");
    }

    auto typeIt = strToType.find(type.name);
    PULE_assert(typeIt != strToType.end() && "unsupported type");

    std::string fieldCount = "1";
    if (type.name == "std.array") {
      fieldCount = type.templateParams[1].name;
    }

    write(out, "      {\n");
    write(out, "        .dt = PuleDt_%s,\n", typeIt->second.c_str());
    write(
      out,
      "        .fieldByteOffset = offsetof(%s, %s),\n",
      bs.name.c_str(), field.name.c_str()
    );
    write(out, "        .fieldCount = %s,\n", fieldCount.c_str());
    write(out, "      },\n");
  }

  write(out, "      { PuleDt_ptr, 0, 0 },\n");

  write(out, "    };\n");
}

void writeDeserialized(
  PuleStreamWrite const & out,
  BindingStruct const & bs
) {
  auto const & write = puleStreamWriteStrFormat;
  write(out, "extern \"C\" {\n");

  write(
    out,
    "void %sDeserialize(void * const data, PuleDsValue const dsValue) {\n",
    structLowered(bs).c_str()
  );

  write(
    out, "  puleDsStructDeserialize(dsValue, %s, data);\n",
    (structLowered(bs) + "_fields").c_str()
  );
  write(out, "}\n");

  write(out, "} // extern C\n");
}

void writeSerialized(
  PuleStreamWrite const & out,
  BindingStruct const & bs
) {
  auto const & write = puleStreamWriteStrFormat;
  write(out, "extern \"C\" {\n");

  write(
    out,
    (
      "  void %sSerialize(void const * const result, PuleDsValue const dsValue)"
      " {\n"
    ),
    structLowered(bs).c_str()
  );

  write(
    out,
    "    puleDsStructSerialize(dsValue, puleAllocateDefault(), %s, result);\n",
    (structLowered(bs) + "_fields").c_str()
  );

  write(out, "  }\n");
  write(out, "} // extern C\n");
}

void generateBindingFileSerializer(GenerateBindingInfo const & inforef) {
  GenerateBindingInfo info = inforef;
  auto const & file = info.file;

  if (file.serializedEntities.size() == 0) {
    return;
  }

  PuleFileStream fileStream = createFile(info);
  puleScopeExit { puleFileStreamClose(fileStream); };

  auto const & out = info.output;
  auto const & write = puleStreamWriteStrFormat;

  write(out, "/* auto generated file %s */\n", info.path.contents);
  write(out, "/* include this in src file once at bottom */\n\n");

  for (BindingStruct const & se : file.serializedEntities) {
    writeStructFields(out, se);
    writeSerialized(out, se);
    writeDeserialized(out, se);
  }
}
