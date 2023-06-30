#include <pulchritude-ecs/ecs.h>

#include "../include/debug.hpp"

#include <pulchritude-log/log.h>
#include <pulchritude-math/math.h>
#include <pulchritude-platform/platform.h>
#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-tui/tui.h>
#include <pulchritude-ecs-serializer/ecs-serializer.h>
#include <pulchritude-ecs/ecs.h>
#include <pulchritude-imgui/imgui.h>
#include <pulchritude-asset/model.h>
#include <pulchritude-asset/tiled.h>
#include <pulchritude-asset/font.h>
#include <pulchritude-asset/image.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-stream/stream.h>
#include <pulchritude-array/array.h>
#include <pulchritude-allocator/allocator.h>
#include <pulchritude-imgui-engine/imgui-engine.h>
#include <pulchritude-gfx-debug/gfx-debug.h>
#include <pulchritude-error/error.h>
#include <pulchritude-string/string.h>
#include <pulchritude-renderer-3d/renderer-3d.h>
#include <pulchritude-file/file.h>
#include <pulchritude-script/script.h>
#include <pulchritude-plugin/plugin.h>
#include <pulchritude-plugin/plugin-payload.h>
#include <pulchritude-time/time.h>
#include <pulchritude-gfx/pipeline.h>
#include <pulchritude-gfx/shader-module.h>
#include <pulchritude-gfx/image.h>
#include <pulchritude-gfx/gfx.h>
#include <pulchritude-gfx/commands.h>
#include <pulchritude-gfx/barrier.h>

//#include <volk.h>

#include <GLFW/glfw3.h>

#include <string>
#include <thread>
#include <unordered_map>

static PuleEngineLayer pul;
static std::thread::id renderThread;

