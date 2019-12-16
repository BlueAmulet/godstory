//===========================================================================================================
// 文档说明:仙篆录操作
// 创建时间:2009-12-15
// 创建人: Batcel
// 修改时间:2009-12-29
// 修改人:thinking
//===========================================================================================================
function OpenXianZhuanWnd()
{
	if(!XianZhuanWndGui.IsVisible())
	{
		GameMainWndGui_Layer3.pushToBack(XianZhuanWndGui);
		XianZhuanWndGui.setVisible(1);
		$IsDirty++;
		QueryCallboardEvent();
		ActiveStartCheckBoxGui.setStateOn(getShowActiveStart()); 
    ActNotify_BntCtl.setStateOn(getActivityNotifyStatus());
    CurrentDayCheckBoxGui.setStateOn(getShowCurrentDay());
    CaptainCheckBoxGui.setStateOn(getShowAliveNpc());
		OpenEverydayMovement();
		
	}
	else
	{
		XianZhuanWndGui.setVisible(0);
		$IsDirty--;
	}
}

function CloseXianZhuanWnd()
{
	if(XianZhuanWndGui.IsVisible())
	{
		XianZhuanWndGui.setVisible(0);
		
	  CaptainListBox.clearItems();
	  AllServerListBox.clearItems();
	 
		$IsDirty--;
	}
}

function OpenEverydayMovement()
{
	Daily_Bnt.setStateOn(true);
	EverydayMovementGui.setVisible(1);
	EveryweekMovementGui.setVisible(0);
	DayMovementGui.setVisible(0);
	CaptainMovementGui.setVisible(0);
	AllServerMovementGui.setVisible(0);
	TimeItemGui.setVisible(0);
	
	ActiveStartCheckBoxGui.setVisible(1);
	ActNotify_BntCtl.setVisible(1);   
	CurrentDayCheckBoxGui.setVisible(0);
	CaptainCheckBoxGui.setVisible(0);
}

function OpenEveryweekMovement()
{
	EverydayMovementGui.setVisible(0);
	EveryweekMovementGui.setVisible(1);
	DayMovementGui.setVisible(0);
	CaptainMovementGui.setVisible(0);
	AllServerMovementGui.setVisible(0);
	TimeItemGui.setVisible(0);
	
	ActiveStartCheckBoxGui.setVisible(0);
	ActNotify_BntCtl.setVisible(1); 
	CurrentDayCheckBoxGui.setVisible(1);
	CaptainCheckBoxGui.setVisible(0);
}

function OpenDayMovement()
{
	EverydayMovementGui.setVisible(0);
	EveryweekMovementGui.setVisible(0);
	DayMovementGui.setVisible(1);
	CaptainMovementGui.setVisible(0);
	AllServerMovementGui.setVisible(0);
	TimeItemGui.setVisible(0);
	
	ActiveStartCheckBoxGui.setVisible(0);
	ActNotify_BntCtl.setVisible(1); 
	CurrentDayCheckBoxGui.setVisible(1);
	CaptainCheckBoxGui.setVisible(0);
}

function OpenCaptainMovement()
{
	EverydayMovementGui.setVisible(0);
	EveryweekMovementGui.setVisible(0);
	DayMovementGui.setVisible(0);
	CaptainMovementGui.setVisible(1);
	AllServerMovementGui.setVisible(0);
	TimeItemGui.setVisible(0);
	
	ActiveStartCheckBoxGui.setVisible(0);
	ActNotify_BntCtl.setVisible(0); 
	CurrentDayCheckBoxGui.setVisible(0);
	CaptainCheckBoxGui.setVisible(1);
}
function OpenAllServerMovement()
{
	EverydayMovementGui.setVisible(0);
	EveryweekMovementGui.setVisible(0);
	DayMovementGui.setVisible(0);
	CaptainMovementGui.setVisible(0);
	AllServerMovementGui.setVisible(1);
	TimeItemGui.setVisible(0);
	
	ActiveStartCheckBoxGui.setVisible(0);
	ActNotify_BntCtl.setVisible(0); 
	CurrentDayCheckBoxGui.setVisible(0);
	CaptainCheckBoxGui.setVisible(0);
}

