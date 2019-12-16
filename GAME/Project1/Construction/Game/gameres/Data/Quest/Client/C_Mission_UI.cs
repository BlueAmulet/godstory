//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端任务相关界面内显示的文本
//==================================================================================

//刷新任务或对话相关UI数据
function RefreshMission(%Player, %op, %Mid)
{
	switch(%op)
	{
		case 1: // 添加一个任务
			UIAddOneMission(%Player, %Mid);
		case 2:	// 更新一个任务
			UIUpdateOneMission(%Player, %Mid);
		case 4: // 删除一个任务
			UIDelOneMission(%Player, %Mid);
		case 6:	// 完成一个任务
			UIDelOneMission(%Player, %Mid);
		case 7: // 更新所有任务数据
		  RefreshAcceptedMission(%Player);
		  RefreshTraceMission(%Player);
		  RefreshCanAcceptMission(%Player);
		case 12: // 更新所有共享任务数据
			RefreshShareMission(%Player);
		default:
			echo("RefreshMission Error");
	}
}

// 在任务浏览界面的已接任务树上创建一个任务的树结点
function CreateAcceptedMissionNode(%Player, %Mid, %Mission_Map)
{
	%MissionData = "MissionData_" @ %Mid;
	%node = SingleWnd1_Tree1.findMissionTreeNode(%Mid);
//	echo(" ");
//	echo("=====================================");
//	echo("CreateAcceptedMissionNode = %node = "@%node);
	if(%node == 0)
		{
			%kind       = GetMissionKind(%Mid);
			%Mid_LeiXin = $MissionKind[ %kind, 1 ];
			%Mid_Map    = $Mission_Map[%MissionData.Map];
			if(%Mid == 20001)
				{
					if(%Player.GetFamily() == 0){%Mid_Map = $Mission_Map[1001];} //测试：无职业
					if(%Player.GetFamily() == 1){%Mid_Map = $Mission_Map[1001];} //圣师门NPC
					if(%Player.GetFamily() == 2){%Mid_Map = $Mission_Map[1001];} //佛师门NPC
					if(%Player.GetFamily() == 3){%Mid_Map = $Mission_Map[1001];} //仙师门NPC
					if(%Player.GetFamily() == 4){%Mid_Map = $Mission_Map[1001];} //精师门NPC
					if(%Player.GetFamily() == 5){%Mid_Map = $Mission_Map[1001];} //鬼师门NPC
					if(%Player.GetFamily() == 6){%Mid_Map = $Mission_Map[1001];} //怪师门NPC
					if(%Player.GetFamily() == 7){%Mid_Map = $Mission_Map[1001];} //妖师门NPC
					if(%Player.GetFamily() == 8){%Mid_Map = $Mission_Map[1001];} //魔师门NPC
				}

//			echo("CreateAcceptedMissionNode = %kind = "@%kind);
//			echo("CreateAcceptedMissionNode = %Mid_LeiXin = "@%Mid_LeiXin);
//			echo("CreateAcceptedMissionNode = %Mid_Map = "@%Mid_Map);
//			echo("CreateAcceptedMissionNode = %Mission_Map = "@%Mission_Map);

			if(CanDoThisMission(%Player, %Mid, 2, 0, 0) $= "")
				if(%Player.GetMissionFlag(%Mid,8000)!=7)
					%Mid_Over = $Get_Dialog_GeShi[31204] @ " （完成）</t>";
				else
					%Mid_Over = "";

			//默认值，所有任务以任务类型创建树
			if(%Mission_Map == 0)
				{
					%rootnode = AddTreeNode1(0, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %kind, 0, 1);
					if(%rootnode != 0)
						{
							if(%kind == 3)
								AddTreeNode1(%rootnode, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>" @ %Mid_Over, %Mid, 1, 0);
							else
								{
									//任务归属的地图名称
									%Map = AddTreeNode1(%rootnode, $Get_Dialog_GeShi[31201] @ "【" @ %Mid_Map @ "】</t>", %kind @ %MissionData.Map, 0, 1);
									//任务名称
									AddTreeNode1(%Map, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>" @ %Mid_Over, %Mid, 1, 0);
								}
						}
				}

			//修改值，所有任务以地图区分创建树
			if(%Mission_Map == 1)
				{
					if(%kind == 3)
						{
							%rootnode = AddTreeNode1(0, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %kind, 0, 1);

							if(%rootnode != 0)
								AddTreeNode1(%rootnode, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>" @ %Mid_Over, %Mid, 1, 0);
						}
						else
							{
								%rootnode = AddTreeNode1(0, $Get_Dialog_GeShi[31201] @ "【" @ %Mid_Map @ "】</t>", %MissionData.Map, 0, 1);
								if(%rootnode != 0)
									{
										//任务类型名称
										%LeiXin = AddTreeNode1(%rootnode, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %MissionData.Map @ %kind, 0, 1);
										//任务名称
										AddTreeNode1(%LeiXin, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>" @ %Mid_Over, %Mid, 1, 0);
									}
							}
				}
		}
}

// 在任务浏览界面的可接任务树上创建一个任务结点
function CreateCanAcceptMissionNode(%Player, %NoUse)
{
//	Get_Lv_Mission(%Player.GetLevel());

//	echo(" ");
//	echo("=====================================");
//	echo("CreateCanAcceptMissionNode = $Lv_Mission = "@$Lv_Mission);
	%Lv = %Player.GetLevel();

	for(%i = 0; %i < $MissionData_See_Lv[%Lv]; %i++)
	{
		%Mid = $MissionData_See_Lv[ %Lv, %i + 1 ];
//		echo("CreateAcceptedMissionNode = %Mid = "@%Mid@" %i = "@%i);

		if(CanDoThisMission(%Player, %Mid, 1, 0, 0) $= "")
			{
				if( (%Mid == 20001)||(%Mid == 10124)||(%Mid == 10125)||(%Mid == 10126)||(%Mid == 10127)||(%Mid == 10128) )
					if(%Player.GetFamily() == 0)
						continue;

				%MissionData	= "MissionData_" @ %Mid;
				%kind       	= GetMissionKind(%Mid);
				%Mid_LeiXin 	= $MissionKind[ %kind, 1 ];

//				echo("CreateAcceptedMissionNode = %kind = "@%kind);
//				echo("CreateAcceptedMissionNode = %Mid_LeiXin = "@%Mid_LeiXin);
//				echo("CreateAcceptedMissionNode = %MissionData = "@%MissionData);

				%rootnode = AddCanAcceptTreeNode(0, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %kind * 10, 0);
				if(%rootnode != 0)
					{

						if( (%Mid == 20001)||(%Mid == 10124)||(%Mid == 10125)||(%Mid == 10126)||(%Mid == 10127)||(%Mid == 10128) )
							{
								if(%Player.GetFamily() == 1){%Map = 1011;if(%Mid == 20001){%NpcA = 401011006;}if(%Mid == 10124){%NpcA = 410107001;}if(%Mid == 10125){%NpcA = 401011002;}if(%Mid == 10126){%NpcA = 401011002;}if(%Mid == 10127){%NpcA = 401011006;}if(%Mid == 10128){%NpcA = 400001038;}} //圣师门NPC
								if(%Player.GetFamily() == 2){%Map = 1006;if(%Mid == 20001){%NpcA = 401006006;}if(%Mid == 10124){%NpcA = 410207001;}if(%Mid == 10125){%NpcA = 401006002;}if(%Mid == 10126){%NpcA = 401006002;}if(%Mid == 10127){%NpcA = 401006006;}if(%Mid == 10128){%NpcA = 400001044;}} //佛师门NPC
								if(%Player.GetFamily() == 3){%Map = 1007;if(%Mid == 20001){%NpcA = 401007006;}if(%Mid == 10124){%NpcA = 410307001;}if(%Mid == 10125){%NpcA = 401007002;}if(%Mid == 10126){%NpcA = 401007002;}if(%Mid == 10127){%NpcA = 401007006;}if(%Mid == 10128){%NpcA = 400001039;}} //仙师门NPC
								if(%Player.GetFamily() == 4){%Map = 1005;if(%Mid == 20001){%NpcA = 400001042;}if(%Mid == 10124){%NpcA = 410407001;}if(%Mid == 10125){%NpcA = 401005002;}if(%Mid == 10126){%NpcA = 401005002;}if(%Mid == 10127){%NpcA = 401005006;}if(%Mid == 10128){%NpcA = 400001042;}} //精师门NPC
								if(%Player.GetFamily() == 5){%Map = 1009;if(%Mid == 20001){%NpcA = 401009006;}if(%Mid == 10124){%NpcA = 410507001;}if(%Mid == 10125){%NpcA = 401009002;}if(%Mid == 10126){%NpcA = 401009002;}if(%Mid == 10127){%NpcA = 401009006;}if(%Mid == 10128){%NpcA = 400001041;}} //鬼师门NPC
								if(%Player.GetFamily() == 6){%Map = 1010;if(%Mid == 20001){%NpcA = 401010006;}if(%Mid == 10124){%NpcA = 410607001;}if(%Mid == 10125){%NpcA = 401010002;}if(%Mid == 10126){%NpcA = 401010002;}if(%Mid == 10127){%NpcA = 401010006;}if(%Mid == 10128){%NpcA = 400001040;}} //怪师门NPC
								if(%Player.GetFamily() == 7){%Map = 1008;if(%Mid == 20001){%NpcA = 400001043;}if(%Mid == 10124){%NpcA = 410707001;}if(%Mid == 10125){%NpcA = 401008002;}if(%Mid == 10126){%NpcA = 401008002;}if(%Mid == 10127){%NpcA = 401008006;}if(%Mid == 10128){%NpcA = 400001043;}} //妖师门NPC
								if(%Player.GetFamily() == 8){%Map = 1004;if(%Mid == 20001){%NpcA = 400001045;}if(%Mid == 10124){%NpcA = 410807001;}if(%Mid == 10125){%NpcA = 401004002;}if(%Mid == 10126){%NpcA = 401004002;}if(%Mid == 10127){%NpcA = 401004006;}if(%Mid == 10128){%NpcA = 400001045;}} //魔师门NPC
             }
							else
								{
									%NpcA = %MissionData.NpcA;
									%Map  = %MissionData.Map;
								}
						echo("Map = "@%Map);

						%Mid_Map  = $Mission_Map[%Map];
						%XYZ      = GetRoute_Position(%NpcA);
						%Map_XYZ1 = Pos3DTo2D(%Map, GetWord(%XYZ,0), GetWord(%XYZ,1));
						%Map_XYZ2 = GetWord(%Map_XYZ1,0) @ "," @ GetWord(%Map_XYZ1,1);

						//任务名称
						%Mission_Name = AddCanAcceptTreeNode(%rootnode, $Get_Dialog_GeShi[31204] @ GetMission_Name(%Player , %Mid , 1) @ "</t>", %Mid, 0);

						//任务接受NPC
						%Txt = $Get_Dialog_GeShi[31201] @ %Mid_Map @ "</t> " @ "<c cid='" @ Get_TraceButton() @ "' cmd='\"findpath(" @ %NpcA @ ");\",\"" @ $Get_Dialog_GeShi[31214] @ GetNpcData(%NpcA,1) @ "(" @ %Map_XYZ2 @ ")</t>\",\"GuiMissionTraceButtonProfile6\"' cf='createButton' />";
						AddCanAcceptTreeNode(%Mission_Name, %Txt, %Mid * 10, 0);
					}
			}
	}
}

// 在任务浏览界面的共享任务树上创建一个任务的树结点
function CreateShareMissionNode(%Player, %Mid, %Mission_Map)
{
	%MissionData = "MissionData_" @ %Mid;
	%node = SingleWnd1_Tree3.findMissionTreeNode(%Mid);
	if(%node == 0)
	{
		%kind       = GetMissionKind(%Mid);
		%Mid_LeiXin = $MissionKind[ %kind, 1 ];
		%Mid_Map    = $Mission_Map[%MissionData.Map];
		if(%Mid == 20001)
		{
			if(%Player.GetFamily() == 0){%Mid_Map = $Mission_Map[1001];} //测试：无职业
			if(%Player.GetFamily() == 1){%Mid_Map = $Mission_Map[1001];} //圣师门NPC
			if(%Player.GetFamily() == 2){%Mid_Map = $Mission_Map[1001];} //佛师门NPC
			if(%Player.GetFamily() == 3){%Mid_Map = $Mission_Map[1001];} //仙师门NPC
			if(%Player.GetFamily() == 4){%Mid_Map = $Mission_Map[1001];} //精师门NPC
			if(%Player.GetFamily() == 5){%Mid_Map = $Mission_Map[1001];} //鬼师门NPC
			if(%Player.GetFamily() == 6){%Mid_Map = $Mission_Map[1001];} //怪师门NPC
			if(%Player.GetFamily() == 7){%Mid_Map = $Mission_Map[1001];} //妖师门NPC
			if(%Player.GetFamily() == 8){%Mid_Map = $Mission_Map[1001];} //魔师门NPC
		}

		//默认值，所有任务以任务类型创建树
		if(%Mission_Map == 0)
		{
			%rootnode = AddTreeNode3(0, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %kind, 0, 1);
			if(%rootnode != 0)
			{
				if(%kind == 3)
					AddTreeNode3(%rootnode, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>", %Mid, 1, 0);
				else
				{
					//任务归属的地图名称
					%Map = AddTreeNode3(%rootnode, $Get_Dialog_GeShi[31201] @ "【" @ %Mid_Map @ "】</t>", %kind @ %MissionData.Map, 0, 1);
					//任务名称
					AddTreeNode3(%Map, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>", %Mid, 1, 0);
				}
			}
		}

		//修改值，所有任务以地图区分创建树
		if(%Mission_Map == 1)
		{
			if(%kind == 3)
			{
				%rootnode = AddTreeNode3(0, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %kind, 0, 1);
				if(%rootnode != 0)
					AddTreeNode3(%rootnode, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>", %Mid, 1, 0);
			}
			else
			{
				%rootnode = AddTreeNode3(0, $Get_Dialog_GeShi[31201] @ "【" @ %Mid_Map @ "】</t>", %MissionData.Map, 0, 1);
				if(%rootnode != 0)
				{
					//任务类型名称
					%LeiXin = AddTreeNode3(%rootnode, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %MissionData.Map @ %kind, 0, 1);
					//任务名称
					AddTreeNode3(%LeiXin, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>", %Mid, 1, 0);
				}
			}
		}
	}
}

// 在任务追踪界面的已接任务树上创建一个任务结点
function CreateTraceMissionNode(%Player, %Mid)
{
	%node = MissionTraceWnd_Tree.findMissionTreeNode(%Mid);
//	if(%node == 0)
//	{
//		%node = AddTraceTreeNode(0, "<t f='宋体' n='12' c='0xffffffff'>" @ $MissionData[%Mid].Name @ "</t>", %Mid, 1);
//		echo("xxxxx"@GetDialogText(0, %Player, 20003 @ %Mid , 0));
//		echo("xxxxxxxxxxxxxxx"@$MissionData[%Mid].Name);

		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) $= "")
			%Mid_Name = $Get_Dialog_GeShi[50001] @ GetMission_Name(%Player , %Mid , 1) @ " （完成）";
		else
			%Mid_Name = $Get_Dialog_GeShi[50001] @ GetMission_Name(%Player , %Mid , 1);

//		%Txt = "<c cid='ShowMissionInfoButton' cmd='\"MissionTraceHover("@%Mid@");\",\"" @ $Get_Dialog_GeShi[50000] @ GetMission_Name(%Player , %Mid , 1) @ %Mid_Over @ "</t>\",\"GuiMissionTraceButtonProfile4\"' cf='createButton' />";
		%Txt = "<c cid='ShowMissionInfoButton' cmd='\"MissionTraceHover("@%Mid@");\",\"" @ %Mid_Name @ "</t>\",\"GuiMissionTraceButtonProfile4\"' cf='createButton' />";
		//判断是否限时任务
		if(%Player.IsTimeInfo(%Mid))
			{
				%seconds = %Player.GetMissionFlag(%Mid,10) - %Player.GetMissionFlag(%Mid,11);
				%Txt = %Txt @ "<c cid='xclock' cmd='" @ %seconds @ ",\"GuiCurrencyShowInfoTextProfile_1\"' cf='createClockLabel' />";
			}
		%node = AddTraceTreeNode(0, %Txt, %Mid, 0);
//	}
	if(%node != 0)
	{
		//清除所有结点
		MissionTraceWnd_Tree.removeAllChildren(%node);

		%Num = 0;

		for(%i = 0; %i < 7; %i++)
		{
			if(%i == 0){%ii = 1200;}//寻找目标
			if(%i == 1){%ii = 3200;}//杀怪计数
			if(%i == 2){%ii = 2250;}//运输道具
			if(%i == 3){%ii = 2200;}//搜集道具
			if(%i == 4){%ii = 1250;}//寻找特殊目标
			if(%i == 5){%ii = 8000;}//特殊数值
			if(%i == 6){%ii = 9000;}//文字描述
					
			if(%i < 5)
				for(%x = 0; %x < 50; %x++)
				{
					if(%Player.GetMissionFlag(%Mid, %ii + %x) > 0)
						{
							%Num = %Num + 1;
							%NodeId = %Num * 100;
							AddTraceTreeNode(%node, GetMissionNeedText(%Player, %Mid, %ii + %x), %Mid @ %NodeId, 0);
						}
						else
							break;
				}

			if(%i == 5)
				for(%x = 0; %x < 50; %x++)
				{
					if(%Player.GetMissionFlag(%Mid, %ii + %x) > 0)
						{
							%Num = %Num + 1;
							%NodeId = %Num * 100;
							AddTraceTreeNode(%node, GetMissionNeedText(%Player, %Mid, %ii + %x), %Mid @ %NodeId, 0);
						}
						else
							break;
				}

			if(%i == 6)
				for(%x = 0; %x < 50; %x++)
				{
					if(%Player.GetMissionFlag(%Mid, %ii + %x) > 0)
						{
							%Num = %Num + 1;
							%NodeId = %Num * 100;
							AddTraceTreeNode(%node, GetMissionNeedText(%Player, %Mid, %ii + %x), %Mid @ %NodeId, 0);
						}
						else
							break;
				}
		}
		//交任务目标
		AddTraceTreeNode(%node, GetMission_Flag(%Player , %Mid , "Npc" , 99 , 4), %Mid @ 990, 0);
	}
}

// 添加一个任务的GUI操作
function UIAddOneMission(%Player, %Mid)
{
	// 若任务日志界面是可见的
	if(MissionWndGui_EntireWnd.isVisible())
	{
		// 任务日志界面默认已接任务树可见
		SingleWnd1_ScrollCtrl1.setvisible(1);
		SingleWnd1_ScrollCtrl2.setvisible(0);

		// 刷新UI的已接任务数量
		RefreshMissionNum(%Player.GetAcceptedMission());

		// 如果将任务以地图归类按钮打开
		if(isOrderByMap())
			%Mission_Map = 1;
		else
			%Mission_Map = 0;

		// 在已接任务树上创建一个任务的树结点
		CreateAcceptedMissionNode(%Player, %Mid, %Mission_Map);

		// 缺省展开已接任务树
		SingleWnd1_Tree1.expandToItem(0,1);

		// 若“任务信息”界面是可见的，刷新任务描述
		if(SingleWnd2_ShowMissionInfo.isVisible())
		{
			RefreshMissionDesc(%Mid);
		}
	}

	// 若任务追踪界面是可视的
	if(MissionTraceWnd.isVisible() && MissionWndGui_MissionTraceCheckBox.IsStateOn)
	{
		// 在任务追踪树上创建一个任务结点
		CreateTraceMissionNode(%Player, %Mid);
		// 缺省展开追踪树
		MissionTraceWnd_Tree.expandToItem(0,1);

		// 从可接任务树上移除一个任务结点
		%node = MissionTraceWnd_CanSelectMissionTree.findMissionTreeNode(%Mid);
		if(%node != 0)
			MissionTraceWnd_CanSelectMissionTree.clearChildItem(%node);
	}
}

// 更新一个任务的GUI操作
function UIUpdateOneMission(%Player, %Mid)
{
	// 若任务日志界面是可见的
	if(MissionWndGui_EntireWnd.isVisible())
	{
		// 任务日志界面默认已接任务树可见
		SingleWnd1_ScrollCtrl1.setvisible(1);
		SingleWnd1_ScrollCtrl2.setvisible(0);
		// 缺省展开已接任务树
		SingleWnd1_Tree1.expandToItem(0,1);

		// 若“任务信息”界面是可视的
		if(SingleWnd2_ShowMissionInfo.isVisible())
		{
			 RefreshMissionDesc(%Mid);
		}
	}

	// 若任务追踪界面是可视的
	if(MissionTraceWnd.isVisible())
	{
		// 在任务追踪树上创建一个任务结点
		CreateTraceMissionNode(%Player, %Mid);
		// 缺省展开追踪树
		MissionTraceWnd_Tree.expandToItem(0,1);

		// 从可接任务树上移除一个任务结点
		%node = MissionTraceWnd_CanSelectMissionTree.findMissionTreeNode(%Mid);
		if(%node != 0)
			MissionTraceWnd_CanSelectMissionTree.clearChildItem(%node);
	}
}

// 删除一个任务的GUI操作
function UIDelOneMission(%Player, %Mid)
{
	if($CurrentMissionID == %Mid)
		$CurrentMissionID = -1;
	// 若任务日志界面是可见的
	if(MissionWndGui_EntireWnd.isVisible())
	{
		// 任务日志界面默认已接任务树可见
		SingleWnd1_ScrollCtrl1.setvisible(1);
		SingleWnd1_ScrollCtrl2.setvisible(0);
		// 刷新UI的已接任务数量
		RefreshMissionNum(%Player.GetAcceptedMission());

		// 从已接任务树上移除一个任务结点
		%node = SingleWnd1_Tree1.findMissionTreeNode(%Mid);
	 	SingleWnd1_Tree1.clearChildItem(%node);

		// 缺省展开已接任务树
		SingleWnd1_Tree1.expandToItem(0,1);

		// 若“任务信息”界面是可视的
		if(SingleWnd2_ShowMissionInfo.isVisible())
			RefreshMissionDesc(%Mid);
	}

	// 若任务追踪界面是可视的
	if(MissionTraceWnd.isVisible())
	{
		// 从任务追踪树上移除一个任务结点
		%node = MissionTraceWnd_Tree.findMissionTreeNode(%Mid);
//		echo("%node = "@%node);
		MissionTraceWnd_Tree.clearChildItem(%node);

		// 在任务可接树上创建一个任务结点
		CreateCanAcceptMissionNode(%Player, 0);
		// 展开任务可接树
		MissionTraceWnd_CanSelectMissionTree.expandToItem(0,1);

		// 缺省展开追踪树
		MissionTraceWnd_Tree.expandToItem(0,1);

		//如果已接任务数量为0,追踪切换到可接页
//		if(%Player.GetAcceptedMission() == 0)
//			OpenCanSelectMission();
	}
}

// 追踪单个任务
function TraceMission()
{
	if($CurrentMissionID == -1)
		return;

	MissionTraceWnd.setVisible(1);
	%Player = GetPlayer();
	%Mid = $CurrentMissionID;
	// 在任务追踪树上创建一个任务结点
	CreateTraceMissionNode(%Player, %Mid);

	// 从可接任务树上移除一个任务结点
	%node = MissionTraceWnd_CanSelectMissionTree.findMissionTreeNode(%Mid);
	if(%node != 0)
		MissionTraceWnd_CanSelectMissionTree.clearChildItem(%node);

	// 缺省展开追踪树
	MissionTraceWnd_Tree.expandToItem(0,1);
}

// 刷新任务浏览界面的已接任务列表
function RefreshAcceptedMission(%Player)
{
	// 先清空树，重置树的列表
	Singlewnd1_Tree1.clearitem();

	// 任务已接数量
	%AcceptCount = %Player.GetAcceptedMission();

	// 刷新UI的已接任务数量
	RefreshMissionNum(%AcceptCount);
	// 刷新UI的任务描述,读取任务信息，执行GetMissionText函数
	RefreshMissionDesc(-1);

	// 如果将任务以地图归类按钮打开
	if(isOrderByMap())
		%Mission_Map = 1;
	else
		%Mission_Map = 0;

	// 遍历玩家所有已接任务，添加到树列表
	for(%i = 0; %i < %AcceptCount; %i++)
	{
		%Mid = %Player.GetMission(%i);
		%kind = GetMissionKind(%Mid);

//		%ZoneID = GetZoneID();
		// 在已接任务树上创建一个任务的树结点
		CreateAcceptedMissionNode(%Player, %Mid, %Mission_Map);
	}
	// 缺省展开已接任务树
	SingleWnd1_Tree1.expandToItem(0,1);
}

// 刷新任务浏览界面的可接任务列表
function RefreshCanAcceptMission(%Player)
{
	MissionTraceWnd_CanSelectMissionTree.clearitem();

//	%AcceptCount = %Player.GetAcceptedMission();

	// 遍历玩家所有已接任务，添加到树列表
//	for(%i = 0; %i < %AcceptCount; %i++)
//	{
//		%Mid = %Player.GetMission(%i);
		// 在任务追踪树上创建一个任务结点
		CreateCanAcceptMissionNode(%Player, 0);
//	}
//	if(%AcceptCount != 0)
		MissionTraceWnd_CanSelectMissionTree.expandToItem(0,1);
}

// 刷新任务浏览界面的共享任务列表
function RefreshShareMission(%Player)
{
	// 先清空树，重置树的列表
	SingleWnd1_Tree3.clearitem();
	%Count = %Player.GetShareMissionCount();

	// 遍历玩家收集的所有共享任务，添加到树列表
	for(%i = 0; %i < %Count; %i++)
	{
		%Mid = %Player.GetShareMission(%i);
		CreateShareMissionNode(%player, %Mid, 0);
	}
}

// 刷新界面的任务追踪列表
function RefreshTraceMission(%Player)
{
	MissionTraceWnd_Tree.clearitem();

	%AcceptCount = %Player.GetAcceptedMission();

	// 遍历玩家所有已接任务，添加到树列表
	for(%i = 0; %i < %AcceptCount; %i++)
	{
		%Mid = %Player.GetMission(%i);
		// 在任务追踪树上创建一个任务结点
		CreateTraceMissionNode(%Player, %Mid);
	}
	if(%AcceptCount != 0)
		MissionTraceWnd_Tree.expandToItem(0,1);
}

// 获取任务全部内容文本
function GetMissionText(%Player, %Mid)
{
	if( (strlen(%Mid) >= 6)||(strlen(%Mid) <= 1) )
		return "";

	if(strlen(%Mid) <= 4)
		%Mid = "8880" @ %Mid;
	else
		%Mid = "888" @ %Mid;

//	echo("GetMissionText = "@%Mid);
	return GetDialogText(0, %Player, %Mid , 0);
}

// 获取任务需求文本
function GetMissionNeedText(%Player, %Mid, %Type)
{
  if(%Type < 9999)
  	{
  		if( (%Type >= 3200)&&(%Type < 3300)&&(%Player.GetMissionFlag(%Mid,%Type) > 0) )
  			return GetMission_Flag(%Player , %Mid , "MonsterKill" , %Type - 3200 , 5);

  		if( (%Type >= 2250)&&(%Type < 2300)&&(%Player.GetMissionFlag(%Mid,%Type) > 0) )
  			return GetMission_Flag(%Player , %Mid , "ItemAdd" , %Type - 2250 , 5);

  		if( (%Type >= 2200)&&(%Type < 2250)&&(%Player.GetMissionFlag(%Mid,%Type) > 0) )
  			return GetMission_Flag(%Player , %Mid , "ItemGet" , %Type - 2200 , 5);

  		if( (%Type >= 1250)&&(%Type < 1300)&&(%Player.GetMissionFlag(%Mid,%Type) > 0) )
  			return GetMission_Flag(%Player , %Mid , "Npc" , %Type - 1250 , 5);

  		if( (%Type >= 1200)&&(%Type < 1250)&&(%Player.GetMissionFlag(%Mid,%Type) > 0) )
  			return GetMission_Flag(%Player , %Mid , "Target" , %Type - 1200 , 5);

  		if( (%Type >= 8000)&&(%Type < 9000)&&(%Player.GetMissionFlag(%Mid,%Type) > 0) )
  			return GetMission_Flag(%Player , %Mid , "Step" , %Type - 8000, 5);

  		if( (%Type >= 9000)&&(%Type < 10000)&&(%Player.GetMissionFlag(%Mid,%Type) > 0) )
  			return GetMission_Flag(%Player , %Mid , "Test" , %Type - 9000, 5);
  	}
  if(%Type == 9999)
  	{
  		%TxtA = "";

			for(%i = 0; %i < 7; %i++)
			{
				if(%i == 0){%ii = 1200;}//寻找目标
				if(%i == 1){%ii = 3200;}//杀怪计数
				if(%i == 2){%ii = 2250;}//运输道具
				if(%i == 3){%ii = 2200;}//搜集道具
				if(%i == 4){%ii = 1250;}//寻找特殊目标
				if(%i == 5){%ii = 8000;}//特殊数值
				if(%i == 6){%ii = 9000;}//文字描述

				if(%i < 5)
					for(%x = 0; %x < 50; %x++)
					{
						if(%Player.GetMissionFlag(%Mid,%ii + %x) > 0)
							%TxtA = %TxtA @ GetMissionNeedText(%Player, %Mid, %ii + %x) @ "<b/>";
						else
							break;
					}

				if(%i == 5)
					for(%x = 0; %x < 50; %x++)
					{
						if(%Player.GetMissionFlag(%Mid, %ii + %x) > 0)
							%TxtA = %TxtA @ GetMissionNeedText(%Player, %Mid, %ii + %x) @ "<b/>";
						else
							break;
					}
					
				if(%i == 6)
					for(%x = 0; %x < 50; %x++)
					{
						if(%Player.GetMissionFlag(%Mid, %ii + %x) > 0)
							%TxtA = %TxtA @ GetMissionNeedText(%Player, %Mid, %ii + %x) @ "<b/>";
						else
							break;
					}
			}

			%TxtZ = GetMission_Flag(%Player , %Mid , "Npc" , 99 , 4) @ "<b/>";
//			echo("%Type = "@%Type);
//			echo("%Txt1 = "@%Txt1);
			return %TxtA @ %TxtZ;
  	}

	return "GetMissionNeedText = 错误 Player【"@%Player@"】Mid【"@%Mid@"】Type【"@%Type@"】";
}

//获取任务难度
function GetMission_NanDu(%Mid)
{
	%Png1 = isFile("gameres/gui/images/GUIWindow26_1_010.png");
	%Png2 = isFile("gameres/gui/images/GUIWindow26_1_011.png");

	%MissionData = "MissionRewardData_" @ %Mid;

	if((%Mid == 20001)||(%Mid == 20002)||(%Mid == 20003))
		{
			%Player = GetPlayer();

			if(%Player.GetLevel() >= 20){%NanDu1 = 1;}
			if(%Player.GetLevel() >= 40){%NanDu1 = 2;}
			if(%Player.GetLevel() >= 60){%NanDu1 = 3;}
			if(%Player.GetLevel() >= 80){%NanDu1 = 4;}
		}
		else
			%NanDu1 = %MissionData.NanDu;

	%NanDu2 = 9 - %NanDu1;

	%R1 = "";
	%R2 = "";

	if( (%Png1 == 1)&&(%Png2 == 1) )
		{
			for(%i = 0; %i < %NanDu1; %i++)
				%R1 = %R1 @ "<i s='gameres/gui/images/GUIWindow26_1_010.png' w='16' h='16'/>";

			for(%i = 0; %i < %NanDu2; %i++)
				%R2 = %R2 @ "<i s='gameres/gui/images/GUIWindow26_1_011.png' w='16' h='16'/>";

			return %R1 @ %R2;
		}
		else
			{
				for(%i = 0; %i < %NanDu1; %i++)
					%R1 = %R1 @ "★";

				for(%i = 0; %i < %NanDu2; %i++)
					%R2 = %R2 @ "☆";

				return $Get_Dialog_GeShi[31206] @ %R1 @ "</t> <t>" @ %R2 @ "</t>";
			}

	return "【图片数据错误】<b/>";
}

//获取任务完整名称
function GetMission_Name(%Player , %Mid , %Type)
{
	%MissionData = "MissionData_" @ %Mid;
	%Mission_Name = "[" @ %MissionData.JianYi @ "] " @ %MissionData.Name;
//	%Mission_Name = " " @ %MissionData.Name;

	if(%Type == 1)
			 return %Mission_Name;
	else
		return $Get_Dialog_GeShi[31800] @ %Mission_Name @ "</t>";

}

//获取任务各个界面内相关旗标的文字与颜色与格式
function GetMission_Flag(%Player , %Mid , %Type , %Num , %GetBack)
{
//	echo("==========================================");
//	echo("【%Mid = "@%Mid@"】");
//	echo("【%Type = "@%Type@"】");
//	echo("【%Num = "@%Num@"】");
//	echo("【%GetBack = "@%GetBack@"】");

	%MissionData	= "MissionData_" @ %Mid;

	//NPC类
	if(%Type $= "Npc")
		{
			if(%Num == 100){%Id_Type = %MissionData.NpcA;}
			if(%Num == 99 )
				{
					%Id_Type = %MissionData.NpcZ;

					if(%Mid == 10001){%Id_Type = 400001007;}
					if(%Mid == 10122)
						{
							  echo("太白金星");
								%Id_Type = 400001101;
						}

					if(%Mid == 10123)
						{
							echo("各门宗的宗主10118");
							%Id_Type = %Mid;
						}

					if( ( (%Mid >= 10124)&&(%Mid <= 10128) )||(%Mid == 20001) )
						{
						  if(%Player.GetFamily() == 0){if(%Mid == 20001){%Id_Type = 401011006;}if(%Mid == 10124){%Id_Type = 000000000;}if(%Mid == 10125){%Id_Type = 000000000;}if(%Mid == 10126){%Id_Type = 000000000;}if(%Mid == 10127){%Id_Type = 000000000;}} //测试：无职业
							if(%Player.GetFamily() == 1){if(%Mid == 20001){%Id_Type = 401011006;}if(%Mid == 10124){%Id_Type = 401011002;}if(%Mid == 10125){%Id_Type = 401011002;}if(%Mid == 10126){%Id_Type = 401011006;}if(%Mid == 10127){%Id_Type = 400001038;}} //圣师门NPC
							if(%Player.GetFamily() == 2){if(%Mid == 20001){%Id_Type = 401006006;}if(%Mid == 10124){%Id_Type = 401006002;}if(%Mid == 10125){%Id_Type = 401006002;}if(%Mid == 10126){%Id_Type = 401006006;}if(%Mid == 10127){%Id_Type = 400001044;}} //佛师门NPC
							if(%Player.GetFamily() == 3){if(%Mid == 20001){%Id_Type = 401007006;}if(%Mid == 10124){%Id_Type = 401007002;}if(%Mid == 10125){%Id_Type = 401007002;}if(%Mid == 10126){%Id_Type = 401007006;}if(%Mid == 10127){%Id_Type = 400001039;}} //仙师门NPC
							if(%Player.GetFamily() == 4){if(%Mid == 20001){%Id_Type = 400001042;}if(%Mid == 10124){%Id_Type = 401005002;}if(%Mid == 10125){%Id_Type = 401005002;}if(%Mid == 10126){%Id_Type = 401005006;}if(%Mid == 10127){%Id_Type = 400001042;}} //精师门NPC
							if(%Player.GetFamily() == 5){if(%Mid == 20001){%Id_Type = 401009006;}if(%Mid == 10124){%Id_Type = 401009002;}if(%Mid == 10125){%Id_Type = 401009002;}if(%Mid == 10126){%Id_Type = 401009006;}if(%Mid == 10127){%Id_Type = 400001041;}} //鬼师门NPC
							if(%Player.GetFamily() == 6){if(%Mid == 20001){%Id_Type = 401010006;}if(%Mid == 10124){%Id_Type = 401010002;}if(%Mid == 10125){%Id_Type = 401010002;}if(%Mid == 10126){%Id_Type = 401010006;}if(%Mid == 10127){%Id_Type = 400001040;}} //怪师门NPC
							if(%Player.GetFamily() == 7){if(%Mid == 20001){%Id_Type = 400001043;}if(%Mid == 10124){%Id_Type = 401008002;}if(%Mid == 10125){%Id_Type = 401008002;}if(%Mid == 10126){%Id_Type = 401008006;}if(%Mid == 10127){%Id_Type = 400001043;}} //妖师门NPC
							if(%Player.GetFamily() == 8){if(%Mid == 20001){%Id_Type = 400001045;}if(%Mid == 10124){%Id_Type = 401004002;}if(%Mid == 10125){%Id_Type = 401004002;}if(%Mid == 10126){%Id_Type = 401004006;}if(%Mid == 10127){%Id_Type = 400001045;}} //魔师门NPC

							if(%Mid == 10128)
								%Id_Type = 400001059;
							if(%Mid == 20002)
								%Id_Type = 410000013;
							if(%Mid == 20003)
								%Id_Type = 410000013;
						}
				}
			if(%Num <  99 ){%Id_Type = %Player.GetMissionFlag(%Mid, 1150 + %Num);}

//			echo("%Id_Type = "@%Id_Type);
		}
		else
			{
				if(%Type $= "MonsterKill")	//杀怪数量
					{
						%Id_Type = %Player.GetMissionFlag(%Mid, 3100 + %Num);
						%Id_Num  = %Player.GetMissionFlag(%Mid, 3200 + %Num);
					}
				if(%Type $= "ItemAdd")			//送物品
					{
						%Id_Type	= %Player.GetMissionFlag(%Mid, 2150 + %Num);
						%Id_Num		= %Player.GetMissionFlag(%Mid, 2250 + %Num);
					}
				if(%Type $= "ItemGet")			//取物品
					{
						%Id_Type = %Player.GetMissionFlag(%Mid, 2100 + %Num);
						%Id_Num  = %Player.GetMissionFlag(%Mid, 2200 + %Num);
					}
				if(%Type $= "Target")				//触发类型
					{
						%Id_Type = %Player.GetMissionFlag(%Mid, 1100 + %Num);
						%Id_Num  = %Player.GetMissionFlag(%Mid, 1200 + %Num);
					}
				if(%Type $= "Step")				//特殊数值－学习生活技能
					{
						//学习生活技能
						if(%Mid == 22002){%ID_Type = %Mid;%Id_Num = 8000;}	//钓鱼
						if(%Mid == 22004){%ID_Type = %Mid;%Id_Num = 8000;}	//狩猎
						if(%Mid == 22006){%ID_Type = %Mid;%Id_Num = 8000;}	//采药
						if(%Mid == 22008){%ID_Type = %Mid;%Id_Num = 8000;}	//种植
						if(%Mid == 22010){%ID_Type = %Mid;%Id_Num = 8000;}	//伐木
						if(%Mid == 22012){%ID_Type = %Mid;%Id_Num = 8000;}	//采矿
						if(%Mid == 10000){%ID_Type = %Mid;%Id_Num = 8000;}	//人物等级
					}
			}


//------------------------------------组合部件----------------------------------------↓----

	switch(%GetBack)
	{
		case 99:			//名字
//			echo("道具类ItemAdd=2");
//			echo("%Id_Type == "@%Id_Type);

			//5位的编号
			if(strlen(%Id_Type) == 5)
				{
					echo("各门宗的宗主");
					if(%Mid == 10123)
						return "各门宗的宗主";
//					if(%Mid == 10122)
//						return "太白金星";
//					if(%Mid == 20001)
//						{
//						  if(%Player.GetFamily() == 0){return GetNpcData(400001101,1);} //测试：无职业
//							if(%Player.GetFamily() == 1){return GetNpcData(400001038,1);} //圣师门NPC
//							if(%Player.GetFamily() == 2){return GetNpcData(400001044,1);} //佛师门NPC
//							if(%Player.GetFamily() == 3){return GetNpcData(400001039,1);} //仙师门NPC
//							if(%Player.GetFamily() == 4){return GetNpcData(400001042,1);} //精师门NPC
//							if(%Player.GetFamily() == 5){return GetNpcData(400001041,1);} //鬼师门NPC
//							if(%Player.GetFamily() == 6){return GetNpcData(400001040,1);} //怪师门NPC
//							if(%Player.GetFamily() == 7){return GetNpcData(400001043,1);} //妖师门NPC
//							if(%Player.GetFamily() == 8){return GetNpcData(400001045,1);} //魔师门NPC
//						}
				}
			//9位的编号
			if(strlen(%Id_Type) == 9)
				{
//					echo("道具类ItemAdd=21111111111");
					if(GetSubStr(%Id_Type,0,1) $= "1"){return GetItemData(%Id_Type,1);}	//道具
					if(GetSubStr(%Id_Type,0,1) $= "4")																	//NPC类
						{
							return GetNpcData(%Id_Type,1);
						}
					if(GetSubStr(%Id_Type,0,1) $= "5")                                  //采集
						{
							if(%Id_Type $= "552000056"){return "神兵台";}
							if(%Id_Type $= "552000057"){return "封印灵石";}
              if(%Id_Type $= "552000053"){return "莲花台";}
							if(%Id_Type $= "552000022"){return "巨石一";}
							if(%Id_Type $= "552000023"){return "巨石二";}
							if(%Id_Type $= "552000024"){return "巨石三";}
							if(%Id_Type $= "552000038"){return "碧色珠果";}
							if(%Id_Type $= "552000039"){return "青色珠果";}
							if(%Id_Type $= "552000040"){return "草色珠果";}
							if(%Id_Type $= "552000041"){return "稻草人";}
							if(%Id_Type $= "552000043"){return "凤凰花";}
							if(%Id_Type $= "552000027"){return "空炎";}
							if(%Id_Type $= "552000042"){return "凤凰花精残骸";}
							if(%Id_Type $= "552000044"){return "被囚禁的男人";}
							if(%Id_Type $= "552000028"){return "空炎";}
							if(%Id_Type $= "552000047"){return "干燥的树枝";}


						}
					if(GetSubStr(%Id_Type,0,1) $= "8")																	//区域
						{
							if(%Id_Type $= "810010119"){return "南天柱顶端";}
							if(%Id_Type $= "810010106"){return "日月精气";}
							if(%Id_Type $= "811010103"){return "神水树";}
							if(%Id_Type $= "811030110"){return "西碧珠营地";}
							if(%Id_Type $= "811030111"){return "东碧珠营地";}
							if(%Id_Type $= "811030112"){return "北碧珠营地";}
							if(%Id_Type $= "811030113"){return "花盘附近";}
							if(%Id_Type $= "811030106"){return "南无月灯明佛首";}
							if(%Id_Type $= "811030107"){return "南无月灯光佛首";}
							if(%Id_Type $= "811030108"){return "南无智灯明佛首";}
						}
				}

//			if(%Type $= "Npc"        ){return GetNpcData(%Id_Type,1);}	//NPC类
//			if(%Type $= "MonsterKill"){return GetNpcData(%Id_Type,1);}	//NPC类
//			if(%Type $= "ItemGet"    ){return GetItemData(%Id_Type,1);}	//道具
//			if(%Type $= "ItemAdd"    ){return GetItemData(%Id_Type,1);}	//道具
//			if(%Type $= "Target"     ){return "xxxxxxxxxxxx";}					//区域

			return "";
		case 98:			//地图
			return $Get_Dialog_GeShi[31203] @ $Mission_Map[ %MissionData.Map ] @ "</t>";
//		return $Get_Dialog_GeShi[31203] @ $Mission_Map[ GetRoute_MapID( %Id_Type ) ] @ "</t>";


		case 97:			//坐标，有白色下划线与寻径按钮
			%XYZ      = GetRoute_Position(%Id_Type);
			%Map      = GetSubStr(GetRoute_MapID(%Id_Type),1,4);
			%Map_XYZ1 = Pos3DTo2D(%MissionData.Map, GetWord(%XYZ,0), GetWord(%XYZ,1));
			%Map_XYZ2 = GetWord(%Map_XYZ1,0) @ "," @ GetWord(%Map_XYZ1,1);

			return "<c cid='" @ Get_TraceButton() @ "' cmd='\"findpath("@%Id_Type@");\",\"" @ $Get_Dialog_GeShi[50000] @ "(" @ %Map_XYZ2 @ ")</t>\",\"GuiMissionTraceButtonProfile3\"' cf='createButton' />";

		case 96:			//数量，白色字体
//			echo("道具类ItemAdd=3");
			if(%Type $= "Npc"        ){%NumId = 1050;}	//NPC
			if(%Type $= "MonsterKill"){%NumId = 3000;}	//怪物
			if(%Type $= "ItemGet"    ){%NumId = 2000;}	//道具
			if(%Type $= "ItemAdd"    ){%NumId = 2050;}	//道具
			if(%Type $= "Target"     ){%NumId = 1000;}	//触发数

//			if( (%Type $= "ItemGet")||(%Type $= "ItemAdd") )
//				%KillNumMin = %Player.GetItemCount(%Id_Type);	//背包内的当前道具数量
//			else
			%KillNumMin = %Player.GetMissionFlag(%Mid,%NumId + %Num + 300);//当前数量
			%KillNumMax = %Player.GetMissionFlag(%Mid,%NumId + %Num + 200);//最大数量

//			if(%KillNumMin < %KillNumMax)
//				%FlagOver = "";
//			else
//				%FlagOver	= "(完成)";
//				%FlagOver	= $Get_Dialog_GeShi[31204] @ "(完成)</t><b/>";

//			return " " @ %KillNumMin @ " / " @ %KillNumMax @ "</t> " @ %FlagOver;
			return " " @ %KillNumMin @ "/" @ %KillNumMax @ "</t>";

		case 95:			//目标执行动作
//			echo("道具类ItemAdd=4");
			if(%Type $= "Npc"        )									//NPC
				{
					if(%Mid == 10103){return "开启：";	}
					if(%Mid == 10109){return "传送副本：";	}
					return "对话：";
				}
			if(%Type $= "MonsterKill")
				{
					if((%Id_Type $= "410701005" )&&(%Mid ==10385)){return "摧毁：";	}

					return "击杀：";
				}	//怪物
			if(%Type $= "ItemAdd"    )
				{
					if((%Id_Type $= "108020060" )&&(%Mid ==10151)){return "查看：";	}
					return "物品：";
				}	//道具
			if(%Type $= "ItemGet"    )									//道具
				{
					if((%Id_Type $= "107019001" )&&(%Mid ==10114)){return "购买：";	}
					if((%Id_Type $= "108020267" )&&(%Mid ==10114)){return "采集：";	}
					if(%Mid == 10700){return "购买：";	}
					if(%Mid == 10185){return "吸取：";	}
					if(%Mid == 10303){return "寻找：";	}
					if(%Mid == 10305){return "烹制：";	}

					return "物品：";
				}
			if(%Type $= "Target"     )									//目标
				{
					if(%Mid == 10114){return "采集：";	}
					if(%Mid == 10116){return "吸取：";	}
					if(%Mid == 10120){return "跳上：";	}
					if(%Mid == 10188){return "目标：";	}
					if((%Mid == 10510)||(%Mid == 10522)){return "观察：";	}
					if(%Mid == 10544){return "解救：";	}
					if(%Mid == 10547){return "寻找：";	}
					if(%Mid == 10344){return "召唤：";	}
					if(%Mid == 10787){return "开启：";	}
					return "使用：";
			  }
			if(%Type $= "Step"       )				//特殊数值
				{
					if(%Mid == 22002){return "学习生活技能：钓鱼";}
					if(%Mid == 22004){return "学习生活技能：狩猎";}
					if(%Mid == 22006){return "学习生活技能：采药";}
					if(%Mid == 22008){return "学习生活技能：种植";}
					if(%Mid == 22010){return "学习生活技能：伐木";}
					if(%Mid == 22012){return "学习生活技能：采矿";}
					if(%Mid == 10000){return "人物达到等级：";}
				}
			if(%Type $= "Test"       )				//特殊数值
				{
					if(%Mid == 10001){return "宠物达到10级";}
				}


//------------------------------------组合部件----------------------------------------↑----

//此处开始组合

		case 1:			//返回：有颜色的名字
//			echo("道具类ItemAdd=5");
			if(%Type $= "Npc"        ){%Color = 31203;}//NPC
			if(%Type $= "MonsterKill"){%Color = 31206;}//怪物
			if(%Type $= "ItemAdd"    ){%Color = 31203;}//道具
			if(%Type $= "ItemGet"    ){%Color = 31203;}//道具
			if(%Type $= "Target"     ){%Color = 31203;}//目标

//			echo($Get_Dialog_GeShi[ %Color ] @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ "</t>");

			return $Get_Dialog_GeShi[ %Color ] @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ "</t>";

		case 2:			//返回：位于+地名+的+有颜色的NPC名+寻径坐标
			return "<t>位于</t>" @ GetMission_Flag(%Player , %Mid , %Type , %Num , 98) @ "<t>的</t>" @ GetMission_Flag(%Player , %Mid , %Type , %Num , 1) @ GetMission_Flag(%Player , %Mid , %Type , %Num , 97);

		case 3:			//返回：已杀死/已拾取/已对话+白色的NPC名+数量
			return $Get_Dialog_GeShi[31201] @ GetMission_Flag(%Player , %Mid , %Type , %Num , 95) @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ GetMission_Flag(%Player , %Mid , %Type , %Num , 96);

		case 4:			//返回：交任务NPC
			if(CanDoThisMission(%Player, %Mid, 2, 0, 0) $= "")
				%Over = $Get_Dialog_GeShi[31203] @ " √</t>";
			else
				%Over = $Get_Dialog_GeShi[31201] @ " ·</t>";

			return %Over @ $Get_Dialog_GeShi[31201] @ "交任务：</t>" @ "<c cid='" @ Get_TraceButton() @ "' cmd='\"findpath("@%Id_Type@");\",\"" @ $Get_Dialog_GeShi[31214] @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ "</t>\",\"GuiMissionTraceButtonProfile6\"' cf='createButton' /><b/>";

		case 5:			//返回：已杀死/已拾取/已对话+白色的NPC名+数量+寻径格式
//			%Back = GetMission_Flag(%Player , %Mid , %Type , %Num , 95) @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ GetMission_Flag(%Player , %Mid , %Type , %Num , 96);

			//计算此条件是否满足，因为要变色
			if(%Type $= "Npc"        ){%NumId = 1050;}	//NPC
			if(%Type $= "MonsterKill"){%NumId = 3000;}	//怪物
			if(%Type $= "ItemGet"    ){%NumId = 2000;}	//道具
			if(%Type $= "ItemAdd"    ){%NumId = 2050;}	//道具
			if(%Type $= "Target"     ){%NumId = 1000;}	//触发数

			//特殊数值
			if(%Type $= "Step")
				{
					%KillNumMin = %Player.GetMissionFlag(%Mid, 8100 + %Num);//当前数量
					%KillNumMax = %Player.GetMissionFlag(%Mid, 8000 + %Num);//最大数量
				}
				else
					{
						%KillNumMin = %Player.GetMissionFlag(%Mid, %NumId + %Num + 300);//当前数量
						%KillNumMax = %Player.GetMissionFlag(%Mid, %NumId + %Num + 200);//最大数量
					}

			//寻径编号
			%XYZ_ID = %Id_Type;

			if(strlen(%Id_Type) == 5)
				{
					if(%Mid == 22002){%XYZ_ID = 400001019;}	//钓鱼
					if(%Mid == 22004){%XYZ_ID = 400001022;}	//狩猎
					if(%Mid == 22006){%XYZ_ID = 400001021;}	//采药
					if(%Mid == 22008){%XYZ_ID = 400001020;}	//种植
					if(%Mid == 22010){%XYZ_ID = 400001018;}	//伐木
					if(%Mid == 22012){%XYZ_ID = 400001017;}	//采矿
				}
			if(strlen(%Id_Type) == 9)
				if(GetSubStr(%Id_Type,0,1) $= "1")
					{
						//任务道具
						if((GetItemData(%Id_Type,2) == 8)||(GetItemData(%Id_Type,2) == 7)||(GetItemData(%Id_Type,2) == 6))
							{
								%Mon_Num_Max = GetWordCount( $Item_Monster[ %Id_Type ] );
								%Mon_Nun     = GetRandom(1, %Mon_Num_Max);

								%XYZ_ID = GetWord( $Item_Monster[ %Id_Type ], %Mon_Nun - 1 );
							}
						else
							%XYZ_ID = %Id_Type;	//杂物道具
					}

			//计数问题
			if(%KillNumMin < %KillNumMax)
				{
					%ColorA = 31201;	//白色
					%ColorB = 31214;	//草绿色+下划线
					%OK = " ·";
				}
				else
					{
						%ColorA = 31203;	//绿色无下划线
						%ColorB = 31203;	//绿色无下划线
						%OK = " √";
					}

			//特殊旗标不需要计数也不需要寻径
			if(%Type $= "Test")
				return $Get_Dialog_GeShi[ 31201 ] @ " ·" @ GetMission_Flag(%Player , %Mid , %Type , %Num , 95) @ "</t>";
			else
				return $Get_Dialog_GeShi[ %ColorA ] @ %OK @ GetMission_Flag(%Player , %Mid , %Type , %Num , 95) @ "</t>" @ "<c cid='" @ Get_TraceButton() @ "' cmd='\"findpath("@%XYZ_ID@");\",\"" @ $Get_Dialog_GeShi[ %ColorB ] @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ "</t> \",\"GuiMissionTraceButtonProfile6\"' cf='createButton' />" @ $Get_Dialog_GeShi[ %ColorA ] @ %KillNumMin @ "/" @ %KillNumMax @ "</t>";
			
	}
//	return "GetMission_Flag = 错误【%Player = "@%Player@"】【%Mid = "@%Mid@"】【%Type = "@%Type@"】【%Num = "@%Num@"】【%GetBack = "@%GetBack@"】";

//	echo( "GetMission_Flag = 错误【%Player = "@%Player@"】【%Mid = "@%Mid@"】【%Type = "@%Type@"】【%Num = "@%Num@"】【%GetBack = "@%GetBack@"】");
	return "【1="@%Player@"】【2="@%Mid@"】【3="@%Type@"】【4="@%Num@"】【5="@%GetBack@"】";
}

function CheckDialogState(%state)
{
  %MidState = GetSubStr(%State,0,3);
	%Mid      = GetSubStr(%State,3,5);
	%checkok  = "";

	if(%MidState == 110)
		%checkok = CanDoThisMission(GetPlayer(), %Mid, 1, 1, 1);

	if(%MidState == 910)
		%checkok = CanDoThisMission(GetPlayer(), %Mid, 2, 1, 1);

  return %checkok;
}
