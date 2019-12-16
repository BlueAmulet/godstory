//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//�ͻ��������˹��õĽű����������񽻽������ж�
//==================================================================================




//���񽻽������ж�
function CanDoThisMission(%Player, %Mid, %Type, %Other, %Msg)
{
//	echo("CanDoThisMission");

	%TiaoJian    = "";
	%MissionData = "MissionData_" @ %Mid;

	//��������������ж�
	if(%Type == 1)
		{
			%PreMid  = %MissionData.PreMid;
			
//			echo("���� �� "@%Mid);
//			echo("ǰ������ �� "@%PreMid);

			if(%Player.IsAcceptedMission(%Mid))														{%TiaoJian = %TiaoJian @ "A ";}				//�Ƿ��Ѿ����ܵ�ǰ����
			if(%Player.IsFinishedMission(%Mid))														{%TiaoJian = %TiaoJian @ "B ";}				//�Ƿ��Ѿ���ɵ�ǰ����
			if( (%PreMid !$= "0")&&(!%Player.IsFinishedMission(%PreMid)) ){%TiaoJian = %TiaoJian @ "C ";}				//�Ƿ��Ѿ����ǰ������
			if(%Player.GetLevel() < %MissionData.LvA )										{%TiaoJian = %TiaoJian @ "L ";}				//�Ƿ��Ѿ��ﵽ������ܵȼ�
			

			//Ϊ����Ĺ��µ����񵥶��ӵ���������
//			if(%Mid == 10140)
//				if(%Player.IsAcceptedMission(10122))
//					%TiaoJian = %TiaoJian @ "X ";
					
			//�ж��Ƿ�Ҫ�ٶ��������������
			if(%Other == 1)
				{
					if(%Player.GetAcceptedMission() >= 20)			{%TiaoJian = %TiaoJian @ "Q ";}				//�Ѿ����ܵ����������Ƿ�����

					if( ($Now_Script == 2)&&(%TiaoJian $= "")&&(%MissionData.ItemAdd !$= "0") )
						{
							for(%i = 0; %i < 9; %i++)
							{
								%Item = GetWord(%MissionData.ItemAdd, %i * 2);
								%Num  = GetWord(%MissionData.ItemAdd, %i * 2 + 1);

								if( (%Item $= "")&&(%Num $= "") )
									break;
								else
									{
										%Num = %Num - %Player.GetItemCount(%Item);
										
										if(%Num > 0)
											%ItemAdd = %Player.PutItem(%Item, %Num);
									}
							}

							%ItemAdd = %Player.AddItem();

							if(!%ItemAdd)
								%TiaoJian = %TiaoJian @ "I ";
						}
				}
//			echo("Type = 1 = "@%Mid @" = "@ %TiaoJian);
		}

	//��������������ж�
	if(%Type == 2)
		{
			if(!%Player.IsAcceptedMission(%Mid))	{%TiaoJian = %TiaoJian @ "A ";}		//�Ƿ��Ѿ����ܵ�ǰ����
			if(%Player.IsFinishedMission(%Mid))		{%TiaoJian = %TiaoJian @ "B ";}		//�Ƿ��Ѿ���ɵ�ǰ����

			//�����ȡ�����Ƿ��㹻
			for(%i = 0; %i < 9; %i++)
			{
				%ItemGet = %Player.GetMissionFlag(%Mid, 2100 + %i);
				%GetNum  = %Player.GetMissionFlag(%Mid, 2200 + %i);

				if( (%ItemGet $= "")&&(%GetNum $= "") )
					break;

				if(%Player.GetItemCount(%ItemGet) < %GetNum)
					{
						%TiaoJian = %TiaoJian @ "G ";
						break;
					}
			}

			//������ʱ���ĵ����Ƿ��㹻����
			for(%i = 0; %i < 9; %i++)
			{
				%ItemAdd    = %Player.GetMissionFlag(%Mid, 2150 + %i);
				%AddNum     = %Player.GetMissionFlag(%Mid, 2250 + %i);
				%ItemDelete = GetWord(%MissionData.ItemDelete, %i);

				if( (%ItemAdd $= "")&&(%AddNum $= "") )
					break;

				if(%ItemDelete > 0)
					if(%Player.GetItemCount(%ItemAdd) < %AddNum)
						{
							%TiaoJian = %TiaoJian @ "S ";
							break;
						}
			}

			//����ʹ�õ��������Ƿ��㹻
			for(%i = 0; %i < 9; %i++)
			{
				%ItemUse = %Player.GetMissionFlag(%Mid, 2400 + %i);
				%UseNum  = %Player.GetMissionFlag(%Mid, 2500 + %i);

				if( (%ItemUse $= "")&&(%UseNum $= "") )
					break;

				if(%Player.GetMissionFlag(%Mid, 2600 + %i) < %UseNum)
					{
						%TiaoJian = %TiaoJian @ "U ";
						break;
					}
			}

			//����ʹ�õ��ߴ���Ŀ�������Ƿ��㹻
			for(%i = 0; %i < 9; %i++)
			{
				%Target = %Player.GetMissionFlag(%Mid, 1100 + %i);
				%TarNum = %Player.GetMissionFlag(%Mid, 1200 + %i);

				if( (%Target $= "")&&(%TarNum $= "") )
					break;

				if(%Player.GetMissionFlag(%Mid, 1300 + %i) < %TarNum)
					{
						%TiaoJian = %TiaoJian @ "T ";
						break;
					}
			}

			//������;�ĶԻ�Ŀ�������Ƿ��㹻
			for(%i = 0; %i < 9; %i++)
			{
				%NpcX    = %Player.GetMissionFlag(%Mid, 1150 + %i);
				%NpcXNum = %Player.GetMissionFlag(%Mid, 1250 + %i);

				if( (%NpcX $= "")&&(%NpcXNum $= "") )
					break;

				if( (%NpcX > 0)&&(%NpcXNum == 0)&&(%Other == 0) )
					{
						%TiaoJian = %TiaoJian @ "N ";
						break;
					}

				if(%Player.GetMissionFlag(%Mid, 1350 + %i) < %NpcXNum)
					{
						%TiaoJian = %TiaoJian @ "N ";
						break;
					}
			}

			//��������ɱ�����Ƿ��㹻
			for(%i = 0; %i < 9; %i++)
			{
				%Monster = %Player.GetMissionFlag(%Mid, 3100 + %i);
				%KillNum = %Player.GetMissionFlag(%Mid, 3200 + %i);

				if( (%Monster $= "")&&(%KillNum $= "") )
					break;

				if(%Player.GetMissionFlag(%Mid, 3300 + %i) < %Player.GetMissionFlag(%Mid, 3200 + %i))
					{
						%TiaoJian = %TiaoJian @ "M ";
						break;
					}
			}
			
			//�жϴ���������
			for(%i = 0; %i < 9; %i++)
			{
				%Step8X_Min = %Player.GetMissionFlag(%Mid, 8100 + %i);
				%Step8X_Max = %Player.GetMissionFlag(%Mid, 8000 + %i);

				if( (%Step8X_Min $= "")&&(%Step8X_Max $= "") )
					break;

				if(%Player.GetMissionFlag(%Mid, 8100 + %i) < %Player.GetMissionFlag(%Mid, 8000 + %i))
					{
						%TiaoJian = %TiaoJian @ "P ";
						break;
					}
			}
				
			//�ж��Ƿ�Ҫ�ٶ��������������
			if(%Other == 1)
				{
					if(%Player.GetLevel() < %MissionData.LvZ )
						%TiaoJian = %TiaoJian @ "Z ";			//�Ƿ��Ѿ��ﵽ���񽻸��ȼ�
				}
//			echo("Type = 2 = "@%Mid @" = "@ %TiaoJian);
		}

	//�����޷����ܻ���ɵ�ԭ����Ϣ
	if( (%TiaoJian !$= "")&&(%Msg > 0) )
		SendWhyMsg(%Player, %TiaoJian, %Type, %Msg);

//	echo("CanDoThisMission = ��"@%TiaoJian@"��");
	return %TiaoJian;
}

