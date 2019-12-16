//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端任务对话，【章回任务】第五卷
//==================================================================================

//【章回任务】
//第一编号为【卷】，每一张地图为一卷，每一卷的任务编号间隔数为200，第二编号段为【章】，每一卷暂定为三章，每一章的任务编号间隔数为40，附章间隔为20，最后多余20个编号。
//目前编号段规划，最大可至24卷，24*3=72章，即可以满足72张地图所需的章回任务，视后期任务需求，可挪用一部分的支线任务编号段

//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■
function Mission10900Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到祝祭大泽找血皤谈谈</t><b/>";
		case 10002:	return "<t>河洛东陆尚有许多秘密没有揭晓。</t><b/>" @
											 "<t>在月幽境南方的土地上，有一片泱泱大泽，终年云雾缭绕，不知其中藏有什么。</t><b/>" @
											 "<t>传说大泽中居住了一支远古文明的种族，九幽教的阴司血皤正在调查此事，你在大泽的入口能找到他。</t><b/>";
		case 10005:	return "<t>陌生人，你不该踏入此地的，这片沼泽很……邪……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10901Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "兄弟";
	else
		%Sex = "姑娘";

	switch(%Tid)
	{
		case 10001:	return "<t>从附近的泥浆怪身上取到一些黑色毒腺给血皤</t><b/>";
		case 10002:	return "<t>我一直在各地寻找毒虫药草，直到我发现此地，这里实在毒透了！</t><b/>" @
											 "<t>这里的一些生物让我很感兴趣，看到那些粘稠的泥浆怪了吗？它们身上的毒令人望而生畏，你能帮我搞到它们的毒腺，是吧？" @ %Sex @ "。</t><b/>";
		case 10005:	return "<t>哦哟哟，小心些，这些毒能让一个人马上死翘翘！</t><b/>" @
											 "<t>来，慢点给我。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10902Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "兄弟";
	else
		%Sex = "姑娘";

	switch(%Tid)
	{
		case 10001:	return "<t>从毒泥怪身上取到一些染疫毒液，交给血皤</t><b/>";
		case 10002:	return "<t>此毒……我相信一滴就能毒翻一头巨兽。惟一不足之处，气味实在……臭极了……</t><b/>" @
											 "<t>光是毒腺无法让我做更多的测试，我还需要纯正的毒液。游走在附近的毒泥怪是很不错的对象，取来它们的毒液，我就能制作天下第一的毒药啦！</t><b/>";
		case 10005:	return "<t>哈哈，这些毒液多么新鲜！" @ %Sex @ "你实在太棒了！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10903Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>用空瓶子到大锅处装满毒液，然后倒入毒腺中，使之混合，直到获得毒液混合物为止</t><b/>";
		case 10002:	return "<t>到目前为止，你做得很好。我愿意传授你一些制毒的手艺，以便让你对毒有更多的认识。</t><b/>" @
											 "<t>拿着这个空瓶子，从我左手边的大锅中取得毒液，再将它们倒入右手边的毒腺中，直到你获得安宁的毒液混合物为止。</t><b/>" @
											 "<t>小心些，假若失败，会发生意料不到的事情！</t><b/>";
		case 10005:	return "<t>哈，你悟性不错，这么快就制成了毒液混合物。</t><b/>" @
											 "<t>过程很美妙吧，哈哈哈！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10904Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>对污泥巨怪使用毒液混合物后杀了它，再去找邪骷</t><b/>";
		case 10002:	return "<t>恭喜你，马上就能感受到九幽教的强力毒药。</t><b/>" @
											 "<t>在这附近有许多巨大的污泥怪，奇毒无比，拥有强悍的抗毒能力。</t><b/>" @
											 "<t>现在，将你做的毒液洒在它身上，看看是它的抗毒能力强，还是我们的毒药强。完事后去找邪骷，他会告诉你接下来该做什么。</t><b/>" @
											 "<t>记住，让那团臭泥飞灰烟灭！</t><b/>";
		case 10005:	return "<t>【邪骷空洞的眼窟冷冷看着你】</t><b/>" @
											 "<t>你双手染过九幽教之毒，那我且不杀你……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10905Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从沼泽巨蛙嘴中夺到古怪令牌，再交给邪骷</t><b/>";
		case 10002:	return "<t>越过此线，再往内就进入大泽深处了，你最好想清楚。</t><b/>" @
											 "<t>这片沼泽诡异而神奇，你能在征战沼泽寻找些蛛丝马迹吗？</t><b/>";
		case 10005:	return "<t>这块令牌上刻画的符文诡异而优美，非中土之物，估计是沼泽内的居民物品。</t><b/>" @
											 "<t>祝祭大泽迷雾千里，里面有居民也不奇怪，重要的是那些居民的文明如何。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10906Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从沼泽鳄身上取一些鳄鱼牙齿交给邪骷</t><b/>";
		case 10002:	return "<t>我猜测大泽中有不为人知的种族，贸然闯入绝非上策。你必须要有一身过硬的本领，可是……你有吗？</t><b/>" @
											 "<t>证明给我看看吧，这附近有不少鳄鱼，我要看到它们最锋利的牙齿。</t><b/>";
		case 10005:	return "<t>【邪骷接过牙齿，冷冷笑道】</t><b/>" @
											 "<t>这些鳄鱼都凶残狠辣，你能拔下它们的牙齿，还不算懦夫。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10907Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>用惊魂棒击打征战沼泽的族人尸体，直至召唤出5个族人魂魄，听听他们会说什么</t><b/>";
		case 10002:	return "<t>看到那些死人了吗？魂魄在沼泽中游走，消散于未知的空间。</t><b/>" @
											 "<t>我们要搞清楚他们经历了什么。来，拿着这根惊魂棒，对沼泽的族人尸体使用，运气好的话他们的魂魄会出现，并告诉你一些事情。</t><b/>" @
										 	 "<t>如果运气不好嘛……那……很高兴认识你……</t><b/>";
		case 10005:	return "<t>邪神？邪恶的神明？他们信仰的是何方神圣？</t><b/>" @
											 "<t>想不明白，还有比鬼王更邪的存在吗？哈哈。</t><b/>";
											 
	  case 170 : return"<t>那些巫师发疯了，他们是蛇神的诅咒！祝祭大泽会成为死亡废墟！</t><b/>";
	  case 171 : return"<t>我已经死了吗？邪巫们信仰了一个强大的神，走上了错误的道路。巫族正在四分五裂，邪恶的神即将降临……</t><b/>";
	  case 172 : return"<t>所有人都会死！没人能逃脱邪神的惩罚！我将永远沉沦在痛苦之中！</t><b/>";
	  case 173 : return"<t>陌生人？巫咸族的祭司有了不同的信仰，他们从族落分离，用巫术蛊惑了一群人民，并对我们的家园开战……血流成河……</t><b/>";
	  case 174 : return"<t>邪恶的神明会将万物吞噬，我的家园将不复存在……救救我们……救救我们……</t><b/>"; 
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10908Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着阴司灯，扮成阴司去吓唬巴尔丁</t><b/>";
		case 10002:	return "<t>【邪骷干哑的笑了起来】</t><b/>" @
											 "<t>这片沼泽比地府还要诡异，看来不会有什么好事。</t><b/>" @
											 "<t>你要了解那个所谓的邪神，在征战沼泽的南方，有一个将死的巫师，带着这个阴司灯去吓唬他。</t><b/>" @
											 "<t>你这么给他说，若他告诉你邪神的秘密，那么就让他轮回转世；若他不说嘛……嘿嘿，就把他丢到十八层地狱的油锅里炸成肉干！</t><b/>" @
											 "<t>哈哈，过程肯定很美妙！记得把阴司灯提在手上！</t><b/>";
		case 10005:	return "<t>我一定把所知的一切都告诉阴司大人！多谢阴司大人饿恩德！</t><b/>";
			
		case 100: return "<t>走开！我浑身都是烂疮和毒质，你怎么死都不知道。</t><b/>";
	  case 101: return $Get_Dialog_GeShi[31204] @ "吾乃夺命阴司。巴尔丁，你阳寿已尽，速速随我前往地府。</t>";
	  case 110: return "<t>阴司？哈哈哈，老夫终于死了！可以脱离苦海，不再受毒疮溃烂之苦！走，这就前去地府！</t><b/>";
	  case 111: return $Get_Dialog_GeShi[31204] @ "你犯下滔天罪行！与邪神勾结，现如今要下那油锅地狱，火山地狱，万万年不得超生！</t>";
	  case 120: return "<t>【巴尔丁浑身发颤】</t><b/>"@
										 "<t>不！不要！我没有与邪神勾结！我只是被邪神蛊惑，但并未背叛家园啊！如今我一个人躲在这里等死，就是不想再被邪巫们寻到。</t><b/>"@
										 "<t>阴司饶命！我的确未信仰邪神啊！</t><b/>";
		case 121: return $Get_Dialog_GeShi[31204] @ " 哼！如今你还有一条路可走，就是告诉我邪神之秘，或许能减轻你的罪责，进入轮回！</t>";
		
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10909Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在巴尔丁的巫术下，观察大泽内的三处地点</t><b/>";
		case 10002:	return "<t>我们是巫咸族人，毒祖是我们信仰的古神。但族内有一部分巫师信仰了一个邪神，他们蛊惑族人，导致族落四分五裂。</t><b/>" @
											 "<t>我与他们不同，现在就让我施以巫术，带你去看看那些危险的地方。</t><b/>" @
											 "<t>但是，阴司大人，我们的所到之处将十分险恶，请你千万不要暴露踪迹，查探之后与我交谈，我会用巫术协助离开。准备好了吗？</t><b/>";
		case 10005:	return "<t>邪神相柳神通广大，阴司大人还是不要与之交恶为好……</t><b/>" @
											 "<t>咳咳，我还能苟活数日……唉，就让我在此为那些邪巫的罪行忏悔吧！</t><b/>";
											 
		case 200:	return $Get_Dialog_GeShi[31204] @ "我准备好了</t>";
		case 300:	return $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>"; 
			
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10910Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉沼泥怪后去找邪骷谈谈</t><b/>";
		case 10002:	return "<t>若阴司大人能拯救我们的族人，那将是天大的恩德！</t><b/>" @
											 "<t>除了安扎克的危险，大泽中许多毒性生物也成了我族的障碍，若大人愿意帮助巫咸族，除去那些盘踞的沼泥怪必定是一件大功德。</t><b/>" @
											 "<t>【巴尔丁叹了口气，埋头不语】</t><b/>" @
											 "<t>唉，杀掉那些沼泥怪后去找邪骷谈谈吧。</t><b/>";
		case 10005:	return "<t>嘿嘿，那家伙吓坏了吧！把灯还给我，不然我怎么交差啊？</t><b/>" @
											 "<t>对了，他有没有给你说些什么？</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10911Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从碧火蟾身上取到一些搏动的心脏，交给巫咸族落的达拉曼</t><b/>";
		case 10002:	return "<t>哦？巫咸族？果然有一个族落，我们是否要帮助他们呢？</t><b/>" @
											 "<t>这样吧，首先你进入他们的部落——巫咸族落。大泽中有一种剧毒无比的碧火蟾，若你能拿到它们的心脏，再交给巫咸族的巫祝，我相信这些喜欢毒物的家伙们会喜欢的。</t><b/>";
		case 10005:	return "<t>外族人，你是怎样进入了大泽？</t><b/>" @
											 "<t>哦，这些可爱的小心脏正是我需要的巫术材料，送给我的？陌生人，你如果是来帮助我们的，请向我表明来意。</t><b/>" @
											 "<t>如果有半点诡计，我不介意马上将你变成一只青蛙，再踩爆你的肚子！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10912Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉一些安扎邪卒后与达拉曼谈谈</t><b/>";
		case 10002:	return "<t>光用嘴说说就能获得信任？没这么简单吧，外族人。</t><b/>" @
											 "<t>我们与邪恶的安扎克交战数年，若你真的是来帮助我们的，就去杀掉一些安扎邪卒，那时候我们再好好谈。</t><b/>";
		case 10005:	return "<t>哈哈，干得不错，那些可恶的邪卒就应该死！</t><b/>" @
											 "<t>看着你沾血的双手，我开始有点信任你了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10913Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉一些安扎邪巫，带回邪巫面具给达拉曼</t><b/>";
		case 10002:	return "<t>邪神相柳妄想将所有人都变成他的仆从，他亵渎了我们信仰的神明，蛊惑了许多族人背叛，组成了如今的安扎克。</t><b/>" @
											 "<t>安扎邪巫在征战沼泽中游走，屈从于邪神那黑暗古老的力量之下，我们不能让她们活着！去吧，杀了你看到的所有邪巫，并带回她们的面具。</t><b/>";
		case 10005:	return "<t>你每杀死一个安扎族，都为大泽的安宁和自由进了一分力量。</t><b/>" @
											 "<t>你是值得信任的盟友，欢迎你来到巫咸族。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10914Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>把鲜艳的羽毛带给族长桑木乌达</t><b/>";
		case 10002:	return "<t>带着这根羽毛，这代表巫族的认可。拿着它去见我们的族长桑木乌达，他会欢迎你的，朋友。</t><b/>";
		case 10005:	return "<t>呵呵，你能得到达拉曼巫祝的认可，远比得到我的认可要困难许多。</t><b/>" @
											 "<t>欢迎你来到巫咸族，可惜我们无法用最好的美酒招待你，因为此刻正面临战争。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10915Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>巫咸族长桑木乌达希望你在他附近的柱子上使用尖刀，然后和他对话</t><b/>";
		case 10002:	return "<t>安扎克最早属于巫咸族，他们信仰了古老黑暗的力量，成为了沼泽的敌人。</t><b/>" @
											 "<t>你愿意与我们联盟吗？一起对抗黑暗的安扎克。</t><b/>" @
											 "<t>【桑木乌达用他的指尖割开了手掌，然后把手伸前。】</t><b/>" @
											 "<t>那么，你怎么想？让我们的血液留在一起，歃血为盟怎么样？在我旁边就有一把尖刀，用它割破你的手。</t><b/>";
		case 10005:	return "<t>你我已血脉相连，兄弟！</t><b/>";
			
		case 200:	return $Get_Dialog_GeShi[31204] @ "将桑木乌达血淋淋的手紧紧握住</t>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10916Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>桑木乌达让你从商人那里购得陶土罐，再带回一些黑泥鳅和腐茎</t><b/>";
		case 10002:	return "<t>首先有些准备工作要做，兄弟。</t><b/>" @
											 "<t>要想在大泽中生存，你需要得到我们强大的蛊毒力量。</t><b/>" @
											 "<t>我需要一只罐子——那边的巫咸商人拖木萨就有卖，一些生长在毒咒泥沼的黑泥鳅和腐茎。</t><b/>" @
											 "<t>在安扎克攻击我们前，赶紧的，兄弟。</t><b/>";
		case 10005:	return "<t>你动作真快，兄弟。</t><b/>" @
											 "<t>蛊对你而言，兴许很神秘，但对于大泽，是必不可少的圣物。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10917Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>对毒咒泥沼的黑眶蟾蜍使用泥鳅蛊后杀了它们，再和桑木乌达对话</t><b/>";
		case 10002:	return "<t>对于第一次接触蛊毒的人来说，是有些恐怖。呵呵，未知的恐怖。</t><b/>" @
											 "<t>你要尝试用蛊，我将你带回的材料制成了泥鳅蛊，这种蛊毒能让一些生物神智迷乱，血液横流。</t><b/>" @
											 "<t>带着这个泥鳅蛊到毒咒泥沼，对那些黑眶蟾蜍使用，之后杀了它们。</t><b/>" @
											 "<t>用过之后，你会发现蛊之强大！</t><b/>";
		case 10005:	return "<t>怎么样，兄弟，用蛊的感觉很美妙吧！哈哈哈！</t><b/>" @
											 "<t>看你的脸色，我就知道你很喜欢。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10918Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀了安扎克游民，拿到他们的头颅当作战利品，交给大督军沃尤</t><b/>";
		case 10002:	return "<t>如今你也知道蛊术的毒辣强悍，同样的蛊，在巫咸族被当成圣物；而安扎克却用来杀死我们的兄弟、亲人和妇儒！</t><b/>" @
											 "<t>杀了在征战沼泽徘徊的安扎克游民，把他们的头骨当做战利品带给前线大督军沃尤。</t><b/>";
		case 10005:	return "<t>这些肮脏的头颅……曾经是我们的兄弟，如今却成了死敌，唉……</t><b/>" @
											 "<t>一切都由信仰而起……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10919Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀了傀儡尸，再向沃尤回报</t><b/>";
		case 10002:	return "<t>征战沼泽上有无数族人的尸体，安扎克的巫师利用巫术，让我们可怜的人民重新站起，成为傀儡。</t><b/>" @
											 "<t>我请你去解救那些痛苦的魂灵，最直接的办法，让他们的肉体毁灭，从而使魂灵安息。</t><b/>" @
											 "<t>去吧，带着尊敬，杀了那些傀儡尸。</t><b/>";
		case 10005:	return "<t>我的兄弟们已经安息了吗？他们以最崇高的方式死亡……伟大的战士……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10920Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>进入安扎克窃取物资，然后去找卧底卓耶九</t><b/>";
		case 10002:	return "<t>邪恶的安扎克就在前方，战士。</t><b/>" @
											 "<t>战争需要物资，这点你应该知道。如果安扎克没有了这些战争物资，肯定会消停一阵子。</t><b/>" @
											 "<t>【沃尤看着你哈哈大笑】</t><b/>" @
											 "<t>我要你潜入安扎克中，窃取物资，之后找到卓耶九，他是我们派去的卧底。</t><b/>";
		case 10005:	return "<t>你是大督军派来的人？看到了吗，我的身份被识破了，你再晚来几步，我就去和阎王喝酒了！</t><b/>" @
											 "<t>这些混帐安扎克，竟想杀我！不是老子怕死，这些畜生太混帐了，他们的信仰有问题！</t><b/>" @
											 "<t>【卓耶九很生气，继续嘀嘀咕咕的骂着】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10921Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>护送卓耶九离开安扎克</t><b/>";
		case 10002:	return "<t>呸！</t><b/>" @
											 "<t>【卓耶九用力吐了口唾沫，再对你笑道】</t><b/>" @
											 "<t>谢谢你把我从这鸟地方救了出来，怨我疏于防范，居然被他们识破。</t><b/>" @
											 "<t>算了算了，该探的已经被我探得差不多了，我们赶紧跑吧！咦……那边有篝火？</t><b/>";
		case 10005:	return "<t>哈哈，卓耶九回来了！</t><b/>" @
											 "<t>兄弟，你干得不错，我们就是安扎克的阴影。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10922Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>向大督军沃尤复命，转告卓耶九探查的军情</t><b/>";
		case 10002:	return "<t>【" @ %Player.GetPlayerName() @ "】，安扎克后方的大营里才是最精锐的力量，他们直接听命于邪神相柳的力量，并预谋亵渎我们的古神。</t><b/>" @
											 "<t>这是非常恐怖的消息，赶紧向沃尤复命，时局急迫！</t><b/>";
		case 10005:	return "<t>安扎克的狗胆越来越大，竟对我们的古神打起了主意。</t><b/>" @
											 "<t>我需要点时间好好整理思绪，你稍等片刻。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10923Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到毒咒泥沼找大巫医克金亚学习制蛊之术</t><b/>";
		case 10002:	return "<t>恕我直言，尽管你有不凡的能力，但想彻底荡平安扎克，恐怕还差了点。大泽是可怕的。</t><b/>" @
											 "<t>我仔细思考过了，你要学会我族的蛊术，这对你和整个巫咸族都有很大的帮助。</t><b/>" @
											 "<t>拿着这封军令，去毒咒泥沼找大巫医克金亚，他会传授你如何制蛊。</t><b/>" @
											 "<t>在你学习的这段时间，我们会牵制安扎克。</t><b/>";
		case 10005:	return "<t>小家伙，别急匆匆的，把你手中的军令给我看看。</t><b/>" @
											 "<t>【克金亚很快的看过军令，之后大笑起来】</t><b/>" @
											 "<t>哈哈哈，终于要杀那帮龟孙子了吗？我可是期待的很呢！虽然安扎克在十几年前曾是我们的兄弟，但如今已不共戴天！他们必须死！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10924Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>克金亚让你去拿到癞蟾毒腺，沼黄鳄胆和青蛙粪便，之后向他复命</t><b/>";
		case 10002:	return "<t>不要急，一步一步来，你首先需要了解各种材料。</t><b/>" @
											 "<t>蛊乃人工培育的毒虫，再以巫术炼养，最后达到各样功效。毒咒泥沼有许多斑癞蟾，取回他们的毒腺。另外还有沼黄鳄的胆，青蛙的粪便。</t><b/>" @
											 "<t>听起来很恶心？以后你会觉得他们很美。</t><b/>";
		case 10005:	return "<t>你还不算笨，没弄错，就是这些材料。</t><b/>" @
											 "<t>哈哈，这些都是用来喂毒虫的食料，让它们先沾点腥气。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10925Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>按照克金亚的要求，学会泥鳅蛊配方</t><b/>";
		case 10002:	return "<t>不同的蛊有不同的配方和功效，可以伤人无形，或令蛊主受巫术保护，总之千变万化啦。</t><b/>" @
											 "<t>这样吧，你先从我这里领悟初级蛊术的奥秘，我还会给你一个配方，两者都领悟后我再教你下一步。</t><b/>";
		case 10005:	return "<t>嘿嘿，看来你领悟得还不错，我们继续吧。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10926Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着克金亚给你的蛊去杀了刺皮老鳄，带回它的鳄鱼鳞</t><b/>";
		case 10002:	return "<t>离下课还早，小子。我要你带着这个泥鳅蛊，到毒咒泥沼去找一头老鳄鱼，对它使用后再杀掉，顺便带回它的鳞片。</t><b/>" @
											 "<t>提高警惕，老鳄鱼肯定不会独自行动。</t><b/>";
		case 10005:	return "<t>嘿！多美的鳞！这可是不错的材料！</t><b/>" @
											 "<t>好好调整下，接下来你所学的东西将匪夷所思。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10927Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>把克金亚的书信带给黑图</t><b/>";
		case 10002:	return "<t>为了让你记忆深刻，并且体现我直观的教学方式，我准备让你……做活人实验。</t><b/>" @
											 "<t>黑图是我族最出名的杀手，他一直在暗杀大泽里单独行动的安扎克。带着我的这封书信去找他，让他带几个俘虏给你，我们将用来学习蛊术。</t><b/>" @
											 "<t>【克金亚邪恶地笑笑】</t><b/>" @
											 "<t>还等什么，如今他在毒咒泥沼通往毒祖滩的路上。</t><b/>";
		case 10005:	return "<t>【黑图阴阴看着你】</t><b/>" @
											 "<t>大巫医说要带三个人给他，哼，又多了三个倒霉鬼！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10928Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>对三个哈扎克使用迫虫蛊后，将之带回黑图处</t><b/>";
		case 10002:	return "<t>听着，这附近就有三个藏匿的安扎克，他们潜入了我们的后方，控制了那里的怪物。</t><b/>" @
											 "<t>都是些小打小闹而已，带着这个迫虫蛊找到他们，对他们使用后，他们就会跟着你，把他们一个个都带来。</t><b/>" @
											 "<t>顺便提醒你，每个迫虫蛊只有一段时间的功效，你要在有效的时间里把人带来。</t><b/>";
		case 10005:	return "<t>这三个家伙运气好，不用死在我的刀下。我的刀割断喉咙的时候，那可不是一般的痛。</t><b/>" @
											 "<t>【黑图盯着你的脖子，阴阴一笑】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10929Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>和黑图对话，让他传送你到大巫医克金亚处，再和克金亚对话</t><b/>";
		case 10002:	return "<t>我已让人把那三个家伙送到大巫医手上了，现在我传送你到大巫医那里。</t><b/>" @
											 "<t>哼！并非我愿意传你过去，只是见你一路奔波，想让你留点体力好好折磨那三个混蛋。</t><b/>" @
											 "<t>记着，用尽一切办法折磨他们！</t><b/>" @
											 "<t>准备好去大巫医处就给我说一声。</t><b/>" @
											 "<t>我准备好了</t><b/>";
		case 10005:	return "<t>小子，我们又见面了！</t><b/>" @
											 "<t>有了这三个倒霉鬼，你很快就能学到我的制蛊技术，成功出师。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10930Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>克金亚让你学会给你的两个配方后和他谈谈</t><b/>";
		case 10002:	return "<t>你会喜欢我这个老师的，我的教学方式是如此简单而有艺术感，又快又直接。</t><b/>" @
											 "<t>【克金亚得意大笑】</t><b/>" @
											 "<t>一步一步来，先拿着这两个配方，仔细看看，将他们牢牢记在脑子里，之后我们再谈其他的。</t><b/>";
		case 10005:	return "<t>都记牢啦！很简单吧，在我的方法下，没有什么蛊是难的。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10931Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在一定时间内完成克金亚给予你的任务</t><b/>";
		case 10002:	return "<t>有难度的事即将上演，深呼吸，放轻松点。</t><b/>" @
											 "<t>我会随意说出泥鳅蛊、傀儡蛊和火蚕蛊，当我说了哪个，你就要赶紧到我两边的草娄中寻到材料。</t><b/>" @
											 "<t>当你把三种蛊都做出来时，就算出师了！</t><b/>" @
											 "<t>这件事不会很轻松的，但我保证你会获得丰厚的好处！</t><b/>" @
											 "<t>接下来，第一种：傀儡蛊！</t><b/>";
		case 10005:	return "<t>哈！表现很不错嘛！</t><b/>" @
											 "<t>来，拿着这个配方，这是正式的，只要你以后搜集到了材料，就能随意使用！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10932Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在一定时间内完成克金亚给予你的任务</t><b/>";
		case 10002:	return "<t>接下来是你熟悉的泥鳅蛊，快一点，这个应该一点都不难！</t><b/>" @
											 "<t>给我仔细点，否则你别想过关！</t><b/>";
		case 10005:	return "<t>哈！表现很不错嘛！</t><b/>" @
											 "<t>来，拿着这个配方，这是正式的，只要你以后搜集到了材料，就能随意使用！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10933Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在一定时间内完成克金亚给予你的任务</t><b/>";
		case 10002:	return "<t>最后是火蚕蛊，快一点，这个可没那么简单！</t><b/>" @
											 "<t>谨慎点，否则你会功亏一篑。</t><b/>";
		case 10005:	return "<t>你做的很好，小子，你完成了最难的事情！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10934Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>分别使用三个蛊后，看看会发生什么</t><b/>";
		case 10002:	return "<t>你成功地制出了三种蛊，接下来的事情变得很简单了。</t><b/>" @
											 "<t>三个安扎克实验品登场了！泥鳅蛊让纳班好好享受；拉莫喜欢傀儡蛊；而火蚕蛊则是对你自身使用，之后宰了吉索利！</t><b/>";
		case 10005:	return "<t>恭喜你，小子，你得到了我的真传！</t><b/>" @
											 "<t>怎样，我的教学方式很强吧？哈哈哈，没有什么蛊能难住我克金亚！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10935Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到巫咸族后去找桑木乌达</t><b/>";
		case 10002:	return "<t>你在这里待得够久了，现在你要尽快赶回巫咸族，他们肯定等得很急。</t><b/>" @
											 "<t>我会传送你前往巫咸族，到达之后去找族长，告诉他你已经得我真传。</t><b/>" @
											 "<t>准备好了吗？</t><b/>" @
											 "<t>我准备好了</t><b/>";
		case 10005:	return "<t>我们一直在等待你的归来，战争已蓄势待发，兄弟。</t><b/>" @
											 "<t>听到你学会蛊术，我万分欣喜啊！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10936Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>按照克金亚的要求，去取一条黑泥鳅，一根腐茎，再从克金亚处购得一瓶蛊药，之后在蛊瓮附近使用配方</t><b/>";
		case 10002:	return "<t>小子！听好了，所谓蛊，是将各种的毒虫材料放在一个瓮里，再倒入蛊药，经过人工培育，最后变成蛊。</t><b/>" @
											 "<t>看过配方了吧，现在你到我旁边取一条泥鳅和一根腐茎，再从我这里买一瓶蛊药，之后在我面前的蛊瓮附近制作这个配方，看看会怎样。</t><b/>";
		case 10005:	return "<t>哈哈，看到了吗？这条漆黑的玩意就是泥鳅蛊了。它的功效你之前已经见过，是不是还想尝试尝试？</t><b/>" @
											 "<t>【克金亚邪恶地笑起来】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■
