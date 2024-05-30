# asset-image

## enums
### PuleErrorAssetImage
```c
enum {
  none,
  decode,
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
