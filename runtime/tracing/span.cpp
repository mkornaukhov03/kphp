// Compiler for PHP (aka KPHP)
// Copyright (c) 2022 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#include "runtime/tracing/tracing.h"
#include "runtime/tracing/span.h"
#include "runtime/tracing/span_recorder.h"
#include "server/json-logger.h"

namespace tracing {

Span::Span(TraceID trace_id, string name, const Span *parent)
  : trace_id(std::move(trace_id))
  , span_id(vk::singleton<IDGenerator>::get().generate_span_id())
  , name(std::move(name))
  , start_time(std::chrono::high_resolution_clock::now())
  , parent(parent) {
  if (parent) {
    span_recorder = parent->span_recorder;
  }
}

Span *Span::start_child(const string &child_name) const noexcept {
  return make_span(trace_id, child_name, this);
}

void Span::finish() noexcept {
  end_time = std::chrono::high_resolution_clock::now();
  if (!is_root()) {
    return;
  }
  vk::singleton<JsonLogger>::get().write_trace(span_recorder);
}

Span *Span::make_span(TraceID trace_id, const string &name, const Span *parent, SpanRecorder *span_recorder) {
  auto span = std::make_unique<Span>(trace_id, name, parent);
  if (span_recorder) {
    span->span_recorder = span_recorder;
  }
  Span *span_ptr = span.get();
  bool ok = span->span_recorder->record_span(std::move(span));
  php_assert(ok); // TODO: handle more gracefully
  return span_ptr;
}

} // namespace tracing