extern "C" {
void puleEngineLayerEntry(PuleEngineLayer * const parentLayer) {
  puleLog("Using layer entry: %s", parentLayer->layerName.contents);
  pul = *parentLayer;
}
// log
bool * puleLogDebugEnabled() {
  debugLayerFnEntry();
  return pul.logDebugEnabled();
}

// no variadic generation: puleLog
// no variadic generation: puleLogDebug
// no variadic generation: puleLogError
// no variadic generation: puleLogLn
// no variadic generation: puleLogRaw
// math
PuleF32v2 puleF32v2(float const identity) {
  debugLayerFnEntry();
  return pul.f32v2(identity);
}

PuleF32v2 puleF32v2Ptr(float const * const values) {
  debugLayerFnEntry();
  return pul.f32v2Ptr(values);
}

PuleF32v2 puleF32v2Add(PuleF32v2 const a, PuleF32v2 const b) {
  debugLayerFnEntry();
  return pul.f32v2Add(a, b);
}

PuleF32v2 puleF32v2Sub(PuleF32v2 const a, PuleF32v2 const b) {
  debugLayerFnEntry();
  return pul.f32v2Sub(a, b);
}

PuleF32v2 puleF32v2Abs(PuleF32v2 const a) {
  debugLayerFnEntry();
  return pul.f32v2Abs(a);
}

PuleF32v3 puleF32v3(float const identity) {
  debugLayerFnEntry();
  return pul.f32v3(identity);
}

PuleF32v3 puleF32v3Ptr(float const * const values) {
  debugLayerFnEntry();
  return pul.f32v3Ptr(values);
}

PuleF32v3 puleF32v3Add(PuleF32v3 const a, PuleF32v3 const b) {
  debugLayerFnEntry();
  return pul.f32v3Add(a, b);
}

PuleF32v3 puleF32v3Sub(PuleF32v3 const a, PuleF32v3 const b) {
  debugLayerFnEntry();
  return pul.f32v3Sub(a, b);
}

PuleF32v3 puleF32v3Neg(PuleF32v3 const a) {
  debugLayerFnEntry();
  return pul.f32v3Neg(a);
}

PuleF32v3 puleF32v3Mul(PuleF32v3 const a, PuleF32v3 const b) {
  debugLayerFnEntry();
  return pul.f32v3Mul(a, b);
}

PuleF32v3 puleF32v3Div(PuleF32v3 const a, PuleF32v3 const b) {
  debugLayerFnEntry();
  return pul.f32v3Div(a, b);
}

float puleF32v3Dot(PuleF32v3 const a, PuleF32v3 const b) {
  debugLayerFnEntry();
  return pul.f32v3Dot(a, b);
}

float puleF32v3Length(PuleF32v3 const a) {
  debugLayerFnEntry();
  return pul.f32v3Length(a);
}

PuleF32v3 puleF32v3Normalize(PuleF32v3 const b) {
  debugLayerFnEntry();
  return pul.f32v3Normalize(b);
}

PuleF32v3 puleF32v3Cross(PuleF32v3 const a, PuleF32v3 const b) {
  debugLayerFnEntry();
  return pul.f32v3Cross(a, b);
}

PuleF32v4 puleF32v4(float const identity) {
  debugLayerFnEntry();
  return pul.f32v4(identity);
}

PuleF32m44 puleF32m44(float const identity) {
  debugLayerFnEntry();
  return pul.f32m44(identity);
}

PuleF32m44 puleF32m44Ptr(float const * const data) {
  debugLayerFnEntry();
  return pul.f32m44Ptr(data);
}

PuleF32m44 puleF32m44PtrTranspose(float const * const data) {
  debugLayerFnEntry();
  return pul.f32m44PtrTranspose(data);
}

PuleF32m44 puleProjectionPerspective(
  float const fieldOfViewRadians,
  float const aspectRatio,
  float const near,
  float const far
) {
  debugLayerFnEntry();
  return (
    pul.projectionPerspective(
      fieldOfViewRadians,
      aspectRatio,
      near,
      far
    )
  );
}

PuleF32m44 puleViewLookAt(
  PuleF32v3 const origin,
  PuleF32v3 const center,
  PuleF32v3 const up
) {
  debugLayerFnEntry();
  return (
    pul.viewLookAt(
      origin,
      center,
      up
    )
  );
}

// platform
void pulePlatformInitialize(PuleError * const error) {
  debugLayerFnEntry();
  return pul.platformInitialize(error);
}

void pulePlatformShutdown() {
  debugLayerFnEntry();
  return pul.platformShutdown();
}

PulePlatform pulePlatformCreate(
  PulePlatformCreateInfo const info,
  PuleError * const error
) {
  debugLayerFnEntry();
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  return (
    pul.platformCreate(
      info,
      error
    )
  );
}

void pulePlatformDestroy(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformDestroy(window);
}

bool pulePlatformShouldExit(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformShouldExit(window);
}

void * pulePlatformGetProcessAddress() {
  debugLayerFnEntry();
  return pul.platformGetProcessAddress();
}

void pulePlatformPollEvents(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformPollEvents(window);
}

void pulePlatformSwapFramebuffer(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformSwapFramebuffer(window);
}

PuleI32v2 pulePlatformWindowSize(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformWindowSize(window);
}

PuleI32v2 pulePlatformFramebufferSize(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformFramebufferSize(window);
}

void pulePlatformFramebufferResizeCallback(
  PulePlatformFramebufferResizeCallbackCreateInfo const info
) {
  debugLayerFnEntry();
  return (
    pul.platformFramebufferResizeCallback(
      info
    )
  );
}

void pulePlatformWindowResizeCallback(
  PulePlatformWindowResizeCallbackCreateInfo const info
) {
  debugLayerFnEntry();
  return (
    pul.platformWindowResizeCallback(
      info
    )
  );
}

double pulePlatformGetTime() {
  debugLayerFnEntry();
  return pul.platformGetTime();
}

bool pulePlatformNull(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformNull(window);
}

bool pulePlatformFocused(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformFocused(window);
}

void pulePlatformMouseOriginSet(
  PulePlatform const window,
  PuleI32v2 const origin
) {
  debugLayerFnEntry();
  return (
    pul.platformMouseOriginSet(
      window,
      origin
    )
  );
}

PuleI32v2 pulePlatformMouseOrigin(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformMouseOrigin(window);
}

bool pulePlatformCursorEnabled(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformCursorEnabled(window);
}

void pulePlatformCursorHide(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformCursorHide(window);
}

void pulePlatformCursorShow(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.platformCursorShow(window);
}

bool puleInputKey(PulePlatform const window, PuleInputKey const key) {
  debugLayerFnEntry();
  return pul.inputKey(window, key);
}

PuleInputKeyModifier puleInputKeyModifiers(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.inputKeyModifiers(window);
}

bool puleInputMouse(PulePlatform const window, PuleInputMouse const mouseKey) {
  debugLayerFnEntry();
  return pul.inputMouse(window, mouseKey);
}

int32_t puleInputScroll(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.inputScroll(window);
}

void puleInputKeyCallback(PuleInputKeyCallbackCreateInfo const info) {
  debugLayerFnEntry();
  return pul.inputKeyCallback(info);
}

void puleInputMouseButtonCallback(
  PuleInputMouseButtonCallbackCreateInfo const info
) {
  debugLayerFnEntry();
  return (
    pul.inputMouseButtonCallback(
      info
    )
  );
}

void puleInputRawTextCallback(PuleInputRawTextCallbackCreateInfo const info) {
  debugLayerFnEntry();
  return pul.inputRawTextCallback(info);
}

// data-serializer
int64_t puleDsAsI64(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsI64(value);
}

double puleDsAsF64(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsF64(value);
}

bool puleDsAsBool(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsBool(value);
}

PuleStringView puleDsAsString(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsString(value);
}

PuleDsValueArray puleDsAsArray(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsArray(value);
}

PuleDsValueObject puleDsAsObject(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsObject(value);
}

PuleDsValueBuffer puleDsAsBuffer(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsBuffer(value);
}

float puleDsAsF32(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsF32(value);
}

int32_t puleDsAsI32(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsI32(value);
}

size_t puleDsAsUSize(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsUSize(value);
}

uint64_t puleDsAsU64(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsU64(value);
}

uint32_t puleDsAsU32(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsU32(value);
}

uint16_t puleDsAsU16(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsU16(value);
}

int16_t puleDsAsI16(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsI16(value);
}

uint8_t puleDsAsU8(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsU8(value);
}

int8_t puleDsAsI8(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsAsI8(value);
}

bool puleDsIsI64(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsIsI64(value);
}

bool puleDsIsF64(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsIsF64(value);
}

bool puleDsIsString(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsIsString(value);
}

bool puleDsIsArray(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsIsArray(value);
}

bool puleDsIsObject(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsIsObject(value);
}

bool puleDsIsBuffer(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsIsBuffer(value);
}

void puleDsDestroy(PuleDsValue const value) {
  debugLayerFnEntry();
  return pul.dsDestroy(value);
}

PuleDsValue puleDsCreateI64(int64_t const value) {
  debugLayerFnEntry();
  return pul.dsCreateI64(value);
}

PuleDsValue puleDsCreateBool(bool const value) {
  debugLayerFnEntry();
  return pul.dsCreateBool(value);
}

PuleDsValue puleDsCreateU64(uint64_t const value) {
  debugLayerFnEntry();
  return pul.dsCreateU64(value);
}

PuleDsValue puleDsCreateF64(double const value) {
  debugLayerFnEntry();
  return pul.dsCreateF64(value);
}

PuleDsValue puleDsCreateString(PuleStringView const stringView) {
  debugLayerFnEntry();
  return pul.dsCreateString(stringView);
}

PuleDsValue puleDsCreateArray(PuleAllocator const allocator) {
  debugLayerFnEntry();
  return pul.dsCreateArray(allocator);
}

PuleDsValue puleDsCreateObject(PuleAllocator const allocator) {
  debugLayerFnEntry();
  return pul.dsCreateObject(allocator);
}

PuleDsValue puleDsCreateBuffer(
  PuleAllocator const allocator,
  PuleArrayView const data
) {
  debugLayerFnEntry();
  return (
    pul.dsCreateBuffer(
      allocator,
      data
    )
  );
}

PuleDsValue puleDsArrayAppend(
  PuleDsValue const array,
  PuleDsValue const value
) {
  debugLayerFnEntry();
  return (
    pul.dsArrayAppend(
      array,
      value
    )
  );
}

void puleDsArrayPopBack(PuleDsValue const array) {
  debugLayerFnEntry();
  return pul.dsArrayPopBack(array);
}

void puleDsArrayPopFront(PuleDsValue const array) {
  debugLayerFnEntry();
  return pul.dsArrayPopFront(array);
}

void puleDsArrayRemoveAt(PuleDsValue const array, size_t const idx) {
  debugLayerFnEntry();
  return pul.dsArrayRemoveAt(array, idx);
}

size_t puleDsArrayLength(PuleDsValue const array) {
  debugLayerFnEntry();
  return pul.dsArrayLength(array);
}

PuleDsValue puleDsArrayElementAt(PuleDsValue const array, size_t const idx) {
  debugLayerFnEntry();
  return pul.dsArrayElementAt(array, idx);
}

PuleDsValue puleDsObjectMember(
  PuleDsValue const object,
  char const * const memberLabel
) {
  debugLayerFnEntry();
  return (
    pul.dsObjectMember(
      object,
      memberLabel
    )
  );
}

PuleDsValue puleDsValueCloneRecursively(
  PuleDsValue const object,
  PuleAllocator const allocator
) {
  debugLayerFnEntry();
  return (
    pul.dsValueCloneRecursively(
      object,
      allocator
    )
  );
}

PuleDsValue puleDsObjectMemberAssign(
  PuleDsValue const objectValue,
  PuleStringView const memberLabel,
  PuleDsValue const valueToEmplace
) {
  debugLayerFnEntry();
  return (
    pul.dsObjectMemberAssign(
      objectValue,
      memberLabel,
      valueToEmplace
    )
  );
}

PuleDsValue puleDsObjectMemberOverwrite(
  PuleDsValue const objectValue,
  PuleStringView const memberLabel,
  PuleDsValue const valueToEmplace
) {
  debugLayerFnEntry();
  return (
    pul.dsObjectMemberOverwrite(
      objectValue,
      memberLabel,
      valueToEmplace
    )
  );
}

int64_t puleDsMemberAsI64(PuleDsValue const obj, char const * const label) {
  debugLayerFnEntry();
  return pul.dsMemberAsI64(obj, label);
}

double puleDsMemberAsF64(
  PuleDsValue const object,
  char const * const memberLabel
) {
  debugLayerFnEntry();
  return (
    pul.dsMemberAsF64(
      object,
      memberLabel
    )
  );
}

bool puleDsMemberAsBool(
  PuleDsValue const object,
  char const * const memberLabel
) {
  debugLayerFnEntry();
  return (
    pul.dsMemberAsBool(
      object,
      memberLabel
    )
  );
}

PuleStringView puleDsMemberAsString(
  PuleDsValue const object,
  char const * const memberLabel
) {
  debugLayerFnEntry();
  return (
    pul.dsMemberAsString(
      object,
      memberLabel
    )
  );
}

PuleDsValueArray puleDsMemberAsArray(
  PuleDsValue const object,
  char const * const memberLabel
) {
  debugLayerFnEntry();
  return (
    pul.dsMemberAsArray(
      object,
      memberLabel
    )
  );
}

PuleDsValueObject puleDsMemberAsObject(
  PuleDsValue const object,
  char const * const memberLabel
) {
  debugLayerFnEntry();
  return (
    pul.dsMemberAsObject(
      object,
      memberLabel
    )
  );
}

PuleDsValueBuffer puleDsMemberAsBuffer(
  PuleDsValue const object,
  char const * const memberLabel
) {
  debugLayerFnEntry();
  return (
    pul.dsMemberAsBuffer(
      object,
      memberLabel
    )
  );
}

// tui
PuleTuiWindow puleTuiInitialize() {
  debugLayerFnEntry();
  return pul.tuiInitialize();
}

void puleTuiDestroy(PuleTuiWindow const window) {
  debugLayerFnEntry();
  return pul.tuiDestroy(window);
}

void puleTuiClear(PuleTuiWindow const window) {
  debugLayerFnEntry();
  return pul.tuiClear(window);
}

void puleTuiRefresh(PuleTuiWindow const window) {
  debugLayerFnEntry();
  return pul.tuiRefresh(window);
}

void puleTuiRenderString(
  PuleTuiWindow const window,
  PuleStringView const content,
  PuleF32v3 const rgb
) {
  debugLayerFnEntry();
  return (
    pul.tuiRenderString(
      window,
      content,
      rgb
    )
  );
}

int32_t puleTuiReadInputCharacterBlocking(PuleTuiWindow const window) {
  debugLayerFnEntry();
  return pul.tuiReadInputCharacterBlocking(window);
}

void puleTuiMoveCursor(PuleTuiWindow const window, PuleI32v2 const position) {
  debugLayerFnEntry();
  return pul.tuiMoveCursor(window, position);
}

PuleI32v2 puleTuiWindowDim(PuleTuiWindow const window) {
  debugLayerFnEntry();
  return pul.tuiWindowDim(window);
}

// ecs-serializer
PuleDsValue puleEcsSerializeWorld(PuleEcsSerializeWorldInfo const info) {
  debugLayerFnEntry();
  return pul.ecsSerializeWorld(info);
}

void puleEcsDeserializeWorld(PuleEcsDeserializeWorldInfo const info) {
  debugLayerFnEntry();
  return pul.ecsDeserializeWorld(info);
}

// ecs
PuleEcsWorld puleEcsWorldCreate() {
  debugLayerFnEntry();
  return pul.ecsWorldCreate();
}

void puleEcsWorldDestroy(PuleEcsWorld const world) {
  debugLayerFnEntry();
  return pul.ecsWorldDestroy(world);
}

void puleEcsWorldAdvance(PuleEcsWorld const world, float const msDelta) {
  debugLayerFnEntry();
  return pul.ecsWorldAdvance(world, msDelta);
}

PuleEcsComponent puleEcsComponentCreate(
  PuleEcsWorld const world,
  PuleEcsComponentCreateInfo const info
) {
  debugLayerFnEntry();
  return (
    pul.ecsComponentCreate(
      world,
      info
    )
  );
}

void puleEcsEntityIterateComponents(
  PuleEcsEntityIterateComponentsInfo const info
) {
  debugLayerFnEntry();
  return (
    pul.ecsEntityIterateComponents(
      info
    )
  );
}

PuleEcsComponent puleEcsComponentFetchByLabel(
  PuleEcsWorld const world,
  PuleStringView const label
) {
  debugLayerFnEntry();
  return (
    pul.ecsComponentFetchByLabel(
      world,
      label
    )
  );
}

PuleStringView puleEcsComponentLabel(
  PuleEcsWorld const world,
  PuleEcsComponent const component
) {
  debugLayerFnEntry();
  return (
    pul.ecsComponentLabel(
      world,
      component
    )
  );
}

PuleEcsComponentSerializer puleEcsComponentSerializer(
  PuleEcsWorld const world,
  PuleEcsComponent const component
) {
  debugLayerFnEntry();
  return (
    pul.ecsComponentSerializer(
      world,
      component
    )
  );
}

PuleEcsComponentInfo puleEcsComponentInfo(
  PuleEcsWorld const world,
  PuleEcsComponent const component
) {
  debugLayerFnEntry();
  return (
    pul.ecsComponentInfo(
      world,
      component
    )
  );
}

void puleEcsComponentIterateAll(
  PuleEcsComponentIterateAllCallback const callback
) {
  debugLayerFnEntry();
  return (
    pul.ecsComponentIterateAll(
      callback
    )
  );
}

size_t puleEcsIteratorEntityCount(PuleEcsIterator const iterator) {
  debugLayerFnEntry();
  return pul.ecsIteratorEntityCount(iterator);
}

size_t puleEcsIteratorRelativeOffset(PuleEcsIterator const iterator) {
  debugLayerFnEntry();
  return pul.ecsIteratorRelativeOffset(iterator);
}

void * puleEcsIteratorQueryComponents(
  PuleEcsIterator const iterator,
  size_t const componentIndex,
  size_t const componentByteLength
) {
  debugLayerFnEntry();
  return (
    pul.ecsIteratorQueryComponents(
      iterator,
      componentIndex,
      componentByteLength
    )
  );
}

PuleEcsEntity * puleEcsIteratorQueryEntities(PuleEcsIterator const iterator) {
  debugLayerFnEntry();
  return pul.ecsIteratorQueryEntities(iterator);
}

PuleEcsWorld puleEcsIteratorWorld(PuleEcsIterator const iterator) {
  debugLayerFnEntry();
  return pul.ecsIteratorWorld(iterator);
}

PuleEcsSystem puleEcsIteratorSystem(PuleEcsIterator const iterator) {
  debugLayerFnEntry();
  return pul.ecsIteratorSystem(iterator);
}

void * puleEcsIteratorUserData(PuleEcsIterator const iterator) {
  debugLayerFnEntry();
  return pul.ecsIteratorUserData(iterator);
}

PuleEcsSystem puleEcsSystemCreate(
  PuleEcsWorld const world,
  PuleEcsSystemCreateInfo const info
) {
  debugLayerFnEntry();
  return (
    pul.ecsSystemCreate(
      world,
      info
    )
  );
}

void puleEcsSystemAdvance(
  PuleEcsWorld const world,
  PuleEcsSystem const system,
  float const deltaTime,
  void * const userdata
) {
  debugLayerFnEntry();
  return (
    pul.ecsSystemAdvance(
      world,
      system,
      deltaTime,
      userdata
    )
  );
}

PuleEcsEntity puleEcsEntityCreate(
  PuleEcsWorld const world,
  PuleStringView const label
) {
  debugLayerFnEntry();
  return (
    pul.ecsEntityCreate(
      world,
      label
    )
  );
}

PuleStringView puleEcsEntityName(
  PuleEcsWorld const world,
  PuleEcsEntity const entity
) {
  debugLayerFnEntry();
  return (
    pul.ecsEntityName(
      world,
      entity
    )
  );
}

void puleEcsEntityDestroy(
  PuleEcsWorld const world,
  PuleEcsEntity const entity
) {
  debugLayerFnEntry();
  return (
    pul.ecsEntityDestroy(
      world,
      entity
    )
  );
}

void puleEcsEntityAttachComponent(
  PuleEcsWorld const world,
  PuleEcsEntity const entity,
  PuleEcsComponent const component,
  void const * const nullableInitialData
) {
  debugLayerFnEntry();
  return (
    pul.ecsEntityAttachComponent(
      world,
      entity,
      component,
      nullableInitialData
    )
  );
}

void const * puleEcsEntityComponentData(
  PuleEcsWorld const world,
  PuleEcsEntity const entity,
  PuleEcsComponent const component
) {
  debugLayerFnEntry();
  return (
    pul.ecsEntityComponentData(
      world,
      entity,
      component
    )
  );
}

PuleEcsQuery puleEcsQueryByComponent(
  PuleEcsWorld const world,
  PuleEcsComponent * const componentList,
  size_t const componentListSize,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.ecsQueryByComponent(
      world,
      componentList,
      componentListSize,
      error
    )
  );
}

