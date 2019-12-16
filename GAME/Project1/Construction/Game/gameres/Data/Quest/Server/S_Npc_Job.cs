//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//本处脚本多为NPC功能性脚本，所有地图均会载入
//脚本接入后，依据NPC编号来分流显示
//==================================================================================


//所有功能最初的传入口
function Npc_Job(%Npc, %Player, %State , %Conv)
{
	//NPC功能分类：
	//6位：
		//首位1：商店编号
		//首位2：功能界面编号

	//4位：地图编号
	//5位：传送地图指令编号


	//特殊功能
//	switch$(%Npc.GetDataID())
//	{
//		case 400001101:		//新手功能引导	风阳子
//			NowAdd(%Npc, %Player, %State , %Conv);

//		case 400001104:		//郭钟
//			SpawnNewNpc(%Npc, %Player, %State , %Conv);

//	}
	//开店功能
	NpcOpenShop(%Npc, %Player, %State , %Conv);

	//开启功能性界面
	NpcOpenJob(%Npc, %Player, %State , %Conv);

	//开启功能性对话，无界面
	NpcCanDoJob(%Npc, %Player, %State , %Conv);

	//传送功能
	NpcTransportObject(%Npc, %Player, %State , %Conv);

	//飞行点功能
	NpcFly(%Npc, %Player, %State , %Conv);

	//新手各种帮助
	NewPlayerHelp(%Npc, %Player, %State , %Conv);

	//活动相关的功能
//	PlayGame(%Npc, %Player, %State , %Conv);
}

