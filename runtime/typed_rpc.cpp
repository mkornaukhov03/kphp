#include "runtime/typed_rpc.h"

#include "common/containers/final_action.h"
#include "common/rpc-const.h"

#include "runtime/resumable.h"
#include "runtime/rpc.h"

namespace {
class_instance<C$VK$TL$RpcResponse> fetch_result(unique_object<RpcRequestResult> result_fetcher, const RpcErrorFactory &error_factory) {
  php_assert(result_fetcher && !result_fetcher->empty());

  auto rpc_error = error_factory.fetch_error_if_possible();
  if (!rpc_error.is_null()) {
    return rpc_error;
  }

  auto resp = result_fetcher->fetch_typed_response();

  rpc_error = error_factory.make_error_from_exception_if_possible();
  if (!rpc_error.is_null()) {
    return rpc_error;
  }

  if (!f$fetch_eof()) {
    php_warning("Not all data fetched");
    return error_factory.make_error("Not all data fetched", TL_ERROR_EXTRA_DATA);
  }

  return resp;
}

class typed_rpc_tl_query_result_one_resumable : public Resumable {
public:
  typed_rpc_tl_query_result_one_resumable(const class_instance<RpcQuery> &query, const RpcErrorFactory &error_factory) :
    query_(query),
    error_factory_(error_factory) {
  }

private:
  using ReturnT = class_instance<C$VK$TL$RpcResponse>;

  bool run() final {
    bool ready = false;

    RESUMABLE_BEGIN
      last_rpc_error_reset();
      ready = f$rpc_get_and_parse(query_.get()->query_id, -1);
      TRY_WAIT(rpc_get_and_parse_resumable_label_0, ready, bool);
      if (!ready) {
        php_assert (last_rpc_error_get());
        query_.get()->result_fetcher.reset();
        RETURN(error_factory_.make_error(last_rpc_error_get(), TL_ERROR_UNKNOWN));
      }

      CurrentProcessingQuery::get().set_current_tl_function(query_);
      auto rpc_result = fetch_result(std::move(query_.get()->result_fetcher), error_factory_);
      CurrentProcessingQuery::get().reset();
      rpc_parse_restore_previous();
      RETURN(rpc_result);
    RESUMABLE_END
  }

  class_instance<RpcQuery> query_;
  const RpcErrorFactory &error_factory_;
};

array<class_instance<C$VK$TL$RpcResponse>> sort_rpc_results(const array<class_instance<C$VK$TL$RpcResponse>> &unsorted_results,
                                                      const array<int> &query_ids,
                                                      const RpcErrorFactory &error_factory) {
  array<class_instance<C$VK$TL$RpcResponse>> rpc_results{unsorted_results.size()};
  for (auto it = query_ids.begin(); it != query_ids.end(); ++it) {
    const int query_id = it.get_value();
    if (!unsorted_results.isset(query_id)) {
      string err = query_id <= 0
                   ? string("Very wrong query_id ")
                   : string("No answer received or duplicate/wrong query_id ");
      rpc_results[it.get_key()] = error_factory.make_error(err.append(query_id), TL_ERROR_WRONG_QUERY_ID);
    } else {
      rpc_results[it.get_key()] = unsorted_results.get_value(query_id);
    }
  }
  return rpc_results;
}

class typed_rpc_tl_query_result_resumable : public Resumable {
public:
  explicit typed_rpc_tl_query_result_resumable(const array<int> &query_ids, const RpcErrorFactory &error_factory) :
    query_ids_(query_ids),
    unsorted_results_(array_size(query_ids_.count(), 0, false)),
    error_factory_(error_factory) {
  }

private:
  using ReturnT = array<class_instance<C$VK$TL$RpcResponse>>;

  bool run() {
    RESUMABLE_BEGIN
      if (query_ids_.count() == 1) {
        query_id_ = query_ids_.begin().get_value();
        unsorted_results_[query_id_.val()] = typed_rpc_tl_query_result_one_impl(query_id_.val(), error_factory_);
        TRY_WAIT(rpc_tl_query_result_resumable_label_0,
                 unsorted_results_[query_id_.val()],
                 class_instance<C$VK$TL$RpcResponse>);
      } else {
        queue_id_ = wait_queue_create(query_ids_);

        while (true) {
          query_id_ = f$wait_queue_next(queue_id_, -1);
          TRY_WAIT(rpc_tl_query_result_resumable_label_1, query_id_, decltype(query_id_));
          if (query_id_.val() <= 0) {
            break;
          }
          unsorted_results_[query_id_.val()] = typed_rpc_tl_query_result_one_impl(query_id_.val(), error_factory_);
          php_assert (resumable_finished);
        }

        unregister_wait_queue(queue_id_);
      }

      auto rpc_results = sort_rpc_results(unsorted_results_, query_ids_, error_factory_);
      RETURN(rpc_results);
    RESUMABLE_END
  }

