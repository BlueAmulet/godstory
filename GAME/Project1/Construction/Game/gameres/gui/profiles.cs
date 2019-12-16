//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

$Gui::fontCacheDirectory = getPrefsPath("fonts");

// If we got back no prefs path modification
if( $Gui::fontCacheDirectory $= "fonts" )
{
   $Gui::fontCacheDirectory = expandFilename( "fonts" );
}

//---------------------------------------------------------------------------------------------
// GuiDefaultProfile is a special profile that all other profiles inherit defaults from. It
// must exist.
//---------------------------------------------------------------------------------------------
if(!isObject(GuiDefaultProfile)) new GuiControlProfile (GuiDefaultProfile)
{
   tab = false;
   canKeyFocus = false;
   hasBitmapArray = false;
   mouseOverSelected = false;

   // fill color
   opaque = false;
   fillColor = "127 136 153";
   fillColorHL = "197 202 211";
   fillColorNA = "144 154 171";

   // border color
   border = 0;
   borderColor   = "0 0 0"; 
   borderColorHL = "197 202 211";
   borderColorNA = "91 101 119";

   // font
   fontType = "����";
   fontSize = 12;
   fontBold = false;
   fontItalic  = false;
   bitmapFont  = true;
   fontOutline = "0 0 0";
   fontCharset = ANSI;

   fontColor = "0 0 0";
   fontColorHL = "23 32 47";
   fontColorNA = "0 0 0";
   fontColorSEL= "126 137 155";

   // bitmap information
   bitmap = "";
   bitmapBase = "";
   textOffset = "0 0";

   // used by guiTextControl
   modal = true;
   justify = "left";
   autoSizeWidth = false;
   autoSizeHeight = false;
   returnTab = false;
   numbersOnly = false;
   cursorColor = "0 0 0 255";

   // sounds
   soundButtonDown = "";
   soundButtonOver = "";
};

if(!isObject(GuiSolidDefaultProfile)) new GuiControlProfile (GuiSolidDefaultProfile)
{
   opaque = true;
   border = true;
};

if(!isObject(GuiTransparentProfile)) new GuiControlProfile (GuiTransparentProfile)
{
   opaque = false;
   border = false;
};


if(!isObject(GuiToolTipProfile)) new GuiControlProfile (GuiToolTipProfile)
{
   // fill color
   fillColor = "239 237 222";

   // border color
   borderColor   = "138 134 122";

   // font
   fontType = "����";
   fontSize = 14;
   fontColor = "0 0 0";

};

if(!isObject(GuiModelessDialogProfile)) new GuiControlProfile("GuiModelessDialogProfile")
{
   modal = false;
};

if(!isObject(GuiFrameSetProfile)) new GuiControlProfile (GuiFrameSetProfile)
{
   fillColor = GuiDefaultProfile.fillColorNA;
   borderColor   = GuiDefaultProfile.borderColorNA;
   opaque = true;
   border = true;
};


if(!isObject(GuiWindowProfile)) new GuiControlProfile (GuiWindowProfile)
{
   opaque = true;
   border = 2;
   fillColor = "145 154 171";
   fillColorHL = "221 202 173";
   fillColorNA = "221 202 173";
   fontColor = "255 255 255";
   fontColorHL = "255 255 255";
   text = "untitled";
   bitmap = "./images/window";
   textOffset = "6 6";
   hasBitmapArray = true;
   justify = "center";
};

//----------------------------------------------------ͨ�ô��ڽ���--------------------------------------------------
//ͨ��Window����
if(!isObject(GuiGeneralDialogProfile_1)) new GuiControlProfile (GuiGeneralDialogProfile_1)
{
   opaque = true;
   border = 0;
   fillColor = "86 99 70 204";
   text = "untitled";
   bitmap = "./images/GUIWindow99_1_001";
   hasBitmapArray = true;
   justify = "center";
   textoffset = "25 23";
};

//������洰��
if(!isObject(GuiMissionWindowProfile)) new GuiControlProfile (GuiMissionWindowProfile)
{
   opaque = true;
   border = 0;
   fillColor = "192 208 190";
   text = "untitled";
   bitmap = "./images/GUIWindow27_1_001";
   hasBitmapArray = true;
   justify = "center";
   textoffset = "30 30";
};

//------------------------------------------------------------------------------------------------------------------

if(!isObject(GuiContentProfile)) new GuiControlProfile (GuiContentProfile)
{
   opaque = true;
   fillColor = "255 255 255";
};

if(!isObject(GuiBlackContentProfile)) new GuiControlProfile (GuiBlackContentProfile)
{
   opaque = true;
   fillColor = "0 0 0";
};

if(!isObject(GuiInputCtrlProfile)) new GuiControlProfile( GuiInputCtrlProfile )
{
   tab = false;
   canKeyFocus = true;
};

if(!isObject(GuiTextProfile)) new GuiControlProfile (GuiTextProfile)
{
	 justify = "center";
	 fontType = "����";
	 fontSize = 14;
   fontColor = "255 255 255";
};

if(!isObject(GuiTextProfile1)) new GuiControlProfile (GuiTextProfile1)
{
	 justify = "center";
	 fontType = "����";
	 fontSize = 12;
   fontColor = "255 255 255";
};

//---------------------------------------------------��ʾ����------------------------------------------------------

//��ʾ��Ϣͨ������1
if(!isObject(GuiCurrencyShowInfoTextProfile_1)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_1)
{
	 justify = "center";
	 fontType = "����";
	 fontSize = 12;
	 modal = false;
   fontColor = "255 255 255";
   bitmapFont = true;
};

//��ʾ��Ϣͨ������2
if(!isObject(GuiCurrencyShowInfoTextProfile_2)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_2)
{
	 justify = "center";
	 fontType = "����";
   fontSize = 14;
   modal = false;
   fontColor = "255 255 255";
};

//��ʾ��Ϣͨ������3
if(!isObject(GuiCurrencyShowInfoTextProfile_3)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_3)
{
	 justify = "center";
	 fontType = "����";
   fontSize = 10;
   modal = false;
   fontColor = "255 255 255";
};

//��ʾ��Ϣͨ������4
if(!isObject(GuiCurrencyShowInfoTextProfile_4)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_4)
{
	 justify = "left";
	 fontType = "����";
   fontSize = 12;
   //modal = false;
   fontColor = "255 255 255";
};

//ͨ�ô��ڱ���1
if(!isObject(GuiCurrencyWndNameTextProfile_1)) new GuiControlProfile (GuiCurrencyWndNameTextProfile_1)
{
	 justify = "center";
	 fontType = "����_GB2312";
	 fontSize = 20;
	 modal = false;
   fontColor = "234 251 197";
   bitmapfont = false;
};

if(!isObject(GuiTextProfile5)) new GuiControlProfile (GuiTextProfile5)
{
	 justify = "right";
	 fontType = "����";
   fontSize = 12;
   modal = false;
   fontColor = "255 255 255";
};

//��ʾ��Ϣͨ������5
if(!isObject(GuiCurrencyShowInfoTextProfile_5)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_5)
{
	 justify = "center";
	 fontType = "����";
	 fontSize = 12;
	 //modal = false;
   fontColor = "0 0 0";
};

//��ʾ��Ϣͨ������6
if(!isObject(GuiCurrencyShowInfoTextProfile_6)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_6)
{
	 justify = "left";
	 fontType = "����";
   fontSize = 10;
   modal = false;
   fontColor = "255 255 255";
};

//��ʾ��Ϣͨ������7
if(!isObject(GuiCurrencyShowInfoTextProfile_7)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_7)
{
	 justify = "right";
	 fontType = "����";
   fontSize = 10;
   modal = false;
   fontColor = "255 255 255";
};

//��ʾ��Ϣͨ������8
if(!isObject(GuiCurrencyShowInfoTextProfile_8)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_8)
{
	 justify = "center";
	 fontType = "����";
   fontSize = 12;
   modal = false;
   fontColor = "255 255 255";
   bitmapFont = false;
};

//��ʾ��Ϣͨ������9
if(!isObject(GuiCurrencyShowInfoTextProfile_9)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_9)
{
	 justify = "left";
	 fontType = "����";
	 fontSize = 14;
   fontColor = "255 255 255";
};

//��ʾ��Ϣͨ������10
if(!isObject(GuiCurrencyShowInfoTextProfile_10)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_10)
{
	 justify = "left";
	 fontType = "����_GB2312";
	 fontSize = 14;
   fontColor = "255 255 255";
   bitmapFont = false;
};

//��ʾ��Ϣͨ������11
if(!isObject(GuiCurrencyShowInfoTextProfile_11)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_11)
{
	 justify = "center";
	 fontType = "����_GB2312";
   fontSize = 18;
   modal = false;
   fontColor = "250 250 0";
   bitmapfont = false;
};


//��ʾ��Ϣͨ������12
if(!isObject(GuiCurrencyShowInfoTextProfile_12)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_12)
{
	 justify = "center";
	 fontType = "����_GB2312";
   fontSize = 16;
   modal = false;
   fontColor = "229 229 229";
   bitmapFont = false;
};

//��ʾ��Ϣͨ������13
if(!isObject(GuiCurrencyShowInfoTextProfile_13)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_13)
{
	 justify = "center";
	 fontType = "����";
   fontSize = 12;
   modal = false;
   fontColor = "255 0 0";
};

//��ʾ��Ϣͨ������14
if(!isObject(GuiCurrencyShowInfoTextProfile_14)) new GuiControlProfile (GuiCurrencyShowInfoTextProfile_14)
{
	 justify = "left";
	 fontType = "����";
	 fontSize = 12;
   fontColor = "0 0 0";
};

//-----------------------------------------------------------------------------------------------------------

if(!isObject(GuiAutoSizeTextProfile)) new GuiControlProfile (GuiAutoSizeTextProfile)
{
   fontColor = "0 0 0";
   autoSizeWidth = true;
   autoSizeHeight = true;   
};

if(!isObject(GuiTextRightProfile)) new GuiControlProfile (GuiTextRightProfile : GuiTextProfile)
{
   justify = "right";
};

if(!isObject(GuiMLTextProfile)) new GuiControlProfile ("GuiMLTextProfile")
{
   fontColorLink = "255 96 96";
   fontColorLinkHL = "0 0 255";
   autoSizeWidth = true;
   autoSizeHeight = true;  
   border = false;
};

if(!isObject(GuiMLTextProfile2)) new GuiControlProfile ("GuiMLTextProfile2")
{
	 fontType = "����_GB2312";
   fontSize = 16;
	 fontColor = "255 255 255";
	 autoSizeWidth = true;
   autoSizeHeight = true;
   border = false;
};

if(!isObject(GuiTextArrayProfile)) new GuiControlProfile (GuiTextArrayProfile : GuiTextProfile)
{
   fontColorHL = "32 100 100";
   fillColorHL = "200 200 200";
   border = false;
};

if(!isObject(GuiTextListProfile)) new GuiControlProfile (GuiTextListProfile : GuiTextProfile) 
{
	 fontType = "����";
   fontSize = 12;
   justify = "left";
   fontColor = "255 255 255";
   tab = false;
   canKeyFocus = true;
};