//新手各种帮助
function NewPlayerHelp(%Npc, %Player, %State , %Conv)
{
//--------------------------------------NewPlayerHelp--------State == 0----↓----------------------
	switch(%State)
	{
		case 0:
			//万灵城NPC
			switch$(%Npc.GetDataID())
			{
				case "400001062":		//新手帮助天书
					%Conv.AddOption(2210, 2210);	//怎样打开背包
					%Conv.AddOption(2270, 2270);	//怎样组成队伍
					%Conv.AddOption(2280, 2280);	//游戏热键一览
					%Conv.AddOption(2220, 2220);	//怎样查看任务
					%Conv.AddOption(2000, 2000);	//更多……

				case "400001101":%Conv.AddOption(3400, 3400);	//新手功能引导	风阳子//门宗加入指南
				case "400001001":%Conv.AddOption(4201, 4201); //怎样学习制作药物	帮助
				case "400001002":%Conv.AddOption(4202, 4202); //怎样学习烹饪食物	帮助
				case "400001003":%Conv.AddOption(4203, 4203); //怎样学习铸造武器	帮助
				case "400001004":%Conv.AddOption(4204, 4204); //怎样学习制作衣盔	帮助
				case "400001005":%Conv.AddOption(4205, 4205); //怎样学习加工首饰	帮助
				case "400001007":
					%Conv.AddOption(4206, 4206); //怎样获得灵兽	帮助
					%Conv.AddOption(4207, 4207); //怎样喂食灵兽	帮助
				case "400001008":%Conv.AddOption(4208, 4208); //灵兽功能说明	帮助
				case "400001009":%Conv.AddOption(4209, 4209); //怎样获得骑乘	帮助
				case "400001011":%Conv.AddOption(4210, 4210); //怎样摆摊	帮助
				case "400001017":%Conv.AddOption(4211, 4211); //怎样挖矿	帮助
				case "400001018":%Conv.AddOption(4212, 4212); //怎样伐木	帮助
				case "400001019":%Conv.AddOption(4213, 4213); //怎样钓鱼	帮助
				case "400001020":%Conv.AddOption(4214, 4214); //怎样种植	帮助
				case "400001021":%Conv.AddOption(4215, 4215); //怎样采药	帮助
				case "400001022":%Conv.AddOption(4216, 4216); //怎样狩猎	帮助
				case "400001023":%Conv.AddOption(4217, 4217); //怎样铸造武器	帮助
				case "400001024":%Conv.AddOption(4218, 4218); //怎样制造衣盔	帮助
				case "400001025":%Conv.AddOption(4219, 4219); //怎样加工首饰	帮助
				case "400001026":%Conv.AddOption(4220, 4220); //怎样制作符咒	帮助
				case "400001027":%Conv.AddOption(4221, 4221); //怎样炼成丹药	帮助
				case "400001028":%Conv.AddOption(4222, 4222); //怎样烹饪食物	帮助
				case "400001029":%Conv.AddOption(4223, 4223); //怎样制造家具	帮助
				case "400001030":%Conv.AddOption(4224, 4224); //怎样饲养蛊	帮助
				case "400001046":%Conv.AddOption(4225, 4225); //如何扩展混元界	帮助
				case "400001049":%Conv.AddOption(4226, 4226); //怎样获得洞府	帮助
				case "400001059":%Conv.AddOption(4227, 4227); //鉴定的功能	帮助
				case "400001060":%Conv.AddOption(4228, 4228); //双倍经验的说明	帮助

				case "400001038":															//圣门宗接引者	磐龙使
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "400001039":															//仙门宗接引者	玄真
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "400001040":															//怪门宗接引者	雷啸天
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "400001041":															//鬼门宗接引者	殇魂
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "400001042":															//精门宗接引者	花舞
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "400001043":															//妖门宗接引者	王梦絮
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "400001044":															//佛门宗接引者	梵音伽蓝
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "400001045":															//魔门宗接引者	魔灵
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地

			}
			//月幽境NPC
			switch$(%Npc.GetDataID())
			{
				case "401002001":%Conv.AddOption(4201, 4201);	//怎样学习制作药物	帮助
				case "401002002":%Conv.AddOption(4202, 4202);	//怎样学习烹饪食物	帮助
				case "401002003":%Conv.AddOption(4203, 4203);	//怎样学习铸造武器	帮助
				case "401002004":%Conv.AddOption(4204, 4204);	//怎样学习制作衣盔	帮助
				case "401002005":%Conv.AddOption(4205, 4205);	//怎样学习加工首饰	帮助
				case "401002007":%Conv.AddOption(4206, 4206);	//怎样获得灵兽	帮助
				case "401002007":%Conv.AddOption(4207, 4207);	//怎样喂食灵兽	帮助
				case "401002008":%Conv.AddOption(4208, 4208);	//灵兽功能说明	帮助
				case "401002009":%Conv.AddOption(4209, 4209);	//怎样获得骑乘	帮助
				case "401002010":%Conv.AddOption(4210, 4210);	//怎样摆摊	帮助
				case "401002011":%Conv.AddOption(4211, 4211);	//怎样挖矿	帮助
				case "401002012":%Conv.AddOption(4212, 4212);	//怎样伐木	帮助
				case "401002013":%Conv.AddOption(4213, 4213);	//怎样钓鱼	帮助
				case "401002014":%Conv.AddOption(4214, 4214);	//怎样种植	帮助
				case "401002015":%Conv.AddOption(4215, 4215);	//怎样采药	帮助
				case "401002016":%Conv.AddOption(4216, 4216);	//怎样狩猎	帮助
				case "401002017":%Conv.AddOption(4217, 4217);	//怎样铸造武器	帮助
				case "401002018":%Conv.AddOption(4218, 4218);	//怎样制造衣盔	帮助
				case "401002019":%Conv.AddOption(4219, 4219);	//怎样加工首饰	帮助
				case "401002020":%Conv.AddOption(4220, 4220);	//怎样制作符咒	帮助
				case "401002021":%Conv.AddOption(4221, 4221);	//怎样炼成丹药	帮助
				case "401002022":%Conv.AddOption(4222, 4222);	//怎样烹饪食物	帮助
				case "401002023":%Conv.AddOption(4223, 4223);	//怎样制造家具	帮助
				case "401002024":%Conv.AddOption(4224, 4224);	//怎样饲养蛊	帮助
				case "401002037":%Conv.AddOption(4225, 4225); //如何扩展混元界	帮助
				case "401002040":%Conv.AddOption(4226, 4226); //怎样获得洞府	帮助
				case "401002042":%Conv.AddOption(4227, 4227); //鉴定的功能	帮助
				case "401002043":%Conv.AddOption(4228, 4228); //双倍经验的说明	帮助

				case "401002029":															//圣门宗接引者	迎龙使
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "401002030":															//仙门宗接引者	玄青
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "401002031":															//怪门宗接引者	炎烬
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "401002032":															//鬼门宗接引者	璃魄
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "401002033":															//精门宗接引者	花澈
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "401002034":															//妖门宗接引者	狐小媚
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "401002035":															//佛门宗接引者	笑弥勒
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地
				case "401002036":															//魔门宗接引者	幻魔
					%Conv.AddOption(4010, 4010);	//门宗背景
					%Conv.AddOption(4020, 4020);	//门宗特色
					%Conv.AddOption(4030, 4030);	//门宗坐骑
					%Conv.AddOption(4040, 4040);	//宗门圣地

				case "410107001":               //玄苍龙(圣）
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
					  		%Conv.AddOption(41010, 3800);

				case "410207001":               //大日如来（佛）
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(%Player.GetSex() == 1)
								if(!%Player.IsAcceptedMission(10123))
									%Conv.AddOption(41020, 3800);

				case "410307001":								//凌丹霞（仙）
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
					    	%Conv.AddOption(41030, 3800);

				case "410407001":								//花母（精）
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(%Player.GetSex() == 2)
								if(!%Player.IsAcceptedMission(10123))
									%Conv.AddOption(41040, 3800);

				case "410507001":								//鬼王（鬼）
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
								%Conv.AddOption(41050, 3800);

				case "410607001":								//苏石（怪）
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
								%Conv.AddOption(41060, 3800);

				case "410707001":								//夏姬（妖）
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
								%Conv.AddOption(41070, 3800);

				case "410807001":								//魔尊（魔）
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
								%Conv.AddOption(41080, 3800);

			}
//				deflt:
//					i (%Npc.GetDataID() $= "401011003")||			//圣门宗传送使
//						"(%Npc.GetDataID() $= "401007003")||     //仙门宗传送使
//							(%Npc.GetDataID() $= "401010003")||     //怪门宗传送使
//							(%Npc.GetDataID() $= "401009003")||     //鬼门宗传送使
//							(%Npc.GetDataID() $= "401005003")||     //精门宗传送使
//							(%Npc.GetDataID() $= "401008003")||     //妖门宗传送使
//							(%Npc.GetDataID() $= "401006003")||     //佛门宗传送使
//							(%Npc.GetDataID() $= "401004003") )     //魔门宗传送使
//						{
//							%Conv.SetText(3501);					//需要啥帮助不？
//							%Conv.AddOption(3500, 3500);	//查看本门宗的NPC位置
//							%Conv.AddOption(3510, 3510);	//我要去其他门宗
//							%Conv.AddOption(3520, 3520);	//我要回城去
//							%Conv.AddOption(3600, 0);	//门宗任务介绍
//							%Conv.AddOption(3700, 0);	//我要领取门宗任务
//							%Conv.AddOption(3530, 3530);	//没什么需要帮助的，谢谢
//						}
//					if( (%Npc.GetDataID() $= "401011002")||			//圣门宗技能使
//							(%Npc.GetDataID() $= "401007002")||     //仙门宗技能使
//							(%Npc.GetDataID() $= "401010002")||     //怪门宗技能使
//							(%Npc.GetDataID() $= "401009002")||     //鬼门宗技能使
//							(%Npc.GetDataID() $= "401005002")||     //精门宗技能使
//							(%Npc.GetDataID() $= "401008002")||     //妖门宗技能使
//							(%Npc.GetDataID() $= "401006002")||     //佛门宗技能使
//							(%Npc.GetDataID() $= "401004002") )     //魔门宗技能使
//						{
//							%Conv.AddOption(3200, 0);	//查看门宗技能
//						}
	}

//--------------------------------------NewPlayerHelp--------State == 0----↑----------------------
//--------------------------------------NewPlayerHelp-------State > 0----↓----------------------
	//“浏览天书”
	switch(%State)
	{
		case 2000:
//			%Conv.SetText(2100);	//天书内容
			%Conv.AddOption(2210, 2210);	//怎样打开背包
			%Conv.AddOption(2220, 2220);	//怎样查看任务
			%Conv.AddOption(2230, 2230);	//怎样查看地图
			%Conv.AddOption(2240, 2240);	//怎样领取双倍经验
			%Conv.AddOption(2250, 2250);	//怎样存储仓库
			%Conv.AddOption(2260, 2260);	//怎样添加好友
			%Conv.AddOption(2270, 2270);	//怎样组成队伍
			%Conv.AddOption(2280, 2280);	//游戏热键一览
			%Conv.AddOption(4 , 0 );//返回

		default:
			if( (%State >= 2210)&&(%State <= 2280) )
				{
					%Conv.SetText(%State + 1);
					%Conv.AddOption(4 , 2000);//返回
				}
	}

	//门宗帮助
	switch(%State)
	{
		case 3400:
			%Conv.SetText(3410);						//如若你拥有力量，那你希望用这力量制霸天下，还是维护苍生？
			%Conv.AddOption(3411 , 3420 );	//制霸天下，力量若不用来称霸天下那还能做什么
			%Conv.AddOption(3412 , 3450 );	//维护苍生，强者保护弱者乃天道
			%Conv.AddOption(4 , 0 );//返回

		case 3420:
			%Conv.SetText(%State);											//如果人生可以选择，你希望你的一生是跌宕起伏还是波澜不惊？
			%Conv.AddOption(%State + 1 , %State + 10 );	//跌宕起伏，人生怎能无大浪
			%Conv.AddOption(%State + 2 , %State + 20 );	//波澜不惊，春花哪堪几回霜，平淡即可
			%Conv.AddOption(4 , 3400 );//返回

		case 3450:
			%Conv.SetText(%State);											//不得已而战，你愿化为矛还是化为盾
			%Conv.AddOption(%State + 1 , %State + 10 );	//矛，我愿成为迎击敌人的力量
			%Conv.AddOption(%State + 2 , %State + 20 );	//盾，我愿成为抵挡攻击的盾
			%Conv.AddOption(4 , 3400 );//返回

		case 3430:
			%Conv.SetText(%State);									//如若战败，你是重整旗鼓还是战死沙场？
			%Conv.AddOption(%State + 1 , 3496 );		//重整旗鼓，胜败乃常事，君子报仇10年不晚
			%Conv.AddOption(%State + 2 , 3498 );		//战死沙场，只可战死，不可战败
			%Conv.AddOption(4 , 3420 );//返回

		case 3440:
			%Conv.SetText(%State);									//和平之时，你更喜欢修生养性还是习武强身？
			%Conv.AddOption(%State + 1 , 3493 );		//修生养性，提高自身内在修为才是大事
			%Conv.AddOption(%State + 2 , 3491 );		//习武强身，外在的强大是生存的基础
			%Conv.AddOption(4 , 3420 );//返回

		case 3460:
			%Conv.SetText(%State);									//攻有明暗之分，你更希望正面对决还是攻其不备
			%Conv.AddOption(%State + 1 , 3494 );		//正面对决，相信自己实力，正面对决有何不可
			%Conv.AddOption(%State + 2 , 3495 );		//攻其不备，能省一兵则省一兵，能省一份力则省一份力
			%Conv.AddOption(4 , 3450 );//返回

		case 3470:
			%Conv.SetText(%State);									//你愿在前方为强大的屏障还是在后方为坚实的砥柱
			%Conv.AddOption(%State + 1 , 3492 );		//屏障，这是保护想保护人的最直接体现
			%Conv.AddOption(%State + 2 , 3497 );		//砥柱，我愿默默无闻成为大家所能依靠的后援
			%Conv.AddOption(4 , 3450 );//返回

		case 3500:
			%Conv.SetText(3502);		//查看本门宗的NPC位置
			%Conv.AddOption(4 , 0 );//返回

		case 3510:
			%Conv.SetText(3519);		//我要去其他门宗
			if(GetZoneID() !$= "1011"){%Conv.AddOption(1011, 101101);}	//传送至门宗地图"昆仑宗(圣)"昆仑古虚
			if(GetZoneID() !$= "1007"){%Conv.AddOption(1006, 100601);}	//传送至门宗地图"金禅寺(佛)"极乐西天
			if(GetZoneID() !$= "1010"){%Conv.AddOption(1007, 100701);}	//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
			if(GetZoneID() !$= "1009"){%Conv.AddOption(1005, 100501);}	//传送至门宗地图"飞花谷(精)"神木林
			if(GetZoneID() !$= "1005"){%Conv.AddOption(1009, 100901);}	//传送至门宗地图"九幽教(鬼)"幽冥鬼域
			if(GetZoneID() !$= "1008"){%Conv.AddOption(1010, 101001);}	//传送至门宗地图"山海宗(怪)"双生山
			if(GetZoneID() !$= "1006"){%Conv.AddOption(1008, 100801);}	//传送至门宗地图"幻灵宫(妖)"醉梦冰池
			if(GetZoneID() !$= "1004"){%Conv.AddOption(1004, 100401);}	//传送至门宗地图"天魔门(魔)"落夕渊
			%Conv.AddOption(4 , 0 );//返回

		case 3520:
			%Conv.SetText(3529);		//我要回城去
			%Conv.AddOption(1001, 100103 );	//传送至万灵城
			%Conv.AddOption(4 , 0 );//返回

		case 3530:			//没什么需要帮助的，谢谢
			%Conv.SetType(4);

		case 3800:			//我要入门宗

			%ItemAdd = %Player.PutItem(105100102,1);
			%ItemAdd = %Player.AddItem();

			if(!%ItemAdd)
				{
					SendOneChatMessage(0,"<t>背包满了</t>",%Player);
					SendOneScreenMessage(2,"背包满了", %Player);

					return;
				}
			if(%Npc.GetDataID() $= "410107001"){%Player.SetFamily(1);}//"昆仑宗(圣)
			if(%Npc.GetDataID() $= "410207001"){%Player.SetFamily(2);}//"金禅寺(佛)
			if(%Npc.GetDataID() $= "410307001"){%Player.SetFamily(3);}//"蓬莱派(仙)
			if(%Npc.GetDataID() $= "410407001"){%Player.SetFamily(4);}//"飞花谷(精)
			if(%Npc.GetDataID() $= "410507001"){%Player.SetFamily(5);}//"九幽教(鬼)
			if(%Npc.GetDataID() $= "410607001"){%Player.SetFamily(6);}//"山海宗(怪)
			if(%Npc.GetDataID() $= "410707001"){%Player.SetFamily(7);}//"幻灵宫(妖)
			if(%Npc.GetDataID() $= "410807001"){%Player.SetFamily(8);}//"天魔门(魔)

			%Conv.SetType(4);	//关闭对话

		default:
			if( (%State > 3490)&&(%State < 3499) )
				{
					%Conv.SetText(%State);	//你选择的是XXX

					%Conv.AddOption(3401, 3400 );	//显示任务中途对话选择
					%Conv.AddOption(4 , 0 );//返回
				}

	}

	//门宗
	switch(%State)
	{
		case 4010:		//门宗背景
			if(%Npc.GetDataID() $= "400001038"){%Conv.SetText(4011);}     //圣门宗接引者	磐龙使
			if(%Npc.GetDataID() $= "400001044"){%Conv.SetText(4012);}     //佛门宗接引者	梵音伽蓝
			if(%Npc.GetDataID() $= "400001039"){%Conv.SetText(4013);}     //仙门宗接引者	玄真
			if(%Npc.GetDataID() $= "400001042"){%Conv.SetText(4014);}     //精门宗接引者	花舞
			if(%Npc.GetDataID() $= "400001041"){%Conv.SetText(4015);}     //鬼门宗接引者	殇魂
			if(%Npc.GetDataID() $= "400001040"){%Conv.SetText(4016);}     //怪门宗接引者	雷啸天
			if(%Npc.GetDataID() $= "400001043"){%Conv.SetText(4017);}     //妖门宗接引者	王梦絮
			if(%Npc.GetDataID() $= "400001045"){%Conv.SetText(4018);}     //魔门宗接引者	魔灵
			%Conv.AddOption(4,0);//返回

		case 4020:		//门宗特色
			if(%Npc.GetDataID() $= "400001038"){%Conv.SetText(4021);}     //圣门宗接引者	磐龙使
			if(%Npc.GetDataID() $= "400001044"){%Conv.SetText(4022);}     //佛门宗接引者	梵音伽蓝
			if(%Npc.GetDataID() $= "400001039"){%Conv.SetText(4023);}     //仙门宗接引者	玄真
			if(%Npc.GetDataID() $= "400001042"){%Conv.SetText(4024);}     //精门宗接引者	花舞
			if(%Npc.GetDataID() $= "400001041"){%Conv.SetText(4025);}     //鬼门宗接引者	殇魂
			if(%Npc.GetDataID() $= "400001040"){%Conv.SetText(4026);}     //怪门宗接引者	雷啸天
			if(%Npc.GetDataID() $= "400001043"){%Conv.SetText(4027);}     //妖门宗接引者	王梦絮
			if(%Npc.GetDataID() $= "400001045"){%Conv.SetText(4028);}     //魔门宗接引者	魔灵
			%Conv.AddOption(4,0);//返回

		case 4030:		//门宗坐骑
			if(%Npc.GetDataID() $= "400001038"){%Conv.SetText(4031);}     //圣门宗接引者	磐龙使
			if(%Npc.GetDataID() $= "400001044"){%Conv.SetText(4032);}     //佛门宗接引者	梵音伽蓝
			if(%Npc.GetDataID() $= "400001039"){%Conv.SetText(4033);}     //仙门宗接引者	玄真
			if(%Npc.GetDataID() $= "400001042"){%Conv.SetText(4034);}     //精门宗接引者	花舞
			if(%Npc.GetDataID() $= "400001041"){%Conv.SetText(4035);}     //鬼门宗接引者	殇魂
			if(%Npc.GetDataID() $= "400001040"){%Conv.SetText(4036);}     //怪门宗接引者	雷啸天
			if(%Npc.GetDataID() $= "400001043"){%Conv.SetText(4037);}     //妖门宗接引者	王梦絮
			if(%Npc.GetDataID() $= "400001045"){%Conv.SetText(4038);}     //魔门宗接引者	魔灵
			%Conv.AddOption(4,0);//返回

		case 4040:		//宗门圣地
			if(%Npc.GetDataID() $= "400001038"){%Conv.SetText(4041);}     //圣门宗接引者	磐龙使
			if(%Npc.GetDataID() $= "400001044"){%Conv.SetText(4042);}     //佛门宗接引者	梵音伽蓝
			if(%Npc.GetDataID() $= "400001039"){%Conv.SetText(4043);}     //仙门宗接引者	玄真
			if(%Npc.GetDataID() $= "400001042"){%Conv.SetText(4044);}     //精门宗接引者	花舞
			if(%Npc.GetDataID() $= "400001041"){%Conv.SetText(4045);}     //鬼门宗接引者	殇魂
			if(%Npc.GetDataID() $= "400001040"){%Conv.SetText(4046);}     //怪门宗接引者	雷啸天
			if(%Npc.GetDataID() $= "400001043"){%Conv.SetText(4047);}     //妖门宗接引者	王梦絮
			if(%Npc.GetDataID() $= "400001045"){%Conv.SetText(4048);}     //魔门宗接引者	魔灵
			%Conv.AddOption(4,0);//返回

	}

	//帮助
	if( (%State > 4200)&&(%State < 4300) )
		{
			%Conv.SetText(%State + 100);
			%Conv.AddOption(4,0);//返回
		}
//--------------------------------------NewPlayerHelp-------State > 0----↑----------------------
}

