//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//����ͨ��������ʽ�����Ľű�����NPC��ʼ������
//==================================================================================

//������ת��ڣ�ѡ�к�ʹ��F3���ɣ�����㲻����UE���ǾͰﲻ������

//�������������������������д����Ի�����ű���ڡ�������������������������������
//��������������������������ʹ�ýű���������������������������������������������
//��������������������������װ���ű���������������������������������������������
//��������������������������ж�ؽű���������������������������������������������
//�������������������������ߴ��������������������������������������������������
//�������������������������ߴ�������ӽ�����������������������������������������
//�������������������������򴥷��ű���������������������������������������������
//�������������������������ϵĵ��ߴ����ű���������������������������������������
//���������������������������ĵ��ߴ����ű���������������������������������������


//�������������������������д����Ի�����ű���ڡ�������������������������������
function OnTriggerMission(%Player, %X_ID, %State, %TriggerType, %TriggerID, %Param, %XYZ)
{
	echo("=====================================");
//	echo("OnTriggerMission");
//	echo("%X_ID = "@%X_ID);
	echo("%State = "@%State);

	%Conv = new Conversation();

	if(%State == -1)
		{
			%Conv.SetType(4);
			%Conv.Send(%Player);
			return;
		}

	//ɾ�����񴥷�
	if(%State == -2)
		{
			//��������
//			if(GetMissionKind(%X_ID) == 1)
			if(%X_ID ==20001)//ʦ������
				{
					%Hs = %Player.GetMissionFlag(%Mid,100); //ȡ���������
					%Cs = %Player.GetMissionFlag(%Mid,200); //ȡ����������
					
					%Player.SetFlagsByte(1,%Hs);//��������������
					%Player.SetFlagsByte(2,%Cs); //�������������
					
					%Player.AddBuff(320050001, 0);
				}

			%Player.DelMission(%X_ID);
			return;
		}

	//���X_ID��9λ
	if(strlen(%X_ID) == 9)
		{
			//��λ��1�����ǵ���ʹ�ýű�
			if(GetSubStr(%X_ID,0,1) $= "1")
				{
					echo("��λ��1�����ǵ���ʹ�ýű�");

					//�жϴ˵����Ƿ�����ʳ�ҩƷ����Ʒ������
					if( (GetItemData(%X_ID, 3) >= 501)&&(GetItemData(%X_ID, 3) <= 504) )
						{
//							echo("�жϴ˵����Ƿ�����ʳ�ҩƷ����Ʒ������ = "@%X_ID);
//							echo("�ж�ĳ��Ʒ�Ƿ�������ʹ�ô���=" @ %Player.IsItemUsedTimes(%X_ID, %XYZ));
//							echo("�ж�ĳ��Ʒʹ�ô���=" @ %Player.GetItemUsedTimes(%X_ID, %XYZ));
//							echo("�ж�ĳ��Ʒ�Ƿ�ʹ����ʧ=" @ CheckBit(GetItemData(%X_ID, 22), 10));

							//�ж�ĳ��Ʒ�Ƿ�������ʹ�ô���
							if(%Player.IsItemUsedTimes(%X_ID, %XYZ) > 0)
								if(%Player.GetItemUsedTimes(%X_ID, %XYZ) > 0)
									%Player.SetItemUsedTimes(%X_ID, %XYZ, %Player.GetItemUsedTimes(%X_ID, %XYZ) - 1);

							if(CheckBit(GetItemData(%X_ID, 22), 10) == 1)
								if(%Player.GetItemUsedTimes(%X_ID, %XYZ) < 1)
									DelItemFromInventoryByIndex(%Player.GetPlayerID(), %XYZ, 1);

							return;
						}

					//�жϴ˵����Ƿ�����������͵���
					if( (GetSubStr(%X_ID,0,4) == 1051)&&(GetSubStr(%X_ID,3,6) >= 100001)&&(GetSubStr(%X_ID,3,6) <= 100999) )
						{
							Trigger_Bao(%Conv, %Player , %X_ID, %State, %Param);	//���
							return;
						}

					//�жϴ˵����Ƿ������䷽ѧϰ����
					if(GetItemData(%X_ID, 2) == 9)
						{
							%WhyNot = "";																		//Ĭ�ϵķ�����Ϣ
							%PresID = GetItemData(%X_ID, 16);									//�䷽���
							%livingSkillId = GetWord($Pres_Add[%PresID], 0);		//��Ҫ������ܱ��
							%livingSkillId_Exp = GetWord($Pres_Add[%PresID], 1);	//����������������

//							echo("%PresID = "@%PresID);
//							echo("%livingSkillId = "@%livingSkillId);
//							echo("%livingSkillId_Exp = "@%livingSkillId_Exp);

							if(isLearnLivingSkill(%Player.GetPlayerID(),%livingSkillId) == 1)												//�жϸ�������Ƿ��Ѿ�ѧ��,0û��ѧ��,1ѧ��
								{
									if(%Player.GetLivingSkillRipe(%livingSkillId) >= %livingSkillId_Exp)		//�ж�����������ȳ�����Ҫ���ֵ
										{
											AddPrescription(%Player, %PresID);		//ѧϰ�䷽
											DelItemFromInventoryByIndex(%Player.GetPlayerID(), %XYZ, 1);		//ɾ������
										}
										else
											%WhyNot = "����Ӧ������������Ȳ��㡾"@%livingSkillId_Exp@"�����޷�ѧϰ���䷽";
								}
								else
									%WhyNot = "��û��ѧ����Ӧ������ܣ��޷�ѧϰ���䷽";

							if(%WhyNot !$= "")
								{
									SendOneChatMessage(0, "<t>" @ %WhyNot @ "</t>", %Player);
									SendOneScreenMessage(2, %WhyNot, %Player);
								}

							return;
						}

					//�жϵ�ǰ���ߣ��Ƿ�Ҫ�����������
					%Mission_UseItem = $MissionData_ItemUse[%X_ID];
					if(%Mission_UseItem !$= "")
						Mission_UseItem(%Conv, %Player, %X_ID, %State, %TriggerType, %TriggerID);

					//�ж��˵����Ƿ���Ҫ��������ضԻ�����
					%NpcA_Mid = $MissionData_NpcA[%X_ID];
					%NpcX_Mid = $MissionData_NpcX[%X_ID];
					%NpcZ_Mid = $MissionData_NpcZ[%X_ID];

					if( (%NpcA_Mid > 0)||(%NpcX_Mid > 0)||(%NpcZ_Mid > 0) )
						{
							Mission_Accepted_Or_Over(%Conv, %Player , %X_ID, %State, %TriggerType, %TriggerID);
							return;
						}

					eval("Trigger_" @ %X_ID @ "(%Conv, %Player, %X_ID, %State, %TriggerType, %TriggerID);");
				}

			//��λ��8���������򴥷��ű�
			if(GetSubStr(%X_ID,0,1) $= "8")
				eval("Trigger_" @ %X_ID @ "(%Conv, %Player, %X_ID, %State, %TriggerType, %TriggerID);");
		}
		else
			eval("Trigger_" @ %X_ID @ "(%Conv, %Player, %X_ID, %State, %TriggerType, %TriggerID);");
}
//�������������������������д����Ի�����ű���ڡ�������������������������������

//��������������������������ʹ�ýű���������������������������������������������
//���п�ʹ�õĵ��ߣ�ʹ��ʱ�����Ľű�
function Item_Use(%Player , %ItemID, %TriggerType, %TriggerID, %XYZ)
{
//	echo("=====================================");
//	echo("Item_Use = "@%ItemID);

	$Item_Use_Return = 1;

	//0��ʹ��ʧ�ܣ���ִ�к�������
	//1��ʹ�óɹ�

	//���е��ߣ�ͳһִ��OnTriggerMission����
	OnTriggerMission(%Player, %ItemID, 0, %TriggerType, %TriggerID, 0, %XYZ);
	return $Item_Use_Return;
}
//��������������������������ʹ�ýű���������������������������������������������

//��������������������������װ���ű���������������������������������������������
function Item_On(%Player , %ItemID)
{
	echo("Item_On = "@%ItemID);
}
//��������������������������װ���ű���������������������������������������������

//��������������������������ж�ؽű���������������������������������������������
function Item_Off(%Player , %ItemID)
{
	echo("Item_Off = "@%ItemID);
}
//��������������������������ж�ؽű���������������������������������������������

//�������������������������ߴ��������������������������������������������������
function Mission_UseItem(%Conv, %Player , %ItemID, %State, %TriggerType, %TriggerID)
{
	echo("uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu");
	%Num = $MissionData_ItemUse[%ItemID];

	for(%i = 0; %i < %Num; %i++)
	{
		%Mid = $MissionData_ItemUse[%ItemID, %i + 1];

		echo("Mission_UseItem ==Player="@%Player@"=ItemID="@%ItemID@"=State="@%State);
		echo("�жϴ�ָ������ �� " @ %i @ " �� "@ %Mid);

		//�ж��Ƿ���Ҫ����ѭ��
		if(%Mid $= "")
			break;

		//�ж��Ƿ���ܵ�ǰ����
		if(!%Player.IsAcceptedMission(%Mid))
			{
				echo("δ���ܴ�ָ������ �� " @ %i @ " �� "@ %Mid);
				//������ǰ%iֵ��ѭ��������%i+1ֵ��ѭ��
				continue;
			}

		//�ж��Ƿ���ɵ�ǰ����
		if(%Player.IsFinishedMission(%Mid))
			{
				echo("����ɴ�ָ������ �� " @ %i @ " �� "@ %Mid);
				//������ǰ%iֵ��ѭ��������%i+1ֵ��ѭ��
				continue;
			}

		%MissionData = "MissionData_" @ %Mid;

		for(%ii = 0; %ii < 30; %ii++)
		{
			%Target = GetWord(%MissionData.Target, %ii * 2);
			%TarNum = GetWord(%MissionData.Target, %ii * 2 + 1);

			echo("iiiiii========= = "@%ii);
			echo("Target========= = "@%Target);
			echo("TarNum========= = "@%TarNum);

			//�ж��Ƿ���Ҫ����ѭ��
			if( (%Target $= "")&&(%TarNum $= "") )
				{
					%Txt = "��������ָ����Ŀ������� " @ %i @ " ��Ŀ�� = ��"@%Target@"������ǰ���� = ��"@$Player_Area[%Player.GetPlayerID()]@"��";

					echo(%Txt);
//					SetScreenMessage(%Txt, $Color[2]);
//					SetChatMessage(%Txt, $Color[2]);

					break;
				}
				else
					{
						%Yes = 0;
						//�ж�Ŀ���Ƿ���ȷ
						if(strlen(%Target) == 9)
							{
								echo("$Player_Area[%Player.GetPlayerID()] = "@$Player_Area[%Player.GetPlayerID()]);
								echo("%Target = "@%Target);

								//����
								if(GetSubStr(%Target,0,1) $= "8")
									if($Player_Area[%Player.GetPlayerID()] $= %Target)
										%Yes = 1;

								//Npc�����
								if(GetSubStr(%Target,0,1) $= "4")
									if(%Player.Target $= %Target)
										%Yes = 1;

							}

						echo("%Yes = "@%Yes);

						//Ŀ����ȷ������
						if(%Yes == 1)
							if(%Player.GetMissionFlag(%Mid, 1300 + %ii) < %TarNum)
								{
									echo("Ŀ����ȷ������");
									%Player.SetMissionFlag(%Mid, 1300 + %ii, %Player.GetMissionFlag(%Mid, 1300 + %ii) + 1, true); 				//����,Ŀ�괥����ǰ����
									break;
								}
					}
		}
	}
}
//�������������������������ߴ��������������������������������������������������
//�������������������������ߴ�������ӽ�����������������������������������������
function Mission_Accepted_Or_Over(%Conv, %Player , %ItemID, %State, %TriggerType, %TriggerID)
{
	if(%State == -1)
		{
			%Conv.SetType(4);
			%Conv.Send(%Player);
			return;
		}

	echo("Mission_Accepted_Or_Over == "@%ItemID);

	%Option = 0;

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%ItemID);

	%Conv.SetType(1);

	%NpcA_Mid = $MissionData_NpcA[%ItemID];
	%NpcZ_Mid = $MissionData_NpcZ[%ItemID];

	if(%State == 0)
		{
			%Conv.SetText(%ItemID);

			//������ǰNPC���пɽӵ����񣬲�������Ӧѡ��
			for(%a = 0; %a < %NpcA_Mid; %a++)
			{
				%Mid = $MissionData_NpcA[%ItemID, %a + 1];
				if(CanDoThisMission(%Player, %Mid, 1, 0, 2) $= "")
					{
						%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//��ʾ������ѡ��
						%Option = 1;
					}
			}

			//������ǰNPC���пɽ������񣬲�������Ӧѡ��
			for(%z = 0; %z < %NpcZ_Mid; %z++)
			{
				%Mid = $MissionData_NpcZ[%ItemID, %z + 1];
				if(CanDoThisMission(%Player, %Mid, 2, 0, 2) $= "")
					{
						%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//��ʾ������ѡ��
						%Option = 1;
					}
			}

			//���û��ѡ�������һ�������Ի�ѡ��
			if(%Option == 0)
				%Conv.AddOption(10, -1);//��ʾ�����Ի�
		}
	if(%State > 0)
		{
			%MidState = GetSubStr(%State,0,3);
			%Mid      = GetSubStr(%State,3,5);

//			echo("==========================="@%State);
			switch(%MidState)
			{
				case 110:
					//������ܲ���
					if(CanDoThisMission(%Player, %Mid, 1, 1, 1) $= "")
						{
							%Conv.SetType(4);
							AddMissionAccepted(%Player, %Mid);	//�������������Ϣ
						}
						else
							%Conv.SetText(199 @ %Mid);	//��ʾ����������������ĶԻ�

				case 910:
					//���񽻸�����
					if(CanDoThisMission(%Player, %Mid, 2, 1, 1) $= "")
						{
							%Conv.SetType(4);
							AddMissionReward(%Player, %Mid, %Param);	//����������

							if(CanDoThisMission(%Player, %Mid + 1, 1, 1, 1) $= "")
								AddMissionAccepted(%Player, %Mid + 1);	//�������������Ϣ
						}
						else
							%Conv.SetText(998 @ %Mid);		//��ʾ����������������ĶԻ�
			}
		}
	%Conv.Send(%Player);
}
//�������������������������ߴ�������ӽ�����������������������������������������

//�������������������������򴥷��ű���������������������������������������������
function Trigger_810010105(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)//�����
{
//	echo("Trigger10100");
//	echo("%Type = "@%Type);
//	echo("%State = "@%State);

	%Conv = new Conversation();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch$(%State)
	{
		case 10000:
			%Mid = %State;
       
			AddMissionAccepted(%Player, %Mid);	//�������������Ϣ
			%Conv.SetType(4);

	}
	%Conv.Send(%Player);
}


function Trigger_810010106(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch$(%State)
	{
		case 10113:
			%Mid = %State;

			%Conv.SetText(200 @ %Mid);	//��ʾ����Ի�
			%Conv.AddOption(8, 999);//�����Ի�
		case 999:
			%Conv.SetType(4);
	}
	%Conv.Send(%Player);
}

//�������������������������򴥷��ű���������������������������������������������

//�������������������������ϵĵ��ߴ����ű���������������������������������������
function Trigger_Bao(%Conv, %Player , %ItemID, %State, %Param)
{
	echo("Trigger_Bao");
	echo("%Param = "@%Param);

	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%ItemID);
	%Conv.SetType(2);

	if(%Player.GetItemCount(%ItemID) > 0)
		{
			%Item_Num_All = $Item_Bao[%ItemID];

			%NumAll = GetWord(%Item_Num_All, 0);		//����ڵ�������
			%Type   = GetWord(%Item_Num_All, 1);		//�������

			switch(%State)
			{
				case 0:
					%Conv.SetText(%ItemID, %ItemID);					//��ͻ��˷����������ֱ��

//					%Conv.SetText("10000000" @ %Type, %ItemID);					//��ͻ��˷����������ֱ��
//					%Conv.AddOption(9, %ItemID, 3);			//ѡ����鿴��Ʒ��

				default:
					//�۳��������
					%Player.PutItem(%ItemID, -1);

					//�ظ����ߵĴ������
					if(%Type == 1)
						{
							for(%i = 0;%i < %NumAll;%i++)
							{
								%Item_Num_Add = $Item_Bao[%ItemID, %i + 1];

								%Item = GetWord(%Item_Num_Add, 0);
								%Num  = GetWord(%Item_Num_Add, 1);

								echo("I = ["@%i@"] = ["@%Item@"] = ["@%Num@"]");

								if( (%Item $= "")&&(%Num $= "") )
									break;
								else
									%Player.PutItem(%Item, %Num);
							}
						}
					//��������ߵĴ������
					if(%Type == 2)
						{
						}
					//ѡ������ߵĴ������
					if(%Type == 3)
						{
							%Item_Num_Add = $Item_Bao[%ItemID, %Param + 1];

							%Item = GetWord(%Item_Num_Add, 0);
							%Num  = GetWord(%Item_Num_Add, 1);

							if( (%Item !$= "")&&(%Num !$= "") )
								%Player.PutItem(%Item, %Num);
						}
					//�������������ߵĴ������
					if(%Type == 4)
						{
							%Why = GetWord(%Item_Num_All, 2);		//�������


							if(%Why == 1){%ListNum = %Player.GetFamily();%ListSex = %Player.GetSex();}	//���������ж�
							if(%Why == 2){%ListNum = %Player.GetLevel();}		//�ȼ������ж�


							%Item_Num_Add = $Item_Bao[%ItemID, %ListNum, %ListSex];

							for(%ii = 0;%ii < 9;%ii++)
							{
								%Item = GetWord(%Item_Num_Add, %ii * 2);
								%Num  = GetWord(%Item_Num_Add, %ii * 2 + 1);

								if( (%Item $= "")&&(%Num $= "") )
									break;
								else
									%Player.PutItem(%Item, %Num);
							}
						}

					%ItemAdd = %Player.AddItem();

					//�������Ƿ�����
					if(%ItemAdd)
						%Conv.SetType(4);	//�رնԻ�
					else
						%Conv.SetText(6);		//��ͻ��˷����������ֱ��
			}
		}
		else
			%Conv.SetText(7);		//��Ʒ���ݴ���

	%Conv.Send(%Player);
}


//�������������������������ϵĵ��ߴ����ű���������������������������������������

//���������������������������ĵ��ߴ����ű���������������������������������������

//������ߣ����������¹ⱦ��
function Trigger_108020208(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ�Ի�
			%Conv.AddOption(14, 14);	//���Ҹı�ĳ�������״̬
			%Conv.AddOption(29, 13);	//�����������һ�£�9�룩��
//			%Conv.AddOption(28, 15);	//�����������ɣ�ϣ��
			%Conv.AddOption(30, 31);  //�����ɣ�С����!
			%Conv.AddOption(300000, 16);	//"�������(����)";
			%Conv.AddOption(300001, 17);	//"�����þ���(����)";
			%Conv.AddOption(300002, 18);	//"�����ý�Ǯ(����)";
			%Conv.AddOption(309, 309);  //����������ӻ���
			%Conv.AddOption(310, 310);  //����ȥ����

		case 14:
			%Conv.SetText(%Type);			//��ʾ�Ի�
//			%Conv.AddOption(19, 26);	//case 19:return "����������";
			%Conv.AddOption(20, 20);	//case 20:return "���»�����";
//			%Conv.AddOption(21, 26);	//case 21:return "��֧�߾��顿";
//			%Conv.AddOption(22, 26);	//case 22:return "��ÿ��ѭ����";
//			%Conv.AddOption(23, 26);	//case 23:return "���������";
//			%Conv.AddOption(24, 26);	//case 24:return "��ָ������";
//			%Conv.AddOption(25, 26);	//case 25:return "����������";

		case 15:
			%Conv.SetType(4);
			WuDi(%Player);
		case 13:
			%Conv.SetType(4);
			JiaSu(%Player);
//		case 26:
//			%Conv.SetText(26);			//Ŀǰ���޴�������
//			%Conv.AddOption(4, 14);	//����

		case 16:
			%Conv.AddOption(300011, 101);	//"�����1��(����)";
			%Conv.AddOption(300012, 102);	//"�����10��(����)";
			%Conv.AddOption(300013, 103);	//"�����50��(����)";
			%Conv.AddOption(300014, 104);	//"�����120��(����)";

		case 101:%Player.AddLevel(1  );Trigger_108020208(%Conv, %Player, %Type, 16, %TriggerType, %TriggerID);
		case 102:%Player.AddLevel(10 );Trigger_108020208(%Conv, %Player, %Type, 16, %TriggerType, %TriggerID);
		case 103:%Player.AddLevel(50 );Trigger_108020208(%Conv, %Player, %Type, 16, %TriggerType, %TriggerID);
		case 104:%Player.AddLevel(120);Trigger_108020208(%Conv, %Player, %Type, 16, %TriggerType, %TriggerID);

		case 17:
			%Player.AddExp(%Player.GetLevelExp());
			Trigger_108020208(%Conv, %Player, %Type, 0, %TriggerType, %TriggerID);
		case 18:
			%Player.AddMoney(999999,1);
			%Player.AddMoney(999999,2);
			Trigger_108020208(%Conv, %Player, %Type, 0, %TriggerType, %TriggerID);

		case 31:
			  %Conv.AddOption(293, 301);  //ʥ
				%Conv.AddOption(292, 302);	//��
				%Conv.AddOption(291, 303);	//��
				%Conv.AddOption(290, 304);	//��
				%Conv.AddOption(289, 305);	//��
			  %Conv.AddOption(288, 306);	//��
			  %Conv.AddOption(288, 307);  //��
			  %Conv.AddOption(286, 308);	//ħ

		case 301:%Player.SetFamily(1);Trigger_108020208(%Conv, %Player, %Type, 0, %TriggerType, %TriggerID);
		case 302:%Player.SetFamily(2);Trigger_108020208(%Conv, %Player, %Type, 0, %TriggerType, %TriggerID);
		case 303:%Player.SetFamily(3);Trigger_108020208(%Conv, %Player, %Type, 0, %TriggerType, %TriggerID);
		case 304:%Player.SetFamily(4);Trigger_108020208(%Conv, %Player, %Type, 0, %TriggerType, %TriggerID);
		case 305:%Player.SetFamily(5);Trigger_108020208(%Conv, %Player, %Type, 0, %TriggerType, %TriggerID);
		case 306:%Player.SetFamily(6);Trigger_108020208(%Conv, %Player, %Type, 0, %TriggerType, %TriggerID);
		case 307:%Player.SetFamily(7);Trigger_108020208(%Conv, %Player, %Type, 0, %TriggerType, %TriggerID);
		case 308:%Player.SetFamily(8);Trigger_108020208(%Conv, %Player, %Type, 0, %TriggerType, %TriggerID);

		case 309:
			%Conv.SetType(4);
			%Player.AddVigor(100);
			addLivingSkillGuerdon(%Player,501011001, 5000);
			addLivingSkillGuerdon(%Player,501011002, 5000);
			addLivingSkillGuerdon(%Player,501011003, 5000);
			addLivingSkillGuerdon(%Player,501011004, 5000);
			addLivingSkillGuerdon(%Player,501011005, 5000);
			addLivingSkillGuerdon(%Player,501011006, 5000);
			addLivingSkillGuerdon(%Player,501011007, 5000);
			addLivingSkillGuerdon(%Player,501011008, 5000);
			addLivingSkillGuerdon(%Player,501011009, 5000);
			addLivingSkillGuerdon(%Player,501011010, 5000);
			addLivingSkillGuerdon(%Player,501021001, 5000);
			addLivingSkillGuerdon(%Player,501021002, 5000);
			addLivingSkillGuerdon(%Player,501021003, 5000);
			addLivingSkillGuerdon(%Player,501021004, 5000);
			addLivingSkillGuerdon(%Player,501021005, 5000);
			addLivingSkillGuerdon(%Player,501021006, 5000);
			addLivingSkillGuerdon(%Player,501021007, 5000);
			addLivingSkillGuerdon(%Player,501021008, 5000);
			addLivingSkillGuerdon(%Player,501021009, 5000);
			addLivingSkillGuerdon(%Player,501021010, 5000);
			addLivingSkillGuerdon(%Player,501031001, 5000);
			addLivingSkillGuerdon(%Player,501031002, 5000);
			addLivingSkillGuerdon(%Player,501031003, 5000);
			addLivingSkillGuerdon(%Player,501031004, 5000);
			addLivingSkillGuerdon(%Player,501031005, 5000);
			addLivingSkillGuerdon(%Player,501031006, 5000);
			addLivingSkillGuerdon(%Player,501031007, 5000);
			addLivingSkillGuerdon(%Player,501031008, 5000);
			addLivingSkillGuerdon(%Player,501031009, 5000);
			addLivingSkillGuerdon(%Player,501031010, 5000);
			addLivingSkillGuerdon(%Player,501041001, 5000);
			addLivingSkillGuerdon(%Player,501041002, 5000);
			addLivingSkillGuerdon(%Player,501041003, 5000);
			addLivingSkillGuerdon(%Player,501041004, 5000);
			addLivingSkillGuerdon(%Player,501041005, 5000);
			addLivingSkillGuerdon(%Player,501041006, 5000);
			addLivingSkillGuerdon(%Player,501041007, 5000);
			addLivingSkillGuerdon(%Player,501041008, 5000);
			addLivingSkillGuerdon(%Player,501041009, 5000);
			addLivingSkillGuerdon(%Player,501041010, 5000);
			addLivingSkillGuerdon(%Player,501051001, 5000);
			addLivingSkillGuerdon(%Player,501051002, 5000);
			addLivingSkillGuerdon(%Player,501051003, 5000);
			addLivingSkillGuerdon(%Player,501051004, 5000);
			addLivingSkillGuerdon(%Player,501051005, 5000);
			addLivingSkillGuerdon(%Player,501051006, 5000);
			addLivingSkillGuerdon(%Player,501051007, 5000);
			addLivingSkillGuerdon(%Player,501051008, 5000);
			addLivingSkillGuerdon(%Player,501051009, 5000);
			addLivingSkillGuerdon(%Player,501051010, 5000);
			addLivingSkillGuerdon(%Player,501061001, 5000);
			addLivingSkillGuerdon(%Player,501061002, 5000);
			addLivingSkillGuerdon(%Player,501061003, 5000);
			addLivingSkillGuerdon(%Player,501061004, 5000);
			addLivingSkillGuerdon(%Player,501061005, 5000);
			addLivingSkillGuerdon(%Player,501061006, 5000);
			addLivingSkillGuerdon(%Player,501061007, 5000);
			addLivingSkillGuerdon(%Player,501061008, 5000);
			addLivingSkillGuerdon(%Player,501061009, 5000);
			addLivingSkillGuerdon(%Player,501061010, 5000);
			addLivingSkillGuerdon(%Player,502011001, 5000);
			addLivingSkillGuerdon(%Player,502011002, 5000);
			addLivingSkillGuerdon(%Player,502011003, 5000);
			addLivingSkillGuerdon(%Player,502011004, 5000);
			addLivingSkillGuerdon(%Player,502011005, 5000);
			addLivingSkillGuerdon(%Player,502022001, 5000);
			addLivingSkillGuerdon(%Player,502022002, 5000);
			addLivingSkillGuerdon(%Player,502022003, 5000);
			addLivingSkillGuerdon(%Player,502022004, 5000);
			addLivingSkillGuerdon(%Player,502022005, 5000);
			addLivingSkillGuerdon(%Player,502032001, 5000);
			addLivingSkillGuerdon(%Player,502032002, 5000);
			addLivingSkillGuerdon(%Player,502032003, 5000);
			addLivingSkillGuerdon(%Player,502032004, 5000);
			addLivingSkillGuerdon(%Player,502032005, 5000);
			addLivingSkillGuerdon(%Player,503011001, 5000);
			addLivingSkillGuerdon(%Player,503011002, 5000);
			addLivingSkillGuerdon(%Player,503011003, 5000);
			addLivingSkillGuerdon(%Player,503011004, 5000);
			addLivingSkillGuerdon(%Player,503011005, 5000);
			addLivingSkillGuerdon(%Player,503022001, 5000);
			addLivingSkillGuerdon(%Player,503022002, 5000);
			addLivingSkillGuerdon(%Player,503022003, 5000);
			addLivingSkillGuerdon(%Player,503022004, 5000);
			addLivingSkillGuerdon(%Player,503022005, 5000);
			addLivingSkillGuerdon(%Player,503032001, 5000);
			addLivingSkillGuerdon(%Player,503032002, 5000);
			addLivingSkillGuerdon(%Player,503032003, 5000);
			addLivingSkillGuerdon(%Player,503032004, 5000);
			addLivingSkillGuerdon(%Player,503032005, 5000);
			addLivingSkillGuerdon(%Player,503042001, 5000);
			addLivingSkillGuerdon(%Player,503042002, 5000);
			addLivingSkillGuerdon(%Player,503042003, 5000);
			addLivingSkillGuerdon(%Player,503042004, 5000);
			addLivingSkillGuerdon(%Player,503042005, 5000);
			addLivingSkillGuerdon(%Player,504011001, 5000);
			addLivingSkillGuerdon(%Player,504011002, 5000);
			addLivingSkillGuerdon(%Player,504011003, 5000);
			addLivingSkillGuerdon(%Player,504011004, 5000);
			addLivingSkillGuerdon(%Player,504011005, 5000);
			addLivingSkillGuerdon(%Player,504022001, 5000);
			addLivingSkillGuerdon(%Player,504022002, 5000);
			addLivingSkillGuerdon(%Player,504022003, 5000);
			addLivingSkillGuerdon(%Player,504022004, 5000);
			addLivingSkillGuerdon(%Player,504022005, 5000);
			addLivingSkillGuerdon(%Player,505011001, 5000);
			addLivingSkillGuerdon(%Player,505011002, 5000);
			addLivingSkillGuerdon(%Player,505011003, 5000);
			addLivingSkillGuerdon(%Player,505011004, 5000);
			addLivingSkillGuerdon(%Player,505011005, 5000);
			addLivingSkillGuerdon(%Player,505022001, 5000);
			addLivingSkillGuerdon(%Player,505022002, 5000);
			addLivingSkillGuerdon(%Player,505022003, 5000);
			addLivingSkillGuerdon(%Player,505022004, 5000);
			addLivingSkillGuerdon(%Player,505022005, 5000);
			addLivingSkillGuerdon(%Player,506011001, 5000);
			addLivingSkillGuerdon(%Player,506011002, 5000);
			addLivingSkillGuerdon(%Player,506011003, 5000);
			addLivingSkillGuerdon(%Player,506011004, 5000);
			addLivingSkillGuerdon(%Player,506011005, 5000);
			addLivingSkillGuerdon(%Player,506022001, 5000);
			addLivingSkillGuerdon(%Player,506022002, 5000);
			addLivingSkillGuerdon(%Player,506022003, 5000);
			addLivingSkillGuerdon(%Player,506022004, 5000);
			addLivingSkillGuerdon(%Player,506022005, 5000);
			addLivingSkillGuerdon(%Player,507011001, 5000);
			addLivingSkillGuerdon(%Player,507011002, 5000);
			addLivingSkillGuerdon(%Player,507011003, 5000);
			addLivingSkillGuerdon(%Player,507011004, 5000);
			addLivingSkillGuerdon(%Player,507011005, 5000);
			addLivingSkillGuerdon(%Player,507022001, 5000);
			addLivingSkillGuerdon(%Player,507022002, 5000);
			addLivingSkillGuerdon(%Player,507022003, 5000);
			addLivingSkillGuerdon(%Player,507022004, 5000);
			addLivingSkillGuerdon(%Player,507022005, 5000);
			addLivingSkillGuerdon(%Player,508011001, 5000);
			addLivingSkillGuerdon(%Player,508011002, 5000);
			addLivingSkillGuerdon(%Player,508011003, 5000);
			addLivingSkillGuerdon(%Player,508011004, 5000);
			addLivingSkillGuerdon(%Player,508011005, 5000);
			addLivingSkillGuerdon(%Player,509011001, 5000);
			addLivingSkillGuerdon(%Player,509011002, 5000);
			addLivingSkillGuerdon(%Player,509011003, 5000);
			addLivingSkillGuerdon(%Player,509011004, 5000);
			addLivingSkillGuerdon(%Player,509011005, 5000);

		case 310:
			%Conv.AddOption(1001, 100106 + 500000);	//�����������
			%Conv.AddOption(1002, 100203 + 500000);	//���������ľ�
			
			%Conv.AddOption(1011, 101101 + 500000);	//���������ڵ�ͼ"������(ʥ)"���ع���
			%Conv.AddOption(1006, 100601 + 500000);	//���������ڵ�ͼ"������(��)"��������
			%Conv.AddOption(1007, 100701 + 500000);	//���������ڵ�ͼ"������(��)"�����ɾ�
			%Conv.AddOption(1005, 100501 + 500000);	//���������ڵ�ͼ"�ɻ���(��)"��ľ��
			%Conv.AddOption(1009, 100901 + 500000);	//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
			%Conv.AddOption(1010, 101001 + 500000);	//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
			%Conv.AddOption(1008, 100801 + 500000);	//���������ڵ�ͼ"���鹬(��)"���α���
			%Conv.AddOption(1004, 100401 + 500000);	//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
			
			%Conv.AddOption(1101, 110101 + 500000);	//���͵�����
			%Conv.AddOption(1102, 110202 + 500000);	//���͵�������
			%Conv.AddOption(1103, 110301 + 500000);	//���͵����ǲ�Ұ
			%Conv.AddOption(1104, 110401 + 500000);	//���͵�����ɭ��
			
			//������ͼ
			if(GetSubStr(GetZoneID(), 1, 1) == 3)
				%Conv.AddOption(311, 100000 + 500000);	//���������Ծ�
			else
				{
					%Conv.AddOption(1301, 130101 + 500000);	//��������¥
					%Conv.AddOption(1302, 130201 + 500000);	//����ҹ������
					%Conv.AddOption(1303, 130301 + 500000);	//����Ů���������
					%Conv.AddOption(1304, 130401 + 500000);	//����ˮī��������ǰ��
					%Conv.AddOption(1305, 130501 + 500000);	//���������Ծ�
				}
			
		default:
			if( (%State >= 19)&&(%State <= 25) )
				{
					%Conv.SetText(%Type);			//��ʾ�Ի�
//					if(%State == 19)
					if(%State == 20)
						{
							%Conv.AddOption(200, 200);	//����
							%Conv.AddOption(201, 201);	//��1��
							%Conv.AddOption(202, 202);	//��2��
							%Conv.AddOption(203, 203);	//��3��
							%Conv.AddOption(204, 204);	//��4��
							%Conv.AddOption(205, 205);	//��5��
//							%Conv.AddOption(206, 206);	//��6��
						}
//					if(%State == 21)
//					if(%State == 22)
//					if(%State == 23)
//					if(%State == 24)
//					if(%State == 25)

					%Conv.AddOption(4, 14);	//����
				}

			//���»����񡿵�ѡ��
			if( (%State >= 200)&&(%State < 300) )
				{
					%Conv.SetText(%Type);			//��ʾ�Ի�

					%Conv.AddOption(299, %State * 10 + 1);	//��1��
					%Conv.AddOption(298, %State * 10 + 2);	//��2��
					%Conv.AddOption(297, %State * 10 + 3);	//��3��
					%Conv.AddOption(296, %State * 10 + 4);	//��1����
					%Conv.AddOption(295, %State * 10 + 5);	//��2����
					%Conv.AddOption(294, %State * 10 + 6);	//��3����

					%Conv.AddOption(4, 14);	//����
				}

			//���»����񡿾�����������
			if( (%State >= 2000)&&(%State < 3000) )
				{
					%Conv.SetText(%Type);			//��ʾ�Ի�

					//ÿ�����ʼ���
					%Juan = GetSubStr(%State,0,3);
					if(%Juan $= "200"){%A = 10000;}
					if(%Juan $= "201"){%A = 10100;}
					if(%Juan $= "202"){%A = 10300;}
					if(%Juan $= "203"){%A = 10500;}
					if(%Juan $= "204"){%A = 10700;}
					if(%Juan $= "205"){%A = 10900;}
					//ÿ�µ���ʼ���
					if(GetSubStr(%State,3,1) $= "1"){%B = 0;}
					if(GetSubStr(%State,3,1) $= "2"){%B = 40;}
					if(GetSubStr(%State,3,1) $= "3"){%B = 80;}
					if(GetSubStr(%State,3,1) $= "4"){%B = 120;}
					if(GetSubStr(%State,3,1) $= "5"){%B = 140;}
					if(GetSubStr(%State,3,1) $= "6"){%B = 160;}

					for(%k = 0;%k < 40;%k++)
					{
						%Mid = %A + %B + %k;
						%MissionData = "MissionData_" @ %Mid;

						if(%MissionData.Mid $= "")
							break;
						else
							%Conv.AddOption(999 @ %Mid, %Mid);	//��ʶ���е�ǰ������
					}

					%Conv.AddOption(4, %Juan);	//����
				}

			//���嵥��������Ҫִ�еĲ���
			if( (%State >= 10000)&&(%State < 500000) )
				{
					%Conv.SetText(27);			//��ʾ�Ի�

					%Conv.AddOption(15, 1 @ %State);	//ֱ�ӽ���
					%Conv.AddOption(16, 2 @ %State);	//ֱ�����
					%Conv.AddOption(17, 3 @ %State);	//ǿ�Ʒ���
					%Conv.AddOption(18, 4 @ %State);	//���ó�δ���״̬

					%Conv.AddOption(4, 14);	//����

					if( (%State >= 100000)&&(%State < 500000) )
						{
							%Mission = GetSubStr(%State,1,5);

							if(GetSubStr(%State,0,1) $= "1")	//ֱ�ӽ���
								AddMissionAccepted(%Player, %Mission);	//�������������Ϣ

							if(GetSubStr(%State,0,1) $= "2")	//ֱ�����
								AddMissionReward(%Player, %Mission, 0);	//����������

							if(GetSubStr(%State,0,1) $= "3")	//ǿ�Ʒ���
								%Player.DelMission(%Mission);

							if(GetSubStr(%State,0,1) $= "4")	//���ó�δ���״̬
								%Player.SetMission(%Mission,0);
						}
				}
			//���͵�ͼ����
			if( (%State >= 600000)&&(%State < 700000) )
				{
					echo("�¹ⱦ�д��� �� %State = "@%State);
					%Conv.SetType(4);	//�رնԻ�
					
					%MapID = %State - 500000;
					
					if(%MapID == 100000)
						SptCopymap_RemovePlayer( %Player.GetPlayerID() );
					else
						{
							if(GetSubStr(%MapID,1,1) $= "3")
								GoToNextMap_CopyMap( %Player, GetSubStr(%MapID,0,4) );
							else
								GoToNextMap_Normal(%Player, %MapID);
						}
				}

	}
	%Conv.Send(%Player);
}
//����
function Trigger_108020266(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10113;
	if(%Player.IsAcceptedMission(%Mid))
		{
			%ItemAdd = %Player.PutItem(108020266, -1);
			%ItemAdd = %Player.AddItem();

			%Buff = GetRandom(1,3);
			if(%Buff == 1){%Player.AddBuff(320120001);}
			if(%Buff == 2){%Player.AddBuff(320120002);}	
		  if(%Buff == 3){%Player.AddBuff(320120003);}
			%Player.SetMissionFlag(%Mid, 1300, 1, true); 				//����,Ŀ�괥����ǰ����
		}
}

