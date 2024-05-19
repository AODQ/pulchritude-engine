# tui

## entities
### PuleTuiWindow

## functions
### puleTuiInitialize
```c
puleTuiInitialize() PuleTuiWindow;
```
### puleTuiDestroy
```c
puleTuiDestroy(
  _ : PuleTuiWindow
) void;
```
### puleTuiClear
```c
puleTuiClear(
  _ : PuleTuiWindow
) void;
```
### puleTuiRefresh
```c
puleTuiRefresh(
  _ : PuleTuiWindow
) void;
```
### puleTuiRenderString
```c
puleTuiRenderString(
  _ : PuleTuiWindow,
  content : PuleStringView,
  rgb : PuleF32v3
) void;
```
### puleTuiReadInputCharacterBlocking
```c
puleTuiReadInputCharacterBlocking(
  window : PuleTuiWindow
) int32_t;
```
### puleTuiMoveCursor
```c
puleTuiMoveCursor(
  window : PuleTuiWindow,
  position : PuleI32v2
) void;
```
### puleTuiWindowDim
```c
puleTuiWindowDim(
  window : PuleTuiWindow
) PuleI32v2;
```
