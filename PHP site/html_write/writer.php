<?php

//https://something.com/writer.php?access_code=14815162342&in_temp=22.1&in_hum=62.1&out_temp=32.2&out_hum=42.2&pres=1008.33

require_once('./dbaccess.php');

$access_code = mysqli_real_escape_string($db, $_GET['access_code']);
$in_temp = mysqli_real_escape_string($db, $_GET['in_temp']);
$in_hum = mysqli_real_escape_string($db, $_GET['in_hum']);
$out_temp = mysqli_real_escape_string($db, $_GET['out_temp']);
$out_hum = mysqli_real_escape_string($db, $_GET['out_hum']);
$pres = mysqli_real_escape_string($db, $_GET['pres']);

if (empty($access_code) || empty($in_temp) || empty($in_hum) || empty($out_temp) || empty($out_hum) || empty($pres))
{
	echo "<h2>Something is empty!</h2>";
}
else
{
	if ($access_code != $access_code_valid)
	{
		echo "<h2>Invalid access code!</h2>";
	}
	else
	{
		$query = "INSERT INTO readings (in_temp, in_hum, out_temp, out_hum, pres) VALUES (?, ?, ?, ?, ?)";
		$conn = $db->prepare($query);
		$conn->bind_param('ddddd', $in_temp, $in_hum, $out_temp, $out_hum, $pres);

		if ($conn->execute()) 
		{
			echo "<h2>Datas have been sent</h2>";
		}
		else
		{	
			echo "<h2>Something went wrong</h2>";
		}
		$db->close();
	}
}
?>