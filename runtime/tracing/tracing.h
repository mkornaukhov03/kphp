// Compiler for PHP (aka KPHP)
// Copyright (c) 2022 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#pragma once

#include <array>
#include <cstdint>
#include <random>

#include "common/mixin/not_copyable.h"
#include "common/smart_ptrs/singleton.h"
#include "runtime/kphp_core.h"

namespace tracing {

using SpanID = uint64_t;
using TraceID = std::array<uint64_t, 2>;

const char *trace_id_to_str(const TraceID &trace_id);
const char *span_id_to_str(SpanID span_id);

class IDGenerator : vk::not_copyable {
public:
  SpanID generate_span_id();
  TraceID generate_trace_id();
private:
  std::random_device rd;
  std::mt19937_64 e2;
  std::uniform_int_distribution<SpanID> dist;

  IDGenerator();

  friend class vk::singleton<IDGenerator>;
};


} // namespace tracing
