#include <pulchritude-mesh/mesh.h>

void puleGfxMeshRender(
  PuleGfxMesh const * const pMesh,
  PuleGfxGpuBuffer const cameraUniformBuffer,
  PuleGfxFramebuffer const targetFramebuffer,
  PuleGfxCommandListRecorder const recorder
) {
  PuleGfxMesh const & mesh = *pMesh;

  puleGfxCommandListAppendAction(
    recorder,
    PuleGfxCommand {
      .bindFramebuffer = {
      }
    }
  );
  puleGfxCommandListAppendAction(
    recorder,
    PuleGfxCommand {
      .bindPipeline = {
      }
    }
  );
  puleGfxCommandListAppendAction(
    recorder,
    PuleGfxCommand {
      .bindBuffer = {
        cameraUniformBuffer,
      }
    }
  );

  for (size_t it = 0; it < PuleGfxMeshAttributeSize; ++ it) {
    if (mesh.attributes[it].buffer == 0) { continue; }
    puleGfxCommandListAppendAction(
      recorder,
      PuleGfxCommand {
        .bindAttribute = {
        }
      }
    );
  }

  if (mesh.indices.buffer.id > 0) {
    puleGfxCommandListAppendAction(
      recorder,
      PuleGfxCommand {
        .dispatchRenderElements = {
        }
      }
    );
  } else {
    puleGfxCommandListAppendAction(
      recorder,
      PuleGfxCommand {
        .dispatchRender = {
        }
      }
    );
  }
}
