//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端所有对话的入口文本，普通对话
//==================================================================================


//获取对话文本内容主路径，脚本主入口
function GetDialogText(%Npc, %Player, %DialogID, %Param)
{
	//触发防骗防盗提示
	if($Fang_Pian_Fang_Dao == 0)
		Fang_Pian_Fang_Dao();

//	echo("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
//	echo("%Param ======="@%Param);
//	echo("%DialogID = " @ %DialogID);
//	echo("%Param = " @ %Param);//对话附加参数

//	if(%Param == 1)
//		{
//			echo("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
//			echo("%Param ======="@%Param);
//			if($Mid_Accepted_Num != %Player.GetAcceptedMission())
//				%Num = %Player.GetAcceptedMission();
//			for(%i = 0; %i < %Num; %i++)
//			{
//
//			}
//		}
	//根据DialogID的长度，来区分此对话的类型

	//通用类对话
	if(strlen(%DialogID) <= 7)
		return Get_TY_Dialog(%Npc, %Player, %DialogID, %Param);
	//9位编号的相关对话
	if(strlen(%DialogID) == 9)
		{
			//可以依照ID来区分编号类型，也可依据编号首位数字来判断编号类型

			//编号首位代表大类标识，取值范围为1-9，目前已确定：
			//1、道具
			//2、技能
			//3、状态
			//4、NPC&Monster
			//5、召唤（可能）
			//
			//6.7.9.10尚未确定
			//8、区域

			if(GetSubStr(%DialogID,0,1) $= "1"){return Get_Item_Dialog(%Npc, %Player, %DialogID, %Param);}
			if(GetSubStr(%DialogID,0,1) $= "2"){return Get_Skill_Dialog(%Npc, %Player, %DialogID, %Param);}
			if(GetSubStr(%DialogID,0,1) $= "3"){return Get_Buff_Dialog(%Npc, %Player, %DialogID, %Param);}
			if(GetSubStr(%DialogID,0,1) $= "4"){return Get_Npc_Dialog(%Npc, %Player, %DialogID, %Param);}
			if(GetSubStr(%DialogID,0,1) $= "8"){return Get_Area_Dialog(%Npc, %Player, %DialogID, %Param);}
			if(GetSubStr(%DialogID,0,1) $= "9"){return Get_Fly_Dialog(%Npc, %Player, %DialogID, %Param);}

		}
	//任务类对话
	if(strlen(%DialogID) == 8)
		{
			%Tid = GetSubStr(%DialogID,0,3);
			%Mid = GetSubStr(%DialogID,3,5);

			%kind       	= GetMissionKind(%Mid);
			%Mid_LeiXin 	= $MissionKind[ %kind, 2];

			//基本任务对话
			if(%Tid == 100) return $Icon[1] @ $Get_Dialog_GeShi[31203] @ " 接取－" @ %Mid_LeiXin @ "：" @ GetMission_Name(%Player , %Mid , 1) @ "</t>";
			if(%Tid == 900)
				{
					if(CanDoThisMission(%Player, %Mid, 2, 0, 0) $= "")
						%Mid_Over = "（完成）";
					else
						%Mid_Over = "";

					return $Icon[2] @ $Get_Dialog_GeShi[31204] @ " 交付－" @ %Mid_LeiXin @ "：" @ GetMission_Name(%Player , %Mid , 1) @ %Mid_Over @ "</t>";
				}
			if(%Tid == 999)
				{
					%Step = $Get_Dialog_GeShi[31203] @ GetMission_Name(%Player , %Mid , 1) @ "(未完成)</t>";

					if(%Player.IsAcceptedMission(%Mid)){%Step = $Get_Dialog_GeShi[31202] @ GetMission_Name(%Player , %Mid , 1) @ "(已接受)</t>";}
					if(%Player.IsFinishedMission(%Mid)){%Step = $Get_Dialog_GeShi[31204] @ GetMission_Name(%Player , %Mid , 1) @ "(已经完成)</t>";}

					return %Step;
				}

			//10001,2+4,任务描述+任务奖励,显示任务接受时对话
			if(%Tid == 110) return Get_Mis_Dialog(%Npc, %Player, %Mid, 2) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 4);

			//10009,显示任务交付条件不满足时的对话
			if(%Tid == 199) return Get_Mis_Dialog(%Npc, %Player, %Mid, 2) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 4);

			//10009,显示任务中途NPC的对话与选项
			if( (%Tid >= 200)&&(%Tid < 300) ) return eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, %Tid);");	//"显示任务中途NPC的对话选项";

			//20001,5+4,任务完成+任务奖励,显示任务交付时对话
			if(%Tid == 910) return Get_Mis_Dialog(%Npc, %Player, %Mid, 5) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 4);

			//20002,,任务完成需求对话,显示在任务界面内，所有的追踪条件
//			if(%Tid == 20002)	return GetMissionNeedText(%Player, %Mid, 9999);

			//20003,,任务追踪处的热感文字，任务目标+任务奖励
			if(%Tid == 800)
				{
					if(%Mid == 20001)
						return Mission20001Dialog(%Npc, %Player, %Mid, %Tid);
					else if(%Mid == 20002)
						return Mission20002Dialog(%Npc, %Player, %Mid, %Tid);
					else if(%Mid == 20003)
						return Mission20003Dialog(%Npc, %Player, %Mid, %Tid);
					else
						return $Get_Dialog_GeShi[31201] @ Get_Mis_Dialog(%Npc, %Player, %Mid, 1) @ GetMissionNeedText(%Player, %Mid, 9999) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 4) @ "</t>";
				}

			//20010,3+1+6+2+4,任务难度+目标+需求+描述+奖励,任务界面内使用
			if(%Tid == 888)
				{
					if(%Mid == 20001)
						return Mission20001Dialog(%Npc, %Player, %Mid, %Tid);
					else if(%Mid == 20002)
						return Mission20002Dialog(%Npc, %Player, %Mid, %Tid);
					else if(%Mid == 20003)
						return Mission20003Dialog(%Npc, %Player, %Mid, %Tid);
					else
						return $Get_Dialog_GeShi[31201] @ Get_Mis_Dialog(%Npc, %Player, %Mid, 3) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 1) @ GetMissionNeedText(%Player, %Mid, 9999) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 2) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 4) @ "</t>";
				}

//			%T10001 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10001);");	//"任务目标";
//			%T10002 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10002);");	//"任务描述";
//			%T10004 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10004);");	//"任务奖励";
//			%T10005 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10005);");	//"任务完成对话";

//			%T20011 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 20011);");	//"任务完成需求对话1";
//			%T20012 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 20012);");	//"任务完成需求对话2";
//			%T20013 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 20013);");	//"任务完成需求对话3";
//			%T20014 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 20014);");	//"任务完成需求对话4";
//			%T20015 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 20015);");	//"任务完成需求对话4";

//			echo("%T10001 = " @ %T10001);
//			echo("%T10002 = " @ %T10002);
//			echo("%T10003 = " @ %T10003);
//			echo("%T10004 = " @ %T10004);
//			echo("%T10005 = " @ %T10005);
//			echo("%T10006 = " @ %T10006);

			//20011,,任务完成需求对话,显示在任务追踪处，白色字体
//			if(%Tid == 20011) return $Get_Dialog_GeShi[31201] @ %T20011;
//			if(%Tid == 20012) return $Get_Dialog_GeShi[31201] @ %T20012;
//			if(%Tid == 20013) return $Get_Dialog_GeShi[31201] @ %T20013;
//			if(%Tid == 20014) return $Get_Dialog_GeShi[31201] @ %T20014;
//			if(%Tid == 20015) return $Get_Dialog_GeShi[31201] @ %T20015;

//			if(%Tid == 20011){return %T20011 @ "<b/>";}
//			if(%Tid == 20012){return %T20012 @ "<b/>";}
//			if(%Tid == 20013){return %T20013 @ "<b/>";}
//			if(%Tid == 20014){return %T20014 @ "<b/>";}
//			if(%Tid == 20015){return %T20015 @ "<b/>";}

			//自动将任务编号加入至函数命名中去，并执行相应的函数，寻找剩余对话
			%MDFunction = "Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, %Tid);";
			return eval(%MDFunction);
		}
	return "GetDialogText == 错误 Npc【"@%Npc@"】Player【"@%Player@"】DialogID【"@%DialogID@"】Id【"@%Param@"】";
}

