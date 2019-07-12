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
$connection = new Socket($RaspberryPiIP,$RaspberryPiPORT); // Create a new socet Connection object. 
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

$connection->send_data($command); //Send command String

// We want the command to take effect before we query the 
// status. Hence a short sleep here
usleep(2000000);

}

$command = "SREQ";

$connection->send_data($command); //Send command String

$therminalStatus = $connection->read_data();

// Latest status is on the form:
// {pool temp: 06.5},{solar temp 33.9},{filter pump on/off},{solar pump on/off},{manual/auto}
// Example: "06.2,33.9,on,off,auto"

$statusArray = explode(",", $therminalStatus);
$poolTemp = $statusArray[0];
$solarTemp  = $statusArray[1];
$filterPump = $statusArray[2];
$solarPump = $statusArray[3];
$state = $statusArray[4];

$connection->close_socket(); //////////Close connection 
///////////// DONE :)  ///////////////

?>
<html>
<head>Therminal management page!</head>
<body>
<p>Port number: <?php echo $portString; ?></p>
<p>Pool temperature: <?php echo $poolTemp; ?></p>
<p>Solar temperature: <?php echo $solarTemp; ?></p>
<p>Filter pump: <?php echo $filterPump; ?></p>
<p>Solar pump: <?php echo $solarPump; ?></p>
<p>State: <?php echo $state; ?></p>
<table width="800" border="1">
  <tr>
    <td><a href="?op=auto">Auto</a></td>
    <td><a href="?op=manl">Manual</a></td>
    <td><a href="?op=s_on">Solar Pump On</a></td>
    <td><a href="?op=soff">Solar Pump Off</a></td>
    <td><a href="?op=f_on">Filter Pump On</a></td>
    <td><a href="?op=foff">Filter Pump Off</a></td>
    <td><a href="index.php">Refresh</a></td>
  </tr>
</table>
<p>&nbsp;</p>
<p></p>






</body>
</html>
