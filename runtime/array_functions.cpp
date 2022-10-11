// Compiler for PHP (aka KPHP)
// Copyright (c) 2020 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#include "runtime/array_functions.h"

template<class FN>
void walk_parts(const char *d, int64_t d_len, const string &str, int64_t limit, FN handle_part) {
  const char *s = str.c_str();
  int64_t s_len = str.size();
  int64_t prev = 0;

  if (d_len == 1) {
    // a faster path for 1-char delimiter (a very frequent case)
    if (limit > 1) {
      char delimiter_char = d[0];
      for (string::size_type i = 0; i < s_len;) {
        if (delimiter_char == s[i]) {
          handle_part(s + prev, static_cast<string::size_type>(i - prev));
          i++;
          prev = i;
          limit--;
          if (limit == 1) {
            break;
          }
        } else {
          i++;
        }
      }
    }
    handle_part(s + prev, static_cast<string::size_type>(s_len - prev));
    return;
  }

  if (limit > 1) {
    for (int64_t i = 0; i < s_len;) {
      int64_t j = 0;
      for (j = 0; j < d_len && d[j] == s[i + j]; j++) {
      }
      if (j == d_len) {
        handle_part(s + prev, static_cast<string::size_type>(i - prev));
        i += d_len;
        prev = i;
        limit--;
        if (limit == 1) {
          break;
        }
      } else {
        i++;
      }
    }
  }
  handle_part(s + prev, static_cast<string::size_type>(s_len - prev));
}

string f$_explode_nth(const string &delimiter, const string &str, int64_t index) {
  if (delimiter.empty()) {
    php_warning("Empty delimiter in function explode");
    return {};
  }
  int result_index = 0;
  string res;
  walk_parts(delimiter.c_str(), delimiter.size(), str, index+2, [&](const char *s, string::size_type l) {
    if (result_index++ == index) {
      res = string(s, l);
    }
  });
  return res;
}

string f$_explode_1(const string &delimiter, const string &str) {
  return f$_explode_nth(delimiter, str, 0);
}

std::tuple<string, string> f$_explode_tuple2(const string &delimiter, const string &str, int64_t mask, int64_t limit) {
  if (delimiter.empty()) {
    php_warning("Empty delimiter in function explode");
    return {};
  }
  int result_index = 0;
  std::tuple<string, string> res;
  walk_parts(delimiter.c_str(), delimiter.size(), str, limit, [&](const char *s, string::size_type l) {
    int index = result_index++;
    if (((1 << index) & mask) == 0) {
      return; // this result index will be discarded, don't allocate the string
    }
    switch (index) {
      case 0: std::get<0>(res) = string(s, l); return;
      case 1: std::get<1>(res) = string(s, l); return;
      default: return;
    }
  });
  return res;
}

std::tuple<string, string, string> f$_explode_tuple3(const string &delimiter, const string &str, int64_t mask, int64_t limit) {
  if (delimiter.empty()) {
    php_warning("Empty delimiter in function explode");
    return {};
  }
  int result_index = 0;
  std::tuple<string, string, string> res;
  walk_parts(delimiter.c_str(), delimiter.size(), str, limit, [&](const char *s, string::size_type l) {
    int index = result_index++;
    if (((1 << index) & mask) == 0) {
      return; // this result index will be discarded, don't allocate the string
    }
    switch (index) {
      case 0: std::get<0>(res) = string(s, l); return;
      case 1: std::get<1>(res) = string(s, l); return;
      case 2: std::get<2>(res) = string(s, l); return;
      default: return;
    }
  });
  return res;
}

