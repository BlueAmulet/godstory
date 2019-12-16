//===========================================================================================================
// 文档说明:任务浏览界面操作
// 创建时间:2009-6-8
// 创建人: soar
//=========================================================================================================== 
function ChangeLong()
{
	%ContorlExtentX = getWord(MissionWndGui_EntireWnd.extent,0);
	%ContorlExtentY = getWord(MissionWndGui_EntireWnd.extent,1);
	%WindowExtentX = getWord(MissionWndGui_Window.extent,0);
	%WindowExtentY = getWord(MissionWndGui_Window.extent,1);
	%BitmapExtentX = getWord(MissionWndGui_ChangeBitMap.extent,0);
	%BitmapExtentY = getWord(MissionWndGui_ChangeBitMap.extent,1);
	if(%ContorlExtentX == 335)
	{
		%ContorlExtentX = 615;
		%WindowExtentX = 615;
		%BitmapExtentX = 475;
		MissionWndGui_EntireWnd.setExtent(%ContorlExtentX,%ContorlExtentY);
		MissionWndGui_Window.setExtent(%WindowExtentX,%WindowExtentY);
		MissionWndGui_ChangeBitMap.setExtent(%BitmapExtentX,%BitmapExtentY);
		MissionWndGui_SingleWnd2.setVisible(1);
	}	
}

function MissionWndLeftMove()
{
	%ContorlExtentX = getWord(MissionWndGui_EntireWnd.extent,0);
	%ContorlExtentY = getWord(MissionWndGui_EntireWnd.extent,1);
	%WindowExtentX = getWord(MissionWndGui_Window.extent,0);
	%WindowExtentY = getWord(MissionWndGui_Window.extent,1);
	%BitmapExtentX = getWord(MissionWndGui_ChangeBitMap.extent,0);
	%BitmapExtentY = getWord(MissionWndGui_ChangeBitMap.extent,1);
	if(%ContorlExtentX == 615)
	{
		%ContorlExtentX = 335;
		%WindowExtentX = 335;
		%BitmapExtentX = 268;
		MissionWndGui_EntireWnd.setExtent(%ContorlExtentX,%ContorlExtentY);
		MissionWndGui_Window.setExtent(%WindowExtentX,%WindowExtentY);
		MissionWndGui_ChangeBitMap.setExtent(%BitmapExtentX,%BitmapExtentY);
		MissionWndGui_SingleWnd2.setVisible(0);
	}
}

// 打开或关闭任务浏览窗口
function OpenOrCloseMissionWnd()
{
	if(MissionWndGui_EntireWnd.IsVisible())
		CloseMissionWnd();
	else
		OpenMissionWnd();
}

//打开任务浏览窗口
function OpenMissionWnd()
{
	if(!MissionWndGui_EntireWnd.IsVisible())
	{
		PlayOpenOrCloseGuiSound();
		GameMainWndGui_Layer3.PushToBack(MissionWndGui_EntireWnd);
		MissionWndGui_EntireWnd.setVisible(1);
		$IsDirty++;
		ShowHasMission();
	}
}

//关闭任务浏览窗口
function CloseMissionWnd()
{
	if(MissionWndGui_EntireWnd.IsVisible())
	{
		PlayOpenOrCloseGuiSound();
		MissionWndGui_EntireWnd.setVisible(0);
		$IsDirty--;
	}
}

// 显示已接任务树
function ShowHasMission()
{
	SingleWnd1_ScrollCtrl1.setvisible(1);
	SingleWnd1_ScrollCtrl2.setvisible(0);
	SingleWnd1_ScrollCtrl3.setvisible(0);
	MissionWndGui_GiveUpMissionButton.setText("放弃任务");
	MissionWndGui_GiveUpMissionButton.Command = "DelectMission();";
	MissionWndGui_ShareMissionButton.setvisible(0);
	RefreshAcceptedMission(GetPlayer());
}

// 显示可接任务树
function ShowCanAcceptMission()
{
	SingleWnd1_ScrollCtrl1.setvisible(0);
	SingleWnd1_ScrollCtrl2.setvisible(1);	
	SingleWnd1_ScrollCtrl3.setvisible(0);
	MissionWndGui_GiveUpMissionButton.setText("接取任务");
	MissionWndGui_GiveUpMissionButton.Command = "WillAcceptMission();";
	MissionWndGui_ShareMissionButton.setvisible(0);
	RefreshCanAcceptMission(GetPlayer());
}

