<?php

namespace Tracing;

require_once __DIR__ . '/Span.php';

class Tracer {
  public static function startSpan(string $name) : Span;
}
