//人物脸型_男
$PlayerFaceShape_Boy[0] = 21091;
$PlayerFaceShape_Boy[1] = 21092; 
$PlayerFaceShape_Boy[2] = 21093;
$PlayerFaceShape_Boy[3] = 21094;
$PlayerFaceShape_Boy[4] = 21095;
$PlayerFaceShape_Boy[5] = 21096;

//人物脸型名称_男
$PlayerFaceName_Boy[0] = "男五官1";
$PlayerFaceName_Boy[1] = "男五官2";
$PlayerFaceName_Boy[2] = "男五官3";
$PlayerFaceName_Boy[3] = "男五官4";
$PlayerFaceName_Boy[4] = "男五官5";
$PlayerFaceName_Boy[5] = "男五官6";
			
//人物脸型_女
$PlayerFaceShape_Girl[0] = 21591;
$PlayerFaceShape_Girl[1] = 21592;
$PlayerFaceShape_Girl[2] = 21593;
$PlayerFaceShape_Girl[3] = 21594;
$PlayerFaceShape_Girl[4] = 21595;
$PlayerFaceShape_Girl[5] = 21596;

//人物脸型名称_女
$PlayerFaceName_Girl[0] = "女五官1";
$PlayerFaceName_Girl[1] = "女五官2";
$PlayerFaceName_Girl[2] = "女五官3";
$PlayerFaceName_Girl[3] = "女五官4";
$PlayerFaceName_Girl[4] = "女五官5";
$PlayerFaceName_Girl[5] = "女五官6";
	
//人物发型名称_男
$PlayerHairName_Boy[0] = "男发型1";
$PlayerHairName_Boy[1] = "男发型2";
$PlayerHairName_Boy[2] = "男发型3";
$PlayerHairName_Boy[3] = "男发型4";
$PlayerHairName_Boy[4] = "男发型5";
$PlayerHairName_Boy[5] = "男发型6";
		
//人物发型名称_女
$PlayerHairName_Girl[0] = "女发型1";
$PlayerHairName_Girl[1] = "女发型2";
$PlayerHairName_Girl[2] = "女发型3";
$PlayerHairName_Girl[3] = "女发型4";
$PlayerHairName_Girl[4] = "女发型5";
$PlayerHairName_Girl[5] = "女发型6";	

//人物发型_男
$PlayerHairShape_Boy[0] = 31091;
$PlayerHairShape_Boy[1] = 31092;
$PlayerHairShape_Boy[2] = 31093;
$PlayerHairShape_Boy[3] = 31094;
$PlayerHairShape_Boy[4] = 31095;
$PlayerHairShape_Boy[5] = 31096;

//人物发型_女
$PlayerHairShape_Girl[0] = 31591;
$PlayerHairShape_Girl[1] = 31592;
$PlayerHairShape_Girl[2] = 31593;
$PlayerHairShape_Girl[3] = 31594;
$PlayerHairShape_Girl[4] = 31595;
$PlayerHairShape_Girl[5] = 31596;

//人物发色
$PlayerHairColorShape[0] = "a";
$PlayerHairColorShape[1] = "b";
$PlayerHairColorShape[2] = "c";
$PlayerHairColorShape[3] = "d";
$PlayerHairColorShape[4] = "e";
$PlayerHairColorShape[5] = "f";

//人物发色名称
$PlayerHairColorName[0] = "发色1";
$PlayerHairColorName[1] = "发色2";
$PlayerHairColorName[2] = "发色3";
$PlayerHairColorName[3] = "发色4";
$PlayerHairColorName[4] = "发色5";
$PlayerHairColorName[5] = "发色6";

//人物身体_男								   
$PlayerBodyShape_Boy[0] = "ply01_001_3";
$PlayerBodyShape_Boy[1] = "ply01_002_3";
$PlayerBodyShape_Boy[2] = "ply01_003_3";	
//人物服装_男								   
$PlayerApparelShape_Boy[0] = "102029001";
$PlayerApparelShape_Boy[1] = "102029002";
$PlayerApparelShape_Boy[2] = "102029003";	
//人物服装名称_男	                                       
$PlayerApparelName_Boy[0] = "男服装1";
$PlayerApparelName_Boy[1] = "男服装2";
$PlayerApparelName_Boy[2] = "男服装3";
//人物身体_女
$PlayerBodyShape_Girl[0] = "ply02_001_3";
$PlayerBodyShape_Girl[1] = "ply02_002_3";
$PlayerBodyShape_Girl[2] = "ply02_003_3";	
//人物服装_女								   
$PlayerApparelShape_Girl[0] = "102029004";
$PlayerApparelShape_Girl[1] = "102029005";
$PlayerApparelShape_Girl[2] = "102029006";	                                     
//人物服装名称_女                                       
$PlayerApparelName_Girl[0] = "女服装1";
$PlayerApparelName_Girl[1] = "女服装2";
$PlayerApparelName_Girl[2] = "女服装3";		

$FirstFaceNum = 0;			
$FirstHairNum = 0;
$FirstHColorNum = 0;
$FirstApparelNum = 0;			 

$FaceMaxNum = 5;
$HairMaxNum = 5;
$HColorMaxNum = 5;
$ApparelMaxNum = 2;


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
