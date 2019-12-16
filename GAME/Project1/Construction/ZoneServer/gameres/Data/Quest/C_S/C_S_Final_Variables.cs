//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//管理客户端与服务端的所有全局变量：任务数据变量不在本脚本内
//==================================================================================



//=========客户端==================================================================
//■■■■■制定各字体的格式■■■■■■■■■■■■■■■■■■■■■■■■■■
//■■■■■NPC随机对话■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■■■■■任务相关名称■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■■■■■地图名称■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//■■■■■图标变量■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//====================服务端=======================================================
//■■■■■■■■■■■经验值公式相关■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■区域传送变量记录■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■地图传送变量■■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■道具鉴定符变量■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■活动相关变量■■■■■■■■■■■■■■■■■■■■■■■
//===============================服务端与客户端共用的===============================
//■■■■■■■■■■■全局的特效编号■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■任务最大数量■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■获取门宗名称与地图名字■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■道具是哪个怪物掉落数据■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■各个礼包类型的道具数据■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■各个采集物给于的物品■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■各个怪物等级给于的基本经验■■■■■■■■■■■■■■■
//■■■■■■■■■■■各配方学习需求■■■■■■■■■■■■■■■■■■■■■






//=========客户端==================================================================
if($Now_Script == 1)
	{
//■■■■■制定各字体的格式■■■■■■■■■■■■■■■■■■■■■■■■■■
	//只能使用以下四种字体:
	//宋体,黑体,楷体_GB2312,隶书
	//字号从10,12,14,16,18,20共6种。
	//只有宋体12和宋体14是点阵字体
	//其它字体均使用抗锯齿处理

	//f = 字体
	//m = 是否点阵字显示，1点阵字，0优化抗锯齿显示
	//n = 字号
	//c = 文字颜色
	//o = 文字描边的颜色
	//u = 下划线的颜色
	//h = 是否左右居中，0或不填默认为左对齐，1左右居中，2右对齐
	//v = 是否垂直居中，0默认下对齐，1垂直居中，2向上对齐，文本内文字默认为0,按钮内文字默认为1

//黑体
		$Get_Dialog_GeShi[11200] = "<t f='黑体' m='0' n='12' c='0xf3d300ff' o='0x291b00ff'>";				//黄色
		$Get_Dialog_GeShi[11600] = "<t f='黑体' m='0' n='16' c='0xf3d300ff' o='0x291b00ff'>";				//黄色
		$Get_Dialog_GeShi[11400] = "<t f='黑体' m='0' n='14' c='0x00b0f0ff' o='0x291b00ff'>";				//蓝色
//隶书
		$Get_Dialog_GeShi[21800] = "<t f='隶书' m='0' n='18' c='0xf3d300ff' o='0x291b00ff'>";				//黄色
//宋体
		$Get_Dialog_GeShi[31200] = "<t f='宋体' m='1' n='12' c='0x000000ff' o='0xffffffff'>";				//黑色
		$Get_Dialog_GeShi[31201] = "<t f='宋体' m='1' n='12' c='0xffffffff' o='0x2d2d2dff'>";				//白色
		$Get_Dialog_GeShi[31202] = "<t f='宋体' m='1' n='12' c='0x00b0f0ff' o='0x291b00ff'>";				//蓝色
		$Get_Dialog_GeShi[31203] = "<t f='宋体' m='1' n='12' c='0x00ff4eff' o='0x010101FF'>";				//绿色
		$Get_Dialog_GeShi[31204] = "<t f='宋体' m='1' n='12' c='0xf3d300ff' o='0x291b00ff'>";				//黄色
		$Get_Dialog_GeShi[31205] = "<t f='宋体' m='1' n='12' c='0xff8585ff' o='0x291b00ff'>";				//粉色
		$Get_Dialog_GeShi[31206] = "<t f='宋体' m='1' n='12' c='0xff3232ff' o='0x2d1900ff'>";				//红色

		$Get_Dialog_GeShi[31207] = "<t f='宋体' m='1' n='12' c='0xccccccff' o='0x291b00ff'>";				//灰色
		$Get_Dialog_GeShi[31208] = "<t f='宋体' m='1' n='12' c='0x4400ffff' o='0x291b00ff'>";				//蓝色
		$Get_Dialog_GeShi[31209] = "<t f='宋体' m='1' n='12' c='0xff00ffff' o='0x291b00ff'>";				//紫色

		$Get_Dialog_GeShi[31210] = "<t f='宋体' m='1' n='12' c='0xffffffff'>";				//白色无描边
		$Get_Dialog_GeShi[31211] = "<t f='宋体' m='1' n='12' c='0x7F0000ff' o='0x614018ff'>";				//褐色
		$Get_Dialog_GeShi[31212] = "<t f='宋体' m='1' n='12' c='0x00ff4eff' o='0x010101FF'>";				//绿色，任务奖励文字
		$Get_Dialog_GeShi[31213] = "<t f='宋体' m='1' n='12' c='0xf3d300ff' o='0x291b00ff'>";				//黄色
		$Get_Dialog_GeShi[31214] = "<t f='宋体' m='1' n='12' u='0xbfff00ff' c='0xbfff00ff' o='0x010101ff'>";				//草绿色+白色下划线

		$Get_Dialog_GeShi[31401] = "<t f='宋体' m='1' n='14' c='0xffffffff' o='0x000000ff'>";				//白色无描边

		$Get_Dialog_GeShi[31800] = "<t f='宋体' m='0' n='18' c='0xf3d300ff' o='0x291b00ff' h='1'>";				//黄色_居中
//楷体
		$Get_Dialog_GeShi[41200] = "<t f='楷体_GB2312' m='0' n='12' c='0xf3d300ff' o='0x291b00ff'>";				//黄色

		$Get_Dialog_GeShi[41400] = "<t f='楷体_GB2312' m='0' n='14' c='0xf3d300ff' o='0x291b00ff'>";				//黄色
		$Get_Dialog_GeShi[41401] = "<t f='楷体_GB2312' m='0' n='14' c='0x4f2200ff' o='0xac854eff'>";				//棕色
		$Get_Dialog_GeShi[41402] = "<t f='楷体_GB2312' m='0' n='14' c='0x0048ffff' o='0x4fa8b3ff'>";				//蓝色
		$Get_Dialog_GeShi[41403] = "<t f='楷体_GB2312' m='0' n='14' c='0xffffffff' o='0x291b00ff'>";				//白色
		$Get_Dialog_GeShi[41404] = "<t f='楷体_GB2312' m='0' n='14' c='0x00ff4eff' o='0x291b00ff'>";				//绿色

		$Get_Dialog_GeShi[41601] = "<t f='楷体_GB2312' m='0' n='16' c='0x4f2200ff' o='0xac854eff'>";				//棕色
//特殊格式
		$Get_Dialog_GeShi[50000] = "<t u='0xffffffff' o='0x291b00ff'>";				//下划线－白色
		$Get_Dialog_GeShi[50001] = "<t c='0xf3d300ff' o='0x291b00ff'>";				//描边
		$Get_Dialog_GeShi[50002] = "<t v='0' u='0xffffffff' o='0x291b00ff'>";				//下划线－白色
		$Get_Dialog_GeShi[50003] = "<t c='0x00ff4eff' o='0x010101FF'>";				//描边

//■■■■■制定各字体的格式■■■■■■■■■■■■■■■■■■■■■■■■■■

//■■■■■NPC随机对话■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//万灵城
		$NpcRandomDialog[400001101,1] = "修道之人重在修心，戒骄戒躁，明镜在心方能得成正果。";
		$NpcRandomDialog[400001101,2] = "切忌损人利己，若为求成而损人利己，终难逃天劫。";
		$NpcRandomDialog[400001101,3] = "修道之路必历尽万难方能得成正果，投机取巧终不得。";
		$NpcRandomDialog[400001062,1] = "天书似乎不太愿意搭理你。";
		$NpcRandomDialog[400001062,2] = "天书尝试了用各种语言解释你的问题，但你都无法看懂。";
		$NpcRandomDialog[400001004,1] = "经常会有九幽的人跑来告诉我，他们后悔生前没有多花点钱买防具。";
		$NpcRandomDialog[400001004,2] = "你手里的兵器不错，让我帮你熔了做一件护甲吧。";
		$NpcRandomDialog[400001003,1] = "滚开，别妨碍老子做生意。";
		$NpcRandomDialog[400001003,2] = "也许你让我锤几下，我能像修装备一样修好你。";
		$NpcRandomDialog[400001005,1] = "这位客官您说笑了，小店一向只卖身不卖艺的。";
		$NpcRandomDialog[400001059,1] = "我就是传说中的装备杀手，人称“活武冢”";
		$NpcRandomDialog[400001059,2] = "鉴定师最基本的素质就是心狠手辣，不要向人们的怒火屈服，不要被MM的泪水蒙蔽！";
		$NpcRandomDialog[400001007,1] = "不要以为用一颗棒棒糖就能让我跟你走，哼~（苏苏努力的咽了一口口水）";
		$NpcRandomDialog[400001007,2] = "哎呀哎呀，你灵兽的鼻子流口水了。";
		$NpcRandomDialog[400001046,1] = "昨天晚上仓库失窃，好像你保管的东西也不见了。";
		$NpcRandomDialog[400001046,2] = "你最好一去不回，那你的东西就都归我了。";
		$NpcRandomDialog[400001046,3] = "东西只要到了我手里谁都别想拿走。";
		$NpcRandomDialog[400001024,1] = "一件男装的布料足够做十件女装";
		$NpcRandomDialog[400001024,2] = "好端端的衣服居然故意弄破几个洞，还叫什么时尚";
		$NpcRandomDialog[400001023,1] = "如果连你都能打造最好的武器，那还要BOSS做什么。";
		$NpcRandomDialog[400001025,1] = "如果你把戒指带在脚趾上，那就别想它能帮你什么忙。";
		$NpcRandomDialog[400001026,1] = "符咒乃高深之术，就算你画错几笔别人也看不出来。";
		$NpcRandomDialog[400001027,1] = "如果你知道平时吃的药是用什么做的，你一定宁死不吃。";
		$NpcRandomDialog[400001031,1] = "养生之道在于收心养性，以内养外，气血调和。";
		$NpcRandomDialog[400001020,1] = "如果有什么花凋谢的比昙花还快，那一定就是传说中的烟花。";
		$NpcRandomDialog[400001022,1] = "自从有了妖怪之后，人常常作为猎物而存在。";
		$NpcRandomDialog[400001022,2] = "如果你无法找到猎物，就想办法让猎物找到你。";
		$NpcRandomDialog[400001021,1] = "仙草灵药都是人拿命去换的，所以我们还是采采路边的野花吧。";
		$NpcRandomDialog[400001021,2] = "不要随便尝草药，你不是神农，没他命大。";
		$NpcRandomDialog[400001030,1] = "你身上至少已经中了三种蛊术了，祝你好运。";
		$NpcRandomDialog[400001018,1] = "传说有一种树叫铁树，那一定是锯子锯不断的。";
		$NpcRandomDialog[400001017,1] = "最近的矿石都跟集体自杀似地从地面冒出来。";
		$NpcRandomDialog[400001028,1] = "为什么，为什么你会流泪呢？因为我加了洋葱。";
		$NpcRandomDialog[400001029,1] = "传说木匠神书《缺意门》中记载着无数精巧无比的家具设计。";
		$NpcRandomDialog[400001019,1] = "如果你没办法让鱼自愿上钩，你最好把鱼饵装上。";
		$NpcRandomDialog[400001061,1] = "你如果想去什么地方，我可以送你一程，除了龙宫……";
		$NpcRandomDialog[400001002,1] = "你是来买吃的？这些是食神昨天吃剩的，你看看吧。";
		$NpcRandomDialog[400001002,2] = "你看起来味道不错，让食神咬一口尝尝。";
		$NpcRandomDialog[400001048,1] = "天上繁星聚汇，地上众生联结。";
		$NpcRandomDialog[400001006,1] = "神具通灵，法宝认主，一切皆是缘。";
		$NpcRandomDialog[400001050,1] = "缘由天定，你苦寻知己之时早已红线绕指。";
		$NpcRandomDialog[400001050,2] = "谁都想找最爱的人，可是花轿也无法帮你分辨。";
		$NpcRandomDialog[400001050,3] = "世人皆叹真爱难求，但有几人能真心相爱至白头。";
		$NpcRandomDialog[400001011,1] = "我生前可以是一个老实的生意人。";
		$NpcRandomDialog[400001011,2] = "这就传说中的一日丧命散，和西王母的长生不老药有一样的效果哦，试试吧。";
		$NpcRandomDialog[400001001,1] = "自从我将砒霜装错瓶当补血丹卖给了一个昆仑弟子之后，我就再也没有见过那个人了。";
		$NpcRandomDialog[400001072,1] = "缺胳膊断腿我还可以治，如果只剩胳膊了，我就没办法了。";
		$NpcRandomDialog[400001009,1] = "也许你跑的比它快，但你驮的肯定没它多。";
		$NpcRandomDialog[400001008,1] = "你的灵兽有伴了，那我呢？";
		$NpcRandomDialog[400001008,2] = "让我们来做一些令人害羞的事情吧。";
		$NpcRandomDialog[400001008,3] = "我漂亮吗？";
		$NpcRandomDialog[400001038,1] = "昆仑圣宗以侠义为本，重攻而不重守。";
		$NpcRandomDialog[400001039,1] = "蓬莱仙宗讲究逍遥自在，善以气御剑。";
		$NpcRandomDialog[400001041,1] = "如果你厌倦了尘世，我们会在黄泉另一头等你。";
		$NpcRandomDialog[400001042,1] = "大地生命的力量都汇聚在我们身边。";
		$NpcRandomDialog[400001038,1] = "如果你想使用五行的神力，请加入我们吧。";
		$NpcRandomDialog[400001043,1] = "世间最宝贵的莫过于真情，你和我一样在寻找玛？";
		$NpcRandomDialog[400001044,1] = "佛者本着慈悲心，普度众生，正大光明。";
		$NpcRandomDialog[400001045,1] = "想要对抗你自己的命运吗？我们给你力量。";
		$NpcRandomDialog[400001036,1] = "从这些通缉令的奖金看来，似乎都是一些棘手的家伙。";
		$NpcRandomDialog[400001036,2] = "你猛然看到一张布告上写着你的名字，奖励为一个肉包。";
		$NpcRandomDialog[400001035,1] = "上面贴满了广告：一夜成神水、千年修为丹……";
		$NpcRandomDialog[400001037,1] = "慢慢来，不要急，你现在过去的话会和刚走的那个人合体的。";
		$NpcRandomDialog[400001047,1] = "能与你在此地相遇便是缘，修神之路我会伴你左右。";
		$NpcRandomDialog[400001049,1] = "什么？你说什么？我听不见，请你说大声一点好吗？";
		$NpcRandomDialog[400001060,1] = "此地汇集了女娲神气，为多少修真者梦想之地。";
		$NpcRandomDialog[400001103,1] = "仙鹤眨了眨眼睛，似乎在向你打招呼。";
		$NpcRandomDialog[400001119,1] = "灵元大阵虽非万灵城灵气之源，但其调节能力必不可少。";
		$NpcRandomDialog[400001119,2] = "大阵需要有十名弟子合力催动，任何一方都不能稍有偏差。";
		$NpcRandomDialog[400001119,3] = "你最好离大阵远一点，里面绕动的灵气不是你能够承受的。";
		$NpcRandomDialog[400001120,1] = "护阵弟子正在专心的灌入灵气，还是不要打扰他了";
		$NpcRandomDialog[400001120,2] = "从他的表情可以看出，护阵需要消耗极大的灵气。";
		$NpcRandomDialog[400001121,1] = "护阵弟子正在专心的灌入灵气，还是不要打扰他了";
		$NpcRandomDialog[400001121,2] = "从他的表情可以看出，护阵需要消耗极大的灵气。";
//月幽境
		$NpcRandomDialog[401002010,1] = "上次回来路上被打劫，差点被杀了，虽然我已经死了。";
		$NpcRandomDialog[401002010,2] = "昨天进货赶了几十里山路腿都快跑断了，虽然我没有腿。";
		$NpcRandomDialog[401002001,1] = "今天我用新的材料制作了补血丹，免费试吃啊，来看看吧。";
		$NpcRandomDialog[401002002,1] = "豆腐放上两天是酸豆腐，放两星期是臭豆腐，放两个月就成豆腐乳了。";
		$NpcRandomDialog[401002003,1] = "滚开，别妨碍老子做生意。";
		$NpcRandomDialog[401002003,2] = "也许你让我锤几下，我能像修装备一样修好你。";
		$NpcRandomDialog[401002004,1] = "经常会有九幽的人跑来告诉我，他们后悔生前没有多花点钱买防具。";
		$NpcRandomDialog[401002004,2] = "你手里的兵器不错，让我帮你熔了做一件护甲吧。";
		$NpcRandomDialog[401002005,1] = "和尚脖子上那佛珠明明都可以当武器耍了，偏要放我这儿当饰品买，真是的。";
		$NpcRandomDialog[401002006,1] = "神具通灵，法宝认主，一切皆是缘。";
		$NpcRandomDialog[401002007,1] = "谁能送我最可爱灵兽，我一定毫不犹豫的嫁给他，就算是女的我也嫁。";
		$NpcRandomDialog[401002008,1] = "怎样我才能像媚儿姐姐那样招揽这么多客人呢。";
		$NpcRandomDialog[401002008,2] = "恩…那个…其实…除了宠物之外…我还提…提供一些特…特殊服务";
		$NpcRandomDialog[401002009,1] = "夸父为什么死了，因为他没买坐骑。";
		$NpcRandomDialog[401002011,1] = "最近的矿石都跟集体自杀似地从地面冒出来。";
		$NpcRandomDialog[401002012,1] = "传说有一种树叫铁树，那一定是锯子锯不断的。";
		$NpcRandomDialog[401002013,1] = "如果你没办法让鱼自愿上钩，你最好把鱼饵装上。";
		$NpcRandomDialog[401002014,1] = "如果有什么花凋谢的比昙花还快，那一定就是传说中的烟花。";
		$NpcRandomDialog[401002015,1] = "仙草灵药都是人拿命去换的，所以我们还是采采路边的野花吧。";
		$NpcRandomDialog[401002015,2] = "不要随便尝草药，你不是神农，没他命大。";
		$NpcRandomDialog[401002016,1] = "自从有了妖怪之后，人常常作为猎物而存在。";
		$NpcRandomDialog[401002016,2] = "如果你无法找到猎物，就想办法让猎物找到你。";
		$NpcRandomDialog[401002017,1] = "如果连你都能打造最好的武器，那还要BOSS做什么。";
		$NpcRandomDialog[401002018,1] = "一件男装的布料足够做十件女装";
		$NpcRandomDialog[401002018,2] = "好端端的衣服居然故意弄破几个洞，还叫什么时尚";
		$NpcRandomDialog[401002019,1] = "如果你把戒指带在脚趾上，那就别想它能帮你什么忙。";
		$NpcRandomDialog[401002020,1] = "符咒乃高深之术，就算你画错几笔别人也看不出来。";
		$NpcRandomDialog[401002021,1] = "如果你知道平时吃的药是用什么做的，你一定宁死不吃。";
		$NpcRandomDialog[401002022,1] = "为什么，为什么你会流泪呢？因为我加了洋葱。";
		$NpcRandomDialog[401002023,1] = "这是我新发明的折叠板凳，方便携带，可杀人于无形。";
		$NpcRandomDialog[401002023,2] = "传说木匠神书《缺意门》中记载着无数精巧无比的家具设计。";
		$NpcRandomDialog[401002024,1] = "白驼山擅长用毒，我住在白驼山西边，所以我外号毒西";
		$NpcRandomDialog[401002025,1] = "婆婆我今年八十有六，人家都夸我保养得好，看着像三八";
		$NpcRandomDialog[401002026,1] = "上面贴满了广告：一夜成神水、千年修为丹……";
		$NpcRandomDialog[401002027,1] = "从这些通缉令的奖金看来，似乎都是一些棘手的家伙。";
		$NpcRandomDialog[401002027,2] = "你猛然看到一张布告上写着你的名字，奖励为一个肉包。";
		$NpcRandomDialog[401002028,1] = "传送时请不要乱动，我不敢保证能把你一次性传过去。";
		$NpcRandomDialog[401002029,1] = "昆仑圣宗以侠义为本，重攻而不重守。";
		$NpcRandomDialog[401002030,1] = "蓬莱仙宗讲究逍遥自在，善以气御剑。";
		$NpcRandomDialog[401002031,1] = "如果你厌倦了尘世，我们会在黄泉另一头等你。";
		$NpcRandomDialog[401002032,1] = "大地生命的力量都汇聚在我们身边。";
		$NpcRandomDialog[401002033,1] = "如果你想使用五行的神力，请加入我们吧。";
		$NpcRandomDialog[401002034,1] = "世间最宝贵的莫过于真情，你和我一样在寻找玛？";
		$NpcRandomDialog[401002035,1] = "佛者本着慈悲心，普度众生，正大光明。";
		$NpcRandomDialog[401002036,1] = "想要对抗你自己的命运吗？我们给你力量。";
		$NpcRandomDialog[401002037,1] = "昨天晚上仓库失窃，好像你保管的东西也不见了。";
		$NpcRandomDialog[401002037,2] = "东西只要到了我手里谁都别想拿走。";
		$NpcRandomDialog[401002037,3] = "你最好一去不回，那你的东西就都归我了。";
		$NpcRandomDialog[401002038,1] = "能与你在此地相遇便是缘，修神之路我会伴你左右。";
		$NpcRandomDialog[401002039,1] = "天上繁星聚汇，地上众生联结。";
		$NpcRandomDialog[401002040,1] = "什么？你说什么？我听不见，请你说大声一点好吗？";
		$NpcRandomDialog[401002041,1] = "缘由天定，你苦寻知己之时早已红线绕指。";
		$NpcRandomDialog[401002041,2] = "谁都想找最爱的人，可是花轿也无法帮你分辨。";
		$NpcRandomDialog[401002041,3] = "世人皆叹真爱难求，但有几人能真心相爱至白头。";
		$NpcRandomDialog[401002042,1] = "我就是传说中的装备杀手，人称“活武冢”";
		$NpcRandomDialog[401002042,2] = "鉴定师最基本的素质就是心狠手辣，不要向人们的怒火屈服，不要被MM的泪水蒙蔽！";
		$NpcRandomDialog[401002043,1] = "若能寻得灵丹妙药，修真道路上必能事半功倍。";
		$NpcRandomDialog[401002044,1] = "我想让你去哪里，我就传送你到哪里，我想怎么传就怎么传。";
		$NpcRandomDialog[401002045,1] = "天书似乎不太愿意搭理你。";
		$NpcRandomDialog[401002045,1] = "天书尝试了用各种语言解释你的问题，但你都无法看懂。";
//门宗-昆仑古墟
//极乐西天
		$NpcRandomDialog[410207001,1] = "一切万法，悉皆含摄于悲智二法之中";
		$NpcRandomDialog[410207001,2] = "奇哉，奇哉！大地众生皆有如来智慧德相";
		$NpcRandomDialog[410207001,3] = "佛者普度众生，以济世之怀拯救苍生，功德无量。";
		$NpcRandomDialog[410206001,1] = "为恶未报，前世之德，为善不果，前世有孽。";
		$NpcRandomDialog[410205001,1] = "大肚能容天下难容之事，笑口常开笑天下可笑之人。";
		$NpcRandomDialog[401006007,1] = "世间疾苦，我佛慈悲，普度众生。";
		$NpcRandomDialog[401006001,1] = "佛法浩瀚无边，其所学皆济世渡人之法。";
		$NpcRandomDialog[401006002,1] = "成佛没有捷径，修行是点滴功夫，不经一番寒澈骨，焉得梅花扑鼻香？";
		$NpcRandomDialog[401006006,1] = "我不入地狱，谁入地狱？地狱未空，誓不成佛；众生度尽，方证菩提。";
		$NpcRandomDialog[401006005,1] = "若你为本宗做出贡献，此处的宝物便可为你所有。";
		$NpcRandomDialog[401006004,1] = "若一个你得一坐骑，请勿纵它下凡为害。";
		$NpcRandomDialog[401006003,1] = "阿弥陀佛，小僧在此恭候多时了。";
		$NpcRandomDialog[410204001,1] = "此乃佛门圣地，不得喧哗。";
		$NpcRandomDialog[410203001,1] = "佛祖尝以身割肉喂鹰，此乃大慈悲。";
//落日渊
		$NpcRandomDialog[410807001,1] = "可笑，难道你还相信天上的神明会保佑你吗？";
		$NpcRandomDialog[410807001,2] = "这个世界上能相信的只有自己的力量。";
		$NpcRandomDialog[410807001,3] = "感觉到自己的无力了吗？来吧我给你力量。";
		$NpcRandomDialog[410806001,1] = "你的失败不是因为对手太强，而是你太弱了。";
		$NpcRandomDialog[410805001,1] = "只有强大的力量才能让你的伙伴永远站在你这边。";
		$NpcRandomDialog[401004007,1] = "每个人内心都有魔，任何人都无法逃避自己的心魔。";
		$NpcRandomDialog[401004001,1] = "魔由心生，用不着借助任何人的力量，你就是力量的源泉。";
		$NpcRandomDialog[401004002,1] = "把你的心魔释放出来，壮大它，让对手感到恐惧。";
		$NpcRandomDialog[401004006,1] = "用你的力量去改变所有你不希望看到的事，你就是万物的主宰。。";
		$NpcRandomDialog[401004005,1] = "若你为本宗做出贡献，此处的宝物便可为你所有。";
		$NpcRandomDialog[401004004,1] = "魔行于八方，气吞天下，何人敢挡。";
		$NpcRandomDialog[401004003,1] = "这里是强者的地盘，一切都用实力说话。";
		$NpcRandomDialog[410804001,1] = "我能感受到你身上奔腾的鲜血，来与我一战吧。";
		$NpcRandomDialog[410803001,1] = "你的心在颤抖，怕死的话就离我远点。";
//醉梦冰池
		$NpcRandomDialog[410707001,1] = "妖若为善妖非孽，人若为恶怎称人。";
		$NpcRandomDialog[410707001,2] = "世人笑梦虚，天地始至终不过创世神一场梦而已。";
		$NpcRandomDialog[410707001,3] = "人修真为生，妖修真为情，若得真情，无寿又何妨。";
		$NpcRandomDialog[410706001,1] = "要学会看透对方的内心，让敌人陷于股掌。";
		$NpcRandomDialog[410705001,1] = "不是每个人都会把真实的想法写在脸上的。";
		$NpcRandomDialog[401008001,1] = "看到刚才天上有头牛飞过吗？他就是你师兄。";
		$NpcRandomDialog[401008002,1] = "妖的内丹乃修行之本，化为人也皆因此丹。";
		$NpcRandomDialog[401008006,1] = "门宗内事务繁多，经常出现人手不足的情况。";
		$NpcRandomDialog[401008005,1] = "若你为本宗做出贡献，此处的宝物便可为你所有。";
		$NpcRandomDialog[401008004,1] = "我们本为百兽，百兽都是我们的同伴，请你好好珍惜。";
		$NpcRandomDialog[401008003,1] = "欢迎来到醉梦冰池，我们会好好招待你的。";
		$NpcRandomDialog[401008007,1] = "妖虽迷幻妩媚，但论阴险虚假，远不及人。";
		$NpcRandomDialog[410704001,1] = "人以为只有人会做梦，其实天下生灵皆有梦。";
		$NpcRandomDialog[410703001,1] = "妖宗心法讲究攻心，使妖气让敌人不攻自溃。";
//神木林
		$NpcRandomDialog[410407001,1] = "自然是慷慨的，她赐予我们生命和灵性。";
		$NpcRandomDialog[410407001,2] = "我们都是大自然的女儿。";
		$NpcRandomDialog[410407001,3] = "玉簪姐姐在天上过的好吗？";
		$NpcRandomDialog[410406001,1] = "平静的树林会平静的带走那些躁动的生命。";
		$NpcRandomDialog[410405001,1] = "请别忘记，我们一直都在你们身边。";
		$NpcRandomDialog[401005007,1] = "听见了吗？蝴蝶们正在邀请你一起飞舞。";
		$NpcRandomDialog[401005001,1] = "想要使用自然的力量，就先要学着与她沟通。";
		$NpcRandomDialog[401005002,1] = "精乃集天地精华所成，精元为修炼之本。";
		$NpcRandomDialog[401005006,1] = "门宗内事务繁多，经常出现人手不足的情况。";
		$NpcRandomDialog[401005005,1] = "若你为本宗做出贡献，此处的宝物便可为你所有。";
		$NpcRandomDialog[401005004,1] = "当你迷路的时候不要担心，花儿会为你指引方向。";
		$NpcRandomDialog[401005003,1] = "欢迎来到神木林。";
		$NpcRandomDialog[410404001,1] = "在自然面前生命是如此的脆弱。";
		$NpcRandomDialog[410404001,2] = "不要小看植物的力量。";
		$NpcRandomDialog[410403001,1] = "请时刻爱护身边的一草一木。";
//幽冥鬼域
		$NpcRandomDialog[410507001,1] = "这里不是一个活人应该来的地方。";
		$NpcRandomDialog[410507001,2] = "忘记你的过去，忘记你自己，因为你永远无法回头。";
		$NpcRandomDialog[410507001,3] = "你已经不属于那里，你的存在很快会被人遗忘。";
		$NpcRandomDialog[410506001,1] = "我们就是死亡的化身、绝望的使者。";
		$NpcRandomDialog[410505001,1] = "将人死后的眷恋带回人间，这是与人界的最后一次通话。";
		$NpcRandomDialog[401009001,1] = "九幽乃纯阴之地，若想成神，必先通死气之术。";
		$NpcRandomDialog[401009002,1] = "肉身有涯，魂魄无终，元神即为吾辈之本体。";
		$NpcRandomDialog[401009006,1] = "门宗内事务繁多，经常出现人手不足的情况。";
		$NpcRandomDialog[401009005,1] = "若你为本宗做出贡献，此处的宝物便可为你所有。";
		$NpcRandomDialog[401009004,1] = "若你能寻得奇兽，万重地狱你可任驰骋。";
		$NpcRandomDialog[401009003,1] = "那刻骨铭心的爱，你真的可以忘记吗？你真的舍得忘记吗？";
		$NpcRandomDialog[401009003,2] = "你想死一次看看吗？";
		$NpcRandomDialog[401009003,3] = "世间有多少有情人为情所伤，又岂是我孟婆的一碗汤可医。";
		$NpcRandomDialog[410504001,1] = "时刻小心你的背后，你已经被我盯上了。";
		$NpcRandomDialog[410503001,1] = "你身上似乎还有活人的气息。";
		$NpcRandomDialog[401009007,1] = "若想在九幽黄泉中穿行，御鬼之术是必须。";
		$NpcRandomDialog[401009008,1] = "你生前死后的罪孽将在我手里一次算清。";
//双生山
		$NpcRandomDialog[410607001,1] = "如果你遇到了什么困难，我们愿意帮助你。";
		$NpcRandomDialog[410607001,2] = "我们的力量是来源于天地万物。";
		$NpcRandomDialog[410607001,3] = "请用你的力量帮助所有需要帮助的人。";
		$NpcRandomDialog[410606001,1] = "你不应该为了自己的利益去伤害别人。";
		$NpcRandomDialog[410605001,1] = "请不要忘记我们来源于自然。";
		$NpcRandomDialog[401010001,1] = "请随时与大自然保持联系。";
		$NpcRandomDialog[401010002,1] = "不是我们支配自然，而是自然慷慨赐予我们力量。";
		$NpcRandomDialog[401010002,2] = "修炼是个漫长的过程，不要操之过急。";
		$NpcRandomDialog[401010006,1] = "门宗内事务繁多，经常出现人手不足的情况。";
		$NpcRandomDialog[401010005,1] = "若你为本宗做出贡献，此处的宝物便可为你所有。";
		$NpcRandomDialog[401010004,1] = "当你踏上坐骑，丛山峻岭便是泥丸土堆。";
		$NpcRandomDialog[401010003,1] = "欢迎来到双生山。";
		$NpcRandomDialog[410604001,1] = "你需要帮助吗？请别客气。";
		$NpcRandomDialog[410603001,1] = "看来，你是一个好对手。";
//蓬莱仙境
		$NpcRandomDialog[410307001,1] = "天外之景绝非凡人所想，天外之事也非凡人能涉。";
		$NpcRandomDialog[410307001,2] = "天地间的浩荡又有几人得见。";
		$NpcRandomDialog[410307001,3] = "事能知足心常惬，人到无求品自高。";
		$NpcRandomDialog[410306001,1] = "蓬莱漂以海外，若非本宗人接引，纵千万人不得寻。";
		$NpcRandomDialog[410305001,1] = "我思故我在，我观故自在.";
		$NpcRandomDialog[401007001,1] = "仙宗之术讲究御气，可达手中无兵心中万剑之境。";
		$NpcRandomDialog[401007002,1] = "仙宗御气，故内修尤为重要，行气固元乃术之本。";
		$NpcRandomDialog[401007006,1] = "门宗内事务繁多，经常出现人手不足的情况。";
		$NpcRandomDialog[401007005,1] = "若你为本宗做出贡献，此处的宝物便可为你所有。";
		$NpcRandomDialog[401007004,1] = "驰骋天地之间乃仙家必需之术，更乃凡人之梦亦。";
		$NpcRandomDialog[401007003,1] = "欢迎来到蓬莱仙宗。";
		$NpcRandomDialog[410304001,1] = "仙宗以气修剑，以气御剑，可击人与数丈外。";
		$NpcRandomDialog[410303001,1] = "蓬莱虽超脱尘世，但仙宗弟子为修神体常行于世。";
//昆仑古墟
		$NpcRandomDialog[410107001,1] = "昆仑之所以为圣，因其高于尘而行于尘，故凡人敬。";
		$NpcRandomDialog[410107001,2] = "圣者为天下之大任，除恶扬善为吾道，此为昆仑本。";
		$NpcRandomDialog[410107001,3] = "昆仑创派千年，臂上环蛟乃圣者之傲，上成者方得。";
		$NpcRandomDialog[410106001,1] = "昆仑有三十六天机，以凡人之力纵历经万难亦不至。";
		$NpcRandomDialog[410105001,1] = "昆仑之圣并非全使刃习武，凡间各种皆可为圣。";
		$NpcRandomDialog[401011001,1] = "仙法神力非一日能成，昆仑仙法重根基，功到自成。";
		$NpcRandomDialog[401011002,1] = "修神之路需内外兼修，我宗心法为先修外后修内。";
		$NpcRandomDialog[401011006,1] = "门宗内事务繁多，经常出现人手不足的情况。";
		$NpcRandomDialog[401011005,1] = "若你为本宗做出贡献，此处的宝物便可为你所有。";
		$NpcRandomDialog[401011004,1] = "昆仑的御术也为圣，天下无双。";
		$NpcRandomDialog[401011003,1] = "此为昆仑古墟，乃昆仑圣宗之地。";
		$NpcRandomDialog[410104001,1] = "昆仑历来以侠义闻名于世，经千年而不变。";
		$NpcRandomDialog[410103001,1] = "昆仑之圣自古就被凡人向往。";
//■■■■■NPC随机对话■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//■■■■■任务相关名称■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//任务类型名称
		$MissionKind[0,1] = "〖门宗任务〗";	$MissionKind[0,2] = "门宗";
		$MissionKind[1,1] = "〖章回任务〗";	$MissionKind[1,2] = "章回";
		$MissionKind[2,1] = "〖支线剧情〗";	$MissionKind[2,2] = "支线";
		$MissionKind[3,1] = "〖每日循环〗";	$MissionKind[3,2] = "每日";
		$MissionKind[4,1] = "〖帮会任务〗";	$MissionKind[4,2] = "帮会";
		$MissionKind[5,1] = "〖指引任务〗";	$MissionKind[5,2] = "指引";
		$MissionKind[6,1] = "〖活动任务〗";	$MissionKind[6,2] = "活动";
		$MissionKind[7,1] = "〖其它任务〗";	$MissionKind[7,2] = "其它";

//任务卷名称
//$ZhangHuiMission_Juan[200000] = "〖序：三十三天封神门〗";
//$ZhangHuiMission_Juan[200100] = "〖始卷：渺渺神踪何处寻〗";
//$ZhangHuiMission_Juan[200300] = "〖贰柬：犹问尘间多少事〗";
//$ZhangHuiMission_Juan[200500] = "〖叁卷：凤凰枝头凤凰瑶〗";
//$ZhangHuiMission_Juan[200700] = "〖肆卷：会挽雕弓射满月〗";
//$ZhangHuiMission_Juan[200900] = "〖伍卷：瘴云散尽水如天〗";
//$ZhangHuiMission_Juan[201100] = "〖陆卷：黑龙舞兮云飞扬〗";
//$ZhangHuiMission_Juan[201300] = "〖柒卷：神山此去无多路〗";
//$ZhangHuiMission_Juan[201500] = "〖〗";
//$ZhangHuiMission_Juan[201700] = "〖〗";
//$ZhangHuiMission_Juan[201900] = "〖〗";
//$ZhangHuiMission_Juan[202100] = "〖〗";
//
////任务章名称
//$ZhangHuiMission_Zhang[200000] = "〖单章：恍如一梦间〗";
//
//$ZhangHuiMission_Zhang[200101] = "〖初章：汇万灵之息〗";
//$ZhangHuiMission_Zhang[200102] = "〖贰章：群鬼乱清风〗";
//$ZhangHuiMission_Zhang[200103] = "〖叁章：灵兽饮神水〗";
//$ZhangHuiMission_Zhang[200111] = "〖附章一：碧波舞长枪〗";
//$ZhangHuiMission_Zhang[200112] = "〖附章二：此物最相思〗";
//$ZhangHuiMission_Zhang[200113] = "〖附章三：对君洗红妆〗";
//
//$ZhangHuiMission_Zhang[200301] = "〖初章：童子悲无踪〗";
//$ZhangHuiMission_Zhang[200302] = "〖贰章：使我泪幻花〗";
//$ZhangHuiMission_Zhang[200303] = "〖叁章：幻变可惊仙〗";
//$ZhangHuiMission_Zhang[200311] = "〖附章一：这把，那把〗";
//$ZhangHuiMission_Zhang[200312] = "〖附章二：极暴烈烹饪〗";
//$ZhangHuiMission_Zhang[200313] = "〖附章三：幽幽放逐渊〗";
//
//$ZhangHuiMission_Zhang[200501] = "〖初章：神炎烫琼浆〗";
//$ZhangHuiMission_Zhang[200502] = "〖贰章：沉醉不知归〗";
//$ZhangHuiMission_Zhang[200503] = "〖叁章：杳杳灵凤曲〗";
//$ZhangHuiMission_Zhang[200511] = "〖附章一：花下醉生死〗";
//$ZhangHuiMission_Zhang[200512] = "〖附章二：不共彩云飞〗";
//$ZhangHuiMission_Zhang[200513] = "〖附章三：黑岩镇魂调〗";
//
//$ZhangHuiMission_Zhang[200701] = "〖初章：山雨欲来时〗";
//$ZhangHuiMission_Zhang[200702] = "〖贰章：必逢风满楼〗";
//$ZhangHuiMission_Zhang[200703] = "〖叁章：无情潇潇雨〗";
//$ZhangHuiMission_Zhang[200711] = "〖附章一：忽闻有仙草〗";
//$ZhangHuiMission_Zhang[200712] = "〖附章二：百步穿杨箭〗";
//$ZhangHuiMission_Zhang[200713] = "〖附章三：吟歌愈伤哀〗";
//
//$ZhangHuiMission_Zhang[200901] = "〖初章：断肠巫蛊咒〗";
//$ZhangHuiMission_Zhang[200902] = "〖贰章：毒云缭绕升〗";
//$ZhangHuiMission_Zhang[200903] = "〖叁章：大泽水泱泱〗";
//$ZhangHuiMission_Zhang[200911] = "〖附章一：试药养蛊虫〗";
//$ZhangHuiMission_Zhang[200912] = "〖附章二：将以毒攻毒〗";
//$ZhangHuiMission_Zhang[200913] = "〖附章三：世事两茫茫〗";
//
//$ZhangHuiMission_Zhang[201101] = "〖初章：冲怒斩蛟鳄〗";
//$ZhangHuiMission_Zhang[201102] = "〖贰章：吞吐风云势〗";
//$ZhangHuiMission_Zhang[201103] = "〖叁章：惊涛卷苍龙〗";
//$ZhangHuiMission_Zhang[201111] = "〖附章一：曲尽河星稀〗";
//$ZhangHuiMission_Zhang[201112] = "〖附章二：斩罢溅灵虹〗";
//$ZhangHuiMission_Zhang[201113] = "〖附章三：踏入阴阳路〗";
//
//$ZhangHuiMission_Zhang[201301] = "〖初章：急雪舞回风〗";
//$ZhangHuiMission_Zhang[201302] = "〖贰章：柔雪可崩山〗";
//$ZhangHuiMission_Zhang[201303] = "〖叁章：古魂泪沾襟〗";
//$ZhangHuiMission_Zhang[201311] = "〖〗";
//$ZhangHuiMission_Zhang[201312] = "〖〗";
//$ZhangHuiMission_Zhang[201313] = "〖〗";

//■■■■■任务相关名称■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//■■■■■地图名称■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
		$Mission_Map[1001] = "万灵城";
		$Mission_Map[1002] = "月幽境";
		$Mission_Map[1003] = "神邸";
		$Mission_Map[1004] = "落夕渊";
		$Mission_Map[1005] = "神木林";
		$Mission_Map[1006] = "极乐西天";
		$Mission_Map[1007] = "蓬莱仙境";
		$Mission_Map[1008] = "醉梦冰池";
		$Mission_Map[1009] = "幽冥鬼域";
		$Mission_Map[1010] = "双生山";
		$Mission_Map[1011] = "昆仑古虚";
		$Mission_Map[1101] = "清风滨";
		$Mission_Map[1102] = "仙游岭";
		$Mission_Map[1103] = "无忧草野";
		$Mission_Map[1104] = "遮月森林";
		$Mission_Map[1105] = "祝祭大泽";
		$Mission_Map[1106] = "浴龙川";
		$Mission_Map[1107] = "载天雪原";
		$Mission_Map[1108] = "蛮荒大漠";
		$Mission_Map[1109] = "风神滩";
		$Mission_Map[1110] = "沧浪口";
		$Mission_Map[1111] = "梦境";
		$Mission_Map[1112] = "烈焰原";
		$Mission_Map[1113] = "北溟";
		$Mission_Map[1114] = "须弥灵境";
		$Mission_Map[1115] = "盘古之心";
		$Mission_Map[1116] = "三十三天门";
		$Mission_Map[1117] = "氐人巨海";
		$Mission_Map[1118] = "卷云高原";
		$Mission_Map[1119] = "伏羲秘境";
		$Mission_Map[1120] = "三辰池";
		$Mission_Map[1121] = "亘古禁土";
		$Mission_Map[1122] = "封神陵";
		$Mission_Map[1123] = "落夕之井";
		$Mission_Map[1124] = "灵谷";
		$Mission_Map[1125] = "摆摊地图";
		$Mission_Map[1126] = "惊仙窟";
		$Mission_Map[1127] = "三十三天门（序章）";
		$Mission_Map[1301] = "万书楼";
		$Mission_Map[1302] = "闹鬼的渔村（夜晚）";
		$Mission_Map[1303] = "十方游梦阵";
		$Mission_Map[1304] = "水墨古镇（七日前）";
		$Mission_Map[1305] = "试炼秘境";
		$Mission_Map[1401] = "黄泉道";
		$Mission_Map[1402] = "游乐场";
		$Mission_Map[1501] = "测试地图";

		$Mission_Map[1001001] = "万灵城";
		$Mission_Map[1101001] = "清风滨";
//■■■■■地图名称■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//■■■■■图标变量■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
		if(isFile("gameres/data/icon/function/accept.png"			)){$Icon[1 ] = "<i s='gameres/data/icon/function/accept.png' w='16' h='16'/> ";				}else{$Icon[1 ] = "";}//任务接受图标
		if(isFile("gameres/data/icon/function/finish.png"			)){$Icon[2 ] = "<i s='gameres/data/icon/function/finish.png' w='16' h='16'/> ";				}else{$Icon[2 ] = "";}//任务完成图标
		if(isFile("gameres/data/icon/function/business.png"		)){$Icon[3 ] = "<i s='gameres/data/icon/function/business.png' w='16' h='16'/> ";			}else{$Icon[3 ] = "";}//元宝商城图标
		if(isFile("gameres/data/icon/function/dialog.png"			)){$Icon[4 ] = "<i s='gameres/data/icon/function/dialog.png' w='16' h='16'/> ";				}else{$Icon[4 ] = "";}//对话图标
		if(isFile("gameres/data/icon/function/function.png"		)){$Icon[5 ] = "<i s='gameres/data/icon/function/function.png' w='16' h='16'/> ";			}else{$Icon[5 ] = "";}//功能图标
		if(isFile("gameres/data/icon/function/help.png"				)){$Icon[6 ] = "<i s='gameres/data/icon/function/help.png' w='16' h='16'/> ";					}else{$Icon[6 ] = "";}//帮助图标
		if(isFile("gameres/data/icon/function/repository.png"	)){$Icon[7 ] = "<i s='gameres/data/icon/function/repository.png' w='16' h='16'/> ";		}else{$Icon[7 ] = "";}//仓库图标
		if(isFile("gameres/data/icon/function/shop.png"				)){$Icon[8 ] = "<i s='gameres/data/icon/function/shop.png' w='16' h='16'/> ";					}else{$Icon[8 ] = "";}//商店图标
		if(isFile("gameres/data/icon/function/study.png"			)){$Icon[9 ] = "<i s='gameres/data/icon/function/study.png' w='16' h='16'/> ";				}else{$Icon[9 ] = "";}//学习图标
		if(isFile("gameres/data/icon/function/transport.png"	)){$Icon[10] = "<i s='gameres/data/icon/function/transport.png' w='16' h='16'/> ";		}else{$Icon[10] = "";}//传送图标
		if(isFile("gameres/data/icon/function/fly.png"	      )){$Icon[11] = "<i s='gameres/data/icon/function/fly.png' w='16' h='16'/> ";			    }else{$Icon[11] = "";}//飞行传送图标
//■■■■■图标变量■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

	}