PuleEcsQuery puleEcsQueryAllEntities(
  PuleEcsWorld const world,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.ecsQueryAllEntities(
      world,
      error
    )
  );
}

void puleEcsQueryDestroy(PuleEcsQuery const query) {
  debugLayerFnEntry();
  return pul.ecsQueryDestroy(query);
}

PuleEcsQueryIterator puleEcsQueryIterator(
  PuleEcsWorld const world,
  PuleEcsQuery const query
) {
  debugLayerFnEntry();
  return (
    pul.ecsQueryIterator(
      world,
      query
    )
  );
}

PuleEcsIterator puleEcsQueryIteratorNext(PuleEcsQueryIterator const iter) {
  debugLayerFnEntry();
  return pul.ecsQueryIteratorNext(iter);
}

void puleEcsQueryIteratorDestroy(PuleEcsQueryIterator const iter) {
  debugLayerFnEntry();
  return pul.ecsQueryIteratorDestroy(iter);
}

// imgui
void puleImguiInitialize(PulePlatform const window) {
  debugLayerFnEntry();
  return pul.imguiInitialize(window);
}

void puleImguiShutdown() {
  debugLayerFnEntry();
  return pul.imguiShutdown();
}

void puleImguiNewFrame() {
  debugLayerFnEntry();
  return pul.imguiNewFrame();
}

