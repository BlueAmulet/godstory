//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端任务对话，【章回任务】第三卷
//==================================================================================

//【章回任务】
//第一编号为【卷】，每一张地图为一卷，每一卷的任务编号间隔数为200，第二编号段为【章】，每一卷暂定为三章，每一章的任务编号间隔数为40，附章间隔为20，最后多余20个编号。
//目前编号段规划，最大可至24卷，24*3=72章，即可以满足72张地图所需的章回任务，视后期任务需求，可挪用一部分的支线任务编号段

//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■第二章■■■■■■■■■■■■■■■■
//■■■■■■■■■■■■■■■第三章■■■■■■■■■■■■

//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■
function Mission10500Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到万灵城找传送师</t><b/>";
		case 10002:	return "<t>在轩辕大陆的东方有一个奇妙世界，若要修炼，那个地方你不得不去。万灵城的传送师能告诉你此去的方法。</t><b/>";
		case 10005:	return "<t>神州世界广阔无边，但有我在，去哪里都方便。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10501Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到达月幽境后，与护城者玉莲谈谈</t><b/>";
		case 10002:	return "<t>你要去的地方叫河洛东陆，是轩辕中州东边的一块神秘大陆。嗯，那里孕育出了精怪二族，相信你能看到许多奇妙的景致。</t><b/>" @
											"<t>我将送你到达月幽境，那是河洛东陆的大城。嗯，抵达后记住找护城者玉莲报到，否则你会被认为入侵者。</t><b/>";
		case 10005:	return "<t>这里是月幽境，你是前来的修真者吗？在此地行动请谨慎小心，切勿随意破坏自然之物，我们欢迎热爱自然的修真者，否则请离开。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10502Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着含灵树枝去找紫飞飞</t><b/>";
		case 10002:	return "<t>早在百年前，月幽境的神树【巨木】便自断了一截，留下了庞大的树桩和灵气，月幽境方利用这些灵气建造而成。</t><b/>" @
											"<t>你将这根树枝带给城中的紫飞飞，她一直在研究巨木的历史。</t><b/>";
		case 10005:	return "<t>哦？这半截树枝……是玉莲姐姐让你来的吗？</t><b/>" @
											"<t>你看，这树枝比平常树枝大许多，而且灵力丰沛，可以想象当年神树完璧时是怎样一番胜景！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10503Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去找姚兰馨谈谈</t><b/>";
		case 10002:	return "<t>感谢你带来这半截树枝，这对我研究巨木的历史有很大帮助。</t><b/>" @
											"<t>虽然我研究巨木的时间不长，只有一百多年，但我敢说，巨木是神州最古老的树木之一。</t><b/>" @
											"<t>你需要去一趟树屋找姚兰馨，她知道你来了后一直要见你。</t><b/>";
		case 10005:	return "<t>你终于来了！据说你帮助了神女在凡间的爱侣，天大的本事！</t><b/>" @
											"<t>你这样的人早该来河洛东陆，这里才是适合你修炼的地方。我叫姚兰馨，专寻找那种有潜力的天才，呵呵，无疑你就是。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10504Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在城中的箱子中收集姚兰馨的手稿，之后回报姚兰馨</t><b/>";
		case 10002:	return "<t>这个世界并非你想象那么平和，各种各样修神之士掺和在一起，他们有的善良，有的邪恶，还有更多很无知。</t><b/>" @
											"<t>大自然在其中遭受了极大的破坏，唉，这是我们精族最不愿看到的。所以我要找到适合修炼的人，给他们指出正确的方法，告诫世人修真不能随性乱来。</t><b/>" @
											"<t>在此之前，你需要拿来我的六张手稿，我把它们放在了城中的箱子里。</t><b/>";
		case 10005:	return "<t>手稿是我一位好友藏在城中的，其中记录了世界缓慢变化的证据。而这些，都是胡乱修真引起的……</t><b/>" @
											"<t>【叹了一口气】</t><b/>" @
											"<t>让我好好看看，之后你要去解决这些问题。在其中能获得丰富的修真经验，至少会知道什么是不能做的。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10505Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到无忧草野找到戊地</t><b/>";
		case 10002:	return "<t>天啊，这怎么可能？</t><b/>" @
											"<t>无忧草野的生物已经变异得十分古怪，来不及给你说其中细节了。你要火速去处理这件事。</t><b/>" @
											"<t>对了，我朋友叫戊地，就在无忧草野路口，你需要从映月渡口坐船到达。</t><b/>" @
											"<t>等什么，快去呀！</t><b/>";
		case 10005:	return "<t>不要被我的外形吓住，我是一名土元素怪族。</t><b/>" @
											"<t>许多任意而为的修真者胡乱的获取天地灵气，却在不知不觉中破坏了天地元素的平衡。</t><b/>" @
											"<t>无忧草野就是个典型的例子，我们要修复大地的伤痕，哪怕困难重重……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10506Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀死花脚草蚊，从它们身上收集蚊子的针刺，之后交给戊地</t><b/>";
		case 10002:	return "<t>我能感觉大地母亲的灵力变得很不稳定，灵力喷激，就好象有一座灵力火山随时会爆发！</t><b/>" @
											"<t>这太危险了，朋友。当务之急我们要获取一些灵力的样品。这里的许多蚊虫深受灵力感染，身体变异，是不错的材料。</t><b/>" @
											"<t>你先去取一些花脚草蚊的针刺，希望你看到那些蚊子时不会害怕，因为它们……已经变得很大……</t><b/>";
		case 10005:	return "<t>这些针刺上的灵力多的可怕，显然是蚊子在吸食时沾染了过量的灵力。</t><b/>" @
											"<t>我能听到大地母亲的痛苦，究竟是什么造成了此地的境况。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10507Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从花脚流蚊身上取到斑斓的蚊足，之后带给戊地</t><b/>";
		case 10002:	return "<t>这样的蚊子不止一种，有一种变异得更为夸张，不仅体型变大，还拥有了极高的毒性。</t><b/>" @
											"<t>这种蚊子叫花脚流蚊，我需要它们那斑斓的长脚。</t><b/>" @
											"<t>在击杀它们时不用留情，它们已经是邪恶之物了。</t><b/>" @
											"<t>愿天地元素保佑你。</t><b/>";
		case 10005:	return "<t>就是这个，小心些，不要沾上毒质。</t><b/>" @
											"<t>这些脚实在令人恶心，我却不得不仔细研究它。</t><b/>";
										
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10508Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>对花脚荧蚊使用【雷答】，虚弱它们，之后从它们身上取到荧光之翅，再向戊地回报</t><b/>";
		case 10002:	return "<t>我惊奇地发现这些足尖也沾了同样的灵力，灵力粘附在一种粉质的物品上，我还不清楚粉质物究竟是什么。答案越来越近了，朋友。</t><b/>" @
											"<t>你还要跑一次，这里有一种花脚荧蚊，我要它们的翅膀。</t><b/>" @
											"<t>嗯，翅膀在打斗时很容易毁坏，我有一种被称为【雷答】的物品，你将之掷往荧蚊，它会昏昏欲睡，接下来就好办多了。</t><b/>";
		case 10005:	return "<t>朋友，你回来的真快。让我看看这些翅膀。</t><b/>";
										
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10509Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>采集一些珠花后去找谷雨</t><b/>";
		case 10002:	return "<t>足尖与针刺上沾染了灵力，而翅膀没有，那些粉质物我猜测是蚊虫经常吸食的一种花粉。</t><b/>" @
											"<t>那种花粉来自于珠花，对于植物，我并不是很了解。我想你应该采集一定数量的珠花，再找一个理解植物的人看看。</t><b/>" @
											"<t>谷雨是探寻草野秘密的精族，她肯定能告诉你一些什么。</t><b/>" @
											"<t>带着珠花去找她吧，愿天地元素保佑你。</t><b/>";
		case 10005:	return "<t>珠花，这里是碧珠花海，满地都是珠花。</t><b/>" @
											"<t>戊地专门让你采来的？那我要仔细看看。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10510Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到西碧珠营地外、东碧珠营地外和北碧珠营地外看看碧珠人有什么变化，再去找谷雨</t><b/>";
		case 10002:	return "<t>这些珠花的颜色……有问题。</t><b/>" @
											"<t>珠花应该是碧绿无暇，但这些的根茎却完全泛黄，这很不正常。</t><b/>" @
											"<t>我认为是由于珠花灵力过盛。就算你不采，它们不久也会死去。这附近有许多碧珠人，它们依靠珠花而生，你去观察他们是否有什么变化。</t><b/>" @
											"<t>他们有三个营地，一个也别错过，站在远处看看就行。</t><b/>";
		case 10005:	return "<t>它们在打斗？果然也受到了影响……</t><b/>" @
											"<t>这些碧绿的小人曾经都是些性格温和的种族啊……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10511Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从碧珠居民身上取回一些绿顶珠给谷雨</t><b/>";
		case 10002:	return "<t>碧珠人显然发疯了，这些可爱生灵怎会变得如此暴躁。</t><b/>" @
											"<t>我们要找到事情的缘由，就不得不从他们身体上入手。杀了那些碧珠居民，取回他们的顶珠。</t><b/>" @
											"<t>尽管这有些残忍，但为了自然，不得不做！</t><b/>";
		case 10005:	return "<t>也许你原来没见过他们的顶珠，我可以告诉你，这些顶珠比原来的大多了。</t><b/>" @
											"<t>碧珠人被过盛的灵力冲晕了头，性情才如此大变。</t><b/>" @
											"<t>整片碧珠花海的灵力极度过量，不知何人所为。唉，总有许多修真者以为盲目获得灵力就能有所修为，实则是误入邪道。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10512Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到西碧珠营地偷取一些柔韧草茎和鲜艳的碧珠花交给谷雨</t><b/>";
		case 10002:	return "<t>我们要救这些可怜的种族，它们本是大自然的芸芸众生之一。</t><b/>" @
											"<t>朋友，我想碧珠人的暴躁是食用了珠花果实的原因，那是他们的食物。然而珠果沾了更多的灵力，应当被他们保护了起来。</t><b/>" @
											"<t>你先去偷一些碧珠人的柔韧草茎和鲜艳的碧珠花给我，我自有办法。这些东西在西营地，小心点。</t><b/>";
		case 10005:	return "<t>太好了，有了这些草茎和花朵，我们就能拯救碧珠人啦！</t><b/>";
										
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10513Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>穿上草香碧珠衣，变成碧珠人的样子，然后去找碧珠厨子</t><b/>";
		case 10002:	return "<t>能不开杀戒就勿滥杀，我们要尽量和平的解决碧珠人的问题。他们并不该承担这份罪责。</t><b/>" @
											"<t>我将你采集来的草茎和花朵做成了一件衣服，你穿上后就能变成他们的模样，应当还有一股淡淡的草香味，呵呵。</t><b/>" @
											"<t>变成他们的样子渗入他们内部，到他们的厨子处打听下，他肯定知道珠果藏在哪里。</t><b/>";
		case 10005:	return "<t>【碧珠厨子瞪着大大的眼睛看了你好一阵子】</t><b/>" @
											"<t>你……好瘦，要多吃点……不吃东西会变成笨蛋的，碧珠人都是聪明顶呱呱。</t><b/>" @
											"<t>你以后要多吃点，呃……我突然忘记今天有没有做午餐了。</t><b/>" @
											"<t>【碧珠厨子又瞪着大大的眼睛看着你】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10514Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>用厨子给你的网去网几个到处乱跑的碧珠小孩，然后带给碧珠厨子</t><b/>";
		case 10002:	return "<t>对了，我已经做了午饭了……我还吃了三颗珠果，其他人都只能吃一颗。</t><b/>" @
											"<t>【打了个又长又大的饱嗝】</t><b/>" @
											"<t>你想知道珠果在哪里，然后去偷吃吗？我也觉得你应该偷吃！你太瘦了！</t><b/>" @
											"<t>不过你要帮我做点事。那些讨厌的碧珠小孩吃了饭到处乱跑，你用这张网把他们网回来。</t><b/>" @
											"<t>呃……用网比较快，不用管他们痛不痛。</t><b/>";
		case 10005:	return "<t>哼哼哼！下次再跑我就把你们关在锅里。吃饭是为了养脑子的，不是拿来消耗体力的，小笨蛋们。</t><b/>" @
											"<t>【瞪着大眼睛看着你】</t><b/>" @
											"<t>你是一个聪明的碧珠人，至少接近我的智商了，如果你再胖点，兴许能成为一个绝顶聪明的碧珠人。</t><b/>";
										
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10515Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>找碧珠雅雅询问厨子中午去了哪里</t><b/>";
		case 10002:	return "<t>你快去偷珠果吃！快去！</t><b/>" @
											"<t>对了，我还没告诉你珠果藏哪里。是藏在……藏在……太好了，我忘了！</t><b/>" @
											"<t>不要担心，失忆对我而言是很平常的事，你去问问碧珠雅雅我中午去过哪里，就知道珠果在哪里了。</t><b/>" @
											"<t>【瞪着大眼睛又看着你】</t><b/>";
		case 10005:	return "<t>厨子，你也喜欢厨子？雅雅真是太喜欢他了，上次他抱了抱我，那个又软又弹的肚子好舒服。</t><b/>" @
											"<t>雅雅以绝顶的智慧告诉你，人生最美好的事就是睡觉，而更美好的事就是在厨子的大肚子上睡觉！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10516Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀了荧蚊女王后，去找碧珠雅雅</t><b/>";
		case 10002:	return "<t>雅雅可以告诉你厨子的行踪，但是你得帮我。</t><b/>" @
											"<t>我在午觉的时候总是被一只蚊子骚扰，那是一只大蚊子！我敢说它很蠢，居然敢叮我！</t><b/>" @
											"<t>蠢蚊子叫荧蚊女王，呸。你去帮雅雅拍死它，如果你杀不了它，你就是笨蛋；如果你杀了它，你还是笨蛋。不过我会告诉你厨子中午去了哪。</t><b/>" @
											"<t>快去啊，我困死啦！</t><b/>";
		case 10005:	return "<t>哇哦，这个蠢蚊子死了吧。哈哈哈，我终于可以舒舒服服地睡觉啦！</t><b/>" @
											"<t>你干得不错，是个有潜力的碧珠人，我看好你哟！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10517Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到河边的小山洞拿几颗珠果，之后回去找谷雨</t><b/>";
		case 10002:	return "<t>【打了个大大的呵欠】</t><b/>" @
											"<t>我困死了，厨子中午去了河边的小山洞，你去找你想找的东西吧。</t><b/>" @
											"<t>雅雅要睡觉了，这么好的天气，又没有蚊子的骚扰，一定要睡个天昏地暗，头晕眼花才行！</t><b/>";
		case 10005:	return "<t>看到你安全归来真是令人高兴。</t><b/>" @
											"<t>有了这些珠果，我们就能救这些碧珠人了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10518Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从碧珠居民身上收集一些碧色血精，然后对珠果使用，将之倒在碧色珠果上，启动治疗的能力和谷雨幻象</t><b/>";
		case 10002:	return "<t>我会施展法力，将这些珠果放在各个营地之中，使其具备治疗的能力。</t><b/>" @
											"<t>但要启动治疗，就需要用病人的血……你即刻赶往西碧珠营地，收集碧珠居民的血，然后将之倒在珠果之上。这样便会启动治疗的能力，去吧，我的幻影会与你交流。</t><b/>";
		case 10005:	return "<t>治疗的能力已经启动，这里的碧珠人会逐渐得到治疗。这需要一个过程，大概一两月的时间即可恢复正常。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10519Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从碧珠农夫身上收集一些青色血精，然后对珠果使用，将之倒在青色珠果上，启动治疗的能力和谷雨幻象</t><b/>";
		case 10002:	return "<t>我能感觉这里已经开始散发出新鲜的自然气息了。</t><b/>" @
											"<t>接下来我们要治疗东边的营地，如你之前一样，从碧珠农夫处取到青色血精，那里同样有珠果，把血精倒在珠果上。</t><b/>";
		case 10005:	return "<t>很好，这里的碧珠人也会慢慢得到恢复和治疗，他们的生活会重回平和。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10520Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从碧珠长老身上收集一些草色血精，然后对珠果使用，将之倒在草色珠果上，启动治疗的能力和谷雨幻象</t><b/>";
		case 10002:	return "<t>如今惟剩北方的营地了，还是老方法，不过这次你要取的是草色血精，这种血液只有碧珠人的长老才有。</t><b/>" @
											"<t>每个长老都有一名碧珠奴仆保护着，你要当心。取到血精后依然对我放置的珠果使用。</t><b/>" @
											"<t>请一切小心谨慎，我们就要做到了。</t><b/>";
		case 10005:	return "<t>【惊讶】</t><b/>" @
											"<t>奇怪！我完全感受不到治疗的能力，相反有一种炽热的灵力在向我反噬！</t><b/>" @
											"<t>【狂喷一口血】</t><b/>" @
											"<t>这……这是罪恶之源，就是……这股力量在搅乱大自然……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10521Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着谷雨的书信去找白桐</t><b/>";
		case 10002:	return "<t>【谷雨的幻像深深吸了一口气】</t><b/>" @
											"<t>这股力量我很熟悉，是一种花的力量，炽热、汹涌。这是火云花才具备的灵力。</t><b/>" @
											"<t>若我猜得不错，这里一定有花精在修炼，她不惜一切代价的采集天地灵力，崩溃了无忧草野的平衡。</t><b/>" @
											"<t>【叹了口气】</t><b/>" @
											"<t>可惜我受了很重的伤，在前方有我的姐妹白桐，你带着这封信去找她，她会帮助你的。</t><b/>";									
		case 10005:	return "<t>我的姐妹……谷雨受了伤……</t><b/>" @
											"<t>【接过书信，默默看完】</t><b/>" @
											"<t>感谢谷雨，我一直以来都在寻找破坏这片大自然的源头，现在她终于找到了。</t><b/>" @
											"<t>朋友，谷雨的血不会白流。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10522Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>进入游花丛，杀一些粉红花精和映日花精后，探查游花丛中有什么</t><b/>";
		case 10002:	return "<t>后方是游花丛，火云花女应该就在其中修炼。</t><b/>" @
											"<t>我屡次进入游花丛，总是被一些花精阻碍。碍于同为精族，我并未与之争执。现今想来，她们应当都是火云花的手下。</t><b/>" @
											"<t>为了大自然，不要留情，杀了她们，并查看游花丛中究竟有什么。</t><b/>";
		case 10005:	return "<t>里面有个类似花座的建筑吗？在游花丛的角落里？</t><b/>" @
											"<t>那必是采集灵力的工具，火云花女应当很看重。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10523Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>将白梧桐种子种植在花盘之下，毁掉花盘，同时引出火云花女，击败她后去找白桐</t><b/>";
		case 10002:	return "<t>花座是火云花女用来采集灵力的工具，若我们破坏了花座，碧珠花海自然会慢慢恢复平衡。</t><b/>" @
											"<t>带着这个种子，种植在花座下后，它会成长为一颗大树，进而破坏掉花座。不过种下种子会引来火云花女，少不了恶战。</t><b/>" @
											"<t>准备好了就去打垮她，胡乱破坏自然的精族不应该存在！</t><b/>";
		case 10005:	return "<t>你拯救了这里的自然。大自然的力量是无限的，它会缓慢的自我调节，恢复正常。</t><b/>" @
											"<t>谢谢你，朋友。</t><b/>" @
											"<t>【白桐感激地看着你】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10524Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着白桐给你的梧桐叶，去见散仙檀郎</t><b/>";
		case 10002:	return "<t>修炼不仅仅是能力的提升，更重要的是心境的历练。要想在这个世界有不凡的能力，就要爱这个世界，善良和良知是如此重要。</t><b/>" @
											"<t>呵呵，感谢你的仗义相助，我还要收拾下烂摊子。作为感谢，我会引荐你去一名散仙处，他一身本事，能带领你到达另一个境界。</t><b/>" @
											"<t>带着这片梧桐叶去找他吧。大自然会看着你成长，聆听你的心声，护佑你。</t><b/>" @
											"<t>【闭上眼对你诵念精族的祝福语言】</t><b/>";
		case 10005:	return "<t>【醉醺醺……看着你给他的梧桐叶，仔细看了半天】</t><b/>" @
											"<t>这是什么饼……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10525Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>收集三卷诗的章节然后合成出完整的三卷诗,交给檀郎</t><b/>";
		case 10002:	return "<t>今朝有……有酒今朝醉，千里江陵一日还……白毛浮绿水，红掌拨清波……好诗啊好诗！</t><b/>" @
											"<t>我之前写的诗被风吹散了，你得帮我找回来，这些诗共分三卷。依次是八荒剑气卷、映月辉星卷、两情依依卷，每卷各有两章。前两卷在这附近的草堆里就能找到，后一卷在般若虚土的和尚身上，不知道那些秃驴拿这些情诗做什么？</t><b/>" @
											"<t>听清楚了？快去快回！</t><b/>";
		case 10005:	return "<t>【醉醺醺】</t><b/>" @
											"<t>想必你也翻看过，是否觉得我文武双全，才高八斗，佩服得五体投地啊……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10526Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到附近收集柳橙和青果，再从淡香花精身上取得花粉后，交给檀郎</t><b/>";
		case 10002:	return "<t>找回了我的宝贝诗，我总要请你喝两盅，可惜这壶酒已空空如也。</t><b/>" @
											"<t>平时我都自己酿酒，你可以随我学习酿酒之道，我只酿制天上才有的仙酒！</t><b/>" @
											"<t>酒叫百草酒，乃一种花果酒，口味清新，甘甜芬芳。材料是柳橙，青果和花粉。</t><b/>" @
											"<t>柳橙和青果在附近掉得满地都是，而花粉你需要从淡香花精身上取来。</t><b/>";
		case 10005:	return "<t>闻到这清新的香味了么，虽没有关外白酒的辛辣醇厚，但胜在香气四溢，酒气与花果交互缠绕，美味之极。</t><b/>" @
											"<t>来！尝尝！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10527Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>把酒给张南、周明和无忧仙居的沐风喝，听听他们的评价，之后找沐风谈谈</t><b/>";
		case 10002:	return "<t>觉得如何？是否如若登云飞天，妙不可言。</t><b/>" @
											"<t>【檀郎哈哈大笑】</t><b/>" @
											"<t>我们还有多的酒，可以给更多人尝尝，听听他们的评价。你将这些酒分别给张南和周明，他们都在这附近，最后把酒带给无忧仙居的沐风，让那老家伙尝尝真正的佳酿！哈哈哈！</t><b/>";
		case 10005:	return "<t>唉，贪狼星君依然如此随性潇洒，我只是区区小仙，他堂堂神族，也时常将我记挂着。</t><b/>";
			
		case 201:	return $Icon[2] @ $Get_Dialog_GeShi[31204] @ "评酒</t>";
		case 200: return "<t>这是酒？如此清淡，我还以为是果汁，这实在不怎么样啊……</t>";
	  case 300: return "<t>好！好果汁！橙汁中有一股花香，好！</t>";
	  case 400: return "<t>檀郎？哈哈，他其实是贪狼星君，响当当的神族！不过这酒味道嘛，呵呵……的确普通了。</t>";
		   
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10528Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小兄弟";
	else
		%Sex = "小姑娘";

	switch(%Tid)
	{
		case 10001:	return "<t>杀掉一些凤凰花精后向沐风回报</t><b/>";
		case 10002:	return "<t>自从你来之后，这附近的凤凰花精总是在此徘徊，而且面有凶意，不知发生了何事？</t><b/>" @
											"<t>" @ %Sex @ "，我感觉这些花精似乎对你很有成见，难道你之前有得罪她们？</t><b/>" @
											"<t>不管如何，她们在老头子家门口晃荡总是不好，你去将她们赶走吧。</t><b/>";
		case 10005:	return "<t>你身手倒是很麻利，难怪贪狼星君那么喜欢你。要知道，他一般对人是不理不睬的。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10529Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>找到凤姝后，杀掉她，并从她那里找到花精愤怒的蛛丝马迹，之后向沐风回报</t><b/>";
		case 10002:	return "<t>你究竟杀了多少花精，老夫感觉这些花精非但不少，隐隐还有更多。</t><b/>" @
											"<t>你能感觉她们的仇视更强烈了吗？我想附近应当有一名领导者，杀掉她才能解决根本问题。说不定还能从她那里得知她们愤怒的缘由。</t><b/>";
		case 10005:	return "<t>原来是因为贪狼星君让你杀了花精，才导致她们的仇恨……而如今，似乎这事越做越不对劲了……</t><b/>" @
											"<t>这兴许正是贪狼星君给你的历练。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10530Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>将戒指给稻草人戴上，看看会发生什么，再找沐风回报</t><b/>";
		case 10002:	return "<t>不知是幸运还是你有足够的毅力，你并没有带上这个戒指。</t><b/>" @
											"<t>你定然想知道戴上它会怎样，现在你把这个戒指给一旁的稻草人戴上，看看会发生什么。我相信你不会喜欢的，呵呵。</t><b/>";
		case 10005:	return "<t>是不是吓出了一身冷汗，总之我们现在知道了，这是凤凰花精的报复。</t><b/>" @
											"<t>勿用担忧，对于修真的困难来说，这点危险微不足道。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10531Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去找觉空谈谈，寻求他的帮助</t><b/>";
		case 10002:	return "<t>你已经过了贪狼星君的一道试炼，也许过不了的人就戴上了戒指，然后烧为飞灰……</t><b/>" @
											"<t>对于花精的问题，是时候补救了。你需要去找觉空，他是佛宗的大德之士，会告诉你该如何做。</t><b/>";
		case 10005:	return "<t>阿弥陀佛，此地凤凰花精并非邪物，施主又何必杀生？</t><b/>" @
											"<t>万物有生有死，死即是生，生死相交。</t><b/>";
										
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10532Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到焚烬园取到空中火，然后对凤凰花精残骸使用，再向觉空回报</t><b/>";
		case 10002:	return "<t>凤凰花精乃沾染凤凰气息而生，拥有凤凰神鸟的涅磐之息，故而救之性命也无非不可。</t><b/>" @
											"<t>你应听过浴火重生，花精正是如此。到焚烬园取得空中火，然后将之在花精的尸体上燃烧，如此她们可得新生。切记，空中火有火灵守护，施主需小心行事。</t><b/>" @
											"<t>愿施主带上慈悲之心，阿弥陀佛。</t><b/>";
		case 10005:	return "<t>安善好此事，花精应当不会再对施主那么仇视。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■第二章■■■■■■■■■■■■■■■■
function Mission10540Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从河流精灵处取得一些精灵项圈后去找觉空</t><b/>";
		case 10002:	return "<t>施主，尽管花精得到了新生，但你毕竟杀过她们，不得不考虑她们母亲的感受。</t><b/>" @
											"<t>她们的母亲乃神鸟凤凰，其神力之威，我们实在不可与之并论。</t><b/>" @
											"<t>若你能帮她做点事，兴许会好些。西边湖畔有许多河流精灵，凤凰一直很痛恨这种污染湖水的邪恶生灵，如果你能带着它们的项圈回来，那最好不过。</t><b/>";
		case 10005:	return "<t>有了这些项圈，凤凰兴许会原谅你的鲁莽。</t><b/>" @
											"<t>不用过于担忧，凤凰乃神禽，属于真神，不会蛮横无理。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10541Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着觉空给你的项圈和书信去见凤凰</t><b/>";
		case 10002:	return "<t>施主，我相信此事会得到一个圆满的解决，现在你就去找神禽凤凰，带着项圈和这封信。凤凰不会为难你的。</t><b/>" @
											"<t>阿弥陀佛，佛祖保佑。</t><b/>";
		case 10005:	return "<t>你竟敢大胆找上门来！若非我看你存有善念，此刻你已成为一滩灰烬！</t><b/>" @
											"<t>【尽管凤凰看着很虚弱，但在她的怒视下，你仍感觉浑身上下犹如浇过岩浆】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10542Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>用凤凰之羽对凤凰花使用，之后去找凤凰</t><b/>";
		case 10002:	return "<t>你应该明白你的处境，凡人！</t><b/>" @
											"<t>凤凰花精乃浸染吾之灵力修炼而成，你杀害她们就如同杀了我的女儿，这是藐视神族！</t><b/>" @
											"<t>你还要继续弥补你的过错，拿着这片羽毛，对醉花荫里的凤凰花使用，让它们沾染我的灵力。</t><b/>" @
											"<t>快去，我再多看你一眼就忍不住将你烧成灰！</t><b/>";
		case 10005:	return "<t>哼，你倒是很听话。如此最好，还能保住你的小命。</t><b/>" @
											"<t>我能感觉新的花精已经在孕育，生命的气息真是迷人。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10543Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从沉香和闻香身上取得一些花盘和花冠，带给凤凰</t><b/>";
		case 10002:	return "<t>作为我的女儿，花精中有一些却狂妄自大，以为自己有些修为，就作恶多端。若非我正值修炼周期，疲惫不堪，我会亲自赐予她们毁灭！</t><b/>" @
											"<t>【凤凰斜斜看着你】</t><b/>" @
											"<t>别以为我很虚弱你就能逃跑，要惩治你只需要轻轻吹一口气。</t><b/>" @
											"<t>你即刻去杀掉沉香和闻香，把他们的花盘和花冠给我带来。</t><b/>";
		case 10005:	return "<t>【看着这些花盘和花冠，凤凰眼睛里流露出一丝痛苦，但转瞬即逝】</t><b/>" @
											"<t>她们……的确应当付出代价……</t><b/>" @
											"<t>你做的不错，先到一旁等等吧，我想一个人静静。</t><b/>" @
											"<t>【凤凰再看了一眼花盘花冠，陷入了深思】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10544Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀了知香，拿到花语牢笼钥匙，放了被囚禁的男人</t><b/>";
		case 10002:	return "<t>她们的修炼进入了误区，她们竟以男性的阳气修炼，这太荒唐了。我相信你在击杀她们时看到了许多牢笼，里面关了那些男人。</t><b/>" @
											"<t>拥有钥匙的是她们的首领，叫知香。杀了她，拿到钥匙，放了那些可怜的人。</t><b/>" @
											"<t>去吧，杀了知香，不要……留情……</t><b/>" @
											"<t>【你从凤凰的眼睛里读出了极度的悲伤】</t><b/>";
		case 10005:	return "<t>都放走了吗？很好……</t><b/>" @
											"<t>凡人，你洗净了你的罪孽，还证明了你是一个值得信任的修真者。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10545Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>骑上凤凰之灵到浮弥处</t><b/>";
		case 10002:	return "<t>你是一个值得信任的人，有一件事我觉得你应该了解一下，那对修真来说，极有帮助。</t><b/>" @
											"<t>西南面的般若虚土原本是一处净地，由许多金刚明王守护，但近来出了许多麻烦，你现在去找守护僧浮弥。</t><b/>" @
											"<t>我会召唤凤凰之灵送你过去，准备好了就说一声。</t><b/>";
		case 10005:	return "<t>佛祖保佑，终于有人来了，这里在最近发生了翻天覆地的变化！</t><b/>" @
											"<t>有一大堆麻烦事要解决，唉，该如何下手。</t><b/>";
											
		case 200:	return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我准备好去般若虚土了</t>";
		case 300:	return  $Icon[4] @ $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10546Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉附近的般若明王和般若金刚后去找浮弥</t><b/>";
		case 10002:	return "<t>金刚和明王叛变了！他们在一个邪佛的领导下，亵渎了信仰，不再敬畏西天佛祖，而选择了邪恶之境。</t><b/>" @
											"<t>曾经的圣地变成了废墟，这里的三尊佛像被他们破坏，不能再如此下去。</t><b/>" @
											"<t>佛心宏大，但不可容忍亵渎。去杀了般若金刚和般若明王，他们选择的是永无超生的无间地狱！</t><b/>";
		case 10005:	return "<t>阿弥陀佛，这是他们该付的代价。既然堕入魔道，背离众生，那除魔卫道乃佛家本职。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10547Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>找到南无月灯明佛首、南无月灯光佛首、南无智灯明佛首分别在何处，并将之回报给守护僧浮弥</t><b/>";
		case 10002:	return "<t>这里曾是南无月灯明佛、南无月灯光佛、南无智灯明佛的净土，如今他们的尊贵佛像已被破坏，残缺不全。</t><b/>" @
											"<t>施主，你能去找到三佛的佛首吗？西天的尊严不能被随意践踏，吾等立誓，总有一天会歼灭邪佛！</t><b/>" @
											"<t>找到佛首后到菩提枝找释厄，他会带领你净化此处。</t><b/>";
		case 10005:	return "<t>邪佛原本是西天诸佛之一，但最终背离了信仰，如今不知藏身何处。</t><b/>" @
											"<t>邪佛培养了大量的信徒，他们称他十方混沌佛祖，唉……心兮魔兮，我们追寻的是极乐净土，来世圆满，他们所追寻的又是什么？</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10548Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>收集佛像的碎片交给释厄</t><b/>";
		case 10002:	return "<t>佛像受到了毁灭性的破坏，他们一定是想得到佛像的灵力，我们决计不容！</t><b/>" @
											"<t>我请你去找回佛像的碎片，但这些碎片被许多镇塔金刚贪婪的守护着，你要击败他们。</t><b/>" @
											"<t>阿弥陀佛，这片土地如今太过可怕，我们不能让其蔓延。</t><b/>";
		case 10005:	return "<t>阿弥陀佛，我们收集了尽可能多的碎片，没让邪恶过分的蔓延。这都是施主你的功德。</t><b/>" @
											"<t>佛祖保佑你。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10549Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉大罗千叶明王，然后与释厄谈谈</t><b/>";
		case 10002:	return "<t>还有最后一件事要做，就是消灭他们在此地的首领。</t><b/>" @
											"<t>般若虚土的首领叫大罗千叶明王，作为邪佛的手下之一，他带领了金刚和明王毁坏一切。</t><b/>" @
											"<t>大罗千叶明王就在此地深处，面对他，你要小心。</t><b/>" @
											"<t>他应该下地狱，却永远得不到超度。</t><b/>";
		case 10005:	return "<t>阿弥陀佛，此地终于重归宁静，但十方混沌佛的背叛并未彻底根除。</t><b/>" @
											"<t>施主功德无量，贫僧感激不尽。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

//■■■■■■■■■■■第二章■■■■■■■■■■■■■■■■

//■■■■■■■■■■■■■■■第三章■■■■■■■■■■■■
function Mission10580Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着木中火回去找神鸟凤凰</t><b/>";
		case 10002:	return "<t>施主，我还要打扫战场的残遗，你的恩德，极乐西天会永远牢记。</t><b/>" @
											"<t>现在你要回到凤凰神鸟身边。我知道她正处修炼周期，需要这件东西，请你带给她，这是西天对她的敬意。</t><b/>" @
											"<t>此路遥远，我会施佛法送你去醉花荫，准备好后跟我说说。</t><b/>";
		case 10005:	return "<t>你在般若虚土的事我已知道了。</t><b/>" @
											"<t>【凤凰强打起精神】</t><b/>" @
											"<t>你应该看到我很虚弱，因为每隔三百年，我就会进入修炼周期，突破之后，我的神力会极大的激进！</t><b/>" @
											"<t>释厄让你带来的是木中火，是我突破修炼的关键物之一。</t><b/>";
											
		case 200:	return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我准备好去醉花荫了</t>";
		case 300:	return  $Icon[4] @ $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>"; 
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10581Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>神鸟凤凰让你到焚烬园取得空中火，到黑岩石窟中取得石中火</t><b/>";
		case 10002:	return "<t>修炼的终极需要我先死，再死而复生，正是凤凰涅磐！</t><b/>" @
											"<t>我需要死于纯正的三昧真火，释厄让你带来的木中火乃三昧之一昧。还需要石中火和空中火。</t><b/>" @
											"<t>空中火就在焚烬园中，石中火在黑岩石窟能找到。拿取这些真火时小心点，它们都有火灵守护。</t><b/>";
		case 10005:	return "<t>【凤凰虚弱地看着你】</t><b/>" @
											"<t>都找齐了么？不错，本神没有看错人。给我，我来将之融合为三昧真火。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10582Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>对凤凰使用三昧真火助其凤凰涅磐</t><b/>";
		case 10002:	return "<t>我已经将三种火融合为三昧真火，而你要做的事，便是用三昧真火焚烧我。</t><b/>" @
											"<t>呵呵，那并不算渎神，你这是在助我修炼。</t><b/>" @
											"<t>手不要抖，把火焰浇在我身上，睁大眼看本神的变化！</t><b/>";
		case 10005:	return "<t>【凤凰仰天长鸣，清亮高亢的声音使你振聋发聩】</t><b/>" @
											"<t>感谢你，凡人。你帮助我度过了一次难关，并且你突破了重重困难，走到了这一步。</t><b/>" @
											"<t>你的信念和坚韧注定你会成为神族的一员，我期待着你以神族的身份再次与我见面。</t><b/>" @
											"<t>【凤凰温和地看着你】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10583Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着一根凤羽去找檀郎，也就是贪狼星君</t><b/>";
		case 10002:	return "<t>重看此事，应当是贪狼星君为你安排的一场历练。现在你可以回去找他了，带着我的羽毛，这是你通过历练的证明。</t><b/>" @
											"<t>我们还会再见面的。</t><b/>";
		case 10005:	return "<t>【醉醺醺】</t><b/>" @
											"<t>你……好面熟……贪狼星君？檀郎即檀郎，如何又贪狼……</t><b/>" @
											"<t>凤凰……你……不错……凤凰……回来了啊……</t><b/>" @
											"<t>【语无伦次，接着就吐了】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10584Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>与檀郎对话，进入试炼迷境</t><b/>";
		case 10002:	return "<t>你是来修炼的吗？你说什么酒？什么诗？我什么都不知道啊。</t><b/>" @
											"<t>【你感觉之前所谓酿酒作诗都是他下的套，让你杀了花精，从而不得不跟凤凰修炼】</t><b/>" @
											"<t>咳咳……听你说你跟凤凰修炼过，我得考考你的本事……</t><b/>" @
											"<t>敢接受我的试炼吗？我们要去一个宽敞点的地方。</t><b/>";
		case 10005:	return "<t>这里是我制造的试炼迷境，我们就在这里开始。</t><b/>" @
											"<t>如果你害怕了，不敢继续，可以随时选择出去。当然，若你能坚持到最后，我会亲自送你回草野。</t><b/>"; 
											
		case 200:	return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我准备好去试炼迷境了</t>";
		case 300:	return  $Icon[4] @ $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>"; 
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10585Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>击败迷境妖蚊后回报檀郎</t><b/>";
		case 10002:	return "<t>先从简单的开始，妖蚊是一种毒性强，攻击性一般的妖虫。</t><b/>" @
											"<t>我会召唤它出现，过程很简单，打败它就行！</t><b/>" @
											"<t>不要让我失望。</t><b/>";
		case 10005:	return "<t>不错不错，比我想象的时间还要短，但这只是第一步，是最简单的。</t><b/>";
		
		case   200: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "首先是妖蚊，做好准备！</t>";
										
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10586Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小子";
	else
		%Sex = "小丫头";

	switch(%Tid)
	{
		case 10001:	return "<t>击败迷境河流精灵后回报檀郎</t><b/>";
		case 10002:	return "<t>接下来是一种诡异肮脏的生物，河流精灵。</t><b/>" @
											"<t>他们常常聚集在河边，污染水质，相信你也见过了。它们可比小蚊子可怕多了。</t><b/>" @
											"<t>还敢继续吗？</t><b/>";
		case 10005:	return "<t>哟！还真有些本事！河流精灵也非你敌手！有点修为啊，" @ %Sex @ "！</t><b/>";
		
		case   200: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "接下里是河流精灵的试炼，做好准备！</t>";	
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10587Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小子";
	else
		%Sex = "小丫头";

	switch(%Tid)
	{
		case 10001:	return "<t>击败迷境碧珠人后回报檀郎</t><b/>";
		case 10002:	return "<t>你应该见过碧珠人，他们明明很愚蠢，但总觉得自己是全天下最聪明的人。</t><b/>" @
											"<t>不过他们倒是很可爱……</t><b/>" @
											"<t>也许你和一些碧珠人交过手，但我相信他们都不算强。但我这里的……很厉害……</t><b/>" @
											"<t>" @ %Sex @ "，若不打退堂鼓，就继续吧！</t><b/>";
		case 10005:	return "<t>尽管我知道结局会如此，但我还是有些惊讶，你是少许能进行到第三步的修真者。</t><b/>" @
											"<t>希望你能坚持到最后。</t><b/>";
											
		case   200: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "碧珠人的试炼开始，做好准备！</t>";										
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10588Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>击败迷境花精后回报檀郎</t><b/>";
		case 10002:	return "<t>花精是无忧草野遍布最广的生灵，她们浸染了凤凰的神力，强大而美丽。</t><b/>" @
											"<t>我不相信你能凭一己之力击败一个花精，如果你觉得你行，那就证明给我看。</t><b/>" @
											"<t>好好思量清楚，修炼的真义并非逞能。</t><b/>";									
		case 10005:	return "<t>你做的非常好！你是我见过最出色的修真者，从未有人能走到这一步，然而你做到了。</t><b/>" @
											"<t>接下来是最后一步，也是无人挑战过的一步，我希望你继续！</t><b/>";
											
		case   200: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "花精即将到来，做好准备！</t>";									
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10589Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小子";
	else
		%Sex = "小丫头";

	switch(%Tid)
	{
		case 10001:	return "<t>击败迷境明王后回报檀郎</t><b/>";
		case 10002:	return "<t>这是最终阶段，从未有人能达到这个程度。</t><b/>" @
											"<t>如果你现在想打退堂鼓，我表示理解，因为对手将会十分强大。</t><b/>" @
											"<t>无忧草野的明王是邪佛混沌佛祖的部下，他亵渎了自己的信仰，一心追随混沌佛祖，在修真路上步入邪途。</t><b/>" @
											"<t>你要击败他，只要这一步胜利了，我保证你之前付出的辛苦都将会有回报！</t><b/>";
		case 10005:	return "<t>" @ %Sex @ "，你没让我失望，的确是个修神的好苗子。我相信有一天你会踏过三十三天门，封入神谱。</t><b/>" @
											"<t>这些都是我的宝贝，随意取拿。之后我会送你回到无忧草野。</t><b/>";
											
		case   200: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "迷境明王即将到来，做好准备！</t>";										
											
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10590Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到月幽境去找酒剑仙，把酒给他</t><b/>";
		case 10002:	return "<t>你在无忧草野的表现十分好，接下来我想介绍你认识一个人，你会很喜欢他的。</t><b/>" @
											"<t>他叫酒剑仙，就在月幽境，记得带上这壶酒。对他而言，有酒什么都好说，哈哈。</t><b/>";
		case 10005:	return "<t>好酒！你刚进城时我就闻到了，这是什么酒，竟有如此香味。</t><b/>" @
											"<t>小朋友，这就是给我的？老夫还说向你讨点来喝呢，解馋解馋！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}
//■■■■■■■■■■■■■■■第三章■■■■■■■■■■■■