//������������ӡ
function Trigger_118010001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ�Ի�
			%Conv.AddOption(10, -1);	//�ر�
	}
	%Conv.Send(%Player);
}

//����ƿ
function Trigger_108020071(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10185;

	if(%Player.IsAcceptedMission(%Mid))
		{
			for(%i = 0; %i < 3; %i++)
			{
				%AreaID = "81101010" @ 4 + %i;
				echo("%AreaID = "@%AreaID);

				if($Player_Area[%Player.GetPlayerID()] $= %AreaID)
					if(%Player.GetItemCount(%Player.GetMissionFlag(%Mid, 2100 + %i)) < %Player.GetMissionFlag(%Mid, 2200 + %i))
						%Player.PutItem(%Player.GetMissionFlag(%Mid, 2100 + %i), 1);
			}

			%ItemAdd = %Player.AddItem();
		}

	if(!%ItemAdd)
		{
			SendOneChatMessage(0,"<t>��������</t>",%Player);
			SendOneScreenMessage(2,"��������", %Player);
			
			return;
		}
	
	%Conv.Send(%Player);
}

function Trigger_105089001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ�Ի�
			%Conv.AddOption(8, 999);	//��
		case 999:
			%Conv.SetType(4);	//�رնԻ�
			%Player.PutItem(%Type, -1);
			%Player.AddItem();
	}
	%Conv.Send(%Player);
}

