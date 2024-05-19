# imgui-engine

## structs
### PuleImguiEngineDisplayInfo

  presents engine internal/debug information through the gui this can be used
  in conjunction with an editor and the real application

```c
struct {
  world : PuleEcsWorld;
  platform : PulePlatform;
};
```

## functions
### puleImguiEngineDisplay
```c
puleImguiEngineDisplay(
  info : PuleImguiEngineDisplayInfo
) void;
```
