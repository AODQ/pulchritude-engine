# asset-image

## enums
### PuleErrorAssetImage
```c
enum {
  PuleErrorAssetImage_none,
  PuleErrorAssetImage_decode,
}
```
### PuleAssetImageSupportFlag
```c
enum {
  PuleAssetImageSupportFlag_none,
  PuleAssetImageSupportFlag_read,
  PuleAssetImageSupportFlag_write,
  PuleAssetImageSupportFlag_readWrite,
}
```
### PuleAssetImageFormat
```c
enum {
  PuleAssetImageFormat_rgbaU8,
  PuleAssetImageFormat_rgbaU16,
  PuleAssetImageFormat_rgbU8,
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
  requestedFormat : PuleAssetImageFormat,
  error : PuleError ptr
) PuleAssetImage;
```
### puleAssetImageDestroy
```c
puleAssetImageDestroy(
  image : PuleAssetImage
) void;
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
