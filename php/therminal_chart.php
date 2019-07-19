<?php
// First, we only want to read the file once, sorting through the data
// so lets do it in the beginning and make a data table

// Open file
$fn = fopen("/home/pi/therminal/therminal_log.txt", "r");

// Eat up the first header line
$logLine = fgets($fn);

// Normally we want the latest data from the file, i.e. the 
// very end of the file. Problem is we have to account for 
// shorter files and data gaps. We want to use the date and time
// of each entry, and compare it to the date and time of the last
// (youngest) entry, to find the first entry. Is there any other 
// way than to read the whole file? Not really, we still need to 
// read the file, one line at a time. So lets build a table of the whole file.

$dataTable = array();
$index = 0;

while (!feof($fn))
{
   $logLine = fgets($fn);
   
   // Remove end of line at the end of the line 
   $logLine = str_replace(PHP_EOL, '', $logLine);

   if (!empty($logLine))
   {

      // Latest status is on the form:
      // {pool temp: 06.5};{solar temp 33.9};{filter pump on/off};{solar pump on/off};{manual/auto};{date};{time}
      // Example: "06.2;33.9;on;off;auto;2019-07-14;23:37:45"
    
      $statusArray = explode(";", $logLine);
      $poolTemp = $statusArray[0];
      $solarTemp  = $statusArray[1];
      $filterPump = $statusArray[2];
      $solarPump = $statusArray[3];
      $state = $statusArray[4];
      $date = $statusArray[5];
      $time = $statusArray[6];
 
      // Don't add lines of illegal values
      if (($poolTemp > 0) && ($solarTemp > 0))
      {
         $dataTable[$index] = array();
         $dataTable[$index][0] = $poolTemp;
         $dataTable[$index][1] = $solarTemp;
         $dataTable[$index][2] = $filterPump;
         $dataTable[$index][3] = $solarPump;
         
         if ($state == 'manual')
         {
            $dataTable[$index][4] = 1;
         }
         else if ($state == 'auto')
         {
            $dataTable[$index][4] = 0;
         }
         else
         {
            $dataTable[$index][4] = 2;
         }


         $dateTimeStr = $date . ";" . $time;
         $dataTable[$index][5] = DateTime::createFromFormat("Y-m-d;H:i:s", $dateTimeStr);
         $index++;
      }
   }
}

// Now whole file is in memory
// The latest time/date is in $dataTable[$index][5]
$lastIndex = $index - 1;
$lastSampleTime = $dataTable[$lastIndex][5];
   
// $range is the graph range (and also the steps)
// Possible values are: eightHours, day (24 hours), week, month
// eightHours: every sample, 2400 sample
// day: every minute, every 5 sample, 1440 samples
// week: every 5 minutes, every 25 sample, 2016 samples
// month: every 15 minutes, every 75 sample, 2112 samples

// default range to eightHours
$range = "eightHours";
$interval = new DateInterval("PT8H");

if (isset($_GET["op"]))
{
   $range = $_GET["op"];
}

if ($range == "eightHours")
{
   $numSamples = 2400;
   $n = 1;
   $interval = new DateInterval("PT8H");
}
else if ($range == "day")
{
   $numSamples = 1440;
   $n = 5;
   $interval = new DateInterval("PT24H");
}
else if ($range == "week")
{
   $numSamples = 2016;
   $n = 25;
   $interval = new DateInterval("P7D");
}
else if ($range == "month")
{
   $interval = new DateInterval("P1M");
   $numSamples = 2112;
   $n = 75;
}
else
{
   // Unknown value, lets default to eightHours
   $numSamples = 2400;
   $n = 1;
   $interval = new DateInterval("PT8H");
}

// Now we want to find the index to start the chart from
// This is the sample just after $lastSampleTime - $interval 
$firstTime = date_sub($lastSampleTime, $interval);
$firstIndex = 0;

