//===========================================================================================================
// 文档说明:帮助指引操作
// 创建时间:2010-3-10
// 创建人: Batcel
//===========================================================================================================
//%Horiz (1 左偏移 2中  3右偏移) %Vert(1 上偏移 2中  3下偏移)
function OpenHelpDirectWnd(%aimCtrl,%type,%Horiz,%Vert,%txt,%time)
{ 
	if(!isobject(%aimCtrl))
	    return;
	    
  for(%i =1;%i<=4;%i++)
	   ("HelpDirectWndGui_" @ %i).setVisible(0);
	   
	%aimCtrlPos = %aimCtrl.getPosition();
	
	%ExtentY = 80;
	%lineCount = mFloor(strlen(%txt)*0.045)-1;
	if(%lineCount>0)
	  %ExtentY+=%lineCount*16;
	
	("HelpDirectWndGui_" @ %type).setExtent(160,%ExtentY);
	
	%aimCtrlextent = %aimCtrl.getExtent();
	("HelpDirectContent_" @%type).setContent(%txt);
	%NewPosX = 0;
	%NewPosY = 0;
		
	if(%Horiz == 1)
	{
		%NewPosX = getWord(%aimCtrlPos,0)-getWord(("HelpDirectWndGui_" @ %type).extent,0);
	}
	else if(%Horiz == 2)
	{
		%NewPosX = getWord(%aimCtrlPos,0)+getWord(%aimCtrlextent,0)*0.5;
	}
	else if(%Horiz == 3)
	{
		%NewPosX = getWord(%aimCtrlPos,0)+getWord(%aimCtrlextent,0);
	}
	
	if(%Vert == 1)
	{
		%NewPosY = getWord(%aimCtrlPos,1)-getWord(("HelpDirectWndGui_" @ %type).extent,1);
	}
	else if(%Vert == 2)
	{
		%NewPosY = getWord(%aimCtrlPos,1)+getWord(%aimCtrlextent,1)*0.5;
	}
	else if(%Vert == 3)
	{
		%NewPosY = getWord(%aimCtrlPos,1)+getWord(%aimCtrlextent,1);		
	}
	echo(%NewPosX @"======" @ %NewPosY);
	("HelpDirectWndGui_" @ %type).setPosition(%NewPosX,%NewPosY);
	GameMainWndGui_Layer5.pushtoback("HelpDirectWndGui_" @ %type);
	("HelpDirectWndGui_" @ %type).setVisible(1);
	
	if(%time !=0)
	  schedule(%time,0,"CloseHelpDirectWnd",%type);	
}

function CloseHelpDirectWnd(%type)
{
  ("HelpDirectWndGui_" @ %type).setVisible(0);
}

//OpenHelpDirectWnd("BottomControlWndGui",4,2,1,"城市似的堵塞外双打答复",0);