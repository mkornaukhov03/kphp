@ok
<?php

/**
 * @kphp-infer
 * @param any $args
 */
function f(...$args) {
}

f (new Exception());
//  var_dump (new Exception());
