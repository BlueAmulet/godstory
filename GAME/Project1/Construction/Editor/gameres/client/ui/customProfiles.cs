//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (c) 2002 FireRain.com
//-----------------------------------------------------------------------------

if (isObject(GuiDefaultProfile)) GuiDefaultProfile.delete();
new GuiControlProfile (GuiDefaultProfile)
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
   border = false;
   borderColor   = "0 0 0"; 
   borderColorHL = "197 202 211";
   borderColorNA = "91 101 119";

   // font
   fontType = "Arial";
   fontSize = 14;
   fontCharset = ANSI;

   fontColor = "0 0 0";
   fontColorHL = "73 82 97";
   fontColorNA = "0 0 0";
   fontColorSEL= "226 237 255";

   // bitmap information
   bitmap = "./demoWindow";
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

if (isObject(GuiWindowProfile)) GuiWindowProfile.delete();
new GuiControlProfile (GuiWindowProfile)
{
   opaque = true;
   border = 2;
   fillColor = "107 169 161";
   fillColorHL = "221 202 173";
   fillColorNA = "221 202 173";
   fontColor = "255 255 255";
   fontColorHL = "255 255 255";
   text = "GuiWindowCtrl test";
   bitmap = "./demoWindow";
   textOffset = "6 6";
   hasBitmapArray = true;
   justify = "center";
};

if (isObject(GuiScrollProfile)) GuiScrollProfile.delete();
new GuiControlProfile (GuiScrollProfile)
{
   opaque = true;
   fillColor = "255 255 255";
   border = 3;
   borderThickness = 2;
   borderColor = "0 0 0";
   bitmap = "./demoScroll";
   hasBitmapArray = true;
};

$fontColorHL = "55 64 78";

if (isObject(GuiButtonProfile)) GuiButtonProfile.delete();
new GuiControlProfile (GuiButtonProfile)
{
   opaque = true;
   border = true;
   fontColor = "0 0 0";
   fontColorHL = $fontColorHL;
   fixedExtent = true;
   justify = "center";
   bitmap  = "./Button";
	canKeyFocus = true;
	hasBitmapArray = true;
};
new GuiControlProfile (GuiDefaultButtonProfile)
{
   opaque = true;
   border = true;
   fillColor = "107 169 161";
   fillColorNA = "107 169 161";
   fontColorHL = $fontColorHL;
   fixedExtent = true;
   justify = "center";
	 canKeyFocus = false;
	 hasBitmapArray = false;
};

if (isObject(GuiCheckBoxProfile)) GuiCheckBoxProfile.delete();
new GuiControlProfile (GuiCheckBoxProfile)
{
   opaque = false;
   fillColor = "0 0 0";
   border = false;
   borderColor = "0 0 0";
   fontSize = 14;
   fontColor = "0 0 0";
   fontColorHL = $fontColorHL;
   fixedExtent = true;
   justify = "left";
   bitmap = "./demoCheck";
   hasBitmapArray = true;
};

if (isObject(GuiRadioProfile)) GuiRadioProfile.delete();
new GuiControlProfile (GuiRadioProfile)
{
   fontSize = 14;
   fillColor = "0 0 0";
   fontColorHL = $fontColorHL;
   fixedExtent = true;
   bitmap = "./demoRadio";
   hasBitmapArray = true;
};

if (isObject(GuiTitleProfile)) GuiTitleProfile.delete();
new GuiControlProfile (GuiTitleProfile)
{
   opaque = false;
   fontType = "Arial Bold";
   fontSize = 32;
   fontColor = "255 255 255";
   fontColorHL = "255 255 255";
   justify = "right";
};

if (isObject(GuiPopUpTextProfile)) GuiPopUpTextProfile.delete();
new GuiControlProfile (GuiPopUpTextProfile)
{
   fontType = "Arial";
   fontSize = 14;
   fontColor = "255 255 255";
   fontColorHL = "255 255 255";
   fontColorLink = "255 96 96";
   fontColorLinkHL = "0 0 255";
};

if (isObject(GuiProgressProfile)) GuiProgressProfile.delete();
new GuiControlProfile (GuiProgressProfile)
{
   opaque = false;
   fillColor = "228 0 1 200";
   border = true;
   borderColor   = "0 0 0 255";
};



