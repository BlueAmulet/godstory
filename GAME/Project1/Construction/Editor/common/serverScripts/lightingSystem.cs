//-----------------------------------------------
// Copyright ?Synapse Gaming 2004
// Written by John Kabus
//-----------------------------------------------

$lightEditor::lightDBPath = "gameres/data/DATABLOCKS/lights/";
$lightEditor::filterDBPath = $lightEditor::lightDBPath;
//LoadDataBlocks($lightEditor::lightDBPath);
LoadDataBlocks($lightEditor::filterDBPath);

function serverCmdGetLightDBId(%client, %db)
{
   %id = nameToId(%db);
   commandToClient(%client, 'GetLightDBIdCallback', %id);
}