if(!isObject(GuiTextListProfile_1)) new GuiControlProfile (GuiTextListProfile_1 : GuiTextProfile) 
{
	 fontType = "����";
   fontSize = 12;
   justify = "center";
   fontColor = "255 255 255";
   fontColorHL = "255 255 0";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   tab = false;
   canKeyFocus = true;
};

if(!isObject(GuiTextEditProfile)) new GuiControlProfile (GuiTextEditProfile)
{
   opaque = true;
   fillColor = "255 255 255";
   fillColorHL = "128 128 128";
   border = -2;
   bitmap = "./images/textEdit";
   borderColor = "40 40 40 100";
   fontColor = "0 0 0";
   fontColorHL = "255 255 255";
   fontColorNA = "128 128 128";
   textOffset = "4 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = false;
   canKeyFocus = true;
   
};

//-----------------------------------------------------���������----------------------------------------------------

//ͨ�������1_ֻ��������
if(!isObject(GuiCurrencyTextEditProfile_1)) new GuiControlProfile (GuiCurrencyTextEditProfile_1)
{
	 opaque = false;
	 border = -2;
	 fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   cursorColor = "255 255 255";
   textOffset = "4 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = false;
   canKeyFocus = true;
   justify = "right";
   numbersOnly = true;
};

//ͨ�������2
if(!isObject(GuiGeneralTextEditProfile_2)) new GuiControlProfile (GuiGeneralTextEditProfile_2)
{
   opaque = false;
   border = -2;
   fontType = "����";
   fontSize = 12;
   fontColor = "0 0 0";
   cursorColor = "0 0 0";
   textOffset = "4 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = false;
   canKeyFocus = true;
   justify = "left";
};

//ͨ�������3
if(!isObject(GuiGeneralTextEditProfile_3)) new GuiControlProfile (GuiGeneralTextEditProfile_3)
{
	 opaque = false;
	 border = -2;
	 fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   cursorColor = "255 255 255";
   textOffset = "4 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = false;
   canKeyFocus = true;
   justify = "left";
};

//ͨ�������4
if(!isObject(GuiGeneralTextEditProfile_4)) new GuiControlProfile (GuiGeneralTextEditProfile_4)
{
	 opaque = false;
	 border = -2;
	 fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   cursorColor = "255 255 255";
   textOffset = "4 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = false;
   canKeyFocus = true;
   justify = "center";
};

//ͨ�������5
if(!isObject(GuiGeneralTextEditProfile_5)) new GuiControlProfile (GuiGeneralTextEditProfile_5)
{
	 opaque = false;
	 border = -2;
	 fontType = "����";
   fontSize = 14;
   fontColor = "255 255 255";
   cursorColor = "255 255 255";
   textOffset = "4 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = false;
   canKeyFocus = true;
   justify = "center";
};

//ͨ�������6_ֻ��������
if(!isObject(GuiCurrencyTextEditProfile_6)) new GuiControlProfile (GuiCurrencyTextEditProfile_6)
{
	 opaque = false;
	 border = -2;
	 fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   cursorColor = "255 255 255";
   textOffset = "4 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = false;
   canKeyFocus = true;
   justify = "center";
   numbersOnly = true;
};
//�ʺ�����ר�������
if(!isObject(AccountPassWordTextEditProfile)) new GuiControlProfile (AccountPassWordTextEditProfile)
{
   opaque = false;
   border = -2;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   cursorColor = "255 255 255";
   textOffset = "4 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = true;
   canKeyFocus = true;
   justify = "left";
};

//----------------------------------------------------------------------------------------------------------------

if(!isObject(GuiTextEditNumericProfile)) new GuiControlProfile (GuiTextEditNumericProfile : GuiTextEditProfile)
{
   numbersOnly = true;
};

if(!isObject(GuiProgressProfile)) new GuiControlProfile ("GuiProgressProfile")
{
   opaque = false;
   fillColor = "228 0 1 200";
   border = true;
   borderColor   = "0 0 0 255";
};

if(!isObject(GuiProgressTextProfile)) new GuiControlProfile ("GuiProgressTextProfile")
{
   fontColor = "255 255 255";
   justify = "center";
};

if(!isObject(GuiButtonProfile)) new GuiControlProfile (GuiButtonProfile)
{
   opaque = true;
   border = true;
   fontColor = "0 0 0";
   fontColorHL = "32 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = false;
   hasBitmapArray = false;
};

//----------------------------------------------------------------��ť-----------------------------------------------------------------
//ѡ��������水ť_ͨ��
if(!isObject(GuiServerSelectButtonProfile_1)) new GuiControlProfile (GuiServerSelectButtonProfile_1)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin03_1_012.png";
   hasBitmapArray = true;
};

if(!isObject(GuiServerSelectButtonProfile_2)) new GuiControlProfile (GuiServerSelectButtonProfile_2)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin03_1_013.png";
   hasBitmapArray = true;
};

if(!isObject(GuiServerSelectButtonProfile_3)) new GuiControlProfile (GuiServerSelectButtonProfile_3)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin03_1_004_1.png";
   hasBitmapArray = true;
};

if(!isObject(GuiServerSelectButtonProfile_4)) new GuiControlProfile (GuiServerSelectButtonProfile_4)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin03_1_004_2.png";
   hasBitmapArray = true;
};

//����������水ť
if(!isObject(GuiCreatePlayerButtonProfile_1)) new GuiControlProfile (GuiCreatePlayerButtonProfile_1)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_015.png";
   hasBitmapArray = true;
};

if(!isObject(GuiCreatePlayerButtonProfile_2)) new GuiControlProfile (GuiCreatePlayerButtonProfile_2)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_017.png";
   hasBitmapArray = true;
};

if(!isObject(GuiCreatePlayerButtonProfile_3)) new GuiControlProfile (GuiCreatePlayerButtonProfile_3)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_018.png";
   hasBitmapArray = true;
};

if(!isObject(GuiCreatePlayerButtonProfile_4)) new GuiControlProfile (GuiCreatePlayerButtonProfile_4)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_018.png";
   hasBitmapArray = true;
};

if(!isObject(GuiCreatePlayerButtonProfile_5)) new GuiControlProfile (GuiCreatePlayerButtonProfile_5)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_017.png";
   hasBitmapArray = true;
};

if(!isObject(GuiCreatePlayerButtonProfile_6)) new GuiControlProfile (GuiCreatePlayerButtonProfile_6)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_021.png";
   hasBitmapArray = true;
};

if(!isObject(GuiCreatePlayerButtonProfile_7)) new GuiControlProfile (GuiCreatePlayerButtonProfile_7)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_022.png";
   hasBitmapArray = true;
};

if(!isObject(GuiCreatePlayerButtonProfile_8)) new GuiControlProfile (GuiCreatePlayerButtonProfile_8)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_004.png";
   hasBitmapArray = true;
};

if(!isObject(GuiCreatePlayerButtonProfile_10)) new GuiControlProfile (GuiCreatePlayerButtonProfile_10)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_032.png";
   hasBitmapArray = true;
};

//Ŀ�갴ť
if(!isObject(GuiObjectButtonProfile)) new GuiControlProfile (GuiObjectButtonProfile)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fontOutline = "50 50 50";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow14_1_005.png";
   hasBitmapArray = true;
};

//�������ť1
if(!isObject(GuiTeamAdministerButtonProfile_1)) new GuiControlProfile (GuiTeamAdministerButtonProfile_1)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow14_1_009.png";
   hasBitmapArray = true;
};

//�������ť2
if(!isObject(GuiTeamAdministerButtonProfile_2)) new GuiControlProfile (GuiTeamAdministerButtonProfile_2)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "215 202 255";
   fontColorNA = "215 202 255";
   fontColorSEL= "100 100 100";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow14_1_010.png";
   hasBitmapArray = true;
};

//ͨ�ð�ť1
if(!isObject(GuiGeneralButtonProfile_1)) new GuiControlProfile (GuiGeneralButtonProfile_1)
{
   opaque = true;
   border = true;
   fontType = "����_GB2312";
   fontSize = 16;
   fontColor = "229 229 229";
   fontColorHL = "122 255 238";
   fontColorNA = "175 175 175";
   fontColorSEL= "46 25 0";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIbutton02_1_001";
   hasBitmapArray = true;
   bitmapfont = false;
};

//ͨ�ð�ť2
if(!isObject(GuiGeneralButtonProfile_2)) new GuiControlProfile (GuiGeneralButtonProfile_2)
{
   opaque = true;
   border = true;
   fontType = "����_GB2312";
   fontSize = 16;
   fontColor = "229 229 229";
   fontColorHL = "122 255 238";
   fontColorNA = "175 175 175";
   fontColorSEL= "46 25 0";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIbutton02_1_005";
   hasBitmapArray = true;
   bitmapfont = false;
};

//ͨ�ð�ť3
if(!isObject(GuiGeneralButtonProfile_3)) new GuiControlProfile (GuiGeneralButtonProfile_3)
{
   opaque = true;
   border = true;
   fontType = "����_GB2312";
   fontSize = 16;
   fontColor = "229 229 229";
   fontColorHL = "122 255 238";
   fontColorNA = "175 175 175";
   fontColorSEL= "46 25 0";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIbutton02_1_008";
   hasBitmapArray = true;
   bitmapfont = false;
};

//ͨ�ð�ť5
if(!isObject(GuiGeneralButtonProfile_5)) new GuiControlProfile (GuiGeneralButtonProfile_5)
{
   opaque = true;
   border = true;
   fontColor = "255 255 255";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIButton01_1_005";
   hasBitmapArray = true;
};

//��ʾ������ת��ť
if(!isObject(GuiShowPlayerLeftButtonProfile)) new GuiControlProfile (GuiShowPlayerLeftButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_010";
   hasBitmapArray = true;
};

//��ʾ������ת��ť
if(!isObject(GuiShowPlayerRightButtonProfile)) new GuiControlProfile (GuiShowPlayerRightButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIlogin04_1_011";
   hasBitmapArray = true;
};

//������̰�ť
if(!isObject(GuiOpenKeyboardButtonProfile)) new GuiControlProfile (GuiOpenKeyboardButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = false;
   bitmap  = "./images/GUIbutton02_1_002";
   hasBitmapArray = false;
};

//����̰�ť
if(!isObject(GuiKeyboardButtonProfile)) new GuiControlProfile (GuiKeyboardButtonProfile)
{
   opaque = true;
   border = true;
   fontColor = "229 229 229";
   fontColorSEL = "170 170 170";
   fontColorHL = "255 255 255";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = false;
   bitmap  = "./images/GUIbutton02_1_004";
   hasBitmapArray = false;
};

//��������ҳ��ť_��
if(!isObject(GuiSkillShortcutWndUpButtonProfile)) new GuiControlProfile (GuiSkillShortcutWndUpButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow18_1_002";
   hasBitmapArray = true;
};

//��������ҳ��ť_��
if(!isObject(GuiSkillShortcutWndDownButtonProfile)) new GuiControlProfile (GuiSkillShortcutWndDownButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow18_1_003";
   hasBitmapArray = true;
};

