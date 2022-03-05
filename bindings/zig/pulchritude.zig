pub const PuleString = extern struct {
  contents: [*c] u8,
  len: usize,
  allocator: PuleAllocator,
};
pub const PuleStringView = extern struct {
  contents: [*c] const u8,
  len: usize,
};
pub extern fn puleStringDefault(
  baseContents: [*c] const u8,
) callconv(.C) PuleString;
pub extern fn puleString(
  allocator: PuleAllocator,
  baseContents: [*c] const u8,
) callconv(.C) PuleString;
pub extern fn puleStringDeallocate(
  puleStringInout: [*c] PuleString,
) callconv(.C) void;
pub extern fn puleStringAppend(
  stringInout: [*c] PuleString,
  append: PuleStringView,
) callconv(.C) void;
pub extern fn puleStringFormat(
  allocator: PuleAllocator,
  format: [*c] const u8,
  ...
) callconv(.C) PuleString;
pub extern fn puleStringFormatDefault(
  format: [*c] const u8,
  ...
) callconv(.C) PuleString;
pub extern fn puleStringView(
  string: PuleString,
) callconv(.C) PuleStringView;
pub extern fn puleStringViewCStr(
  cstr: [*c] const u8,
) callconv(.C) PuleStringView;
pub extern fn puleLogDebugEnabled() callconv(.C) [*c] bool;
pub extern fn puleLog(
  formatCStr: [*c] const u8,
  ...
) callconv(.C) void;
pub extern fn puleLogDebug(
  formatCStr: [*c] const u8,
  ...
) callconv(.C) void;
pub extern fn puleLogError(
  formatCStr: [*c] const u8,
  ...
) callconv(.C) void;
pub const PuleGfxAction = enum(u32) {
  bindPipeline = 0,
  dispatchRender = 1,
  dispatchRenderElements = 2,
  clearFramebufferColor = 3,
  clearFramebufferDepth = 4,
  pushConstants = 5,
};
pub const PuleGfxActionBindPipeline = extern struct {
  action: PuleGfxAction,
  pipeline: PuleGfxPipeline,
};
pub const PuleGfxDrawPrimitive = enum(u32) {
  triangle = 0,
  triangleStrip = 1,
  point = 2,
  line = 3,
};
pub const PuleGfxActionDispatchRender = extern struct {
  action: PuleGfxAction,
  drawPrimitive: PuleGfxDrawPrimitive,
  vertexOffset: usize,
  numVertices: usize,
};
pub const PuleGfxActionClearFramebufferColor = extern struct {
  action: PuleGfxAction,
  red: f32,
  green: f32,
  blue: f32,
  alpha: f32,
};
pub const PuleGfxActionClearFramebufferDepth = extern struct {
  action: PuleGfxAction,
  depth: f32,
};
pub const PuleGfxCommand = extern union {
  bindPipeline: PuleGfxActionBindPipeline,
  dispatchRender: PuleGfxActionDispatchRender,
  clearFramebufferColor: PuleGfxActionClearFramebufferColor,
  clearFramebufferDepth: PuleGfxActionClearFramebufferDepth,
};
pub const PuleGfxCommandList = extern struct {
  id: usize,
};
pub const PuleGfxCommandListRecorder = extern struct {
  id: usize,
};
pub extern fn puleGfxCommandListCreate() callconv(.C) PuleGfxCommandList;
pub extern fn puleGfxCommandListDestroy(
  commandList: PuleGfxCommandList,
) callconv(.C) void;
pub extern fn puleGfxCommandListRecorder(
  commandList: PuleGfxCommandList,
) callconv(.C) PuleGfxCommandListRecorder;
pub extern fn puleGfxCommandListRecorderFinish(
  commandListRecorder: PuleGfxCommandListRecorder,
) callconv(.C) void;
pub extern fn puleGfxCommandListAppendAction(
  commandListRecorder: PuleGfxCommandListRecorder,
  action: PuleGfxCommand,
) callconv(.C) void;
pub extern fn puleGfxCommandListSubmit(
  commandList: PuleGfxCommandList,
) callconv(.C) void;
pub const PuleGfxShaderModule = extern struct {
  id: u64,
};
pub extern fn puleGfxShaderModuleCreate(
  vertexShaderSource: [*c] const u8,
  fragmentShaderSource: [*c] const u8,
  err: [*c] PuleError,
) callconv(.C) PuleGfxShaderModule;
pub extern fn puleGfxShaderModuleDestroy(
  shaderModule: PuleGfxShaderModule,
) callconv(.C) void;
pub const PuleErrorGfx = enum(u32) {
  none = 0,
  creationFailed = 1,
  shaderModuleCompilationFailed = 2,
  invalidDescriptorSet = 3,
};
pub const PuleGfxContext = extern struct {
  implementation: * anyopaque,
};
pub const PuleGfxGpuImage = extern struct {
  id: u64,
};
pub const PuleGfxGpuFramebuffer = extern struct {
  id: u64,
};
pub const PuleGfxGpuBuffer = extern struct {
  id: u64,
};
pub const PuleGfxGpuBufferUsage = enum(u32) {
  bufferAttribute = 0,
  bufferElement = 1,
  bufferUniform = 2,
  bufferStorage = 3,
  bufferAccelerationStructure = 4,
  bufferIndirect = 5,
};
pub const PuleGfxGpuBufferVisibilityFlag = enum(u32) {
  deviceOnly = 1,
  hostVisible = 2,
  hostWritable = 4,
};
pub extern fn puleGfxGpuBufferCreate(
  nullableInitialData: * anyopaque,
  byteLength: usize,
  usage: PuleGfxGpuBufferUsage,
  visibility: PuleGfxGpuBufferVisibilityFlag,
) callconv(.C) PuleGfxGpuBuffer;
pub extern fn puleGfxGpuBufferDestroy(
  buffer: PuleGfxGpuBuffer,
) callconv(.C) void;
pub const PuleGfxDescriptorType = enum(u32) {
  sampler = 0,
  uniformBuffer = 1,
  storageBuffer = 2,
};
pub const PuleGfxAttributeDataType = enum(u32) {
  float = 0,
};
pub const PuleGfxPipelineDescriptorAttributeBinding = extern struct {
  buffer: PuleGfxGpuBuffer,
  numComponents: usize,
  dataType: PuleGfxAttributeDataType,
  normalizeFixedDataTypeToNormalizedFloating: bool,
  stridePerElement: usize,
  offsetIntoBuffer: usize,
};
pub const PuleGfxPipelineDescriptorSetLayout = extern struct {
  bufferUniformBindings: [16] PuleGfxGpuBuffer,
  bufferAttributeBindings: [16] PuleGfxPipelineDescriptorAttributeBinding,
  bufferElementBinding: PuleGfxGpuBuffer,
};
pub extern fn puleGfxPipelineDescriptorSetLayout() callconv(.C) PuleGfxPipelineDescriptorSetLayout;
pub const PuleGfxPipelineLayout = extern struct {
  id: u64,
};
pub extern fn puleGfxPipelineLayoutCreate(
  descriptorSetLayout: [*c] const PuleGfxPipelineDescriptorSetLayout,
  err: [*c] PuleError,
) callconv(.C) PuleGfxPipelineLayout;
pub extern fn puleGfxPipelineLayoutDestroy(
  pipelineLayout: PuleGfxPipelineLayout,
) callconv(.C) void;
pub const PuleGfxPipeline = extern struct {
  shaderModule: PuleGfxShaderModule,
  layout: PuleGfxPipelineLayout,
};
pub const PuleGfxPushConstant = extern struct {
  index: usize,
  data: * anyopaque,
};
pub extern fn puleGfxInitialize(
  err: [*c] PuleError,
) callconv(.C) void;
pub extern fn puleGfxShutdown() callconv(.C) void;
pub extern fn puleGfxFrameStart() callconv(.C) void;
pub extern fn puleGfxFrameEnd() callconv(.C) void;
pub extern fn puleGfxGpuImageCreate(
  width: u32,
  height: u32,
  data: * const anyopaque,
) callconv(.C) void;
pub const PuleFloat2 = extern struct {
  x: f32,
  y: f32,
};
pub const PuleFloat3 = extern struct {
  x: f32,
  y: f32,
  z: f32,
};
pub const PuleFloat4 = extern struct {
  x: f32,
  y: f32,
  z: f32,
  w: f32,
};
pub const PuleFloat4x4 = extern struct {
  elements: [16] f32,
};
pub const PuleErrorArray = enum(u32) {
  none = 0,
  errorAllocation = 1,
};
pub const PuleArray = extern struct {
  content: * anyopaque,
  elementByteLength: usize,
  elementAlignmentByteLength: usize,
  elementCount: usize,
  allocator: PuleAllocator,
};
pub const PuleArrayCreateInfo = extern struct {
  elementByteLength: usize,
  elementAlignmentByteLength: usize,
  allocator: PuleAllocator,
};
pub extern fn puleArray(
  info: PuleArrayCreateInfo,
) callconv(.C) PuleArray;
pub extern fn puleArrayDestroy(
  array: PuleArray,
) callconv(.C) void;
pub extern fn puleArrayAppend(
  array: [*c] PuleArray,
) callconv(.C) * anyopaque;
pub extern fn puleArrayElementAt(
  array: PuleArray,
  idx: usize,
) callconv(.C) * anyopaque;
pub const PuleArrayView = extern struct {
  data: * const anyopaque,
  elementStride: usize,
  elementCount: usize,
};
pub const PuleWindowVsyncMode = enum(u32) {
  none = 0,
  single = 1,
  triple = 2,
};
pub const PuleErrorWindow = enum(u32) {
  none = 0,
  windowCreationFailed = 1,
  invalidConfiguration = 2,
};
pub const PuleWindowCreateInfo = extern struct {
  name: PuleStringView,
  width: usize,
  height: usize,
  vsyncMode: PuleWindowVsyncMode,
};
pub const PuleWindow = extern struct {
  data: * anyopaque,
};
pub extern fn puleWindowInitialize(
  err: [*c] PuleError,
) callconv(.C) void;
pub extern fn puleWindowShutdown() callconv(.C) void;
pub extern fn puleWindowCreate(
  info: PuleWindowCreateInfo,
  err: [*c] PuleError,
) callconv(.C) PuleWindow;
pub extern fn puleWindowDestroy(
  window: PuleWindow,
) callconv(.C) void;
pub extern fn puleWindowGetProcessAddress() callconv(.C) * anyopaque;
pub extern fn puleWindowPollEvents(
  window: PuleWindow,
) callconv(.C) void;
pub extern fn puleWindowSwapFramebuffer(
  window: PuleWindow,
) callconv(.C) void;
pub const PulePluginType = enum(u32) {
  library = 0,
  component = 1,
};
pub const PulePluginInfo = extern struct {
  name: [*c] const u8,
  id: usize,
};
pub extern fn pulePluginsLoad() callconv(.C) void;
pub extern fn pulePluginsFree() callconv(.C) void;
pub extern fn pulePluginsReload() callconv(.C) void;
pub extern fn pulePluginIdFromName(
  pluginNameCStr: [*c] const u8,
) callconv(.C) usize;
pub extern fn pulePluginLoadFn(
  pluginId: usize,
  fnCStr: [*c] const u8,
) callconv(.C) * anyopaque;
pub extern fn puleTryPluginLoadFn(
  pluginId: usize,
  fnCStr: [*c] const u8,
) callconv(.C) * anyopaque;
pub extern fn puleIteratePlugins(
  userdata: * anyopaque,
) callconv(.C) void;
pub const PuleError = extern struct {
  description: PuleString,
  id: u32,
};
pub extern fn puleErrorConsume(
  err: [*c] PuleError,
) callconv(.C) u32;
pub const PuleAllocateInfo = extern struct {
  zeroOut: u8,
  numBytes: usize,
  alignment: usize,
};
pub const PuleReallocateInfo = extern struct {
  allocation: * anyopaque,
  numBytes: usize,
  alignment: usize,
};
pub const PuleAllocator = extern struct {
  implementation: * anyopaque,
};
pub extern fn puleAllocateDefault() callconv(.C) PuleAllocator;
pub extern fn puleAllocate(
  allocator: PuleAllocator,
  allocateInfo: PuleAllocateInfo,
) callconv(.C) * anyopaque;
pub extern fn puleReallocate(
  allocator: PuleAllocator,
  reallocateInfo: PuleReallocateInfo,
) callconv(.C) * anyopaque;
pub extern fn puleDeallocate(
  allocator: PuleAllocator,
  allocationNullable: * anyopaque,
) callconv(.C) void;