//====================服务端=======================================================
if($Now_Script == 2)
	{
//■■■■■■■■■■■经验值公式相关■■■■■■■■■■■■■■■■■■■■■■
//等级削减经验值比例
		$Lv_Exp[11 ] = 0;
		$Lv_Exp[10 ] = 0.2;
		$Lv_Exp[9  ] = 0.3;
		$Lv_Exp[8  ] = 0.4;
		$Lv_Exp[7  ] = 0.5;
		$Lv_Exp[6  ] = 0.6;
		$Lv_Exp[5  ] = 0.7;
		$Lv_Exp[4  ] = 0.8;
		$Lv_Exp[3  ] = 0.9;
		$Lv_Exp[2  ] = 1;
		$Lv_Exp[1  ] = 1;
		$Lv_Exp[0  ] = 1;
		$Lv_Exp[-1 ] = 1;
		$Lv_Exp[-2 ] = 1;
		$Lv_Exp[-3 ] = 0.95;
		$Lv_Exp[-4 ] = 0.90;
		$Lv_Exp[-5 ] = 0.85;
		$Lv_Exp[-6 ] = 0.80;
		$Lv_Exp[-7 ] = 0.75;
		$Lv_Exp[-8 ] = 0.70;
		$Lv_Exp[-9 ] = 0.65;
		$Lv_Exp[-10] = 0.60;
		$Lv_Exp[-11] = 0.55;
		$Lv_Exp[-12] = 0.50;
		$Lv_Exp[-13] = 0.45;
		$Lv_Exp[-14] = 0.40;
		$Lv_Exp[-15] = 0.35;
		$Lv_Exp[-16] = 0;
//共享范围内的队友人数加成
		$SharePlayerNum[0] = 1;
		$SharePlayerNum[1] = 1.15;
		$SharePlayerNum[2] = 1.4;
		$SharePlayerNum[3] = 1.8;
		$SharePlayerNum[4] = 2.1;
		$SharePlayerNum[5] = 2.5;
//共享范围内的队友最大关系加成
		$MaxConnection[0] = 0;			//临时
		$MaxConnection[1] = 0.2;		//配偶
		$MaxConnection[2] = 0.1;		//兄弟
		$MaxConnection[3] = 0.15;		//师傅
		$MaxConnection[4] = 0.15;		//徒弟
		$MaxConnection[5] = 0.05;		//好友
		$MaxConnection[6] = 0;			//仇人
		$MaxConnection[7] = 0;			//屏蔽
//■■■■■■■■■■■经验值公式相关■■■■■■■■■■■■■■■■■■■■■■
//■■■■■■■■■■■区域传送变量记录■■■■■■■■■■■■■■■■■■■■■
//普通传送区域
		$Area[810010101] = 110101; //传送到清风滨
		$Area[810010102] = 110202; //传送到仙游岭
		$Area[810010107] = 100401; //万灵城传送到落夕渊
		$Area[810010108] = 100501; //万灵城传送到神木林
		$Area[810010109] = 100601; //万灵城传送到极乐西天
		$Area[810010110] = 100701; //万灵城传送到蓬莱仙境
		$Area[810010111] = 100801; //万灵城传送到醉梦冰池
		$Area[810010112] = 100901; //万灵城	传送到幽冥鬼域
		$Area[810010113] = 101001; //万灵城传送到双生山
		$Area[810010114] = 101101; //万灵城	传送到昆仑古墟
		$Area[810010115] = 112401; //万灵城	传送到灵谷
		$Area[810010117] = 112501; //传送至万灵贸易船港
		$Area[810010121] = 110105; //万灵城	杨镖头飞行传送到清风滨
		$Area[810010123] = 110106; //万灵城	任惊梦飞行传送到清风滨
		$Area[810010124] = 100203; //万灵城飞行传送到月幽境

		$Area[810020101] = 110301; //传送到无忧草野
		$Area[810020102] = 110401; //传送到遮月森林
		$Area[810020106] = 100103; //月幽境飞行传送到万灵城
		$Area[811020107] = 112601; //仙游岭传送至惊仙窟

		$Area[810040101] = 100118; //落夕渊送到万灵城
		$Area[810050101] = 100120; //神木林传送到万灵城
		$Area[810060101] = 100114; //极乐西天传送到万灵城
		$Area[810070101] = 100115; //蓬莱仙境传送到万灵城
		$Area[810080101] = 100117; //醉梦冰池传送到万灵城
		$Area[810090101] = 100111; //从幽冥鬼域传送到万灵城
		$Area[810100101] = 100116; //双生山传送到万灵城
		$Area[810110101] = 100112; //昆仑古墟传送到万灵城

		$Area[811250101] = 100113; //从万灵贸易船港传送到万灵城
		$Area[811260101] = 110205; //惊仙窟传送到仙游岭
		$Area[811270101] = 100121; //巨灵神飞行传送到万灵城

		$Area[811010101] = 100102; //万灵城传送到清风滨
		$Area[811010102] = 110203; //清风滨传送到仙游岭
		$Area[811010108] = 100122; //清风滨	夏梦狼飞行传送到万灵城

		$Area[811020101] = 100107; //仙游岭传送到万灵城
		$Area[811020102] = 110102; //仙游岭传送到清风滨

		$Area[811030101] = 100201; //无忧草野传送到万灵城
		$Area[811040101] = 100202; //从遮月森林传送到万灵城

		$Area[811040102] = 110402; //遮月森林	矿洞下方传送到上方
		$Area[811040103] = 110403; //遮月森林	矿洞上方传送到下方

		//飞行点区域
		$Area[810010120] = 10010101; 	//序章进入飞行触发区域
		$Area[810010122] = 10010103; 	//夏梦狼飞行触发区域
		$Area[811010109] = 11010102; 	//杨镖头飞行触发区域
		$Area[811010110] = 11010103; 	//任惊梦飞行触发区域
		$Area[810010125] = 10010006; 	//月幽境飞行触发区域
		$Area[810020105] = 10020001; 	//万灵城飞行触发区域

//■■■■■■■■■■■区域传送变量记录■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■地图传送变量■■■■■■■■■■■■■■■■■■■■■■■

		$TransportObject[100101] = "%Player.TransportObject( 0 , 1001 , -339.431 , -232.974 , 106.02   );";	//传送到万灵城-宠物师面前
		$TransportObject[100102] = "%Player.TransportObject( 0 , 1001 , -257.902 , -420.606 , 100.176  );";	//万灵城传送到清风滨
		$TransportObject[100103] = "%Player.TransportObject( 0 , 1001 , -131.197 , -319.748 , 114.938  );";	//月幽境传送至万灵城
		$TransportObject[100104] = "%Player.TransportObject( 0 , 1001 , -344.392 , -348.353 , 96.9999  );";	//传送到万灵城朱萱
//$TransportObject[100105] = "%Player.TransportObject( 0 , 1001 , -195.709 ,  224.599 , 121.915  );";	//传送到万灵城
		$TransportObject[100106] = "%Player.TransportObject( 0 , 1001 , -256.911 , -216.236 , 108.5    );";	//巨灵神处传送到万灵城
		$TransportObject[100107] = "%Player.TransportObject( 0 , 1001 , -390.166 , -322.307 , 100.176  );";	//仙游岭传送到万灵城

//$TransportObject[100108] = "%Player.TransportObject( 0 , 1001 , -234.777 , -243.625 , 106.111  );";	//传送到万灵城

		$TransportObject[100109] = "%Player.TransportObject( 0 , 1001 , -257.128 , -260.645 , 106.885  );";	//女娲试炼传送到万灵城
		$TransportObject[100110] = "%Player.TransportObject( 0 , 1001 , -234.777 , -243.625 , 106.111	 );";	//从遮月森林传送到万灵城

		$TransportObject[100111] = "%Player.TransportObject( 0 , 1001 ,-266.947  , -214.493 , 107.703	 );";	//从幽冥鬼域传送到万灵城
		$TransportObject[100112] = "%Player.TransportObject( 0 , 1001 ,-251.26   , -218.983 , 107.703  );";	//从昆仑古墟传送到万灵城
		$TransportObject[100113] = "%Player.TransportObject( 0 , 1001 ,-311.353  , -180.3   , 100.676  );";	//从万灵贸易船港传送到万灵城
		$TransportObject[100114] = "%Player.TransportObject( 0 , 1001 ,-251.172  , -198.819 , 107.703  );";  //极乐西天传送到万灵城
		$TransportObject[100115] = "%Player.TransportObject( 0 , 1001 ,-262.754  , -198.803 , 107.703  );";  //蓬莱仙境传送到万灵城
		$TransportObject[100116] = "%Player.TransportObject( 0 , 1001 ,-262.776  , -218.694 , 107.703  );";  //双生山传送到万灵城
		$TransportObject[100117] = "%Player.TransportObject( 0 , 1001 ,-268.533  , -208.77  , 107.703  );";  //醉梦冰池传送到万灵城
		$TransportObject[100118] = "%Player.TransportObject( 0 , 1001 ,-247.005  , -214.568 , 107.703  );";  //落夕渊传送到万灵城
		$TransportObject[100119] = "%Player.TransportObject( 0 , 1001 ,-257.068  , -253.588 , 106.906  );";  //风阳子传送到体魄的试炼
		$TransportObject[100120] = "%Player.TransportObject( 0 , 1001 ,-245.215  , -208.82  , 107.703  );";  //神木林传送到万灵城

	  $TransportObject[100121] = "%Player.TransportObject( 0 , 1001 ,-272.538  , -264.3   , 127.359	 );";  //巨灵神飞行传送到万灵城
	  $TransportObject[100122] = "%Player.TransportObject( 0 , 1001 ,-348.585  , -362.045 , 113.43   );";  //清风滨	夏梦狼飞行传送到万灵城

		$TransportObject[100201] = "%Player.TransportObject( 0 , 1002 ,-179.488  , -31.736  , 100.422	 );";	//从无忧草野传送到月幽境
	  $TransportObject[100202] = "%Player.TransportObject( 0 , 1002 ,	6.9472   ,  190.338 , 100.784  );";	//从遮月森林传送到月幽境
	  $TransportObject[100203] = "%Player.TransportObject( 0 , 1002 ,-97.8223  , -9.74629 , 115.975  );";	//从万灵城传送到月幽境

	  $TransportObject[100401] = "%Player.TransportObject( 0 , 1004 ,	-11.6215 , -29.3318 , 169.367  );"; 	//万灵城传送到落夕渊
	  $TransportObject[100501] = "%Player.TransportObject( 0 , 1005 ,	-69.3645 , -39.5686 , 119.029  );"; 	//万灵城传送到神木林
	  $TransportObject[100601] = "%Player.TransportObject( 0 , 1006 ,	139.242  ,  86.0577 , 94.4966  );"; 	//万灵城传送到极乐西天
	  $TransportObject[100701] = "%Player.TransportObject( 0 , 1007 ,	-15.0216 ,  84.0957 , 132.972  );"; 	//万灵城传送到蓬莱仙境
		$TransportObject[100901] = "%Player.TransportObject( 0 , 1009 , 0.918146 , -87.0586 , 139.985  );";	//万灵城传送到幽冥鬼域
		$TransportObject[100801] = "%Player.TransportObject( 0 , 1008 , 5.53776 ,  -98.756  , 139.832	 );";	//万灵城传送到醉梦冰池
		$TransportObject[101001] = "%Player.TransportObject( 0 , 1010 , 18.5082  , -102.339 , 137.676  );";  //万灵城	传送到双生山
		$TransportObject[101101] = "%Player.TransportObject( 0 , 1011 ,  109.257 , -34.8149 , 117.345  );";	//万灵城传送至昆仑古墟

		$TransportObject[110101] = "%Player.TransportObject( 0 , 1101 , -162.959 , -156.327 , 40.2415  );";	//万灵城传送到清风滨
		$TransportObject[110102] = "%Player.TransportObject( 0 , 1101 , -355.001 , -240.491 , 15.038   );";	//仙游岭传送到清风滨
		$TransportObject[110103] = "%Player.TransportObject( 0 , 1101 , -113.814 , -196.221 , 50.0018  );";	//传送到清风滨
		$TransportObject[110104] = "%Player.TransportObject( 0 , 1101 , -267.849 , -113.659 , 50.02    );";	//传送到清风滨夏梦狼

		$TransportObject[110105] = "%Player.TransportObject( 0 , 1101 , -236.403 , -220.304 , 50.0179  );";	//万灵城	杨镖头飞行传送到清风滨
		$TransportObject[110106] = "%Player.TransportObject( 0 , 1101 , -229.657 , -134.439 , 63.9063  );";	//万灵城	任惊梦飞行传送到清风滨

//$TransportObject[110201] = "%Player.TransportObject( 0 , 1102 ,  263.178 ,  140.918 , 100.02   );";	//传送到测试仙游岭
		$TransportObject[110202] = "%Player.TransportObject( 0 , 1102 ,  160.705 ,  57.6988 ,  72.1083 );";	//传送到测试仙游岭
		$TransportObject[110203] = "%Player.TransportObject( 0 , 1102 ,  170.338 , -135.698 , 95.4025  );";	//清风滨传送到仙游岭
	  $TransportObject[110204] = "%Player.TransportObject( 0 , 1102 , -22.0096 , -6.5094  , 	89.586 );";	//传送至水墨洞天顶
	  $TransportObject[110205] = "%Player.TransportObject( 0 , 1102 ,  85.2432 ,  85.1601 , 72.96	   );";	//惊仙窟传送至仙游岭
	  $TransportObject[110206] = "%Player.TransportObject( 0 , 1102 ,  59.2682 ,  29.8064 , 72.832   );";	//水墨古镇（七日前）传送至仙游岭

		$TransportObject[110301] = "%Player.TransportObject( 0 , 1103 , 137.448  , -86.5564 , 71.1643  );";	//传送到无忧草野
		$TransportObject[110302] = "%Player.TransportObject( 0 , 1103 , 43.6255  , -26.6551 , 79.8202  );";	//东边下方传送到上方
		$TransportObject[110303] = "%Player.TransportObject( 0 , 1103 , 54.1369  , -54.7871 , 73.026   );";	//东边上方传送到下方
		$TransportObject[110304] = "%Player.TransportObject( 0 , 1103 , -154.427 , -44.2454 , 84.1124  );";	//西边下方传送到上方
		$TransportObject[110305] = "%Player.TransportObject( 0 , 1103 , -152.985 , -77.297  , 71.7338  );";	//西边上方传送到下方
	  $TransportObject[110306] = "%Player.TransportObject( 0 , 1103 , -36.8976 , -110.87  , 71.8023  );";	//醉花荫传送到般若虚土
	  $TransportObject[110307] = "%Player.TransportObject( 0 , 1103 , -59.0003 , 89.6427  , 99.0842  );";	//般若虚土传送到醉花荫
	  $TransportObject[110308] = "%Player.TransportObject( 0 , 1103 ,	 8.46981 , -40.756  , 73.8298  );";	//试炼迷境传送到无忧草野

		$TransportObject[110401] = "%Player.TransportObject( 0 , 1104 , -116.657 , -138.661 , 103.381  );";	//传送到遮月森林
		$TransportObject[110402] = "%Player.TransportObject( 0 , 1104 , -128.492 , 48.7926  , 125.024  );";	//遮月森林	矿洞下方传送到上方
		$TransportObject[110403] = "%Player.TransportObject( 0 , 1104 , -82.4807 , 60.1497  , 111.02   );";	//遮月森林	矿洞上方传送到下方

	  $TransportObject[112601] = "%Player.TransportObject( 0 , 1126 ,  -98.0831  , 28.1368 , 99.1704 );";	//仙游岭传送至惊仙窟

	  $TransportObject[112701] = "%Player.TransportObject( 0 , 1127 ,  92.0594  , 99.5996 , 128.688  );";	//守护巨像传送至巨灵神

		$TransportObject[112401] = "%Player.TransportObject( 0 , 1124 ,  21.514  , -24.7174 , 101.614  );";	//传送到灵谷-抓宠物地图

//$TransportObject[140101] = "%Player.TransportObject( 0 , 1401 , -314.917 ,  296.979 , 105.188  );";	//传送到测试黄泉
		$TransportObject[140102] = "%Player.TransportObject( 0 , 1401 , 8.2923   , -87.8359 , 103.505  );";	//传送至黄泉道

		$TransportObject[112501] = "%Player.TransportObject( 0 , 1125 , 23.2288  , 5.96536  , 100.02	 );";	//万灵城	传送至万灵贸易船港

//副本暂时用传送
		$TransportObject[130101] = "%Player.TransportObject( 0 , 1301 , -0.521185 , -3.28804  , 133.91 );";	//传送到万书楼
		$TransportObject[130201] = "%Player.TransportObject( 0 , 1302 , -290.566 , -230.297  , 14.3938 );";	//传送到测试闹鬼的渔村（夜晚）
		$TransportObject[130301] = "%Player.TransportObject( 0 , 1303 , 7.35208  , 15.3407   , 133.334 );";	//传送至女娲试炼
	  $TransportObject[130401] = "%Player.TransportObject( 0 , 1304 , 61.7801  , -15.7809  , 76.6993 );";	//仙游岭传送至水墨古镇（七日前）
	  $TransportObject[130501] = "%Player.TransportObject( 0 , 1305 , 0        ,      0    , 111.027 );";	//无忧草野传送至试炼迷境

//副本传送
		$TransportObject[1301] = "-0.521185 -3.28804 133.91";	//传送到万书楼
		$TransportObject[1302] = "-290.566 -230.297 14.3938";	//传送到测试闹鬼的渔村（夜晚）
		$TransportObject[1303] = "7.35208 15.3407 133.334";		//传送至女娲试炼
		$TransportObject[1304] = "61.7801 -15.7809 76.6993";	//仙游岭传送至水墨古镇（七日前）
		$TransportObject[1305] = "0 0 111.027";								//无忧草野传送至试炼迷境

//■■■■■■■■■■■地图传送变量■■■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■道具鉴定符变量■■■■■■■■■■■■■■■■■■■■■■
		$JianDingFu[113050001] = "20 0 0";
		$JianDingFu[113050002] = "20 90001 0";
		$JianDingFu[113050003] = "20 99001 0";
		$JianDingFu[113050004] = "20 99901 0";
		$JianDingFu[113050005] = "20 99991 0";
		$JianDingFu[113050006] = "20 100000 0";
		$JianDingFu[113050007] = "30 0 0";
		$JianDingFu[113050008] = "30 90001 0";
		$JianDingFu[113050009] = "30 99001 0";
		$JianDingFu[113050010] = "30 99901 0";
		$JianDingFu[113050011] = "30 99991 0";
		$JianDingFu[113050012] = "30 100000 0";
		$JianDingFu[113050013] = "40 0 0";
		$JianDingFu[113050014] = "40 90001 0";
		$JianDingFu[113050015] = "40 99001 0";
		$JianDingFu[113050016] = "40 99901 0";
		$JianDingFu[113050017] = "40 99991 0";
		$JianDingFu[113050018] = "40 100000 0";
		$JianDingFu[113050019] = "50 0 0";
		$JianDingFu[113050020] = "50 90001 0";
		$JianDingFu[113050021] = "50 99001 0";
		$JianDingFu[113050022] = "50 99901 0";
		$JianDingFu[113050023] = "50 99991 0";
		$JianDingFu[113050024] = "50 100000 0";
		$JianDingFu[113050025] = "60 0 0";
		$JianDingFu[113050026] = "60 90001 0";
		$JianDingFu[113050027] = "60 99001 0";
		$JianDingFu[113050028] = "60 99901 0";
		$JianDingFu[113050029] = "60 99991 0";
		$JianDingFu[113050030] = "60 100000 0";
		$JianDingFu[113050031] = "70 0 0";
		$JianDingFu[113050032] = "70 90001 0";
		$JianDingFu[113050033] = "70 99001 0";
		$JianDingFu[113050034] = "70 99901 0";
		$JianDingFu[113050035] = "70 99991 0";
		$JianDingFu[113050036] = "70 100000 0";
		$JianDingFu[113050037] = "80 0 0";
		$JianDingFu[113050038] = "80 90001 0";
		$JianDingFu[113050039] = "80 99001 0";
		$JianDingFu[113050040] = "80 99901 0";
		$JianDingFu[113050041] = "80 99991 0";
		$JianDingFu[113050042] = "80 100000 0";
		$JianDingFu[113050043] = "90 0 0";
		$JianDingFu[113050044] = "90 90001 0";
		$JianDingFu[113050045] = "90 99001 0";
		$JianDingFu[113050046] = "90 99901 0";
		$JianDingFu[113050047] = "90 99991 0";
		$JianDingFu[113050048] = "90 100000 0";
		$JianDingFu[113050049] = "100 0 0";
		$JianDingFu[113050050] = "100 90001 0";
		$JianDingFu[113050051] = "100 99001 0";
		$JianDingFu[113050052] = "100 99901 0";
		$JianDingFu[113050053] = "100 99991 0";
		$JianDingFu[113050054] = "100 100000 0";
//■■■■■■■■■■■道具鉴定符变量■■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■活动相关变量■■■■■■■■■■■■■■■■■■■■■■■
//【参悟天书】采集物坐标
		$HuoDon_CWTS[1004,1] = 70.0177	;$HuoDon_CWTS[1004,2] = 60.0154	;$HuoDon_CWTS[1004,3] = 169.06 ;		//落夕渊  
		$HuoDon_CWTS[1005,1] = -2.20368 ;$HuoDon_CWTS[1005,2] = 6.38678	;$HuoDon_CWTS[1005,3] = 115.556;  	//神木林  
		$HuoDon_CWTS[1006,1] = -50.2518 ;$HuoDon_CWTS[1006,2] = 22.7328	;$HuoDon_CWTS[1006,3] = 104.36 ;  	//极乐西天
		$HuoDon_CWTS[1007,1] = -98.5129 ;$HuoDon_CWTS[1007,2] = -16.972	;$HuoDon_CWTS[1007,3] = 151.647;  	//蓬莱仙境
		$HuoDon_CWTS[1008,1] = 5.06524	;$HuoDon_CWTS[1008,2] = -3.7523	;$HuoDon_CWTS[1008,3] = 147.325;  	//醉梦冰池
		$HuoDon_CWTS[1009,1] = 0.431608 ;$HuoDon_CWTS[1009,2] = 120.658	;$HuoDon_CWTS[1009,3] = 156.403;  	//幽冥鬼域
		$HuoDon_CWTS[1010,1] = 19.2876	;$HuoDon_CWTS[1010,2] = 98.4331	;$HuoDon_CWTS[1010,3] = 141.995;  	//双生山  
		$HuoDon_CWTS[1011,1] = 17.6303	;$HuoDon_CWTS[1011,2] = -29.0727;$HuoDon_CWTS[1011,3] = 123.072;  	//昆仑古墟
//■■■■■■■■■■■活动相关变量■■■■■■■■■■■■■■■■■■■■■■■


	}
