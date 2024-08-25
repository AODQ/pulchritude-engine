#include "binding-generator.hpp"

// C++ binding generator
// wraps c header for the most part
// also introduces C++ exclusive utilities

static std::string mathCppHeader = (
"#include <initializer_list>\n"
"\n"
"namespace pule {\n"
"\n"
"using F32 = float;\n"
"\n"
"struct F32v2 {\n"
"  float x, y;\n"
"};\n"
"\n"
"struct F32v3 {\n"
"  float x, y, z;\n"
"\n"
"  F32v3();\n"
"  ~F32v3() = default;\n"
"  F32v3(F32v3 && ff) = default;\n"
"  F32v3(F32v3 const & ff) = default;\n"
"  F32v3 & operator=(F32v3 && ff) = default;\n"
"  F32v3 & operator=(F32v3 const & ff) = default;\n"
"\n"
"  F32v3(PuleF32v3 const & a);\n"
"  F32v3(float x, float y, float z);\n"
"\n"
"  inline operator PuleF32v3() const { return {x,y,z}; }\n"
"\n"
"  PuleF32v3 toFloat() const;\n"
"\n"
"  F32v3 operator+(F32v3 const & rhs) const;\n"
"  F32v3 operator-(F32v3 const & rhs) const;\n"
"  F32v3 operator/(F32v3 const & rhs) const;\n"
"  F32v3 operator*(F32v3 const & rhs) const;\n"
"  F32v3 operator-() const;\n"
"\n"
"  F32v3 operator-=(F32v3 const & rhs);\n"
"\n"
"  F32v3 operator/(F32 const & rhs) const;\n"
"  F32v3 operator*(F32 const & rhs) const;\n"
"\n"
"  F32v3 operator+=(F32v3 const & rhs);\n"
"\n"
"  // TODO many missing operators, just fill in as needed\n"
"  // -=, *=, /=, etc\n"
"  // as well as comparison operators\n"
"  // as well as their equivalents for scalars\n"
"  // as well as CABI-equivalents\n"
"};\n"
"\n"
"struct F32v4 {\n"
"  float x, y, z, w;\n"
"\n"
"  F32v4();\n"
"  ~F32v4() = default;\n"
"  F32v4(F32v4 && ff) = default;\n"
"  F32v4(F32v4 const & ff) = default;\n"
"  F32v4 & operator=(F32v4 && ff) = default;\n"
"  F32v4 & operator=(F32v4 const & ff) = default;\n"
"\n"
"  inline operator PuleF32v4() const { return {x,y,z,w,}; }\n"
"\n"
"  F32v4(PuleF32v4 const & a);\n"
"  F32v4(float x, float y, float z, float w);\n"
"};\n"
"\n"
"struct F32m33 {\n"
"  pule::F32 elem[9];\n"
"\n"
"  F32m33(pule::F32 identity = 0.0f);\n"
"  F32m33(PuleF32m33 const & a);\n"
"  F32m33(std::initializer_list<pule::F32> const & list);\n"
"  ~F32m33() = default;\n"
"  F32m33(F32m33 && ff) = default;\n"
"  F32m33(F32m33 const & ff) = default;\n"
"  F32m33 & operator=(F32m33 && ff) = default;\n"
"  F32m33 & operator=(F32m33 const & ff) = default;\n"
"\n"
"  inline operator PuleF32m33() const {\n"
"    return {\n"
"      elem[0], elem[1], elem[2],\n"
"      elem[3], elem[4], elem[5],\n"
"      elem[6], elem[7], elem[8],\n"
"    };\n"
"  }\n"
"\n"
"  F32v3 operator *(pule::F32v3 const & rhs) const;\n"
"  F32m33 operator *(pule::F32 const & rhs) const;\n"
"  F32m33 operator *(F32m33 const & rhs) const;\n"
"  F32m33 operator +(F32m33 const & rhs) const;\n"
"\n"
"  F32m33 const & operator *=(F32m33 const & rhs);\n"
"  F32m33 const & operator *=(pule::F32 const & rhs);\n"
"};\n"
"\n"
"} // namespace pule\n"
"\n"
"// free-form operators\n"
"pule::F32 dot(pule::F32v3 const & a, pule::F32v3 const & b);\n"
"pule::F32 length(pule::F32v3 const & a);\n"
"pule::F32 lengthSqr(pule::F32v3 const & a);\n"
"pule::F32v3 cross(pule::F32v3 const & a, pule::F32v3 const & b);\n"
"pule::F32v3 normalize(pule::F32v3 const & a);\n"
"\n"
"pule::F32 magnitude(pule::F32v3 const a);\n"
"\n"
"pule::F32 trace(pule::F32m33 const &);\n"
"pule::F32 determinant(pule::F32m33 const &);\n"
"pule::F32m33 transpose(pule::F32m33 const &);\n"
"pule::F32m33 inverse(pule::F32m33 const &);\n"
"pule::F32 cofactor(pule::F32m33 const &, size_t row, size_t col);\n"
);

