//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//服务端任务脚本集，包含所有特殊的任务脚本
//
//==================================================================================


function Mission_Special(%Npc, %Player, %State, %Conv, %Param)
{
	
	if(%Npc.GetDataID() $= "400001122"){Mission_Special_10103(%Npc, %Player, %State, %Conv, %Param);}	//莲花台	
	if(%Npc.GetDataID() $= "400001102"){Mission_Special_10109(%Npc, %Player, %State, %Conv, %Param);}	//女娲碑文
//	if(%Npc.GetDataID() $= "401303101"){Mission_Special_10110(%Npc, %Player, %State, %Conv, %Param);}	//凤阳子的幻象

  if(%Npc.GetDataID() $= "400001007")
  	{
  		Mission_Special_10001(%Npc, %Player, %State, %Conv, %Param,10001);
  		Mission_Special_10112(%Npc, %Player, %State, %Conv, %Param,10112);
  	}//苏苏
  	
  if(%Npc.GetDataID() $= "400001001")//李时珍
  	{
  		Mission_Special_10113(%Npc, %Player, %State, %Conv, %Param);
  		Mission_Special_10114(%Npc, %Player, %State, %Conv, %Param);
  	}
  if(%Npc.GetDataID() $= "400001060"){Mission_Special_10116(%Npc, %Player, %State, %Conv, %Param);}//福星
   	
	if(%Npc.GetDataID() $= "400001101")//风阳子
		{
			Mission_Special_10000(%Npc, %Player, %State, %Conv, %Param, 10000);//扬名万灵城
			Mission_Special_10119(%Npc, %Player, %State, %Conv, %Param, 10119);//心灵的试炼
			Mission_Special_10121(%Npc, %Player, %State, %Conv, %Param, 10121);//力量的试炼
			Mission_Special_10122(%Npc, %Player, %State, %Conv, %Param, 10122);//八大门宗
		}
  if(%Npc.GetDataID() $= "400001102"){Mission_Special_10123(%Npc, %Player, %State, %Conv, %Param,10123);}	//女娲神像
	if( (%Npc.GetDataID() $= "410107001")||(%Npc.GetDataID() $= "410207001")||(%Npc.GetDataID() $= "410307001")||(%Npc.GetDataID() $= "410407001")||
			(%Npc.GetDataID() $= "410507001")||(%Npc.GetDataID() $= "410607001")||(%Npc.GetDataID() $= "410707001")||(%Npc.GetDataID() $= "410807001") )
		{
			Mission_Special_10123(%Npc, %Player, %State, %Conv, %Param,10123);//正宗之道
			Mission_Special_10123_2(%Npc, %Player, %State, %Conv, %Param, 10123);//正宗之道
			Mission_Special_10124(%Npc, %Player, %State, %Conv, %Param, 10124);//新的神通
		}

	if( (%Npc.GetDataID() $= "401011002")||(%Npc.GetDataID() $= "401006002")||(%Npc.GetDataID() $= "401007002")||(%Npc.GetDataID() $= "401005002")||
			(%Npc.GetDataID() $= "401009002")||(%Npc.GetDataID() $= "401010002")||(%Npc.GetDataID() $= "401008002")||(%Npc.GetDataID() $= "401004002") )
		{
			Mission_Special_10124(%Npc, %Player, %State, %Conv, %Param, 10124);//新的神通
			Mission_Special_10125(%Npc, %Player, %State, %Conv, %Param, 10125);//破木
			Mission_Special_10126(%Npc, %Player, %State, %Conv, %Param, 10126);//属于你的任务
		}

	if( (%Npc.GetDataID() $= "401011006")||(%Npc.GetDataID() $= "401006006")||(%Npc.GetDataID() $= "401007006")||(%Npc.GetDataID() $= "401005006")||
			(%Npc.GetDataID() $= "401009006")||(%Npc.GetDataID() $= "401010006")||(%Npc.GetDataID() $= "401008006")||(%Npc.GetDataID() $= "401004006") )
		{
			Mission_Special_10126(%Npc, %Player, %State, %Conv, %Param, 10126);//属于你的任务
			Mission_Special_10127(%Npc, %Player, %State, %Conv, %Param, 10127);//相助同门弟子
		}

	if( (%Npc.GetDataID() $= "400001038")||(%Npc.GetDataID() $= "400001044")||(%Npc.GetDataID() $= "400001039")||(%Npc.GetDataID() $= "400001042")||
			(%Npc.GetDataID() $= "400001041")||(%Npc.GetDataID() $= "400001040")||(%Npc.GetDataID() $= "400001043")||(%Npc.GetDataID() $= "400001045") )
		{
			Mission_Special_10122(%Npc, %Player, %State, %Conv, %Param, 10122);//八大门宗
			Mission_Special_10127(%Npc, %Player, %State, %Conv, %Param, 10127);//相助同门弟子
			Mission_Special_10128(%Npc, %Player, %State, %Conv, %Param, 10128);//火眼金精
		}
		
		
	if(%Npc.GetDataID() $= "400001059")
		{
			Mission_Special_10128(%Npc, %Player, %State, %Conv, %Param,10128);
			Mission_Special_10140(%Npc, %Player, %State, %Conv, %Param);
		}	//火眼金精
	if(%Npc.GetDataID() $= "400002105"){Mission_Special_10153(%Npc, %Player, %State, %Conv, %Param);}	//夏梦狼
	if(%Npc.GetDataID() $= "400002105"){Mission_Special_10161(%Npc, %Player, %State, %Conv, %Param);}	//夏梦狼
	if(%Npc.GetDataID() $= "400001104"){Mission_Special_10156(%Npc, %Player, %State, %Conv, %Param);}	//任惊梦
	if(%Npc.GetDataID() $= "400002110"){Mission_Special_10186(%Npc, %Player, %State, %Conv, %Param);}	//青和
	if(%Npc.GetDataID() $= "400002115"){Mission_Special_10233(%Npc, %Player, %State, %Conv, %Param);}	//甘婆婆
	if(%Npc.GetDataID() $= "401102121"){Mission_Special_10303(%Npc, %Player, %State, %Conv, %Param);}	//蛇洞一
	if(%Npc.GetDataID() $= "401102122"){Mission_Special_10303(%Npc, %Player, %State, %Conv, %Param);}	//蛇洞二	
	if(%Npc.GetDataID() $= "401102123"){Mission_Special_10303(%Npc, %Player, %State, %Conv, %Param);}	//蛇洞三
  if(%Npc.GetDataID() $= "401102109"){Mission_Special_10305(%Npc, %Player, %State, %Conv, %Param);}	//大锅
  if(%Npc.GetDataID() $= "401102103"){Mission_Special_10316(%Npc, %Player, %State, %Conv, %Param);}	//罗村长
  if(%Npc.GetDataID() $= "401102112"){Mission_Special_10320(%Npc, %Player, %State, %Conv, %Param);}	//夕苍
  if(%Npc.GetDataID() $= "401102115"){Mission_Special_10321(%Npc, %Player, %State, %Conv, %Param);}	//魔玉
  if(%Npc.GetDataID() $= "401103020"){Mission_Special_10527_1(%Npc, %Player, %State, %Conv, %Param);}	//张南
  if(%Npc.GetDataID() $= "401103021"){Mission_Special_10527_2(%Npc, %Player, %State, %Conv, %Param);}	//周明
  if(%Npc.GetDataID() $= "401103015"){Mission_Special_10527_3(%Npc, %Player, %State, %Conv, %Param);}	//沐风	
  if(%Npc.GetDataID() $= "401103017"){Mission_Special_10545(%Npc, %Player, %State, %Conv, %Param);}	//凤凰
  if(%Npc.GetDataID() $= "401103019"){Mission_Special_10580(%Npc, %Player, %State, %Conv, %Param);}	//释厄
  if(%Npc.GetDataID() $= "401103014"){Mission_Special_10584(%Npc, %Player, %State, %Conv, %Param);}	//檀郎	
  if(%Npc.GetDataID() $= "401305001"){Mission_Special_10585(%Npc, %Player, %State, %Conv, %Param);}	//贪狼星君·檀郎
  if(%Npc.GetDataID() $= "401002106"){Mission_Special_10704(%Npc, %Player, %State, %Conv, %Param);}	//谢竺君	
  if(%Npc.GetDataID() $= "401104005"){Mission_Special_10743(%Npc, %Player, %State, %Conv, %Param);}	//紫昙秋
  if(%Npc.GetDataID() $= "401104010"){Mission_Special_10761(%Npc, %Player, %State, %Conv, %Param);}	//桑雪琴
  if(%Npc.GetDataID() $= "401105003"){Mission_Special_10907(%Npc, %Player, %State, %Conv, %Param);}	//族人魂魄
  if(%Npc.GetDataID() $= "401105004"){Mission_Special_10908(%Npc, %Player, %State, %Conv, %Param);}	//巴尔丁
  if(%Npc.GetDataID() $= "401105009"){Mission_Special_10915(%Npc, %Player, %State, %Conv, %Param);}	//桑木乌达
								 		
}

