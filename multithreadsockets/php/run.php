<?php
ini_set("display_errors",1);
error_reporting(E_ALL);

include './socket.php';

$RaspberryPiIP = "127.0.0.1"; // Change by your RaspberryPi/PC IP 


// Read port number from file: /home/pi/multithreadsockets/socket_config.txt
$myfile = fopen("/home/pi/multithreadsockets/socket_config.txt", "r") or die("Unable to open file!");
$portString = fgets($myfile);
fclose($myfile);


$RaspberryPiPORT = (int)$portString; // Change by your RaspberryPi / PC Port Number ..
$connection = new Socket($RaspberryPiIP,$RaspberryPiPORT); // Create a new socet Connection object. 
$connection->init();

////////  Get Command from front end page

$autoMsg = "AUTO";
$manualMsg = "MANL";
$sonMsg = "S_ON";
$soffMsg = "SOFF";
$fonMsg = "F_ON";
$foffMsg = "FOFF";
$data = "";
$operation = $_GET["op"];

if ($operation =="auto")
{
   $command = $autoMsg;
}

if ($operation =="manl")
{
   $command = $manualMsg;
}

if ($operation =="s_on")
{
   $command = $sonMsg;
}

if ($operation =="soff")
{
   $command = $soffMsg;
}

if ($operation =="f_on")
{
   $command = $fonMsg;
}

if ($operation =="foff")
{
   $command = $foffMsg;
}

//// end Get Command from front end page

$connection->open_socket(); // Connect PHP to RaspberryPi or computer.
$connection->send_data($command); //Send command String
$connection->close_socket(); //////////Close connection 
///////////// DONE :)  ///////////////


?>