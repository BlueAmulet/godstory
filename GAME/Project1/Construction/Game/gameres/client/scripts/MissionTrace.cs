// ��������׷�����ڵ�
function AddTraceTreeNode(%node, %text, %missionId, %click)
{
	return MissionTraceWnd_Tree.init(%node,%text,%missionId,%click);
}

// ��������׷�����ڵ�
function MissionTraceWnd_Tree::init(%this,%node,%text,%missionId,%click)
{
	if(%click)
			%Create = "TraceTree_Click(" @ %node @ "," @ %missionId @ ");";
		else
			%Create = "";
	%missionNode = MissionTraceWnd_Tree.findMissionTreeNode(%missionId);
	if(%missionNode)
		%this.editItem(%missionNode, %text, %Create);
	else
		%missionNode = %this.insertItem(%node,%text,"","",0,0,%click,%missionId);
	return %missionNode;
}

// ѡ���������ڵ�
function MissionTraceWnd_Tree::onSelect(%this,%obj)
{
   eval(%this.getItemValue(%obj));       
}

// ���ӿɽ��������ڵ�
function AddCanAcceptTreeNode(%node, %text, %missionId, %click)
{
	return MissionTraceWnd_CanSelectMissionTree.init(%node,%text,%missionId,%click);
}

// ���ӿɽ��������ڵ�
function MissionTraceWnd_CanSelectMissionTree::init(%this,%node,%text,%missionId,%click)
{
	if(%click)
			%Create = "TraceTree_Click(" @ %node @ "," @ %missionId @ ");";
		else
			%Create = "";
	%missionNode = MissionTraceWnd_CanSelectMissionTree.findMissionTreeNode(%missionId);
	if(%missionNode)
		%this.editItem(%missionNode, %text, %Create);
	else
		%missionNode = %this.insertItem(%node,%text,"","",0,0,%click,%missionId);
	return %missionNode;
}

// ѡ�пɽ��������ڵ�
function MissionTraceWnd_CanSelectMissionTree::onSelect(%this,%obj)
{
   eval(%this.getItemValue(%obj));       
}

function TraceTree_Click(%node,%missionId)
{
	echo(%node);
}

// �ж��Ƿ񰴵�ͼ��������
function isOrderByMap()
{
	return (MissionWndGui_ArrayMissionButton.getText() !$= "����ͼ��������");
}

// ������������ť��Ӧ����
function CheckFilterMission()
{
	if(isOrderByMap())
		MissionWndGui_ArrayMissionButton.setText("����ͼ��������");
	else
		MissionWndGui_ArrayMissionButton.setText("��������������");
	MissionTraceWnd.setVisible(1);
	RefreshAcceptedMission(GetPlayer());
}

// ��ʾ����׷�ٽ���
function ShowTraceWnd()
{
	MissionTraceWnd.setVisible(1);
	MissionTraceWnd_ControlWnd1.setvisible(1);
	TraceTreeBackWnd.setvisible(1);
	if(MissionTraceWnd_ControlWnd2.isvisible())
	{
		MissionTraceWnd_ControlWnd2.setPosition(0,248);
		TraceTreeBackWnd1.setPosition(1,273);
	}
}

// ��������׷�ٽ���
function HideTraceWnd()
{
	MissionTraceWnd_ControlWnd1.setvisible(0);
	TraceTreeBackWnd.setvisible(0);
	if(MissionTraceWnd_ControlWnd2.isvisible())
	{
		MissionTraceWnd_ControlWnd2.setPosition(0,0);
		TraceTreeBackWnd1.setPosition(1,25);
	}
	else
		MissionTraceWnd.setVisible(0);
}

// "����׷��"��ѡ����Ӧ����
function CheckTraceMission()
{
	if(MissionWndGui_MissionTraceCheckBox.IsStateOn())
	{
		ShowTraceWnd();
		RefreshTraceMission(GetPlayer());
	}
	else
		HideTraceWnd();
}

