<?php
class Capability {
	var $id;
	var $right;
	var $resourceId;
	
	function __construct() {
	}

    function getId() {
        return $this->id;
    }

    function setId($id) {
        $this->id = $id;
    }

    function getResourceId() {
        return $this->resourceId;
    }

    function setResourceId($rid) {
        $this->resourceId = $rid;
    }

    function getRight() {
        return $this->right;
    }

    function setRight($right) {
        $this->right = $right;
    }	
}
?>