//С��ͼ_��ͼ�Ŵ�
if(!isObject(GuiAddMapButtonProfile)) new GuiControlProfile (GuiAddMapButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_004";
   hasBitmapArray = true;
};

//С��ͼ_��ͼ��С
if(!isObject(GuiDecMapButtonProfile)) new GuiControlProfile (GuiDecMapButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_005";
   hasBitmapArray = true;
};

//С��ͼ_ȫ��ͼ��ť
if(!isObject(GuiBigMapButtonProfile)) new GuiControlProfile (GuiBigMapButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_006";
   hasBitmapArray = true;
};

//С��ͼ_������ť
if(!isObject(GuiBirdMapHelpButtonProfile)) new GuiControlProfile (GuiBirdMapHelpButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_007";
   hasBitmapArray = true;
};

//С��ͼ_������ť
if(!isObject(GuiGradeButtonProfile)) new GuiControlProfile (GuiGradeButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_008";
   hasBitmapArray = true;
};

//С��ͼ_��ʾ��ť
if(!isObject(GuiObligateButtonProfile)) new GuiControlProfile (GuiObligateButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_009";
   hasBitmapArray = true;
};

//С��ͼ_������ť
if(!isObject(GuiLieLockButtonProfile)) new GuiControlProfile (GuiLieLockButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_010";
   hasBitmapArray = true;
};

//С��ͼ_��ת¼��ť
if(!isObject(GuiNoteButtonProfile)) new GuiControlProfile (GuiNoteButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_011";
   hasBitmapArray = true;
};

//С��ͼ_�ż���ť
if(!isObject(GuiLetterButtonProfile)) new GuiControlProfile (GuiLetterButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_012";
   hasBitmapArray = true;
};

//С��ͼ_�̳ǰ�ť
if(!isObject(GuiShopButtonProfile)) new GuiControlProfile (GuiShopButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_013";
   hasBitmapArray = true;
};

//С��ͼ_ѡ·����ѡ��ť
if(!isObject(GuiLineSelectButtonProfile)) new GuiControlProfile (GuiLineSelectButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_014";
   hasBitmapArray = true;
};

//С��ͼ_��С����ť
if(!isObject(GuiMinimizeButtonProfile)) new GuiControlProfile (GuiMinimizeButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_015";
   hasBitmapArray = true;
};

//С��ͼ_��󻯰�ť
if(!isObject(GuiMaxmizeButtonProfile)) new GuiControlProfile (GuiMaxmizeButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_016";
   hasBitmapArray = true;
};

//С��ͼ_������ť
if(!isObject(GuiBirdViewFBButtonProfile)) new GuiControlProfile (GuiBirdViewFBButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow07_1_018";
   hasBitmapArray = true;
};

//��ʾ����ͷ��_��С����ť
if(!isObject(GuiPlayerMinimizeButtonProfile)) new GuiControlProfile (GuiPlayerMinimizeButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow02_1_012";
   hasBitmapArray = true;
};

//��ʾ����ͷ��_��󻯰�ť
if(!isObject(GuiPlayerMaxmizeButtonProfile)) new GuiControlProfile (GuiPlayerMaxmizeButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow05_1_008";
   hasBitmapArray = true;
};

//��ʾ����ͷ��_PK1
if(!isObject(GuiPKOpenButtonProfile)) new GuiControlProfile (GuiPKOpenButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow02_1_003";
   hasBitmapArray = true;
};

//��ʾ����ͷ��_PK2
if(!isObject(GuiPKCloseButtonProfile)) new GuiControlProfile (GuiPKCloseButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow02_1_004";
   hasBitmapArray = true;
};

//��ʾ����ͷ��ť1
if(!isObject(GuiPlayerIconButtonProfile_1)) new GuiControlProfile (GuiPlayerIconButtonProfile_1)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow05_1_004";
   hasBitmapArray = true;
};

//��ʾ����ͷ��ť2
if(!isObject(GuiPlayerIconButtonProfile_2)) new GuiControlProfile (GuiPlayerIconButtonProfile_2)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow05_1_005";
   hasBitmapArray = true;
};

//��ʾ����ͷ��ť3
if(!isObject(GuiPlayerIconButtonProfile_3)) new GuiControlProfile (GuiPlayerIconButtonProfile_3)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow05_1_006";
   hasBitmapArray = true;
};

//��ʾ����ͷ��ť4
if(!isObject(GuiPlayerIconButtonProfile_4)) new GuiControlProfile (GuiPlayerIconButtonProfile_4)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow05_1_007";
   hasBitmapArray = true;
};

//���촰��_�ۺ�Ƶ��
if(!isObject(GuiSelectChannelButtonProfile_1)) new GuiControlProfile (GuiSelectChannelButtonProfile_1)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_040";  
   hasBitmapArray = true;
};

//���촰��_ͬ��Ƶ��
if(!isObject(GuiSelectChannelButtonProfile_2)) new GuiControlProfile (GuiSelectChannelButtonProfile_2)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_041";  
   hasBitmapArray = true;
};

//���촰��_���Ƶ��
if(!isObject(GuiSelectChannelButtonProfile_3)) new GuiControlProfile (GuiSelectChannelButtonProfile_3)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_042";  
   hasBitmapArray = true;
};
//���촰��_����Ƶ��
if(!isObject(GuiSelectChannelButtonProfile_4)) new GuiControlProfile (GuiSelectChannelButtonProfile_4)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_043";  
   hasBitmapArray = true;
};

//���촰��_����Ƶ��
if(!isObject(GuiSelectChannelButtonProfile_5)) new GuiControlProfile (GuiSelectChannelButtonProfile_5)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_044";  
   hasBitmapArray = true;
};

//���촰��_���ϴ��ڷŴ�
if(!isObject(GuiChatWndUpMaqnifyButtonProfile)) new GuiControlProfile (GuiChatWndUpMaqnifyButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_003";
   hasBitmapArray = true;
};

//���촰��_���´�����С
if(!isObject(GuiChatWndDownDwindleButtonProfile)) new GuiControlProfile (GuiChatWndDownDwindleButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_004";
   hasBitmapArray = true;
};

//���촰��_���Ҵ��ڷŴ�
if(!isObject(GuiChatWndRightMaqnifyButtonProfile)) new GuiControlProfile (GuiChatWndRightMaqnifyButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_005";
   hasBitmapArray = true;
};

//���촰��_���Ҵ�����С
if(!isObject(GuiChatWndRightDwindleButtonProfile)) new GuiControlProfile (GuiChatWndRightDwindleButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_006";
   hasBitmapArray = true;
};

//���촰��_�趨
if(!isObject(GuiChatWndSettingButtonProfile)) new GuiControlProfile (GuiChatWndSettingButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_007";
   hasBitmapArray = true;
};

//���촰��_�½�
if(!isObject(GuiChatWndNewButtonProfile)) new GuiControlProfile (GuiChatWndNewButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_008";
   hasBitmapArray = true;
};

//���촰��_����
if(!isObject(GuiChatWndSaveButtonProfile)) new GuiControlProfile (GuiChatWndSaveButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_009";
   hasBitmapArray = true;
};

//���촰��_δ����
if(!isObject(GuiChatWndUnLockButtonProfile)) new GuiControlProfile (GuiChatWndUnLockButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_010";
   hasBitmapArray = true;
};

//���촰��_����
if(!isObject(GuiChatWndLockButtonProfile)) new GuiControlProfile (GuiChatWndLockButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_011";
   hasBitmapArray = true;
};

//���촰��_���Ϸ�ҳ
if(!isObject(GuiChatWndUpPageButtonProfile)) new GuiControlProfile (GuiChatWndUpPageButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_012";
   hasBitmapArray = true;
};

//���촰��_���·�ҳ
if(!isObject(GuiChatWndDownPageButtonProfile)) new GuiControlProfile (GuiChatWndDownPageButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_013";
   hasBitmapArray = true;
};

//���촰��_���·�ҳ����
if(!isObject(GuiChatWndFinallyDownPageButtonProfile)) new GuiControlProfile (GuiChatWndFinallyDownPageButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_014";
   hasBitmapArray = true;
};

//���������_��Ϣ���Ͱ�ť
if(!isObject(GuiChatInputWndSelectInfoButtonProfile)) new GuiControlProfile (GuiChatInputWndSelectInfoButtonProfile)
{
   opaque = true;
   border = true;
   fontColor = "255 255 255";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_016";
   hasBitmapArray = true;
};

//���������_����
if(!isObject(GuiChatInputWndFaceButtonProfile)) new GuiControlProfile (GuiChatInputWndFaceButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_017";
   hasBitmapArray = true;
};

//���������_����
if(!isObject(GuiChatInputWndInputButtonProfile)) new GuiControlProfile (GuiChatInputWndInputButtonProfile)
{
   opaque = true;
   border = true;
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow15_1_018";
   hasBitmapArray = true;
};

//���׽���ͨ�ð�ť1
if(!isObject(GuiTradeButtonProfile_1)) new GuiControlProfile (GuiTradeButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow28_1_004";
   hasBitmapArray = true;
};

//���׽���ͨ�ð�ť2
if(!isObject(GuiTradeButtonProfile_2)) new GuiControlProfile (GuiTradeButtonProfile_2)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow28_1_003";
   hasBitmapArray = true;
};

//���׽���ͨ�ð�ť3
if(!isObject(GuiTradeButtonProfile_3)) new GuiControlProfile (GuiTradeButtonProfile_3)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "194 238 164";
	 fontColorHL = "255 222 0";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIButton01_1_001";
   hasBitmapArray = true;
};

//���׽���ͨ�ð�ť4
if(!isObject(GuiTradeButtonProfile_4)) new GuiControlProfile (GuiTradeButtonProfile_4)
{
	 fontType = "����_GB2312";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 222 0";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIButton01_1_002";
   hasBitmapArray = true;
};

//���׽���ͨ�ð�ť5
if(!isObject(GuiTradeButtonProfile_5)) new GuiControlProfile (GuiTradeButtonProfile_5)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow28_1_007";
   hasBitmapArray = true;
};
//ͨ������ť1
if(!isObject(GuiCurrencyMendButtonProfile_1)) new GuiControlProfile (GuiCurrencyMendButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIButton01_1_008";
   hasBitmapArray = true;
};

//ͨ��ȫ�ް�ť1
if(!isObject(GuiCurrencyAllMendButtonProfile_1)) new GuiControlProfile (GuiCurrencyAllMendButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIButton01_1_009";
   hasBitmapArray = true;
};

//��ʾ������水ť1
if(!isObject(GuiMissionWndButtonProfile_1)) new GuiControlProfile (GuiMissionWndButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "125 125 125";
   justify = "center";
   bitmap  = "./images/GUIWindow27_1_002";
   hasBitmapArray = true;
};

//��ʾ������水ť6
if(!isObject(GuiMissionWndButtonProfile_6)) new GuiControlProfile (GuiMissionWndButtonProfile_6)
{
   bitmap  = "./images/GUIWindow12_1_002";
   hasBitmapArray = true;
};