function updateCallboardTime(%weekday, %time)
{
	  %weekName ="";
	  switch(%weekday)
	  {
	  	case 0:
	  		%weekName = "星期日";
	  	case 1:
	  		%weekName = "星期一";
	  	case 2:
	  		%weekName = "星期二";
	  	case 3:
	  		%weekName = "星期三";
	  	case 4:
	  		%weekName = "星期四";
	  	case 5:
	  		%weekName = "星期五";
	    case 6:
	    	%weekName = "星期六";
	  }
	  
	  %text = %weekName SPC %time;
	  CallboardTime_Text.setText(%text);
}
//每日活动
function updateEverydayList(%indexID,%degree,%Name,%startTime,%endTime,%level,%lineName,%isStart)
{
		%player = GetPlayer();
	  if(%player == 0)
	   return;
	   
		%currLevel = %player.getLevel();
		%degreeImage = "";
		%imageSolid  = "<i s='gameres/gui/images/GUIbutton03_1_008.png'/>";
		%imageHollow = "<i s='gameres/gui/images/GUIbutton03_1_007.png'/>";
	
		switch(%degree)
		{
			case 0:
				%degreeImage = %imageHollow @ %imageHollow @ %imageHollow @ %imageHollow @ %imageHollow;
			case 1:
				%degreeImage = %imageSolid @ %imageHollow @ %imageHollow @ %imageHollow @ %imageHollow;
			case 2:
				%degreeImage = %imageSolid @ %imageSolid @ %imageHollow @ %imageHollow @ %imageHollow;
			case 3:
				%degreeImage = %imageSolid @ %imageSolid @ %imageSolid @ %imageHollow @ %imageHollow;
			case 4:
				%degreeImage = %imageSolid @ %imageSolid @ %imageSolid @ %imageSolid @ %imageHollow;
			case 5:
				%degreeImage = %imageSolid @ %imageSolid @ %imageSolid @ %imageSolid @ %imageSolid;
		}
		
		%Color = "<t h='1' c='0xffffffff'>";
		%strName = "<t c='0xffffffff'>" @ %Name @ "</t>";
		%status = "<i s='gameres/gui/images/GUIWindow41_1_002.png'/>";
		
		if(%isStart)
		{
			%Color = "<t h='1' c='0x00ff00ff'>";
			%strName = "<t c='0x00ff00ff'>" @ %Name @ "</t>";
			%status = "<i s='gameres/gui/images/GUIWindow41_1_003.png'/>";
		}
		
		%Time = %Color @ %startTime @ "-" @ %endTime @ "</t>";
		
		if(%currLevel >= %level)
			 %level = %Color @ %level @ "级" @ "</t>";
		else
		   %level = "<t h='1' c='0xff0000ff'>" @ %level @ "级" @ "</t>";
		
		%lineName = %Color @ %lineName @ "</t>";
	  %content = %strName TAB %Time TAB %level TAB %lineName;

		
		%text = %degreeImage TAB %content TAB %status;
	  EverydayListBox.addItem(%text,%indexID);
	  EverydayListBox.setListBoxTagText(%isStart,%indexID);  //设置标记用于更新
	  //设置鼠标事件
	  EverydayListBox.setCommand("EverydayListBox.onMouseDown();",%indexID);
	  EverydayListBox.setAltCommand("EverydayListBox.dblclickMouseDown();",%indexID);
		
}
//每周活动
function updateWeekDayList(%indexID,%degree,%Name,%weekDay,%startTime,%endTime,%level,%lineName,%isStart)
{
	  %player = GetPlayer();
	  if(%player == 0)
	   return;
	    
	  %currLevel =  %player.getLevel();
		%degreeImage = "";
		%imageSolid  = "<i s='gameres/gui/images/GUIbutton03_1_008.png'/>";
		%imageHollow = "<i s='gameres/gui/images/GUIbutton03_1_007.png'/>";
	
		switch(%degree)
		{
			case 0:
				%degreeImage = %imageHollow @ %imageHollow @ %imageHollow @ %imageHollow @ %imageHollow;
			case 1:
				%degreeImage = %imageSolid @ %imageHollow @ %imageHollow @ %imageHollow @ %imageHollow;
			case 2:
				%degreeImage = %imageSolid @ %imageSolid @ %imageHollow @ %imageHollow @ %imageHollow;
			case 3:
				%degreeImage = %imageSolid @ %imageSolid @ %imageSolid @ %imageHollow @ %imageHollow;
			case 4:
				%degreeImage = %imageSolid @ %imageSolid @ %imageSolid @ %imageSolid @ %imageHollow;
			case 5:
				%degreeImage = %imageSolid @ %imageSolid @ %imageSolid @ %imageSolid @ %imageSolid;
		}
		
		%weekName ="";
	  switch(%weekday)
	  {
	  	case 1:
	  		%weekName = "星期一";
	  	case 2:
	  		%weekName = "星期二";
	  	case 3:
	  		%weekName = "星期三";
	  	case 4:
	  		%weekName = "星期四";
	  	case 5:
	  		%weekName = "星期五";
	    case 6:
	    	%weekName = "星期六";
	    case 7:
	  		%weekName = "星期日";
	  }
		
		%Color = "<t h='1' c='0xffffffff'>";
		%strName = "<t c='0xffffffff'>" @ %Name @ "</t>";
		%status = "<i s='gameres/gui/images/GUIWindow41_1_002.png'/>";
		
		if(%isStart)
		{
			%Color = "<t h='1' c='0x00ff00ff'>";
			%strName = "<t c='0x00ff00ff'>" @ %Name @ "</t>";
			%status = "<i s='gameres/gui/images/GUIWindow41_1_003.png'/>";
		}
		
		%weekName = %Color @ %weekName @ "</t>";
		%Time = %Color @ %startTime @ "-" @ %endTime @ "</t>";
		
		if(%currLevel >= %level)
			 %level = %Color @ %level @ "级" @ "</t>";
		else
		   %level = "<t h='1' c='0xff0000ff'>" @ %level @ "级" @ "</t>";
		
		%lineName = %Color @ %lineName @ "</t>";
	  %content = %strName TAB %weekName TAB %Time TAB %level TAB %lineName;

		
		%text = %degreeImage TAB %content TAB %status;
	  EveryweekListBox.addItem(%text,%indexID);
	  EveryweekListBox.setListBoxTagText(%isStart,%indexID);
	  //设置鼠标事件
	  EveryweekListBox.setCommand("EveryweekListBox.onMouseDown();",%indexID);
	  EveryweekListBox.setAltCommand("EveryweekListBox.dblclickMouseDown();",%indexID);
}
//日常活动
function updateDailyList(%indexID,%MissionID,%degree,%Name,%weekDay,%level,%countLoop,%bPrize,%isStart)
{
	  %player = GetPlayer();
	  if(%player == 0)
	    return;
	    
	  %CurrLoop  = %player.GetCycleMissionTimes(%MissionID);  //当前完成次数
	  %currLevel = %player.getLevel();
		%degreeImage = "";
		%imageSolid  = "<i s='gameres/gui/images/GUIbutton03_1_008.png'/>";
		%imageHollow = "<i s='gameres/gui/images/GUIbutton03_1_007.png'/>";
		
	  switch(%degree)
		{
			case 0:
				%degreeImage = %imageHollow @ %imageHollow @ %imageHollow @ %imageHollow @ %imageHollow;
			case 1:
				%degreeImage = %imageSolid @ %imageHollow @ %imageHollow @ %imageHollow @ %imageHollow;
			case 2:
				%degreeImage = %imageSolid @ %imageSolid @ %imageHollow @ %imageHollow @ %imageHollow;
			case 3:
				%degreeImage = %imageSolid @ %imageSolid @ %imageSolid @ %imageHollow @ %imageHollow;
			case 4:
				%degreeImage = %imageSolid @ %imageSolid @ %imageSolid @ %imageSolid @ %imageHollow;
			case 5:
				%degreeImage = %imageSolid @ %imageSolid @ %imageSolid @ %imageSolid @ %imageSolid;
		}
		
		%weekName ="";
	  switch(%weekday)
	  {
	  	case 0:
	  	  %weekName = "每日";
	  	case 1:
	  		%weekName = "星期一";
	  	case 2:
	  		%weekName = "星期二";
	  	case 3:
	  		%weekName = "星期三";
	  	case 4:
	  		%weekName = "星期四";
	  	case 5:
	  		%weekName = "星期五";
	    case 6:
	    	%weekName = "星期六";
	    case 7:
	  		%weekName = "星期日";
	  }
	  
	  %Color = "<t h='1' c='0xffffffff'>";
		%strName = "<t c='0xffffffff'>" @ %Name @ "</t>";
		
		if(%isStart)
		{
			%Color = "<t h='1' c='0x00ff00ff'>";
			%strName = "<t c='0x00ff00ff'>" @ %Name @ "</t>";
		}
		
		%weekName = %Color @ %weekName @ "</t>";
		if(%currLevel >= %level)
			 %level = %Color @ %level @ "级" @ "</t>";
		else
		   %level = "<t h='1' c='0xff0000ff'>" @ %level @ "级" @ "</t>";
		
		%loopNum = %Color @ %CurrLoop @ "/" @ %countLoop @ "</t>";
		
		if(%bPrize)
		  %Prize = %Color @ "有" @ "</t>";
		else
		  %Prize = %Color @ "无" @ "</t>";
		 
	  %content = %strName TAB %weekName TAB %level TAB %loopNum TAB %Prize TAB %MissionID;
		%text = %degreeImage TAB %content;	  
	  DayListBox.addItem(%text,%indexID);
	  DayListBox.setListBoxTagText(%isStart,%indexID);
	  //设置鼠标事件
	  DayListBox.setCommand("DayListBox.onMouseDown();",%indexID);
	  DayListBox.setAltCommand("DayListBox.dblclickMouseDown();",%indexID);
	
}
//首领击杀类
function updateKillNpcList(%indexID,%npcID,%type,%time,%mapName)
{
	  %npcName = getNpcData(%npcID,1);
	  %npcLevel = getNpcData(%npcID,9);
	  
	  %Color = "<t h='1' c='0xffffffff'>";  //RGBA
	  %status = "<t h='1' c='0x808080ff'>" @ "未刷新" @ "</t>";
	  %reliveTime = %Color @ "无" @ "</t>";
	  
	  if(%type)
	  	 %status = %Color @ "存活" @ "</t>";
	  
	  	
	  if(%time>0)
	  {
	     %hour = %time / 3600;
	     %min  = (%time %3600) / 60;
	     if(%min >0)
	     	  %reliveTime = %Color @ %hour @ "点" @ %min @ "分复活" @ "</t>";
	     else
	     	  %reliveTime = %Color @ %hour @ "点复活" @ "</t>";
	  }
	  
	  %indexText = %Color @ %indexID @ "</t>";
	  %mapName = %Color @ %mapName @ "</t>";
	  %npcLevel = %Color @ %npcLevel @ "级" @ "</t>";
	  %content = %indexText TAB %npcName TAB %npcLevel TAB %status TAB %reliveTime TAB %mapName;
	  CaptainListBox.addItem(%content,%indexID);
	  //设置鼠标事件
	  CaptainListBox.setCommand("CaptainListBox.onMouseDown();",%indexID);
	  CaptainListBox.setAltCommand("CaptainListBox.dblclickMouseDown();",%indexID);
}
//全服重要事件类
function UpDateGlobalEventList(%indexID,%eventName,%type,%mapName)
{
	%Color = "<t h='1' c='0xffffffff'>";  //RGBA
	%typeText = "";
	switch(%type)
	{
		case 0:
			%typeText = "<t h='1' c='0x808080ff'>" @ "未激活" @ "</t>";
	  case 1:
	  	%typeText = %Color @ "进行中" @"</t>";
	  case 2:
	  	%typeText = "<t h='1' c='0x00ff00ff'>" @ "完成" @ "</t>";
	}
	
	%indexText = %Color @ %indexID @ "</t>";
	%mapName = %Color @ %mapName @ "</t>";
	%content = %indexText TAB %eventName TAB %typeText TAB %mapName;
	AllServerListBox.addItem(%content,%indexID + 99);
	//设置鼠标事件
	AllServerListBox.setCommand("AllServerListBox.onMouseDown();",%indexID + 99);
	AllServerListBox.setAltCommand("AllServerListBox.dblclickMouseDown();",%indexID + 99);
}
//--------------------------------------------------------------------------------------------
//              鼠标双击和单击事件
//              双击寻径  如有多个NPC  随机取一个寻径
//              单击读出当前选中项内容描述信息
//---------------------------------------------------------------------------------------------
//每日活动
function EverydayListBox::dblclickMouseDown(%this)
{
	%player = GetPlayer();
	if(%player == 0)
	   return;
	   
	%indexID = %this.getSelectedId();
	if(%indexID $= "" || %indexID == 0)
	   return;
	   
	%npcSting = getCallboardDailyData(%indexID,10);
	%npcCount = getWordCount(%npcSting," /\n");
	
	%npcID = "";
	//如果可寻径的Npc>=2,那么随机取NpcID寻径
	if(%npcCount>1)
	{
		%RandomIndex = getRandom(%npcCount-1);
		%npcID = getWord(%npcSting,%RandomIndex," /\n");
	}
	else
	{
		%npcID = getWord(%npcSting,0," /\n");
	}
	
	%npcPosition =  getPath_Position(%npcID);
	if(%npcPosition !$= "")
	   %player.setPath(%npcPosition);
}

