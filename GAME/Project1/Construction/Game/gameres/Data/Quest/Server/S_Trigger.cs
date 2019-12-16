//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//所有通过其它方式触发的脚本（非NPC起始触发）
//==================================================================================

//快速跳转入口，选中后使用F3即可，如果你不是用UE，那就帮不了你了

//■■■■■■■■■■■所有触发对话界面脚本入口■■■■■■■■■■■■■■■■
//■■■■■■■■■■■道具使用脚本■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■道具装备脚本■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■道具卸载脚本■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■道具触发任务计数■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■道具触发任务接交■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■区域触发脚本■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■整合的道具触发脚本■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■独立的道具触发脚本■■■■■■■■■■■■■■■■■■■


//■■■■■■■■■■■所有触发对话界面脚本入口■■■■■■■■■■■■■■■■
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

	//删除任务触发
	if(%State == -2)
		{
			//门宗任务
//			if(GetMissionKind(%X_ID) == 1)
			if(%X_ID ==20001)//师门任务
				{
					%Hs = %Player.GetMissionFlag(%Mid,100); //取任务环数旗标
					%Cs = %Player.GetMissionFlag(%Mid,200); //取任务次数旗标
					
					%Player.SetFlagsByte(1,%Hs);//设置任务次数旗标
					%Player.SetFlagsByte(2,%Cs); //设置任务环数旗标
					
					%Player.AddBuff(320050001, 0);
				}

			%Player.DelMission(%X_ID);
			return;
		}

	//如果X_ID是9位
	if(strlen(%X_ID) == 9)
		{
			//首位是1，则是道具使用脚本
			if(GetSubStr(%X_ID,0,1) $= "1")
				{
					echo("首位是1，则是道具使用脚本");

					//判断此道具是否属于食物·药品·补品·绷带
					if( (GetItemData(%X_ID, 3) >= 501)&&(GetItemData(%X_ID, 3) <= 504) )
						{
//							echo("判断此道具是否属于食物·药品·补品·绷带 = "@%X_ID);
//							echo("判断某物品是否限制了使用次数=" @ %Player.IsItemUsedTimes(%X_ID, %XYZ));
//							echo("判断某物品使用次数=" @ %Player.GetItemUsedTimes(%X_ID, %XYZ));
//							echo("判断某物品是否使用消失=" @ CheckBit(GetItemData(%X_ID, 22), 10));

							//判断某物品是否限制了使用次数
							if(%Player.IsItemUsedTimes(%X_ID, %XYZ) > 0)
								if(%Player.GetItemUsedTimes(%X_ID, %XYZ) > 0)
									%Player.SetItemUsedTimes(%X_ID, %XYZ, %Player.GetItemUsedTimes(%X_ID, %XYZ) - 1);

							if(CheckBit(GetItemData(%X_ID, 22), 10) == 1)
								if(%Player.GetItemUsedTimes(%X_ID, %XYZ) < 1)
									DelItemFromInventoryByIndex(%Player.GetPlayerID(), %XYZ, 1);

							return;
						}

					//判断此道具是否属于礼包类型道具
					if( (GetSubStr(%X_ID,0,4) == 1051)&&(GetSubStr(%X_ID,3,6) >= 100001)&&(GetSubStr(%X_ID,3,6) <= 100999) )
						{
							Trigger_Bao(%Conv, %Player , %X_ID, %State, %Param);	//礼包
							return;
						}

					//判断此道具是否属于配方学习道具
					if(GetItemData(%X_ID, 2) == 9)
						{
							%WhyNot = "";																		//默认的返回信息
							%PresID = GetItemData(%X_ID, 16);									//配方编号
							%livingSkillId = GetWord($Pres_Add[%PresID], 0);		//需要的生活技能编号
							%livingSkillId_Exp = GetWord($Pres_Add[%PresID], 1);	//需求的生活技能熟练度

//							echo("%PresID = "@%PresID);
//							echo("%livingSkillId = "@%livingSkillId);
//							echo("%livingSkillId_Exp = "@%livingSkillId_Exp);

							if(isLearnLivingSkill(%Player.GetPlayerID(),%livingSkillId) == 1)												//判断该生活技能是否已经学会,0没有学过,1学会
								{
									if(%Player.GetLivingSkillRipe(%livingSkillId) >= %livingSkillId_Exp)		//判断生活技能熟练度超过所要求的值
										{
											AddPrescription(%Player, %PresID);		//学习配方
											DelItemFromInventoryByIndex(%Player.GetPlayerID(), %XYZ, 1);		//删除道具
										}
										else
											%WhyNot = "你相应的生活技能熟练度不足【"@%livingSkillId_Exp@"】，无法学习此配方";
								}
								else
									%WhyNot = "你没有学会相应的生活技能，无法学习此配方";

							if(%WhyNot !$= "")
								{
									SendOneChatMessage(0, "<t>" @ %WhyNot @ "</t>", %Player);
									SendOneScreenMessage(2, %WhyNot, %Player);
								}

							return;
						}

					//判断当前道具，是否要触发任务计数
					%Mission_UseItem = $MissionData_ItemUse[%X_ID];
					if(%Mission_UseItem !$= "")
						Mission_UseItem(%Conv, %Player, %X_ID, %State, %TriggerType, %TriggerID);

					//判定此道具是否需要打开任务相关对话界面
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

			//首位是8，则是区域触发脚本
			if(GetSubStr(%X_ID,0,1) $= "8")
				eval("Trigger_" @ %X_ID @ "(%Conv, %Player, %X_ID, %State, %TriggerType, %TriggerID);");
		}
		else
			eval("Trigger_" @ %X_ID @ "(%Conv, %Player, %X_ID, %State, %TriggerType, %TriggerID);");
}
//■■■■■■■■■■■所有触发对话界面脚本入口■■■■■■■■■■■■■■■■

