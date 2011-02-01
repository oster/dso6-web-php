<?php

$capabilityId = $_GET['capability-id'];



if (!isset($capabilityId)) {
	echo "<h2>internal error...</h2>";
	exit();
}

require_once 'includes/So6ServiceFileImpl.inc';

$codebase = "http://localhost:8888/dso6/";
$service = new So6ServiceFileImpl();

$capability = $service->getQueueCapability($capabilityId);
if (! isset($capability)) {
	echo "<h2>internal error...</h2>";
	exit();	
}


$queue = $service->getQueue($capability->getResourceId());

switch ($capability->getRight()) {
	case R_UPDATE:
		$action = "Update";
		$repositoryName = $queue->getName();
		$encoding = $queue->getFileEncoding();
		$mainClass = "fr.loria.ecoo.dso6.core.Update"; //"org.libresource.so6.core.exec.ui.Update";
		$arguments[] = $codebase;
		$arguments[] = $capability->getResourceId();
		$arguments[] = $capabilityId;

		include 'templates/DooSo6.jnlp.tmpl';
		break;
	case R_COMMIT:
		$action = "Commit";
		$repositoryName = $queue->getName();
		$encoding = $queue->getFileEncoding();
		$mainClass = "fr.loria.ecoo.dso6.core.Commit"; //"org.libresource.so6.core.exec.ui.Commit";
		$arguments[] = $codebase;
		$arguments[] = $capability->getResourceId();		
		$arguments[] = $capabilityId;

		include 'templates/DooSo6.jnlp.tmpl';
		break;
	case R_ADMIN:
		break;
	default:
	echo "<h2>internal error...</h2>";
	exit();		
}


?>