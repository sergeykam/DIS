<?php
function str_split_unicode($str, $l = 0) {
    if ($l > 0) {
        $ret = array();
        $len = mb_strlen($str, "UTF-8");
        for ($i = 0; $i < $len; $i += $l) {
            $ret[] = mb_substr($str, $i, $l, "UTF-8");
        }
        return $ret;
    }
    return preg_split("//u", $str, -1, PREG_SPLIT_NO_EMPTY);
}


$result = '';

$string = file_get_contents('index.html');
$string = str_replace("\n", "", $string);
$string = str_replace("\t", "", $string);

$array = str_split_unicode($string, 128);

foreach ($array as $value) {
    $result .= sprintf("\"%s\",\n", addslashes($value));
}

if(file_put_contents('compressed.txt', $result)){
    echo 'Done!';
    echo PHP_EOL;
} else {
    echo 'Failed to write file compressed.txt';
    echo PHP_EOL;
}
?>