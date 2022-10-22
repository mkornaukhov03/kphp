// Compiler for PHP (aka KPHP)
// Copyright (c) 2022 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#include <cstdio>
#include "runtime/tracing/tracing.h"

namespace tracing {

const char *trace_id_to_str(const TraceID &trace_id) {
  static char buf[32 + 1];
  sprintf(buf, "%016lx%016lx", trace_id[0], trace_id[1]);
  return buf;
}

const char *span_id_to_str(SpanID span_id) {
  static char buf[16 + 1];
  sprintf(buf, "%016lx", span_id);
  return buf;
}

IDGenerator::IDGenerator() : e2(rd()) {}

SpanID tracing::IDGenerator::generate_span_id() {
  return dist(e2);
}

TraceID tracing::IDGenerator::generate_trace_id() {
  return TraceID{dist(e2), dist(e2)};
}

} // namespace tracing
