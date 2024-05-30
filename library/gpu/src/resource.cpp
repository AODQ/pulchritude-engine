#include <pulchritude/gpu.h>

#include <util.hpp>

#include <pulchritude/log.h>

#include <volk.h>

#include <unordered_map>

//------------------------------------------------------------------------------
//-- image chain ---------------------------------------------------------------
//------------------------------------------------------------------------------

extern "C" {

PuleGpuImageChain puleGpuImageChain_create(
  PuleAllocator const allocator,
  PuleStringView const label,
  PuleGpuImageCreateInfo const createInfo
) {
  auto imageChain = util::ImageChain {
    .chain = {},
    .allocator = allocator
  };
  // create the chain of images
  for (size_t it = 0; it < util::ctx().swapchainImages.size(); ++ it) {
    std::string const imageLabelContent = (
      std::string(label.contents, label.len)
    );
    std::string const imageLabel = (
      imageLabelContent + "-frame-" + std::to_string(it)
    );
    PuleGpuImageCreateInfo mutCreateInfo = createInfo;
    mutCreateInfo.label = puleCStr(imageLabel.c_str());
    imageChain.chain.emplace_back(util::ImageChainImage{
      .image = puleGpuImageCreate(mutCreateInfo),
      .label = imageLabel,
      .frameIdx = it,
    });
  }
  return { .id = util::ctx().imageChains.create(imageChain), };
}

void puleGpuImageChain_destroy(PuleGpuImageChain const imageChain) {
  util::ctx().imageChains.destroy(imageChain.id);
}

PuleGpuImage puleGpuImageChain_current(
  PuleGpuImageChain const imageChain
) {
  auto & chain = util::ctx().imageChains.at(imageChain.id)->chain;
  return chain.at(util::ctx().frameIdx % chain.size()).image;
}

} // C

//------------------------------------------------------------------------------
//-- image reference -----------------------------------------------------------
//------------------------------------------------------------------------------

namespace pulint {

PuleGpuImageReference imageReferenceCreate(
  util::ImageReferenceType const type,
  uint64_t const handle
) {
  // check if the reference already exists for this image chain
  PULE_assert((size_t)type < (size_t)util::ImageReferenceType::size);
  auto & imageHandleToReference = (
    util::ctx().imageHandleToReference[(size_t)type]
  );
  auto ptr = imageHandleToReference.find(handle);
  if (ptr != imageHandleToReference.end()) {
    util::ctx().imageReferences.at(ptr->second)->ownerCount += 1;
    return { .id = ptr->second, };
  }

  // create the image reference
  auto imageReference = util::ImageReference {
    .image = { .id = 0, },
    .type = type,
    .ownerCount = 1,
  };
  switch (type) {
    default: PULE_assert(false && "unknown image reference type");
    case util::ImageReferenceType::imageChain:
      imageReference.image = { .id = handle, };
    break;
    case util::ImageReferenceType::image:
      imageReference.imageChain = { .id = handle, };
    break;
  }
  uint64_t const imageReferenceHandle = (
    util::ctx().imageReferences.create(imageReference)
  );
  imageHandleToReference.emplace(handle, imageReferenceHandle);
  return { .id = imageReferenceHandle, };
}

void imageReferenceUpdate(
  util::ImageReferenceType const type,
  uint64_t const baseHandle,
  uint64_t const newHandle
) {
  // check if the reference already exists for this image chain
  auto & imageHandleToReference = (
    util::ctx().imageHandleToReference[(size_t)type]
  );
  auto imageReferenceHandle = imageHandleToReference.at(baseHandle);
  auto & imageReference = *util::ctx().imageReferences.at(imageReferenceHandle);
  switch (type) {
    default: PULE_assert(false && "unknown image reference type");
    case util::ImageReferenceType::imageChain:
      imageReference.image = { .id = newHandle, };
    break;
    case util::ImageReferenceType::image:
      imageReference.imageChain = { .id = newHandle, };
    break;
  }
}

} // pulint

extern "C" {

PuleGpuImageReference puleGpuImageReference_createImageChain(
  PuleGpuImageChain const imageChain
) {
  return (
    pulint::imageReferenceCreate(
      util::ImageReferenceType::imageChain,
      imageChain.id
    )
  );
}

PuleGpuImageReference puleGpuImageReference_createImage(
  PuleGpuImage const image
) {
  return (
    pulint::imageReferenceCreate(util::ImageReferenceType::image, image.id)
  );
}

void puleGpuImageReference_updateImageChain(
  PuleGpuImageChain const baseImageChain,
  PuleGpuImageChain const newImageChain
) {
  pulint::imageReferenceUpdate(
    util::ImageReferenceType::imageChain,
    baseImageChain.id,
    newImageChain.id
  );
}

void puleGpuImageReference_updateImage(
  PuleGpuImage const baseImage,
  PuleGpuImage const newImage
) {
  pulint::imageReferenceUpdate(
    util::ImageReferenceType::image,
    baseImage.id,
    newImage.id
  );
}

void puleGpuImageReference_destroy(
  PuleGpuImageReference const reference
) {
  if (reference.id == 0) { return; }
  auto & imageReference = *util::ctx().imageReferences.at(reference.id);
  // if others still reference image, don't delete
  imageReference.ownerCount -= 1;
  if (imageReference.ownerCount > 0) { return; }
  // destroy image handle to reference relation
  (
    util::ctx()
    .imageHandleToReference[(size_t)imageReference.type]
    .erase(imageReference.imageChain.id)
  );
  // destroy image reference
  util::ctx().imageReferences.destroy(reference.id);
}

PuleGpuImage puleGpuImageReference_image(
  PuleGpuImageReference const reference
) {
  if (reference.id == (size_t)-1) {
    return {
      .id = reinterpret_cast<uint64_t>(
        util::ctx().swapchainImages[util::ctx().swapchainCurrentImageIdx]
      ),
    };
  }
  auto & imageReference = *util::ctx().imageReferences.at(reference.id);
  switch (imageReference.type) {
    default: PULE_assert(false && "unknown image reference type");
    case util::ImageReferenceType::image:
      return imageReference.image;
    case util::ImageReferenceType::imageChain: {
      return (
        puleGpuImageChain_current(imageReference.imageChain)
      );
    }
  }
}

PuleGpuImage puleGpuWindowSwapchainImage() {
  return {
    .id = reinterpret_cast<uint64_t>(
      util::ctx().swapchainImages[util::ctx().swapchainCurrentImageIdx]
    ),
  };
}

PuleGpuImageReference puleGpuWindowSwapchainImageReference() {
  // -1 is the image reference for swapchain
  return { .id = (size_t)-1, };
}

} // C