//■■■■■■■■■■■道具使用脚本■■■■■■■■■■■■■■■■■■■■■■
//所有可使用的道具，使用时触发的脚本
function Item_Use(%Player , %ItemID, %TriggerType, %TriggerID, %XYZ)
{
//	echo("=====================================");
//	echo("Item_Use = "@%ItemID);

	$Item_Use_Return = 1;

	//0，使用失败，不执行后续操作
	//1，使用成功

	//所有道具，统一执行OnTriggerMission函数
	OnTriggerMission(%Player, %ItemID, 0, %TriggerType, %TriggerID, 0, %XYZ);
	return $Item_Use_Return;
}
//■■■■■■■■■■■道具使用脚本■■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■道具装备脚本■■■■■■■■■■■■■■■■■■■■■■
function Item_On(%Player , %ItemID)
{
	echo("Item_On = "@%ItemID);
}
//■■■■■■■■■■■道具装备脚本■■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■道具卸载脚本■■■■■■■■■■■■■■■■■■■■■■
function Item_Off(%Player , %ItemID)
{
	echo("Item_Off = "@%ItemID);
}
//■■■■■■■■■■■道具卸载脚本■■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■道具触发任务计数■■■■■■■■■■■■■■■■■■■■
function Mission_UseItem(%Conv, %Player , %ItemID, %State, %TriggerType, %TriggerID)
{
	echo("uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu");
	%Num = $MissionData_ItemUse[%ItemID];

	for(%i = 0; %i < %Num; %i++)
	{
		%Mid = $MissionData_ItemUse[%ItemID, %i + 1];

		echo("Mission_UseItem ==Player="@%Player@"=ItemID="@%ItemID@"=State="@%State);
		echo("判断此指定任务 ＝ " @ %i @ " ＝ "@ %Mid);

		//判断是否需要跳出循环
		if(%Mid $= "")
			break;

		//判断是否接受当前任务
		if(!%Player.IsAcceptedMission(%Mid))
			{
				echo("未接受此指定任务 ＝ " @ %i @ " ＝ "@ %Mid);
				//结束当前%i值的循环，进入%i+1值的循环
				continue;
			}

		//判断是否完成当前任务
		if(%Player.IsFinishedMission(%Mid))
			{
				echo("已完成此指定任务 ＝ " @ %i @ " ＝ "@ %Mid);
				//结束当前%i值的循环，进入%i+1值的循环
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

			//判断是否需要跳出循环
			if( (%Target $= "")&&(%TarNum $= "") )
				{
					%Txt = "不是任务指定的目标或区域＝ " @ %i @ " ：目标 = 【"@%Target@"】，当前区域 = 【"@$Player_Area[%Player.GetPlayerID()]@"】";

					echo(%Txt);
//					SetScreenMessage(%Txt, $Color[2]);
//					SetChatMessage(%Txt, $Color[2]);

					break;
				}
				else
					{
						%Yes = 0;
						//判断目标是否正确
						if(strlen(%Target) == 9)
							{
								echo("$Player_Area[%Player.GetPlayerID()] = "@$Player_Area[%Player.GetPlayerID()]);
								echo("%Target = "@%Target);

								//区域
								if(GetSubStr(%Target,0,1) $= "8")
									if($Player_Area[%Player.GetPlayerID()] $= %Target)
										%Yes = 1;

								//Npc与怪物
								if(GetSubStr(%Target,0,1) $= "4")
									if(%Player.Target $= %Target)
										%Yes = 1;

							}

						echo("%Yes = "@%Yes);

						//目标正确，计数
						if(%Yes == 1)
							if(%Player.GetMissionFlag(%Mid, 1300 + %ii) < %TarNum)
								{
									echo("目标正确，计数");
									%Player.SetMissionFlag(%Mid, 1300 + %ii, %Player.GetMissionFlag(%Mid, 1300 + %ii) + 1, true); 				//设置,目标触发当前数量
									break;
								}
					}
		}
	}
}
//■■■■■■■■■■■道具触发任务计数■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■道具触发任务接交■■■■■■■■■■■■■■■■■■■■
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

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%ItemID);

	%Conv.SetType(1);

	%NpcA_Mid = $MissionData_NpcA[%ItemID];
	%NpcZ_Mid = $MissionData_NpcZ[%ItemID];

	if(%State == 0)
		{
			%Conv.SetText(%ItemID);

			//遍历当前NPC所有可接的任务，并给予相应选项
			for(%a = 0; %a < %NpcA_Mid; %a++)
			{
				%Mid = $MissionData_NpcA[%ItemID, %a + 1];
				if(CanDoThisMission(%Player, %Mid, 1, 0, 2) $= "")
					{
						%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//显示接任务选择
						%Option = 1;
					}
			}

			//遍历当前NPC所有可交的任务，并给予相应选项
			for(%z = 0; %z < %NpcZ_Mid; %z++)
			{
				%Mid = $MissionData_NpcZ[%ItemID, %z + 1];
				if(CanDoThisMission(%Player, %Mid, 2, 0, 2) $= "")
					{
						%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//显示交任务选择
						%Option = 1;
					}
			}

			//如果没有选择，则添加一个结束对话选项
			if(%Option == 0)
				%Conv.AddOption(10, -1);//显示结束对话
		}
	if(%State > 0)
		{
			%MidState = GetSubStr(%State,0,3);
			%Mid      = GetSubStr(%State,3,5);

//			echo("==========================="@%State);
			switch(%MidState)
			{
				case 110:
					//任务接受部分
					if(CanDoThisMission(%Player, %Mid, 1, 1, 1) $= "")
						{
							%Conv.SetType(4);
							AddMissionAccepted(%Player, %Mid);	//设置任务相关信息
						}
						else
							%Conv.SetText(199 @ %Mid);	//显示接任务条件不满足的对话

				case 910:
					//任务交付部分
					if(CanDoThisMission(%Player, %Mid, 2, 1, 1) $= "")
						{
							%Conv.SetType(4);
							AddMissionReward(%Player, %Mid, %Param);	//设置任务奖励

							if(CanDoThisMission(%Player, %Mid + 1, 1, 1, 1) $= "")
								AddMissionAccepted(%Player, %Mid + 1);	//设置任务相关信息
						}
						else
							%Conv.SetText(998 @ %Mid);		//显示交任务条件不满足的对话
			}
		}
	%Conv.Send(%Player);
}
//■■■■■■■■■■■道具触发任务接交■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■区域触发脚本■■■■■■■■■■■■■■■■■■■■■■
function Trigger_810010105(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)//万灵城
{
//	echo("Trigger10100");
//	echo("%Type = "@%Type);
//	echo("%State = "@%State);

	%Conv = new Conversation();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch$(%State)
	{
		case 10000:
			%Mid = %State;
       
			AddMissionAccepted(%Player, %Mid);	//设置任务基础信息
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

			%Conv.SetText(200 @ %Mid);	//显示任务对话
			%Conv.AddOption(8, 999);//结束对话
		case 999:
			%Conv.SetType(4);
	}
	%Conv.Send(%Player);
}