//开启商店
function NpcOpenShop(%Npc, %Player, %State , %Conv)
{
	switch(%State)
	{
		case 0:
			switch$(%Npc.GetDataID())
			{
				case "400001001":%Conv.AddOption(402051, 402051);	//打开药品商店
				case "400001002":%Conv.AddOption(402042, 402042);	//打开食物商店
				case "400001003":%Conv.AddOption(402011, 402011);	//打开武器商店
				case "400001004":%Conv.AddOption(402021, 402021);	//打开防具商店
				case "400001005":%Conv.AddOption(402031, 402031);	//打开首饰商店
//				case "400001006":%Conv.AddOption(      ,       );	//打开法宝商店
				case "400001007":%Conv.AddOption(402161, 402161);	//打开灵兽道具商店
//				case "400001007":%Conv.AddOption(      ,       );	//打开灵兽装备商店
//				case "400001009":%Conv.AddOption(      ,       );	//打开骑乘商店
				case "400001011":%Conv.AddOption(402041, 402041);	//打开杂货商店
				case "400001017":%Conv.AddOption(402111, 402111);	//打开工具商店
				case "400001018":%Conv.AddOption(402111, 402111);	//打开工具商店
				case "400001019":%Conv.AddOption(402111, 402111);	//打开工具商店
//				case "400001020":%Conv.AddOption(      ,       );	//打开工具商店
				case "400001021":%Conv.AddOption(402111, 402111);	//打开工具商店
				case "400001022":%Conv.AddOption(402111, 402111);	//打开工具商店
				case "400001023":%Conv.AddOption(402061, 402061);	//打开工具商店
				case "400001024":%Conv.AddOption(402071, 402071);	//打开工具商店
				case "400001025":%Conv.AddOption(402081, 402081);	//打开工具商店
//				case "400001026":%Conv.AddOption(      ,       );	//打开工具商店
				case "400001027":%Conv.AddOption(402141, 402141);	//打开工具商店
				case "400001028":%Conv.AddOption(402131, 402131);	//打开工具商店
				case "400001029":%Conv.AddOption(402151, 402151);	//打开工具商店
				case "400001030":%Conv.AddOption(402101, 402101);	//打开工具商店
//				case "400001031":%Conv.AddOption(      ,       );	//打开工具商店
				case "400001059":%Conv.AddOption(402181, 402181);	//鉴定符商店

				case "401011004":%Conv.AddOption(411171, 411171); //圣坐骑商人
				case "401006004":%Conv.AddOption(412171, 412171); //佛坐骑商人
				case "401007004":%Conv.AddOption(413171, 413171); //仙坐骑商人
				case "401005004":%Conv.AddOption(414171, 414171); //精坐骑商人
				case "401009004":%Conv.AddOption(415171, 415171); //鬼坐骑商人
				case "401010004":%Conv.AddOption(416171, 416171); //怪坐骑商人
				case "401008004":%Conv.AddOption(417171, 417171); //妖坐骑商人
				case "401004004":%Conv.AddOption(418171, 418171); //魔坐骑商人

				case "401002001":%Conv.AddOption(405051, 405051);	//打开药品商店
				case "401002002":%Conv.AddOption(405042, 405042);	//打开食物商店
				case "401002003":%Conv.AddOption(405011, 405011);	//打开武器商店
				case "401002004":%Conv.AddOption(405021, 405021);	//打开防具商店
				case "401002005":%Conv.AddOption(405031, 405031);	//打开首饰商店
				case "401002006":%Conv.AddOption(405191, 405191);	//打开法宝商店
				case "401002007":
					%Conv.AddOption(405161, 405161);	//打开灵兽道具商店
					%Conv.AddOption(405162, 405162);	//打开灵兽装备商店
				case "401002009":%Conv.AddOption(405171, 405171);	//打开骑乘商店
				case "401002010":%Conv.AddOption(405041, 405041);	//打开杂货商店
				case "401002011":%Conv.AddOption(405111, 405111);	//打开工具商店
				case "401002012":%Conv.AddOption(405111, 405111);	//打开工具商店
				case "401002013":%Conv.AddOption(405111, 405111);	//打开工具商店
				case "401002014":%Conv.AddOption(405111, 405111);	//打开工具商店
				case "401002015":%Conv.AddOption(405111, 405111);	//打开工具商店
				case "401002016":%Conv.AddOption(405111, 405111);	//打开工具商店
				case "401002017":%Conv.AddOption(405061, 405061);	//打开工具商店
				case "401002018":%Conv.AddOption(405071, 405071);	//打开工具商店
				case "401002019":%Conv.AddOption(405081, 405081);	//打开工具商店
				case "401002020":%Conv.AddOption(405201, 405201);	//打开工具商店
				case "401002021":%Conv.AddOption(405141, 405141);	//打开工具商店
				case "401002022":%Conv.AddOption(405131, 405131);	//打开工具商店
				case "401002023":%Conv.AddOption(405151, 405151);	//打开工具商店
				case "401002024":%Conv.AddOption(405101, 405101);	//打开工具商店
				case "401002025":%Conv.AddOption(405211, 405211);	//打开工具商店
				case "401002042":%Conv.AddOption(405181, 405181);	//打开鉴定符商店

//				case "400001107":		//摊位-测试商店
//					%Conv.AddOption(400001, 400001);	//"<t>所有道具测试：	400001</t>";
//					%Conv.AddOption(400002, 400002);	//"<t>镶嵌测试商店：	400002</t>";
//					%Conv.AddOption(400003, 400003);	//"<t>1--80级武器测试：	400003</t>";
//					%Conv.AddOption(400004, 400004);	//"<t>1--80级防具测试：	400004</t>";
//					%Conv.AddOption(400005, 400005);	//"<t>1--80级饰品测试：	400005</t>";
//					%Conv.AddOption(400006, 400006);	//"<t>强化测试商店：	400006</t>";
//					%Conv.AddOption(400007, 400007);	//"<t>鉴定测试商店：	400007</t>";
//					%Conv.AddOption(400008, 400008);	//"<t>模型测试商店：	400008</t>";
//					%Conv.AddOption(400009, 400009);	//"<t>任务测试商店：  400009</t>";
//					%Conv.AddOption(400010, 400010);	//"<t>测试商店：  400010</t>";
			}
	}

	if( (%State >= 400000)&&(%State < 500000) )
		OpenNpcShop(%Player,%State);
}

