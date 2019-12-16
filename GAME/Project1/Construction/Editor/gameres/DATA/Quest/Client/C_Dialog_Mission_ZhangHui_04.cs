//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端任务对话，【章回任务】第四卷
//==================================================================================

//【章回任务】
//第一编号为【卷】，每一张地图为一卷，每一卷的任务编号间隔数为200，第二编号段为【章】，每一卷暂定为三章，每一章的任务编号间隔数为40，附章间隔为20，最后多余20个编号。
//目前编号段规划，最大可至24卷，24*3=72章，即可以满足72张地图所需的章回任务，视后期任务需求，可挪用一部分的支线任务编号段

//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■
function Mission10700Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "好小子";
	else
		%Sex = "好丫头";

	switch(%Tid)
	{
		case 10001:	return "<t>到杂货商人鬼见愁处购得陈年老酒，带给酒剑仙</t><b/>";
		case 10002:	return "<t>光这些酒根本不够我喝！你瞧，我还站这么稳，看东西也清楚得过分！</t><b/>" @
											"<t>我喜欢那种晕乎乎、眼花花的感觉。</t><b/>" @
											"<t>到杂货店买点酒来，要劲大味厚的陈年老酒。</t><b/>" @
											"<t>杂货商人鬼见愁这几天正卖好酒呢，快去快去！</t><b/>";
		case 10005:	return "<t>哈哈！好，" @ %Sex @ "！</t><b/>" @
											"<t>我能好好喝一顿了！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10701Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去和月幽境的龙飞谈谈</t><b/>";
		case 10002:	return "<t>【醉醺醺，有点东倒西歪】</t><b/>" @
											"<t>这种感觉太好了，贪狼星君一定很清楚醉酒的快感。</t><b/>" @
											"<t>好吧，【" @ %Player.GetPlayerName() @ "】，我告诉你一件事情：月幽境并非表面上那么宁静平和，镇守城池的精怪两族为一件事正十分头痛。</t><b/>" @
											"<t>具体什么事你去找龙飞，他就在城西。</t><b/>";
		case 10005:	return "<t>吾乃龙飞，昆仑宗圣之弟子。</t><b/>" @
											"<t>看你行路匆匆，找我何事？</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10702Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>龙飞让你去找谢竺君了解月幽境的麻烦事</t><b/>";
		case 10002:	return "<t>月幽境需要得到帮助，然而精族和怪族却束手无策，我猜测是他们内部出了问题。</t><b/>" @
											"<t>我之前询问过卫兵谢竺君，她犹犹豫豫没说太多，你再说问问她吧，若有为难处，昆仑决计不会袖手旁观！</t><b/>";
		case 10005:	return "<t>我们……月幽境……唉，之前有一个热心的圣族也来询问过，我该不该说呢？</t><b/>" @
											"<t>这事关系着月幽境的颜面啊……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10703Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>和谢竺君谈后去找龙飞</t><b/>";
		case 10002:	return "<t>【谢竺君咬着嘴唇，突然定睛看着你】</t><b/>" @
											"<t>这事十分棘手，我们的确需要帮助。</t><b/>" @
											"<t>精族自古以来都在研究巨木的秘密，收集了许多巨木截断前的树枝，然而一根树枝在三天前突然站了起来！</t><b/>" @
											"<t>它化为了一个强壮的树精，四处破坏，我们理应毁灭它，可那是巨木的树枝啊！那是神的遗产……我们压住了此事的传播，却束手无策。</t><b/>" @
											"<t>你能帮助我们吗？</t><b/>" @
											"<t>【得知缘由，去找龙飞谈谈吧】</t><b/>";
		case 10005:	return "<t>原来如此，精族敬仰的巨木的树枝在作恶，她们不知该如何处置。</t><b/>" @
											"<t>那树枝理应神圣无比，怎会突然变得如此？这其中必有缘由。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10704Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>答应龙飞去找谢竺君，她会带你去见巨木树枝精，击败巨木树枝精，然后查看会有什么蛛丝马迹</t><b/>";
		case 10002:	return "<t>我并非精族，故而无须忌讳，你愿随我一起查询真相吗？</t><b/>" @
											"<t>巨木树枝变异，我们就要从树枝本身下手，去找谢竺君，让她引你去找树精，击败它，再看看有什么蛛丝马迹。</t><b/>" @
											"<t>若为正义行事，则百无禁忌，不必在意任何。</t><b/>";
		case 10005:	return "<t>你击溃它后，它就变回了这根树枝？</t><b/>" @
											"<t>这根树枝浑体漆黑，一股怨气从中升腾，好是奇怪。</t><b/>";
											
	  case 200: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "遮月之泪（三）</t>";		
	  							
	  case 201: return "<t>你……真的要见它，请当心，它真的疯了。</t>";
	  	
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10705Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>将发黑的枝桠给月幽境的芷歌看看</t><b/>";
		case 10002:	return "<t>按理来说，精族的植物应当是优雅高贵，可这截树枝乌黑丑陋，怨气阵阵，极为诡异。</t><b/>" @
											"<t>你要把树枝带给月幽境的女祭司芷歌，告诉她树精作祸的麻烦已被解决，但真正的麻烦应该是这截树枝背后的事情。</t><b/>";
		case 10005:	return "<t>你们帮我们解除了树精的威胁？真是感谢，可是……巨木……</t><b/>" @
											"<t>【你将发黑的枝桠给芷歌，芷歌轻轻发出惊呼】</t><b/>" @
											"<t>太可怖了！怎会变成此形貌！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10706Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到映波池舀一杯水，然后去找芷歌</t><b/>";
		case 10002:	return "<t>我想请你去映波池舀一杯水来，那里的水能洗净树枝上的怨气，解脱它痛苦的灵魂。</t><b/>" @
											"<t>带着这个杯子，快去快回，好吗？</t><b/>";
		case 10005:	return "<t>映波池的水能洗净树枝的怨气，现在请你稍微等等。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10707Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到遮月森林找叶潇潇谈谈</t><b/>";
		case 10002:	return "<t>遮月森林有麻烦了，而且不会是小事。</t><b/>" @
											"<t>你能去一趟森林吗？森林入口处，我们的守林者叶潇潇就在那里，她会告诉你更多细节。</t><b/>" @
											"<t>感谢你在月幽境的义举。</t><b/>" @
											"<t>【芷歌对你诵念祈福的精族语言】</t><b/>";
		case 10005:	return "<t>太好了，终于派人来遮月森林了！</t><b/>" @
											"<t>遮月森林现在有大麻烦，你绝非来观光的！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10708Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从扭曲的树枝精身上取到树枝的残本，交给叶潇潇</t><b/>";
		case 10002:	return "<t>你能听到森林的怒吼吗？大自然出了问题。</t><b/>" @
											"<t>许多掉落的树枝都变成了可怕的妖怪，它们漫山遍野，如僵尸般在地上蹒跚！</t><b/>" @
											"<t>现在只是一部分树枝幻化为了妖怪，你可以想象如果整座森林的树枝都变异了，那会是何等恐怖！</t><b/>" @
											"<t>带点树枝的残本回来，最好我们能找到变异的原因。</t><b/>";
		case 10005:	return "<t>【叶潇潇接过树枝，细细看后闻了闻】</t><b/>" @
											"<t>样子看上去似乎没什么毛病，但是……有股怪怪的味道，这个味道我很熟悉，容我想想。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10709Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从深斑树枝精身上取回树枝的末梢</t><b/>";
		case 10002:	return "<t>树枝精变异的速度非常快，在很短的时间内已经成百上千，如果加以时日，整座森林都会被占领！</t><b/>" @
											"<t>一种树枝我无法准确的告诉你有什么，你能再跑一趟吗？我需要深斑树枝精身上的树枝的末梢。</t><b/>";
		case 10005:	return "<t>树枝精的变异十分紧急，它们虽然单个威胁不大，但数量上让人焦急……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10710Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>采集一些腐化光菇，带给夜光林的木棉</t><b/>";
		case 10002:	return "<t>两种树枝都有共同的味道，放在一起后更加浓烈！</t><b/>" @
											"<t>【叶潇潇又闻了闻】</t><b/>" @
											"<t>这是腐化光菇的味道，难道是这些蘑菇出了问题？朋友，请你采集一些腐化光菇，将之带给夜光林深处的木棉，好吗？</t><b/>";
		case 10005:	return "<t>感谢你带来这些光菇，它们沾染了邪气。事实上，不仅仅是光菇，森林的每一种植物都开始变异了，整座森林的邪化只是迟早之事。</t><b/>" @
											"<t>究竟是什么……让这片温和的森林变得如此恐怖……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10711Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>拿着木棉给予的灼炎花，烧掉落木谷的树枝</t><b/>";
		case 10002:	return "<t>森林在低吼……山风带着愤怒和狂躁，想摧毁一切！</t><b/>" @
											"<t>大量的树枝精十分可怕，他们成千上百，而且还在不断的增多，大自然痛恨邪恶的存在！</t><b/>" @
											"<t>在落木谷，很多树枝掉落在那里，未雨绸缪，要尽快处理掉那些尚未变异的树枝。</t><b/>" @
											"<t>带着这朵灼炎花，烧掉你看见的树枝。</t><b/>";
		case 10005:	return "<t>那些树枝都烧掉了么？</t><b/>" @
											"<t>我们尚在遮月森林外围，里面究竟有多恐怖，令人不敢猜测……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10712Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉一些夜梦兔后去找木棉</t><b/>";
		case 10002:	return "<t>你知道月神吗？十二月神之一，嫦娥，是飞花谷的上代谷主。她曾经就居住在这片森林中。</t><b/>" @
											"<t>她养过一只玉兔，并留下了子嗣在这里繁衍，我们叫它们为夜梦兔。</t><b/>" @
											"<t>夜梦兔如今随同森林一起邪化了，成为了不小的威胁，请你去除掉它们吧。它们会毁了森林的。</t><b/>";
		case 10005:	return "<t>谢谢你……也许它们曾经是神族的遗嗣，但现在却是邪恶的化身。</t><b/>" @
											"<t>飞花谷的上代谷主是一朵玉簪花，后来修炼成神，也就是十二月神之一，嫦娥。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10713Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从落木谷的映月兔身上取到映月兔心，再收集一些发光的树皮后去找木棉</t><b/>";
		case 10002:	return "<t>我们一直在追寻森林为何会邪化，要知道，这曾是嫦娥居住过的森林。</t><b/>" @
											"<t>落木谷的某些东西值得我们研究，我需要几颗兔心和发光的树皮，兔心在映月兔身上能找到，而树皮需要你睁大眼睛找找……</t><b/>" @
											"<t>如果能收集齐全，至少能查出些许原因。</t><b/>";
		case 10005:	return "<t>自然之神保佑，你安全归来了。我看你走后就很不安，总觉得落木谷太过危险……</t><b/>" @
											"<t>谢谢你，这些东西让我好好看看。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10714Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着毒瘤去树林幽径找夕娥</t><b/>";
		case 10002:	return "<t>带着这个毒瘤去树林幽径，那里有飞花谷的小营地，也许药师夕娥能解释这一切。</t><b/>" @
											"<t>路上小心，这座森林危机重重，我很担心你会出事。</t><b/>" @
											"<t>【对你念诵精族的祝福语言】</t><b/>";
		case 10005:	return "<t>【夕娥温和的看着你】</t><b/>" @
											"<t>也许你不该踏入此地，遮月森林如今与任何人为敌，孤身涉境太过危险了。</t><b/>" @
											"<t>呵呵，你随便坐坐吧，我需要点时间看看你给我的东西。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10715Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>夕娥让你到树王岗杀掉一些芽孢树精</t><b/>";
		case 10002:	return "<t>这种毒瘤……似乎出自一种特殊的树精身上。</t><b/>" @
											"<t>那种树被称为芽孢树，树干生长的袍子含有毒性，不过是很好的药材。如今这些芽孢树显然成精了，将毒性传播到其他生物身上，使其中毒邪化。</t><b/>" @
											"<t>树王岗附近有大量的芽孢树，如果你看到芽孢树精，请亲手摧毁它。</t><b/>";
		case 10005:	return "<t>用自己的能力使周遭的生物中毒，树精已经发疯了，它们必定在某种力量的控制下才变得如此。</t><b/>" @
											"<t>我们熟悉的遮月森林现在已经变得那么陌生，陌生得让人害怕……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10716Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>采集几份雨滴水晶给夕娥</t><b/>";
		case 10002:	return "<t>你除掉的树精仅仅是傀儡，主使者尚未现身。</t><b/>" @
											"<t>不过他太强大了，我们需要点计谋。呵呵，有时候奸诈点也非坏事。</t><b/>" @
											"<t>你应该看到这附近有许多水晶，它们有丰沛的灵力，能助我们击溃敌人。现在就去采点水晶来吧！小心点，水晶附近有不少五灵族的勘矿者，他们很疯狂……</t><b/>";
		case 10005:	return "<t>呵呵，大自然真是伟大，这些明晃晃的小石头竟然蕴含了如此丰厚的灵力。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10717Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>用水晶石虚弱芽孢树精王后杀死它，带回树精王之根</t><b/>";
		case 10002:	return "<t>现在是最关键的时刻了，我们要好好利用水晶的力量。</t><b/>" @
											"<t>芽孢树精王拥有恐怖的力量，你要用水晶的灵力暂时削弱他的力量，否则一定是你死。</t><b/>" @
											"<t>记住，用水晶先削弱他，然后再击败他！我要看到它的树根。</t><b/>" @
											"<t>【夕娥对你诵念祝福的精族语言】</t><b/>";
		case 10005:	return "<t>什么？你说树精王死的时候说它感谢你？他得到了解脱？</t><b/>" @
											"<t>这么说他也是被控制的，在幕后还有主使者！！！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10718Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>保护夕娥施法完成，听听她和芽孢树精王灵魂的对话</t><b/>";
		case 10002:	return "<t>我简直不敢相信！利用你带回的树根，我能召唤出树精王死去不久的灵魂，我要亲自和它谈谈。</t><b/>" @
											"<t>使用这个仙术会运用周围植物的灵力，一些存有邪念的植物会出现攻击我，请你保护我，一直到我的法术施放完成。</t><b/>";
		case 10005:	return "<t>【夕娥大口大口的喘气】</t><b/>" @
											"<t>怎会是嫦娥的力量？这不可能……不可能……</t><b/>" @
											"<t>嫦娥曾经在此奔月成神，这是她最爱的地方，养育着她最爱的子民，她绝不会做出这种邪恶之事。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■第二章■■■■■■■■■■■■■■■■

function Mission10740Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去林边树居找精族长老紫昙秋</t><b/>";
		case 10002:	return "<t>关于嫦娥的传说，你要去找紫昙秋，她是精族的长老，对嫦娥之事知道得更多。</t><b/>" @
											"<t>把你知道的所有都告诉她，她会告诉你下一步该怎么做。</t><b/>" @
											"<t>夕娥感谢你为精族做的一切。</t><b/>";
		case 10005:	return "<t>嫦娥在邪化遮月森林？这绝不可能！呵呵，我太了解她了，这种事嫦娥做不出来。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10741Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀死月菇娃娃后再找紫昙秋谈谈</t><b/>";
		case 10002:	return "<t>嫦娥是飞花谷的上代谷主，她曾是一朵玉簪花，后来与神将后羿相恋。</t><b/>" @
											"<t>好了，你来这里并不是听故事，你还有许多事情要做。树居被大量的月菇娃娃所侵扰，我很讨厌它们吵吵闹闹的，还破坏我的家园。</t><b/>" @
											"<t>它们需要死亡，死亡才能获得新生。</t><b/>";
		case 10005:	return "<t>呵呵，我觉得你杀得还不够多，不过我总算能过几天清净日子。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10742Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小伙子";
	else
		%Sex = "小姑娘";

	switch(%Tid)
	{
		case 10001:	return "<t>杀一些成年树精，拿他们的根须去见紫昙秋</t><b/>";
		case 10002:	return "<t>你不会没发现那些大个子吧？就是成年树精，他们数量不多，但所过之处总是一片狼藉。</t><b/>" @
											"<t>这些成年树精十分棘手，清除他们后我才能安静地给你讲故事呢。对了，别空手回来，我要看到树精的根须。</t><b/>" @
											"<t>呵呵，去吧，" @ %Sex @ "！</t><b/>";
		case 10005:	return "<t>呵呵，你果然身怀绝技，那些大个子都不是好对付的。</t><b/>" @
											"<t>看来你有资格了解一些过去的事情了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10743Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>听紫昙秋讲述嫦娥成神的往事</t><b/>";
		case 10002:	return "<t>那些事情过去数百年了，让我好好回忆下。</t><b/>" @
											"<t>好了，你准备好听那些古老的传说了吗？仔细听听，也许对你有不小的帮助。</t><b/>";
		case 10005:	return "<t>嫦娥在成神前为飞花谷谷主，性格温驯善良，她是决计不会做出邪化森林之事的。</t><b/>";
			
		case 200: return   $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我准备好了</t>";
			
		case 300: return  "<t>想想也有五百多年了……那时，玉簪花还未成为嫦娥，她时常在森林修炼，直到她遇到了后羿，一个勇猛无畏的传奇男子。</t><b/>"@
			                "<t>后羿为助玉簪花修炼，经历千辛万苦，为她找到了不老药。服下不老药，即可飞升成神，可笑的是，不老药只有一份。</t><b/>"@
			                "<t>这份不老药就一直无人服用，直到后羿射日，被封为神将的那晚。修为深厚的玉簪花终于服下了不老药，奔月而去，飞升成神，名嫦娥。</t><b/>"@
			                "<t>从找到不老药到服下，中途就经历了百多年，两人最后也算圆满之局。</t><b/>"; 
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10744Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去赝月花丛找到月柔</t><b/>";
		case 10002:	return "<t>我只能告诉你这么多，但有一个人可以让你更清楚。</t><b/>" @
											"<t>月柔是嫦娥曾经的侍女，她最近也来调查遮月森林的异事，三天前她去了赝月花丛，不知发生何事，如今尚未归来。</t><b/>" @
											"<t>你应该去看看，我很担心那姑娘已经出事了。</t><b/>";
		case 10005:	return "<t>【脸色发白，嘴唇发紫，看起来十分难受】</t><b/>" @
											"<t>这里很危险，赝月花精散发的花香化为了一种毒雾，会使人灵力涣散，虚弱无力。</t><b/>" @
											"<t>咳咳……我藏在这里躲过了赝月的查探，但是我根本无力离开这里。</t><b/>" @
											"<t>小心不要吸入毒雾……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10745Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从赝月身上取得赝月花瓣，从月菇灵童身上取到月菇伞，再拾取一些夜来香种子，交给月柔</t><b/>";
		case 10002:	return "<t>咳咳……赝月也变得邪恶了，我……我不能一直呆在这里……请你帮我找到解药。</t><b/>" @
											"<t>我需要赝月的花瓣，月菇灵童的菇伞，还有地上散落的夜来香种子。</t><b/>" @
											"<t>你刚来此地不久，暂不会受毒雾的影响，但速度一定要快，我们……时间不多。</t><b/>";
		case 10005:	return "<t>谢谢你，有了这些东西，我就能将之培育成解药。</t><b/>" @
											"<t>十分感谢你。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10746Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>护送月柔走出赝月花丛，之后去找紫昙秋</t><b/>";
		case 10002:	return "<t>【" @ %Player.GetPlayerName() @ "】，我依然十分虚弱，然而我们必须赶快离开，再多呆一会恐怕会被发现。</t><b/>" @
											"<t>请护送我回到林边树居，只要出了赝月花丛，我就能自己回去。紫昙秋要是看到我平安回去一定会开心。</t><b/>";
		case 10005:	return "<t>再次感激你，朋友。月柔已经回来了，我让她先回家调养，她让我转达她的谢意。</t><b/>" @
											"<t>月柔将所知的一切都告诉了我，只能说，真相十分惊人！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10747Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去找常丹子打探关于不老药的事情</t><b/>";
		case 10002:	return "<t>嫦娥当时有两个侍女，一个是月柔，另一个叫碧荷。</t><b/>" @
											"<t>在嫦娥成神之后，她们发现，嫦娥并没有服下不老药！那么嫦娥是单凭自己能力成神，可是不老药在哪呢？</t><b/>" @
											"<t>月柔和碧荷找寻了很久，一无所获，直到两人分道扬镳。月柔这次回来一是为了查询森林变异的原因，二来她也想寻到不老药的踪迹。</t><b/>" @
											"<t>你要去找常丹子，他是仙界炼丹的宗师，对不老药也许有所闻。</t><b/>";
		case 10005:	return "<t>不老药出自天界瑶池，服下之人可长生不老，修为大增。但一夜成神的说法，似乎并不可靠，修神不仅仅需灵力深厚，更重要的是心境。</t><b/>" @
											"<t>这世上不会存有一夜成神，除非神族亲自赋予你神力。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10748Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀一些醉月后，去找常丹子</t><b/>";
		case 10002:	return "<t>你难道不觉此地气氛诡异，乾坤倒转？一股邪气冲天而起，令人堪忧。</t><b/>" @
											"<t>我十分想知道究竟是何等妖邪能感染整片森林，请你去一趟赝月花丛，除掉那些醉月，这些妖魅的花精不除，始终是个威胁。</t><b/>";
		case 10005:	return "<t>这是一种夜来香花精，看来她们的修为都不差，怎会如此甘愿堕落呢？邪恶的力量究竟来自何处？</t><b/>" @
											"<t>我不认为这是嫦娥所为，真相已经很近，只差一步。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10749Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在赝月花丛杀足够多的花精，直到引出夜歌，之后杀了夜歌，把她的花蕊带给常丹子</t><b/>";
		case 10002:	return "<t>夜歌是这些花精的花母，如果我们能找到她邪化的原因，就能得知赝月为何堕落。</t><b/>" @
											"<t>只要你在赝月花丛杀了足够多的花精，夜歌就会出现。接下来很简单，杀了她！带回她的花蕊。勿用迟疑，除邪乃正道本份！</t><b/>" @
											"<t>【常丹子正色看着你】</t><b/>";
		case 10005:	return "<t>事关重大，这绝非普通妖邪，我……</t><b/>" @
											"<t>【常丹子伤势十分严重，紧紧捂住心口】</t><b/>" @
											"<t>五百年前的古老力量，五百年前嫦娥正在森林成神，其中必有……必有蹊跷……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10750Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到落木谷后方去寻找五灵族人金宝</t><b/>";
		case 10002:	return "<t>朋友，关于五百年前的事情，谁也不清楚，但有一个种族肯定明白。我想你听说过五灵族，一个专门寻找古代的遗迹和笼络钱财的种族。</t><b/>" @
											"<t>【常丹子又激烈的咳嗽起来】</t><b/>" @
											"<t>咳咳……在落木谷后方，有个五灵族人金宝，你赶紧去找他。利用他的知识，也许能破解五百年间发生的事情。</t><b/>" @
											"<t>不用管我，这点伤势还不至于让我常丹子毙命。</t><b/>";
		case 10005:	return "<t>你是抢匪吗？我没钱！我没钱！</t><b/>" @
											"<t>【金宝手舞足蹈，嘶声大吼，你好容易才告诉他缘由】</t><b/>" @
											"<t>呃……原来你有事相求，吓死我了，这个鬼森林真是到处都不安全！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10751Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>帮金宝找回掉落在附近的火药桶</t><b/>";
		case 10002:	return "<t>区区五百年而已，金宝动动指头就知道发生过什么，啊哈哈！</t><b/>" @
											"<t>不过向人打听是要付钱的，这是礼貌！</t><b/>" @
											"<t>唉，看你长得就没有财气。这样吧，你帮我做事，我告诉你想知道的料。</t><b/>" @
											"<t>首先，我的火药桶散落在附近了，帮我找回来，那都是些很棒的货！</t><b/>";
		case 10005:	return "<t>哇哦，闻到这些火药味让我浑身兴奋。</t><b/>" @
											"<t>你肯定不知道这些东西有多强力，你以为只能做爆竹？那你真的太低端了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10752Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小孩";
	else
		%Sex = "女孩";

	switch(%Tid)
	{
		case 10001:	return "<t>使用金宝给你的大爆竹去攻击月菇童子</t><b/>";
		case 10002:	return "<t>直接讲重点，前方坠星谷有着无匹的财富，嗯……很多竹子，很多水晶矿……</t><b/>" @
											"<t>【金宝两眼放光】</t><b/>" @
											"<t>但是有许多混帐蘑菇人，他们愚蠢又烦人！总是偷袭我们！</t><b/>" @
											"<t>来，拿着这些我制作的强力爆竹，现在就去把他们都……炸！上！天！</t><b/>";
		case 10005:	return "<t>哈哈，" @ %Sex @ "，是不是觉得很过瘾？</t><b/>" @
											"<t>金宝制造的大爆竹向来威猛无比，神仙都要惧我三分！哇哈哈！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10753Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>把黑火药带给雷斧</t><b/>";
		case 10002:	return "<t>有趣的事情才刚开始，朋友！</t><b/>" @
											"<t>雷斧肯定不知道我的火药有多美，帮我带点火药给他，他擅长的是另一手。</t><b/>";
		case 10005:	return "<t>我等这种黑火药等了一辈子！呃……至少等了好几天……</t><b/>" @
											"<t>好了，发挥雷斧作用的时刻到来了，为了五灵族，为了竹子！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10754Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>向剥皮树精使用火箭，之后杀了他们</t><b/>";
		case 10002:	return "<t>坠星谷有许多竹子和水晶矿，你看，那里都是我们采集后留下的残迹，多美！</t><b/>" @
											"<t>我们被那些蘑菇人和树精骚扰，呃……蘑菇人据说已经被炸飞了，但讨厌的树精还在。</t><b/>" @
											"<t>知道为什么金宝不让你去炸树精吗？因为树精太重了，炸不飞。</t><b/>" @
											"<t>不过我们可以烧他们，哈哈！带着这些火箭，向他们发射！嗖嗖嗖，烧成炭！</t><b/>";
		case 10005:	return "<t>一片火海，很厉害吧？</t><b/>" @
											"<t>哼哼，雷斧不发威，当我是病熊猫！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10755Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>将北面石板，南面石板搬回来见雷斧</t><b/>";
		case 10002:	return "<t>金宝跟我说，你在寻找五百年前的故事，那实在是太简单了。</t><b/>" @
											"<t>坠星谷中有几块石板，刻了许多歪扭的文字，呃……那兴许是一种语言，但是难不倒五灵族。</t><b/>" @
											"<t>给我把石板带来，小心点，一旦摔坏了，那什么都玩完！</t><b/>";
		case 10005:	return "<t>这石板真硬，炸碎的声音一定很好听！</t><b/>" @
											"<t>我只是说说，我不会炸的。</t><b/>" @
											"<t>【又盯着石板，喃喃自语】</t><b/>" @
											"<t>好想炸……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10756Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小子";
	else
		%Sex = "丫头";

	switch(%Tid)
	{
		case 10001:	return "<t>从蟾宫兔身上取到遮月兔油</t><b/>";
		case 10002:	return "<t>" @ %Sex @ "！这石板脏极了，灰尘把字都填满了，我怎么看得清楚！</t><b/>" @
											"<t>东边的蟾宫兔身上有不错的油，擦擦就能跟新的一样。</t><b/>" @
											"<t>还看什么看，快去啊，五灵族最讨厌浪费时间的人啦！</t><b/>";
		case 10005:	return "<t>来来来，让我擦擦，擦擦就清楚了。</t><b/>" @
											"<t>翻译这些文字很伤脑筋的，看在你帮我们做了不少事，就不让你付钱了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10757Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着翻译后的纸张去奔月台找桑雪琴</t><b/>";
		case 10002:	return "<t>好了！真累人，这些石碑的确讲述了五百年前的琐事。</t><b/>" @
											"<t>拿着，这是我译后的篇章，你有必要好好看看。嗯……我想你要进行一次长跑，在那里你一定能得知真相。</t><b/>" @
											"<t>那地方叫奔月台，一定有个叫桑雪琴的精族在等你，嘿嘿，石碑上都写着呢！</t><b/>";
		case 10005:	return "<t>这上面写的是古碑的文字？那些文字我都不认得，五灵族居然能译出，这……真是让人难以置信。</t><b/>" @
											"<t>让我看看吧，那段历史太过神秘了。</t><b/>" @
											"<t>【桑雪琴越看脸色越白，最后大呼一声！】</t><b/>" @
											"<t>嫦娥说自己并未服下不老药，是突然成神！这又是何故？神族之密，我等凡夫俗子猜测不透。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10758Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到寒梦居找方瑶儿谈谈</t><b/>";
		case 10002:	return "<t>看来不老药就掉在奔月台上，只要有后羿的力量，我们就能发现它。</t><b/>" @
											"<t>在此之前，我们要清除此地的所有邪恶，你到寒梦居找方瑶，她会帮助你。</t><b/>";
		case 10005:	return "<t>此地有许多弓箭怪，它们都与后羿射日弓有脱不开的干系。</t><b/>" @
											"<t>如今，这些曾经神圣的怪族也变得邪恶了……方瑶很担忧，也很害怕……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10759Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉附近的羿弓残影</t><b/>";
		case 10002:	return "<t>我听说，羿弓残影是后羿之弓的影子，身上并未后羿的力量。</t><b/>" @
											"<t>它们带着狂暴的力量游走在此地，若是不老药被它们发现，少不了一场恶战。</t><b/>" @
											"<t>方瑶请你杀了盘踞在此的羿弓残影，一个不留！</t><b/>";
		case 10005:	return "<t>回来了吗？想着不老药会重现天日，真令人激动……</t><b/>" @
											"<t>我和雪琴姊姊会把不老药带回飞花谷，此乃神物，若被奸恶小人得到，苍生必是一场浩劫。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10760Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>从羿弓残息处取得后羿之力，再去找桑雪琴</t><b/>";
		case 10002:	return "<t>现在我们要取得后羿的力量，让不老药重现！</t><b/>" @
											"<t>在奔月台附近有三把羿弓残息，乃后羿射日弓的神力聚集而成，它们沾染了后羿之力。</t><b/>" @
											"<t>从它们身上取得后羿之力，然后交给雪琴姊姊，她会让你看到五百年来不为人知的一切。</t><b/>";
		case 10005:	return "<t>你带来的是这片森林的财富，后羿之力，神将后羿本人的力量！</t><b/>" @
											"<t>这种雄壮无匹的力量一定能让不老药重现于天下，【" @ %Player.GetPlayerName() @ "】，对你的感激之情，雪琴实在不知该如何表达。</t><b/>" @
											"<t>谢谢……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10761Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>陪桑雪琴启动后羿的力量，然后面临即将到来的一切</t><b/>";
		case 10002:	return "<t>请你与我共同见证这史诗般的一刻，五百年了，不老药即将重现。</t><b/>" @
											"<t>【桑雪琴的声音在微微发颤】</t><b/>" @
											"<t>你准备好了吗？我将启动后羿之力。</t><b/>";
		case 10005:	return "<t>碧荷……</t><b/>" @
											"<t>怎……怎么会这样！居然是她盗走了不老药，她是如何知道不老药在哪里的？</t><b/>" @
											"<t>也许她是嫦娥宠爱的侍女，也许她就得知了一些秘密……</t><b/>" @
											"<t>不……不……</t><b/>" @
											"<t>【桑雪琴神思恍惚】</t><b/>";
											
	  case 200: return   $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我准备好了</t>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

//■■■■■■■■■■■第二章■■■■■■■■■■■■■■■■

//■■■■■■■■■■■■■■■第三章■■■■■■■■■■■■

function Mission10780Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>把信件送给不老洞外的灵珊</t><b/>";
		case 10002:	return "<t>碧荷即将把整片森林邪化，那时这里将成为……地狱。</t><b/>" @
											"<t>当务之急是要净化森林，再去长生殿找她算帐！</t><b/>" @
											"<t>朋友，你即刻前往森林深处，把这封书信交给灵珊。我必须马上赶回飞花谷，告之一切。</t><b/>" @
											"<t>我会传你前往不老洞外，灵珊就在那里。</t><b/>";
		case 10005:	return "<t>【看过信后，灵珊发出一声惊呼】</t><b/>" @
											"<t>事态已如此紧张了？真没想到会是碧荷！</t><b/>" @
											"<t>我们没时间了，要净化森林！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10781Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉附近的鬼魅花精和凶煞弓影后去找梅幽</t><b/>";
		case 10002:	return "<t>这里是遮月森林北部，我身后方的不老洞就连接了长生殿的侧门。此地之险绝非森林外围可比！</t><b/>" @
											"<t>当务之急是尽可能多地消灭碧荷的爪牙，我还要侦查附近的局势，杀掉周围的鬼魅花精和凶煞弓影后去找梅幽，抓紧时间！</t><b/>";
		case 10005:	return "<t>灵珊姊姊已经告诉我了，把你卷入此事真是抱歉。</t><b/>" @
											"<t>我们要尽全力拯救森林，你明白此刻的处境，谁都不安全……</t><b/>" @
											"<t>【梅幽喃喃自语】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10782Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>梅幽让你去找灵珊，她会告诉你接下来要做的事情</t><b/>";
		case 10002:	return "<t>你为我们争取了宝贵的时间！</t><b/>" @
											"<t>如今的一分一秒比任何时候都急迫，森林随时会变成地狱！</t><b/>" @
											"<t>你该去找灵珊姊姊了，她应该有了详细的计划。</t><b/>";
		case 10005:	return "<t>你击杀花精和弓影拖延了森林邪化的速度，我们比任何时候都需要时间。</t><b/>" @
											"<t>大自然会感谢你的仗义相助。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10783Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀了暗香后向灵珊回报</t><b/>";
		case 10002:	return "<t>在你击杀花精和弓影时，我查到了他们的首领。那是一个叫做暗香的花精，她是碧荷在此地的使者。</t><b/>" @
											"<t>暗香曾经是高贵圣洁的花精，如今已彻底邪化，惟有……死亡才能解脱。</t><b/>" @
											"<t>【" @ %Player.GetPlayerName() @ "】，让我可怜的姊妹从痛苦中脱离吧。</t><b/>" @
											"<t>杀……了……她……</t><b/>";
		case 10005:	return "<t>她……已经死了吗？</t><b/>" @
											"<t>【灵珊悠悠叹了一口气】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10784Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>速速前往东方，去找森林的护法柳儿</t><b/>";
		case 10002:	return "<t>森林邪化的速度越来越快！越来越严重！</t><b/>" @
											"<t>我们要寻找净化邪气的灵器，至少要解救森林中那些强大的生命，有了他们的帮助，我们才有希望拯救森林。</t><b/>" @
											"<t>你要前往东方，去找森林的护法柳儿。恳求自然福泽，柳儿还未遭到毒手。</t><b/>";
		case 10005:	return "<t>感谢自然之光，终于有人来了！</t><b/>" @
											"<t>森林如今在苦苦挣扎，随时会失去理智，成为魔鬼！</t><b/>" @
											"<t>我们快没时间了！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10785Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到舞风顶杀了那些结界之魂，再找阵卫谈谈</t><b/>";
		case 10002:	return "<t>是时候了，我们要开启精族历代守护的圣物。</t><b/>" @
											"<t>在舞风顶，你会看到一个巨石法阵，周围有许多结界之魂，杀了那些结界之魂，再找阵卫谈谈。</t><b/>";
		case 10005:	return "<t>你想开启巨石阵，这个……</t><b/>" @
											"<t>森林已经走到这一步了吗……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10786Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀了肃杀，夺取它的暴躁之灵</t><b/>";
		case 10002:	return "<t>巨石法阵镇守了精族历代守护的灵器之一，要开启巨石阵，同样需要强大的灵力。</t><b/>" @
											"<t>这里有一个弓影怪，它具备强大的灵力，它的力量正好可作为种子，开启巨石阵。</t><b/>";
		case 10005:	return "<t>唉，巨石阵已经沉睡了近千年，如今又要使用它了……</t><b/>" @
											"<t>【阵卫深深叹了一口气】</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10787Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>启动巨石阵，击败圣灵，拿到精族灵器</t><b/>";
		case 10002:	return "<t>我已经开启了巨石法阵，在巨石阵周围有三块巨石，依次挪动它们，会召唤出圣灵，灵器便在圣灵身上。</t><b/>" @
											"<t>小心点，【" @ %Player.GetPlayerName() @ "】,圣灵存在的意义即是守护灵器，你要击败他才能得到灵器。</t><b/>";
		case 10005:	return "<t>精族灵器的力量十分庞大，你一定要善用。切记！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10788Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着精族灵器回到柳儿身边</t><b/>";
		case 10002:	return "<t>我能感觉森林邪化的速度已如洪流，带好灵器，尽快回到柳儿身边，她会引导你使用这件圣物的力量。</t><b/>";
		case 10005:	return "<t>你终于回来了，局势已万分急迫！</t><b/>" @
											"<t>还好我们拥有灵器，我们现在能与碧荷的邪化大军抗衡！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10789Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到云霭之森，从邪化树精身上取到足够多的根须能量，注入精族灵器内，再对树精之王古槐使用</t><b/>";
		case 10002:	return "<t>在全面对抗之前，我们要寻找强大的盟友。</t><b/>" @
											"<t>在云霭之森，有遮月森林最伟大的树精之王——古槐。我担忧他已受到邪化的影响，我们一定要解救他，并得到他的帮助。</t><b/>" @
											"<t>带着精族灵器到那里去，从邪化树精身上取得足够多的根须能量，然后注入灵器之内，便可发挥灵器神威。</t><b/>" @
											"<t>将充能的精族灵器向古槐使用，驱散他体内的邪恶。</t><b/>"@
											$Get_Dialog_GeShi[31206] @ "【小帮助：收集到足够数量的根须能量后，打开背包，对根须能量点击右键，再点击精族灵器，便可向精族灵器充能啦^_^】</t><b/>";
		case 10005:	return "<t>太好了，能得到树精之王的帮助，就有希望阻止碧荷的恶行。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10790Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到婆娑之海，从邪化花精身上取到足够多的花瓣能量，注入精族灵器内，再对望月花母使用</t><b/>";
		case 10002:	return "<t>我们无法停止了，还有一位森林之王需要我们解救。</t><b/>" @
											"<t>在东边的婆娑之海，望月花母正苦苦挣扎，用同样的方式，从邪化花精身上取到花瓣能量，注入灵器，驱除花母的邪恶。</t><b/>";
		case 10005:	return "<t>花母也得到了拯救，【" @ %Player.GetPlayerName() @ "】，一切还来得及，你将创造奇迹！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10791Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>与柳儿交谈，准备与碧荷的邪化军团作战</t><b/>";
		case 10002:	return "<t>碧荷已经邪化了整片森林，她将大军集结在长生殿门口，他们要摧毁一切，然而我们要摧毁他们！</t><b/>" @
											"<t>【柳儿深深吸了一口气】</t><b/>" @
											"<t>树精之王和花母都会伸以援手，他们此刻正赶往长生殿，我们速速与他们汇合。</t><b/>";
		case 10005:	return "<t>天啊！浩浩荡荡的邪化军团让我几乎喘不过气，碧荷的邪力竟已达到如此境地！</t><b/>" @
											"<t>这将是一场硬战！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10792Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>与古槐交谈，与他并肩作战，并净化云霭古树</t><b/>";
		case 10002:	return "<t>在云霭之森和婆娑之海各有一棵古树，是遮月森林的灵力之源。只要净化了两棵古树，纯净的灵力便能洗清森林的所有邪恶。</t><b/>" @
											"<t>古槐和花母会帮助我们，只有森林之王，才有净化古树的资格。</t><b/>" @
											$Get_Dialog_GeShi[31206] @ "【记住，古槐具有净化云霭古树的能力，而花母则会净化婆娑古树，千万不要犯错！】</t><b/>" @
											"<t>先去找古槐，他会带你打一场漂亮仗！</t><b/>";
		case 10005:	return "<t>要抓紧了，云霭古树已经得到了净化，碧荷的邪化大军攻势也愈加凌厉了。</t><b/>" @
											"<t>好好休整，准备进化下一株古树。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10793Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>与望月花母交谈，与她并肩作战，并净化婆娑古树</t><b/>";
		case 10002:	return "<t>婆娑之海在东边，你需要望月花母的帮助。</t><b/>" @
											"<t>去找花母，她会与你并肩作战！</t><b/>";
		case 10005:	return "<t>【柳儿长长吁出一口气】</t><b/>" @
											"<t>两株古树终于得到了净化，遮月森林的邪恶会慢慢洗涤。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10794Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>分别与古槐、望月花母谈谈</t><b/>";
		case 10002:	return "<t>你看，邪化大军在节节败退，大自然发挥了它纯洁的力量，呵呵。</t><b/>" @
											"<t>感谢你，朋友，没有你的帮助，遮月森林不知道会变成什么样子。</t><b/>" @
											"<t>我还要打扫战场，请你代我向古槐和花母说声谢谢，是他们的帮助，遮月森林才恢复了宁静。</t><b/>";
		case 10005:	return "<t>【柳儿对着你微笑】</t><b/>" @
											"<t>呵呵，我能听到森林的感激，那些花草让我对你说，谢谢。</t><b/>" @
											"<t>我们回去吧，虽然碧荷还未遭到她应得的惩罚，好歹森林的危机解除了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10795Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着柳儿的信件回月幽境找夜葵</t><b/>";
		case 10002:	return "<t>带着这封信，交给月幽境的城主夜葵。</t><b/>" @
											"<t>她会指引我们迈向终点，面对碧荷的决战！</t><b/>" @
											"<t>【柳儿对你诵念祝福的精族语言】</t><b/>";
		case 10005:	return "<t>遮月森林得到了净化，碧荷一定遭受了不小的打击。</t><b/>" @
											"<t>以碧荷之狂妄，必会恼羞成怒。月幽境的精族要联合一起，直捣长生殿！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

//■■■■■■■■■■■■■■■第三章■■■■■■■■■■■■
