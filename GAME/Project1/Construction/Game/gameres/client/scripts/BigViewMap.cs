//===========================================================================================================
// 文档说明:大地图操作
// 创建时间:2010-03-05
// 创建人  :thinking
//===========================================================================================================

function OpenBigViewMap()
{
	PlayOpenOrCloseGuiSound();
	if(BigViewMap.isVisible())
	{
		CloseBigViewMap();
		$IsDirty--;
	}
	else
	{
		BigViewMapCtrl.openInit();
		BigViewMap.setVisible(true);
		$IsDirty++;
	}
}

function CloseBigViewMap()
{
	BigViewMap.setVisible(false);
	BigMapMenu_DisplayScene.forceClose();
	BigViewMapCtrl.resetSelectMap();
}

function BigViewMapCtrl::openInit(%this)
{
	//所属大陆
	if(BigMapMenu_DisplayMainland.size() == 0)
	{
		BigMapMenu_DisplayMainland.add("天界",0);
	  BigMapMenu_DisplayMainland.add("河洛东陆",1);
	  BigMapMenu_DisplayMainland.add("轩辕中洲",2);
	  BigMapMenu_DisplayMainland.add("魔界",3);
	  BigMapMenu_DisplayMainland.add("鬼域",4);
	}
	//地图名称
	if(BigMapMenu_SelectDisplayObj.size() == 0)
	{
		BigMapMenu_SelectDisplayObj.add("全部显示",0);
		BigMapMenu_SelectDisplayObj.add("只显玩家",1);
		BigMapMenu_SelectDisplayObj.add("只显示NPC",2);
 	  BigMapMenu_SelectDisplayObj.add("只显示怪物",3);
 	  BigMapMenu_SelectDisplayObj.add("全部不显示",4);
 	  BigMapMenu_SelectDisplayObj.setSelected(0,false);
	}
	
	%this.updateBigMapUI();
}

function BigMapMenu_DisplayMainland::onSelect(%this,%id,%buff)
{
	BigViewMapCtrl.updateBigMapUI(%buff);
}

function BigMapMenu_DisplayScene::onSelect(%this,%id,%buff)
{
	BigViewMapCtrl.setSelectBigMap(%id);
	BigViewMapCtrl.queryBigMapInfo(0);
}

function BigMapMenu_SelectDisplayObj::onSelect(%this,%id,%buff)
{
	switch(%id)
	{
		case 0:
		  viewMapSetDisplayAll(true);
		case 1:
		  viewMapSetDisplayPlayer(true);
		case 2:
		  viewMapSetDisplayNpc(true);
		case 3:
		  viewMapSetDisplayMonster(true);
		case 4:
		  viewMapSetDisplayAll(false);
	}
}

function BigMapMenu_SeekLoadList::onSelect(%this,%id,%buff)
{
	BigViewMapCtrl.queryBigMapInfo(%id);
}

function BigViewMapCtrl::clearQueryInfo()
{
	BigMapMenu_SeekLoadList.clear();
	BigViewMapTree.clear();
}

function BigViewMapTree::onSelect(%this,%obj)
{
	%player = GetPlayer();
	if(%player == 0)
	   return;
	    
	%pos = eval(%this.getItemValue(%obj));
	if(%pos !$= "")
	  %player.setPath(%pos);
}

function OpenFindPathDialog(%mapId,%PosX,%PosY)
{
	%mapName = getViewMapData(%mapId,2);
	SeekLoadText.setContent("<t c='0xffffffff' o='0x4a7977ff'>前往</t><t c='0xffff00ff' o='0x504a23ff'> [" @ %mapName  @"] [" @%PosX@","@%PosY @ "]</t>");
	addFindPathRecordText(%mapId,%PosX @ "," @%PosY,0);
	SeekLoadDialogWnd.setVisible(1);
	$IsDirty++;
	BigViewMap_Xpos.setValue(%PosX);
	BigViewMap_YPos.setValue(%PosY);
}
function OpenFindStridePathDialog(%routeId,%mapId,%npcName)
{
	%mapName = getViewMapData(%mapId,2);
	SeekLoadText.setContent("<t c='0xffffffff' o='0x4a7977ff'>前往</t><t c='0xffff00ff' o='0x504a23ff'> ["@ %mapName @"] ["@%npcName @"]</t>");	 
	addFindPathRecordText(%mapId,%npcName,%routeId);
	SeekLoadDialogWnd.setVisible(1);
	$IsDirty++;    
}

