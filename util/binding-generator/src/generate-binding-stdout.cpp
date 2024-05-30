#include "binding-generator.hpp"

#include <pulchritude/log.h>

// C binding generator

void generateBindingFileStdout(BindingFile const & file, PuleStringView path) {
  // print out the parsed data
  puleLog("<><>");
  puleLog("Parsed file: %s\n", path.contents);
  puleLog("<><>");
  for (auto const &s : file.structs) {
    puleLog("Struct: %s", s.name.c_str());
    puleLog("  Comment: %s", s.comment.c_str());
    for (auto const &f : s.fields) {
      puleLog("  Field: %s", f.name.c_str());
      puleLog("    Type: %s", f.type.c_str());
      switch (f.assignmentType) {
        case BindingAssignmentType::tInt:
          printf("  assignment (int): %ld\n", f.assignment.valInt);
          break;
        case BindingAssignmentType::tFloat:
          printf("  assignment (float): %f\n", (double)f.assignment.valFloat);
          break;
        case BindingAssignmentType::tChar:
          printf("  assignment (char): %c\n", f.assignment.valChar);
          break;
        case BindingAssignmentType::tBool:
          printf(
            "  assignment (bool): %s\n",
            f.assignment.valBool ? "true" : "false"
          );
          break;
        case BindingAssignmentType::identifier:
          puleLog(
            "  assignment (identifier): %s",
            f.assignment.valIdentifier.contents
          );
          break;
        case BindingAssignmentType::none:
          break;
      }
      puleLog("    Comment: %s", f.comment.c_str());
    }
  }
  for (auto const &e : file.enums) {
    puleLog("Enum: %s", e.name.c_str());
    puleLog("  Comment: %s", e.comment.c_str());
    for (auto const &f : e.fields) {
      puleLog("  Field: %s", f.c_str());
    }
  }
  for (auto const &f : file.funcs) {
    puleLog("Func: %s", f.name.c_str());
    puleLog("  Comment: %s", f.comment.c_str());
    puleLog("  Return Type: %s", f.returnType.c_str());
    for (auto const &p : f.parameters) {
      puleLog("  Param: %s", p.name.c_str());
      puleLog("    Type: %s", p.type.c_str());
    }
  }
}
