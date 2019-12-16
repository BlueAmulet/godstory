//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//�ͻ���������ؽ�������ʾ���ı�
//==================================================================================

//ˢ�������Ի����UI����
function RefreshMission(%Player, %op, %Mid)
{
	switch(%op)
	{
		case 1: // ���һ������
			UIAddOneMission(%Player, %Mid);
		case 2:	// ����һ������
			UIUpdateOneMission(%Player, %Mid);
		case 4: // ɾ��һ������
			UIDelOneMission(%Player, %Mid);
		case 6:	// ���һ������
			UIDelOneMission(%Player, %Mid);
		case 7: // ����������������
		  RefreshAcceptedMission(%Player);
		  RefreshTraceMission(%Player);
		  RefreshCanAcceptMission(%Player);
		case 12: // �������й�����������
			RefreshShareMission(%Player);
		default:
			echo("RefreshMission Error");
	}
}

// ���������������ѽ��������ϴ���һ������������
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
					if(%Player.GetFamily() == 0){%Mid_Map = $Mission_Map[1001];} //���ԣ���ְҵ
					if(%Player.GetFamily() == 1){%Mid_Map = $Mission_Map[1001];} //ʥʦ��NPC
					if(%Player.GetFamily() == 2){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
					if(%Player.GetFamily() == 3){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
					if(%Player.GetFamily() == 4){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
					if(%Player.GetFamily() == 5){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
					if(%Player.GetFamily() == 6){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
					if(%Player.GetFamily() == 7){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
					if(%Player.GetFamily() == 8){%Mid_Map = $Mission_Map[1001];} //ħʦ��NPC
				}

//			echo("CreateAcceptedMissionNode = %kind = "@%kind);
//			echo("CreateAcceptedMissionNode = %Mid_LeiXin = "@%Mid_LeiXin);
//			echo("CreateAcceptedMissionNode = %Mid_Map = "@%Mid_Map);
//			echo("CreateAcceptedMissionNode = %Mission_Map = "@%Mission_Map);

			if(CanDoThisMission(%Player, %Mid, 2, 0, 0) $= "")
				if(%Player.GetMissionFlag(%Mid,8000)!=7)
					%Mid_Over = $Get_Dialog_GeShi[31204] @ " ����ɣ�</t>";
				else
					%Mid_Over = "";

			//Ĭ��ֵ�������������������ʹ�����
			if(%Mission_Map == 0)
				{
					%rootnode = AddTreeNode1(0, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %kind, 0, 1);
					if(%rootnode != 0)
						{
							if(%kind == 3)
								AddTreeNode1(%rootnode, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>" @ %Mid_Over, %Mid, 1, 0);
							else
								{
									//��������ĵ�ͼ����
									%Map = AddTreeNode1(%rootnode, $Get_Dialog_GeShi[31201] @ "��" @ %Mid_Map @ "��</t>", %kind @ %MissionData.Map, 0, 1);
									//��������
									AddTreeNode1(%Map, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>" @ %Mid_Over, %Mid, 1, 0);
								}
						}
				}

			//�޸�ֵ�����������Ե�ͼ���ִ�����
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
								%rootnode = AddTreeNode1(0, $Get_Dialog_GeShi[31201] @ "��" @ %Mid_Map @ "��</t>", %MissionData.Map, 0, 1);
								if(%rootnode != 0)
									{
										//������������
										%LeiXin = AddTreeNode1(%rootnode, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %MissionData.Map @ %kind, 0, 1);
										//��������
										AddTreeNode1(%LeiXin, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>" @ %Mid_Over, %Mid, 1, 0);
									}
							}
				}
		}
}

// �������������Ŀɽ��������ϴ���һ��������
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
								if(%Player.GetFamily() == 1){%Map = 1011;if(%Mid == 20001){%NpcA = 401011006;}if(%Mid == 10124){%NpcA = 410107001;}if(%Mid == 10125){%NpcA = 401011002;}if(%Mid == 10126){%NpcA = 401011002;}if(%Mid == 10127){%NpcA = 401011006;}if(%Mid == 10128){%NpcA = 400001038;}} //ʥʦ��NPC
								if(%Player.GetFamily() == 2){%Map = 1006;if(%Mid == 20001){%NpcA = 401006006;}if(%Mid == 10124){%NpcA = 410207001;}if(%Mid == 10125){%NpcA = 401006002;}if(%Mid == 10126){%NpcA = 401006002;}if(%Mid == 10127){%NpcA = 401006006;}if(%Mid == 10128){%NpcA = 400001044;}} //��ʦ��NPC
								if(%Player.GetFamily() == 3){%Map = 1007;if(%Mid == 20001){%NpcA = 401007006;}if(%Mid == 10124){%NpcA = 410307001;}if(%Mid == 10125){%NpcA = 401007002;}if(%Mid == 10126){%NpcA = 401007002;}if(%Mid == 10127){%NpcA = 401007006;}if(%Mid == 10128){%NpcA = 400001039;}} //��ʦ��NPC
								if(%Player.GetFamily() == 4){%Map = 1005;if(%Mid == 20001){%NpcA = 400001042;}if(%Mid == 10124){%NpcA = 410407001;}if(%Mid == 10125){%NpcA = 401005002;}if(%Mid == 10126){%NpcA = 401005002;}if(%Mid == 10127){%NpcA = 401005006;}if(%Mid == 10128){%NpcA = 400001042;}} //��ʦ��NPC
								if(%Player.GetFamily() == 5){%Map = 1009;if(%Mid == 20001){%NpcA = 401009006;}if(%Mid == 10124){%NpcA = 410507001;}if(%Mid == 10125){%NpcA = 401009002;}if(%Mid == 10126){%NpcA = 401009002;}if(%Mid == 10127){%NpcA = 401009006;}if(%Mid == 10128){%NpcA = 400001041;}} //��ʦ��NPC
								if(%Player.GetFamily() == 6){%Map = 1010;if(%Mid == 20001){%NpcA = 401010006;}if(%Mid == 10124){%NpcA = 410607001;}if(%Mid == 10125){%NpcA = 401010002;}if(%Mid == 10126){%NpcA = 401010002;}if(%Mid == 10127){%NpcA = 401010006;}if(%Mid == 10128){%NpcA = 400001040;}} //��ʦ��NPC
								if(%Player.GetFamily() == 7){%Map = 1008;if(%Mid == 20001){%NpcA = 400001043;}if(%Mid == 10124){%NpcA = 410707001;}if(%Mid == 10125){%NpcA = 401008002;}if(%Mid == 10126){%NpcA = 401008002;}if(%Mid == 10127){%NpcA = 401008006;}if(%Mid == 10128){%NpcA = 400001043;}} //��ʦ��NPC
								if(%Player.GetFamily() == 8){%Map = 1004;if(%Mid == 20001){%NpcA = 400001045;}if(%Mid == 10124){%NpcA = 410807001;}if(%Mid == 10125){%NpcA = 401004002;}if(%Mid == 10126){%NpcA = 401004002;}if(%Mid == 10127){%NpcA = 401004006;}if(%Mid == 10128){%NpcA = 400001045;}} //ħʦ��NPC
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

						//��������
						%Mission_Name = AddCanAcceptTreeNode(%rootnode, $Get_Dialog_GeShi[31204] @ GetMission_Name(%Player , %Mid , 1) @ "</t>", %Mid, 0);

						//�������NPC
						%Txt = $Get_Dialog_GeShi[31201] @ %Mid_Map @ "</t> " @ "<c cid='" @ Get_TraceButton() @ "' cmd='\"findpath(" @ %NpcA @ ");\",\"" @ $Get_Dialog_GeShi[31214] @ GetNpcData(%NpcA,1) @ "(" @ %Map_XYZ2 @ ")</t>\",\"GuiMissionTraceButtonProfile6\"' cf='createButton' />";
						AddCanAcceptTreeNode(%Mission_Name, %Txt, %Mid * 10, 0);
					}
			}
	}
}