function Mission_Special_10000(%Npc, %Player, %State, %Conv, %Param,%Mid)
{
	if(%State == 0)
		{
			if(%Player.IsAcceptedMission(%Mid))
				if(%Player.GetLevel() < 10)
						  %Conv.AddOption(900 @ %Mid, 120 @ %Mid);//显示接任务选择
		}
		
	if((%State > 0)&&(%Mid == 10000))
		{
			%MidState = GetSubStr(%State,0,3);
			
			if(%MidState == 120)
				{
				   %Conv.SetText(%Mid);	
				}
		}  
}

function Mission_Special_10001(%Npc, %Player, %State, %Conv, %Param,%Mid)
{
	%Mid = 10001;
	
	if(%State == 0)
		{
			if(%Player.IsFinishedMission(10112))
				if(!%Player.IsFinishedMission(%Mid))
					if(!%Player.IsAcceptedMission(%Mid))
						  %Conv.AddOption(100 @ %Mid, 120 @ %Mid);//显示接任务选择
						 
			if(%Player.IsAcceptedMission(%Mid))
				 if(!%Player.IsFinishedMission(%Mid))
		   		   %Conv.AddOption(900 @ %Mid, 920 @ %Mid);//显示交任务选择
		}
		
	if((%State > 0)&&(%Mid == 10001))
		{
			%MidState = GetSubStr(%State,0,3);
			%Pet = %Player.GetSpawnedPet();

			if(%MidState == 120)
				{
					if(%Pet.GetLevel() == 0)
						  %Conv.SetText(%Mid + 1);
					else 
						{
						  %Conv.SetText(201 @ %Mid);
							%Conv.AddOption(210 @ %Mid, 210 @ %Mid);//接受任务
							%Conv.AddOption(300 @ %Mid, 300 @ %Mid);//关闭
						}
							
				}
			if(%MidState == 210)
				{
					AddMissionAccepted(%Player, %Mid);	//设置任务相关信息
					%Conv.SetType(4);
				}
				
			if(%MidState == 920)
				{
           if(%Pet.GetLevel() >= 10)
           	 { 
	              AddMissionReward(%Player, %Mid, %Param);	//设置任务奖励
	              %Conv.SetType(4);
             }
           else
							%Conv.SetText(%Mid);
				}
		}  
}


function Mission_Special_10103(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10103;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			
			if(%State == 0)
				{
					if(%Player.GetMissionFlag(%Mid, 1350) == 0)		
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{	
							%Player.SetMissionFlag(%Mid, 1350, 1, true); //设置,目标触发当前数量
						  %Conv.SetType(4);
					
						}			
				}
		}
}

function Mission_Special_10109(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10109;
	
	if(%Player.IsAcceptedMission(%Mid))
			{
				if(%State == 0)
					{
						%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//进入女娲神像体内
						%Conv.AddOption(300 @ %Mid, -1 );	//稍等片刻，我还需修整片刻
					}
	
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						
						if(%MidState == 200)
							{
								 if((%Player.GetMissionFlag(%Mid,3100) == 0)||(%Player.GetMissionFlag(%Mid,3101) == 0))
									 {  
	
									 	  %Player.SetMissionFlag(%Mid, 1350, 1, true); 				//设置,目标触发当前数量
											%Player.SetMissionFlag(%Mid,3100,410700022);		//设置,目标编号
										  %Player.SetMissionFlag(%Mid,3200,4);				//设置,目标触发总数
											%Player.SetMissionFlag(%Mid,3300,0); 				//设置,目标触发当前数量
											%Player.SetMissionFlag(%Mid,3101,410700024);		//设置,目标编号
											%Player.SetMissionFlag(%Mid,3201,1);				//设置,目标触发总数
										  %Player.SetMissionFlag(%Mid,3301,0); 				//设置,目标触发当前数量
										  %Player.UpdateMission(%Mid);							 //更新任务信息
									 }
		                  %Player.SetFlagsByte(25,0);
		                  %Player.SetFlagsByte(27,0);
		                  
											GoToNextMap_CopyMap(%Player, 1303);
											%Conv.SetType(4);
	
						  }	
					}
			}
}

function Mission_Special_10112(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	%Mid = 10112;
	
	if(%Player.IsAcceptedMission(%Mid))
		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) !$= "")
			{
				if(%State == 0)
					{
		   			%Conv.AddOption(900 @ %Mid, 920 @ %Mid);//显示交任务选择
		   		}
		
				if((%State > 0)&&(%Mid == 10112))
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 920)
							{
								%Conv.SetText(%Mid);
								%Conv.AddOption(200 @ %Mid, 200 @ %Mid);//传送去灵谷捕兽
						    %Conv.AddOption(300 @ %Mid, -1);//等一下再去捕兽
							}
							
					 if(%MidState == 200)
							{
								GoToNextMap_Normal(%Player,112401);
								%Conv.SetType(4);
							}
					}  
			}
}

function Mission_Special_10113(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10113;
	
	if(%Player.IsAcceptedMission(%Mid))
		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) !$= "")
			{
				if(%State == 0)
					{
		   			%Conv.AddOption(900 @ %Mid, 920 @ %Mid);//显示交任务选择
		   		}
		
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 920)
							{
								%Conv.SetText(%Mid);
							}
					}  
			}
}

function Mission_Special_10114(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10114;
	
	if(%Player.IsAcceptedMission(%Mid))
		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) !$= "")
			{
				if(%State == 0)
					{
		   			%Conv.AddOption(900 @ %Mid, 920 @ %Mid);//显示交任务选择
		   		}
		
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 920)
							{
								%Conv.SetText(%Mid);
							}
					}  
			}
}

function Mission_Special_10116(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10116;
	
	if(%Player.IsAcceptedMission(%Mid))
		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) !$= "")
			{
				
				if(%State == 0)
					{
		   			%Conv.AddOption(900 @ %Mid, 920 @ %Mid);//显示交任务选择
		   		}
		
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 920)
							{
								%Conv.SetText(%Mid);
							}
					}  
			}
}

