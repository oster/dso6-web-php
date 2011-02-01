<?php

$name = $_POST['repository-title'];
$description = $_POST['repository-description'];
$creator = $_POST['username'];
$creatorEmail = $_POST['email'];



if (!isset($name) || !isset($description) || !isset($creator)) {
/*
	echo "<h2>internal error...</h2>";

	echo "<h3>variables</h3>";
	echo "Name: $name <br/>";
	echo "Description: $description <br/>";
	echo "Creator: $creator <br/>";
	echo "Email: $creatorEmail <br/>";
*/
	exit();
}


require_once 'includes/So6ServiceFileImpl.inc';

$service = new So6ServiceFileImpl();



try {
	$fileEncoding = "UTF-8";
	$binaryExtensions = "png jpg class bin zip pdf ps";
	$qid = $service->createQueue($name, $description, $fileEncoding, $binaryExtensions, $creator, $creatorEmail);
	
	$queueCapabilities = $service->getQueueCapabilities($qid);
	
	$adminCapability = $queueCapabilities[R_ADMIN]->getId();
	$updateCapability = $queueCapabilities[R_UPDATE]->getId();
	$commitCapability = $queueCapabilities[R_COMMIT]->getId();		
	
	include 'templates/RepositoryCreatedPage.tmpl';
		
} catch (Exception $ex) {
	echo "<h2>internal error...</h2>";
	echo "<pre>";
	echo $ex;
	echo "</pre>";
}

?>