//�سǷ�
function Trigger_113010001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	echo("Trigger_113010001");
	GoToNextMap_Normal(%Player, 100109);
}

function Trigger_113020001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 101101);}//113020001	�����ڷ��з�
function Trigger_113020002(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100601);}//113020002	�����·��з�
function Trigger_113020003(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100701);}//113020003	�����ɷ��з�
function Trigger_113020004(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100501);}//113020004	�ɻ��ȷ��з�
function Trigger_113020005(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100901);}//113020005	���Ľ̷��з�
function Trigger_113020006(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 101001);}//113020006	ɽ���ڷ��з�
function Trigger_113020007(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100801);}//113020007	���鹬���з�
function Trigger_113020008(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100401);}//113020008	��ħ�ŷ��з�

function Trigger_113020011(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 101101);}//113020011	�����ڷ��з�
function Trigger_113020012(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100601);}//113020012	�����·��з�
function Trigger_113020013(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100701);}//113020013	�����ɷ��з�
function Trigger_113020014(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100501);}//113020014	�ɻ��ȷ��з�
function Trigger_113020015(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100901);}//113020015	���Ľ̷��з�
function Trigger_113020016(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 101001);}//113020016	ɽ���ڷ��з�
function Trigger_113020017(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100801);}//113020017	���鹬���з�
function Trigger_113020018(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100401);}//113020018	��ħ�ŷ��з�

//��˵���
function Trigger_104010001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	echo("Trigger_104010001");
	echo("Player = " @ %Player);

	%Player.MountTest();
}

//���ӱ���
function Trigger_118010002(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	echo("Trigger_118010002");
	//�ٻ����ɢ
	PetTest();
}

//�ɷ�
function Trigger_108020259(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10223;
	%AreaID = "810010118";
  %PlayerID = %Player.GetPlayerID();

  if(%Player.IsAcceptedMission(%Mid))
  	{
    	if($Player_Area[%PlayerID] $= %AreaID)
				{
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)
						{
							%ItemAdd = %Player.PutItem(108020250,1);
							%ItemAdd = %Player.AddItem();

							if(!%ItemAdd)
								{
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);

									return;
							  }
							%Player.SetMissionFlag(%Mid, 1300, 1, true);
					  }
				}
		}
}

//����
function Trigger_108020258(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10236;
	%AreaID = "811010107";
  %PlayerID = %Player.GetPlayerID();

  if(%Player.IsAcceptedMission(%Mid))
  	{
    	if($Player_Area[%PlayerID] $= %AreaID)
				{
					if($New_108020258 == 0)
						{
					 		%Player.SetMissionFlag(%Mid, 1300, 1, true);
				   		$New_108020258 = SpNewNpc(%Player,400002116,"-367.59 -434.486 37.067",0);
					 		Schedule(60000, 0, "RemoveNpc_108020258");
						}
				}
		}
}

function RemoveNpc_108020258()
{
	$New_108020258.SafeDeleteObject();
	$New_108020258 = 0;
}

//�ڹ�Ѫ
function Trigger_108020016(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid1 =10163;
	%Mid2 =10164;
	%Mid3 =10165;
  %CopyMapID = %Player.GetLayerId();
  
	%AreaID1 = "813020101";
	%AreaID2 = "813020102";
	%AreaID3 = "813020103";
  
  %PlayerID = %Player.GetPlayerID();

  	if(%Player.IsAcceptedMission(%Mid1))
	    {
				if(($Player_Area[%PlayerID] $= %AreaID1)&&(%Player.GetItemCount(108020065)==0))
					{
						if($New_108020016_1 ==0)
							{
		        		$New_108020016_1 = SpNewNpc3(%Player, 410500064, "-259.635 -287.976 12.0111", %CopyMapID,0,"2 2 2");
		        		Schedule(320000, 0, "RemoveNpc_108020016_1");
		        	}
					}
	    }

	 if(%Player.IsAcceptedMission(%Mid2))
	   {
			 if(($Player_Area[%PlayerID] $= %AreaID2)&&(%Player.GetItemCount(108020066)==0))
				 {
				 	 if($New_108020016_2 ==0)
							{
				       	$New_108020016_2 = SpNewNpc3(%Player, 410500065, "-274.15 -292.852 12.1879", %CopyMapID,0,"2 2 2");
				       	Schedule(320000, 0, "RemoveNpc_108020016_2");
				      }
				 }
	   }

	if(%Player.IsAcceptedMission(%Mid3))
	  {
			if(($Player_Area[%PlayerID] $= %AreaID3)&&(%Player.GetItemCount(108020067)==0))
				{
					if($New_108020016_3 ==0)
						{
		      		$New_108020016_3 = SpNewNpc3(%Player, 410500066, "-263.715 -302.646 12.1856", %CopyMapID,0,"2 2 2");
		      		Schedule(320000, 0, "RemoveNpc_108020016_3");
		      	}
				}

	  }
}

function RemoveNpc_108020016_1()
{
	$New_108020016_1.SafeDeleteObject();
	$New_108020016_1 =0;
}
function RemoveNpc_108020016_2()
{
	$New_108020016_2.SafeDeleteObject();
	$New_108020016_2 =0;
}
function RemoveNpc_108020016_3()
{
	$New_108020016_3.SafeDeleteObject();
	$New_108020016_3 =0;
}

//�����������
function Trigger_108020101(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10319;

  %AreaID = "811020103";
  %PlayerID = %Player.GetPlayerID();

	if(%Player.IsAcceptedMission(%Mid))
	  {
			if(($Player_Area[%PlayerID] $= %AreaID)&&(%Player.GetItemCount(108020102)==0))
				{
					
					if($New_108020101 == 0)
						{	
	        		$New_108020101 = SpNewNpc3(%Player, 411101008, "-5.21105 52.3665 72.3145", 0,0,"2 2 2");
	        		Schedule(120000, 0, "RemoveNpc_108020101");
	        	}		
				}
	  }
}

function RemoveNpc_108020101()
{
	$New_108020101.SafeDeleteObject();
	$New_108020101 = 0;
}


//����
function Trigger_108020119(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10343;

  %AreaID1 = "811020104";
  %AreaID2 = "811020105";
  %PlayerID = %Player.GetPlayerID();

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(($Player_Area[%PlayerID] $= %AreaID1)&&(%Player.GetItemCount(108020105)==0))
				{
					if($New_108020119_1 == 0)
						{	
			        $New_108020119_1 = SpNewNpc(%Player,410601001,0,0);
			        Schedule(320000, 0, "RemoveNpc_108020119_1");
			      }
				}

			if(($Player_Area[%PlayerID] $= %AreaID2)&&(%Player.GetItemCount(108020106)==0))
			  {
			  	if($New_108020119_2 == 0)
						{	
			        $New_108020119_2 = SpNewNpc(%Player,410601002,0,0);
			        Schedule(320000, 0, "RemoveNpc_108020119_2");
			      }
				}
	  }

}

function RemoveNpc_108020119_1()
{
	$New_108020119_1.SafeDeleteObject();
	$New_108020119_1 = 0;
}

function RemoveNpc_108020119_2()
{
	$New_108020119_2.SafeDeleteObject();
	$New_108020119_2 = 0;
}


//�¾ɵĺ���
function Trigger_108020107(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10344;

  %AreaID = "811020109";

  %PlayerID = %Player.GetPlayerID();

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(($Player_Area[%PlayerID] $= %AreaID)&&(%Player.GetMissionFlag(%Mid,1300) == 0))
				{
					if($New_108020107 == 0)
						{
							%Player.SetMissionFlag(%Mid, 1300, 1, true);
							
							$New_108020107 = SpNewNpc(%Player,401102124,"-47.3281 77.0148 71.8691",0);
							Schedule(20000, 0, "RemoveNpc_108020107");
						}
			  }
		 }
}

function RemoveNpc_108020107()
{
	$New_108020107.SafeDeleteObject();
	$New_108020107 = 0;
}


//Կ��
function Trigger_108020117(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10392;
  %AreaID = " 811020106";
  %PlayerID = %Player.GetPlayerID();

	if(%Player.IsAcceptedMission(%Mid))
		{
				if($Player_Area[%PlayerID] $= %AreaID)
					{
						%Player.SetMissionFlag(%Mid, 1300, 1, true);
					}

		}

}