for ($i = 0; $i <= $lastIndex; $i++)
{
   if ($dataTable[$i][5] > $firstTime)
   {
      $firstIndex = $i;
      break;
   }
}
?>
<html>
  <head>
    <link rel="stylesheet" type="text/css" href="therminal.css" >
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['corechart']});
      google.charts.setOnLoadCallback(drawTemperatureChart);
      google.charts.setOnLoadCallback(drawFunctionChart);

      function drawTemperatureChart() {
        var data = google.visualization.arrayToDataTable([
<?php
   echo("['Time', 'Pool Temp', 'Heater Temp']");

   for ($i = $firstIndex; $i<$lastIndex; )
   {
      echo(",\n");

      echo("[new Date(");
      echo(date_format($dataTable[$i][5], "Y,m,d,H,i,s"));
      echo("),");

      echo($dataTable[$i][0]);
      echo(",");
      echo($dataTable[$i][1]);
      echo("]");

      $i += $n;
   }
   echo("\n");
?>
        ]);

        var options = {
          title: 'Pool temperatures',
          legend: { position: 'bottom' },
          hAxis: { format: '<?php 
   if ($range == "eightHours")
   {
       echo("h:m:s");
   }
   else if ($range == "day")
   {
       echo("h:m");
   }
   else if ($range == "week")
   {
       echo("yy-M-d");
   }
   else if ($range == "month")
   {
       echo("yy-M-d");
   }
?>' },
	  chartArea: {width: '75%', height: '80%'}

        };

        var chart = new google.visualization.LineChart(document.getElementById('temperature_chart'));

        chart.draw(data, options);
      }
      
      
      
      
      
      function drawFunctionChart() {
        var data = google.visualization.arrayToDataTable([
<?php
   echo("['Time', 'Solar Pump (0 = off, 1 = on)', 'Filter Pump (2 = off, 3 = on)', 'State (4 = auto, 5 = manual)' ]");

   for ($i = $firstIndex; $i<$lastIndex; )
   {
      echo(",\n");

      echo("[new Date(");
      echo(date_format($dataTable[$i][5], "Y,m,d,H,i,s"));
      echo("),");

      echo($dataTable[$i][3]);
      echo(",");
      echo($dataTable[$i][2] + 2);
      echo(",");
      echo($dataTable[$i][4] + 4);
      echo("]");

      $i += $n;
   }
   echo("\n");
?>
        ]);

        var options = {
          title: 'Function',
          legend: { position: 'bottom' },
          hAxis: { format: '<?php 
   if ($range == "eightHours")
   {
       echo("h:m:s");
   }
   else if ($range == "day")
   {
       echo("h:m");
   }
   else if ($range == "week")
   {
       echo("yy-M-d");
   }
   else if ($range == "month")
   {
       echo("yy-M-d");
   }
?>' },
          chartArea: {width: '75%', height: '50%'}
        };

        var chart = new google.visualization.LineChart(document.getElementById('function_chart'));

        chart.draw(data, options);
      }
      </script>
  </head>
  <body background="poolwater.jpg">
     <table>
        <tr><div id="temperature_chart" style="width: 97vw; height: 50vh; left: 4vw "></div></tr>
        <tr><div id="function_chart" style="width: 97vw; height: 25vh; left: 4vw"></div></tr>
     </table>

      <a href="therminal_chart.php?op=eightHours"> 
         <div class="divBase chartButtons chartButton1 enabled notSelected">
            <div class="centered">
               <h2>8 Hours</h2>
            </div>
         </div>
      </a> 

      <a href="therminal_chart.php?op=day"> 
         <div class="divBase chartButtons chartButton2 enabled notSelected">
            <div class="centered">
               <h2>Day</h2>
            </div>
         </div>
      </a> 

      <a href="therminal_chart.php?op=week"> 
         <div class="divBase chartButtons chartButton3 enabled notSelected">
            <div class="centered">
               <h2>Week</h2>
            </div>
         </div>
      </a> 

      <a href="therminal_chart.php?op=month"> 
         <div class="divBase chartButtons chartButton4 enabled notSelected">
            <div class="centered">
               <h2>Month</h2>
            </div>
         </div>
      </a> 

      <a href="index.php"> 
         <div class="divBase chartButtons chartButton5 enabled notSelected">
            <div class="centered">
               <h2>Back</h2>
            </div>
         </div>
      </a> 

  </body>
</html>