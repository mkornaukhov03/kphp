// Compiler for PHP (aka KPHP)
// Copyright (c) 2022 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#include "runtime/tracing/php_tracing.h"
#include "runtime/tracing/span.h"

class_instance<C$Tracing$Span> f$Tracing$Span$$startChild(class_instance<C$Tracing$Span> const &v$this, const string &name) noexcept {
  class_instance<C$Tracing$Span> res;
  res.alloc();
  res->span = v$this->span->start_child(name);
  return res;
}

void f$Tracing$Span$$finish(class_instance<C$Tracing$Span> const &v$this) noexcept {
  return v$this.get()->span->finish();
}

bool f$Tracing$Span$$isRoot(class_instance<C$Tracing$Span> const &v$this) noexcept {
  return v$this.get()->span->is_root();
}

class_instance<C$Tracing$Span> f$Tracing$Tracer$$startSpan(const string &name) noexcept {
  class_instance<C$Tracing$Span> res;
  res.alloc();
  res->span_recorder.alloc();
  const auto &trace_id = vk::singleton<tracing::IDGenerator>::get().generate_trace_id();
  res.get()->span = tracing::Span::make_span(trace_id, name, nullptr, res->span_recorder.get());
  return res;
}