//获取通用类对话
function Get_TY_Dialog(%Npc, %Player, %DialogID, %Param)
{
//	echo("获取其它类对话 = " @ %DialogID);
	switch(%DialogID)
	{
		case 0: return $Get_Dialog_GeShi[31401] @ "欢迎来到《神仙传》世界！</t><b/><b/>";

		case 1:	return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "结束对话</t>";
		case 2:	return "接受任务";
		case 3:	return $Icon[1] @ $Get_Dialog_GeShi[31204] @ "完成任务</t>";
		case 4:	return $Icon[4] @ "<t>返回</t>";
		case 5:	return "打开";
		case 6:	return "你的背包已满，放不下了，请清理一下吧";
		case 7:	return "物品数据错误";
		case 8:	return "知道了";
		case 9:	return "查看物品";
		case 10:return $Icon[4] @ $Get_Dialog_GeShi[31204] @"关闭</t>";
		case 11:return $Icon[4] @ "<t>离开这里</t>";
		case 12:return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "我愿加入</t>";
		case 13:return "启动控制台";
		case 14:return "让我改变某个任务的状态";
		case 15:return "直接接受";
		case 16:return "直接完成";
		case 17:return "强制放弃";
		case 18:return "设置成未完成状态";
		case 19:return "【门宗任务】";
		case 20:return "【章回任务】";
		case 21:return "【支线剧情】";
		case 22:return "【每日循环】";
		case 23:return "【帮会任务】";
		case 24:return "【指引任务】";
		case 25:return "【其它任务】";
		case 26:return "目前暂无此类任务";
		case 27:return "你想对此任务干什么？";
		case 28:return "赐予我力量吧！希瑞！";
		case 29:return "让我轻舞飞扬一下（9秒）！";
		case 30:return "爆发吧！小宇宙！";
		case 31:return "不对哦，你没有足够的货币……";
		case 32:return "暂未开放，敬请期待！";
		case 33:return $Icon[5] @ "<t>灵珠进阶</t>";
		case 34:return "本命灵珠进阶条件不满足，无法提升阶段";
		case 35:return $Icon[5] @ "<t>进阶指南</t>";
		case 36:return $Icon[5] @ "<t>使用</t>";
		case 37:return $Icon[5] @ "<t>开窍</t>";
	}

	switch(%DialogID)
	{
		case 200:	return "初卷";
		case 201:	return "第1卷";
		case 202:	return "第2卷";
		case 203:	return "第3卷";
		case 204:	return "第4卷";
		case 205:	return "第5卷";
		case 206:	return "第6卷";
		case 207:	return "第7卷";
		case 208:	return "第8卷";
		case 209:	return "第9卷";
		case 210:	return "第10卷";


		case 299:	return "第1章";
		case 298:	return "第2章";
		case 297:	return "第3章";
		case 296:	return "第1附章";
		case 295:	return "第2附章";
		case 294:	return "第3附章";
		case 293: return "圣";
		case 292: return "佛";
		case 291: return "仙";
		case 290: return "精";
		case 289: return "鬼";
		case 288: return "怪";
		case 287: return "妖";
		case 286: return "魔";

		case 309:return "生活技能升级加活力";
		case 310:return "圣旨到！奉天承运，命你去……";
		case 311:return $Icon[4] @ "<t>让我离开副本！</t>";
		case 312:return $Icon[4] @ "<t>如何获得【" @ GetItemData(105100021, 1) @ "】</t>";
		case 313:return "你没有【" @ GetItemData(105100021, 1) @ "】，无法使用【" @ GetItemData(105030012, 1) @ "】</t>";
		case 314:return $Icon[4] @ "<t>如何获得【" @ GetItemData(105100022, 1) @ "】</t>";
		case 315:return "你没有【" @ GetItemData(105100022, 1) @ "】，无法使用【" @ GetItemData(105030013, 1) @ "】</t>";
		case 316:return $Icon[5] @ "<t>祭拜女娲神像</t>";
		case 317:return $Icon[5] @ "<t>单人祭拜</t>";
		case 318:return $Icon[5] @ "<t>组队祭拜</t>";
		case 319:return $Get_Dialog_GeShi[31401] @ "<t>祭起补天石，参拜女娲娘娘！</t><b/><b/>" @
										$Get_Dialog_GeShi[31213] @ "<t>单人祭拜：</t><b/>" @
										$Get_Dialog_GeShi[31206] @ "<t> ·每天只能祭拜一次</t><b/>" @
										$Get_Dialog_GeShi[31206] @ "<t> ·每次祭拜需要消耗5个【" @ GetItemData(105102018, 1) @ "】</t><b/><b/>" @
										$Get_Dialog_GeShi[31213] @ "<t>组队祭拜：</t><b/>" @
										$Get_Dialog_GeShi[31206] @ "<t> ·每天只能祭拜一次</t><b/>" @
										$Get_Dialog_GeShi[31206] @ "<t> ·祭拜一次需要5个【" @ GetItemData(105102018, 1) @ "】</t><b/>" @
										$Get_Dialog_GeShi[31206] @ "<t> ·只能由队长发起祭拜，所有队员必需在队长身边</t><b/>";

	}

	//以下区间属于地图名称
	if( (%DialogID >= 1000)&&(%DialogID < 2000) )
		return $Icon[10] @ "<t>传送至【" @ $Mission_Map[ %DialogID ] @ "】</t>";

	switch(%DialogID)
	{
		case 2000:	return $Icon[6] @ "<t>更多……</t>";
		case 2210:	return $Icon[6] @ "<t>怎样打开背包</t>";
		case 2220:	return $Icon[6] @ "<t>怎样查看任务</t>";
		case 2230:	return $Icon[6] @ "<t>怎样查看地图</t>";
		case 2240:	return $Icon[6] @ "<t>怎样领取双倍经验</t>";
		case 2250:	return $Icon[6] @ "<t>怎样存储仓库</t>";
		case 2260:	return $Icon[6] @ "<t>怎样添加好友</t>";
		case 2270:	return $Icon[6] @ "<t>怎样组成队伍</t>";
		case 2280:	return $Icon[6] @ "<t>游戏热键一览</t>";

		case 2211:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "使用热键【Alt+A】即可以打开背包，右键点击背包中的道具，即可以进行使用或装备</t><b/>";
		case 2221:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "使用热键【Alt+Q】即可以查看任务，点击带有下划线的字符即可以进行寻径</t><b/>";
		case 2231:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "使用热键【Tab】即可以查看当前地图，使用热键【Alt+M】即可以查看全部地图</t><b/>";
		case 2241:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "与万灵城的npc福星对话，即可以领取双倍经验，也可以存多余的双倍经验</t><b/>";
		case 2251:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "在三大主城的钱庄掌柜处，即可以存储仓库</t><b/>";
		case 2261:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "选中想要添加的玩家后，在目标栏上单击右键，选中【添加好友】即可；或者使用【Alt+F】呼出好友界面，选择添加并且输入对方姓名即可</t><b/>";
		case 2271:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "使用热键【Alt+T】呼出队伍界面，创建队伍后即可以组成队伍，或者选中目标后，单击右键选择邀请加入队伍，成功后自动组成队伍</t><b/>";
		case 2281:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @
											 "<t>【Alt+C】  人物属性（基本信息）</t><b/>" @
											 "<t>【Alt+S】  职业技能</t><b/>" @
											 "<t>【Alt+A】  个人包裹</t><b/>" @
											 "<t>【Alt+T】  队伍</t><b/>" @
											 "<t>【Alt+Q】  任务（已接任务）</t><b/>" @
											 "<t>【Alt+X】  宠物(召唤兽)</t><b/>" @
											 "<t>【Tab】    当前地图</t><b/>" @
											 "<t>【Alt+M】  大地图</t><b/>" @
											 "<t>【Alt+F】  社交关系（个人）</t><b/>" @
											 "<t>【Alt+G】  帮会</t><b/>" @
											 "<t>【Alt+W】  人物元神（信息）</t><b/>" @
											 "<t>【Alt+D】  洞府（洞天）</t><b/>" @
											 "<t>【Alt+H】  帮助</t><b/>" @
											 "<t>【Alt+V】  商城</t><b/>" @
											 "<t>【Alt+E】  个人书信（收件箱）</t><b/>" @
											 "<t>【Alt+B】  AC</t><b/>" @
											 "<t>【Alt+N】  公告牌</t><b/>" @
											 "<t>【Alt+L】  休闲游戏</t><b/>" @
											 "<t>【Alt+I】  仙篆录</t><b/>";

	}
	switch(%DialogID)
	{
		case 3400:	return $Icon[6] @ " <t>门宗加入指南</t>";
		case 3401:	return $Icon[4] @ " <t>让我再研究研究吧……</t>";

		case 3410:	return $Get_Dialog_GeShi[31401] @ "如若你拥有力量，那你希望用这力量制霸天下  维护苍生</t>";
		case 3411:	return $Icon[4] @ " <t>制霸天下，力量乃天赐霸者之物</t>";
		case 3412:	return $Icon[4] @ " <t>维护苍生，强者保护弱者乃天道</t>";

		case 3420:	return $Get_Dialog_GeShi[31401] @ "如果人生可以选择，你希望你的一生是 跌宕起伏 还是 波澜不惊</t>";
		case 3421:	return $Icon[4] @ " <t>跌宕起伏，人生怎能无大浪</t>";
		case 3422:	return $Icon[4] @ " <t>波澜不惊，春花哪堪几回霜，平淡即可</t>";

		case 3430:	return $Get_Dialog_GeShi[31401] @ "如若战败，你是重整旗鼓还是战死沙场</t>";
		case 3431:	return $Icon[4] @ " <t>重整旗鼓，君子报仇10年不晚</t>";
		case 3432:	return $Icon[4] @ " <t>战死沙场，只可战死，不可战败</t>";

		case 3440:	return $Get_Dialog_GeShi[31401] @ "和平之时，你更喜欢修生养性还是习武强身？</t>";
		case 3441:	return $Icon[4] @ " <t>修生养性，提高自身内在修为才是大事</t>";
		case 3442:	return $Icon[4] @ " <t>习武强身，外在的强大是生存的基础</t>";

		case 3450:	return $Get_Dialog_GeShi[31401] @ "不得已而战，你愿化为矛还是化为盾？</t>";
		case 3451:	return $Icon[4] @ " <t>矛，我愿成为迎击敌人的力量</t>";
		case 3452:	return $Icon[4] @ " <t>盾，我愿成为抵挡攻击的盾</t>";

		case 3460:	return $Get_Dialog_GeShi[31401] @ "攻有明暗之分，你更希望正面对决还是攻其不备</t>";
		case 3461:	return $Icon[4] @ " <t>相信自己实力，正面对决有何不可</t>";
		case 3462:	return $Icon[4] @ " <t>攻其不备，能省一兵则省一兵</t>";

		case 3470:	return $Get_Dialog_GeShi[31401] @ "你愿在前方为强大的屏障还是在后方为坚实的砥柱？</t>";
		case 3471:	return $Icon[4] @ " <t>屏障，我喜欢最直接的体现</t>";
		case 3472:	return $Icon[4] @ " <t>砥柱，我愿成为大家所能依靠的后援</t>";

		case 3501:	return "Do U Need Help ？";
		case 3502:	return "各个NPC位置如下：";
		case 3500:	return "查看本门宗的NPC位置";
		case 3510:	return "我要去其他门宗";
		case 3520:	return "我要回城去";
		case 3529:	return "我要回城去：万灵城，XX城";
		case 3530:	return "没什么需要帮助的，谢谢";

		case 3600:	return "门宗任务介绍";
		case 3700:	return "我要领取门宗任务";

		case 4000:	return "让我看看你的梦魇！";

		default:
			if( (%DialogID > 3490)&&(%DialogID < 3499) )
				return $Get_Dialog_GeShi[31401] @ "<t>根据你的选择，最适合你的门宗是：</t><b/><b/><t>   【"@$FamilyName[%DialogID - 3490, 1]@"】</t></t>";

	}
	switch(%DialogID)
	{
		case 4010: return $Icon[6] @ " <t>门宗背景</t>";
    case 4011: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4012: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4013: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4014: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4015: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4016: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4017: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4018: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";

		case 4020: return $Icon[6] @ " <t>门宗特色</t>";
    case 4021: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4022: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4023: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4024: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4025: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4026: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4027: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4028: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";

    case 4030: return $Icon[6] @ " <t>门宗坐骑</t>";
    case 4031: return $Get_Dialog_GeShi[31401] @ "门宗骑宠：昆仑神驹</t><b/>"@"<t>由昆仑灵气孕育而生，极有灵性，一生只认一主，不知疲倦不进水草，下可日行千里上可踏云而行，非圣者不能御。</t>";
    case 4032: return $Get_Dialog_GeShi[31401] @ "门宗骑宠：六牙白象</t><b/>"@"<t>佛法普度众生，西天白象受佛法教化而得神通生六牙，能以巨大之躯乘风驾云而行。</t>";
    case 4033: return $Get_Dialog_GeShi[31401] @ "门宗骑宠：瑞角鹿</t><b/>"@"<t>蓬莱独有仙鹿，四足汇聚灵气，一步十丈。但极为怕生，御仙气方可接近。瑞角鹿身头顶彩茸乃修为所聚，修为越久色彩越鲜艳。</t>";
    case 4034: return $Get_Dialog_GeShi[31401] @ "门宗骑宠：朝花鸟</t><b/>"@"<t>为神鸟凤凰散落凡间的羽根，天性护花，终年以花为伴，八宗之中唯一的天性坐骑，未经驯化，不需缰绳，自愿随百花行走天下。</t>";
    case 4035: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4036: return $Get_Dialog_GeShi[31401] @ "门宗骑宠：岩犀</t><b/>"@"<t>双生山中生有灵犀天生异质，其皮肤可汇聚地中灵气，浑身会被岩石覆盖，性情憨厚，易驾驭，载千斤仍能畅行山河之中。</t>";
    case 4037: return $Get_Dialog_GeShi[31401] @ "敬请期待</t>";
    case 4038: return $Get_Dialog_GeShi[31401] @ "门宗骑宠：魔狱恶犬</t><b/>"@"<t>深渊特有的生物，被四周魔气所染，性情残暴，嗜杀任何靠近的活物，而驯服恶犬也为魔宗弟子必修之课，常有弟子命丧于此。</t>";

    case 4040: return $Icon[6] @ " <t>门宗圣地</t>";
    case 4041: return $Get_Dialog_GeShi[31401] @ "昆仑古墟</t><b/>"@"<t>昆仑古墟巍巍雄踞，三十六奇险，称昆仑“三十六天机”，宗处极巅，一柄巨剑穿天连地。后山有一处众圣弃兵之地，称“武冢”</t>";
    case 4042: return $Get_Dialog_GeShi[31401] @ "雷音寺</t><b/>"@"<t>雷音寺位于极乐西天，寺庙正大光明，佛光普照，菩提树、明镜台深含佛意。一鼎金钟不撞自响，炸出一片片金色梵文，在空中缓缓飘散。</t>";
    case 4043: return $Get_Dialog_GeShi[31401] @ "蓬莱仙境</t><b/>"@"<t>蓬莱仙境终年烟水弥漫，乃东海内涌的大瀑布中央一座孤岛，岛应天地四象之位，中心昊天塔四周布乾坤剑阵、三仙六宫九剑大符。</t>";
    case 4044: return $Get_Dialog_GeShi[31401] @ "神木林</t><b/>"@"<t>优美雅致的神木林，以一棵参天巨树为心，四周百花簇拥。其巨树乃神木，由上古而生，其树杈枝桠皆灵力浓郁，润泽自然，又有护卫之能。</t>";
    case 4045: return $Get_Dialog_GeShi[31401] @ "幽冥鬼域</t><b/>"@"<t>幽冥鬼域乃天下极阴之地，无边无际，阴风阵阵，黄泉腾腾。此地有树却无叶，有光却无暖意。九幽以镇魂台为心，鬼气连绵不绝。</t>";
    case 4046: return $Get_Dialog_GeShi[31401] @ "双生山</t><b/>"@"<t>河洛东州之南双生山，此山嶙峋怪异，沿山壁凿出一座宏伟的宫殿，建筑无不是粗犷豪迈，大气磅礴，集天下河山为一体。</t>";
    case 4047: return $Get_Dialog_GeShi[31401] @ "醉梦冰池</t><b/>"@"<t>醉梦冰池位于轩辕中州北面，是妖族以特有之幻境玄术建立的缥缈梦幻宫殿，其中所见无一不是迷离绝美。</t>";
    case 4048: return $Get_Dialog_GeShi[31401] @ "落夕渊</t><b/>"@"<t>落夕渊位于大地内部，由天魔一掌击成，意为“沉沦之日，永绝晨曦，即为落夕。”其中以一座天魔像为门宗象征。</t>";



	}

	switch(%DialogID)
	{
		case 4201: return $Icon[6] @ "<t>怎样学习制作药物</t>";
		case 4202: return $Icon[6] @ "<t>怎样学习烹饪食物</t>";
		case 4203: return $Icon[6] @ "<t>怎样学习铸造武器</t>";
		case 4204: return $Icon[6] @ "<t>怎样学习制作衣盔</t>";
		case 4205: return $Icon[6] @ "<t>怎样学习加工首饰</t>";
		case 4206: return $Icon[6] @ "<t>怎样获得灵兽</t>";
		case 4207: return $Icon[6] @ "<t>怎样喂食灵兽</t>";
		case 4208: return $Icon[6] @ "<t>灵兽功能说明</t>";
		case 4209: return $Icon[6] @ "<t>怎样获得骑乘</t>";
		case 4210: return $Icon[6] @ "<t>怎样摆摊</t>";
		case 4211: return $Icon[6] @ "<t>怎样挖矿</t>";
		case 4212: return $Icon[6] @ "<t>怎样伐木</t>";
		case 4213: return $Icon[6] @ "<t>怎样钓鱼</t>";
		case 4214: return $Icon[6] @ "<t>怎样种植</t>";
		case 4215: return $Icon[6] @ "<t>怎样采药</t>";
		case 4216: return $Icon[6] @ "<t>怎样狩猎</t>";
		case 4217: return $Icon[6] @ "<t>怎样铸造武器</t>";
		case 4218: return $Icon[6] @ "<t>怎样制造衣盔</t>";
		case 4219: return $Icon[6] @ "<t>怎样加工首饰</t>";
		case 4220: return $Icon[6] @ "<t>怎样制作符咒</t>";
		case 4221: return $Icon[6] @ "<t>怎样炼成丹药</t>";
		case 4222: return $Icon[6] @ "<t>怎样烹饪食物</t>";
		case 4223: return $Icon[6] @ "<t>怎样制造家具</t>";
		case 4224: return $Icon[6] @ "<t>怎样饲养蛊</t>";
		case 4225: return $Icon[6] @ "<t>如何扩展混元界</t>";
		case 4226: return $Icon[6] @ "<t>怎样获得洞府</t>";
		case 4227: return $Icon[6] @ "<t>鉴定的功能</t>";
		case 4228: return $Icon[6] @ "<t>双倍经验的说明</t>";

		case 4301: return "制作药品是一门高深而又繁杂的学问，20级时可习得“采药”，来获取药材，30级时可学会“炼丹”，将药材炼为丹药。";
		case 4302: return "民以食为天，要烹饪食物，你必须有相应的食材，食材来源有很多，怪物掉落的，狩猎的，种植出来的。";
		case 4303: return "神兵利器是帮助你修真的不二选择，30级能学习“挖矿“和“铸造“两门技能，就可以进行铸造了！";
		case 4304: return "修真时也应弱肉强食的法则，提升自身的防御力也很重要，10级时可以学习“狩猎”获得材料，15级去学习“裁缝”就可以制作了";
		case 4305: return "首饰的可以给你带来很多无法想象的效果，所以首饰的获得很特殊，你只有在40级时学习“工艺”才能完全掌握它";
		case 4306: return "60级以下可以去【灵兽谷】中捕捉灵兽，不过一定要带上“捆灵索”哦。不同的灵兽有不同的专长，即使同一种灵兽也有成长的不同";
		case 4307: return "当你的灵兽快乐、生命不足时，你就无法召唤它出来战斗了，这时候你就要购买玩具或者灵兽食物来喂养它了，点击喂养就可以了";
		case 4308: return "敬请期待!";
		case 4309: return "敬请期待!";
		case 4310: return "在万灵城交易港口，才可以进行摆摊。摆摊的办法很简单，点开“乾坤袋”界面，点击摆摊就可以放上想要出售或者收购的东西了";
		case 4311: return "学会了“挖矿”以后，还要购买“矿镐”才能挖矿。准备妥当了以后，去清风滨等野外的矿藏区域点击矿藏就可以了。";
		case 4312: return "学会了“伐木”以后，还要购买“斧头”才能伐木。准备妥当了以后，去清风滨等野外的伐木区域点击木材就可以了。";
		case 4313: return "学会了“钓鱼”以后，还要购买“鱼竿”才能钓鱼，准备妥当了以后，在我身边就有一级钓鱼区域，点击鱼群就可以了。";
		case 4314: return "学会了“种植”以后，还要购买“种子”才可以种植，在我身边就有耕地，在耕地上选择要种的东西就可以了，要注意种植需要时间哦。";
		case 4315: return "学会了“采药”以后，还要购买“镰刀”才可以采集，在清风滨就有一级的采药区域，点击就可以了。";
		case 4316: return "学会了“狩猎”以后，还要购买“猎弓”才可以狩猎，在清风滨就可以狩猎到一级的猎物。";
		case 4317: return "铸造入门容易提升难，因为很多兵器的铸造方法都是不流传的。但只要获得他们的“配方”，你只要准备好材料就可以铸造出神用的武器！";
		case 4318: return "制造衣盔很简单，你拥有配方，准备好材料，你想做什么就有什么！";
		case 4319: return "首先，你要获得首饰的制造配方，有了配方，其余的材料只要收集到，就不成问题了。";
		case 4320: return "符咒的制作方法很复杂";
		case 4321: return "获得足够的药材后，你要学习各种丹药的配方，按照配方上的比例和火候，你就能炼成逆天的丹药！";
		case 4322: return "烹饪食物要用心去做，你会发现食物并不只是充饥，他会给你带来意想不到的效果，比如，短时间内百毒不侵！当然，前提是你要获得配方！";
		case 4323: return "在你拥有洞府之后，你会发现一件好的家具，给你修真之路上带来的益处是多么大。你能相信有了精元池后，你的宠物可以自己获得经验吗？";
		case 4324: return "敬请期待!";
		case 4325: return "如果你得到了扩展混元界的混元袋，你将他放置在混元界下面的空格位子里，就可以扩展了。!";
		case 4326: return "敬请期待!";
		case 4327: return "一件好的装备有自己的灵性，不用心与他沟通时无法获知的。比如一件 混沌的武器，就可以给你提供五行攻击提升的优异属性!";
		case 4328: return "敬请期待!";

	}

  switch(%DialogID)
	{
		case 10000: return "<t>小神仙，你的等级还没有到10级啊，再接再厉，我很期望你的表现呢！</t><b/>"@ $Get_Dialog_GeShi[31206] @"<t>（提示:alt+q可以打开任务菜单,对已经接受的任务可以进行查看,利用寻径快速的完成任务.同时还可以在可接任务中查看没有接受到的任务.tab键可以打开地图,查看npc的方位和野外的方向,到野外击杀怪物也可以快速的到达10级哦!）</t>";
		case 10001: return "你的宠物还未升到10级,快去带着它升级吧.";
		case 10002: return "<t>你的宠物在哪里，我没有看到啊？没有宠物我怎么帮你鉴定呢？</t><b/>"@ $Get_Dialog_GeShi[31206] @"<t>（提示：打开背包，使用净化的大耳兔封印，你即可获得一只灵兽伙伴）</t>";
		case 10112: return "你还没抓住灵兽,快去灵谷抓吧.";
		case 10113:	return "<t>你还没有变身呢，我无法帮你解答变身药的效果说明啊!</t><b/>"@ $Get_Dialog_GeShi[31206] @"<t>（提示：点开背包，使用任务得到的变身丹，变身后就可以完成任务了）</t>";
		case 10114:	return "你可以先到鬼生财那里买耕药镰刀在去采仙鹤草.";
    case 10116:	return "前往日月池就可以吸取日月灵气了.";
    case 10140: return "你的等级未达到10级,如果你的经验已满,请打开人物属性框,点击右下方的升级按钮。";
    case 41010: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "我愿加入昆仑宗</t>";
    case 41020: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "我愿加入极乐世界</t>";
    case 41030: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "我愿加入蓬莱仙境</t>";
    case 41040: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "我愿加入神木林</t>";
    case 41050: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "我愿加入幽冥鬼域</t>";
    case 41060: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "我愿加入双生山</t>";
    case 41070: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "我愿加入醉梦冰池</t>";
    case 41080: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "我愿加入落夕渊</t>";
	}

	switch(%DialogID)
	{
		case 20000: return $Get_Dialog_GeShi[31401] @ "目标地图尚未开放……</t><b/>";
		case 99999:	return $Get_Dialog_GeShi[31401] @ "<t>欢迎来到《神仙传》的世界！</t><b/><b/>";

	}
	switch(%DialogID)
	{
		case 101111: return "你还没有捕捉到【"@GetItemData(118010001,1)@"】，确定要离开这里吗？";//第一次捕捉
		case 101651: return "你还没有全部完成【清风废墟夜除鬼】任务，确定要离开这里吗？";//清风废墟夜除鬼（肆）
	}
	switch(%DialogID)
	{
		case 200001: return $Icon[5] @ "<t>装备鉴定</t>";
		case 200003: return $Icon[9] @ "<t>技能选择界面</t>";
		case 200004: return $Icon[9] @ "<t>技能学习界面</t>";
		case 200005: return $Icon[5] @ "<t>装备强化</t>";
		case 200006: return $Icon[5] @ "<t>装备打孔</t>";
		case 200007: return $Icon[7] @ "<t>开启钱庄</t>";
		case 200008: return $Icon[5] @ "<t>经久修复装备</t>";
		case 200009: return $Icon[5] @ "<t>最大耐久修复</t>";

		case 200109: return $Icon[9] @ "<t>开启挖矿技能学习界面</t>";
		case 200110: return $Icon[9] @ "<t>开启伐木技能学习界面</t>";
		case 200111: return $Icon[9] @ "<t>开启钓鱼技能学习界面</t>";
		case 200112: return $Icon[9] @ "<t>开启种植技能学习界面</t>";
		case 200113: return $Icon[9] @ "<t>开启采药技能学习界面</t>";
		case 200114: return $Icon[9] @ "<t>开启狩猎技能学习界面</t>";
		case 200101: return $Icon[9] @ "<t>开启铸造技能学习界面</t>";
		case 200102: return $Icon[9] @ "<t>开启裁缝技能学习界面</t>";
		case 200103: return $Icon[9] @ "<t>开启工艺技能学习界面</t>";
		case 200104: return $Icon[9] @ "<t>开启符咒技能学习界面</t>";
		case 200105: return $Icon[9] @ "<t>开启炼丹技能学习界面</t>";
		case 200106: return $Icon[9] @ "<t>开启烹饪技能学习界面</t>";
		case 200107: return $Icon[9] @ "<t>开启工匠技能学习界面</t>";
		case 200108: return $Icon[9] @ "<t>开启蛊术技能学习界面</t>";

		case 200201: return $Icon[5] @ "<t>宠物鉴定</t>";
		case 200202: return $Icon[5] @ "<t>宠物提高灵慧</t>";
		case 200203: return $Icon[5] @ "<t>宠物炼化</t>";
		case 200204: return $Icon[5] @ "<t>宠物还童</t>";
		case 200205: return $Icon[5] @ "<t>宠物繁殖</t>";

		case 200211: return $Icon[5] @ "<t>法宝铸造</t>";
		case 200212: return $Icon[5] @ "<t>法宝鉴定</t>";
		case 200213: return $Icon[5] @ "<t>法宝强化</t>";
		case 200214: return $Icon[5] @ "<t>法宝重铸</t>";
		case 200215: return $Icon[5] @ "<t>法宝合成</t>";

		case 200221: return $Icon[5] @ "<t>告示牌</t>";
		case 200222: return $Icon[5] @ "<t>通缉令</t>";
		case 200223: return $Icon[5] @ "<t>活动界面</t>";
		case 200224: return $Icon[5] @ "<t>公会界面</t>";
		case 200225: return $Icon[5] @ "<t>我要结婚</t>";
	}
	switch(%DialogID)
	{
		case 300000: return "点击升级(暂用)";
		case 300001: return "点击获得经验(暂用)";
		case 300002: return "点击获得金钱(暂用)";
		case 300003: return $Icon[5] @ "<t>领取新手状态(小于30级)</t>";
		case 300004: return $Icon[5] @ "<t>领取月光宝盒(测试)</t>";
		case 300005: return $Icon[5] @ "<t>领悟技能：五灵击</t>";
		case 300006: return "我要拜师";
		case 300007: return "我要收徒";
		case 300008: return "提升师傅等级";

		case 300011: return "点击升1级(暂用)";
		case 300012: return "点击升10级(暂用)";
		case 300013: return "点击升50级(暂用)";
		case 300014: return "点击升120级(暂用)";

		case 300015: return $Icon[5] @ "<t>人物加血（收费）</t>";
		case 300016: return $Icon[5] @ "<t>宠物加血（免费）</t>";
		case 300017: return "来一口不？收费标准如下：每100HP收费1钱（最低消费1钱），很便宜吧！";
		case 300018: return $Icon[5] @ "<t>来一口</t>";
		case 300019: return "我也很想帮你，可惜功能未开放！";
		case 300020: return "兄台，你身体很棒，没问题啊！";

		case 300021: return $Icon[5] @ "<t>前往洞府</t>";
		case 300022: return $Icon[5] @ "<t>友好度循环</t>";
		case 300023: return $Icon[5] @ "<t>领取双倍经验</t>";
		case 300024: return $Icon[5] @ "<t>冻结双倍经验</t>";
		case 300025: return $Icon[4] @ "<t>万灵城的由来</t>";
		case 300026: return $Icon[5] @ "<t>领悟技能：五灵升华-火球</t>";
		case 300027: return $Icon[5] @ "<t>领悟技能：五灵升华-冰箭</t>";
		case 300028: return $Icon[5] @ "<t>提升领悟技能：五灵升华-火球</t>";
		case 300029: return $Icon[5] @ "<t>提升领悟技能：五灵升华-冰箭</t>";
		case 300030: return $Icon[5] @ "<t>领悟新的分支技能：五灵升华</t>";
		case 300031: return "<t>提升领悟技能：五灵升华，您现在可以领悟另一个提升技能：</t><b/>" @
												"<t>五灵升华-火球</t>" @
												"<t>五灵升华-冰箭</t>" @
												"<t>当您领悟其中一个提升技能后，将不可以再领悟另一个提升技能</t>";
		case 300032: return $Icon[5] @ "<t>提升领悟技能：五灵升华-【火雨】</t>";
		case 300033: return $Icon[5] @ "<t>提升领悟技能：五灵升华-【暴雪】</t>";
		case 300034: return $Icon[5] @ "<t>提升领悟技能：五灵击</t>";
		case 300035: return $Icon[5] @ "<t>进化领悟技能：五灵击</t>";
		case 300036: return $Icon[4] @ "<t>祭拜女娲</t>";


		case 399999:	return $Get_Dialog_GeShi[41401] @ "首先你需要了解一些基础帮助：</t><b/>"@
												 $Get_Dialog_GeShi[41402] @ "·点击[任务]按钮【快捷键Alt+Q】可查看已经接受的所有任务信息</t><b/>"@
												 $Get_Dialog_GeShi[41402] @ "·点击键盘的【快捷键Alt+H】可查看游戏的操作帮助说明</t><b/>"@
												 $Get_Dialog_GeShi[41401] @ "同时，我们将有四份惊喜礼物赠送给您：</t><b/>"@
												 $Get_Dialog_GeShi[41401] @ "您的第一份礼物是：修仙礼包。您可以打开背包【快捷键Alt+A】查看礼包。</t><b/>"@
												 $Get_Dialog_GeShi[41401] @ "再过5分钟，您将获得赠送的第二份惊喜大礼。</t><b/><b/>"@
												 $Get_Dialog_GeShi[31401] @ "您面前的【巨灵神】正等待着您。修仙之路为您开启！相信您就是命运注定的封神之人！</t><b/>";
	}

	switch(%DialogID)
	{
//		case 400010: return "打开测试商店";
//		case 400012: return "打开镶嵌道具商店";

		case 400001: return $Icon[8] @ "<t>所有道具测试：	400001</t>";
		case 400002: return $Icon[8] @ "<t>镶嵌测试商店：	400002</t>";
		case 400003: return $Icon[8] @ "<t>1--80级武器测试：	400003</t>";
		case 400004: return $Icon[8] @ "<t>1--80级防具测试：	400004</t>";
		case 400005: return $Icon[8] @ "<t>1--80级饰品测试：	400005</t>";
		case 400006: return $Icon[8] @ "<t>强化测试商店：	400006</t>";
		case 400007: return $Icon[8] @ "<t>鉴定测试商店：	400007</t>";
		case 400008: return $Icon[8] @ "<t>模型测试商店：	400008</t>";
		case 400009: return $Icon[8] @ "<t>任务测试商店：  400009</t>";
		case 400010: return $Icon[8] @ "<t>测试商店：  400010</t>";

		case 402011: return $Icon[8] @ "<t>打开武器商店</t>";
		case 402021: return $Icon[8] @ "<t>打开防具商店</t>";
		case 402031: return $Icon[8] @ "<t>打开首饰商店</t>";
		case 402041: return $Icon[8] @ "<t>打开杂货商店</t>";
		case 402042: return $Icon[8] @ "<t>打开食物商店</t>";
		case 402051: return $Icon[8] @ "<t>打开药品商店</t>";
		case 402061: return $Icon[8] @ "<t>打开工具商店</t>";
		case 402071: return $Icon[8] @ "<t>打开工具商店</t>";
		case 402081: return $Icon[8] @ "<t>打开工具商店</t>";

		case 402101: return $Icon[8] @ "<t>打开工具商店</t>";
		case 402111: return $Icon[8] @ "<t>打开工具商店</t>";
		case 402131: return $Icon[8] @ "<t>打开工具商店</t>";
		case 402141: return $Icon[8] @ "<t>打开工具商店</t>";
		case 402151: return $Icon[8] @ "<t>打开工具商店</t>";
		case 402161: return $Icon[8] @ "<t>打开灵兽道具商店</t>";
		case 402181: return $Icon[8] @ "<t>打开鉴定符商店</t>";

//		case       :" 打开工具商店</t>";
//		case       :" 打开工具商店</t>";
//		case       :" 打开法宝商店</t>";
//		case       :" 打开灵兽装备商店</t>";
//		case       :" 打开骑乘商店</t>";

		case 405011: return $Icon[8] @ "<t>打开武器商店</t>";
		case 405021: return $Icon[8] @ "<t>打开防具商店</t>";
		case 405031: return $Icon[8] @ "<t>打开首饰商店</t>";
		case 405041: return $Icon[8] @ "<t>打开杂货商店</t>";
		case 405042: return $Icon[8] @ "<t>打开食物商店</t>";
		case 405051: return $Icon[8] @ "<t>打开药品商店</t>";
		case 405061: return $Icon[8] @ "<t>打开工具商店</t>";
		case 405071: return $Icon[8] @ "<t>打开工具商店</t>";
		case 405081: return $Icon[8] @ "<t>打开工具商店</t>";

		case 405101: return $Icon[8] @ "<t>打开工具商店</t>";
		case 405111: return $Icon[8] @ "<t>打开工具商店</t>";
		case 405131: return $Icon[8] @ "<t>打开工具商店</t>";
		case 405141: return $Icon[8] @ "<t>打开工具商店</t>";
		case 405151: return $Icon[8] @ "<t>打开工具商店</t>";
		case 405161: return $Icon[8] @ "<t>打开灵兽道具商店</t>";
		case 405162: return $Icon[8] @ "<t>打开灵兽装备商店</t>";
		case 405171: return $Icon[8] @ "<t>打开骑乘商店</t>";
		case 405181: return $Icon[8] @ "<t>打开鉴定符商店</t>";
		case 405191: return $Icon[8] @ "<t>打开法宝商店</t>";
		case 405201: return $Icon[8] @ "<t>打开工具商店</t>";
		case 405211: return $Icon[8] @ "<t>打开工具商店</t>";


		case 411171: return $Icon[8] @ "<t>坐骑商店</t>";
		case 412171: return $Icon[8] @ "<t>坐骑商店</t>";
		case 413171: return $Icon[8] @ "<t>坐骑商店</t>";
		case 414171: return $Icon[8] @ "<t>坐骑商店</t>";
		case 415171: return $Icon[8] @ "<t>坐骑商店</t>";
		case 416171: return $Icon[8] @ "<t>坐骑商店</t>";
		case 417171: return $Icon[8] @ "<t>坐骑商店</t>";
		case 418171: return $Icon[8] @ "<t>坐骑商店</t>";

	}

	return "Get_TY_Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】DialogID【"@%DialogID@"】Id【"@%Param@"】";
}
//获取道具相关对话
function Get_Item_Dialog(%Npc, %Player, %DialogID, %Param)
{
	if(%DialogID > 0)
		{
			switch$(%DialogID)
			{
				case "108020048":
					%Txt = "你未接受与【"@GetItemData(%DialogID,1)@"】相关的任务，无法使用此物品";
					if( (%Player.IsAcceptedMission(10113))||(%Player.IsAcceptedMission(10114)) )
						{
							if(%Player.GetMissionFlag(10113, 1300) == 0){%Txt = GetItemData(%DialogID,1) @ "内空空如也，尚未吸收日月之精华";}
							if(%Player.GetMissionFlag(10113, 1300) == 1){%Txt = GetItemData(%DialogID,1) @ "沉甸甸的，已经吸满了日月之精华";}
							if(%Player.IsAcceptedMission(10114)){%Txt = GetItemData(%DialogID,1) @ "沉甸甸的，已经吸满了日月之精华，快去找【</t><c cid='" @ Get_TraceButton() @ "' cmd='\"findpath(400001060);\",\"" @ $Get_Dialog_GeShi[50000] @ "福星</t>\",\"GuiMissionTraceButtonProfile6\"' cf='createButton' /><t>】交付任务吧";}
						}
					return $Get_Dialog_GeShi[31201] @ "<t>这是【" @ GetItemData(%DialogID,1) @ "】，据说可在【</t>" @ "<c cid='" @ Get_TraceButton() @ "' cmd='\"findpath("@%DialogID@");\",\"" @ $Get_Dialog_GeShi[50000] @ "日月池内</t>\",\"GuiMissionTraceButtonProfile6\"' cf='createButton' />" @ "<t>】处吸收日月之精华！</t><b/><b/><t>" @ %Txt @ "</t></t>";

				case "105089001":
					return $Get_Dialog_GeShi[31201] @ "<t>你已经学会了【五灵击】技能，这个技能将会随着你的等级提升而领悟升级，当你到达10级，20级，30级……的时候回来找我看看吧。</t></t><b/><b/>" @
								 $Get_Dialog_GeShi[31206] @ "<t>【Alt+S】呼出技能界面，在战斗技能的综合技能里能找到【五灵击】技能，可以鼠标右键抓取该目标后放置在快捷栏上进行使用</t>";

				case "108020060":
					return $Get_Dialog_GeShi[31206] @ "<t>小萱爱妻：</t><b/>" @
								 "<t>清风鬼村，恶鬼横行。若见此书，我已遇害。此刻回想诸多往事，感慨万千，此生有你相伴十余年，死有何憾？</t><b/>" @
								 "<t>从今而后，你孤身一人，辛酸艰苦，广川对你不住！惟有来世再报。</t></t><b/>";

				case "108020048":return "该灵狐封印已经被诅咒了，去找苏苏 ，她能帮你解除封印。";

//				case "100000001":return "你可以获得以下的所有物品，点击查看物品";
//				case "100000002":return "你可以从以下物品序列中随机获得某个物品，点击查看物品序列";
//				case "100000003":return "你需要从以下数个物品序列中选择获得某个物品，点击查看物品序列";
//				case "100000004":return "你可以获得以下的所有物品，点击查看物品";

				case "108020208":return "老婆！快和牛魔王出来看上帝！";

				case "105099001":return $Get_Dialog_GeShi[31401] @ "<t>应天地大气运而生的灵珠，是不可多得的天地灵宝，而且随着修炼的日益精进，灵珠也可以进阶，最后成为逆天的无上法宝！</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>你当前的本命灵珠处于：【"@GetItemData(%DialogID,1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>当满足以下条件，即可进阶为：【"@GetItemData(105099002,1)@"】</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·任意加入八大门宗之一</t></t><b/>";
				case "105099002":return $Get_Dialog_GeShi[31401] @ "<t>应天地大气运而生的灵珠，是不可多得的天地灵宝，而且随着修炼的日益精进，灵珠也可以进阶，最后成为逆天的无上法宝！</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>你当前的本命灵珠处于：【"@GetItemData(%DialogID,1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>当满足以下条件，即可进阶为：【"@GetItemData(105099003,1)@"】</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·处于组队状态</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·自身等级达到13级</t></t><b/>";
				case "105099003":return $Get_Dialog_GeShi[31401] @ "<t>应天地大气运而生的灵珠，是不可多得的天地灵宝，而且随着修炼的日益精进，灵珠也可以进阶，最后成为逆天的无上法宝！</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>你当前的本命灵珠处于：【"@GetItemData(%DialogID,1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>当满足以下条件，即可进阶为：【"@GetItemData(105099004,1)@"】</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·获得一位师傅</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·自身等级达到17级</t></t><b/>";
				case "105099004":return $Get_Dialog_GeShi[31401] @ "<t>应天地大气运而生的灵珠，是不可多得的天地灵宝，而且随着修炼的日益精进，灵珠也可以进阶，最后成为逆天的无上法宝！</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>你当前的本命灵珠处于：【"@GetItemData(%DialogID,1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>当满足以下条件，即可进阶为：【"@GetItemData(105099005,1)@"】</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·元神等级达到5级</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·自身等级达到23级</t></t><b/>";
				case "105099005":return $Get_Dialog_GeShi[31401] @ "<t>应天地大气运而生的灵珠，是不可多得的天地灵宝，而且随着修炼的日益精进，灵珠也可以进阶，最后成为逆天的无上法宝！</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>你当前的本命灵珠处于：【"@GetItemData(%DialogID,1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>当满足以下条件，即可进阶为：【"@GetItemData(105099006,1)@"】</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·加入任意一个公会</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·自身等级达到28级</t></t><b/>";
				case "105099006":return $Get_Dialog_GeShi[31401] @ "<t>应天地大气运而生的灵珠，是不可多得的天地灵宝，而且随着修炼的日益精进，灵珠也可以进阶，最后成为逆天的无上法宝！</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>你当前的本命灵珠处于：【"@GetItemData(%DialogID,1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>当满足以下条件，即可进阶为：【"@GetItemData(105099007,1)@"】</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·获得一位仙侣</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·自身等级达到35级</t></t><b/>";
				case "105099007":return $Get_Dialog_GeShi[31401] @ "<t>应天地大气运而生的灵珠，是不可多得的天地灵宝，而且随着修炼的日益精进，灵珠也可以进阶，最后成为逆天的无上法宝！</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>你当前的本命灵珠处于：【"@GetItemData(%DialogID,1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>当满足以下条件，即可进阶为：【"@GetItemData(105099008,1)@"】</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·拥有二名徒弟</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·拥有二粒【"@GetItemData(105102015, 1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·自身等级达到55级</t></t><b/>";
				case "105099008":return $Get_Dialog_GeShi[31401] @ "<t>应天地大气运而生的灵珠，是不可多得的天地灵宝，而且随着修炼的日益精进，灵珠也可以进阶，最后成为逆天的无上法宝！</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>你当前的本命灵珠处于：【"@GetItemData(%DialogID,1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>当满足以下条件，即可进阶为：【"@GetItemData(105099009,1)@"】</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·拥有自己的洞府</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·拥有二粒【"@GetItemData(105102016, 1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·自身等级达到65级</t></t><b/>";
				case "105099009":return $Get_Dialog_GeShi[31401] @ "<t>应天地大气运而生的灵珠，是不可多得的天地灵宝，而且随着修炼的日益精进，灵珠也可以进阶，最后成为逆天的无上法宝！</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>你当前的本命灵珠处于：【"@GetItemData(%DialogID,1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>当满足以下条件，即可进阶为：【"@GetItemData(105099002,1)@"】</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·出师的徒弟达到10人</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·拥有二粒【"@GetItemData(105102017, 1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ·自身等级达到75级</t></t><b/>";
				case "105099010":return $Get_Dialog_GeShi[31401] @ "<t>应天地大气运而生的灵珠，是不可多得的天地灵宝，而且随着修炼的日益精进，灵珠也可以进阶，最后成为逆天的无上法宝！</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>你当前的本命灵珠处于：【"@GetItemData(%DialogID,1)@"】</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>本命灵珠已经进阶至终极阶段</t><b/>";

				case "105030012":return "此乃【" @ GetItemData(%DialogID,1) @ "】，需要消耗1个【" @ GetItemData(105100021, 1) @ "】才可开启。";//蓝色道果
				case "105030013":return "此乃【" @ GetItemData(%DialogID,1) @ "】，需要消耗1个【" @ GetItemData(105100022, 1) @ "】才可开启。";//紫色道果
				case "105030014":return "此乃【" @ GetItemData(%DialogID,1) @ "】，可通过开窍获知其品质，同时也会获得大量经验。";//道果

				case "105028011":return "此乃【" @ GetItemData(%DialogID,1) @ "】，使用后可在1小时内提高参悟天书所获得的经验。";//初级清聪灵液
				case "105028013":return "此乃【" @ GetItemData(%DialogID,1) @ "】，使用后可在2小时内提高参悟天书所获得的经验。";//中级清聪灵液
				case "105028014":return "此乃【" @ GetItemData(%DialogID,1) @ "】，使用后可在3小时内提高参悟天书所获得的经验。";//高级清聪灵液

				case "105100021":return "此乃【" @ GetItemData(%DialogID,1) @ "】，可通过打怪获得。";//地露液
				case "105100022":return "此乃【" @ GetItemData(%DialogID,1) @ "】，于元宝商城出售。";//天露液


				default:
					//礼包类道具组合对话
					if( (GetSubStr(%DialogID,0,4) == 1051)&&(GetSubStr(%DialogID,3,6) >= 100001)&&(GetSubStr(%DialogID,3,6) <= 100999) )
						{
//							echo("DialogID = "@%DialogID);
							%Player.ClearMissionItem();
							%Item_Num_All = $Item_Bao[%DialogID];

							%NumAll = GetWord(%Item_Num_All, 0);		//礼包内道具数量
							%Type   = GetWord(%Item_Num_All, 1);		//礼包类型

							//选择给道具的打包道具
							if(%Type == 3)
								{
									for(%i = 0;%i < %NumAll;%i++)
									{

										%Item = GetWord($Item_Bao[%DialogID, %i + 1], 0);
										%Num  = GetWord($Item_Bao[%DialogID, %i + 1], 1);

//										echo("Item = "@%i@" = "@%Item@" = "@%Num);
										%Player.AddItemToMission(0, %Item, %Num);
									}
									%Txt1 = "打开此包裹，你可以从以下物品序列中选择获得某个物品：</t><b/><b/>" @ GetOptionalMissionItemText();
//									echo("%Txt1%Txt1 ===="@%Txt1);
								}
								else
									{
//										echo("Type1 = "@%Type);
										if(%Type == 4)
											{
												//依据条件给道具的打包道具
												%Why = GetWord(%Item_Num_All, 2);		//礼包类型

												if(%Why == 1){%ListNum = %Player.GetFamily();%ListSex = %Player.GetSex();}	//门宗条件判断
												if(%Why == 2){%ListNum = %Player.GetLevel();}		//等级条件判断

												%Item_Num_Add = $Item_Bao[%DialogID, %ListNum, %ListSex];

//												echo("Item_Num_Add = "@%Item_Num_Add);

												for(%ii = 0;%ii < 9;%ii++)
												{
													%Item = GetWord(%Item_Num_Add, %ii * 2);
													%Num  = GetWord(%Item_Num_Add, %ii * 2 + 1);

//													echo("Item = "@%ii@" = "@%Item@" = "@%Num);

													if( (%Item $= "")&&(%Num $= "") )
														break;
													else
														%Player.AddItemToMission(1, %Item, %Num);
												}
											}
											else
												{
//													echo("Type2 = "@%Type);
													//必给道具的打包道具
													//随机给道具的打包道具
													for(%iii = 0;%iii < %NumAll;%iii++)
													{
														%Item = GetWord($Item_Bao[%DialogID, %iii + 1], 0);
														%Num  = GetWord($Item_Bao[%DialogID, %iii + 1], 1);

														%Player.AddItemToMission(1, %Item, %Num);

//														echo("i = "@%i@" "@%Item@" "@%Num);
													}
												}

										if(%Type == 1){%Txt1 = "打开此包裹，你将获得以下物品：</t><b/><b/>" @ GetFixedMissionItemText();}
										if(%Type == 2){%Txt1 = "打开此包裹，你将从以下物品序列中随机获得某个物品：</t><b/><b/>" @ GetFixedMissionItemText();}
										if(%Type == 4){%Txt1 = "打开此包裹，你将获得以下物品：</t><b/><b/>" @ GetFixedMissionItemText();}
									}

								return $Get_Dialog_GeShi[31401] @ %Txt1;
						}

			}
		}

	return "Get_Item_Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】DialogID【"@%DialogID@"】Id【"@%Param@"】";
}
//获取技能相关对话
function Get_Skill_Dialog(%Npc, %Player, %DialogID, %Param)
{
	return "Get_Skill_Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】DialogID【"@%DialogID@"】Id【"@%Param@"】";
}
//获取状态对话
function Get_Buff_Dialog(%Npc, %Player, %DialogID, %Param)
{
	return "Get_Buff_Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】DialogID【"@%DialogID@"】Id【"@%Param@"】";
}
//获取Npc与怪物相关对话
function Get_Npc_Dialog(%Npc, %Player, %DialogID, %Param)
{
//	echo("获取Npc与怪物相关对话");
//	echo(%Npc.GetDataID() @ " = " @ %DialogID);

	if(%Npc.GetDataID() $= %DialogID)
		{
			switch$(%Npc.GetDataID())
			{
				case "400001102":return $Get_Dialog_GeShi[31401] @ "<t>天地交融兮阴阳初构，</t><b/>" @
																													 "<t>乾坤和平兮日月始分；</t><b/>" @
																													 "<t>天神远去兮地上混沌，</t><b/>" @
																													 "<t>茫然洪荒兮豁然清晰；</t><b/>" @
																													 "<t>女娲德能兮捏泥造人，</t><b/>" @
																													 "<t>万灵再现兮撑天立地。</t><b/>";
				case "410107001":return $Get_Dialog_GeShi[31401] @ "吾乃【"@GetNpcData(%Npc.GetDataID(),1)@"】职掌【"@$FamilyName[1,1]@"】</t>";	//"昆仑宗(圣)
				case "410207001":return $Get_Dialog_GeShi[31401] @ "吾乃【"@GetNpcData(%Npc.GetDataID(),1)@"】职掌【"@$FamilyName[2,1]@"】</t>";	//"金禅寺(佛)
				case "410307001":return $Get_Dialog_GeShi[31401] @ "吾乃【"@GetNpcData(%Npc.GetDataID(),1)@"】职掌【"@$FamilyName[3,1]@"】</t>";	//"蓬莱派(仙)
				case "410407001":return $Get_Dialog_GeShi[31401] @ "吾乃【"@GetNpcData(%Npc.GetDataID(),1)@"】职掌【"@$FamilyName[4,1]@"】</t>";	//"飞花谷(精)
				case "410507001":return $Get_Dialog_GeShi[31401] @ "吾乃【"@GetNpcData(%Npc.GetDataID(),1)@"】职掌【"@$FamilyName[5,1]@"】</t>";	//"九幽教(鬼)
				case "410607001":return $Get_Dialog_GeShi[31401] @ "吾乃【"@GetNpcData(%Npc.GetDataID(),1)@"】职掌【"@$FamilyName[6,1]@"】</t>";	//"山海宗(怪)
				case "410707001":return $Get_Dialog_GeShi[31401] @ "吾乃【"@GetNpcData(%Npc.GetDataID(),1)@"】职掌【"@$FamilyName[7,1]@"】</t>";	//"幻灵宫(妖)
				case "410807001":return $Get_Dialog_GeShi[31401] @ "吾乃【"@GetNpcData(%Npc.GetDataID(),1)@"】职掌【"@$FamilyName[8,1]@"】</t>";	//"天魔门(魔)

				case "400001072":return "五毛一口，三口一块，欲购从速，过期不候！";	//医生加血
			//孟婆起死回生
				case "401401001":return $Get_Dialog_GeShi[31200] @ "<t>如果真的有一种水可以让你让我喝了不会醉</t><b/>" @
																								 "<t>那么也许有一种泪可以让你让我流了不伤悲</t><b/>" @
																								 "<t>总是把爱看的太完美那种豪赌一场的感觉</t><b/>" @
																								 "<t>今生输了前世的诺言才发现水已悄悄泛成了泪</t><b/>" @
																								 "<t>虽然看不到听不到可是逃不掉忘不了</t><b/>" @
																								 "<t>就连枕边的你的发梢都变成了煎熬</t><b/>" @
																								 "<t>虽然你知道我知道可是泪在漂心在掏</t><b/>" @
																								 "<t>过了这一秒这一个笑喝下这碗解药</t><b/>" @
																								 "<t>忘了所有的好所有的寂寥</t><b/><b/>" @
																								 "<t>如果真的有一种水可以让你让我喝了不会醉</t><b/>" @
																								 "<t>那么也许有一种泪可以让你让我流了不伤悲</t><b/>" @
																								 "<t>总是把爱看的太完美那种豪赌一场的感觉</t><b/>" @
																								 "<t>今生输了前世的诺言才发现水已悄悄泛成了泪</t><b/>" @
																								 "<t>虽然看不到听不到可是逃不掉忘不了</t><b/>" @
																								 "<t>就连枕边的你的发梢都变成了煎熬</t><b/>" @
																								 "<t>虽然你知道我知道可是泪在漂心在掏</t><b/>" @
																								 "<t>过了这一秒这一个笑喝下这碗解药</t><b/>" @
																								 "<t>忘了所有的好所有的寂寥</t><b/>" @
																								 "<t>虽然看不到听不到可是逃不掉忘不了</t><b/>" @
																								 "<t>就连枕边的你的发梢都变成了煎熬</t><b/>" @
																								 "<t>虽然你知道我知道可是泪在漂心在掏</t><b/><b/>" @
																								 "<t>过了这一秒这一个笑喝下这碗解药</t><b/>" @
																								 "<t>忘了所有的好</t><b/>" @
																								 "<t>所有的寂寥</t><b/></t>";

				case "400001019":return "老头我垂钓数十年，对鱼颇有见地。这鱼肉香嫩美味，是不错的食材，更有罕见的赤鲤、神仙鱼、玉锦鲤等，都是能增加修为灵能的神物。据说还有人从水底钓起了稀罕珍贵的坐骑呢！你是否要学习垂钓之术呢？可以让你的修真之路事半功倍！";//钓鱼Npc
				case "400001022":return "我浸润狩猎之术数十年，什么奇珍异兽都猎过，那些可都是宝物啊。兽骨可以做法宝，兽肉可以做食材，兽皮还可以做装备，实在是修真秘籍不二法门啊！你是否要学习狩猎之术呢？可以让你的修真之路事半功倍！";//狩猎Npc
				case "400001021":return "采药之术传承上古，只要会识别，会采集，天地之间万物皆可入药，学会了采药你会发现天下都是宝贝，怎样，要不要学学？";//采药Npc
				case "400001020":return "你可别小看这种植之术，上古大神神农之所以尝百草，是为研究万物性情，后又有大神后稷研究如何种植，所以我这种植之术还是许多大神的结晶，现在这种植之术早已神奇非常，天地都无法产生的奇物都可以通过种植得来，怎样，心动了吧！";//种植Npc
				case "400001018":return "伐木之术，看似简单，其实需要掌握的东西太多了，同一树木，会伐木的人砍伐下来的就是神木，不会砍伐的就是一截烂木，要懂得保存这断木的灵气，同时还要保护树木不会因为砍伐失去灵气，中间的价值天差地别啊。要不要学习我这伐木之术啊？";//伐木Npc
				case "400001017":return "说到采矿，那如品茶，初品味涩，其后味浓，妙趣无方。嘿嘿，你别怪我不会打比喻，以后你就知道了，这采矿之技，有成者或得神兵，或富甲一方，哪个没得好处呢？";//采矿Npc

				default:
					if($NpcRandomDialog[%Npc.GetDataID(),1] !$= "")
						{
							%D = 1;
							if($NpcRandomDialog[%Npc.GetDataID(),2] !$= ""){%D = 2;}
							if($NpcRandomDialog[%Npc.GetDataID(),3] !$= ""){%D = 3;}

							return $NpcRandomDialog[%Npc.GetDataID(),GetRandom(1,%D)];
						}
					else
						return $Get_Dialog_GeShi[31401] @ "欢迎来到《神仙传》的世界</t><b/><b/>";
			}

		}
		else
			return "Get_Npc_Dialog == 编号不对 NpcID【"@%Npc.GetDataID()@"】Player【"@%Player@"】DialogID【"@%DialogID@"】Id【"@%Param@"】";

	return "Get_Npc_Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】DialogID【"@%DialogID@"】Id【"@%Param@"】";
}
//获取区域相关对话
//function Get_Area_Dialog(%Npc, %Player, %DialogID, %Param)
//{
//	if(%DialogID $= "813030100")
//		return $Get_Dialog_GeShi[31201] @ "玄奥静谧的空间陡然响起太白金星的声音：杀六魂，证天命！</t><b/><b/>" @
//					 $Get_Dialog_GeShi[31206] @ "击败历练之魂，历练之魄，于太白金星证明自己的实力。</t>";
//
//	return "Get_Area_Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】DialogID【"@%DialogID@"】Id【"@%Param@"】";
//}

//获取飞行点相关对话
function Get_Fly_Dialog(%Npc, %Player, %DialogID, %Param)
{
	%FlyID = GetSubStr(%DialogID,1,8);

	switch$(%FlyID)
	{
		case "10010101":	return $Icon[11] @ "<t>序章进入飞行触发区域</t>";
		case "10010102":	return $Icon[11] @ "<t>杨镖头飞行传送到清风滨</t>";
		case "10010103":	return $Icon[11] @ "<t>夏梦狼飞行触发区域</t>";
		case "10010104":	return $Icon[11] @ "<t>任惊梦飞行传送到清风滨</t>";
		case "11010101":	return $Icon[11] @ "<t>夏梦狼飞行传送到万灵城</t>";
		case "11010102":	return $Icon[11] @ "<t>杨镖头飞行触发区域</t>";
		case "11010103":	return $Icon[11] @ "<t>任惊梦飞行触发区域</t>";
		case "11270001":	return $Icon[11] @ "<t>巨灵神飞行传送到万灵城</t>";
		case "10010005":	return $Icon[11] @ "<t>万灵城飞行传送到月幽境</t>";
		case "10010006":	return $Icon[11] @ "<t>月幽境飞行触发区域</t>";
		case "10020001":	return $Icon[11] @ "<t>万灵城飞行触发区域</t>";
		case "10020002":	return $Icon[11] @ "<t>月幽境飞行传送到万灵城</t>";

	}

	return "Get_Fly_Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】DialogID【"@%DialogID@"】Id【"@%Param@"】";
}
//获取事先设置的任务通用文字
function Get_Mis_Dialog(%Npc, %Player, %Mid, %Tid)
{
	%PngA = isFile("gameres/gui/images/GUIWindow25_1_005.png");//任务描述
	%PngB = isFile("gameres/gui/images/GUIWindow25_1_006.png");//任务目标
	%PngC = isFile("gameres/gui/images/GUIWindow25_1_007.png");//任务奖励
	%PngD = isFile("gameres/gui/images/GUIWindow25_1_008.png");//任务完成
	%PngE = isFile("gameres/gui/images/GUIWindow25_1_009.png");//任务难度

	if(%PngA == 1){%Png_A = "<i s='gameres/gui/images/GUIWindow25_1_005.png' w='16' h='16'/>";}else{%Png_A = "任务描述</t>";}
	if(%PngB == 1){%Png_B = "<i s='gameres/gui/images/GUIWindow25_1_006.png' w='16' h='16'/>";}else{%Png_B = "任务目标</t>";}
	if(%PngC == 1){%Png_C = "<i s='gameres/gui/images/GUIWindow25_1_007.png' w='16' h='16'/>";}else{%Png_C = "任务奖励</t>";}
	if(%PngD == 1){%Png_D = "<i s='gameres/gui/images/GUIWindow25_1_008.png' w='16' h='16'/>";}else{%Png_D = "任务完成</t>";}
	if(%PngE == 1){%Png_E = "<i s='gameres/gui/images/GUIWindow25_1_009.png' w='16' h='16'/>";}else{%Png_E = "任务难度 </t>";}

	//任务通用文字
	if(%Tid == 1)
		{
			%T10001 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10001);");	//"任务目标";
			%Txt = %Png_B @ "<b/>" @ %T10001;
		}
	if(%Tid == 2)
		{
			%TxtID = %Player.GetMissionFlag(%Mid, 4000);

			if(%TxtID < 1)
				%TxtID = 10002;

			%T10002 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, %TxtID);");	//"任务描述";
			%Txt = %Png_A @ "<b/>" @ %T10002;
		}
	if(%Tid == 3)
		{
			%Txt = %Png_E @ GetMission_NanDu(%Mid);	//"任务难度";
		}
	if(%Tid == 4)
		{
			%Player.ClearMissionItem();

			if(%Mid == 20001)
				{
					%T999 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 999);");	//"任务奖励";

					return %T999;
				}

			%MissionRewardData = "MissionRewardData_" @ %Mid;

			if(%MissionRewardData.JiFen     != 0){%JiFen = $Get_Dialog_GeShi[31212] @ "积分： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %MissionRewardData.JiFen @ " </t><t>]</t><b/>";}else{%JiFen = "";}
			if(%MissionRewardData.Exp       != 0){%Exp   = $Get_Dialog_GeShi[31212] @ "经验： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %MissionRewardData.Exp   @ " </t><t>]</t><b/>";}else{%Exp   = "";}
			if(%MissionRewardData.Gold      != 0){%Gold  = $Get_Dialog_GeShi[31212] @ "元宝： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %MissionRewardData.Gold  @ " </t><t>]</t><b/>";}else{%Gold  = "";}

			//金元奖励
			if(%MissionRewardData.Money     != 0)
				{
//					echo("金元奖励 = "@ %MissionRewardData.Money);

					%a = %MissionRewardData.Money;
					%b = 9 - strlen(%a);
					%c = "1";
					for(%i = 0;%i < %b;%i++)
						%c = %c @ "0";

					%Money = %c @ %a;

//					%Money = 100000000 + %MissionRewardData.Money;
//					%Money = 123456789 + %MissionRewardData.Money;
//					echo("Money = "@ %Money);
//					echo("Money a = "@ GetSubStr(%Money,1,4));
//					echo("Money b = "@ GetSubStr(%Money,5,2));
//					echo("Money c = "@ GetSubStr(%Money,7,2));
//
//					%Money_A = $Get_Dialog_GeShi[31201] @ GetSubStr(%Money,1,4) @ "</t>";	//金币
//					%Money_B = $Get_Dialog_GeShi[31201] @ GetSubStr(%Money,5,2) @ "</t>";	//银币
//					%Money_C = $Get_Dialog_GeShi[31201] @ GetSubStr(%Money,7,2) @ "</t>";	//铜币

//					echo("Money_A = "@ %Money_A);
//					echo("Money_B = "@ %Money_B);
//					echo("Money_C = "@ %Money_C);

//					%Money = 100000 + %MissionRewardData.Money;
//					echo("Money = "@ %Money);

					%Money_A = GetSubStr(%Money,1,5) + 0;	//金币
					%Money_B = GetSubStr(%Money,6,2) + 0;	//银币
					%Money_C = GetSubStr(%Money,8,2) + 0;	//铜币

//					echo("Money_A = "@ %Money_A);
//					echo("Money_B = "@ %Money_B);
//					echo("Money_C = "@ %Money_C);

					%Png1 = isFile("gameres/gui/images/GUIWindow31_1_024.png"); //金币
					%Png2 = isFile("gameres/gui/images/GUIWindow31_1_025.png"); //银币
					%Png3 = isFile("gameres/gui/images/GUIWindow31_1_026.png"); //铜币

					%Txt = $Get_Dialog_GeShi[31201];

					if(%Money_A > 0)
						{
							if(%Png1 == 1)
								%Show_A = %Txt @ %Money_A @ "</t><i s='gameres/gui/images/GUIWindow31_1_024.png' w='18' h='18'/><t> </t>";
							else
								%Show_A = %Txt @ %Money_A @ "金 </t>";
						}
						else
							%Show_A = "";

					if(%Money_B > 0)
						{
							if(%Png2 == 1)
								%Show_B = %Txt @ %Money_B @ "</t><i s='gameres/gui/images/GUIWindow31_1_025.png' w='18' h='18'/><t> </t>";
							else
								%Show_B = %Txt @ %Money_B @ "银 </t>";
						}
						else
							%Show_B = "";

					if(%Money_C > 0)
						{
							if(%Png2 == 1)
								%Show_C = %Txt @ %Money_C @ "</t><i s='gameres/gui/images/GUIWindow31_1_026.png' w='18' h='18'/><t> </t>";
							else
								%Show_C = %Txt @ %Money_C @ "铜 </t>";
						}
						else
							%Show_C = "";

					%Money = $Get_Dialog_GeShi[31212] @ "金元： </t><t>[ </t>" @ %Show_A @ %Show_B @ %Show_C @ "<t>]</t><b/>";

//					echo("%Money = "@%Money);
				}
				else
					%Money = "";

			//灵元奖励
			if(%MissionRewardData.Yuan     != 0)
				{
//					echo("灵元：奖励 = "@ %MissionRewardData.Yuan);

					%a = %MissionRewardData.Yuan;
					%b = 9 - strlen(%a);
					%c = "1";
					for(%i = 0;%i < %b;%i++)
						%c = %c @ "0";

					%Yuan = %c @ %a;

//					echo("%Yuan = "@%Yuan);
					%Yuan_A = GetSubStr(%Yuan,1,5) + 0;	//金币
					%Yuan_B = GetSubStr(%Yuan,6,2) + 0;	//银币
					%Yuan_C = GetSubStr(%Yuan,8,2) + 0;	//铜币

//					echo("Yuan_A = "@ %Yuan_A);
//					echo("Yuan_B = "@ %Yuan_B);
//					echo("Yuan_C = "@ %Yuan_C);

					%Png1 = isFile("gameres/gui/images/GUIWindow31_1_020.png"); //金币
					%Png2 = isFile("gameres/gui/images/GUIWindow31_1_021.png"); //银币
					%Png3 = isFile("gameres/gui/images/GUIWindow31_1_022.png"); //铜币

//					echo("Png1 = "@ %Png1);
//					echo("Png2 = "@ %Png2);
//					echo("Png3 = "@ %Png3);

					%Txt = $Get_Dialog_GeShi[31201];

					if(%Yuan_A > 0)
						{
							if(%Png1 == 1)
								%Show_A = %Txt @ %Yuan_A @ "</t><i s='gameres/gui/images/GUIWindow31_1_020.png' w='18' h='18'/><t> </t>";
							else
								%Show_A = %Txt @ %Yuan_A @ "金 </t>";
						}
						else
							%Show_A = "";

					if(%Yuan_B > 0)
						{
							if(%Png2 == 1)
								%Show_B = %Txt @ %Yuan_B @ "</t><i s='gameres/gui/images/GUIWindow31_1_021.png' w='18' h='18'/><t> </t>";
							else
								%Show_B = %Txt @ %Yuan_B @ "银 </t>";
						}
						else
							%Show_B = "";

					if(%Yuan_C > 0)
						{
							if(%Png2 == 1)
								%Show_C = %Txt @ %Yuan_C @ "</t><i s='gameres/gui/images/GUIWindow31_1_022.png' w='18' h='18'/><t> </t>";
							else
								%Show_C = %Txt @ %Yuan_C @ "铜 </t>";
						}
						else
							%Show_C = "";

//					echo("Show_A = "@ %Show_A);
//					echo("Show_B = "@ %Show_B);
//					echo("Show_C = "@ %Show_C);

					%Yuan = $Get_Dialog_GeShi[31212] @ "灵元： </t><t>[ </t>" @ %Show_A @ %Show_B @ %Show_C @ "<t>]</t><b/>";

//					echo("%Yuan = "@%Yuan);
				}
				else
					%Yuan = "";

			//声望奖励
			if(%MissionRewardData.ShengWang !$= "0")
				{
					%ShengWang = $Get_Dialog_GeShi[31212] @ "声望： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %MissionRewardData.ShengWang @ "</t><t> ]</t><b/>";
				}
				else
					%ShengWang = "";

			//选择性道具
			if(%MissionRewardData.ItemChose !$= "0")
				{
					%Set = 0;
					%I_Num = 9;

					//判断性别限制
					if( (%Mid == 10103)||(%Mid == 10106)||(%Mid == 10163) )
						{
							%I_Num = 3;

							if(%Player.GetSex() == 2)
								%Set = 3;
						}
					//判断门宗限制
					if(%Mid == 10127)
						{
							%I_Num = 2;

							if(%Player.GetFamily() == 1){%Set = 0;}
							if(%Player.GetFamily() == 2){%Set = 2;}
							if(%Player.GetFamily() == 3){%Set = 4;}
							if(%Player.GetFamily() == 4){%Set = 6;}
							if(%Player.GetFamily() == 5){%Set = 8;}
							if(%Player.GetFamily() == 6){%Set = 10;}
							if(%Player.GetFamily() == 7){%Set = 12;}
							if(%Player.GetFamily() == 8){%Set = 14;}

						}

					for(%i = 0; %i < %I_Num; %i++)
					{
						%Item = GetWord(%MissionRewardData.ItemChose, (%i + %Set) * 2);
						%Num  = GetWord(%MissionRewardData.ItemChose, (%i + %Set) * 2 + 1);

						echo("Mid=[ "@%Mid@" ]=Set=[ "@%Set@" ]=i=[ "@%i@" ]=Item=[ "@%Item@" ]=Num=[ "@%Num@" ]");

						if( (%Item $= "")&&(%Num $= "") )
							break;
						else
							%Player.AddItemToMission(0, %Item, %Num);
					}

					%ItemChose = $Get_Dialog_GeShi[31212] @ "可选任务奖励：</t><b/>" @ GetOptionalMissionItemText();
				}
				else
					%ItemChose = "";

			//必给道具
			if(%MissionRewardData.ItemSet !$= "0")
				{
					for(%i = 0; %i < 9; %i++)
					{
						%Item = GetWord(%MissionRewardData.ItemSet, %i * 2);
						%Num  = GetWord(%MissionRewardData.ItemSet, %i * 2 + 1);

						if( (%Item $= "")&&(%Num $= "") )
							break;
						else
							%Player.AddItemToMission(1, %Item, %Num);
					}

					%ItemSet = $Get_Dialog_GeShi[31212] @ "固定任务奖励：</t><b/>" @ GetFixedMissionItemText();
				}
				else
					%ItemSet = "";

			//技能
			if(%MissionData.Skill !$= "0")
				{
				}
				else
					%Skill = "";

			//状态
			if(%MissionData.Buff !$= "0")
				{
				}
				else
					%Buff = "";

			%Reward = %JiFen @ %Exp @ %Money @ %Yuan @ %Gold @ %ShengWang @ %ItemChose @ %ItemSet @ %Skill @ %Buff;

			if(%Reward !$= "")
				%Txt = %Png_C @ "<b/>" @ %Reward;
			else
				%Txt = "";
		}
	if(%Tid == 5)
		{
			%T10005 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10005);");	//"任务完成对话";
			%Txt = %Png_D @ "<b/>" @ %T10005;
		}

	if(%Txt !$= "")
		return $Get_Dialog_GeShi[31201] @ %Txt @ "<b/>";

	return "Get_Mis_Dialog == 错误 Tid【"@%Tid@"】";
}

