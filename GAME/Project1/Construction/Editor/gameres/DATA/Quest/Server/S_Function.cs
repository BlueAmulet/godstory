//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//服务端函数脚本集，包括程序与策划定义的函数
//只在服务端使用，但函数名建议不与客户端函数使用相同名称
//==================================================================================

//快速跳转入口，选中后使用F3即可，如果你不是用UE，那就帮不了你了

//■■■■■■■■■■■点击Npc触发的脚本■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■进入区域触发脚本■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■离开区域触发脚本■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■怪物死亡触发脚本■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■刷新NPC■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■刷新采集物■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■计时器结束后触发脚本■■■■■■■■■■■■■■■■
//■■■■■■■■■■■传送地图脚本■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■新手角色初始化脚本■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■采集事件调用脚本■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■角色成功登录■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■角色切换线程■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■角色切换地图■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■角色升级■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■角色死亡■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■角色复活■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■特定过滤某些共享任务的脚本方法■■■■■■■■■■■
//■■■■■■■■■■■道具鉴定脚本■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■服务器指定时间脚本■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■学习生活技能自动学会配方■■■■■■■■■■■■■■
//■■■■■■■■■■■命运有缘人成功建立后脚本■■■■■■■■■■■■■■
//■■■■■■■■■■■新建人物出生点坐标■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■服务器定时刷新排行榜■■■■■■■■■■■■■■■■
//■■■■■■■■■■■状态结束后触发■■■■■■■■■■■■■■■■■■■



//■■■■■■■■■■■点击Npc触发的脚本■■■■■■■■■■■■■■■■■
//点击Npc触发的脚本
function NpcObject::OnTrigger(%Npc, %Player, %State, %Param)
{
	//玩家出生点的首个NPC
//	if($FirstBlood == "")
//		if(%Npc.GetDataID() $= 400001101)
//			$FirstBlood = %Npc;

//	echo("ID = "@%Npc.GetDataID()@"Npc = "@%Npc);

	%Conv = new Conversation();
	%Conv.SetType(1);	//对话方式（无选项）

	if(%State == -1)
		{
//			echo("关闭对话关闭对话");
			%Conv.SetType(4);	//关闭对话
			%Conv.Send(%Player);
			return;
		}

	//执行Npc点击触发的脚本
	NpcOnTrigger(%Conv, %Npc, %Player, %State, %Param);
//	if(%State == 0)
//		{
//			%Conv.SetType(1);
//			%Conv.AddOption(1,-1);//默认有结束对话
//		}

	%Conv.Send(%Player);
}

//执行Npc点击触发的脚本
function NpcOnTrigger(%Conv, %Npc, %Player, %State, %Param)
{
	if(%State == 0)
		%Conv.SetText(%Npc.GetDataID());//显示NPC默认对话

	//%State变量等于0或只有8位时，属于任务脚本
	if( (%State == 0)||(strlen(%State) == 8) )
		{
//			Mission_Lead(%Npc, %Player, %State, %Conv, %Param);			//指引任务脚本
			Mission_Special(%Npc, %Player, %State, %Conv, %Param);	//特殊任务脚本
			Mission_Normal(%Npc, %Player, %State, %Conv, %Param);		//普通任务脚本
		}

	 //师门任务－测试
   //Mission20001(%Npc, %Player, %State , %Conv);
	//寻踪-测试
	//Mission20002(%Npc, %Player, %State , %Conv);
	//探宝-测试
	//Mission20003(%Npc, %Player, %State , %Conv);


	//%State变量少于等于6位时，进入此地
	if(strlen(%State) <= 6)
		Npc_Job(%Npc, %Player, %State , %Conv);		//NPC功能性脚本
//		{
//			//自动将地图编号与NPC编号加入至函数命名中去，并执行相应的函数
//			%HeBingFunction = "Map" @ GetZoneID() @ "Npc" @ %Npc.GetDataID() @ "(%Npc, %Player, %State , %Conv);";
//			eval(%HeBingFunction);
//		}

}

//■■■■■■■■■■■点击Npc触发的脚本■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■进入区域触发脚本■■■■■■■■■■■■■■■■■■

//进入区域触发脚本
function OnEnterTrigger(%Player,%TriggerId,%FatherTriggerID)
{
	echo("OnEnterTrigger = IN = " @ %TriggerId);

	//自动将当前玩家的区域保存至临时数据中
	$Player_Area[%Player.GetPlayerID()] = %TriggerId;

	//传送触发
	if($Area[%TriggerId] > 0)
		{
			if($Area[%TriggerId] < 900000)
				GoToNextMap_Normal(%Player, $Area[%TriggerId] );	//普通区域触发
			else
				FlyMeToTheMoon(%Player, $Area[%TriggerId] );			//飞行点触发

			return;
		}

	if(%TriggerId $= "811270100")
		{
      %Player.AddTimerControl(1, 300000,1,0,1,1);
			%Player.SetTimerControl(1, 1, "105100104");
			%Player.AddBuff(390010001, 0);
			%Player.AddBuff(390020001, 0);
		}
	if(%TriggerId $= "813030100")
		{
			%Mid = 10109;

			%CopyMapID = %Player.GetLayerId();

			if(%Player.IsAcceptedMission(%Mid))
	  			{
	  				if(%Player.GetMissionFlag(%Mid, 3300) == 4)
	  					{
	  						if(%Player.GetFlagsByte(27) ==0)
	  							{
	  								%Player.SetFlagsByte(25,1);
	  								%Player.SetFlagsByte(27,1);

				  		  		SpNewNpc3(%Player, 410700024, "-1.24882 -4.7772 133.91", %CopyMapID,0,"2 2 2");
				  		  	}
				  		}
	  			}

		}

	if(%TriggerId $= "810010105")
		{
			//万灵城

			%Mid = 10000;
			if( (!%Player.IsAcceptedMission(%Mid))&&(!%Player.IsFinishedMission(%Mid)) )
				OnTriggerMission(%Player, %TriggerId , %Mid, 0, 0, 0, 0);
		}

	if(%TriggerId $= "810010106")
		{
			//日月池区域
			%Mid = 10113;

			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)
						OnTriggerMission(%Player, %TriggerId , %Mid, 0, 0, 0, 0);
		}

	if(%TriggerId $= "810010119")
	  {
	  	//南天柱顶端
	  	%Mid =10120;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)
	  				{
	  					%Player.SetMissionFlag(%Mid, 1300, 1, true);
	  					GoToNextMap_Normal(%Player, 100119);
	  				}
	  }

	if(%TriggerId $= "811030110")
	  {
	  	//西碧珠营地
	  	%Mid =10510;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)
	  				%Player.SetMissionFlag(%Mid, 1300, 1, true);
	  }

	 if(%TriggerId $= "811030111")
	  {
	  	//东碧珠营地
	  	%Mid =10510;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1301) < 1)
	  				%Player.SetMissionFlag(%Mid, 1301, 1, true);
	  }

	  if(%TriggerId $= "811030112")
	  {
	  	//北碧珠营地

	  	%Mid =10510;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1302) < 1)
	  				%Player.SetMissionFlag(%Mid, 1302, 1, true);
	  }

	if(%TriggerId $= "811030113")
	  {
	  	//花盘附近区域触
	  	%Mid =10522;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)

	  		%Player.SetMissionFlag(%Mid, 1300, 1, true);
	  }

	if(%TriggerId $= "811030106")
	  {
	  	//南无月灯明佛首
	  	%Mid =10547;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)
	  				%Player.SetMissionFlag(%Mid, 1300, 1, true);
	  }

	if(%TriggerId $= "811030107")
	  {
	  	//南无月灯光佛首
	  	%Mid =10547;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1301) < 1)
	  				%Player.SetMissionFlag(%Mid, 1301, 1, true);
	  }

	if(%TriggerId $= "811030108")
	  {
	  	//南无智灯明佛首
	  	%Mid =10547;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1302) < 1)
	  				%Player.SetMissionFlag(%Mid, 1302, 1, true);
	  }
	if(%TriggerId $= "810040100")//落夕渊
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410807001);		//设置,目标编号
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//设置,目标触发总数
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//设置,目标触发当前数量
					}
		}
		
	if(%TriggerId $= "810050100")//神木林
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410407001);		//设置,目标编号
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//设置,目标触发总数
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//设置,目标触发当前数量
					}
		}

	if(%TriggerId $= "810060100")//极乐西天
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410207001);		//设置,目标编号
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//设置,目标触发总数
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//设置,目标触发当前数量
					}
		}

	if(%TriggerId $= "810070100")//蓬莱仙境
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410307001);		//设置,目标编号
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//设置,目标触发总数
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//设置,目标触发当前数量
					}
		}
		
	if(%TriggerId $= "810080100")//醉梦冰池
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410707001);		//设置,目标编号
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//设置,目标触发总数
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//设置,目标触发当前数量
					}
		}
		
	if(%TriggerId $= "810090100")//幽冥鬼域
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410507001);		//设置,目标编号
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//设置,目标触发总数
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//设置,目标触发当前数量
					}
		}

	if(%TriggerId $= "810100100")//双生山
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410607001);		//设置,目标编号
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//设置,目标触发总数
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//设置,目标触发当前数量
					}
		}
		
	if(%TriggerId $= "810110100")//昆仑古虚
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410107001);		//设置,目标编号
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				   //设置,目标触发总数
						%Player.SetMissionFlag(%Mid, 1350, 0); 				   //设置,目标触发当前数量
					}
		}
}



