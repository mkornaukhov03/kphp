@kphp_should_fail
/but bool type is passed/
/but int\[\] type is passed/
/but Foo type is passed/
/but string type is passed/
<?php

class Foo {}

function data(): ?int {
    return null;
}

/**
 * @return int|false
 */
function data2() {
    return false;
}

function f() {
    $array = [1];
    $str = "Hello";

    echo $str[true];
    echo $str[$array];
    echo $str[new Foo];
    echo $str["1"];

    echo $str[1];
    echo $str[data()];
    echo $str[data2()];
}

f();
