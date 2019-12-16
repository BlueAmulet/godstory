//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//服务端任务脚本集，包含所有普通任务脚本
//普通任务流程统一，采用模板方式制作
//
//==================================================================================


//普通任务脚本
function Mission_Normal(%Npc, %Player, %State, %Conv, %Param)
{
	%ConvText = 0;

	%NpcA_Mid = $MissionData_NpcA[%Npc.GetDataID()];
	%NpcX_Mid = $MissionData_NpcX[%Npc.GetDataID()];
	%NpcZ_Mid = $MissionData_NpcZ[%Npc.GetDataID()];

	if(%State == 0)
		{
			//遍历当前NPC所有可交的任务，并给予相应选项
			for(%z = 0; %z < %NpcZ_Mid; %z++)
			{
				%Mid = $MissionData_NpcZ[%Npc.GetDataID(), %z + 1];
				if(CanDoThisMission(%Player, %Mid, 2, 0, 0) $= "")
					%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//显示交任务选择
			}

			//遍历当前NPC所有可接的任务，并给予相应选项
			for(%a = 0; %a < %NpcA_Mid; %a++)
			{
				%Mid = $MissionData_NpcA[%Npc.GetDataID(), %a + 1];
				if(CanDoThisMission(%Player, %Mid, 1, 0, 0) $= "")
					%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//显示接任务选择
			}
		}
	if((%State > 0)&&(strlen(%State) == 8))
		{
			%MidState = GetSubStr(%State,0,3);
			%Mid      = GetSubStr(%State,3,5);

//      echo("%MidState="@%MidState);
//			echo("==========================="@%State);

			if( (%Mid < 10124)||(%Mid > 10128) )
				switch(%MidState)
				{
					case 110:
						//任务接受部分
						if(CanDoThisMission(%Player, %Mid, 1, 1, 1) $= "")
							{
								AddMissionAccepted(%Player, %Mid);	//设置任务相关信息

								//判定是否要给于鼠标指引
								switch(%Mid)
								{
									case 10100:HelpDirectByIndex(%Player.GetPlayerID(), %Mid);	//追踪界面指引
								}
								
								
								//判定是否要再次打开对话界面
								if((%Mid == 10110)||(%Mid == 10112)||(%Mid == 10121)||(%Mid == 10122)||(%Mid == 10153)||(%Mid == 10156)||(%Mid == 10161)||(%Mid ==10320)||(%Mid ==10321)||(%Mid ==10545)||(%Mid ==10580)||(%Mid ==10584)||(%Mid ==10585)||(%Mid ==10586)||(%Mid ==10587)||(%Mid ==10588)||(%Mid ==10589)||(%Mid ==10761))
									%ConvText = 1;

								//判定是否要打开其它界面
								if( (%Mid == 22002)||(%Mid == 22004)||(%Mid == 22006)||(%Mid == 22008)||(%Mid == 22010)||(%Mid == 22012) )
									%ConvText = 2;

								if(%ConvText == 0)
									for(%a = 0; %a < %NpcA_Mid; %a++)
									{
										%MidA = $MissionData_NpcA[%Npc.GetDataID(), %a + 1];
										if(CanDoThisMission(%Player, %MidA, 1, 0, 0) $= "")
											{
												%ConvText = 1;
												break;
											}
									}

								if(%ConvText == 0)
									for(%z = 0; %z < %NpcZ_Mid; %z++)
									{
										%MidZ = $MissionData_NpcZ[%Npc.GetDataID(), %z + 1];
										if(CanDoThisMission(%Player, %MidZ, 2, 0, 0) $= "")
											{
												%ConvText = 1;
												break;
											}
									}

//							echo("(%Mid 11111-= " @ %Mid);
//							echo("(%ConvText 11111-= " @ %ConvText);

								if(%ConvText == 0)
									%Conv.SetType(4);

								if(%ConvText == 1)
									NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//执行Npc点击触发的脚本

								if(%ConvText == 2)
									{
										%Conv.SetType(4);
										//打开生活技能学习界面
										if(%Mid == 22002){OpenLivingSkillStudy(%Player, 11);}	//钓鱼
										if(%Mid == 22004){OpenLivingSkillStudy(%Player, 14);}	//狩猎
										if(%Mid == 22006){OpenLivingSkillStudy(%Player, 13);}	//采药
										if(%Mid == 22008){OpenLivingSkillStudy(%Player, 12);}	//种植
										if(%Mid == 22010){OpenLivingSkillStudy(%Player, 10);}	//伐木
										if(%Mid == 22012){OpenLivingSkillStudy(%Player, 9 );}	//采矿
									}
							}
							else
								%Conv.SetType(4);

//						else
//							%Conv.SetText(199 @ %Mid);	//显示接任务条件不满足的对话

					case 910:
						//任务交付部分
						if(CanDoThisMission(%Player, %Mid, 2, 1, 1) $= "")
							{
								AddMissionReward(%Player, %Mid, %Param);	//设置任务奖励
								
//							echo("(%ConvText 22222-= " @ %ConvText);

								//判定是否要再次打开对话界面
								if( (%Mid == 10123)||(%Mid == 10124)||(%Mid == 10125)||(%Mid == 10126)||(%Mid == 10127)||(%Mid == 10128) )
									%ConvText = 1;

								if(%ConvText == 0)
									for(%a = 0; %a < %NpcA_Mid; %a++)
									{
										%MidA = $MissionData_NpcA[%Npc.GetDataID(), %a + 1];
										if(CanDoThisMission(%Player, %MidA, 1, 0, 0) $= "")
											{
												%ConvText = 1;
												break;
											}
									}

								if(%ConvText == 0)
									for(%x = 0; %x < %NpcX_Mid; %x++)
									{
										%MidX = $MissionData_NpcX[%Npc.GetDataID(), %x + 1];
										if(CanDoThisMission(%Player, %MidX, 2, 0, 0) $= "")
											{
												%ConvText = 1;
												break;
											}
									}

								if(%ConvText == 0)
									for(%z = 0; %z < %NpcZ_Mid; %z++)
									{
										%MidZ = $MissionData_NpcZ[%Npc.GetDataID(), %z + 1];
										if(CanDoThisMission(%Player, %MidZ, 2, 0, 0) $= "")
											{
												%ConvText = 1;
												break;
											}
									}

								if(%ConvText == 0)
									%Conv.SetType(4);

								if(%ConvText == 1)
									NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//执行Npc点击触发的脚本
							}
								else
									%Conv.SetType(4);
				}
		}
}



//共享任务
function Mission_Share(%Player, %Mid, %State)
{
	echo("Mission_Share == "@%Mid);

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Mid);

	if(%State == -1)
		{
			%Conv.SetType(4);
			%Conv.Send(%Player);
			return;
		}

	%Conv.SetType(2);

	if(%State == 0)
		%Conv.SetText(110 @ %Mid, 110 @ %Mid);	//开启任务描述接受界面

	if((%State > 0)&&(strlen(%State) == 8)&&(GetSubStr(%State,0,3) == 110))
		AddMissionAccepted(%Player, %Mid);	//设置任务相关信息

	%Conv.Send(%Player);
}