static std::string coreCppHeader = (
"#include <utility>\n"
"#ifndef puleScopeExit\n"
"#define _puleScopeExitConcatImpl(x, y) x##y\n"
"#define _puleScopeExitConcat(x, y) _puleScopeExitConcatImpl(x, y)\n"
"#define puleScopeExit \\\n"
"  auto const _puleScopeExitConcat(_puleScopeExit, __COUNTER__) \\\n"
"    = pule::ScopeGuard() << [&]()\n"
"#endif\n"
"namespace pule {\n"
"template <typename Fn>\n"
"struct ScopeGuardImpl {\n"
"  Fn fn;\n"
"  explicit ScopeGuardImpl(Fn && fn) : fn(fn) {}\n"
"  ScopeGuardImpl(ScopeGuardImpl && other) : fn(std::move(other.fn)) {}\n"
"  ScopeGuardImpl(const ScopeGuardImpl &) = delete;\n"
"  ScopeGuardImpl & operator=(const ScopeGuardImpl &) = delete;\n"
"  ~ScopeGuardImpl() { this->fn(); }\n"
"};\n"
"\n"
"struct ScopeGuard {\n"
"  template <typename F>\n"
"  ScopeGuardImpl<F> operator<<(F && fn) {\n"
"    return ScopeGuardImpl<F>(std::move(fn));\n"
"  }\n"
"};\n"
"}\n"
"\n"
"#include <unordered_map>\n"
"namespace pule {\n"
"// TODO replace unordered_map with own implementation that uses allocator\n"
"// though probably safe to use unordered_map if passed in default allocator\n"
"template <typename T, typename Handle=uint64_t> struct ResourceContainer {\n"
"  std::unordered_map<uint64_t, T> data = {};\n"
"  uint64_t idCount = 1;\n"
"  using TUnderlyingValue =\n"
"    std::conditional_t<std::is_pointer_v<T>, std::remove_pointer_t<T>, T>\n"
"  ;\n"
"\n"
"  // emplaces a new resource into the container\n"
"  //Handle create(T && resource) {\n"
"  //  uint64_t const id = this->idCount ++;\n"
"  //  this->data.emplace(this->idCount, resource);\n"
"  //  return Handle { id };\n"
"  //}\n"
"\n"
"  Handle create(T const & resource) {\n"
"    uint64_t const id = this->idCount ++;\n"
"    this->data.emplace(id, resource);\n"
"    return Handle { id };\n"
"  }\n"
"\n"
"  void destroy(Handle const handle) {\n"
"    if constexpr (std::is_same<Handle, uint64_t>::value) {\n"
"      if (handle == 0) { return; }\n"
"      this->data.erase(handle);\n"
"    } else {\n"
"      if (handle.id == 0) { return; }\n"
"      this->data.erase(handle.id);\n"
"    }\n"
"  }\n"
"\n"
"  auto begin() { return this->data.begin(); }\n"
"  auto end() { return this->data.end(); }\n"
"\n"
"  TUnderlyingValue * at(Handle const handle) {\n"
"    uint64_t uHandle;\n"
"    if constexpr (std::is_same<Handle, uint64_t>::value) {\n"
"      uHandle = handle;\n"
"    } else {\n"
"      uHandle = handle.id;\n"
"    }\n"
"    auto ptr = this->data.find(uHandle);\n"
"    if constexpr(std::is_pointer_v<T>) {\n"
"      return ((ptr == this->data.end()) ? nullptr : ptr->second);\n"
"    } else {\n"
"      return ((ptr == this->data.end()) ? nullptr : &ptr->second);\n"
"    }\n"
"  }\n"
"\n"
"  TUnderlyingValue const * at(Handle const handle) const {\n"
"    uint64_t uHandle;\n"
"    if constexpr (std::is_same<Handle, uint64_t>::value) {\n"
"      uHandle = handle;\n"
"    } else {\n"
"      uHandle = handle.id;\n"
"    }\n"
"    auto ptr = this->data.find(uHandle);\n"
"    if (ptr == this->data.end()) {\n"
"      return nullptr;\n"
"    }\n"
"    if constexpr(std::is_pointer_v<T>) {\n"
"      return ptr->second;\n"
"    } else {\n"
"      return &ptr->second;\n"
"    }\n"
"  }\n"
"};\n"
"}\n"
);