//���������
function Trigger_108020130(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10513;
  
	if(%Player.IsAcceptedMission(%Mid))
		{
			%ItemAdd = %Player.PutItem(108020130, -1);
			%ItemAdd = %Player.AddItem();
			
			%Player.AddBuff(320120002);
			%Player.SetMissionFlag(%Mid, 1300, 1, true);
		}

}


//����ͩ����
function Trigger_108020139(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10523;
	%AreaID = "811030113";
  %PlayerID = %Player.GetPlayerID();

  if(%Player.IsAcceptedMission(%Mid))
  	{
    	if($Player_Area[%PlayerID] $= %AreaID)
				{
					if($New_108020139 ==0)
						{
							$New_108020139 = SpNewNpc3(%Player, 410400005, "128.479 14.0543 77.3676", 0,0,"2 2 2");
							Schedule(320000, 0, "RemoveNpc_108020139");
						}
				}
		}
}

function RemoveNpc_108020139()
{
	$New_108020139 =0;
	$New_108020139.SafeDeleteObject();
}

//�˻Ľ�������һ��
function Trigger_108020142(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10525;
  %PlayerID = %Player.GetPlayerID();

  if(%Player.IsAcceptedMission(%Mid))
  	{
    	if((%Player.GetItemCount(108020143) >= 1)&&(%Player.GetItemCount(108020144) == 0))
					{

								%ItemAdd = %Player.PutItem(108020144, 1);
								%ItemAdd = %Player.PutItem(108020142, -1);
								%ItemAdd = %Player.PutItem(108020143, -1);
								%ItemAdd = %Player.AddItem();

								if(!%ItemAdd)
							  {
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);

									return;
							  }

								%Player.SetMissionFlag(%Mid, 2103, "");		//����,ITEM���-ȡ��
								%Player.SetMissionFlag(%Mid, 2203, "");		//����,ITEM��������-ȡ��
								%Player.SetMissionFlag(%Mid, 2303, "");		//����,ITEM���䵱ǰ����-ȡ��

                %Player.SetMissionFlag(%Mid, 2104, "");		//����,ITEM���-ȡ��
								%Player.SetMissionFlag(%Mid, 2204, "");		//����,ITEM��������-ȡ��
								%Player.SetMissionFlag(%Mid, 2304, "");		//����,ITEM���䵱ǰ����-ȡ��

								%Item = 0;

							  for(%i = 3; %i < 9; %i++)
							  	if(%Player.GetMissionFlag(%Mid, 2100 + %i) > 0)
							  		%Item[%Item++] = %Player.GetMissionFlag(%Mid, 2100 + %i);

										%Player.SetMissionFlag(%Mid, 2105, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2205, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2305, "");		//����,ITEM���䵱ǰ����-ȡ��

		                %Player.SetMissionFlag(%Mid, 2106, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2206, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2306, "");		//����,ITEM���䵱ǰ����-ȡ��

										%Player.SetMissionFlag(%Mid, 2107, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2207, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2307, "");		//����,ITEM���䵱ǰ����-ȡ��

                    %Player.SetMissionFlag(%Mid, 2108, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2208, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2308, "");		//����,ITEM���䵱ǰ����-ȡ��

							  for(%ii = 0; %ii < %Item; %ii++)
							  {
							  	%ItemID = %Item[%ii + 1];

									%Player.SetMissionFlag(%Mid, 2100 + %ii + 3, %ItemID);//����,ITEM���-ȡ��
									%Player.SetMissionFlag(%Mid, 2200 + %ii + 3, 1);		//����,ITEM��������-ȡ��
									%Player.SetMissionFlag(%Mid, 2300 + %ii + 3, %Player.GetItemCount(%ItemID));		//����,ITEM���䵱ǰ����-ȡ��
							  }

								%Player.UpdateMission(%Mid);					//����������Ϣ

					}
		}
}

//�˻Ľ����������
function Trigger_108020143(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10525;
  %PlayerID = %Player.GetPlayerID();

  if(%Player.IsAcceptedMission(%Mid))
  	{
    	if((%Player.GetItemCount(108020142) >= 1)&&(%Player.GetItemCount(108020144) == 0))
					{

								%ItemAdd = %Player.PutItem(108020144, 1);
								%ItemAdd = %Player.PutItem(108020142, -1);
								%ItemAdd = %Player.PutItem(108020143, -1);
								%ItemAdd = %Player.AddItem();

								if(!%ItemAdd)
							  {
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);

									return;
							  }

								%Player.SetMissionFlag(%Mid, 2103, "");		//����,ITEM���-ȡ��
								%Player.SetMissionFlag(%Mid, 2203, "");		//����,ITEM��������-ȡ��
								%Player.SetMissionFlag(%Mid, 2303, "");		//����,ITEM���䵱ǰ����-ȡ��

                %Player.SetMissionFlag(%Mid, 2104, "");		//����,ITEM���-ȡ��
								%Player.SetMissionFlag(%Mid, 2204, "");		//����,ITEM��������-ȡ��
								%Player.SetMissionFlag(%Mid, 2304, "");		//����,ITEM���䵱ǰ����-ȡ��

								%Item = 0;

							  for(%i = 3; %i < 9; %i++)
							  	if(%Player.GetMissionFlag(%Mid, 2100 + %i) > 0)
							  		%Item[%Item++] = %Player.GetMissionFlag(%Mid, 2100 + %i);

										%Player.SetMissionFlag(%Mid, 2105, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2205, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2305, "");		//����,ITEM���䵱ǰ����-ȡ��

		                %Player.SetMissionFlag(%Mid, 2106, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2206, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2306, "");		//����,ITEM���䵱ǰ����-ȡ��

										%Player.SetMissionFlag(%Mid, 2107, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2207, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2307, "");		//����,ITEM���䵱ǰ����-ȡ��

                    %Player.SetMissionFlag(%Mid, 2108, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2208, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2308, "");		//����,ITEM���䵱ǰ����-ȡ��

							  for(%ii = 0; %ii < %Item; %ii++)
							  {
							  	%ItemID = %Item[%ii + 1];

									%Player.SetMissionFlag(%Mid, 2100 + %ii + 3, %ItemID);//����,ITEM���-ȡ��
									%Player.SetMissionFlag(%Mid, 2200 + %ii + 3, 1);		//����,ITEM��������-ȡ��
									%Player.SetMissionFlag(%Mid, 2300 + %ii + 3, %Player.GetItemCount(%ItemID));		//����,ITEM���䵱ǰ����-ȡ��
							  }

								%Player.UpdateMission(%Mid);					//����������Ϣ

					}

		}
}

//ӳ�»��Ǿ���һ��
function Trigger_108020145(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10525;
  %PlayerID = %Player.GetPlayerID();

  if(%Player.IsAcceptedMission(%Mid))
  	{
    	if((%Player.GetItemCount(108020146) >= 1)&&(%Player.GetItemCount(108020147) == 0))
					{

								%ItemAdd = %Player.PutItem(108020147, 1);
								%ItemAdd = %Player.PutItem(108020145, -1);
								%ItemAdd = %Player.PutItem(108020146, -1);
								%ItemAdd = %Player.AddItem();

								if(!%ItemAdd)
							  {
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);

									return;
							  }

							  if(%Player.GetMissionFlag(%Mid, 2103) $= "108020145")
							  	{
										%Player.SetMissionFlag(%Mid, 2103, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2203, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2303, "");		//����,ITEM���䵱ǰ����-ȡ��

		                %Player.SetMissionFlag(%Mid, 2104, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2204, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2304, "");		//����,ITEM���䵱ǰ����-ȡ��
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2105) $= "108020145")
							  	{
										%Player.SetMissionFlag(%Mid, 2105, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2205, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2305, "");		//����,ITEM���䵱ǰ����-ȡ��

			              %Player.SetMissionFlag(%Mid, 2106, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2206, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2306, "");		//����,ITEM���䵱ǰ����-ȡ��
							  	}

								%Item = 0;

							  for(%i = 3; %i < 9; %i++)
							  	if(%Player.GetMissionFlag(%Mid, 2100 + %i) > 0)
							  		%Item[%Item++] = %Player.GetMissionFlag(%Mid, 2100 + %i);

										%Player.SetMissionFlag(%Mid, 2103, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2203, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2303, "");		//����,ITEM���䵱ǰ����-ȡ��

		                %Player.SetMissionFlag(%Mid, 2104, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2204, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2304, "");		//����,ITEM���䵱ǰ����-ȡ��

										%Player.SetMissionFlag(%Mid, 2105, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2205, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2305, "");		//����,ITEM���䵱ǰ����-ȡ��

			              %Player.SetMissionFlag(%Mid, 2106, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2206, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2306, "");		//����,ITEM���䵱ǰ����-ȡ��

										%Player.SetMissionFlag(%Mid, 2107, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2207, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2307, "");		//����,ITEM���䵱ǰ����-ȡ��

                    %Player.SetMissionFlag(%Mid, 2108, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2208, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2308, "");		//����,ITEM���䵱ǰ����-ȡ��

							  for(%ii = 0; %ii < %Item; %ii++)
							  {
							  	%ItemID = %Item[%ii + 1];

									%Player.SetMissionFlag(%Mid, 2100 + %ii + 3, %ItemID);//����,ITEM���-ȡ��
									%Player.SetMissionFlag(%Mid, 2200 + %ii + 3, 1);		//����,ITEM��������-ȡ��
									%Player.SetMissionFlag(%Mid, 2300 + %ii + 3, %Player.GetItemCount(%ItemID));		//����,ITEM���䵱ǰ����-ȡ��
							  }

								%Player.UpdateMission(%Mid);					//����������Ϣ

					}
		}
}

