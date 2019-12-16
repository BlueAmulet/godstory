//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
// �ĵ�˵��:�����༭��������
// ����ʱ��:2009-2-3
// ������: thinking
//-----------------------------------------------------------------------------
function SceneEditirEvent()
{
	SceneEditirButton.setStateOn(true);
	EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,0,"�����༭");
	SceneToolbarTabCtrl.SetVisible(true);
	TerrainToolbarTabCtrl.SetVisible(false);
	NavigationToolbarCtrl.SetVisible(false);
	SceneToolbarTabCtrl.selectPage(0);
	EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,2,"����������");   
}
function TerrainEditirEvent()
{
	EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,5,"���α༭��");
	SceneToolbarTabCtrl.SetVisible(false);
	TerrainToolbarTabCtrl.SetVisible(true);
	NavigationToolbarCtrl.SetVisible(false);
  TerrainToolbarTabCtrl.selectPage(0);
}
function NavigationEditirEvent()
{
	EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,10,"��������"); 
	SceneToolbarTabCtrl.SetVisible(false);
	TerrainToolbarTabCtrl.SetVisible(false);
	NavigationToolbarCtrl.SetVisible(true); 
}
//�ı��ˢ�ߴ��С
function AlterEditorBrushSize(%Size)
{
  //��ǰ��ˢ�ߴ��С
  %BrushSize = ETerrainEditor.brushSize;
  //��ˢ�ߴ�Ӵ�
  if(%Size>0 && %BrushSize<25)
  {
     switch(%BrushSize)
     {
        case 1:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,1,"�ߴ� 3 X 3");
        case 3:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,2,"�ߴ� 5 X 5");
        case 5:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,3,"�ߴ� 9 X 9");
        case 9:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,4,"�ߴ� 15 X 15");
        case 15:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,5,"�ߴ� 25 X 25");
        default: return;
     }
  }
  //��ˢ�ߴ���С
  if(%Size<0 && %BrushSize>1)
  {
     switch(%BrushSize)
     {
        case 25:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,4,"�ߴ� 15 X 15");
        case 15:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,3,"�ߴ� 9 X 9");
        case 9:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,2,"�ߴ� 5 X 5");
        case 5:
            EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,1,"�ߴ� 3 X 3");
        case 3:
           EditorBrushSizeMenu::onSelectItem(EditorGui.brushSizeMenu,0,"�ߴ� 1 X 1");
        default:
        return;
     }
  }
}
//��ˢ��״����ʽѡ��
function SelectEditorBrushStyle(%Style)
{
	if(%Style ==0)
		EditorBrushMenu::onSelectItem(EditorGui.brushMenu,0,"���α�ˢ");
	if(%Style ==1)    
		EditorBrushMenu::onSelectItem(EditorGui.brushMenu,1,"Բ�α�ˢ"); 
	if(%Style ==3)
		EditorBrushMenu::onSelectItem(EditorGui.brushMenu,3,"���ˢ"); 
	if(%Style ==4) 
		EditorBrushMenu::onSelectItem(EditorGui.brushMenu,4,"Ӳ��ˢ");
}
//���β�������
function TerrainEditirAction(%index)
{
	if(%index !=10)
		EditorGui-->TerrainPainter.setVisible(false);
	//%index��������е����
	switch(%index)
	{
		case 1:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,7,"�����߶�");
		case 2:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,5,"���Ӹ߶�");
		case 3:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,6,"���͸߶�");
		case 4:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,10,"�趨�߶�");
			SetTerrainHeight.SetVisible(!SetTerrainHeight.isVisible());
			if(SetTerrainHeight.isVisible())
				SetTerrainHeight.setText(ETerrainEditor.setHeightVal);
			else
				ETerrainEditor.setHeightVal =SetTerrainHeight.getText();
		case 5:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,8,"��Ϊƽ��");
		case 6:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,9,"ƽ������");
		case 7:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,12,"�ڳ��ر�");
		case 8:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,13,"���ر�");
		case 9:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,16,"�������");
		case 10:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,15,"������");
			EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,8,"������");
		case 11:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,19,"�������");
		case 12:
			EditorActionMenu::onSelectItem(EditorGui.actionMenu,18,"������");
		
			SetBrushColor.SetVisible(!SetBrushColor.isVisible());
			SetBrushColorButton.SetVisible(!SetBrushColorButton.isVisible());			
			
			if(SetBrushColor.isVisible())
				SetBrushColor.setText(ETerrainEditor.brushColor);
			else
				ETerrainEditor.brushColor = SetBrushColor.getText();
		default:return;
		
	}
}
//���������������
function NavigationAction(%index)
{
	//%index��������е����
	switch(%index)
	{
		case 1:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,0,"���Ƶ�������");
		case 2:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,1,"ɾ����������");
		case 3:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,2,"��Ƭɾ������");
		case 4:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,3,"�����߶�");
		case 5:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,10,"Ѱ������");
		case 6:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,6,"ɾ��ȫ������");
		case 7:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,7,"�Զ���������");
		case 8:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,8,"�Ż�����������");
		case 9:
			EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,4,"�и����");
        case 10:
            EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,11,"��ʾ�Ż�����");
        case 11:
            EditorNaviGridMenu::onSelectItem(EditorGui.NaviGridMenu,12,"����Z���");
		default: return;
	}
}