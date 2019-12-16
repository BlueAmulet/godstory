//===========================================================================================================
// �ĵ�˵��:�����������
// ����ʱ��:2009-3-13
// ������: soar
//=========================================================================================================== 
$FirstVisualizeNum = 1;
PlayerFamilyInfo.setcontent("<t>����Ԥ��</t><b/><b/><t>������  ������  ������  �ɻ���  ���Ľ�  ɽ����  ���鹬  ��ħ��</t><b/><b/><t>��ҿ���10��ʱ��������</t>");

function EnterCreatePlayer()
{
	// ���볡��
	CreateObjectView.setScene(Scene_3);

	// �����Ů��1 - ��   2 - Ů
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

	// �������
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

	// ���ö���
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
			PlayerFamilyInfo.setcontent("<t>������</t><b/><t>�����ھ�����ԯ��������������ع����С����ع���ΡΡ�۾ᣬɽ���ն���������ʮ������֮������Ϊ���ء���ʮ������������۴�Ϊ�����ڡ����н����������磬һ������ɽ�ڵľ޽�ֱ�幬��֮�ģ��������ء���ɽ�ز��������н��鸫����ʥ����֮������Ϊ����ڣ����</t>");
		case 2:
			PlayerFamilyInfo.setcontent("<t>������</t><b/><t>������λ�ڼ������죬�����������������һ�����Ӳ�ײ���죬ը��һƬƬ��ɫ���ģ��ڿ��л���Ʈɢ�����˽����ĵĴ���֮���������÷𷨵İ��崴����Ӧ�Ļ��࣬ʹ֮�Ի�������ǿ��ķ�����ǰ�����������޷���ֹ�Լ��ĸ���ִ�������㯺ͻ��ҡ�</t>");
		case 3:
			PlayerFamilyInfo.setcontent("<t>������</t><b/><t>�����ɵ�������ˮ�������˶�����ӿ�Ĵ��ٲ�����һ���µ�����Ӧ�������֮λ����Ϊ�Ĺ��ɾ����ɵ�����ƮȻ�����������Ĺ���ġ��������Ϊ��Ȼ���������Χ��Ǭ���������������Ž���������оű��ɽ���Ϊһ�壬������֮����</t>");
		case 4:
			PlayerFamilyInfo.setcontent("<t>�ɻ���</t><b/><t>�ɻ��Ⱦ����������µ���ľ�֣���һ�ò������Ϊ�ģ����ܰٻ���ӵ�����������ľ�����ϹŶ����������֦�������Ũ����������Ȼ�����л���֮�ܡ���ľ���һ��׷ɣ����緼�٣����조���ͳ��ľ�������������ˣ��Ը�ʽֲ�ﻨ��Ϊ��־��</t>");
		case 5:
			PlayerFamilyInfo.setcontent("<t>���Ľ�</t><b/><t>���Ľ������¼���֮�أ������ޱ��޼ʣ��������󣬻�Ȫ���ڡ��˵�����ȴ��Ҷ���й�ȴ��ů�⡣���������̨Ϊ�ģ��������಻����</t>");
		case 6:
			PlayerFamilyInfo.setcontent("<t>ɽ����</t><b/><t>ɽ���ھ�λ�ں��嶫��֮�ϵ�˫��ɽ����ɽ��Ᾱ��죬��ɽ�����һ����ΰ�Ĺ�������޲��Ǵ���������������磬�����º�ɽΪһ�壬��ɽ���ڡ�ɽ������Ů������µ�һ���������ʯΪ������������ʯ��ӡ����Ů����µ�һ��ھ�����ӡ��֮����</t>");
		case 7:
			PlayerFamilyInfo.setcontent("<t>���鹬</t><b/><t>���鹬������ԯ���ݱ���Ļ��Ĺ���������Ů������ھ�֮������ϵ���ڡ��ļ����쵼�£�����������֮�þ���������������λõĹ���������һ��������������ر��ԡ��ι���Ϊ�������ι��˴�š��Ρ��ķ��䣬���������ڴ˳�Ϊ������͸�����ݣ������ڿա�</t>");
		case 8:
			PlayerFamilyInfo.setcontent("<t>��ħ��</t><b/><t>��ϦԨλ�ڴ���ڲ�������ħһ�ƻ��ɣ���Ϊ������֮�գ��������أ���Ϊ��Ϧ������ϦԨ֮ʱ����Զͣ����Ϧ������֮ʱ�������ƻ衣Ũ��֮���Ĵ����б�ֱ������һ��������ɽ�壬������һ����ħ��Ϊ����������</t>");
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
	PlayerFamilyInfo.setcontent("<t>����Ԥ��</t><b/><b/><t>������  ������  ������  �ɻ���  ���Ľ�  ɽ����  ���鹬  ��ħ��</t><b/><b/><t>��ҿ���10��ʱ��������</t>");
}

function PlayerSexButton_Man::onMouseEnter(%this)
{
	OpenShowManBack();
}

function PlayerSexButton_Woman::onMouseEnter(%this)
{
	OpenShowWomanBack();
}

// ʥ
function PlayerFamilySign_0::onMouseEnter(%this)
{
	GetFamilyInfo(1);
	FamilyFashionPreview($createPlayer_Sex, "102080001");
}

// ��
function PlayerFamilySign_1::onMouseEnter(%this)
{
	if(PlayerSexButton_Man.IsStateOn() && $createPlayer_Sex == 1)
	{
		GetFamilyInfo(2);
		FamilyFashionPreview($createPlayer_Sex, "102080002");
	}
}

// ��
function PlayerFamilySign_2::onMouseEnter(%this)
{
	GetFamilyInfo(3);
	FamilyFashionPreview($createPlayer_Sex, "102080003");
}

// ��
function PlayerFamilySign_3::onMouseEnter(%this)
{
	if(PlayerSexButton_Woman.IsStateOn() && $createPlayer_Sex == 2)
	{
		GetFamilyInfo(4);
		FamilyFashionPreview($createPlayer_Sex, "102080004");
	}
}

// ��
function PlayerFamilySign_4::onMouseEnter(%this)
{
	GetFamilyInfo(5);
	FamilyFashionPreview($createPlayer_Sex, "102080005");
}

// ��
function PlayerFamilySign_5::onMouseEnter(%this)
{
	GetFamilyInfo(6);
	FamilyFashionPreview($createPlayer_Sex, "102080006");
}

// ��
function PlayerFamilySign_6::onMouseEnter(%this)
{
    GetFamilyInfo(7);
    FamilyFashionPreview($createPlayer_Sex, "102080007");
}

// ħ
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
	// �� Ů��ֱ���˳�
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
	// �� ����ֱ���˳�
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
	
	// ���ö���
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
	 
	 // ���ö���
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
	 
	 // ���ö���
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
	
	PlayerShapeName.settext("����" @ $FirstVisualizeNum);
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
	// ���ö���
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
	PlayerShapeName.settext("�Զ���");
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