//■■■■■■■■■■■区域触发脚本■■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■整合的道具触发脚本■■■■■■■■■■■■■■■■■■■
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

			%NumAll = GetWord(%Item_Num_All, 0);		//礼包内道具数量
			%Type   = GetWord(%Item_Num_All, 1);		//礼包类型

			switch(%State)
			{
				case 0:
					%Conv.SetText(%ItemID, %ItemID);					//向客户端发送描述文字编号

//					%Conv.SetText("10000000" @ %Type, %ItemID);					//向客户端发送描述文字编号
//					%Conv.AddOption(9, %ItemID, 3);			//选项：【查看物品】

				default:
					//扣除礼包本身
					%Player.PutItem(%ItemID, -1);

					//必给道具的打包道具
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
					//随机给道具的打包道具
					if(%Type == 2)
						{
						}
					//选择给道具的打包道具
					if(%Type == 3)
						{
							%Item_Num_Add = $Item_Bao[%ItemID, %Param + 1];

							%Item = GetWord(%Item_Num_Add, 0);
							%Num  = GetWord(%Item_Num_Add, 1);

							if( (%Item !$= "")&&(%Num !$= "") )
								%Player.PutItem(%Item, %Num);
						}
					//依据条件给道具的打包道具
					if(%Type == 4)
						{
							%Why = GetWord(%Item_Num_All, 2);		//礼包类型


							if(%Why == 1){%ListNum = %Player.GetFamily();%ListSex = %Player.GetSex();}	//门宗条件判断
							if(%Why == 2){%ListNum = %Player.GetLevel();}		//等级条件判断


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

					//检查包裹是否已满
					if(%ItemAdd)
						%Conv.SetType(4);	//关闭对话
					else
						%Conv.SetText(6);		//向客户端发送描述文字编号
			}
		}
		else
			%Conv.SetText(7);		//物品数据错误

	%Conv.Send(%Player);
}


//■■■■■■■■■■■整合的道具触发脚本■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■独立的道具触发脚本■■■■■■■■■■■■■■■■■■■

