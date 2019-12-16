//===========================================================================================================
// 文档说明:创建人物操作
// 创建时间:2009-3-13
// 创建人: soar
//=========================================================================================================== 
$FirstVisualizeNum = 1;
PlayerFamilyInfo.setcontent("<t>门宗预览</t><b/><b/><t>昆仑宗  金禅寺  蓬莱派  飞花谷  九幽教  山海宗  幻灵宫  天魔门</t><b/><b/><t>玩家可在10级时加入门宗</t>");

function EnterCreatePlayer()
{
	// 载入场景
	CreateObjectView.setScene(Scene_3);

	// 随机男女，1 - 男   2 - 女
	%sex = getRandom(1,2);
	
	%appearanceNum = getRandom(1,6);
	
	if(%sex == 1)
	{
		PlayerSexButton_Man.SetStateOn(true);
		PlayerSexImage_Man.setVisible(1);
		PlayerSexImage_Woman.setVisible(0);
		PlayerSexBack_USMan.setvisible(0);
		PlayerSexBack_USWoman.setvisible(1);
		PlayerSexBack_SMan.setvisible(1);
		PlayerSexBack_SWoman.setvisible(0);
		PlayerFamilySign_3.setActive(0);
		PlayerFamilySign_1.setActive(1);
		PlayerInfo_Face.settext($PlayerFaceName_Boy[$FirstFaceNum]);
		PlayerInfo_Hair.settext($PlayerHairName_Boy[$FirstHairNum]);
		PlayerInfo_HColor.settext($PlayerHairColorName[$FirstHColorNum]);
		PlayerInfo_Apparel.settext($PlayerApparelName_Boy[$FirstApparelNum]);
	}
	else
	{
		PlayerSexButton_Woman.setStateOn(true);
		PlayerSexImage_Man.setVisible(0);
		PlayerSexImage_Woman.setVisible(1);
		PlayerSexBack_USMan.setvisible(1);
		PlayerSexBack_USWoman.setvisible(0);
		PlayerSexBack_SMan.setvisible(0);
		PlayerSexBack_SWoman.setvisible(1);
		PlayerFamilySign_1.setActive(0);
		PlayerFamilySign_3.setActive(1);
		PlayerInfo_Face.settext($PlayerFaceName_Girl[$FirstFaceNum]);
		PlayerInfo_Hair.settext($PlayerHairName_Girl[$FirstHairNum]);
		PlayerInfo_HColor.settext($PlayerHairColorName[$FirstHColorNum]);
		PlayerInfo_Apparel.settext($PlayerApparelName_Girl[$FirstApparelNum]);
	}

	CreatePlayer_SetSex(%sex);
	
	setVisualize(%appearanceNum,0);

	// 载入界面
	Canvas.setContent(CreatePlayerGui);	
}

function CreatePlayer_SetSex(%sex)
{
	if(%sex == 1)
	{
		PlayerSexImage_Man.setVisible(1);
		PlayerSexImage_Woman.setVisible(0);
		PlayerSexBack_USMan.setvisible(0);
		PlayerSexBack_SMan.setvisible(1);
		PlayerSexBack_USWoman.setvisible(1);
		PlayerSexBack_SWoman.setvisible(0);
		PlayerFamilySign_3.setActive(0);
		PlayerFamilySign_1.setActive(1);
		PlayerHeadImg.setbitmap("gameres/gui/images/GUIWindow02_1_021.png");
		$createPlayer_Data = 1000;
	}
	else if(%sex == 2)
	{
		PlayerSexImage_Man.setVisible(0);
		PlayerSexImage_Woman.setVisible(1);
		PlayerSexBack_USMan.setvisible(1);
		PlayerSexBack_SMan.setvisible(0);
		PlayerSexBack_USWoman.setvisible(0);
		PlayerSexBack_SWoman.setvisible(1);
		PlayerFamilySign_3.setActive(1);
		PlayerFamilySign_1.setActive(0);
		PlayerHeadImg.setbitmap("gameres/gui/images/GUIWindow02_1_022.png");
		$createPlayer_Data = 1500;	
	}
	else
		return;
		
	$createPlayer_Sex = %sex;
	
	UsePlayerApparelShape(0,0);
	UsePlayerFaceShape(0,0);
	UsePlayerHairShape(0,0,0,0);

	// 设置动作
	CreateObjectView.SetSeq(0);
}

$autoConnect = false;

