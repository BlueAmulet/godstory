function Mission10000Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>指引找神秘人谈话</t><b/>";
		case 10002:	return "<t>欢迎界面内容：欢迎你来到《神仙传》</t><b/>" @
											"<t>与你面前的神秘人谈谈，开启你的修神之路吧！</t><b/>";
		case 10005:	return "<t>你看起来很疲劳，呵呵，攀上须弥山巅，自然不会很轻松。</t><b/>" @
											"<t>怎么？不记得了？你从山底至此，用了整整三十年，一路艰险你都克服过来了。</t><b/>" @
											"<t>恭喜你，此地乃天人门户，再往上，就是通往神界的盘古之心！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10001Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>拔出天神剑，装备后去找神秘人对话</t><b/>";
		case 10002:	return "<t>多少人的梦想啊，修炼成神……</t><b/>" @
											"<t>如今盘古之心近在眼前，越过它，你将超越古今无数修真者，化身为神！</t><b/>" @
											"<t>前方的龙剑台，天神剑就封印在石台中，必须要这把剑，你才能突破最后的险关。</t><b/>" @
											"<t>去吧，发挥你的力量，拔起它！</t><b/>";
		case 10005:	return "<t>【神秘人看着你笑了】</t><b/>" @
											"<t>我就知道你有这个实力。切忌小觑此剑，也许它其貌不扬，但它藏有你所不知的秘密。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10002Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>击败一个灵石巨像，从它身上取回一颗仙灵石</t><b/>";
		case 10002:	return "<t>我能看出你还是很迷惘，不要紧，答案会一步步揭开。</t><b/>" @
											"<t>【神秘人阴阴笑了】</t><b/>" @
											"<t>话说回来，在你成神前，你必须能熟练地使用天神剑。在崖边有一些灵石巨像，用你的剑击碎它，从它们身上找到一颗仙灵石。</t><b/>";
		case 10005:	return "<t>对，就是这样，你挥剑的动作一如往日，呵呵。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10003Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>开启封印灵石，召唤守护巨像，让它打开盘古之心入口</t><b/>";
		case 10002:	return "<t>还不够，这样还不够，【" @ %Player.GetPlayerName() @ "】。</t><b/>" @
											"<t>要破除盘古之心的封印，才能开启入口。在山崖另一边，是伟大的封印石阵。去吧，开启石阵的能量，守护巨像会现身，它会打开盘古之心的大门！</t><b/>";
		case 10005:	return "<t>你知道你在做什么吗？盘古之心的恐怖是你无法想象的！</t><b/>";
			
	 case 200: return  $Get_Dialog_GeShi[31204] @ "请送我去封印石阵！</t>";	
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10004Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去找巨灵神，告诉他，你要成神！</t><b/>";
		case 10002:	return "<t>你犯下了错误，,【" @ %Player.GetPlayerName() @ "】。</t><b/>" @
											"<t>盘古之心的入口已经打开，去见守护神将——巨灵神。也许……你不该去……</t><b/>" @
											"<t>你一定要去吗？</t><b/>";
		case 10005:	return "<t>【巨灵神注视着你】</t><b/>" @
											"<t>孩子，还不是时候。</t><b/>" @
											"<t>成神的欲望冲昏了你的心智，你并不在这里，你身置梦境……</t><b/>" @
											"<t>引领你来此地的人，正是你自己的梦魇，那是你心底最可怕的欲望，离他远点！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10005Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在巨灵神的帮助下离开梦境，到万灵城找太白金星</t><b/>";
		case 10002:	return "<t>孩子，有一天，你会真正踏上这片土地。那时候，你会拔出属于你的天神剑，战胜你的梦魇。现在别去，你还不足以挑战他，他会一直在这里等你。</t><b/>" @
											"<t>我将送你离开梦境，到万灵聚集的城池，半神太白金星在那边等你。</t><b/>" @
											"<t>相信我，孩子，我能看到未来，你会回来；而我，将恭迎你步入神界！</t><b/>" @
											"<t>准备好离开梦境，去万灵城了吗？</t><b/>";
		case 10005:	return "<t>你终于醒了，呵呵……</t><b/>" @
											"<t>我是太白金星，此地乃女娲灵气最旺之城——万灵城！</t><b/>" @
											"<t>【太白金星慈和地笑了，看着你】</t><b/>" @
											"<t>欢迎来到万灵城。</t><b/>";
											
	 	case 200:	return $Get_Dialog_GeShi[31204] @ "我已经准备好去万灵城了</t>";
		case 300:	return $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>";										
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}