//��ʾ������水ť7
if(!isObject(GuiMissionWndButtonProfile_7)) new GuiControlProfile (GuiMissionWndButtonProfile_7)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "211 74 0";
   fontColorSEL = "255 255 255";
   justify = "center";
   bitmap  = "./images/GUIWindow12_1_003";
   hasBitmapArray = true;
   textOffset = "0 10";
};

//��ʾ������水ť8
if(!isObject(GuiMissionWndButtonProfile_8)) new GuiControlProfile (GuiMissionWndButtonProfile_8)
{
   bitmap  = "./images/GUIWindow12_1_004";
   hasBitmapArray = true;
};

//��ʾ������水ť9
if(!isObject(GuiMissionWndButtonProfile_9)) new GuiControlProfile (GuiMissionWndButtonProfile_9)
{
   bitmap  = "./images/GUIWindow12_1_005";
   hasBitmapArray = true;
};

//������水ť_�޸Ķ�������
if(!isObject(GuiChangeTNButtonProfile)) new GuiControlProfile (GuiChangeTNButtonProfile)
{
   opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   fontColorHL = "100 100 100";
   fontColorNA = "255 255 255";
   fontColorSEL= "0 0 0";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow26_1_006";
   hasBitmapArray = true;
};

////������Ϣ���水ť_ʱװ
//if(!isObject(GuiPlayerInfoFashionButtonProfile)) new GuiControlProfile (GuiPlayerInfoFashionButtonProfile)
//{
//	 fontType = "����";
//   fontSize = 12;
//	 fontColor = "178 214 154";
//	 fontColorHL = "236 205 79";
//   fontColorSEL = "96 103 84";
//   justify = "center";
//   bitmap  = "./images/GUIWindow33_1_017";
//   hasBitmapArray = true;
//};

//������Ϣ���水ť1
if(!isObject(GuiPlayerInfoButtonProfile_1)) new GuiControlProfile (GuiPlayerInfoButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow33_1_016";
   hasBitmapArray = true;
};

//������Ϣ���水ť2
if(!isObject(GuiPlayerInfoButtonProfile_2)) new GuiControlProfile (GuiPlayerInfoButtonProfile_2)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow33_1_012";
   hasBitmapArray = true;
};

//������Ϣ���水ť3
if(!isObject(GuiPlayerInfoButtonProfile_3)) new GuiControlProfile (GuiPlayerInfoButtonProfile_3)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow33_1_011";
   hasBitmapArray = true;
};

//������Ϣ���水ť4
if(!isObject(GuiPlayerInfoButtonProfile_4)) new GuiControlProfile (GuiPlayerInfoButtonProfile_4)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow33_1_018";
   hasBitmapArray = true;
};

//������Ϣ���水ť5
if(!isObject(GuiPlayerInfoButtonProfile_5)) new GuiControlProfile (GuiPlayerInfoButtonProfile_5)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow33_1_022";
   hasBitmapArray = true;
};

//������Ϣ���水ť6
if(!isObject(GuiPlayerInfoButtonProfile_6)) new GuiControlProfile (GuiPlayerInfoButtonProfile_6)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow33_1_020";
   hasBitmapArray = true;
};

//������Ϣ���水ť7
if(!isObject(GuiPlayerInfoButtonProfile_7)) new GuiControlProfile (GuiPlayerInfoButtonProfile_7)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow33_1_021";
   hasBitmapArray = true;
};

//������Ϣ���水ť8
if(!isObject(GuiPlayerInfoButtonProfile_8)) new GuiControlProfile (GuiPlayerInfoButtonProfile_8)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIbutton02_1_038";
   hasBitmapArray = true;
};

//���ͼ����ͨ�ð�ť
if(!isObject(GuiBigViewMapButtonProfile)) new GuiControlProfile (GuiBigViewMapButtonProfile)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 222 0";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIButton01_1_013";
   hasBitmapArray = true;
};

//�̵����ѡ�а�ť
if(!isObject(GuiShopSelectButtonProfile)) new GuiControlProfile (GuiShopSelectButtonProfile)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "178 214 154";
	 fontColorHL = "236 205 79";
   fontColorSEL = "96 103 84";
   justify = "center";
   bitmap  = "./images/GUIWindow29_1_004";
   hasBitmapArray = true;
};

//�̵���水ť1
if(!isObject(GuiShopButtonProfile_1)) new GuiControlProfile(GuiShopButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_019.png";
   hasBitmapArray = true;
};

//�̵���水ť2
if(!isObject(GuiShopButtonProfile_2)) new GuiControlProfile(GuiShopButtonProfile_2)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIWindow29_1_007.png";
   hasBitmapArray = true;
};

//�̵���水ť3
if(!isObject(GuiShopButtonProfile_3)) new GuiControlProfile(GuiShopButtonProfile_3)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIWindow29_1_008.png";
   hasBitmapArray = true;
};

if(!isObject(GuiCloseButtonProfile)) new GuiControlProfile (GuiCloseButtonProfile)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "171 117 117";
   justify = "center";
   bitmap  = "./images/GUIbutton02_1_039";
   hasBitmapArray = true;
};

if(!isObject(GuiCloseButtonProfile1)) new GuiControlProfile(GuiCloseButtonProfile1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_001";
   hasBitmapArray = true;
};

//������水ť2
if(!isObject(GuiHelpButtonProfile)) new GuiControlProfile (GuiHelpButtonProfile)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "171 117 117";
   justify = "center";
   bitmap  = "./images/GUIbutton02_1_040";
   hasBitmapArray = true;
};
//������水ť3
if(!isObject(GuiPetButtonProfile_3)) new GuiControlProfile (GuiPetButtonProfile_3)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "171 117 117";
   justify = "center";
   bitmap  = "./images/GUIbutton02_1_041";
   hasBitmapArray = true;
};
//������水ť4
if(!isObject(GuiPetButtonProfile_4)) new GuiControlProfile (GuiPetButtonProfile_4)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "171 117 117";
   justify = "center";
   bitmap  = "./images/GUIbutton02_1_042";
   hasBitmapArray = true;
};
//������水ť5
if(!isObject(GuiPetButtonProfile_5)) new GuiControlProfile (GuiPetButtonProfile_5)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "117 171 126";
   justify = "center";
   bitmap  = "./images/GUIWindow35_1_013";
   hasBitmapArray = true;
};
//������水ť6
if(!isObject(GuiPetButtonProfile_6)) new GuiControlProfile (GuiPetButtonProfile_6)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow35_1_014";
   hasBitmapArray = true;
};
//������水ť7
if(!isObject(GuiPetButtonProfile_7)) new GuiControlProfile (GuiPetButtonProfile_7)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow35_1_015";
   hasBitmapArray = true;
};
//������水ť8
if(!isObject(GuiPetButtonProfile_8)) new GuiControlProfile (GuiPetButtonProfile_8)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow35_1_026";
   hasBitmapArray = true;
};
//������水ť9
if(!isObject(GuiPetButtonProfile_9)) new GuiControlProfile (GuiPetButtonProfile_9)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow35_1_022";
   hasBitmapArray = true;
};
//������水ť10
if(!isObject(GuiPetButtonProfile_10)) new GuiControlProfile (GuiPetButtonProfile_10)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow35_1_027";
   hasBitmapArray = true;
};
//������水ť11
if(!isObject(GuiPetButtonProfile_11)) new GuiControlProfile (GuiPetButtonProfile_11)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow35_1_005";
   hasBitmapArray = true;
};
//������水ť12
if(!isObject(GuiPetButtonProfile_12)) new GuiControlProfile (GuiPetButtonProfile_12)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow35_1_025";
   hasBitmapArray = true;
};
//������水ť13
if(!isObject(GuiPetButtonProfile_13)) new GuiControlProfile (GuiPetButtonProfile_13)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow35_1_007";
   hasBitmapArray = true;
};

//������水ť14
if(!isObject(GuiPetButtonProfile_14)) new GuiControlProfile (GuiPetButtonProfile_14)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIbutton02_1_049";
   hasBitmapArray = true;
};

//������水ť15
if(!isObject(GuiPetButtonProfile_15)) new GuiControlProfile (GuiPetButtonProfile_15)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow35_1_006";
   hasBitmapArray = true;
};

//������水ť16
if(!isObject(GuiPetButtonProfile_16)) new GuiControlProfile(GuiPetButtonProfile_16)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "150 150 150";
   fontColorNA = "150 150 150";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_025";
   hasBitmapArray = true;
};

//����׷��Ѱ�����水ť
for(%i = 0; %i < 7; %i++)
{
	 switch(%i)
	 {
	 		case 0:
	 			%RGB = "255 113 113";
	 		case 1:
	 			%RGB = "176 0 0";
	 		case 2:
	 			%RGB = "0 176 80";
	 		case 3:
	 			%RGB = "0 176 240";
	 		case 4:
	 			%RGB = "255 192 0";
	 		case 5:
	 			%RGB = "0 0 0";
	 		case 6:
	 			%RGB = "255 255 255";
	 }
	 
   if(!isObject("GuiMissionTraceButtonProfile" @ %i)) new GuiControlProfile ("GuiMissionTraceButtonProfile" @ %i)
   {
   	  fontType = "����";
      fontSize = 12;
   	  fontColor = %RGB;
   	  fontColorHL = "211 74 0";
      fontColorSEL = "255 255 255";
      justify = "left";
      hasBitmapArray = true;
   }; 
}

//����׷�ٽ�������رհ�ť
if(!isObject(GuiMissionTraceCloseButtonProfile)) new GuiControlProfile (GuiMissionTraceCloseButtonProfile)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "211 74 0";
   fontColorSEL = "255 255 255";
   justify = "left";
   bitmap  = "./images/GUIbutton02_1_021";
   hasBitmapArray = true;
};


//����Ի����水ť
if(!isObject(GuiMissionDialogButtonProfile)) new GuiControlProfile (GuiMissionDialogButtonProfile)
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 0";
   fontColorSEL = "155 155 155";
   justify = "center";
   bitmap  = "./images/GUIbutton02_1_022";
   hasBitmapArray = true;
};



//�ײ���������ť1
if(!isObject(GuiBottomControlButtonProfile_1)) new GuiControlProfile (GuiBottomControlButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_002";
   hasBitmapArray = true;
};

//�ײ���������ť2
if(!isObject(GuiBottomControlButtonProfile_2)) new GuiControlProfile (GuiBottomControlButtonProfile_2)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_003";
   hasBitmapArray = true;
};

//�ײ���������ť3
if(!isObject(GuiBottomControlButtonProfile_3)) new GuiControlProfile (GuiBottomControlButtonProfile_3)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_004";
   hasBitmapArray = true;
};

//�ײ���������ť4
if(!isObject(GuiBottomControlButtonProfile_4)) new GuiControlProfile (GuiBottomControlButtonProfile_4)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_005";
   hasBitmapArray = true;
};

//�ײ���������ť5
if(!isObject(GuiBottomControlButtonProfile_5)) new GuiControlProfile (GuiBottomControlButtonProfile_5)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_006";
   hasBitmapArray = true;
};