function AutoConnect() 
{
	if( $autoConnect == true ) {
		SptSelectPlayer(PlayerListNum()-1);
		SptConnectGameServer();
	}
}

function CreatePlayerOK()
{
	if(InputPlayerName.getText() $= "")
	{
	}
	else
	{
		%name = InputPlayerName.getText();
		SptCreatePlayer(%name,$createPlayer_Sex,$createPlayer_Data,$FirstFaceNum,$FirstHairNum,$FirstHColorNum,$createPlayer_Apparel);
		$autoConnect = true;
		InputPlayerName.settext("");
  }
}

function ClearName()
{
	InputPlayerName.settext("");
}

function OpenServerSelectWnd2()
{
	if(CPRightWnd_ChangeServerWnd.IsVisible())
	{
		//CPRightWnd_PlayerInfoWnd.setvisible(1);
		CPRightWnd_ChangeServerWnd.setvisible(0);
	}
	else
	{
		//CPRightWnd_PlayerInfoWnd.setvisible(0);
		CPRightWnd_ChangeServerWnd.setvisible(1);
	}
}

function OpenPlayerInfoWnd()
{
	CPRightWnd_PlayerInfoWnd.setvisible(1);
	CPRightWnd_ChangeServerWnd.setvisible(0);
}

function OpenPlayerShapeExplainWnd()
{
	PlayerShapeExplainWnd.setvisible(1);
	PlayerShapeImageWnd.setvisible(0);
}

function OpenPlayerShapeImageWnd()
{
	PlayerShapeExplainWnd.setvisible(0);
	PlayerShapeImageWnd.setvisible(1);
}

function ShowPlayerShapeImage(%index)
{
	switch(%index)
	{
		case 0:
			PlayerShapeImage_HColor.setvisible(0);
			$ShapeImageMaxNum = 1;
			for(%i = 0; %i < $ShapeImageMaxNum; %i++)
			{
				if($FirstFaceNum >= %i * 6 && $FirstFaceNum < (%i+1) * 6)
				{
					$ShapeImageNum = %i+1;
				}
			}               
		case 1:
			PlayerShapeImage_HColor.setvisible(0);
			$ShapeImageMaxNum = 1;
			for(%i = 0; %i < $ShapeImageMaxNum; %i++)
			{
				if($FirstHairNum >= %i * 6 && $FirstHairNum < (%i+1) * 6)
				{
					$ShapeImageNum = %i+1;
				}
			}
		case 2:
			PlayerShapeImage_HColor.setvisible(1);
			$ShapeImageMaxNum = 1;
			for(%i = 0; %i < $ShapeImageMaxNum; %i++)
			{
				if($FirstHColorNum >= %i * 6 && $FirstHColorNum < (%i+1) * 6)
				{
					$ShapeImageNum = %i+1;
				}
			}
		case 3:
			PlayerShapeImage_HColor.setvisible(0);
			$ShapeImageMaxNum = 1;
			for(%i = 0; %i < $ShapeImageMaxNum; %i++)
			{
				if($FirstApparelNum >= %i * 6 && $FirstApparelNum < (%i+1) * 6)
				{
					$ShapeImageNum = %i+1;
				}
			}
	}
	$UseShapeType = %index;
	setPlayerShapeImageNum(0);
	setPlayerShapeButtonState();
}

function AddCreatePlayerDistance()
{
	CreateAddButton.setvisible(0);
	CreateDesButton.setvisible(1);
}

function DesCreatePlayerDistance()
{
	CreateAddButton.setvisible(1);
	CreateDesButton.setvisible(0);
}