//ӳ�»��Ǿ������
function Trigger_108020146(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10525;
  %PlayerID = %Player.GetPlayerID();

  if(%Player.IsAcceptedMission(%Mid))
  	{
    	if((%Player.GetItemCount(108020145) >= 1)&&(%Player.GetItemCount(108020147) == 0))
					{

								%ItemAdd = %Player.PutItem(108020147, 1);
								%ItemAdd = %Player.PutItem(108020145, -1);
								%ItemAdd = %Player.PutItem(108020146, -1);
								%ItemAdd = %Player.AddItem();

								if(!%ItemAdd)
							  {
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);

									return;
							  }

							  if(%Player.GetMissionFlag(%Mid, 2104) $= "108020146")
							  	{
										%Player.SetMissionFlag(%Mid, 2103, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2203, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2303, "");		//����,ITEM���䵱ǰ����-ȡ��

		                %Player.SetMissionFlag(%Mid, 2104, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2204, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2304, "");		//����,ITEM���䵱ǰ����-ȡ��
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2106) $= "108020146")
							  	{
										%Player.SetMissionFlag(%Mid, 2105, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2205, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2305, "");		//����,ITEM���䵱ǰ����-ȡ��

			              %Player.SetMissionFlag(%Mid, 2106, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2206, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2306, "");		//����,ITEM���䵱ǰ����-ȡ��
							  	}

								%Item = 0;

							  for(%i = 3; %i < 9; %i++)
							  	if(%Player.GetMissionFlag(%Mid, 2100 + %i) > 0)
							  		%Item[%Item++] = %Player.GetMissionFlag(%Mid, 2100 + %i);

										%Player.SetMissionFlag(%Mid, 2103, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2203, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2303, "");		//����,ITEM���䵱ǰ����-ȡ��

		                %Player.SetMissionFlag(%Mid, 2104, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2204, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2304, "");		//����,ITEM���䵱ǰ����-ȡ��

										%Player.SetMissionFlag(%Mid, 2105, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2205, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2305, "");		//����,ITEM���䵱ǰ����-ȡ��

			              %Player.SetMissionFlag(%Mid, 2106, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2206, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2306, "");		//����,ITEM���䵱ǰ����-ȡ��

										%Player.SetMissionFlag(%Mid, 2107, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2207, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2307, "");		//����,ITEM���䵱ǰ����-ȡ��

                    %Player.SetMissionFlag(%Mid, 2108, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2208, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2308, "");		//����,ITEM���䵱ǰ����-ȡ��

							  for(%ii = 0; %ii < %Item; %ii++)
							  {
							  	%ItemID = %Item[%ii + 1];

									%Player.SetMissionFlag(%Mid, 2100 + %ii + 3, %ItemID);//����,ITEM���-ȡ��
									%Player.SetMissionFlag(%Mid, 2200 + %ii + 3, 1);		//����,ITEM��������-ȡ��
									%Player.SetMissionFlag(%Mid, 2300 + %ii + 3, %Player.GetItemCount(%ItemID));		//����,ITEM���䵱ǰ����-ȡ��
							  }

								%Player.UpdateMission(%Mid);					//����������Ϣ

					}
		}
}

//������������һ��
function Trigger_108020148(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10525;
  %PlayerID = %Player.GetPlayerID();

  if(%Player.IsAcceptedMission(%Mid))
  	{
    	if((%Player.GetItemCount(108020149) >= 1)&&(%Player.GetItemCount(108020150) == 0))
					{

								%ItemAdd = %Player.PutItem(108020150, 1);
								%ItemAdd = %Player.PutItem(108020148, -1);
								%ItemAdd = %Player.PutItem(108020149, -1);
								%ItemAdd = %Player.AddItem();


								if(!%ItemAdd)
							  {
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);

									return;
							  }

							  if(%Player.GetMissionFlag(%Mid, 2103) $= "108020148")
							  	{
										%Player.SetMissionFlag(%Mid, 2103, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2203, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2303, "");		//����,ITEM���䵱ǰ����-ȡ��

		                %Player.SetMissionFlag(%Mid, 2104, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2204, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2304, "");		//����,ITEM���䵱ǰ����-ȡ��
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2105) $= "108020148")
							  	{
										%Player.SetMissionFlag(%Mid, 2105, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2205, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2305, "");		//����,ITEM���䵱ǰ����-ȡ��

			              %Player.SetMissionFlag(%Mid, 2106, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2206, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2306, "");		//����,ITEM���䵱ǰ����-ȡ��
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2107) $= "108020148")
							  	{
										%Player.SetMissionFlag(%Mid, 2107, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2207, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2307, "");		//����,ITEM���䵱ǰ����-ȡ��

                    %Player.SetMissionFlag(%Mid, 2108, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2208, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2308, "");		//����,ITEM���䵱ǰ����-ȡ��
									}

								%Player.UpdateMission(%Mid);					//����������Ϣ

					}
		}
}

//���������������
function Trigger_108020149(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10525;
  %PlayerID = %Player.GetPlayerID();

  if(%Player.IsAcceptedMission(%Mid))
  	{
    	if((%Player.GetItemCount(108020148) >= 1)&&(%Player.GetItemCount(108020150) == 0))
					{

								%ItemAdd = %Player.PutItem(108020150, 1);
								%ItemAdd = %Player.PutItem(108020148, -1);
								%ItemAdd = %Player.PutItem(108020149, -1);
								%ItemAdd = %Player.AddItem();

								if(!%ItemAdd)
							  {
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);

									return;
							  }

							  if(%Player.GetMissionFlag(%Mid, 2104) $= "108020149")
							  	{
										%Player.SetMissionFlag(%Mid, 2103, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2203, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2303, "");		//����,ITEM���䵱ǰ����-ȡ��

		                %Player.SetMissionFlag(%Mid, 2104, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2204, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2304, "");		//����,ITEM���䵱ǰ����-ȡ��
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2106) $= "108020149")
							  	{
										%Player.SetMissionFlag(%Mid, 2105, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2205, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2305, "");		//����,ITEM���䵱ǰ����-ȡ��

			              %Player.SetMissionFlag(%Mid, 2106, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2206, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2306, "");		//����,ITEM���䵱ǰ����-ȡ��
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2108) $= "108020149")
							  	{
										%Player.SetMissionFlag(%Mid, 2107, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2207, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2307, "");		//����,ITEM���䵱ǰ����-ȡ��

                    %Player.SetMissionFlag(%Mid, 2108, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2208, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2308, "");		//����,ITEM���䵱ǰ����-ȡ��
									}

								%Player.UpdateMission(%Mid);					//����������Ϣ

					}
		}
}

//����ձ�
function Trigger_108020175(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid = 10706;

	%AreaID = "810020104";
  %PlayerID = %Player.GetPlayerID();

  if(%Player.IsAcceptedMission(%Mid))
		{
			if((%Player.GetMissionFlag(%Mid,1300) == 0)&&($Player_Area[%PlayerID] $= %AreaID))
				{
						%ItemAdd = %Player.PutItem(108020176, 1);
						%ItemAdd = %Player.PutItem(108020175, -1);
					  %ItemAdd = %Player.AddItem();


						if(!%ItemAdd)
							{
								SendOneChatMessage(0,"<t>��������</t>",%Player);
								SendOneScreenMessage(2,"��������", %Player);

								return;
							}

						%Player.SetMissionFlag(%Mid, 2300, 1, true);
						%Player.SetMissionFlag(%Mid, 1300, 1, true);
			 }
		}
}

//��������
function Trigger_108020202(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%Mid1 = 10789;
  %Mid2 = 10790;

	if(%Player.IsAcceptedMission(%Mid1))
		{
			 	%ItemAdd = %Player.PutItem(108020204, 1);
				%ItemAdd = %Player.PutItem(108020203, -6);
			  %ItemAdd = %Player.PutItem(108020202, -1);
			  %ItemAdd = %Player.AddItem();

			  if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>��������</t>",%Player);
						SendOneScreenMessage(2,"��������", %Player);

						return;
					}
          %Player.SetMissionFlag(%Mid, 1300, 1, true);
					%Player.SetMissionFlag(%Mid, 2100, "");		//����,ITEM���-ȡ��
				  %Player.SetMissionFlag(%Mid, 2200, "");		//����,ITEM��������-ȡ��
					%Player.SetMissionFlag(%Mid, 2300, "");		//����,ITEM���䵱ǰ����-ȡ��
		}

	if(%Player.IsAcceptedMission(%Mid2))
		{
			 	%ItemAdd = %Player.PutItem(108020204, 1);
				%ItemAdd = %Player.PutItem(108020205, -6);
			  %ItemAdd = %Player.PutItem(108020202, -1);
			  %ItemAdd = %Player.AddItem();

			  if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>��������</t>",%Player);
						SendOneScreenMessage(2,"��������", %Player);

						return;
					}
          %Player.SetMissionFlag(%Mid, 1300, 1, true);
					%Player.SetMissionFlag(%Mid, 2100, "");		//����,ITEM���-ȡ��
				  %Player.SetMissionFlag(%Mid, 2200, "");		//����,ITEM��������-ȡ��
					%Player.SetMissionFlag(%Mid, 2300, "");		//����,ITEM���䵱ǰ����-ȡ��
		 }
}

//�ϳɱ���
function Trigger_105090006(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
//	%itemInfo = 0;//�ϳɵ����б�
	OpenItemComposeWnd(%Player.GetPlayerID());//�ϳɽ���
}
//�ֽⱦ��
function Trigger_105090007(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	OpenItemSplitWnd(%Player.GetPlayerID());//�ֽ����
}