// �������������Ĺ����������ϴ���һ������������
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
			if(%Player.GetFamily() == 0){%Mid_Map = $Mission_Map[1001];} //���ԣ���ְҵ
			if(%Player.GetFamily() == 1){%Mid_Map = $Mission_Map[1001];} //ʥʦ��NPC
			if(%Player.GetFamily() == 2){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
			if(%Player.GetFamily() == 3){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
			if(%Player.GetFamily() == 4){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
			if(%Player.GetFamily() == 5){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
			if(%Player.GetFamily() == 6){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
			if(%Player.GetFamily() == 7){%Mid_Map = $Mission_Map[1001];} //��ʦ��NPC
			if(%Player.GetFamily() == 8){%Mid_Map = $Mission_Map[1001];} //ħʦ��NPC
		}

		//Ĭ��ֵ�������������������ʹ�����
		if(%Mission_Map == 0)
		{
			%rootnode = AddTreeNode3(0, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %kind, 0, 1);
			if(%rootnode != 0)
			{
				if(%kind == 3)
					AddTreeNode3(%rootnode, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>", %Mid, 1, 0);
				else
				{
					//��������ĵ�ͼ����
					%Map = AddTreeNode3(%rootnode, $Get_Dialog_GeShi[31201] @ "��" @ %Mid_Map @ "��</t>", %kind @ %MissionData.Map, 0, 1);
					//��������
					AddTreeNode3(%Map, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>", %Mid, 1, 0);
				}
			}
		}

		//�޸�ֵ�����������Ե�ͼ���ִ�����
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
				%rootnode = AddTreeNode3(0, $Get_Dialog_GeShi[31201] @ "��" @ %Mid_Map @ "��</t>", %MissionData.Map, 0, 1);
				if(%rootnode != 0)
				{
					//������������
					%LeiXin = AddTreeNode3(%rootnode, $Get_Dialog_GeShi[31201] @ %Mid_LeiXin @ "</t>", %MissionData.Map @ %kind, 0, 1);
					//��������
					AddTreeNode3(%LeiXin, $Get_Dialog_GeShi[31201] @ GetMission_Name(%Player , %Mid , 1) @ "</t>", %Mid, 1, 0);
				}
			}
		}
	}
}

// ������׷�ٽ�����ѽ��������ϴ���һ��������
function CreateTraceMissionNode(%Player, %Mid)
{
	%node = MissionTraceWnd_Tree.findMissionTreeNode(%Mid);
//	if(%node == 0)
//	{
//		%node = AddTraceTreeNode(0, "<t f='����' n='12' c='0xffffffff'>" @ $MissionData[%Mid].Name @ "</t>", %Mid, 1);
//		echo("xxxxx"@GetDialogText(0, %Player, 20003 @ %Mid , 0));
//		echo("xxxxxxxxxxxxxxx"@$MissionData[%Mid].Name);

		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) $= "")
			%Mid_Name = $Get_Dialog_GeShi[50001] @ GetMission_Name(%Player , %Mid , 1) @ " ����ɣ�";
		else
			%Mid_Name = $Get_Dialog_GeShi[50001] @ GetMission_Name(%Player , %Mid , 1);

//		%Txt = "<c cid='ShowMissionInfoButton' cmd='\"MissionTraceHover("@%Mid@");\",\"" @ $Get_Dialog_GeShi[50000] @ GetMission_Name(%Player , %Mid , 1) @ %Mid_Over @ "</t>\",\"GuiMissionTraceButtonProfile4\"' cf='createButton' />";
		%Txt = "<c cid='ShowMissionInfoButton' cmd='\"MissionTraceHover("@%Mid@");\",\"" @ %Mid_Name @ "</t>\",\"GuiMissionTraceButtonProfile4\"' cf='createButton' />";
		//�ж��Ƿ���ʱ����
		if(%Player.IsTimeInfo(%Mid))
			{
				%seconds = %Player.GetMissionFlag(%Mid,10) - %Player.GetMissionFlag(%Mid,11);
				%Txt = %Txt @ "<c cid='xclock' cmd='" @ %seconds @ ",\"GuiCurrencyShowInfoTextProfile_1\"' cf='createClockLabel' />";
			}
		%node = AddTraceTreeNode(0, %Txt, %Mid, 0);
//	}
	if(%node != 0)
	{
		//������н��
		MissionTraceWnd_Tree.removeAllChildren(%node);

		%Num = 0;

		for(%i = 0; %i < 7; %i++)
		{
			if(%i == 0){%ii = 1200;}//Ѱ��Ŀ��
			if(%i == 1){%ii = 3200;}//ɱ�ּ���
			if(%i == 2){%ii = 2250;}//�������
			if(%i == 3){%ii = 2200;}//�Ѽ�����
			if(%i == 4){%ii = 1250;}//Ѱ������Ŀ��
			if(%i == 5){%ii = 8000;}//������ֵ
			if(%i == 6){%ii = 9000;}//��������
					
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
		//������Ŀ��
		AddTraceTreeNode(%node, GetMission_Flag(%Player , %Mid , "Npc" , 99 , 4), %Mid @ 990, 0);
	}
}

// ���һ�������GUI����
function UIAddOneMission(%Player, %Mid)
{
	// ��������־�����ǿɼ���
	if(MissionWndGui_EntireWnd.isVisible())
	{
		// ������־����Ĭ���ѽ��������ɼ�
		SingleWnd1_ScrollCtrl1.setvisible(1);
		SingleWnd1_ScrollCtrl2.setvisible(0);

		// ˢ��UI���ѽ���������
		RefreshMissionNum(%Player.GetAcceptedMission());

		// ����������Ե�ͼ���ఴť��
		if(isOrderByMap())
			%Mission_Map = 1;
		else
			%Mission_Map = 0;

		// ���ѽ��������ϴ���һ������������
		CreateAcceptedMissionNode(%Player, %Mid, %Mission_Map);

		// ȱʡչ���ѽ�������
		SingleWnd1_Tree1.expandToItem(0,1);

		// ����������Ϣ�������ǿɼ��ģ�ˢ����������
		if(SingleWnd2_ShowMissionInfo.isVisible())
		{
			RefreshMissionDesc(%Mid);
		}
	}

	// ������׷�ٽ����ǿ��ӵ�
	if(MissionTraceWnd.isVisible() && MissionWndGui_MissionTraceCheckBox.IsStateOn)
	{
		// ������׷�����ϴ���һ��������
		CreateTraceMissionNode(%Player, %Mid);
		// ȱʡչ��׷����
		MissionTraceWnd_Tree.expandToItem(0,1);

		// �ӿɽ����������Ƴ�һ��������
		%node = MissionTraceWnd_CanSelectMissionTree.findMissionTreeNode(%Mid);
		if(%node != 0)
			MissionTraceWnd_CanSelectMissionTree.clearChildItem(%node);
	}
}

// ����һ�������GUI����
function UIUpdateOneMission(%Player, %Mid)
{
	// ��������־�����ǿɼ���
	if(MissionWndGui_EntireWnd.isVisible())
	{
		// ������־����Ĭ���ѽ��������ɼ�
		SingleWnd1_ScrollCtrl1.setvisible(1);
		SingleWnd1_ScrollCtrl2.setvisible(0);
		// ȱʡչ���ѽ�������
		SingleWnd1_Tree1.expandToItem(0,1);

		// ����������Ϣ�������ǿ��ӵ�
		if(SingleWnd2_ShowMissionInfo.isVisible())
		{
			 RefreshMissionDesc(%Mid);
		}
	}

	// ������׷�ٽ����ǿ��ӵ�
	if(MissionTraceWnd.isVisible())
	{
		// ������׷�����ϴ���һ��������
		CreateTraceMissionNode(%Player, %Mid);
		// ȱʡչ��׷����
		MissionTraceWnd_Tree.expandToItem(0,1);

		// �ӿɽ����������Ƴ�һ��������
		%node = MissionTraceWnd_CanSelectMissionTree.findMissionTreeNode(%Mid);
		if(%node != 0)
			MissionTraceWnd_CanSelectMissionTree.clearChildItem(%node);
	}
}

// ɾ��һ�������GUI����
function UIDelOneMission(%Player, %Mid)
{
	if($CurrentMissionID == %Mid)
		$CurrentMissionID = -1;
	// ��������־�����ǿɼ���
	if(MissionWndGui_EntireWnd.isVisible())
	{
		// ������־����Ĭ���ѽ��������ɼ�
		SingleWnd1_ScrollCtrl1.setvisible(1);
		SingleWnd1_ScrollCtrl2.setvisible(0);
		// ˢ��UI���ѽ���������
		RefreshMissionNum(%Player.GetAcceptedMission());

		// ���ѽ����������Ƴ�һ��������
		%node = SingleWnd1_Tree1.findMissionTreeNode(%Mid);
	 	SingleWnd1_Tree1.clearChildItem(%node);

		// ȱʡչ���ѽ�������
		SingleWnd1_Tree1.expandToItem(0,1);

		// ����������Ϣ�������ǿ��ӵ�
		if(SingleWnd2_ShowMissionInfo.isVisible())
			RefreshMissionDesc(%Mid);
	}

	// ������׷�ٽ����ǿ��ӵ�
	if(MissionTraceWnd.isVisible())
	{
		// ������׷�������Ƴ�һ��������
		%node = MissionTraceWnd_Tree.findMissionTreeNode(%Mid);
//		echo("%node = "@%node);
		MissionTraceWnd_Tree.clearChildItem(%node);

		// ������ɽ����ϴ���һ��������
		CreateCanAcceptMissionNode(%Player, 0);
		// չ������ɽ���
		MissionTraceWnd_CanSelectMissionTree.expandToItem(0,1);

		// ȱʡչ��׷����
		MissionTraceWnd_Tree.expandToItem(0,1);

		//����ѽ���������Ϊ0,׷���л����ɽ�ҳ
//		if(%Player.GetAcceptedMission() == 0)
//			OpenCanSelectMission();
	}
}

// ׷�ٵ�������
function TraceMission()
{
	if($CurrentMissionID == -1)
		return;

	MissionTraceWnd.setVisible(1);
	%Player = GetPlayer();
	%Mid = $CurrentMissionID;
	// ������׷�����ϴ���һ��������
	CreateTraceMissionNode(%Player, %Mid);

	// �ӿɽ����������Ƴ�һ��������
	%node = MissionTraceWnd_CanSelectMissionTree.findMissionTreeNode(%Mid);
	if(%node != 0)
		MissionTraceWnd_CanSelectMissionTree.clearChildItem(%node);

	// ȱʡչ��׷����
	MissionTraceWnd_Tree.expandToItem(0,1);
}

// ˢ���������������ѽ������б�
function RefreshAcceptedMission(%Player)
{
	// ������������������б�
	Singlewnd1_Tree1.clearitem();

	// �����ѽ�����
	%AcceptCount = %Player.GetAcceptedMission();

	// ˢ��UI���ѽ���������
	RefreshMissionNum(%AcceptCount);
	// ˢ��UI����������,��ȡ������Ϣ��ִ��GetMissionText����
	RefreshMissionDesc(-1);

	// ����������Ե�ͼ���ఴť��
	if(isOrderByMap())
		%Mission_Map = 1;
	else
		%Mission_Map = 0;

	// ������������ѽ�������ӵ����б�
	for(%i = 0; %i < %AcceptCount; %i++)
	{
		%Mid = %Player.GetMission(%i);
		%kind = GetMissionKind(%Mid);

//		%ZoneID = GetZoneID();
		// ���ѽ��������ϴ���һ������������
		CreateAcceptedMissionNode(%Player, %Mid, %Mission_Map);
	}
	// ȱʡչ���ѽ�������
	SingleWnd1_Tree1.expandToItem(0,1);
}

// ˢ�������������Ŀɽ������б�
function RefreshCanAcceptMission(%Player)
{
	MissionTraceWnd_CanSelectMissionTree.clearitem();

//	%AcceptCount = %Player.GetAcceptedMission();

	// ������������ѽ�������ӵ����б�
//	for(%i = 0; %i < %AcceptCount; %i++)
//	{
//		%Mid = %Player.GetMission(%i);
		// ������׷�����ϴ���һ��������
		CreateCanAcceptMissionNode(%Player, 0);
//	}
//	if(%AcceptCount != 0)
		MissionTraceWnd_CanSelectMissionTree.expandToItem(0,1);
}

// ˢ�������������Ĺ��������б�
function RefreshShareMission(%Player)
{
	// ������������������б�
	SingleWnd1_Tree3.clearitem();
	%Count = %Player.GetShareMissionCount();

	// ��������ռ������й���������ӵ����б�
	for(%i = 0; %i < %Count; %i++)
	{
		%Mid = %Player.GetShareMission(%i);
		CreateShareMissionNode(%player, %Mid, 0);
	}
}

// ˢ�½��������׷���б�
function RefreshTraceMission(%Player)
{
	MissionTraceWnd_Tree.clearitem();

	%AcceptCount = %Player.GetAcceptedMission();

	// ������������ѽ�������ӵ����б�
	for(%i = 0; %i < %AcceptCount; %i++)
	{
		%Mid = %Player.GetMission(%i);
		// ������׷�����ϴ���һ��������
		CreateTraceMissionNode(%Player, %Mid);
	}
	if(%AcceptCount != 0)
		MissionTraceWnd_Tree.expandToItem(0,1);
}

// ��ȡ����ȫ�������ı�
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

// ��ȡ���������ı�
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
				if(%i == 0){%ii = 1200;}//Ѱ��Ŀ��
				if(%i == 1){%ii = 3200;}//ɱ�ּ���
				if(%i == 2){%ii = 2250;}//�������
				if(%i == 3){%ii = 2200;}//�Ѽ�����
				if(%i == 4){%ii = 1250;}//Ѱ������Ŀ��
				if(%i == 5){%ii = 8000;}//������ֵ
				if(%i == 6){%ii = 9000;}//��������

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

	return "GetMissionNeedText = ���� Player��"@%Player@"��Mid��"@%Mid@"��Type��"@%Type@"��";
}

