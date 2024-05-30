//
//void puleGpuMeshRender(
//  PuleGpuMesh const * const pMesh,
//  PuleGpuBuffer const cameraUniformBuffer,
//  PuleGpuFramebuffer const targetFramebuffer,
//  PuleGpuCommandListRecorder const recorder
//) {
//  PuleGpuMesh const & mesh = *pMesh;
//  (void)mesh;
//  (void)cameraUniformBuffer;
//  (void)targetFramebuffer;
//  (void)recorder;
//
//  /* puleGpuCommandListAppendAction( */
//  /*   recorder, */
//  /*   PuleGpuCommand { */
//  /*     .bindFramebuffer = { */
//  /*     } */
//  /*   } */
//  /* ); */
//  /* puleGpuCommandListAppendAction( */
//  /*   recorder, */
//  /*   PuleGpuCommand { */
//  /*     .bindPipeline = { */
//  /*     } */
//  /*   } */
//  /* ); */
//  /* puleGpuCommandListAppendAction( */
//  /*   recorder, */
//  /*   PuleGpuCommand { */
//  /*     .bindBuffer = { */
//  /*       cameraUniformBuffer, */
//  /*     } */
//  /*   } */
//  /* ); */
//
//  /* for (size_t it = 0; it < PuleGpuMeshAttributeSize; ++ it) { */
//  /*   if (mesh.attributes[it].buffer == 0) { continue; } */
//  /*   puleGpuCommandListAppendAction( */
//  /*     recorder, */
//  /*     PuleGpuCommand { */
//  /*       .bindAttribute = { */
//  /*       } */
//  /*     } */
//  /*   ); */
//  /* } */
//
//  /* if (mesh.indices.buffer.id > 0) { */
//  /*   puleGpuCommandListAppendAction( */
//  /*     recorder, */
//  /*     PuleGpuCommand { */
//  /*       .dispatchRenderElements = { */
//  /*       } */
//  /*     } */
//  /*   ); */
//  /* } else { */
//  /*   puleGpuCommandListAppendAction( */
//  /*     recorder, */
//  /*     PuleGpuCommand { */
//  /*       .dispatchRender = { */
//  /*       } */
//  /*     } */
//  /*   ); */
//  /* } */
//}