function Mission_Special_10119(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//尚未对话
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//显示任务中途对话选择

			if((%State > 0)&&(%Mid == 10119))
				{
					%MidState = GetSubStr(%State,0,3);
					if( (%MidState >= 200)&&(%MidState <= 400) )
						{
							%Dlg = %MidState + GetRandom(1,3) * 10;

							%Conv.SetText(%Dlg @ %Mid);	//显示问题

							%Yes = GetRandom(1,2);
							if(%Yes == 1){%No = 2;%a = %MidState + 100;%b = 998;}
							if(%Yes == 2){%No = 1;%b = %MidState + 100;%a = 998;}

							%Conv.AddOption(%Dlg + %Yes @ %Mid , %a @ %Mid);//选择1
							%Conv.AddOption(%Dlg + %No  @ %Mid , %b @ %Mid);//选择2
						}
					if(%MidState == 500)
						{
							%Conv.SetText(%MidState @ %Mid);	//全部答对了
							%Conv.AddOption(4 , 0 );		//返回

							%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//设置,目标触发当前数量
						}
					if(%MidState == 998)
						{
							%Conv.SetText(%MidState @ %Mid);	//答错了
					    %Player.AddBuff(320110001);
							%Conv.AddOption(4 , 0 );		//返回
						}
				}
		}
}

function Mission_Special_10121(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%Player.IsAcceptedMission(%Mid))
			{
				if(%State == 0)
					if((%Player.GetMissionFlag(%Mid, 3300) != 1)||(%Player.GetMissionFlag(%Mid, 3301) != 3)||(%Player.GetMissionFlag(%Mid, 2300) != 1))
					{
						%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//我已经准备好去万书楼了
						%Conv.AddOption(300 @ %Mid, -1 );	//稍等片刻，我还需修整片刻
					}
	
				if((%State > 0)&&(%Mid == 10121))
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 200)
							{
								%Player.SetFlagsByte(26,0);	
								
								GoToNextMap_CopyMap(%Player, 1301);
								
								%Conv.SetType(4);
							}
					}
			}
}

function Mission_Special_10122(%Npc, %Player, %State, %Conv, %Param,%Mid)
{
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					if(%Npc.GetDataID() $= "400001042"){%Conv.AddOption(200 @ %Mid, 201 @ %Mid);}//精门宗接引者
					if(%Npc.GetDataID() $= "400001041"){%Conv.AddOption(200 @ %Mid, 202 @ %Mid);}//鬼门宗接引者
					if(%Npc.GetDataID() $= "400001039"){%Conv.AddOption(200 @ %Mid, 203 @ %Mid);}//仙门宗接引者
					if(%Npc.GetDataID() $= "400001038"){%Conv.AddOption(200 @ %Mid, 204 @ %Mid);}//圣门宗接引者
					if(%Npc.GetDataID() $= "400001040"){%Conv.AddOption(200 @ %Mid, 205 @ %Mid);}//怪门宗接引者
					if(%Npc.GetDataID() $= "400001043"){%Conv.AddOption(200 @ %Mid, 206 @ %Mid);}//妖门宗接引者		
					if(%Npc.GetDataID() $= "400001044"){%Conv.AddOption(200 @ %Mid, 207 @ %Mid);}//佛门宗接引者
					if(%Npc.GetDataID() $= "400001045"){%Conv.AddOption(200 @ %Mid, 208 @ %Mid);}//魔门宗接引者
		
					if(%Npc.GetDataID() $= "400001101")
						if((%Player.GetMissionFlag(%Mid, 1350) ==1)&&(%Player.GetMissionFlag(%Mid, 1351) ==1)
							 &&(%Player.GetMissionFlag(%Mid, 1352) ==1)&&(%Player.GetMissionFlag(%Mid, 1353) ==1)
							 &&(%Player.GetMissionFlag(%Mid, 1354) ==1)&&(%Player.GetMissionFlag(%Mid, 1355) ==1)
							 &&(%Player.GetMissionFlag(%Mid, 1356) ==1)&&(%Player.GetMissionFlag(%Mid, 1357) ==1) )
						{%Conv.AddOption(209 @ %Mid, 209 @ %Mid);%Conv.AddOption(300 @ %Mid, 3400);}	
				}		
			if((%State > 0)&&(%Mid == 10122))
				{
					%MidState = GetSubStr(%State,0,3);

					if( (%MidState > 199)&&(%MidState < 209) )
						{
							if(%MidState == 201){%Conv.SetText(201 @ %Mid);%i=0;}
							if(%MidState == 202){%Conv.SetText(202 @ %Mid);%i=1;}
							if(%MidState == 203){%Conv.SetText(203 @ %Mid);%i=2;}
							if(%MidState == 204){%Conv.SetText(204 @ %Mid);%i=3;}
							if(%MidState == 205){%Conv.SetText(205 @ %Mid);%i=4;}
							if(%MidState == 206){%Conv.SetText(206 @ %Mid);%i=5;}
							if(%MidState == 207){%Conv.SetText(207 @ %Mid);%i=6;}
							if(%MidState == 208){%Conv.SetText(208 @ %Mid);%i=7;}
							
							%Player.SetMissionFlag(%Mid, 1350 + %i, 1, true); 				//设置,目标触发当前数量
							%Player.UpdateMission(%Mid);							//更新任务信息
							
						}
						
					if(%MidState ==209)
					  {
					    if(%Player.IsAcceptedMission(%Mid))
								 if(!%Player.IsFinishedMission(%Mid))
										{
											AddMissionReward(%Player, %Mid, %Param);	//设置任务奖励
											NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//执行Npc点击触发的脚本
										}
					  }	
					
				}	
		}						
														
}

