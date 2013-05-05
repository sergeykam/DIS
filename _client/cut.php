<?php
$string = file_get_contents('index.html');
$string = str_replace("\n", "", $string);
$string = str_replace("\t", "", $string);
$string = str_replace("\"", "$", $string);

$string = chunk_split($string, 128, "\n");
$string = str_replace("$", "\"", $string);
$string = addslashes($string);


file_put_contents('index-min.html', $string);
?>