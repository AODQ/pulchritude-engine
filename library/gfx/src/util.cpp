#include <util.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <glad/glad.h>
#pragma GCC diagnostic pop

#include <unordered_map>

namespace {
  std::unordered_map<uint64_t, util::PipelineLayout> pipelineLayouts;
  size_t pipelineLayoutIdx = 0;
}

util::PipelineLayout const * util::pipelineLayout(size_t const id) {
  auto pipelineLayoutIter = ::pipelineLayouts.find(id);
  if (pipelineLayoutIter == ::pipelineLayouts.end()) {
    return nullptr;
  }
  return &pipelineLayoutIter->second;
}

uint64_t util::createPipelineLayout(util::PipelineLayout const pipelineLayout) {
  uint64_t const prevPipelineLayoutIdx = ::pipelineLayoutIdx;
  ::pipelineLayoutIdx += 1;
  ::pipelineLayouts.emplace(prevPipelineLayoutIdx, pipelineLayout);
  return prevPipelineLayoutIdx;
}

void util::destroyPipelineLayout(uint64_t const id) {
  GLuint handle = static_cast<GLuint>(::pipelineLayouts.at(id).descriptor);
  glDeleteVertexArrays(1, &handle);
  ::pipelineLayouts.erase(id);
}
