//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端任务对话，【指引任务】
//==================================================================================

//■■■■■■■学习生活技能■■■■■■■■■■■■■■■■■■■■


//■■■■■■■学习生活技能■■■■■■■■■■■■■■■■■■■■

function Mission22001Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>拜访奇人钓鱼技能师姜子牙</t><b/>";
		case 10002:	return "<t>小神仙，你可知道这个世界里，有很多不世出的奇人呢，他们会的技巧五花八门，神仙都不会，大致分为 钓鱼 狩猎 采药 种植 挖矿 伐木。学会之后，对修真之路可是获益良多啊！我正好认识一位奇人姜子牙，给你引荐一下吧！</t><b/>";
		case 10005: return "<t>哦，是易火眼的朋友啊，不错！看来他是要我给你一些指导呢！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22002Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>学会钓鱼并钓上几枚河虾</t><b/>";
		case 10002:	return "<t>钓鱼乃修心，与那修道之法有共通之处。你我有缘，我这传家宝给你，拿起它，在此湖抛竿即可。来！先试试身手，钓上二枚河虾即可。</t><b/>" @
											 "<t>学会了钓鱼，装备上钓竿，在钓鱼点周围，点击鱼群即可。鱼竿有耐久，能一直用到损坏为止。</t><b/>";
		case 10005: return "<t>恭喜你，你学会了钓鱼，以后多练练，熟能生巧！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22003Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>拜访奇人-狩猎技能师夏心</t><b/>";
		case 10002:	return "<t>小神仙，这天下之大，奇人多如牛毛，我看你这么诚心好学，给你引荐一个，狩猎技能师夏心，快去拜访她吧！</t><b/>";
		case 10005: return "<t>老渔翁让你来的？好好，看你根骨奇佳，眉目清秀，奇材啊！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22004Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>学会狩猎之术并猎到几只鸡羽</t><b/>";
		case 10002:	return "<t>狩猎乃修身，动如风，不动如风。你我有缘，我这传家宝给你，拿着它去清风滨狩猎点就可以狩猎，带二只鸡羽给我。</t><b/>" @
											 "<t>学会了狩猎，装备上猎弓，在狩猎点里，点击猎物即可。猎弓有耐久，能一直用到损坏为止。</t><b/>";
		case 10005: return "<t>恩，果然不出我所料，你现在只能猎到鸡羽，以后多练练，你会看到神兽的！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22005Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>拜访奇人-采药技能师 何药乌</t><b/>";
		case 10002:	return "<t>这仙游岭如此多的奇珍异果，可惜你我都不懂这采药之术啊!不过我倒是有认识一位奇人何药乌，他会采药之术，要不我给你引荐一下？</t><b/>";
		case 10005: return "<t>恩？御清那牛鼻子让你过来找我？好小子，他倒是到处给我宣传啊！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22006Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>学会采药之术并采到几根兔子草根</t><b/>";
		case 10002:	return "<t>采药乃修性，天下万物皆可为药材，用的好就是药，用的不好就是毒！这是我的药锄，现在传给你，去带两根兔子草根来给我。</t><b/>" @
											 "<t>学会了采药，装备上药锄，找到采集的药草点击即可。药锄有耐久，能一直用到损坏为止。</t><b/>";
		case 10005: return "<t>恩，不错，这兔子草根还是可以的，算是半步入门了吧，以后要多加学习啊！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22007Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>拜访奇人-种植技能师 柳素兰</t><b/>";
		case 10002:	return "<t>虽然天地一鼎炉，万物为药材，但是如果自己掌握了种植之术，就可以随心所欲，生产药材了，甚至是一些连天地都无法生成的奇物。可惜这门奇术我却没有掌握，不过我引荐种植技能师柳素兰与你，她兴许会传授与你！</t><b/>";
		case 10005: return "<t>何药乌推荐你来的？他倒是有自知之明，知道他掌握采药的只是外门而已！凡事还是要靠自己辛苦劳动所获。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22008Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>学会种植之术并种出几只玉米</t><b/>";
		case 10002:	return "<t>种植要有耐心，凡事都有他自己的规律，讲究循序渐进，不可拔苗助长。我这社稷之锄就给与了你，希望你能好好种地，天天向上！等一下拿2只玉米给我看看。</t><b/>" @
											 "<t>学会了采药，装备上药锄，找到采集的药草点击即可。药锄有耐久，能一直用到损坏为止。</t><b/>";
		case 10005: return "<t>恩，不错，这玉米种的很不错，看来你很有种地的天赋啊，哈哈！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22009Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>拜访奇人-伐木技能师 鲁班</t><b/>";
		case 10002:	return "<t>你可知道，这天地有灵气的地方，孕育了很多蕴含灵气的植物，但是如果不善砍伐的话，只能是暴殄天物，使得植物受伤再也不能吸收灵气。如果你要学习这植物砍伐之法，去万灵城找鲁班吧，他是最有经验的伐木人！</t><b/>";
		case 10005: return "<t>哦？你从月幽境来的？那里是伐木人的天堂啊！最有灵气的巨木之所！可惜不识天物不会伐木的人太多了！痛心！！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22010Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>学会伐木之术并砍伐几段棕榈木树皮</t><b/>";
		case 10002:	return "<t>伐木要有力道与技巧。一些上古奇木可比金属还要坚硬，不会伐木休想入木一分。我这把 树木的朋友 送给你，希望你能研究透这斧子的名字，伐木之术即大成！等一下砍2段棕榈木树皮给我吧。</t><b/>" @
											 "<t>学会了伐木，装备上斧子，找到可以伐木的地点点击即可。斧子有耐久，能一直用到损坏为止。</t><b/>";
		case 10005: return "<t>这确实是棕榈木树皮不错，不过你看你砍的这长短不一，木头灵气也接近枯竭，再好好练练吧！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22011Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>拜访奇人-挖矿技能师 铁独山</t><b/>";
		case 10002:	return "<t>说到伐木，不得不提另一门奇术，那就是挖矿，你可知道那些传说中的神兵利器？如果没有好的矿炼制而成，那和凡兵废铁又有什么区别？想学习吗，去找铁独山吧！</t><b/>";
		case 10005: return "<t>鲁班是这么抬举我的吗？啊哈哈哈！挖矿虽然很重要，但是如果没有神木相辅相成，怎么会有神兵利器呢。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission22012Dialog(%Npc, %Player, %Mid, %Tid)
{
	%MissionData = "MissionData_" @ %Mid;

	switch(%Tid)
	{
		case 10001:	return "<t>学会挖矿之术并挖到几块铜矿渣</t><b/>";
		case 10002:	return "<t>传你挖矿之法，到清风滨的矿脉处进行挖掘就行了。现在就去挖掘两块铜矿渣给我吧。</t><b/>" @
											 "<t>学会了挖矿，装备上矿镐，找到可以挖矿的地点点击即可。矿镐有耐久，能一直用到损坏为止。</t><b/>";
		case 10005: return "<t>这确实是铜矿渣不错，再好好练练吧！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


//■■■■■■■学习生活技能■■■■■■■■■■■■■■■■■■■■