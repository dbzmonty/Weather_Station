<!DOCTYPE html>
<html lang="en">
<head>
  <title>Weather Station Vécs</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css">
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js"></script>
  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js"></script>
  <script src="https://canvasjs.com/assets/script/canvasjs.min.js"></script>
    
<?php
	require_once('./dbaccess.php');

	$reading_timestamp = array();	
	$reading_timestamp_year = array();
	$reading_timestamp_month = array();
	$reading_timestamp_day = array();
	$reading_timestamp_hour = array();
	$reading_timestamp_minute = array();
	$reading_timestamp_second = array();
	$in_temp = array();
	$out_temp = array();
	$out_hum = array();
	$pres = array();
	
	if ($db->connect_errno) echo "Failed to connect to MySQL: (" . $db->connect_errno . ") " . $db->connect_error;

	$sql = 'SELECT reading_timestamp, in_temp, in_hum, out_temp, out_hum, pres FROM readings ORDER BY reading_timestamp';
	$result_db = $db->query($sql) or die('Error perform query!');

	while ($r = $result_db->fetch_object())
	{		
		array_push($reading_timestamp, $r->reading_timestamp);
		array_push($in_temp, $r->in_temp);
		array_push($out_temp, $r->out_temp);
		array_push($out_hum, $r->out_hum);
		array_push($pres, $r->pres);
	}
	$db->close();
	
	foreach($reading_timestamp as $read)
	{		
		array_push($reading_timestamp_year, date("Y", strtotime($read)));
		array_push($reading_timestamp_month, date("m", strtotime($read)));
		array_push($reading_timestamp_day, date("d", strtotime($read)));
		array_push($reading_timestamp_hour, date("H", strtotime($read)));
		array_push($reading_timestamp_minute, date("i", strtotime($read)));
		array_push($reading_timestamp_second, date("s", strtotime($read)));		
	}
?>	
	
<script>
window.onload = function () {

    //TempChart
    var TempChart = new CanvasJS.Chart("TempChartContainer", {
        animationEnabled: false,
        title: {
            text: "Indoor and Outdoor temperature (in °C)"
        },
        axisX: {
            valueFormatString: "HH:mm"
        },
        axisY: {
            includeZero: false,
            suffix: " °C"
        },
        legend: {
            cursor: "pointer",
            fontSize: 16
        },
        toolTip: {
            shared: false
        },
        data: [{
            name: "Indoor",
            type: "spline",
			xValueFormatString: "HH:mm",
            yValueFormatString: "#0.## °C",
            showInLegend: true,
            dataPoints: getIndoorTemp()
        },
        {
            name: "Outdoor",
            type: "spline",
			xValueFormatString: "HH:mm",
            yValueFormatString: "#0.## °C",
            showInLegend: true,
            dataPoints: getOutdoorTemp()
        }]
    });
    TempChart.render(); //Rendering

	//AirPressureChart
	var chart = new CanvasJS.Chart("AirPressureChartContainer", {
		animationEnabled: false,  
		title:{
			text: "Air Pressure"
		},
		axisY: {
			includeZero: false,
			suffix: " hPa"
		},
		data: [{
			xValueFormatString: "HH:mm",
			suffix: " hPa",
			type: "spline",
			dataPoints: getAirPressureData()
		}]
	});
	chart.render();	
	
    //HumidityChart
    var HumidityChart = new CanvasJS.Chart("HumidityChartContainer", {
        theme: "light2", // "light1", "light2", "dark1", "dark2"
        animationEnabled: false,
        zoomEnabled: false,
        title: {
            text: "Humidity (in %)"
        },		
        axisY: {            
			includeZero: false,
		},
        data: [{
			xValueFormatString: "HH:mm",
            type: "area",
            dataPoints: getHumidityData()
        }]
    });
    HumidityChart.render(); //Rendering
}

//in_temp
function getIndoorTemp() {
	var dataPoints = [];
	
	var temp_year = <?php echo json_encode($reading_timestamp_year) ?>;
	var temp_month = <?php echo json_encode($reading_timestamp_month) ?>;
	var temp_day = <?php echo json_encode($reading_timestamp_day) ?>;
	var temp_hour = <?php echo json_encode($reading_timestamp_hour) ?>;
	var temp_minute = <?php echo json_encode($reading_timestamp_minute) ?>;
	var temp_second = <?php echo json_encode($reading_timestamp_second) ?>;	
	var temp_in_temp = <?php echo json_encode($in_temp) ?>;
	
	for (var i = 0; i < temp_in_temp.length; i++) { 
		dataPoints.push({ x: new Date(parseInt(temp_year[i]), parseInt(temp_month[i]), parseInt(temp_day[i]), parseInt(temp_hour[i]), parseInt(temp_minute[i])), y: parseFloat(temp_in_temp[i]) });
	}
	
	return dataPoints;
}

