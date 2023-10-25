#pragma once
// File automatically generated by c-layer-from-json-binding-generator.py
// Use the functions provided by this file to allow layering support
#include <pulchritude-allocator/allocator.h>
#include <pulchritude-array/array.h>
#include <pulchritude-asset/asset-script-task-graph.h>
#include <pulchritude-asset/image.h>
#include <pulchritude-asset/tiled.h>
#include <pulchritude-asset/font.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-asset/render-graph.h>
#include <pulchritude-asset/shader-module.h>
#include <pulchritude-camera/camera.h>
#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-ecs-serializer/ecs-serializer.h>
#include <pulchritude-ecs/ecs.h>
#include <pulchritude-error/error.h>
#include <pulchritude-file/file.h>
#include <pulchritude-gfx-debug/gfx-debug.h>
#include <pulchritude-gfx-mesh/mesh.h>
#include <pulchritude-imgui-engine/imgui-engine.h>
#include <pulchritude-imgui/imgui.h>
#include <pulchritude-log/log.h>
#include <pulchritude-math/math.h>
#include <pulchritude-platform/platform.h>
#include <pulchritude-plugin/plugin-payload.h>
#include <pulchritude-plugin/plugin.h>
#include <pulchritude-raycast/raycast.h>
#include <pulchritude-renderer-3d/renderer-3d.h>
#include <pulchritude-script/script.h>
#include <pulchritude-stream/stream.h>
#include <pulchritude-string/string.h>
#include <pulchritude-task-graph/task-graph.h>
#include <pulchritude-time/time.h>
#include <pulchritude-tui/tui.h>
#include <pulchritude-gpu/commands.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/image.h>
#include <pulchritude-gpu/pipeline.h>
#include <pulchritude-gpu/resource.h>
#include <pulchritude-gpu/shader-module.h>
#include <pulchritude-gpu/synchronization.h>
#include <pulchritude-physx/collision.h>
#include <pulchritude-physx/physx2d.h>
#include <pulchritude-render-graph/render-graph.h>
#include <pulchritude-text/text.h>