//■■■■■■■■■■■进入区域触发脚本■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■离开区域触发脚本■■■■■■■■■■■■■■■■■■
//离开区域触发脚本
function OnLeaveTrigger(%Player,%TriggerId,%FatherTriggerID)
{

}

//■■■■■■■■■■■离开区域触发脚本■■■■■■■■■■■■■■■■■■


//■■■■■■■■■■■怪物死亡触发脚本■■■■■■■■■■■■■■■■■■
//怪物死亡触发脚本
function Player::KilledMonster(%Player, %Monster, %SharePlayerNum, %MaxConnection)
{
	//获取怪物的编号
	%MonsterID = %Monster.GetDataID();
	//获取玩家的编号
	%PlayerID = %Player.GetPlayerID();
	//获取玩家等级
	%P_Lv[1] = %Player.GetLevel();
	//获取当前宠物等级
	%Pet = %Player.GetSpawnedPet();
	if(%Pet > 0)
		{
			%P_Lv[2] = %Pet.GetLevel();
			%x = 3;
		}
		else
			%x = 2;

	//获取怪物等级
	%MonLv = %Monster.GetLevel();
	//获取怪物的基本经验
	if(GetNpcData(%MonsterID, 16) $= "0")
		%Exp = $Monster_Exp[ %MonLv, 1 ];
	else
		%Exp = GetNpcData(%MonsterID, 16);

	//获取怪物的基本宠物经验
	%Exp2 = $Monster_Exp[ %MonLv, 2 ];

	//杀怪的各种奖励
	//经验计算参数
		//等级削减公式
		for(%i = 1;%i < %x;%i++)
		{
			%Lv = %P_Lv[%i] - %MonLv;

			if($Lv_Exp[ %Lv ] $= "")	//如果等级差距过大，属于削减公式内的空置
				%Y[%i] = 0;
			else
				%Y[%i] = $Lv_Exp[ %Lv ];
		}

		//判断是否为单人计算规则
		if(%Y[1] > 0)
			{
				//玩家状态加成
				%PlayerBuff = 1 + %Player.GetBuffCount(39110, 1) * 0.5 + %Player.GetBuffCount(39110, 2) * 2 + %Player.GetBuffCount(39110, 3) * 3;

				if(%SharePlayerNum == 0)
					{
						%Player_Exp = %Exp * %Y[1] * %PlayerBuff;
//						SendOneChatMessage(0, "<t>人物获得经验＝【怪物基础经验"@%Exp@"×等级削减"@%Y@"×状态加成"@%PlayerBuff@"】</t>", %Player);
					}
					else
						{
							//共享范围内的队友人数加成
							%S = $SharePlayerNum[ %SharePlayerNum ];

							//共享范围内的队友最大关系加成
							%N = $MaxConnection[ %MaxConnection ];

							%Player_Exp = (%Exp * %S / %SharePlayerNum * ( 1 + %N ) ) * %Y[1] * %PlayerBuff;
//						SendOneChatMessage(0, "<t>人物获得经验＝【怪物基础经验"@%Exp@"×队友加成"@%S@"/队友人数"@%SharePlayerNum@"×（1+队友最大关系加成"@%N@"）】×【等级削减"@%Y@"×状态加成"@%PlayerBuff@"】</t>", %Player);
						}

				//玩家经验
				%Player.AddExp(%Player_Exp);
			}

	echo("玩家["@%Player.GetPlayerName()@"]杀死了怪物["@GetNpcData(%MonsterID,1)@"]ID = "@%MonsterID@"获得经验="@%Player_Exp);

	//宠物经验
	if( (%Y[2] > 0)&&(%Pet > 0) )
		{
			//宠物状态加成
			%PetBuff = 1 + %Player.GetBuffCount(39111, 1) * 0.5 + %Player.GetBuffCount(39111, 2) * 2 + %Player.GetBuffCount(39111, 3) * 3;

			%Player.AddPetExp(%Player.GetSpawnedPetSlot(), (%Exp2 * %Y[2] * %PetBuff) );
		}
//	SendOneChatMessage(0, "<t>宠物获得经验＝【怪物基础经验"@%Exp@"×等级削减"@%Y@"×状态加成"@%PetBuff@"】</t>", %Player);

//	%Player.AddPetExp(%slot, %exp);
//	%Player.AddPetLevel(%slot, %level);
//	%Player.GetSpawnedPetSlot();


	//10级以内玩家自动升级
	if(%Player.GetLevel() < 10)
		if(%Player.GetExp() >= %Player.GetLevelExp())
			{
				%Player.AddExp(- %Player.GetLevelExp());
				%Player.AddLevel(1);
			}

	%MonsterKill_Mid = $MissionData_MonsterKill[%MonsterID];

	//自动匹配当前编号的怪物，查找相应任务并执行杀怪计数
	for(%a = 0; %a < %MonsterKill_Mid; %a++)
	{
		%Mid = $MissionData_MonsterKill[%MonsterID, %a + 1];

//		echo("%Mid = "@%Mid);

		if(%Player.IsAcceptedMission(%Mid))
			for(%i = 0; %i < 50; %i++)
			{
//				echo("Yes = 1");
				if(%Player.GetMissionFlag(%Mid, 3100 + %i) > 0)
					{
//						echo("Yes = 2");
						if(%Player.GetMissionFlag(%Mid, 3100 + %i) $= %MonsterID)
							{
//								echo("Yes = 3");
								%MaxFlag = %Player.GetMissionFlag(%Mid, 3200 + %i);
								%MinFlag = %Player.GetMissionFlag(%Mid, 3300 + %i);

//								echo("%MaxFlag = "@%MaxFlag);
//								echo("%MinFlag = "@%MinFlag);

								if( %MinFlag < %MaxFlag )
									{
//										echo("Yes = 4");
										%Player.SetMissionFlag(%Mid, 3300 + %i, %MinFlag + 1, true); //设置,怪物当前数量
									}
							}
					}
					else
						break;
			}
	}

	if((%MonsterID $="410400037")||(%MonsterID $="410400041"))
	  {
	  		if(%Player.IsAcceptedMission(10749))
	  			{
	  					 %Flag = %Player.GetFlagsByte(10);

		  			   if(%Flag == 5)

				  		{
				  				SpNewNpc3(%Player, 410400043, "44.3506 -23.5513 119.717", %CopyMapID,0,"2 2 2");
				  				%Player.SetFlagsByte(10,%Flag++);
				  		}
		  				else

		  				{
				  				%Player.SetFlagsByte(10,%Flag++);
		  				}

	  	    }
	  }

	 if((%MonsterID $="410700022")||(%MonsterID $="410700023")||(%MonsterID $="410700024"))
	   {
	  	  %Mid = 10109;

	  		if(%Player.IsAcceptedMission(%Mid))
	  			{
						for(%i = 0; %i < 50; %i++)
						   {
								if(%Player.GetMissionFlag(%Mid, 3100 + %i) > 0)
									{

										if(%Player.GetMissionFlag(%Mid, 3100 + %i) $= %MonsterID)
											{

												%MaxFlag = %Player.GetMissionFlag(%Mid, 3200 + %i);
												%MinFlag = %Player.GetMissionFlag(%Mid, 3300 + %i);

												if( %MinFlag < %MaxFlag )
													{
														%Player.SetMissionFlag(%Mid, 3300 + %i, %MinFlag + 1, true); //设置,怪物当前数量
													}
											}
									 }
										else
											break;
						   }

				   }

	    }

	   if(%MonsterID $="410700022")
	     {
	  	    %Mid = 10109;
	        %CopyMapID = %Player.GetLayerId();

	        if(%Player.IsAcceptedMission(%Mid))
	        	{
		  				if(%Player.GetMissionFlag(%Mid, 3300) == 4)
		  					{
		  						if(%Player.GetFlagsByte(25) ==0)
		  							{
		  	              %Player.SetFlagsByte(25,1);

						  				SpNewNpc3(%Player, 410700024, "-1.24882 -4.7772 133.91", %CopyMapID,0,"2 2 2");

						  			}
						  	}
						}
			}

	 if((%MonsterID $="410900001")||(%MonsterID $="410900002"))
	   {
	  	  %Mid = 10121;
	  	  %CopyMapID = %Player.GetLayerId();

	  	  if(%Player.IsAcceptedMission(%Mid))
	  			{
	  				if((%Player.GetMissionFlag(%Mid, 3300) == 1)&&(%Player.GetMissionFlag(%Mid, 3301) == 3))
	  					{
	  						if(%Player.GetFlagsByte(26) ==0)
	  							{
	  								%Player.SetFlagsByte(26,1);

				  		  		SpNewNpc3(%Player, 401301001, "-0.234988 -11.3923 140.351", %CopyMapID,0,"2 2 2");
				  		  	}
				  		}
	  			}
	  }

	if( (%MonsterID $="410100001")||(%MonsterID $="410200001")||(%MonsterID $="410300001")||(%MonsterID $="410400001")||
			(%MonsterID $="410500001")||(%MonsterID $="410600001")||(%MonsterID $="410700001")||(%MonsterID $="410800001") )
		{
			%Mid = 10125;

			if(%Player.IsAcceptedMission(%Mid))
				{
					for(%i = 0; %i < 50; %i++)
					{
						if(%Player.GetMissionFlag(%Mid, 3100 + %i) > 0)
							{

								if(%Player.GetMissionFlag(%Mid, 3100 + %i) $= %MonsterID)
									{

										%MaxFlag = %Player.GetMissionFlag(%Mid, 3200 + %i);
										%MinFlag = %Player.GetMissionFlag(%Mid, 3300 + %i);

										if( %MinFlag < %MaxFlag )
											{
												%Player.SetMissionFlag(%Mid, 3300 + %i, %MinFlag + 1, true); //设置,怪物当前数量
											}
									}
							 }
								else
									break;
					 }
				}
		}

	 if(%MonsterID $="410500064"){$New_108020016_1 =0;}
   if(%MonsterID $="410500065"){$New_108020016_2 =0;}
   if(%MonsterID $="410500066"){$New_108020016_3 =0;}

	//其它任务触发

	for(%a = 0; %a < 1; %a++)
	{
		%Mid = 20001;

//		echo("%Mid = "@%Mid);

		if(%Player.IsAcceptedMission(%Mid))
			for(%i = 0; %i < 50; %i++)
			{
//				echo("Yes = 1");
				if(%Player.GetMissionFlag(%Mid, 3100 + %i) > 0)
					{
//						echo("Yes = 2");
						if(%Player.GetMissionFlag(%Mid, 3100 + %i) $= %MonsterID)
							{
//								echo("Yes = 3");
								%MaxFlag = %Player.GetMissionFlag(%Mid, 3200 + %i);
								%MinFlag = %Player.GetMissionFlag(%Mid, 3300 + %i);

//								echo("%MaxFlag = "@%MaxFlag);
//								echo("%MinFlag = "@%MinFlag);

								if( %MinFlag < %MaxFlag )
									{
//										echo("Yes = 4");
										%Player.SetMissionFlag(%Mid, 3300 + %i, %MinFlag + 1, true); //设置,怪物当前数量
									}
							}
					}
					else
						break;
			}
	}

}