function Mission_Special_10123(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					if(%Npc.GetDataID() $= "400001102")
						{
							if(%Player.GetSex() == 1)
								{
									%Conv.AddOption(201 @ %Mid, 201 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
									%Conv.AddOption(202 @ %Mid, 202 @ %Mid);//传送至门宗地图"金禅寺(佛)"极乐西天
									%Conv.AddOption(203 @ %Mid, 203 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
									%Conv.AddOption(205 @ %Mid, 205 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
									%Conv.AddOption(206 @ %Mid, 206 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
									%Conv.AddOption(207 @ %Mid, 207 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
									%Conv.AddOption(208 @ %Mid, 208 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
							  }
							else
								{
									%Conv.AddOption(201 @ %Mid, 201 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
									%Conv.AddOption(203 @ %Mid, 203 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
									%Conv.AddOption(204 @ %Mid, 204 @ %Mid);//传送至门宗地图"飞花谷(精)"神木林
									%Conv.AddOption(205 @ %Mid, 205 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
									%Conv.AddOption(206 @ %Mid, 206 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
									%Conv.AddOption(207 @ %Mid, 207 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
									%Conv.AddOption(208 @ %Mid, 208 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
							  }
						}
					if(%Player.GetMissionFlag(%Mid, 1150) $= %Npc.GetDataID() )
						{
							%Player.SetMissionFlag(%Mid, 1350, 1); 				//设置,目标触发当前数量
							%Player.UpdateMission(%Mid);							//更新任务信息
						}
				}

			if((%State > 0)&&(%Mid == 10123))
				{
					%MidState = GetSubStr(%State,0,3);

					if( (%MidState > 200)&&(%MidState < 209) )
						{
							if(%MidState == 201){%NpcX = 410107001;%Map = 101101;}//"昆仑宗(圣)"昆仑古虚
							if(%MidState == 202){%NpcX = 410207001;%Map = 100601;}//"金禅寺(佛)"极乐西天
							if(%MidState == 203){%NpcX = 410307001;%Map = 100701;}//"蓬莱派(仙)"蓬莱仙境
							if(%MidState == 204){%NpcX = 410407001;%Map = 100501;}//"飞花谷(精)"神木林
							if(%MidState == 205){%NpcX = 410507001;%Map = 100901;}//"九幽教(鬼)"幽冥鬼域
							if(%MidState == 206){%NpcX = 410607001;%Map = 101001;}//"山海宗(怪)"双生山
							if(%MidState == 207){%NpcX = 410707001;%Map = 100801;}//"幻灵宫(妖)"醉梦冰池
							if(%MidState == 208){%NpcX = 410807001;%Map = 100401;}//"天魔门(魔)"落夕渊

							%Conv.SetType(4);
							%Player.SetMissionFlag(%Mid, 1150, %NpcX);		//设置,目标编号
							%Player.SetMissionFlag(%Mid, 1250, 1);				//设置,目标触发总数
							%Player.SetMissionFlag(%Mid, 1350, 0); 				//设置,目标触发当前数量
							%Player.UpdateMission(%Mid);							//更新任务信息
							GoToNextMap_Normal(%Player, %Map);
						}
					if(%MidState == 998)
						{
							%Conv.SetText(%MidState @ %Mid);	//咻咻，地图未制作完成
							%Conv.AddOption(4 , 0 );		//返回
						}
				}
		}
}

function Mission_Special_10123_2(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
			  {
			 	 	if(%Npc.GetDataID() $= "410107001"){%Conv.AddOption(41010, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 301 @ %Mid);}//昆仑宗(圣)
			 	 	if(%Npc.GetDataID() $= "410207001"){%Conv.AddOption(41020, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 302 @ %Mid);}//金禅寺(佛)
			 	 	if(%Npc.GetDataID() $= "410307001"){%Conv.AddOption(41030, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 303 @ %Mid);}//蓬莱派(仙)
			 	 	if(%Npc.GetDataID() $= "410407001"){%Conv.AddOption(41040, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 304 @ %Mid);}//飞花谷(精)
			 	 	if(%Npc.GetDataID() $= "410507001"){%Conv.AddOption(41050, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 305 @ %Mid);}//九幽教(鬼)
			 	 	if(%Npc.GetDataID() $= "410607001"){%Conv.AddOption(41060, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 306 @ %Mid);}//山海宗(怪)
			 	 	if(%Npc.GetDataID() $= "410707001"){%Conv.AddOption(41070, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 307 @ %Mid);}//幻灵宫(妖)
			 	 	if(%Npc.GetDataID() $= "410807001"){%Conv.AddOption(41080, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 308 @ %Mid);}//天魔门(魔)					
			 }
				
			if((%State > 0)&&(%Mid == 10123))
				{
					%MidState = GetSubStr(%State,0,3);
					
					if(%MidState == 200)
						{
							%ItemAdd = %Player.PutItem(105100102,1);
							%ItemAdd = %Player.AddItem();
							
							if(!%ItemAdd)
								{
									SendOneChatMessage(0,"<t>背包满了</t>",%Player);
									SendOneScreenMessage(2,"背包满了", %Player);
				
									return;
								}
								
							AddMissionReward(%Player, 10123, 0);	
							if(%Npc.GetDataID() $= "410107001"){%Player.SetFamily(1);}//"昆仑宗(圣)
							if(%Npc.GetDataID() $= "410207001"){%Player.SetFamily(2);}//"金禅寺(佛)
							if(%Npc.GetDataID() $= "410307001"){%Player.SetFamily(3);}//"蓬莱派(仙)
							if(%Npc.GetDataID() $= "410407001"){%Player.SetFamily(4);}//"飞花谷(精)
							if(%Npc.GetDataID() $= "410507001"){%Player.SetFamily(5);}//"九幽教(鬼)
							if(%Npc.GetDataID() $= "410607001"){%Player.SetFamily(6);}//"山海宗(怪)
							if(%Npc.GetDataID() $= "410707001"){%Player.SetFamily(7);}//"幻灵宫(妖)
							if(%Npc.GetDataID() $= "410807001"){%Player.SetFamily(8);}//"天魔门(魔)
						}
						
					if( (%MidState > 300)&&(%MidState < 309) )
						{  
							if(%MidState == 301) //圣
								if(%Player.GetSex() == 1)
								{
									%Conv.AddOption(202 @ %Mid, 402 @ %Mid);//传送至门宗地图"金禅寺(佛)"极乐西天
									%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
									%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
									%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
									%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
									%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
							  }
							else
								{
									%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
									%Conv.AddOption(204 @ %Mid, 404 @ %Mid);//传送至门宗地图"飞花谷(精)"神木林
									%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
									%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
									%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
									%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
							  }
							  
						  if(%MidState == 302)//佛
								if(%Player.GetSex() == 1)
								{
									%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
									%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
									%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
									%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
									%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
									%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
							  }
							else
								{
									%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
									%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
									%Conv.AddOption(204 @ %Mid, 404 @ %Mid);//传送至门宗地图"飞花谷(精)"神木林
									%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
									%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
									%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
									%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
							  }
							  
							  if(%MidState == 303)//仙
									if(%Player.GetSex() == 1)
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//传送至门宗地图"金禅寺(佛)"极乐西天
										%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
										%Conv.AddOption(204 @ %Mid, 404 @ %Mid);//传送至门宗地图"飞花谷(精)"神木林
										%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
								  }
								  
							if(%MidState == 304)//精
								if(%Player.GetSex() == 1)
								  {
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//传送至门宗地图"金禅寺(佛)"极乐西天
                    %Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
										%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
										%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
										%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
								  }		  		
								  
							if(%MidState == 305)//鬼
								if(%Player.GetSex() == 1)
								  {
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//传送至门宗地图"金禅寺(佛)"极乐西天
                    %Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
										%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
                    %Conv.AddOption(204 @ %Mid, 404 @ %Mid);//传送至门宗地图"飞花谷(精)"神木林
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
								  }
						 if(%MidState == 306)//怪
								if(%Player.GetSex() == 1)
								  {
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//传送至门宗地图"金禅寺(佛)"极乐西天
                    %Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
                    %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
										%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
                    %Conv.AddOption(204 @ %Mid, 404 @ %Mid);//传送至门宗地图"飞花谷(精)"神木林
									  %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
								  }
						if(%MidState == 307)//妖
								if(%Player.GetSex() == 1)
								  {
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//传送至门宗地图"金禅寺(佛)"极乐西天
                    %Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
                    %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
										%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
                    %Conv.AddOption(204 @ %Mid, 404 @ %Mid);//传送至门宗地图"飞花谷(精)"神木林
									  %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//传送至门宗地图"天魔门(魔)"落夕渊
								  }
								  
						if(%MidState == 308)//魔
								if(%Player.GetSex() == 1)
								  {
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//传送至门宗地图"金禅寺(佛)"极乐西天
                    %Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
                    %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//传送至门宗地图"昆仑宗(圣)"昆仑古虚
										%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
                    %Conv.AddOption(204 @ %Mid, 404 @ %Mid);//传送至门宗地图"飞花谷(精)"神木林
									  %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//传送至门宗地图"九幽教(鬼)"幽冥鬼域
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//传送至门宗地图"山海宗(怪)"双生山
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//传送至门宗地图"幻灵宫(妖)"醉梦冰池
								  }	
						}  
						if( (%MidState > 400)&&(%MidState < 409) )
							 {
									if(%MidState == 401){%NpcX = 410107001;%Map = 101101;}//"昆仑宗(圣)"昆仑古虚
									if(%MidState == 402){%NpcX = 410207001;%Map = 100601;}//"金禅寺(佛)"极乐西天
									if(%MidState == 403){%NpcX = 410307001;%Map = 100701;}//"蓬莱派(仙)"蓬莱仙境
									if(%MidState == 404){%NpcX = 410407001;%Map = 100501;}//"飞花谷(精)"神木林
									if(%MidState == 405){%NpcX = 410507001;%Map = 100901;}//"九幽教(鬼)"幽冥鬼域
									if(%MidState == 406){%NpcX = 410607001;%Map = 101001;}//"山海宗(怪)"双生山
									if(%MidState == 407){%NpcX = 410707001;%Map = 100801;}//"幻灵宫(妖)"醉梦冰池
									if(%MidState == 408){%NpcX = 410807001;%Map = 100401;}//"天魔门(魔)"落夕渊
					
									%Conv.SetType(4);
									%Player.SetMissionFlag(%Mid, 1150, %NpcX);		//设置,目标编号
									%Player.SetMissionFlag(%Mid, 1250, 1);				//设置,目标触发总数
									%Player.SetMissionFlag(%Mid, 1350, 0); 				//设置,目标触发当前数量
									%Player.UpdateMission(%Mid);							//更新任务信息
									GoToNextMap_Normal(%Player, %Map);
							 }
						if(%MidState == 998)
							{
								%Conv.SetText(%MidState @ %Mid);	//咻咻，地图未制作完成
								%Conv.AddOption(4 , 0 );		//返回
							}

			   }

	  }												
}


function Mission_Special_10124(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%State == 0)
		{
			//接任务
			if(%Player.IsFinishedMission(%Mid - 1))
				if(!%Player.IsAcceptedMission(%Mid))
					if(!%Player.IsFinishedMission(%Mid))
						{
							%Yes = 0;

							if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "410107001") ){%Yes = 1;} //圣NPC
							if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "410207001") ){%Yes = 1;} //佛NPC
							if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "410307001") ){%Yes = 1;} //仙NPC
							if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "410407001") ){%Yes = 1;} //精NPC
							if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "410507001") ){%Yes = 1;} //鬼NPC
							if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "410607001") ){%Yes = 1;} //怪NPC
							if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "410707001") ){%Yes = 1;} //妖NPC
							if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "410807001") ){%Yes = 1;} //魔NPC

							if(%Yes == 1)
								%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//显示接任务选择
						}

			//交任务
			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					{
						%No = 0;

						if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011002") ){%No = 1;} //圣NPC
						if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006002") ){%No = 1;} //佛NPC
						if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007002") ){%No = 1;} //仙NPC
						if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005002") ){%No = 1;} //精NPC
						if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009002") ){%No = 1;} //鬼NPC
						if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010002") ){%No = 1;} //怪NPC
						if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008002") ){%No = 1;} //妖NPC
						if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004002") ){%No = 1;} //魔NPC

						if(%No == 1)
							%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//显示交任务选择
					}
		}

	if((%State > 0)&&(%Mid == 10124))
		{
			%MidState = GetSubStr(%State,0,3);

			switch(%MidState)
			{
				case 110:
					if(%Player.IsFinishedMission(%Mid - 1))
						if(!%Player.IsAcceptedMission(%Mid))
							if(!%Player.IsFinishedMission(%Mid))
								{
									AddMissionAccepted(%Player, %Mid);	//设置任务相关信息
									%Conv.SetType(4);
								}

				case 910:
					if(%Player.IsAcceptedMission(%Mid))
						if(!%Player.IsFinishedMission(%Mid))
							{
								AddMissionReward(%Player, %Mid, %Param);	//设置任务奖励

								NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//执行Npc点击触发的脚本
								
								if(%Mid == 10124)
									{
											%Conv.SetType(4);

										//开启技能选择界面
										if( (%Player.GetClasses(0) == 0)||(%Player.GetClasses(1) == 0) )
											{
												%A = %Player.GetClasses(0);
												%B = %Player.GetClasses(1);
												%C = %Player.GetClasses(2);
												%D = %Player.GetClasses(3);
												OpenSkillSelect(%Player,%A,%B,%C,%D);
											}
										//开启技能学习界面
										if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
											{
												%A = %Player.GetClasses(0);
												%B = %Player.GetClasses(1);
												%C = %Player.GetClasses(2);
												%D = %Player.GetClasses(3);
												%E = %Player.GetClasses(4);
												OpenSkillStudy(%Player,%A,%B,%C,%D,%E);
											}
									}
							}
			 }
		}
}