function GetFamilyInfo( %index )
{
	switch(%index)
	{
		case 1:
			PlayerFamilyInfo.setcontent("<t>昆仑宗</t><b/><t>昆仑宗居于轩辕中州西北外的昆仑古虚中。昆仑古虚巍巍雄踞，山势险陡，共有三十六奇险之处，称为昆仑“三十六天机”，极巅处为昆仑宗。派中建筑大气磅礴，一柄大如山壁的巨剑直插宫殿之心，穿天连地。后方山地插满无数残剑碎斧，乃圣遗弃之器，称为“武冢”。</t>");
		case 2:
			PlayerFamilyInfo.setcontent("<t>金禅寺</t><b/><t>金禅寺位于极乐西天，寺庙正大光明，其中一鼎金钟不撞自响，炸出一片片金色梵文，在空中缓缓飘散。最了解人心的脆弱之处，善于用佛法的奥义创造相应的幻相，使之迷惑。在他们强大的法力面前，敌人往往无法抑止自己的各种执念，陷于迷惘和混乱。</t>");
		case 3:
			PlayerFamilyInfo.setcontent("<t>蓬莱派</t><b/><t>蓬莱仙岛终年烟水弥漫，乃东海内涌的大瀑布中央一座孤岛，岛应天地四象之位，称为四宫仙境，仙岛建筑飘然潇洒，以中心宫殿的“昊天塔”为超然，昊天塔周围布乾坤剑阵、三仙六宫九剑大符，其中九柄仙剑融为一体，乃镇派之宝。</t>");
		case 4:
			PlayerFamilyInfo.setcontent("<t>飞花谷</t><b/><t>飞花谷居于优美雅致的神木林，以一棵参天巨树为心，四周百花簇拥。其巨树乃神木，由上古而生，其树杈枝桠皆灵力浓郁，润泽自然，又有护卫之能。神木林桃花纷飞，美如芳踪，首领“桃娘”统领的精族皆是优雅迷人，以各式植物花草为标志。</t>");
		case 5:
			PlayerFamilyInfo.setcontent("<t>九幽教</t><b/><t>九幽教乃天下极阴之地，大至无边无际，阴风阵阵，黄泉腾腾。此地有树却无叶，有光却无暖意。九幽以镇魂台为心，鬼气连绵不绝。</t>");
		case 6:
			PlayerFamilyInfo.setcontent("<t>山海宗</t><b/><t>山海宗居位于河洛东州之南的双生山，此山嶙峋怪异，沿山壁凿出一座宏伟的宫殿，建筑无不是粗犷豪迈，大气磅礴，集天下河山为一体，乃山海宗。山海宗以女娲遗留下的一部分五彩神石为镇派神器，神石上印记了女娲留下的一句口诀，正印怪之修神。</t>");
		case 7:
			PlayerFamilyInfo.setcontent("<t>幻灵宫</t><b/><t>幻灵宫居于轩辕中州北面的幻幽宫，乃修炼女娲遗留口诀之正宗派系，在“夏姬”领导下，以妖族特有之幻境玄术建立了缥缈梦幻的宫殿。宫殿建筑无一不是迷离绝美，特别以“梦宫”为精绝。梦宫乃存放“梦”的房间，无数的梦在此成为无数的透明泡泡，悬浮于空。</t>");
		case 8:
			PlayerFamilyInfo.setcontent("<t>天魔门</t><b/><t>落夕渊位于大地内部，由天魔一掌击成，意为“沉沦之日，永绝晨曦，即为落夕。”落夕渊之时辰永远停留于夕阳西下之时，处处黄昏。浓密之极的丛林中笔直竖立起一座座料峭山峰，其中以一座天魔像为门宗象征。</t>");
	}
	
}

function OpenShowManBack()
{
	if(PlayerSexButton_Man.IsStateOn())
	{
		
	}
	else
	{
		PlayerSexBack_USMan.setvisible(0);
		PlayerSexBack_SMan.setvisible(1);
	}
}

function OpenShowWomanBack()
{
	if(PlayerSexButton_Woman.IsStateOn())
	{
		
	}
	else
	{
		PlayerSexBack_USWoman.setvisible(0);
		PlayerSexBack_SWoman.setvisible(1);
	}
}

function CloseShowManBack()
{
	if(PlayerSexButton_Man.IsStateOn())
	{
		
	}
	else
	{
		PlayerSexBack_USMan.setvisible(1);
		PlayerSexBack_SMan.setvisible(0);
	}
}

function CloseShowWomanBack()
{
	if(PlayerSexButton_Woman.IsStateOn())
	{
		
	}
	else
	{
		PlayerSexBack_USWoman.setvisible(1);
		PlayerSexBack_SWoman.setvisible(0);
	}
}

function ShowFamilyBaseInfo()
{
	PlayerFamilyInfo.setcontent("<t>门宗预览</t><b/><b/><t>昆仑宗  金禅寺  蓬莱派  飞花谷  九幽教  山海宗  幻灵宫  天魔门</t><b/><b/><t>玩家可在10级时加入门宗</t>");
}

function PlayerSexButton_Man::onMouseEnter(%this)
{
	OpenShowManBack();
}