function addFindPathRecordText(%mapId,%text,%routeId)
{
	%mapName = getViewMapData(%mapId,2);
  %time = "[" @ BirdShowTime.getValue() @"]:";
	%content = "";
	%command = "";
		  
	if(%routeId>0 && %routeId<899999999)
	{
		%content = "<t c='0x000000ff' f='宋体' n='12'>" @ %time @ %mapName @ " " @ %text @ "</t>";
		%command = "\"findpath(" @ %routeId @ ");\",\"" @ %content @ "\",\"GuiMissionTraceButtonProfile6\"";
	}
	else
	{
		%tempText = strreplace(%text,","," ");
		%PosX = getWord(%tempText,0);
		%PosY = getWord(%tempText,1);
		
		%text = "[" @%PosX@","@%PosY @ "]";
		%content = "<t c='0x000000ff' f='宋体' n='12'>" @ %time @ %mapName @ " " @ %text @ "</t>";
		%command = "\"BigViewMapCtrl.moveToPos2D("@%PosX@","@%PosY@","@%mapId@");\",\"" @ %content @ "\",\"GuiMissionTraceButtonProfile6\"";
	}
	
	%button = "<c cid='findPathButton' cmd='" @ %command @ "' cf='createButton'/>";
	BigViewMapPathList.AddMsg(15,%button,"","");
}

function closeStrideFindPath()
{
	  CheckTeamFollowState();
	  if(SeekLoadDialogWnd.isVisible())
		{
			SeekLoadDialogWnd.setVisible(0);
			$IsDirty--;
		}
}
function CloseFindPathDialog()
{
	SeekLoadDialogWnd.setVisible(0);
	$IsDirty--;
}

function StartOffFindPath()
{
	%PosX = BigViewMap_Xpos.getValue();
	%PosY = BigViewMap_YPos.getValue();
	//BirdViewMap.PathMark=1;
	BigViewMapCtrl.moveToPos2D(%PosX,%PosY);
}

//function TranSport()
//{
//	%PosX=BigViewMap_Xpos.getValue();
//	%PosY=BigViewMap_YPos.getValue();
//	if(%PosX !$="" && %PosY !$="")
//	{	
//		%Pos=ChangeToTerrPos(%PosX,%PosY);
//		%PosX=getWord(%Pos,0);
//		%PosY=getWord(%Pos,1);
//	  SitPlane(1,GetZoneId(),%PosX,%PosY,115);
//	}
//}

//窗口缩放
function ResizeMap(%vaul)
{
	if(%vaul)
	{
		BigViewMap_Resize.setVisible(true);
	}
	else
	{
		BigViewMap_Resize.setVisible(false);
	  BigViewMapCtrl.setPosition(12,34);
	}
}
//移动地图(800*600)方式下
function moveImageToPosition(%Resize)
{
	%Pos = BigViewMapCtrl.getPosition();
	%PosX = getWord(%Pos,0);
	%posY = getWord(%Pos,1);
	
	switch(%Resize)
	{
		case 1:   //top
			%posY += 25;
			if(%posY > 84)
				BigViewMapCtrl.setPosition(%posX,84);
			else
			  BigViewMapCtrl.setPosition(%posX,%posY);
		case 2:  //bottom
			%posY -= 25;
			if(%posY < -16)
				BigViewMapCtrl.setPosition(%posX,-16);
			else
			  BigViewMapCtrl.setPosition(%posX,%posY);
		case 3:  //left
			%posX += 50;
			if(%posX > 112)
				BigViewMapCtrl.setPosition(112,%posY);
			else
			  BigViewMapCtrl.setPosition(%posX,%posY);
		case 4:  //right
			%posX -= 50;
			if(%posX < -88)
				BigViewMapCtrl.setPosition(-88,%posY);
			else
			  BigViewMapCtrl.setPosition(%posX,%posY);
		default: return;
	}
}
