#include <pulchritude-asset/tiled.h>

#include <pulchritude-asset/pds.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {
struct InternalTiledMap {
  PuleAssetTiledMapInfo info;
  std::vector<PuleAssetTiledLayer> layers;
  std::vector<PuleAssetTiledTileset> tilesets;
  std::vector<size_t> tileIds;
  std::vector<PuleAssetTiledObject> objects;

  std::vector<std::string> objectNames;
};
std::unordered_map<uint64_t, InternalTiledMap> maps;
size_t mapsCount = 1;
}

struct FileStream {
  PuleAllocator allocator;
  void * allocation;
  PuleFile file;
  PuleStreamRead stream;

  static FileStream open(
    PuleAllocator const allocator,
    PuleStringView const url,
    PuleError * err
  ) {
    FileStream self;
    self.allocator = allocator;
    self.allocation = (
      puleAllocate(
        allocator,
        PuleAllocateInfo {
          .zeroOut = 0,
          .numBytes = 512,
          .alignment = 0,
        }
      )
    );
    puleLogDebug("opening: '%s'", url.contents);
    self.file = (
      puleFileOpen(
        url.contents, PuleFileDataMode_text, PuleFileOpenMode_read, err
      )
    );
    if (puleErrorConsume(err)) {
      puleDeallocate(self.allocator, self.allocation);
      return { };
    }

    self.stream = (
      puleFileStreamRead(
        self.file,
        PuleArrayViewMutable {
          .data = reinterpret_cast<uint8_t *>(self.allocation),
          .elementStride = 1,
          .elementCount = 512,
        }
      )
    );

    return self;
  }

  static void destroy(FileStream const self) {
    puleDeallocate(self.allocator, self.allocation);
    puleFileClose(self.file);
    puleStreamReadDestroy(self.stream);
  }
};