function EverydayListBox::onMouseDown(%this)
{
	%indexID = %this.getSelectedId();
	if(%indexID $= "" || %indexID == 0)
	   return;
	   
	%strText = GetActivityMsg(%indexID);
	if(%strText $= "")
	   return;
	   
	callboardContent_TextCrl.setContent(%strText);
}
//每周活动------------------------------------------------------------------------------------------------------
//双击寻径
function EveryweekListBox::dblclickMouseDown(%this)
{
	%player = GetPlayer();
	if(%player == 0)
	   return;
	   
	%indexID = %this.getSelectedId();
	if(%indexID $= "" || %indexID == 0)
	   return;
	   
	%npcSting = getCallboardDailyData(%indexID,10);
	%npcCount = getWordCount(%npcSting," /\n");
	
	%npcID = "";
	//如果可寻径的Npc>=2,那么随机取NpcID寻径
	if(%npcCount>1)
	{
		%RandomIndex = getRandom(%npcCount-1);
		%npcID = getWord(%npcSting,%RandomIndex," /\n");
	}
	else
	{
		%npcID = getWord(%npcSting,0," /\n");
	}
	
	%npcPosition =  getPath_Position(%npcID);
	if(%npcPosition !$= "")
	   %player.setPath(%npcPosition);
}