function Mission_Special_10125(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%State == 0)
		{
			//接任务
			if(%Player.IsFinishedMission(%Mid - 1))
				if(!%Player.IsAcceptedMission(%Mid))
					if(!%Player.IsFinishedMission(%Mid))
						{
							%Yes = 0;

							if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011002") ){%Yes = 1;} //圣NPC
							if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006002") ){%Yes = 1;} //佛NPC
							if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007002") ){%Yes = 1;} //仙NPC
							if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005002") ){%Yes = 1;} //精NPC
							if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009002") ){%Yes = 1;} //鬼NPC
							if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010002") ){%Yes = 1;} //怪NPC
							if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008002") ){%Yes = 1;} //妖NPC
							if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004002") ){%Yes = 1;} //魔NPC

							if(%Yes == 1)
								%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//显示接任务选择
						}

			//交任务
			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					{
						%No = 0;

						if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011002") ){%No = 1;} //圣NPC
						if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006002") ){%No = 1;} //佛NPC
						if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007002") ){%No = 1;} //仙NPC
						if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005002") ){%No = 1;} //精NPC
						if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009002") ){%No = 1;} //鬼NPC
						if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010002") ){%No = 1;} //怪NPC
						if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008002") ){%No = 1;} //妖NPC
						if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004002") ){%No = 1;} //魔NPC

						if((%No == 1)&&(%Player.GetMissionFlag(%Mid, 3300) == 3))
							%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//显示交任务选择
					}
		}

	if((%State > 0)&&(%Mid == 10125))
		{
			%MidState = GetSubStr(%State,0,3);

			switch(%MidState)
			{
				case 110:
					if(%Player.IsFinishedMission(%Mid - 1))
						if(!%Player.IsAcceptedMission(%Mid))
							if(!%Player.IsFinishedMission(%Mid))
								{
									if( (%Player.GetFamily() == 1)){%NpcX = 410100001;}
									if( (%Player.GetFamily() == 2)){%NpcX = 410200001;}
									if( (%Player.GetFamily() == 3)){%NpcX = 410300001;}
									if( (%Player.GetFamily() == 4)){%NpcX = 410400001;}
									if( (%Player.GetFamily() == 5)){%NpcX = 410500001;}
									if( (%Player.GetFamily() == 6)){%NpcX = 410600001;}
									if( (%Player.GetFamily() == 7)){%NpcX = 410700001;}
									if( (%Player.GetFamily() == 8)){%NpcX = 410800001;}
										
				           echo("%NpcX ="@%NpcX);
				           %Player.SetMissionFlag(%Mid, 3100, %NpcX);		//设置,目标编号
									 %Player.SetMissionFlag(%Mid, 3200, 3);				//设置,目标触发总数
									 %Player.SetMissionFlag(%Mid, 3300, 0); 				//设置,目标触发当前数量 
					 				 %Player.UpdateMission(%Mid);							//更新任务信息
					 				 
									AddMissionAccepted(%Player, %Mid);	//设置任务相关信息
									%Conv.SetType(4);
								}

				case 910:
					if(%Player.IsAcceptedMission(%Mid))
						if(!%Player.IsFinishedMission(%Mid))
							{
								AddMissionReward(%Player, %Mid, %Param);	//设置任务奖励

								NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//执行Npc点击触发的脚本
							}
			}
		}
}

