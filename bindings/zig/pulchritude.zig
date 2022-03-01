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
pub const PuleGfxShader = extern struct {
  id: u64,
};
pub const PuleGfxPass = extern struct {
  id: u64,
};
pub extern fn puleGfxCreate() callconv(.C) void;
pub extern fn puleGfxDestroy() callconv(.C) void;
pub extern fn puleGfxFrameStart() callconv(.C) void;
pub extern fn puleGfxFrameEnd() callconv(.C) void;
pub extern fn puleGfxGpuImageCreate(
  width: u32,
  height: u32,
  data: * const anyopaque,
) callconv(.C) void;
pub const PuleErrorArray = enum(u32) {
  none = 0,
  errorAllocation = 1,
};
pub const PuleArrayView = extern struct {
  data: * const anyopaque,
  len: usize,
};
pub const PuleWindowVsyncMode = enum(u32) {
  none = 0,
  single = 1,
  triple = 2,
};
pub const PuleErrorWindow = enum(u32) {
  windowCreationFailed = 0,
  invalidConfiguration = 1,
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
pub extern fn puleWindowCreate(
  info: PuleWindowCreateInfo,
  err: [*c] PuleError,
) callconv(.C) PuleWindow;
pub extern fn puleWindowDestroy(
  window: PuleWindow,
) callconv(.C) void;
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