function PlayerSexButton_Woman::onMouseEnter(%this)
{
	OpenShowWomanBack();
}

// 圣
function PlayerFamilySign_0::onMouseEnter(%this)
{
	GetFamilyInfo(1);
	FamilyFashionPreview($createPlayer_Sex, "102080001");
}

// 佛
function PlayerFamilySign_1::onMouseEnter(%this)
{
	if(PlayerSexButton_Man.IsStateOn() && $createPlayer_Sex == 1)
	{
		GetFamilyInfo(2);
		FamilyFashionPreview($createPlayer_Sex, "102080002");
	}
}

// 仙
function PlayerFamilySign_2::onMouseEnter(%this)
{
	GetFamilyInfo(3);
	FamilyFashionPreview($createPlayer_Sex, "102080003");
}

// 精
function PlayerFamilySign_3::onMouseEnter(%this)
{
	if(PlayerSexButton_Woman.IsStateOn() && $createPlayer_Sex == 2)
	{
		GetFamilyInfo(4);
		FamilyFashionPreview($createPlayer_Sex, "102080004");
	}
}

// 鬼
function PlayerFamilySign_4::onMouseEnter(%this)
{
	GetFamilyInfo(5);
	FamilyFashionPreview($createPlayer_Sex, "102080005");
}

// 怪
function PlayerFamilySign_5::onMouseEnter(%this)
{
	GetFamilyInfo(6);
	FamilyFashionPreview($createPlayer_Sex, "102080006");
}

// 妖
function PlayerFamilySign_6::onMouseEnter(%this)
{
    GetFamilyInfo(7);
    FamilyFashionPreview($createPlayer_Sex, "102080007");
}

// 魔
function PlayerFamilySign_7::onMouseEnter(%this)
{
	GetFamilyInfo(8);
	FamilyFashionPreview($createPlayer_Sex, "102080008");
}

function PlayerSexButton_Man::onMouseLeave(%this)
{
	CloseShowManBack();
}

function PlayerSexButton_Woman::onMouseLeave(%this)
{
	CloseShowWomanBack();
}

function PlayerFamilySign_0::onMouseLeave(%this)
{
	ShowFamilyBaseInfo();
	CreateObjectView.setPlayer("");
	UsePlayerFaceShape(0,0);
	UsePlayerHairShape(0,0,0,0);
	UsePlayerApparelShape(0,0);
}

function PlayerFamilySign_1::onMouseLeave(%this)
{
	ShowFamilyBaseInfo();
	// 佛 女性直接退出
	if(PlayerSexButton_Woman.IsStateOn() && $createPlayer_Sex == 2)
		return;
	CreateObjectView.setPlayer("");
	UsePlayerFaceShape(0,0);
	UsePlayerHairShape(0,0,0,0);
	UsePlayerApparelShape(0,0);
}

function PlayerFamilySign_2::onMouseLeave(%this)
{
	ShowFamilyBaseInfo();
	CreateObjectView.setPlayer("");
	UsePlayerFaceShape(0,0);
	UsePlayerHairShape(0,0,0,0);
	UsePlayerApparelShape(0,0);
}

function PlayerFamilySign_3::onMouseLeave(%this)
{
	ShowFamilyBaseInfo();
	// 精 男性直接退出
	if(PlayerSexButton_Man.IsStateOn() && $createPlayer_Sex == 1)
		return;
	CreateObjectView.setPlayer("");
	UsePlayerFaceShape(0,0);
	UsePlayerHairShape(0,0,0,0);
	UsePlayerApparelShape(0,0);
}

function PlayerFamilySign_4::onMouseLeave(%this)
{
	ShowFamilyBaseInfo();
	CreateObjectView.setPlayer("");
	UsePlayerFaceShape(0,0);
	UsePlayerHairShape(0,0,0,0);
	UsePlayerApparelShape(0,0);
}

function PlayerFamilySign_5::onMouseLeave(%this)
{
	ShowFamilyBaseInfo();
	CreateObjectView.setPlayer("");
	UsePlayerFaceShape(0,0);
	UsePlayerHairShape(0,0,0,0);
	UsePlayerApparelShape(0,0);
}

function PlayerFamilySign_6::onMouseLeave(%this)
{
	ShowFamilyBaseInfo();
	CreateObjectView.setPlayer("");
	UsePlayerFaceShape(0,0);
	UsePlayerHairShape(0,0,0,0);
	UsePlayerApparelShape(0,0);
}