function Mission_Special_10126(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%State == 0)
		{
			//接任务
			if(%Player.IsFinishedMission(%Mid - 1))
				if(!%Player.IsAcceptedMission(%Mid))
					if(!%Player.IsFinishedMission(%Mid))
						{
							%Yes = 0;

							if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011002") ){%Yes = 1;} //圣NPC
							if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006002") ){%Yes = 1;} //佛NPC
							if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007002") ){%Yes = 1;} //仙NPC
							if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005002") ){%Yes = 1;} //精NPC
							if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009002") ){%Yes = 1;} //鬼NPC
							if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010002") ){%Yes = 1;} //怪NPC
							if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008002") ){%Yes = 1;} //妖NPC
							if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004002") ){%Yes = 1;} //魔NPC

							if(%Yes == 1)
								%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//显示接任务选择
						}

			//交任务
			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					{
						%No = 0;

						if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011006") ){%No = 1;} //圣NPC
						if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006006") ){%No = 1;} //佛NPC
						if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007006") ){%No = 1;} //仙NPC
						if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005006") ){%No = 1;} //精NPC
						if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009006") ){%No = 1;} //鬼NPC
						if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010006") ){%No = 1;} //怪NPC
						if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008006") ){%No = 1;} //妖NPC
						if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004006") ){%No = 1;} //魔NPC

						if(%No == 1)
							%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//显示交任务选择
					}
		}

	if((%State > 0)&&(%Mid == 10126))
		{
			%MidState = GetSubStr(%State,0,3);

			switch(%MidState)
			{
				case 110:
					if(%Player.IsFinishedMission(%Mid - 1))
						if(!%Player.IsAcceptedMission(%Mid))
							if(!%Player.IsFinishedMission(%Mid))
								{
									AddMissionAccepted(%Player, %Mid);	//设置任务相关信息
                  %Conv.SetType(4);
									echo("任务接受部分 = "@%Mid);
								}

				case 910:
					if(%Player.IsAcceptedMission(%Mid))
						if(!%Player.IsFinishedMission(%Mid))
							{
								AddMissionReward(%Player, %Mid, %Param);	//设置任务奖励

								NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//执行Npc点击触发的脚本
							}
			}
		}
}


function Mission_Special_10127(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%State == 0)
		{
			//接任务
			if(%Player.IsFinishedMission(%Mid - 1))
				if(!%Player.IsAcceptedMission(%Mid))
					if(!%Player.IsFinishedMission(%Mid))
						{
							%Yes = 0;

							if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011006") ){%Yes = 1;} //圣NPC
							if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006006") ){%Yes = 1;} //佛NPC
							if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007006") ){%Yes = 1;} //仙NPC
							if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005006") ){%Yes = 1;} //精NPC
							if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009006") ){%Yes = 1;} //鬼NPC
							if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010006") ){%Yes = 1;} //怪NPC
							if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008006") ){%Yes = 1;} //妖NPC
							if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004006") ){%Yes = 1;} //魔NPC

							if(%Yes == 1)
								%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//显示接任务选择
						}

			//交任务
			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					{
						%No = 0;

						if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "400001038") ){%No = 1;} //圣NPC
						if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "400001044") ){%No = 1;} //佛NPC
						if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "400001039") ){%No = 1;} //仙NPC
						if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "400001042") ){%No = 1;} //精NPC
						if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "400001041") ){%No = 1;} //鬼NPC
						if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "400001040") ){%No = 1;} //怪NPC
						if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "400001043") ){%No = 1;} //妖NPC
						if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "400001045") ){%No = 1;} //魔NPC

						if(%No == 1)
							%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//显示交任务选择
					}
		}

	if((%State > 0)&&(%Mid == 10127))
		{
			%MidState = GetSubStr(%State,0,3);

			switch(%MidState)
			{
				case 110:
					if(%Player.IsFinishedMission(%Mid - 1))
						if(!%Player.IsAcceptedMission(%Mid))
							if(!%Player.IsFinishedMission(%Mid))
								{
									AddMissionAccepted(%Player, %Mid);	//设置任务相关信息
									%Conv.SetType(4);
								}

				case 910:
					if(%Player.IsAcceptedMission(%Mid))
						if(!%Player.IsFinishedMission(%Mid))
							{
								AddMissionReward(%Player, %Mid, %Param);	//设置任务奖励

								NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//执行Npc点击触发的脚本
							}
			}
		}
}


function Mission_Special_10128(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%State == 0)
		{
			//接任务
			if(%Player.IsFinishedMission(%Mid - 1))
				if(!%Player.IsAcceptedMission(%Mid))
					if(!%Player.IsFinishedMission(%Mid))
						{
							%Yes = 0;

							if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "400001038") ){%Yes = 1;} //圣NPC
							if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "400001044") ){%Yes = 1;} //佛NPC
							if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "400001039") ){%Yes = 1;} //仙NPC
							if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "400001042") ){%Yes = 1;} //精NPC
							if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "400001041") ){%Yes = 1;} //鬼NPC
							if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "400001040") ){%Yes = 1;} //怪NPC
							if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "400001043") ){%Yes = 1;} //妖NPC
							if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "400001045") ){%Yes = 1;} //魔NPC

							if(%Yes == 1)
								%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//显示接任务选择
						}

			//交任务
			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Npc.GetDataID() $= "400001059")
						%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//显示交任务选择
		}

	if((%State > 0)&&(%Mid == 10128))
		{
			%MidState = GetSubStr(%State,0,3);

			switch(%MidState)
			{
				case 110:
					if(%Player.IsFinishedMission(%Mid - 1))
						if(!%Player.IsAcceptedMission(%Mid))
							if(!%Player.IsFinishedMission(%Mid))
								{
									AddMissionAccepted(%Player, %Mid);	//设置任务相关信息
									%Conv.SetType(4);
								}

				case 910:
					if(%Player.IsAcceptedMission(%Mid))
						if(!%Player.IsFinishedMission(%Mid))
							{
								AddMissionReward(%Player, %Mid, %Param);	//设置任务奖励
								//打开装备鉴定界面

								%Conv.SetType(4);
								OpenIdentify(%Player);
							}
			}
		}
}

function Mission_Special_10140(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10140;
	
	if(%State == 0)
		{
			if(%Player.IsFinishedMission(10128))
				if(%Player.GetLevel() < 11)
						  %Conv.AddOption(100 @ %Mid, 120 @ %Mid);//显示接任务选择
		}
		
	if(%State > 0)
		{
			%MidState = GetSubStr(%State,0,3);
			%Pet = %Player.GetSpawnedPet();
					
			if(%MidState == 120)
				{
				   %Conv.SetText(%Mid);	
				}
		}  
}

