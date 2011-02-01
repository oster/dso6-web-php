<?php

$capabilityId = $_GET['capability-id'];

if (!isset($capabilityId)) {
	echo "<h2>internal error...</h2>";
	exit();
}

require_once 'includes/So6ServiceFileImpl.inc';

$service = new So6ServiceFileImpl();

$capability = $service->getQueueCapability($capabilityId);
if (! isset($capability)) {
	echo "<h2>internal error...</h2>";
	exit();	
}


$queue = $service->getQueue($capability->getResourceId());

switch ($capability->getRight()) {
	case R_UPDATE:
		include 'templates/UpdatePage.tmpl';
		break;
	case R_COMMIT:
		$commitCapabilityId = $capabilityId;
		$updateCapability = $service->getQueueCapabilities($queue->getId());
		$updateCapabilityId = $updateCapability[R_UPDATE]->getId();
		include 'templates/CommitPage.tmpl';
		break;
	case R_ADMIN:
		include 'templates/AdminPage.tmpl';
		break;
	default:
	echo "<h2>internal error...</h2>";
	exit();		
}


?>