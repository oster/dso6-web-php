<?php

require_once 'So6ServiceFileImpl.php';

echo "== Running Tests (".__FILE__.") ==\n";

$service = new So6ServiceFileImpl();



//$qid = $service->createQueue("So6Doodle", "a VCS a la Doodle", "UTF-8", "png jpg class bin zip pdf ps");

//$qid = "2509795b5c015b061c98763a5c26094b";
//$q = $service->getQueue($qid);

//$pid = $service->createPatch($qid, 1, 10, "no comment", "index.php", true);

//$patch = $service->getPatch($pid);

//var_dump($patch);

//var_dump($service->getPatchDatas($pid));
/*
echo "queueIds = ".
var_dump($service->listQueues());
echo "\n";
echo "pid in queue = ";
var_dump($service->listPatchesInQueue($qid));
echo "\n";
*/


//$service->__loadQueues();
//$service->__saveQueues();
//$service->__clear();



?>