//特殊道具：测试任务：月光宝盒
function Trigger_108020208(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示对话
			%Conv.AddOption(14, 14);	//让我改变某个任务的状态
			%Conv.AddOption(29, 13);	//让我轻舞飞扬一下（9秒）！
//			%Conv.AddOption(28, 15);	//赐予我力量吧，希瑞！
			%Conv.AddOption(30, 31);  //爆发吧！小宇宙!
			%Conv.AddOption(300000, 16);	//"点击升级(暂用)";
			%Conv.AddOption(300001, 17);	//"点击获得经验(暂用)";
			%Conv.AddOption(300002, 18);	//"点击获得金钱(暂用)";
			%Conv.AddOption(309, 309);  //生活技能升级加活力
			%Conv.AddOption(310, 310);  //传我去……

		case 14:
			%Conv.SetText(%Type);			//显示对话
//			%Conv.AddOption(19, 26);	//case 19:return "【门宗任务】";
			%Conv.AddOption(20, 20);	//case 20:return "【章回任务】";
//			%Conv.AddOption(21, 26);	//case 21:return "【支线剧情】";
//			%Conv.AddOption(22, 26);	//case 22:return "【每日循环】";
//			%Conv.AddOption(23, 26);	//case 23:return "【帮会任务】";
//			%Conv.AddOption(24, 26);	//case 24:return "【指引任务】";
//			%Conv.AddOption(25, 26);	//case 25:return "【其它任务】";

		case 15:
			%Conv.SetType(4);
			WuDi(%Player);
		case 13:
			%Conv.SetType(4);
			JiaSu(%Player);
//		case 26:
//			%Conv.SetText(26);			//目前暂无此类任务
//			%Conv.AddOption(4, 14);	//返回

		case 16:
			%Conv.AddOption(300011, 101);	//"点击升1级(暂用)";
			%Conv.AddOption(300012, 102);	//"点击升10级(暂用)";
			%Conv.AddOption(300013, 103);	//"点击升50级(暂用)";
			%Conv.AddOption(300014, 104);	//"点击升120级(暂用)";

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
			  %Conv.AddOption(293, 301);  //圣
				%Conv.AddOption(292, 302);	//佛
				%Conv.AddOption(291, 303);	//仙
				%Conv.AddOption(290, 304);	//精
				%Conv.AddOption(289, 305);	//鬼
			  %Conv.AddOption(288, 306);	//怪
			  %Conv.AddOption(288, 307);  //妖
			  %Conv.AddOption(286, 308);	//魔

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
			%Conv.AddOption(1001, 100106 + 500000);	//传送至万灵城
			%Conv.AddOption(1002, 100203 + 500000);	//传送至月幽境
			
			%Conv.AddOption(1011, 101101 + 500000);	//传送至门宗地图"昆仑宗(圣)"昆仑古虚
			%Conv.AddOption(1006, 100601 + 500000);	//传送至门宗地图"金禅寺(佛)"极乐西天
			%Conv.AddOption(1007, 100701 + 500000);	//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
			%Conv.AddOption(1005, 100501 + 500000);	//传送至门宗地图"飞花谷(精)"神木林
			%Conv.AddOption(1009, 100901 + 500000);	//传送至门宗地图"九幽教(鬼)"幽冥鬼域
			%Conv.AddOption(1010, 101001 + 500000);	//传送至门宗地图"山海宗(怪)"双生山
			%Conv.AddOption(1008, 100801 + 500000);	//传送至门宗地图"幻灵宫(妖)"醉梦冰池
			%Conv.AddOption(1004, 100401 + 500000);	//传送至门宗地图"天魔门(魔)"落夕渊
			
			%Conv.AddOption(1101, 110101 + 500000);	//传送到清风滨
			%Conv.AddOption(1102, 110202 + 500000);	//传送到仙游岭
			%Conv.AddOption(1103, 110301 + 500000);	//传送到无忧草野
			%Conv.AddOption(1104, 110401 + 500000);	//传送到遮月森林
			
			//副本地图
			if(GetSubStr(GetZoneID(), 1, 1) == 3)
				%Conv.AddOption(311, 100000 + 500000);	//进入试炼迷境
			else
				{
					%Conv.AddOption(1301, 130101 + 500000);	//进入万书楼
					%Conv.AddOption(1302, 130201 + 500000);	//进入夜晚的渔村
					%Conv.AddOption(1303, 130301 + 500000);	//传入女娲神像体内
					%Conv.AddOption(1304, 130401 + 500000);	//进入水墨古镇（七日前）
					%Conv.AddOption(1305, 130501 + 500000);	//进入试炼迷境
				}
			
		default:
			if( (%State >= 19)&&(%State <= 25) )
				{
					%Conv.SetText(%Type);			//显示对话
//					if(%State == 19)
					if(%State == 20)
						{
							%Conv.AddOption(200, 200);	//初卷
							%Conv.AddOption(201, 201);	//第1卷
							%Conv.AddOption(202, 202);	//第2卷
							%Conv.AddOption(203, 203);	//第3卷
							%Conv.AddOption(204, 204);	//第4卷
							%Conv.AddOption(205, 205);	//第5卷
//							%Conv.AddOption(206, 206);	//第6卷
						}
//					if(%State == 21)
//					if(%State == 22)
//					if(%State == 23)
//					if(%State == 24)
//					if(%State == 25)

					%Conv.AddOption(4, 14);	//返回
				}

			//【章回任务】的选项
			if( (%State >= 200)&&(%State < 300) )
				{
					%Conv.SetText(%Type);			//显示对话

					%Conv.AddOption(299, %State * 10 + 1);	//第1章
					%Conv.AddOption(298, %State * 10 + 2);	//第2章
					%Conv.AddOption(297, %State * 10 + 3);	//第3章
					%Conv.AddOption(296, %State * 10 + 4);	//第1附章
					%Conv.AddOption(295, %State * 10 + 5);	//第2附章
					%Conv.AddOption(294, %State * 10 + 6);	//第3附章

					%Conv.AddOption(4, 14);	//返回
				}

			//【章回任务】具体任务名称
			if( (%State >= 2000)&&(%State < 3000) )
				{
					%Conv.SetText(%Type);			//显示对话

					//每卷的起始编号
					%Juan = GetSubStr(%State,0,3);
					if(%Juan $= "200"){%A = 10000;}
					if(%Juan $= "201"){%A = 10100;}
					if(%Juan $= "202"){%A = 10300;}
					if(%Juan $= "203"){%A = 10500;}
					if(%Juan $= "204"){%A = 10700;}
					if(%Juan $= "205"){%A = 10900;}
					//每章的起始编号
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
							%Conv.AddOption(999 @ %Mid, %Mid);	//标识所有当前类任务
					}

					%Conv.AddOption(4, %Juan);	//返回
				}

			//具体单个任务所要执行的操作
			if( (%State >= 10000)&&(%State < 500000) )
				{
					%Conv.SetText(27);			//显示对话

					%Conv.AddOption(15, 1 @ %State);	//直接接受
					%Conv.AddOption(16, 2 @ %State);	//直接完成
					%Conv.AddOption(17, 3 @ %State);	//强制放弃
					%Conv.AddOption(18, 4 @ %State);	//设置成未完成状态

					%Conv.AddOption(4, 14);	//返回

					if( (%State >= 100000)&&(%State < 500000) )
						{
							%Mission = GetSubStr(%State,1,5);

							if(GetSubStr(%State,0,1) $= "1")	//直接接受
								AddMissionAccepted(%Player, %Mission);	//设置任务相关信息

							if(GetSubStr(%State,0,1) $= "2")	//直接完成
								AddMissionReward(%Player, %Mission, 0);	//设置任务奖励

							if(GetSubStr(%State,0,1) $= "3")	//强制放弃
								%Player.DelMission(%Mission);

							if(GetSubStr(%State,0,1) $= "4")	//设置成未完成状态
								%Player.SetMission(%Mission,0);
						}
				}
			//传送地图命令
			if( (%State >= 600000)&&(%State < 700000) )
				{
					echo("月光宝盒传送 ＝ %State = "@%State);
					%Conv.SetType(4);	//关闭对话
					
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
//变身丹
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
			%Player.SetMissionFlag(%Mid, 1300, 1, true); 				//设置,目标触发当前数量
		}
}

//被诅咒的灵狐封印
function Trigger_118010001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示对话
			%Conv.AddOption(10, -1);	//关闭
	}
	%Conv.Send(%Player);
}

//净神瓶
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
			SendOneChatMessage(0,"<t>背包满了</t>",%Player);
			SendOneScreenMessage(2,"背包满了", %Player);
			
			return;
		}
	
	%Conv.Send(%Player);
}

function Trigger_105089001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示对话
			%Conv.AddOption(8, 999);	//打开
		case 999:
			%Conv.SetType(4);	//关闭对话
			%Player.PutItem(%Type, -1);
			%Player.AddItem();
	}
	%Conv.Send(%Player);
}

//回城符
function Trigger_113010001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	echo("Trigger_113010001");
	GoToNextMap_Normal(%Player, 100109);
}