void puleImguiJoinNext() {
  debugLayerFnEntry();
  return pul.imguiJoinNext();
}

bool puleImguiSliderF32(
  char const * const label,
  float * const data,
  float const min,
  float const max
) {
  debugLayerFnEntry();
  return (
    pul.imguiSliderF32(
      label,
      data,
      min,
      max
    )
  );
}

bool puleImguiSliderZu(
  char const * const label,
  size_t * const data,
  size_t const min,
  size_t const max
) {
  debugLayerFnEntry();
  return (
    pul.imguiSliderZu(
      label,
      data,
      min,
      max
    )
  );
}

void puleImguiWindowBegin(
  char const * const labelFormat,
  bool * const openNullable
) {
  debugLayerFnEntry();
  return (
    pul.imguiWindowBegin(
      labelFormat,
      openNullable
    )
  );
}

void puleImguiWindowEnd() {
  debugLayerFnEntry();
  return pul.imguiWindowEnd();
}

bool puleImguiSectionBegin(char const * const label) {
  debugLayerFnEntry();
  return pul.imguiSectionBegin(label);
}

void puleImguiSectionEnd() {
  debugLayerFnEntry();
  return pul.imguiSectionEnd();
}

// no variadic generation: puleImguiText
bool puleImguiToggle(char const * const label, bool * const data) {
  debugLayerFnEntry();
  return pul.imguiToggle(label, data);
}

bool puleImguiButton(char const * const label) {
  debugLayerFnEntry();
  return pul.imguiButton(label);
}

PuleImguiCallbackRegistry puleImguiCallbackRegister(
  PuleImguiCallback const callback
) {
  debugLayerFnEntry();
  return (
    pul.imguiCallbackRegister(
      callback
    )
  );
}

void puleImguiCallbackUnregister(PuleImguiCallbackRegistry const registry) {
  debugLayerFnEntry();
  return pul.imguiCallbackUnregister(registry);
}

void puleImguiCallbackShowAll() {
  debugLayerFnEntry();
  return pul.imguiCallbackShowAll();
}

// asset-tiled
PuleAssetTiledMapInfo puleAssetTiledMapInfo(PuleAssetTiledMap const tiledMap) {
  debugLayerFnEntry();
  return pul.assetTiledMapInfo(tiledMap);
}

PuleAssetTiledMap puleAssetTiledMapLoadFromStream(
  PuleAssetTiledMapLoadCreateInfo const info,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.assetTiledMapLoadFromStream(
      info,
      error
    )
  );
}

PuleAssetTiledMap puleAssetTiledMapLoadFromFile(
  PuleAssetTiledMapLoadFromFileCreateInfo const info,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.assetTiledMapLoadFromFile(
      info,
      error
    )
  );
}

void puleAssetTiledMapDestroy(PuleAssetTiledMap const map) {
  debugLayerFnEntry();
  return pul.assetTiledMapDestroy(map);
}

// asset-font
PuleAssetFont puleAssetFontLoad(
  PuleBufferView const fontSource,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.assetFontLoad(
      fontSource,
      error
    )
  );
}

void puleAssetFontDestroy(PuleAssetFont const font) {
  debugLayerFnEntry();
  return pul.assetFontDestroy(font);
}

void puleAssetFontRenderToU8Buffer(
  PuleAssetFontRenderInfo const info,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.assetFontRenderToU8Buffer(
      info,
      error
    )
  );
}

// asset-image
PuleAssetImageSupportFlag puleAssetImageExtensionSupported(
  char const * const extension
) {
  debugLayerFnEntry();
  return (
    pul.assetImageExtensionSupported(
      extension
    )
  );
}

PuleAssetImage puleAssetImageLoadFromStream(
  PuleAllocator const allocator,
  PuleStreamRead const imageSource,
  PuleAssetImageFormat const requestedFormat,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.assetImageLoadFromStream(
      allocator,
      imageSource,
      requestedFormat,
      error
    )
  );
}

void puleAssetImageDestroy(PuleAssetImage const image) {
  debugLayerFnEntry();
  return pul.assetImageDestroy(image);
}

void * puleAssetImageDecodedData(PuleAssetImage const image) {
  debugLayerFnEntry();
  return pul.assetImageDecodedData(image);
}

size_t puleAssetImageDecodedDataLength(PuleAssetImage const image) {
  debugLayerFnEntry();
  return pul.assetImageDecodedDataLength(image);
}

uint32_t puleAssetImageWidth(PuleAssetImage const image) {
  debugLayerFnEntry();
  return pul.assetImageWidth(image);
}

uint32_t puleAssetImageHeight(PuleAssetImage const image) {
  debugLayerFnEntry();
  return pul.assetImageHeight(image);
}

// asset-pds
PuleDsValue puleAssetPdsLoadFromStream(
  PuleAllocator const allocator,
  PuleStreamRead const stream,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.assetPdsLoadFromStream(
      allocator,
      stream,
      error
    )
  );
}

PuleDsValue puleAssetPdsLoadFromRvalStream(
  PuleAllocator const allocator,
  PuleStreamRead const rvalStream,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.assetPdsLoadFromRvalStream(
      allocator,
      rvalStream,
      error
    )
  );
}

PuleDsValue puleAssetPdsLoadFromFile(
  PuleAllocator const allocator,
  PuleStringView const filename,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.assetPdsLoadFromFile(
      allocator,
      filename,
      error
    )
  );
}

