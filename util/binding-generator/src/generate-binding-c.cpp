#include "binding-generator.hpp"

// C binding generator

std::string coreHeaderInl = (
"// TODO need to set this to the correct symbol export pragma on windows\n"
"#define PULE_exportFn\n"
"\n"
"// common C types such as uint64_t, bool, etc\n"
"#ifdef __cplusplus\n"
"#include <cmath>\n"
"#include <cstddef>\n"
"#include <cstdint>\n"
"#include <cstdlib>\n"
"#include <cstring>\n"
"#include <cstdarg>\n"
"#else\n"
"#include <math.h>\n"
"#include <stdbool.h>\n"
"#include <stddef.h>\n"
"#include <stdint.h>\n"
"#include <stdlib.h>\n"
"#include <string.h>\n"
"#include <stdarg.h>\n"
"#endif\n"
"\n"
"// gives the size of a static array; T[40] will work, T* will NOT work\n"
"#define PULE_arraySize(ARR) ((int32_t)(sizeof(ARR) / sizeof(*(ARR))))\n"
"\n"
"// allows multiline strings to work in C/C++ by converting the entire inputted\n"
"//   symbol to a string\n"
"#define PULE_multilineString(...) #__VA_ARGS__\n"
"\n"
"// struct initializers for C++\n"
"#ifdef __cplusplus\n"
"#define PULE_param(...) = __VA_ARGS__\n"
"#define PULE_defaultField(...) = __VA_ARGS__\n"
"#else\n"
"#define PULE_param(...)\n"
"#define PULE_defaultField(...)\n"
"#endif\n"
);

std::string dataSerializerHeaderInl = (
"#define PULE_dsStructField(structtype, fieldtype, member, count) \\\n"
"  { PuleDt_##fieldtype , offsetof(structtype, member) , count , }\n"
"#define PULE_dsStructTerminator { PuleDt_ptr , 0 , 0 , }\n"
);

std::string errorHeaderInl = (
"// errors out with message, storing the error ID to error as well\n"
"#define PULE_error(ErrorId, message, ...) \\\n"
"  puleErrorPropagate( \\\n"
"    error, \\\n"
"    PuleError { \\\n"
"      .description = ( \\\n"
"        puleStringFormatDefault(message __VA_OPT__(,) __VA_ARGS__) \\\n"
"      ), \\\n"
"      .id = ErrorId, \\\n"
"      .sourceLocationNullable = __FILE__, \\\n"
"      .lineNumber = __LINE__, \\\n"
"      .child = nullptr, \\\n"
"    } \\\n"
"  );\n"
"\n"
"// asserts an expression, which on failure will return the error with the\n"
"//   expression as it's description\n"
"#define PULE_errorAssert(X, ErrorId, RetValue) \\\n"
"  if (!(X)) { \\\n"
"    PULE_error(ErrorId, \"assertion failed; %s\", #X); \\\n"
"    return RetValue; \\\n"
"  }\n"
"\n"
"// irrecoverable assert, should only be on for relwithdebinfo builds i guess\n"
"#define PULE_assert(X) { \\\n"
"  if (!(X)) { \\\n"
"    puleLogError( \\\n"
"      \"assertion failed; %s @ %s:%d\", #X, __FILE__, __LINE__ \\\n"
"    ); \\\n"
"    abort(); \\\n"
"  } \\\n"
"}\n"
"\n"
"#define PULE_assertComparison(X, Y) { \\\n"
"  if (X != Y) { \\\n"
"    puleLogError( \\\n"
"      \"assert failed; %s != %s (%d != %d) @ %s:%d\", \\\n"
"      #X, #Y, X, Y, __FILE__, __LINE__ \\\n"
"    ); \\\n"
"  } \\\n"
"}\n"
"\n"
"// pretty print error, format start with (\"%s\")\n"
"#define PULE_prettyError(str, ...) { \\\n"
"  puleLogError(str, __PRETTY_FUNCTION__, __VA_ARGS__); \\\n"
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
      return "* ";
    case BindingTypeModifierType::tPtr:
      return "* ";
    case BindingTypeModifierType::tArray:
      return "[" + m.size + "] ";
  }
}

std::string formatTypeC(
  BindingType type, std::string const & name,
  std::string (*formatModifierFn)(BindingTypeModifier)
) {
  std::string result;
  if (type.type == BindingTypeType::fnPtr) {
    result += formatTypeC(type.fnptrParams[0], "", formatModifierFn);
    result += "(* ";
    for (auto & m : type.modifiers) {
      result += formatModifierFn(m);
    }
    result += name;
    result += ")(";
    for (auto & p : type.fnptrParams) {
      if (&p == &type.fnptrParams[0]) { continue; }
      result += formatTypeC(p, "", formatModifierFn);
      if (&p != &type.fnptrParams.back()) { result += ", "; }
    }
    result += ")";
  } else {
    if (type.name == "__VA_ARGS__") {
      PULE_assert(type.modifiers.size() == 0);
      return "...";
    }
    // if starts with Pule and is a ptr then use struct
    if (
         type.name.substr(0, 4) == "Pule"
      && type.modifiers.size() == 1
      && type.modifiers[0].type == BindingTypeModifierType::tPtr
    ) {
      //result += "struct ";
    }
    result += type.name;
    if (type.modifiers.size() > 0) {
      result += " ";
    }
    for (auto & m : type.modifiers) {
      if (m.type == BindingTypeModifierType::tArray) {
        continue;
      }
      result += formatModifierFn(m);
    }
    // remove trailing space
    if (type.modifiers.size() > 0) {
      result.pop_back();
    }
    if (name.size() > 0 && name != "_") {
      result += " " + name;
    }
    for (auto & m : type.modifiers) {
      if (m.type != BindingTypeModifierType::tArray) {
        continue;
      }
      result += formatModifierFn(m);
    }
  }

  return result;
}

