<!DOCTYPE html>
<html lang="en">
<head>
  <title>Weather Station Vécs</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css">
    
<style>
  .fakeimg {
    height: 200px;
    background: #aaa;
  }
</style>
</head>
<body>

<!--Header-->
<div class="jumbotron text-center" style="margin-bottom:0">
  <h1>Weather Station</h1>
  <p>Vécs</p> 
</div>

<!--Navbar-->

<nav class="navbar navbar-expand-sm bg-dark navbar-dark">
  <a class="navbar-brand" href="https://google.com/">Navbar</a>
  <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#collapsibleNavbar">
    <span class="navbar-toggler-icon"></span>
  </button>
  <div class="collapse navbar-collapse" id="collapsibleNavbar">
    <!--
	<ul class="navbar-nav">
      <li class="nav-item">
        <a class="nav-link" href="#">Link</a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="#">Link</a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="#">Link</a>
      </li>    
    </ul>
	-->
  </div>
</nav>


<div class="container" style="margin-top:30px">
  <div class="row">
		
		<!--Linkbar-->
		<div class="col-sm-4">		  
		  <h3>Linkek</h3>		  
		  <ul class="nav nav-pills flex-column">
			<li class="nav-item">
			  <a class="nav-link" href="https://google.com/">Diagram</a>
			</li>
			<li class="nav-item">
			  <a class="nav-link active" href="https://google.com/raw/">Lista</a>
			</li>			
		  </ul>
		  <hr class="d-sm-none">
		</div>
		
		<!--Content-->
		<div class="col-sm-8">
			<table border="2">
				<tr>
					<td width=200  bgcolor="#F2F5A9">Loggolás dátuma:</td>
					<td width=100  bgcolor="#F2F5A9"><center>in_temp:</center></td>
					<td width=100  bgcolor="#F2F5A9"><center>in_hum:</center></td>
					<td width=100  bgcolor="#F2F5A9"><center>out_temp:</center></td>
					<td width=100  bgcolor="#F2F5A9"><center>out_hum:</center></td>
					<td width=100  bgcolor="#F2F5A9"><center>pres:</center></td>
				</tr>
			</table>

			<table border="1">
				<?php
				require_once('./dbaccess.php');

				if ($db->connect_errno) echo "Failed to connect to MySQL: (" . $db->connect_errno . ") " . $db->connect_error;

				$sql = 'SELECT reading_timestamp, in_temp, in_hum, out_temp, out_hum, pres FROM readings ORDER BY reading_timestamp DESC';
				$result_db = $db->query($sql) or die('Error perform query!');

				while ($r = $result_db->fetch_object())
				{
					echo '<tr>';
					echo '<td width=200>' . $r->reading_timestamp . '</td>';
					echo '<td width=100> <center> ' . $r->in_temp . ' </center> </td>';
					echo '<td width=100> <center> ' . $r->in_hum . ' </center> </td>';
					echo '<td width=100> <center> ' . $r->out_temp . ' </center> </td>';
					echo '<td width=100> <center> ' . $r->out_hum . ' </center> </td>';
					echo '<td width=100> <center> ' . $r->pres . ' </center> </td>';
					echo '</tr>';
				}
				$db->close();
				?>
			</table>
			<hr>
		</div>
  </div>
</div>

<!--Footer-->
<div class="jumbotron text-center" style="margin-bottom:0">
  <p>Thank you for visiting!</p>
</div>
</body>
</html>