//■■■■■■■■■■■怪物死亡触发脚本■■■■■■■■■■■■■■■■■■


//■■■■■■■■■■■刷新NPC■■■■■■■■■■■■■■■■■■■■■■
//刷新Npc
function SpNewNpc(%Player, %NpcID, %Pos, %CopyMapID)
{
	if(%CopyMapID !$= "0")
		%obj = SpawnNpc(%NpcID, %CopyMapID);
	else
		%obj = SpawnNpc(%NpcID);

	if(%Pos !$= "0")
		%obj.SetPosition(%Pos);
	else
		%obj.SetPosition(%Player.GetPosition());

	return %obj;
}

//刷新Npc，有带朝向参数
function SpNewNpc2(%Player, %NpcID, %Pos, %CopyMapID, %Rot)
{
	if(%CopyMapID !$= "0")
		%obj = SpawnNpc(%NpcID, %CopyMapID);
	else
		%obj = SpawnNpc(%NpcID);

	//判断坐标参数
	if(%Pos $= "0")
		%Pos = %Player.GetPosition();

	//判断朝向参数
	if(%Rot !$= "0")
		%obj.SetPosition(%Pos, %Rot);
	else
		%obj.SetPosition(%Pos);

	return %obj;
}

//刷新Npc，有带朝向参数，带放大参数
function SpNewNpc3(%Player, %NpcID, %Pos, %CopyMapID, %Rot, %Scale)
{
	if(%CopyMapID !$= "0")
		%obj = SpawnNpc(%NpcID, %CopyMapID);
	else
		%obj = SpawnNpc(%NpcID);

	//判断坐标参数
	if(%Pos $= "0")
		%Pos = %Player.GetPosition();

	//判断朝向参数
	if(%Rot !$= "0")
		%obj.SetPosition(%Pos, %Rot);
	else
		%obj.SetPosition(%Pos);

	//判断模型放大参数
	if(%Scale !$= "0")
		%obj.SetScale(%Scale);

	return %obj;
}
//■■■■■■■■■■■刷新NPC■■■■■■■■■■■■■■■■■■■■■■

//临时保存，每个玩家的当前区域编号，日哦
$Player_Area = 0;

//■■■■■■■■■■■刷新采集物■■■■■■■■■■■■■■■■■■■■■
function SpNewCj(%Player, %ID, %Pos, %Rot)
{
//	echo("■■■■■■■■■■■刷新采集物■■■■■■■■■■■■■■■■■■■■■");

	%obj = new CollectionObject() {
         canSaveDynamicFields = "1";
         Enabled = "1";
         position = %Pos;	//坐标"-111.301 -226.354 50.3384"
         rotation = %Rot;	//朝向"1 0 0 0"
         scale = "1 1 1";
         objBox = "0 0 0 1 1 1";
         dataBlockId = %ID;	//采集物编号"552000001"
      };

	return %obj;
}
//■■■■■■■■■■■刷新采集物■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■计时器结束后触发脚本■■■■■■■■■■■■■■■■
//计时器结束后触发脚本
function OnTimerControlTriggered(%Player, %Index)
{
	switch(%Index)
	{
		case 1:
			%Player.PutItem(105100104, 1);
			%Player.AddItem();
			%Player.AddTimerControl(2,900000,1,0,1,1);
			%Player.SetTimerControl(2,1,"105100105");

		case 2:
			%Player.PutItem(105100105, 1);
			%Player.AddItem();
			%Player.AddTimerControl(3,1800000,1,0,1,1);
			%Player.SetTimerControl(3,1,"105100106");

		case 3:
			%Player.PutItem(105100106, 1);
			%Player.AddItem();
			%Player.AddTimerControl(4,3600000,1,0,1,1);
			%Player.SetTimerControl(4,1,"105100107");

		case 4:
			%Player.PutItem(105100107, 1);
			%Player.AddItem();
	}
}

//■■■■■■■■■■■计时器结束后触发脚本■■■■■■■■■■■■■■■■

//■■■■■■■■■■■传送地图脚本■■■■■■■■■■■■■■■■■■■■

//传送普通地图脚本
function GoToNextMap_Normal(%Player, %MapId)
{
	//如果在副本内，则使用返回之前进入坐标
	if(GetSubStr(GetZoneID(), 1, 1) $= "3")
		SptCopymap_RemovePlayer( %Player.GetPlayerID() );
	else
		eval($TransportObject[ %MapId ]);
}

//传送副本地图脚本
function GoToNextMap_CopyMap( %Player, %MapID )
{
	if(GetSubStr(GetZoneID(), 1, 1) $= "3")
		SptCopymap_RemovePlayer( %Player.GetPlayerID() );
	else
		{
		  if($TransportObject[ %MapId ] !$= "")
		  	SptCopyMap_TransportPlayer(%Player.GetPlayerID(), %MapID, $TransportObject[ %MapId ], 0);
		}
}

