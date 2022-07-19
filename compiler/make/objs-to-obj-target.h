// Compiler for PHP (aka KPHP)
// Copyright (c) 2020 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#pragma once

#include <sstream>

#include "compiler/compiler-settings.h"
#include "compiler/make/target.h"

class Objs2ObjTarget : public Target {
public:
  std::string get_cmd() final {
    std::stringstream ss;
    ss << settings->cxx.get() <<
       " " << settings->cxx_toolchain_option.get() <<
//       " " << "-fuse-ld=lld-11" <<
//       " --target=aarch64-pc-linux" <<
//       " --sysroot=/data/arm_sys_root" <<
       " " << settings->incremental_linker_flags.get() <<
       " -o " << output() <<
       " " << dep_list();
    std::string target = settings->target.get();
    std::string sysroot = settings->sys_root.get();
    if (!target.empty() && !sysroot.empty()) {
      ss << " --target=" << target <<
            " --sysroot=" << sysroot;
    }
    return ss.str();
  }
};
