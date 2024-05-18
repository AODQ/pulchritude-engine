#include <pulchritude-core/core.h>


#ifdef __cplusplus
extern "C" {
#endif

// a C API that describes a logic-level circuit board,
//   similar to Verilog or VHDL.

typedef struct { uint64_t id; } PuleVhdBoard;

PULE_exportFn PuleVhdBoard puleVhdBoardCreate(
  uint64_t inputs,
  uint64_t outputs
);

typedef struct { uint64_t id; } PuleVhdBlock;

uint64_t puleVhdBlockBoardId(PuleVhdBoard b, PuleVhdBlock block);
uint64_t * puleVhdBlockInputPinIds(PuleVhdBoard b, PuleVhdBlock block);
uint64_t puleVhdBlockInputPinCount(PuleVhdBoard b, PuleVhdBlock block);
uint64_t * puleVhdBlockOutputPinIds(PuleVhdBoard b, PuleVhdBlock block);
uint64_t puleVhdBlockOutputPinCount(PuleVhdBoard b, PuleVhdBlock block);

typedef struct {
  uint64_t id;
} PuleVhdPin;

uint64_t puleVhdPinBlockId(PuleVhdBoard b, PuleVhdPin pin);
uint64_t * puleVhdAttachedPinIds(PuleVhdBoard b, PuleVhdPin pin);
uint64_t puleVhdAttachedPinCount(PuleVhdBoard b, PuleVhdPin pin);

typedef struct PuleVhdBlockCreateInfo {
  PuleVhdBoard board;
  bool negateInput PULE_defaultField(false);
  bool negateOutput PULE_defaultField(false);
  union {
    struct {
      uint64_t inputPins;
    } lgAnd;
    struct {
      uint64_t inputPins;
    } lgOr;
    struct {
      uint64_t inputPins;
    } lgXor;
    struct {
      uint64_t inputPins; // set 0 for default
      uint64_t outputPins; // set 0 for default
      uint64_t boardId;
    } board;
  } block;
} PuleVhdBlockCreateInfo;

PULE_exportFn PuleVhdBlock puleVhdBlockCreate(PuleVhdBlockCreateInfo ci);

#ifdef __cplusplus
}
#endif