//飞行点功能
function FlyMeToTheMoon(%Player, %FlyID)
{
	%MountId = 720000101;

//	if(%FlyID == 10010101){%MountId = 720000101;} //序章进入飞行触发区域
//	if(%FlyID == 10010102){%MountId = 720000101;} //杨镖头飞行传送到清风滨
//	if(%FlyID == 10010103){%MountId = 720000101;} //夏梦狼飞行触发区域
//	if(%FlyID == 10010104){%MountId = 720000101;} //任惊梦飞行传送到清风滨
//	if(%FlyID == 11010101){%MountId = 720000101;} //夏梦狼飞行传送到万灵城
//	if(%FlyID == 11010102){%MountId = 720000101;} //杨镖头飞行触发区域
//	if(%FlyID == 11010103){%MountId = 720000101;} //任惊梦飞行触发区域
//	if(%FlyID == 11270001){%MountId = 720000101;} //巨灵神飞行传送到万灵城
//	if(%FlyID == 10010005){%MountId = 720000101;} //万灵城飞行传送到月幽境
//	if(%FlyID == 10010006){%MountId = 720000101;} //月幽境飞行触发区域
//	if(%FlyID == 10020001){%MountId = 720000101;} //万灵城飞行触发区域
//	if(%FlyID == 10020002){%MountId = 720000101;} //月幽境飞行传送到万灵城

	if( (%FlyID > 0)&&(%MountId > 0) )
		%Player.SetFlyPath(%FlyID, %MountId);
}

//■■■■■■■■■■■传送地图脚本■■■■■■■■■■■■■■■■■■■■

//临时牛逼
function WuDi(%Player)
{
	%Player.AddBuff(340010120 ,0);
	%Player.AddBuff(340020120 ,0);
	%Player.AddBuff(340030120 ,0);
	%Player.AddBuff(340040120 ,0);
	%Player.AddBuff(340050120 ,0);
	%Player.AddBuff(340060120 ,0);
	%Player.AddBuff(340070120 ,0);
	%Player.AddBuff(313050100 ,0);
	%Player.AddBuff(313090050 ,0);
//	%Player.AddBuff(309100050 ,0);
	%Player.AddBuff(309110050 ,0);
	%Player.AddBuff(309130050 ,0);
	%Player.AddBuff(313120070 ,0);

	%Player.AddBuff(313150005 ,0);
	%Player.AddBuff(307250005 ,0);
	%Player.AddBuff(313180020 ,0);
	%Player.AddBuff(309700100 ,0);

	%Player.AddBuff(307100100 ,0);
	%Player.AddBuff(313010124 ,0);
	%Player.AddBuff(309010050 ,0);

	if(%Player.GetLevel() > 0)
		Schedule(9000, 0, "WuDi", %Player);
}
//超级加速
function JiaSu(%Player)
{
	%Player.AddBuff(309100050 ,0);

	if(%Player.GetLevel() > 0)
		Schedule(9000, 0, "JiaSu", %Player);
}

//■■■■■■■■■■■新手角色初始化脚本■■■■■■■■■■■■■■■■■
//新手角色初始化脚本（每个角色只在第一次进入游戏场景时触发）
function NewbieInit(%Player)
{
	//玩家创建新角色后身上即携带道具
	%Player.PutItem(105100001, 1);	//修仙之礼
	%Player.AddItem();

	//添加物品到装备栏
	%Player.AddItemToEquip(102048010);	//粗布肩  肩部
	%Player.AddItemToEquip(102078010);	//布鞋    脚部
	%Player.AddItemToEquip(102058010);	//护腕    手部

	//添加技能
	%Player.AddSkill(200330001);	//跳跃技能
	%Player.AddSkill(200030001);	//捕捉技能

	//添加生活技能
	LearnLivingSkill(%Player,507011001);	//烹饪技能
	LearnLivingSkill(%Player,520011001);	//某个生活技能
	LearnLivingSkill(%Player,520011002);	//活动用技能-打坐

	//添加物品/技能/生活技能/宠物/骑宠对象到快捷栏
	%Player.AddObjectToPanel(3, 507011001, 7);//8：烹饪技能
	%Player.AddObjectToPanel(2, 200330001, 8);//9：跳跃技能
	%Player.AddObjectToPanel(2, 200030001, 9);//0：捕捉技能


//	%isSucceed = %player.addObjectToPanel(%type,%id,%num);
//	%type 为1-物品 2-技能 3-生活技能 4-宠物 5-骑宠
//	%id 为相应对象ID
//	%num 为快捷栏位置，0开始：0=1,1=2...

}
//■■■■■■■■■■■新手角色初始化脚本■■■■■■■■■■■■■■■■■


//■■■■■■■■■■■采集事件调用脚本■■■■■■■■■■■■■■■■■■
//采集掉物前置判断脚本
function EventTriggerCondition(%Player, %nEventTriggerId)
{
//	echo("EventTriggerCondition in =======" @ %nEventTriggerId);
	
	switch$(%nEventTriggerId)
	{
		case "552000061":		//【参悟天书】经书状采集物
			%Txt = "";
			if($FamilyName[%Player.GetFamily(), 3] $= GetZoneID())
				{
					if(%Player.GetLevel() >= 30)
						{
							if($HuoDon_CWTS == 1)
								return true;
							else
								%Txt = "【参悟天书】活动已经结束";
						}
						else
							%Txt = "您的等级不够，至少需要30级才能参悟门宗的女娲天书拓印";
				}
				else
					%Txt = "您不能参悟其它门宗的女娲天书拓印，请回自己门宗参悟";
			
			if(%Txt !$= "")
				{
					SendOneChatMessage(0, %text, %Player);
					SendOneScreenMessage(2, %text, %Player);
				}

		case 2:		//脚本触发
			return false;
		
		default:
			return true;
	}
}
//采集成功后调用脚本
function OnServerTriggerEvent(%Player, %EventID)
{
//	echo("%EventID = "@%EventID);
	switch$(%EventID)
	{
		case "552000027":Mission_Event_10532(%Player, %EventID);			//空炎1
		case "552000028":Mission_Event_10581(%Player, %EventID);			//空炎2
		case "552000038":Mission_Event_10518(%Player, %EventID);			//碧色珠果
		case "552000039":Mission_Event_10519(%Player, %EventID);			//青色珠果
		case "552000040":Mission_Event_10520(%Player, %EventID);			//草色珠果
		case "552000041":Mission_Event_10530(%Player, %EventID);			//稻草人
		case "552000042":Mission_Event_10532_1(%Player, %EventID);		//凤凰花精残骸
		case "552000043":Mission_Event_10542(%Player, %EventID);			//凤凰花
		case "552000044":Mission_Event_10544(%Player, %EventID);			//被囚禁的男人
		case "552000047":Mission_Event_10711(%Player, %EventID);			//干燥的树枝
		case "552000048":Mission_Event_10903_1(%Player, %EventID);		//大锅
		case "552000049":Mission_Event_10907(%Player, %EventID);			//族人尸体
		case "552000050":Mission_Event_10915(%Player, %EventID);			//尖刀
		case "552000051":Mission_Event_10903_2(%Player, %EventID);		//毒腺
		case "552000054":Mission_Event_10114(%Player, %EventID);			//仙鹤草
		case "552000055":Mission_Event_10116(%Player, %EventID);			//日月灵气
			
		case "552000061":		//【参悟天书】经书状采集物
			if(%Player.GetBuffCount(20015) > 0)
				%Exp = 10;
			else
				%Exp = 20;
			
			%Player.AddExp($Monster_Exp[%Player.GetLevel() ,1] / %Exp);
			%Player.AddBuff(390230001, 0);
			
			if(%Player.GetBuffCount(39023) >= 1200)
				if(%Player.CompareFlagsDate(100,0,0,1,0,0,0))
					if(AddItemToInventory(%Player.GetPlayerID(), 105030014, 1))
						%Player.SaveFlagsDate(100);
								
		default:
			if((%EventID $= "552000022")||(%EventID $= "552000023")||(%EventID $= "552000024")){Mission_Event_10787(%Player, %EventID);}//巨石
	}

	//自动采集，给于道具
	%Item_and_Jilv = $Item_CaiJi[%EventID];

//	echo("============================================");
//	echo("%Item_and_Jilv = "@%Item_and_Jilv);

	//重置变量
	%Item_Num = 0;//所有可能给于的道具总数
	%Jilv_All = 0;//所有几率总和

	//从采集掉落表中获取数据
//	echo("---------------------");
	for(%i = 0;%i < 9;%i++)
	{
		%Item[%i + 1] = GetWord(%Item_and_Jilv, %i * 2);
		%Jilv[%i + 1] = GetWord(%Item_and_Jilv, %i * 2 + 1);

		if((%Item[%i + 1] !$= "")&&(%Jilv[%i + 1] !$= ""))
			{
				%Item_Num = %Item_Num + 1;	//可能给于的道具+1
				%Jilv_All = %Jilv_All + %Jilv[%i + 1];	//几率总和增加

//				echo("%i = "@%i@" = "@%Item[%i + 1]@" = "@%Jilv[%i + 1]);
			}
			else
				break;
	}

	if( (%Item_Num > 0)&&(%Jilv_All > 0) )
		{
//			echo("%Item_Num = "@%Item_Num);
//			echo("%Jilv_All = "@%Jilv_All);

			//处理随机数
			%a = GetRandom(1,%Jilv_All);

//			echo("%a = "@%a);

			%Jilv_Num = 0;

//			echo("+++++++++++++++++++++");
			for(%ii = 1;%ii <= %Item_Num;%ii++)
			{
				%Jilv_Num = %Jilv_Num + %Jilv[%ii];

//				echo("%ii = "@%ii@" = "@%Jilv_Num);

				if(%a <= %Jilv_Num)
					{
//						echo("%Item = "@%Item[%ii]);
						%Player.PutItem(%Item[%ii],1);
						break;
					}
					else
						continue;
			}

			//发送道具
			%Player.AddItem();

			//扣除相应消耗

		}
}

