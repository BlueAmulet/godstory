//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端任务对话，【章回任务】第二卷
//==================================================================================

//【章回任务】
//第一编号为【卷】，每一张地图为一卷，每一卷的任务编号间隔数为200，第二编号段为【章】，每一卷暂定为三章，每一章的任务编号间隔数为40，附章间隔为20，最后多余20个编号。
//目前编号段规划，最大可至24卷，24*3=72章，即可以满足72张地图所需的章回任务，视后期任务需求，可挪用一部分的支线任务编号段

//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■第二章■■■■■■■■■■■■■■■■
//■■■■■■■■■■■■■■■第三章■■■■■■■■■■■■


//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■
function Mission10300Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到仙游岭找到仙人御清</t><b/>";
		case 10002:	return "<t>小娃娃你帮我了大忙，老夫也该离开了。我看你资质不凡，是个修炼的料，你把这封书信带给仙游岭的御清，他会提点提点你的。</t><b/>";
		case 10005:	return "<t>嗯？璇光真神介绍你来的，看来你有过人之处啊。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10301Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在青草坪的蝮蛇身上取回几份蝮蛇肉，再交给御清</t><b/>";
		case 10002:	return "<t>年轻人，勿要着急，在历练开始前，我们得先吃饭！</t><b/>" @
											"<t>知道天下最好的美食吗？嘿嘿，那就是蛇肉！在青草坪藏匿了许多蝮蛇，去取点蝮蛇肉来做食材。</t><b/>";
		case 10005:	return "<t>哈哈，我仿佛已经闻到美味蛇肉的味道了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10302Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在青草坪的乌蛇身上取回几份乌蛇肉，再交给御清</t><b/>";
		case 10002:	return "<t>别急别急，光这样一种食材是不够的，你得再找点别的。</t><b/>" @
											"<t>我们要做一锅鲜美的蛇汤，蛇膳之中以乌蛇为最上乘，这附近可有不少的乌蛇啊，我们来煮一锅双龙戏珠，你现在去找点乌蛇肉来。</t><b/>";
		case 10005:	return "<t>好好好！多鲜嫩的肉啊！美食在向我们招手呢！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10303Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>找到青草坪和求雨台间的蛇洞，其中有一个应该是泽蛇呆过的，挨个调查调查，有线索后去找费森森</t><b/>";
		case 10002:	return "<t>现在就剩下最后一种食材了，乃双龙戏珠的【珠】，也就是蛇胆。</t><b/>" @
											 "<t>青草坪附近一直传说有一条叫泽蛇的巨蛇，不知是真是假，如果你有本事，应该能调查清楚。</t><b/>" @
											 "<t>此地有许多蛇洞，你去搜索搜索，如果有什么讯息，就去找费森森。他是这一带出了名的猎人，要问泽蛇的消息，找他最合适不过。</t><b/>";
		case 10005:	return "<t>你居然在找泽蛇，我没听错吧？我看你是活腻了！</t><b/>" @
											 "<t>什么，你还找到了蛇蜕！快……快给我看看。</t><b/>" @
											 "<t>【仔细看了半天】</t><b/>" @
											 "<t>呃……这蛇蜕上的鳞片粗犷庞大，腥味浓烈，肯定是泽蛇不错！费森森如果能抓到泽蛇，爹爹和哥哥一定会奖励我娶村里最胖的姑娘，我喜欢胖姑娘，呵呵呵。</t><b/>";
											
		case 200: return "<t>此洞小而平滑，应当不是泽蛇留下的。</t><b/>";
		case 201: return "<t>这个……只比平常蛇洞大了一点点，应当也不是泽蛇洞……</t><b/>";
		case 202: return "<t>洞如碗大小，腥臭扑鼻，还有一截巨大的白色蛇蜕，应是泽蛇所留。</t><b/>";
												
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10304Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到求雨台附近杀掉泽蛇，取回它的蛇胆，向御清报道</t><b/>";
		case 10002:	return "<t>费震震告诉你，泽蛇很聪明，你搜索了它的洞穴，它现在正等待你，它肯定比普通蝮蛇更大、更壮、更凶悍！</t><b/>" @
											"<t>在求雨台的附近，泽蛇就在那处游荡。杀了它，取到蛇胆后就去找御清吧。小心点，搞不好它才是猎人，而你是猎物。</t><b/>";
		case 10005:	return "<t>哈哈，这么大的蛇胆，我还是初次见到。与它的战斗想来必不轻松吧，不过好歹你成功了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10305Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>按照御清的吩咐烹制出双龙戏珠，然后交给御清</t><b/>";
		case 10002:	return "<t>现在我们来烹制美味蛇汤，双龙戏珠的材料都备齐了。将蛇肉、泽蛇胆和香料倒入我身边的大锅，之后再将炉火点燃，静待片刻就成了。快去快去，我等着大快朵颐呢！</t><b/>";
		case 10005:	return "<t>啧啧啧，就是这个味道，真是美味无穷！这锅鲜汤你功劳甚大，来来，也喝上一口！</t><b/>";
			
		case 200: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "放置蛇肉</t>";
		case 201: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "放置泽蛇胆</t>";
		case 202:	return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "放置香料</t>";
		case 203: return "<t>食材都放好了，现在准备点火了吗？</t><b/>";
		case 204: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "点火</t>";
		case 210: return "<t>已放置好蛇肉.</t><b/>";
		case 220: return "<t>已放置好泽蛇胆.</t><b/>";	
		case 230: return "<t>已放置好香料.</t><b/>";	
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10306Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去找费森森谈谈，关于新的修炼</t><b/>";
		case 10002:	return "<t>嗯，你现在可以走了……什么？修炼？方才不是修炼了吗？你那么勇猛的杀了泽蛇，泽蛇可不是普通人能伤害的啊！</t><b/>" @
											"<t>咳，说到修炼啊……你不觉得猎人费森森很有一身本事吗？去找他谈谈，他肯定能给你安排一场不错的修炼。</t><b/>";
		case 10005:	return "<t>你又来找费震震了，哈哈哈，费森森是很强悍的，是一个捕猎大王！当然，费森森的爹爹和哥哥也是超强的！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10307Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去捕杀几只火云狐后向费森森呢证明自己</t><b/>";
		case 10002:	return "<t>费森森是一个超强的猎人，费森森的哥哥也是，爹爹更是强的可怕，我和哥哥私下都称他为猛男！</t><b/>" @
											"<t>你也想当一名猎人吗？狩猎可不是那么简单的事，你需要向我证明你有本事。看，这片绿野有许多游荡的火云狐，你去杀了它们再来找我。</t><b/>";
		case 10005:	return "<t>你很不错，不过这还不算狩猎，在你猎过费森森亲自猎过的东西之前，还不能算是一名真正的猎人。我会给你更难的考题，呵呵呵。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10308Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉几只赤焰狐后去找费森森</t><b/>";
		case 10002:	return "<t>你已向我证明你能杀掉普通的火云狐，但危险的是赤焰狐。</t><b/>" @
											"<t>赤焰狐浑体如火，我感觉它们像是一种妖兽。一个好猎人是不会畏惧这种生物的，费森森就从不怕，爹爹和哥哥也不怕。</t><b/>" @
											"<t>去杀掉它们，再回来谈谈你的狩猎技术。</t><b/>";
		case 10005:	return "<t>你越来越像一个好猎人了，费森森很看好你！真想把你介绍给爹爹和哥哥认识。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10309Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀了血尾，带回它的尾巴给费森森</t><b/>";
		case 10002:	return "<t>血尾是赤焰狐中最凶狠狡猾的一个，也是它们的首领。它可是出了名的凶残。</t><b/>" @
											"<t>我观察它一段时间了，它就在赤焰狐群中徘徊，你想要证明你是一名优秀的猎人吗？那就将血尾的尾巴带回来。</t><b/>" @
											"<t>费森森提醒你，血尾很恐怖，身后还跟了一群赤焰狐，杀它的时候用点力。</t><b/>";
		case 10005:	return "<t>哈哈，这条尾巴……多么迷人啊！</t><b/>" @
											"<t>费森森很喜欢你，你以后肯定还能遇见爹爹和哥哥，他们在更远的猎场捕猎，他们也会喜欢你的。</t><b/>" @
											"<t>来，这是你该得的报酬。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10310Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>前往水墨古镇，与镇上的张黎谈谈</t><b/>";
		case 10002:	return "<t>费森森要休息一会，你得离开了。前方不远有个建在湖上的古镇，那里好像有什么传说。</t><b/>" @
											"<t>村里的猎户张黎是我朋友，你去找他谈谈，也许能对你修神有帮助呢。记得修神途中要打打猎，多打猎才能吸引胖姑娘的爱慕。</t><b/>";
		case 10005:	return "<t>费森森介绍你来的？他们一家是传说中的猎人啊！我毕生之愿就是成为费家那样的好汉！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10311Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "小兄弟";
	else
		%Sex = "小姑娘";

	switch(%Tid)
	{
		case 10001:	return "<t>将你之前杀得遍地都是的狐狸毛皮收集回来，交给张黎</t><b/>";
		case 10002:	return "<t>老天！费森森让你猎杀了这附近的狐狸？什么，你还杀了血尾！</t><b/>" @
											"<t>" @ %Sex @ "，我不知道该赞美还是责骂你，你若知道仙游岭是由一个狐仙护佑之地，也许就不会这么干了。</t><b/>" @
											"<t>赶紧打扫战场，把你搞得遍地的狐狸毛皮统统收集回来，要是狐仙看到了怎么办？快去！</t><b/>";
		case 10005:	return "<t>毛皮都这么多，看来你杀得不少，唉，狐仙大人神通广大，也不知道他是否知道。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10312Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>找到水墨古镇上的夕苍，与他谈谈，也许他能帮你出谋划策</t><b/>";
		case 10002:	return "<t>我们这个村子居住在湖泊之上，是狐仙大人保佑此地风平浪静，否则有个大风大浪的怎么办？</t><b/>" @
											"<t>你捕杀了如此多的狐狸，唉……叹气也无用，你快去找镇上的夕苍灵师谈谈，他也许能知道这是福是祸。</t><b/>";
		case 10005:	return "<t>吾乃魔族，路过此地，见天青草碧，故歇息游玩几日。哪知道就这短短数日，我就发现此地非同小可。在美丽飘渺的传说背后，有不为人知的秘密。</t><b/>" @
											"<t>我看你是修神之士，你杀那些狐狸，难不成也是发现了这个秘密？</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10313Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到镇东口晒渔网的地方找到何英竹，从她那里了解仙游岭的传说</t><b/>";
		case 10002:	return "<t>杀狐之事，尚不知对错与否，我发现的秘密或能看清仙游岭传说背后的故事。</t><b/>" @
											"<t>不管如何，你需要先了解这里的传说，一个很美丽的传说。先去找何英竹聊聊，她就在镇东口晒渔网的地方。</t><b/>";
		case 10005:	return "<t>夕苍让你来的？就是问传说的事吗？没有别的什么？唉，我还以为他让你来传些心里话。他模样长得俊，就是性格冷冷的。</t><b/>" @
											"<t>至于这传说么，那真是太美了，来过仙游岭的人都深深被这传说打动。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10314Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到镇上找到骆叔，接着听他讲述仙游岭的传说</t><b/>";
		case 10002:	return "<t>相传百年之前，仙游岭还是一片荒野，一个书生居住在此，寒窗苦读。有一天，百花神女的七个女儿下凡游玩，其中小女儿紫罗兰花神便来到了仙游岭。那天，她敲开了书生的茅屋，看到了书生。</t><b/>" @
											"<t>书生很害怕，嘻嘻，他为何会怕呢？难道神女长得很丑陋？你去问骆叔叔吧，他会接着给你说。</t><b/>";
		case 10005:	return "<t>呵呵，英竹那小丫头又埋伏笔啊。神女怎么会长得丑陋？她是不出凡尘之容，冰清玉洁，美得令日月黯色。</t><b/>" @
											"<t>书生害怕是因为他哪里见过这么超凡脱俗的女子，换成你想想，本来人迹罕至，突然房屋里进来一个仙女，能不吓你一跳？</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10315Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>到镇上找到罗村长，接着听他讲述仙游岭的传说</t><b/>";
		case 10002:	return "<t>那天，神女与书生相谈甚欢，也深深被书生的才华品格吸引。之后数日他们都在一起，久而久之，两者已不能失去对方。</t><b/>" @
											"<t>呵呵，问世间情为何物，惟情能跨越种族，不畏万物，这一人一神，到头来还是相爱了。神女在此居住了三年，夫妻恩爱，还养了一条银狐。</t><b/>" @
											"<t>唉，到这里原本是个很好的故事，可惜啊……可惜……后面的事情我讲不下去了，每次说都不舒服，你去找罗村长讲吧。</t><b/>";
		case 10005:	return "<t>你也是来仙游岭游玩的人吧，好，就让我把这个故事给你讲完，听完后你对这里就更了解了。</t><b/>" @
											"<t>要知道，人神是不可相恋的，书生和神女打破禁锢，悲惨的事情也随之而来了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10316Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>听完罗村长讲的传说</t><b/>";
		case 10002:	return "<t>接下来的故事凄美婉转，仙游岭所有的美丽都蕴藏在其中，呵呵，是故事终会有结局，你静下心准备听结局了吗？</t><b/>";
		case 10005:	return "<t>呵呵，这虽然只是个传说，但仙游岭的人都相信这是真的，因为此地的确有狐仙，我们也受他的福泽呢。</t><b/>";
		
	  case 200: return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我静下心了，村长请讲。</t>";
	  case 201: return "<t>话说天上一日，地上一年。三年之后，百花神女让女儿回归天界。紫罗兰花神当然也不可抗命，她与书生就在此日分离。那天，花神泪洒原野，泪水触地便化为了大片大片的紫罗兰花。美丽的</t><b/>"@
	  	               "<t>紫云架就在花神的泪水中诞生。神女离去，书生痛苦欲绝，数日后伤情而亡，死后变成了青草，相伴紫罗兰花。</t><b/>"@
	  	               "<t>而他们养的银狐，三年中沾了神女的些许灵力，后来修成了狐仙，守护着主人留下的土地，保护着仙游岭，让这片土地永远如百年前那般美。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10317Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>听完传说了，去找夕苍吧</t><b/>";
		case 10002:	return "<t>呵呵，故事到这里就结束了。是夕苍让你来的吧，他是外来的修神人，一个魔族，听了故事后就怪怪的。你去看看他吧。</t><b/>";
		case 10005:	return "<t>听完传说了？如何，是否觉得凄美绝伦？我却不这么看。这传说背后，一定有些什么，其中真真假假，倒还说不清楚。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10318Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>按照夕苍的吩咐，从白狐身上取到几份白狐内丹</t><b/>";
		case 10002:	return "<t>你我都是修神之士，能看出来这里的狐狸都乃狐妖，且修炼旁门左道，凶戾无比。这传说背后肯定有更深的隐晦。</t><b/>" @
											"<t>你愿与我共寻这秘密吗？在此之前，我需要一些白狐内丹，有了这些内丹，我就能带你去一个神妙之地，那里定能找到一些蛛丝马迹。</t><b/>";
		case 10005:	return "<t>看，这些内丹恶臭冲天，修的不知是何妖术，根本不是名门正派之法。</t><b/>" @
											"<t>管它是好是坏，里面的灵力倒是很有用的。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10319Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在银色小丘附近放置香喷喷的炖鸡，吸引出雪牙，之后将她的内丹带给夕苍</t><b/>";
		case 10002:	return "<t>我已将灵力提炼出来，但还不够，不足以我施展那个法术。哼，这些狐妖实在太弱！</t><b/>" @
											"<t>在白狐群中，有一堆银色的小丘，白狐的母亲【雪牙】就隐匿在附近。把这盘鸡肉带去，放在小丘附近，当雪牙闻到香味就会出现。之后宰了她，拿回她的内丹！</t><b/>";
		case 10005:	return "<t>这颗内丹足够大了，应当能让我施展一次法术。好好准备一下，我们这就去了解传说之秘。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10320Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>在水墨洞天顶层上找到能让你回到七天前的魔玉</t><b/>";
		case 10002:	return "<t>现在的灵力足够让我施展一次魔族的【魔之徊徨】，这个法术属于心魔之法，能将人带到过去的真实之境。不过我法力不强，并不能让你回到百年之前，你只能在七日前探查真相。</t><b/>" @
											"<t>魔之徊徨被封印在一个魔玉之内，藏在水墨洞天顶层，我即刻传你过去。找到魔玉，便能回到七日之前，看到真实发生的事情。</t><b/>";
		case 10005:	return "<t>这就是夕苍所说的魔玉吧，沉甸甸的感觉，可如何使用呢？</t><b/>";
			
		case 200:	return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我准备好了</t>";
		case 300:	return  $Icon[4] @ $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>";	
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10321Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>进入魔之徊徨后，与夕苍交谈</t><b/>";
		case 10002:	return "<t>拿起魔玉的瞬间，你觉得一阵眩晕，所有的意识猛然往魔玉内灌入。耳边隐隐想起夕苍的声音：心魔之力，忍耐片刻即可。我在七日前等你。</t><b/>" @
											"<t>【一个大漩涡将你彻底拉入，一片寂黑】</t><b/>";
		case 10005:	return "<t>你来了，我们时间不多，魔之徊徨耗费灵力巨大，我支持不了太久。</t><b/>" @
											"<t>在此地切勿乱跑，心魔乃属魔界之地，在心魔中不知藏着多少凶狠暴戾的暗魔。当心点！一不留神就会被心魔反噬，成为暗魔，永远困在此地。</t><b/>";
											
		case 200:	return  $Icon[2] @ $Get_Dialog_GeShi[31204] @ "我准备好了</t>";
		case 300:	return  $Icon[4] @ $Get_Dialog_GeShi[31204] @ "稍等片刻，我还需修整片刻</t>";	
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10322Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去亭子旁找到商小莲，了解她为什么哭</t><b/>";
		case 10002:	return "<t>我听到一个小女孩的抽泣声，就在镇上的亭子旁，你去看看有何情况。路上小心那些暗魔。</t><b/>";
		case 10005:	return "<t>你……你是谁？也是臭狐狸派来的坏人？</t><b/>" @
											"<t>呜……小莲半夜被一个怪声音叫醒，一条白色的狐狸把我引了出来，之后变成了一个妖怪，跟我说狐仙要让我去当童子。呜呜……小莲不想去，小莲想陪爹爹，我知道这些狐狸都是妖怪，根本不是传说中那样……</t><b/>" @
											"<t>小莲要爹爹，要爹爹……呜呜……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10323Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉几个妖气白狐后安慰商小莲</t><b/>";
		case 10002:	return "<t>那狐妖说几天之内就会让村里人将我送上狐仙居，呜呜，小莲不想去。狐仙是骗子，镇上人不知道，但爹爹知道。爹爹说传说有一半是假的，真正的事情不是这样的！</t><b/>" @
											"<t>刚才诱我出来的狐狸就从北面出去了，那里有好多狐狸，你去把它们赶走，行吗？</t><b/>";
		case 10005:	return "<t>你把它们赶走了么？呜呜……小莲想爹爹，小莲害怕。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10324Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀了妖狐使者和妖狐小卒，将妖狐使者身上的发钗交给商小莲</t><b/>";
		case 10002:	return "<t>刚才诱我出来的狐狸抢走了我一根发钗。狐仙如果拿到了我的发钗，就会说我适合修炼，他选中了我。，那么村里人就会送我去狐仙居了！</t><b/>" @
											"<t>死狐狸在骗人！根本没有什么修炼，没有的！</t><b/>" @
											"<t>你能帮我把发钗抢回来吗？那是个妖狐使者，它还带着四个妖狐小卒。小莲害怕……</t><b/>";
		case 10005:	return "<t>不，不是这根，小莲的发钗是银钗，上有娘亲刻的【紫罗兰】三字。</t><b/>" @
											"<t>呜呜，怎么办，妖怪肯定将我的发钗藏起来了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10325Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "大哥哥";
	else
		%Sex = "大姐姐";

	switch(%Tid)
	{
		case 10001:	return "<t>将小莲的事情告诉夕苍，商讨解决之法</t><b/>";
		case 10002:	return "<t>爹爹虽然知道真相，但阻碍不了村里的人，村里的人都相信狐仙的传说。</t><b/>" @
											"<t>" @ %Sex @ "，想办法救救小莲，小莲害怕……小莲害怕……</t><b/>" @
											"<t>【抚摸小莲的头发。思忖：还是去找夕苍谈谈吧】</t><b/>";
		case 10005:	return "<t>如此看来，这传说的确有猫腻，而狐仙就是其中的关键。</t><b/>" @
											"<t>我法力已尽，无法再维持此术，我们没办法去帮小莲夺回发钗了。但是她说她娘亲在发钗上写的【紫罗兰】三字又是何意呢？</t><b/>" @
											"<t>【面目骤然痛苦】</t><b/>" @
											"<t>不行，心魔已开始反噬，我们赶紧出去。</t><b/>";
											
//		case 200:	return $Get_Dialog_GeShi[31204] @  "我准备好了</t>";
//		case 300:	return $Get_Dialog_GeShi[31204] @  "稍等片刻，我还需修整片刻</t>";										
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10326Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>找罗村长询问商小莲的住处</t><b/>";
		case 10002:	return "<t>当务之急是速速找到商小莲，不知这女童住何家何户，问问村长应当知道。</t><b/>";
		case 10005:	return "<t>小莲，呵呵，这娃娃运气当真好，被狐仙大人选中随他修炼。</t><b/>" @
											"<t>今儿个一早我们就送她去了登云台，这会儿应当与狐仙大人在一起了。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10327Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>速速前往登云台，杀掉那里的灵狐守卫，再看看商小莲是否留下了什么东西</t><b/>";
		case 10002:	return "<t>小莲都已送到登云台了，这又不是什么坏事，你为何还固执要寻她？</t><b/>" @
											"<t>我们一早将她送到登云台后就离开了，狐仙大人会亲自来领她。我们这些普通人怎能随便看狐仙大人的真颜？</t><b/>" @
											"<t>你若要去登云台看看，就在仙游岭的西方，那里有数名守卫，劝你还是别乱闯。</t><b/>";
		case 10005:	return "<t>这支银发钗上写有【紫罗兰】三个娟秀字迹，应当是小莲的。这样随意丢在此地，看来是被狐仙强行带走，挣扎留下。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

//■■■■■■■第一章■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■第二章■■■■■■■■■■■■■■■■

function Mission10340Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去找登云台附近的蓝衣村民谈谈</t><b/>";
		case 10002:	return "<t>看着这支银发钗，你不仅暗想：商小莲被送到登云台，为何他的父亲无动于衷？这一路过来看到有个蓝衣村民就在登云台外，去问问他吧。</t><b/>";
		case 10005:	return "<t>我是镇上的人，平时都是我打扫登云台。你是外来的人吧，咳咳，跟你说个秘密，方才我听到小莲那女娃娃的哭声，哭得那个凄惨啊！</t><b/>" @
											"<t>我总觉得这不像是去修炼的，修炼怎会这样害怕，这话如果我不说，心里不安啊！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10341Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>带着小莲的发钗到紫云架找到商浩</t><b/>";
		case 10002:	return "<t>小莲的父亲叫商浩，是六年前才搬来的书生。当时他抱着还是婴儿的小莲，孤孤单单，也没有妻子。大家见他可怜，便让他留在镇上。</t><b/>" @
											"<t>商浩一向不信狐仙传说，此次将小莲送往登云台，他一介书生无法阻止镇民，唉，这会儿应当在紫云架难过呢。</t><b/>" @
											"<t>你把这根银钗带上，去安慰安慰他吧，唉，两父女真够可怜的。</t><b/>";
		case 10005:	return "<t>小莲的发钗……小莲啊小莲，爹爹对你不住，爹爹恨自己手无缚鸡之力，不能护你啊！紫萝，我愧对于你，我们的女儿……她……她……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10342Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉妖狐小卒后再回报商浩</t><b/>";
		case 10002:	return "<t>我要救小莲！不管如何，我也要救她！</t><b/>" @
											"<t>你知道传说是假的吗？真实并非如此，这些都是狐妖！是修邪道的狐妖，是妖怪！</t><b/>" @
											"<t>你若助我，便将这紫云架的妖狐小卒统统杀掉，它们不配在此地。</t><b/>";
		case 10005:	return "<t>这些妖狐都是狐三的手下，狐三便是他们说的狐仙大人。哼，此物乃邪妖而已，又如何成了狐仙。它瞒得过他人，又怎骗得了我？</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10343Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>用铲子挖掘西边花篮和北边花篮，击败秘法守卫和灵法守卫后，将盒子给商浩</t><b/>";
		case 10002:	return "<t>为了让你真正知道狐仙乃邪妖，你需要知道真实的传说。呵呵，对我来说，那并非传说……而是尘封百年的回忆……</t><b/>" @
											"<t>你在紫云架的西方和北面能找到两个花篮，里面埋了两个盒子，你用这把铲子去挖来吧。小心些，这两个花篮都有法术守护，你需要击败那些守卫。</t><b/>";
		case 10005:	return "<t>唉，我已太久没有见过这两个盒子了，想想已有八十年了吧……</t><b/>" @
											"<t>紫萝啊紫萝，人间八十三年匆匆而过，如大梦一场……你让我如今不老不死，这永恒的孤独寂寞，你可知其中滋味……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10344Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>帮助商浩打开盒子，召唤出紫萝的幻象，听他们谈些什么</t><b/>";
		case 10002:	return "<t>你应当能猜出我的身份了吧。不错，我便是百年前与花神相爱的书生，这两个盒中装的是我与她的定情之物。</t><b/>" @
											"<t>对了，我夫人叫紫萝。</t><b/>" @
											"<t>紫萝曾说，两个盒子打开能召出她的幻象，能与我交流一次，但法力仅可维持一次。如今我必须打开了，为了我们的女儿……</t><b/>" @
											"<t>朋友，可我……我实在没有勇气去打开它们，你能帮我吗？</t><b/>";
		case 10005:	return "<t>这漫山遍野的紫罗兰花，开了一季败了一季，但花败后还能再开，紫萝却不知何时才能回到我的身边。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

//■■■■■■■■■■■第二章■■■■■■■■■■■■■■■■

//■■■■■■■■■■■■■■■第三章■■■■■■■■■■■■

function Mission10380Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉妖狐贼匪和彩纹狐后去找唐诗诗</t><b/>";
		case 10002:	return "<t>那什么狐仙便是当年我和紫萝养的银狐，紫萝不在，它现在作威作福，已没将人放在眼里。</t><b/>" @
											"<t>幻变之法……原本我以为永远都不会使用它，也是该用之时了！杀掉那些妖狐贼匪和彩纹狐后去找唐诗诗，她是我的好友，知道我要做什么。</t><b/>";
		case 10005:	return "<t>商浩让你杀了狐妖？他总算要反击了么？呵呵，他的本事别人可没有。整个仙游岭恐怕只有我唐诗诗最了解他吧。</t><b/>" @
											"<t>商浩必要使用幻术了，那是花神赋予力量，他此刻一定在唤醒那种能力，我们则助他一臂之力！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10381Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉妖狐小将，从它们身上取到几份浓烈的妖气</t><b/>";
		case 10002:	return "<t>一步一步来，小侠，有些准备工作要做。</t><b/>" @
											"<t>追云坡直通狐仙居，坡上守卫了许多妖狐小将，它们体内的妖气极为浓烈，我们要搞到手，呵呵。</t><b/>" @
											"<t>下手时不要留情，狠狠的！死得越痛，妖气就越浓，要下狠手哟。</t><b/>";
		case 10005:	return "<t>呵呵，这些妖气浓厚得如同墨汁，小侠你还真狠呢！不过我很喜欢～</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10382Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>唐诗诗要你到追云坡采几株雾叶草带给她</t><b/>";
		case 10002:	return "<t>有一种传说中的植物，能将妖气中的邪恶气息散发到最大，一个普通人哪怕沾了一丁点，也会被人误以为是妖怪。</t><b/>" @
											"<t>我们需要这种植物，呵呵，你真走运，虽然大多数人以为这种植物是传说，但唐诗诗很清楚。</t><b/>" @
											"<t>就在追云坡，带回那些雾叶草。</t><b/>";
		case 10005:	return "<t>我的天啊，这么多雾叶草，足以使妖气散发得冲天蔽日啦！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10383Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>杀掉惊仙窟的巡礼妖狐，扒了它们的皮，带几份给唐诗诗</t><b/>";
		case 10002:	return "<t>在东边有个叫惊仙窟的洞穴，里面生长许多仙果灵草，一些巡礼妖狐专门采集这些仙果，敬奉给它们的狐仙大人。</t><b/>" @
											"<t>巡礼妖狐能接近狐仙，我们要从它们身上得到帮助。所以……这次……我要它们的……皮……</t><b/>" @
											"<t>你没听错，杀了那些巡礼妖狐后，把皮扒下来带给我。</t><b/>";
		case 10005:	return "<t>哎呀，小侠，这些皮是不行的。你看，你下手太重了，这些狐皮都破啦～</t><b/>" @
											"<t>而且它们太小了！</t><b/>" @
											"<t>【大眼睛盯着你看了半天】</t><b/>" @
											"<t>套在你身上肯定不合身！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10384Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>去惊仙窟中找到灵眼</t><b/>";
		case 10002:	return "<t>要找一个大的妖狐，呵呵。</t><b/>" @
											"<t>在找这个大家伙之前，我们要削弱惊仙窟内妖狐的力量，免得到时候群起而攻之，我们再大的本事也应付不了呀。</t><b/>" @
											"<t>我在惊仙窟中藏了一块灵眼，我能通过它看清洞里的形势并方便与你交谈。你现在去把灵眼找到。</t><b/>";
		case 10005:	return "<t>嗯，就是这个，听到我的声音了么？</t><b/>" @
											"<t>【灵眼骨碌碌转了转】</t><b/>" @
											"<t>现在你看不到我啦，我却能将你看得清清楚楚，这样看着你似乎还挺不错的。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10385Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>摧毁妖术阵，杀掉青焰蛇和妖狐监督后，去灵眼处回报</t><b/>";
		case 10002:	return "<t>那些巡礼妖狐被你之前杀去了七七八八，但这里还有许多妖术阵和遍地的青焰蛇。嗯，你要杀掉那些蛇，摧毁妖术阵，如此妖狐就无法在这里获得更多力量了。</t><b/>" @
											"<t>对了，还有那些妖狐监督，记得也给它们放点血。</t><b/>";
		case 10005:	return "<t>你办事总是很利索呢，诗诗越来越喜欢了。加把劲，我们要帮助商浩施展幻变之术。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10386Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>对【管家】黑老奴投掷仙术瓶后，杀掉他，拿他的皮去见唐诗诗</t><b/>";
		case 10002:	return "<t>差不多了，是时候去找那个大家伙了。</t><b/>" @
											"<t>黑老奴藏在洞深处，他是狐仙的管家，拥有一个庞大的身躯。带上这个瓶子，瓶内的仙术会帮助你削弱管家的妖力。</t><b/>" @
											"<t>记住，千万别直接去挑战管家，否则你一定会死得很惨。先对他投掷仙术瓶，等他变弱后再杀他不迟，不要忘了带回他的皮。</t><b/>" @
											"<t>办到后直接到洞外找我。</t><b/>";
		case 10005:	return "<t>哈，照我说的做没错吧，扒皮的快感很特别吧～</t><b/>" @
											"<t>诗诗可是一个温柔的姑娘，我下不了手咧！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10387Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>将打包的布袋交给商浩</t><b/>";
		case 10002:	return "<t>现在妖气、雾叶草、狐皮都准备好了，商浩的幻变之术应当也准备就绪。我将这些物品装在了布袋里，你带回商浩处吧，记得代我向他问声好。诗诗盼着你和他写下真实的传说。</t><b/>";
		case 10005:	return "<t>多谢你和诗诗了，你我相识不久，却为我奔赴重重险地，商浩一家永生难忘。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10388Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>穿上幻变妖狐袍，变成一个巡礼妖狐，之后去狐仙居打听小莲的下落</t><b/>";
		case 10002:	return "<t>我将这些材料做成了幻变妖狐袍，穿上后你会变成……一个巡礼妖狐……</t><b/>" @
											"<t>穿上后你会有妖气，它们分辨不出来，不过一旦你攻击或被攻击，就会现出原形。</t><b/>" @
											"<t>此时你要以妖狐的身份混入狐仙居，那里应该有妖狐监使，它肯定知道小莲的下落。</t><b/>";
		case 10005:	return "<t>干！女娃子在哪关你何事？瞧你那点出息，那是你能吃的么？狐三大爷说这个女娃子体内有灵气，搞不好吃了就修炼成神了。</t><b/>" @
											"<t>老子都没福享用，只能天天干看着，闹心！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10389Dialog(%Npc, %Player, %Mid, %Tid)
{
	switch(%Tid)
	{
		case 10001:	return "<t>把菜肴给商小莲送去</t><b/>";
		case 10002:	return "<t>他娘的，老子还要伺候那小女娃，喂吃喂喝的，还不能对她怎样！否则狐三大爷一定会把我丢去和老虎摔跤！</t><b/>" @
											"<t>你不是想看那女娃娃吗？把这碗饭给她送去，老子可不想看她，眼不见什么来着？对！眼不见为屁！这些讨厌的事情就如同放了个屁，嘣！啊～舒服～</t><b/>";
		case 10005:	return "<t>死妖怪！死妖怪！我要爹爹，呜呜呜……</t><b/>" @
											"<t>【你走进后她盯着你，微微抽泣，没有大哭了】</t><b/>" @
											"<t>你……你不是妖怪，小莲看得出来，你是七天前的那个人，你是来救我的吗？</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10390Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "哥哥";
	else
		%Sex = "姐姐";

	switch(%Tid)
	{
		case 10001:	return "<t>杀掉几个妖狐头子后与小莲谈谈</t><b/>";
		case 10002:	return "<t>" @ %Sex @ "是爹爹让你来的！呵呵，我就知道爹爹最有本事！</t><b/>" @
											"<t>这个笼子很坚固，有好大一把锁，这里有好多妖狐头子，不知道他们身上有没有钥匙。你能去帮我找找吗？我想回家，想见爹爹。</t><b/>" @
											"<t>我一个人在这里很害怕，不管有没有找到，你一会都要来跟我说说话哦。</t><b/>";
		case 10005:	return "<t>还是没有钥匙么？那小莲怎么出去？呜呜呜……</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10391Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "哥哥";
	else
		%Sex = "姐姐";

	switch(%Tid)
	{
		case 10001:	return "<t>去见狐三大爷，从他那里打听牢笼钥匙</t><b/>";
		case 10002:	return "<t>呜呜，是谁拿了钥匙呀……</t><b/>" @
											"<t>应该只有那个老狐狸才知道了，" @ %Sex @ "，那个臭狐仙自称狐三大爷，又丑又老。你敢去找他么？他肯定知道钥匙在哪，但是，小莲怕他也能看穿你是假的……</t><b/>";
		case 10005:	return "<t>嗯，你这个小狐狸，看着怎的眼生？</t><b/>" @
											"<t>大爷我这段时间需调养生息，好服下灵童，飞升成神，没事勿来扰我。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10392Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "哥哥";
	else
		%Sex = "姐姐";

	switch(%Tid)
	{
		case 10001:	return "<t>找到笑面狐，从它身上取到牢笼钥匙后去救小莲</t><b/>";
		case 10002:	return "<t>什么？你说关女娃的锁有点松？那笑面狐怎么搞的，你快去找他，叫他把笼子给我锁牢靠些。如果出了什么漏子，哼！</t><b/>" @
											"<t>【看来钥匙在一个笑面狐的身上，去砍翻它抢到钥匙就能救小莲了】</t><b/>";
		case 10005:	return "<t>哈哈，" @ %Sex @ "你真厉害，小莲终于可以回家了！</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10393Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "哥哥";
	else
		%Sex = "姐姐";

	switch(%Tid)
	{
		case 10001:	return "<t>护送商小莲逃出狐仙居</t><b/>";
		case 10002:	return "<t>现在我们要离开这里了，" @ %Sex @ "，我们要小心点，这里的妖怪好多。</t><b/>" @
											"<t>我们现在就下山吧……</t><b/>";
		case 10005:	return "<t>终于到山下了，哈哈，" @ %Sex @ "你真厉害，那些妖狐都被你打得屁滚尿流。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

function Mission10394Dialog(%Npc, %Player, %Mid, %Tid)
{
	if(%Player.GetSex() == 1)
		%Sex = "哥哥";
	else
		%Sex = "姐姐";

	switch(%Tid)
	{
		case 10001:	return "<t>找商浩谈谈</t><b/>";
		case 10002:	return "<t>小莲知道怎么回家，" @ %Sex @ "你去找爹爹吧，他见到你一定很高兴的。</t><b/>" @
											"<t>谢谢你，再见。</t><b/>";
		case 10005:	return "<t>你已救回小莲了吗？谢谢恩公，此恩难以言谢，商浩永记一生。</t><b/>" @
											"<t>这些东西虽然不算很好，但请恩公一定收下。</t><b/>";
	}
	return "Mission" @ %Mid @ "Dialog == 错误 Npc 【" @ %Npc @ "】Player【" @ %Player @ "】Mid【" @ %Mid @ "】Tid【" @ %Tid @ "】";
}

//■■■■■■■■■■■■■■■第三章■■■■■■■■■■■■
