# asset-shader-module

## entities
### PuleAssetShaderModule

## functions
### puleAssetShaderModuleCreateFromPaths
```c
puleAssetShaderModuleCreateFromPaths(
  shaderModuleLabel : PuleStringView,
  vertexPath : PuleStringView,
  fragmentPath : PuleStringView
) PuleAssetShaderModule;
```
### puleAssetShaderModuleDestroy
does not destroy underlying shader module
```c
puleAssetShaderModuleDestroy(
  assetShaderModule : PuleAssetShaderModule
) void;
```
### puleAssetShaderModuleLabel
```c
puleAssetShaderModuleLabel(
  assetShaderModule : PuleAssetShaderModule
) PuleStringView;
```
### puleAssetShaderModuleHandle
```c
puleAssetShaderModuleHandle(
  assetShaderModule : PuleAssetShaderModule
) PuleGpuShaderModule;
```
