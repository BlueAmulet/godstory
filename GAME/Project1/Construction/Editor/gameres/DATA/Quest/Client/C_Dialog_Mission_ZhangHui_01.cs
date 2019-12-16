//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端任务对话，【章回任务】第一卷
//==================================================================================

//【章回任务】
//第一编号为【卷】，每一张地图为一卷，每一卷的任务编号间隔数为200，第二编号段为【章】，每一卷暂定为三章，每一章的任务编号间隔数为40，附章间隔为20，最后多余20个编号。
//目前编号段规划，最大可至24卷，24*3=72章，即可以满足72张地图所需的章回任务，视后期任务需求，可挪用一部分的支线任务编号段

//■■■■■■■特殊任务■■■■■■■■■■■■■■■■■■■■
function Mission10000Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>人物达到10级后，与太白金星谈话</t><b/>";
		case 10002:	return "<t>万灵城是轩辕中州的核心，也是人界灵气最充沛的地方，所谓汇万灵之息，纳众生之气。到处看看吧，感受这个地大物博的世界！顺便在此历练，扬名万灵城，达到10级后去找太白金星，他会给你指引的！</t><b/>";
		case 10005:	return "<t>很好，【" @ %Player.GetPlayerName() @ "】，你的名气已经在万灵城里传开了啊！</t><b/>";
			
	
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10001Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>灵兽达到10级后，鉴定成长</t><b/>";
		case 10002:	return "<t>小家伙，你可知道每一只灵兽都是天地灵气孕育而生。但是一样米养百样人，灵兽本身也有不同的成长资质。一开始还看不出来，但是到了10级后，你就可以带你的宠物来找我，我就帮你鉴定一下这个宠物成长如何。</t><b/>";
		case 10005:	return "<t>看到了吧，你的宠物成长的还不错哦。以后你会有许多的灵兽伙伴的，当它们达到10级时，你都可以来找我鉴定哦。</t><b/>";
			
		case 201:	return "<t>小家伙，你可知道每一只灵兽都是天地灵气孕育而生。但是一样米养百样人，灵兽本身也有不同的成长资质。一开始还看不出来，但是到了10级后，你就可以带你的宠物来找我，我就帮你鉴定一下这个宠物成长如何。</t><b/>";	
		case 210: return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "接受任务</t>"; 
		case 300: return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "关闭</t>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}
//■■■■■■■特殊任务■■■■■■■■■■■■■■■■■■■■

//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■