//�ײ���������ť6
if(!isObject(GuiBottomControlButtonProfile_6)) new GuiControlProfile (GuiBottomControlButtonProfile_6)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_007";
   hasBitmapArray = true;
};

//�ײ���������ť7
if(!isObject(GuiBottomControlButtonProfile_7)) new GuiControlProfile (GuiBottomControlButtonProfile_7)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_008";
   hasBitmapArray = true;
};

//�ײ���������ť8
if(!isObject(GuiBottomControlButtonProfile_8)) new GuiControlProfile (GuiBottomControlButtonProfile_8)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_009";
   hasBitmapArray = true;
};

//�ײ���������ť9
if(!isObject(GuiBottomControlButtonProfile_9)) new GuiControlProfile (GuiBottomControlButtonProfile_9)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_010";
   hasBitmapArray = true;
};

//�ײ���������ť10
if(!isObject(GuiBottomControlButtonProfile_10)) new GuiControlProfile (GuiBottomControlButtonProfile_10)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_011";
   hasBitmapArray = true;
};

//�ײ���������ť11
if(!isObject(GuiBottomControlButtonProfile_11)) new GuiControlProfile (GuiBottomControlButtonProfile_11)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow18_1_012";
   hasBitmapArray = true;
};

//�ײ���������ť12
if(!isObject(GuiBottomControlButtonProfile_12)) new GuiControlProfile (GuiBottomControlButtonProfile_12)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow18_1_013";
   hasBitmapArray = true;
};

//�ײ���������ť13
if(!isObject(GuiBottomControlButtonProfile_13)) new GuiControlProfile (GuiBottomControlButtonProfile_13)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_001";
   hasBitmapArray = true;
};

//�ײ���������ť14
if(!isObject(GuiBottomControlButtonProfile_14)) new GuiControlProfile (GuiBottomControlButtonProfile_14)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_012";
   hasBitmapArray = true;
};

//�ײ���������ť15
if(!isObject(GuiBottomControlButtonProfile_15)) new GuiControlProfile (GuiBottomControlButtonProfile_15)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_013";
   hasBitmapArray = true;
};

//�ײ���������ť16
if(!isObject(GuiBottomControlButtonProfile_16)) new GuiControlProfile (GuiBottomControlButtonProfile_16)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   bitmap  = "./images/GUIWindow19_1_014";
   hasBitmapArray = true;
};

//����ϵ��ť--��ϵ
if(!isObject(GuiSkillSelectBtnProfile_Jin)) new GuiControlProfile ("GuiSkillSelectBtnProfile_Jin")
{
   bitmap  = "./images/GUIWindow32_1_030";
   hasBitmapArray = true;
};

//����ϵ��ť--ľϵ
if(!isObject(GuiSkillSelectBtnProfile_Mu)) new GuiControlProfile ("GuiSkillSelectBtnProfile_Mu")
{
   bitmap  = "./images/GUIWindow32_1_027";
   hasBitmapArray = true;
};

//����ϵ��ť--ˮϵ
if(!isObject(GuiSkillSelectBtnProfile_Shui)) new GuiControlProfile ("GuiSkillSelectBtnProfile_Shui")
{
   bitmap  = "./images/GUIWindow32_1_031";
   hasBitmapArray = true;
};

//����ϵ��ť--��ϵ
if(!isObject(GuiSkillSelectBtnProfile_Huo)) new GuiControlProfile ("GuiSkillSelectBtnProfile_Huo")
{
   bitmap  = "./images/GUIWindow32_1_028";
   hasBitmapArray = true;
};

//����ϵ��ť--��ϵ
if(!isObject(GuiSkillSelectBtnProfile_Tu)) new GuiControlProfile ("GuiSkillSelectBtnProfile_Tu")
{
   bitmap  = "./images/GUIWindow32_1_029";
   hasBitmapArray = true;
};

//����ϵ��ť_�ۺ�
if(!isObject(GuiSkillSelectBtnProfile_ZH)) new GuiControlProfile ("GuiSkillSelectBtnProfile_ZH")
{
   bitmap  = "./images/GUIWindow32_1_039";
   hasBitmapArray = true;
};


//����ϵѡ�а�ť
if(!isObject(GuiSkillSeriesSelectBtnProfile)) new GuiControlProfile ("GuiSkillSeriesSelectBtnProfile")
{
   bitmap  = "./images/GUIWindow32_1_004";
   hasBitmapArray = true;
};



//����ѧϰѡ�а�ť
if(!isObject(GuiSkillStudySelectBtnProfile)) new GuiControlProfile ("GuiSkillStudySelectBtnProfile")
{
   bitmap  = "./images/GUIbutton02_1_031";
   hasBitmapArray = true;
};

//ϵͳ���ý��������ť
if(!isObject(GuiSystemSettingHelpBtnProfile)) new GuiControlProfile("GuiSystemSettingHelpBtnProfile")
{
	 bitmap  = "./images/GUIbutton02_1_007";
   hasBitmapArray = true;
};

//ϵͳ���ý���رհ�ť
if(!isObject(GuiSystemSettingCloseBtnProfile)) new GuiControlProfile("GuiSystemSettingCloseBtnProfile")
{
	 bitmap  = "./images/GUIbutton02_1_006";
   hasBitmapArray = true;
};

//ϵͳ���ý�������ť
if(!isObject(GuiSystemSettingLeftBtnProfile)) new GuiControlProfile("GuiSystemSettingLeftBtnProfile")
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIWindow13_1_005";
   hasBitmapArray = true;
};

//ϵͳ���ý������Ұ�ť
if(!isObject(GuiSystemSettingRightBtnProfile)) new GuiControlProfile("GuiSystemSettingRightBtnProfile")
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIWindow13_1_004";
   hasBitmapArray = true;
};

//����ܽ��水ť
if(!isObject(GuiLivingSkillBtnProfile)) new GuiControlProfile ("GuiLivingSkillBtnProfile")
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_047";
   hasBitmapArray = true;
};

//����ܽ���+��ť
if(!isObject(GuiLivingSkillBtnProfile_1)) new GuiControlProfile ("GuiLivingSkillBtnProfile_1")
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_052";
   hasBitmapArray = true;
};

//����ܽ���-��ť
if(!isObject(GuiLivingSkillBtnProfile_2)) new GuiControlProfile ("GuiLivingSkillBtnProfile_2")
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_053";
   hasBitmapArray = true;
};

//����ܽ���ɼ���ť
if(!isObject(GuiLivingSkillBtnProfile_CJ)) new GuiControlProfile ("GuiLivingSkillBtnProfile_CJ")
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_055";
   hasBitmapArray = true;
};

//����ܽ������찴ť
if(!isObject(GuiLivingSkillBtnProfile_ZZ)) new GuiControlProfile ("GuiLivingSkillBtnProfile_ZZ")
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_056";
   hasBitmapArray = true;
};

//����ܽ���÷찴ť
if(!isObject(GuiLivingSkillBtnProfile_CF)) new GuiControlProfile ("GuiLivingSkillBtnProfile_CF")
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_057";
   hasBitmapArray = true;
};

//����ܽ��湤�հ�ť
if(!isObject(GuiLivingSkillBtnProfile_GY)) new GuiControlProfile ("GuiLivingSkillBtnProfile_GY")
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_058";
   hasBitmapArray = true;
};


//����ܽ�����䰴ť
if(!isObject(GuiLivingSkillBtnProfile_FZ)) new GuiControlProfile ("GuiLivingSkillBtnProfile_FZ")
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_059";
   hasBitmapArray = true;
};

//����ܽ���������ť
if(!isObject(GuiLivingSkillBtnProfile_LD)) new GuiControlProfile ("GuiLivingSkillBtnProfile_LD")
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_060";
   hasBitmapArray = true;
};

//����ܽ�����⿰�ť
if(!isObject(GuiLivingSkillBtnProfile_PR)) new GuiControlProfile ("GuiLivingSkillBtnProfile_PR")
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_061";
   hasBitmapArray = true;
};

//����ܽ��湤����ť
if(!isObject(GuiLivingSkillBtnProfile_GJ)) new GuiControlProfile ("GuiLivingSkillBtnProfile_GJ")
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_062";
   hasBitmapArray = true;
};

//����ܽ��������ť
if(!isObject(GuiLivingSkillBtnProfile_GS)) new GuiControlProfile ("GuiLivingSkillBtnProfile_GS")
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "252 177 26";
	 fontColorHL = "252 177 26";
   fontColorSEL = "252 177 26";
   justify = "center";
   bitmap  = "./images/GUIWindow32_1_063";
   hasBitmapArray = true;
};

//�ֿⰴť1
if(!isObject(GuiDepotButtonProfile_1)) new GuiControlProfile(GuiDepotButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 0";
   fontColorSEL = "150 150 150";
   fontColorNA = "150 150 150";
   justify = "center";
	 bitmap  = "./images/GUIWindow53_1_006";
   hasBitmapArray = true;
};

//�ֿⰴť3
if(!isObject(GuiDepotButtonProfile_3)) new GuiControlProfile(GuiDepotButtonProfile_3)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIWindow53_1_003";
   hasBitmapArray = true;
};

//�ֿⰴť4
if(!isObject(GuiDepotButtonProfile_4)) new GuiControlProfile(GuiDepotButtonProfile_4)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIWindow53_1_004";
   hasBitmapArray = true;
};

//�ֿⰴť5
if(!isObject(GuiDepotButtonProfile_5)) new GuiControlProfile(GuiDepotButtonProfile_5)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "211 255 115";
	 fontColorHL = "255 255 0";
   fontColorSEL = "150 150 150";
   fontColorNA = "150 150 150";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_012";
   hasBitmapArray = true;
};

//�ֿⰴť6
if(!isObject(GuiDepotButtonProfile_6)) new GuiControlProfile(GuiDepotButtonProfile_6)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_010";
   hasBitmapArray = true;
};

//��̯��ť1
if(!isObject(GuiStallButtonProfile_1)) new GuiControlProfile(GuiStallButtonProfile_1)
{
	 fontType = "����_GB2312";
   fontSize = 14;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIWindow55_1_010";
   hasBitmapArray = true;
   bitmapFont = false;
};

//��̯��ť2
if(!isObject(GuiStallButtonProfile_2)) new GuiControlProfile(GuiStallButtonProfile_2)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_017";
   hasBitmapArray = true;
};

//��̯��ť3
if(!isObject(GuiStallButtonProfile_3)) new GuiControlProfile(GuiStallButtonProfile_3)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_018";
   hasBitmapArray = true;
};

//����������С����ť
if(!isObject(GuiChatFriendButtonProfile_1)) new GuiControlProfile(GuiChatFriendButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_003";
   hasBitmapArray = true;
};

//������水ť1
if(!isObject(GuiTeamButtonProfile)) new GuiControlProfile( GuiTeamButtonProfile )
{
	 bitmap  = "./images/GUIWindow26_1_003";
   hasBitmapArray = true;
};

//�������水ť1
if(!isObject(GuiBagButtonProfile_1)) new GuiControlProfile(GuiBagButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbotton01_1_011";
   hasBitmapArray = true;
};