function EveryweekListBox::onMouseDown(%this)
{
	%indexID = %this.getSelectedId();
	if(%indexID $= "" || %indexID == 0)
	   return;
	   
	%strText = GetActivityMsg(%indexID);
	if(%strText $= "")
	   return;
	   
	callboardContent_TextCrl.setContent(%strText);
}
//日常活动------------------------------------------------------------------------------------------------------
function DayListBox::dblclickMouseDown(%this)
{
	%player = GetPlayer();
	if(%player == 0)
	   return;
	   
	%indexID = %this.getSelectedId();
	if(%indexID $= "" || %indexID == 0)
	   return;
	   
	%npcSting = getCallboardMissionData(%indexID,8);
	%npcCount = getWordCount(%npcSting," /\n");
	
	%npcID = "";
	//如果可寻径的Npc>=2,那么随机取NpcID寻径
	if(%npcCount>1)
	{
		%RandomIndex = getRandom(%npcCount-1);
		%npcID = getWord(%npcSting,%RandomIndex," /\n");
	}
	else
	{
		%npcID = getWord(%npcSting,0," /\n");
	}
	
	%npcPosition =  getPath_Position(%npcID);
	if(%npcPosition !$= "")
	   %player.setPath(%npcPosition);
}

function DayListBox::onMouseDown(%this)
{
	%player = GetPlayer();
	if(%player == 0)
	   return;
	   
	%indexID = %this.getSelectedId();
	if(%indexID $= "" || %indexID == 0)
	   return;
	
	%missionID = getCallboardMissionData(%indexID,1);
	if(%missionID $= "")
	  return;
	   
	%strText = GetDayMissionDialog(%player,%missionID);
	if(%strText $= "")
	   return;
	   
	callboardContent_TextCrl.setContent(%strText);
}
//首领击杀------------------------------------------------------------------------------------------------------
function CaptainListBox::dblclickMouseDown(%this)
{
	%player = GetPlayer();
	if(%player == 0)
	   return;
	   
	%indexID = %this.getSelectedId();
	if(%indexID $= "" || %indexID == 0)
	   return;
	   
	%npcSting = getGlobalEventData(%indexID,1);
	%npcCount = getWordCount(%npcSting," /\n");
	
	%npcID = "";
	//如果可寻径的Npc>=2,那么随机取NpcID寻径
	if(%npcCount>1)
	{
		%RandomIndex = getRandom(%npcCount-1);
		%npcID = getWord(%npcSting,%RandomIndex," /\n");
	}
	else
	{
		%npcID = getWord(%npcSting,0," /\n");
	}
	
	%npcPosition =  getPath_Position(%npcID);
	if(%npcPosition !$= "")
	   %player.setPath(%npcPosition);
}