//传送地图
function NpcTransportObject(%Npc, %Player, %State , %Conv )
{
//	echo("zzzzzzzzz%State = "@%State);
	switch(%State)
	{
		case 0:
			//对话传送功能
			switch$(%Npc.GetDataID())
			{
				case "400001037":
					if(GetZoneID() $= "1002"){%Conv.AddOption(1001, 100103 );}	//传送至万灵城
					if(GetZoneID() $= "1001"){%Conv.AddOption(1002, 100203 );}	//传送至月幽境

				case "400004037":
					if(GetZoneID() $= "1401"){%Conv.AddOption(1001, 100106 );}	//传送到万灵城

				case "401011003":%Conv.AddOption(1001, 100103 );	//传送至万灵城
				case "411004600":%Conv.AddOption(1001, 100106 );	//33天门巨灵神同志传送至万灵城
				case "401124001":%Conv.AddOption(1001, 10111 + 500000 );	//灵谷传送至万灵城
				case "401302001":%Conv.AddOption(11  , 10165 + 500000 );	//离开闹鬼的渔村至夏梦狼

				case "400002105":
					if( (%Player.IsAcceptedMission(10162))||(%Player.IsAcceptedMission(10163))||(%Player.IsAcceptedMission(10164))||(%Player.IsAcceptedMission(10165)) )
						%Conv.AddOption(1302, 130201 );	//进入夜晚的渔村

				case "400001007":%Conv.AddOption(1124, 112401 );	//传送去灵谷

//				case "400001102":
//					if((%Player.IsFinishedMission(10109))&&(!%Player.IsFinishedMission(10110)))
//						%Conv.AddOption(1303, 130301);	//传送进女娲试炼

				case "401301001":%Conv.AddOption(1001,100119);

				case "401303101":%Conv.AddOption(1001,100119);

				case "401102112":
					if((%Player.IsFinishedMission(10320))&&(!%Player.IsFinishedMission(10325)))
				 		%Conv.AddOption(1102, 110204 );	//进入水墨洞天顶

				case "401102115":
					if((%Player.IsFinishedMission(10321))&&(!%Player.IsFinishedMission(10325)))
						%Conv.AddOption(1304, 130401 );	//进入水墨古镇（七日前）
				 	  %Conv.AddOption(1102, 110206 );  //到夕苍身边

				case "401103014":
					if((%Player.IsFinishedMission(10584))&&(!%Player.IsFinishedMission(10589)))
						%Conv.AddOption(1305, 130501 );	//进入试炼迷境

				case "401305001":%Conv.AddOption(1103, 110308);//传出试炼迷境
		  	case "401304001":%Conv.AddOption(1102, 110206);//传出水墨古镇
				case "401103001":%Conv.AddOption(13, 110302 );//东边下方传送到上方
				case "401103002":%Conv.AddOption(13, 110303 );//东边上方传送到下方
				case "401103003":%Conv.AddOption(13, 110304 );//西边下方传送到上方
				case "401103004":%Conv.AddOption(13, 110305 );//西边上方传送到下方

				case "401401001":
					%Conv.AddOption(1001, 100106 );	//传送至万灵城
					%Conv.AddOption(1002, 100203 );	//传送至月幽境
					%Conv.AddOption(1011, 101101 );	//传送至门宗地图"昆仑宗(圣)"昆仑古虚
					%Conv.AddOption(1006, 100601 );	//传送至门宗地图"金禅寺(佛)"极乐西天
					%Conv.AddOption(1007, 100701 );	//传送至门宗地图"蓬莱派(仙)"蓬莱仙境
					%Conv.AddOption(1005, 100501 );	//传送至门宗地图"飞花谷(精)"神木林
					%Conv.AddOption(1009, 100901 );	//传送至门宗地图"九幽教(鬼)"幽冥鬼域
					%Conv.AddOption(1010, 101001 );	//传送至门宗地图"山海宗(怪)"双生山
					%Conv.AddOption(1008, 100801 );	//传送至门宗地图"幻灵宫(妖)"醉梦冰池
					%Conv.AddOption(1004, 100401 );	//传送至门宗地图"天魔门(魔)"落夕渊
			}
	}

	if(strlen(%State) == 6)
		{
//			echo("yyyyyyyy%State = "@%State);
			if(%State == 200000)
				{
					%Conv.SetText(%State);
					%Conv.AddOption(4 , 0 );//返回
				}
			if(GetSubStr(%State,0,1) $= "1")
				{
					echo("xxxxxxxxxx%State = "@%State);
					%Conv.SetType(4);	//关闭对话

					if(GetSubStr(%State,1,1) $= "3")
						GoToNextMap_CopyMap( %Player, GetSubStr(%State,0,4) );
					else
						GoToNextMap_Normal(%Player, %State);
				}

			//需要判断任务是否满足条件，不满足条件者给出二次确认才可传送
			if(GetSubStr(%State,0,1) $= "5")
				{
					%YesToo = 0;
					%Mid = %State - 500000;

					//第一次捕捉
					if(%Mid == 10111)
						{
							%Map = 100101;

							if(%Player.IsAcceptedMission(%Mid))
								if(%Player.GetItemCount(%Player.GetMissionFlag(%Mid, 2100)) < %Player.GetMissionFlag(%Mid, 2200))
									%YesToo = 1;
						}

					//清风废墟夜除鬼（肆）
					if(%Mid == 10165)
						{
							%Map = 110104;

							if(!%Player.IsFinishedMission(%Mid))
								%YesToo = 1;
						}

					if(%YesToo == 1)
						{
							%Conv.SetText(%Mid @ "1");	//任务条件不满足
							%Conv.AddOption(GetSubStr(%Map,0,4), %Map );	//传送出去
						}
						else
							{
								%Conv.SetType(4);	//关闭对话
								GoToNextMap_Normal(%Player, %Map);
							}
				}
		}
}

