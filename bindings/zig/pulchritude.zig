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
pub const PuleF32v2 = extern struct {
  x: f32,
  y: f32,
};
pub const PuleF32v3 = extern struct {
  x: f32,
  y: f32,
  z: f32,
};
pub const PuleF32v4 = extern struct {
  x: f32,
  y: f32,
  z: f32,
  w: f32,
};
pub const PuleI32v2 = extern struct {
  x: i32,
  y: i32,
};
pub const PuleI32v3 = extern struct {
  x: i32,
  y: i32,
  z: i32,
};
pub const PuleI32v4 = extern struct {
  x: i32,
  y: i32,
  z: i32,
  w: i32,
};
pub extern fn pulef32v2(
  identity: f32,
) callconv(.C) PuleF32v2;
pub extern fn pulef32v2Ptr(
  values: [*c] const f32,
) callconv(.C) PuleF32v3;
pub extern fn pulef32v3(
  identity: f32,
) callconv(.C) PuleF32v3;
pub extern fn pulef32v3Ptr(
  values: [*c] const f32,
) callconv(.C) PuleF32v3;
pub extern fn pulef32v3Add(
  a: PuleF32v3,
  b: PuleF32v3,
) callconv(.C) PuleF32v3;
pub extern fn pulef32v3Sub(
  a: PuleF32v3,
  b: PuleF32v3,
) callconv(.C) PuleF32v3;
pub extern fn pulef32v3Neg(
  a: PuleF32v3,
) callconv(.C) PuleF32v3;
pub extern fn pulef32v3Mul(
  a: PuleF32v3,
  b: PuleF32v3,
) callconv(.C) PuleF32v3;
pub extern fn pulef32v3Div(
  a: PuleF32v3,
  b: PuleF32v3,
) callconv(.C) PuleF32v3;
pub extern fn pulef32v3Dot(
  a: PuleF32v3,
  b: PuleF32v3,
) callconv(.C) f32;
pub extern fn pulef32v3Length(
  a: PuleF32v3,
) callconv(.C) f32;
pub extern fn pulef32v3Normalize(
  b: PuleF32v3,
) callconv(.C) PuleF32v3;
pub extern fn pulef32v3Cross(
  a: PuleF32v3,
  b: PuleF32v3,
) callconv(.C) PuleF32v3;
pub extern fn pulef32v4(
  identity: f32,
) callconv(.C) PuleF32v4;
pub const PuleF32m44 = extern struct {
  elements: [16] f32,
};
pub extern fn puleF32m44(
  identity: f32,
) callconv(.C) PuleF32m44;
pub extern fn puleF32m44Ptr(
  data: [*c] const f32,
) callconv(.C) PuleF32m44;
pub extern fn puleF32m44PtrTranspose(
  data: [*c] const f32,
) callconv(.C) PuleF32m44;
pub extern fn puleProjectionPerspective(
  fieldOfViewRadians: f32,
  aspectRatio: f32,
  near: f32,
  far: f32,
) callconv(.C) PuleF32m44;
pub extern fn puleViewLookAt(
  origin: PuleF32v3,
  center: PuleF32v3,
  up: PuleF32v3,
) callconv(.C) PuleF32m44;
pub const PulePlatformVsyncMode = enum(u32) {
  none = 0,
  double = 1,
  triple = 2,
  End = 3,
};
pub const PuleErrorWindow = enum(u32) {
  none = 0,
  windowCreationFailed = 1,
  invalidConfiguration = 2,
};
pub const PulePlatformCreateInfo = extern struct {
  name: PuleStringView,
  width: usize,
  height: usize,
  vsyncMode: PulePlatformVsyncMode,
};
pub const PulePlatform = extern struct {
  data: ?* anyopaque,
};
pub extern fn pulePlatformInitialize(
  err: [*c] PuleError,
) callconv(.C) void;
pub extern fn pulePlatformShutdown() callconv(.C) void;
pub extern fn pulePlatformCreate(
  info: PulePlatformCreateInfo,
  err: [*c] PuleError,
) callconv(.C) PulePlatform;
pub extern fn pulePlatformDestroy(
  window: PulePlatform,
) callconv(.C) void;
pub extern fn pulePlatformShouldExit(
  window: PulePlatform,
) callconv(.C) bool;
pub extern fn pulePlatformGetProcessAddress() callconv(.C) ?* anyopaque;
pub extern fn pulePlatformPollEvents(
  window: PulePlatform,
) callconv(.C) void;
pub extern fn pulePlatformSwapFramebuffer(
  window: PulePlatform,
) callconv(.C) void;
pub extern fn pulePlatformWindowSize(
  window: PulePlatform,
) callconv(.C) PuleI32v2;
pub extern fn pulePlatformFramebufferSize(
  window: PulePlatform,
) callconv(.C) PuleI32v2;
pub const PulePlatformFramebufferResizeCallbackCreateInfo = extern struct {
};
pub extern fn pulePlatformFramebufferResizeCallback(
  info: PulePlatformFramebufferResizeCallbackCreateInfo,
) callconv(.C) void;
pub const PulePlatformWindowResizeCallbackCreateInfo = extern struct {
};
pub extern fn pulePlatformWindowResizeCallback(
  info: PulePlatformWindowResizeCallbackCreateInfo,
) callconv(.C) void;
pub extern fn pulePlatformGetTime() callconv(.C) f64;
pub extern fn pulePlatformNull(
  window: PulePlatform,
) callconv(.C) bool;
pub extern fn pulePlatformFocused(
  window: PulePlatform,
) callconv(.C) bool;
pub extern fn pulePlatformMouseOriginSet(
  window: PulePlatform,
  origin: PuleI32v2,
) callconv(.C) void;
pub extern fn pulePlatformMouseOrigin(
  window: PulePlatform,
) callconv(.C) PuleI32v2;
pub extern fn pulePlatformCursorEnabled(
  window: PulePlatform,
) callconv(.C) bool;
pub extern fn pulePlatformCursorHide(
  window: PulePlatform,
) callconv(.C) void;
pub extern fn pulePlatformCursorShow(
  window: PulePlatform,
) callconv(.C) void;
pub const PuleInputKey = enum(u32) {
  tab = 0,
  left = 1,
  right = 2,
  up = 3,
  down = 4,
  pageUp = 5,
  pageDown = 6,
  home = 7,
  end = 8,
  insert = 9,
  delete = 10,
  backspace = 11,
  space = 12,
  enter = 13,
  escape = 14,
  apostrophe = 15,
  comma = 16,
  minus = 17,
  period = 18,
  slash = 19,
  semicolon = 20,
  equal = 21,
  leftBracket = 22,
  backslash = 23,
  rightBracket = 24,
  graveAccent = 25,
  capsLock = 26,
  scrollLock = 27,
  numLock = 28,
  printScreen = 29,
  pause = 30,
  kp0 = 31,
  kp1 = 32,
  kp2 = 33,
  kp3 = 34,
  kp4 = 35,
  kp5 = 36,
  kp6 = 37,
  kp7 = 38,
  kp8 = 39,
  kp9 = 40,
  kpDecimal = 41,
  kpDivide = 42,
  kpMultiply = 43,
  kpSubtract = 44,
  kpAdd = 45,
  kpEnter = 46,
  kpEqual = 47,
  leftShift = 48,
  leftControl = 49,
  leftAlt = 50,
  leftSuper = 51,
  rightShift = 52,
  rightControl = 53,
  rightAlt = 54,
  rightSuper = 55,
  menu = 56,
  i0 = 57,
  i1 = 58,
  i2 = 59,
  i3 = 60,
  i4 = 61,
  i5 = 62,
  i6 = 63,
  i7 = 64,
  i8 = 65,
  i9 = 66,
  a = 67,
  b = 68,
  c = 69,
  d = 70,
  e = 71,
  f = 72,
  g = 73,
  h = 74,
  i = 75,
  j = 76,
  k = 77,
  l = 78,
  m = 79,
  n = 80,
  o = 81,
  p = 82,
  q = 83,
  r = 84,
  s = 85,
  t = 86,
  u = 87,
  v = 88,
  w = 89,
  x = 90,
  y = 91,
  z = 92,
  f1 = 93,
  f2 = 94,
  f3 = 95,
  f4 = 96,
  f5 = 97,
  f6 = 98,
  f7 = 99,
  f8 = 100,
  f9 = 101,
  f10 = 102,
  f11 = 103,
  f12 = 104,
};
pub const PuleInputKeyModifier = enum(u32) {
  ctrl = 1,
  shift = 2,
  alt = 4,
  super = 8,
};
pub const PuleInputMouse = enum(u32) {
  left = 0,
  right = 1,
  middle = 2,
  side1 = 3,
  side2 = 4,
  End = 5,
};
pub extern fn puleInputKey(
  window: PulePlatform,
  key: PuleInputKey,
) callconv(.C) bool;
pub extern fn puleInputKeyModifiers(
  window: PulePlatform,
) callconv(.C) PuleInputKeyModifier;
pub extern fn puleInputMouse(
  window: PulePlatform,
  mouseKey: PuleInputMouse,
) callconv(.C) bool;
pub extern fn puleInputScroll(
  window: PulePlatform,
) callconv(.C) i32;
pub const PuleInputKeyCallbackCreateInfo = extern struct {
};
pub extern fn puleInputKeyCallback(
  info: PuleInputKeyCallbackCreateInfo,
) callconv(.C) void;
pub const PuleInputMouseButtonCallbackCreateInfo = extern struct {
};
pub extern fn puleInputMouseButtonCallback(
  info: PuleInputMouseButtonCallbackCreateInfo,
) callconv(.C) void;
pub const PuleInputRawTextCallbackCreateInfo = extern struct {
};
pub extern fn puleInputRawTextCallback(
  info: PuleInputRawTextCallbackCreateInfo,
) callconv(.C) void;
pub const PuleErrorDataSerializer = enum(u32) {
  none = 0,
  invalidFormat = 1,
};
pub const PuleDsValue = extern struct {
  id: u64,
};
pub const PuleDsValueArray = extern struct {
  values: [*c] const PuleDsValue,
  length: usize,
};
pub const PuleDsValueObject = extern struct {
  labels: [*c] const PuleStringView,
  values: [*c] const PuleDsValue,
  length: usize,
};
pub extern fn puleDsAsI64(
  value: PuleDsValue,
) callconv(.C) i64;
pub extern fn puleDsAsF64(
  value: PuleDsValue,
) callconv(.C) f64;
pub extern fn puleDsAsString(
  value: PuleDsValue,
) callconv(.C) PuleStringView;
pub extern fn puleDsAsArray(
  value: PuleDsValue,
) callconv(.C) PuleDsValueArray;
pub extern fn puleDsAsObject(
  value: PuleDsValue,
) callconv(.C) PuleDsValueObject;
pub extern fn puleDsIsI64(
  value: PuleDsValue,
) callconv(.C) bool;
pub extern fn puleDsIsF64(
  value: PuleDsValue,
) callconv(.C) bool;
pub extern fn puleDsIsString(
  value: PuleDsValue,
) callconv(.C) bool;
pub extern fn puleDsIsArray(
  value: PuleDsValue,
) callconv(.C) bool;
pub extern fn puleDsIsObject(
  value: PuleDsValue,
) callconv(.C) bool;
pub extern fn puleDsLoadFromStream(
  allocator: PuleAllocator,
  stream: PuleStreamRead,
  err: [*c] PuleError,
) callconv(.C) PuleDsValue;
pub extern fn puleDsDestroy(
  value: PuleDsValue,
) callconv(.C) void;
pub extern fn puleDsCreateI64(
  value: i64,
) callconv(.C) PuleDsValue;
pub extern fn puleDsCreateF64(
  value: f64,
) callconv(.C) PuleDsValue;
pub extern fn puleDsCreateString(
  stringView: PuleStringView,
) callconv(.C) PuleDsValue;
pub extern fn puleDsCreateArray(
  allocator: PuleAllocator,
) callconv(.C) PuleDsValue;
pub extern fn puleDsCreateObject(
  allocator: PuleAllocator,
) callconv(.C) PuleDsValue;
pub extern fn puleDsAppendArray(
  array: PuleDsValue,
  value: PuleDsValue,
) callconv(.C) void;
pub extern fn puleDsArrayLength(
  array: PuleDsValue,
) callconv(.C) usize;
pub extern fn puleDsArrayElementAt(
  array: PuleDsValue,
  idx: usize,
) callconv(.C) PuleDsValue;
pub extern fn puleDsObjectMember(
  object: PuleDsValue,
  memberLabel: PuleStringView,
) callconv(.C) PuleDsValue;
pub extern fn puleDsAssignObjectMember(
  object: PuleDsValue,
  memberLabel: PuleStringView,
  valueToEmplace: PuleDsValue,
) callconv(.C) void;
pub const PuleDsWriteInfo = extern struct {
  head: PuleDsValue,
  prettyPrint: bool,
  spacePerTab: u32,
  initialTabLevel: u32,
};
pub extern fn puleDsWriteToStdout(
  info: PuleDsWriteInfo,
) callconv(.C) void;
pub const PuleEcsWorld = extern struct {
  id: u64,
};
pub const PuleEcsComponent = extern struct {
  id: u64,
};
pub const PuleEcsSystem = extern struct {
  id: u64,
};
pub const PuleEcsEntity = extern struct {
  id: u64,
};
pub extern fn puleEcsWorldCreate() callconv(.C) PuleEcsWorld;
pub extern fn puleEcsWorldDestroy(
  world: PuleEcsWorld,
) callconv(.C) void;
pub extern fn puleEcsWorldAdvance(
  world: PuleEcsWorld,
  msDelta: f32,
) callconv(.C) void;
pub const PuleEcsComponentCreateInfo = extern struct {
  label: [*c] const u8,
  byteLength: usize,
  byteAlignment: usize,
};
pub extern fn puleEcsComponentCreate(
  world: PuleEcsWorld,
  info: PuleEcsComponentCreateInfo,
) callconv(.C) PuleEcsComponent;
pub const PuleEcsIterator = extern struct {
  data: ?* anyopaque,
};
pub extern fn puleEcsIteratorEntityCount(
  iterator: PuleEcsIterator,
) callconv(.C) usize;
pub extern fn puleEcsIteratorRelativeOffset(
  iterator: PuleEcsIterator,
) callconv(.C) usize;
pub extern fn puleEcsIteratorQueryComponents(
  iterator: PuleEcsIterator,
  componentIndex: usize,
  componentByteLength: usize,
) callconv(.C) ?* anyopaque;
pub extern fn puleEcsIteratorQueryEntities(
  iterator: PuleEcsIterator,
) callconv(.C) [*c] PuleEcsEntity;
pub extern fn puleEcsIteratorWorld(
  iterator: PuleEcsIterator,
) callconv(.C) PuleEcsWorld;
pub extern fn puleEcsIteratorUserData(
  iterator: PuleEcsIterator,
) callconv(.C) ?* anyopaque;
pub const PuleEcsSystemCallbackFrequency = enum(u32) {
  none = 0,
  preUpdate = 1,
  onUpdate = 2,
  postUpdate = 3,
};
pub const PuleEcsSystemCreateInfo = extern struct {
  label: [*c] const u8,
  commaSeparatedComponentLabels: [*c] const u8,
  callbackFrequency: PuleEcsSystemCallbackFrequency,
};
pub extern fn puleEcsSystemCreate(
  world: PuleEcsWorld,
  info: PuleEcsSystemCreateInfo,
) callconv(.C) PuleEcsSystem;
pub extern fn puleEcsSystemAdvance(
  world: PuleEcsWorld,
  system: PuleEcsSystem,
  deltaTime: f32,
  userdata: ?* anyopaque,
) callconv(.C) void;
pub extern fn puleEcsEntityCreate(
  world: PuleEcsWorld,
) callconv(.C) PuleEcsEntity;
pub extern fn puleEcsEntityDestroy(
  world: PuleEcsWorld,
  entity: PuleEcsEntity,
) callconv(.C) void;
pub extern fn puleEcsEntityAttachComponent(
  world: PuleEcsWorld,
  entity: PuleEcsEntity,
  component: PuleEcsComponent,
  nullableInitialData: ?* anyopaque,
) callconv(.C) void;
pub extern fn puleImguiInitialize(
  window: PulePlatform,
) callconv(.C) void;
pub extern fn puleImguiShutdown() callconv(.C) void;
pub extern fn puleImguiNewFrame() callconv(.C) void;
pub extern fn puleImguiRender() callconv(.C) void;
pub extern fn puleImguiSliderF32(
  label: [*c] const u8,
  data: [*c] f32,
  min: f32,
  max: f32,
) callconv(.C) bool;
pub extern fn puleImguiSliderZu(
  label: [*c] const u8,
  data: [*c] usize,
  min: usize,
  max: usize,
) callconv(.C) bool;
pub extern fn puleImguiToggle(
  label: [*c] const u8,
  data: [*c] bool,
) callconv(.C) bool;
pub extern fn puleImguiButton(
  label: [*c] const u8,
) callconv(.C) bool;
pub const PuleErrorAssetImage = enum(u32) {
  none = 0,
  decode = 1,
};
pub const PuleAssetImage = extern struct {
  id: u64,
};
pub const PuleAssetImageSupportFlag = enum(u32) {
  none = 0,
  read = 1,
  write = 2,
  readWrite = 3,
};
pub extern fn puleAssetImageExtensionSupported(
  extension: [*c] const u8,
) callconv(.C) PuleAssetImageSupportFlag;
pub const PuleAssetImageFormat = enum(u32) {
  rgbaU8 = 0,
  rgbaU16 = 1,
  rgbU8 = 2,
};
pub extern fn puleAssetImageLoadFromStream(
  allocator: PuleAllocator,
  imageSource: PuleStreamRead,
  requestedFormat: PuleAssetImageFormat,
  err: [*c] PuleError,
) callconv(.C) PuleAssetImage;
pub extern fn puleAssetImageDestroy(
  image: PuleAssetImage,
) callconv(.C) void;
pub extern fn puleAssetImageDecodedData(
  image: PuleAssetImage,
) callconv(.C) ?* anyopaque;
pub extern fn puleAssetImageDecodedDataLength(
  image: PuleAssetImage,
) callconv(.C) usize;
pub extern fn puleAssetImageWidth(
  image: PuleAssetImage,
) callconv(.C) u32;
pub extern fn puleAssetImageHeight(
  image: PuleAssetImage,
) callconv(.C) u32;
pub const PuleStreamRead = extern struct {
  userdata: ?* anyopaque,
};
pub extern fn puleStreamReadByte(
  stream: PuleStreamRead,
) callconv(.C) u8;
pub extern fn puleStreamPeekByte(
  stream: PuleStreamRead,
) callconv(.C) u8;
pub extern fn puleStreamReadIsDone(
  stream: PuleStreamRead,
) callconv(.C) bool;
pub extern fn puleStreamReadDestroy(
  stream: PuleStreamRead,
) callconv(.C) void;
pub const PuleStreamWrite = extern struct {
  userdata: ?* anyopaque,
};
pub extern fn puleStreamWriteBytes(
  stream: PuleStreamWrite,
  bytes: ?* anyopaque,
  length: usize,
) callconv(.C) void;
pub const PuleErrorArray = enum(u32) {
  none = 0,
  errorAllocation = 1,
};
pub const PuleArray = extern struct {
  content: ?* anyopaque,
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
) callconv(.C) ?* anyopaque;
pub extern fn puleArrayElementAt(
  array: PuleArray,
  idx: usize,
) callconv(.C) ?* anyopaque;
pub const PuleArrayView = extern struct {
  data: [*c] const u8,
  elementStride: usize,
  elementCount: usize,
};
pub const PuleArrayViewMutable = extern struct {
  data: [*c] u8,
  elementStride: usize,
  elementCount: usize,
};
pub const PuleAllocateInfo = extern struct {
  zeroOut: u8,
  numBytes: usize,
  alignment: usize,
};
pub const PuleReallocateInfo = extern struct {
  allocation: ?* anyopaque,
  numBytes: usize,
  alignment: usize,
};
pub const PuleAllocator = extern struct {
  implementation: ?* anyopaque,
};
pub extern fn puleAllocateDefault() callconv(.C) PuleAllocator;
pub extern fn puleAllocate(
  allocator: PuleAllocator,
  allocateInfo: PuleAllocateInfo,
) callconv(.C) ?* anyopaque;
pub extern fn puleReallocate(
  allocator: PuleAllocator,
  reallocateInfo: PuleReallocateInfo,
) callconv(.C) ?* anyopaque;
pub extern fn puleDeallocate(
  allocator: PuleAllocator,
  allocationNullable: ?* anyopaque,
) callconv(.C) void;
pub const PuleError = extern struct {
  description: PuleString,
  id: u32,
};
pub extern fn puleError() callconv(.C) PuleError;
pub extern fn puleErrorConsume(
  err: [*c] PuleError,
) callconv(.C) u32;
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
pub const PuleErrorFile = enum(u32) {
  none = 0,
  fileOpen = 1,
};
pub const PuleFileDataMode = enum(u32) {
  text = 0,
  binary = 1,
};
pub const PuleFileOpenMode = enum(u32) {
  read = 0,
  writeOverwrite = 1,
  writeAppend = 2,
  readWriteOverwrite = 3,
  readWrite = 4,
};
pub const PuleFile = extern struct {
  id: u64,
};
pub extern fn puleFileOpen(
  filename: [*c] const u8,
  dataMode: PuleFileDataMode,
  openMode: PuleFileOpenMode,
  err: [*c] PuleError,
) callconv(.C) PuleFile;
pub extern fn puleFileClose(
  file: PuleFile,
) callconv(.C) void;
pub extern fn puleFileIsDone(
  file: PuleFile,
) callconv(.C) bool;
pub extern fn puleFileReadByte(
  file: PuleFile,
) callconv(.C) u8;
pub extern fn puleFileReadBytes(
  file: PuleFile,
  destination: PuleArrayViewMutable,
) callconv(.C) usize;
pub extern fn puleFileReadBytesWithStride(
  file: PuleFile,
  destination: PuleArrayViewMutable,
) callconv(.C) usize;
pub extern fn puleFileWriteBytes(
  file: PuleFile,
  source: PuleArrayView,
) callconv(.C) void;
pub extern fn puleFileSize(
  file: PuleFile,
) callconv(.C) u64;
pub extern fn puleFileAdvanceFromStart(
  file: PuleFile,
  offset: i64,
) callconv(.C) void;
pub extern fn puleFileAdvanceFromEnd(
  file: PuleFile,
  offset: i64,
) callconv(.C) void;
pub extern fn puleFileAdvanceFromCurrent(
  file: PuleFile,
  offset: i64,
) callconv(.C) void;
pub extern fn puleFileStream(
  file: PuleFile,
  view: PuleArrayViewMutable,
) callconv(.C) PuleStreamRead;
pub const PulePluginType = enum(u32) {
  library = 0,
  component = 1,
  End = 2,
};
pub const PulePluginInfo = extern struct {
  name: [*c] const u8,
  id: u64,
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
) callconv(.C) ?* anyopaque;
pub extern fn puleTryPluginLoadFn(
  pluginId: usize,
  fnCStr: [*c] const u8,
) callconv(.C) ?* anyopaque;
pub extern fn puleIteratePlugins(
  userdata: ?* anyopaque,
) callconv(.C) void;
pub const PuleNanosecond = extern struct {
  value: i64,
};
pub const PuleMicrosecond = extern struct {
  value: i64,
};
pub extern fn puleSleepMicrosecond(
  us: PuleMicrosecond,
) callconv(.C) void;
pub const PuleGfxDescriptorType = enum(u32) {
  sampler = 0,
  uniformBuffer = 1,
  storageBuffer = 2,
};
pub const PuleGfxAttributeDataType = enum(u32) {
  float = 0,
  unsignedByte = 1,
};
pub const PuleGfxPipelineAttributeDescriptorBinding = extern struct {
  buffer: PuleGfxGpuBuffer,
  numComponents: usize,
  dataType: PuleGfxAttributeDataType,
  normalizeFixedDataTypeToNormalizedFloating: bool,
  stridePerElement: usize,
  offsetIntoBuffer: usize,
};
pub const PuleGfxPipelineDescriptorSetLayout = extern struct {
  bufferUniformBindings: [16] PuleGfxGpuBuffer,
  bufferStorageBindings: [16] PuleGfxGpuBuffer,
  bufferAttributeBindings: [16] PuleGfxPipelineAttributeDescriptorBinding,
  textureBindings: [8] PuleGfxGpuImage,
  bufferElementBinding: PuleGfxGpuBuffer,
};
pub extern fn puleGfxPipelineDescriptorSetLayout() callconv(.C) PuleGfxPipelineDescriptorSetLayout;
pub const PuleGfxPipelineConfig = extern struct {
  blendEnabled: bool,
  scissorTestEnabled: bool,
  viewportUl: PuleI32v2,
  viewportLr: PuleI32v2,
  scissorUl: PuleI32v2,
  scissorLr: PuleI32v2,
};
pub const PuleGfxPipelineCreateInfo = extern struct {
  shaderModule: PuleGfxShaderModule,
  framebuffer: PuleGfxFramebuffer,
  layout: [*c] const PuleGfxPipelineDescriptorSetLayout,
  config: PuleGfxPipelineConfig,
};
pub const PuleGfxPipeline = extern struct {
  id: u64,
};
pub extern fn puleGfxPipelineCreate(
  createInfo: [*c] const PuleGfxPipelineCreateInfo,
  err: [*c] PuleError,
) callconv(.C) PuleGfxPipeline;
pub extern fn puleGfxPipelineUpdate(
  pipeline: PuleGfxPipeline,
  updateInfo: [*c] const PuleGfxPipelineCreateInfo,
  err: [*c] PuleError,
) callconv(.C) void;
pub extern fn puleGfxPipelineDestroy(
  pipeline: PuleGfxPipeline,
) callconv(.C) void;
pub const PuleGfxPushConstant = extern struct {
  index: usize,
  data: ?* anyopaque,
};
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
pub const PuleGfxImageMagnification = enum(u32) {
  nearest = 0,
};
pub const PuleGfxImageWrap = enum(u32) {
  clampToEdge = 0,
};
pub const PuleGfxSampler = extern struct {
  id: u64,
};
pub const PuleGfxSamplerCreateInfo = extern struct {
  minify: PuleGfxImageMagnification,
  magnify: PuleGfxImageMagnification,
  wrapU: PuleGfxImageWrap,
  wrapV: PuleGfxImageWrap,
};
pub extern fn puleGfxSamplerCreate(
  createInfo: PuleGfxSamplerCreateInfo,
) callconv(.C) PuleGfxSampler;
pub extern fn puleGfxSamplerDestroy(
  sampler: PuleGfxSampler,
) callconv(.C) void;
pub const PuleGfxGpuImage = extern struct {
  id: u64,
};
pub const PuleGfxImageByteFormat = enum(u32) {
  rgba8U = 0,
  rgb8U = 1,
};
pub const PuleGfxImageTarget = enum(u32) {
  i2D = 0,
};
pub const PuleGfxImageCreateInfo = extern struct {
  width: u32,
  height: u32,
  target: PuleGfxImageTarget,
  byteFormat: PuleGfxImageByteFormat,
  sampler: PuleGfxSampler,
  optionalInitialData: ?* const anyopaque,
};
pub extern fn puleGfxGpuImageCreate(
  imageCreateInfo: PuleGfxImageCreateInfo,
) callconv(.C) PuleGfxGpuImage;
pub extern fn puleGfxGpuImageDestroy(
  image: PuleGfxGpuImage,
) callconv(.C) void;
pub const PuleGfxFramebuffer = extern struct {
  id: u64,
};
pub const PuleGfxFramebufferAttachment = enum(u32) {
  color0 = 0,
  color1 = 1,
  color3 = 2,
  color4 = 3,
  depth = 4,
  stencil = 5,
  depthStencil = 6,
  End = 7,
};
pub const PuleGfxFramebufferType = enum(u32) {
  renderStorage = 0,
  imageStorage = 1,
};
pub const PuleGfxRenderStorage = extern struct {
  id: u64,
};
pub const PuleGfxFramebufferImageAttachment = extern struct {
  image: PuleGfxGpuImage,
  mipmapLevel: u32,
};
pub const PuleGfxFramebufferAttachments = extern union {
  images: [@enumToInt(PuleGfxFramebufferAttachment.End)] PuleGfxFramebufferImageAttachment,
  renderStorages: [@enumToInt(PuleGfxFramebufferAttachment.End)] PuleGfxRenderStorage,
};
pub const PuleGfxFramebufferCreateInfo = extern struct {
  attachment: PuleGfxFramebufferAttachments,
  attachmentType: PuleGfxFramebufferType,
};
pub extern fn puleGfxFramebufferCreateInfo() callconv(.C) PuleGfxFramebufferCreateInfo;
pub extern fn puleGfxFramebufferCreate(
  framebufferCreateInfo: PuleGfxFramebufferCreateInfo,
  err: [*c] PuleError,
) callconv(.C) PuleGfxFramebuffer;
pub extern fn puleGfxFramebufferDestroy(
  framebuffer: PuleGfxFramebuffer,
) callconv(.C) void;
pub extern fn puleGfxFramebufferWindow() callconv(.C) PuleGfxFramebuffer;
pub const PuleErrorGfx = enum(u32) {
  none = 0,
  creationFailed = 1,
  shaderModuleCompilationFailed = 2,
  invalidDescriptorSet = 3,
  invalidCommandList = 4,
  invalidFramebuffer = 5,
  submissionFenceWaitFailed = 6,
};
pub const PuleGfxContext = extern struct {
  implementation: ?* anyopaque,
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
  optionalInitialData: ?* const anyopaque,
  byteLength: usize,
  usage: PuleGfxGpuBufferUsage,
  visibility: PuleGfxGpuBufferVisibilityFlag,
) callconv(.C) PuleGfxGpuBuffer;
pub extern fn puleGfxGpuBufferDestroy(
  buffer: PuleGfxGpuBuffer,
) callconv(.C) void;
pub const PuleGfxGpuBufferMapAccess = enum(u32) {
  hostVisible = 1,
  hostWritable = 2,
  invalidate = 4,
};
pub const PuleGfxGpuBufferMapRange = extern struct {
  buffer: PuleGfxGpuBuffer,
  access: PuleGfxGpuBufferMapAccess,
  byteOffset: usize,
  byteLength: usize,
};
pub const PuleGfxGpuBufferMappedFlushRange = extern struct {
  buffer: PuleGfxGpuBuffer,
  byteOffset: usize,
  byteLength: usize,
};
pub extern fn puleGfxGpuBufferMap(
  range: PuleGfxGpuBufferMapRange,
) callconv(.C) ?* anyopaque;
pub extern fn puleGfxGpuBufferMappedFlush(
  range: PuleGfxGpuBufferMappedFlushRange,
) callconv(.C) void;
pub extern fn puleGfxGpuBufferUnmap(
  buffer: PuleGfxGpuBuffer,
) callconv(.C) void;
pub extern fn puleGfxInitialize(
  err: [*c] PuleError,
) callconv(.C) void;
pub extern fn puleGfxShutdown() callconv(.C) void;
pub extern fn puleGfxFrameStart() callconv(.C) void;
pub extern fn puleGfxFrameEnd() callconv(.C) void;
pub const PuleGfxAction = enum(u32) {
  bindPipeline = 0,
  clearFramebufferColor = 1,
  clearFramebufferDepth = 2,
  dispatchRender = 3,
  dispatchRenderElements = 4,
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
pub const PuleGfxElementType = enum(u32) {
  u8 = 0,
  u16 = 1,
  u32 = 2,
};
pub const PuleGfxActionDispatchRenderElements = extern struct {
  action: PuleGfxAction,
  drawPrimitive: PuleGfxDrawPrimitive,
  numElements: usize,
  elementType: PuleGfxElementType,
  elementOffset: usize,
  baseVertexOffset: usize,
};
pub const PuleGfxActionClearFramebufferColor = extern struct {
  action: PuleGfxAction,
  framebuffer: PuleGfxFramebuffer,
  color: PuleF32v4,
};
pub const PuleGfxActionClearFramebufferDepth = extern struct {
  action: PuleGfxAction,
  framebuffer: PuleGfxFramebuffer,
  depth: f32,
};
pub const PuleGfxConstantValue = extern union {
  constantF32: f32,
  constantF32v2: PuleF32v2,
  constantF32v3: PuleF32v3,
  constantF32v4: PuleF32v4,
  constantI32: i32,
  constantI32v2: i32,
  constantI32v3: i32,
  constantI32v4: i32,
  constantF32m44: PuleF32m44,
};
pub const PuleGfxConstantTypeTag = enum(u32) {
  f32 = 0,
  f32v2 = 1,
  f32v3 = 2,
  f32v4 = 3,
  i32 = 4,
  i32v2 = 5,
  i32v3 = 6,
  i32v4 = 7,
  f32m44 = 8,
};
pub const PuleGfxConstant = extern struct {
  value: PuleGfxConstantValue,
  typeTag: PuleGfxConstantTypeTag,
  bindingSlot: u32,
};
pub const PuleGfxActionPushConstants = extern struct {
  action: PuleGfxAction,
  constants: [*c] const PuleGfxConstant,
  constantsLength: usize,
};
pub const PuleGfxCommand = extern union {
  action: PuleGfxAction,
  bindPipeline: PuleGfxActionBindPipeline,
  clearFramebufferColor: PuleGfxActionClearFramebufferColor,
  clearFramebufferDepth: PuleGfxActionClearFramebufferDepth,
  dispatchRender: PuleGfxActionDispatchRender,
  dispatchRenderElements: PuleGfxActionDispatchRenderElements,
  pushConstants: PuleGfxActionPushConstants,
};
pub const PuleGfxCommandList = extern struct {
  id: u64,
};
pub const PuleGfxCommandListRecorder = extern struct {
  id: u64,
};
pub extern fn puleGfxCommandListCreate(
  allocator: PuleAllocator,
) callconv(.C) PuleGfxCommandList;
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
pub const PuleGfxCommandListSubmitInfo = extern struct {
  commandList: PuleGfxCommandList,
  fenceTargetStart: [*c] PuleGfxFence,
  fenceTargetFinish: [*c] PuleGfxFence,
};
pub extern fn puleGfxCommandListSubmit(
  info: PuleGfxCommandListSubmitInfo,
  err: [*c] PuleError,
) callconv(.C) void;
pub const PuleGfxFence = extern struct {
  id: u64,
};
pub const PuleGfxFenceConditionFlag = enum(u32) {
  all = 0,
};
pub extern fn puleGfxFenceCreate(
  condition: PuleGfxFenceConditionFlag,
) callconv(.C) PuleGfxFence;
pub extern fn puleGfxFenceDestroy(
  fence: PuleGfxFence,
) callconv(.C) void;
pub extern fn puleGfxFenceCheckSignal(
  fence: PuleGfxFence,
  timeout: PuleNanosecond,
) callconv(.C) bool;
pub const PuleGfxMemoryBarrierFlag = enum(u32) {
  attribute = 1,
  element = 2,
  bufferUpdate = 4,
  all = 7,
  PuleGfxMemoryBarrierFlagEnd = 5,
};
pub extern fn puleGfxMemoryBarrier(
  barrier: PuleGfxMemoryBarrierFlag,
) callconv(.C) void;
