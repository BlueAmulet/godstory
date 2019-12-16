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

      item[0] = "最慢" TAB "Ctrl-Shift 1";
      item[1] = "很慢" TAB "Ctrl-Shift 2";
      item[2] = "慢" TAB "Ctrl-Shift 3";
      item[3] = "中" TAB "Ctrl-Shift 4";
      item[4] = "快" TAB "Ctrl-Shift 5";
      item[5] = "很快" TAB "Ctrl-Shift 6";
      item[6] = "最快" TAB "Ctrl-Shift 7";
   };

   %this.renderFillMode = new PopupMenu()
   {
   		superClass = "MenuBuilder";
  	 	class = "EditorRenderFillMode";
  	 	item[0] = "点模式";
  	 	item[1] = "线框模式";
  	 	item[2] = "实体模式";
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

         barTitle = "文件";
         
         item[0] = "新建场景" TAB "" TAB "EditorNewMission();";
         item[1] = "打开场景 ..." TAB "Ctrl O" TAB "EditorOpenMission();";
         item[2] = "保存场景" TAB "Ctrl S" TAB "EditorSaveMissionMenu();";
         item[3] = "场景另存为 ..." TAB "" TAB "EditorSaveMissionAs();";
         item[4] = "-";
         item[5] = "导入地形数据 ..." TAB "" TAB "Heightfield::import();";
         item[6] = "导入材质数据 ..." TAB "" TAB "Texture::import();";
         item[7] = "-";
         item[8] = "导出地形数据 ..." TAB "" TAB "Heightfield::saveBitmap(\"\");";
         item[9] = "-";
         item[10] = "开/关编辑界面" TAB "F11" TAB "Editor.close(\"PlayGui\");";
         item[11] = "退出" TAB "" TAB "EditorQuitMission();";
      };
      
      // Edit Menu
      new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorEditMenu";

         barTitle = "编辑";
         
         item[0] = "撤销" TAB "Ctrl Z" TAB "EditorMenuEditUndo();";
         item[1] = "重复" TAB "Ctrl Y" TAB "EditorMenuEditRedo();";
         item[2] = "-";
         item[3] = "剪切" TAB "Ctrl X" TAB "EditorMenuEditCut();";
         item[4] = "复制" TAB "Ctrl C" TAB "EditorMenuEditCopy();";
         item[5] = "粘贴" TAB "Ctrl V" TAB "EditorMenuEditPaste();";
         item[6] = "-";
         //item[7] = "Select All" TAB "Ctrl A" TAB "EditorMenuEditSelectAll();";
         //item[8] = "Select None" TAB "Ctrl N" TAB "EditorMenuEditSelectNone();";
         //item[9] = "-";
         item[7] = "场景编辑设定 ..." TAB "" TAB "Canvas.pushDialog(WorldEditorSettingsDlg);";
         item[8] = "地形编辑设定 ..." TAB "" TAB "Canvas.pushDialog(TerrainEditorValuesSettingsGui, 99);";
      };
      
      // Camera Menu
      new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorCameraMenu";

         barTitle = "镜头";
         
         item[0] = "摄像机跟随选择" TAB "Alt Q" TAB "commandToServer('dropCameraAtPlayer');";
         item[1] = "玩家跟随摄像机" TAB "Alt W" TAB "commandToServer('DropPlayerAtCamera');";
         item[2] = "-";
         item[3] = "切换摄像机" TAB "Alt C" TAB "commandToServer('ToggleCamera');";
         item[4] = "-";
         item[5] = "摄像机移动速度" TAB %this.cameraSpeedMenu;
         item[6] = "渲染填充模式" TAB %this.renderFillMode;
      };
      
      // Window Menu
      new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorWindowMenu";
         internalName = "WindowMenu";

         barTitle = "窗口";
         
         item[0] = "场景编辑器" TAB "F2";
         item[1] = "场景巡视器" TAB "F3";
         item[2] = "场景创建器" TAB "F4";
         item[3] = "场景区域编辑器" TAB "F5";
         item[4] = "-";
         item[5] = "地形编辑器" TAB "F6";
         item[6] = "地形材质编辑器" TAB "F7";
         item[7] = "地表材质填充器" TAB "F8";
         item[8] = "地表材质修改器" TAB "F9";
         item[9] = "-";
         item[10] = "导航网格编辑器" TAB "";
      };
   };
   
   // Menus that are added/removed dynamically (temporary)
   
   // 光照菜单
   if(!isObject(%this.LightingMenu))
   {
   	 %this.LightingMenu = new PopupMenu()
   	 {
   	 	 superClass = "MenuBuilder";
         class = "EditorLightingMenu";
         
         barTitle = "光照";
         
         item[0] = "光照编辑器" TAB "Alt G" TAB "lightEditor.toggle();";
         item[1] = "-";
         item[2] = "过滤重新光照" TAB "Alt F" TAB "lightEditor.filteredRelight();";
         item[3] = "全部重新光照" TAB "Alt L" TAB "Editor.lightScene(\"\", forceAlways);";
   	 };
   } 
   %m++;
   //粒子菜单
   if(!isObject(%this.ParticleMenu))
   {
   	 %this.ParticleMenu = new PopupMenu()
   	 {
   	 	   superClass = "MenuBuilder";
         class = "EditorParticleMenu";
         
         barTitle = "粒子";
         
         //item[0] = "粒子编辑" TAB "" TAB "toggleParticleEditor(1);";
         item[0] = "粒子发射器" TAB "" TAB "toggleParticleEditor(2);";
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
         item[0] = "掉落到原点" TAB "" TAB "atOrigin";
         item[1] = "掉落到摄像机" TAB "" TAB "atCamera";
         item[2] = "掉落到摄像机 w/Rot" TAB "" TAB "atCameraRot";
         item[3] = "掉落到摄像机下面" TAB "" TAB "belowCamera";
         item[4] = "掉落到屏幕中心" TAB "" TAB "screenCenter";
         item[5] = "掉落到场景中心" TAB "" TAB "atCentroid";
         item[6] = "掉落到地面" TAB "" TAB "toGround";
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
         item[0] = "X 轴" TAB "" TAB "0";
         item[1] = "Y 轴" TAB "" TAB "1";
         item[2] = "Z 轴" TAB "" TAB "2";
      };
      
      %this.worldMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorWorldMenu";

         barTitle = "世界";
         
         item[0] = "锁定选择" TAB "Ctrl L" TAB "EWorldEditor.lockSelection(true);";
         item[1] = "解锁选择" TAB "Ctrl-Shift L" TAB "EWorldEditor.lockSelection(false);";
         item[2] = "-";
         item[3] = "隐藏选择" TAB "Ctrl H" TAB "EWorldEditor.hideSelection(true);";
         item[4] = "显示选择" TAB "Ctrl-Shift H" TAB "EWorldEditor.hideSelection(false);";
         item[5] = "-";
         item[6] = "按边界排列" TAB %this.alignBoundsMenu;
         item[7] = "按中心排列" TAB %this.alignCenterMenu;
         item[8] = "-";
         item[9] = "重设旋转" TAB "" TAB "EWorldEditor.resetSelectedRotation();";
         item[10] = "重设缩放" TAB "" TAB "EWorldEditor.resetSelectedScale();";
         item[11] = "-";
         item[12] = "删除选择" TAB "Delete" TAB "EWorldEditor.deleteSelection();EditorTree.deleteSelection();Inspector.uninspect();";
         item[13] = "镜头到选择" TAB "Ctrl Q" TAB "EWorldEditor.dropCameraToSelection();";
         item[14] = "重设转换" TAB "Ctrl R" TAB "EWorldEditor.resetTransforms();";
         item[15] = "添加选择到当前组" TAB "" TAB "EWorldEditor.addSelectionToAddGroup();";
         item[16] = "掉落选择" TAB "Ctrl D" TAB "EWorldEditor.dropSelection();";
         item[17] = "-";
         item[18] = "掉落位置" TAB %this.dropTypeMenu;
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

         barTitle = "操作";
         
         item[0] = "选择" TAB "" TAB "";
         item[1] = "清除选择" TAB "" TAB "";
         item[2] = "调整选择" TAB "" TAB "";
         item[3] = "清空选择" TAB "" TAB "";
         item[4] = "-";
         item[5] = "增加高度" TAB "" TAB "";
         item[6] = "降低高度" TAB "" TAB "";
         item[7] = "调整高度" TAB "Ctrl 1" TAB "";
         item[8] = "夷为平地" TAB "" TAB "";
         item[9] = "平滑地形" TAB "" TAB "";
         item[10] = "设置高度" TAB "" TAB "";
         item[11] = "-";
         item[12] = "挖除地表" TAB "" TAB "";
         item[13] = "填充地表" TAB "" TAB "";
         item[14] = "-";
         item[15] = "填充材质" TAB "Ctrl 2" TAB "";
         item[16] = "清除材质" TAB "" TAB "";  
         item[17] = "-";
         item[18] = "填充光照" TAB "" TAB "";
         item[19] = "清除光照" TAB "" TAB "";           
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
         item[0] = "尺寸 1 x 1" TAB "Alt 1" TAB "1";
         item[1] = "尺寸 3 x 3" TAB "Alt 2" TAB "3";
         item[2] = "尺寸 5 x 5" TAB "Alt 3" TAB "5";
         item[3] = "尺寸 9 x 9" TAB "Alt 4" TAB "9";
         item[4] = "尺寸 15 x 15" TAB "Alt 5" TAB "15";
         item[5] = "尺寸 25 x 25" TAB "Alt 6" TAB "25";
      };
      
      %this.brushMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorBrushMenu";

         barTitle = "笔刷";
         
         item[0] = "方形笔刷" TAB "" TAB "ETerrainEditor.setBrushType(box);";
         item[1] = "圆形笔刷" TAB "" TAB "ETerrainEditor.setBrushType(ellipse);";
         item[2] = "-";
         item[3] = "软笔刷" TAB "" TAB "ETerrainEditor.enableSoftBrushes = true;";
         item[4] = "硬笔刷" TAB "" TAB "ETerrainEditor.enableSoftBrushes = false;";
         item[5] = "-";
         item[6] = "笔刷尺寸" TAB %this.brushSizeMenu;
      };
   }
   
   // 导航网格
   %m++;
   if(! isObject(%this.NaviGridMenu))
   {
      %this.NaviGridBrushSizeMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorNaviGridBrushSizeMenu";

         item[0] = "尺寸 1 x 1" TAB "Alt 1" TAB "1";
         item[1] = "尺寸 3 x 3" TAB "Alt 2" TAB "3";
         item[2] = "尺寸 5 x 5" TAB "Alt 3" TAB "5";
         item[3] = "尺寸 9 x 9" TAB "Alt 4" TAB "9";
         item[4] = "尺寸 15 x 15" TAB "Alt 5" TAB "15";
         item[5] = "尺寸 25 x 25" TAB "Alt 6" TAB "25";
      };

      %this.NaviGridMenu = new PopupMenu()
      {
         superClass = "MenuBuilder";
         class = "EditorNaviGridMenu";
      
         barTitle = "导航网格";
         
         item[0] = "绘制导航网格" TAB "Alt F1" TAB "";
         item[1] = "删除导航网格" TAB "Alt F2" TAB "";
         item[2] = "整片删除网格" TAB "" TAB "";
         item[3] = "调整高度" TAB "" TAB "";
         item[4] = "切割公共边" TAB "" TAB "";
         item[5] = "-";
         item[6] = "全部删除网格" TAB "" TAB "";
         item[7] = "自动生成网格" TAB "" TAB "";
         item[8] = "优化并保存" TAB "" TAB "";
         item[9] = "-";
         item[10] = "寻径测试" TAB "" TAB "";
         item[11] = "显示优化网格" TAB "" TAB "";
         item[12] = "开启Z检测" TAB "" TAB "";
         item[13] = "-";
         item[14] = "笔刷尺寸" TAB %this.brushSizeMenu;

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
