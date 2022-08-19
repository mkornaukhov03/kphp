#pragma once

#include <postgresql/libpq-fe.h>

#include "common/kprintf.h"
#include "runtime/allocator.h"
#include "runtime/critical_section.h"
#include "runtime/kphp_core.h"

DECLARE_VERBOSITY(pgsql);

#define LIB_PGSQL_CALL(call) (dl::CriticalSectionGuard{}, call)

namespace database_drivers {

void free_pgsql_lib();

} // namespace database_drivers

struct PGSQL {
  PGSQL() = default;

  void remember_result_status(PGresult *res) {
    // copy error message
    char *result_error = PQresultErrorField(res, PG_DIAG_SQLSTATE);
    sqlstate = result_error ? string{result_error} : string{""};
    last_result_status = LIB_PGSQL_CALL(PQresultStatus(res));
  }

  void reset_result_status() {
    sqlstate = string{""};
    last_result_status = 0;
  }

  PGconn *conn{nullptr};
  string sqlstate{""};
  int last_result_status{0};
};