function Trigger_113020001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 101101);}//113020001	昆仑宗飞行符
function Trigger_113020002(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100601);}//113020002	雷音寺飞行符
function Trigger_113020003(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100701);}//113020003	蓬莱派飞行符
function Trigger_113020004(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100501);}//113020004	飞花谷飞行符
function Trigger_113020005(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100901);}//113020005	九幽教飞行符
function Trigger_113020006(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 101001);}//113020006	山海宗飞行符
function Trigger_113020007(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100801);}//113020007	幻灵宫飞行符
function Trigger_113020008(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100401);}//113020008	天魔门飞行符

function Trigger_113020011(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 101101);}//113020011	昆仑宗飞行符
function Trigger_113020012(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100601);}//113020012	雷音寺飞行符
function Trigger_113020013(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100701);}//113020013	蓬莱派飞行符
function Trigger_113020014(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100501);}//113020014	飞花谷飞行符
function Trigger_113020015(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100901);}//113020015	九幽教飞行符
function Trigger_113020016(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 101001);}//113020016	山海宗飞行符
function Trigger_113020017(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100801);}//113020017	幻灵宫飞行符
function Trigger_113020018(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID){GoToNextMap_Normal(%Player, 100401);}//113020018	天魔门飞行符

//骑乘道具
function Trigger_104010001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	echo("Trigger_104010001");
	echo("Player = " @ %Player);

	%Player.MountTest();
}

//兔子宝宝
function Trigger_118010002(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	echo("Trigger_118010002");
	//召唤或解散
	PetTest();
}

//仙粉
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
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);

									return;
							  }
							%Player.SetMissionFlag(%Mid, 1300, 1, true);
					  }
				}
		}
}

//画像
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

//黑狗血
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

//香喷喷的炖鸡
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


//铲子
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


//陈旧的盒子
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


//钥匙
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

//草香碧珠衣
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


//白梧桐种子
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

//八荒剑气卷（卷一）
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
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);

									return;
							  }

								%Player.SetMissionFlag(%Mid, 2103, "");		//设置,ITEM编号-取消
								%Player.SetMissionFlag(%Mid, 2203, "");		//设置,ITEM运输总数-取消
								%Player.SetMissionFlag(%Mid, 2303, "");		//设置,ITEM运输当前数量-取消

                %Player.SetMissionFlag(%Mid, 2104, "");		//设置,ITEM编号-取消
								%Player.SetMissionFlag(%Mid, 2204, "");		//设置,ITEM运输总数-取消
								%Player.SetMissionFlag(%Mid, 2304, "");		//设置,ITEM运输当前数量-取消

								%Item = 0;

							  for(%i = 3; %i < 9; %i++)
							  	if(%Player.GetMissionFlag(%Mid, 2100 + %i) > 0)
							  		%Item[%Item++] = %Player.GetMissionFlag(%Mid, 2100 + %i);

										%Player.SetMissionFlag(%Mid, 2105, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2205, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2305, "");		//设置,ITEM运输当前数量-取消

		                %Player.SetMissionFlag(%Mid, 2106, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2206, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2306, "");		//设置,ITEM运输当前数量-取消

										%Player.SetMissionFlag(%Mid, 2107, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2207, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2307, "");		//设置,ITEM运输当前数量-取消

                    %Player.SetMissionFlag(%Mid, 2108, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2208, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2308, "");		//设置,ITEM运输当前数量-取消

							  for(%ii = 0; %ii < %Item; %ii++)
							  {
							  	%ItemID = %Item[%ii + 1];

									%Player.SetMissionFlag(%Mid, 2100 + %ii + 3, %ItemID);//设置,ITEM编号-取消
									%Player.SetMissionFlag(%Mid, 2200 + %ii + 3, 1);		//设置,ITEM运输总数-取消
									%Player.SetMissionFlag(%Mid, 2300 + %ii + 3, %Player.GetItemCount(%ItemID));		//设置,ITEM运输当前数量-取消
							  }

								%Player.UpdateMission(%Mid);					//更新任务信息

					}
		}
}

//八荒剑气卷（卷二）
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
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);

									return;
							  }

								%Player.SetMissionFlag(%Mid, 2103, "");		//设置,ITEM编号-取消
								%Player.SetMissionFlag(%Mid, 2203, "");		//设置,ITEM运输总数-取消
								%Player.SetMissionFlag(%Mid, 2303, "");		//设置,ITEM运输当前数量-取消

                %Player.SetMissionFlag(%Mid, 2104, "");		//设置,ITEM编号-取消
								%Player.SetMissionFlag(%Mid, 2204, "");		//设置,ITEM运输总数-取消
								%Player.SetMissionFlag(%Mid, 2304, "");		//设置,ITEM运输当前数量-取消

								%Item = 0;

							  for(%i = 3; %i < 9; %i++)
							  	if(%Player.GetMissionFlag(%Mid, 2100 + %i) > 0)
							  		%Item[%Item++] = %Player.GetMissionFlag(%Mid, 2100 + %i);

										%Player.SetMissionFlag(%Mid, 2105, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2205, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2305, "");		//设置,ITEM运输当前数量-取消

		                %Player.SetMissionFlag(%Mid, 2106, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2206, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2306, "");		//设置,ITEM运输当前数量-取消

										%Player.SetMissionFlag(%Mid, 2107, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2207, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2307, "");		//设置,ITEM运输当前数量-取消

                    %Player.SetMissionFlag(%Mid, 2108, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2208, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2308, "");		//设置,ITEM运输当前数量-取消

							  for(%ii = 0; %ii < %Item; %ii++)
							  {
							  	%ItemID = %Item[%ii + 1];

									%Player.SetMissionFlag(%Mid, 2100 + %ii + 3, %ItemID);//设置,ITEM编号-取消
									%Player.SetMissionFlag(%Mid, 2200 + %ii + 3, 1);		//设置,ITEM运输总数-取消
									%Player.SetMissionFlag(%Mid, 2300 + %ii + 3, %Player.GetItemCount(%ItemID));		//设置,ITEM运输当前数量-取消
							  }

								%Player.UpdateMission(%Mid);					//更新任务信息

					}

		}
}

