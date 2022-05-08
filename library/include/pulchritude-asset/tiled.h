#pragma once

#include <pulchritude-asset/image.h>
#include <pulchritude-math/math.h>

// reads Tiled PDS loading (mapeditor.org); note that Tiled only natively
//   supports writing JSON/tmx, so a conversion to the internal PDS format is
//   necessary if you are not using a JSON backend for pulchritude-pds.

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorAssetTiled_none,
  PuleErrorAssetTiled_decode,
} PuleErrorAssetTiled;

typedef struct {
  PuleAssetImage spritesheet;
  size_t tileLength;
  PuleU32v2 tileDim;
  size_t spritesheetStartingGlobalId;
} PuleAssetTiledTileset;

typedef enum {
  PuleAssetTiledLayerType_tiles,
  PuleAssetTiledLayerType_objects,
} PuleAssetTiledLayerType;

typedef struct {
  size_t * tileIds;
  size_t tileIdsLength; // = height*width
  PuleU32v2 dim;
} PuleAssetTiledLayerTiles;

typedef struct {
  size_t globalId;
  PuleU32v2 originInPixels;
  PuleStringView name;
  // [TODO] PuleAssetTiledProperty * properties;
  // [TODO] size_t propertiesLength;
} PuleAssetTiledObject;

typedef struct {
  PuleAssetTiledObject * objects;
  size_t objectsLength;
} PuleAssetTiledLayerObjects;

typedef union {
  PuleAssetTiledLayerObjects object;
  PuleAssetTiledLayerTiles tiles;
} PuleAssetTiledLayerTaggedUnion;

typedef struct {
  PuleAssetTiledLayerType dataType;
  PuleAssetTiledLayerTaggedUnion data;
} PuleAssetTiledLayer;

typedef struct {
  PuleU32v2 dimInTiles;
  PuleU32v2 tileDimInPixels;

  PuleAssetTiledLayer * layers;
  size_t layersLength;

  PuleAssetTiledTileset * tilesets;
  size_t tilesetsLength;
} PuleAssetTiledMapInfo;

typedef struct {
  uint64_t id;
} PuleAssetTiledMap;

PULE_exportFn PuleAssetTiledMapInfo puleAssetTiledMapInfo(
  PuleAssetTiledMap const tiledMap
);

typedef struct {
  PuleAllocator allocator;
  PuleStringView baseUrl;
  PuleStreamRead mapSource;
  PuleAssetImageFormat requestedImageFormat;
} PuleAssetTiledMapLoadCreateInfo;

PULE_exportFn PuleAssetTiledMap puleAssetTiledMapLoadFromStream(
  PuleAssetTiledMapLoadCreateInfo const info,
  PuleError * const error
);

typedef struct {
  PuleAllocator allocator;
  PuleStringView url;
  PuleAssetImageFormat requestedImageFormat;
} PuleAssetTiledMapLoadFromFileCreateInfo;
PULE_exportFn PuleAssetTiledMap puleAssetTiledMapLoadFromFile(
  PuleAssetTiledMapLoadFromFileCreateInfo const info,
  PuleError * const error
);

PULE_exportFn void puleAssetTiledMapDestroy(PuleAssetTiledMap const map);

#ifdef __cplusplus
}
#endif