function PlayerFamilySign_7::onMouseLeave(%this)
{
	ShowFamilyBaseInfo();
	CreateObjectView.setPlayer("");
	UsePlayerFaceShape(0,0);
	UsePlayerHairShape(0,0,0,0);
	UsePlayerApparelShape(0,0);
}

function UsePlayerFaceShape(%ChangeIndex,%index)
{
	if(%ChangeIndex != 0)
	{
		$FirstFaceNum = %ChangeIndex - 1;
	}
	else
	{
	  if(%index != 0)
	  {
	  	$FirstFaceNum += %index;
	   	if($FirstFaceNum > $FaceMaxNum)
	   	{
	   		$FirstFaceNum = 0;
	   	}
	   	else if($FirstFaceNum < 0)
	   	{
	   		$FirstFaceNum = $FaceMaxNum;
	   	}
	  } 
	}
	 
	if($createPlayer_Sex == 1)
	{	    
		PlayerInfo_Face.settext($PlayerFaceName_Boy[$FirstFaceNum]);                                                                                                                                        
		CreateObjectView.setMounted(0,$PlayerFaceShape_Boy[$FirstFaceNum],Link01Point,Link01Point);
	}  
	else if($createPlayer_Sex == 2)
	{	    
		PlayerInfo_Face.settext($PlayerFaceName_Girl[$FirstFaceNum]);                                                                                                                                         
		CreateObjectView.setMounted(0,$PlayerFaceShape_Girl[$FirstFaceNum],Link01Point,Link01Point);
	}
	
	// 设置动作
	CreateObjectView.SetSeq(0);
}

function UsePlayerHairShape(%HairChangeIndex,%HColorChangeIndex,%HairIndex,%HColorIndex)
{
	if(%HairChangeIndex != 0)
	{
		$FirstHairNum = %HairChangeIndex - 1;
	}
	else
	{
	  if(%HairIndex != 0)
	  {
	  	$FirstHairNum += %HairIndex;
	   	if($FirstHairNum > $HairMaxNum)
	   	{
	   		$FirstHairNum = 0;
	   	}
	   	else if($FirstHairNum < 0)
	   	{
	   		$FirstHairNum = $HairMaxNum;
	   	}
	  }
	}
	
	if(%HColorChangeIndex != 0)
	{
		$FirstHColorNum = %HColorChangeIndex - 1;
	}
	else
	{
	  if(%HColorIndex != 0)
	  {
	  	$FirstHColorNum += %HColorIndex;
	   	if($FirstHColorNum > $HColorMaxNum)
	   	{
	   		$FirstHColorNum = 0;
	   	}
	   	else if($FirstHColorNum < 0)
	   	{
	   		$FirstHColorNum = $HColorMaxNum;
	   	}
	  }
	}
	 
	 if($createPlayer_Sex == 1)
	 {
	 		PlayerInfo_Hair.settext($PlayerHairName_Boy[$FirstHairNum]); 
	 		PlayerInfo_HColor.settext($PlayerHairColorName[$FirstHColorNum]);  
	 		//$createPlayer_Hair = ($FirstHairNum-1) * 6 + $FirstHColorNum;                       
	 		CreateObjectView.setMounted(1,$PlayerHairShape_Boy[$FirstHairNum],Link01Point,Link01Point,$PlayerHairColorShape[$FirstHColorNum]);
	 }
	 else if($createPlayer_Sex == 2)
	 {	    
	 		PlayerInfo_Hair.settext($PlayerHairName_Girl[$FirstHairNum]); 
	 		PlayerInfo_HColor.settext($PlayerHairColorName[$FirstHColorNum]);  
	 		//$createPlayer_Hair = ($FirstHairNum-1) * 7 + $FirstHColorNum;                       
	 		CreateObjectView.setMounted(1,$PlayerHairShape_Girl[$FirstHairNum],Link01Point,Link01Point,$PlayerHairColorShape[$FirstHColorNum]);
	 }
	 
	 // 设置动作
	CreateObjectView.SetSeq(0);
}