// 显示共享任务树
function ShowShareMission()
{
	SingleWnd1_ScrollCtrl1.setvisible(0);
	SingleWnd1_ScrollCtrl2.setvisible(0);	
	SingleWnd1_ScrollCtrl3.setvisible(1);
	MissionWndGui_GiveUpMissionButton.setText("接取任务");
	MissionWndGui_GiveUpMissionButton.Command = "WillAcceptMission();";
	MissionWndGui_ShareMissionButton.setvisible(0);
	//发送请求获取队伍共享任务
	GetPlayer().SendMissionRequest(3, 0);	
}

// 添加树结点（已接任务树列表)
function AddTreeNode1(%node,%text,%missionId,%click, %constringency)
{	
   //%name = "TraceFlagBitmap" @ %Mid ;
	 //%text = "<c cid='" @ %name @ "' cmd='test' cf='CreateFlagBitmap' />";
	 //AddTreeNode1(%ZhangHuiTree2, %text @ "<t f='宋体' n='12' c='0xffffffff' v='1'>" @ $Mid_Name[%Mid] @ "</t>", %Mid, 1);	}
   return SingleWnd1_Tree1.init(%node,%text,%missionId, %click, %constringency);
}

// 添加树结点（可接任务树列表)
function AddTreeNode2(%node,%text,%missionId, %click,%constringency)
{
   return SingleWnd1_Tree2.init(%node,%text,%missionId, %click, %constringency);
}

// 添加树结点（共享任务树列表)
function AddTreeNode3(%node,%text,%missionId, %click,%constringency)
{
   return SingleWnd1_Tree3.init(%node,%text,%missionId, %click,%constringency);
}

function SingleWnd1_Tree1::init(%this,%node,%text,%missionId, %click,%constringency)
{
	%missionNode = SingleWnd1_Tree1.findMissionTreeNode(%missionId);
	if(%missionNode)
	{
		%this.editItem(%missionNode, %text, %Create);
	}
	else
	{
		if(%click)
			%Create = "Tree_Click(" @ %node @ "," @ %missionId @ ");";
		else
			%Create = "";
		%missionNode = %this.insertItem(%node,%text,%Create,"",0,0,%constringency,%missionId);
	}
	return %missionNode;
}

function SingleWnd1_Tree2::init(%this, %node, %text, %missionId, %click, %constringency)
{
	%missionNode = SingleWnd1_Tree2.findMissionTreeNode(%missionId);
	if(%missionNode)
	{
		%this.editItem(%missionNode, %text, %Create);
	}
	else
	{		
		if(%click)
			%Create = "Tree_Click(" @ %node @ "," @ %missionId @ ");";
		else
			%Create = "";
		%missionNode = %this.insertItem(%node,%text,%Create,"",0,0,%constringency,%missionId);
	}
	return %missionNode;
}

function SingleWnd1_Tree3::init(%this, %node, %text, %missionId, %click, %constringency)
{
	%missionNode = SingleWnd1_Tree3.findMissionTreeNode(%missionId);
	if(%missionNode)
	{
		%this.editItem(%missionNode, %text, %Create);
	}
	else
	{		
		if(%click)
			%Create = "Tree_Click(" @ %node @ "," @ %missionId @ ");";
		else
			%Create = "";
		%missionNode = %this.insertItem(%node,%text,%Create,"",0,0,%constringency,%missionId);
	}
	return %missionNode;
}

//选择已接任务树
function SingleWnd1_Tree1::onSelect(%this,%obj)
{
   eval(%this.getItemValue(%obj));       
   ChangeLong();
}

//选择可接任务树
function SingleWnd1_Tree2::onSelect(%this,%obj)
{
   eval(%this.getItemValue(%obj));
   ChangeLong();
}

//选择共享任务树
function SingleWnd1_Tree3::onSelect(%this,%obj)
{
   eval(%this.getItemValue(%obj));
   ChangeLong();
}

// 当前选择的任务编号
$CurrentMissionID = -1;

// 点击树节点，根据MissionID显示任务描述
function Tree_Click(%node,%missionId)
{
	if(SingleWnd1_ScrollCtrl1.isvisible())
		MissionWndGui_ShareMissionButton.setvisible(1);
	else
		MissionWndGui_ShareMissionButton.setvisible(0);
	$CurrentMissionID = %missionId;
	RefreshMissionDesc(%missionId);
}