void puleAssetPdsWriteToStream(
  PuleStreamWrite const stream,
  PuleAssetPdsWriteInfo const writeInfo
) {
  debugLayerFnEntry();
  return (
    pul.assetPdsWriteToStream(
      stream,
      writeInfo
    )
  );
}

void puleAssetPdsWriteToFile(
  PuleDsValue const head,
  PuleStringView const filename,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.assetPdsWriteToFile(
      head,
      filename,
      error
    )
  );
}

void puleAssetPdsWriteToStdout(PuleDsValue const head) {
  debugLayerFnEntry();
  return pul.assetPdsWriteToStdout(head);
}

PuleDsValue puleAssetPdsLoadFromCommandLineArguments(
  PuleAssetPdsCommandLineArgumentsInfo const info,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.assetPdsLoadFromCommandLineArguments(
      info,
      error
    )
  );
}

// stream
uint8_t puleStreamReadByte(PuleStreamRead const stream) {
  debugLayerFnEntry();
  return pul.streamReadByte(stream);
}

uint8_t puleStreamPeekByte(PuleStreamRead const stream) {
  debugLayerFnEntry();
  return pul.streamPeekByte(stream);
}

bool puleStreamReadIsDone(PuleStreamRead const stream) {
  debugLayerFnEntry();
  return pul.streamReadIsDone(stream);
}

void puleStreamReadDestroy(PuleStreamRead const stream) {
  debugLayerFnEntry();
  return pul.streamReadDestroy(stream);
}

PuleStreamRead puleStreamReadFromString(PuleStringView const stringView) {
  debugLayerFnEntry();
  return pul.streamReadFromString(stringView);
}

void puleStreamWriteBytes(
  PuleStreamWrite const stream,
  uint8_t const * const bytes,
  size_t const length
) {
  debugLayerFnEntry();
  return (
    pul.streamWriteBytes(
      stream,
      bytes,
      length
    )
  );
}

void puleStreamWriteFlush(PuleStreamWrite const stream) {
  debugLayerFnEntry();
  return pul.streamWriteFlush(stream);
}

void puleStreamWriteDestroy(PuleStreamWrite const stream) {
  debugLayerFnEntry();
  return pul.streamWriteDestroy(stream);
}

PuleStreamWrite puleStreamStdoutWrite() {
  debugLayerFnEntry();
  return pul.streamStdoutWrite();
}

// array
PuleArray puleArray(PuleArrayCreateInfo const info) {
  debugLayerFnEntry();
  return pul.array(info);
}

void puleArrayDestroy(PuleArray const array) {
  debugLayerFnEntry();
  return pul.arrayDestroy(array);
}

void * puleArrayAppend(PuleArray * const array) {
  debugLayerFnEntry();
  return pul.arrayAppend(array);
}

void * puleArrayElementAt(PuleArray const array, size_t const idx) {
  debugLayerFnEntry();
  return pul.arrayElementAt(array, idx);
}

// allocator
PuleAllocator puleAllocateDefault() {
  debugLayerFnEntry();
  return pul.allocateDefault();
}

void * puleAllocate(
  PuleAllocator const allocator,
  PuleAllocateInfo const allocateInfo
) {
  debugLayerFnEntry();
  return (
    pul.allocate(
      allocator,
      allocateInfo
    )
  );
}

void * puleReallocate(
  PuleAllocator const allocator,
  PuleReallocateInfo const reallocateInfo
) {
  debugLayerFnEntry();
  return (
    pul.reallocate(
      allocator,
      reallocateInfo
    )
  );
}

void puleDeallocate(
  PuleAllocator const allocator,
  void * const allocationNullable
) {
  debugLayerFnEntry();
  return (
    pul.deallocate(
      allocator,
      allocationNullable
    )
  );
}

// imgui-engine
void puleImguiEngineDisplay(PuleImguiEngineDisplayInfo const info) {
  debugLayerFnEntry();
  return pul.imguiEngineDisplay(info);
}

// gfx-debug
void puleGfxDebugInitialize(PulePlatform const platform) {
  debugLayerFnEntry();
  return pul.gfxDebugInitialize(platform);
}

void puleGfxDebugShutdown() {
  debugLayerFnEntry();
  return pul.gfxDebugShutdown();
}

void puleGfxDebugRenderLine(
  PuleF32v3 const originStart,
  PuleF32v3 const originEnd,
  PuleF32v3 const color
) {
  debugLayerFnEntry();
  return (
    pul.gfxDebugRenderLine(
      originStart,
      originEnd,
      color
    )
  );
}

void puleGfxDebugRenderRectOutline(
  PuleF32v3 const originCenter,
  PuleF32v2 const dimensions,
  PuleF32v3 const color
) {
  debugLayerFnEntry();
  return (
    pul.gfxDebugRenderRectOutline(
      originCenter,
      dimensions,
      color
    )
  );
}

void puleGfxDebugRenderCircle(
  PuleF32v3 const origin,
  float const radius,
  PuleF32v3 const colorSurface,
  PuleF32v3 const colorInner
) {
  debugLayerFnEntry();
  return (
    pul.gfxDebugRenderCircle(
      origin,
      radius,
      colorSurface,
      colorInner
    )
  );
}

void puleGfxDebugRenderPoint(
  PuleF32v3 const origin,
  float const pointWidth,
  PuleF32v3 const color
) {
  debugLayerFnEntry();
  return (
    pul.gfxDebugRenderPoint(
      origin,
      pointWidth,
      color
    )
  );
}

void puleGfxDebugFrameStart() {
  debugLayerFnEntry();
  return pul.gfxDebugFrameStart();
}

void puleGfxDebugRender(
  PuleGfxFramebuffer const framebuffer,
  PuleF32m44 const transform
) {
  debugLayerFnEntry();
  return (
    pul.gfxDebugRender(
      framebuffer,
      transform
    )
  );
}

// error
PuleError puleError() {
  debugLayerFnEntry();
  return pul.error();
}

uint32_t puleErrorConsume(PuleError * const error) {
  debugLayerFnEntry();
  return pul.errorConsume(error);
}

bool puleErrorExists(PuleError * const error) {
  debugLayerFnEntry();
  return pul.errorExists(error);
}

// string
PuleString puleStringDefault(char const * const baseContents) {
  debugLayerFnEntry();
  return pul.stringDefault(baseContents);
}

PuleString puleString(
  PuleAllocator const allocator,
  char const * const baseContents
) {
  debugLayerFnEntry();
  return (
    pul.string(
      allocator,
      baseContents
    )
  );
}

void puleStringDestroy(PuleString const puleStringInout) {
  debugLayerFnEntry();
  return pul.stringDestroy(puleStringInout);
}

void puleStringAppend(
  PuleString * const stringInout,
  PuleStringView const append
) {
  debugLayerFnEntry();
  return (
    pul.stringAppend(
      stringInout,
      append
    )
  );
}

// no variadic generation: puleStringFormat
// no variadic generation: puleStringFormatDefault
PuleStringView puleStringView(PuleString const string) {
  debugLayerFnEntry();
  return pul.stringView(string);
}

PuleStringView puleCStr(char const * const cstr) {
  debugLayerFnEntry();
  return pul.cStr(cstr);
}

bool puleStringViewEq(PuleStringView const v0, PuleStringView const v1) {
  debugLayerFnEntry();
  return pul.stringViewEq(v0, v1);
}

bool puleStringViewEqCStr(PuleStringView const v0, char const * const v1) {
  debugLayerFnEntry();
  return pul.stringViewEqCStr(v0, v1);
}