static std::string stringCppHeader = (
"PuleStringView operator \"\"_psv(char const * const cstr, size_t const len);\n"
"#include <string>\n" // TODO remove this w/ custom string
"namespace pule {\n"
"  struct str {\n"
"    PuleString data;\n"
"    str() : data { nullptr, 0, {}, } {}\n"
"    str(PuleString const & data) : data(data) {}\n"
"    str(PuleString && data) : data(data) {}\n"
"    str(str const & other) : data(puleString(other.data.contents)) {}\n"
"    str(str && other) : data(other.data) { other.data = { nullptr, 0, {}, }; }\n"
"    operator PuleString() const { return this->data; }\n"
"    operator std::string() const { \n"
"      return std::string(this->data.contents, this->data.len);\n"
"    }\n"
"    ~str() { puleStringDestroy(this->data); }\n"
"  };\n"
"}\n"
);

static std::string formatModifier(BindingTypeModifier m) {
  switch (m.type) {
    default: PULE_assert(false);
    case BindingTypeModifierType::tConst:
      return "const ";
    // ref is a hint for languages that support it to generate additional
    //   symbols from, but is recognized as a pointer in the C binding
    case BindingTypeModifierType::tRef:
      return "& ";
    case BindingTypeModifierType::tPtr:
      return "* ";
    case BindingTypeModifierType::tArray:
      return "[" + m.size + "] ";
  }
}

static std::string formatType(BindingType type, std::string const & name) {
  return formatTypeC(type, name, &formatModifier);
}

static PuleFileStream createFile(GenerateBindingInfo & info) {
  static std::string prefix = "../../../../../library/include/pulchritude/";
  if (!puleFilesystemPathExists(puleCStr(prefix.c_str()))) {
    puleFileDirectoryCreate(puleCStr(prefix.c_str()));
  }
  std::string path = prefix + std::string(info.path.contents) + ".hpp";
  auto fileOut = (
    puleFileStreamWriteOpen(
      puleCStr(path.c_str()), PuleFileDataMode_text
    )
  );
  info.output = puleFileStreamWriter(fileOut);
  return fileOut;
}

static bool isEntityFn(
  std::string entityName,
  BindingFunc func
) {
  auto enumNameLower = entityName;
  enumNameLower[0] = std::tolower(enumNameLower[0]);
  if (func.parameters.size() == 0) { return false; }
  // can not have __VA_ARGS__
  if (func.parameters.back().type.name == "__VA_ARGS__") { return false; }
  // it can either just be a ref to a single thing or have no modifiers
  if (func.parameters[0].type.modifiers.size() > 1) { return false; }
  if (func.parameters[0].type.modifiers.size() > 0) {
    auto & m = func.parameters[0].type.modifiers[0];
    if (m.type != BindingTypeModifierType::tRef) {
      return false;
    }
  }
  return (
       func.name.size() > entityName.size()
    && func.name.substr(0, entityName.size()) == enumNameLower
    && func.parameters[0].type.name == entityName
  );
}

static bool isEntityConstructor(
  std::string entityName,
  BindingFunc func
) {
  auto enumNameLower = entityName;
  enumNameLower[0] = std::tolower(enumNameLower[0]);
  // can not have __VA_ARGS__
  if (
       func.parameters.size() > 0
    && func.parameters.back().type.name == "__VA_ARGS__"
  ) { return false; }
  // constructors start with the entityName, don't use itself as first param
  //   and return the entity
  return (
       func.name.size() > entityName.size()
    && func.name.substr(0, entityName.size()) == enumNameLower
    && (
      func.parameters.size() > 0
      ? func.parameters[0].type.name != entityName
      : true
    )
    && func.returnType.name == entityName
  );
}

