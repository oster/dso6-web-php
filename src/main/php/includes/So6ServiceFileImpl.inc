<?php
require_once 'So6Service.inc';
require_once 'Queue.inc';
require_once 'Patch.inc';
require_once 'ServiceException.inc';
require_once 'InvalidTicketException.inc';
require_once 'Capability.inc';

define("STORAGE_DIRECTORY", './so6-service-store');
define("PATCHES_SUBDIR", STORAGE_DIRECTORY.'/patches');
define("QUEUES_SUBDIR", STORAGE_DIRECTORY.'/queues');
define("QUEUES_FILE_NAME", STORAGE_DIRECTORY.'/queues.ser');
define("CAPABILITIES_FILE_NAME", STORAGE_DIRECTORY.'/capabilities.ser');
define("CODEBASE_URI", "http://localhost:8888/dso6/");

define("R_ADMIN", 7);
define("R_UPDATE", 9);
define("R_COMMIT", 13);



class So6ServiceFileImpl implements So6Service {
	
	var $queuesId;
	var $queues = array(); // lazy-loading
	var $patches = array(); // lazy-loading

	var $capabilities;
	 	
	function __construct() {
		if ((! file_exists(STORAGE_DIRECTORY)) && (! mkdir(STORAGE_DIRECTORY))) {
				throw new ServiceException('could not create provided storage directory <'.STORAGE_DIRECTORY.'>');				
		} else if (! is_dir(STORAGE_DIRECTORY)) {
				throw new ServiceException('provided storage directory (<'.STORAGE_DIRECTORY.'>) is not a directory');
		}

		if ((! file_exists(PATCHES_SUBDIR)) && (! mkdir(PATCHES_SUBDIR))) {
				throw new ServiceException('could not create patch storage directory <'.PATCHES_SUBDIR.'>');				
		} else if (! is_dir(PATCHES_SUBDIR)) {
				throw new ServiceException('patch storage directory (<'.PATCHES_SUBDIR.'>) is not a directory');
		}
		
		if ((! file_exists(QUEUES_SUBDIR)) && (! mkdir(QUEUES_SUBDIR))) {
				throw new ServiceException('could not create queues storage directory <'.QUEUES_SUBDIR.'>');				
		} else if (! is_dir(QUEUES_SUBDIR)) {
				throw new ServiceException('queue storage directory (<'.QUEUES_SUBDIR.'>) is not a directory');
		}
		
		try {
            $this->__loadQueuesIds();
        } catch (Exception $ex) {
            throw new ServiceException("unable to reload queues", $ex);
        }	
		try {
            $this->__loadCapabilities();
        } catch (Exception $ex) {
            throw new ServiceException("unable to reload capabilities", $ex);
        }	
	}

    function createQueue($name, $description, $fileEncoding, $binaryExtensions, $owner, $email) {
		$uuid = md5(uniqid(rand(), true)); // might check if queue already exist with this uuid ?
        $q = new Queue();
        $q->setId($uuid);
        $q->setName($name);
        $q->setDescription($description);
        $q->setFileEncoding($fileEncoding);
        $q->setBinaryExtensions($binaryExtensions);
        $q->setLastTicket(0);
        $q->setOwner($owner);
        $q->setEmail($email);

        $this->queuesId[] = $uuid;
        $this->queues[$uuid] = $q;
        
		$adminCapability = $this->__createCapability($uuid, R_ADMIN);
		$updateCapability = $this->__createCapability($uuid, R_UPDATE);
		$commitCapability = $this->__createCapability($uuid, R_COMMIT);
		
		$this->capabilities[$adminCapability->getId()] = $adminCapability;
		$this->capabilities[$updateCapability->getId()] = $updateCapability;
		$this->capabilities[$commitCapability->getId()] = $commitCapability;
		
        try {
            $this->__saveQueuesIds();
			$this->__saveQueue($q);
			$this->__saveCapabilities();
			
			//$queueDir = PATCHES_SUBDIR."/".$q->getId();
			//if ((! file_exists($queueDir)) && (! mkdir($queueDir))) {
	        // 	throw new ServiceException('cannot create queue storage directory for queue <'.$q->getId().'>  queues');
			//}						
        } catch (Exception $ex) {
            throw new ServiceException("unable to save queues or capabilities", $ex);
        }

        return $q->getId();
	}