size_t puleStringViewHash(PuleStringView const view) {
  debugLayerFnEntry();
  return pul.stringViewHash(view);
}

// renderer-3d
PuleRenderer3D puleRenderer3DCreate(
  PuleEcsWorld const world,
  PulePlatform const platform
) {
  debugLayerFnEntry();
  return (
    pul.renderer3DCreate(
      world,
      platform
    )
  );
}

PuleEcsSystem puleRenderer3DEcsSystem(PuleRenderer3D const renderer3D) {
  debugLayerFnEntry();
  return pul.renderer3DEcsSystem(renderer3D);
}

PuleEcsComponent puleRenderer3DAttachComponentRender(
  PuleEcsWorld const world,
  PuleRenderer3D const renderer3DSystem,
  PuleEcsEntity const entity,
  PuleRenderer3DModel const model
) {
  debugLayerFnEntry();
  return (
    pul.renderer3DAttachComponentRender(
      world,
      renderer3DSystem,
      entity,
      model
    )
  );
}

// file
PuleFile puleFileOpen(
  PuleStringView const filename,
  PuleFileDataMode const dataMode,
  PuleFileOpenMode const openMode,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.fileOpen(
      filename,
      dataMode,
      openMode,
      error
    )
  );
}

void puleFileClose(PuleFile const file) {
  debugLayerFnEntry();
  return pul.fileClose(file);
}

bool puleFileIsDone(PuleFile const file) {
  debugLayerFnEntry();
  return pul.fileIsDone(file);
}

PuleStringView puleFilePath(PuleFile const file) {
  debugLayerFnEntry();
  return pul.filePath(file);
}

uint8_t puleFileReadByte(PuleFile const file) {
  debugLayerFnEntry();
  return pul.fileReadByte(file);
}

size_t puleFileReadBytes(
  PuleFile const file,
  PuleArrayViewMutable const destination
) {
  debugLayerFnEntry();
  return (
    pul.fileReadBytes(
      file,
      destination
    )
  );
}

void puleFileWriteBytes(PuleFile const file, PuleArrayView const source) {
  debugLayerFnEntry();
  return pul.fileWriteBytes(file, source);
}

void puleFileWriteString(PuleFile const file, PuleStringView const source) {
  debugLayerFnEntry();
  return pul.fileWriteString(file, source);
}

uint64_t puleFileSize(PuleFile const file) {
  debugLayerFnEntry();
  return pul.fileSize(file);
}

void puleFileAdvanceFromStart(PuleFile const file, int64_t const offset) {
  debugLayerFnEntry();
  return pul.fileAdvanceFromStart(file, offset);
}

void puleFileAdvanceFromEnd(PuleFile const file, int64_t const offset) {
  debugLayerFnEntry();
  return pul.fileAdvanceFromEnd(file, offset);
}

void puleFileAdvanceFromCurrent(PuleFile const file, int64_t const offset) {
  debugLayerFnEntry();
  return pul.fileAdvanceFromCurrent(file, offset);
}

PuleStreamRead puleFileStreamRead(
  PuleFile const file,
  PuleArrayViewMutable const intermediateBuffer
) {
  debugLayerFnEntry();
  return (
    pul.fileStreamRead(
      file,
      intermediateBuffer
    )
  );
}

PuleStreamWrite puleFileStreamWrite(
  PuleFile const file,
  PuleArrayViewMutable const intermediateBuffer
) {
  debugLayerFnEntry();
  return (
    pul.fileStreamWrite(
      file,
      intermediateBuffer
    )
  );
}

bool puleFilesystemPathExists(PuleStringView const path) {
  debugLayerFnEntry();
  return pul.filesystemPathExists(path);
}

bool puleFileCopy(
  PuleStringView const sourcePath,
  PuleStringView const destinationPath
) {
  debugLayerFnEntry();
  return (
    pul.fileCopy(
      sourcePath,
      destinationPath
    )
  );
}

bool puleFileRemove(PuleStringView const filePath) {
  debugLayerFnEntry();
  return pul.fileRemove(filePath);
}

bool puleFileRemoveRecursive(PuleStringView const filePath) {
  debugLayerFnEntry();
  return pul.fileRemoveRecursive(filePath);
}

bool puleFileDirectoryCreate(PuleStringView const path) {
  debugLayerFnEntry();
  return pul.fileDirectoryCreate(path);
}

bool puleFileDirectoryCreateRecursive(PuleStringView const path) {
  debugLayerFnEntry();
  return pul.fileDirectoryCreateRecursive(path);
}

PuleStringView puleFilesystemExecutablePath() {
  debugLayerFnEntry();
  return pul.filesystemExecutablePath();
}

PuleString puleFilesystemCurrentPath(PuleAllocator const allocator) {
  debugLayerFnEntry();
  return pul.filesystemCurrentPath(allocator);
}

bool puleFilesystemSymlinkCreate(
  PuleStringView const target,
  PuleStringView const linkDst
) {
  debugLayerFnEntry();
  return (
    pul.filesystemSymlinkCreate(
      target,
      linkDst
    )
  );
}

PuleTimestamp puleFilesystemTimestamp(PuleStringView const path) {
  debugLayerFnEntry();
  return pul.filesystemTimestamp(path);
}

PuleFileWatcher puleFileWatch(PuleFileWatchCreateInfo const createinfo) {
  debugLayerFnEntry();
  return pul.fileWatch(createinfo);
}

// script
PuleScriptContext puleScriptContextCreate() {
  debugLayerFnEntry();
  return pul.scriptContextCreate();
}

void puleScriptContextDestroy(PuleScriptContext const context) {
  debugLayerFnEntry();
  return pul.scriptContextDestroy(context);
}

PuleScriptModule puleScriptModuleCreateFromSource(
  PuleScriptContext const context,
  PuleScriptModuleFromSourceCreateInfo const createInfo,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.scriptModuleCreateFromSource(
      context,
      createInfo,
      error
    )
  );
}

void puleScriptModuleUpdateFromSource(
  PuleScriptContext const context,
  PuleScriptModule const scriptModule,
  PuleStringView scriptSource,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.scriptModuleUpdateFromSource(
      context,
      scriptModule,
      scriptSource,
      error
    )
  );
}

PuleScriptModule puleScriptModuleCreateFromBinary(
  PuleScriptContext const context,
  PuleArrayView const scriptBinary,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.scriptModuleCreateFromBinary(
      context,
      scriptBinary,
      error
    )
  );
}

void puleScriptModuleDestroy(
  PuleScriptContext const context,
  PuleScriptModule const scriptModule
) {
  debugLayerFnEntry();
  return (
    pul.scriptModuleDestroy(
      context,
      scriptModule
    )
  );
}

void puleScriptModuleExecute(
  PuleScriptContext const context,
  PuleScriptModule const scriptModule,
  PuleStringView const functionLabel,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.scriptModuleExecute(
      context,
      scriptModule,
      functionLabel,
      error
    )
  );
}

void * puleScriptGlobal(PuleStringView const globalLabel) {
  debugLayerFnEntry();
  return pul.scriptGlobal(globalLabel);
}

void puleScriptGlobalSet(PuleStringView const globalLabel, void * const data) {
  debugLayerFnEntry();
  return pul.scriptGlobalSet(globalLabel, data);
}

PuleScriptArrayF32 puleScriptArrayF32Create(PuleAllocator const allocator) {
  debugLayerFnEntry();
  return pul.scriptArrayF32Create(allocator);
}