function UsePlayerApparelShape(%ChangeIndex,%index)
{
	if(%ChangeIndex != 0)
	{
		$FirstApparelNum = %ChangeIndex - 1;
	}
	else
	{
	  if(%index != 0)
	  {
	  	$FirstApparelNum += %index;
	   	if($FirstApparelNum > $ApparelMaxNum)
	   	{
	   		$FirstApparelNum = 0;
	   	}
	   	else if($FirstApparelNum < 0)
	   	{
	   		$FirstApparelNum = $ApparelMaxNum;
	   	}
	  }   
	}
	 if($createPlayer_Sex == 1)
	 {	    
	 		PlayerInfo_Apparel.settext($PlayerApparelName_Boy[$FirstApparelNum]);
	    $createPlayer_Apparel = $PlayerApparelShape_Boy[$FirstApparelNum];
	    CreateObjectView.setPlayer($PlayerBodyShape_Boy[$FirstApparelNum]);
	    CreateObjectView.setMounted(0,$PlayerFaceShape_Boy[$FirstFaceNum],Link01Point,Link01Point);
	    CreateObjectView.setMounted(1,$PlayerHairShape_Boy[$FirstHairNum],Link01Point,Link01Point,$PlayerHairColorShape[$FirstHColorNum]);
	 }
	 else if($createPlayer_Sex == 2)
	 {	    
	 		PlayerInfo_Apparel.settext($PlayerApparelName_Girl[$FirstApparelNum]);
	    $createPlayer_Apparel = $PlayerApparelShape_Girl[$FirstApparelNum];
	    CreateObjectView.setPlayer($PlayerBodyShape_Girl[$FirstApparelNum]);
	    CreateObjectView.setMounted(0,$PlayerFaceShape_Girl[$FirstFaceNum],Link01Point,Link01Point);
	    CreateObjectView.setMounted(1,$PlayerHairShape_Girl[$FirstHairNum],Link01Point,Link01Point,$PlayerHairColorShape[$FirstHColorNum]);
	 }
	 
	 // 设置动作
	CreateObjectView.SetSeq(0);
}

function setVisualize(%ChangeIndex,%index)
{
	if(%ChangeIndex != 0)
	{
		$FirstVisualizeNum = %ChangeIndex;
	}
	else
	{
		if(%index != 0)
	  {
	  	$FirstVisualizeNum += %index;
	   	if($FirstVisualizeNum > 6)
	   	{
	   		$FirstVisualizeNum = 1;
	   	}
	   	else if($FirstVisualizeNum < 1)
	   	{
	   		$FirstVisualizeNum = 6;
	   	}
	  }
	}
	
	PlayerShapeName.settext("形象" @ $FirstVisualizeNum);
	if($createPlayer_Sex == 1)
	{
		switch($FirstVisualizeNum)
	  {
	  	case 1:
	  		UsePlayerFaceShape(1,0);
	  		UsePlayerHairShape(1,0,0,0);
	  		UsePlayerHairShape(0,1,0,0);
	  		UsePlayerApparelShape(1,0);
	  	case 2:
	  		UsePlayerFaceShape(2,0);
	  		UsePlayerHairShape(2,0,0,0);
	  		UsePlayerHairShape(0,2,0,0);
	  		UsePlayerApparelShape(2,0);
	  	case 3:
	  		UsePlayerFaceShape(3,0);
	  		UsePlayerHairShape(3,0,0,0);
	  		UsePlayerHairShape(0,3,0,0);
	  		UsePlayerApparelShape(3,0);
	  	case 4:
	  		UsePlayerFaceShape(4,0);
	  		UsePlayerHairShape(4,0,0,0);
	  		UsePlayerHairShape(0,4,0,0);
	  		UsePlayerApparelShape(1,0);
	  	case 5:
	  		UsePlayerFaceShape(5,0);
	  		UsePlayerHairShape(5,0,0,0);
	  		UsePlayerHairShape(0,5,0,0);
	  		UsePlayerApparelShape(2,0);
	  	case 6:
	  		UsePlayerFaceShape(6,0);
	  		UsePlayerHairShape(6,0,0,0);
	  		UsePlayerHairShape(0,6,0,0);
	  		UsePlayerApparelShape(3,0);
	  }
	}
	else if($createPlayer_Sex == 2)
	{
		switch($FirstVisualizeNum)
	  {
	  	case 1:
	  		UsePlayerFaceShape(1,0);
	  		UsePlayerHairShape(1,0,0,0);
	  		UsePlayerHairShape(0,1,0,0);
	  		UsePlayerApparelShape(1,0);
	  	case 2:
	  		UsePlayerFaceShape(2,0);
	  		UsePlayerHairShape(2,0,0,0);
	  		UsePlayerHairShape(0,2,0,0);
	  		UsePlayerApparelShape(2,0);
	  	case 3:
	  		UsePlayerFaceShape(3,0);
	  		UsePlayerHairShape(3,0,0,0);
	  		UsePlayerHairShape(0,3,0,0);
	  		UsePlayerApparelShape(3,0);
	  	case 4:
	  		UsePlayerFaceShape(4,0);
	  		UsePlayerHairShape(4,0,0,0);
	  		UsePlayerHairShape(0,4,0,0);
	  		UsePlayerApparelShape(1,0);
	  	case 5:
	  		UsePlayerFaceShape(5,0);
	  		UsePlayerHairShape(5,0,0,0);
	  		UsePlayerHairShape(0,5,0,0);
	  		UsePlayerApparelShape(2,0);
	  	case 6:
	  		UsePlayerFaceShape(6,0);
	  		UsePlayerHairShape(6,0,0,0);
	  		UsePlayerHairShape(0,6,0,0);
	  		UsePlayerApparelShape(3,0);
	  }
	}
	// 设置动作
	CreateObjectView.SetSeq(0);
}