std::tuple<string, string, string, string> f$_explode_tuple4(const string &delimiter, const string &str, int64_t mask, int64_t limit) {
  if (delimiter.empty()) {
    php_warning("Empty delimiter in function explode");
    return {};
  }
  int result_index = 0;
  std::tuple<string, string, string, string> res;
  walk_parts(delimiter.c_str(), delimiter.size(), str, limit, [&](const char *s, string::size_type l) {
    int index = result_index++;
    if (((1 << index) & mask) == 0) {
      return; // this result index will be discarded, don't allocate the string
    }
    switch (index) {
      case 0: std::get<0>(res) = string(s, l); return;
      case 1: std::get<1>(res) = string(s, l); return;
      case 2: std::get<2>(res) = string(s, l); return;
      case 3: std::get<3>(res) = string(s, l); return;
      default: return;
    }
  });
  return res;
}

array<string> explode(char delimiter, const string &str, int64_t limit) {
  static char delimiter_storage[1] = {0};
  delimiter_storage[0] = delimiter;
  array<string> res(array_size(limit < 10 ? limit : 1, 0, true));
  walk_parts(delimiter_storage, 1, str, limit, [&](const char *s, string::size_type l) {
    res.push_back(string(s, l));
  });
  return res;
}

array<string> f$explode(const string &delimiter, const string &str, int64_t limit) {
  if (limit < 1) {
    php_warning("Wrong limit %" PRIi64 " specified in function explode", limit);
    limit = 1;
  }
  if (delimiter.empty()) {
    php_warning("Empty delimiter in function explode");
    return {};
  }

  array<string> res(array_size(limit < 10 ? limit : 1, 0, true));
  walk_parts(delimiter.c_str(), delimiter.size(), str, limit, [&](const char *s, string::size_type l) {
    res.push_back(string(s, l));
  });
  return res;
}

array<mixed> range_int(int64_t from, int64_t to, int64_t step) {
  if (from < to) {
    if (step <= 0) {
      php_warning("Wrong parameters from = %" PRIi64 ", to = %" PRIi64 ", step = %" PRIi64 " in function range", from, to, step);
      return {};
    }
    array<mixed> res(array_size((to - from + step) / step, 0, true));
    for (int64_t i = from; i <= to; i += step) {
      res.push_back(i);
    }
    return res;
  } else {
    if (step == 0) {
      php_warning("Wrong parameters from = %" PRIi64 ", to = %" PRIi64 ", step = %" PRIi64 " in function range", from, to, step);
      return {};
    }
    if (step < 0) {
      step = -step;
    }
    array<mixed> res(array_size((from - to + step) / step, 0, true));
    for (int64_t i = from; i >= to; i -= step) {
      res.push_back(i);
    }
    return res;
  }
}

array<mixed> range_string(const string &from_s, const string &to_s, int64_t step) {
  if (from_s.empty() || to_s.empty() || from_s.size() > 1 || to_s.size() > 1) {
    php_warning("Wrong parameters \"%s\" and \"%s\" for function range", from_s.c_str(), to_s.c_str());
    return {};
  }
  if (step != 1) {
    php_critical_error ("unsupported step = %" PRIi64 " in function range", step);
  }
  const int64_t from = static_cast<unsigned char>(from_s[0]);
  const int64_t to = static_cast<unsigned char>(to_s[0]);
  if (from < to) {
    array<mixed> res(array_size(to - from + 1, 0, true));
    for (int64_t i = from; i <= to; i++) {
      res.push_back(f$chr(i));
    }
    return res;
  } else {
    array<mixed> res(array_size(from - to + 1, 0, true));
    for (int64_t i = from; i >= to; i--) {
      res.push_back(f$chr(i));
    }
    return res;
  }
}

array<mixed> f$range(const mixed &from, const mixed &to, int64_t step) {
  if ((from.is_string() && !from.is_numeric()) || (to.is_string() && !to.is_numeric())) {
    return range_string(from.to_string(), to.to_string(), step);
  }
  return range_int(from.to_int(), to.to_int(), step);
}

template<>
array<int64_t> f$array_diff(const array<int64_t> &a1, const array<int64_t> &a2) {
  return array_diff_impl(a1, a2, [](int64_t val) { return val; });
}