//映月辉星卷（卷一）
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
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);

									return;
							  }

							  if(%Player.GetMissionFlag(%Mid, 2103) $= "108020145")
							  	{
										%Player.SetMissionFlag(%Mid, 2103, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2203, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2303, "");		//设置,ITEM运输当前数量-取消

		                %Player.SetMissionFlag(%Mid, 2104, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2204, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2304, "");		//设置,ITEM运输当前数量-取消
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2105) $= "108020145")
							  	{
										%Player.SetMissionFlag(%Mid, 2105, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2205, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2305, "");		//设置,ITEM运输当前数量-取消

			              %Player.SetMissionFlag(%Mid, 2106, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2206, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2306, "");		//设置,ITEM运输当前数量-取消
							  	}

								%Item = 0;

							  for(%i = 3; %i < 9; %i++)
							  	if(%Player.GetMissionFlag(%Mid, 2100 + %i) > 0)
							  		%Item[%Item++] = %Player.GetMissionFlag(%Mid, 2100 + %i);

										%Player.SetMissionFlag(%Mid, 2103, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2203, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2303, "");		//设置,ITEM运输当前数量-取消

		                %Player.SetMissionFlag(%Mid, 2104, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2204, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2304, "");		//设置,ITEM运输当前数量-取消

										%Player.SetMissionFlag(%Mid, 2105, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2205, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2305, "");		//设置,ITEM运输当前数量-取消

			              %Player.SetMissionFlag(%Mid, 2106, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2206, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2306, "");		//设置,ITEM运输当前数量-取消

										%Player.SetMissionFlag(%Mid, 2107, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2207, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2307, "");		//设置,ITEM运输当前数量-取消

                    %Player.SetMissionFlag(%Mid, 2108, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2208, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2308, "");		//设置,ITEM运输当前数量-取消

							  for(%ii = 0; %ii < %Item; %ii++)
							  {
							  	%ItemID = %Item[%ii + 1];

									%Player.SetMissionFlag(%Mid, 2100 + %ii + 3, %ItemID);//设置,ITEM编号-取消
									%Player.SetMissionFlag(%Mid, 2200 + %ii + 3, 1);		//设置,ITEM运输总数-取消
									%Player.SetMissionFlag(%Mid, 2300 + %ii + 3, %Player.GetItemCount(%ItemID));		//设置,ITEM运输当前数量-取消
							  }

								%Player.UpdateMission(%Mid);					//更新任务信息

					}
		}
}

//映月辉星卷（卷二）
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
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);

									return;
							  }

							  if(%Player.GetMissionFlag(%Mid, 2104) $= "108020146")
							  	{
										%Player.SetMissionFlag(%Mid, 2103, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2203, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2303, "");		//设置,ITEM运输当前数量-取消

		                %Player.SetMissionFlag(%Mid, 2104, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2204, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2304, "");		//设置,ITEM运输当前数量-取消
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2106) $= "108020146")
							  	{
										%Player.SetMissionFlag(%Mid, 2105, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2205, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2305, "");		//设置,ITEM运输当前数量-取消

			              %Player.SetMissionFlag(%Mid, 2106, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2206, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2306, "");		//设置,ITEM运输当前数量-取消
							  	}

								%Item = 0;

							  for(%i = 3; %i < 9; %i++)
							  	if(%Player.GetMissionFlag(%Mid, 2100 + %i) > 0)
							  		%Item[%Item++] = %Player.GetMissionFlag(%Mid, 2100 + %i);

										%Player.SetMissionFlag(%Mid, 2103, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2203, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2303, "");		//设置,ITEM运输当前数量-取消

		                %Player.SetMissionFlag(%Mid, 2104, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2204, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2304, "");		//设置,ITEM运输当前数量-取消

										%Player.SetMissionFlag(%Mid, 2105, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2205, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2305, "");		//设置,ITEM运输当前数量-取消

			              %Player.SetMissionFlag(%Mid, 2106, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2206, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2306, "");		//设置,ITEM运输当前数量-取消

										%Player.SetMissionFlag(%Mid, 2107, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2207, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2307, "");		//设置,ITEM运输当前数量-取消

                    %Player.SetMissionFlag(%Mid, 2108, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2208, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2308, "");		//设置,ITEM运输当前数量-取消

							  for(%ii = 0; %ii < %Item; %ii++)
							  {
							  	%ItemID = %Item[%ii + 1];

									%Player.SetMissionFlag(%Mid, 2100 + %ii + 3, %ItemID);//设置,ITEM编号-取消
									%Player.SetMissionFlag(%Mid, 2200 + %ii + 3, 1);		//设置,ITEM运输总数-取消
									%Player.SetMissionFlag(%Mid, 2300 + %ii + 3, %Player.GetItemCount(%ItemID));		//设置,ITEM运输当前数量-取消
							  }

								%Player.UpdateMission(%Mid);					//更新任务信息

					}
		}
}

//两情依依卷（卷一）
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
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);

									return;
							  }

							  if(%Player.GetMissionFlag(%Mid, 2103) $= "108020148")
							  	{
										%Player.SetMissionFlag(%Mid, 2103, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2203, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2303, "");		//设置,ITEM运输当前数量-取消

		                %Player.SetMissionFlag(%Mid, 2104, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2204, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2304, "");		//设置,ITEM运输当前数量-取消
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2105) $= "108020148")
							  	{
										%Player.SetMissionFlag(%Mid, 2105, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2205, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2305, "");		//设置,ITEM运输当前数量-取消

			              %Player.SetMissionFlag(%Mid, 2106, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2206, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2306, "");		//设置,ITEM运输当前数量-取消
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2107) $= "108020148")
							  	{
										%Player.SetMissionFlag(%Mid, 2107, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2207, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2307, "");		//设置,ITEM运输当前数量-取消

                    %Player.SetMissionFlag(%Mid, 2108, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2208, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2308, "");		//设置,ITEM运输当前数量-取消
									}

								%Player.UpdateMission(%Mid);					//更新任务信息

					}
		}
}

