//===========================================================================================================
// �ĵ�˵��:��׭¼����
// ����ʱ��:2009-12-15
// ������: Batcel
// �޸�ʱ��:2009-12-29
// �޸���:thinking
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
	  		%weekName = "������";
	  	case 1:
	  		%weekName = "����һ";
	  	case 2:
	  		%weekName = "���ڶ�";
	  	case 3:
	  		%weekName = "������";
	  	case 4:
	  		%weekName = "������";
	  	case 5:
	  		%weekName = "������";
	    case 6:
	    	%weekName = "������";
	  }
	  
	  %text = %weekName SPC %time;
	  CallboardTime_Text.setText(%text);
}
//ÿ�ջ
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
			 %level = %Color @ %level @ "��" @ "</t>";
		else
		   %level = "<t h='1' c='0xff0000ff'>" @ %level @ "��" @ "</t>";
		
		%lineName = %Color @ %lineName @ "</t>";
	  %content = %strName TAB %Time TAB %level TAB %lineName;

		
		%text = %degreeImage TAB %content TAB %status;
	  EverydayListBox.addItem(%text,%indexID);
	  EverydayListBox.setListBoxTagText(%isStart,%indexID);  //���ñ�����ڸ���
	  //��������¼�
	  EverydayListBox.setCommand("EverydayListBox.onMouseDown();",%indexID);
	  EverydayListBox.setAltCommand("EverydayListBox.dblclickMouseDown();",%indexID);
		
}
//ÿ�ܻ
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
	  		%weekName = "����һ";
	  	case 2:
	  		%weekName = "���ڶ�";
	  	case 3:
	  		%weekName = "������";
	  	case 4:
	  		%weekName = "������";
	  	case 5:
	  		%weekName = "������";
	    case 6:
	    	%weekName = "������";
	    case 7:
	  		%weekName = "������";
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
			 %level = %Color @ %level @ "��" @ "</t>";
		else
		   %level = "<t h='1' c='0xff0000ff'>" @ %level @ "��" @ "</t>";
		
		%lineName = %Color @ %lineName @ "</t>";
	  %content = %strName TAB %weekName TAB %Time TAB %level TAB %lineName;

		
		%text = %degreeImage TAB %content TAB %status;
	  EveryweekListBox.addItem(%text,%indexID);
	  EveryweekListBox.setListBoxTagText(%isStart,%indexID);
	  //��������¼�
	  EveryweekListBox.setCommand("EveryweekListBox.onMouseDown();",%indexID);
	  EveryweekListBox.setAltCommand("EveryweekListBox.dblclickMouseDown();",%indexID);
}
//�ճ��
function updateDailyList(%indexID,%MissionID,%degree,%Name,%weekDay,%level,%countLoop,%bPrize,%isStart)
{
	  %player = GetPlayer();
	  if(%player == 0)
	    return;
	    
	  %CurrLoop  = %player.GetCycleMissionTimes(%MissionID);  //��ǰ��ɴ���
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
	  	  %weekName = "ÿ��";
	  	case 1:
	  		%weekName = "����һ";
	  	case 2:
	  		%weekName = "���ڶ�";
	  	case 3:
	  		%weekName = "������";
	  	case 4:
	  		%weekName = "������";
	  	case 5:
	  		%weekName = "������";
	    case 6:
	    	%weekName = "������";
	    case 7:
	  		%weekName = "������";
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
			 %level = %Color @ %level @ "��" @ "</t>";
		else
		   %level = "<t h='1' c='0xff0000ff'>" @ %level @ "��" @ "</t>";
		
		%loopNum = %Color @ %CurrLoop @ "/" @ %countLoop @ "</t>";
		
		if(%bPrize)
		  %Prize = %Color @ "��" @ "</t>";
		else
		  %Prize = %Color @ "��" @ "</t>";
		 
	  %content = %strName TAB %weekName TAB %level TAB %loopNum TAB %Prize TAB %MissionID;
		%text = %degreeImage TAB %content;	  
	  DayListBox.addItem(%text,%indexID);
	  DayListBox.setListBoxTagText(%isStart,%indexID);
	  //��������¼�
	  DayListBox.setCommand("DayListBox.onMouseDown();",%indexID);
	  DayListBox.setAltCommand("DayListBox.dblclickMouseDown();",%indexID);
	
}
//�����ɱ��
function updateKillNpcList(%indexID,%npcID,%type,%time,%mapName)
{
	  %npcName = getNpcData(%npcID,1);
	  %npcLevel = getNpcData(%npcID,9);
	  
	  %Color = "<t h='1' c='0xffffffff'>";  //RGBA
	  %status = "<t h='1' c='0x808080ff'>" @ "δˢ��" @ "</t>";
	  %reliveTime = %Color @ "��" @ "</t>";
	  
	  if(%type)
	  	 %status = %Color @ "���" @ "</t>";
	  
	  	
	  if(%time>0)
	  {
	     %hour = %time / 3600;
	     %min  = (%time %3600) / 60;
	     if(%min >0)
	     	  %reliveTime = %Color @ %hour @ "��" @ %min @ "�ָ���" @ "</t>";
	     else
	     	  %reliveTime = %Color @ %hour @ "�㸴��" @ "</t>";
	  }
	  
	  %indexText = %Color @ %indexID @ "</t>";
	  %mapName = %Color @ %mapName @ "</t>";
	  %npcLevel = %Color @ %npcLevel @ "��" @ "</t>";
	  %content = %indexText TAB %npcName TAB %npcLevel TAB %status TAB %reliveTime TAB %mapName;
	  CaptainListBox.addItem(%content,%indexID);
	  //��������¼�
	  CaptainListBox.setCommand("CaptainListBox.onMouseDown();",%indexID);
	  CaptainListBox.setAltCommand("CaptainListBox.dblclickMouseDown();",%indexID);
}
//ȫ����Ҫ�¼���
function UpDateGlobalEventList(%indexID,%eventName,%type,%mapName)
{
	%Color = "<t h='1' c='0xffffffff'>";  //RGBA
	%typeText = "";
	switch(%type)
	{
		case 0:
			%typeText = "<t h='1' c='0x808080ff'>" @ "δ����" @ "</t>";
	  case 1:
	  	%typeText = %Color @ "������" @"</t>";
	  case 2:
	  	%typeText = "<t h='1' c='0x00ff00ff'>" @ "���" @ "</t>";
	}
	
	%indexText = %Color @ %indexID @ "</t>";
	%mapName = %Color @ %mapName @ "</t>";
	%content = %indexText TAB %eventName TAB %typeText TAB %mapName;
	AllServerListBox.addItem(%content,%indexID + 99);
	//��������¼�
	AllServerListBox.setCommand("AllServerListBox.onMouseDown();",%indexID + 99);
	AllServerListBox.setAltCommand("AllServerListBox.dblclickMouseDown();",%indexID + 99);
}
//--------------------------------------------------------------------------------------------
//              ���˫���͵����¼�
//              ˫��Ѱ��  ���ж��NPC  ���ȡһ��Ѱ��
//              ����������ǰѡ��������������Ϣ
//---------------------------------------------------------------------------------------------
//ÿ�ջ
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
	//�����Ѱ����Npc>=2,��ô���ȡNpcIDѰ��
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
//ÿ�ܻ------------------------------------------------------------------------------------------------------
//˫��Ѱ��
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
	//�����Ѱ����Npc>=2,��ô���ȡNpcIDѰ��
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
//�ճ��------------------------------------------------------------------------------------------------------
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
	//�����Ѱ����Npc>=2,��ô���ȡNpcIDѰ��
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
//�����ɱ------------------------------------------------------------------------------------------------------
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
	//�����Ѱ����Npc>=2,��ô���ȡNpcIDѰ��
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
//ȫ���¼�------------------------------------------------------------------------------------------------------
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
	//�����Ѱ����Npc>=2,��ô���ȡNpcIDѰ��
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
//            ���ʼ��洦��
//-----------------------------------------------------------------------
function EverydayStartNotify(%indexID,%notifyTime)
{
	if(%indexID == 0)
	   return;
	   
	%strText = GetActivityTime(%indexID,%notifyTime);
	
	if(%strText $= "")
	   return;
	
	//������ʾ
	SetScreenMessage(%strText,0xFF0000);
	
}

function WeekDayStartNotify(%indexID,%notifyTime)
{
	if(%indexID == 0)
	   return;
	   
	%strText = GetActivityTime(%indexID,%notifyTime);
	
	if(%strText $= "")
	   return;
	
	//������ʾ
	SetScreenMessage(%strText,0xFF0000);
}

//������Ʒ��ȡ
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