//===============================服务端与客户端共用的===============================

//■■■■■■■■■■■全局的特效编号■■■■■■■■■■■■■■■■■■■■■
$SP_Effect[1] = 660005;	//玩家接受任务特效
$SP_Effect[2] = 660004;	//玩家交付任务特效
$SP_Effect[3] = 660005;	//NPC有可接受任务特效
$SP_Effect[4] = 660004;	//NPC有可交付任务特效
$SP_Effect[5] = 660007;	//玩家升级特效
//■■■■■■■■■■■全局的特效编号■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■任务最大数量■■■■■■■■■■■■■■■■■■■■■■
$MissionCount = 0;
//■■■■■■■■■■■任务最大数量■■■■■■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■获取门宗名称与地图名字■■■■■■■■■■■■■■■■■
$FamilyName[0,1] = "任我行";					$FamilyName[0,2] = "任我行";   	$FamilyName[0,3] = 0000;
$FamilyName[1,1] = "昆仑宗(圣)";			$FamilyName[1,2] = "昆仑古虚";  $FamilyName[1,3] = 1011;
$FamilyName[2,1] = "金禅寺(佛)";			$FamilyName[2,2] = "极乐西天";  $FamilyName[2,3] = 1006;
$FamilyName[3,1] = "蓬莱派(仙)";			$FamilyName[3,2] = "蓬莱仙境";  $FamilyName[3,3] = 1007;
$FamilyName[4,1] = "飞花谷(精)";			$FamilyName[4,2] = "神木林";    $FamilyName[4,3] = 1005;
$FamilyName[5,1] = "九幽教(鬼)";			$FamilyName[5,2] = "幽冥鬼域";  $FamilyName[5,3] = 1009;
$FamilyName[6,1] = "山海宗(怪)";			$FamilyName[6,2] = "双生山";    $FamilyName[6,3] = 1010;
$FamilyName[7,1] = "幻灵宫(妖)";			$FamilyName[7,2] = "醉梦冰池";  $FamilyName[7,3] = 1008;
$FamilyName[8,1] = "天魔门(魔)";			$FamilyName[8,2] = "落夕渊";    $FamilyName[8,3] = 1004;
//■■■■■■■■■■■获取门宗名称与地图名字■■■■■■■■■■■■■■■■■