function Mission_Special_10153(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10153;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//我已经准备好去万灵城了
					%Conv.AddOption(300 @ %Mid, -1 );	//稍等片刻，我还需修整片刻
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						//GoToNextMap_Normal(%Player, 100104);
						FlyMeToTheMoon(%Player, 11010101);
				}
		}
}


function Mission_Special_10156(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10156;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//我准备好去找夏梦狼了
					%Conv.AddOption(300 @ %Mid, -1 );	//稍等片刻，我还需修整片刻
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						//GoToNextMap_Normal(%Player, 110104);
						FlyMeToTheMoon(%Player, 10010104);
				}
		}
}


function Mission_Special_10161(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10161;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//我准备好了
					%Conv.AddOption(300 @ %Mid, -1 );	//稍等片刻，我还需修整片刻
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_CopyMap(%Player,1302);
						%Conv.SetType(4);
				}
		}
}


function Mission_Special_10186(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10186;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//尚未对话
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//夫人请讲

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							%Conv.SetText(300 @ %Mid);	//对白
							%Conv.AddOption(4 , 0 );		//返回

							%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//设置,目标触发当前数量
						}
				}
		}
}

function Mission_Special_10233(%Npc, %Player, %State, %Conv, %Param)
{
  %Mid = 10233;

	if(%Player.IsAcceptedMission(%Mid))
		{
				if(%State == 0)
					if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//尚未对话
						%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//婆婆请讲
	
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 200)
							{
								%Conv.SetText(201 @ %Mid);	//对白
								%Conv.AddOption(202 @ %Mid, 203 @ %Mid );	
							}
						if(%MidState ==203)
							{
								%Conv.SetText(203 @ %Mid);	//对白
								%Conv.AddOption(204 @ %Mid, 205 @ %Mid );	
								
							}
						if(%MidState ==205)
							{
								%Conv.SetText(205 @ %Mid);	//对白
								%Conv.AddOption(4 , 0 );		//返回
								
								%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//设置,目标触发当前数量
							}
					}
		  }
}

function Mission_Special_10303(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10303;
	%PlayerID = %Player.GetPlayerID();

	if(%State == 0)
		{	
				if(%Npc.GetDataID() $= "401102121")
					{
					
						%Conv.SetText(200 @ %Mid);	//蛇洞一
					
					}
					
				if(%Npc.GetDataID() $= "401102122")
					{
						
					  %Conv.SetText(201 @ %Mid);	//蛇洞二
			
				  }
				  
				if(%Npc.GetDataID() $= "401102123")	
					{	
				 	
						%Conv.SetText(202 @ %Mid);	//蛇洞三
				
						if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020093)==0))
							{
									%ItemAdd = %Player.PutItem(108020093, 1);
									%ItemAdd = %Player.AddItem();
									
									if(!%ItemAdd)
								    {
											SendOneChatMessage(0,"<t>背包满了</t>",%Player);
											SendOneScreenMessage(2,"背包满了", %Player);
											%Conv.SetType(4);
											return;
								    }	
							} 
          }
     }   
}

function Mission_Special_10305(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10305;
	%PlayerID = %Player.GetPlayerID();
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
			{	
				if(%player.GetItemCount(108020095)==1){%Conv.AddOption(200 @ %Mid, 200 @ %Mid );}
				if(%player.GetItemCount(108020096)==1){%Conv.AddOption(201 @ %Mid, 201 @ %Mid );}
				if(%player.GetItemCount(108020097)==1){%Conv.AddOption(202 @ %Mid, 202 @ %Mid );}
		    if((%player.GetItemCount(108020095)==0)&&(%player.GetItemCount(108020096)==0)&&(%player.GetItemCount(108020097)==0)&&(%player.GetItemCount(108020118)==0))
		    {
		    	%Conv.SetText(203 @ %Mid);
		    	%Conv.AddOption(204 @ %Mid, 204 @ %Mid );
		    }
		    
		  }  	
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 200)
							{
								%Conv.SetText(210 @ %Mid);	//对白
								%Conv.AddOption(4,0 );		//返回
								
								%ItemAdd = %Player.PutItem(108020095, -1);
								%ItemAdd = %Player.AddItem();
							}
						if(%MidState == 201)	
							{
								%Conv.SetText(220 @ %Mid);	//对白
								%Conv.AddOption(4,0 );		//返回	
								
								%ItemAdd = %Player.PutItem(108020096, -1);
								%ItemAdd = %Player.AddItem();
							}
						if(%MidState == 202)
							{
								%Conv.SetText(230 @ %Mid);	//对白
								%Conv.AddOption(4,0 );		//返回
								
								%ItemAdd = %Player.PutItem(108020097, -1);
								%ItemAdd = %Player.AddItem();
							}
						if(%MidState == 204)
							{
								%ItemAdd = %Player.PutItem(108020118, 1);
								%ItemAdd = %Player.AddItem();
								
									if(!%ItemAdd)
								  {
										SendOneChatMessage(0,"<t>背包满了</t>",%Player);
										SendOneScreenMessage(2,"背包满了",%Player);
										
										return;
								  }
								  %Conv.SetType(4);
						  }
					}
		  }				
}

function Mission_Special_10316(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10316;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//尚未对话
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//村长请讲

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							%Conv.SetText(201 @ %Mid);	//对白
							%Conv.AddOption(4 , 0 );		//结束对话
							%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//设置,目标触发当前数量
						}
				}
		}
}	

function Mission_Special_10320(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10320;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//我准备好了
					%Conv.AddOption(300 @ %Mid, -1 );	//稍等片刻，我还需修整片刻
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_Normal(%Player, 110204);
				}
		}
}

function Mission_Special_10321(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10321;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//我准备好了
					%Conv.AddOption(300 @ %Mid, -1 );	//稍等片刻，我还需修整片刻
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_CopyMap( %Player, 1304 );
				}
		}
}

function Mission_Special_10527_1(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10527;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			 if(%State == 0)
				{
					if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//尚未对话
					%Conv.AddOption(201 @ %Mid, 200 @ %Mid );	//评酒
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					
					if(%MidState == 200)
						{	
							%Conv.SetText(200 @ %Mid);	//对白
							%Conv.AddOption(1,-1);		//结束对话		
							%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//设置,目标触发当前数量

						}

				}
		}
}

function Mission_Special_10527_2(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10527;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			
			if(%State == 0)
				{
					if(%Player.GetMissionFlag(%Mid, 1351) == 0)		
					%Conv.AddOption(201 @ %Mid, 300 @ %Mid );	//评酒
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 300)
						{	
							%Conv.SetText(300 @ %Mid);	//对白
							%Conv.AddOption(1,-1);		//结束对话	
							%Player.SetMissionFlag(%Mid, 1351, 1, true); //设置,目标触发当前数量
						}				
				}
		}
}

function Mission_Special_10527_3(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10527;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			
			if(%State == 0)
				{
					if(%Player.GetMissionFlag(%Mid, 1352) == 0)		
					%Conv.AddOption(201 @ %Mid, 400 @ %Mid );	//评酒
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 400)
						{	
						
							%Conv.SetText(400 @ %Mid);	//对白
							%Conv.AddOption(1,-1);		//结束对话	
							%Player.SetMissionFlag(%Mid, 1352, 1, true); //设置,目标触发当前数量
					
						}			
				}
		}
}


function Mission_Special_10545(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10545;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//我准备好了
					%Conv.AddOption(300 @ %Mid, -1 );	//稍等片刻，我还需修整片刻
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_Normal(%Player, 110306);
				}
		}
}

