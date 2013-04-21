<?php
$arr = array (
//	rand(1, 12344),
	23534,
	rand(1, 2235),
	rand(1, 5735),
	rand(1, 1033),
	's' => array(
		array(
			'gas' => 1,
			'sensor' => 1,
			'dim' => 1,
			'volt' => 1,
			'status' => 1,
			),
		array(
			'gas' => 2,
			'sensor' => 2,
			'dim' => 2,
			'volt' => 2,
			'status' => 2,
			),
		array(
			'gas' => 3,
			'sensor' => 3,
			'dim' => 3,
			'volt' => 3,
			'status' => 3,
			),
		array(
			'gas' => 4,
			'sensor' => 4,
			'dim' => 4,
			'volt' => 4,
			'status' => 4,
			),
	)	
);

if(!empty($_GET['a']) && isset($_GET['a'])){
	echo json_encode($arr, true);
	
}else{
	print_r($_GET);
}
?>
