// Compiler for PHP (aka KPHP)
// Copyright (c) 2022 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#include "compiler/rewrite-rules/rules_runtime.h"

#include "compiler/inferring/public.h"
#include "compiler/type-hint.h"
#include "compiler/data/function-data.h"
#include "compiler/vertex-util.h"

namespace rewrite_rules {

Context &get_context() {
  static thread_local Context ctx;
  return ctx;
}

bool add_to_cache(Context &ctx, VertexPtr v) {
  switch (v->type()) {
    case op_int_const:
      ctx.int_const_cache_.push(v.as<op_int_const>());
      return true;

    case op_string:
      ctx.string_cache_.push(v.as<op_string>());
      return true;

    case op_func_call: {
      auto func_call = v.as<op_func_call>();
      kphp_assert(func_call->extra_type == op_ex_none);
      switch (func_call->args().size()) {
        case 0:
          ctx.func_call0_cache_.push(func_call);
          return true;
        case 1:
          ctx.func_call1_cache_.push(func_call);
          return true;
        case 2:
          ctx.func_call2_cache_.push(func_call);
          return true;
        case 3:
          ctx.func_call3_cache_.push(func_call);
          return true;
      }
      return false;
    }

    default:
      return false;
  }
}

void retire_vertex(Context &ctx, VertexPtr v) {
  add_to_cache(ctx, v);
}

bool is_pure(VertexPtr v) {
  switch (v->type()) {
    case op_var:
    case op_int_const:
    case op_float_const:
    case op_string:
    case op_false:
    case op_true:
    case op_null:
      return true;

    default:
      return false;
  }
}

bool is_same(VertexPtr x, VertexPtr y) {
  if (x->type() != y->type()) {
    return false;
  }

  switch (x->type()) {
    case op_var:
    case op_string:
    case op_int_const:
    case op_float_const:
      return x->get_string() == y->get_string();

    case op_index: {
      auto x_index = x.as<op_index>();
      auto y_index = y.as<op_index>();
      return is_same(x_index->array(), y_index->array()) &&
             is_same(x_index->key(), y_index->key());
    }

    case op_instance_prop: {
      auto x_instance_prop = x.as<op_instance_prop>();
      auto y_instance_prop = y.as<op_instance_prop>();
      return is_same(x_instance_prop->instance(), y_instance_prop->instance()) &&
             x_instance_prop->get_string() == y_instance_prop->get_string();
    }

    case op_false:
    case op_true:
    case op_null:
      return true;

    default:
      return false;
  }
}

static VertexAdaptor<op_var> extract_var(VertexPtr v) {
  switch (v->type()) {
    case op_var:
      return v.as<op_var>();

    case op_index:
      return extract_var(v.as<op_index>()->array());

    case op_instance_prop:
      return extract_var(v.as<op_instance_prop>()->instance());

    case op_conv_array_l:
    case op_conv_int_l:
    case op_conv_string_l:
      return extract_var(v.as<meta_op_unary>()->expr());

    default:
      return {};
  }
}

bool contains(VertexPtr tree, VertexPtr x) {
  if (is_same(tree, x)) {
    return true;
  }
  return std::any_of(tree->begin(), tree->end(), [x](VertexPtr e) {
    return contains(e, x);
  });
}

bool contains_var(VertexPtr tree, VertexPtr var) {
  auto x = extract_var(var);
  if (!x) {
    return false;
  }
  return contains(tree, x);
}

} // namespace rewrite_rules