//开启功能界面
function NpcOpenJob(%Npc, %Player, %State , %Conv)
{
//--------------------------------------------NpcOpenJob-------------State == 0----↓----------------------
	switch(%State)
	{
		case 0:
			//万灵城NPC
			switch$(%Npc.GetDataID())
			{
				case "400001006":
					%Conv.AddOption(200211, 200211);		//法宝铸造	功能界面
					%Conv.AddOption(200212, 200212);		//法宝鉴定	功能界面
					%Conv.AddOption(200213, 200213);		//法宝强化	功能界面
					%Conv.AddOption(200214, 200214);		//法宝重铸	功能界面
					%Conv.AddOption(200215, 200215);		//法宝合成	功能界面

				case "400001008":
					%Conv.AddOption(200201, 200201);   //灵兽鉴定	功能界面
					%Conv.AddOption(200202, 200202);   //灵兽提高灵慧	功能界面
					%Conv.AddOption(200203, 200203);   //灵兽炼化	功能界面
					%Conv.AddOption(200204, 200204);   //灵兽还童	功能界面
					%Conv.AddOption(200205, 200205);   //灵兽繁殖	功能界面

				case "400001017": %Conv.AddOption(200109, 200109);		//升级挖矿技能	功能界面
				case "400001018": %Conv.AddOption(200110, 200110);		//升级伐木技能	功能界面
				case "400001019": %Conv.AddOption(200111, 200111);		//升级钓鱼技能	功能界面
				case "400001020": %Conv.AddOption(200112, 200112);		//升级种植技能	功能界面
				case "400001021": %Conv.AddOption(200113, 200113);		//升级采药技能	功能界面
				case "400001022": %Conv.AddOption(200114, 200114);		//升级狩猎技能	功能界面

				case "400001023": %Conv.AddOption(200101, 200101);		//升级铸造技能	功能界面
				case "400001024": %Conv.AddOption(200102, 200102);		//升级裁缝技能	功能界面
				case "400001025": %Conv.AddOption(200103, 200103);		//升级工艺技能	功能界面
				case "400001026": %Conv.AddOption(200104, 200104);		//升级符咒技能	功能界面
				case "400001027": %Conv.AddOption(200105, 200105);		//升级炼丹技能	功能界面
				case "400001028": %Conv.AddOption(200106, 200106);		//升级烹饪技能	功能界面
				case "400001029": %Conv.AddOption(200107, 200107);		//升级工匠技能	功能界面
				case "400001030": %Conv.AddOption(200108, 200108);		//升级蛊术技能	功能界面
				case "400001031": %Conv.AddOption(200115, 200115);		//升级养生技能	功能界面

				case "400001046": %Conv.AddOption(200007, 200007);		//打开混元界	功能界面
				case "400001035": %Conv.AddOption(200221, 200221);		//告示牌	功能界面
				case "400001036": %Conv.AddOption(200222, 200222);		//通缉令	功能界面
				case "400001047": %Conv.AddOption(200223, 200223);		//活动界面	功能界面
				case "400001048": %Conv.AddOption(200224, 200224);		//公会界面	功能界面
				case "400001050": %Conv.AddOption(200225, 200225);		//我要结婚	功能界面
				case "400001059":
					%Conv.AddOption(200001, 200001);		//鉴定装备	功能界面
					%Conv.AddOption(200005, 200005);		//装备强化	功能界面
					%Conv.AddOption(200006, 200006);		//装备打孔	功能界面

				case "400001003":		//武器店	周老大
					%Conv.AddOption(200008, 200008);		//打开修理界面//经久修复装备
					%Conv.AddOption(200009, 200009);		//打开修理界面//最大耐久修复
			}
			//月幽境NPC
			switch$(%Npc.GetDataID())
			{
				case "401002006":
					%Conv.AddOption(200211, 200211);		//法宝铸造	功能界面
					%Conv.AddOption(200212, 200212);		//法宝鉴定	功能界面
					%Conv.AddOption(200213, 200213);		//法宝强化	功能界面
					%Conv.AddOption(200214, 200214);		//法宝重铸	功能界面
					%Conv.AddOption(200215, 200215);		//法宝合成	功能界面

				case "401002008":
					%Conv.AddOption(200201, 200201);   //灵兽鉴定	功能界面
					%Conv.AddOption(200202, 200202);   //灵兽提高灵慧	功能界面
					%Conv.AddOption(200203, 200203);   //灵兽炼化	功能界面
					%Conv.AddOption(200204, 200204);   //灵兽还童	功能界面
					%Conv.AddOption(200205, 200205);   //灵兽繁殖	功能界面

				case "401002011": %Conv.AddOption(200109, 200109);		//升级挖矿技能	功能界面
				case "401002012": %Conv.AddOption(200110, 200110);		//升级伐木技能	功能界面
				case "401002013": %Conv.AddOption(200111, 200111);		//升级钓鱼技能	功能界面
				case "401002014": %Conv.AddOption(200112, 200112);		//升级种植技能	功能界面
				case "401002015": %Conv.AddOption(200113, 200113);		//升级采药技能	功能界面
				case "401002016": %Conv.AddOption(200114, 200114);		//升级狩猎技能	功能界面

				case "401002017": %Conv.AddOption(200101, 200101);		//升级铸造技能	功能界面
				case "401002018": %Conv.AddOption(200102, 200102);		//升级裁缝技能	功能界面
				case "401002019": %Conv.AddOption(200103, 200103);		//升级工艺技能	功能界面
				case "401002020": %Conv.AddOption(200104, 200104);		//升级符咒技能	功能界面
				case "401002021": %Conv.AddOption(200105, 200105);		//升级炼丹技能	功能界面
				case "401002022": %Conv.AddOption(200106, 200106);		//升级烹饪技能	功能界面
				case "401002023": %Conv.AddOption(200107, 200107);		//升级工匠技能	功能界面
				case "401002024": %Conv.AddOption(200108, 200108);		//升级蛊术技能	功能界面
				case "401002025": %Conv.AddOption(200115, 200115);		//升级养生技能	功能界面

				case "401002037": %Conv.AddOption(200007, 200007);		//打开混元界	功能界面
				case "401002026": %Conv.AddOption(200221, 200221);		//告示牌	功能界面
				case "401002027": %Conv.AddOption(200222, 200222);		//通缉令	功能界面
				case "401002038": %Conv.AddOption(200223, 200223);		//活动界面	功能界面
				case "401002039": %Conv.AddOption(200224, 200224);		//公会界面	功能界面
				case "401002041": %Conv.AddOption(200225, 200225);		//我要结婚	功能界面
				case "401002042":
					%Conv.AddOption(200001, 200001);		//鉴定装备	功能界面
					%Conv.AddOption(200005, 200005);		//装备强化	功能界面
					%Conv.AddOption(200006, 200006);		//装备打孔	功能界面

				case "401002003":		//武器店
					%Conv.AddOption(200008, 200008);		//打开修理界面//经久修复装备
					%Conv.AddOption(200009, 200009);		//打开修理界面//最大耐久修复
			}
			//其它地图NPC
			switch$(%Npc.GetDataID())
			{
				case "401011002":			//圣门宗技能使
					if(%Player.GetFamily() == 1)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//选择第1系技能
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//选择第2系技能
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第3系技能
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第4系技能
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第5系技能

							//开启技能学习界面
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401007002":			//仙门宗技能使
					if(%Player.GetFamily() == 3)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//选择第1系技能
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//选择第2系技能
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第3系技能
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第4系技能
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第5系技能

							//开启技能学习界面
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401010002":			//怪门宗技能使
					if(%Player.GetFamily() == 6)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//选择第1系技能
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//选择第2系技能
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第3系技能
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第4系技能
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第5系技能

							//开启技能学习界面
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401009002":			//鬼门宗技能使
					if(%Player.GetFamily() == 5)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//选择第1系技能
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//选择第2系技能
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第3系技能
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第4系技能
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第5系技能

							//开启技能学习界面
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401005002":			//精门宗技能使
					if(%Player.GetFamily() == 4)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//选择第1系技能
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//选择第2系技能
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第3系技能
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第4系技能
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第5系技能

							//开启技能学习界面
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401008002":			//妖门宗技能使
					if(%Player.GetFamily() == 7)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//选择第1系技能
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//选择第2系技能
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第3系技能
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第4系技能
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第5系技能

							//开启技能学习界面
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401006002":			//佛门宗技能使
					if(%Player.GetFamily() == 2)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//选择第1系技能
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//选择第2系技能
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第3系技能
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第4系技能
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第5系技能

							//开启技能学习界面
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401004002":			//魔门宗技能使
					if(%Player.GetFamily() == 8)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//选择第1系技能
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//选择第2系技能
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第3系技能
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第4系技能
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//选择第5系技能

							//开启技能学习界面
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
			}
	}
//-----------------------------------------NpcOpenJob----------------State == 0----↑----------------------

//--------------------------------------------NpcOpenJob-------------State > 0----↓----------------------
	switch(%State)
	{
		case 200001:OpenIdentify(%Player);//开启装备鉴定功能窗口服务端脚本
		case 200003:OpenSkillSelect(%Player, %Player.GetClasses(0), %Player.GetClasses(1), %Player.GetClasses(2), %Player.GetClasses(3));//开启技能选择界面
		case 200004:OpenSkillStudy(%Player, %Player.GetClasses(0), %Player.GetClasses(1), %Player.GetClasses(2), %Player.GetClasses(3), %Player.GetClasses(4));//开启技能学习界面
		case 200005:%Player.OpenEquipStrengthenDialog(%Npc);	//服务端执行，打开强化列表
		case 200006:%Player.OpenEquipPunchHoleDialog(%Npc);		//服务端执行，打开打孔界面
		case 200007:%Player.OpenBank(0);											//服务端执行，打开仓库界面
		case 200008:%Player.OpenRepair(1);										//服务端执行，打开修理界面1 为经久修复 2为当前最大耐久修复
		case 200009:%Player.OpenRepair(2);										//服务端执行，打开修理界面1 为经久修复 2为当前最大耐久修复
		case 200109:OpenLivingSkillStudy(%Player,9 );	//400001017	挖矿技能师
		case 200110:OpenLivingSkillStudy(%Player,10);	//400001018	伐木技能师
		case 200111:OpenLivingSkillStudy(%Player,11);	//400001019	钓鱼技能师
		case 200112:OpenLivingSkillStudy(%Player,12);	//400001020	种植技能师
		case 200113:OpenLivingSkillStudy(%Player,13);	//400001021	采药技能师
		case 200114:OpenLivingSkillStudy(%Player,14);	//400001022	狩猎技能师
		case 200101:OpenLivingSkillStudy(%Player,1 );	//400001023	铸造技能师
		case 200102:OpenLivingSkillStudy(%Player,2 );	//400001024	裁缝技能师
		case 200103:OpenLivingSkillStudy(%Player,3 );	//400001025	工艺技能师
		case 200104:OpenLivingSkillStudy(%Player,4 );	//400001026	符咒技能师
		case 200105:OpenLivingSkillStudy(%Player,5 );	//400001027	炼丹技能师
		case 200106:OpenLivingSkillStudy(%Player,6 );	//400001028	烹饪技能师
		case 200107:OpenLivingSkillStudy(%Player,7 );	//400001029	工匠技能师
		case 200108:OpenLivingSkillStudy(%Player,8 );	//400001030	蛊术技能师
		case 200201:%Player.PetOperation_OpenPetIdentify(%Npc);	//宠物鉴定
		case 200202:%Player.PetOperation_OpenPetInsight(%Npc);	//宠物提高灵慧
		case 200203:%Player.PetOperation_OpenPetLianHua(%Npc);	//宠物炼化
		case 200204:%Player.PetOperation_OpenPetHuanTong(%Npc);	//宠物还童
		case 200205:	//宠物繁殖

		case 200211:	//法宝铸造
		case 200212:  //法宝鉴定
		case 200213:  //法宝强化
		case 200214:  //法宝重铸
		case 200215:  //法宝合成

		case 200221:	//告示牌	功能界面
		case 200222:  //通缉令	功能界面
		case 200223:  //活动界面	功能界面
		case 200224:  //公会界面	功能界面
		case 200225:  //我要结婚	功能界面
	}
//-----------------------------------------NpcOpenJob----------------State > 0----↑----------------------
}

