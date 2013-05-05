<?php
$result = '';

$string = file_get_contents('index.html');
$string = str_replace("\n", "", $string);
$string = str_replace("\t", "", $string);

$array = str_split($string, 128);

foreach ($array as $value) {
    $result .= sprintf("%s\n", addslashes($value));
}

file_put_contents('compressed.txt', $result);
?>