// 刷新当前任务数量
function RefreshMissionNum(%num)
{
	SingleWnd1_MissionNum.setText(%num @ "/20");
}

// 刷新当前任务描述
function RefreshMissionDesc(%MissionID)
{
		if($CurrentMissionID != %MissionID || %MissionID == -1)
			SingleWnd2_ShowMissionInfo.setcontent("");
		else
			SingleWnd2_ShowMissionInfo.setcontent(GetMissionText(GetPlayer(), %MissionID));
}

// 删除任务给出提示
function DelectMission()
{
	if($CurrentMissionID != -1)
	{
		OpenDialog_OK_Cancel();
		Dialog_OK_Cancel.SetTitle("测试一下");
		Dialog_OK_Cancel.SetContent("您确定要删除当前选择的任务吗？");
		Dialog_OK_Cancel.SetOK("确定", "DelectMission_OK();");
		Dialog_OK_Cancel.SetCancel("否定", "DelectMission_Cancel();");
	}
}

// 确定删除任务
function DelectMission_OK()
{
	%player = GetPlayer();
	%player.SendMissionRequest(1, $CurrentMissionID);
}

//发送接取任务请求
function WillAcceptMission()
{
	if($CurrentMissionID != -1)
	{
		%player = GetPlayer();
		%player.SendMissionRequest(0, $CurrentMissionID);
	}
}

//发送共享任务给队友请求
function WillShareMission()
{
	if($CurrentMissionID != -1)
	{
		%player = GetPlayer();
		%player.SendMissionRequest(2, $CurrentMissionID);
	}
}

//例子
function createButton(%name, %parent, %x, %y, %cmd,%text,%p)
{
	if(!isObject(%p))
	{
		%p = "GuiMissionTraceButtonProfile0";
	}
	
	new GuiRichButtonCtrl(%name){
		canSaveDynamicFields = "0";
    Enabled = "1";
    isContainer = "0";
    Profile = %p;                            
    HorizSizing = "right";
    VertSizing = "bottom";
    position = %x SPC %y;
    Extent = "280 16";
    MinExtent = "8 2";
    canSave = "1";
    Visible = "1";
    command = %cmd;
    text = %text;
    hovertime = "1000";
    groupNum = "-1";
    buttonType = "PushButton";
    useMouseEvents = "1";
    IsStateOn = "0";
    IsUseTextEx = "1";
	};
}

//创建一个显示倒计时文本标签
function createClockLabel(%name, %parent, %x, %y, %cmd, %p)
{
	if(!isObject(%p))
	{
		%p = "GuiCurrencyShowInfoTextProfile_1";
	}
	
	 new GuiClockCtrl(%name){
	 		Profile = %p;
   		HorizSizing = "right";
   		VertSizing = "bottom";
      position = %x SPC %y;
      Extent = "59 19";
      MinExtent = "8 2";
      canSave = "1";
      Visible = "1";
      hovertime = "1000";
      lifetime = %cmd;
	   };
}

function CreateFlagBitmap(%name,%parent,%x,%y,%cmd)
{
	 echo("----------------------------" @ %name );
	     new GuiBitmapCtrl(%name) {
         canSaveDynamicFields = "0";
         Enabled = "1";
         isContainer = "0";
         HorizSizing = "right";
         VertSizing = "bottom";
         position = %x SPC %y;
         Extent = "14 14";
         MinExtent = "8 2";
         canSave = "1";
         Visible = "1";
         mustPrerender = "0";
         hovertime = "1000";
         alpha = "255";
         useFadeout = "1";
         bitmap = "gameres/gui/images/GUIWindow27_1_002.png";
         wrap = "0";
      };
}

function setMissionInfoState(%id,%vb)
{
	if(SingleWnd2_ShowMissionInfo.GetNodeAttribute(%id,"vb") == "1")
	{
		SingleWnd2_ShowMissionInfo.SetNodeAttribute(%id,"vb","0");
	}
	else
	{
		SingleWnd2_ShowMissionInfo.SetNodeAttribute(%id,"vb","1");
	}
	SingleWnd2_ShowMissionInfo.Refresh();
}

function ShowMissionInfoButton::onMouseUp(%this)
{
	return "ShowMissionInfoButton";
}