//out_temp
function getOutdoorTemp() {
	var dataPoints = [];
	
	var temp_year = <?php echo json_encode($reading_timestamp_year) ?>;
	var temp_month = <?php echo json_encode($reading_timestamp_month) ?>;
	var temp_day = <?php echo json_encode($reading_timestamp_day) ?>;
	var temp_hour = <?php echo json_encode($reading_timestamp_hour) ?>;
	var temp_minute = <?php echo json_encode($reading_timestamp_minute) ?>;
	var temp_second = <?php echo json_encode($reading_timestamp_second) ?>;	
	var temp_out_temp = <?php echo json_encode($out_temp) ?>;
	
	for (var i = 0; i < temp_out_temp.length; i++) { 
		dataPoints.push({ x: new Date(parseInt(temp_year[i]), parseInt(temp_month[i]), parseInt(temp_day[i]), parseInt(temp_hour[i]), parseInt(temp_minute[i])), y: parseFloat(temp_out_temp[i]) });
	}
	
	return dataPoints;
}

//pres
function getAirPressureData() {
	var dataPoints = [];
	
	var temp_year = <?php echo json_encode($reading_timestamp_year) ?>;
	var temp_month = <?php echo json_encode($reading_timestamp_month) ?>;
	var temp_day = <?php echo json_encode($reading_timestamp_day) ?>;
	var temp_hour = <?php echo json_encode($reading_timestamp_hour) ?>;
	var temp_minute = <?php echo json_encode($reading_timestamp_minute) ?>;
	var temp_second = <?php echo json_encode($reading_timestamp_second) ?>;	
	var temp_pres = <?php echo json_encode($pres) ?>;
	
	for (var i = 0; i < temp_pres.length; i++) { 
		dataPoints.push({ x: new Date(parseInt(temp_year[i]), parseInt(temp_month[i]), parseInt(temp_day[i]), parseInt(temp_hour[i]), parseInt(temp_minute[i])), y: parseFloat(temp_pres[i]) });
	}
	
	return dataPoints;
}

//out_hum
function getHumidityData() {
	var dataPoints = [];
	
	var temp_year = <?php echo json_encode($reading_timestamp_year) ?>;
	var temp_month = <?php echo json_encode($reading_timestamp_month) ?>;
	var temp_day = <?php echo json_encode($reading_timestamp_day) ?>;
	var temp_hour = <?php echo json_encode($reading_timestamp_hour) ?>;
	var temp_minute = <?php echo json_encode($reading_timestamp_minute) ?>;
	var temp_second = <?php echo json_encode($reading_timestamp_second) ?>;	
	var temp_out_hum = <?php echo json_encode($out_hum) ?>;
	
	for (var i = 0; i < temp_out_hum.length; i++) { 
		dataPoints.push({ x: new Date(parseInt(temp_year[i]), parseInt(temp_month[i]), parseInt(temp_day[i]), parseInt(temp_hour[i]), parseInt(temp_minute[i])), y: parseFloat(temp_out_hum[i]) });
	}
	
	return dataPoints;
}
</script>
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
  <a class="navbar-brand" href="https://something.com/">Navbar</a>
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
			  <a class="nav-link active" href="https://something.com/">Diagram</a>
			</li>
			<li class="nav-item">
			  <a class="nav-link" href="https://something.com/raw/">Lista</a>
			</li>			
		  </ul>
		  <hr class="d-sm-none">
		</div>
		
		<!--Content-->
		<div class="col-sm-8">
		  
		  <!--Temperature-->
		  <div id="TempChartContainer" style="height: 370px; width: 100%;"></div>		  
		  <hr>
		  <br>		  
		  <!--AirPressure-->
		  <div id="AirPressureChartContainer" style="height: 370px; width: 100%;"></div>
		  <hr>
		  <br>
		  <!--Humidity-->
		  <div id="HumidityChartContainer" style="height: 370px; width: 100%;"></div>
		  <hr>
		  <br>
		</div>
  </div>
</div>

<!--Footer-->
<div class="jumbotron text-center" style="margin-bottom:0">
  <p>Thank you for visiting!</p>
</div>
</body>
</html>