//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

function EditorGui::buildMenus(%this)
{
   if(isObject(%this.menuBar))
      return;
   
   // Sub menus (temporary, until MenuBuilder gets updated)
   %this.cameraSpeedMenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      class = "EditorCameraSpeedMenu";

      item[0] = "����" TAB "Ctrl-Shift 1";
      item[1] = "����" TAB "Ctrl-Shift 2";
      item[2] = "��" TAB "Ctrl-Shift 3";
      item[3] = "��" TAB "Ctrl-Shift 4";
      item[4] = "��" TAB "Ctrl-Shift 5";
      item[5] = "�ܿ�" TAB "Ctrl-Shift 6";
      item[6] = "���" TAB "Ctrl-Shift 7";
   };

   %this.renderFillMode = new PopupMenu()
   {
   		superClass = "MenuBuilder";
  	 	class = "EditorRenderFillMode";
  	 	item[0] = "��ģʽ";
  	 	item[1] = "�߿�ģʽ";
  	 	item[2] = "ʵ��ģʽ";
   };
   
   // Menu bar
   %this.menuBar = new MenuBar()
   {
      dynamicItemInsertPos = 3;
      
      // File Menu
      new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorFileMenu";

         barTitle = "�ļ�";
         
         item[0] = "�½�����" TAB "" TAB "EditorNewMission();";
         item[1] = "�򿪳��� ..." TAB "Ctrl O" TAB "EditorOpenMission();";
         item[2] = "���泡��" TAB "Ctrl S" TAB "EditorSaveMissionMenu();";
         item[3] = "�������Ϊ ..." TAB "" TAB "EditorSaveMissionAs();";
         item[4] = "-";
         item[5] = "����������� ..." TAB "" TAB "Heightfield::import();";
         item[6] = "����������� ..." TAB "" TAB "Texture::import();";
         item[7] = "-";
         item[8] = "������������ ..." TAB "" TAB "Heightfield::saveBitmap(\"\");";
         item[9] = "-";
         item[10] = "��/�ر༭����" TAB "F11" TAB "Editor.close(\"PlayGui\");";
         item[11] = "�˳�" TAB "" TAB "EditorQuitMission();";
      };
      
      // Edit Menu
      new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorEditMenu";

         barTitle = "�༭";
         
         item[0] = "����" TAB "Ctrl Z" TAB "EditorMenuEditUndo();";
         item[1] = "�ظ�" TAB "Ctrl Y" TAB "EditorMenuEditRedo();";
         item[2] = "-";
         item[3] = "����" TAB "Ctrl X" TAB "EditorMenuEditCut();";
         item[4] = "����" TAB "Ctrl C" TAB "EditorMenuEditCopy();";
         item[5] = "ճ��" TAB "Ctrl V" TAB "EditorMenuEditPaste();";
         item[6] = "-";
         //item[7] = "Select All" TAB "Ctrl A" TAB "EditorMenuEditSelectAll();";
         //item[8] = "Select None" TAB "Ctrl N" TAB "EditorMenuEditSelectNone();";
         //item[9] = "-";
         item[7] = "�����༭�趨 ..." TAB "" TAB "Canvas.pushDialog(WorldEditorSettingsDlg);";
         item[8] = "���α༭�趨 ..." TAB "" TAB "Canvas.pushDialog(TerrainEditorValuesSettingsGui, 99);";
      };
      
      // Camera Menu
      new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorCameraMenu";

         barTitle = "��ͷ";
         
         item[0] = "���������ѡ��" TAB "Alt Q" TAB "commandToServer('dropCameraAtPlayer');";
         item[1] = "��Ҹ��������" TAB "Alt W" TAB "commandToServer('DropPlayerAtCamera');";
         item[2] = "-";
         item[3] = "�л������" TAB "Alt C" TAB "commandToServer('ToggleCamera');";
         item[4] = "-";
         item[5] = "������ƶ��ٶ�" TAB %this.cameraSpeedMenu;
         item[6] = "��Ⱦ���ģʽ" TAB %this.renderFillMode;
      };
      
      // Window Menu
      new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorWindowMenu";
         internalName = "WindowMenu";

         barTitle = "����";
         
         item[0] = "�����༭��" TAB "F2";
         item[1] = "����Ѳ����" TAB "F3";
         item[2] = "����������" TAB "F4";
         item[3] = "��������༭��" TAB "F5";
         item[4] = "-";
         item[5] = "���α༭��" TAB "F6";
         item[6] = "���β��ʱ༭��" TAB "F7";
         item[7] = "�ر���������" TAB "F8";
         item[8] = "�ر�����޸���" TAB "F9";
         item[9] = "-";
         item[10] = "��������༭��" TAB "";
      };
   };
   
   // Menus that are added/removed dynamically (temporary)
   
   // ���ղ˵�
   if(!isObject(%this.LightingMenu))
   {
   	 %this.LightingMenu = new PopupMenu()
   	 {
   	 	 superClass = "MenuBuilder";
         class = "EditorLightingMenu";
         
         barTitle = "����";
         
         item[0] = "���ձ༭��" TAB "Alt G" TAB "lightEditor.toggle();";
         item[1] = "-";
         item[2] = "�������¹���" TAB "Alt F" TAB "lightEditor.filteredRelight();";
         item[3] = "ȫ�����¹���" TAB "Alt L" TAB "Editor.lightScene(\"\", forceAlways);";
   	 };
   } 
   %m++;
   //���Ӳ˵�
   if(!isObject(%this.ParticleMenu))
   {
   	 %this.ParticleMenu = new PopupMenu()
   	 {
   	 	   superClass = "MenuBuilder";
         class = "EditorParticleMenu";
         
         barTitle = "����";
         
         //item[0] = "���ӱ༭" TAB "" TAB "toggleParticleEditor(1);";
         item[0] = "���ӷ�����" TAB "" TAB "toggleParticleEditor(2);";
   	 };
   }
   %m++;
   // World Menu
   if(! isObject(%this.worldMenu))
   {
      %this.dropTypeMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorDropTypeMenu";

         // The onSelectItem() callback for this menu re-purposes the command field
         // as the MenuBuilder version is not used.
         item[0] = "���䵽ԭ��" TAB "" TAB "atOrigin";
         item[1] = "���䵽�����" TAB "" TAB "atCamera";
         item[2] = "���䵽����� w/Rot" TAB "" TAB "atCameraRot";
         item[3] = "���䵽���������" TAB "" TAB "belowCamera";
         item[4] = "���䵽��Ļ����" TAB "" TAB "screenCenter";
         item[5] = "���䵽��������" TAB "" TAB "atCentroid";
         item[6] = "���䵽����" TAB "" TAB "toGround";
      };
      
      %this.alignBoundsMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorAlignBoundsMenu";

         // The onSelectItem() callback for this menu re-purposes the command field
         // as the MenuBuilder version is not used.
         item[0] = "+X" TAB "" TAB "0";
         item[1] = "+Y" TAB "" TAB "1";
         item[2] = "+Z" TAB "" TAB "2";
         item[3] = "-X" TAB "" TAB "3";
         item[4] = "-Y" TAB "" TAB "4";
         item[5] = "-Z" TAB "" TAB "5";
      };
      
      %this.alignCenterMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorAlignCenterMenu";

         // The onSelectItem() callback for this menu re-purposes the command field
         // as the MenuBuilder version is not used.
         item[0] = "X ��" TAB "" TAB "0";
         item[1] = "Y ��" TAB "" TAB "1";
         item[2] = "Z ��" TAB "" TAB "2";
      };
      
      %this.worldMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorWorldMenu";

         barTitle = "����";
         
         item[0] = "����ѡ��" TAB "Ctrl L" TAB "EWorldEditor.lockSelection(true);";
         item[1] = "����ѡ��" TAB "Ctrl-Shift L" TAB "EWorldEditor.lockSelection(false);";
         item[2] = "-";
         item[3] = "����ѡ��" TAB "Ctrl H" TAB "EWorldEditor.hideSelection(true);";
         item[4] = "��ʾѡ��" TAB "Ctrl-Shift H" TAB "EWorldEditor.hideSelection(false);";
         item[5] = "-";
         item[6] = "���߽�����" TAB %this.alignBoundsMenu;
         item[7] = "����������" TAB %this.alignCenterMenu;
         item[8] = "-";
         item[9] = "������ת" TAB "" TAB "EWorldEditor.resetSelectedRotation();";
         item[10] = "��������" TAB "" TAB "EWorldEditor.resetSelectedScale();";
         item[11] = "-";
         item[12] = "ɾ��ѡ��" TAB "Delete" TAB "EWorldEditor.deleteSelection();EditorTree.deleteSelection();Inspector.uninspect();";
         item[13] = "��ͷ��ѡ��" TAB "Ctrl Q" TAB "EWorldEditor.dropCameraToSelection();";
         item[14] = "����ת��" TAB "Ctrl R" TAB "EWorldEditor.resetTransforms();";
         item[15] = "���ѡ�񵽵�ǰ��" TAB "" TAB "EWorldEditor.addSelectionToAddGroup();";
         item[16] = "����ѡ��" TAB "Ctrl D" TAB "EWorldEditor.dropSelection();";
         item[17] = "-";
         item[18] = "����λ��" TAB %this.dropTypeMenu;
      };
   }

   // Action Menu
   %m++;
   if(! isObject(%this.actionMenu))
   {
      %this.actionMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorActionMenu";

         barTitle = "����";
         
         item[0] = "ѡ��" TAB "" TAB "";
         item[1] = "���ѡ��" TAB "" TAB "";
         item[2] = "����ѡ��" TAB "" TAB "";
         item[3] = "���ѡ��" TAB "" TAB "";
         item[4] = "-";
         item[5] = "���Ӹ߶�" TAB "" TAB "";
         item[6] = "���͸߶�" TAB "" TAB "";
         item[7] = "�����߶�" TAB "Ctrl 1" TAB "";
         item[8] = "��Ϊƽ��" TAB "" TAB "";
         item[9] = "ƽ������" TAB "" TAB "";
         item[10] = "���ø߶�" TAB "" TAB "";
         item[11] = "-";
         item[12] = "�ڳ��ر�" TAB "" TAB "";
         item[13] = "���ر�" TAB "" TAB "";
         item[14] = "-";
         item[15] = "������" TAB "Ctrl 2" TAB "";
         item[16] = "�������" TAB "" TAB "";  
         item[17] = "-";
         item[18] = "������" TAB "" TAB "";
         item[19] = "�������" TAB "" TAB "";           
      };
   }

   // Brush Menu
   %m++;
   if(! isObject(%this.brushMenu))
   {
      %this.brushSizeMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorBrushSizeMenu";

         // The onSelectItem() callback for this menu re-purposes the command field
         // as the MenuBuilder version is not used.
         item[0] = "�ߴ� 1 x 1" TAB "Alt 1" TAB "1";
         item[1] = "�ߴ� 3 x 3" TAB "Alt 2" TAB "3";
         item[2] = "�ߴ� 5 x 5" TAB "Alt 3" TAB "5";
         item[3] = "�ߴ� 9 x 9" TAB "Alt 4" TAB "9";
         item[4] = "�ߴ� 15 x 15" TAB "Alt 5" TAB "15";
         item[5] = "�ߴ� 25 x 25" TAB "Alt 6" TAB "25";
      };
      
      %this.brushMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorBrushMenu";

         barTitle = "��ˢ";
         
         item[0] = "���α�ˢ" TAB "" TAB "ETerrainEditor.setBrushType(box);";
         item[1] = "Բ�α�ˢ" TAB "" TAB "ETerrainEditor.setBrushType(ellipse);";
         item[2] = "-";
         item[3] = "���ˢ" TAB "" TAB "ETerrainEditor.enableSoftBrushes = true;";
         item[4] = "Ӳ��ˢ" TAB "" TAB "ETerrainEditor.enableSoftBrushes = false;";
         item[5] = "-";
         item[6] = "��ˢ�ߴ�" TAB %this.brushSizeMenu;
      };
   }
   
   // ��������
   %m++;
   if(! isObject(%this.NaviGridMenu))
   {
      %this.NaviGridBrushSizeMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorNaviGridBrushSizeMenu";

         item[0] = "�ߴ� 1 x 1" TAB "Alt 1" TAB "1";
         item[1] = "�ߴ� 3 x 3" TAB "Alt 2" TAB "3";
         item[2] = "�ߴ� 5 x 5" TAB "Alt 3" TAB "5";
         item[3] = "�ߴ� 9 x 9" TAB "Alt 4" TAB "9";
         item[4] = "�ߴ� 15 x 15" TAB "Alt 5" TAB "15";
         item[5] = "�ߴ� 25 x 25" TAB "Alt 6" TAB "25";
      };

      %this.NaviGridMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorNaviGridMenu";
      
         barTitle = "��������";
         
         item[0] = "���Ƶ�������" TAB "Alt F1" TAB "";
         item[1] = "ɾ����������" TAB "Alt F2" TAB "";
         item[2] = "��Ƭɾ������" TAB "" TAB "";
         item[3] = "�����߶�" TAB "" TAB "";
         item[4] = "�и����" TAB "" TAB "";
         item[5] = "-";
         item[6] = "ȫ��ɾ������" TAB "" TAB "";
         item[7] = "�Զ���������" TAB "" TAB "";
         item[8] = "�Ż�������" TAB "" TAB "";
         item[9] = "-";
         item[10] = "Ѱ������" TAB "" TAB "";
         item[11] = "��ʾ�Ż�����" TAB "" TAB "";
         item[12] = "����Z���" TAB "" TAB "";
         item[13] = "-";
         item[14] = "��ˢ�ߴ�" TAB %this.brushSizeMenu;

      };
   }
}

//////////////////////////////////////////////////////////////////////////

function EditorGui::attachMenus(%this)
{
   %this.menuBar.attachToCanvas(Canvas, 0);
}

function EditorGui::detachMenus(%this)
{
   %this.menuBar.removeFromCanvas();
}

function EditorGui::setMenuDefaultState(%this)
{  
   if(! isObject(%this.menuBar))
      return 0;
      
   for(%i = 0;%i < %this.menuBar.getCount();%i++)
   {
      %menu = %this.menuBar.getObject(%i);
      %menu.setupDefaultState();
   }
   
   %this.worldMenu.setupDefaultState();
   %this.actionMenu.setupDefaultState();
   %this.brushMenu.setupDefaultState();
}

//////////////////////////////////////////////////////////////////////////

function EditorGui::findMenu(%this, %name)
{
   if(! isObject(%this.menuBar))
      return 0;
      
   for(%i = 0;%i < %this.menuBar.getCount();%i++)
   {
      %menu = %this.menuBar.getObject(%i);
      
      if(%name $= %menu.barTitle)
         return %menu;
   }
   
   return 0;
}