function Mission_Special_10580(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10580;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//我准备好了
					%Conv.AddOption(300 @ %Mid, -1 );	//稍等片刻，我还需修整片刻
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_Normal(%Player, 110307);
				}
		}
}	

function Mission_Special_10584(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10584;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//我准备好了
					%Conv.AddOption(300 @ %Mid, -1 );	//稍等片刻，我还需修整片刻
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_CopyMap( %Player, 1305 );
				}
		}
}

function Mission_Special_10585(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid1 = 10585;
	%Mid2 = 10586;
	%Mid3 = 10587;
	%Mid4 = 10588;
  %Mid5 = 10589;
  
	if((%Player.IsAcceptedMission(%Mid1))&&(%Player.GetMissionFlag(%Mid1, 3300) == 0))	
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid1, 200 @ %Mid1 );	
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							SpNewNpc3(%Player, 411101016, "-5.75982 -9.61186 110.746", %CopyMapID,0,"2 2 2");
							%Conv.SetType(4);
						}
				}
		}
		
	if((%Player.IsAcceptedMission(%Mid2))&&(%Player.GetMissionFlag(%Mid2, 3300) == 0))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid2, 200 @ %Mid2 );	
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
					  {
							SpNewNpc3(%Player, 410600007, "17.6165 -9.62924 110.756", %CopyMapID,0,"2 2 2");
							%Conv.SetType(4);
					  }
				}
		}
		
			
	if((%Player.IsAcceptedMission(%Mid3))&&(%Player.GetMissionFlag(%Mid3, 3300) == 0))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid3, 200 @ %Mid3 );	
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							SpNewNpc3(%Player, 410701041, "5.98371 10.1313 110.749", %CopyMapID,0,"2 2 2");
							%Conv.SetType(4);
						}
				}
				
		}
		
	if((%Player.IsAcceptedMission(%Mid4))&&(%Player.GetMissionFlag(%Mid4, 3300) == 0))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid4, 200 @ %Mid4 );	
				}
	
			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							SpNewNpc3(%Player, 410400019, "-5.58389 3.83402 110.756", %CopyMapID,0,"2 2 2");
							%Conv.SetType(4);
						}
				}
					
		}
		
 if((%Player.IsAcceptedMission(%Mid5))&&(%Player.GetMissionFlag(%Mid5, 3300) == 0))
	 {
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid5, 200 @ %Mid5 );	
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
					  {
							SpNewNpc3(%Player, 410200007, "5.91351 -2.7875 111.028", %CopyMapID,0,"2 2 2");
		          %Conv.SetType(4);
            }
				}
			
	}
			
}

function Mission_Special_10704(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10704;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			
			if(%State == 0)
				{
					if(%Player.GetMissionFlag(%Mid, 2300) == 0)
						{		
							%Conv.AddOption(200 @ %Mid, 200 @ %Mid );
						}	
				}

			if(%State > 0)
				{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 200)
							{	
								%Conv.SetText(201 @ %Mid);	//对白	
								%Conv.AddOption(1,300 @ %Mid);		//结束对话
							}	
						
						if(%MidState == 300)
						  {	
								if($New_10704 ==0)
									{
		           			$New_10704 = SpNewNpc(%Player,410400022,0,0);
		           			%Player.SetMissionFlag(%Mid, 1350, 1, true); //设置,目标触发当前数量
		           			Schedule(60000, 0, "RemoveNpc_10704");
		           			%Conv.SetType(4);
									}
						
						  }
				 }
		}
}

function RemoveNpc_10704()
{
	$New_10704.SafeDeleteObject();
	$New_10704= 0;
}

function Mission_Special_10743(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10743;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//尚未对话
					{
						%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	
					}
					
			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							%Conv.SetText(300 @ %Mid);	//对白
							%Conv.AddOption(4,0);		//返回
							%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//设置,目标触发当前数量
						}
				}
		}
}

function Mission_Special_10761(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10761;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					if((%Player.GetMissionFlag(%Mid, 3300) == 0)&&(%Player.GetMissionFlag(%Mid, 3301) == 0))
						{
							%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	
				  	}
				}
				
			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							if($New_10761 ==0)
							  {
						       $New_10761 = SpNewNpc(%Player,401104012,0,0);
							     %Player.SetMissionFlag(%Mid, 1300, 1, true);
							     Schedule(3000, 0, "RemoveNpc_10761");
							  }
						}
				}
		}
}

function RemoveNpc_10761()
{
	SpNewNpc(%Player,410400049,0,0);
  SpNewNpc(%Player,410400051,0,0);
	$New_10761.SafeDeleteObject();
}

function Mission_Special_10907(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10907;

	if(%Player.IsAcceptedMission(%Mid))
		{
	    if(%State ==0)
	    	{
		       if(%Player.GetMissionFlag(%Mid, 1300) == 0)
		       	 {
				       	%Conv.SetText(170 @ %Mid);	//对白
				       	%Conv.AddOption(1,0);		//结束对话
				       	%Player.SetMissionFlag(%Mid, 1300, 1, true);
				        $New_552000049_1.SafeDeleteObject();
			       }
			     if(%Player.GetMissionFlag(%Mid, 1300) == 1)
			     	 {
			     		 %Conv.SetText(171 @ %Mid);	//对白
			     		 %Conv.AddOption(1,0);		//结束对话
				       %Player.SetMissionFlag(%Mid, 1300, 2, true);
				       $New_552000049_1.SafeDeleteObject();
				     } 
			     if(%Player.GetMissionFlag(%Mid, 1300) == 2)
			     	 {
			     		 %Conv.SetText(172 @ %Mid);	//对白
			     		 %Conv.AddOption(1,0);		//结束对话
				       %Player.SetMissionFlag(%Mid, 1300, 3, true);
				       $New_552000049_1.SafeDeleteObject();
				     } 
				      
				   if(%Player.GetMissionFlag(%Mid, 1300) == 3)
				   	 {
			     		 %Conv.SetText(173 @ %Mid);	//对白
			     		 %Conv.AddOption(1,0);		//结束对话
				       %Player.SetMissionFlag(%Mid, 1300, 4, true);
				       $New_552000049_1.SafeDeleteObject();
				     }  
				   if(%Player.GetMissionFlag(%Mid, 1300) == 4)
				     {
				     	 %Conv.SetText(174 @ %Mid);	//对白
				     	 %Conv.AddOption(1,0);		//结束对话
					     %Player.SetMissionFlag(%Mid, 1300, 5, true);
					     $New_552000049_1.SafeDeleteObject();	     	 
		         }
	       }
	   }
}

function Mission_Special_10908(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10908;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//尚未对话
				  {	
						%Conv.SetText(100 @ %Mid);	//对白
						%Conv.AddOption(101 @ %Mid, 101 @ %Mid );	
          }
          
			if(%State == 101)
				{
					%Conv.SetText(110 @ %Mid);	//对白
					%Conv.AddOption(111 @ %Mid, 111 @ %Mid );	
				}
				
			if(%State == 111)
				{
					%Conv.SetText(120 @ %Mid);	//对白
					%Conv.AddOption(121 @ %Mid, 121 @ %Mid );	
				}
			if(%State == 121)
	      {
			     %Player.SetMissionFlag(%Mid, 1350, 1, true); 				//设置,目标触发当前数量
			     %Conv.SetType(4);     
			  }
		}
		
}

function Mission_Special_10915(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10915;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1300) == 1)
					if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//尚未对话
						%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//村长请讲
	
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 200)
							{
								%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//设置,目标触发当前数量
							}
					}
		}
}						