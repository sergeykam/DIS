<?php

$dis_data = array(rand(0, 3),1,1,3,30, floatval(20 . '.' . rand(0, 50000)));

if(!empty($_GET['a']) && isset($_GET['a'])){
	echo json_encode($dis_data, true);
	
}else{
	print_r($_GET);
}
?>
