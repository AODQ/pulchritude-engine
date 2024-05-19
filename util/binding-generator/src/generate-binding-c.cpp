#include "binding-generator.hpp"

// C binding generator

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

static std::string formatType(BindingType type, std::string const & name) {
  std::string result;
  if (type.type == BindingTypeType::fnPtr) {
    result += formatType(type.fnptrParams[0], "");
    result += "(* ";
    for (auto & m : type.modifiers) {
      result += formatModifier(m);
    }
    result += name;
    result += ")(";
    for (auto & p : type.fnptrParams) {
      if (&p == &type.fnptrParams[0]) { continue; }
      result += formatType(p, "");
      if (&p != &type.fnptrParams.back()) { result += ", "; }
    }
    result += ")";
  } else {
    if (type.name == "__VA_ARGS__") {
      PULE_assert(type.modifiers.size() == 0);
      return "...";
    }
    result += type.name;
    if (type.modifiers.size() > 0) {
      result += " ";
    }
    for (auto & m : type.modifiers) {
      result += formatModifier(m);
    }
    // remove trailing space
    if (type.modifiers.size() > 0) {
      result.pop_back();
    }
    if (name.size() > 0) {
      result += " " + name;
    }
  }

  return result;
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
  write(out, "#include <pulchritude/core.h>\n");
  write(out, "\n");

  // write out includes
  for (auto const & i : file.includes) {
    write(out, "#include <pulchritude/%s.h>\n", i.c_str());
  }


  // in case included by C++ (which happens by default in C++ bindings)
  write(out, "\n");
  write(out, "#ifdef __cplusplus\n");
  write(out, "extern \"C\" {\n");
  write(out, "#endif\n");

  write(out, "\n// structs\n");

  // write out structs
  for (auto const & s : file.structs) {
    if (s.comment.size() > 0) {
      write(out, "/* %s */\n", s.comment.c_str());
    }
    write(out, "typedef %s {\n", (s.isUnion ? "union" : "struct"));
    for (auto const & f : s.fields) {
      if (f.comment.size() > 0) {
        write(out, "  /* %s */\n", f.comment.c_str());
      }
      write(out, "  %s;\n", formatType(f.type, f.name).c_str());
      // C doesn't have assignments in struct definitions
    }
    write(out, "} %s;\n", s.name.c_str());
  }

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
      write(out, "  %s,\n", f.name.c_str());
    }
    write(out, "} %s;\n", e.name.c_str());
  }

  write(out, "\n// entities\n");

  // write out entities
  for (auto const & e : file.entities) {
    if (e.comment.size() > 0) {
      write(out, "/* %s */\n", e.comment.c_str());
    }
    write(out, "typedef struct { uint64_t id; } %s;\n", e.name.c_str());
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
