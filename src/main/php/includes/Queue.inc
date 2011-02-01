<?php

class Queue {
    var $id; 
    var $name;
    var $description; 
    var $fileEncoding; 
    var $binaryExtensions; 

    var $lastTicket;
    var $patches;

	var $owner; 
	var $email; 

	function __construct() {
		$this->patches = array();
	}

    function getId() {
        return $this->id;
    }

    function setId($id) {
        $this->id = $id;
    }

   function getLastTicket() {
        return $this->lastTicket;
    }

    function setLastTicket($lastTicket) {
        $this->lastTicket = $lastTicket;
    }

    function getPatches() {
		return array_values($this->patches);
    }

    function addPatch($fromTicket, $patchId) {
        $this->patches[$fromTicket] = $patchId;
    }

	function getPatch($fromTicket) {
		return $this->patches[$fromTicket];
	}

    function getName() {
        return $this->name;
    }

    function setName($name) {
        $this->name = $name;
    }

    function getDescription() {
        return $this->description;
    }

    function setDescription($description) {
        $this->description = $description;
    }

    function getBinaryExtensions() {
        return $this->binaryExtensions;
    }

    function setBinaryExtensions($binaryExtensions) {
        $this->binaryExtensions = $binaryExtensions;
    }

    function getFileEncoding() {
        return $this->fileEncoding;
    }

    function setFileEncoding($fileEncoding) {
        $this->fileEncoding = $fileEncoding;
    }

	function getOwner() {
		return $this->owner;
	}

	function setOwner($owner) {
		$this->owner = $owner;
	}

	function getEmail() {
		return $this->email;
	}
	
	function setEmail($email) {
		$this->email = $email;
	}
		
}

?>