#include <pulchritude/gpu-ir.h>

#include <pulchritude/allocator.hpp>
#include <pulchritude/array.hpp>
#include <pulchritude/core.hpp>

#include <string>
#include <unordered_map>
#include <vector>

#include <spirv-tools/libspirv.hpp>
#include <spirv-tools/optimizer.hpp>

void puleGpuIr_compileSpirv(PuleStringView str) {
  // -- generate spir-v
  spvtools::SpirvTools tools(SPV_ENV_UNIVERSAL_1_6);
  spvtools::Optimizer optimizer(SPV_ENV_UNIVERSAL_1_6);

  auto print_msg_to_stderr = (
    [](
      spv_message_level_t, const char*, const spv_position_t&, const char* m
    ) {
      puleLogError("error: %s", m);
    }
  );
  tools.SetMessageConsumer(print_msg_to_stderr);
  optimizer.SetMessageConsumer(print_msg_to_stderr);

  std::vector<uint32_t> spirv;
  if (!tools.Assemble(str.contents, &spirv)) {
    PULE_assert(false && "failed to assemble SPIR-V");
  }
  std::string disassembled;
  tools.Disassemble(spirv, &disassembled);
  puleLogDev(
    "Assembled SPIR-V, disassembled:\n```%s\n```",
    disassembled.c_str()
  );
  puleLogDev("Validating SPIR-V");
  if (!tools.Validate(spirv)) {
    PULE_assert(false && "failed to validate SPIR-V");
  }
  puleLogDev("Validated SPIR-V");
}