//�������水ť2
if(!isObject(GuiBagButtonProfile_2)) new GuiControlProfile(GuiBagButtonProfile_2)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "150 150 150";
   fontColorNA = "150 150 150";
   justify = "left";
	 bitmap  = "./images/GUIbutton02_1_025";
   hasBitmapArray = true;
};

//�������水ť3
if(!isObject(GuiBagButtonProfile_3)) new GuiControlProfile(GuiBagButtonProfile_3)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "150 150 150";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_026";
   hasBitmapArray = true;
};

//�������水ť4
if(!isObject(GuiBagButtonProfile_4)) new GuiControlProfile(GuiBagButtonProfile_4)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_027";
   hasBitmapArray = true;
};

//�������水ť5
if(!isObject(GuiBagButtonProfile_5)) new GuiControlProfile(GuiBagButtonProfile_5)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_028";
   hasBitmapArray = true;
};

//�������水ť6
if(!isObject(GuiBagButtonProfile_6)) new GuiControlProfile(GuiBagButtonProfile_6)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_029";
   hasBitmapArray = true;
};

//�������水ť7
if(!isObject(GuiBagButtonProfile_7)) new GuiControlProfile(GuiBagButtonProfile_7)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_030";
   hasBitmapArray = true;
};

//��ӭ���水ť1
if(!isObject(GuiWalcomeButtonProfile_1)) new GuiControlProfile(GuiWalcomeButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_032";
   hasBitmapArray = true;
};

//��ʱ������С��ť
if(!isObject(GuiTimerButtonProfile_downSize)) new GuiControlProfile(GuiTimerButtonProfile_downSize)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_043";
   hasBitmapArray = true;
};

//��ʱ����Ŵ�ť
if(!isObject(GuiTimerButtonProfile_MaxSize)) new GuiControlProfile(GuiTimerButtonProfile_MaxSize)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_044";
   hasBitmapArray = true;
};

//��ʱ����رհ�ť
if(!isObject(GuiTimerButtonProfile_Close)) new GuiControlProfile(GuiTimerButtonProfile_Close)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_045";
   hasBitmapArray = true;
};

//��ͼƬ��ť
if(!isObject(GuiNotImageButtonProfile)) new GuiControlProfile(GuiNotImageButtonProfile)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
   hasBitmapArray = true;
};

//������水ť1
if(!isObject(GuiResurgenceButtonProfile_1)) new GuiControlProfile(GuiResurgenceButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_046";
   hasBitmapArray = true;
};

//�ʼ����水ť2
if(!isObject(GuiMailButtonProfile_2)) new GuiControlProfile(GuiMailButtonProfile_2)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIWindow40_1_008";
   hasBitmapArray = true;
};

//�ʼ����水ť3
if(!isObject(GuiMailButtonProfile_3)) new GuiControlProfile(GuiMailButtonProfile_3)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton02_1_048";
   hasBitmapArray = true;
};

//������ʾ��ť1
if(!isObject(GuiUpgradeButtonProfile_1)) new GuiControlProfile(GuiUpgradeButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "243 255 160";
	 fontColorHL = "255 255 255";
	 fontColorNA = "177 177 177";
   fontColorSEL = "58 209 94";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_004";
   hasBitmapArray = true;
};

//��׭¼��ť1
if(!isObject(GuiXianZhuanNoteButtonProfile_1)) new GuiControlProfile(GuiXianZhuanNoteButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "243 250 160";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_005";
   hasBitmapArray = true;
};

//��׭¼��ť2
if(!isObject(GuiXianZhuanNoteButtonProfile_2)) new GuiControlProfile(GuiXianZhuanNoteButtonProfile_2)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "243 250 160";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_006";
   hasBitmapArray = true;
};

//�̳ǰ�ť1
if(!isObject(GuiBusinessCityButtonProfile_1)) new GuiControlProfile(GuiBusinessCityButtonProfile_1)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "243 250 160";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_008";
   hasBitmapArray = true;
};

//�̳ǰ�ť2
if(!isObject(GuiBusinessCityButtonProfile_2)) new GuiControlProfile(GuiBusinessCityButtonProfile_2)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "243 250 160";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_009";
   hasBitmapArray = true;
};

//Ԫ���츳�û���ť
if(!isObject(GuiSoulGeniusButtonProfile)) new GuiControlProfile(GuiSoulGeniusButtonProfile)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "243 250 160";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_010";
   hasBitmapArray = true;
};

//�������ȸйرհ�ť
if(!isObject(GuiChatHotButtonProfile)) new GuiControlProfile(GuiChatHotButtonProfile)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "243 250 160";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_007";
   hasBitmapArray = true;
};

//С��ͼ���߰�ť
if(!isObject(GuiBirdMapChangeLineProfile)) new GuiControlProfile(GuiBirdMapChangeLineProfile)
{
	 fontType = "����";
   fontSize = 12;
	 fontColor = "243 250 160";
	 fontColorHL = "255 255 255";
   fontColorSEL = "175 175 175";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_013";
   hasBitmapArray = true;
};

//�Ҽ��˵���ť
if(!isObject(GuiPopUpButtonProfile)) new GuiControlProfile(GuiPopUpButtonProfile)
{ 
	 opaque = true;
   border = true;
   fontType = "����";
   fontSize = 12;
   fontColor = "229 229 229";
   fontColorHL = "122 255 238";
   fontColorNA = "175 175 175";
   fontColorSEL= "46 25 0";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = true;
   bitmap  = "./images/GUIbutton04_1_011";
   hasBitmapArray = true;
   bitmapfont = false;
};

//��֤��ѡ��ť1
if(!isObject(GuiCheckButtonProfile_1)) new GuiControlProfile(GuiCheckButtonProfile_1)
{
	 fontType = "����";
   fontSize = 14;
	 fontColor = "255 255 255";
	 fontColorHL = "255 255 255";
   fontColorSEL = "150 150 150";
   justify = "center";
	 bitmap  = "./images/GUIbutton04_1_012";
   hasBitmapArray = true;
};
//-----------------------------------------------------------------------------------------------------------------------

if(!isObject(GuiCheckBoxProfile)) new GuiControlProfile (GuiCheckBoxProfile)
{
   opaque = false;
   fillColor = "232 232 232";
   border = false;
   borderColor = "0 0 0";
   fontSize = 14;
   fontColor = "0 0 0";
   fontColorHL = "32 100 100";
   fixedExtent = true;
   justify = "left";
   bitmap = "./images/checkBox";
   hasBitmapArray = true;
};

//---------------------------------------------------------��ѡ��ť------------------------------------------------------
//ͨ�ù�ѡ��ť1
if(!isObject(GuiGeneralCheckBoxProfile_1)) new GuiControlProfile (GuiGeneralCheckBoxProfile_1)
{
   opaque = false;
   fillColor = "232 232 232";
   border = false;
   borderColor = "0 0 0";
   fontSize = 14;
   fontColor = "0 0 0";
   fontColorHL = "32 100 100";
   fixedExtent = true;
   justify = "left";
   bitmap = "./images/testcheckbox";
   hasBitmapArray = true;
};

//����Ƶ����ѡ��
if(!isObject(GuiCloseChunnelCheckBoxProfile)) new GuiControlProfile (GuiCloseChunnelCheckBoxProfile)
{
   opaque = false;
   fillColor = "232 232 232";
   border = false;
   borderColor = "0 0 0";
   fontSize = 14;
   fontColor = "0 0 0";
   fontColorHL = "32 100 100";
   fixedExtent = true;
   justify = "left";
   bitmap = "./images/GUIWindow15_1_021.png";
   hasBitmapArray = true;
};


//��½��ѡ��ť
if(!isObject(GuiLoginCheckBtnProfile)) new GuiControlProfile ("GuiLoginCheckBtnProfile")
{
   
   opaque = false;
   border = false;
   fontType = "����";
   fontSize = 12;
   fillColor = "255 255 255";
   fontColor = "255 255 255";
   fontColorHL = "255 255 255";
   fixedExtent = true;
   justify = "left";
   bitmap = "./images/GUIbutton02_1_003";
   hasBitmapArray = true;
};


//ϵͳ���ý��湴ѡ��ť
if(!isObject(GuiSystemSettingCheckBtnProfile)) new GuiControlProfile ("GuiSystemSettingCheckBtnProfile")
{
   
   opaque = false;
   border = false;
   fontType = "����";
   fontSize = 12;
   fillColor = "255 255 255";
   fontColor = "255 255 255";
   fontColorHL = "255 255 255";
   fixedExtent = true;
   justify = "left";
   bitmap = "./images/GUIbutton02_1_009";
   hasBitmapArray = true;
};

//----------------------------------------------------------------------------------------------------------------

if(!isObject(GuiRadioProfile)) new GuiControlProfile (GuiRadioProfile)
{
   fontSize = 14;
   fillColor = "232 232 232";
   fontColorHL = "32 100 100";
   fixedExtent = true;
   bitmap = "./images/radioButton";
   hasBitmapArray = true;
};

if(!isObject(GuiScrollProfile)) new GuiControlProfile (GuiScrollProfile)
{
   opaque = true;
   fillcolor = "255 255 255";
   borderColor = GuiDefaultProfile.borderColor;
   border = true;
   bitmap = "./images/scrollBar";
   hasBitmapArray = true;
};

//-------------------------------------------------------------������-----------------------------------------------------------
if(!isObject(GuiCurrencyScrollProfile_1)) new GuiControlProfile (GuiCurrencyScrollProfile_1)
{
   opaque = false;
   fillcolor = "255 255 255";
   //borderColor = GuiDefaultProfile.borderColor;
   border = false;
   bitmap = "./images/GUIWindow99_1_002";
   hasBitmapArray = true;
};

if(!isObject(GuiCurrencyScrollProfile_2)) new GuiControlProfile (GuiCurrencyScrollProfile_2)
{
   opaque = false;
   fillcolor = "255 255 255";
   //borderColor = GuiDefaultProfile.borderColor;
   border = false;
   bitmap = "./images/GUIWindow28_1_009";
   hasBitmapArray = true;
};

if(!isObject(GuiCurrencyScrollProfile_3)) new GuiControlProfile (GuiCurrencyScrollProfile_3)
{
   opaque = false;
   fillcolor = "255 255 255";
   //borderColor = GuiDefaultProfile.borderColor;
   border = false;
   bitmap = "./images/GUIWindow99_1_002";
   hasBitmapArray = true;
   modal = false;
};

if(!isObject(GuiCurrencyScrollProfile_4)) new GuiControlProfile (GuiCurrencyScrollProfile_4)
{
   opaque = false;
   fillcolor = "255 255 255";
   //borderColor = GuiDefaultProfile.borderColor;
   border = false;
   bitmap = "./images/GUIbutton01_1_014";
   hasBitmapArray = true;
   modal = true;
};

if(!isObject(GuiCurrencyScrollProfile_5)) new GuiControlProfile (GuiCurrencyScrollProfile_5)
{
   opaque = false;
   fillcolor = "255 255 255";
   //borderColor = GuiDefaultProfile.borderColor;
   border = false;
   bitmap = "./images/GUIbutton02_1_037";
   hasBitmapArray = true;
   modal = true;
};