//两情依依卷（卷二）
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
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);

									return;
							  }

							  if(%Player.GetMissionFlag(%Mid, 2104) $= "108020149")
							  	{
										%Player.SetMissionFlag(%Mid, 2103, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2203, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2303, "");		//设置,ITEM运输当前数量-取消

		                %Player.SetMissionFlag(%Mid, 2104, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2204, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2304, "");		//设置,ITEM运输当前数量-取消
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2106) $= "108020149")
							  	{
										%Player.SetMissionFlag(%Mid, 2105, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2205, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2305, "");		//设置,ITEM运输当前数量-取消

			              %Player.SetMissionFlag(%Mid, 2106, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2206, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2306, "");		//设置,ITEM运输当前数量-取消
							  	}

							  if(%Player.GetMissionFlag(%Mid, 2108) $= "108020149")
							  	{
										%Player.SetMissionFlag(%Mid, 2107, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2207, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2307, "");		//设置,ITEM运输当前数量-取消

                    %Player.SetMissionFlag(%Mid, 2108, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2208, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2308, "");		//设置,ITEM运输当前数量-取消
									}

								%Player.UpdateMission(%Mid);					//更新任务信息

					}
		}
}

//镶玉空杯
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
								SendOneChatMessage(0,"<t>背包满了</t>",%Player);
								SendOneScreenMessage(2,"背包满了", %Player);

								return;
							}

						%Player.SetMissionFlag(%Mid, 2300, 1, true);
						%Player.SetMissionFlag(%Mid, 1300, 1, true);
			 }
		}
}

//精族灵器
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
						SendOneChatMessage(0,"<t>背包满了</t>",%Player);
						SendOneScreenMessage(2,"背包满了", %Player);

						return;
					}
          %Player.SetMissionFlag(%Mid, 1300, 1, true);
					%Player.SetMissionFlag(%Mid, 2100, "");		//设置,ITEM编号-取消
				  %Player.SetMissionFlag(%Mid, 2200, "");		//设置,ITEM运输总数-取消
					%Player.SetMissionFlag(%Mid, 2300, "");		//设置,ITEM运输当前数量-取消
		}

	if(%Player.IsAcceptedMission(%Mid2))
		{
			 	%ItemAdd = %Player.PutItem(108020204, 1);
				%ItemAdd = %Player.PutItem(108020205, -6);
			  %ItemAdd = %Player.PutItem(108020202, -1);
			  %ItemAdd = %Player.AddItem();

			  if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>背包满了</t>",%Player);
						SendOneScreenMessage(2,"背包满了", %Player);

						return;
					}
          %Player.SetMissionFlag(%Mid, 1300, 1, true);
					%Player.SetMissionFlag(%Mid, 2100, "");		//设置,ITEM编号-取消
				  %Player.SetMissionFlag(%Mid, 2200, "");		//设置,ITEM运输总数-取消
					%Player.SetMissionFlag(%Mid, 2300, "");		//设置,ITEM运输当前数量-取消
		 }
}

//合成宝盒
function Trigger_105090006(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
//	%itemInfo = 0;//合成道具列表
	OpenItemComposeWnd(%Player.GetPlayerID());//合成界面
}
//分解宝盒
function Trigger_105090007(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	OpenItemSplitWnd(%Player.GetPlayerID());//分解界面
}

//本命灵珠系列
//原始灵珠
function Trigger_105099001(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示灵珠进阶对话
			%Conv.AddOption(35, 35);	//进阶指南
			%Conv.AddOption(33, 33);	//灵珠进阶
		case 35:
			%Conv.SetText(32);			//暂未开放，敬请期待！
			%Conv.AddOption(4, 0);	//返回
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
						%Conv.SetText(34);			//显示灵珠进阶条件不足的对话
						%Conv.AddOption(35, 35);	//进阶指南
						%Conv.AddOption(4, 0);	//返回
					}
	}
	%Conv.Send(%Player);
}
//结丹灵珠
function Trigger_105099002(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示灵珠进阶对话
			%Conv.AddOption(35, 35);	//进阶指南
			%Conv.AddOption(33, 33);	//灵珠进阶
		case 35:
			%Conv.SetText(32);			//暂未开放，敬请期待！
			%Conv.AddOption(4, 0);	//返回
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
						%Conv.SetText(34);			//显示灵珠进阶条件不足的对话
						%Conv.AddOption(35, 35);	//进阶指南
						%Conv.AddOption(4, 0);	//返回
					}
	}
	%Conv.Send(%Player);
}
//启蒙灵珠
function Trigger_105099003(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示灵珠进阶对话
			%Conv.AddOption(35, 35);	//进阶指南
			%Conv.AddOption(33, 33);	//灵珠进阶
		case 35:
			%Conv.SetText(32);			//暂未开放，敬请期待！
			%Conv.AddOption(4, 0);	//返回
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
						%Conv.SetText(34);			//显示灵珠进阶条件不足的对话
						%Conv.AddOption(35, 35);	//进阶指南
						%Conv.AddOption(4, 0);	//返回
					}
	}
	%Conv.Send(%Player);
}
//炼神灵珠
function Trigger_105099004(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示灵珠进阶对话
			%Conv.AddOption(35, 35);	//进阶指南
			%Conv.AddOption(33, 33);	//灵珠进阶
		case 35:
			%Conv.SetText(32);			//暂未开放，敬请期待！
			%Conv.AddOption(4, 0);	//返回
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
						%Conv.SetText(34);			//显示灵珠进阶条件不足的对话
						%Conv.AddOption(35, 35);	//进阶指南
						%Conv.AddOption(4, 0);	//返回
					}
	}
	%Conv.Send(%Player);
}
//聚众灵珠
function Trigger_105099005(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示灵珠进阶对话
			%Conv.AddOption(35, 35);	//进阶指南
			%Conv.AddOption(33, 33);	//灵珠进阶
		case 35:
			%Conv.SetText(32);			//暂未开放，敬请期待！
			%Conv.AddOption(4, 0);	//返回
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
						%Conv.SetText(34);			//显示灵珠进阶条件不足的对话
						%Conv.AddOption(35, 35);	//进阶指南
						%Conv.AddOption(4, 0);	//返回
					}
	}
	%Conv.Send(%Player);
}
//情缘灵珠
function Trigger_105099006(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示灵珠进阶对话
			%Conv.AddOption(35, 35);	//进阶指南
			%Conv.AddOption(33, 33);	//灵珠进阶
		case 35:
			%Conv.SetText(32);			//暂未开放，敬请期待！
			%Conv.AddOption(4, 0);	//返回
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
						%Conv.SetText(34);			//显示灵珠进阶条件不足的对话
						%Conv.AddOption(35, 35);	//进阶指南
						%Conv.AddOption(4, 0);	//返回
					}
	}
	%Conv.Send(%Player);
}
//培育灵珠
function Trigger_105099007(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示灵珠进阶对话
			%Conv.AddOption(35, 35);	//进阶指南
			%Conv.AddOption(33, 33);	//灵珠进阶
		case 35:
			%Conv.SetText(32);			//暂未开放，敬请期待！
			%Conv.AddOption(4, 0);	//返回
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
						%Conv.SetText(34);			//显示灵珠进阶条件不足的对话
						%Conv.AddOption(35, 35);	//进阶指南
						%Conv.AddOption(4, 0);	//返回
					}
	}
	%Conv.Send(%Player);
}
//开山灵珠
function Trigger_105099008(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示灵珠进阶对话
			%Conv.AddOption(35, 35);	//进阶指南
			%Conv.AddOption(33, 33);	//灵珠进阶
		case 35:
			%Conv.SetText(32);			//暂未开放，敬请期待！
			%Conv.AddOption(4, 0);	//返回
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
						%Conv.SetText(34);			//显示灵珠进阶条件不足的对话
						%Conv.AddOption(35, 35);	//进阶指南
						%Conv.AddOption(4, 0);	//返回
					}
	}
	%Conv.Send(%Player);
}
//大成灵珠
function Trigger_105099009(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示灵珠进阶对话
			%Conv.AddOption(35, 35);	//进阶指南
			%Conv.AddOption(33, 33);	//灵珠进阶
		case 35:
			%Conv.SetText(32);			//暂未开放，敬请期待！
			%Conv.AddOption(4, 0);	//返回
		case 35:
			%Conv.SetText(32);			//暂未开放，敬请期待！
			%Conv.AddOption(4, 0);	//返回
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
						%Conv.SetText(34);			//显示灵珠进阶条件不足的对话
						%Conv.AddOption(35, 35);	//进阶指南
						%Conv.AddOption(4, 0);	//返回
					}
	}
	%Conv.Send(%Player);
}

