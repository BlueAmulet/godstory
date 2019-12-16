//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端与服务端共用的脚本函数，任务交接条件判断
//==================================================================================




//任务交接条件判断
function CanDoThisMission(%Player, %Mid, %Type, %Other, %Msg)
{
//	echo("CanDoThisMission");

	%TiaoJian    = "";
	%MissionData = "MissionData_" @ %Mid;

	//接受任务的条件判断
	if(%Type == 1)
		{
			%PreMid  = %MissionData.PreMid;
			
//			echo("任务 ＝ "@%Mid);
//			echo("前置任务 ＝ "@%PreMid);

			if(%Player.IsAcceptedMission(%Mid))														{%TiaoJian = %TiaoJian @ "A ";}				//是否已经接受当前任务
			if(%Player.IsFinishedMission(%Mid))														{%TiaoJian = %TiaoJian @ "B ";}				//是否已经完成当前任务
			if( (%PreMid !$= "0")&&(!%Player.IsFinishedMission(%PreMid)) ){%TiaoJian = %TiaoJian @ "C ";}				//是否已经完成前置任务
			if(%Player.GetLevel() < %MissionData.LvA )										{%TiaoJian = %TiaoJian @ "L ";}				//是否已经达到任务接受等级
			

			//为清风村的怪事的任务单独加的特殊条件
//			if(%Mid == 10140)
//				if(%Player.IsAcceptedMission(10122))
//					%TiaoJian = %TiaoJian @ "X ";
					
			//判断是否要再额外计算任务条件
			if(%Other == 1)
				{
					if(%Player.GetAcceptedMission() >= 20)			{%TiaoJian = %TiaoJian @ "Q ";}				//已经接受的任务数量是否已满

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

	//交付任务的条件判断
	if(%Type == 2)
		{
			if(!%Player.IsAcceptedMission(%Mid))	{%TiaoJian = %TiaoJian @ "A ";}		//是否已经接受当前任务
			if(%Player.IsFinishedMission(%Mid))		{%TiaoJian = %TiaoJian @ "B ";}		//是否已经完成当前任务

			//任务获取道具是否足够
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

			//接任务时给的道具是否足够数量
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

			//任务使用道具数量是否足够
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

			//任务使用道具触发目标数量是否足够
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

			//任务中途的对话目标数量是否足够
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

			//任务怪物击杀数量是否足够
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
			
			//判断纯粹旗标计数
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
				
			//判断是否要再额外计算任务条件
			if(%Other == 1)
				{
					if(%Player.GetLevel() < %MissionData.LvZ )
						%TiaoJian = %TiaoJian @ "Z ";			//是否已经达到任务交付等级
				}
//			echo("Type = 2 = "@%Mid @" = "@ %TiaoJian);
		}

	//发送无法接受或完成的原因消息
	if( (%TiaoJian !$= "")&&(%Msg > 0) )
		SendWhyMsg(%Player, %TiaoJian, %Type, %Msg);

//	echo("CanDoThisMission = 【"@%TiaoJian@"】");
	return %TiaoJian;
}

//$Color[0] 白色
//$Color[1] 黑色
//$Color[2] 红色
//$Color[3] 绿色
//$Color[4] 蓝色
//$Color[5] 黄色
//$Color[6] 紫色
//$Color[7] 青色
//$Color[8] 灰色
//$Color[9] 暗红
//$Color[10] 暗绿
//$Color[11] 暗蓝
//$Color[12] 暗黄
//$Color[13] 暗紫
//$Color[14] 暗青
//$Color[15] 品红色
//$Color[16] 亮绿色
//$Color[17] 湖蓝色
//$Color[18] 橙黄色
//$Color[19] 褐紫色
//$Color[20] 新绿色


//发送无法接受或完成的原因消息
function SendWhyMsg(%Player, %TiaoJian, %Type, %Msg)
{
//	echo("%TiaoJian =" @ %TiaoJian);
	
	%Txt1 = "";
	%Txt2 = "";
	
	//判断是否要发送消息
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
						case "A":%Txt1 = "您已经接受了此任务";
						case "B":%Txt1 = "您已经完成了此任务";
						case "C":%Txt1 = "您尚未完成前置任务";
						case "L":%Txt1 = "您的等级过低";
						case "P":%Txt1 = "您学会相应的生活技能";
						case "Q":%Txt1 = "您身上接受的任务数量已满";
						case "I":%Txt1 = "您背包已满，放不下任务物品";
						case "T":%Txt1 = "用于交任务的条件不满足";
						case "G":%Txt1 = "用于交任务的物品条件不满足";
						case "S":%Txt1 = "用于交任务的物品条件不满足";
						case "U":%Txt1 = "用于交任务的物品条件不满足";
						case "M":%Txt1 = "用于交任务的怪物条件不满足";
						case "N":%Txt1 = "您尚未找到所有需要对话的NPC";
						case "X":%Txt1 = "您的必需先完成【女娲的灵佑】任务";
						case "Z":%Txt1 = "您的等级过低";
					}
	
					if(%Where == 1)
						%Txt2 = "，无法接受任务";
					else
						%Txt2 = "，无法交付任务";
					
					%Txt3 = %Txt1 @ %Txt2;
					%Txt4 = "<t>" @ %Txt1 @ %Txt2 @ "</t>";
					
//					echo("%Txt3 =" @ %Txt3);
//					echo("%Txt4 =" @ %Txt4);
					
					//判断当前是否为客户端调用，发系统消息
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