    function getQueue($queueId) {
		if (! in_array($queueId, $this->queuesId)) {			
            throw new ServiceException("queue with id=[".$queueId."] does not exist");
		}
		if (! array_key_exists($queueId, $this->queues)) { // lazy-loading
			$this->queues[$queueId] = $this->__loadQueue($queueId);
		}
		$q = $this->queues[$queueId]; 
		return $q;
	}
	
    //function deleteQueue($queueId) {
	//	throw new ServiceException('removal of queue is not supported.');
	//	// 1. remove all patches
	//	// 2. remove queue
	//	// 3. remove queueId
	//}

    //function editQueue($queueId, $name, $description, $fileEncoding, $binaryExtensions, $lastTicket) {
	//	throw new ServiceException('edition of queue is not supported.');	
	//}
 
    function createPatch($queueId, $fromTicket, $toTicket, $comments, $tmpdatafile, $validate) {
		$q = $this->getQueue($queueId);
		
		if ($validate && ($fromTicket != ($q->getLastTicket() + 1))) {
            throw new InvalidTicketException("Invalid patch ticket, waiting for ".($q->getLastTicket() + 1)." got ".$fromTicket);
        }

		$uuid = md5(uniqid(rand(), true)); // might check if queue already exist with this uuid ?
        $p = new Patch();
        $p->setId($uuid);
        $p->setQueue($queueId);
        $p->setFromTicket($fromTicket);
        $p->setToTicket($toTicket);
        $p->setComment($comments);

		$p->setCreationTime(date('l jS\, F Y h:i:s A')); // Monday 8th, August 2005 03:12:46 PM

		// TODO: might use file handler
		//if (! file_put_contents(PATCHES_SUBDIR."/".$p->getId().".".$p->fromTicket."-".$p->toTicket, $data)) {
		//	throw new ServiceException('could not save patch data <'.$p->getId().'>');				
		//}
		$patchDataFile = PATCHES_SUBDIR."/".$p->getId().".".$p->fromTicket."-".$p->toTicket;
		
		// disabled 
		if (! move_uploaded_file($tmpdatafile, $patchDataFile)) {
			throw new ServiceException('could not save patch data <'.$p->getId().'>');				
		}

        $p->setSize(filesize($patchDataFile));

		$this->__savePatch($p);

        $this->patches[$uuid] = $p;
        $q->addPatch($fromTicket, $uuid);
        $q->setLastTicket($toTicket);
        $this->__saveQueue($q);

		return $p->getId();		
	}	 
    
	function getPatch($patchId) {
		if (! array_key_exists($patchId, $this->patches)) { // lazy-loading
			$this->patches[$patchId] = $this->__loadPatch($patchId);
		}
		$p = $this->patches[$patchId]; 
		return $p;
	}

    function getPatchDatas($patchId) {
		$p = $this->getPatch($patchId);
		
		// TODO: might use file handler
		//if (! ($d = file_get_contents(PATCHES_SUBDIR."/".$p->getId().".".$p->getFromTicket()."-".$p->getToTicket()))) {
		//	throw new ServiceException('could not load patch data <'.$p->getId().'>');				
		//}
		$patchDataFile = PATCHES_SUBDIR."/".$p->getId().".".$p->fromTicket."-".$p->toTicket;

		return $patchDataFile;
	}

    //function editPatch($patchId, $fromTicket, $toTicket, $comments, $data) {
	//	throw new ServiceException('edition of patch is not supported.');		
	//} 

    function listPatchesInQueue($queueId) {
		$q = $this->getQueue($queueId);
		return $q->getPatches();
	}
	
