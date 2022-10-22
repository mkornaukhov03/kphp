// Compiler for PHP (aka KPHP)
// Copyright (c) 2022 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#pragma once

#include <array>

#include "runtime/kphp_core.h"
#include "runtime/tracing/span.h"
#include "runtime/refcountable_php_classes.h"

class JsonLogger;

namespace tracing {

class SpanRecorder : public refcountable_php_classes<SpanRecorder> {
  static constexpr size_t MAX_SPANS = 1000;
public:
  SpanRecorder() = default;

  bool record_span(std::unique_ptr<Span> &&span) noexcept;
private:
  std::array<std::unique_ptr<Span>, MAX_SPANS> spans;
  size_t size{0};

  friend class ::JsonLogger;
};

} // namespace tracing