void puleScriptArrayF32Destroy(PuleScriptArrayF32 const arr) {
  debugLayerFnEntry();
  return pul.scriptArrayF32Destroy(arr);
}

void * puleScriptArrayF32Ptr(PuleScriptArrayF32 const arr) {
  debugLayerFnEntry();
  return pul.scriptArrayF32Ptr(arr);
}

void puleScriptArrayF32Append(
  PuleScriptArrayF32 const arr,
  float const value
) {
  debugLayerFnEntry();
  return (
    pul.scriptArrayF32Append(
      arr,
      value
    )
  );
}

void puleScriptArrayF32Remove(
  PuleScriptArrayF32 const arr,
  size_t const index
) {
  debugLayerFnEntry();
  return (
    pul.scriptArrayF32Remove(
      arr,
      index
    )
  );
}

float puleScriptArrayF32At(PuleScriptArrayF32 const arr, size_t const index) {
  debugLayerFnEntry();
  return pul.scriptArrayF32At(arr, index);
}

size_t puleScriptArrayF32Length(PuleScriptArrayF32 const arr) {
  debugLayerFnEntry();
  return pul.scriptArrayF32Length(arr);
}

size_t puleScriptArrayF32ElementByteSize(PuleScriptArrayF32 const arr) {
  debugLayerFnEntry();
  return pul.scriptArrayF32ElementByteSize(arr);
}

PuleScriptModuleFileWatchReturn puleScriptModuleFileWatch(
  PuleScriptContext const context,
  PuleAllocator const allocator,
  PuleStringView const filename,
  PuleScriptDebugSymbols debug,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.scriptModuleFileWatch(
      context,
      allocator,
      filename,
      debug,
      error
    )
  );
}

// plugin
void pulePluginsLoad(
  PuleStringView const * const paths,
  size_t const pathsLength
) {
  debugLayerFnEntry();
  return (
    pul.pluginsLoad(
      paths,
      pathsLength
    )
  );
}

void pulePluginsFree() {
  debugLayerFnEntry();
  return pul.pluginsFree();
}

void pulePluginsReload() {
  debugLayerFnEntry();
  return pul.pluginsReload();
}

size_t pulePluginIdFromName(char const * const pluginNameCStr) {
  debugLayerFnEntry();
  return pul.pluginIdFromName(pluginNameCStr);
}

void * pulePluginLoadFn(size_t const pluginId, char const * const fnCStr) {
  debugLayerFnEntry();
  return pul.pluginLoadFn(pluginId, fnCStr);
}

void * puleTryPluginLoadFn(size_t const pluginId, char const * const fnCStr) {
  debugLayerFnEntry();
  return pul.tryPluginLoadFn(pluginId, fnCStr);
}

void puleIteratePlugins(
  void (* fn)(PulePluginInfo const plugin, void * const userdata),
  void * const userdata
) {
  debugLayerFnEntry();
  return (
    pul.iteratePlugins(
      fn,
      userdata
    )
  );
}

void * pulePluginPayloadFetch(
  PulePluginPayload const payload,
  PuleStringView const handle
) {
  debugLayerFnEntry();
  return (
    pul.pluginPayloadFetch(
      payload,
      handle
    )
  );
}

uint64_t pulePluginPayloadFetchU64(
  PulePluginPayload const payload,
  PuleStringView const handle
) {
  debugLayerFnEntry();
  return (
    pul.pluginPayloadFetchU64(
      payload,
      handle
    )
  );
}

void pulePluginPayloadStore(
  PulePluginPayload const payload,
  PuleStringView const handle,
  void * const data
) {
  debugLayerFnEntry();
  for (size_t it = 0; it < handle.len; ++ it) {
    char const ch = handle.contents[it];
    if (ch == '\t' || ch == '\n' || ch == ' ' || ch == '\r' || ch == '\n') {
      puleLogError(
        "invalid whitespace character in handle: %s", handle.contents
      );
    }
  }
  return pul.pluginPayloadStore(payload, handle, data);
}

void pulePluginPayloadStoreU64(
  PulePluginPayload const payload,
  PuleStringView const handle,
  uint64_t const data
) {
  debugLayerFnEntry();
  for (size_t it = 0; it < handle.len; ++ it) {
    char const ch = handle.contents[it];
    if (ch == '\t' || ch == '\n' || ch == ' ' || ch == '\r' || ch == '\n') {
      puleLogError(
        "invalid whitespace character in handle: %s", handle.contents
      );
    }
  }
  return (
    pul.pluginPayloadStoreU64(
      payload,
      handle,
      data
    )
  );
}

void pulePluginPayloadRemove(
  PulePluginPayload const payload,
  PuleStringView const handle
) {
  debugLayerFnEntry();
  return (
    pul.pluginPayloadRemove(
      payload,
      handle
    )
  );
}

// time
PuleMicrosecond puleMicrosecond(int64_t const value) {
  debugLayerFnEntry();
  return pul.microsecond(value);
}

void puleSleepMicrosecond(PuleMicrosecond const us) {
  debugLayerFnEntry();
  return pul.sleepMicrosecond(us);
}

// gfx
PuleGfxPipelineDescriptorSetLayout puleGfxPipelineDescriptorSetLayout() {
  debugLayerFnEntry();
  return pul.gfxPipelineDescriptorSetLayout();
}

PuleGfxPipeline puleGfxPipelineCreate(
  PuleGfxPipelineCreateInfo const * const createInfo,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.gfxPipelineCreate(
      createInfo,
      error
    )
  );
}

void puleGfxPipelineUpdate(
  PuleGfxPipeline const pipeline,
  PuleGfxPipelineCreateInfo const * const updateInfo,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.gfxPipelineUpdate(
      pipeline,
      updateInfo,
      error
    )
  );
}

void puleGfxPipelineDestroy(PuleGfxPipeline const pipeline) {
  debugLayerFnEntry();
  return pul.gfxPipelineDestroy(pipeline);
}

PuleGfxShaderModule puleGfxShaderModuleCreate(
  PuleBufferView const vertexShaderBytecode,
  PuleBufferView const fragmentShaderBytecode,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.gfxShaderModuleCreate(
      vertexShaderBytecode,
      fragmentShaderBytecode,
      error
    )
  );
}

void puleGfxShaderModuleDestroy(PuleGfxShaderModule const shaderModule) {
  debugLayerFnEntry();
  return pul.gfxShaderModuleDestroy(shaderModule);
}

PuleGfxSampler puleGfxSamplerCreate(
  PuleGfxSamplerCreateInfo const createInfo
) {
  debugLayerFnEntry();
  return (
    pul.gfxSamplerCreate(
      createInfo
    )
  );
}

void puleGfxSamplerDestroy(PuleGfxSampler const sampler) {
  debugLayerFnEntry();
  return pul.gfxSamplerDestroy(sampler);
}

PuleGfxGpuImage puleGfxGpuImageCreate(
  PuleGfxImageCreateInfo const imageCreateInfo
) {
  debugLayerFnEntry();
  return (
    pul.gfxGpuImageCreate(
      imageCreateInfo
    )
  );
}

void puleGfxGpuImageDestroy(PuleGfxGpuImage const image) {
  debugLayerFnEntry();
  return pul.gfxGpuImageDestroy(image);
}

PuleGfxFramebufferCreateInfo puleGfxFramebufferCreateInfo() {
  debugLayerFnEntry();
  return pul.gfxFramebufferCreateInfo();
}

PuleGfxFramebuffer puleGfxFramebufferCreate(
  PuleGfxFramebufferCreateInfo const framebufferCreateInfo,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.gfxFramebufferCreate(
      framebufferCreateInfo,
      error
    )
  );
}