//开启功能性对话，无界面
function NpcCanDoJob(%Npc, %Player, %State , %Conv)
{
//--------------------------------------NpcCanDoJob------------State == 0----↓----------------------
	switch(%State)
	{
		case 0:
			//万灵城NPC
			switch$(%Npc.GetDataID())
			{
				case "400001101":
					%Conv.AddOption(300003, 300003);	//领取新手状态(小于30级)
//					%Conv.AddOption(300004, 300004);	//"领取月光宝盒(测试)";

					if(%Player.IsFinishedMission(10108))
						{
							//五灵击
							if(%Player.GetSkillLevel(20004) > 0)
								{
									if( (%Player.GetLevel() >= 10)&&(%Player.GetLevel() < 20) )
										if(%Player.GetSkillLevel(20004) == 1)
											%Conv.AddOption(300034, 300034);	//"提升领悟技能：五灵击2";

									if(%Player.GetLevel() >= 20)
										if(%Player.GetSkillLevel(20004) == 2)
											%Conv.AddOption(300034, 300034);	//"提升领悟技能：五灵击3";
								}
								else
									%Conv.AddOption(300005, 300005);	//"领悟技能：五灵击";

							//五灵升华
							if( (%Player.GetSkillLevel(20004) == 3)&&(%Player.GetLevel() >= 30) )
								{
									if( (%Player.GetSkillLevel(20005) == 0)&&(%Player.GetSkillLevel(20006) == 0) )
										%Conv.AddOption(300030, 300030);	//"领悟新的分支技能：五灵升华";
									else
										{
											if( (%Player.GetLevel() >= 40)&&(%Player.GetLevel() < 50) )
												{
													if(%Player.GetSkillLevel(20005) <= 1){%Conv.AddOption(300028, 300026);}	//"提升领悟技能：五灵升华-火球2";
													if(%Player.GetSkillLevel(20006) <= 1){%Conv.AddOption(300029, 300027);}	//"提升领悟技能：五灵升华-冰箭2";
												}
											if( (%Player.GetLevel() >= 50)&&(%Player.GetLevel() < 60) )
												{
													if(%Player.GetSkillLevel(20005) <= 2){%Conv.AddOption(300028, 300026);}	//"提升领悟技能：五灵升华-火球3";
													if(%Player.GetSkillLevel(20005) <= 2){%Conv.AddOption(300029, 300027);}	//"提升领悟技能：五灵升华-冰箭3";
												}
											if(%Player.GetLevel() >= 60)
												{
													if(%Player.GetSkillLevel(20005) <= 3){%Conv.AddOption(300032, 300026);}	//"提升领悟技能：五灵升华-火雨";
													if(%Player.GetSkillLevel(20005) <= 3){%Conv.AddOption(300033, 300027);}	//"提升领悟技能：五灵升华-暴雪";
												}
										}
								}
						}
						
					%Conv.AddOption(300025, 300025);	//万灵城的由来

				case "400001072":
					%Conv.AddOption(300015, 300015);	//人物加血
					%Conv.AddOption(300016, 300016);	//宠物加血

				case "400001049":%Conv.AddOption(300021, 300021);	//前往洞府
				case "400001050":%Conv.AddOption(300022, 300022);	//友好度循环
				case "400001060":
					%Conv.AddOption(300023, 300023);	//领取双倍经验
					%Conv.AddOption(300024, 300024);	//冻结双倍经验

				case "400001102":%Conv.AddOption(300036, 300036);	//祭拜女娲

			}
			//月幽境NPC
			switch$(%Npc.GetDataID())
			{
				case "401002040":%Conv.AddOption(300021, 300021);	//前往洞府
				case "401002041":%Conv.AddOption(300022, 300022);	//友好度循环
				case "401002043":
					%Conv.AddOption(300023, 300023);	//领取双倍经验
					%Conv.AddOption(300024, 300024);	//冻结双倍经验
			}
	}
//--------------------------------------NpcCanDoJob------------State == 0----↑----------------------
//--------------------------------------NpcCanDoJob---------State > 0----↓----------------------
	switch(%State)
	{
		case 300003:
			%Player.AddBuff(390010001, 0);
			%Player.AddBuff(390020001, 0);
			%Conv.SetType(4);

		case 300004:
			%Player.PutItem(108020208,1);
			%Player.AddItem();
			%Conv.SetType(4);

		case 300005:
			%Player.AddSkill(200040001);
			%Conv.SetType(4);

		case 300021:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//暂未开放，敬请期待！//返回	//前往洞府
		case 300022:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//暂未开放，敬请期待！//返回	//友好度循环
		case 300023:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//暂未开放，敬请期待！//返回	//领取双倍经验
		case 300024:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//暂未开放，敬请期待！//返回	//冻结双倍经验
		case 300025:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//暂未开放，敬请期待！//返回	//万灵城的由来
		case 300036:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//暂未开放，敬请期待！//返回	//祭拜女娲

		case 300026:
			if(%Player.GetSkillLevel(20004) == 3)
				{
					//五灵升华-火球1
					if(%Player.GetLevel() >= 30)
						if(%Player.GetSkillLevel(20005) == 0)
							%Player.AddSkill(200050001);
					//五灵升华-火球2
					if(%Player.GetLevel() >= 40)
						if(%Player.GetSkillLevel(20005) == 1)
							%Player.AddSkill(200050002);
					//五灵升华-火球3
					if(%Player.GetLevel() >= 50)
						if(%Player.GetSkillLevel(20005) == 2)
							%Player.AddSkill(200050003);
					//五灵升华-火雨
					if(%Player.GetLevel() >= 60)
						if(%Player.GetSkillLevel(20005) == 3)
							%Player.AddSkill(200050004);
				}
			%Conv.SetType(4);

		case 300027:
			if(%Player.GetSkillLevel(20004) == 3)
				{
					//五灵升华-冰箭1
					if(%Player.GetLevel() >= 30)
						if(%Player.GetSkillLevel(20006) == 0)
							%Player.AddSkill(200060001);
					//五灵升华-冰箭2
					if(%Player.GetLevel() >= 40)
						if(%Player.GetSkillLevel(20006) == 1)
							%Player.AddSkill(200060002);
					//五灵升华-冰箭3
					if(%Player.GetLevel() >= 50)
						if(%Player.GetSkillLevel(20006) == 2)
							%Player.AddSkill(200060003);
					//五灵升华-暴雪
					if(%Player.GetLevel() >= 60)
						if(%Player.GetSkillLevel(20006) == 3)
							%Player.AddSkill(200060004);
				}
			%Conv.SetType(4);
			
		case 300030:
			%Conv.SetText(300031);
			%Conv.AddOption(300026, 300026);	//"领悟技能：五灵升华-火球1";
			%Conv.AddOption(300027, 300027);	//"领悟技能：五灵升华-冰箭1";

		case 300034:
			if(%Player.GetLevel() >= 10)
				if(%Player.GetSkillLevel(20004) == 1)
					%Player.AddSkill(200040002);	//"提升领悟技能：五灵击2";

			if(%Player.GetLevel() >= 20)
				if(%Player.GetSkillLevel(20004) == 2)
					%Player.AddSkill(200040003);	//"提升领悟技能：五灵击3";
					
		%Conv.SetType(4);
	}

	//医生加血
	switch(%State)
	{
		case 300015:
			%Conv.SetText(300017);	//来一口不？
			%Conv.AddOption(300018, 300018);	//人物加血
			%Conv.AddOption(4 , 0 );//返回

		case 300016:
			%Conv.SetText(300019);	//功能未开放！
			%Conv.AddOption(4 , 0 );//返回

		case 300018:
			%Conv.AddOption(4 , 0 );//返回
			%HP = %Player.GetMaxHP() - %Player.GetHP();
			if(%HP > 0)
				{
					%Money = 1 + %HP / 100;
					if(%Player.GetMoney(1) > %Money)
						{
							%Player.ReduceMoney(%Money, 1);
							%Player.AddBuff(390210001, 0);
							%Conv.SetType(4);
							return;
						}
						else
							%Conv.SetText(31);	//无钱无奶
 				}
 				else
 					%Conv.SetText(300020);	//兄台，你身体很棒啊！

	}

//--------------------------------------NpcCanDoJob---------State > 0----↑----------------------
}

