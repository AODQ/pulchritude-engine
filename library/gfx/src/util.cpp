#include <util.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <glad/glad.h>
#pragma GCC diagnostic pop

#include <unordered_map>

namespace {
  std::unordered_map<uint64_t, util::Pipeline> pipelines;
  size_t pipelineIdx = 1;
}

util::Pipeline * util::pipeline(size_t const id) {
  auto pipelineIter = ::pipelines.find(id);
  if (pipelineIter == ::pipelines.end()) {
    return nullptr;
  }
  return &pipelineIter->second;
}

uint64_t util::createPipeline() {
  uint64_t const prevPipelineIdx = ::pipelineIdx;
  ::pipelineIdx += 1;
  puleLogDebug("incrementing pipeline idx now: %lu", ::pipelineIdx);
  util::Pipeline utilPipeline = {};
  ::pipelines.emplace(prevPipelineIdx, utilPipeline);
  puleLogDebug("new pipeline idex to return is %lu", prevPipelineIdx);
  return prevPipelineIdx;
}

void util::destroyPipeline(uint64_t const id) {
  auto const handle = (
    static_cast<GLuint>(::pipelines.at(id).attributeDescriptorHandle)
  );
  if (handle != 0) {
    glDeleteVertexArrays(1, &handle);
  }
  ::pipelines.erase(id);
}