PuleAssetTiledMap puleAssetTiledMapLoadFromStream(
  PuleAssetTiledMapLoadCreateInfo const info,
  PuleError * error
) {
  PuleDsValue head = (
    puleAssetPdsLoadFromStream(info.allocator, info.mapSource, error)
  );
  if (puleErrorConsume(error)) {
    PULE_error(PuleErrorAssetTiled_decode, "failed to parse tilemap mapSource");
    return { 0 };
  }

  InternalTiledMap tilemap = {};

  PULE_assert(puleDsIsObject(head));
  tilemap.info.dimInTiles = {
    puleDsAsI32(puleDsObjectMember(head, "width")),
    puleDsAsI32(puleDsObjectMember(head, "height")),
  };
  tilemap.info.tileDimInPixels = {
    puleDsAsI32(puleDsObjectMember(head, "tilewidth")),
    puleDsAsI32(puleDsObjectMember(head, "tileheight")),
  };

  PuleDsValueArray tilesets = (
    puleDsAsArray(puleDsObjectMember(head, "tilesets"))
  );

  // tilesets { firstgid: 1, source: "filename" }
  for (size_t tilesetIt = 0; tilesetIt < tilesets.length; ++ tilesetIt) {
    auto const tilesetObj = tilesets.values[tilesetIt];
    auto const tilesetLabel = (
      puleDsAsString(puleDsObjectMember(tilesetObj, "source"))
    );
    auto const tilesetAbsFilepath = (
      std::string{info.baseUrl.contents} + std::string{tilesetLabel.contents}
    );

    auto tilesetSource = (
      FileStream::open(
        info.allocator,
        puleStringViewCStr(tilesetAbsFilepath.c_str()),
        error
      )
    );
    if (puleErrorConsume(error)) {
      PULE_error(
        PuleErrorAssetTiled_decode,
        "failed to open tileset: %s", tilesetLabel
      );
      puleDsDestroy(head);
      return { 0 };
    }
    PuleDsValue tilesetHead = (
      puleAssetPdsLoadFromStream(info.allocator, tilesetSource.stream, error)
    );
    if (puleErrorConsume(error)) {
      PULE_error(
        PuleErrorAssetTiled_decode,
        "failed to parse tilesethead: %s", puleDsAsString(tilesetObj)
      );
      FileStream::destroy(tilesetSource);
      puleDsDestroy(tilesetHead);
      puleDsDestroy(head);
      return { 0 };
    }

    PuleAssetImage image;
    { // load image
      auto const imageUrl = (
        puleDsAsString(puleDsObjectMember(tilesetHead, "image"))
      );
      auto const imageAbsFilepath = (
        std::string{info.baseUrl.contents} + std::string{imageUrl.contents}
      );
      auto const spritesheetSource = (
        FileStream::open(
          info.allocator,
          puleStringViewCStr(imageAbsFilepath.c_str()),
          error
        )
      );
      if (puleErrorConsume(error)) {
        PULE_error(
          PuleErrorAssetTiled_decode,
          "failed to open image: %s", imageUrl
        );
        FileStream::destroy(tilesetSource);
        puleDsDestroy(tilesetHead);
        puleDsDestroy(head);
        return { 0 };
      }
      image = (
        puleAssetImageLoadFromStream(
          info.allocator,
          spritesheetSource.stream,
          info.requestedImageFormat,
          error
        )
      );
      FileStream::destroy(spritesheetSource);

      if (puleErrorConsume(error)) {
        PULE_error(
          PuleErrorAssetTiled_decode,
          "failed to read image: %s", imageUrl
        );
        puleAssetImageDestroy(image);
        FileStream::destroy(tilesetSource);
        puleDsDestroy(tilesetHead);
        puleDsDestroy(head);
        return { 0 };
      }
    }

    tilemap.tilesets.emplace_back( PuleAssetTiledTileset {
      .spritesheet = image,
      .tileLength = puleDsAsUSize(puleDsObjectMember(tilesetHead, "tilecount")),
      .tileDim = {
        .x = puleDsAsI32(puleDsObjectMember(tilesetHead, "tilewidth")),
        .y = puleDsAsI32(puleDsObjectMember(tilesetHead, "tileheight")),
      },
      .spritesheetStartingGlobalId = (
        puleDsAsUSize(puleDsObjectMember(tilesetObj, "firstgid"))
      ),
    });

    FileStream::destroy(tilesetSource);
    puleDsDestroy(tilesetHead);
  }

  // layers
  PuleDsValueArray layers = (
    puleDsAsArray(puleDsObjectMember(head, "layers"))
  );
  for (size_t layerIt = 0; layerIt < layers.length; ++ layerIt) {
    auto const layerObj = layers.values[layerIt];
    auto const layerTypePuleStrView = (
      puleDsAsString(puleDsObjectMember(layerObj, "type"))
    );
    auto const layerTypeView = (
      std::string_view(layerTypePuleStrView.contents, layerTypePuleStrView.len)
    );

    // get layer type
    PuleAssetTiledLayerType layerType;
    if (layerTypeView == "tilelayer") {
      layerType = PuleAssetTiledLayerType_tiles;
    } else if (layerTypeView == "objectgroup") {
      layerType = PuleAssetTiledLayerType_objects;
    } else {
      PULE_error(
        PuleErrorAssetTiled_decode,
        "unknown layer type: %s", layerTypePuleStrView.contents
      );
      return { 0 };
    }

    // get object/tiles
    PuleAssetTiledLayerTaggedUnion layerUnion;

    puleLogDebug(
      "loading object layer: %s",
      puleDsAsString(puleDsObjectMember(layerObj, "name")).contents
    );

    switch (layerType) {
      case PuleAssetTiledLayerType_tiles: {
        // tilemap ids; push into global array and then have this layer
        //   reference a slice of that array
        PuleDsValueArray ids = (
          puleDsAsArray(puleDsObjectMember(layerObj, "data"))
        );
        size_t const relativeOffset = tilemap.tileIds.size();
        for (size_t idIt = 0; idIt < ids.length; ++ idIt) {
          tilemap.tileIds.emplace_back(puleDsAsI64(ids.values[idIt]));
        }

        layerUnion.tiles = {
          .tileIds = tilemap.tileIds.data() + relativeOffset,
          .tileIdsLength = ids.length,
          .dim = PuleU32v2 {
            .x = puleDsAsI32(puleDsObjectMember(layerObj, "width")),
            .y = puleDsAsI32(puleDsObjectMember(layerObj, "height")),
          },
        };
      } break;
      case PuleAssetTiledLayerType_objects: {
        PuleDsValueArray objects = (
          puleDsAsArray(puleDsObjectMember(layerObj, "objects"))
        );
        size_t const relativeOffset = tilemap.objects.size();
        for (size_t objectIt = 0; objectIt < objects.length; ++ objectIt) {
          auto const object = objects.values[objectIt];
          tilemap.objectNames.emplace_back(
            puleDsAsString(puleDsObjectMember(object, "name")).contents
          );
          tilemap.objects.emplace_back( PuleAssetTiledObject {
            .globalId = puleDsAsUSize(puleDsObjectMember(object, "gid")),
            .originInPixels = {
              .x = puleDsAsI32(puleDsObjectMember(object, "x")),
              .y = puleDsAsI32(puleDsObjectMember(object, "y")),
            },
            .name = {
              .contents = tilemap.objectNames.back().c_str(),
              .len = tilemap.objectNames.back().size()
            },
          });
        }

        layerUnion.object = {
          .objects = tilemap.objects.data() + relativeOffset,
          .objectsLength = objects.length,
        };
      } break;
    }

    tilemap.layers.emplace_back( PuleAssetTiledLayer {
      .dataType = layerType,
      .data = layerUnion,
    });
  }

  tilemap.info.layers = tilemap.layers.data();
  tilemap.info.layersLength = tilemap.layers.size();
  tilemap.info.tilesets = tilemap.tilesets.data();
  tilemap.info.tilesetsLength = tilemap.tilesets.size();

  puleDsDestroy(head);
  maps.emplace(mapsCount, tilemap);
  return { mapsCount ++ };
}

PuleAssetTiledMap puleAssetTiledMapLoadFromFile(
  PuleAssetTiledMapLoadFromFileCreateInfo const info,
  PuleError * const error
) {
  FileStream fileStream = FileStream::open(info.allocator, info.url, error);
  if (puleErrorConsume(error)) {
    PULE_error(
      PuleErrorAssetTiled_decode,
      "failed to open tiled map '%s'", info.url
    );
    return { 0 };
  }

  std::string baseUrl = std::string { info.url.contents };
  baseUrl = baseUrl.substr(0, baseUrl.find_last_of("\\/")) + "/";

  PuleAssetTiledMap tiledMap = (
    puleAssetTiledMapLoadFromStream(
      PuleAssetTiledMapLoadCreateInfo {
        .allocator = info.allocator,
        .baseUrl = puleStringViewCStr(baseUrl.c_str()),
        .mapSource = fileStream.stream,
        .requestedImageFormat = info.requestedImageFormat,
      },
      error
    )
  );

  FileStream::destroy(fileStream);

  return tiledMap;
}

void puleAssetTiledMapDestroy(PuleAssetTiledMap const map) {
  if (map.id == 0) { return; }

  maps.erase(map.id);
}
PuleAssetTiledMapInfo puleAssetTiledMapInfo(
  PuleAssetTiledMap const tiledMap
) {
  auto const ptr = maps.find(tiledMap.id);
  if (ptr == maps.end()) {
    puleLogError("invalid tiled map %u", tiledMap.id);
    return {};
  }
  return ptr->second.info;
}