static std::string formatType(BindingType type, std::string const & name) {
  return formatTypeC(type, name, &formatModifier);
}

static PuleFileStream createFile(GenerateBindingInfo & info) {
  static std::string prefix = "../../../../../library/include/pulchritude/";
  if (!puleFilesystemPathExists(puleCStr(prefix.c_str()))) {
    puleFileDirectoryCreate(puleCStr(prefix.c_str()));
  }
  std::string path = prefix + std::string(info.path.contents) + ".h";
  auto fileOut = (
    puleFileStreamWriteOpen(
      puleCStr(path.c_str()), PuleFileDataMode_text
    )
  );
  info.output = puleFileStreamWriter(fileOut);
  return fileOut;
}

void generateBindingFileC(GenerateBindingInfo const & inforef) {
  GenerateBindingInfo info = inforef;
  PuleFileStream fileStream = createFile(info);
  puleScopeExit { puleFileStreamClose(fileStream); };
  auto const & file = info.file;
  auto const & out = info.output;
  auto const & write = puleStreamWriteStrFormat;
  write(out, "/* auto generated file %s */\n", info.path);
  write(out, "#pragma once\n");

  if (puleStringViewEqCStr(info.path, "core")) {
    // TODO set this up to use windows dllimport/export
    write(out, "%s\n", coreHeaderInl.c_str());
  } else if (puleStringViewEqCStr(info.path, "error")) {
    write(out, "%s\n", errorHeaderInl.c_str());
    write(out, "#include \"core.h\"\n\n");
  } else if (puleStringViewEqCStr(info.path, "data-serializer")) {
    write(out, "%s\n", dataSerializerHeaderInl.c_str());
    write(out, "#include \"core.h\"\n\n");
  } else {
    write(out, "#include \"core.h\"\n\n");
  }

  // write out includes
  for (auto const & i : file.includes) {
    write(out, "#include \"%s.h\"\n", i.c_str());
  }

  // in case included by C++ (which happens by default in C++ bindings)
  write(out, "\n");
  write(out, "#ifdef __cplusplus\n");
  write(out, "extern \"C\" {\n");
  write(out, "#endif\n");

  write(out, "\n// enum\n");

  // write out enums
  for (auto const & e : file.enums) {
    if (e.comment.size() > 0) {
      write(out, "/* %s */\n", e.comment.c_str());
    }
    write(out, "typedef enum {\n");
    for (auto const & f : e.fields) {
      if (f.comment.size() > 0) {
        write(out, "  /* %s */\n", f.comment.c_str());
      }
      write(out, "  %s_%s = %lld,\n", e.name.c_str(), f.name.c_str(), f.value);
    }
    write(out, "} %s;\n", e.name.c_str());

    write(
      out, "const uint32_t %sSize = %lld;\n",
      e.name.c_str(), e.fields.size()
    );
  }

  write(out, "\n// entities\n");

  // write out entities
  for (auto const & e : file.entities) {
    if (e.comment.size() > 0) {
      write(out, "/* %s */\n", e.comment.c_str());
    }
    write(
      out,
      "typedef struct %s { uint64_t id; } %s;\n",
      e.name.c_str(), e.name.c_str()
    );
  }

  write(out, "\n// structs\n");

  // write out struct prototypes
  for (auto const & s : file.structs) {
    write(
      out, "%s %s;\n", (s.isUnion ? "union" : "struct"), s.name.c_str()
    );
  }
  write(out, "\n");

  // write out structs
  for (auto const & s : file.structs) {
    if (s.comment.size() > 0) {
      write(out, "/* %s */\n", s.comment.c_str());
    }
    write(
      out,
      "typedef %s %s {\n",
      (s.isUnion ? "union" : "struct"),
      s.name.c_str()
    );
    for (auto const & f : s.fields) {
      if (f.comment.size() > 0) {
        write(out, "  /* %s */\n", f.comment.c_str());
      }
      // if field type has same name as struct, need to use struct keyword
      if (f.type.name == s.name) {
        write(out, "  struct ");
      } else {
        write(out, "  ");
      }
      write(out, "%s;\n", formatType(f.type, f.name).c_str());
      // C doesn't have assignments in struct definitions
    }
    write(out, "} %s;\n", s.name.c_str());
  }

  write(out, "\n// functions\n");

  // write out functions
  for (auto const & f : file.funcs) {
    if (f.comment.size() > 0) {
      write(out, "/* %s */\n", f.comment.c_str());
    }
    write(
      out,
      "PULE_exportFn %s %s(",
      formatType(f.returnType, "").c_str(),
      f.name.c_str()
    );
    size_t paramIt = 0;
    for (auto const & p : f.parameters) {
      write(
        out, "%s",
        formatType(p.type, p.name).c_str()
      );
      if (f.parameters.size()-1 != paramIt) {
        write(out, ", ");
      }
      ++ paramIt;
    }
    write(out, ");\n");
  }
  write(out, "\n");

  write(out, "#ifdef __cplusplus\n");
  write(out, "} // extern C\n");
  write(out, "#endif\n");
}
