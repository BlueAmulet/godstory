//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
// 文档说明:场景编辑器工具栏
// 创建时间:2009-2-3
// 创建人: thinking
//-----------------------------------------------------------------------------
function SceneEditirEvent()
{
	SceneEditirButton.setStateOn(true);
	EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,0,"场景编辑");
	SceneToolbarTabCtrl.SetVisible(true);
	TerrainToolbarTabCtrl.SetVisible(false);
	NavigationToolbarCtrl.SetVisible(false);
	SceneToolbarTabCtrl.selectPage(0);
	EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,2,"场景创建器");   
}
function TerrainEditirEvent()
{
	EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,5,"地形编辑器");
	SceneToolbarTabCtrl.SetVisible(false);
	TerrainToolbarTabCtrl.SetVisible(true);
	NavigationToolbarCtrl.SetVisible(false);
  TerrainToolbarTabCtrl.selectPage(0);
}
function NavigationEditirEvent()
{
	EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,10,"导航网格"); 
	SceneToolbarTabCtrl.SetVisible(false);
	TerrainToolbarTabCtrl.SetVisible(false);
	NavigationToolbarCtrl.SetVisible(true); 
}
//改变笔刷尺寸大小
function AlterEditorBrushSize(%Size)
{
  //当前笔刷尺寸大小
  %BrushSize = ETerrainEditor.brushSize;
  //笔刷尺寸加大
  if(%Size>0 && %BrushSize<25)
  {
     switch(%BrushSize)
     {
        case 1:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,1,"尺寸 3 X 3");
        case 3:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,2,"尺寸 5 X 5");
        case 5:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,3,"尺寸 9 X 9");
        case 9:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,4,"尺寸 15 X 15");
        case 15:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,5,"尺寸 25 X 25");
        default: return;
     }
  }
  //笔刷尺寸缩小
  if(%Size<0 && %BrushSize>1)
  {
     switch(%BrushSize)
     {
        case 25:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,4,"尺寸 15 X 15");
        case 15:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,3,"尺寸 9 X 9");
        case 9:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,2,"尺寸 5 X 5");
        case 5:
            EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,1,"尺寸 3 X 3");
        case 3:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,0,"尺寸 1 X 1");
        default:
        return;
     }
  }
}
//笔刷形状与样式选择
function SelectEditorBrushStyle(%Style)
{
	if(%Style ==0)
		EditorBrushMenu::onSelectItem(EditorGui.brushMenu,0,"方形笔刷");
	if(%Style ==1)    
		EditorBrushMenu::onSelectItem(EditorGui.brushMenu,1,"圆形笔刷"); 
	if(%Style ==3)
		EditorBrushMenu::onSelectItem(EditorGui.brushMenu,3,"软笔刷"); 
	if(%Style ==4) 
		EditorBrushMenu::onSelectItem(EditorGui.brushMenu,4,"硬笔刷");
}
//地形操作功能
function TerrainEditirAction(%index)
{
	if(%index !=10)
		EditorGui-->TerrainPainter.setVisible(false);
	//%index工具面板中的序号
	switch(%index)
	{
		case 1:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,7,"调整高度");
		case 2:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,5,"增加高度");
		case 3:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,6,"降低高度");
		case 4:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,10,"设定高度");
			SetTerrainHeight.SetVisible(!SetTerrainHeight.isVisible());
			if(SetTerrainHeight.isVisible())
				SetTerrainHeight.setText(ETerrainEditor.setHeightVal);
			else
				ETerrainEditor.setHeightVal =SetTerrainHeight.getText();
		case 5:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,8,"夷为平地");
		case 6:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,9,"平滑地形");
		case 7:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,12,"挖除地表");
		case 8:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,13,"填充地表");
		case 9:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,16,"清除材质");
		case 10:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,15,"填充材质");
			EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,8,"描绘材质");
		case 11:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,19,"清除光照");
		case 12:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,18,"填充光照");
		
			SetBrushColor.SetVisible(!SetBrushColor.isVisible());
			SetBrushColorButton.SetVisible(!SetBrushColorButton.isVisible());			
			
			if(SetBrushColor.isVisible())
				SetBrushColor.setText(ETerrainEditor.brushColor);
			else
				ETerrainEditor.brushColor = SetBrushColor.getText();
		default:return;
		
	}
}
//导航网格操作功能
function NavigationAction(%index)
{
	//%index工具面板中的序号
	switch(%index)
	{
		case 1:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,0,"绘制导航网格");
		case 2:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,1,"删除导航网格");
		case 3:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,2,"整片删除网格");
		case 4:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,3,"调整高度");
		case 5:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,10,"寻径测试");
		case 6:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,6,"删除全部网格");
		case 7:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,7,"自动生成网格");
		case 8:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,8,"优化并保存网格");
		case 9:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,4,"切割公共边");
        case 10:
            EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,11,"显示优化网格");
        case 11:
            EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,12,"开启Z检测");
		default: return;
	}
}