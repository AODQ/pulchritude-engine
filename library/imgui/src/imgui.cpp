#include <pulchritude-imgui/imgui.h>

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-asset/render-graph.h>
#include <pulchritude-asset/shader-module.h>
#include <pulchritude-error/error.h>
#include <pulchritude-gpu/commands.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/image.h>
#include <pulchritude-log/log.h>
#include <pulchritude-platform/platform.h>
#include <pulchritude-string/string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#include <imgui/imgui.h>
#pragma GCC diagnostic pop

#include <unordered_map>
#include <string>

////////////////////////////////////////////////////////////////////////////////
//-- GRAPHICS ------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

namespace {

struct RenderData {
  PuleGpuPipeline pipeline { 0 };
  PuleGpuImage fontImage { 0 };
  PuleGpuSampler fontImageSampler { 0 };
  PuleGpuShaderModule shaderModule { 0 };
  PuleGpuBuffer vertexBuffer { 0 };
  PuleGpuBuffer elementsBuffer { 0 };
  size_t vertexBufferLength = 0;
  size_t elementsBufferLength = 0;
};

PuleGpuPipelineLayoutDescriptorSet createPipelineDescriptorSetLayout() {
  auto descriptorSetLayout = puleGpuPipelineDescriptorSetLayout();
  descriptorSetLayout.attributeBufferBindings[0] = {
    .stridePerElement = sizeof(ImDrawVert),
  };
  descriptorSetLayout.attributeBindings[0] = {
    .dataType = PuleGpuAttributeDataType_float,
    .bufferIndex = 0,
    .numComponents = 2,
    .convertFixedDataTypeToNormalizedFloating = false,
    .offsetIntoBuffer = offsetof(ImDrawVert, pos),
  };
  descriptorSetLayout.attributeBindings[1] = {
    .dataType = PuleGpuAttributeDataType_float,
    .bufferIndex = 0,
    .numComponents = 2,
    .convertFixedDataTypeToNormalizedFloating = false,
    .offsetIntoBuffer = offsetof(ImDrawVert, uv),
  };
  descriptorSetLayout.attributeBindings[2] = {
    .dataType = PuleGpuAttributeDataType_unsignedByte,
    .bufferIndex = 0,
    .numComponents = 4,
    .convertFixedDataTypeToNormalizedFloating = true,
    .offsetIntoBuffer = offsetof(ImDrawVert, col),
  };
  // descriptorSetLayout.bufferElementBinding = bd.elementsBuffer;
  descriptorSetLayout.textureBindings[0] = PuleGpuDescriptorStage_fragment;
  return descriptorSetLayout;
}

void createFontsTexture() {
  ImGuiIO & io = ImGui::GetIO();
  auto & bd = *reinterpret_cast<RenderData *>(io.BackendRendererUserData);

  bd.fontImageSampler = (
    puleGpuSamplerCreate({
      .minify  = PuleGpuImageMagnification_nearest,
      .magnify = PuleGpuImageMagnification_nearest,
      .wrapU = PuleGpuImageWrap_clampToEdge,
      .wrapV = PuleGpuImageWrap_clampToEdge,
    })
  );

  uint8_t * pixels;
  int32_t width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
  bd.fontImage = (
    puleGpuImageCreate({
      .width = static_cast<uint32_t>(width),
      .height = static_cast<uint32_t>(height),
      .target = PuleGpuImageTarget_i2D,
      .byteFormat = PuleGpuImageByteFormat_rgba8U,
      .sampler = bd.fontImageSampler,
      .optionalInitialData = pixels,
    })
  );

  io.Fonts->SetTexID(reinterpret_cast<ImTextureID>(bd.fontImage.id));
}

void initializeRenderData([[maybe_unused]]PulePlatform const platform) {
  ImGuiIO & io = ImGui::GetIO();
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
  auto & bd = (
    *reinterpret_cast<RenderData *>(io.BackendRendererUserData)
  );

  auto assetShaderModule = (
    puleAssetShaderModuleCreateFromPaths(
      puleCStr("imgui-module"),
      puleCStr("assets/imgui.vert.spv"),
      puleCStr("assets/imgui.frag.spv")
    )
  );

  bd.shaderModule = puleAssetShaderModuleHandle(assetShaderModule);

  // TODO create vertex+elements buffer
  createFontsTexture();

  auto const descriptorSetLayout = createPipelineDescriptorSetLayout();
  auto const pushConstants = PuleGpuPipelineLayoutPushConstant {
    .stage = PuleGpuDescriptorStage_vertex,
    .byteLength = sizeof(float)*4*4,
    .byteOffset = 0,
  };
  auto const pipelineCI = PuleGpuPipelineCreateInfo {
    .shaderModule = bd.shaderModule,
    .layoutDescriptorSet = &descriptorSetLayout,
    .layoutPushConstantsCount = 1,
    .layoutPushConstants = &pushConstants,
    .config = {},
  };

  PuleError err = puleError();
  bd.pipeline = puleGpuPipelineCreate(&pipelineCI, &err);
  if (puleErrorConsume(&err)) { return; }
}

void renderDrawData(
  ImDrawData * const drawData,
  PuleGpuCommandListRecorder const commandListRecorder
) {
  // don't render when minized
  int32_t const framebufferWidth = (
    static_cast<int32_t>(
      drawData->DisplaySize.x * drawData->FramebufferScale.x
    )
  );
  int32_t const framebufferHeight = (
    static_cast<int32_t>(
      drawData->DisplaySize.y * drawData->FramebufferScale.y
    )
  );
  if (framebufferWidth <= 0 || framebufferHeight <= 0) {
    return;
  }

  auto & bd = (
    *reinterpret_cast<RenderData *>(ImGui::GetIO().BackendRendererUserData)
  );

  PuleF32m44 orthographicProjection;
  {
    float const L = drawData->DisplayPos.x;
    float const R = drawData->DisplayPos.x + drawData->DisplaySize.x;
    float const T = drawData->DisplayPos.y;
    float const B = drawData->DisplayPos.y + drawData->DisplaySize.y;
    float const ortho[16] = {
      2.0f/(R-L),  0.0f,         0.0f, 0.0f,
      0.0f,        2.0f/(T-B),   0.0f, 0.0f,
      0.0f,        0.0f,        -1.0f, 0.0f,
      (R+L)/(L-R), (T+B)/(B-T),  0.0f, 1.0f,
    };

    orthographicProjection = puleF32m44PtrTranspose(&ortho[0]);
  }

  ImVec2 clipOff = drawData->DisplayPos;
  ImVec2 clipScale = drawData->FramebufferScale;

  PuleError err = puleError();

  for (
    int32_t drawListIt = 0;
    drawListIt < drawData->CmdListsCount;
    ++ drawListIt
  ) {
    ImDrawList const * drawList = drawData->CmdLists[drawListIt];

    auto const vertexBufferLength = (
      static_cast<size_t>(drawList->VtxBuffer.Size * sizeof(ImDrawVert))
    );
    auto const elementsBufferLength = (
      static_cast<size_t>(drawList->IdxBuffer.Size * sizeof(ImDrawIdx))
    );

    if (bd.vertexBufferLength < vertexBufferLength) {
      bd.vertexBufferLength = vertexBufferLength;
      puleGpuBufferDestroy(bd.vertexBuffer);
      bd.vertexBuffer = puleGpuBufferCreate(
        puleCStr("imgui-vertex-buffer"),
        nullptr,//reinterpret_cast<void *>(drawList->VtxBuffer.Data),
        bd.vertexBufferLength,
        PuleGpuBufferUsage_attribute,
        PuleGpuBufferVisibilityFlag_hostWritable
      );
    }

    { // update vertex buffer
      uint8_t * mappedData = (
        reinterpret_cast<uint8_t *>(
          puleGpuBufferMap({
            .buffer = bd.vertexBuffer,
            .access = PuleGpuBufferMapAccess_hostWritable,
            .byteOffset = 0,
            .byteLength = bd.vertexBufferLength,
          })
        )
      );
      memcpy(
        mappedData,
        reinterpret_cast<void *>(drawList->VtxBuffer.Data),
        bd.vertexBufferLength
      );
      puleGpuBufferMappedFlush({
        .buffer = bd.vertexBuffer,
        .byteOffset = 0,
        .byteLength = bd.vertexBufferLength,
      });
      puleGpuBufferUnmap(bd.vertexBuffer);
    }

    if (bd.elementsBufferLength < elementsBufferLength) {
      bd.elementsBufferLength = elementsBufferLength;
      puleGpuBufferDestroy(bd.elementsBuffer);
      bd.elementsBuffer = puleGpuBufferCreate(
        puleCStr("imgui-elements-buffer"),
        nullptr,//reinterpret_cast<void *>(drawList->IdxBuffer.Data),
        bd.elementsBufferLength,
        PuleGpuBufferUsage_element,
        PuleGpuBufferVisibilityFlag_hostWritable
      );
    }

    { // update element buffer
      uint8_t * mappedData = (
        reinterpret_cast<uint8_t *>(
          puleGpuBufferMap({
            .buffer = bd.elementsBuffer,
            .access = PuleGpuBufferMapAccess_hostWritable,
            .byteOffset = 0,
            .byteLength = bd.elementsBufferLength,
          })
        )
      );
      memcpy(
        mappedData,
        reinterpret_cast<void *>(drawList->IdxBuffer.Data),
        bd.elementsBufferLength
      );
      puleGpuBufferMappedFlush({
        .buffer = bd.elementsBuffer,
        .byteOffset = 0,
        .byteLength = bd.elementsBufferLength,
      });
      puleGpuBufferUnmap(bd.elementsBuffer);
    }

    puleGpuMemoryBarrier(PuleGpuMemoryBarrierFlag_bufferUpdate);

    for (int32_t drawIt = 0; drawIt < drawList->CmdBuffer.Size; ++ drawIt) {
      ImDrawCmd const & drawCmd = drawList->CmdBuffer[drawIt];
      if (drawCmd.UserCallback != nullptr) {
        drawCmd.UserCallback(drawList, &drawCmd);
        continue;
      }
      // project scissor/clipping rectangles
      auto clipMin = ImVec2 {
        (drawCmd.ClipRect.x - clipOff.x) * clipScale.x,
        (drawCmd.ClipRect.y - clipOff.y) * clipScale.y,
      };
      auto clipMax = ImVec2 {
        (drawCmd.ClipRect.z - clipOff.x) * clipScale.x,
        (drawCmd.ClipRect.w - clipOff.y) * clipScale.y,
      };
      if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y) {
        continue;
      }

      {
        puleGpuCommandListAppendAction(
          commandListRecorder,
          PuleGpuCommand {
            .renderPassBegin {
              .action = PuleGpuAction_renderPassBegin,
              .viewportUpperLeft = PuleI32v2 { .x = 0, .y = 0, },
              .viewportLowerRight = PuleI32v2 {
                .x = framebufferWidth,
                .y = framebufferHeight
              },
              .attachmentColorCount = 1,
              .attachmentColor = {
                PuleGpuImageAttachment {
                  .opLoad = PuleGpuImageAttachmentOpLoad_clear,
                  .opStore = PuleGpuImageAttachmentOpStore_store,
                  .layout = PuleGpuImageLayout_attachmentColor,
                  .clearColor = {},
                  .imageView = (
                    PuleGpuImageView {
                      .image = puleGpuWindowImage(),
                      .mipmapLevelStart = 0, .mipmapLevelCount = 1,
                      .arrayLayerStart = 0, .arrayLayerCount = 1,
                      .byteFormat = PuleGpuImageByteFormat_bgra8U,
                    }
                  ),
                },
              },
              .attachmentDepth = {},
            },
          }
        );
        puleGpuCommandListAppendAction(
          commandListRecorder,
          PuleGpuCommand {
            .bindPipeline {
              .action = PuleGpuAction_bindPipeline,
              .pipeline = bd.pipeline,
            }
          }
        );

        // pos , uv , col
        puleGpuCommandListAppendAction(
          commandListRecorder,
          PuleGpuCommand {
            .bindAttributeBuffer {
              .action = PuleGpuAction_bindAttributeBuffer,
              .pipeline = bd.pipeline,
              .bindingIndex = 0,
              .buffer = bd.vertexBuffer,
              .offset = 0,
              .stride = sizeof(ImDrawVert),
            }
          }
        );

        puleGpuCommandListAppendAction(
          commandListRecorder,
          PuleGpuCommand {
            .bindElementBuffer {
              .action = PuleGpuAction_bindElementBuffer,
              .buffer = bd.elementsBuffer,
              .offset = 0,
              .elementType = (
                sizeof(ImDrawIdx) == 2
                  ? PuleGpuElementType_u16
                  : PuleGpuElementType_u32
              )
            }
          }
        );

        puleGpuCommandListAppendAction(
          commandListRecorder,
          {
            .pushConstants = {
              .action = PuleGpuAction_pushConstants,
              .stage = PuleGpuDescriptorStage_vertex,
              .byteLength = sizeof(orthographicProjection),
              .byteOffset = 0,
              .data = &orthographicProjection,
            }
          }
        );

        puleLogDev(
          "render element:: "
          " numElements: %d,"
          " elementOffset: %zu,"
          " baseVertexOffset: %zu",
          drawCmd.ElemCount,
          drawCmd.IdxOffset * sizeof(ImDrawIdx),
          drawCmd.VtxOffset
        );
        puleGpuCommandListAppendAction(
          commandListRecorder,
          {
            .dispatchRenderElements = {
              .action = PuleGpuAction_dispatchRenderElements,
              .drawPrimitive = PuleGpuDrawPrimitive_triangle,
              .numElements = drawCmd.ElemCount,
              .elementOffset = drawCmd.IdxOffset * sizeof(ImDrawIdx),
              .baseVertexOffset = drawCmd.VtxOffset,
            },
          }
        );

        puleGpuCommandListAppendAction(
          commandListRecorder,
          PuleGpuCommand {
            .renderPassEnd {
              .action = PuleGpuAction_renderPassEnd,
            },
          }
        );
      }
    }
  }
}