//■■■■■■■■■■■道具是哪个怪物掉落数据■■■■■■■■■■■■■■■■■
//==========================================道具是哪个怪物掉落数据=========================================================================
//填写规则：

//例子：$Item_Monster[108020051] = "410700059";
//============↑=========↑===========↑============
//=========全局变量====道具编号====怪物编号：如有多个采用空格隔开=====
//新手指引

$Item_Monster[108020273] = "400001007";		//被诅咒的灵狐封印
$Item_Monster[107019001] = "400001011";		//耕药镰刀
$Item_Monster[108020270] = "552000055";		//装满灵力的葫芦
$Item_Monster[108020271] = "552000055";		//日月灵气
$Item_Monster[108020267] = "552000054";		//仙鹤草
$Item_Monster[108020258] = "811010107";		//画像
$Item_Monster[108020259] = "810010118";		//仙粉


//第一卷
$Item_Monster[108020051] = "410700059";		//黄鼬的尾巴
$Item_Monster[108020053] = "410700061";		//啃草大王的牙齿
$Item_Monster[108020054] = "550000002";		//被污染的木桩
$Item_Monster[108020055] = "550000003";		//被污染的花朵
$Item_Monster[108020056] = "410700077";		//偷走的粮食
$Item_Monster[108020057] = "410700081";		//大黄的内丹
$Item_Monster[108020058] = "410700082";		//二黄的内丹
$Item_Monster[108020059] = "410700083";		//三黄的内丹
$Item_Monster[108020061] = "550000004";		//新鲜的木头
$Item_Monster[108020062] = "550000005";		//稻草
$Item_Monster[108020063] = "410700086";		//灵螺黏液
$Item_Monster[108020065] = "410500064";		//东屋小鬼头颅
$Item_Monster[108020066] = "410500065";		//西屋小鬼头颅
$Item_Monster[108020067] = "410500066";		//邪鬼小王头颅
$Item_Monster[108020069] = "410700091";		//尖刺螺壳
$Item_Monster[108020070] = "410700092";		//青色虾钳
$Item_Monster[108020072] = "811010104";		//第一道地缝
$Item_Monster[108020073] = "811010105";		//第二道地缝
$Item_Monster[108020074] = "811010106";		//第三道地缝
$Item_Monster[108020075] = "410700093";		//玉珊瑚
$Item_Monster[108020076] = "410700094";		//六彩绸缎
$Item_Monster[108020077] = "410700093";		//潮汐鼓
//第一卷支线
$Item_Monster[108020250] = "810010118";		//烈焰草
$Item_Monster[108020254] = "410700100";		//巨钳
$Item_Monster[108020256] = "410700106";		//深海珍珠粉
//第二卷
$Item_Monster[108020091] = "411101001";		//蝮蛇肉
$Item_Monster[108020092] = "411101002";		//乌蛇肉
$Item_Monster[108020093] = "401102121";		//巨大的蛇蜕
$Item_Monster[108020094] = "411101003";		//泽蛇胆
$Item_Monster[108020098] = "411101006";		//血尾的尾巴
$Item_Monster[108020099] = "552000005";		//散落的狐狸毛皮
$Item_Monster[108020100] = "411101007";		//白狐内丹
$Item_Monster[108020102] = "811020103";		//雪牙内丹
$Item_Monster[108020103] = "410701002";		//木发钗
$Item_Monster[108020105] = "811020104";		//陈旧的盒子
$Item_Monster[108020106] = "811020105";		//陈旧的盒子
$Item_Monster[108020109] = "410701011";		//浓烈的妖气
$Item_Monster[108020110] = "552000006";		//雾叶草
$Item_Monster[108020111] = "410701009";		//破损妖狐毛皮
$Item_Monster[108020113] = "410701007";		//巨大狐皮
$Item_Monster[108020117] = "410701008";		//牢笼钥匙
$Item_Monster[108020118] = "401102109";		//双龙戏珠
//第三卷
$Item_Monster[108020122] = "411101012";		//蚊子的针刺
$Item_Monster[108020123] = "411101013";		//斑斓的蚊足
$Item_Monster[108020125] = "411101014";		//荧光之翅
$Item_Monster[108020126] = "552000008";		//珠花
$Item_Monster[108020121] = "552000007";		//潦草的手稿
$Item_Monster[108020127] = "410701034";		//绿顶珠
$Item_Monster[108020128] = "552000009";		//柔韧草茎
$Item_Monster[108020129] = "552000010";		//鲜艳的碧珠花
$Item_Monster[108020134] = "552000011";		//珠果
$Item_Monster[108020135] = "410701034";		//碧色血精
$Item_Monster[108020136] = "410701035";		//青色血精
$Item_Monster[108020137] = "410701036";		//草色血精
$Item_Monster[108020151] = "552000014";		//柳橙
$Item_Monster[108020152] = "552000015";		//青果
$Item_Monster[108020153] = "410400007";		//花粉
$Item_Monster[108020163] = "410400013";		//花盘
$Item_Monster[108020164] = "410400015";		//花冠
$Item_Monster[108020158] = "401103100";		//空中火
$Item_Monster[108020166] = "552000016";		//佛像碎片
$Item_Monster[108020144] = "552000012";		//八荒剑气卷
$Item_Monster[108020142] = "552000012";		//八荒剑气卷（章一）
$Item_Monster[108020143] = "552000012";		//八荒剑气卷（章二）
$Item_Monster[108020147] = "552000013";		//映月辉星卷
$Item_Monster[108020145] = "552000013";		//映月辉星卷（章一）
$Item_Monster[108020146] = "552000013";		//映月辉星卷（章二）
$Item_Monster[108020150] = "410200002";		//两情依依卷
$Item_Monster[108020148] = "410200002";		//两情依依卷（章一）
$Item_Monster[108020149] = "410200002";		//两情依依卷（章二）
$Item_Monster[108020159] = "410600003";		//精灵项圈
$Item_Monster[108020165] = "410400017";   //花语牢笼钥匙
$Item_Monster[108020168] = "401103100";   //空中火
$Item_Monster[108020169] = "401103101";   //石中火
//第四卷
$Item_Monster[108020173] = "401002010";   //陈年老酒
$Item_Monster[108020174] = "401002106";   //发黑的枝桠
$Item_Monster[108020175] = "810020104";   //镶玉空杯
$Item_Monster[108020176] = "810020104";   //装满水的杯子
$Item_Monster[108020177] = "410400021";   //树枝的残本
$Item_Monster[108020178] = "410400023";   //树枝的末梢
$Item_Monster[108020179] = "552000017";   //腐化光菇
$Item_Monster[108020181] = "410701045";   //映月兔心
$Item_Monster[108020207] = "552000018";   //发光的树皮
$Item_Monster[108020183] = "552000019";   //雨滴水晶
$Item_Monster[108020185] = "410400029";   //树精王之根
$Item_Monster[108020186] = "410400035";   //树精根须
$Item_Monster[108020187] = "410400037";   //赝月花瓣
$Item_Monster[108020188] = "410400039";   //月菇伞
$Item_Monster[108020189] = "552000020";   //夜来香种子
$Item_Monster[108020190] = "410400043";   //夜来香花蕊
$Item_Monster[108020191] = "552000021";   //火药桶
$Item_Monster[108020197] = "410701047";   //遮月兔油
$Item_Monster[108020199] = "410600011";   //后羿之力
$Item_Monster[108020201] = "410600017";   //暴躁能量
$Item_Monster[108020203] = "410400057";   //根须能量
$Item_Monster[108020205] = "410400059";   //花瓣能量
//第五卷
$Item_Monster[108020209] = "410600032";   //黑色毒腺
$Item_Monster[108020210] = "410600034";   //染疫毒液
$Item_Monster[108020214] = "411101020";   //古怪令牌
$Item_Monster[108020215] = "411101022";   //鳄鱼牙齿
$Item_Monster[108020218] = "411101024";   //搏动的心脏
$Item_Monster[108020219] = "411200005";   //邪巫面具
$Item_Monster[108020226] = "411200007";   //焦黄头颅
$Item_Monster[108020229] = "411101028";   //癞蟾毒腺
$Item_Monster[108020230] = "411101030";   //沼黄鳄胆
$Item_Monster[108020237] = "411101032";   //鳄鱼鳞