enum class EntityFnType {
  member,
  freeform,
  constructor
};

static std::string fixParam(std::string const & name, size_t it) {
  return (name == "" || name == "_") ? ("arg" + std::to_string(it)) : name;
}

static void generateEntityFn(
  std::string entityName,
  BindingFunc const & func,
  EntityFnType type,
  PuleStreamWrite const & out
) {
  auto funcCppName = func.name.substr(entityName.size());
  funcCppName[0] = std::tolower(funcCppName[0]);

  auto entityCppName = entityName.substr(4);

  auto const & write = puleStreamWriteStrFormat;

  bool isStructConstructor = false;

  // perhaps later source files will be generated, but for now inl it all
  if (type == EntityFnType::constructor) {
    // if the entityName is the same as function name, besides lower case
    // on the first ltter, it's an actual constructor
    puleLogDev("checking if %s is a struct constructor for %s", func.name.c_str(), entityName.c_str());
    if (
         entityName.substr(1) == func.name.substr(1)
      && tolower(entityName[0]) == func.name[0]
    ) {
      isStructConstructor = true;
      puleLogDev("found struct constructor %s", func.name.c_str());
    }
    write(
      out,
      "  static inline %s %s(",
      entityCppName.c_str(),
      funcCppName.c_str()
    );
  } else {
    write(
      out,
      "  inline %s %s(",
      formatType(func.returnType, "").c_str(),
      funcCppName.c_str()
    );
  }
  size_t paramStart = 0;
  bool isParamReference = false;
  if (
       type != EntityFnType::constructor
    && func.parameters[0].type.modifiers.size() > 0
  ) {
    auto & m = func.parameters[0].type.modifiers[0];
    isParamReference = m.type == BindingTypeModifierType::tRef;
  }
  std::string paramRefStr = (isParamReference ? " &" : "");
  if (type == EntityFnType::freeform) {
    write(out, "pule::%s%s self", entityCppName.c_str(), paramRefStr.c_str());
    if (func.parameters.size() > 1) {
      write(out, ", ");
    }
    paramStart = 1;
  } else if (type == EntityFnType::member) {
    paramStart = 1;
  }
  for (size_t i = paramStart; i < func.parameters.size(); ++i) {
    auto const & p = func.parameters[i];
    write(
      out,
      "%s",
      formatType(p.type, fixParam(p.name, i)).c_str()
    );
    if (i + 1 < func.parameters.size()) {
      write(out, ", ");
    }
  }
  write(out, ") {\n");
  if (type == EntityFnType::constructor) {
    if (isStructConstructor) {
      write(out, "    *this = { ._handle = %s(", func.name.c_str());
    } else {
      write(out, "    return { ._handle = %s(", func.name.c_str());
    }
  } else {
    write(
      out,
      "    return %s(%s%s",
      func.name.c_str(),
      paramRefStr.c_str(),
      (type == EntityFnType::freeform ? "self._handle" : "this->_handle")
    );
    if (func.parameters.size() > paramStart) {
      write(out, ", ");
    }
  }
  for (size_t i = paramStart; i < func.parameters.size(); ++i) {
    auto const & p = func.parameters[i];
    write(out, "%s", fixParam(p.name, i).c_str());
    if (i + 1 < func.parameters.size()) {
      write(out, ", ");
    }
  }
  if (type == EntityFnType::constructor) {
    write(out, "),};\n");
  } else {
    write(out, ");\n");
  }
  write(out, "  }\n");
}