	function listQueues() {
		return $this->queuesId;
	}


	function getQueueCapability($capabilityId) {
		return $this->capabilities[$capabilityId];
	}

	function getQueueCapabilities($qid) {
		$queueCapabilities = array();
		foreach($this->capabilities as $capability) {

			if ($capability->getResourceId() == $qid) {
				$queueCapabilities[$capability->getRight()] = $capability;
			}
		}
		return $queueCapabilities;
	}

	function __loadPatch($patchId) {
		if (! file_exists(PATCHES_SUBDIR."/".$patchId)) {
			throw new ServiceException('file storing patch object <'.$patchId.'> does not exist');						
		}
		if (! ($dump = file_get_contents(PATCHES_SUBDIR."/".$patchId))) {
			throw new ServiceException('could not load patch object <'.$patchId.'>');				
		}
	  	$p = unserialize($dump);
		return $p;
	}
	
	function __savePatch($patch) {
	  	$dump = serialize($patch);
		if (! file_put_contents(PATCHES_SUBDIR."/".$patch->getId(), $dump)) {
			throw new ServiceException('could not save patch object <'.$patch->getId().'>');				
		}
	}
	
	function __loadQueue($queueId) {
		if (! file_exists(QUEUES_SUBDIR."/".$queueId)) {
			throw new ServiceException('file storing queue object <'.$queueId.'> does not exist');						
		}
		if (! ($dump = file_get_contents(QUEUES_SUBDIR."/".$queueId))) {
			throw new ServiceException('could not load queue object <'.$queueId.'>');				
		}
	  	$q = unserialize($dump);
		return $q;
	}

	function __saveQueue($queue) {
	  	$dump = serialize($queue);
		if (! file_put_contents(QUEUES_SUBDIR."/".$queue->getId(), $dump)) {
			throw new ServiceException('could not save queue object <'.$queue->getId().'>');				
		}
	}

	function __loadQueuesIds() {
		if (! file_exists(QUEUES_FILE_NAME)) {
			$this->queuesId = array();	
		} else {		
			if (! ($dump = file_get_contents(QUEUES_FILE_NAME))) {
				throw new ServiceException('could not load queues');				
			}
	  		$this->queuesId = unserialize($dump);
		}
	}
	
	function __saveQueuesIds() {
	  	$dump = serialize($this->queuesId);
		if (! file_put_contents(QUEUES_FILE_NAME, $dump)) {
			throw new ServiceException('could not save queues');				
		}
	}


	function __loadCapabilities() {
		if (! file_exists(CAPABILITIES_FILE_NAME)) {
			$this->capabilities = array();	
		} else {		
			if (! ($dump = file_get_contents(CAPABILITIES_FILE_NAME))) {
				throw new ServiceException('could not load capabilities');				
			}
	  		$this->capabilities = unserialize($dump);
		}
	}
	
	function __saveCapabilities() {
	  	$dump = serialize($this->capabilities);
		if (! file_put_contents(CAPABILITIES_FILE_NAME, $dump)) {
			throw new ServiceException('could not save capabilities');				
		}
	}

	function __createCapability($qid, $right) {
		$uuid = md5(uniqid(rand(), true)); // might check if some capability already exist with this uuid ?		
		$capability = new Capability();
		$capability->setId($uuid);
		$capability->setResourceId($qid);
		$capability->setRight($right);
				
		return $capability;
	}

	function __clear() {
		$this->__rmdirRecursive(STORAGE_DIRECTORY);
	}

	function __rmdirRecursive($path) {
	    $path = rtrim($path, '/').'/';
	    $handle = opendir($path);
	    for (;false !== ($file = readdir($handle));)
	        if($file != "." and $file != ".." ) {
	            $fullpath= $path.$file;
	            if( is_dir($fullpath) ) {
	                $this->__rmdirRecursive($fullpath);
	            } else {
	                unlink($fullpath);
	            }
	    }
	    closedir($handle);
	    rmdir($path);
	}

}

?>