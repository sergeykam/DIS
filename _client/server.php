<?php

$dis_data = array(
    1 => array(
        's' => 1,
        'g' => 1,
        'u' => 1,
        'v' => 30,
        'd' => 20
    ),
    2 => array(
        's' => 1,
        'g' => 1,
        'u' => 1,
        'v' => 30,
        'd' => 20
    ),
    3 => array(
        's' => 1,
        'g' => 1,
        'u' => 1,
        'v' => 30,
        'd' => 20
    ),
    4 => array(
        's' => 1,
        'g' => 1,
        'u' => 1,
        'v' => 30,
        'd' => 20
    )
);

if(!empty($_GET['a']) && isset($_GET['a'])){
	echo json_encode($dis_data, true);
	
}else{
	print_r($_GET);
}

echo '<pre>';
echo json_encode($dis_data, true);
echo '</pre>';
?>