//生活技能指引任务
$Item_Monster[106050101] = "551030101";		//
$Item_Monster[106060101] = "551060101";		//
$Item_Monster[106010101] = "551050101";		//
$Item_Monster[106030101] = "551040101";		//
$Item_Monster[106040101] = "551020101";		//
$Item_Monster[106020101] = "551010101";		//

//$Item_Monster[] = "";		//
//$Item_Monster[] = "";		//
//$Item_Monster[] = "";		//
//$Item_Monster[] = "";		//

//■■■■■■■■■■■道具是哪个怪物掉落数据■■■■■■■■■■■■■■■■■


//■■■■■■■■■■■各个礼包类型的道具数据■■■■■■■■■■■■■■■■■
//==========================================各个礼包类型的道具数据=========================================================================
//填写规则：

//$Item_Bao_105100001 = "6 1";
//各个参数以空格隔开：不需要的参数可以不填
//参数1代表此礼包内有道具的总数量
//参数2代表此礼包的类型：
//		1给所有道具
//		2随机获得某个道具
//		3选择其中某个道具
//		4根据某个条件获得其中某个道具
//参数3，依据参数2做调整，参数2为2.3类型时，参数3代表随机或选择获取道具的数量(不填默认为1)，参数2为4类型时，参数3代表条件值：1依据门宗值，2依据等级，3....
//参数4，依据参数3做调整，当参数3为条件值时，参数4代表获取道具的数量

//$Item_Bao_105100001[1] = "105070001 1";	//礼包内道具以数组方式添加，参数1为道具编号，参数2为道具数量
//$Item_Bao_105100001[2] = "105020001 2";
//$Item_Bao_105100001[3] = "105020002 2";
//$Item_Bao_105100001[4] = "105010001 5";
//$Item_Bao_105100001[5] = "105010002 5";
//$Item_Bao_105100001[6] = "105100002 1";

