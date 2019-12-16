//��������_��
$PlayerFaceShape_Boy[0] = 21091;
$PlayerFaceShape_Boy[1] = 21092; 
$PlayerFaceShape_Boy[2] = 21093;
$PlayerFaceShape_Boy[3] = 21094;
$PlayerFaceShape_Boy[4] = 21095;
$PlayerFaceShape_Boy[5] = 21096;

//������������_��
$PlayerFaceName_Boy[0] = "�����1";
$PlayerFaceName_Boy[1] = "�����2";
$PlayerFaceName_Boy[2] = "�����3";
$PlayerFaceName_Boy[3] = "�����4";
$PlayerFaceName_Boy[4] = "�����5";
$PlayerFaceName_Boy[5] = "�����6";
			
//��������_Ů
$PlayerFaceShape_Girl[0] = 21591;
$PlayerFaceShape_Girl[1] = 21592;
$PlayerFaceShape_Girl[2] = 21593;
$PlayerFaceShape_Girl[3] = 21594;
$PlayerFaceShape_Girl[4] = 21595;
$PlayerFaceShape_Girl[5] = 21596;

//������������_Ů
$PlayerFaceName_Girl[0] = "Ů���1";
$PlayerFaceName_Girl[1] = "Ů���2";
$PlayerFaceName_Girl[2] = "Ů���3";
$PlayerFaceName_Girl[3] = "Ů���4";
$PlayerFaceName_Girl[4] = "Ů���5";
$PlayerFaceName_Girl[5] = "Ů���6";
	
//���﷢������_��
$PlayerHairName_Boy[0] = "�з���1";
$PlayerHairName_Boy[1] = "�з���2";
$PlayerHairName_Boy[2] = "�з���3";
$PlayerHairName_Boy[3] = "�з���4";
$PlayerHairName_Boy[4] = "�з���5";
$PlayerHairName_Boy[5] = "�з���6";
		
//���﷢������_Ů
$PlayerHairName_Girl[0] = "Ů����1";
$PlayerHairName_Girl[1] = "Ů����2";
$PlayerHairName_Girl[2] = "Ů����3";
$PlayerHairName_Girl[3] = "Ů����4";
$PlayerHairName_Girl[4] = "Ů����5";
$PlayerHairName_Girl[5] = "Ů����6";	

//���﷢��_��
$PlayerHairShape_Boy[0] = 31091;
$PlayerHairShape_Boy[1] = 31092;
$PlayerHairShape_Boy[2] = 31093;
$PlayerHairShape_Boy[3] = 31094;
$PlayerHairShape_Boy[4] = 31095;
$PlayerHairShape_Boy[5] = 31096;

//���﷢��_Ů
$PlayerHairShape_Girl[0] = 31591;
$PlayerHairShape_Girl[1] = 31592;
$PlayerHairShape_Girl[2] = 31593;
$PlayerHairShape_Girl[3] = 31594;
$PlayerHairShape_Girl[4] = 31595;
$PlayerHairShape_Girl[5] = 31596;

//���﷢ɫ
$PlayerHairColorShape[0] = "a";
$PlayerHairColorShape[1] = "b";
$PlayerHairColorShape[2] = "c";
$PlayerHairColorShape[3] = "d";
$PlayerHairColorShape[4] = "e";
$PlayerHairColorShape[5] = "f";

//���﷢ɫ����
$PlayerHairColorName[0] = "��ɫ1";
$PlayerHairColorName[1] = "��ɫ2";
$PlayerHairColorName[2] = "��ɫ3";
$PlayerHairColorName[3] = "��ɫ4";
$PlayerHairColorName[4] = "��ɫ5";
$PlayerHairColorName[5] = "��ɫ6";

//��������_��								   
$PlayerBodyShape_Boy[0] = "ply01_001_3";
$PlayerBodyShape_Boy[1] = "ply01_002_3";
$PlayerBodyShape_Boy[2] = "ply01_003_3";	
//�����װ_��								   
$PlayerApparelShape_Boy[0] = "102029001";
$PlayerApparelShape_Boy[1] = "102029002";
$PlayerApparelShape_Boy[2] = "102029003";	
//�����װ����_��	                                       
$PlayerApparelName_Boy[0] = "�з�װ1";
$PlayerApparelName_Boy[1] = "�з�װ2";
$PlayerApparelName_Boy[2] = "�з�װ3";
//��������_Ů
$PlayerBodyShape_Girl[0] = "ply02_001_3";
$PlayerBodyShape_Girl[1] = "ply02_002_3";
$PlayerBodyShape_Girl[2] = "ply02_003_3";	
//�����װ_Ů								   
$PlayerApparelShape_Girl[0] = "102029004";
$PlayerApparelShape_Girl[1] = "102029005";
$PlayerApparelShape_Girl[2] = "102029006";	                                     
//�����װ����_Ů                                       
$PlayerApparelName_Girl[0] = "Ů��װ1";
$PlayerApparelName_Girl[1] = "Ů��װ2";
$PlayerApparelName_Girl[2] = "Ů��װ3";		

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