//��ȡ�����Ѷ�
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
					%R1 = %R1 @ "��";

				for(%i = 0; %i < %NanDu2; %i++)
					%R2 = %R2 @ "��";

				return $Get_Dialog_GeShi[31206] @ %R1 @ "</t> <t>" @ %R2 @ "</t>";
			}

	return "��ͼƬ���ݴ���<b/>";
}

//��ȡ������������
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

//��ȡ����������������������������ɫ���ʽ
function GetMission_Flag(%Player , %Mid , %Type , %Num , %GetBack)
{
//	echo("==========================================");
//	echo("��%Mid = "@%Mid@"��");
//	echo("��%Type = "@%Type@"��");
//	echo("��%Num = "@%Num@"��");
//	echo("��%GetBack = "@%GetBack@"��");

	%MissionData	= "MissionData_" @ %Mid;

	//NPC��
	if(%Type $= "Npc")
		{
			if(%Num == 100){%Id_Type = %MissionData.NpcA;}
			if(%Num == 99 )
				{
					%Id_Type = %MissionData.NpcZ;

					if(%Mid == 10001){%Id_Type = 400001007;}
					if(%Mid == 10122)
						{
							  echo("̫�׽���");
								%Id_Type = 400001101;
						}

					if(%Mid == 10123)
						{
							echo("�����ڵ�����10118");
							%Id_Type = %Mid;
						}

					if( ( (%Mid >= 10124)&&(%Mid <= 10128) )||(%Mid == 20001) )
						{
						  if(%Player.GetFamily() == 0){if(%Mid == 20001){%Id_Type = 401011006;}if(%Mid == 10124){%Id_Type = 000000000;}if(%Mid == 10125){%Id_Type = 000000000;}if(%Mid == 10126){%Id_Type = 000000000;}if(%Mid == 10127){%Id_Type = 000000000;}} //���ԣ���ְҵ
							if(%Player.GetFamily() == 1){if(%Mid == 20001){%Id_Type = 401011006;}if(%Mid == 10124){%Id_Type = 401011002;}if(%Mid == 10125){%Id_Type = 401011002;}if(%Mid == 10126){%Id_Type = 401011006;}if(%Mid == 10127){%Id_Type = 400001038;}} //ʥʦ��NPC
							if(%Player.GetFamily() == 2){if(%Mid == 20001){%Id_Type = 401006006;}if(%Mid == 10124){%Id_Type = 401006002;}if(%Mid == 10125){%Id_Type = 401006002;}if(%Mid == 10126){%Id_Type = 401006006;}if(%Mid == 10127){%Id_Type = 400001044;}} //��ʦ��NPC
							if(%Player.GetFamily() == 3){if(%Mid == 20001){%Id_Type = 401007006;}if(%Mid == 10124){%Id_Type = 401007002;}if(%Mid == 10125){%Id_Type = 401007002;}if(%Mid == 10126){%Id_Type = 401007006;}if(%Mid == 10127){%Id_Type = 400001039;}} //��ʦ��NPC
							if(%Player.GetFamily() == 4){if(%Mid == 20001){%Id_Type = 400001042;}if(%Mid == 10124){%Id_Type = 401005002;}if(%Mid == 10125){%Id_Type = 401005002;}if(%Mid == 10126){%Id_Type = 401005006;}if(%Mid == 10127){%Id_Type = 400001042;}} //��ʦ��NPC
							if(%Player.GetFamily() == 5){if(%Mid == 20001){%Id_Type = 401009006;}if(%Mid == 10124){%Id_Type = 401009002;}if(%Mid == 10125){%Id_Type = 401009002;}if(%Mid == 10126){%Id_Type = 401009006;}if(%Mid == 10127){%Id_Type = 400001041;}} //��ʦ��NPC
							if(%Player.GetFamily() == 6){if(%Mid == 20001){%Id_Type = 401010006;}if(%Mid == 10124){%Id_Type = 401010002;}if(%Mid == 10125){%Id_Type = 401010002;}if(%Mid == 10126){%Id_Type = 401010006;}if(%Mid == 10127){%Id_Type = 400001040;}} //��ʦ��NPC
							if(%Player.GetFamily() == 7){if(%Mid == 20001){%Id_Type = 400001043;}if(%Mid == 10124){%Id_Type = 401008002;}if(%Mid == 10125){%Id_Type = 401008002;}if(%Mid == 10126){%Id_Type = 401008006;}if(%Mid == 10127){%Id_Type = 400001043;}} //��ʦ��NPC
							if(%Player.GetFamily() == 8){if(%Mid == 20001){%Id_Type = 400001045;}if(%Mid == 10124){%Id_Type = 401004002;}if(%Mid == 10125){%Id_Type = 401004002;}if(%Mid == 10126){%Id_Type = 401004006;}if(%Mid == 10127){%Id_Type = 400001045;}} //ħʦ��NPC

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
				if(%Type $= "MonsterKill")	//ɱ������
					{
						%Id_Type = %Player.GetMissionFlag(%Mid, 3100 + %Num);
						%Id_Num  = %Player.GetMissionFlag(%Mid, 3200 + %Num);
					}
				if(%Type $= "ItemAdd")			//����Ʒ
					{
						%Id_Type	= %Player.GetMissionFlag(%Mid, 2150 + %Num);
						%Id_Num		= %Player.GetMissionFlag(%Mid, 2250 + %Num);
					}
				if(%Type $= "ItemGet")			//ȡ��Ʒ
					{
						%Id_Type = %Player.GetMissionFlag(%Mid, 2100 + %Num);
						%Id_Num  = %Player.GetMissionFlag(%Mid, 2200 + %Num);
					}
				if(%Type $= "Target")				//��������
					{
						%Id_Type = %Player.GetMissionFlag(%Mid, 1100 + %Num);
						%Id_Num  = %Player.GetMissionFlag(%Mid, 1200 + %Num);
					}
				if(%Type $= "Step")				//������ֵ��ѧϰ�����
					{
						//ѧϰ�����
						if(%Mid == 22002){%ID_Type = %Mid;%Id_Num = 8000;}	//����
						if(%Mid == 22004){%ID_Type = %Mid;%Id_Num = 8000;}	//����
						if(%Mid == 22006){%ID_Type = %Mid;%Id_Num = 8000;}	//��ҩ
						if(%Mid == 22008){%ID_Type = %Mid;%Id_Num = 8000;}	//��ֲ
						if(%Mid == 22010){%ID_Type = %Mid;%Id_Num = 8000;}	//��ľ
						if(%Mid == 22012){%ID_Type = %Mid;%Id_Num = 8000;}	//�ɿ�
						if(%Mid == 10000){%ID_Type = %Mid;%Id_Num = 8000;}	//����ȼ�
					}
			}


//------------------------------------��ϲ���----------------------------------------��----

	switch(%GetBack)
	{
		case 99:			//����
//			echo("������ItemAdd=2");
//			echo("%Id_Type == "@%Id_Type);

			//5λ�ı��
			if(strlen(%Id_Type) == 5)
				{
					echo("�����ڵ�����");
					if(%Mid == 10123)
						return "�����ڵ�����";
//					if(%Mid == 10122)
//						return "̫�׽���";
//					if(%Mid == 20001)
//						{
//						  if(%Player.GetFamily() == 0){return GetNpcData(400001101,1);} //���ԣ���ְҵ
//							if(%Player.GetFamily() == 1){return GetNpcData(400001038,1);} //ʥʦ��NPC
//							if(%Player.GetFamily() == 2){return GetNpcData(400001044,1);} //��ʦ��NPC
//							if(%Player.GetFamily() == 3){return GetNpcData(400001039,1);} //��ʦ��NPC
//							if(%Player.GetFamily() == 4){return GetNpcData(400001042,1);} //��ʦ��NPC
//							if(%Player.GetFamily() == 5){return GetNpcData(400001041,1);} //��ʦ��NPC
//							if(%Player.GetFamily() == 6){return GetNpcData(400001040,1);} //��ʦ��NPC
//							if(%Player.GetFamily() == 7){return GetNpcData(400001043,1);} //��ʦ��NPC
//							if(%Player.GetFamily() == 8){return GetNpcData(400001045,1);} //ħʦ��NPC
//						}
				}
			//9λ�ı��
			if(strlen(%Id_Type) == 9)
				{
//					echo("������ItemAdd=21111111111");
					if(GetSubStr(%Id_Type,0,1) $= "1"){return GetItemData(%Id_Type,1);}	//����
					if(GetSubStr(%Id_Type,0,1) $= "4")																	//NPC��
						{
							return GetNpcData(%Id_Type,1);
						}
					if(GetSubStr(%Id_Type,0,1) $= "5")                                  //�ɼ�
						{
							if(%Id_Type $= "552000056"){return "���̨";}
							if(%Id_Type $= "552000057"){return "��ӡ��ʯ";}
              if(%Id_Type $= "552000053"){return "����̨";}
							if(%Id_Type $= "552000022"){return "��ʯһ";}
							if(%Id_Type $= "552000023"){return "��ʯ��";}
							if(%Id_Type $= "552000024"){return "��ʯ��";}
							if(%Id_Type $= "552000038"){return "��ɫ���";}
							if(%Id_Type $= "552000039"){return "��ɫ���";}
							if(%Id_Type $= "552000040"){return "��ɫ���";}
							if(%Id_Type $= "552000041"){return "������";}
							if(%Id_Type $= "552000043"){return "��˻�";}
							if(%Id_Type $= "552000027"){return "����";}
							if(%Id_Type $= "552000042"){return "��˻����к�";}
							if(%Id_Type $= "552000044"){return "������������";}
							if(%Id_Type $= "552000028"){return "����";}
							if(%Id_Type $= "552000047"){return "�������֦";}


						}
					if(GetSubStr(%Id_Type,0,1) $= "8")																	//����
						{
							if(%Id_Type $= "810010119"){return "����������";}
							if(%Id_Type $= "810010106"){return "���¾���";}
							if(%Id_Type $= "811010103"){return "��ˮ��";}
							if(%Id_Type $= "811030110"){return "������Ӫ��";}
							if(%Id_Type $= "811030111"){return "������Ӫ��";}
							if(%Id_Type $= "811030112"){return "������Ӫ��";}
							if(%Id_Type $= "811030113"){return "���̸���";}
							if(%Id_Type $= "811030106"){return "�����µ�������";}
							if(%Id_Type $= "811030107"){return "�����µƹ����";}
							if(%Id_Type $= "811030108"){return "�����ǵ�������";}
						}
				}

//			if(%Type $= "Npc"        ){return GetNpcData(%Id_Type,1);}	//NPC��
//			if(%Type $= "MonsterKill"){return GetNpcData(%Id_Type,1);}	//NPC��
//			if(%Type $= "ItemGet"    ){return GetItemData(%Id_Type,1);}	//����
//			if(%Type $= "ItemAdd"    ){return GetItemData(%Id_Type,1);}	//����
//			if(%Type $= "Target"     ){return "xxxxxxxxxxxx";}					//����

			return "";
		case 98:			//��ͼ
			return $Get_Dialog_GeShi[31203] @ $Mission_Map[ %MissionData.Map ] @ "</t>";
//		return $Get_Dialog_GeShi[31203] @ $Mission_Map[ GetRoute_MapID( %Id_Type ) ] @ "</t>";


		case 97:			//���꣬�а�ɫ�»�����Ѱ����ť
			%XYZ      = GetRoute_Position(%Id_Type);
			%Map      = GetSubStr(GetRoute_MapID(%Id_Type),1,4);
			%Map_XYZ1 = Pos3DTo2D(%MissionData.Map, GetWord(%XYZ,0), GetWord(%XYZ,1));
			%Map_XYZ2 = GetWord(%Map_XYZ1,0) @ "," @ GetWord(%Map_XYZ1,1);

			return "<c cid='" @ Get_TraceButton() @ "' cmd='\"findpath("@%Id_Type@");\",\"" @ $Get_Dialog_GeShi[50000] @ "(" @ %Map_XYZ2 @ ")</t>\",\"GuiMissionTraceButtonProfile3\"' cf='createButton' />";

		case 96:			//��������ɫ����
//			echo("������ItemAdd=3");
			if(%Type $= "Npc"        ){%NumId = 1050;}	//NPC
			if(%Type $= "MonsterKill"){%NumId = 3000;}	//����
			if(%Type $= "ItemGet"    ){%NumId = 2000;}	//����
			if(%Type $= "ItemAdd"    ){%NumId = 2050;}	//����
			if(%Type $= "Target"     ){%NumId = 1000;}	//������

//			if( (%Type $= "ItemGet")||(%Type $= "ItemAdd") )
//				%KillNumMin = %Player.GetItemCount(%Id_Type);	//�����ڵĵ�ǰ��������
//			else
			%KillNumMin = %Player.GetMissionFlag(%Mid,%NumId + %Num + 300);//��ǰ����
			%KillNumMax = %Player.GetMissionFlag(%Mid,%NumId + %Num + 200);//�������

//			if(%KillNumMin < %KillNumMax)
//				%FlagOver = "";
//			else
//				%FlagOver	= "(���)";
//				%FlagOver	= $Get_Dialog_GeShi[31204] @ "(���)</t><b/>";

//			return " " @ %KillNumMin @ " / " @ %KillNumMax @ "</t> " @ %FlagOver;
			return " " @ %KillNumMin @ "/" @ %KillNumMax @ "</t>";

		case 95:			//Ŀ��ִ�ж���
//			echo("������ItemAdd=4");
			if(%Type $= "Npc"        )									//NPC
				{
					if(%Mid == 10103){return "������";	}
					if(%Mid == 10109){return "���͸�����";	}
					return "�Ի���";
				}
			if(%Type $= "MonsterKill")
				{
					if((%Id_Type $= "410701005" )&&(%Mid ==10385)){return "�ݻ٣�";	}

					return "��ɱ��";
				}	//����
			if(%Type $= "ItemAdd"    )
				{
					if((%Id_Type $= "108020060" )&&(%Mid ==10151)){return "�鿴��";	}
					return "��Ʒ��";
				}	//����
			if(%Type $= "ItemGet"    )									//����
				{
					if((%Id_Type $= "107019001" )&&(%Mid ==10114)){return "����";	}
					if((%Id_Type $= "108020267" )&&(%Mid ==10114)){return "�ɼ���";	}
					if(%Mid == 10700){return "����";	}
					if(%Mid == 10185){return "��ȡ��";	}
					if(%Mid == 10303){return "Ѱ�ң�";	}
					if(%Mid == 10305){return "���ƣ�";	}

					return "��Ʒ��";
				}
			if(%Type $= "Target"     )									//Ŀ��
				{
					if(%Mid == 10114){return "�ɼ���";	}
					if(%Mid == 10116){return "��ȡ��";	}
					if(%Mid == 10120){return "���ϣ�";	}
					if(%Mid == 10188){return "Ŀ�꣺";	}
					if((%Mid == 10510)||(%Mid == 10522)){return "�۲죺";	}
					if(%Mid == 10544){return "��ȣ�";	}
					if(%Mid == 10547){return "Ѱ�ң�";	}
					if(%Mid == 10344){return "�ٻ���";	}
					if(%Mid == 10787){return "������";	}
					return "ʹ�ã�";
			  }
			if(%Type $= "Step"       )				//������ֵ
				{
					if(%Mid == 22002){return "ѧϰ����ܣ�����";}
					if(%Mid == 22004){return "ѧϰ����ܣ�����";}
					if(%Mid == 22006){return "ѧϰ����ܣ���ҩ";}
					if(%Mid == 22008){return "ѧϰ����ܣ���ֲ";}
					if(%Mid == 22010){return "ѧϰ����ܣ���ľ";}
					if(%Mid == 22012){return "ѧϰ����ܣ��ɿ�";}
					if(%Mid == 10000){return "����ﵽ�ȼ���";}
				}
			if(%Type $= "Test"       )				//������ֵ
				{
					if(%Mid == 10001){return "����ﵽ10��";}
				}


//------------------------------------��ϲ���----------------------------------------��----

//�˴���ʼ���

		case 1:			//���أ�����ɫ������
//			echo("������ItemAdd=5");
			if(%Type $= "Npc"        ){%Color = 31203;}//NPC
			if(%Type $= "MonsterKill"){%Color = 31206;}//����
			if(%Type $= "ItemAdd"    ){%Color = 31203;}//����
			if(%Type $= "ItemGet"    ){%Color = 31203;}//����
			if(%Type $= "Target"     ){%Color = 31203;}//Ŀ��

//			echo($Get_Dialog_GeShi[ %Color ] @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ "</t>");

			return $Get_Dialog_GeShi[ %Color ] @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ "</t>";

		case 2:			//���أ�λ��+����+��+����ɫ��NPC��+Ѱ������
			return "<t>λ��</t>" @ GetMission_Flag(%Player , %Mid , %Type , %Num , 98) @ "<t>��</t>" @ GetMission_Flag(%Player , %Mid , %Type , %Num , 1) @ GetMission_Flag(%Player , %Mid , %Type , %Num , 97);

		case 3:			//���أ���ɱ��/��ʰȡ/�ѶԻ�+��ɫ��NPC��+����
			return $Get_Dialog_GeShi[31201] @ GetMission_Flag(%Player , %Mid , %Type , %Num , 95) @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ GetMission_Flag(%Player , %Mid , %Type , %Num , 96);

		case 4:			//���أ�������NPC
			if(CanDoThisMission(%Player, %Mid, 2, 0, 0) $= "")
				%Over = $Get_Dialog_GeShi[31203] @ " ��</t>";
			else
				%Over = $Get_Dialog_GeShi[31201] @ " ��</t>";

			return %Over @ $Get_Dialog_GeShi[31201] @ "������</t>" @ "<c cid='" @ Get_TraceButton() @ "' cmd='\"findpath("@%Id_Type@");\",\"" @ $Get_Dialog_GeShi[31214] @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ "</t>\",\"GuiMissionTraceButtonProfile6\"' cf='createButton' /><b/>";

		case 5:			//���أ���ɱ��/��ʰȡ/�ѶԻ�+��ɫ��NPC��+����+Ѱ����ʽ
//			%Back = GetMission_Flag(%Player , %Mid , %Type , %Num , 95) @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ GetMission_Flag(%Player , %Mid , %Type , %Num , 96);

			//����������Ƿ����㣬��ΪҪ��ɫ
			if(%Type $= "Npc"        ){%NumId = 1050;}	//NPC
			if(%Type $= "MonsterKill"){%NumId = 3000;}	//����
			if(%Type $= "ItemGet"    ){%NumId = 2000;}	//����
			if(%Type $= "ItemAdd"    ){%NumId = 2050;}	//����
			if(%Type $= "Target"     ){%NumId = 1000;}	//������

			//������ֵ
			if(%Type $= "Step")
				{
					%KillNumMin = %Player.GetMissionFlag(%Mid, 8100 + %Num);//��ǰ����
					%KillNumMax = %Player.GetMissionFlag(%Mid, 8000 + %Num);//�������
				}
				else
					{
						%KillNumMin = %Player.GetMissionFlag(%Mid, %NumId + %Num + 300);//��ǰ����
						%KillNumMax = %Player.GetMissionFlag(%Mid, %NumId + %Num + 200);//�������
					}

			//Ѱ�����
			%XYZ_ID = %Id_Type;

			if(strlen(%Id_Type) == 5)
				{
					if(%Mid == 22002){%XYZ_ID = 400001019;}	//����
					if(%Mid == 22004){%XYZ_ID = 400001022;}	//����
					if(%Mid == 22006){%XYZ_ID = 400001021;}	//��ҩ
					if(%Mid == 22008){%XYZ_ID = 400001020;}	//��ֲ
					if(%Mid == 22010){%XYZ_ID = 400001018;}	//��ľ
					if(%Mid == 22012){%XYZ_ID = 400001017;}	//�ɿ�
				}
			if(strlen(%Id_Type) == 9)
				if(GetSubStr(%Id_Type,0,1) $= "1")
					{
						//�������
						if((GetItemData(%Id_Type,2) == 8)||(GetItemData(%Id_Type,2) == 7)||(GetItemData(%Id_Type,2) == 6))
							{
								%Mon_Num_Max = GetWordCount( $Item_Monster[ %Id_Type ] );
								%Mon_Nun     = GetRandom(1, %Mon_Num_Max);

								%XYZ_ID = GetWord( $Item_Monster[ %Id_Type ], %Mon_Nun - 1 );
							}
						else
							%XYZ_ID = %Id_Type;	//�������
					}

			//��������
			if(%KillNumMin < %KillNumMax)
				{
					%ColorA = 31201;	//��ɫ
					%ColorB = 31214;	//����ɫ+�»���
					%OK = " ��";
				}
				else
					{
						%ColorA = 31203;	//��ɫ���»���
						%ColorB = 31203;	//��ɫ���»���
						%OK = " ��";
					}

			//������겻��Ҫ����Ҳ����ҪѰ��
			if(%Type $= "Test")
				return $Get_Dialog_GeShi[ 31201 ] @ " ��" @ GetMission_Flag(%Player , %Mid , %Type , %Num , 95) @ "</t>";
			else
				return $Get_Dialog_GeShi[ %ColorA ] @ %OK @ GetMission_Flag(%Player , %Mid , %Type , %Num , 95) @ "</t>" @ "<c cid='" @ Get_TraceButton() @ "' cmd='\"findpath("@%XYZ_ID@");\",\"" @ $Get_Dialog_GeShi[ %ColorB ] @ GetMission_Flag(%Player , %Mid , %Type , %Num , 99) @ "</t> \",\"GuiMissionTraceButtonProfile6\"' cf='createButton' />" @ $Get_Dialog_GeShi[ %ColorA ] @ %KillNumMin @ "/" @ %KillNumMax @ "</t>";
			
	}
//	return "GetMission_Flag = ����%Player = "@%Player@"����%Mid = "@%Mid@"����%Type = "@%Type@"����%Num = "@%Num@"����%GetBack = "@%GetBack@"��";

//	echo( "GetMission_Flag = ����%Player = "@%Player@"����%Mid = "@%Mid@"����%Type = "@%Type@"����%Num = "@%Num@"����%GetBack = "@%GetBack@"��");
	return "��1="@%Player@"����2="@%Mid@"����3="@%Type@"����4="@%Num@"����5="@%GetBack@"��";
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
