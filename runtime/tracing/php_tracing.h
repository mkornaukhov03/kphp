// Compiler for PHP (aka KPHP)
// Copyright (c) 2022 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#pragma once

#include <memory>

#include "common/algorithms/hashes.h"
#include "common/wrappers/string_view.h"
#include "runtime/dummy-visitor-methods.h"
#include "runtime/kphp_core.h"
#include "runtime/refcountable_php_classes.h"
#include "runtime/tracing/span_recorder.h"

namespace tracing {
class Span;
} // namespace tracing

struct C$Tracing$Span: public refcountable_php_classes<C$Tracing$Span>, public DummyVisitorMethods {
  tracing::Span *span{}; // SpanRecorder owns this memory

  // TODO: span_recorder создается в корне, и используется только в корне. Не в корне он == nullptr. Отдельный тип для корня???
  class_instance<tracing::SpanRecorder> span_recorder{}; // To clear the whole span tree on root span destruction

  const char *get_class() const noexcept {
    return R"(Tracing\Span)";
  }

  int get_hash() const  noexcept {
    return static_cast<int32_t>(vk::std_hash(vk::string_view(this->get_class())));
  }

  using DummyVisitorMethods::accept;
};

class_instance<C$Tracing$Span> f$Tracing$Span$$startChild(class_instance<C$Tracing$Span> const &v$this, const string &name) noexcept;
void f$Tracing$Span$$finish(class_instance<C$Tracing$Span> const &v$this) noexcept;
bool f$Tracing$Span$$isRoot(class_instance<C$Tracing$Span> const &v$this) noexcept;

class_instance<C$Tracing$Span> f$Tracing$Tracer$$startSpan(const string &name) noexcept;