$Item_Bao[105100104] = "1 1";			//第一份惊喜礼包
$Item_Bao[105100104,1] = "105029003	5";

$Item_Bao[105100105] = "1 1";			//第二份惊喜礼包
$Item_Bao[105100105,1] = "116010001	5";

$Item_Bao[105100106] = "1 1";			//第三份惊喜礼包
$Item_Bao[105100106,1] = "102028025	1";

$Item_Bao[105100107] = "1 1";			//第四份惊喜礼包
$Item_Bao[105100107,1] = "119019009	1";

$Item_Bao[105100001] = "5 1";			//1级包裹
$Item_Bao[105100001,1] = "105020001 2";
$Item_Bao[105100001,2] = "105020002 2";
$Item_Bao[105100001,3] = "105010001 5";
$Item_Bao[105100001,4] = "105010002 5";
$Item_Bao[105100001,5] = "105100002 1";

$Item_Bao[105100002] = "4 1";			//10级包裹
$Item_Bao[105100002,1] = "105028001 1";
$Item_Bao[105100002,2] = "105030001 5";
$Item_Bao[105100002,3] = "103018000 1";
$Item_Bao[105100002,4] = "105100003 1";
//$Item_Bao[105100002,5] = "113050002 5";	//鉴定符

$Item_Bao[105100003] = "7 1";			//20级包裹
$Item_Bao[105100003,1] = "105028001 1";
$Item_Bao[105100003,2] = "105021001 2";
$Item_Bao[105100003,3] = "105021002 2";
$Item_Bao[105100003,4] = "116010001 10";
$Item_Bao[105100003,5] = "105100004 1";
$Item_Bao[105100004,6] = "103021801 1";
$Item_Bao[105100004,7] = "103031801 1";

$Item_Bao[105100004] = "6 1";			//30级包裹
$Item_Bao[105100004,1] = "105028001 1";
$Item_Bao[105100004,2] = "105021001 2";
$Item_Bao[105100004,3] = "105021002 2";
$Item_Bao[105100004,4] = "105022001 1";
$Item_Bao[105100004,5] = "105102013 1";
$Item_Bao[105100004,6] = "105100005 1";

$Item_Bao[105100005] = "4 1";			//40级包裹
$Item_Bao[105100005,1] = "105028001 1";
$Item_Bao[105100005,2] = "105021003 1";
$Item_Bao[105100005,3] = "105021004 1";
$Item_Bao[105100005,4] = "105100006 1";

$Item_Bao[105100006] = "4 1";			//50级包裹
$Item_Bao[105100006,1] = "105028001 1";
$Item_Bao[105100006,2] = "105021003 1";
$Item_Bao[105100006,3] = "105021004 1";
$Item_Bao[105100006,4] = "105100007 1";

$Item_Bao[105100007] = "3 1";			//60级包裹
$Item_Bao[105100007,1] = "105028001 1";
$Item_Bao[105100007,2] = "105021003 1";
$Item_Bao[105100007,3] = "105021004 1";

$Item_Bao[105100101] = "7 3";			//1级武器包裹
$Item_Bao[105100101,1] = "101010800 1";
$Item_Bao[105100101,2] = "101020800 1";
$Item_Bao[105100101,3] = "101030800 1";
$Item_Bao[105100101,4] = "101040800 1";
$Item_Bao[105100101,5] = "101050800 1";
$Item_Bao[105100101,6] = "101060800 1";
$Item_Bao[105100101,7] = "101070800 1";

$Item_Bao[105100102] = "14 4 1";			//门宗礼包
$Item_Bao[105100102,1,1] = "102080011 1 113020001 1 118019001 1";//圣男
$Item_Bao[105100102,1,2] = "102080021 1 113020001 1 118019001 1";//圣女
$Item_Bao[105100102,2,1] = "102080012 1 113020002 1 118019002 1";//佛男
$Item_Bao[105100102,3,1] = "102080013 1 113020003 1 118019003 1";//男仙
$Item_Bao[105100102,3,2] = "102080023 1 113020003 1 118019003 1";//女仙
$Item_Bao[105100102,4,2] = "102080024 1 113020004 1 118019004 1";//女精
$Item_Bao[105100102,5,1] = "102080015 1 113020005 1 118019005 1";//男鬼
$Item_Bao[105100102,5,2] = "102080025 1 113020005 1 118019005 1";//女鬼
$Item_Bao[105100102,6,1] = "102080016 1 113020006 1 118019006 1";//男怪
$Item_Bao[105100102,6,2] = "102080026 1 113020006 1 118019006 1";//女怪
$Item_Bao[105100102,7,1] = "102080017 1 113020007 1 118019007 1";//男妖
$Item_Bao[105100102,7,2] = "102080027 1 113020007 1 118019007 1";//女妖
$Item_Bao[105100102,8,1] = "102080018 1 113020008 1 118019008 1";//男魔
$Item_Bao[105100102,8,2] = "102080028 1 113020008 1 118019008 1";//女魔

$Item_Bao[105100103] = "7 3";			//10级武器包裹
$Item_Bao[105100103,1] = "101010801 1";
$Item_Bao[105100103,2] = "101020801 1";
$Item_Bao[105100103,3] = "101030801 1";
$Item_Bao[105100103,4] = "101040801 1";
$Item_Bao[105100103,5] = "101050801 1";
$Item_Bao[105100103,6] = "101060801 1";
$Item_Bao[105100103,7] = "101070801 1";


//■■■■■■■■■■■各个礼包类型的道具数据■■■■■■■■■■■■■■■■■



//■■■■■■■■■■■各个采集物给于的物品■■■■■■■■■■■■■■■■■■
//==========================================各个采集物给于的物品=========================================================================
$Item_CaiJi[551010101] = "106020101 90 106020201 9 106020301 1";	//铜矿	矿石半成品1级	90%	铜矿石	9%	矿石伴生物1级	1%
$Item_CaiJi[551010201] = "106020102 90 106020202 9 106020302 1";	//铁矿	矿石半成品2级	90%	铁矿石	9%	矿石伴生物2级	1%
$Item_CaiJi[551010301] = "106020103 90 106020203 9 106020303 1";	//银矿	矿石半成品3级	90%	银矿石	9%	矿石伴生物3级	1%
$Item_CaiJi[551010401] = "106020104 90 106020204 9 106020304 1";	//金矿	矿石半成品4级	90%	金矿石	9%	矿石伴生物4级	1%
$Item_CaiJi[551010501] = "106020105 90 106020205 9 106020305 1";	//精晶矿	矿石半成品5级	90%	精晶矿石	9%	矿石伴生物5级	1%
$Item_CaiJi[551010601] = "106020106 90 106020206 9 106020306 1";	//寒月矿	矿石半成品6级	90%	寒月矿石	9%	矿石伴生物6级	1%
$Item_CaiJi[551010701] = "106020107 90 106020207 9 106020307 1";	//巫诅矿	矿石半成品7级	90%	巫诅矿石	9%	矿石伴生物7级	1%
$Item_CaiJi[551010801] = "106020108 90 106020208 9 106020308 1";	//龙鳞矿	矿石半成品8级	90%	龙鳞矿石	9%	矿石伴生物8级	1%
$Item_CaiJi[551010901] = "106020109 90 106020209 9 106020309 1";	//九天矿	矿石半成品9级	90%	九天矿石	9%	矿石伴生物9级	1%
$Item_CaiJi[551011001] = "106020110 90 106020210 9 106020310 1";	//七彩玄冰矿	矿石半成品10级	90%	七彩玄冰矿石	9%	矿石伴生物10级	1%
$Item_CaiJi[551020101] = "106040101 90 106040201 9 106040301 1";	//棕榈木	木材半成品1级	90%	木材成品1级	9%	木材伴生1级	1%
$Item_CaiJi[551020201] = "106040102 90 106040202 9 106040302 1";	//小乔木	木材半成品2级	90%	木材成品2级	9%	木材伴生2级	1%
$Item_CaiJi[551020302] = "106040103 90 106040203 9 106040303 1";	//白皮松	木材半成品3级	90%	木材成品3级	9%	木材伴生3级	1%
$Item_CaiJi[551020401] = "106040104 90 106040204 9 106040304 1";	//红松	木材半成品4级	90%	木材成品4级	9%	木材伴生4级	1%
$Item_CaiJi[551020501] = "106040105 90 106040205 9 106040305 1";	//精木	木材半成品5级	90%	木材成品5级	9%	木材伴生5级	1%
$Item_CaiJi[551020601] = "106040106 90 106040206 9 106040306 1";	//寂静古树	木材半成品6级	90%	木材成品6级	9%	木材伴生6级	1%
$Item_CaiJi[551020701] = "106040107 90 106040207 9 106040307 1";	//缚魂木	木材半成品7级	90%	木材成品7级	9%	木材伴生7级	1%
$Item_CaiJi[551020801] = "106040108 90 106040208 9 106040308 1";	//龙血树	木材半成品8级	90%	木材成品8级	9%	木材伴生8级	1%
$Item_CaiJi[551020901] = "106040109 90 106040209 9 106040309 1";	//麒麟血藤	木材半成品9级	90%	木材成品9级	9%	木材伴生9级	1%
$Item_CaiJi[551021001] = "106040110 90 106040210 9 106040310 1";	//女夷枝条 	木材半成品10级	90%	木材成品10级	9%	木材伴生10级	1%
$Item_CaiJi[551030101] = "106050101 90 106050201 9 106050301 1";	//1级鱼群	钓鱼半生品1级	90%	钓鱼成品1级	9%	钓鱼伴生1级	1%
$Item_CaiJi[551030201] = "106050102 90 106050202 9 106050302 1";	//2级鱼群	钓鱼半生品2级	90%	钓鱼成品2级	9%	钓鱼伴生2级	1%
$Item_CaiJi[551030301] = "106050103 90 106050203 9 106050303 1";	//3级鱼群	钓鱼半生品3级	90%	钓鱼成品3级	9%	钓鱼伴生3级	1%
$Item_CaiJi[551030401] = "106050104 90 106050204 9 106050304 1";	//4级鱼群	钓鱼半生品4级	90%	钓鱼成品4级	9%	钓鱼伴生4级	1%
$Item_CaiJi[551030501] = "106050105 90 106050205 9 106050305 1";	//5级鱼群	钓鱼半生品5级	90%	钓鱼成品5级	9%	钓鱼伴生5级	1%
$Item_CaiJi[551030601] = "106050106 90 106050206 9 106050306 1";	//6级鱼群	钓鱼半生品6级	90%	钓鱼成品6级	9%	钓鱼伴生6级	1%
$Item_CaiJi[551030701] = "106050107 90 106050207 9 106050307 1";	//7级鱼群	钓鱼半生品7级	90%	钓鱼成品7级	9%	钓鱼伴生7级	1%
$Item_CaiJi[551030801] = "106050108 90 106050208 9 106050308 1";	//8级鱼群	钓鱼半生品8级	90%	钓鱼成品8级	9%	钓鱼伴生8级	1%
$Item_CaiJi[551030901] = "106050109 90 106050209 9 106050309 1";	//9级鱼群	钓鱼半生品9级	90%	钓鱼成品9级	9%	钓鱼伴生9级	1%
$Item_CaiJi[551031001] = "106050110 90 106050210 9 106050310 1";	//10级鱼群	钓鱼半生品10级	90%	钓鱼成品10级	9%	钓鱼伴生10级	1%
$Item_CaiJi[551040101] = "000000000 90 000000000 9 000000000 1";	//1级农作物	0	90%	0	9%	0	1%
$Item_CaiJi[551040201] = "000000000 90 000000000 9 000000000 1";	//2级农作物	0	90%	0	9%	0	1%
$Item_CaiJi[551040301] = "000000000 90 000000000 9 000000000 1";	//3级农作物	0	90%	0	9%	0	1%
$Item_CaiJi[551040401] = "000000000 90 000000000 9 000000000 1";	//4级农作物	0	90%	0	9%	0	1%
$Item_CaiJi[551040501] = "000000000 90 000000000 9 000000000 1";	//5级农作物	0	90%	0	9%	0	1%
$Item_CaiJi[551040601] = "000000000 90 000000000 9 000000000 1";	//6级农作物	0	90%	0	9%	0	1%
$Item_CaiJi[551040701] = "000000000 90 000000000 9 000000000 1";	//7级农作物	0	90%	0	9%	0	1%
$Item_CaiJi[551040801] = "000000000 90 000000000 9 000000000 1";	//8级农作物	0	90%	0	9%	0	1%
$Item_CaiJi[551040901] = "000000000 90 000000000 9 000000000 1";	//9级农作物	0	90%	0	9%	0	1%
$Item_CaiJi[551041001] = "000000000 90 000000000 9 000000000 1";	//10级农作物	0	90%	0	9%	0	1%
$Item_CaiJi[551050101] = "106010101 90 106010201 9 106010301 1";	//兔子草	草药半成品1级	90%	草药成品1级	9%	草药伴生1级	1%
$Item_CaiJi[551050201] = "106010102 90 106010202 9 106010302 1";	//蛇舌草	草药半成品2级	90%	草药成品2级	9%	草药伴生2级	1%
$Item_CaiJi[551050401] = "106010103 90 106010203 9 106010303 1";	//黄精	草药半成品3级	90%	草药成品3级	9%	草药伴生3级	1%
$Item_CaiJi[551050501] = "106010104 90 106010204 9 106010304 1";	//佛手瓜	草药半成品4级	90%	草药成品4级	9%	草药伴生4级	1%
$Item_CaiJi[551050601] = "106010105 90 106010205 9 106010305 1";	//月桂	草药半成品5级	90%	草药成品5级	9%	草药伴生5级	1%
$Item_CaiJi[551050602] = "106010106 90 106010206 9 106010306 1";	//竹菇	草药半成品6级	90%	草药成品6级	9%	草药伴生6级	1%
$Item_CaiJi[551050701] = "106010107 90 106010207 9 106010307 1";	//冬虫夏草	草药半成品7级	90%	草药成品7级	9%	草药伴生7级	1%
$Item_CaiJi[551050801] = "106010108 90 106010208 9 106010308 1";	//龙须藤	草药半成品8级	90%	草药成品8级	9%	草药伴生8级	1%
$Item_CaiJi[551050901] = "106010109 90 106010209 9 106010309 1";	//人参	草药半成品9级	90%	草药成品9级	9%	草药伴生9级	1%
$Item_CaiJi[551051001] = "106010110 90 106010210 9 106010310 1";	//灵芝	草药半成品10级	90%	草药成品10级	9%	草药伴生10级	1%
$Item_CaiJi[551060101] = "106060101 90 106060201 9 106060301 1";	//1级狩猎	兽肉办成品1级	90%	兽肉成品1级	9%	兽肉伴生1级	1%
$Item_CaiJi[551060201] = "106060102 90 106060202 9 106060302 1";	//2级狩猎	兽肉办成品2级	90%	兽肉成品2级	9%	兽肉伴生2级	1%
$Item_CaiJi[551060301] = "106060103 90 106060203 9 106060303 1";	//3级狩猎	兽肉办成品3级	90%	兽肉成品3级	9%	兽肉伴生3级	1%
$Item_CaiJi[551060401] = "106060104 90 106060204 9 106060304 1";	//4级狩猎	兽肉办成品4级	90%	兽肉成品4级	9%	兽肉伴生4级	1%
$Item_CaiJi[551060501] = "106060105 90 106060205 9 106060305 1";	//5级狩猎	兽肉办成品5级	90%	兽肉成品5级	9%	兽肉伴生5级	1%
$Item_CaiJi[551060601] = "106060106 90 106060206 9 106060306 1";	//6级狩猎	兽肉办成品6级	90%	兽肉成品6级	9%	兽肉伴生6级	1%
$Item_CaiJi[551060701] = "106060107 90 106060207 9 106060307 1";	//7级狩猎	兽肉办成品7级	90%	兽肉成品7级	9%	兽肉伴生7级	1%
$Item_CaiJi[551060801] = "106060108 90 106060208 9 106060308 1";	//8级狩猎	兽肉办成品8级	90%	兽肉成品8级	9%	兽肉伴生8级	1%
$Item_CaiJi[551060901] = "106060109 90 106060209 9 106060309 1";	//9级狩猎	兽肉办成品9级	90%	兽肉成品9级	9%	兽肉伴生9级	1%
$Item_CaiJi[551061001] = "106060110 90 106060210 9 106060310 1";	//10级狩猎	兽肉办成品10级	90%	兽肉成品10级	9%	兽肉伴生10级	1%