function Mission_Event_10114(%Player, %EventID)	//仙鹤草
{
	%Mid = 10114;

	 if(%EventID $= "552000054")
	   {
	  	 if(%Player.IsAcceptedMission(%Mid))
	  	 	 {
		  	 	 if(%Player.GetItemCount(107019001)== 0)
		  	 	 	 {
			  	 	 	 	SendOneChatMessage(0,"<t>需要耕药镰刀</t>", %Player);
							  SendOneScreenMessage(2,"需要耕药镰刀", %Player);
		         }
					 else	if(%Player.GetMissionFlag(%Mid,1300) == 0)
					  			{
						  			 %ItemAdd = %Player.PutItem(108020267,1);
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

function Mission_Event_10116(%Player, %EventID)	//日月灵气
{
	%Mid = 10116;

	 if(%EventID $= "552000055")
	   {
	  	 if(%Player.IsAcceptedMission(%Mid))
	  	 	 if(%Player.GetItemCount(108020269)== 1)
		  		  if(%Player.GetMissionFlag(%Mid,2300) == 0)
		  			 {

		  			 	  %ItemAdd = %Player.PutItem(108020269,-1);
		  			 		%ItemAdd = %Player.PutItem(108020270,1);
								%ItemAdd = %Player.AddItem();

								if(!%ItemAdd)
									{
										SendOneChatMessage(0,"<t>背包满了</t>",%Player);
										SendOneScreenMessage(2,"背包满了", %Player);

										return;
								  }
                %Player.SetMissionFlag(%Mid, 1300, 1, true);
		  					%Player.SetMissionFlag(%Mid, 2300, 1, true);
		  			 }
	   }

}

function Mission_Event_10518(%Player, %EventID) //碧色珠果
{
	%Mid = 10518;

  if(%EventID $= "552000038")
  	{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020135)== 6))
				{
					if(%Player.GetMissionFlag(%Mid,1300) == 0)
						{
								if($New_552000038 ==0)
								  {
								  	%ItemAdd = %Player.PutItem(108020135, -6);
									  %ItemAdd = %Player.AddItem();

									  %Player.SetMissionFlag(%Mid, 2100, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2200, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2300, "");		//设置,ITEM运输当前数量-取消

										$New_552000038 = SpNewNpc(%Player,401103012,"84.0266 -25.1422 70.3982",0);
										%Player.SetMissionFlag(%Mid, 1300, 1, true);
				            Schedule(60000, 0, "RemoveNpc_552000038");
			           }
			           	%Player.UpdateMission(%Mid);					//更新任务信息
					  }
					if(%Player.GetMissionFlag(%Mid,1300) == 1)
				  	{
					  	if($New_552000038 ==0)
								{
									$New_552000038 = SpNewNpc(%Player,401103012,"84.0266 -25.1422 70.3982",0);
									%Player.SetMissionFlag(%Mid, 1300, 1, true);
					        Schedule(60000, 0, "RemoveNpc_552000038");

								}
						}
				}
		}
}

function RemoveNpc_552000038()
{
	$New_552000038.SafeDeleteObject();
	$New_552000038= 0;
}

function Mission_Event_10519(%Player, %EventID) //青色珠果
{
	%Mid = 10519;

  if(%EventID $= "552000039")
  	{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020136)== 6))
				{
					if(%Player.GetMissionFlag(%Mid,1300) == 0)
						{
							if($New_552000039 ==0)
								{
									  %ItemAdd = %Player.PutItem(108020136, -6);
								  	%ItemAdd = %Player.AddItem();

								  	%Player.SetMissionFlag(%Mid, 2100, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2200, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2300, "");		//设置,ITEM运输当前数量-取消

										$New_552000039 = SpNewNpc(%Player,401103024,"148.993 -15.8033 70.2211",0);
										%Player.SetMissionFlag(%Mid, 1300, 1, true);
				            Schedule(60000, 0, "RemoveNpc_552000039");
			          }
			          %Player.UpdateMission(%Mid);					//更新任务信息
						}
				  if(%Player.GetMissionFlag(%Mid,1300) == 1)
				  	{
					  	if($New_552000039 ==0)
								{
									$New_552000039 = SpNewNpc(%Player,401103024,"148.993 -15.8033 70.2211",0);
									%Player.SetMissionFlag(%Mid, 1300, 1, true);
					        Schedule(60000, 0, "RemoveNpc_552000039");

								}
						}
				 }
	 	}
}

function RemoveNpc_552000039()
{
	$New_552000039.SafeDeleteObject();
	$New_552000039= 0;
}

function Mission_Event_10520(%Player, %EventID) //草色珠果
{
	%Mid = 10520;

  if(%EventID $= "552000040")
  	{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020137)== 6))
				{
					if(%Player.GetMissionFlag(%Mid,1300) == 0)
						{
							if($New_552000040 ==0)
								{
									  %ItemAdd = %Player.PutItem(108020137, -6);
										%ItemAdd = %Player.AddItem();

										%Player.SetMissionFlag(%Mid, 2100, "");		//设置,ITEM编号-取消
										%Player.SetMissionFlag(%Mid, 2200, "");		//设置,ITEM运输总数-取消
										%Player.SetMissionFlag(%Mid, 2300, "");		//设置,ITEM运输当前数量-取消

										$New_552000040 = SpNewNpc(%Player,401103025,"128.2 5.44221 75.0257",0);
										%Player.SetMissionFlag(%Mid, 1300, 1, true);
				            Schedule(60000, 0, "RemoveNpc_552000040");
			          }
			          %Player.UpdateMission(%Mid);					//更新任务信息
						}
					if(%Player.GetMissionFlag(%Mid,1300) == 1)
				  	{
					  	if($New_552000039 ==0)
								{
									$New_552000039 = SpNewNpc(%Player,401103025,"128.2 5.44221 75.0257",0);
									%Player.SetMissionFlag(%Mid, 1300, 1, true);
					        Schedule(60000, 0, "RemoveNpc_552000040");

								}
						}
				}
	 	}
}

function RemoveNpc_552000040()
{
	$New_552000040.SafeDeleteObject();
	$New_552000040= 0;
}

function Mission_Event_10530(%Player, %EventID)//稻草人
{
	%Mid =10530;

	if(%EventID $= "552000041")
		{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020156)== 1))
				 if(%Player.GetMissionFlag(%Mid,1300) == 0)
				 	  %Player.SetMissionFlag(%Mid, 1300, 1, true);

		}

}

function Mission_Event_10542(%Player, %EventID)//凤凰花
{
	%Mid =10542;

	if(%EventID $= "552000043")
		{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020162)== 1))
				 if(%Player.GetMissionFlag(%Mid,1300) == 0)
				  	%Player.SetMissionFlag(%Mid, 1300, 1, true);

		}

}

function Mission_Event_10532(%Player, %EventID)// 空炎1
{
  	if(%EventID $= "552000027")
	  	{
	  		if((%Player.IsAcceptedMission(10532))||(%Player.IsAcceptedMission(10581)))
					{
						if((%player.GetItemCount(108020158)==0)&&(%player.GetItemCount(108020168)==0))
				       {
					     		if($New_552000027 ==0)
										{
											$New_552000027 = SpNewNpc(%Player,410600002,0,0);
											Schedule(320000, 0, "RemoveNpc_552000027");
										}
							 }
				   }
			}
}

function RemoveNpc_552000027()
{
	$New_552000027.SafeDeleteObject();
	$New_552000027 = 0;
}

