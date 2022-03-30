#pragma once

#include <pulchritude-asset/image.h>

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
  size_t tileWidth;
  size_t tileHeight;
  size_t spritesheetStartingGlobalId;
  size_t rotationDegreesX90; // 0 = 0, 1 = 90, 2 = 180, 3 = 270
  size_t flipX;
  size_t flipY;
  // TODO provide support for individual tile properties/animations
} PuleAssetTiledTileTileset;

typedef struct {
  PuleAssetTiledLayerType_tiles,
  PuleAssetTiledLayerType_objects,
} PuleAssetTiledLayerType;

typedef struct {
  size_t * ids;
  size_t length; // = height*width
  size_t width;
  size_t height;
} PuleAssetTiledLayerTiles;

typedef struct {
  size_t globalId;
  size_t width;
  size_t height;
  PuleAssetTiledProperty * properties;
  size_t propertiesLength;
  PuleStringView name;
} PuleAssetTiledObject;

typedef struct {
  PuleAssetTiledObject objects;
  size_t length;
} PuleAssetTiledLayerObjects;

typedef union {
  PuleAssetTiledObject object;
  PuleAssetTiledLayerTiles tiles;
} PuleAssetTiledLayerTaggedUnion;

typedef struct {
  size_t * tileIds;
  size_t tileIdsLength; // = height*width
  size_t height;
  size_t width;
  PuleAssetTiledLayerType type;
  PuleAssetTiledLayerTaggedUnion data;
} PuleAssetTiledLayer;

typedef struct {
  size_t heightInTiles;
  size_t widthInTiles;
  size_t tileWidth;
  size_t tileHeight;
} PuleAssetTiledMap;

typedef struct {
} PuleAssetTiled;

PULE_exportFn Pule

#ifdef __cplusplus
}
#endif