//■■■■■■■■■■■各个采集物给于的物品■■■■■■■■■■■■■■■■■■



//■■■■■■■■■■■各个怪物等级给于的基本经验■■■■■■■■■■■■■■■
$Monster_Exp[1  ,1] = 50  ;	$Monster_Exp[1  ,2] = 15   ;
$Monster_Exp[2  ,1] = 55  ; $Monster_Exp[2  ,2] = 17   ;
$Monster_Exp[3  ,1] = 60  ; $Monster_Exp[3  ,2] = 18   ;
$Monster_Exp[4  ,1] = 65  ; $Monster_Exp[4  ,2] = 20   ;
$Monster_Exp[5  ,1] = 75  ; $Monster_Exp[5  ,2] = 23   ;
$Monster_Exp[6  ,1] = 80  ; $Monster_Exp[6  ,2] = 24   ;
$Monster_Exp[7  ,1] = 90  ; $Monster_Exp[7  ,2] = 27   ;
$Monster_Exp[8  ,1] = 95  ; $Monster_Exp[8  ,2] = 29   ;
$Monster_Exp[9  ,1] = 105 ; $Monster_Exp[9  ,2] = 32   ;
$Monster_Exp[10 ,1] = 115 ; $Monster_Exp[10 ,2] = 35   ;
$Monster_Exp[11 ,1] = 130 ; $Monster_Exp[11 ,2] = 39   ;
$Monster_Exp[12 ,1] = 140 ; $Monster_Exp[12 ,2] = 42   ;
$Monster_Exp[13 ,1] = 155 ; $Monster_Exp[13 ,2] = 47   ;
$Monster_Exp[14 ,1] = 170 ; $Monster_Exp[14 ,2] = 51   ;
$Monster_Exp[15 ,1] = 185 ; $Monster_Exp[15 ,2] = 56   ;
$Monster_Exp[16 ,1] = 205 ; $Monster_Exp[16 ,2] = 62   ;
$Monster_Exp[17 ,1] = 225 ; $Monster_Exp[17 ,2] = 68   ;
$Monster_Exp[18 ,1] = 250 ; $Monster_Exp[18 ,2] = 75   ;
$Monster_Exp[19 ,1] = 275 ; $Monster_Exp[19 ,2] = 83   ;
$Monster_Exp[20 ,1] = 300 ; $Monster_Exp[20 ,2] = 90   ;
$Monster_Exp[21 ,1] = 310 ; $Monster_Exp[21 ,2] = 93   ;
$Monster_Exp[22 ,1] = 320 ; $Monster_Exp[22 ,2] = 96   ;
$Monster_Exp[23 ,1] = 335 ; $Monster_Exp[23 ,2] = 101  ;
$Monster_Exp[24 ,1] = 345 ; $Monster_Exp[24 ,2] = 104  ;
$Monster_Exp[25 ,1] = 355 ; $Monster_Exp[25 ,2] = 107  ;
$Monster_Exp[26 ,1] = 370 ; $Monster_Exp[26 ,2] = 111  ;
$Monster_Exp[27 ,1] = 380 ; $Monster_Exp[27 ,2] = 114  ;
$Monster_Exp[28 ,1] = 395 ; $Monster_Exp[28 ,2] = 119  ;
$Monster_Exp[29 ,1] = 410 ; $Monster_Exp[29 ,2] = 123  ;
$Monster_Exp[30 ,1] = 425 ; $Monster_Exp[30 ,2] = 128  ;
$Monster_Exp[31 ,1] = 440 ; $Monster_Exp[31 ,2] = 132  ;
$Monster_Exp[32 ,1] = 455 ; $Monster_Exp[32 ,2] = 137  ;
$Monster_Exp[33 ,1] = 470 ; $Monster_Exp[33 ,2] = 141  ;
$Monster_Exp[34 ,1] = 485 ; $Monster_Exp[34 ,2] = 146  ;
$Monster_Exp[35 ,1] = 505 ; $Monster_Exp[35 ,2] = 152  ;
$Monster_Exp[36 ,1] = 520 ; $Monster_Exp[36 ,2] = 156  ;
$Monster_Exp[37 ,1] = 540 ; $Monster_Exp[37 ,2] = 162  ;
$Monster_Exp[38 ,1] = 560 ; $Monster_Exp[38 ,2] = 168  ;
$Monster_Exp[39 ,1] = 580 ; $Monster_Exp[39 ,2] = 174  ;
$Monster_Exp[40 ,1] = 600 ; $Monster_Exp[40 ,2] = 180  ;
$Monster_Exp[41 ,1] = 610 ; $Monster_Exp[41 ,2] = 183  ;
$Monster_Exp[42 ,1] = 625 ; $Monster_Exp[42 ,2] = 188  ;
$Monster_Exp[43 ,1] = 640 ; $Monster_Exp[43 ,2] = 192  ;
$Monster_Exp[44 ,1] = 650 ; $Monster_Exp[44 ,2] = 195  ;
$Monster_Exp[45 ,1] = 665 ; $Monster_Exp[45 ,2] = 200  ;
$Monster_Exp[46 ,1] = 680 ; $Monster_Exp[46 ,2] = 204  ;
$Monster_Exp[47 ,1] = 690 ; $Monster_Exp[47 ,2] = 207  ;
$Monster_Exp[48 ,1] = 705 ; $Monster_Exp[48 ,2] = 212  ;
$Monster_Exp[49 ,1] = 720 ; $Monster_Exp[49 ,2] = 216  ;
$Monster_Exp[50 ,1] = 735 ; $Monster_Exp[50 ,2] = 221  ;
$Monster_Exp[51 ,1] = 750 ; $Monster_Exp[51 ,2] = 225  ;
$Monster_Exp[52 ,1] = 765 ; $Monster_Exp[52 ,2] = 230  ;
$Monster_Exp[53 ,1] = 780 ; $Monster_Exp[53 ,2] = 234  ;
$Monster_Exp[54 ,1] = 795 ; $Monster_Exp[54 ,2] = 239  ;
$Monster_Exp[55 ,1] = 815 ; $Monster_Exp[55 ,2] = 245  ;
$Monster_Exp[56 ,1] = 830 ; $Monster_Exp[56 ,2] = 249  ;
$Monster_Exp[57 ,1] = 845 ; $Monster_Exp[57 ,2] = 254  ;
$Monster_Exp[58 ,1] = 865 ; $Monster_Exp[58 ,2] = 260  ;
$Monster_Exp[59 ,1] = 880 ; $Monster_Exp[59 ,2] = 264  ;
$Monster_Exp[60 ,1] = 900 ; $Monster_Exp[60 ,2] = 270  ;
$Monster_Exp[61 ,1] = 915 ; $Monster_Exp[61 ,2] = 275  ;
$Monster_Exp[62 ,1] = 925 ; $Monster_Exp[62 ,2] = 278  ;
$Monster_Exp[63 ,1] = 940 ; $Monster_Exp[63 ,2] = 282  ;
$Monster_Exp[64 ,1] = 955 ; $Monster_Exp[64 ,2] = 287  ;
$Monster_Exp[65 ,1] = 965 ; $Monster_Exp[65 ,2] = 290  ;
$Monster_Exp[66 ,1] = 980 ; $Monster_Exp[66 ,2] = 294  ;
$Monster_Exp[67 ,1] = 995 ; $Monster_Exp[67 ,2] = 299  ;
$Monster_Exp[68 ,1] = 1010; $Monster_Exp[68 ,2] = 303  ;
$Monster_Exp[69 ,1] = 1025; $Monster_Exp[69 ,2] = 308  ;
$Monster_Exp[70 ,1] = 1040; $Monster_Exp[70 ,2] = 312  ;
$Monster_Exp[71 ,1] = 1055; $Monster_Exp[71 ,2] = 317  ;
$Monster_Exp[72 ,1] = 1070; $Monster_Exp[72 ,2] = 321  ;
$Monster_Exp[73 ,1] = 1085; $Monster_Exp[73 ,2] = 326  ;
$Monster_Exp[74 ,1] = 1100; $Monster_Exp[74 ,2] = 330  ;
$Monster_Exp[75 ,1] = 1115; $Monster_Exp[75 ,2] = 335  ;
$Monster_Exp[76 ,1] = 1135; $Monster_Exp[76 ,2] = 341  ;
$Monster_Exp[77 ,1] = 1150; $Monster_Exp[77 ,2] = 345  ;
$Monster_Exp[78 ,1] = 1165; $Monster_Exp[78 ,2] = 350  ;
$Monster_Exp[79 ,1] = 1185; $Monster_Exp[79 ,2] = 356  ;
$Monster_Exp[80 ,1] = 1200; $Monster_Exp[80 ,2] = 360  ;
$Monster_Exp[81 ,1] = 1225; $Monster_Exp[81 ,2] = 368  ;
$Monster_Exp[82 ,1] = 1250; $Monster_Exp[82 ,2] = 375  ;
$Monster_Exp[83 ,1] = 1275; $Monster_Exp[83 ,2] = 383  ;
$Monster_Exp[84 ,1] = 1300; $Monster_Exp[84 ,2] = 390  ;
$Monster_Exp[85 ,1] = 1330; $Monster_Exp[85 ,2] = 399  ;
$Monster_Exp[86 ,1] = 1355; $Monster_Exp[86 ,2] = 407  ;
$Monster_Exp[87 ,1] = 1385; $Monster_Exp[87 ,2] = 416  ;
$Monster_Exp[88 ,1] = 1410; $Monster_Exp[88 ,2] = 423  ;
$Monster_Exp[89 ,1] = 1440; $Monster_Exp[89 ,2] = 432  ;
$Monster_Exp[90 ,1] = 1470; $Monster_Exp[90 ,2] = 441  ;
$Monster_Exp[91 ,1] = 1500; $Monster_Exp[91 ,2] = 450  ;
$Monster_Exp[92 ,1] = 1530; $Monster_Exp[92 ,2] = 459  ;
$Monster_Exp[93 ,1] = 1560; $Monster_Exp[93 ,2] = 468  ;
$Monster_Exp[94 ,1] = 1595; $Monster_Exp[94 ,2] = 479  ;
$Monster_Exp[95 ,1] = 1625; $Monster_Exp[95 ,2] = 488  ;
$Monster_Exp[96 ,1] = 1660; $Monster_Exp[96 ,2] = 498  ;
$Monster_Exp[97 ,1] = 1695; $Monster_Exp[97 ,2] = 509  ;
$Monster_Exp[98 ,1] = 1730; $Monster_Exp[98 ,2] = 519  ;
$Monster_Exp[99 ,1] = 1765; $Monster_Exp[99 ,2] = 530  ;
$Monster_Exp[100,1] = 1800; $Monster_Exp[100,2] = 540  ;
$Monster_Exp[101,1] = 1830; $Monster_Exp[101,2] = 549  ;
$Monster_Exp[102,1] = 1860; $Monster_Exp[102,2] = 558  ;
$Monster_Exp[103,1] = 1890; $Monster_Exp[103,2] = 567  ;
$Monster_Exp[104,1] = 1920; $Monster_Exp[104,2] = 576  ;
$Monster_Exp[105,1] = 1955; $Monster_Exp[105,2] = 587  ;
$Monster_Exp[106,1] = 1985; $Monster_Exp[106,2] = 596  ;
$Monster_Exp[107,1] = 2020; $Monster_Exp[107,2] = 606  ;
$Monster_Exp[108,1] = 2055; $Monster_Exp[108,2] = 617  ;
$Monster_Exp[109,1] = 2085; $Monster_Exp[109,2] = 626  ;
$Monster_Exp[110,1] = 2120; $Monster_Exp[110,2] = 636  ;
$Monster_Exp[111,1] = 2155; $Monster_Exp[111,2] = 647  ;
$Monster_Exp[112,1] = 2190; $Monster_Exp[112,2] = 657  ;
$Monster_Exp[113,1] = 2230; $Monster_Exp[113,2] = 669  ;
$Monster_Exp[114,1] = 2265; $Monster_Exp[114,2] = 680  ;
$Monster_Exp[115,1] = 2305; $Monster_Exp[115,2] = 692  ;
$Monster_Exp[116,1] = 2340; $Monster_Exp[116,2] = 702  ;
$Monster_Exp[117,1] = 2380; $Monster_Exp[117,2] = 714  ;
$Monster_Exp[118,1] = 2420; $Monster_Exp[118,2] = 726  ;
$Monster_Exp[119,1] = 2460; $Monster_Exp[119,2] = 738  ;
$Monster_Exp[120,1] = 2500; $Monster_Exp[120,2] = 750  ;
$Monster_Exp[121,1] = 2540; $Monster_Exp[121,2] = 762  ;
$Monster_Exp[122,1] = 2580; $Monster_Exp[122,2] = 774  ;
$Monster_Exp[123,1] = 2620; $Monster_Exp[123,2] = 786  ;
$Monster_Exp[124,1] = 2660; $Monster_Exp[124,2] = 798  ;
$Monster_Exp[125,1] = 2700; $Monster_Exp[125,2] = 810  ;
$Monster_Exp[126,1] = 2740; $Monster_Exp[126,2] = 822  ;
$Monster_Exp[127,1] = 2785; $Monster_Exp[127,2] = 836  ;
$Monster_Exp[128,1] = 2825; $Monster_Exp[128,2] = 848  ;
$Monster_Exp[129,1] = 2870; $Monster_Exp[129,2] = 861  ;
$Monster_Exp[130,1] = 2915; $Monster_Exp[130,2] = 875  ;
$Monster_Exp[131,1] = 2960; $Monster_Exp[131,2] = 888  ;
$Monster_Exp[132,1] = 3005; $Monster_Exp[132,2] = 902  ;
$Monster_Exp[133,1] = 3055; $Monster_Exp[133,2] = 917  ;
$Monster_Exp[134,1] = 3100; $Monster_Exp[134,2] = 930  ;
$Monster_Exp[135,1] = 3150; $Monster_Exp[135,2] = 945  ;
$Monster_Exp[136,1] = 3195; $Monster_Exp[136,2] = 959  ;
$Monster_Exp[137,1] = 3245; $Monster_Exp[137,2] = 974  ;
$Monster_Exp[138,1] = 3295; $Monster_Exp[138,2] = 989  ;
$Monster_Exp[139,1] = 3350; $Monster_Exp[139,2] = 1005 ;
$Monster_Exp[140,1] = 3400; $Monster_Exp[140,2] = 1020 ;
$Monster_Exp[141,1] = 3450; $Monster_Exp[141,2] = 1035 ;
$Monster_Exp[142,1] = 3495; $Monster_Exp[142,2] = 1049 ;
$Monster_Exp[143,1] = 3545; $Monster_Exp[143,2] = 1064 ;
$Monster_Exp[144,1] = 3595; $Monster_Exp[144,2] = 1079 ;
$Monster_Exp[145,1] = 3645; $Monster_Exp[145,2] = 1094 ;
$Monster_Exp[146,1] = 3700; $Monster_Exp[146,2] = 1110 ;
$Monster_Exp[147,1] = 3750; $Monster_Exp[147,2] = 1125 ;
$Monster_Exp[148,1] = 3805; $Monster_Exp[148,2] = 1142 ;
$Monster_Exp[149,1] = 3855; $Monster_Exp[149,2] = 1157 ;
$Monster_Exp[150,1] = 3910; $Monster_Exp[150,2] = 1173 ;
$Monster_Exp[151,1] = 3965; $Monster_Exp[151,2] = 1190 ;
$Monster_Exp[152,1] = 4025; $Monster_Exp[152,2] = 1208 ;
$Monster_Exp[153,1] = 4080; $Monster_Exp[153,2] = 1224 ;
$Monster_Exp[154,1] = 4135; $Monster_Exp[154,2] = 1241 ;
$Monster_Exp[155,1] = 4195; $Monster_Exp[155,2] = 1259 ;
$Monster_Exp[156,1] = 4255; $Monster_Exp[156,2] = 1277 ;
$Monster_Exp[157,1] = 4315; $Monster_Exp[157,2] = 1295 ;
$Monster_Exp[158,1] = 4375; $Monster_Exp[158,2] = 1313 ;
$Monster_Exp[159,1] = 4435; $Monster_Exp[159,2] = 1331 ;
$Monster_Exp[160,1] = 4500; $Monster_Exp[160,2] = 1350 ;
$Monster_Exp[161,1] = 4565; $Monster_Exp[161,2] = 1370 ;
$Monster_Exp[162,1] = 4630;
$Monster_Exp[163,1] = 4700;
$Monster_Exp[164,1] = 4765;
$Monster_Exp[165,1] = 4835;
$Monster_Exp[166,1] = 4905;
$Monster_Exp[167,1] = 4975;
$Monster_Exp[168,1] = 5050;
$Monster_Exp[169,1] = 5120;
$Monster_Exp[170,1] = 5195;
$Monster_Exp[171,1] = 5270;
$Monster_Exp[172,1] = 5350;
$Monster_Exp[173,1] = 5425;
$Monster_Exp[174,1] = 5505;
$Monster_Exp[175,1] = 5585;
$Monster_Exp[176,1] = 5665;
$Monster_Exp[177,1] = 5745;
$Monster_Exp[178,1] = 5830;
$Monster_Exp[179,1] = 5915;
$Monster_Exp[180,1] = 6000;
$Monster_Exp[181,1] = 6065;
$Monster_Exp[182,1] = 6135;
$Monster_Exp[183,1] = 6205;
$Monster_Exp[184,1] = 6275;
$Monster_Exp[185,1] = 6345;
$Monster_Exp[186,1] = 6415;
$Monster_Exp[187,1] = 6485;
$Monster_Exp[188,1] = 6560;
$Monster_Exp[189,1] = 6635;
$Monster_Exp[190,1] = 6710;
$Monster_Exp[191,1] = 6785;
$Monster_Exp[192,1] = 6860;
$Monster_Exp[193,1] = 6935;
$Monster_Exp[194,1] = 7015;
$Monster_Exp[195,1] = 7095;
$Monster_Exp[196,1] = 7175;
$Monster_Exp[197,1] = 7255;
$Monster_Exp[198,1] = 7335;
$Monster_Exp[199,1] = 7415;
$Monster_Exp[200,1] = 7500;
$Monster_Exp[201,1] = 7600;