//$Color[0] ��ɫ
//$Color[1] ��ɫ
//$Color[2] ��ɫ
//$Color[3] ��ɫ
//$Color[4] ��ɫ
//$Color[5] ��ɫ
//$Color[6] ��ɫ
//$Color[7] ��ɫ
//$Color[8] ��ɫ
//$Color[9] ����
//$Color[10] ����
//$Color[11] ����
//$Color[12] ����
//$Color[13] ����
//$Color[14] ����
//$Color[15] Ʒ��ɫ
//$Color[16] ����ɫ
//$Color[17] ����ɫ
//$Color[18] �Ȼ�ɫ
//$Color[19] ����ɫ
//$Color[20] ����ɫ


//�����޷����ܻ���ɵ�ԭ����Ϣ
function SendWhyMsg(%Player, %TiaoJian, %Type, %Msg)
{
//	echo("%TiaoJian =" @ %TiaoJian);
	
	%Txt1 = "";
	%Txt2 = "";
	
	//�ж��Ƿ�Ҫ������Ϣ
	if(%Msg == 1)
		for(%i = 0; %i < 99; %i++)
		{
			%Why = GetWord(%TiaoJian, %i);
	
			if(%Why $= "")
				break;
			else
				{
					switch$(%Why)
					{
						case "A":%Txt1 = "���Ѿ������˴�����";
						case "B":%Txt1 = "���Ѿ�����˴�����";
						case "C":%Txt1 = "����δ���ǰ������";
						case "L":%Txt1 = "���ĵȼ�����";
						case "P":%Txt1 = "��ѧ����Ӧ�������";
						case "Q":%Txt1 = "�����Ͻ��ܵ�������������";
						case "I":%Txt1 = "�������������Ų���������Ʒ";
						case "T":%Txt1 = "���ڽ����������������";
						case "G":%Txt1 = "���ڽ��������Ʒ����������";
						case "S":%Txt1 = "���ڽ��������Ʒ����������";
						case "U":%Txt1 = "���ڽ��������Ʒ����������";
						case "M":%Txt1 = "���ڽ�����Ĺ�������������";
						case "N":%Txt1 = "����δ�ҵ�������Ҫ�Ի���NPC";
						case "X":%Txt1 = "���ı�������ɡ�Ů洵����ӡ�����";
						case "Z":%Txt1 = "���ĵȼ�����";
					}
	
					if(%Where == 1)
						%Txt2 = "���޷���������";
					else
						%Txt2 = "���޷���������";
					
					%Txt3 = %Txt1 @ %Txt2;
					%Txt4 = "<t>" @ %Txt1 @ %Txt2 @ "</t>";
					
//					echo("%Txt3 =" @ %Txt3);
//					echo("%Txt4 =" @ %Txt4);
					
					//�жϵ�ǰ�Ƿ�Ϊ�ͻ��˵��ã���ϵͳ��Ϣ
					if($Now_Script == 1)
						{
							SetScreenMessage(%Txt3, $Color[2]);
							SetChatMessage(%Txt3, $Color[2]);
						}
						else
							{
								SendOneScreenMessage(2, %Txt3, %Player);
								SendOneChatMessage(0, %Txt4, %Player);
							}
				}
		}
}