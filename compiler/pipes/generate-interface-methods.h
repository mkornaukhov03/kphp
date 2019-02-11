#pragma once

#include "compiler/data/data_ptr.h"
#include "compiler/threading/data-stream.h"

class GenerateInterfaceMethods {
public:
  void execute(FunctionPtr function, DataStream<FunctionPtr> &os);
};