function Mission_Event_10532_1(%Player, %EventID)//凤凰花精残骸
{
	%Mid =10532;

	if(%EventID $= "552000042")
		{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020158)== 1))
				if(%Player.GetMissionFlag(%Mid,1300) == 0)
				 	 %Player.SetMissionFlag(%Mid, 1300, 1, true);

		}

}

function Mission_Event_10544(%Player, %EventID)//被囚禁的男人
{
	%Mid =10544;

	if(%EventID $= "552000044")
		{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020165)== 1))
				{
					if(%Player.GetMissionFlag(%Mid,1300) == 0)
						{
							if($New_552000044 ==0)
								{
									$New_552000044 = SpNewNpc(%Player,401103022,0,0);
									%Player.SetMissionFlag(%Mid, 1300, 1, true);
									Schedule(60000, 0, "RemoveNpc_552000044");

							  }
						}
			  }
		}
}

function RemoveNpc_552000044()
{
	$New_552000044.SafeDeleteObject();
	$New_552000044 = 0;
}

function Mission_Event_10581(%Player, %EventID) // 空炎2
{
	%Mid =10581;

	if(%EventID $= "552000028")
		{
			if((%Player.IsAcceptedMission(10581))&&(%player.GetItemCount(108020169)==0))
	    	{
					  if(%EventID $= "552000028")
						{
						   if($New_552000028 ==0)
								 {
										$New_552000028 = SpNewNpc(%Player,410600005,0,0);
										Schedule(320000, 0, "RemoveNpc_552000028");
								 }
						}
	      }
	  }
}

function RemoveNpc_552000028()
{
	$New_552000028.SafeDeleteObject();
	$New_552000028 = 0;
}

function Mission_Event_10711(%Player, %EventID) //干燥的树枝
{
	%Mid =10711;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%EventID $= "552000047")
				{
					%Zr = GetRandom(1,2);
					%Cs = %Player.GetMissionFlag(%Mid,1300);

					if((%Zr ==1)&&(%Cs==0)){%Player.SetMissionFlag(%Mid, 1300, 1, true);}
					if((%Zr ==1)&&(%Cs==1)){%Player.SetMissionFlag(%Mid, 1300, 2, true);}
					if((%Zr ==1)&&(%Cs==2)){%Player.SetMissionFlag(%Mid, 1300, 3, true);}
					if(%Zr ==2){SpNewNpc(%Player,410600005,0,0);}

				 }
		}

}

function Mission_Event_10787(%Player, %EventID) //巨石
{
	%Mid =10787;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%EventID $= "552000022")
				{
					if(%Player.GetMissionFlag(%Mid, 1300) == 0)
						{
							if((%Player.GetMissionFlag(%Mid, 1301) == 1)&&(%Player.GetMissionFlag(%Mid, 1302) == 1))
								{
									SpNewNpc3(%Player, 410600019, "-146.504 39.5251 127.018", 0,0,"2 2 2");

									%Player.SetMissionFlag(%Mid, 1300, 1, true);
								}
					    else
					        %Player.SetMissionFlag(%Mid, 1300, 1, true);
					  }
				 }

		  if(%EventID $= "552000023")
				{
			   	if(%Player.GetMissionFlag(%Mid, 1301) == 0)
						{
							if((%Player.GetMissionFlag(%Mid, 1300) == 1)&&(%Player.GetMissionFlag(%Mid, 1302) == 1))
								{
									 SpNewNpc3(%Player, 410600019, "-146.504 39.5251 127.018", 0,0,"2 2 2");
									 %Player.SetMissionFlag(%Mid, 1301, 1, true);
								}
					    else
					       	 %Player.SetMissionFlag(%Mid, 1301, 1, true);

					 }
			  }

		 if(%EventID $= "552000024")
			  {
					if(%Player.GetMissionFlag(%Mid, 1302) == 0)
						{
								if((%Player.GetMissionFlag(%Mid, 1300) == 1)&&(%Player.GetMissionFlag(%Mid, 1301) == 1))
									{
										SpNewNpc3(%Player, 410600019, "-146.504 39.5251 127.018", 0,0,"2 2 2");
										%Player.SetMissionFlag(%Mid, 1302, 1, true);
									}
								else
										%Player.SetMissionFlag(%Mid, 1302, 1, true);
						}
				}
		}
}

function Mission_Event_10903_1(%Player, %EventID) //大锅
{
	%Mid =10903;

	if(%Player.IsAcceptedMission(%Mid))
		{
				if(%EventID $= "552000048")
					{
							%ItemAdd = %Player.PutItem(108020212, 1);
							%ItemAdd = %Player.AddItem();

							if(!%ItemAdd)
								{
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);

									return;
								}

					}
	}
}

function Mission_Event_10903_2(%Player, %EventID) //毒腺
{
	%Mid =10903;

	if(%Player.IsAcceptedMission(%Mid))
	{
			if(%EventID $= "552000051")
				{
					%SJ = GetRandom(1,5);

					if(%SJ==1)
					  {
					  	%ItemAdd = %Player.PutItem(108020211, 1);
							%ItemAdd = %Player.AddItem();
							if(!%ItemAdd)
								{
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);

									return;
								}
					  }

					if(%SJ ==2)
					  {
						  if($New_552000051 ==0)
								{
									$New_552000051 = SpNewNpc(%Player,410600030,0,0);
									Schedule(320000, 0, "RemoveNpc_552000051");
								}
					  }

					if(%SJ ==3)
					  {
						  if($New_552000051 ==0)
								{
									$New_552000051 = SpNewNpc(%Player,410600030,0,0);
									Schedule(320000, 0, "RemoveNpc_552000051");
								}
					  }

					if(%SJ ==4)
					  {
						  if($New_552000051 ==0)
								{
									$New_552000051 = SpNewNpc(%Player,410600030,0,0);
									Schedule(320000, 0, "RemoveNpc_552000051");
								}
					  }

					if(%SJ ==5)
					  {
						  %ItemAdd = %Player.PutItem(108020213, 1);
							%ItemAdd = %Player.AddItem();
							if(!%ItemAdd)
								{
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);
									return;
								}
					  }
				}
	 }

}

function RemoveNpc_552000051()
{
	$New_552000051.SafeDeleteObject();
	$New_552000051= 0;
}


function Mission_Event_10907(%Player, %EventID) //族人尸体
{
	%Mid =10907;

	if(%Player.IsAcceptedMission(%Mid))
		{
				if((%EventID $= "552000049")&&(%Player.GetItemCount(108020216)== 1))
					{
						%Zr = GetRandom(1,2);
						if(%Zr ==1)
							{
									if(($New_552000049_1==0)&&(%Player.GetMissionFlag(%Mid, 1300) != 5))
										{
											$New_552000049_1= SpNewNpc(%Player,401105003,0,0);
											Schedule(320000, 0, "RemoveNpc_552000049_1");

										}
							}

						if(%Zr ==2)
							{
								if($New_552000049_2==0)
									{
										$New_552000049_2= SpNewNpc(%Player,410500001,0,0);
										Schedule(320000, 0, "RemoveNpc_552000049_2");

									}
							}
		     }
		 }
}

function RemoveNpc_552000049_1()
{
	$New_552000049_1.SafeDeleteObject();
	$New_552000049_1= 0;
}

function RemoveNpc_552000049_2()
{
	$New_552000049_2.SafeDeleteObject();
	$New_552000049_2= 0;
}


function Mission_Event_10915(%Player, %EventID) //尖刀
{
	%Mid =10915;

	if(%Player.IsAcceptedMission(%Mid))
		{
				if((%EventID $= "552000049")&&(%Player.GetMissionFlag(%Mid, 1300) == 0))
					{
						%Player.SetMissionFlag(%Mid, 1300, 1, true);
					}
		}

}

//■■■■■■■■■■■采集成功后调用脚本■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■角色成功登录■■■■■■■■■■■■■■■■■■■■
function SvrEventLogin(%Player)
{
}
//■■■■■■■■■■■角色成功登录■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■角色切换线程■■■■■■■■■■■■■■■■■■■■
function SvrEventSwitchLine(%Player)
{
}
//■■■■■■■■■■■角色切换线程■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■角色切换地图■■■■■■■■■■■■■■■■■■■■
function SvrEventTransport(%Player)
{
}
//■■■■■■■■■■■角色切换地图■■■■■■■■■■■■■■■■■■■■


