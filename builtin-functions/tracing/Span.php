<?php

namespace Tracing;

class Span {
  public function startChild(string $name) : Span;
  public function finish();
  public function isRoot() : bool;
}
