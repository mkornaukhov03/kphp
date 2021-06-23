@kphp_should_fail
/passed nullable Bar value as \$bar argument to nonnull_bar/
<?php

function test(?Foo $a, ?Foo $b) {
  if ($a !== null) {
  }
  if ($b !== null) {
  }
  if ($a->x) {
    nonnull_bar($b->x);
  }
}

function nonnull_bar(Bar $bar) {}

class Foo {
  public ?Bar $x = null;
}

class Bar {}

test(new Foo(), new Foo());
