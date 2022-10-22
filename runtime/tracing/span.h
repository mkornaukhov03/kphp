// Compiler for PHP (aka KPHP)
// Copyright (c) 2022 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <utility>

#include "runtime/kphp_core.h"
#include "runtime/tracing/tracing.h"

class JsonLogger;

namespace tracing {

class SpanRecorder;

class Span : public ManagedThroughDlAllocator {
  TraceID trace_id{};
  SpanID span_id{};
  string name{};
  std::chrono::high_resolution_clock::time_point start_time{};
  std::chrono::high_resolution_clock::time_point end_time{};
  const Span *parent{nullptr};
  SpanRecorder *span_recorder{nullptr};
public:
  Span(TraceID trace_id, string name, const Span *parent = nullptr);

  static Span *make_span(TraceID trace_id, const string &name, const Span *parent = nullptr, SpanRecorder *span_recorder = nullptr);

  bool is_root() const noexcept {
    // TODO: maintain distributed traces (when root has parent)
    return parent == nullptr;
  }

  Span *start_child(const string &child_name) const noexcept;

  void finish() noexcept;

  friend class ::JsonLogger;
};





} // namespace tracing
