//异宝觅踪
function Mission20002(%Npc, %Player, %State, %Conv)
{
		%Mid = "20002";										 //任务编号
		%PlayerID = %Player.GetPlayerID(); //获取玩家的编号
		%LV = %Player.GetLevel();          //玩家等级               

		if(%State ==0)
			{
				if((%Npc.GetDataID() $= "401002118")||(%Npc.GetDataID() $= "400001118"))//寻宝
					{		
							%Conv.AddOption(700 @ %Mid, 700);//显示藏宝图选项  						  
					}
			}
				
   if(%State ==700)
   	 {
   	 		%Conv.SetText(701 @ %Mid);      //显示任务对话
   	 		%Conv.AddOption(702 @ %Mid, 702);//显示藏宝图选项 
   	 		%Conv.AddOption(1,0);          //结束对话
   	 }
   	 
   if(%State ==702)
		 {
		 	 	if((%LV >= 30)&&(%LV < 40)){%ZHS = 10;}
		    if((%LV >= 40)&&(%LV < 50)){%ZHS = 20;}
		    if(%LV > 50){%ZHS = 50;}
		    
		    if(%Player.GetMissionFlag(%Mid,400)!=0){%Conv.SetText( 501 @ %Mid);}
			  else if(%Player.GetLevel() < 30 ){%Conv.SetText(502 @ %Mid);}
			  else if(%Player.GetAcceptedMission() >= 20){%Conv.SetText(503 @ %Mid);}
			  else if(%Player.GetCycleMissionTimes(%Mid) > %ZHS ){%Conv.SetText(504 @ %Mid);}
			  else if(%Player.GetMoney(1) > 200){%Conv.SetText(504 @ %Mid);}
			  else return Mission20002_LeiXing(%Npc, %Player, %State, %Conv);	//任务类型设置
			  				
			}
}


