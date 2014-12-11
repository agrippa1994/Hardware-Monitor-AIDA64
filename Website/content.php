<?php
	require("config.php");

	$requestedSite = $_POST["site"];

	function loadSite($url)
	{
		global $config;
		return file_get_contents($config["remoteUrl"] . $url);
	}

	function printSite($url)
	{
		echo loadSite($url);
	}

	if(!isset($requestedSite))
		$requestedSite = $config["defaultSite"];

	printSite($requestedSite);
?>