//■■■■■■■■■■■角色升级■■■■■■■■■■■■■■■■■■■■■■
function SvrEventLevelUp(%Player)
{
	echo("玩家【"@%Player.GetPlayerName()@"】升级至【"@%Player.GetLevel()@"】");

	AddEffect(1, %Player, $SP_Effect[5]);	//发送升级特效

	if(%Player.GetLevel() < 10)
		{
			if(%Player.IsAcceptedMission(10000))
				{
					%Player.SetMissionFlag(10000, 8100, %Player.GetLevel(), true);
				}
		}

  if(%Player.GetLevel() >= 10)
		{
		  if(%Player.IsAcceptedMission(10000))
				{
		     	%Player.SetMissionFlag(10000, 8100, 10, true);
		    }
		}

	if(%Player.GetLevel() == 7 ){LotRequest( %Player.GetPlayerID() );}
	if(%Player.GetLevel() == 30){AddLotRequest( %Player.GetPlayerID() );}

}
//■■■■■■■■■■■角色升级■■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■角色死亡■■■■■■■■■■■■■■■■■■■■■■
function SvrEventOnDisabled(%Player)
{
//	玩家死亡传送去
//	1401	8.2923 -87.8359 103.505
}
//■■■■■■■■■■■角色死亡■■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■角色复活■■■■■■■■■■■■■■■■■■■■■■
function SvrEventOnEnabled(%Player)
{
}
//■■■■■■■■■■■角色复活■■■■■■■■■■■■■■■■■■■■■■


//■■■■■■■■■■■特定过滤某些共享任务的脚本方法■■■■■■■■■■■
//特定过滤某些共享任务的脚本方法
function FilterShareMission(%Player)
{
	%count = %Player.GetShareMissionCount();	//获取玩家所有可以共享的任务总数
	for(%i = 0; %i < %count; %i++)
	{
		%Mid = %Player.GetShareMission(%i);	//获取指定位置的共享任务编号
		if(%Mid != -1)
			{
				//判断是否为章回任务
				if(GetMissionKind(%Mid) == 1)
					%Player.DelShareMission(%Mid);	//删除指定共享任务编号
			}
	}
//	%Player.addShareMission(%Mid);	//添加指定共享任务编号
}
//■■■■■■■■■■■特定过滤某些共享任务的脚本方法■■■■■■■■■■■


//■■■■■■■■■■■道具鉴定脚本■■■■■■■■■■■■■■■■■■■■
function ManualIdentify(%Player, %index, %ItemID, %index1, %type)
{
	if($JianDingFu[ %ItemID ] !$= "")
		{
			%Itemlv = %Player.GetItemlimitlevel(%index);	//目标道具等级
			if(%Itemlv <= GetWord($JianDingFu[ %ItemID ],0))
				%Player.SetIdentify(%index, GetWord($JianDingFu[ %ItemID ],1), GetWord($JianDingFu[ %ItemID ],2), %index1, %type);
			else
				%WhyNot = "鉴定符等级太低，无法鉴定高级的目标物品";
		}
		else
			%WhyNot = "鉴定符无法使用";

	if(%WhyNot !$= "")
		{
			SendOneChatMessage(0, "<t>" @ %WhyNot @ "</t>", %Player);
			SendOneScreenMessage(2, %WhyNot, %Player);
		}
}
//■■■■■■■■■■■道具鉴定脚本■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■服务器指定时间脚本■■■■■■■■■■■■■■■■■

//updateTop(0);
//■■■■■■■■■■■服务器指定时间脚本■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■学习生活技能自动学会配方■■■■■■■■■■■■■■
function CallbackLearnLivingSkill(%Player,%livingSkillId)
{
	echo("livingSkillId = "@%livingSkillId);

	//学会生活技能触发任务旗标
	%Mid = 0;
	if(%livingSkillId $= "501031001"){%Mid = 22002;}
	if(%livingSkillId $= "501061001"){%Mid = 22004;}
	if(%livingSkillId $= "501051001"){%Mid = 22006;}
	if(%livingSkillId $= "501041001"){%Mid = 22008;}
	if(%livingSkillId $= "501021001"){%Mid = 22010;}
	if(%livingSkillId $= "501011001"){%Mid = 22012;}

	if(%Mid > 0)
		if(%Player.IsAcceptedMission(%Mid))
			if(%Player.GetMissionFlag(%Mid, 8100) == 0)
				%Player.SetMissionFlag(%Mid, 8100, 1, true);

	if(%livingSkillId $= "502011001")
		{
			AddPrescription(%Player, 612000001);
			AddPrescription(%Player, 612000002);
			AddPrescription(%Player, 612000003);
			AddPrescription(%Player, 612000004);
			AddPrescription(%Player, 612000005);
			AddPrescription(%Player, 612000006);
			AddPrescription(%Player, 612000007);
			AddPrescription(%Player, 612000008);
			AddPrescription(%Player, 612000009);
			AddPrescription(%Player, 612000010);
			AddPrescription(%Player, 612000011);
			AddPrescription(%Player, 612000012);
			AddPrescription(%Player, 612000013);
			AddPrescription(%Player, 612000014);
			AddPrescription(%Player, 612000015);
			AddPrescription(%Player, 612000016);
			AddPrescription(%Player, 612000017);
			AddPrescription(%Player, 612000018);
			AddPrescription(%Player, 612000019);
			AddPrescription(%Player, 612000020);
			AddPrescription(%Player, 612000021);
			AddPrescription(%Player, 612000022);
			AddPrescription(%Player, 612000023);
			AddPrescription(%Player, 612000024);
			AddPrescription(%Player, 612000025);
			AddPrescription(%Player, 612000026);
			AddPrescription(%Player, 612000027);
			AddPrescription(%Player, 612000028);
			AddPrescription(%Player, 612000029);
			AddPrescription(%Player, 612000030);
			AddPrescription(%Player, 612000031);
			AddPrescription(%Player, 612000032);
			AddPrescription(%Player, 612000033);
			AddPrescription(%Player, 612000034);
			AddPrescription(%Player, 612000035);
			AddPrescription(%Player, 612000036);
			AddPrescription(%Player, 612000037);
			AddPrescription(%Player, 612000038);
			AddPrescription(%Player, 612000039);
			AddPrescription(%Player, 612000040);
			AddPrescription(%Player, 630102001);
			AddPrescription(%Player, 630202001);
			AddPrescription(%Player, 630302001);
			AddPrescription(%Player, 630402001);
			AddPrescription(%Player, 630502001);
			AddPrescription(%Player, 630602001);
			AddPrescription(%Player, 630702001);
		}
	if(%livingSkillId $= "503011001")
		{
			AddPrescription(%Player, 612000121);
			AddPrescription(%Player, 612000122);
			AddPrescription(%Player, 612000123);
			AddPrescription(%Player, 612000124);
			AddPrescription(%Player, 612000125);
			AddPrescription(%Player, 612000126);
			AddPrescription(%Player, 612000127);
			AddPrescription(%Player, 612000128);
			AddPrescription(%Player, 612000129);
			AddPrescription(%Player, 612000130);
			AddPrescription(%Player, 612000131);
			AddPrescription(%Player, 612000132);
			AddPrescription(%Player, 612000133);
			AddPrescription(%Player, 612000134);
			AddPrescription(%Player, 612000135);
			AddPrescription(%Player, 612000136);
			AddPrescription(%Player, 612000137);
			AddPrescription(%Player, 612000138);
			AddPrescription(%Player, 612000139);
			AddPrescription(%Player, 612000140);
			AddPrescription(%Player, 612000141);
			AddPrescription(%Player, 612000142);
			AddPrescription(%Player, 612000143);
			AddPrescription(%Player, 612000144);
			AddPrescription(%Player, 612000145);
			AddPrescription(%Player, 612000146);
			AddPrescription(%Player, 612000147);
			AddPrescription(%Player, 612000148);
			AddPrescription(%Player, 612000149);
			AddPrescription(%Player, 612000150);
			AddPrescription(%Player, 612000151);
			AddPrescription(%Player, 612000152);
			AddPrescription(%Player, 612000153);
			AddPrescription(%Player, 612000154);
			AddPrescription(%Player, 612000155);
			AddPrescription(%Player, 612000156);
			AddPrescription(%Player, 612000157);
			AddPrescription(%Player, 612000158);
			AddPrescription(%Player, 612000159);
			AddPrescription(%Player, 612000160);
			AddPrescription(%Player, 630901201);
		}
	if(%livingSkillId $= "504011001")
		{
			AddPrescription(%Player, 612000041);
			AddPrescription(%Player, 612000042);
			AddPrescription(%Player, 612000043);
			AddPrescription(%Player, 612000044);
			AddPrescription(%Player, 612000045);
			AddPrescription(%Player, 612000046);
			AddPrescription(%Player, 612000047);
			AddPrescription(%Player, 612000048);
			AddPrescription(%Player, 612000049);
			AddPrescription(%Player, 612000050);
			AddPrescription(%Player, 612000051);
			AddPrescription(%Player, 612000052);
			AddPrescription(%Player, 612000053);
			AddPrescription(%Player, 612000054);
			AddPrescription(%Player, 612000055);
			AddPrescription(%Player, 612000056);
			AddPrescription(%Player, 612000057);
			AddPrescription(%Player, 612000058);
			AddPrescription(%Player, 612000059);
			AddPrescription(%Player, 612000060);
			AddPrescription(%Player, 631502001);
			AddPrescription(%Player, 631602001);
			AddPrescription(%Player, 631702001);
		}
	if(%livingSkillId $= "505011001")
		{
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
		}
	if(%livingSkillId $= "506011001")
		{
			AddPrescription(%Player, 611920101);
			AddPrescription(%Player, 611920121);
			AddPrescription(%Player, 612000101);
			AddPrescription(%Player, 612000102);
			AddPrescription(%Player, 612000103);
			AddPrescription(%Player, 612000104);
			AddPrescription(%Player, 612000105);
			AddPrescription(%Player, 612000106);
			AddPrescription(%Player, 612000107);
			AddPrescription(%Player, 612000108);
			AddPrescription(%Player, 612000109);
			AddPrescription(%Player, 612000110);
			AddPrescription(%Player, 612000111);
			AddPrescription(%Player, 612000112);
			AddPrescription(%Player, 612000113);
			AddPrescription(%Player, 612000114);
			AddPrescription(%Player, 612000115);
			AddPrescription(%Player, 612000116);
			AddPrescription(%Player, 612000117);
			AddPrescription(%Player, 612000118);
			AddPrescription(%Player, 612000119);
			AddPrescription(%Player, 612000120);
		}
	if(%livingSkillId $= "507011001")
		{
			AddPrescription(%Player, 611810101);
			AddPrescription(%Player, 611810121);
			AddPrescription(%Player, 612000081);
			AddPrescription(%Player, 612000082);
			AddPrescription(%Player, 612000083);
			AddPrescription(%Player, 612000084);
			AddPrescription(%Player, 612000085);
			AddPrescription(%Player, 612000086);
			AddPrescription(%Player, 612000087);
			AddPrescription(%Player, 612000088);
			AddPrescription(%Player, 612000089);
			AddPrescription(%Player, 612000090);
			AddPrescription(%Player, 612000091);
			AddPrescription(%Player, 612000092);
			AddPrescription(%Player, 612000093);
			AddPrescription(%Player, 612000094);
			AddPrescription(%Player, 612000095);
			AddPrescription(%Player, 612000096);
			AddPrescription(%Player, 612000097);
			AddPrescription(%Player, 612000098);
			AddPrescription(%Player, 612000099);
			AddPrescription(%Player, 612000100);
		}
}
//■■■■■■■■■■■学习生活技能自动学会配方■■■■■■■■■■■■■■

