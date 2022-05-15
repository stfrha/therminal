<?php

	ini_set("display_errors",1);
	error_reporting(E_ALL);
	
	echo("Hello world");


    include("Image/Graph.php");
    
    // create the graph
    $Graph =& new Image_Graph(400, 300);
    // create the plotareas
    /// $PlotArea =& $Graph->add(new Image_Graph_PlotArea());
        
    // create the dataset
    /// $DataSet =& new Image_Graph_Dataset_Random(10, 2, 15, true);
    // create the 1st plot as smoothed area chart using the 1st dataset
    /// $Plot =& $PlotArea->addPlot(new Image_Graph_Plot_Line($DataSet));
    
    // set a line color
    /// $Plot->setLineColor(IMAGE_GRAPH_RED);
    
    // add a TrueType font
    /// $Arial =& $Graph->addFont(new Image_Graph_Font_TTF("arial.ttf"));
    // set the font size to 15 pixels
    /// $Arial->setSize(11);
    // add a title using the created font
    /// $Graph->add(new Image_Graph_Title("Simple Line Chart Sample", $Arial));
    
      // output the Graph
    /// $Graph->done();
?>