//------------------------------------------------------------------------------------------------------------------------------

if(!isObject(GuiTransparentScrollProfile)) new GuiControlProfile (GuiTransparentScrollProfile)
{
   opaque = false;
   fillColor = "255 255 255";
   border = false;
   borderThickness = 2;
   borderColor = "0 0 0";
   bitmap = "./images/scrollBar";
   hasBitmapArray = true;
};
 
if(!isObject(GuiSliderProfile)) new GuiControlProfile (GuiSliderProfile)
{
   bitmap = "./images/slider";
};

if(!isObject(GuiSliderProfile_1)) new GuiControlProfile (GuiSliderProfile_1)
{
   bitmap = "./images/GUIWindow13_1_003";
};

if(!isObject(GuiPaneProfile)) new GuiControlProfile(GuiPaneProfile)
{
   bitmap = "./images/popupMenu";
   hasBitmapArray = true;
};

if(!isObject(GuiPopupMenuItemBorder)) new GuiControlProfile ( GuiPopupMenuItemBorder : GuiButtonProfile )
{
   borderThickness = 1;
   borderColor = "51 51 53 200";
   borderColorHL = "51 51 53 200";
};

if(!isObject(GuiPopUpMenuDefault)) new GuiControlProfile (GuiPopUpMenuDefault : GuiDefaultProfile )
{
   opaque = true;
   mouseOverSelected = true;
   textOffset = "3 3";
   border = 1;
   borderThickness = 0;
   fixedExtent = true;
   bitmap = "./images/scrollbar";
   hasBitmapArray = true;
   profileForChildren = GuiPopupMenuItemBorder;
   fillColor = "255 255 255 100";
   fillColorHL = "100 100 100";
   fontColorHL = "220 220 220";
   borderColor = "100 100 108";
};

if(!isObject(GuiPopupBackgroundProfile)) new GuiControlProfile (GuiPopupBackgroundProfile)
{
   modal = true;
};


if(!isObject(GuiPopUpMenuProfile)) new GuiControlProfile (GuiPopUpMenuProfile : GuiPopUpMenuDefault)
{
   textOffset         = "6 3";
   bitmap             = "./images/dropDown";
   hasBitmapArray     = true;
   border             = -3;
   profileForChildren = GuiPopUpMenuDefault;
};

if(!isObject(GuiPopUpMenuEditProfile)) new GuiControlProfile (GuiPopUpMenuEditProfile : GuiPopUpMenuDefault)
{
   textOffset         = "6 3";
   canKeyFocus        = true;
   bitmap             = "./images/dropDown";
   hasBitmapArray     = true;
   border             = -3;
   profileForChildren = GuiPopUpMenuDefault;
};

//------------------------------------------------������ť-----------------------------------------------------
if(!isObject(GuiPopUpMenuGameProfile)) new GuiControlProfile (GuiPopUpMenuGameProfile : GuiDefaultProfile )
{
   opaque = true;
   mouseOverSelected = true;
   textOffset = "3 3";
   border = 1;
   borderThickness = 0;
   fixedExtent = true;
   bitmap = "./images/GUIbutton01_1_014";
   hasBitmapArray = true;
   profileForChildren = GuiPopupMenuItemBorder;
   fillColor         = "0 0 0 100";
	 fillColorHL       = "139 88 0";
   fillColorNA       = "8 80 170";
   borderColor       = "100 100 108";
};
//����������������ͨ�ð�ť1
if(!isObject(GuiPopUpMenuProfile_Currency_1)) new GuiControlProfile (GuiPopUpMenuProfile_Currency_1 : GuiPopUpMenuDefault)
{
   textOffset         = "6 3";
   bitmap             = "./images/GUIButton01_1_010";
   hasBitmapArray     = true;
   border             = 1;
   profileForChildren = GuiPopUpMenuGameProfile;
   fontColor          = "255 255 255";
   fontColorHL        = "255 255 255";
   fontColorNA        = "255 255 255";
   fontColorSEL       = "255 255 255";
};

//����������������ͨ�ð�ť2
if(!isObject(GuiPopUpMenuProfile_Currency_2)) new GuiControlProfile (GuiPopUpMenuProfile_Currency_2 : GuiPopUpMenuDefault)
{
   textOffset         = "6 3";
   bitmap             = "./images/GUIbutton02_1_023";
   hasBitmapArray     = true;
   border             = 1;
   profileForChildren = GuiPopUpMenuGameProfile;
   fontColor          = "255 255 255";
   fontColorHL        = "255 255 255";
   fontColorNA        = "255 255 255";
   fontColorSEL       = "255 255 255";
};

//��������������ͨ�ð�ť1
if(!isObject(GuiPopUpMenuEditProfile_Currency_1)) new GuiControlProfile (GuiPopUpMenuEditProfile_Currency_1 : GuiPopUpMenuDefault)
{
   textOffset         = "6 3";
   canKeyFocus        = true;
   bitmap             = "./images/GUIButton01_1_010";
   hasBitmapArray     = true;
   border             = -3;
   profileForChildren = GuiPopUpMenuDefault;
};
//-----------------------------------------------------------------------------------------------------------------

if(!isObject(GuiListBoxProfile)) new GuiControlProfile (GuiListBoxProfile)
{
   tab = false;
   canKeyFocus = true;
};

if(!isObject(GuiTabBookProfile)) new GuiControlProfile (GuiTabBookProfile)
{
   fillColorHL = "64 150 150";
   fillColorNA = "150 150 150";
   fontColor = "30 30 30";
   fontColorHL = "32 100 100";
   fontColorNA = "0 0 0";
   fontType = "���� Bold";
   fontSize = 14;
   justify = "center";
   bitmap = "./images/tab";
   tabWidth = 64;
   tabHeight = 24;
   tabPosition = "Top";
   tabRotation = "Horizontal";
   textOffset = "0 -3";
   tab = false;
   cankeyfocus = true;
   border = false;
   opaque = false;
};

//--------------------------------------------------��ҳ------------------------------------------------------
if(!isObject(GuiCurrencyTabBookProfile_1)) new GuiControlProfile (GuiCurrencyTabBookProfile_1)
{
   fillColorHL = "64 150 150";
   fillColorNA = "150 150 150";
   fontColor = "178 214 154";
   fontColorHL = "32 100 100";
   fontColorNA = "255 255 255";
   fontColorSEL = "255 255 255";
   fontType = "����";
   fontSize = 12;
   justify = "center";
   bitmap = "./images/GUIWindow33_1_008";
   tabWidth = 64;
   tabHeight = 24;
   tabPosition = "Top";
   tabRotation = "Horizontal";
   textOffset = "0 0";
   tab = false;
   cankeyfocus = true;
   border = false;
   opaque = false;
};

//--------------------------------------------------------------------------------------------------------------

if(!isObject(GuiTabPageProfile)) new GuiControlProfile (GuiTabPageProfile : GuiDefaultProfile )
{
   opaque = false;
};

if(!isObject(GuiMenuBarProfile)) new GuiControlProfile (GuiMenuBarProfile)
{
   fontType = "����";
   fontSize = 15;
   opaque = true;
   fillColor = "239 237 222";
   fillColorHL = "102 153 204";
   borderColor = "138 134 122";
   borderColorHL = "0 51 153";
   border = 5;
   fontColor = "0 0 0";
   fontColorHL = "255 255 255";
   fontColorNA = "128 128 128";
   fixedExtent = true;
   justify = "center";
   canKeyFocus = false;
   mouseOverSelected = true;
   bitmap = "./images/menu";
   hasBitmapArray = true;
};

if(!isObject(GuiConsoleProfile)) new GuiControlProfile (GuiConsoleProfile)
{
   fontType = ($platform $= "macos") ? "Monaco" : "Lucida Console";
   fontSize = ($platform $= "macos") ? 13 : 12;
    fontColor = "255 255 255";
    fontColorHL = "155 155 155";
    fontColorNA = "255 0 0";
    fontColors[6] = "100 100 100";
    fontColors[7] = "100 100 0";
    fontColors[8] = "0 0 100";
    fontColors[9] = "0 100 0";
};

if(!isObject(GuiConsoleTextEditProfile)) new GuiControlProfile (GuiConsoleTextEditProfile : GuiTextEditProfile)
{
   fontType = ($platform $= "macos") ? "Monaco" : "Lucida Console";
   fontSize = ($platform $= "macos") ? 13 : 12;
};

if (!isObject(GuiTreeViewProfile)) new GuiControlProfile (GuiTreeViewProfile)
{  
   bitmap            = "./images/treeView";
   autoSizeHeight    = true;
   canKeyFocus       = true;
   
   fontType = "����";
   fillColor = GuiDefaultProfile.fillColor;
   fillColorHL = GuiDefaultProfile.fillColorHL;
   fillColorNA = GuiDefaultProfile.fillColorNA;

   fontColor = "0 0 0";
   fontColorHL = "255 255 255";
   fontColorNA = GuiDefaultProfile.fontColorNA;
   fontColorSEL= "236 100 0";

   fontSize = 12;
   
   opaque = false;
   border = false;
};

if (!isObject(GuiTreeViewProfile_1)) new GuiControlProfile (GuiTreeViewProfile_1 : GuiTreeViewProfile)
{  
   bitmap            = "./images/GUIbutton02_1_011";
};

if(!isObject(GuiSimpleTreeProfile)) new GuiControlProfile (GuiSimpleTreeProfile : GuiTreeViewProfile)
{
   opaque = true;
   fillColor = "255 255 255 255";
   border = true;
};

if(!isObject(GuiRSSFeedMLTextProfile)) new GuiControlProfile ("GuiRSSFeedMLTextProfile")
{
   fontColorLink = "55 55 255";
   fontColorLinkHL = "255 55 55";
};

if(!isObject(ConsoleScrollProfile)) new GuiControlProfile( ConsoleScrollProfile : GuiScrollProfile )
{
	opaque = true;
	fillColor = "0 0 0 120";
	border = 3;
	borderThickness = 0;
	borderColor = "0 0 0";
};

if(!isObject(GuiTextPadProfile)) new GuiControlProfile( GuiTextPadProfile )
{
   fontType = ($platform $= "macos") ? "Monaco" : "Lucida Console";
   fontSize = ($platform $= "macos") ? 13 : 12;
   tab = false;
   canKeyFocus = true;
   
   // Deviate from the Default
   opaque=true;  
   fillColor = "255 255 255";
   
   border = 0;
};

if(!isObject(GuiTransparentProfileModeless)) new GuiControlProfile (GuiTransparentProfileModeless : GuiTransparentProfile) 
{
   modal = false;
};

if(!isObject(GuiParticleListBoxProfile)) new GuiControlProfile (GuiParitcleListBoxProfile : GuiListBoxProfile)
{
   tab = false;
   canKeyFocus = true;
   fontColor = "0 0 0";
   fontColorHL = "25 25 25 220";
   fontColorNA = "128 128 128";
   fontColor = "0 0 0 150";
};

