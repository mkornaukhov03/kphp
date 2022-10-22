// Compiler for PHP (aka KPHP)
// Copyright (c) 2022 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#include "runtime/tracing/span_recorder.h"

namespace tracing {

bool SpanRecorder::record_span(std::unique_ptr<Span> &&span) noexcept {
  if (size < MAX_SPANS) {
    spans[size++] = std::move(span);
    return true;
  } else {
    return false;
  }
}

} // namespace tracing