[[maybe_unused]]
void newFrame() {
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
//-- WINDOWING -----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

namespace {

struct WindowData {
  PulePlatform window;
  double time;
  ImVec2 lastValidMousePos;
  bool installedCallbacks;
};


ImGuiKey keyToImGui(PuleInputKey key) {
  switch (key) {
    default: return ImGuiKey_None;
    case PuleInputKey_tab: return ImGuiKey_Tab;
    case PuleInputKey_left: return ImGuiKey_LeftArrow;
    case PuleInputKey_right: return ImGuiKey_RightArrow;
    case PuleInputKey_up: return ImGuiKey_UpArrow;
    case PuleInputKey_down: return ImGuiKey_DownArrow;
    case PuleInputKey_pageUp: return ImGuiKey_PageUp;
    case PuleInputKey_pageDown: return ImGuiKey_PageDown;
    case PuleInputKey_home: return ImGuiKey_Home;
    case PuleInputKey_end: return ImGuiKey_End;
    case PuleInputKey_insert: return ImGuiKey_Insert;
    case PuleInputKey_delete: return ImGuiKey_Delete;
    case PuleInputKey_backspace: return ImGuiKey_Backspace;
    case PuleInputKey_space: return ImGuiKey_Space;
    case PuleInputKey_enter: return ImGuiKey_Enter;
    case PuleInputKey_escape: return ImGuiKey_Escape;
    case PuleInputKey_apostrophe: return ImGuiKey_Apostrophe;
    case PuleInputKey_comma: return ImGuiKey_Comma;
    case PuleInputKey_minus: return ImGuiKey_Minus;
    case PuleInputKey_period: return ImGuiKey_Period;
    case PuleInputKey_slash: return ImGuiKey_Slash;
    case PuleInputKey_semicolon: return ImGuiKey_Semicolon;
    case PuleInputKey_equal: return ImGuiKey_Equal;
    case PuleInputKey_leftBracket: return ImGuiKey_LeftBracket;
    case PuleInputKey_backslash: return ImGuiKey_Backslash;
    case PuleInputKey_rightBracket: return ImGuiKey_RightBracket;
    case PuleInputKey_graveAccent: return ImGuiKey_GraveAccent;
    case PuleInputKey_capsLock: return ImGuiKey_CapsLock;
    case PuleInputKey_scrollLock: return ImGuiKey_ScrollLock;
    case PuleInputKey_numLock: return ImGuiKey_NumLock;
    case PuleInputKey_printScreen: return ImGuiKey_PrintScreen;
    case PuleInputKey_pause: return ImGuiKey_Pause;
    case PuleInputKey_kp0: return ImGuiKey_Keypad0;
    case PuleInputKey_kp1: return ImGuiKey_Keypad1;
    case PuleInputKey_kp2: return ImGuiKey_Keypad2;
    case PuleInputKey_kp3: return ImGuiKey_Keypad3;
    case PuleInputKey_kp4: return ImGuiKey_Keypad4;
    case PuleInputKey_kp5: return ImGuiKey_Keypad5;
    case PuleInputKey_kp6: return ImGuiKey_Keypad6;
    case PuleInputKey_kp7: return ImGuiKey_Keypad7;
    case PuleInputKey_kp8: return ImGuiKey_Keypad8;
    case PuleInputKey_kp9: return ImGuiKey_Keypad9;
    case PuleInputKey_kpDecimal: return ImGuiKey_KeypadDecimal;
    case PuleInputKey_kpDivide: return ImGuiKey_KeypadDivide;
    case PuleInputKey_kpMultiply: return ImGuiKey_KeypadMultiply;
    case PuleInputKey_kpSubtract: return ImGuiKey_KeypadSubtract;
    case PuleInputKey_kpAdd: return ImGuiKey_KeypadAdd;
    case PuleInputKey_kpEnter: return ImGuiKey_KeypadEnter;
    case PuleInputKey_kpEqual: return ImGuiKey_KeypadEqual;
    case PuleInputKey_leftShift: return ImGuiKey_LeftShift;
    case PuleInputKey_leftControl: return ImGuiKey_LeftCtrl;
    case PuleInputKey_leftAlt: return ImGuiKey_LeftAlt;
    case PuleInputKey_leftSuper: return ImGuiKey_LeftSuper;
    case PuleInputKey_rightShift: return ImGuiKey_RightShift;
    case PuleInputKey_rightControl: return ImGuiKey_RightCtrl;
    case PuleInputKey_rightAlt: return ImGuiKey_RightAlt;
    case PuleInputKey_rightSuper: return ImGuiKey_RightSuper;
    case PuleInputKey_menu: return ImGuiKey_Menu;
    case PuleInputKey_i0: return ImGuiKey_0;
    case PuleInputKey_i1: return ImGuiKey_1;
    case PuleInputKey_i2: return ImGuiKey_2;
    case PuleInputKey_i3: return ImGuiKey_3;
    case PuleInputKey_i4: return ImGuiKey_4;
    case PuleInputKey_i5: return ImGuiKey_5;
    case PuleInputKey_i6: return ImGuiKey_6;
    case PuleInputKey_i7: return ImGuiKey_7;
    case PuleInputKey_i8: return ImGuiKey_8;
    case PuleInputKey_i9: return ImGuiKey_9;
    case PuleInputKey_a: return ImGuiKey_A;
    case PuleInputKey_b: return ImGuiKey_B;
    case PuleInputKey_c: return ImGuiKey_C;
    case PuleInputKey_d: return ImGuiKey_D;
    case PuleInputKey_e: return ImGuiKey_E;
    case PuleInputKey_f: return ImGuiKey_F;
    case PuleInputKey_g: return ImGuiKey_G;
    case PuleInputKey_h: return ImGuiKey_H;
    case PuleInputKey_i: return ImGuiKey_I;
    case PuleInputKey_j: return ImGuiKey_J;
    case PuleInputKey_k: return ImGuiKey_K;
    case PuleInputKey_l: return ImGuiKey_L;
    case PuleInputKey_m: return ImGuiKey_M;
    case PuleInputKey_n: return ImGuiKey_N;
    case PuleInputKey_o: return ImGuiKey_O;
    case PuleInputKey_p: return ImGuiKey_P;
    case PuleInputKey_q: return ImGuiKey_Q;
    case PuleInputKey_r: return ImGuiKey_R;
    case PuleInputKey_s: return ImGuiKey_S;
    case PuleInputKey_t: return ImGuiKey_T;
    case PuleInputKey_u: return ImGuiKey_U;
    case PuleInputKey_v: return ImGuiKey_V;
    case PuleInputKey_w: return ImGuiKey_W;
    case PuleInputKey_x: return ImGuiKey_X;
    case PuleInputKey_y: return ImGuiKey_Y;
    case PuleInputKey_z: return ImGuiKey_Z;
    case PuleInputKey_f1: return ImGuiKey_F1;
    case PuleInputKey_f2: return ImGuiKey_F2;
    case PuleInputKey_f3: return ImGuiKey_F3;
    case PuleInputKey_f4: return ImGuiKey_F4;
    case PuleInputKey_f5: return ImGuiKey_F5;
    case PuleInputKey_f6: return ImGuiKey_F6;
    case PuleInputKey_f7: return ImGuiKey_F7;
    case PuleInputKey_f8: return ImGuiKey_F8;
    case PuleInputKey_f9: return ImGuiKey_F9;
    case PuleInputKey_f10: return ImGuiKey_F10;
    case PuleInputKey_f11: return ImGuiKey_F11;
    case PuleInputKey_f12: return ImGuiKey_F12;
  }
}

ImGuiMouseButton mouseButtonToImGui(PuleInputMouse const key) {
  switch (key) {
    default: return -1;
    case PuleInputMouse_left: return ImGuiMouseButton_Left;
    case PuleInputMouse_right: return ImGuiMouseButton_Right;
    case PuleInputMouse_middle: return ImGuiMouseButton_Middle;
  }
}

void updateKeyModifiers(PuleInputKeyModifier mod) {
  ImGuiIO & io = ImGui::GetIO();
  io.AddKeyEvent(ImGuiKey_ModCtrl,  (mod & PuleInputKeyModifier_ctrl) > 0);
  io.AddKeyEvent(ImGuiKey_ModShift, (mod & PuleInputKeyModifier_shift) > 0);
  io.AddKeyEvent(ImGuiKey_ModAlt,   (mod & PuleInputKeyModifier_alt) > 0);
  io.AddKeyEvent(ImGuiKey_ModSuper, (mod & PuleInputKeyModifier_super) > 0);
}

[[maybe_unused]]
void scrollCallback(float xoffset, float yoffset) {
  (void)xoffset;
  (void)yoffset;
}

[[maybe_unused]]
void translateUntranslatedKey() {}

void keyCallback(
  PuleInputKey const key,
  PuleInputKeyModifier const modifier,
  bool const pressed
) {
  updateKeyModifiers(modifier);
  ImGuiIO & io = ImGui::GetIO();
  io.AddKeyEvent(keyToImGui(key), pressed);
}

void rawTextCallback(uint32_t const character) {
  ImGuiIO & io = ImGui::GetIO();
  io.AddInputCharacter(character);
}

void mouseButtonCallback(
  PuleInputMouse const key,
  PuleInputKeyModifier const modifier,
  bool const pressed
) {
  updateKeyModifiers(modifier);
  ImGuiIO & io = ImGui::GetIO();
  auto const mouseButton = mouseButtonToImGui(key);
  if (mouseButton != -1) {
    io.AddMouseButtonEvent(mouseButton, pressed);
  }
}

[[maybe_unused]]
void windowFocusCallback() {}

[[maybe_unused]]
void cursorPosCallback() {}

[[maybe_unused]]
void cursorEnterCallback() {}

[[maybe_unused]]
void charCallback() {}

void initWindowing(PulePlatform const window) {
  ImGuiIO & io = ImGui::GetIO();
  if (io.BackendPlatformUserData != nullptr) {
    puleLogError("already initialized imgui windowing backend");
    return;
  }

  WindowData * const bdPtr = IM_NEW(WindowData);
  io.BackendPlatformUserData = reinterpret_cast<void *>(bdPtr);
  WindowData & bd = *bdPtr;
  io.BackendPlatformName = "pulchritude_impl";
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  bd.window = window;
  bd.time = 0.0;

  // add callbacks
  puleInputKeyCallback({keyCallback});
  puleInputRawTextCallback({rawTextCallback});
  puleInputMouseButtonCallback({mouseButtonCallback});
}

void shutdownWindowing() {
  ImGuiIO & io = ImGui::GetIO();
  IM_DELETE(reinterpret_cast<WindowData *>(io.BackendPlatformUserData));
}

void updateKeyData() {
  ImGuiIO & io = ImGui::GetIO();
  ::WindowData & bd = (
    *reinterpret_cast<::WindowData *>(io.BackendPlatformUserData)
  );

  if (!pulePlatformFocused(bd.window)) {
    return;
  }

  // update scroll
  io.AddMouseWheelEvent(0, 0);

  // update modifiers
  updateKeyModifiers(puleInputKeyModifiers(bd.window));
}

void updateMouseData() {
  ImGuiIO & io = ImGui::GetIO();
  ::WindowData & bd = (
    *reinterpret_cast<::WindowData *>(io.BackendPlatformUserData)
  );

  if (!pulePlatformFocused(bd.window)) {
    return;
  }

  if (io.WantSetMousePos) {
    pulePlatformMouseOriginSet(
      bd.window,
      PuleI32v2{
        static_cast<int32_t>(io.MousePos.x),
        static_cast<int32_t>(io.MousePos.y),
      }
    );
  }

  PuleI32v2 const mouseOrigin = pulePlatformMouseOrigin(bd.window);
  auto const mouseOriginF = PuleF32v2 {
    static_cast<float>(mouseOrigin.x), static_cast<float>(mouseOrigin.y),
  };
  io.AddMousePosEvent(mouseOriginF.x, mouseOriginF.y);
  bd.lastValidMousePos = ImVec2(mouseOriginF.x, mouseOriginF.y);
}

void updateMouseCursor() {
  ImGuiIO & io = ImGui::GetIO();
  ::WindowData & bd = (
    *reinterpret_cast<::WindowData *>(io.BackendPlatformUserData)
  );

  if (
    (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
    || !pulePlatformCursorEnabled(bd.window)
  ) {
    return;
  }

  ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
  if (imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
    pulePlatformCursorHide(bd.window);
  } else {
    pulePlatformCursorShow(bd.window);
  }
}

} // namespace

extern "C" {

void puleImguiWindowNewFrame() {
  ImGuiIO & io = ImGui::GetIO();
  ::WindowData & bd = (
    *reinterpret_cast<::WindowData *>(io.BackendPlatformUserData)
  );

  // setup display size
  PuleI32v2 const windowDim = pulePlatformWindowSize(bd.window);
  PuleI32v2 const framebufferDim = pulePlatformFramebufferSize(bd.window);

  io.DisplaySize = ImVec2(
    static_cast<float>(windowDim.x),
    static_cast<float>(windowDim.y)
  );

  if (windowDim.x > 0 && windowDim.y > 0) {
    io.DisplayFramebufferScale = ImVec2(
      static_cast<float>(framebufferDim.x) / windowDim.x,
      static_cast<float>(framebufferDim.y) / windowDim.y
    );
  }

  double const currentTime = pulePlatformGetTime();
  io.DeltaTime = (
    bd.time > 0.0 ? static_cast<float>(currentTime - bd.time) : (1.0f/60.0f)
  );
  bd.time = currentTime;

  updateKeyData();
  updateMouseData();
  updateMouseCursor();
}

} // C

////////////////////////////////////////////////////////////////////////////////
//-- GUI WIDGETS ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

extern "C" {

void puleImguiInitialize(PulePlatform const window) {
  ImGui::CreateContext();
  ImGuiIO & io = ImGui::GetIO();
  if (io.BackendRendererUserData != nullptr) {
    puleLogError("imgui backend already exists");
  }
  ::RenderData * bd = IM_NEW(::RenderData)();
  io.BackendRendererUserData = reinterpret_cast<void *>(bd);
  io.BackendRendererName = "pulchritude_impl";
  ::initializeRenderData(window);
  ::initWindowing(window);
}

void puleImguiShutdown() {
  auto & bd = (
    *reinterpret_cast<RenderData *>(ImGui::GetIO().BackendRendererUserData)
  );

  puleGpuPipelineDestroy(bd.pipeline);
  puleGpuSamplerDestroy(bd.fontImageSampler);
  puleGpuImageDestroy(bd.fontImage);
  IM_DELETE(&bd);

  ::shutdownWindowing();
}

void puleImguiNewFrame() {
  puleImguiWindowNewFrame();
  ImGui::NewFrame();
}

void puleImguiRender(PuleGpuCommandListRecorder const recorder) {
  ImGui::Render();
  renderDrawData(ImGui::GetDrawData(), recorder);
}

void puleImguiJoinNext() {
  ImGui::SameLine();
}

bool puleImguiSliderF32(
  char const * const label,
  float * const data,
  float const min, float const max
) {
  return ImGui::SliderFloat(label, data, min, max);
}

bool puleImguiSliderZu(
  char const * label,
  size_t * const data,
  size_t const min, size_t const max
) {
  int32_t dataI32 = *data;
  bool const ret = (
    ImGui::SliderInt(
      label,
      &dataI32,
      static_cast<int32_t>(min),
      static_cast<int32_t>(max)
    )
  );
  *data = static_cast<size_t>(dataI32);
  return ret;
}

void puleImguiWindowBegin(
  char const * const label,
  bool * const openNullable
) {
  ImGui::Begin(label, openNullable);
}

void puleImguiWindowEnd() {
  ImGui::End();
}

bool puleImguiSectionBegin(char const * const label) {
  return ImGui::TreeNode(label);
}
void puleImguiSectionEnd() {
  ImGui::TreePop();
}

void puleImguiText(char const * const format, ...) {
  va_list args;
  va_start(args, format);
  PuleString const formatStr = (
    puleStringFormatVargs(
      puleAllocateDefault(),
      format, args
    )
  );
  ImGui::Text("%s", formatStr.contents);
  puleStringDestroy(formatStr);
  va_end(args);
}

void puleImguiImage(
  PuleGpuImage const image,
  PuleF32v2 const size,
  PuleF32v2 const uvUl, PuleF32v2 const uvLr,
  PuleF32v4 const border
) {
  ImGui::Image(
    ImTextureID{reinterpret_cast<void *>(image.id)},
    ImVec2{size.x, size.y}, ImVec2{uvUl.x, uvUl.y}, ImVec2{uvLr.x, uvLr.y},
    ImVec4(1,1,1,1), // bg color
    ImVec4{border.x, border.y, border.z, border.w}
  );
}

bool puleImguiLastItemHovered() {
  return ImGui::IsItemHovered();
}

PuleI32v2 puleImguiCurrentOrigin() {
  ImVec2 const o = ImGui::GetCursorScreenPos();
  return PuleI32v2 { (int32_t)o.x, (int32_t)o.y };
}

bool puleImguiToggle(char const * const label, bool * const data) {
  return ImGui::Checkbox(label, data);
}

bool puleImguiButton(char const * const label) {
  return ImGui::Button(label);
}

} // C

// -- callback registry --------------------------------------------------------

namespace {
  std::unordered_map<uint64_t, PuleImguiCallback> registries;
  uint64_t registryCounter = 0;
} // namespace

extern "C" {

PuleImguiCallbackRegistry puleImguiCallbackRegister(
  PuleImguiCallback const callback
) {
  ::registries.emplace(::registryCounter, callback);
  return {.id = ::registryCounter ++};
}

void puleImguiCallbackUnregister(
  PuleImguiCallbackRegistry const registry
) {
  if (::registries.find(registry.id) == ::registries.end()) {
    puleLogError(
      "Trying to unregister unknown imgui callback registry %d\n",
      registry.id
    );
    return;
  }
  ::registries.erase(registry.id);
}

void puleImguiCallbackShowAll() {
  for (auto & registry : registries) {
    registry.second.callback();
  }
}

} // C

