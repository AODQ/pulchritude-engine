/* auto generated file time */
#pragma once
#include "core.hpp"

#include "time.h"

namespace pule {
struct Timestamp {
  PuleTimestamp _handle;
  inline operator PuleTimestamp() const {
    return _handle;
  }
  static inline Timestamp now() {
    return { ._handle = puleTimestampNow(),};
  }
};
}
namespace pule {
using Nanosecond = PuleNanosecond;
}
namespace pule {
using Microsecond = PuleMicrosecond;
}
namespace pule {
using Millisecond = PuleMillisecond;
}