  const array<int> query_ids_;
  array<class_instance<C$VK$TL$RpcResponse>> unsorted_results_;
  const RpcErrorFactory &error_factory_;
  int queue_id_{-1};
  OrFalse<int> query_id_;
};
} // namespace

int typed_rpc_tl_query_impl(const rpc_connection &connection,
                            const RpcRequest &req,
                            double timeout,
                            bool ignore_answer,
                            bool bytes_estimating,
                            int &bytes_sent,
                            bool flush) {
  f$rpc_clean();
  if (req.empty()) {
    php_warning("Writing rpc query error: query function is null");
    return 0;
  }

  unique_object<RpcRequestResult> stored_fetcher = req.store_request();
  if (!stored_fetcher) {
    return 0;
  }

  if (bytes_estimating) {
    estimate_and_flush_overflow(bytes_sent);
  }
  const int query_id = rpc_send(connection, timeout, ignore_answer);
  if (query_id <= 0) {
    return 0;
  }
  if (flush) {
    f$rpc_flush();
  }
  if (ignore_answer) {
    return -1;
  }

  class_instance<RpcQuery> query;
  query.alloc();
  query.get()->query_id = query_id;
  query.get()->result_fetcher = std::move(stored_fetcher);
  query.get()->tl_function_name = req.tl_function_name();
  RpcPendingQueries::get().save(query);
  return query_id;
}


class_instance<C$VK$TL$RpcResponse> typed_rpc_tl_query_result_one_impl(int query_id,
                                                                 const RpcErrorFactory &error_factory) {
  auto resumable_finalizer = vk::finally([] { resumable_finished = true; });

  if (query_id <= 0) {
    return error_factory.make_error("Rpc query has incorrect query id", TL_ERROR_WRONG_QUERY_ID);
  }

  auto query = RpcPendingQueries::get().withdraw(query_id);
  if (query.is_null()) {
    return error_factory.make_error("Unknown query_id", TL_ERROR_WRONG_QUERY_ID);
  }

  if (!query.get()->result_fetcher || query.get()->result_fetcher->empty()) {
    return error_factory.make_error("Rpc query has empty result fetcher", TL_ERROR_INTERNAL);
  }

  resumable_finalizer.disable();
  return start_resumable<class_instance<C$VK$TL$RpcResponse>>(new typed_rpc_tl_query_result_one_resumable(query, error_factory));
}

array<class_instance<C$VK$TL$RpcResponse>> typed_rpc_tl_query_result_impl(const array<int> &query_ids,
                                                                    const RpcErrorFactory &error_factory) {
  return start_resumable<array<class_instance<C$VK$TL$RpcResponse>>>(new typed_rpc_tl_query_result_resumable(query_ids, error_factory));
}

array<class_instance<C$VK$TL$RpcResponse>> typed_rpc_tl_query_result_synchronously_impl(const array<int> &query_ids,
                                                                                  const RpcErrorFactory &error_factory) {
  array<class_instance<C$VK$TL$RpcResponse>> unsorted_results(array_size(query_ids.count(), 0, false));

  if (query_ids.count() == 1) {
    const int query_id = query_ids.begin().get_value();
    f$wait_synchronously(query_id);
    unsorted_results[query_id] = typed_rpc_tl_query_result_one_impl(query_id, error_factory);
    php_assert (resumable_finished);
  } else {
    const int queue_id = wait_queue_create(query_ids);

    int query_id = f$wait_queue_next_synchronously(queue_id).val();
    for (; query_id > 0; query_id = f$wait_queue_next_synchronously(queue_id).val()) {
      unsorted_results[query_id] = typed_rpc_tl_query_result_one_impl(query_id, error_factory);
      php_assert (resumable_finished);
    }

    unregister_wait_queue(queue_id);
  }

  return sort_rpc_results(unsorted_results, query_ids, error_factory);
}

void free_typed_rpc_lib() {
  CurrentProcessingQuery::get().reset();
  RpcPendingQueries::get().hard_reset();
}