//初级清聪灵液
function Trigger_105028011(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示初级清聪灵液对话
			%Conv.AddOption(36, 36);	//使用
		case 36:
			if(DelItemFromInventory(%Player.GetPlayerID(), %Type, 1))
				%Player.AddBuff(200150001, 0);
			%Conv.SetType(4);
	}
	%Conv.Send(%Player);
}
//中级清聪灵液
function Trigger_105028013(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示初级清聪灵液对话
			%Conv.AddOption(36, 36);	//使用
		case 36:
			if(DelItemFromInventory(%Player.GetPlayerID(), %Type, 1))
				%Player.AddBuff(200150002, 0);
			%Conv.SetType(4);
	}
	%Conv.Send(%Player);
}
//高级清聪灵液	
function Trigger_105028014(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示初级清聪灵液对话
			%Conv.AddOption(36, 36);	//使用
		case 36:
			if(DelItemFromInventory(%Player.GetPlayerID(), %Type, 1))
				%Player.AddBuff(200150003, 0);
			%Conv.SetType(4);
	}
	%Conv.Send(%Player);
}
//蓝色道果
function Trigger_105030012(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示蓝色道果对话
			%Conv.AddOption(36, 36);	//使用
			%Conv.AddOption(312, 37);	//如何获得地露液105100021
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
						%Conv.SetText(313);			//显示没有地露液的对话
						%Conv.AddOption(4, 0);	//返回
					}
				
		case 37:
			%Conv.SetText(105100021);			//显示蓝色道果对话
			%Conv.AddOption(4, 0);	//返回
	}
	%Conv.Send(%Player);
}
//紫色道果
function Trigger_105030013(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示紫色道果对话
			%Conv.AddOption(36, 36);	//使用
			%Conv.AddOption(314, 37);	//如何获得天露液105100022
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
						%Conv.SetText(315);			//显示没有天露液的对话
						%Conv.AddOption(4, 0);	//返回
					}
				
		case 37:
			%Conv.SetText(105100022);			//显示紫色道果如何获得的对话
			%Conv.AddOption(4, 0);	//返回
	}
	%Conv.Send(%Player);
}
//道果
function Trigger_105030014(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%PlayerID = %Player.GetPlayerID();

	// 记住下面两条指令对于物品触发任务非常重要
	%Conv.SetTriggerType(1);
	%Conv.SetTriggerMission(%Type);

	%Conv.SetType(1);

	switch(%State)
	{
		case 0:
			%Conv.SetText(%Type);			//显示道果对话
			%Conv.AddOption(37, 37);	//开窍
		case 37:
			if(%Player.GetItemCount(105030014) > 1)
				{
					%A = GetRandom(1, 100);
					if(%A < 86)
						%Item = 105030012;	//蓝色道果
					else
						%Item = 105030013;	//橙色道果
					
					%Player.PutItem(105030014, -1);
					%Player.PutItem(%Item, 1);
					
					if(%Player.AddItem())
						%Player.AddExp($Monster_Exp[%Player.GetLevel() ,1] * 20);
				}
				
			%Conv.SetType(4);
	}
	%Conv.Send(%Player);
}
//■■■■■■■■■■■独立的道具触发脚本■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■循环任务道具触发脚本■■■■■■■■■■■■■■■■■■■
//门宗令
function Trigger_108020263(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)
{
	%ItemAdd = %Player.PutItem(108020263,-1);
	%ItemAdd = %Player.AddItem();

	%Player.AddBuff(320050002);

}

//■■■■■■■■■■■循环任务道具触发脚本■■■■■■■■■■■■■■■■■■■