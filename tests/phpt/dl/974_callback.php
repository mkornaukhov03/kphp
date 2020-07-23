@ok callback
<?php

/**
 * @kphp-required
 */
/**
 * @kphp-required
 * @kphp-infer
 * @param int $x
 * @return int
 */
function sq ($x) {
  return $x * $x;
}

echo "Callback test\n";
$a = array(1,2,3);

array_map ('sq', $a);
var_dump($a);

var_dump (array_map ('sq', $a));

var_dump (array_map (function ($x) {return $x - 5;}, $a));
?>
