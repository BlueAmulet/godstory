// 增加任务追踪树节点
function AddTraceTreeNode(%node, %text, %missionId, %click)
{
	return MissionTraceWnd_Tree.init(%node,%text,%missionId,%click);
}

// 增加任务追踪树节点
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

// 选择任务树节点
function MissionTraceWnd_Tree::onSelect(%this,%obj)
{
   eval(%this.getItemValue(%obj));       
}

// 增加可接任务树节点
function AddCanAcceptTreeNode(%node, %text, %missionId, %click)
{
	return MissionTraceWnd_CanSelectMissionTree.init(%node,%text,%missionId,%click);
}

// 增加可接任务树节点
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

// 选中可接任务树节点
function MissionTraceWnd_CanSelectMissionTree::onSelect(%this,%obj)
{
   eval(%this.getItemValue(%obj));       
}

function TraceTree_Click(%node,%missionId)
{
	echo(%node);
}

// 判断是否按地图排列任务
function isOrderByMap()
{
	return (MissionWndGui_ArrayMissionButton.getText() !$= "按地图排列任务");
}

// 过滤任务树按钮响应方法
function CheckFilterMission()
{
	if(isOrderByMap())
		MissionWndGui_ArrayMissionButton.setText("按地图排列任务");
	else
		MissionWndGui_ArrayMissionButton.setText("按类型排列任务");
	MissionTraceWnd.setVisible(1);
	RefreshAcceptedMission(GetPlayer());
}

// 显示任务追踪界面
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

// 隐藏任务追踪界面
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

// "任务追踪"复选框响应方法
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

// 显示活动追踪界面
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

// 隐藏活动追踪界面
function HideFeastDayWnd()
{	
	MissionTraceWnd_ControlWnd2.setvisible(0);
	TraceTreeBackWnd1.setvisible(0);
	if(!MissionTraceWnd_ControlWnd1.isvisible())	
		MissionTraceWnd.setVisible(0);
}

// "活动追踪"复选框响应方法
function CheckFeastDay()
{
	if(MissionWndGui_FeastDayTraceCheckBox.IsStateOn())
		ShowFeastDayWnd();
	else
		HideFeastDayWnd();
}

// 折叠或展开任务追踪窗口
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

// 折叠或展开活动追踪窗口
function ChangeByWnd2()
{
	if(ShowTraceTreeButtton_PartyChange.isStateOn())
		TraceTreeBackWnd1.setvisible(1);
	else
		TraceTreeBackWnd1.setvisible(0);
}

// 点击任务追踪控制面板上"已接"按钮
function OpenCurrentlyMission()
{
	SingleWnd2_ScrollCtrl.setvisible(1);
	SingleWnd2_ScrollCtrl_CanSelectMission.setvisible(0);
	ShowTraceTreeButtton_Change.setStateOn(1);
	ChangeByWnd1();	
	RefreshTraceMission(GetPlayer());
}

// 点击任务追踪控制面板上"可接"按钮
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