<?php

interface So6Service {
	function createQueue($name, $description, $fileEncoding, $binaryExtensions, $owner, $email); // returns QueueId as string

    function getQueue($queueId); // returns Queue

    //function deleteQueue($queueId);

    //function editQueue($queueId, $name, $description, $fileEncoding, $binaryExtensions, $lastTicket);
 
    function createPatch($queueId, $fromTicket, $toTicket, $comments, $tmpdatafile, $validate); // returns PatchId as string

    function getPatch($patchId); // returns Patch

    function getPatchDatas($patchId); // returns data corrsponding to Patch with the related id

    //function editPatch($patchId, $fromTicket, $toTicket, $comments, $data); 

    function listPatchesInQueue($queueId); // returns list of PatchId

	function listQueues(); // returns list of all QueueId

	// awareness method to notify server that a particular user/client have seen/sent patches till a specific ticket from a queue
	// TODO: specify what is a userId ;)
    function setTicketConsumerByUser($queueId, $ticket, $userId); // return void

    function listTicketConsumedByUsers($queueId); // returns array(userId, ticket)
}

?>