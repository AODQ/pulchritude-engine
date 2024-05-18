#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <pulchritude-string/string.h>
#include <pulchritude-file/file.h>

enum struct BindingTypeModifierType {
  tConst,
  tRef,
  tPtr,
  tArray,
};

struct BindingTypeModifier {
  BindingTypeModifierType type;
  std::string size;
};

enum struct BindingAssignmentType {
  tInt,
  tFloat,
  tChar,
  tBool,
  identifier,
  none,
};
union BindingAssignment {
  int64_t valInt;
  float valFloat;
  char valChar;
  bool valBool;
  PuleString valIdentifier;
};

enum struct BindingTypeType {
  identifier,
  fnPtr,
};

struct BindingType {
  std::string name;
  std::vector<BindingType> fnptrParams; // first param is return type
  std::vector<BindingTypeModifier> modifiers;
  BindingTypeType type;
};

struct BindingStructField {
  std::string name;
  BindingType type;
  BindingAssignment assignment;
  BindingAssignmentType assignmentType;
  std::string comment;
};

struct BindingStruct {
  std::string name;
  std::string comment;
  std::vector<BindingStructField> fields;
  bool isUnion;
};

struct BindingEnumField {
  std::string name;
  std::string comment;
  int64_t value;
};

struct BindingEnum {
  std::string name;
  std::string comment;
  std::vector<BindingEnumField> fields;
};

struct BindingFuncParameter {
  std::string name;
  BindingType type;
};

struct BindingFunc {
  std::string name;
  std::string comment;
  std::vector<BindingFuncParameter> parameters;
  BindingType returnType;
};

struct BindingEntity {
  std::string name;
  std::string comment;
};

struct BindingFile {
  std::vector<BindingStruct> structs;
  std::vector<BindingEnum> enums;
  std::vector<BindingFunc> funcs;
  std::vector<BindingEntity> entities;
};


// these are the binding generator functions for each language
// they are defined in their respective files

struct GenerateBindingInfo {
  BindingFile file;
  PuleStringView path;
  PuleStreamWrite output;
};

void generateBindingFileC  (GenerateBindingInfo const &);
void generateBindingFileCpp(GenerateBindingInfo const &);
void generateBindingFileLua(GenerateBindingInfo const &);
void generateBindingFileZig(GenerateBindingInfo const &);
void generateBindingFilePy (GenerateBindingInfo const &);
void generateBindingFilePds(GenerateBindingInfo const &);
