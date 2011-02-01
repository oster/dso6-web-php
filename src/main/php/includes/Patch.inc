<?php
class Patch {
	var $id;
	var $fromTicket;
	var $toTicket;
	var $creationTime;
	var $comment;
	var $size;
	var $queueId;

    function getId() {
        return $this->id;
    }

    function setId($id) {
        $this->id = $id;
    }

    function getFromTicket() {
        return $this->fromTicket;
    }

    function setFromTicket($fromTicket) {
        $this->fromTicket = $fromTicket;
    }

    function getToTicket() {
        return $this->toTicket;
    }

    function setToTicket($toTicket) {
        $this->toTicket = $toTicket;
    }

    function getCreationTime() {
        return $this->creationTime;
    }

    function setCreationTime($creationTime) {
        $this->creationTime = $creationTime;
    }

    function getComment() {
        return $this->comment;
    }

    function setComment($comment) {
        $this->comment = $comment;
    }

    function getSize() {
        return $this->size;
    }

    function setSize($size) {
        $this->size = $size;
    }

    function getQueue() {
        return $this->queue;
    }

    function setQueue($queueId) {
        $this->queueId = $queueId;
    }
}


?>