if(!isObject(GuiFormProfile)) new GuiControlProfile(GuiFormProfile : GuiTextProfile )
{
   opaque = false;
   border = 5;
   
   bitmap = "./images/form";
   hasBitmapArray = true;

   justify = "center";
   
   profileForChildren = GuiButtonProfile;
   
   // border color
   borderColor   = "153 153 153"; 
   borderColorHL = "230 230 230";
   borderColorNA = "126 79 37";
};

if (!isObject(GuiBreadcrumbsMenuProfile)) new GuiControlProfile(GuiBreadcrumbsMenuProfile)
{
   fontColor = "0 0 0";
   fontType = "����";
   fontSize = 14;
   
   bitmap = "./images/breadcrumbs";
   hasBitmapArray = true;
};

if (!isObject(TargetDecal)) new ProjectDecalData(TargetDecal)
{
   TextureName = "./images/TargetDecal";
};

if (!isObject(CastDecal)) new ProjectDecalData(CastDecal)
{
   TextureName = "./images/CastDecal";
};

if(!isObject(GuiPlayerIconProfile)) new GuiControlProfile (GuiPlayerIconProfile)
{
	 justify = "center";
	 fontType = "����";
   fontSize = 12;
   //modal = false;
   justify = "center";
   fontOutline = "50 50 50";
   fontColor = "255 255 255";
};

if(!isObject(GuiMouseGamePlayProfile)) new GuiControlProfile (GuiMouseGamePlayProfile : GuiContentProfile)
{
	 fontType = "����";
   fontSize = 10;
   fontOutline = "50 50 50";
   fontBold = true;
};

if(!isObject(MouseGamePlayStyleName)) new GuiControlProfile (MouseGamePlayStyleName : GuiMouseGamePlayProfile)
{
	 fontType = "����";
   fontSize = 14;
   fontOutline = "50 50 50";
   fontBold = true;
};

if(!isObject(MouseGamePlayStyleOther)) new GuiControlProfile (MouseGamePlayStyleOther : GuiMouseGamePlayProfile)
{
	 fontType = "����";
   fontSize = 12;
   fontOutline = "50 50 50";
   fontBold = true;
};

//ͨ���ȸ�
if(!isObject(GuiHoverdefaultProfile)) new GuiControlProfile (GuiHoverdefaultProfile:GuiDefaultProfile)
{
	opaque = false;
	hasBitmapArray = true;
	fillColor = "4 24 41 229";
	border = -1;
	modal = false;
	bitmap = "./images/Hover";
};

//���촰�ڱ���Profile
if(!isObject(GuiChatBackGroundProfile)) new GuiControlProfile (GuiChatBackGroundProfile)
{
	opaque = false;
	modal = false;
	hasBitmapArray = true;
	fillColor = "0 0 0 180";
	border = -1;
	bitmap = "./images/GUIWindow15_1_001";
};

//ʱ�䴥��������Profile_1
if(!isObject(GuiTimerTriggerProfile_1)) new GuiControlProfile (GuiTimerTriggerProfile_1)
{
	opaque = false;
	modal = false;
	hasBitmapArray = true;
	fillColor = "0 0 0 178";
	border = -1;
	bitmap = "./images/GUIWindow99_1_008";
};

//�Ի���
if(!isObject(GuiDialogControlProfile)) new GuiControlProfile (GuiDialogControlProfile)
{
	opaque = false;
	modal = false;
	hasBitmapArray = true;
	fillColor = "6 15 32 178";
	border = -1;
	bitmap = "./images/GUIWindow56_1_002";
};

//�����̵�
if(!isObject(GuiShopControlProfile)) new GuiControlProfile (GuiShopControlProfile)
{
	opaque = false;
	modal = false;
	hasBitmapArray = true;
	fillColor = "15 85 116 204";
	border = -1;
	bitmap = "./images/GUIWindow99_1_018";
};

//���ͼ����ƶ���ʾ��
if(!isObject(GuiBigMapViewHotProfile)) new GuiControlProfile (GuiBigMapViewHotProfile)
{
	opaque = false;
	modal = false;
	hasBitmapArray = true;
	fillColor = "0 0 0 178";
	border = -1;
	bitmap = "./images/GUIWindow99_1_009";
};

if(!isObject(GuiMissionTraceHoverProfile)) new GuiControlProfile (GuiMissionTraceHoverProfile:GuiDefaultProfile)
{
	opaque = false;
	hasBitmapArray = true;
	fillColor = "0 0 0 191";
	border = -1;
	bitmap = "./images/GUIWindow12_1_014";
};

//����ָ��(����)
if(!isObject(GuiHelpDirectProfile_1)) new GuiControlProfile (GuiHelpDirectProfile_1)
{
	opaque = false;
	modal = false;
	hasBitmapArray = true;
	fillColor = "164 134 79 255";
	border = -1;
	bitmap = "./images/GUIWindow99_1_025";
};
//����ָ��(����)
if(!isObject(GuiHelpDirectProfile_2)) new GuiControlProfile (GuiHelpDirectProfile_2)
{
	opaque = false;
	modal = false;
	hasBitmapArray = true;
	fillColor = "164 134 79 255";
	border = -1;
	bitmap = "./images/GUIWindow99_1_022";
};
//����ָ��(����)
if(!isObject(GuiHelpDirectProfile_3)) new GuiControlProfile (GuiHelpDirectProfile_3)
{
	opaque = false;
	modal = false;
	hasBitmapArray = true;
	fillColor = "164 134 79 255";
	border = -1;
	bitmap = "./images/GUIWindow99_1_024";
};
//����ָ��(����)
if(!isObject(GuiHelpDirectProfile_4)) new GuiControlProfile (GuiHelpDirectProfile_4)
{
	opaque = false;
	modal = false;
	hasBitmapArray = true;
	fillColor = "164 134 79 255";
	border = -1;
	bitmap = "./images/GUIWindow99_1_023";
};

if(!isObject(GuiDialogOptionButton)) new GuiControlProfile (GuiDialogOptionButton)
{
   opaque = false;
   border = true;
   fixedExtent = true;
   justify = "left";
   canKeyFocus = true;
   bitmap  = "./images/GUIWindow25_1_002";
   hasBitmapArray = true;
   fontColor = "255 255 255";
   fontColorHL = "0 255 255";
   fontColorSEL = "130 95 36";
};

if(!isObject(GuiCoverImageButtonProfile)) new GuiControlProfile(GuiCoverImageButtonProfile:GuiButtonProfile)
{
	 canKeyFocus = true;
   bitmap  = "./images/GUIWindow29_1_004.png";
   hasBitmapArray = true;
};

if(!isObject(GuiRichTextProfile)) new GuiControlProfile(GuiRichTextProfile)
{
	 fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
   bitmapFont = true;
};

if(!isObject(GuiHoverRichTextProfile)) new GuiControlProfile(GuiHoverRichTextProfile)
{
	 modal = false;
	 fontType = "����";
   fontSize = 12;
   fontColor = "255 255 255";
};

if(!isobject(GuiProgressCtrlExProfile)) new GuiControlProfile(GuiProgressCtrlExProfile:GuiTextProfile)
{
	 modal = false;
	 justify = "center";
	 fontType = "����";
	 fontSize = 12;
	 fontOutline = "50 50 50";
   fontColor = "255 255 255";
};

if(!isobject(GuiTimerRichTextCtrlProfile)) new GuiControlProfile(GuiTimerRichTextCtrlProfile)
{
	 modal = false;
	 justify = "left";
	 fontType = "����";
	 fontSize = 13;
   fontColor = "10 255 10";
   bold = "1";
};

if(!isObject(GuiRichTextProfile_Mission)) new GuiControlProfile(GuiRichTextProfile_Mission)
{
	 fontType = "����";
   fontSize = 12;
   bitmapFont = true;
   fontOutline = "45 45 45";
   fontColor = "255 255 255";   
};
//��
if(!isObject(GuiMapEastBtnProfile)) new GuiControlProfile (GuiMapEastBtnProfile:GuiButtonProfile)
{
   hasBitmapArray = true;
   bitmap  = "./images/GUIbutton02_1_033.png";
};
//��
if(!isObject(GuiMapWestBtnProfile)) new GuiControlProfile (GuiMapWestBtnProfile:GuiButtonProfile)
{
   hasBitmapArray = true;
   bitmap  = "./images/GUIbutton02_1_035.png";
};
//��
if(!isObject(GuiMapSouthBtnProfile)) new GuiControlProfile (GuiMapSouthBtnProfile:GuiButtonProfile)
{
   hasBitmapArray = true;
   bitmap  = "./images/GUIbutton02_1_034.png";
};
//��
if(!isObject(GuiMapNorthBtnProfile)) new GuiControlProfile (GuiMapNorthBtnProfile:GuiButtonProfile)
{
   hasBitmapArray = true;
   bitmap  = "./images/GUIbutton02_1_036.png";
};

//BUFF
if(!isObject(GuiBuffProfile)) new GuiControlProfile (GuiBuffProfile)
{
	 justify = "center";
	 fontType = "����";
	 fontSize = 14;
	 modal = true;
   fontColor = "255 255 255";
   bitmapFont = false;
   fontOutline = "50 50 50";
   //fontBold = true;
};

//�Ҽ������˵�
if(!isObject(GuiRightDownProfile)) new GuiControlProfile (GuiRightDownProfile:GuiDefaultProfile)
{
	opaque = false;
	hasBitmapArray = true;
	fillColor = "0 0 0 191";
	border = -1;
	modal = false;
	bitmap = "./images/GUIWindow99_1_012";
};

//�������
if(!isObject(GuiResurgenceWndProfile)) new GuiControlProfile (GuiResurgenceWndProfile:GuiDefaultProfile)
{
	opaque = false;
	hasBitmapArray = true;
	fillColor = "0 0 0 191";
	border = -1;
	modal = false;
	bitmap = "./images/GUIWindow56_1_002";
};

if(!isObject(GuiListBoxAdvProfile)) new GuiControlProfile(GuiListBoxAdvProfile)
{
	 fontType = "����";
   fontSize = 12;
   justify = "left";
   fontColor = "255 255 255";
   tab = false;
   canKeyFocus = true;
};
//ͷ��ð��
if(!isObject(GuiChatInfoProfile)) new GuiControlProfile (GuiChatInfoProfile:GuiDefaultProfile)
{
	opaque = false;
	modal  = false;
	fontType = "����";
  fontSize = 12;
  justify = "center";
  bitmapFont  = true;
  fontOutline = "50 50 50";
	fillColor = "9 27 62 235";
	fontColor = "255 255 255";
	border = -1;
	hasBitmapArray = true;
	bitmap = "./images/GUIWindowChat";
};

if(!isObject(guiBirdViewMapProfile)) new GuiControlProfile(guiBirdViewMapProfile)
{
   justify = "center";
	 fontType = "����";
	 fontSize = 12;
	 modal = true;
   fontColor = "255 255 255";
   fontOutline = "50 50 50";
   bitmapFont = true;
};

if(!isObject(guiBigViewMapProfile)) new GuiControlProfile(guiBigViewMapProfile : GuiChatBackGroundProfile)
{
   justify = "center";
	 fontType = "����";
	 fontSize = 12;
	 modal = true;
	 opaque = false;
   fontColor = "255 255 255";
   fontOutline = "50 50 50";
   bitmapFont = true;
};