//郭钟
//function SpawnNewNpc(%Npc, %Player, %State , %Conv)
//{
//	switch(%State)
//	{
//		case 0:
//			if(%Npc.GetDataID() $= "400001104")
//				if( ($SpNewNpc[1] == 0)||($SpNewNpc[2] == 0)||($SpNewNpc[3] == 0)||($SpNewNpc[4] == 0)||($SpNewNpc[5] == 0) )
//					%Conv.AddOption(4000, 4000);	//让我看看你的梦魇！
//		case 4000:
//			if( ($SpNewNpc[1] == 0)||($SpNewNpc[2] == 0)||($SpNewNpc[3] == 0)||($SpNewNpc[4] == 0)||($SpNewNpc[5] == 0) )
//				{
//					$SpNewNpc[1] = SpNewNpc(%Player, 400001108, "-254.78 -407.974 100.176",0);
//					$SpNewNpc[2] = SpNewNpc(%Player, 400001109, "-254.705 -409.852 100.15",0);
//					$SpNewNpc[3] = SpNewNpc(%Player, 400001109, "-254.14 -408.14 100.15",0);
//					$SpNewNpc[4] = SpNewNpc(%Player, 400001110, "-255.516 -408.267 100.176",0);
//					$SpNewNpc[5] = SpNewNpc(%Player, 400001110, "-257.149 -410.206 100.176",0);
//				}
//	}
//}