typedef struct PuleEngineLayer {
  struct PuleEngineLayer * parent;
  PuleString layerName;
  // allocator
  PuleAllocator (* allocateDefault)();
  void * (* allocate)(PuleAllocator const, PuleAllocateInfo const);
  void * (* reallocate)(PuleAllocator const, PuleReallocateInfo const);
  void (* deallocate)(PuleAllocator const, void * const);
  // array
  PuleArray (* array)(PuleArrayCreateInfo const);
  void (* arrayDestroy)(PuleArray const);
  void * (* arrayAppend)(PuleArray * const);
  void * (* arrayElementAt)(PuleArray const, size_t const);
  // asset-asset-script-task-graph
  PuleTaskGraph (* assetScriptTaskGraphFromPds)(PuleAllocator const, PuleScriptContext const, PuleDsValue const, PuleStringView const);
  // asset-image
  PuleAssetImageSupportFlag (* assetImageExtensionSupported)(char const * const);
  PuleAssetImage (* assetImageLoadFromStream)(PuleAllocator const, PuleStreamRead const, PuleAssetImageFormat const, PuleError * const);
  void (* assetImageDestroy)(PuleAssetImage const);
  void * (* assetImageDecodedData)(PuleAssetImage const);
  size_t (* assetImageDecodedDataLength)(PuleAssetImage const);
  uint32_t (* assetImageWidth)(PuleAssetImage const);
  uint32_t (* assetImageHeight)(PuleAssetImage const);
  // asset-tiled
  PuleAssetTiledMapInfo (* assetTiledMapInfo)(PuleAssetTiledMap const);
  PuleAssetTiledMap (* assetTiledMapLoadFromStream)(PuleAssetTiledMapLoadCreateInfo const, PuleError * const);
  PuleAssetTiledMap (* assetTiledMapLoadFromFile)(PuleAssetTiledMapLoadFromFileCreateInfo const, PuleError * const);
  void (* assetTiledMapDestroy)(PuleAssetTiledMap const);
  // asset-font
  PuleAssetFont (* assetFontLoad)(PuleBufferView const, PuleError * const);
  PuleAssetFont (* assetFontLoadFromPath)(PuleStringView const, PuleError * const);
  void (* assetFontDestroy)(PuleAssetFont const);
  void (* assetFontRenderToU8Buffer)(PuleAssetFontRenderInfo const, PuleError * const);
  // asset-pds
  PuleDsValue (* assetPdsLoadFromStream)(PuleAllocator const, PuleStreamRead const, PuleError * const);
  PuleDsValue (* assetPdsLoadFromRvalStream)(PuleAllocator const, PuleStreamRead const, PuleError * const);
  PuleDsValue (* assetPdsLoadFromFile)(PuleAllocator const, PuleStringView const, PuleError * const);
  PuleDsValue (* assetPdsLoadFromString)(PuleAllocator const, PuleStringView const, PuleError * const);
  void (* assetPdsWriteToStream)(PuleStreamWrite const, PuleAssetPdsWriteInfo const);
  void (* assetPdsWriteToFile)(PuleDsValue const, PuleStringView const, PuleError * const);
  void (* assetPdsWriteToStdout)(PuleDsValue const);
  PuleDsValue (* assetPdsLoadFromCommandLineArguments)(PuleAssetPdsCommandLineArgumentsInfo const, PuleError * const);
  // asset-render-graph
  PuleRenderGraph (* assetRenderGraphFromPds)(PuleAllocator const, PulePlatform const, PuleDsValue const);
  // asset-shader-module
  PuleAssetShaderModule (* assetShaderModuleCreateFromPaths)(PuleStringView const, PuleStringView const, PuleStringView const);
  void (* assetShaderModuleDestroy)(PuleAssetShaderModule const);
  PuleStringView (* assetShaderModuleLabel)(PuleAssetShaderModule);
  PuleGpuShaderModule (* assetShaderModuleHandle)(PuleAssetShaderModule);
  // camera
  PuleCamera (* cameraCreate)();
  void (* cameraDestroy)(PuleCamera);
  PuleF32m44 (* cameraView)(PuleCamera);
  PuleF32m44 (* cameraProj)(PuleCamera);
  void (* cameraLookAt)(PuleCamera, PuleF32v3 const, PuleF32v3 const, PuleF32v3 const);
  PuleCameraPerspective (* cameraPerspective)(PuleCamera);
  void (* cameraPerspectiveSet)(PuleCamera, PuleCameraPerspective);
  PuleCameraSet (* cameraSetCreate)(PuleStringView const);
  void (* cameraSetDestroy)(PuleCameraSet);
  void (* cameraSetAdd)(PuleCameraSet, PuleCamera);
  void (* cameraSetRemove)(PuleCameraSet, PuleCamera);
  PuleCameraSetArray (* cameraSetArray)(PuleCameraSet);
  PuleGpuBuffer (* cameraSetUniformBuffer)(PuleCameraSet);
  PuleGpuFence (* cameraSetRefresh)(PuleCameraSet);
  PuleCameraController (* cameraControllerFirstPerson)(PulePlatform const, PuleCamera);
  void (* cameraControllerDestroy)(PuleCameraController);
  void (* cameraControllerPollEvents)();
  // data-serializer
  int64_t (* dsAsI64)(PuleDsValue const);
  double (* dsAsF64)(PuleDsValue const);
  bool (* dsAsBool)(PuleDsValue const);
  PuleStringView (* dsAsString)(PuleDsValue const);
  PuleDsValueArray (* dsAsArray)(PuleDsValue const);
  PuleDsValueObject (* dsAsObject)(PuleDsValue const);
  PuleDsValueBuffer (* dsAsBuffer)(PuleDsValue const);
  float (* dsAsF32)(PuleDsValue const);
  int32_t (* dsAsI32)(PuleDsValue const);
  size_t (* dsAsUSize)(PuleDsValue const);
  uint64_t (* dsAsU64)(PuleDsValue const);
  uint32_t (* dsAsU32)(PuleDsValue const);
  uint16_t (* dsAsU16)(PuleDsValue const);
  int16_t (* dsAsI16)(PuleDsValue const);
  uint8_t (* dsAsU8)(PuleDsValue const);
  int8_t (* dsAsI8)(PuleDsValue const);
  bool (* dsIsI64)(PuleDsValue const);
  bool (* dsIsF64)(PuleDsValue const);
  bool (* dsIsString)(PuleDsValue const);
  bool (* dsIsArray)(PuleDsValue const);
  bool (* dsIsObject)(PuleDsValue const);
  bool (* dsIsBuffer)(PuleDsValue const);
  void (* dsDestroy)(PuleDsValue const);
  PuleDsValue (* dsCreateI64)(int64_t const);
  PuleDsValue (* dsCreateBool)(bool const);
  PuleDsValue (* dsCreateU64)(uint64_t const);
  PuleDsValue (* dsCreateF64)(double const);
  PuleDsValue (* dsCreateString)(PuleStringView const);
  PuleDsValue (* dsCreateArray)(PuleAllocator const);
  PuleDsValue (* dsCreateObject)(PuleAllocator const);
  PuleDsValue (* dsCreateBuffer)(PuleAllocator const, PuleArrayView const);
  PuleDsValue (* dsArrayAppend)(PuleDsValue const, PuleDsValue const);
  void (* dsArrayPopBack)(PuleDsValue const);
  void (* dsArrayPopFront)(PuleDsValue const);
  void (* dsArrayRemoveAt)(PuleDsValue const, size_t const);
  size_t (* dsArrayLength)(PuleDsValue const);
  PuleDsValue (* dsArrayElementAt)(PuleDsValue const, size_t const);
  PuleDsValue (* dsObjectMember)(PuleDsValue const, char const * const);
  bool (* dsIsNull)(PuleDsValue const);
  PuleDsValue (* dsValueCloneRecursively)(PuleDsValue const, PuleAllocator const);
  PuleDsValue (* dsObjectMemberAssign)(PuleDsValue const, PuleStringView const, PuleDsValue const);
  PuleDsValue (* dsObjectMemberOverwrite)(PuleDsValue const, PuleStringView const, PuleDsValue const);
  int64_t (* dsMemberAsI64)(PuleDsValue const, char const * const);
  double (* dsMemberAsF64)(PuleDsValue const, char const * const);
  float (* dsMemberAsF32)(PuleDsValue const, char const * const);
  bool (* dsMemberAsBool)(PuleDsValue const, char const * const);
  PuleStringView (* dsMemberAsString)(PuleDsValue const, char const * const);
  PuleDsValueArray (* dsMemberAsArray)(PuleDsValue const, char const * const);
  PuleDsValueObject (* dsMemberAsObject)(PuleDsValue const, char const * const);
  PuleDsValueBuffer (* dsMemberAsBuffer)(PuleDsValue const, char const * const);
  // ecs-serializer
  PuleDsValue (* ecsSerializeWorld)(PuleEcsSerializeWorldInfo const);
  void (* ecsDeserializeWorld)(PuleEcsDeserializeWorldInfo const);
  // ecs
  PuleEcsWorld (* ecsWorldCreate)();
  void (* ecsWorldDestroy)(PuleEcsWorld const);
  void (* ecsWorldAdvance)(PuleEcsWorld const, float const);
  PuleEcsComponent (* ecsComponentCreate)(PuleEcsWorld const, PuleEcsComponentCreateInfo const);
  void (* ecsEntityIterateComponents)(PuleEcsEntityIterateComponentsInfo const);
  PuleEcsComponent (* ecsComponentFetchByLabel)(PuleEcsWorld const, PuleStringView const);
  PuleStringView (* ecsComponentLabel)(PuleEcsWorld const, PuleEcsComponent const);
  PuleEcsComponentSerializer (* ecsComponentSerializer)(PuleEcsWorld const, PuleEcsComponent const);
  PuleEcsComponentInfo (* ecsComponentInfo)(PuleEcsWorld const, PuleEcsComponent const);
  void (* ecsComponentIterateAll)(PuleEcsComponentIterateAllCallback const);
  size_t (* ecsIteratorEntityCount)(PuleEcsIterator const);
  size_t (* ecsIteratorRelativeOffset)(PuleEcsIterator const);
  void * (* ecsIteratorQueryComponents)(PuleEcsIterator const, size_t const, size_t const);
  PuleEcsEntity * (* ecsIteratorQueryEntities)(PuleEcsIterator const);
  PuleEcsWorld (* ecsIteratorWorld)(PuleEcsIterator const);
  PuleEcsSystem (* ecsIteratorSystem)(PuleEcsIterator const);
  void * (* ecsIteratorUserData)(PuleEcsIterator const);
  PuleEcsSystem (* ecsSystemCreate)(PuleEcsWorld const, PuleEcsSystemCreateInfo const);
  void (* ecsSystemAdvance)(PuleEcsWorld const, PuleEcsSystem const, float const, void * const);
  PuleEcsEntity (* ecsEntityCreate)(PuleEcsWorld const, PuleStringView const);
  PuleStringView (* ecsEntityName)(PuleEcsWorld const, PuleEcsEntity const);
  void (* ecsEntityDestroy)(PuleEcsWorld const, PuleEcsEntity const);
  void (* ecsEntityAttachComponent)(PuleEcsWorld const, PuleEcsEntity const, PuleEcsComponent const, void const * const);
  void const * (* ecsEntityComponentData)(PuleEcsWorld const, PuleEcsEntity const, PuleEcsComponent const);
  PuleEcsQuery (* ecsQueryByComponent)(PuleEcsWorld const, PuleEcsComponent * const, size_t const, PuleError * const);
  PuleEcsQuery (* ecsQueryAllEntities)(PuleEcsWorld const, PuleError * const);
  void (* ecsQueryDestroy)(PuleEcsQuery const);
  PuleEcsQueryIterator (* ecsQueryIterator)(PuleEcsWorld const, PuleEcsQuery const);
  PuleEcsIterator (* ecsQueryIteratorNext)(PuleEcsQueryIterator const);
  void (* ecsQueryIteratorDestroy)(PuleEcsQueryIterator const);
  // error
  PuleError (* error)();
  uint32_t (* errorConsume)(PuleError * const);
  bool (* errorExists)(PuleError * const);
  // file
  PuleFile (* fileOpen)(PuleStringView const, PuleFileDataMode const, PuleFileOpenMode const, PuleError * const);
  void (* fileClose)(PuleFile const);
  bool (* fileIsDone)(PuleFile const);
  PuleStringView (* filePath)(PuleFile const);
  uint8_t (* fileReadByte)(PuleFile const);
  size_t (* fileReadBytes)(PuleFile const, PuleArrayViewMutable const);
  void (* fileWriteBytes)(PuleFile const, PuleArrayView const);
  void (* fileWriteString)(PuleFile const, PuleStringView const);
  uint64_t (* fileSize)(PuleFile const);
  void (* fileAdvanceFromStart)(PuleFile const, int64_t const);
  void (* fileAdvanceFromEnd)(PuleFile const, int64_t const);
  void (* fileAdvanceFromCurrent)(PuleFile const, int64_t const);
  PuleStreamRead (* fileStreamRead)(PuleFile const, PuleArrayViewMutable const);
  PuleStreamWrite (* fileStreamWrite)(PuleFile const, PuleArrayViewMutable const);
  bool (* filesystemPathExists)(PuleStringView const);
  bool (* fileCopy)(PuleStringView const, PuleStringView const);
  bool (* fileRemove)(PuleStringView const);
  bool (* fileRemoveRecursive)(PuleStringView const);
  bool (* fileDirectoryCreate)(PuleStringView const);
  bool (* fileDirectoryCreateRecursive)(PuleStringView const);
  PuleStringView (* filesystemExecutablePath)();
  PuleString (* filesystemCurrentPath)(PuleAllocator const);
  PuleStringView (* filesystemAssetPath)();
  void (* filesystemAssetPathSet)(PuleStringView const);
  PuleString (* filesystemAbsolutePath)(PuleStringView const);
  bool (* filesystemSymlinkCreate)(PuleStringView const, PuleStringView const);
  PuleTimestamp (* filesystemTimestamp)(PuleStringView const);
  PuleFileWatcher (* fileWatch)(PuleFileWatchCreateInfo const);
  bool (* fileWatchCheckAll)();
  // gfx-debug
  void (* gfxDebugInitialize)(PulePlatform const);
  void (* gfxDebugShutdown)();
  PuleGfxDebugRecorder (* gfxDebugStart)(PuleGpuCommandListRecorder const, PuleGpuActionRenderPassBegin const, PuleF32m44 const);
  void (* gfxDebugEnd)(PuleGfxDebugRecorder const);
  void (* gfxDebugRender)(PuleGfxDebugRecorder const, PuleGfxDebugRenderParam const);
  // gfx-mesh
  void (* gfxMeshRender)(PuleGfxMesh const * const, PuleGpuBuffer const, PuleGpuFramebuffer const, PuleGpuCommandListRecorder const);
  // imgui-engine
  void (* imguiEngineDisplay)(PuleImguiEngineDisplayInfo const);
  // imgui
  void (* imguiInitialize)(PulePlatform const);
  void (* imguiShutdown)();
  void (* imguiNewFrame)();
  void (* imguiRender)(PuleRenderGraph const);
  void (* imguiJoinNext)();
  bool (* imguiSliderF32)(char const * const, float * const, float const, float const);
  bool (* imguiSliderZu)(char const * const, size_t * const, size_t const, size_t const);
  void (* imguiWindowBegin)(char const * const, bool * const);
  void (* imguiWindowEnd)();
  bool (* imguiSectionBegin)(char const * const);
  void (* imguiSectionEnd)();
  void (* imguiText)(char const * const, ...);
  void (* imguiImage)(PuleGpuImage const, PuleF32v2 const, PuleF32v2 const, PuleF32v2 const, PuleF32v4 const);
  bool (* imguiLastItemHovered)();
  PuleI32v2 (* imguiCurrentOrigin)();
  bool (* imguiToggle)(char const * const, bool * const);
  bool (* imguiButton)(char const * const);
  PuleImguiCallbackRegistry (* imguiCallbackRegister)(PuleImguiCallback const);
  void (* imguiCallbackUnregister)(PuleImguiCallbackRegistry const);
  void (* imguiCallbackShowAll)();
  // log
  bool * (* logDebugEnabled)();
  bool * (* logErrorSegfaultsEnabled)();
  void (* log)(char const * const, ...);
  void (* logDebug)(char const * const, ...);
  void (* logWarn)(char const * const, ...);
  void (* logError)(char const * const, ...);
  void (* logLn)(char const * const, ...);
  void (* logRaw)(char const * const, ...);
  // math
  PuleF32v2 (* f32v2)(float const);
  PuleF32v2 (* f32v2Ptr)(float const * const);
  PuleF32v2 (* f32v2Add)(PuleF32v2 const, PuleF32v2 const);
  PuleF32v2 (* f32v2Sub)(PuleF32v2 const, PuleF32v2 const);
  PuleF32v2 (* f32v2Abs)(PuleF32v2 const);
  PuleI32v2 (* i32v2)(int32_t const);
  PuleI32v2 (* i32v2Ptr)(int32_t const * const);
  PuleI32v2 (* i32v2Add)(PuleI32v2 const, PuleI32v2 const);
  PuleI32v2 (* i32v2Sub)(PuleI32v2 const, PuleI32v2 const);
  PuleI32v2 (* i32v2Abs)(PuleI32v2 const);
  PuleF32v3 (* f32v3)(float const);
  PuleF32v3 (* f32v3Ptr)(float const * const);
  PuleF32v3 (* f32v3Add)(PuleF32v3 const, PuleF32v3 const);
  PuleF32v3 (* f32v3Sub)(PuleF32v3 const, PuleF32v3 const);
  PuleF32v3 (* f32v3Neg)(PuleF32v3 const);
  PuleF32v3 (* f32v3Mul)(PuleF32v3 const, PuleF32v3 const);
  PuleF32v3 (* f32v3MulScalar)(PuleF32v3 const, float const);
  PuleF32v3 (* f32v3Div)(PuleF32v3 const, PuleF32v3 const);
  float (* f32v3Dot)(PuleF32v3 const, PuleF32v3 const);
  float (* f32v3Length)(PuleF32v3 const);
  PuleF32v3 (* f32v3Normalize)(PuleF32v3 const);
  PuleF32v3 (* f32v3Cross)(PuleF32v3 const, PuleF32v3 const);
  PuleF32v4 (* f32v4)(float const);
  PuleF32m44 (* f32m44)(float const);
  PuleF32m44 (* f32m44Ptr)(float const * const);
  PuleF32m44 (* f32m44PtrTranspose)(float const * const);
  PuleF32m44 (* f32m44Inverse)(PuleF32m44 const);
  PuleF32v4 (* f32m44MulV4)(PuleF32m44 const, PuleF32v4 const);
  void (* f32m44DumpToStdout)(PuleF32m44);
  PuleF32m44 (* projectionPerspective)(float const, float const, float const, float const);
  PuleF32m44 (* viewLookAt)(PuleF32v3 const, PuleF32v3 const, PuleF32v3 const);
  // platform
  void (* platformInitialize)(PuleError * const);
  void (* platformShutdown)();
  PulePlatform (* platformCreate)(PulePlatformCreateInfo const, PuleError * const);
  void (* platformDestroy)(PulePlatform const);
  bool (* platformShouldExit)(PulePlatform const);
  void * (* platformGetProcessAddress)();
  char const * * (* platformRequiredExtensions)(uint32_t * const);
  void (* platformPollEvents)(PulePlatform const);
  void (* platformSwapFramebuffer)(PulePlatform const);
  PuleI32v2 (* platformWindowSize)(PulePlatform const);
  PuleI32v2 (* platformFramebufferSize)(PulePlatform const);
  void (* platformFramebufferResizeCallback)(PulePlatformFramebufferResizeCallbackCreateInfo const);
  void (* platformWindowResizeCallback)(PulePlatformWindowResizeCallbackCreateInfo const);
  double (* platformGetTime)();
  bool (* platformNull)(PulePlatform const);
  bool (* platformFocused)(PulePlatform const);
  void (* platformMouseOriginSet)(PulePlatform const, PuleI32v2 const);
  PuleI32v2 (* platformMouseOrigin)(PulePlatform const);
  bool (* platformCursorEnabled)(PulePlatform const);
  void (* platformCursorHide)(PulePlatform const);
  void (* platformCursorShow)(PulePlatform const);
  bool (* inputKey)(PulePlatform const, PuleInputKey const);
  PuleInputKeyModifier (* inputKeyModifiers)(PulePlatform const);
  bool (* inputMouse)(PulePlatform const, PuleInputMouse const);
  int32_t (* inputScroll)(PulePlatform const);
  void (* inputKeyCallback)(PuleInputKeyCallbackCreateInfo const);
  void (* inputMouseButtonCallback)(PuleInputMouseButtonCallbackCreateInfo const);
  void (* inputRawTextCallback)(PuleInputRawTextCallbackCreateInfo const);
  // plugin
  PulePluginPayload (* pluginPayloadCreate)(PuleAllocator const);
  void (* pluginPayloadDestroy)(PulePluginPayload const);
  void * (* pluginPayloadFetch)(PulePluginPayload const, PuleStringView const);
  uint64_t (* pluginPayloadFetchU64)(PulePluginPayload const, PuleStringView const);
  void (* pluginPayloadStore)(PulePluginPayload const, PuleStringView const, void * const);
  void (* pluginPayloadStoreU64)(PulePluginPayload const, PuleStringView const, uint64_t const);
  void (* pluginPayloadRemove)(PulePluginPayload const, PuleStringView const);
  void (* pluginsLoad)(PuleStringView const * const, size_t const);
  void (* pluginsFree)();
  void (* pluginsReload)();
  size_t (* pluginIdFromName)(char const * const);
  char const * (* pluginName)(size_t const);
  void * (* pluginLoadFn)(size_t const, char const * const);
  void * (* pluginLoadFnTry)(size_t const, char const * const);
  void (* pluginIterate)(void (* )(PulePluginInfo const, void * const), void * const);
  // raycast
  PuleRaycastTriangleResult (* raycastTriangles)(PuleF32v3 const, PuleF32v3 const, PuleArrayView const);
  // renderer-3d
  PuleRenderer3D (* renderer3DCreate)(PuleEcsWorld const, PulePlatform const);
  PuleEcsSystem (* renderer3DEcsSystem)(PuleRenderer3D const);
  PuleRenderer3DModel (* renderer3DPrepareModel)(PuleRenderer3D, PuleAssetModel const);
  PuleEcsComponent (* renderer3DAttachComponentRender)(PuleEcsWorld const, PuleRenderer3D const, PuleEcsEntity const, PuleRenderer3DModel const);
  // script
  PuleScriptContext (* scriptContextCreate)();
  void (* scriptContextDestroy)(PuleScriptContext const);
  PuleScriptModule (* scriptModuleCreateFromSource)(PuleScriptContext const, PuleScriptModuleFromSourceCreateInfo const, PuleError * const);
  void (* scriptModuleUpdateFromSource)(PuleScriptContext const, PuleScriptModule const, PuleStringView, PuleError * const);
  PuleScriptModule (* scriptModuleCreateFromBinary)(PuleScriptContext const, PuleArrayView const, PuleError * const);
  void (* scriptModuleDestroy)(PuleScriptContext const, PuleScriptModule const);
  void (* scriptModuleExecute)(PuleScriptContext const, PuleScriptModule const, PuleStringView const, PuleError * const);
  void * (* scriptGlobal)(PuleStringView const);
  void (* scriptGlobalSet)(PuleStringView const, void * const);
  PuleScriptArrayF32 (* scriptArrayF32Create)(PuleAllocator const);
  void (* scriptArrayF32Destroy)(PuleScriptArrayF32 const);
  void * (* scriptArrayF32Ptr)(PuleScriptArrayF32 const);
  void (* scriptArrayF32Append)(PuleScriptArrayF32 const, float const);
  void (* scriptArrayF32Remove)(PuleScriptArrayF32 const, size_t const);
  float (* scriptArrayF32At)(PuleScriptArrayF32 const, size_t const);
  size_t (* scriptArrayF32Length)(PuleScriptArrayF32 const);
  size_t (* scriptArrayF32ElementByteSize)(PuleScriptArrayF32 const);
  PuleScriptModuleFileWatchReturn (* scriptModuleFileWatch)(PuleScriptContext const, PuleAllocator const, PuleStringView const, PuleScriptDebugSymbols, PuleError * const);
  // stream
  uint8_t (* streamReadByte)(PuleStreamRead const);
  uint8_t (* streamPeekByte)(PuleStreamRead const);
  bool (* streamReadIsDone)(PuleStreamRead const);
  void (* streamReadDestroy)(PuleStreamRead const);
  PuleStreamRead (* streamReadFromString)(PuleStringView const);
  void (* streamWriteBytes)(PuleStreamWrite const, uint8_t const * const, size_t const);
  void (* streamWriteFlush)(PuleStreamWrite const);
  void (* streamWriteDestroy)(PuleStreamWrite const);
  PuleStreamWrite (* streamStdoutWrite)();
  // string
  PuleString (* stringDefault)(char const * const);
  PuleString (* string)(PuleAllocator const, char const * const);
  PuleString (* stringCopy)(PuleAllocator const, PuleStringView const);
  void (* stringDestroy)(PuleString const);
  void (* stringAppend)(PuleString * const, PuleStringView const);
  PuleString (* stringFormat)(PuleAllocator const, char const * const, ...);
  PuleString (* stringFormatDefault)(char const * const, ...);
  PuleStringView (* stringView)(PuleString const);
  PuleStringView (* cStr)(char const * const);
  bool (* stringViewEq)(PuleStringView const, PuleStringView const);
  bool (* stringViewContains)(PuleStringView const, PuleStringView const);
  bool (* stringViewEqCStr)(PuleStringView const, char const * const);
  size_t (* stringViewHash)(PuleStringView const);
  // task-graph
  PuleTaskGraph (* taskGraphCreate)(PuleAllocator const);
  void (* taskGraphDestroy)(PuleTaskGraph const);
  void (* taskGraphMerge)(PuleTaskGraph const, PuleTaskGraph const);
  PuleTaskGraphNode (* taskGraphNodeCreate)(PuleTaskGraph const, PuleStringView const);
  void (* taskGraphNodeRemove)(PuleTaskGraphNode const);
  PuleStringView (* taskGraphNodeLabel)(PuleTaskGraphNode const);
  PuleTaskGraphNode (* taskGraphNodeFetch)(PuleTaskGraph const, PuleStringView const);
  void (* taskGraphNodeAttributeStore)(PuleTaskGraphNode const, PuleStringView const, void * const);
  void (* taskGraphNodeAttributeStoreU64)(PuleTaskGraphNode const, PuleStringView const, uint64_t const);
  void * (* taskGraphNodeAttributeFetch)(PuleTaskGraphNode const, PuleStringView const);
  uint64_t (* taskGraphNodeAttributeFetchU64)(PuleTaskGraphNode const, PuleStringView const);
  void (* taskGraphNodeAttributeRemove)(PuleTaskGraphNode const, PuleStringView const);
  void (* taskGraphNodeRelationSet)(PuleTaskGraphNode const, PuleTaskGraphNodeRelation const, PuleTaskGraphNode const);
  void (* taskGraphExecuteInOrder)(PuleTaskGraphExecuteInfo const);
  bool (* taskGraphNodeExists)(PuleTaskGraphNode const);
  // time
  PuleMicrosecond (* microsecond)(int64_t const);
  void (* sleepMicrosecond)(PuleMicrosecond const);
  // tui
  PuleTuiWindow (* tuiInitialize)();
  void (* tuiDestroy)(PuleTuiWindow const);
  void (* tuiClear)(PuleTuiWindow const);
  void (* tuiRefresh)(PuleTuiWindow const);
  void (* tuiRenderString)(PuleTuiWindow const, PuleStringView const, PuleF32v3 const);
  int32_t (* tuiReadInputCharacterBlocking)(PuleTuiWindow const);
  void (* tuiMoveCursor)(PuleTuiWindow const, PuleI32v2 const);
  PuleI32v2 (* tuiWindowDim)(PuleTuiWindow const);
  // gpu
  PuleStringView (* gpuActionToString)(PuleGpuAction const);
  PuleGpuCommandList (* gpuCommandListCreate)(PuleAllocator const, PuleStringView const);
  void (* gpuCommandListDestroy)(PuleGpuCommandList const);
  PuleStringView (* gpuCommandListName)(PuleGpuCommandList const);
  PuleGpuCommandListRecorder (* gpuCommandListRecorder)(PuleGpuCommandList const);
  void (* gpuCommandListRecorderFinish)(PuleGpuCommandListRecorder const);
  void (* gpuCommandListAppendAction)(PuleGpuCommandListRecorder const, PuleGpuCommand const);
  void (* gpuCommandListSubmit)(PuleGpuCommandListSubmitInfo const, PuleError * const);
  void (* gpuCommandListSubmitAndPresent)(PuleGpuCommandListSubmitInfo const, PuleError * const);
  void (* gpuCommandListDump)(PuleGpuCommandList const);
  PuleGpuCommandListChain (* gpuCommandListChainCreate)(PuleAllocator const, PuleStringView const);
  void (* gpuCommandListChainDestroy)(PuleGpuCommandListChain const);
  PuleGpuCommandList (* gpuCommandListChainCurrent)(PuleGpuCommandListChain const);
  PuleGpuFence (* gpuCommandListChainCurrentFence)(PuleGpuCommandListChain const);
  PuleString (* gpuResourceBarrierStageLabel)(PuleGpuResourceBarrierStage const);
  PuleString (* gpuResourceAccessLabel)(PuleGpuResourceAccess const);
  PuleGpuBuffer (* gpuBufferCreate)(PuleStringView const, void const * const, size_t const, PuleGpuBufferUsage const, PuleGpuBufferVisibilityFlag const);
  void (* gpuBufferDestroy)(PuleGpuBuffer const);
  void * (* gpuBufferMap)(PuleGpuBufferMapRange const);
  void (* gpuBufferMappedFlush)(PuleGpuBufferMappedFlushRange const);
  void (* gpuBufferUnmap)(PuleGpuBuffer const);
  void (* gpuInitialize)(PulePlatform const, PuleError * const);
  void (* gpuShutdown)();
  void (* gpuDebugPrint)();
  PuleStringView (* gpuBufferUsageLabel)(PuleGpuBufferUsage const);
  PuleGpuSampler (* gpuSamplerCreate)(PuleGpuSamplerCreateInfo const);
  void (* gpuSamplerDestroy)(PuleGpuSampler const);
  PuleStringView (* gpuImageLabel)(PuleGpuImage const);
  void (* gpuImageDestroy)(PuleGpuImage const);
  PuleGpuImage (* gpuImageCreate)(PuleGpuImageCreateInfo const);
  PuleGpuFramebufferCreateInfo (* gpuFramebufferCreateInfo)();
  PuleGpuFramebuffer (* gpuFramebufferCreate)(PuleGpuFramebufferCreateInfo const, PuleError * const);
  void (* gpuFramebufferDestroy)(PuleGpuFramebuffer const);
  PuleStringView (* gpuImageLayoutLabel)(PuleGpuImageLayout const);
  PuleGpuPipelineLayoutDescriptorSet (* gpuPipelineDescriptorSetLayout)();
  PuleGpuPipeline (* gpuPipelineCreate)(PuleGpuPipelineCreateInfo const * const, PuleError * const);
  void (* gpuPipelineDestroy)(PuleGpuPipeline const);
  PuleGpuImageChain (* gpuImageChain_create)(PuleAllocator const, PuleStringView const, PuleGpuImageCreateInfo const);
  void (* gpuImageChain_destroy)(PuleGpuImageChain const);
  PuleGpuImage (* gpuImageChain_current)(PuleGpuImageChain const);
  PuleGpuImageReference (* gpuImageReference_createImageChain)(PuleGpuImageChain const);
  PuleGpuImageReference (* gpuImageReference_createImage)(PuleGpuImage const);
  void (* gpuImageReference_updateImageChain)(PuleGpuImageChain const, PuleGpuImageChain const);
  void (* gpuImageReference_updateImage)(PuleGpuImage const, PuleGpuImage const);
  void (* gpuImageReference_destroy)(PuleGpuImageReference const);
  PuleGpuImage (* gpuImageReference_image)(PuleGpuImageReference const);
  PuleGpuImage (* gpuWindowSwapchainImage)();
  PuleGpuImageReference (* gpuWindowSwapchainImageReference)();
  PuleGpuShaderModule (* gpuShaderModuleCreate)(PuleBufferView const, PuleBufferView const, PuleError * const);
  void (* gpuShaderModuleDestroy)(PuleGpuShaderModule const);
  PuleGpuSemaphore (* gpuSemaphoreCreate)(PuleStringView const);
  void (* gpuSemaphoreDestroy)(PuleGpuSemaphore const);
  PuleGpuFence (* gpuFenceCreate)(PuleStringView const);
  void (* gpuFenceDestroy)(PuleGpuFence const);
  bool (* gpuFenceWaitSignal)(PuleGpuFence const, PuleNanosecond const);
  void (* gpuFenceReset)(PuleGpuFence const);
  PuleGpuSemaphore (* gpuFrameStart)();
  void (* gpuFrameEnd)(size_t const, PuleGpuSemaphore const * const);
  // physx
  PulePhysxCollisionResultShape (* physxRaycastShape)(PulePhysxMode const, PulePhysxRay const, PuleF32m44 const, PulePhysxCollisionShape const);
  PulePhysxCollisionResultMesh (* physxRaycastMesh)(PulePhysxMode const, PulePhysxRay const, PuleF32m44 const, PulePhysxCollisionMesh const);
  bool (* physxShapeOverlapShape)(PulePhysxMode const, PuleF32m44 const, PulePhysxCollisionShape const, PuleF32m44 const, PulePhysxCollisionShape const);
  bool (* physxShapeOverlapMesh)(PulePhysxMode const, PuleF32m44 const, PulePhysxCollisionShape const, PuleF32m44 const, PulePhysxCollisionMesh const);
  PulePhysx2DWorld (* physx2DWorldCreate)();
  void (* physx2DWorldDestroy)(PulePhysx2DWorld const);
  void (* physx2DWorldGravitySet)(PulePhysx2DWorld const, PuleF32v2 const);
  void (* physx2DWorldStep)(PulePhysx2DWorld const, float const, int const, int const);
  PulePhysx2DBody (* physx2DBodyCreate)(PulePhysx2DWorld const, PulePhysx2DBodyCreateInfo const);
  void (* physx2DBodyDestroy)(PulePhysx2DWorld const, PulePhysx2DBody const);
  PuleF32v2 (* physx2DBodyPosition)(PulePhysx2DWorld const, PulePhysx2DBody const);
  float (* physx2DBodyAngle)(PulePhysx2DWorld const, PulePhysx2DBody const);
  void (* physx2DBodyAttachShape)(PulePhysx2DWorld const, PulePhysx2DBody const, PulePhysx2DShape const, PulePhysx2DBodyAttachShapeCreateInfo const);
  PulePhysx2DShape (* physx2DShapeCreateConvexPolygonAsBox)(PuleF32v2 const, PuleF32v2 const, float const);
  // render-graph
  PuleRenderGraph (* renderGraphCreate)(PuleAllocator const);
  void (* renderGraphDestroy)(PuleRenderGraph const);
  void (* renderGraphMerge)(PuleRenderGraph const, PuleRenderGraph const);
  PuleRenderGraphNode (* renderGraphNodeCreate)(PuleRenderGraph const, PuleStringView const);
  void (* renderGraphNodeRemove)(PuleRenderGraphNode const);
  PuleStringView (* renderGraphNodeLabel)(PuleRenderGraphNode const);
  PuleRenderGraphNode (* renderGraphNodeFetch)(PuleRenderGraph const, PuleStringView const);
  void (* renderGraph_resourceCreate)(PuleRenderGraph const, PuleRenderGraph_Resource const);
  PuleRenderGraph_Resource (* renderGraph_resource)(PuleRenderGraph const, PuleStringView const);
  void (* renderGraph_resourceRemove)(PuleRenderGraph const, PuleStringView const);
  void (* renderGraph_node_resourceAssign)(PuleRenderGraphNode const, PuleRenderGraph_Node_Resource const);
  PuleGpuCommandList (* renderGraph_commandList)(PuleRenderGraphNode const);
  PuleGpuCommandListRecorder (* renderGraph_commandListRecorder)(PuleRenderGraphNode const);
  void (* renderGraphNodeRelationSet)(PuleRenderGraphNode const, PuleRenderGraphNodeRelation const, PuleRenderGraphNode const);
  void (* renderGraphFrameStart)(PuleRenderGraph const);
  void (* renderGraphFrameSubmit)(PuleGpuSemaphore const, PuleRenderGraph const);
  void (* renderGraphExecuteInOrder)(PuleRenderGraphExecuteInfo const);
  bool (* renderGraphNodeExists)(PuleRenderGraphNode const);
  // text
  PuleTextRenderer (* textRendererCreate)(PuleAssetFont const, PuleTextType const);
  void (* textRendererDestroy)(PuleTextRenderer const);
  void (* textRender)(PuleTextRenderer const, PuleGpuCommandListRecorder const, PuleTextRenderInfo const * const, size_t const);
} PuleEngineLayer;


#ifdef __cplusplus
extern "C" {
#endif

PULE_exportFn void pulePluginLoadEngineLayer(
  PuleEngineLayer * engineLayerBase,
  PuleStringView const layerLabel, // ie 'default', 'debug'
  PuleEngineLayer * layerParentNullable
);


#ifdef __cplusplus
} // extern C
#endif