void puleGfxFramebufferDestroy(PuleGfxFramebuffer const framebuffer) {
  debugLayerFnEntry();
  return pul.gfxFramebufferDestroy(framebuffer);
}

PuleGfxGpuImage puleGfxWindowImage() {
  debugLayerFnEntry();
  return pul.gfxWindowImage();
}

PuleGfxGpuBuffer puleGfxGpuBufferCreate(
  PuleStringView const name,
  void const * const optionalInitialData,
  size_t const byteLength,
  PuleGfxGpuBufferUsage const usage,
  PuleGfxGpuBufferVisibilityFlag const visibility
) {
  debugLayerFnEntry();
  return (
    pul.gfxGpuBufferCreate(
      name,
      optionalInitialData,
      byteLength,
      usage,
      visibility
    )
  );
}

void puleGfxGpuBufferDestroy(PuleGfxGpuBuffer const buffer) {
  debugLayerFnEntry();
  return pul.gfxGpuBufferDestroy(buffer);
}

void * puleGfxGpuBufferMap(PuleGfxGpuBufferMapRange const range) {
  debugLayerFnEntry();
  PULE_assert(
    std::this_thread::get_id() == renderThread
    && "puleGfxGpuBufferMap must be used on render thread"
  );
  return pul.gfxGpuBufferMap(range);
}

void puleGfxGpuBufferMappedFlush(
  PuleGfxGpuBufferMappedFlushRange const range
) {
  debugLayerFnEntry();
  return (
    pul.gfxGpuBufferMappedFlush(
      range
    )
  );
}

void puleGfxGpuBufferUnmap(PuleGfxGpuBuffer const buffer) {
  debugLayerFnEntry();
  return pul.gfxGpuBufferUnmap(buffer);
}

void puleGfxInitialize(PulePlatform const platform, PuleError * const error) {
  debugLayerFnEntry();
  renderThread = std::this_thread::get_id();
  pul.gfxInitialize(platform, error);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

void puleGfxShutdown() {
  debugLayerFnEntry();
  return pul.gfxShutdown();
}

void puleGfxFrameStart() {
  debugLayerFnEntry();
  return pul.gfxFrameStart();
}

void puleGfxFrameEnd() {
  debugLayerFnEntry();
  return pul.gfxFrameEnd();
}

void puleGfxDebugPrint() {
  debugLayerFnEntry();
  return pul.gfxDebugPrint();
}

PuleStringView puleGfxActionToString(PuleGfxAction const action) {
  debugLayerFnEntry();
  return pul.gfxActionToString(action);
}

PuleGfxCommandList puleGfxCommandListCreate(
  PuleAllocator const allocator,
  PuleStringView const label
) {
  debugLayerFnEntry();
  return (
    pul.gfxCommandListCreate(
      allocator,
      label
    )
  );
}

void puleGfxCommandListDestroy(PuleGfxCommandList const commandList) {
  debugLayerFnEntry();
  return pul.gfxCommandListDestroy(commandList);
}

PuleStringView puleGfxCommandListName(PuleGfxCommandList const commandList) {
  debugLayerFnEntry();
  return pul.gfxCommandListName(commandList);
}

void puleGfxCommandListRecorderFinish(
  PuleGfxCommandListRecorder const commandListRecorder
) {
  debugLayerFnEntry();
  return (
    pul.gfxCommandListRecorderFinish(
      commandListRecorder
    )
  );
}

void puleGfxCommandListAppendAction(
  PuleGfxCommandListRecorder const commandListRecorder,
  PuleGfxCommand const action
) {
  debugLayerFnEntry();
  return (
    pul.gfxCommandListAppendAction(
      commandListRecorder,
      action
    )
  );
}

void puleGfxCommandListSubmit(
  PuleGfxCommandListSubmitInfo const info,
  PuleError * const error
) {
  debugLayerFnEntry();
  return (
    pul.gfxCommandListSubmit(
      info,
      error
    )
  );
}

void puleGfxCommandListDump(PuleGfxCommandList const commandList) {
  debugLayerFnEntry();
  return pul.gfxCommandListDump(commandList);
}

PuleGfxFence puleGfxFenceCreate(PuleGfxFenceConditionFlag const condition) {
  debugLayerFnEntry();
  return pul.gfxFenceCreate(condition);
}

void puleGfxFenceDestroy(PuleGfxFence const fence) {
  debugLayerFnEntry();
  return pul.gfxFenceDestroy(fence);
}

bool puleGfxFenceCheckSignal(
  PuleGfxFence const fence,
  PuleNanosecond const timeout
) {
  debugLayerFnEntry();
  return (
    pul.gfxFenceCheckSignal(
      fence,
      timeout
    )
  );
}

void puleGfxMemoryBarrier(PuleGfxMemoryBarrierFlag const barrier) {
  debugLayerFnEntry();
  return pul.gfxMemoryBarrier(barrier);
}

} // C

/* // TODO all these need to be scoped by world */
/* namespace { */
/*   struct ComponentInfo { */
/*     std::string label; */
/*     size_t byteLength; */
/*   }; */

/*   std::unordered_map<uint64_t, ComponentInfo> componentInfos; */

/*   static PuleEngineLayer pul; */
/* } */

/* extern "C" { */

/* PuleEcsWorld puleEcsWorldCreate() { */
/*   debugLayerFnEntry(); */
/*   return pul.ecsWorldCreate(); */
/* } */

/* void puleEngineLayerEntry(PuleEngineLayer * const parentLayer) { */
/*   puleLog("Using layer entry: %s", parentLayer->layerName.contents); */
/*   pul = *parentLayer; */
/* } */

/* void puleEcsWorldAdvance( */
/*   PuleEcsWorld const world, float const msDelta */
/* ) { */
/*   debugLayerFnEntry(); */
/*   return pul.ecsWorldAdvance(world, msDelta); */
/* } */

/* PuleEcsComponent puleEcsComponentCreate( */
/*   PuleEcsWorld const world, */
/*   PuleEcsComponentCreateInfo const info */
/* ) { */
/*   debugLayerFnEntry(); */
/*   return pul.ecsComponentCreate(world, info); */
/* } */

/* void puleEcsEntityAttachComponent( */
/*   PuleEcsWorld const world, */
/*   PuleEcsEntity const entity, */
/*   PuleEcsComponent const component, */
/*   void const * const nullableInitialData */
/* ) { */
/*   debugLayerFnEntry(); */
/*   if (entity.id == 0 && component.id == 0) { */
/*     debugLayerError("Attaching null component to null entity"); */
/*   } */
/*   if (entity.id == 0) { */
/*     /1* auto const componentInfo = ::componentInfos.at(component.id); *1/ */
/*     debugLayerError( */
/*       "Attaching component '%s' to null entity", */
/*       "" */
/*     ); */
/*   } */
/*   if (component.id == 0) { */
/*     debugLayerError( */
/*       "attaching a null component to entity '%s'", "" */
/*     ); */
/*   } */
/*   pul.ecsEntityAttachComponent(world, entity, component, nullableInitialData); */
/* } */


/* } // C */

// -----------------------------------------------------------------------------
// -- task-graph ---------------------------------------------------------------
// -----------------------------------------------------------------------------
void puleTaskGraphExecuteInOrder(PuleTaskGraphExecuteInfo const execute) {
  debugLayerFnEntry();
  /* if (not puleTaskGraphNodeExists(execute. */
  return pul.taskGraphExecuteInOrder(execute);
}