//■■■■■■■■■■■命运有缘人成功建立后脚本■■■■■■■■■■■■■■
//LotRequest( %playerId );  // 低等级玩家触发
//AddLotRequest( %playerId );  // 高等级玩家进入触发// 玩家到达30级，每隔特定时间去调用这个函数，时间暂定30分钟
//RemoveLotRequest( %playerId );  // 高等级玩家触发移除
//HandleLotRequest( %playerId, %destPlayerId ); // 触发成功

//低级玩家触发
function HandleLotRequest( %PlayerID_Lv1, %PlayerID_Lv99 )
{

}
//高级玩家触发
function HandleLotAck( %PlayerID_Lv99, %PlayerID_Lv1 )
{

}

//■■■■■■■■■■■命运有缘人成功建立后脚本■■■■■■■■■■■■■■

//■■■■■■■■■■■新建人物出生点坐标■■■■■■■■■■■■■■■■■
function SvrGetNewbiePosition()
{
	%X = GetRandom(-2, -14);
	%Y = GetRandom(3, 15);

	return %X @ " " @ %Y @ " 130";
}
//■■■■■■■■■■■新建人物出生点坐标■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■服务器定时刷新排行榜■■■■■■■■■■■■■■■■
function CheckServerTime()
{
	%Time = GetLocalTime();

	%YY = GetWord(%Time, 0);	//年
	%MM = GetWord(%Time, 1);	//月
	%DD = GetWord(%Time, 2);	//日
	%H = GetWord(%Time, 3);	//小时
	%M = GetWord(%Time, 4);	//分钟
	%S = GetWord(%Time, 5);	//秒

	//更新排行榜
	if( (GetZoneID() $= "1001")&&( (%H == 1)&&(%M == 1) ) )
		UpDateTop(0);

	//每日20:00~09:00，八大门宗场景，===========================【参悟天书】====================
//	if(GetZoneID() $= "1001")
//		{
//			if( (%H == 19)&&(%M == 55) )		//活动预告
//				{
//					%Txt = "【参悟天书】（每日20:00~9:00）活动即将在5分钟后开始，大家可速至本门宗的宗主旁，参悟女娲天书拓印，以图大道！";
//					SendAllChatMessage(1, %Txt);//发送跑马信息
//					SendAllChatMessage(3, %Txt);//发送全服信息
//				}
//			if( (%H == 20)&&(%M == 1) )			//活动开始
//				{
//					%Txt = "【参悟天书】（每日20:00~9:00）活动已经开始，大家速至本门宗的宗主旁，参悟女娲天书拓印，以图大道！";
//					SendAllChatMessage(1, %Txt);//发送跑马信息
//					SendAllChatMessage(3, %Txt);//发送全服信息
//				}
//		}
//	if( (GetZoneID() $= "1004")||(GetZoneID() $= "1005")||(GetZoneID() $= "1006")||(GetZoneID() $= "1007")||(GetZoneID() $= "1008")||(GetZoneID() $= "1009")||(GetZoneID() $= "1010")||(GetZoneID() $= "1011") )
//		{
//			if( ( (%H >= 20)||(%H < 9) )&&($HuoDong_CWTS == 0) )		//活动开始
//				{
//					$HuoDong_CWTS = 1;	//活动开始标识
//					%Map = GetZoneID();
//					$HuoDong_CWTS[1] = SpNewCj(0, 552000061, $HuoDon_CWTS[%Map, 1]     @ " " @ $HuoDon_CWTS[%Map, 2]     @ " " @ $HuoDon_CWTS[%Map, 3], "1 0 0 0");
//					$HuoDong_CWTS[2] = SpNewCj(0, 552000061, $HuoDon_CWTS[%Map, 1] + 5 @ " " @ $HuoDon_CWTS[%Map, 2] + 5 @ " " @ $HuoDon_CWTS[%Map, 3], "1 0 0 0");
//					$HuoDong_CWTS[3] = SpNewCj(0, 552000061, $HuoDon_CWTS[%Map, 1] + 5 @ " " @ $HuoDon_CWTS[%Map, 2] - 5 @ " " @ $HuoDon_CWTS[%Map, 3], "1 0 0 0");
//					$HuoDong_CWTS[4] = SpNewCj(0, 552000061, $HuoDon_CWTS[%Map, 1] - 5 @ " " @ $HuoDon_CWTS[%Map, 2] + 5 @ " " @ $HuoDon_CWTS[%Map, 3], "1 0 0 0");
//					$HuoDong_CWTS[5] = SpNewCj(0, 552000061, $HuoDon_CWTS[%Map, 1] - 5 @ " " @ $HuoDon_CWTS[%Map, 2] - 5 @ " " @ $HuoDon_CWTS[%Map, 3], "1 0 0 0");
//				}
//			if( (%H == 9)&&($HuoDong_CWTS == 1) )		//活动结束
//				{
//					$HuoDong_CWTS = 0;	//活动结束标识
//					for(%i = 1;%i < 6;%i++)
//					{
//						%DK = $HuoDong_CWTS[ %i ];
//						%DK.SafeDeleteObject();
//					}
//				}
//		}


	//过一分钟后再执行时间检查
	if(%S < 30)
		%Time = ( 30 - %S + 60 ) * (60 - %M);
	else
		%Time = ( 60 - %S + 30 ) * (60 - %M);

	Schedule(%Time @ "000", 0, "CheckServerTime");
}

Schedule(60000, 0, "CheckServerTime");
//■■■■■■■■■■■服务器定时刷新排行榜■■■■■■■■■■■■■■■■

//■■■■■■■■■■■状态结束后触发■■■■■■■■■■■■■■■■■■■
//function BuffOver(%Player, %BuffID)
//{
//	switch(%BuffID)
//	{
//		case 1111:
//	}
//}
//■■■■■■■■■■■状态结束后触发■■■■■■■■■■■■■■■■■■■