function CaptainListBox::onMouseDown(%this)
{
	%indexID = %this.getSelectedId();
	if(%indexID $= "" || %indexID == 0)
	   return;
	   
	%strText = GetSeverEvenMsg(%indexID);
	if(%strText $= "")
	   return;
	   
	callboardContent_TextCrl.setContent(%strText);
}
//全服事件------------------------------------------------------------------------------------------------------
function AllServerListBox::dblclickMouseDown(%this)
{
	%player = GetPlayer();
	if(%player == 0)
	   return;
	   
	%indexID = %this.getSelectedId();
	if(%indexID $= "" || %indexID == 0)
	   return;
	   
	%npcSting = getGlobalEventData(%indexID,1);
	%npcCount = getWordCount(%npcSting," /\n");
	
	%npcID = "";
	//如果可寻径的Npc>=2,那么随机取NpcID寻径
	if(%npcCount>1)
	{
		%RandomIndex = getRandom(%npcCount-1);
		%npcID = getWord(%npcSting,%RandomIndex," /\n");
	}
	else
	{
		%npcID = getWord(%npcSting,0," /\n");
	}
	
	%npcPosition =  getPath_Position(%npcID);
	if(%npcPosition !$= "")
	   %player.setPath(%npcPosition);
}

function AllServerListBox::onMouseDown(%this)
{
	%indexID = %this.getSelectedId();
	if(%indexID $= "" || %indexID == 0)
	   return;
	   
	%strText = GetSeverEvenMsg(%indexID);
	if(%strText $= "")
	   return;
	   
	callboardContent_TextCrl.setContent(%strText);
}

//-----------------------------------------------------------------------
//            活动开始序告处理
//-----------------------------------------------------------------------
function EverydayStartNotify(%indexID,%notifyTime)
{
	if(%indexID == 0)
	   return;
	   
	%strText = GetActivityTime(%indexID,%notifyTime);
	
	if(%strText $= "")
	   return;
	
	//暂用提示
	SetScreenMessage(%strText,0xFF0000);
	
}

function WeekDayStartNotify(%indexID,%notifyTime)
{
	if(%indexID == 0)
	   return;
	   
	%strText = GetActivityTime(%indexID,%notifyTime);
	
	if(%strText $= "")
	   return;
	
	//暂用提示
	SetScreenMessage(%strText,0xFF0000);
}

//任务物品领取
function getMissionPrizeItem()
{
	%player = GetPlayer();
	if(%player == 0)
	   return;
  
  %missionID = "";
	PlayerDayMissionOver(%player,%missionID);
}

function updateCurrentDayActive()
{
	 EveryweekListBox.clearItems();
	 DayListBox.clearItems();
	 callboardCurrentDay();
}