function Mission20002_LeiXing(%Npc, %Player, %State, %Conv)
{
			%ItemAdd = %Player.PutItem(108020260, 1); //显形符
			%ItemAdd = %Player.AddItem();
			
			if(!%ItemAdd)
				{
					SendOneChatMessage(0,"<t>背包满了</t>",%Player);
					SendOneScreenMessage(2,"背包满了", %Player);
					return;
				}
			 %Player.Reducemoney(200,1); //设置扣除金币		
			 %Player.SetCycleMission(%Mid, %Zs+1);            //设置任务总数
			 %Zd = %Player.GetCycleMissionTimes(%Mid); 
			 %Player.SetMissionFlag(%Mid,9100,%Zd);           //设置任务总数旗标	
						
			 %Conv.SetType(4);	
}
			   
			   
function Trigger_108020260(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)//显形符
{
	%Mid = "20002";											               //任务编号
	%PlayerID = %Player.GetPlayerID();                 //获取玩家的编号
	%Zb1 =%Player.GetMissionFlag(%Mid,8200);           //获取1号坐标点     
	%Zb2 =%Player.GetMissionFlag(%Mid,8300);           //获取2号坐标点
	 
	if((%Zb1 == 0)&&(%Zb2 ==0)) 
		{		
			%Cbt = GetRandom(1,4);
										
			if(%Cbt ==1){eval("%HideMapData = $QuYuDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==2){eval("%HideMapData = $QuYuDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==3){eval("%HideMapData = $QuYuDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==4){eval("%HideMapData = $QuYuDiTu"@ "[%Cbt]" @ ";");}

			echo("%Cbt="@%Cbt);	
			echo("%HideMapData="@%HideMapData);
										
			if(%HideMapData $="811010100")
				{
					%Zb = GetRandom(1,3);
					if(%Zb ==1){eval("%ZuoBiao = $QuYuZuoBiao_QFB"@ "[%Zb]" @ ";");}
					if(%Zb ==2){eval("%ZuoBiao = $QuYuZuoBiao_QFB"@ "[%Zb]" @ ";");}
					if(%Zb ==3){eval("%ZuoBiao = $QuYuZuoBiao_QFB"@ "[%Zb]" @ ";");}							
				}	
			if(%HideMapData $="811020100")
				{
					%Zb = GetRandom(1,3);	  		
					if(%Zb ==1){eval("%ZuoBiao = $QuYuZuoBiao_XYL"@"[%Zb]" @ ";");}
					if(%Zb ==2){eval("%ZuoBiao = $QuYuZuoBiao_XYL"@"[%Zb]" @ ";");}
					if(%Zb ==3){eval("%ZuoBiao = $QuYuZuoBiao_XYL"@"[%Zb]" @ ";");}
													
				}
			if(%HideMapData $="811030100")
				{
					%Zb = GetRandom(1,3);
					if(%Zb ==1){eval("%ZuoBiao = $QuYuZuoBiao_WYCY"@"[%Zb]" @ ";");}
					if(%Zb ==2){eval("%ZuoBiao = $QuYuZuoBiao_WYCY"@"[%Zb]" @ ";");}
					if(%Zb ==3){eval("%ZuoBiao = $QuYuZuoBiao_WYCY"@"[%Zb]" @ ";");}
				}
			if(%HideMapData $="811040100")
				{
					%Zb = GetRandom(1,3);		
					if(%Zb ==1){eval("%ZuoBiao = $QuYuZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
					if(%Zb ==2){eval("%ZuoBiao = $QuYuZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
					if(%Zb ==3){eval("%ZuoBiao = $QuYuZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
				}
				
				echo("%Zb="@%Zb);	
				echo("%ZuoBiao="@%ZuoBiao);	
					
			  %Map_ZuoBiao2 = GetWord(%ZuoBiao,0);             //保存2D坐标第一位		 
			  %Map_ZuoBiao3 = GetWord(%ZuoBiao,1);             //保存2D坐标第二位
			   
		  	echo("%Map_ZuoBiao2="@%Map_ZuoBiao2);
			  echo("%Map_ZuoBiao3="@%Map_ZuoBiao3); 
			  
			  %Player.SetMissionFlag(%Mid,8100,%HideMapData);   //保存地图区域编号
			  %Player.SetMissionFlag(%Mid,8200,%Map_ZuoBiao2);  //旗标值保存坐标点
			  %Player.SetMissionFlag(%Mid,8300,%Map_ZuoBiao3);  //旗标值保存坐标点
			  
			  echo("区域="@%Player.GetMissionFlag(%Mid,8100));
	      echo("坐标1="@%Player.GetMissionFlag(%Mid,8200));
	      echo("坐标2="@%Player.GetMissionFlag(%Mid,8300));

			  %Player.UpdateMission(%Mid);					           //更新任务信息
				%Player.AddPacket( $SP_Effect[ 1 ] );	           //发送接受任务特效
				%Conv.SetText(500 @ %Mid);	                     //显示交任务对话
				%Conv.AddOption(1,710);                          //选项：结束对话
				
		 }
		 		
  if((%Zb1 != 0)&&(%Zb2 != 0))                               //判断是否有任务
    { 
    	  %QuYu = %Player.GetMissionFlag(%Mid,8100);           //获取地图区域编号
    	  %Map_ZuoBiao2 = %Player.GetMissionFlag(%Mid,8200);   //获取地图区域编号
    	  %Map_ZuoBiao3 = %Player.GetMissionFlag(%Mid,8300);   //获取地图区域编号
    	  
    		%WJZB = %Player.GetPosition();			         //获取玩家当前坐标
				%WJZB_1 = GetWord(%WJZB,0);   			         //玩家当前坐标第一位
				%WJZB_2 = GetWord(%WJZB,1);  				         //玩家当前坐标第二位
				echo("%QuYu="@%QuYu);
				echo("%Map_ZuoBiao2="@%Map_ZuoBiao2);
				echo("%Map_ZuoBiao3="@%Map_ZuoBiao3);
				echo("%WJZB="@%WJZB);	
				echo("%WJZB_1="@%WJZB_1);
				echo("%WJZB_2="@%WJZB_2);
				
			if($Player_Area[%PlayerID] $= %QuYu) //判断区域是否正确
				{
					echo("$Player_Area[%PlayerID] = "@$Player_Area[%PlayerID]);
					
					if(((%WJZB_1 <= %Map_ZuoBiao2 + 10)&&(%WJZB_1 >= %Map_ZuoBiao2 - 10))||((%WJZB_2 <= %Map_ZuoBiao3 + 10)&&(%WJZB_2 >= %Map_ZuoBiao3 - 10))) //判断坐标区间
					  {
							if($New_108020208 ==0) //判断是否已经刷怪了
								{
									
									%Player.DelMission(%Mid);
									
									%ItemAdd = %Player.PutItem(108020260, -1); //扣除显形符
									%ItemAdd = %Player.AddItem();

									$New_108020208 = SpNewNpc(%Player,410000013,0,0); //刷新怪物在玩家身边
									Schedule(60000, 0, "RemoveNpc_108020208");        //设置一定时间触发某功能
								}	
						} 
				}
				
		}
		
}

function RemoveNpc_108020208()
{
	$New_108020208.SafeDeleteObject();
	$New_108020208= 0;
}

function Trigger_108020261(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)//藏宝图
{
	%Mid = "20003";											               //任务编号
	%PlayerID = %Player.GetPlayerID();                 //获取玩家的编号
	%WaBaoZb1 =%Player.GetMissionFlag(%Mid,8500);      //获取1号坐标点     
	%WaBaoZb2 =%Player.GetMissionFlag(%Mid,8600);      //获取2号坐标点
	 
	if((%WaBaoZb1 == 0)&&(%WaBaoZb2 ==0)) 
		{		
			%ItemAdd = %Player.PutItem(108020261, -1); //扣除藏宝图
			%ItemAdd = %Player.AddItem();
			
			%ItemAdd = %Player.PutItem(108020262, 1);  //给予已打开藏宝图
			%ItemAdd = %Player.AddItem();

			if(!%ItemAdd)
				{
					SendOneChatMessage(0,"<t>背包满了</t>",%Player);
					SendOneScreenMessage(2,"背包满了", %Player);
					return;
				}
			
			%Cbt = GetRandom(1,4);
										
			if(%Cbt ==1){eval("%HideMapData = $WaBaoDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==2){eval("%HideMapData = $WaBaoDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==3){eval("%HideMapData = $WaBaoDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==4){eval("%HideMapData = $WaBaoDiTu"@ "[%Cbt]" @ ";");}

			if(%HideMapData $="811010100")
				{
					%Zb = GetRandom(1,3);
					if(%Zb ==1){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_QFB"@ "[%Zb]" @ ";");}
					if(%Zb ==2){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_QFB"@ "[%Zb]" @ ";");}
					if(%Zb ==3){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_QFB"@ "[%Zb]" @ ";");}							
				}	
			if(%HideMapData $="811020100")
				{
					%Zb = GetRandom(1,3);	  		
					if(%Zb ==1){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_XYL"@"[%Zb]" @ ";");}
					if(%Zb ==2){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_XYL"@"[%Zb]" @ ";");}
					if(%Zb ==3){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_XYL"@"[%Zb]" @ ";");}
													
				}
			if(%HideMapData $="811030100")
				{
					%Zb = GetRandom(1,3);
					if(%Zb ==1){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_WYCY"@"[%Zb]" @ ";");}
					if(%Zb ==2){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_WYCY"@"[%Zb]" @ ";");}
					if(%Zb ==3){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_WYCY"@"[%Zb]" @ ";");}
				}
			if(%HideMapData $="811040100")
				{
					%Zb = GetRandom(1,3);		
					if(%Zb ==1){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
					if(%Zb ==2){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
					if(%Zb ==3){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
				}
				
			  %WaBao_ZuoBiao2 = GetWord(%WaBaoZuoBiao,0);          //保存2D坐标第一位		 
			  %WaBao_ZuoBiao3 = GetWord(%WaBaoZuoBiao,1);          //保存2D坐标第二位
			   	  
			  %Player.SetMissionFlag(%Mid,8400,%HideMapData);      //保存地图区域编号
			  %Player.SetMissionFlag(%Mid,8500,%WaBao_ZuoBiao2);   //旗标值保存坐标点
			  %Player.SetMissionFlag(%Mid,8600,%WaBao_ZuoBiao3);   //旗标值保存坐标点
		      		
			  %Player.UpdateMission(%Mid);					               //更新任务信息
				%Player.AddPacket( $SP_Effect[ 1 ] );	               //发送接受任务特效
				%Conv.SetText(600 @ %Mid);	                         //显示交任务对话
				%Conv.AddOption(1,710);                              //选项：结束对话
			
		 }
}

function Trigger_108020262(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)//已打开藏宝图
{
	 %Mid = "20003";										                   //任务编号
	 %PlayerID = %Player.GetPlayerID();                    //获取玩家的编号
	 %WaBaoQuYu = %Player.GetMissionFlag(%Mid,8400);       //获取地图区域编号
   %WaBaoZuoBiao2 = %Player.GetMissionFlag(%Mid,8500);   //获取地图区域编号
   %WaBaoZuoBiao3 = %Player.GetMissionFlag(%Mid,8600);   //获取地图区域编号   	  
   %WJZB = %Player.GetPosition();			                   //获取玩家当前坐标
	 %WJZB_1 = GetWord(%WJZB,0);   			                   //玩家当前坐标第一位
	 %WJZB_2 = GetWord(%WJZB,1);  				                 //玩家当前坐标第二位
	
	if($Player_Area[%PlayerID] $= %WaBaoQuYu) 						 //判断区域是否正确
		{
			echo("$Player_Area[%PlayerID] = "@$Player_Area[%PlayerID]);
					
			if(((%WJZB_1 <= %WaBaoZuoBiao2 + 10)&&(%WJZB_1 >= %WaBaoZuoBiao2 - 10))||((%WJZB_2 <= %WaBaoZuoBiao3 + 10)&&(%WJZB_2 >= %WaBaoZuoBiao3 - 10))) //判断坐标区间
				{
						%Player.DelMission(%Mid); //删除任务
						%ItemAdd = %Player.PutItem(108020262, -1); //扣除已打开藏宝图
						%ItemAdd = %Player.AddItem();
						
						%ChuFa = GetRandom(1,5); //随机触发事件
						
						if(%ChuFa ==1) //触发到给予金钱
							{
								eval("%ChuFaData = $WaBao_Money" @ ";"); //确认随机数组
								%SJ_Money = GetRandom(1,%ChuFaData);  //随机数组中的值
								eval("%MoneyID = $WaBao_Money" @ "[%SJ_Money]"@ ";");
								
								Player.Addmoney(%MoneyID,2);
							}
						if(%ChuFa ==2) //触发到刷新怪物
							{
									eval("%ChuFaData = $WaBao_Fight" @ ";");
									%SJ_Fight = GetRandom(1,%ChuFaData);  //随机数组中的值
									eval("%FightID = $WaBao_Fight" @ "[%SJ_Fight]"@ ";");
									
									if($New_108020262 ==0) //判断是否已经刷怪了
										{
											$New_108020262 = SpNewNpc(%Player,%FightID,0,0); //刷新怪物在玩家身边
											Schedule(60000, 0, "RemoveNpc_108020262");        //设置一定时间触发某功能
										}	
								
							}
						if(%ChuFa ==3) //触发到给予物品
							{
								eval("%ChuFaData = $WaBao_Item" @ ";");
								%SJ_Item = GetRandom(1,%ChuFaData);  //随机数组中的值
								eval("%ItemID = $WaBao_Item" @ "[%SJ_Item]"@ ";");
								
								%ItemAdd = %Player.PutItem(%ItemID, 1); 
								%ItemAdd = %Player.AddItem();
										
								if(!%ItemAdd)
								  {
										SendOneChatMessage(0,"<t>背包满了</t>",%Player);
										SendOneScreenMessage(2,"背包满了", %Player);
									  return;
									}
								
							}
						if(%ChuFa ==4) //随机陷阱
							{
								eval("%ChuFaData = $WaBao_XianJing" @ ";");
								%SJ_XianJing = GetRandom(1,%ChuFaData);  //随机数组中的值
								eval("%XianJingID = $WaBao_XianJing" @ "[%SJ_XianJing]"@ ";");
								
								%Player.AddBuff(%XianJingID);
								
							}
						if(%ChuFa ==5) //随机到随机传送
							{
								eval("%ChuFaData = $WaBao_ChuanSong" @ ";");
								%SJ_ChuanSong = GetRandom(1,%ChuFaData);  //随机数组中的值
								eval("%ChuanSongID = $WaBao_ChuanSong" @ "[%SJ_ChuanSong]"@ ";");
								
							}

				  } 
		}
	
}

function RemoveNpc_108020262()
{
	$New_108020262.SafeDeleteObject();
	$New_108020262= 0;
}

//■■■■■■■寻踪■■■■■■■■■■■■■■■■■■■■
//地图区域
$QuYuDiTu[$QuYuDiTu++] = "811010100";
$QuYuDiTu[$QuYuDiTu++] = "811020100";
$QuYuDiTu[$QuYuDiTu++] = "811030100";
$QuYuDiTu[$QuYuDiTu++] = "811040100";

//指定地图坐标

//清风滨
$QuYuZuoBiao_QFB[$QuYuZuoBiao_QFB++] = "-201.459 -182.045 39.4386";
$QuYuZuoBiao_QFB[$QuYuZuoBiao_QFB++] = "-194.866 -403.906 4.24695";
$QuYuZuoBiao_QFB[$QuYuZuoBiao_QFB++] = "-336.401 -164.23 16.5954";
//仙游岭
$QuYuZuoBiao_XYL[$QuYuZuoBiao_XYL++] = "130.507 -124.276 94.9672";
$QuYuZuoBiao_XYL[$QuYuZuoBiao_XYL++] = "140.095 -0.899079 73.4717";
$QuYuZuoBiao_XYL[$QuYuZuoBiao_XYL++] = "-66.9969 31.8353 72.3157";
//无忧草野
$QuYuZuoBiao_WYCY[$QuYuZuoBiao_WYCY++] = "67.7467 -94.3243 73.1447";
$QuYuZuoBiao_WYCY[$QuYuZuoBiao_WYCY++] = "99.0152 -13.9025 70.5331";
$QuYuZuoBiao_WYCY[$QuYuZuoBiao_WYCY++] = "-84.6463 -28.8356 80.3844";
//遮月森林
$QuYuZuoBiao_ZYSL[$QuYuZuoBiao_ZYSL++] = "-83.7572 -94.8396 114.752";
$QuYuZuoBiao_ZYSL[$QuYuZuoBiao_ZYSL++] = "143.008 -54.2059 109.631";
$QuYuZuoBiao_ZYSL[$QuYuZuoBiao_ZYSL++] = "-28.0793 52.1808 122.411";
//■■■■■■■寻踪■■■■■■■■■■■■■■■■■■■■

//■■■■■■■挖宝■■■■■■■■■■■■■■■■■■■■
//地图区域
$WaBaoDiTu[$WaBaoDiTu++] = "811010100";
$WaBaoDiTu[$WaBaoDiTu++] = "811020100";
$WaBaoDiTu[$WaBaoDiTu++] = "811030100";
$WaBaoDiTu[$WaBaoDiTu++] = "811040100";

//指定地图坐标

//清风滨
$WaBaoZuoBiao_QFB[$WaBaoZuoBiao_QFB++] = "-254.13 -442.207 3.57358";
$WaBaoZuoBiao_QFB[$WaBaoZuoBiao_QFB++] = "-166.435 -320.94 24.9156";
$WaBaoZuoBiao_QFB[$WaBaoZuoBiao_QFB++] = "-215.88 -180.382 40.2003";
//仙游岭
$WaBaoZuoBiao_XYL[$WaBaoZuoBiao_XYL++] = "23.1289 63.0308 73.2148";
$WaBaoZuoBiao_XYL[$WaBaoZuoBiao_XYL++] = "-25.5479 -37.3814 73.0522";
$WaBaoZuoBiao_XYL[$WaBaoZuoBiao_XYL++] = "116.454 39.2686 71.9175";
//无忧草野
$WaBaoZuoBiao_WYCY[$WaBaoZuoBiao_WYCY++] = "118.946 -5.22342 74.4776";
$WaBaoZuoBiao_WYCY[$WaBaoZuoBiao_WYCY++] = "-91.5556 -103.62 70.5847";
$WaBaoZuoBiao_WYCY[$WaBaoZuoBiao_WYCY++] = "-37.8235 -93.827 71.739";
//遮月森林
$WaBaoZuoBiao_ZYSL[$WaBaoZuoBiao_ZYSL++] = "45.4196 -47.0228 118.997";
$WaBaoZuoBiao_ZYSL[$WaBaoZuoBiao_ZYSL++] = "-54.7147 8.18625 111";
$WaBaoZuoBiao_ZYSL[$WaBaoZuoBiao_ZYSL++] = "9.46716 -129.449 120.552";

//■■■■■■■挖宝■■■■■■■■■■■■■■■■■■■■

//■■■■■■■挖宝触发事件■■■■■■■■■■■■■■■■■■■■
//金钱
$WaBao_Money[$WaBao_Money++] ="100";
$WaBao_Money[$WaBao_Money++] ="200";
$WaBao_Money[$WaBao_Money++] ="300";
$WaBao_Money[$WaBao_Money++] ="400";
$WaBao_Money[$WaBao_Money++] ="500";
$WaBao_Money[$WaBao_Money++] ="600";
$WaBao_Money[$WaBao_Money++] ="700";
$WaBao_Money[$WaBao_Money++] ="800";
$WaBao_Money[$WaBao_Money++] ="900";
$WaBao_Money[$WaBao_Money++] ="1000";

//怪物
$WaBao_Fight[$WaBao_Fight++] ="410701001";
$WaBao_Fight[$WaBao_Fight++] ="410701002";
$WaBao_Fight[$WaBao_Fight++] ="410701003";

//物品
$WaBao_Item[$WaBao_Item++] ="105010101";
$WaBao_Item[$WaBao_Item++] ="105010102";
$WaBao_Item[$WaBao_Item++] ="105010103";
$WaBao_Item[$WaBao_Item++] ="105010104";
$WaBao_Item[$WaBao_Item++] ="105010105";
$WaBao_Item[$WaBao_Item++] ="105010106";
$WaBao_Item[$WaBao_Item++] ="105010107";
$WaBao_Item[$WaBao_Item++] ="105010108";
$WaBao_Item[$WaBao_Item++] ="105010109";
$WaBao_Item[$WaBao_Item++] ="105010110";

//BOSS

//副本

//陷阱
$WaBao_XianJing[$WaBao_XianJing++] ="307020020";

//传送
$WaBao_ChuanSong[$WaBao_ChuanSong++] ="45.4196 -47.0228 118.997";  
$WaBao_ChuanSong[$WaBao_ChuanSong++] ="-54.7147 8.18625 111";      
$WaBao_ChuanSong[$WaBao_ChuanSong++] ="9.46716 -129.449 120.552";  
$WaBao_ChuanSong[$WaBao_ChuanSong++] ="-91.5556 -103.62 70.5847";
$WaBao_ChuanSong[$WaBao_ChuanSong++] ="-37.8235 -93.827 71.739"; 
                                      
//■■■■■■■挖宝触发事件■■■■■■■■■■■■■■■■■■■■