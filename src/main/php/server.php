<?php

/*
   GET requests
	 /$CID/ticket          returns last queue ticket 
								format: a single integer
	 /$CID/ticket/consumed	returns a set of <userId, lastTicket> which indicates for each userId which ticket he has consumed.
								format: a sequences of lines 'userId ticket'
	 /$CID/patches         returns all patches id 
								format: a sequences of lines 'patchid fromTicket toTicket filesize'
	 /$CID/patches/all        (same as above)
	 /$CID/patches/X       returns patch content starting with ticket X
								format: file content
	 /$CID/patches/X.Y     returns patch content starting with ticket X and ending with Y
								format: file content
   POST requests
     /$CID/patches/X.Y     add a new patch to queue
								multi-part (fromTicket, toTicket, validate, file)
								
	/$CID/ticket/consumed  notify that userId has consumed ticket
								multi-part (userId, ticket)

*/

require_once 'includes/So6ServiceFileImpl.inc';

if (! isset($_GET['capability-id'])) {
	header("HTTP/1.0 404 Not Found - Please provide a capability identifier");
}
$capabilityId = $_GET['capability-id'];

$service = new So6ServiceFileImpl();

switch ($_SERVER['REQUEST_METHOD']) {	
	case "GET":
		// we ignore $_GET parameters
		switch ($_GET['action']) {
			case "ticket":		
				try {
					$capability = $service->getQueueCapability($capabilityId);
					if ($capability->getRight() == R_UPDATE) {
						$queueId = $capability->getResourceId();
						$queue = $service->getQueue($queueId);
						print $queue->getLastTicket();
					}
				} catch (Exception $ex) {
					header("HTTP/1.0 403 Not Found - Please provide a valid capability identifier");		
				}	
				break;
			case "consumed-ticket":
				try {
					$capability = $service->getQueueCapability($capabilityId);
					if ($capability->getRight() == R_READ) {
						$queueId = $capability->getResourceId();
						
						$ticketForUsers = $service->listTicketConsumedByUsers($queueId);
						foreach ($ticketForUsers as $userId => $ticket) {
							print $userId.' '.$ticket."\n";
						}
					}
				} catch (Exception $ex) {
					header("HTTP/1.0 403 Not Found - Please provide a valid capability identifier");		
				}	
				break;
			case "patches":
				$capability = $service->getQueueCapability($capabilityId);
				if ($capability->getRight() == R_UPDATE) {
					$queueId = $capability->getResourceId();
					$queue = $service->getQueue($queueId);
					$patchIds = $queue->getPatches();
					foreach($patchIds as $pid) {
						$patch = $service->getPatch($pid);
						print $patch->getId().' '.$patch->getFromTicket().' '.$patch->getToTicket().' '.$patch->getSize()."\n";
					}
				}
				break;
			
			case "patch":
				$fromTicket = $_GET['fromTicket'];
				if (array_key_exists('toTicket', $_GET) == TRUE) {
					$toTicket = $_GET['toTicket'];
				}
				$capability = $service->getQueueCapability($capabilityId);
				if ($capability->getRight() == R_UPDATE) {
					$queueId = $capability->getResourceId();
					$queue = $service->getQueue($queueId);					
					$pid = $queue->getPatch($fromTicket);
					if (isset($pid)) {
						if (isset($toTicket)) {
							$patch = $service->getPatch($pid);
							if ($toTicket != $patch->getToTicket()) {
								header("HTTP/1.0 403 Not Found - Please provide a valid capability identifier");								
							}
						}
						//header('Content-type: application/pdf');	
						header('Content-Disposition: attachment; filename="'.$pid.'"');
						readfile($service->getPatchDatas($pid));
					} else {
						header("HTTP/1.0 403 Not Found - Please provide a valid capability identifier");														
					}
				} else {
					header("HTTP/1.0 403 Not Found - Please provide a valid capability identifier");														
				}
				break;
			
			default:
				header("HTTP/1.0 403 Not Found - Unknown action");
		}
		
		break;
	case "POST":
		// $_POST parameters should contain multi-part attachments
		
		
		
		switch ($_GET['action']) {
			case "patch":
					$fromTicket = $_POST['fromTicket'];
					$toTicket = $_POST['toTicket'];
					$validate = $_POST['validate'];
					//$tmpdatafile = $_POST['file'];
					$tmpdatafile = $_FILES['patchfile']['tmp_name'];
					$capability = $service->getQueueCapability($capabilityId);
					if ($capability->getRight() == R_COMMIT) {
						$queueId = $capability->getResourceId();
						try {
							$service->createPatch($queueId, $fromTicket, $toTicket, "not provided", $tmpdatafile, $validate);
						} catch (InvalidTicketException $ex) {
							header("HTTP/1.0 404 Not Found - ".$ex->getMessage());																					
						}
					} else {
						header("HTTP/1.0 403 Not Found - Please provide a valid capability identifier <".$capability->getId().">");														
					}					
					break;
					
			case "consumed-ticket":
				$ticket = $_POST['userId'];
				$userId = $_POST['ticket'];
			
   				try {
   					$capability = $service->getQueueCapability($capabilityId);
   					if ($capability->getRight() == R_READ) {
   						$queueId = $capability->getResourceId();
   						$service->setTicketConsumerByUser($queueId, $ticket, $userId));
   					}
   				} catch (Exception $ex) {
   					header("HTTP/1.0 403 Not Found - Please provide a valid capability identifier");		
   				}	
   				break;		
   			
					
			default:
					header("HTTP/1.0 403 Not Found - Unknown action");
		}
		break;
	default:
		header("HTTP/1.0 403 Not Found - Unsupported method");
}
?>