function Mission10100Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小伙子";
	else
		%Sex = "小姑娘";

	switch(%Tid)
	{
		case 10001:	return "<t>去翻动天书看看吧</t><b/>";
		case 10002:	return "<t>你看起来好多了，呵呵。</t><b/>" @
											"<t>【太白金星打量着你】</t><b/>" @
											"<t>" @ %Sex @ "，你想日后登上须弥山巅，跨过盘古之心，化身成神，此地就是起点。</t><b/>" @
											"<t>一步步来，万灵城有许多稀奇古怪的东西，比如我身边这本漂浮的书……</t><b/>" @
											"<t>去翻翻看吧，你会有意想不到的收获。</t><b/>";
		case 10005:	return "<t>刚翻动书卷，一部天书飘然落入你手中。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10101Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>找太白金星谈谈，开始了解万灵城</t><b/>";
		case 10002:	return "<t>尽管书中提到了许多关于修真的帮助，但关于万灵城依然有许多不明白的地方，还是去找太白金星谈谈吧。</t><b/>";
		case 10005:	return "<t>【太白金星对你笑了笑】</t><b/>" @
											"<t>拿天书实则是对你的第一个考验。</t><b/>" @
											"<t>天书与一些神兵类似，皆会认主，能拿到天书，你在此后的修真路肯定事半功倍。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10102Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>找荷仙姑谈谈关于莲花大殿的事情</t><b/>";
		case 10002:	return "<t>现在开始了解万灵城吧。</t><b/>" @
											"<t>我身后这座莲花大殿，它直接吸取来自地底的灵力，从而具备了极强的灵能，众多仙家都在调节它的灵力，以发挥更多的作用。</t><b/>" @
											"<t>关于这点，你到大殿西侧去找护阵灵师荷仙姑，他会告诉你其中妙趣。</t><b/>";
		case 10005:	return "<t>你是新来的修真者？看起来还不错。</t><b/>" @
											"<t>一条灵力丰沛的地脉从大殿之下川流而过，故而整座大殿灵能汹涌，使得万灵城熠熠生辉。</t><b/>" @
											"<t>我等在此调节大殿的灵力，使它具备一些神奇的功能，比如……传送。</t><b/>" @
											"<t>【荷仙姑看着大殿微笑】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10103Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到缺位的阵法位置上引导莲花台，然后和荷仙姑谈谈</t><b/>";
		case 10002:	return "<t>此阵乃灵元大阵，如今一名弟子灵息不稳，此刻缺位不在，阵中空缺了一处，需要有人顶替片刻。</t><b/>" @
											"<t>你能帮帮忙吗？到空缺的位置，用灵力引导莲花台，如此整个大阵就能正常运作了。</t><b/>";
		case 10005:	return "<t>灵力泄漏……好像弄糟了，大阵有些不稳，所有的弟子都被灵力反噬了！</t><b/>" @
											"<t>虽然你有足够的修为，但毕竟不熟悉这个阵法，唉……我应该多给你说清楚些的。</t><b/>";
	  case 200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "开启莲花台</t>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10104Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>为了弥补漏洞，你要去一趟女娲神像的体内，先去找铁匠炎锤谈谈</t><b/>";
		case 10002:	return "<t>灵元大阵的每一个位置都与施法者息息相关，这次泄漏由你而起，也只能靠你去修补。</t><b/>" @
											"<t>要注意，地脉的灵力源于女娲神像，最后你恐是要去一趟神像体内……那个世界也许不太安全。</t><b/>" @
											"<t>在此之前，你去找铁匠炎锤谈谈，你需要准备一把兵器。</t><b/>";
		case 10005:	return "<t>我听说你的事情了，放心吧，我会帮你的！</t><b/>" @
											"<t>小朋友，你不必太过担忧，女娲娘娘曾经以自己性命给予世间万物的灵力，她不会为难你的。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10105Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从摊位上选择一件自己喜欢的兵器，再和炎锤谈谈</t><b/>";
		case 10002:	return "<t>这里是火神大坝！我旁边这尊神像就是火神祝融！以后你会遇到他的，他是神中之神，火焰之王！</t><b/>" @
											"<t>我太激动了，哈哈，说过会给你一把武器的。兵器分为</t>"@ $Get_Dialog_GeShi[31206] @"刀剑、旗杖、塔镜、刀斧、弓、琴、双短</t><t>这七类，就在我身边的摊位上，看看你更喜欢哪类，取来给我看看。</t><b/>";
		case 10005:	return "<t>嗯……看来你选择了不错的兵器，好眼光！</t><b/>" @
											"<t>哈哈，它会陪你度过险关！</t><b/>";
											
		case 200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "任务：查看摊位</t>";
			
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10106Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>为了拿到必须的防具，你现在要去找防具商人防御</t><b/>";
		case 10002:	return "<t>光有兵器是不行的，你还需要一身精良的装备。</t><b/>" @
											"<t>你要去找防具商人房御，他在水神大坝处。你应该知道，水火二神历来不和，我和他也交往不深，能不能得到他的帮助，就看你自己了。</t><b/>" @
											"<t>快去吧，时间不多……灵元大阵越来越不稳定了。</t><b/>";
		case 10005:	return "<t>谁？炎锤让你来的？快滚快滚！</t><b/>" @
											"<t>我讨厌跟那个火炭一样的家伙打交道！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10107Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从摊位上选择一件自己喜欢的防具，再与房御谈谈</t><b/>";
		case 10002:	return "<t>这里是水神的领域！当年若非水神共工心胸仁慈，怎会败给火神祝融！</t><b/>" @
											"<t>什么？什么？你把灵元大阵弄坏了？你这个蠢家伙！</t><b/>" @
											"<t>唉，你要赶紧去修补，否则整座万灵城都会崩溃！快去摊位上选择一件适合的衣甲，让我看看你能不能用。</t><b/>";
		case 10005:	return "<t>看起来还挺合身！</t><b/>" @
											"<t>我还是不放心，这件护腕也给你，一定要修补好灵元大阵！</t><b/>";
											
		case 200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "任务：查看摊位</t>";									
																			
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10108Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>装备和兵器都准备好了，去找太白金星吧！</t><b/>";
		case 10002:	return "<t>你还站在这里干什么！时间不多了，你要进入神像的体内，那是一个神秘的世界，知道吗！</t><b/>" @
											"<t>擦亮你的兵器，穿好装甲，赶紧去找半神太白金星！</t><b/>";
		case 10005:	return "<t>荷仙姑已经给我说过了，你要弥补灵元大阵的漏洞，就一定要进入灵力的源泉处——女娲神像体内。</t><b/>" @
											"<t>那里将是一个广袤无垠的世界，要小心应付啊！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10109Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>进入女娲神像体内，杀掉弟子心魔和灵元心魔，再和太白金星的幻象谈谈</t><b/>";
		case 10002:	return "<t>看到女娲神像前面的石碑了吗？用心去诵念，你就会进入另一个世界。</t><b/>" @
											"<t>我会以幻象之态随你进入，你会看到灵元大阵上的每一个人，他们都是受到灵力反噬的心魔，什么都不要管，将他们的心魔统统除掉！</t><b/>" @
											"<t>我会在里面指引你。</t><b/>";
		case 10005:	return "<t>成功了，你修补了灵元大阵的漏洞，解除了所有弟子的心魔。</t><b/>" @
											"<t>休息片刻，我们准备离开这里</t><b/>";
											
	 	case 200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "进入女娲神像体内</t>";
		case 300:	return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10110Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>传送到万灵城后，去找荷仙姑，告诉她大阵已经修补</t><b/>";
		case 10002:	return "<t>呵呵，你看起来好多了，我们准备出去吧。</t><b/>" @
											"<t>到了万灵城，你要去找荷仙姑，她知道大阵已经恢复正常会很开心的。</t><b/>" @
											"<t>准备好了吗？我们回到万灵城。</t><b/>";
		case 10005:	return "<t>我感觉到了，灵元大阵已经恢复了原先的秩序。</t><b/>" @
											"<t>你做的很好，朋友。坚持不懈，你会成为一个威风八面的修真者！</t><b/>";
											
		case 200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我已经准备好去万灵城了</t>";
		case 300:	return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>";									
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10111Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着妖族羽毛去找宠物商人苏苏</t><b/>";
		case 10002:	return "<t>我差点忘了感谢你，毕竟是你拯救了灵元大阵。</t><b/>" @
											"<t>给，拿着这根羽毛，去城东找宠物商人苏苏，她会对你有很大帮助的。</t><b/>" @
											"<t>【荷仙姑神秘地一笑】</t><b/>" @
											"<t>你应该知道，一个杰出的修真者，身边往往有忠心耿耿的护主灵兽。苏苏是这方面的行家。</t><b/>";
		case 10005:	return "<t>咦，你有妖族的羽毛呀？好漂亮！</t><b/>" @
											"<t>嘻嘻，苏苏就是妖族，妖族都是由动物幻化而成哦。这样吧，你把漂亮羽毛给我，我就帮你找一只灵兽。怎样，很划算吧！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10112Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>抓捕到一只灵兽后去找苏苏</t><b/>";
		case 10002:	return "<t>灵兽往往都很可爱，当然也有凶戾恐怖的，不过它们对主人都很忠诚。</t><b/>" @
											"<t>我知道一个叫小灵谷的地方，那里漫山遍野都是灵兽！嘻嘻，我会送你过去啦。</t><b/>" @
											"<t>记住哦，一定要用我给你的捆灵索来抓捕灵兽，否则它们永远不会认你做主人。</t><b/>" @
											"<t>捕捉成功后与传送师夏高俊对话，他会将你送回来。</t><b/>";
		case 10005:	return "<t>这个小家伙真可爱，苏苏喜欢^_^</t><b/>" @
											"<t>要好好照顾它哦，别让它饿着。世界各地的宠物商店里都有卖宠物饲料，很便宜啦，让小家伙吃得饱饱的！</t><b/>";
											
	  			
		case 200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "传送去灵谷捕兽</t>";
		case 300:	return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "等一下再去捕兽</t>";
				 									
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10113Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>服用苏苏给你的变身丹后，去找神医李时珍</t><b/>";
		case 10002:	return "<t>你听说过变身丹吗？传说服下这种丹药会变成奇怪的生物，苏苏害怕这种丹药，要是变成丑八怪怎么办？</t><b/>" @
											"<t>你……敢服食吗？服下后去找万灵城的神医李时珍，他也许会知道变身药的功效。</t><b/>" @
											"<t>【苏苏胆怯地看着变身药】</t><b/>" @
											"<t>你真要……吃这个？我可不保证会发生什么事……</t><b/>";
		case 10005:	return "<t>哈哈，苏苏那小丫头，变身丹乃是一种稀罕的药品，能让人变幻为不同的形态，并具备一些特殊特性。刚才你不觉跑得很快？</t><b/>" @
											"<t>我这里还有一些，就送给你吧。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10114Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到杂货商人鬼生财处购买一把耕药镰刀，再采集一些仙鹤草后向李时珍回报</t><b/>";
		case 10002:	return "<t>年轻人，既然收了老头的礼物，也帮我做点事吧。</t><b/>" @
											"<t>你可知仙丹仙草，都需要稀罕的草药，刚好万灵城中生长了一种仙鹤草，是制作仙丹的材料。但它需要用特殊的耕药镰刀才能采集，你能跑一趟杂货商人鬼生财处，买一把耕药镰刀，再帮我采集一些仙鹤草吗？</t><b/>";
		case 10005:	return "<t>呵呵，年轻人动作就是麻利，这么快就办妥了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10115Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小兄弟";
	else
		%Sex = "小姑娘";

	switch(%Tid)
	{
		case 10001:	return "<t>带着李时珍的书信去找福星，请求得到他的帮助</t><b/>";
		case 10002:	return "<t>炼丹一事，光有草药还不足，我们还需要灵气来炼制……</t><b/>" @
											"<t>一步一步来，年轻人。万灵城的福星是个奇妙的人，传说他富可敌国，拥有的宝贝比谁都多，你要去问他要一件可以收集灵气的宝贝。</t><b/>" @
											"<t>带着这封书信去找他，他应当会伸以援手。</t><b/>";
		case 10005:	return "<t>哎哟！这是神医的书信啊！神医有事，老夫定然鼎力相助！</t><b/>" @
											"<t>呵呵，" @ %Sex @ "，当年我身染重疾，全靠神医妙手回春，此恩我一直记在心上。</t><b/>" @
											"<t>来，老夫先送你一份见面礼！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10116Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>用紫金葫芦在日月池内吸取灵气，再将装满的葫芦带给福星</t><b/>";
		case 10002:	return "<t>在莲花大殿东侧的日月池，灵力十分充沛，许多修真者都会在那里提取灵气。</t><b/>" @
											"<t>这是能吸取灵气的紫金葫芦，你到达日月池后，对那些漂浮的灵气使用，即可将之吸入葫芦内。</t><b/>";
		case 10005:	return "<t>这一葫芦日月灵气，想必够用了吧。</t><b/>" @
											"<t>来，这些礼物我用不着了，送给你吧，老夫我一向以大方闻名，哈哈哈！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10117Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小兄弟";
	else
		%Sex = "小姑娘";

	switch(%Tid)
	{
		case 10001:	return "<t>将日月灵气送给李时珍</t><b/>";
		case 10002:	return "<t>将这些灵气带给神医吧，" @ %Sex @ "，这一路辛苦你了。 </t><b/>" @
											"<t>在你以后的修真路上，还有困难重重，若是什么难处，随时来找我。</t><b/>";
		case 10005:	return "<t>福星果然不同凡响，连灵气也能轻易取来。</t><b/>" @
											"<t>呵呵，有了这些灵气，我就能炼制仙丹了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10118Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去找太白金星进行试炼</t><b/>";
		case 10002:	return "<t>在你取灵气的时候，太白金星在找你，好像是有什么试炼要进行，这事可不能含糊。</t><b/>" @
											"<t>赶紧去找他吧。</t><b/>";
		case 10005:	return "<t>不管是修补灵元大阵这样的要事，还是送一封信这样的琐事，你都从不推卸。你正是我们所追寻的人。</t><b/>" @
											"<t>我知道你想修炼成至高无上的神，但这是一条坎坷长路，让我们慢慢来吧。</t><b/>" @
											"<t>首先，你需要经历一场试炼，若你能通过，就会得到八大门宗的认可，学习正宗的修神技能。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10119Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>回答太白金星提出的问题，完成心灵的试炼</t><b/>";
		case 10002:	return "<t>首先，是心灵的试炼。</t><b/>" @
											"<t>在这个过程中，我会询问你一些问题，以考验你对万灵城的了解和对一些物品的使用。</t><b/>" @
											"<t>请谨慎面对这次试炼，若回答错误，可能会有意想不到的惩罚。你有充足的时间的准备，【" @ %Player.GetPlayerName() @ "】，别太着急。</t><b/>";
		case 10005:	return "<t>恭喜你通过心灵试炼，非常难得，稍作休息，我们将进行下一步试炼。</t><b/>";
		
		case 200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "任务：接受太白金星的考验</t>";
		case 210:	return $Get_Dialog_GeShi[41601] @ "宠物商人苏苏是什么种族？</t>";
		case 211:	return "妖";
		case 212:	return "精";
		case 220:	return $Get_Dialog_GeShi[41601] @ "购买武器应当找何人？</t>";
		case 221:	return "炎锤";
		case 222:	return "剑神";
		case 230:	return $Get_Dialog_GeShi[41601] @ "查看任务快捷键是什么？</t>";
		case 231:	return "Alt+Q";
		case 232:	return "Alt+E";
				     
		case 310:	return $Get_Dialog_GeShi[41601] @ "莲花大殿的灵力源泉来自于何处？</t>";
		case 311:	return "女娲神像";
		case 312:	return "灵元大阵";
		case 320:	return $Get_Dialog_GeShi[41601] @ "购买药品应当找何人？</t>";
		case 321:	return "李时珍";
		case 322:	return "福星";
		case 330:	return $Get_Dialog_GeShi[41601] @ "查看人物属性的快捷键是什么？</t>";
		case 331:	return "Alt+C";
		case 332:	return "Alt+X";

		case 410:	return $Get_Dialog_GeShi[41601] @ "火神祝融曾经与哪个大神有过战争？</t>";
		case 411:	return "水神共工";
		case 412:	return "火龙炎啸";
		case 420:	return $Get_Dialog_GeShi[41601] @ "防御商人房御不卖以下什么物品？</t>";
		case 421:	return "轻甲";
		case 422:	return "神仙盾";
		case 430:	return $Get_Dialog_GeShi[41601] @ "查看包裹属性的快捷键是什么？</t>";
		case 431:	return "Alt+A";
		case 432:	return "Alt+S";
		
		case 500: return "很好，看来你都融会贯通了。";

		case 998: return "不对吧，你再仔细想想……";	
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10120Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>用轻功登上南天柱顶端，再走进那个可怕的牢笼</t><b/>";
		case 10002:	return "<t>现在你要通过第二个试炼——体魄的试炼。</t><b/>" @
											"<t>在万灵城的南城区，有一根极高的石柱，叫做南天柱。南天柱由数条破碎的栈道连接，直至柱顶。</t><b/>" @
											"<t>当你到达顶端，还没完。你会发现一个可怕的牢笼，鼓起你的勇气，走进去！</t><b/>";
		case 10005:	return "<t>干得好，【" @ %Player.GetPlayerName() @ "】，你又通过了一项试炼。你时时刻刻都在变得强大。</t><b/>";
			
		case 200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我已经准备好送我过去了</t>";
		case 300:	return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>"; 	
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10121Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>进入万书楼，杀了书楼傀儡和司徒尚，取回《女娲与八大种族》交给太白金星</t><b/>";
		case 10002:	return "<t>你与我在一起的时光就要结束了，很高兴你能走到这一步。接下来是最后的试炼，说是试炼，同时也需要你帮助万灵城。</t><b/>" @
											"<t>我要你寻找一本书——《女娲与八大种族》。这本书曾属于万灵城，但被叛徒洪雷风盗走，如今他藏在神秘的万书楼中。</t><b/>" @
											"<t>万书楼被司徒尚用结界藏了起来，我会将你送入结界，杀了他和他的傀儡，把书带回来！</t><b/>";
		case 10005:	return "<t>我很久没有看过这本书了，谢谢你让它又回到了万灵城。</t><b/>" @
											"<t>【" @ %Player.GetPlayerName() @ "】，你证明了自己，完成了所有的试炼，这些东西是你应得的。</t><b/>";
											
		case 200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我已经准备好去万书楼</t>";
		case 300:	return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>"; 
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10122Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>与八大门宗的接引者谈谈，以了解八个门宗，之后与太白金星对话</t><b/>";
		case 10002:	return "<t>勇敢的【" @ %Player.GetPlayerName() @ "】，漫漫修神路已经向你敞开。</t><b/>" @
											"<t>在我身边，有八大门宗的接引者，从左至右分别是</t>"@ $Get_Dialog_GeShi[31206] @"圣族昆仑宗、仙族蓬莱派、鬼族九幽教、精族飞花谷、怪族山海宗、妖族幻灵宫、佛家雷音寺和魔族落夕渊。</t><b/>" @
											"<t>八族曾各自得到过一句女娲真言，有最正宗的修炼方法。你去了解一下各门宗的特点，再来找我，决定你将拜入的门宗。</t><b/>";
		case 10005:	return "<t>怎么样，确定你要选择哪个门宗了吗？</t><b/>";
			
		case 200: return $Icon[2] @ $Get_Dialog_GeShi[31204] @"八大门宗</t>";			
		case 201: return "神木林是个好地方。";	
		case 202: return "幽冥鬼域是个好地方。";	
		case 203: return "蓬莱仙境是个好地方。";	
		case 204: return "昆仑古虚是个好地方。";
		case 205: return "双生山是个好地方。";
		case 206: return "醉梦冰池是个好地方。";
		case 207: return "极乐西天是个好地方。";	
		case 208: return "落夕渊是个好地方。"	;													
		case 209: return $Icon[2] @ $Get_Dialog_GeShi[31204] @"我已经知道要加入那个门宗了。</t>";
		case 300: return $Icon[2] @ $Get_Dialog_GeShi[31204] @"我还是不清楚，能帮助我指引下吗？</t>";
			
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10123Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从女娲神像处可传送至八大门宗，到你选中的门宗去，与门宗首领交谈，拜入该门宗</t><b/>";
		case 10002:	return "<t>祝贺你升到10级，现在到你选择的门宗去，晋见他们的首领，拜入门宗。</t><b/>" @
											"<t>【" @ %Player.GetPlayerName() @ "】，你是我所见过罕有的奇才，希望在此后的修神路中，你能克服万难，化身成神！</t><b/>" @
											"<t>去吧，去拜女娲神像，她会将你送往你要去的门宗。</t><b/>";
		case 10005:	return "<t>【" @ %Player.GetPlayerName() @ "】，万灵城将你的事迹传得沸沸扬扬。如今你确定拜入吾之门宗？一旦加入，则不可更改，你要考虑清楚。</t><b/>";
			
	  case 200: return $Icon[2] @ $Get_Dialog_GeShi[31204] @"若不愿意,吾送你去其他门派.</t>";
		case 201:	return $Icon[11] @ $Get_Dialog_GeShi[31204] @"传送至【昆仑宗(圣)】</t>";
		case 202:	return $Icon[11] @ $Get_Dialog_GeShi[31204] @"传送至【金禅寺(佛)】</t>";
		case 203:	return $Icon[11] @ $Get_Dialog_GeShi[31204] @"传送至【蓬莱派(仙)】</t>";
		case 204: return $Icon[11] @ $Get_Dialog_GeShi[31204] @"传送至【飞花谷(精)】</t>";
		case 205:	return $Icon[11] @ $Get_Dialog_GeShi[31204] @"传送至【九幽教(鬼)】</t>";
		case 206:	return $Icon[11] @ $Get_Dialog_GeShi[31204] @"传送至【山海宗(怪)】</t>";
		case 207:	return $Icon[11] @ $Get_Dialog_GeShi[31204] @"传送至【幻灵宫(妖)】</t>";
		case 208:	return $Icon[11] @ $Get_Dialog_GeShi[31204] @"传送至【天魔门(魔)】</t>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10124Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去找门宗技能师，准备学习入门技能</t><b/>";
		case 10002:	return "<t>刚入门宗，很多地方和规矩你不熟，多跑跑看看就好了。</t><b/>" @
											"<t>吾宗继承女娲真诀，乃正宗大道。这套衣装乃本门标志，现在我将之给你，望你此后好生修炼，踏入神界。</t><b/>" @
											"<t>此刻你去找技能师吧，他会传授你一些本门的入门功法。</t><b/>";
		case 10005:	return "<t>你就是新来的门宗弟子吧，果然一表人才。怎样，对这里还习惯吧。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10125Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>学习门宗技能，然后去击打一旁的木桩，将之击碎为止，再找技能师谈谈</t><b/>";
		case 10002:	return "<t>不管你如何，如今你需要门宗的技能，一切从头开始。</t><b/>" @
											"<t>昆仑宗的技能犹如苍龙，勇猛无比！当你学会技能之后，就去对旁边的木桩试着使用，直到击碎三个木桩为止。这方便你熟悉新的技能。</t><b/>";
		case 10005:	return "<t>不错，随着你能力的提升，要经常回来学习更多的新技能。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10126Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去找任务师谈谈吧</t><b/>";
		case 10002:	return "<t>我暂时没有什么好教你的了，随着你对本门法术的了解和熟悉，慢慢我会传授你更多的技法。</t><b/>" @
											"<t>现在你去找任务师，他会帮助你成为一个杰出的本门弟子！</t><b/>";
		case 10005:	return "<t>吾已听说过你了，新晋弟子【" @ %Player.GetPlayerName() @ "】。门宗任务会帮助你提高宗内的声望，同时亦可获得一些不错的奖励和经验。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10127Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "师弟";
	else
		%Sex = "师妹";

	switch(%Tid)
	{
		case 10001:	return "<t>到万灵城去找接引使，解决他的难题</t><b/>";
		case 10002:	return "<t>修真大道，难免会出现一些诸如经验不足，不知该做什么的情况。</t><b/>" @
											"<t>遇此情形你大可来找我，门宗任务会帮你弥补这些经验，同时还能获得更高的声望和奖励。</t><b/>" @
											"<t>说这么多，你还是先做一件吧，万灵城的接引使有些麻烦事，你去帮帮他怎样？通过传送师即可传送到万灵城，速速去吧。</t><b/>";
		case 10005:	return "<t>原来是你，你加入我宗了啊！那我们就是同门了，哈哈！</t><b/>" @
											"<t>眼下有些事还请" @ %Sex @ "帮忙，请先拿好这把兵器，一切都要从兵器说起。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10128Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去找鉴定师易火眼鉴定武器</t><b/>";
		case 10002:	return "<t>你知道鉴定师吗？那是一个很神奇的职业，他们能轻易分辨出一样物品的优劣，无论任何东西！</t><b/>" @
											"<t>刚才给你的兵器，由我亲自铸造，每次我都会去鉴定，但似乎从未出现过神兵。唉，如今鉴定师见我就躲，你能代我去鉴定下吗？不管结果如何，最后这把兵器都归你了。</t><b/>";
		case 10005:	return "<t>嘿！要鉴定物品吗？但凡好物，往往外表一般，内藏金玉，不经过鉴定是体现不出价值的！</t><b/>" @
											"<t>哈哈哈，我正闲得发慌，把东西给我看看。</t><b/>" @
											"<t>你别在旁边打扰我，这件防具拿去，在一旁安静等等，马上就鉴定好了！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}
//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■第二章■■■■■■■■■■■■■■■■

function Mission10166Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>把消息告诉万灵城的朱萱。</t><b/>";
		case 10002:	return "<t>邪鬼已除，肖广川却再也不能醒转来，唉，生死两茫茫啊。</t><b/>" @
											 "<t>将这个消息告诉朱萱吧，我也不知这算是喜讯还是什么，人总是已离去了。</t><b/>" @
											 "<t>唉，我送你一程，你我就此别过，来日有缘再会。</t><b/>";
		case 10005:	return "<t>呜呜……谢谢你，你是我们的恩人，朱萱此生永记。</t><b/>" @
											 "<t>广川……广川……你知道了吗？你的仇已经报了，安息吧……</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


function Mission10165Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在中屋门口使用辟邪黑狗血，引出邪鬼小王，杀掉它，拿它的头颅向夏梦狼回报</t><b/>";
		case 10002:	return "<t>黎明的曙光已经接近，肖广川的仇恨、清风滨的和平，即将了结。握紧手中的兵器，结束清风滨的苦难！</t><b/>" @
											 "<t>邪鬼小王就藏身在中屋，在中屋泼洒黑狗血，逼出它，拧下它的头颅！邪终不胜正！</t><b/>";
		case 10005:	return "<t>你做到了！夏梦狼对你佩服得五体投地，这实在是……太让人振奋了！清风滨的恶鬼，在你的手下终于灰飞烟灭！</t><b/>" @
											 "<t>朋友，你的事迹将会被人们永远铭记，清风滨的百姓会永远记住你的恩泽！</t><b/>" @
											 "<t>天近拂晓，我们回去吧。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}



function Mission10164Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在西屋门口使用辟邪黑狗血，引出西屋小鬼头，杀掉它，拿它的头颅向夏梦狼回报</t><b/>";
		case 10002:	return "<t>休息好了吗？接下来要死的是东屋小鬼头。那邪鬼手拿鬼头杖，擅使鬼术，阴毒狠辣，千万要小心啊！</t><b/>" @
											 "<t>你依然需要用黑狗血引出它，然后带它的头颅回来！</t><b/>";
		case 10005:	return "<t>神明保佑，西屋小鬼头也除掉了，你的能力，实在不可估量……</t><b/>" @
											 "<t>好好休整，准备迎接最后的邪鬼。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}



function Mission10163Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在东屋门口使用辟邪黑狗血，引出东屋小鬼头，杀掉它，拿它的头颅向夏梦狼回报</t><b/>";
		case 10002:	return "<t>除掉了那些小鬼，接下来我们要解决他们的头目。这些头目都深藏在鬼村中，个个凶残无比，若群起而攻之，则是九死一生之局。哼，我们就逐个击破！</t><b/>" @
											 "<t>你先去除掉东屋小鬼头。那恶鬼据说力大无比，凶狠嗜杀，需格外谨慎。拿着这桶黑狗血，你在东屋门口泼下此血，其血之力会逼它现身，并削弱它的力量，之后杀掉它！把它的头颅带回来！</t><b/>";
		case 10005:	return "<t>哼！这就是那恶心的邪鬼头颅！</t><b/>" @
											 "<t>我们离清风滨的和平已经走近了一大步，休整休整，准备迎接下一个邪鬼。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}



function Mission10162Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉几个清风小鬼和几个清风鬼巫后，向夏梦狼回报</t><b/>";
		case 10002:	return "<t>据我平日的探查，邪鬼在此地有三处据点，分别是东屋，西屋和中屋。你先去除掉进入鬼村一路上的清风小鬼和清风鬼巫，开出一条路来！</t><b/>" @
											 "<t>这些恶鬼手上血迹斑斑，千万不要留情！</t><b/>";
		case 10005:	return "<t>做的好！这些邪鬼死有余辜！好好休整，一切才刚刚开始。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}



function Mission10161Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>与夏梦狼进入鬼村</t><b/>";
		case 10002:	return "<t>黑夜将至，你可准备好与我共赴鬼村？</t><b/>" @
											 "<t>此去一路凶多吉少，你我二人或将面对最大的凶险。但为了清风滨的和平，为了惨死的肖广川的仇恨，你会惧怕这样的凶险？你会拒绝获得这样的荣耀？</t><b/>" @
											 "<t>若一切就绪，就与我一同碾平鬼村！</t><b/>";
		case 10005:	return "<t>月已中天，清风邪鬼应已出没，此地危机四伏，我们当小心行事。</t><b/>";

		case  200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我准备好了</t>";
		case  300:	return $Icon[4] @ $Get_Dialog_GeShi[31204] @"<t>稍等片刻，我还需修整片刻</t>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10160Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀死鬼鼬首领和鬼兔首领，将辟邪黑狗血泼在它们身上，再回报夏梦狼。</t><b/>";
		case 10002:	return "<t>奇怪，这黏液似乎不能盖房子啊，倒是与黑狗血放在一起后，有了一种奇怪的力量。我叫他辟邪黑狗血，专克邪鬼，哈哈，是不是很奇特！</t><b/>" @
											 "<t>【你感觉所谓盖房是被这老家伙骗了】</t><b/>" @
											 "<t>现在让我们试试黑狗血的效果，将血泼在鬼兔首领和鬼鼬首领身上，看看会发生什么？杀掉他们后，你就可以去找夏梦狼了。</t><b/>";
		case 10005:	return "<t>清鸿仙人让你带来了辟邪黑狗血？真是太好了！有此物，我们定能杀尽清风滨的恶鬼，为民除害，为肖广川报仇！</t><b/>" @
											 "<t>朋友，静候黑夜，我们就去那鬼村大闹一场！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10159Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀死灵螺，从它们身上带几份灵螺黏液给清鸿仙人</t><b/>";
		case 10002:	return "<t>哎呀哎呀，爷爷我一时急切，忘记了大事！！！</t><b/>" @
											 "<t>这盖房光是木头稻草怎么成，我还需要一种黏液，有一种灵螺黏液能将木桩黏合的密不透风，根本无需铁钉凿子，十分神奇！</t><b/>" @
											 "<t>嘿嘿，你还不快去搞点灵螺黏液来，门口不远处就有许多灵螺，快去快回啊！</t><b/>";
		case 10005:	return "<t>对对对，就是这东西！嘿嘿，小家伙你帮了爷爷我大忙，我定然会出手相助！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10158Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>收集几份新鲜的木头和几份稻草，带给清鸿仙人</t><b/>";
		case 10002:	return "<t>咳咳……刚才爷爷我有点失态啊，这个嘛，谁让你身上那么臭！爷爷我睡得好好的，活生生被你个小混蛋熏醒！</t><b/>" @
											 "<t>原来你身上染了许多污秽鬼气，臭臭臭！臭气熏天！</t><b/>" @
											 "<t>要让我帮忙就先为我做点事，你看我住的地方，又破又烂，哪里符合我的身份？给我带一些新鲜的木头和稻草回来，爷爷首先要有房子住，才能帮你捉鬼啊！</t><b/>";
		case 10005:	return "<t>哟呵呵呵！小家伙还不赖嘛，把爷爷我哄开心了，那可是一本万利之事啊！哈哈哈，你在这里听听雨落虫鸣，赏赏秋月春风，爷爷我盖房子去！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10157Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀死几只邪鬼黄鼬，几只邪鬼凶兔后，到废弃村舍找清鸿仙人</t><b/>";
		case 10002:	return "<t>在入夜之前，我们要准备许多事。朋友，现今你需前往清风村废墟，探查当地的情况和地势。那里是鬼气最浓烈之地，我虽不才，亦能知道那处的妖邪非普通怪物可比。</t><b/>" @
											 "<t>为方便夜晚行事，我们要除掉白天盘桓的妖怪。去杀掉那里的邪鬼黄鼬和邪鬼凶兔，然后去废弃村舍找清鸿仙人。要除鬼，我们必须得到他的帮助。</t><b/>";
		case 10005:	return "<t>好臭的鬼气！！！</t><b/>" @
											 "<t>臭小鬼！好个小鬼，敢闯入你清鸿爷爷的地盘，简直是活够了！简直是胆大妄为！简直是混帐！简直是不尊不敬不……</t><b/>" @
											 "<t>咦？原来不是邪鬼啊……咳咳……</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10156Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>把所看的梦境告诉夏梦狼</t><b/>";
		case 10002:	return "<t>现在我传你回夏梦狼处，你告诉他所见所闻，他知道该如何行事。</t><b/>" @
											 "<t>我们不能让肖广川白白牺牲。</t><b/>" @
											 "<t>五灵贯通，天地游梦，去吧！</t><b/>";
		case 10005:	return "<t>看过梦境可知，那些恶鬼原来只在半夜出现，哼，如今离夜晚尚有一段时间，我们要好好准备准备！</t><b/>";

		case 200:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我准备好去找夏梦狼了</t>";
		case 300:	return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10155Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>看过梦境之后，与任惊梦谈谈</t><b/>";
		case 10002:	return "<t>朋友，肖广川不会白死。你的手上沾染了他的气息，我虽无什么大的能耐，但妖之一族，最擅游梦，我能将他的气息做成梦境。肖广川事发当夜的所见便能重现，让我们看看清风村废墟究竟发生了什么！</t><b/>";
		case 10005:	return "<t>看清楚了！清风村的恶鬼，就是它们残忍杀死了肖广川，还以此为乐！</t><b/>" @
											 "<t>记住它们丑陋的脸，我们要将仇恨十倍奉还！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10154Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>找到万灵城的任惊梦，与他谈谈鬼村的事。</t><b/>";
		case 10002:	return "<t>身为人妻，我本该以死相随，但我腹中已有广川的骨肉，我怎能让他未临世便……</t><b/>" @
											 "<t>广川生前嫉恶如仇，他痛恨清风滨的恶鬼，才只身前往调查。待我们孩子出世，我会跟他说，他的父亲是一个好人。</t><b/>" @
											 "<t>广川生前和幻灵宫的任惊梦走得很近，他们似乎都在调查鬼村的事情，你去找他吧，你们……一定要为我丈夫报仇！朱萱千恩万谢，呜呜……</t><b/>";
		case 10005:	return "<t>什么！肖广川他！可恨，可恨的清风恶鬼！</t><b/>" @
											 "<t>肖广川为人宽厚，他是为了清风滨的和平才付出生命啊，唉，这世上又有谁知呢？肖广川，你真是笨啊……</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10153Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>把绢巾带给万灵城的朱萱</t><b/>";
		case 10002:	return "<t>朋友，也许你能体会失去挚爱的伤痛，也许你能明白这种悲伤会留在心头，隐隐作痛。活着的人背负了死者的痛苦，日出日落，怎会消逝一分？</t><b/>" @
											 "<t>唉，你将这条绢巾带给万灵城的朱萱吧，我们……还是要将一切告诉她……</t><b/>" @
											 "<t>此路遥远，我驾驭清风送你一程。</t><b/>";
		case 10005:	return "<t>这位侠士，你是？找小女子何事？</t><b/>" @
											 "<t>【接过绢巾，阅读之间双目热泪滚滚而下，而后泣不成声】</t><b/>" @
											 "<t>广川……广川……我的广川，你怎舍得离我而去……呜呜……</t><b/>" @
											 "<t>你留下我独自一人，呜呜……独自一人……</t><b/>";

		case 200:	return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我已经准备好去万灵城了</t>";
		case 300:	return  $Icon[4] @ $Get_Dialog_GeShi[31204] @"<t>稍等片刻，我还需修整片刻</t>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


function Mission10152Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>将肖广川的书信带回夏梦狼处</t><b/>";
		case 10002:	return "<t>看来这是肖广川遇害前写给他妻子的亡笔。若他妻子得知，该是何等伤心……</t><b/>" @
											 "<t>唉，还是先将绢巾带给夏梦狼看看吧。</t><b/>";
		case 10005:	return "<t>肖广川已经……唉……只留下这条绢巾吗？</t><b/>" @
											 "<t>【夏梦狼缓缓读完书信，手已将绢巾捏成一团】</t><b/>" @
											 "<t>恶鬼！荼毒人命！肖广川不过一平民百姓，也遭此毒手，简直罪无可赦！</t><b/>" @
											 "<t>我们必须调查出此事的罪魁祸首，让其偿命！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10151Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>查看带血的绢巾写有什么</t><b/>";
		case 10002:	return "<t>如此暴尸荒野，还是将他埋了，入土为安吧。</t><b/>" @
											 "<t>在整理肖广川的尸体时，你发现他手中紧紧拽着一条绢巾，上面血迹斑斑，或许写了什么。</t><b/>" @
											 $Get_Dialog_GeShi[31206] @ "打开包裹，右键点击带血的绢巾，看查看其中内容。</t><b/>";
		case 10005:	return "<t>看来这是肖广川遇害前写给他妻子的亡笔。若他妻子得知，该是何等伤心……</t><b/>" @
											 "<t>唉，还是先将绢巾带给夏梦狼看看吧。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10150Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到清风村废墟找到肖广川，调查发生何事</t><b/>";
		case 10002:	return "<t>原来是这些内丹的臭味！待我看看……</t><b/>" @
											 "<t>嗯……此乃鬼力，但与九幽教的纯正鬼力不同，是一种污秽邪恶的灵力。我早怀疑此地闹鬼，错不了！</t><b/>" @
											 "<t>【"@%Player.GetPlayerName()@"】，闹鬼的源头应该来自清风村废墟，三天前，万灵城的肖广川去调查村子废墟，但一去之后就失去了讯息，我担心他出事了。你现在去废墟看看，活要见人，死要见尸！</t><b/>";
		case 10005:	return "<t>这……应该就是肖广川吧……</t><b/>" @
											 "<t>真是可怜的人，衣服被完全撕烂，裸露的肌肉变成了黑色，胸腹处有一个可怖的创伤，血液在伤口处凝结成了黑块。</t><b/>" @
											 "<t>谁如此心狠手辣？！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10149Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉大黄，二黄，三黄，把它们的内丹带给夏梦狼</t><b/>";
		case 10002:	return "<t>那些污染力十分邪恶，我猜测正是污染力使清风滨的生物变得暴躁凶恶。</t><b/>" @
											 "<t>【梁盛深深吸了一口气】</t><b/>" @
											 "<t>在你夺粮之时，有没有看到三个黄鼬首领，它们分别是大黄，二黄，三黄，它们是受污染最强的黄鼬妖之一，杀死他们，把他们的内丹带给夏梦狼，污染严重的内丹能帮助我们。</t><b/>";
		case 10005:	return "<t>吾乃幻灵宫弟子，夏梦狼。</t><b/>" @
											 "<t>【对着你噙动鼻翼】</t><b/>" @
											 "<t>我怎么闻到一股子鬼气！好臭！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10148Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀死几只黄鼬护卫，带回几份被偷走的粮食给梁盛</t><b/>";
		case 10002:	return "<t>我要好生研究这些污染物，这段时间你要去做别的事。</t><b/>" @
											 "<t>据说你杀掉了啃草大王，那你一定能做好下一件事情：清风滨的粮食大都被黄鼬妖偷了，它们把粮食聚集在一起，由许多黄鼬护卫看守。你去干掉那些黄鼬护卫，把属于村民的粮食带回来！</t><b/>";
		case 10005:	return "<t>看来你和其他的修神道士不一样，倒有几分真本事！</t><b/>" @
											 "<t>对了，你夺粮时我细细研究了那些受污染的物品，污染力有浓烈的阴寒之气，仿佛不属于这个世界……我能预感将有一场更大的浩劫降临，我很担心……</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10147Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>收集几根被污染的木桩和几朵被污染的花朵，之后向梁盛回报</t><b/>";
		case 10002:	return "<t>我不知你有何本事，杀掉几只兔子又能证明什么？哼，清风滨的危难哪是区区几只兔子就能影响的。</t><b/>" @
											 "<t>你若真的有能力，想做些什么，先在这附近收集下几根被污染的木桩和几朵被污染的花朵，集齐了再来找我吧。</t><b/>";
		case 10005:	return "<t>做的还算不错，不过这是很简单的事罢了！要得到我的承认，必须要做更多更重要的事情。你记住，拯救清风滨绝非随口说说的事情。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10146Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>将磨旧的铜板交给望海居的梁盛</t><b/>";
		case 10002:	return "<t>你做了许多人根本无法办到的事。小神仙，你即可去一趟望海居，我的朋友梁盛在那里，自村子出事后他就在追查此事的源头，你在他那里一定能得到更多的信息。</t><b/>" @
											 "<t>把这个铜板给他，他看了自然知道是我叫你去了，呵呵，自出事后他的性格……就很不好接触。</t><b/>";
		case 10005:	return "<t>你是什么人？速速离去！</t><b/>" @
											 "<t>【将铜板递给他】</t><b/>" @
											 "<t>哼，贾三那家伙又找了什么神仙道士吗？统统都是废物，不然我们的村子……哼！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


function Mission10145Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀死啃草大王和他的几名黄鼬打手，将啃草大王的牙齿带给清风村的贾三</t><b/>";
		case 10002:	return "<t>小神仙，尽管你除去了许多捣药兔，但关键的还是他们的头领。那只兔妖自称啃草大王，颇有妖力，不仅坐山称王，据说还收伏了几只黄鼬妖当手下。</t><b/>" @
											 "<t>要彻底清楚捣药兔的威胁，必须杀了啃草大王和他奴役的黄鼬打手！带回啃草大王的牙齿给我。</t><b/>";
		case 10005:	return "<t>好！你做到了！捣药兔的威胁终于清除了，大伙知道一定会兴奋的！谢谢你，小神仙，这些东西不成敬意，还请一定收下。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


function Mission10144Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>清风村的贾三让你杀掉几只捣药兔，之后向他回报</t><b/>";
		case 10002:	return "<t>大伙都说村子中了邪，染了鬼气，我却不那么认为。</t><b/>" @
											 "<t>哼，总是有原因的！清风滨的生物近来也变得暴躁，包括那些兔子的行为也古里古怪，像发了狂般啃食植物的根茎，如此下去，清风滨会变成一片荒漠！</t><b/>" @
											 "<t>小神仙，去除掉一些捣药兔，我在这里等你的好消息！</t><b/>";
		case 10005:	return "<t>好！捣药兔的疯狂行为必定与村子的怪病有很大关系。你除掉这些兔子，总是好些的。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10143Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>陶村长让你与清风村的贾三谈谈。</t><b/>";
		case 10002:	return "<t>村子之前有很多人都染上了怪病，直到搬了出来……唉，不瞒你说，这病实在过于诡异，我担心是沾染了鬼气！</t><b/>" @
											 "<t>具体的事情你去问问村里的贾三，他会告诉你更多的事情。</t><b/>";
		case 10005:	return "<t>村长让你来的？你就是小神仙？【疑惑地打量你，半天才继续说】还像！还像那么回事！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10142Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小哥哥";
	else
		%Sex = "小姐姐";

	switch(%Tid)
	{
		case 10001:	return "<t>杀死偷粮黄鼬妖收集几条黄鼬的尾巴，交给清风村的陶村长</t><b/>";
		case 10002:	return "<t>你知道吗？村子在未搬出之前，许多村民都染了病，很奇怪的病呢，皮肤溃烂，毛发脱落，大人们说这是染了鬼气。</t><b/>" @
											 "<t>对了，"@%Sex@"，清风滨还有许多偷粮黄鼬妖，偷了好多好多粮食，我们可厌恶它们了！若你消灭它们，把它们的尾巴带给清风村的陶村长，村长爷爷一定会很感激你的。</t><b/>";
		case 10005:	return "<t>咳咳！这……这不是黄鼬妖的……你真是神仙啊！小虎之前说一个神仙来了我们清风滨，起先我还不信，原来真是神仙下凡啊！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10141Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小哥哥";
	else
		%Sex = "小姐姐";

	switch(%Tid)
	{
		case 10001:	return "<t>清风滨的小虎让你去杀死几只大耳兔，完成后与小虎谈谈</t><b/>";
		case 10002:	return "<t>我们村叫清风村，最早在清风滨的中央，后来村子出了事，就搬到了……搬到了……看，就在我身后的小山上！</t><b/>" @
											 "<t>哎呀，小虎的牛儿饿了，最近清风滨多了很多兔子，它们不好好吃胡萝卜，专抢牛儿的青草。"@%Sex@"，能帮我赶走那些讨厌的兔子吗？小虎会告诉你村子发生的事情。</t><b/>";
		case 10005:	return "<t>太好了，"@%Sex@"，你把讨厌的兔子都赶走了，牛儿一定能吃得饱饱的！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


function Mission10140Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到清风滨找到小虎，打探下清风村发生了什么事</t><b/>";
		case 10002:	return "<t>最近城中传出了一件大事，城外的清风村仿佛遇到了诡异的事情，连村子都整个搬了出来，村民四处寻找神仙灵者，不知发生何事？</t><b/>" @
											 "<t>我与村中的放牛娃小虎是忘年交，你代我向他问候一声，再问问究竟发生了何事？他就在清风滨的入口。</t><b/>";
		case 10005:	return "<t>嘻，周叔让你来的呀？小虎什么都好哦，谢谢周叔，谢谢你。</t><b/>" @
											 "<t>嘻嘻……</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

//■■■■■■■■■■■第二章■■■■■■■■■■■■■■■■

//■■■■■■■■■■■■■■■第三章■■■■■■■■■■■■

function Mission10188Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>对神水树使用灵镜，封印归鱼</t><b/>";
		case 10002:	return "<t>咳咳，那个……小朋友啊……那个你看起来很闲是吧？</t><b/>" @
											 "<t>来，本神给你一点趣事做做。我将那些材料做成了一面灵镜，你对着神水树使用灵镜，嘿嘿，会发生奇妙的事情哦！封印归鱼这种千秋大业，神功伟绩我就让给你来做了！</t><b/>";
		case 10005:	return "<t>嘿嘿，感觉很不错吧，我人很好很大方吧。那么你从青和那里听的事就都忘了哦，对谁都不能说！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


function Mission10187Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从虾兵蟹将身上取得六彩绸缎，玉珊瑚，潮汐鼓后，去找璇光</t><b/>";
		case 10002:	return "<t>璇光让你找我，无非是想让我帮他找齐神水树的封印材料。既来自水，则取之水，此事对你来说也不难。</t><b/>" @
											 "<t>你看到崖下那一片海滩了么，当中徘徊许多虾兵蟹将，它们从龙宫出来，身上必然有玉珊瑚、六彩绸缎、潮汐鼓这些东西。有了这三样东西，璇光就能重新封印神水树了。拿到这些东西后，你就去找璇光吧。</t><b/>";
		case 10005:	return "<t>咳咳！这些东西可不是我让你给我的啊，我什么都不知道，你也什么都别问。本神很忙的！很忙，嗯，很忙！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


function Mission10186Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>听青和讲述神水树与归鱼的往事</t><b/>";
		case 10002:	return "<t>璇光老头在种下神树之后，遍云游四方，不料百年之后，神树神力涣散，归鱼蠢蠢欲动，于是水下翻滚。你看这不是，清风滨很多土地都崩裂出地缝，清风村废墟我想那有最大的一条吧。</t><b/>" @
											 "<t>呵呵，我大概清楚这里发生的事了，说起来很玄奇，你要听吗？</t><b/>";
		case 10005:	return "<t>知道了吧，他这老神仙，脸皮真是比女儿家还薄呢！</t><b/>";

		case 200: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "夫人请讲</t>";
		case 300: return "<t>你从地缝中不是收集了阴气吗？呵呵，这是因为地缝皆由归鱼的神力震开，于是打穿了阴阳两界的结界，一些阴间外围的小鬼就偷偷出来祸害一方。</t><b/>" @
										 "<t>想来璇光也料到百年间神树封印必然松动，故而回来想再度封印一次。谁知道你就找上门了。</t><b/>" @
										 "<t>他让你来找我，是因为他脸皮薄，觉得连一条鱼都封不住，呵呵。归鱼好歹也是神兽，他能一封百年，已经难能可贵，哪有封印千年万年的。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10185Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在地缝一、地缝二、地缝三分别使用净神瓶，取得阴气一、阴气二和阴气三，再给青和看看</t><b/>";
		case 10002:	return "<t>你说闹鬼啊？这个……这个事嘛……</t><b/>" @
											 "<t>【犹犹豫豫】百年之前，此地并未神树，只有一潭深湖，其中生一神兽，名归鱼。此鱼曾为祸一方，后我在此种下神树，将其镇于湖底。</t><b/>" @
											 "<t>不过最近似乎……这样吧，附近好像多了许多裂缝，你把这个瓶子拿着，去看看那些裂缝有何异样，然后一路到望夫灯，那里有个叫青和的夫人，她会告诉你发生了何事。</t><b/>";
		case 10005:	return "<t>哼，那个懒神仙又想让我帮他做事了，你说他哪有些神族的样子！真是气死人了。</t><b/>" @
											 "<t>堂堂神族，还要一个人类女子帮忙，这……这说出去别人都不相信嘛。</t><b/>" @
											 "<t>他肯定不好意思说出此事缘由，呵呵，真是好笑。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10184Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀死尖刺螺妖和青钳虾妖，从它们身上取到尖刺螺壳和青色虾钳，交给璇光</t><b/>";
		case 10002:	return "<t>哎呀，你这小孩子怎么能这样！怎么能随随便便杀生！怎么能随随便便就将这些可爱的小螺小虾杀掉了！你应该……你应该去把它们的头目一起杀了啊，不杀头目有什么用啊！</t><b/>" @
											 "<t>真是笨啊，快去快去，一个叫尖刺螺妖，一个叫青钳虾妖，把它们的螺壳和虾钳带给我。</t><b/>";
		case 10005:	return "<t>这才对了嘛，老人家的大门口总算清净了。嘿嘿，这些小毛虫，我都懒得去搭理它们。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


function Mission10183Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉几个碧水螺和几个青虾小妖后，与璇光谈谈</t><b/>";
		case 10002:	return "<t>哦哦，原来是找老神仙啊。呵呵，他叫璇光，就住在神水树的房子里。不过老神仙性格怪异，不愿涉及凡尘之事。</t><b/>" @
											 "<t>呵呵，正巧他居住的神水树最近被一些碧水螺和青虾小妖所扰，你去杀掉那些妖怪，他肯定会帮你的。</t><b/>";
		case 10005:	return "<t>咦，这些讨厌的东西怎么少了一大片？难道我睡觉时不小心打了个呼，把它们全灭了？罪过啊罪过……</t><b/>" @
											 "<t>你这小家伙是谁？杵老人家面前做什么？真不懂礼貌。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


function Mission10182Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "兄弟";
	else
		%Sex = "姑娘";

	switch(%Tid)
	{
		case 10001:	return "<t>骑上杨镖头的马，到清风滨找到老陈</t><b/>";
		case 10002:	return "<t>"@%Sex@"原来是想去找清风滨的神仙，那只是个传说啊，你还真相信？</t><b/>" @
											 "<t>这样吧，你骑上我的马，它会带你去清风滨。那里有个叫老陈的老汉，常年居住在清风滨，也许知道你要找的神仙吧。</t><b/>" @
											 "<t>好了，现在就骑马去清风滨吧！它会带你去找老陈的。</t><b/>";
		case 10005:	return "<t>呵呵，大老远来找我这糟老头有什么事啊？我年纪大了，耳朵有点背，少侠说话还请大声点，呵呵。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}


function Mission10181Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到万灵城的驿站找到老陈</t><b/>";
		case 10002:	return "<t>少侠请留步，这清风滨邪鬼已除，但你是否想过，这鬼来自何处？为何而来？清风滨怎会突然闹鬼？深究下去，怕是患难虽除，隐患仍留啊！</t><b/>" @
											 "<t>老夫在清风滨认识一位高人，他乃真正神族，你若能得他指点一二，必能明了所有原因。</t><b/>" @
											 "<t>你去驿站处找杨镖头，他有快马正要前往清风滨，如此你也不用长途跋涉了。</t><b/>";
		case 10005:	return "<t>你是赵天师介绍来的朋友？哈哈，我正有匹快马要去清风滨呢！骑我的马，又快又安全！一般的强盗山贼看了都怕，哈哈哈！</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

function Mission10180Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着朱萱给你的一张平安符到赵天师处还愿</t><b/>";
		case 10002:	return "<t>在英雄除鬼的日子里，我在赵天师处为你求了一张平安符，如今你安全归来，朱萱总算安心了。就请英雄去赵天师处还个愿吧。</t><b/>";
		case 10005:	return "<t>【"@%Player.GetPlayerName()@"】，你吉人自有天相，做下这无量功德，此后的修神路途定然会越来越顺的。</t><b/>";
	}
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
}

//■■■■■■■■■■■■■■■第三章■■■■■■■■■■■■