function setSelectPlayerShape(%index)
{
	%buttonNum = ($ShapeImageNum -1) * 6 + %index;
	switch($UseShapeType)
	{
		case 0:
			UsePlayerFaceShape(%buttonNum,0);
		case 1:
			UsePlayerHairShape(%buttonNum,0,0,0);
		case 2:
			UsePlayerHairShape(0,%buttonNum,0,0);
		case 3:
			UsePlayerApparelShape(%buttonNum,0);
	}
}

function setPlayerShapeImageNum(%index)
{
	if(%index != 0)
	{
	  $ShapeImageNum += %index;
	  if($ShapeImageNum > $ShapeImageMaxNum)
	  {
	  	$ShapeImageNum = 1;
	  }
	  else if($ShapeImageNum < 1)
	  {
	  	$ShapeImageNum = $ShapeImageMaxNum;
	  }  
	}
	ShapeNumber.settext($ShapeImageNum @ "/" @ $ShapeImageMaxNum);
	setPlayerShapeButtonState();
}

function setPlayerShapeButtonState()
{
	switch($UseShapeType)
	{
		case 0:
			%num = $FaceMaxNum + 1;
		case 1:
			%num = $HairMaxNum + 1;
		case 2:
			%num = $HColorMaxNum + 1;
		case 3:
			%num = $ApparelMaxNum + 1;
	}

	if($ShapeImageNum == $ShapeImageMaxNum && $ShapeImageMaxNum * 6 > %num)
	{
		%temp = $ShapeImageMaxNum * 6 - %num;
		%index = 6;
		for(%i = 0; %i < %temp; %i++)
		{
			("SelectPlayerShapeButton_" @ %index).setActive(0);
			%index--;
		}
	}
	else
	{
		for(%i = 1; %i <= 6; %i++)
		{
			("SelectPlayerShapeButton_" @ %i).setActive(1);
		}
	}
}

function getCreateFace(%sex, %face)
{
	if(%face < 0 || %face > $FaceMaxNum)
		%face = 0;
	if(%sex == 1)
	{
		return $PlayerFaceShape_Boy[%face];
	}
	else
	{
		return $PlayerFaceShape_Girl[%face];
	}
}

function getCreateHair(%sex, %hair)
{
	if(%hair < 0 || %hair > $HairMaxNum)
		%hair = 0;
		
	if(%sex == 1)
	{
		return $PlayerHairShape_Boy[%hair];
	}
	else
	{
		return $PlayerHairShape_Girl[%hair];
	}
}

function getCreateHairColor(%color)
{
	if(%color < 0 || %color > $HColorMaxNum)
		%color = 0;

	return $PlayerHairColorShape[%color];

}

function ShowUserDefinedInfo()
{
	PlayerShapeName.settext("自定义");
}

function RandomSelectAppearance()
{
	UsePlayerFaceShape(getRandom(1,$FaceMaxNum + 1),0);
	UsePlayerHairShape(getRandom(1,$HairMaxNum + 1),0,0,0);
	UsePlayerHairShape(0,getRandom(1,$HColorMaxNum + 1),0,0);
	UsePlayerApparelShape(getRandom(1,$ApparelMaxNum + 1),0);
	OpenPlayerShapeExplainWnd();
	ShowUserDefinedInfo();
}

