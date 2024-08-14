# asset-image

## enums
### PuleErrorAssetImage
```c
enum {
  none,
  decode,
  encode,
}
```
### PuleAssetImageSupportFlag
```c
enum {
  none,
  read,
  write,
  readWrite,
}
```
### PuleAssetImageFormat
```c
enum {
  rgbaU8,
  rgbaU16,
  rgbU8,
}
```

## entities
### PuleAssetImage

## functions
### puleAssetImageExtensionSupported
```c
puleAssetImageExtensionSupported(
  extension : char const ptr
) PuleAssetImageSupportFlag;
```
### puleAssetImageLoadFromStream
```c
puleAssetImageLoadFromStream(
  allocator : PuleAllocator,
  imageSource : PuleStreamRead,
  imageExtension : PuleStringView,
  requestedFormat : PuleAssetImageFormat,
  error : PuleError ptr
) PuleAssetImage;
```
### puleAssetImage
```c
puleAssetImage(
  width : uint32_t,
  height : uint32_t,
  format : PuleAssetImageFormat
) PuleAssetImage;
```
### puleAssetImageWriteToStream
```c
puleAssetImageWriteToStream(
  image : PuleAssetImage,
  imageDst : PuleStreamWrite,
  imageExtension : PuleStringView,
  error : PuleError ptr
) void;
```
### puleAssetImageDestroy
```c
puleAssetImageDestroy(
  image : PuleAssetImage
) void;
```
### puleAssetImageData
```c
puleAssetImageData(
  image : PuleAssetImage
) PuleBufferView;
```
### puleAssetImageDecodedData
```c
puleAssetImageDecodedData(
  image : PuleAssetImage
) void ptr;
```
### puleAssetImageDecodedDataLength
```c
puleAssetImageDecodedDataLength(
  image : PuleAssetImage
) size_t;
```
### puleAssetImageWidth
```c
puleAssetImageWidth(
  image : PuleAssetImage
) uint32_t;
```
### puleAssetImageHeight
```c
puleAssetImageHeight(
  image : PuleAssetImage
) uint32_t;
```
### puleAssetImageTexel
```c
puleAssetImageTexel(
  image : PuleAssetImage,
  x : uint32_t,
  y : uint32_t
) PuleF32v4;
```
### puleAssetImageTexelSet
```c
puleAssetImageTexelSet(
  image : PuleAssetImage,
  x : uint32_t,
  y : uint32_t,
  rgba : PuleF32v4
) void;
```
