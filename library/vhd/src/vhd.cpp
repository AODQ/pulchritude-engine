#include <pulchritude-vhd/vhd.h>

#include <pulchritude-log/log.h>
#include <pulchritude-string/string.h>

#include <vector>

namespace pint {

struct Block {
  PuleVhdBlockCreateInfo ci;

  std::vector<PuleVhdPin> inputPins;
  std::vector<PuleVhdPin> outputPins;
};

struct Pin {
  PuleVhdBlock block; // 0 if attached to board
  std::vector<PuleVhdPin> attachedPins;
};

struct Board {
  pule::ResourceContainer<pint::Pin, PuleVhdPin> pins;
  pule::ResourceContainer<pint::Block, PuleVhdBlock> blocks;
};

pule::ResourceContainer<pint::Board, PuleVhdBoard> boards;

} // namespace pint

PuleVhdBoard puleVhdBoardCreate(uint64_t inputs, uint64_t outputs) {
  PuleVhdBoard board = pint::boards.create({});
  auto & b = *pint::boards.at(board);
  for (uint64_t i = 0; i < inputs; ++ i) {
    b.pins.create(pint::Pin {.block = 0, .attachedPins = {},});
  }
  for (uint64_t i = 0; i < outputs; ++ i) {
    b.pins.create(pint::Pin { .block = 0, .attachedPins = {}, });
  }
  return board;
}

uint64_t puleVhdBlockBoardId(PuleVhdBoard b, PuleVhdBlock block) {
  return pint::boards.at(b)->blocks.at(block)->ci.board.id;
}
uint64_t * puleVhdBlockInputPinIds(PuleVhdBoard b, PuleVhdBlock block) {
  return pint::boards.at(b)->blocks.at(block)->
}