//■■■■■■■■■■■各个怪物等级给于的基本经验■■■■■■■■■■■■■■■


//■■■■■■■■■■■各配方学习需求■■■■■■■■■■■■■■■■■■■■■

$Pres_Add[612000001] = "502011001 0";
$Pres_Add[612000002] = "502011001 0";
$Pres_Add[612000003] = "502011001 0";
$Pres_Add[612000004] = "502011001 0";
$Pres_Add[612000005] = "502011001 0";
$Pres_Add[612000006] = "502011001 0";
$Pres_Add[612000007] = "502011001 0";
$Pres_Add[612000008] = "502011001 0";
$Pres_Add[612000009] = "502011001 0";
$Pres_Add[612000010] = "502011001 0";
$Pres_Add[612000011] = "502011001 0";
$Pres_Add[612000012] = "502011001 0";
$Pres_Add[612000013] = "502011001 0";
$Pres_Add[612000014] = "502011001 0";
$Pres_Add[612000015] = "502011001 0";
$Pres_Add[612000016] = "502011001 0";
$Pres_Add[612000017] = "502011001 0";
$Pres_Add[612000018] = "502011001 0";
$Pres_Add[612000019] = "502011001 0";
$Pres_Add[612000020] = "502011001 0";
$Pres_Add[612000021] = "502011001 0";
$Pres_Add[612000022] = "502011001 0";
$Pres_Add[612000023] = "502011001 0";
$Pres_Add[612000024] = "502011001 0";
$Pres_Add[612000025] = "502011001 0";
$Pres_Add[612000026] = "502011001 0";
$Pres_Add[612000027] = "502011001 0";
$Pres_Add[612000028] = "502011001 0";
$Pres_Add[612000029] = "502011001 0";
$Pres_Add[612000030] = "502011001 0";
$Pres_Add[612000031] = "502011001 0";
$Pres_Add[612000032] = "502011001 0";
$Pres_Add[612000033] = "502011001 0";
$Pres_Add[612000034] = "502011001 0";
$Pres_Add[612000035] = "502011001 0";
$Pres_Add[612000036] = "502011001 0";
$Pres_Add[612000037] = "502011001 0";
$Pres_Add[612000038] = "502011001 0";
$Pres_Add[612000039] = "502011001 0";
$Pres_Add[612000040] = "502011001 0";
$Pres_Add[630102001] = "502011001 0";
$Pres_Add[630202001] = "502011001 0";
$Pres_Add[630302001] = "502011001 0";
$Pres_Add[630402001] = "502011001 0";
$Pres_Add[630502001] = "502011001 0";
$Pres_Add[630602001] = "502011001 0";
$Pres_Add[630702001] = "502011001 0";
$Pres_Add[612000121] = "503011001 0";
$Pres_Add[612000122] = "503011001 0";
$Pres_Add[612000123] = "503011001 0";
$Pres_Add[612000124] = "503011001 0";
$Pres_Add[612000125] = "503011001 0";
$Pres_Add[612000126] = "503011001 0";
$Pres_Add[612000127] = "503011001 0";
$Pres_Add[612000128] = "503011001 0";
$Pres_Add[612000129] = "503011001 0";
$Pres_Add[612000130] = "503011001 0";
$Pres_Add[612000131] = "503011001 0";
$Pres_Add[612000132] = "503011001 0";
$Pres_Add[612000133] = "503011001 0";
$Pres_Add[612000134] = "503011001 0";
$Pres_Add[612000135] = "503011001 0";
$Pres_Add[612000136] = "503011001 0";
$Pres_Add[612000137] = "503011001 0";
$Pres_Add[612000138] = "503011001 0";
$Pres_Add[612000139] = "503011001 0";
$Pres_Add[612000140] = "503011001 0";
$Pres_Add[612000141] = "503011001 0";
$Pres_Add[612000142] = "503011001 0";
$Pres_Add[612000143] = "503011001 0";
$Pres_Add[612000144] = "503011001 0";
$Pres_Add[612000145] = "503011001 0";
$Pres_Add[612000146] = "503011001 0";
$Pres_Add[612000147] = "503011001 0";
$Pres_Add[612000148] = "503011001 0";
$Pres_Add[612000149] = "503011001 0";
$Pres_Add[612000150] = "503011001 0";
$Pres_Add[612000151] = "503011001 0";
$Pres_Add[612000152] = "503011001 0";
$Pres_Add[612000153] = "503011001 0";
$Pres_Add[612000154] = "503011001 0";
$Pres_Add[612000155] = "503011001 0";
$Pres_Add[612000156] = "503011001 0";
$Pres_Add[612000157] = "503011001 0";
$Pres_Add[612000158] = "503011001 0";
$Pres_Add[612000159] = "503011001 0";
$Pres_Add[612000160] = "503011001 0";
$Pres_Add[630901201] = "503011001 0";
$Pres_Add[612000041] = "504011001 0";
$Pres_Add[612000042] = "504011001 0";
$Pres_Add[612000043] = "504011001 0";
$Pres_Add[612000044] = "504011001 0";
$Pres_Add[612000045] = "504011001 0";
$Pres_Add[612000046] = "504011001 0";
$Pres_Add[612000047] = "504011001 0";
$Pres_Add[612000048] = "504011001 0";
$Pres_Add[612000049] = "504011001 0";
$Pres_Add[612000050] = "504011001 0";
$Pres_Add[612000051] = "504011001 0";
$Pres_Add[612000052] = "504011001 0";
$Pres_Add[612000053] = "504011001 0";
$Pres_Add[612000054] = "504011001 0";
$Pres_Add[612000055] = "504011001 0";
$Pres_Add[612000056] = "504011001 0";
$Pres_Add[612000057] = "504011001 0";
$Pres_Add[612000058] = "504011001 0";
$Pres_Add[612000059] = "504011001 0";
$Pres_Add[612000060] = "504011001 0";
$Pres_Add[631502001] = "504011001 0";
$Pres_Add[631602001] = "504011001 0";
$Pres_Add[631702001] = "504011001 0";
$Pres_Add[611920101] = "506011001 0";
$Pres_Add[611920121] = "506011001 0";
$Pres_Add[612000101] = "506011001 0";
$Pres_Add[612000102] = "506011001 0";
$Pres_Add[612000103] = "506011001 0";
$Pres_Add[612000104] = "506011001 0";
$Pres_Add[612000105] = "506011001 0";
$Pres_Add[612000106] = "506011001 0";
$Pres_Add[612000107] = "506011001 0";
$Pres_Add[612000108] = "506011001 0";
$Pres_Add[612000109] = "506011001 0";
$Pres_Add[612000110] = "506011001 0";
$Pres_Add[612000111] = "506011001 0";
$Pres_Add[612000112] = "506011001 0";
$Pres_Add[612000113] = "506011001 0";
$Pres_Add[612000114] = "506011001 0";
$Pres_Add[612000115] = "506011001 0";
$Pres_Add[612000116] = "506011001 0";
$Pres_Add[612000117] = "506011001 0";
$Pres_Add[612000118] = "506011001 0";
$Pres_Add[612000119] = "506011001 0";
$Pres_Add[612000120] = "506011001 0";
$Pres_Add[611810101] = "507011001 0";
$Pres_Add[611810121] = "507011001 0";
$Pres_Add[612000081] = "507011001 0";
$Pres_Add[612000082] = "507011001 0";
$Pres_Add[612000083] = "507011001 0";
$Pres_Add[612000084] = "507011001 0";
$Pres_Add[612000085] = "507011001 0";
$Pres_Add[612000086] = "507011001 0";
$Pres_Add[612000087] = "507011001 0";
$Pres_Add[612000088] = "507011001 0";
$Pres_Add[612000089] = "507011001 0";
$Pres_Add[612000090] = "507011001 0";
$Pres_Add[612000091] = "507011001 0";
$Pres_Add[612000092] = "507011001 0";
$Pres_Add[612000093] = "507011001 0";
$Pres_Add[612000094] = "507011001 0";
$Pres_Add[612000095] = "507011001 0";
$Pres_Add[612000096] = "507011001 0";
$Pres_Add[612000097] = "507011001 0";
$Pres_Add[612000098] = "507011001 0";
$Pres_Add[612000099] = "507011001 0";
$Pres_Add[612000100] = "507011001 0";
$Pres_Add[611810102] = "507011001 300";
$Pres_Add[611810103] = "507011001 600";
$Pres_Add[611810104] = "507011002 1000";
$Pres_Add[611810105] = "507011002 1500";
$Pres_Add[611810106] = "507011003 2000";
$Pres_Add[611810122] = "507011001 300";
$Pres_Add[611810123] = "507011001 600";
$Pres_Add[611810124] = "507011002 1000";
$Pres_Add[611810125] = "507011002 1500";
$Pres_Add[611810126] = "507011003 2000";
$Pres_Add[611920102] = "506011002 1000";
$Pres_Add[611920103] = "506011002 1500";
$Pres_Add[611920104] = "506011003 2000";
$Pres_Add[611920105] = "506011004 3000";
$Pres_Add[611920106] = "506011005 4000";
$Pres_Add[611920107] = "506011005 4300";
$Pres_Add[611920108] = "506011005 4600";
$Pres_Add[611920109] = "506011005 5000";
$Pres_Add[611920122] = "506011002 1000";
$Pres_Add[611920123] = "506011002 1500";
$Pres_Add[611920124] = "506011003 2000";
$Pres_Add[611920125] = "506011004 3000";
$Pres_Add[611920126] = "506011005 4000";
$Pres_Add[611920127] = "506011005 4300";
$Pres_Add[611920128] = "506011005 4600";
$Pres_Add[611920129] = "506011005 5000";
$Pres_Add[611920141] = "506011001 500";
$Pres_Add[611920142] = "506011002 1500";
$Pres_Add[611920143] = "506011003 2500";
$Pres_Add[611920144] = "506011003 2500";
$Pres_Add[611920145] = "506011004 3500";
$Pres_Add[611920146] = "506011004 3500";
$Pres_Add[611920147] = "506011005 4500";
$Pres_Add[611920148] = "506011005 4500";
$Pres_Add[611920149] = "506011005 4500";
$Pres_Add[611920150] = "506011005 4500";
$Pres_Add[630103001] = "502011001 500";
$Pres_Add[630104001] = "502011002 1000";
$Pres_Add[630105001] = "502011002 1500";
$Pres_Add[630106001] = "502011003 2000";
$Pres_Add[630203001] = "502011001 500";
$Pres_Add[630204001] = "502011002 1000";
$Pres_Add[630205001] = "502011002 1500";
$Pres_Add[630206001] = "502011003 2000";
$Pres_Add[630303001] = "502011001 500";
$Pres_Add[630304001] = "502011002 1000";
$Pres_Add[630305001] = "502011002 1500";
$Pres_Add[630306001] = "502011003 2000";
$Pres_Add[630403001] = "502011001 500";
$Pres_Add[630404001] = "502011002 1000";
$Pres_Add[630405001] = "502011002 1500";
$Pres_Add[630406001] = "502011003 2000";
$Pres_Add[630503001] = "502011001 500";
$Pres_Add[630504001] = "502011002 1000";
$Pres_Add[630505001] = "502011002 1500";
$Pres_Add[630506001] = "502011003 2000";
$Pres_Add[630603001] = "502011001 500";
$Pres_Add[630604001] = "502011002 1000";
$Pres_Add[630605001] = "502011002 1500";
$Pres_Add[630606001] = "502011003 2000";
$Pres_Add[630703001] = "502011001 500";
$Pres_Add[630704001] = "502011002 1000";
$Pres_Add[630705001] = "502011002 1500";
$Pres_Add[630706001] = "502011003 2000";
$Pres_Add[630801601] = "503011001 80";
$Pres_Add[630802601] = "503011001 430";
$Pres_Add[630803601] = "503011001 780";
$Pres_Add[630804601] = "503011002 1140";
$Pres_Add[630805601] = "503011002 1630";
$Pres_Add[630902201] = "503011001 330";
$Pres_Add[630903201] = "503011001 680";
$Pres_Add[630904201] = "503011002 1000";
$Pres_Add[630905201] = "503011002 1490";
$Pres_Add[631002001] = "503011001 280";
$Pres_Add[631003001] = "503011001 630";
$Pres_Add[631004001] = "503011001 980";
$Pres_Add[631005001] = "503011002 1420";
$Pres_Add[631006001] = "503011002 1910";
$Pres_Add[631102001] = "503011001 180";
$Pres_Add[631103001] = "503011001 530";
$Pres_Add[631104001] = "503011001 880";
$Pres_Add[631105001] = "503011002 1280";
$Pres_Add[631106001] = "503011002 1770";
$Pres_Add[631201401] = "503011001 30";
$Pres_Add[631202401] = "503011001 380";
$Pres_Add[631203401] = "503011001 730";
$Pres_Add[631204401] = "503011002 1070";
$Pres_Add[631205401] = "503011002 1560";
$Pres_Add[631302001] = "503011001 230";
$Pres_Add[631303001] = "503011001 580";
$Pres_Add[631304001] = "503011001 930";
$Pres_Add[631305001] = "503011002 1350";
$Pres_Add[631306001] = "503011002 1840";
$Pres_Add[631401801] = "503011001 130";
$Pres_Add[631402801] = "503011001 480";
$Pres_Add[631403801] = "503011001 830";
$Pres_Add[631404801] = "503011002 1210";
$Pres_Add[631405801] = "503011002 1700";
$Pres_Add[631503001] = "504011001 500";
$Pres_Add[631504001] = "504011002 1000";
$Pres_Add[631505001] = "504011002 1500";
$Pres_Add[631506001] = "504011003 2000";
$Pres_Add[631603001] = "504011001 500";
$Pres_Add[631604001] = "504011002 1000";
$Pres_Add[631605001] = "504011002 1500";
$Pres_Add[631606001] = "504011003 2000";
$Pres_Add[631703001] = "504011001 500";
$Pres_Add[631704001] = "504011002 1000";
$Pres_Add[631705001] = "504011002 1500";
$Pres_Add[631706001] = "504011003 2000";


//$Pres_Add[
//$Pres_Add[
//$Pres_Add[
//$Pres_Add[
//$Pres_Add[
//$Pres_Add[
//$Pres_Add[
//$Pres_Add[
//$Pres_Add[
//$Pres_Add[
//$Pres_Add[
//$Pres_Add[

//■■■■■■■■■■■各配方学习需求■■■■■■■■■■■■■■■■■■■■■