//��������ϵ��
//ԭʼ����
function Trigger_105099001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ������׶Ի�
			%Conv.AddOption(35, 35);	//����ָ��
			%Conv.AddOption(33, 33);	//�������
		case 35:
			%Conv.SetText(32);			//��δ���ţ������ڴ���
			%Conv.AddOption(4, 0);	//����
		case 33:
			if(%Player.GetFamily() > 0)
				{
					%Player.PutItem(105099001, -1);
					%Player.PutItem(105099002, 1);
					%Player.AddItem();
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(34);			//��ʾ���������������ĶԻ�
						%Conv.AddOption(35, 35);	//����ָ��
						%Conv.AddOption(4, 0);	//����
					}
	}
	%Conv.Send(%Player);
}
//�ᵤ����
function Trigger_105099002(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ������׶Ի�
			%Conv.AddOption(35, 35);	//����ָ��
			%Conv.AddOption(33, 33);	//�������
		case 35:
			%Conv.SetText(32);			//��δ���ţ������ڴ���
			%Conv.AddOption(4, 0);	//����
		case 33:
			if( (%Player.GetLevel() >= 13)&&(SvrIsMyselfHasTeam( %Player.GetPlayerID() )) )
				{
					%Player.PutItem(105099002, -1);
					%Player.PutItem(105099003, 1);
					%Player.AddItem();
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(34);			//��ʾ���������������ĶԻ�
						%Conv.AddOption(35, 35);	//����ָ��
						%Conv.AddOption(4, 0);	//����
					}
	}
	%Conv.Send(%Player);
}
//��������
function Trigger_105099003(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ������׶Ի�
			%Conv.AddOption(35, 35);	//����ָ��
			%Conv.AddOption(33, 33);	//�������
		case 35:
			%Conv.SetText(32);			//��δ���ţ������ڴ���
			%Conv.AddOption(4, 0);	//����
		case 33:
			if(%Player.GetLevel() >= 17)
				{
					%Player.PutItem(105099003, -1);
					%Player.PutItem(105099004, 1);
					%Player.AddItem();
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(34);			//��ʾ���������������ĶԻ�
						%Conv.AddOption(35, 35);	//����ָ��
						%Conv.AddOption(4, 0);	//����
					}
	}
	%Conv.Send(%Player);
}
//��������
function Trigger_105099004(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ������׶Ի�
			%Conv.AddOption(35, 35);	//����ָ��
			%Conv.AddOption(33, 33);	//�������
		case 35:
			%Conv.SetText(32);			//��δ���ţ������ڴ���
			%Conv.AddOption(4, 0);	//����
		case 33:
			if(%Player.GetLevel() >= 23)
				{
					%Player.PutItem(105099004, -1);
					%Player.PutItem(105099005, 1);
					%Player.AddItem();
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(34);			//��ʾ���������������ĶԻ�
						%Conv.AddOption(35, 35);	//����ָ��
						%Conv.AddOption(4, 0);	//����
					}
	}
	%Conv.Send(%Player);
}
//��������
function Trigger_105099005(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ������׶Ի�
			%Conv.AddOption(35, 35);	//����ָ��
			%Conv.AddOption(33, 33);	//�������
		case 35:
			%Conv.SetText(32);			//��δ���ţ������ڴ���
			%Conv.AddOption(4, 0);	//����
		case 33:
			if(%Player.GetLevel() >= 28)
				{
					%Player.PutItem(105099005, -1);
					%Player.PutItem(105099006, 1);
					%Player.AddItem();
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(34);			//��ʾ���������������ĶԻ�
						%Conv.AddOption(35, 35);	//����ָ��
						%Conv.AddOption(4, 0);	//����
					}
	}
	%Conv.Send(%Player);
}
//��Ե����
function Trigger_105099006(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ������׶Ի�
			%Conv.AddOption(35, 35);	//����ָ��
			%Conv.AddOption(33, 33);	//�������
		case 35:
			%Conv.SetText(32);			//��δ���ţ������ڴ���
			%Conv.AddOption(4, 0);	//����
		case 33:
			if(%Player.GetLevel() >= 35)
				{
					%Player.PutItem(105099006, -1);
					%Player.PutItem(105099007, 1);
					%Player.AddItem();
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(34);			//��ʾ���������������ĶԻ�
						%Conv.AddOption(35, 35);	//����ָ��
						%Conv.AddOption(4, 0);	//����
					}
	}
	%Conv.Send(%Player);
}
//��������
function Trigger_105099007(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ������׶Ի�
			%Conv.AddOption(35, 35);	//����ָ��
			%Conv.AddOption(33, 33);	//�������
		case 35:
			%Conv.SetText(32);			//��δ���ţ������ڴ���
			%Conv.AddOption(4, 0);	//����
		case 33:
			if( (%Player.GetLevel() >= 55)&&(%Player.GetItemCount(105102015) >= 2) )
				{
					%Player.PutItem(105099007, -1);
					%Player.PutItem(105099008, 1);
					%Player.AddItem();
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(34);			//��ʾ���������������ĶԻ�
						%Conv.AddOption(35, 35);	//����ָ��
						%Conv.AddOption(4, 0);	//����
					}
	}
	%Conv.Send(%Player);
}
//��ɽ����
function Trigger_105099008(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ������׶Ի�
			%Conv.AddOption(35, 35);	//����ָ��
			%Conv.AddOption(33, 33);	//�������
		case 35:
			%Conv.SetText(32);			//��δ���ţ������ڴ���
			%Conv.AddOption(4, 0);	//����
		case 33:
			if( (%Player.GetLevel() >= 65)&&(%Player.GetItemCount(105102016) >= 2) )
				{
					%Player.PutItem(105099008, -1);
					%Player.PutItem(105099009, 1);
					%Player.AddItem();
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(34);			//��ʾ���������������ĶԻ�
						%Conv.AddOption(35, 35);	//����ָ��
						%Conv.AddOption(4, 0);	//����
					}
	}
	%Conv.Send(%Player);
}
//�������
function Trigger_105099009(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ������׶Ի�
			%Conv.AddOption(35, 35);	//����ָ��
			%Conv.AddOption(33, 33);	//�������
		case 35:
			%Conv.SetText(32);			//��δ���ţ������ڴ���
			%Conv.AddOption(4, 0);	//����
		case 35:
			%Conv.SetText(32);			//��δ���ţ������ڴ���
			%Conv.AddOption(4, 0);	//����
		case 33:
			if( (%Player.GetLevel() >= 75)&&(%Player.GetItemCount(105102017) >= 2) )
				{
					%Player.PutItem(105099009, -1);
					%Player.PutItem(105099010, 1);
					%Player.AddItem();
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(34);			//��ʾ���������������ĶԻ�
						%Conv.AddOption(35, 35);	//����ָ��
						%Conv.AddOption(4, 0);	//����
					}
	}
	%Conv.Send(%Player);
}

//���������Һ
function Trigger_105028011(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ���������Һ�Ի�
			%Conv.AddOption(36, 36);	//ʹ��
		case 36:
			if(DelItemFromInventory(%Player.GetPlayerID(), %Type, 1))
				%Player.AddBuff(200150001, 0);
			%Conv.SetType(4);
	}
	%Conv.Send(%Player);
}
//�м������Һ
function Trigger_105028013(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ���������Һ�Ի�
			%Conv.AddOption(36, 36);	//ʹ��
		case 36:
			if(DelItemFromInventory(%Player.GetPlayerID(), %Type, 1))
				%Player.AddBuff(200150002, 0);
			%Conv.SetType(4);
	}
	%Conv.Send(%Player);
}
//�߼������Һ	
function Trigger_105028014(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ���������Һ�Ի�
			%Conv.AddOption(36, 36);	//ʹ��
		case 36:
			if(DelItemFromInventory(%Player.GetPlayerID(), %Type, 1))
				%Player.AddBuff(200150003, 0);
			%Conv.SetType(4);
	}
	%Conv.Send(%Player);
}
//��ɫ����
function Trigger_105030012(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ��ɫ�����Ի�
			%Conv.AddOption(36, 36);	//ʹ��
			%Conv.AddOption(312, 37);	//��λ�õ�¶Һ105100021
		case 36:
			if(%Player.GetItemCount(105100021) > 1)
				{
					%Player.PutItem(105100021, -1);
					%Player.PutItem(105030012, -1);
					if(%Player.AddItem())
						%Player.AddExp(50000);
						
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(313);			//��ʾû�е�¶Һ�ĶԻ�
						%Conv.AddOption(4, 0);	//����
					}
				
		case 37:
			%Conv.SetText(105100021);			//��ʾ��ɫ�����Ի�
			%Conv.AddOption(4, 0);	//����
	}
	%Conv.Send(%Player);
}
//��ɫ����
function Trigger_105030013(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ��ɫ�����Ի�
			%Conv.AddOption(36, 36);	//ʹ��
			%Conv.AddOption(314, 37);	//��λ����¶Һ105100022
		case 36:
			if(%Player.GetItemCount(105100022) > 1)
				{
					%Player.PutItem(105100022, -1);
					%Player.PutItem(105030013, -1);
					if(%Player.AddItem())
						%Player.AddExp(200000);
						
					%Conv.SetType(4);
				}
				else
					{
						%Conv.SetText(315);			//��ʾû����¶Һ�ĶԻ�
						%Conv.AddOption(4, 0);	//����
					}
				
		case 37:
			%Conv.SetText(105100022);			//��ʾ��ɫ������λ�õĶԻ�
			%Conv.AddOption(4, 0);	//����
	}
	%Conv.Send(%Player);
}
//����
function Trigger_105030014(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// ��ס��������ָ�������Ʒ��������ǳ���Ҫ
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//��ʾ�����Ի�
			%Conv.AddOption(37, 37);	//����
		case 37:
			if(%Player.GetItemCount(105030014) > 1)
				{
					%A = GetRandom(1, 100);
					if(%A < 86)
						%Item = 105030012;	//��ɫ����
					else
						%Item = 105030013;	//��ɫ����
					
					%Player.PutItem(105030014, -1);
					%Player.PutItem(%Item, 1);
					
					if(%Player.AddItem())
						%Player.AddExp($Monster_Exp[%Player.GetLevel() ,1] * 20);
				}
				
			%Conv.SetType(4);
	}
	%Conv.Send(%Player);
}
//���������������������������ĵ��ߴ����ű���������������������������������������

//����������������������ѭ��������ߴ����ű���������������������������������������
//������
function Trigger_108020263(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%ItemAdd = %Player.PutItem(108020263,-1);
	%ItemAdd = %Player.AddItem();

	%Player.AddBuff(320050002);

}

//����������������������ѭ��������ߴ����ű���������������������������������������