//飞行点功能
function NpcFly(%Npc, %Player, %State , %Conv)
{
	switch(%State)
	{
		case 0:
			switch$(%Npc.GetDataID())
			{
				case "400001113":
					%Conv.AddOption( 9 @ 10010102, 310001);		//杨镖头飞行传送到清风滨
					%Conv.AddOption( 9 @ 10010005, 310005);		//万灵城飞行传送到月幽境
//				case "400001104":%Conv.AddOption( 9 @ 10010104, 310002);		//任惊梦飞行传送到清风滨
//				case "400002105":%Conv.AddOption( 9 @ 11010101, 310003);		//夏梦狼飞行传送到万灵城
				case "411004600":%Conv.AddOption( 9 @ 11270001, 310004);		//巨灵神飞行传送到万灵城
				case "401002028":%Conv.AddOption( 9 @ 10020002, 310006);		//月幽境飞行传送到万灵城
			}
	}

	//飞行点功能
	switch(%State)
	{
		case 310001:FlyMeToTheMoon(%Player, 10010102);
//		case 310002:FlyMeToTheMoon(%Player, 10010104);
//		case 310003:FlyMeToTheMoon(%Player, 11010101);
		case 310004:FlyMeToTheMoon(%Player, 11270001);
		case 310005:FlyMeToTheMoon(%Player, 10010005);
		case 310006:FlyMeToTheMoon(%Player, 10020002);
	}
}


//function NowAdd(%Npc, %Player, %State, %Conv)
//{
//
//	switch(%State)
//	{
//		case 0:
//			%Conv.AddOption(300004, 300004);	//"领取月光宝盒(测试)";

//			%Conv.AddOption(300008, 300008);	//"我要拜师";
//			%Conv.AddOption(300006, 300006);	//"我要拜师";
//			%Conv.AddOption(300007, 300007);	//"我要收徒";

//
//		case 300006:
//			CreateRelation(%Player.GetPlayerID(),3);
//			%Conv.SetType(4);
//
//		case 300007:
//			CreateRelation(%Player.GetPlayerID(),4);
//			%Conv.SetType(4);
//
//		case 300008:
//			AddMasterLevel(%Player.GetPlayerID(),1);
//			%Conv.SetType(4);

//	}
//}


//活动相关的功能
function PlayGame(%Npc, %Player, %State , %Conv)
{
	switch(%State)
	{
		case 0:
			switch$(%Npc.GetDataID())
			{
				case "400001102":	//女娲神像
					%Conv.AddOption(316, 316);	//"祭拜女娲神像";
			}

	}

	//女娲补天
	switch(%State)
	{
		case 316:
			%Conv.SetText(319);	//祭拜说明
			%Conv.AddOption(317, 317);	//"单人祭拜";
			%Conv.AddOption(318, 318);	//"组队祭拜";
		case 317:


		case 318:

	}
}
