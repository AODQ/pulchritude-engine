#include "binding-generator.hpp"

// markdown binding generator

static std::string formatModifier(BindingTypeModifier m) {
  switch (m.type) {
    default: PULE_assert(false);
    case BindingTypeModifierType::tConst:
      return "const ";
    // ref is a hint for languages that support it to generate additional
    //   symbols from, but is recognized as a pointer in the C binding
    case BindingTypeModifierType::tRef:
      return "ref ";
    case BindingTypeModifierType::tPtr:
      return "ptr ";
    case BindingTypeModifierType::tArray:
      return "[" + m.size + "] ";
  }
}

static std::string formatBindingAssignment(
  BindingAssignment a,
  BindingAssignmentType type
) {
  switch (type) {
    default: PULE_assert(false);
    case BindingAssignmentType::tInt:
      return std::to_string(a.valInt);
    case BindingAssignmentType::tFloat:
      return std::to_string(a.valFloat);
    case BindingAssignmentType::tChar:
      return std::to_string(a.valChar);
    case BindingAssignmentType::tBool:
      return a.valBool ? "true" : "false";
    case BindingAssignmentType::identifier:
      return a.valIdentifier.contents;
    case BindingAssignmentType::none:
      return "";
  }
}

static std::string formatType(BindingType type) {
  std::string result;
  if (type.type == BindingTypeType::fnPtr) {
    result += "@fnptr(";
    for (auto & p : type.fnptrParams) {
      result += formatType(p);
      if (&p != &type.fnptrParams.back()) { result += ", "; }
    }
    result += ")";
    if (type.modifiers.size() > 0) {
      result += " ";
    }
    for (auto & m : type.modifiers) {
      result += formatModifier(m);
    }
  } else {
    if (type.name == "__VA_ARGS__") {
      PULE_assert(type.modifiers.size() == 0);
      return "__VA_ARGS__";
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
  }

  return result;
}

static PuleFileStream createFile(GenerateBindingInfo & info) {
  static std::string prefix = "../../../../../documentation/";
  if (!puleFilesystemPathExists(puleCStr(prefix.c_str()))) {
    puleFileDirectoryCreate(puleCStr(prefix.c_str()));
  }
  std::string path = prefix + std::string(info.path.contents) + ".md";
  auto fileOut = (
    puleFileStreamWriteOpen(
      puleCStr(path.c_str()), PuleFileDataMode_text
    )
  );
  info.output = puleFileStreamWriter(fileOut);
  return fileOut;
}

void generateBindingFileMd(GenerateBindingInfo const & inforef) {
  GenerateBindingInfo info = inforef;
  PuleFileStream fileStream = createFile(info);
  puleScopeExit { puleFileStreamClose(fileStream); };
  auto const & file = info.file;
  auto const & out = info.output;
  auto const & write = puleStreamWriteStrFormat;
  write(out, "# %s\n", info.path.contents);

  if (file.structs.size() > 0) {
    write(out, "\n## structs\n");
  }

  // write out structs
  for (auto const & s : file.structs) {
    write(out, "### %s\n", s.name.c_str());
    if (s.comment.size() > 0) {
      write(out, "%s\n", s.comment.c_str());
    }
    write(out, "```c\n");
    write(out, "%s {\n", (s.isUnion ? "union" : "struct"));
    for (auto const & f : s.fields) {
      if (f.comment.size() > 0) {
        write(out, "  /* %s */\n", f.comment.c_str());
      }
      write(out, "  %s : %s", f.name.c_str(), formatType(f.type).c_str());
      if (f.assignmentType != BindingAssignmentType::none) {
        write(
          out,
          " = %s",
          formatBindingAssignment(f.assignment, f.assignmentType).c_str()
        );
      }
      write(out, ";\n");
    }
    write(out, "};\n", s.name.c_str());
    write(out, "```\n");
  }

  if (file.enums.size() > 0) {
    write(out, "\n## enums\n");
  }

  // write out enums
  for (auto const & e : file.enums) {
    write(out, "### %s\n", e.name.c_str());
    if (e.comment.size() > 0) {
      write(out, "%s\n", e.comment.c_str());
    }
    write(out, "```c\n");
    write(out, "enum {\n");
    for (auto const & f : e.fields) {
      if (f.comment.size() > 0) {
        write(out, "  /* %s */\n", f.comment.c_str());
      }
      write(out, "  %s,\n", f.name.c_str());
    }
    write(out, "}\n");
    write(out, "```\n");
  }

  if (file.entities.size() > 0) {
    write(out, "\n## entities\n");
  }

  // write out entities
  for (auto const & e : file.entities) {
    write(out, "### %s\n", e.name.c_str());
    if (e.comment.size() > 0) {
      write(out, "%s\n", e.comment.c_str());
    }
    // TODO write out associated functions?
  }

  if (file.funcs.size() > 0) {
    write(out, "\n## functions\n");
  }

  // write out functions
  for (auto const & f : file.funcs) {
    write(out, "### %s\n", f.name.c_str());
    if (f.comment.size() > 0) {
      write(out, "%s\n", f.comment.c_str());
    }
    write(out, "```c\n");
    write(out, "%s(", f.name.c_str());
    if (f.parameters.size() > 0) {
      write(out, "\n");
    }
    size_t paramIt = 0;
    for (auto const & p : f.parameters) {
      write(
        out, "  %s : %s",
        p.name.c_str(), formatType(p.type).c_str()
      );
      if (f.parameters.size()-1 != paramIt) {
        write(out, ",\n");
      } else {
        write(out, "\n");
      }
      ++ paramIt;
    }
    write(out, ") %s;\n", formatType(f.returnType).c_str());
    write(out, "```\n");
  }
}
