//===========================================================================================================
// 文档说明:显示BUFF操作
// 创建时间:2009-8-31
// 创建人: soar
//=========================================================================================================== 

function CloseBuffHover()
{
	BuffHover.setvisible(0);
}

function ShowBuffHover(%mousePoint,%text)
{
	BuffHover.setposition(getword(%mousePoint,0),getword(%mousePoint,1));
	BuffHoverInfo.setcontent(%text);
	BuffHover.setextent(getword(BuffHoverInfo.getextent(),0)+20,getword(BuffHoverInfo.getextent(),1)+20);
	BuffHover.setvisible(1);
}