// ��ʾ�׷�ٽ���
function ShowFeastDayWnd()
{	
	MissionTraceWnd.setVisible(1);
	if(MissionTraceWnd_ControlWnd1.isvisible())
	{
		MissionTraceWnd_ControlWnd2.setPosition(0,248);
		TraceTreeBackWnd1.setPosition(1,273);
	}
	else
	{
		MissionTraceWnd_ControlWnd2.setPosition(0,0);
		TraceTreeBackWnd1.setPosition(1,25);
	}
	MissionTraceWnd_ControlWnd2.setvisible(1);
	TraceTreeBackWnd1.setvisible(1);
}

// ���ػ׷�ٽ���
function HideFeastDayWnd()
{	
	MissionTraceWnd_ControlWnd2.setvisible(0);
	TraceTreeBackWnd1.setvisible(0);
	if(!MissionTraceWnd_ControlWnd1.isvisible())	
		MissionTraceWnd.setVisible(0);
}

// "�׷��"��ѡ����Ӧ����
function CheckFeastDay()
{
	if(MissionWndGui_FeastDayTraceCheckBox.IsStateOn())
		ShowFeastDayWnd();
	else
		HideFeastDayWnd();
}

// �۵���չ������׷�ٴ���
function ChangeByWnd1()
{
	if(ShowTraceTreeButtton_Change.isStateOn())
	{
		TraceTreeBackWnd.setvisible(1);
		MissionTraceWnd_ControlWnd2.setPosition(0,248);
		TraceTreeBackWnd1.setPosition(1,273);
		CheckTraceMission();
	}
	else
	{
		TraceTreeBackWnd.setvisible(0);
		MissionTraceWnd_ControlWnd2.setPosition(0,25);
		TraceTreeBackWnd1.setPosition(1,50);
	}
}

// �۵���չ���׷�ٴ���
function ChangeByWnd2()
{
	if(ShowTraceTreeButtton_PartyChange.isStateOn())
		TraceTreeBackWnd1.setvisible(1);
	else
		TraceTreeBackWnd1.setvisible(0);
}

// �������׷�ٿ��������"�ѽ�"��ť
function OpenCurrentlyMission()
{
	SingleWnd2_ScrollCtrl.setvisible(1);
	SingleWnd2_ScrollCtrl_CanSelectMission.setvisible(0);
	ShowTraceTreeButtton_Change.setStateOn(1);
	ChangeByWnd1();	
	RefreshTraceMission(GetPlayer());
}

// �������׷�ٿ��������"�ɽ�"��ť
function OpenCanSelectMission()
{
	SingleWnd2_ScrollCtrl.setvisible(0);
	SingleWnd2_ScrollCtrl_CanSelectMission.setvisible(1);
	ShowTraceTreeButtton_Change.setStateOn(1);
	ChangeByWnd1();	
	RefreshCanAcceptMission(GetPlayer());
}

function FindPath(%PathId)
{
	%player = getPlayer();
	if(%player == 0)
	   return;
	   
	%pos = getPath_Position(%PathId);
	if (%pos != "")
		%player.setPath(%pos);
}

function MissionTraceHover(%Mid)
{
	%player = getPlayer();
	if(%player == 0)
	   return;
	   
	MissionHoverInfo.setcontent(GetDialogText(0, %player, 800 @ %Mid , 0));
  MissionHover.setextent(getword(MissionHoverInfo.getextent(),0)+15,getword(MissionHoverInfo.getextent(),1)+15);
	OpenMisssionHover();
}

function mousePosition(%position)
{
	%tempx = getword(MissionTraceWnd.getposition(),0) - 230;
	MissionHover.setposition(%tempx,getword(%position,1));
}

function OpenMisssionHover()
{
	MissionHover.setvisible(1);
	$IsDirty++;
}

function CloseMisssionHover()
{
	MissionHover.setvisible(0);
	$IsDirty--;
}