// works for structs and entity
void generateEntityBinding(
  std::string const & name,
  std::vector<BindingFunc> const & funcs,
  PuleStreamWrite const & out
) {
  auto const & write = puleStreamWriteStrFormat;

  // first check if there is even any functions to generate
  bool hasFuncs = false;
  for (auto const & f : funcs) {
    if (isEntityFn(name, f) || isEntityConstructor(name, f)) {
      hasFuncs = true;
      break;
    }
  }

  write(out, "namespace pule {\n");

  if (!hasFuncs) {
    write(out, "using %s = %s;\n", name.substr(4).c_str(), name.c_str());
    write(out, "}\n");
    return;
  }

  write(out, "struct %s {\n", name.substr(4).c_str());

  // (need to use _handle to avoid name collision with functions)
  write(out, "  %s _handle;\n", name.c_str());
  write(
    out,
    "  inline operator %s() const {\n    return _handle;\n  }\n",
    name.c_str()
  );

  // generate member functions by using fns that use the entity as first param
  for (auto const & f : funcs) {
    if (!isEntityFn(name, f)) { continue; }
    generateEntityFn(name, f, EntityFnType::member, out);
  }

  // generate static constructors
  for (auto const & f  : funcs) {
    if (!isEntityConstructor(name, f)) { continue; }
    generateEntityFn(name, f, EntityFnType::constructor, out);
  }

  write(out, "};\n");

  write(out, "}\n");

  // generate freeform functions
  for (auto const & f : funcs) {
    if (!isEntityFn(name, f)) { continue; }
    generateEntityFn(name, f, EntityFnType::freeform, out);
  }
}

void generateBindingFileCpp(GenerateBindingInfo const & inforef) {
  GenerateBindingInfo info = inforef;
  PuleFileStream fileStream = createFile(info);
  puleScopeExit { puleFileStreamClose(fileStream); };
  auto const & file = info.file;
  auto const & out = info.output;
  auto const & write = puleStreamWriteStrFormat;
  write(out, "/* auto generated file %s */\n", info.path);
  write(out, "#pragma once\n");
  write(out, "#include \"core.hpp\"\n\n");

  // include self c header
  write(out, "#include \"%s.h\"\n", info.path.contents);

  if (puleStringViewEqCStr(info.path, "core")) {
    // TODO set this up to use windows dllimport/export
    write(out, "%s\n", coreCppHeader.c_str());
  } else if (puleStringViewEqCStr(info.path, "math")) {
    write(out, "%s\n", mathCppHeader.c_str());
    return;
  }

  // write out includes
  for (auto const & i : file.includes) {
    write(out, "#include \"%s.hpp\"\n", i.c_str());
  }

  write(out, "\n");

  // write out entity class wrappers
  for (auto const & e : file.entities) {
    generateEntityBinding(e.name, file.funcs, out);
  }
  for (auto const & s : file.structs) {
    generateEntityBinding(s.name, file.funcs, out);
  }

  // write out toStr function (doesn't work for core since circular dependency)
  if (file.enums.size() > 0 && !puleStringViewEqCStr(info.path, "core")) {
    write(out, "#include \"string.hpp\"\n");
    write(out, "#include <string>\n"); // TODO remove this w/ custom string
    write(out, "namespace pule { //tostr \n");
    for (auto const & e : file.enums) {
      write(out, "inline pule::str toStr(%s const e) {\n", e.name.c_str());
      if (e.isBitfield) {
        write(out, "  std::string str = \"( \";\n");
        for (size_t i = 0; i < e.fields.size(); ++ i) {
          auto const & v = e.fields[i];
          write(
            out,
            "  if (e & %s_%s) {\n    str += \"%s | \";\n  }\n",
            e.name.c_str(), v.name.c_str(), v.name.c_str()
          );
        }
        write(out, "  str += \")\";\n");
        write(out, "  PuleString strCp = puleString(str.c_str());\n");
        write(out, "  return strCp;\n");
      } else {
        write(out, "  switch (e) {\n");
        for (size_t i = 0; i < e.fields.size(); ++ i) {
          auto const & v = e.fields[i];
          // if there are any duplicate values, skip this
          for (size_t j = 0; j < i; ++ j) {
            auto const & v2 = e.fields[j];
            if (v2.value == v.value) { goto SKIP_WRITE; }
          }
          write(
            out,
            "    case %s_%s: return puleString(\"%s\");\n",
            e.name.c_str(), v.name.c_str(), v.name.c_str()
          );
          SKIP_WRITE:;
        }
        write(out, "    default: return puleString(\"N/A\");\n");
        write(out, "  }\n");
      }
      write(out, "}\n");
    }
    write(out, "}\n");
  }


  // write out to

  if (puleStringViewEqCStr(info.path, "string")) {
    write(out, "%s\n", stringCppHeader.c_str());
  }
}
