<?php

ini_set("display_errors",1);
error_reporting(E_ALL);

include './socket.php';

$RaspberryPiIP = "127.0.0.1"; // Change by your RaspberryPi/PC IP 

// Read port number from file: /home/pi/therminal/socket_config.txt
$myfile = fopen("/home/pi/therminal/socket_config.txt", "r") or die("Unable to open file!");
$portString = fgets($myfile);
fclose($myfile);

$RaspberryPiPORT = (int)$portString; // Change by your RaspberryPi / PC Port Number ..
$connection = new Socket($RaspberryPiIP,$RaspberryPiPORT); // Create a new socket object. 
$connection->init();

$connection->open_socket(); // Connect PHP to RaspberryPi or computer.

////////  Get Command from front end page

$autoMsg = "AUTO";
$manualMsg = "MANL";
$sonMsg = "S_ON";
$soffMsg = "SOFF";
$fonMsg = "F_ON";
$foffMsg = "FOFF";
$data = "";

if (isset($_GET["op"]))
{
	
	$operation = $_GET["op"];
	
	if ($operation =="auto")
	{
	   $command = $autoMsg;
	}
	else if ($operation =="manl")
	{
	   $command = $manualMsg;
	}
	else if ($operation =="s_on")
	{
	   $command = $sonMsg;
	}
	else if ($operation =="soff")
	{
	   $command = $soffMsg;
	}
	else if ($operation =="f_on")
	{
	   $command = $fonMsg;
	}
	else if ($operation =="foff")
	{
	   $command = $foffMsg;
	}
   else
   {
      // In this case someone requested a page with an illegal
      // command. Just answer with the status
      $command = "SREQ";
   }
}
else
{
   // If no command is specified we query and display the status
   $command = "SREQ";
}
	
$connection->send_data($command); //Send command String


// TODO: Wait for a response here. A response (like the one answering an SREQ
// command) will always be sent, when the change has taken affect.

// We want the command to take effect before we query the 
// status. Hence a short sleep here
// usleep(2000000);


// Below is replaced with if-statement above
// $command = "SREQ";
// $connection->send_data($command); //Send command String

$therminalStatus = $connection->read_data();

// Latest status is on the form:
// {pool temp: 06.5};{solar temp 33.9};{filter pump on/off};{solar pump on/off};{manual/auto};{date};{time}
// Example: "06.2;33.9;on;off;auto;2019-07-14;23:37:45"

$statusArray = explode(";", $therminalStatus);
$poolTemp = $statusArray[0];
$solarTemp  = $statusArray[1];
$filterPump = $statusArray[2];
$solarPump = $statusArray[3];
$state = $statusArray[4];

$connection->close_socket(); 

?>

<html>
   <head>
      <link rel="stylesheet" type="text/css" href="therminal.css" >
   </head>
   <body background="poolwater.jpg">
      <div class="divBase poolTempSplit enabled notSelected">
        <div class="centered">
          <h2>Pool Temp: <?php echo $poolTemp; ?></h2>
        </div>
      </div>

      <div class="divBase solarTempSplit enabled notSelected">
        <div class="centered">
          <h2>Solar Temp: <?php echo $solarTemp; ?></h2>
        </div>
      </div>

      <a href="?op=auto">  
         <div class="divBase topButtonSplit left enabled <?php if ($state == "manual") echo("notSelected"); else echo("selected"); ?>">
           <div class="centered">
             <h2>Auto</h2>
           </div>
         </div>
      </a>

      <a href="?op=manl"> 
         <div class="divBase topButtonSplit right enabled <?php if ($state == "auto") echo("notSelected"); else echo("selected"); ?>">
           <div class="centered">
             <h2>Manual</h2>
           </div>
         </div>
      </a>

      <a href="?op=<?php if ($filterPump == "1") echo "foff"; else echo "f_on";?>"> 
         <div class="divBase bottomButtonSplit left <?php if ($state == "manual") echo("enabled"); else echo("disabled");?> <?php if ($filterPump == "0") echo("notSelected"); else echo("selected"); ?>">
           <div class="centered">
             <h2>Filter Pump: <?php echo $filterPump; ?></h2>
           </div>
         </div>
      </a>

      <a href="?op=<?php if ($solarPump == "1") echo "soff"; else echo "s_on";?>"> 
         <div class="divBase bottomButtonSplit right <?php if ($state == "manual") echo("enabled"); else echo("disabled");?> <?php if ($solarPump == "0") echo("notSelected"); else echo("selected"); ?>">
           <div class="centered">
             <h2>Solar Pump: <?php echo $solarPump; ?></h2>
           </div>
         </div>
      </a>

      <a href="index.php"> 
         <div class="divBase refreshSplit enabled notSelected">
            <div class="centered">
               <h2>Refresh</h2>
            </div>
         </div>
      </a> 

      <a href="therminal_chart.php"> 
         <div class="divBase chartSplit enabled notSelected">
            <div class="centered">
               <h2>Chart</h2>
            </div>
         </div>
      </a> 

   </body>
</html>


