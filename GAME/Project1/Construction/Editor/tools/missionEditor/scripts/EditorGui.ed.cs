//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

function EditorGui::init(%this)
{
   %this.getPrefs();

   if(!isObject(ETerraformer))
      new Terraformer(ETerraformer);

   $SelectedOperation = -1;
   $NextOperationId   = 1;
   $HeightfieldDirtyRow = -1;

   %this.buildMenus();

   if( !isObject( %this-->EWToolbarWindow ) )
   {
      // 加载脚本并创建工具栏界面
      exec("~/missionEditor/gui/guiEditorToolbar.ed.gui");
      if( isObject( %guiContent ) )
         %this.add( %guiContent->EWToolbarWindow );
   }
   // Build Creator tree
   if( !isObject( %this-->InspectorWindow ) )
   {
      // Load Creator/Inspector GUI
      exec("~/missionEditor/gui/guiWorldEditorMissionInspector.ed.gui");
      if( isObject( %guiContent ) )
         //%this.add( %guiContent->InspectorWindow );
         %this-->EWToolbarWindow.add(%guiContent->InspectorWindow);//整合到场景编辑器工具栏
   }
  
   if( !isObject( %this-->TerrainPainter ) )
   {
      // Load Terrain Painter GUI
      exec("~/missionEditor/gui/guiTerrainPainterContainer.ed.gui");
      if( isObject( %guiContent ) )
         //%this.add( %guiContent->TerrainPainter );
         %this-->EWToolbarWindow.add(%guiContent->TerrainPainter);
   }

   if( !isObject( %this-->TextureEditor ) )
   {
      // Load Terrain Texture Editor GUI
      exec("~/missionEditor/gui/guiTerrainTexEdContainer.ed.gui");
      if( isObject( %guiContent ) )
      {
      	 //%this.add( %guiContent->TextureEditor );
      	 %this-->EWToolbarWindow.add(%guiContent->TextureEditor );
         %this.add( %guiContent->TexPrevWindow );
      }  
   }

   if( !isObject( %this-->MissionAreaEditor ) )
   {
      // Load Mission Area Editor GUI
      exec("~/missionEditor/gui/guiMissionAreaEditorContainer.ed.gui");
      if( isObject( %guiContent ) )
         %this.add( %guiContent->MissionAreaEditor );      
   }

   if(!isObject(%this-->TerraformEditor))
   {
      exec("~/missionEditor/gui/guiTerrainEditorContainer.ed.gui");
      if(isObject (%guiContent))
      {   
         //%this.add(%guiContent->TerraformEditor);
         %this-->EWToolbarWindow.add(%guiContent->TerraformEditor);
         %this.add(%guiContent->HeightfieldWindow);
      }
   }

   EWorldEditor.init();
   ETerrainEditor.attachTerrain();
   TerraformerInit();
   TextureInit();



   //
   //Creator.init();
   EditorTree.init();
   ObjectBuilderGui.init();

   %this.setMenuDefaultState();

   EWorldEditor.isDirty = false;
   ETerrainEditor.isDirty = false;
   ETerrainEditor.isMissionDirty = false;
   EditorGui.saveAs = false;
   EditorSaveBnt.setActive(false);

   EWorldEditorDisplayPopup.clear();
   EWorldEditorDisplayPopup.add("Top" TAB "(XY)", 0);
   EWorldEditorDisplayPopup.add("Bottom" TAB "(XY)", 1);
   EWorldEditorDisplayPopup.add("Front" TAB "(XZ)", 2);
   EWorldEditorDisplayPopup.add("Back" TAB "(XZ)", 3);
   EWorldEditorDisplayPopup.add("Left" TAB "(YZ)", 4);
   EWorldEditorDisplayPopup.add("Right" TAB "(YZ)", 5);
   EWorldEditorDisplayPopup.add("Perspective", 6);
   EWorldEditorDisplayPopup.add("Isometric", 7);
   EWorldEditorDisplayPopup.setSelected(EWorldEditor.getDisplayType());

   //EWorldEditorInteriorPopup.clear();
   //EWorldEditorInteriorPopup.add("Interior Outline", 1);
   //EWorldEditorInteriorPopup.add("Interior Zones", 2);
   //EWorldEditorInteriorPopup.add("Interior Normal", 0);
   //EWorldEditorInteriorPopup.setSelected($MFDebugRenderMode);
}

//-----------------------------------------------------------------------------

function SetNaviGridBrush(%flag)
{
	if(%flag)
	{
		ETerrainEditor.setBrushType(NaviGrid);
		ETerrainEditor.enableSoftBrushes = flase;
	}
	else
	{
		ETerrainEditor.setBrushType(ellipse);
		ETerrainEditor.enableSoftBrushes = true;
	}
}

function EditorGui::setWorldEditorVisible(%this)
{
   EWorldEditor.setVisible(true);
   ETerrainEditor.setVisible(false);
   %this.menuBar.insert(%this.worldMenu, %this.menuBar.dynamicItemInsertPos);
   %this.menuBar.insert(%this.LightingMenu,%this.menuBar.dynamicItemInsertPos+1);
   %this.menuBar.insert(%this.ParticleMenu,%this.menuBar.dynamicItemInsertPos+2);
   
   %this.menuBar.remove(%this.actionMenu);
   %this.menuBar.remove(%this.brushMenu);
   %this.menuBar.remove(%this.NaviGridMenu);
   ETerrainEditor.showNavigationGrid = false;
   EWorldEditor.makeFirstResponder(true);
   EditorTree.open(MissionGroup,true);

   WorldEditorMap.push();
}

function EditorGui::setTerrainEditorVisible(%this)
{
   EWorldEditor.setVisible(false);
   ETerrainEditor.setVisible(true);
   ETerrainEditor.attachTerrain();
   EHeightField.setVisible(false);
   %this.menuBar.remove(%this.worldMenu);
   %this.menuBar.remove(%this.LightingMenu);
   %this.menuBar.remove(%this.ParticleMenu);
   %this.menuBar.insert(%this.actionMenu, %this.menuBar.dynamicItemInsertPos);
   %this.menuBar.insert(%this.brushMenu, %this.menuBar.dynamicItemInsertPos + 1);
   %this.menuBar.remove(%this.NaviGridMenu);
   ETerrainEditor.showNavigationGrid = false;
   SetNaviGridBrush(false);
   ETerrainEditor.makeFirstResponder(true);

   WorldEditorMap.pop();
}

function EditorGui::setNaviGridEditorVisible(%this)
{
   EWorldEditor.setVisible(false);
   ETerrainEditor.setVisible(true);
   ETerrainEditor.attachTerrain();
   EHeightField.setVisible(false);
   %this.menuBar.remove(%this.LightingMenu);
   %this.menuBar.remove(%this.ParticleMenu);
   %this.menuBar.remove(%this.worldMenu);
   %this.menuBar.remove(%this.actionMenu);
   %this.menuBar.remove(%this.brushMenu);
   %this.menuBar.insert(%this.NaviGridMenu, %this.menuBar.dynamicItemInsertPos);
   ETerrainEditor.showNavigationGrid = true;
   SetNaviGridBrush(true);
   ETerrainEditor.makeFirstResponder(true);

   WorldEditorMap.pop();
}

function EditorGui::setEditor(%this, %editor)
{
   %this.currentEditor = %editor;
   EWStickyObjectButton.setVisible(false);
   error("-------------------------->"@%editor);
   switch$(%editor)
   {
      case "场景编辑器":
         EWMissionArea.setVisible(false);
         %this.setWorldEditorVisible();
         %this-->InspectorWindow.setVisible(false);
         %this-->TerrainPainter.setVisible(false);
         %this-->MissionAreaEditor.setVisible(false);
         %this-->TextureEditor.setVisible(false);
         %this-->TerraformEditor.setVisible(false);
         %this-->HeightfieldWindow.setVisible(false);
         %this-->TexPrevWindow.setVisible(false);          
      case "场景巡视器":
         EWMissionArea.setVisible(false);
         EWInspectorFrame.setVisible(true);
         EWInspectorFrame1.setVisible(false);
         EWCreatorPane.setVisible(false);
         EWTreePane.setVisible(true);
         EWCreatorInspectorPane.setVisible(true);
         %this.setWorldEditorVisible();  
         %this-->InspectorWindow.setVisible(true);
         %this-->TerrainPainter.setVisible(false);
         %this-->MissionAreaEditor.setVisible(false);
         %this-->TextureEditor.setVisible(false);  
         %this-->TerraformEditor.setVisible(false);
         %this-->HeightfieldWindow.setVisible(false);
         %this-->TexPrevWindow.setVisible(false);  
      case "场景创建器":
         EWMissionArea.setVisible(false);
         EWInspectorFrame1.setVisible(true);
         EWInspectorFrame.setVisible(false);
         EWCreatorPane.setVisible(true);
         EWTreePane.setVisible(false);
         EWCreatorInspectorPane.setVisible(false);
         
         EWStickyObjectButton.setVisible(true); 
         StickyObjectNormal.setStateOn(true);  //按下普通按钮状态
         EWorldEditor.StickyObjectAction = 0;  //设默认值
         %this.setWorldEditorVisible();
         %this-->InspectorWindow.setVisible(true);
         %this-->TerrainPainter.setVisible(false);
         %this-->MissionAreaEditor.setVisible(false);
         %this-->TextureEditor.setVisible(false);
         %this-->TerraformEditor.setVisible(false);  
         %this-->HeightfieldWindow.setVisible(false);
         %this-->TexPrevWindow.setVisible(false); 
      case "场景区域编辑器":
         EWMissionArea.setVisible(true);
         %this.setWorldEditorVisible();
         %this-->InspectorWindow.setVisible(false);
         %this-->TerrainPainter.setVisible(false);
         %this-->MissionAreaEditor.setVisible(true);
         %this-->TextureEditor.setVisible(false);    
         %this-->TerraformEditor.setVisible(false);
         %this-->HeightfieldWindow.setVisible(false);      
         %this-->TexPrevWindow.setVisible(false); 
      case "地形编辑器":
         %this.setTerrainEditorVisible();
         %this-->InspectorWindow.setVisible(false);
         //%this-->TerrainPainter.setVisible(false);
         %this-->MissionAreaEditor.setVisible(false);
         %this-->TextureEditor.setVisible(false); 
         %this-->TerraformEditor.setVisible(false);
         %this-->HeightfieldWindow.setVisible(false);  
         ETerrainEditor.setup();
         %this-->TexPrevWindow.setVisible(false);     
         
      case "地形材质编辑器":
         %this.setTerrainEditorVisible();
         EHeightField.setVisible(true);
         %this-->InspectorWindow.setVisible(false);
         //%this-->TerrainPainter.setVisible(false);
         %this-->MissionAreaEditor.setVisible(false);
         %this-->TextureEditor.setVisible(false);
         %this-->TerraformEditor.setVisible(true);
         %this-->HeightfieldWindow.setVisible(true);
         %this-->TexPrevWindow.setVisible(false);   
      
      case "地表材质填充器":
         %this.setTerrainEditorVisible();
         %this-->InspectorWindow.setVisible(false);
         //%this-->TerrainPainter.setVisible(false);
         %this-->MissionAreaEditor.setVisible(false);
         %this-->TextureEditor.setVisible(true);   
         %this-->TerraformEditor.setVisible(false);
         %this-->HeightfieldWindow.setVisible(false);       
         %this-->TexPrevWindow.setVisible(true); 
         
      case "地表材质修改器":
         %this.setTerrainEditorVisible();
         %this-->InspectorWindow.setVisible(false);
         %this-->TerrainPainter.setVisible(true);
         %this-->MissionAreaEditor.setVisible(false);
         %this-->TextureEditor.setVisible(false);  
         %this-->TerraformEditor.setVisible(false); 
         %this-->HeightfieldWindow.setVisible(false);                
         EPainter.setup();
         %this-->TexPrevWindow.setVisible(false); 
         
      case "导航网格编辑器":
         %this.setNaviGridEditorVisible();
         %this-->InspectorWindow.setVisible(false);
         %this-->TerrainPainter.setVisible(false);
         %this-->MissionAreaEditor.setVisible(false);
         %this-->TextureEditor.setVisible(false); 
         %this-->TerraformEditor.setVisible(false);
         %this-->HeightfieldWindow.setVisible(false);
         %this-->TexPrevWindow.setVisible(false);
         SetupEditorNaviGridMenu();
   }
}

//-----------------------------------------------------------------------------

function EditorGui::getHelpPage(%this)
{
   switch$(%this.currentEditor)
   {
      case "World Editor" or "World Editor Inspector" or "World Editor Creator":
         return "5. World Editor";
      case "Mission Area Editor":
         return "6. Mission Area Editor";
      case "Terrain Editor":
         return "7. Terrain Editor";
      case "Terrain Terraform Editor":
         return "8. Terrain Terraform Editor";
      case "Terrain Texture Editor":
         return "9. Terrain Texture Editor";
      case "Terrain Painter":
         return "10. Terrain Painter";
   }
}

//-----------------------------------------------------------------------------

function EditorGui::onWake(%this)
{
   MoveMap.push();
   EditorMap.push();
   %this.setEditor(%this.currentEditor);

   if (isObject(DemoEditorAlert) && DemoEditorAlert.helpTag<2)
      Canvas.pushDialog(DemoEditorAlert);

}

function EditorGui::onSleep(%this)
{
   %this.setPrefs();

   EditorMap.pop();
   MoveMap.pop();
   if(isObject($Server::CurrentScene))
      $Server::CurrentScene.open();
}

//-----------------------------------------------------------------------------

// Called when we have been set as the content and onWake has been called
function EditorGui::onSetContent(%this, %oldContent)
{
   %this.attachMenus();
}

// Called before onSleep when the canvas content is changed
function EditorGui::onUnsetContent(%this, %newContent)
{
   %this.detachMenus();
}

//------------------------------------------------------------------------------

function EditorGui::addCameraBookmark(%this, %name)
{
   %obj = new CameraBookmark() {
      datablock = CameraBookmarkMarker;
      name = %name;
   };

   // Place into correct group
   if(!isObject(CameraBookmarks))
   {
      %grp = new SimGroup(CameraBookmarks);
      $InstantGroup.add(%grp);
   }
   CameraBookmarks.add(%obj);

   %cam = LocalClientConnection.camera.getTransform();
   %obj.setTransform(%cam);
}

function EditorGui::removeCameraBookmark(%this, %name)
{
   if(!isObject(CameraBookmarks))
      return;

   %count = CameraBookmarks.getCount();
   for(%i=0; %i<%count; %i++)
   {
      %obj = CameraBookmarks.getObject(%i);
      if(%obj.name $= %name)
      {
         %obj.delete();
         return;
      }
   }
}

function EditorGui::removeCameraBookmarkIndex(%this, %index)
{
   if(!isObject(CameraBookmarks))
      return;

   if(%index < 0 || %index >= CameraBookmarks.getCount())
      return;

   %obj = CameraBookmarks.getObject(%index);
   %obj.delete();
}

function EditorGui::jumpToBookmark(%this, %name)
{
   if(!isObject(CameraBookmarks))
      return;

   %count = CameraBookmarks.getCount();
   for(%i=0; %i<%count; %i++)
   {
      %obj = CameraBookmarks.getObject(%i);
      if(%obj.name $= %name)
      {
         LocalClientConnection.camera.setTransform(%obj.getTransform());
         return;
      }
   }
}

function EditorGui::jumpToBookmarkIndex(%this, %index)
{
   if(!isObject(CameraBookmarks))
      return;

   if(%index < 0 || %index >= CameraBookmarks.getCount())
      return;

   %trans = CameraBookmarks.getObject(%index).getTransform();
   LocalClientConnection.camera.setTransform(%trans);
}

//-----------------------------------------------------------------------------

function WorldEditor::toggleSnapToGrid(%this)
{
   %this.snapToGrid = !(%this.snapToGrid);
}

//-----------------------------------------------------------------------------

function EWorldEditorDisplayPopup::onSelect(%this, %id, %text)
{
   EWorldEditor.setDisplayType(%id);
}

//-----------------------------------------------------------------------------

//function EWorldEditorInteriorPopup::onSelect(%this, %id, %text)
//{
//   $MFDebugRenderMode = %id;
//
//   switch(%id)
//   {
//      case 0:
//         // Back to normal
//         setInteriorRenderMode(0);
//
//      case 1:
//         // Outline mode, including fonts so no stats
//         setInteriorRenderMode(1);
//
//      case 2:
//         // Interior debug mode
//         setInteriorRenderMode(7);
//   }
//}
//-----------------------------------------------------------------------------

function EditorTree::onObjectDeleteCompleted(%this)
{
   EWorldEditor.copySelection();
   EWorldEditor.deleteSelection();
}

function EditorTree::onClearSelected(%this)
{
   WorldEditor.clearSelection();
}

function EditorTree::init(%this)
{
   //%this.open(MissionGroup);

   // context menu
   new GuiControl(ETContextPopupDlg)
   {
    profile = "GuiModelessDialogProfile";
      horizSizing = "width";
      vertSizing = "height";
      position = "0 0";
      extent = "640 480";
      minExtent = "8 8";
      visible = "1";
      setFirstResponder = "0";
      modal = "1";

      new GuiPopUpMenuCtrl(ETContextPopup)
      {
         profile = "GuiScrollProfile";
         position = "0 0";
         extent = "0 0";
         minExtent = "0 0";
         maxPopupHeight = "200";
         command = "canvas.popDialog(ETContextPopupDlg);";
      };
   };
   ETContextPopup.setVisible(false);
}

function EditorTree::onInspect(%this, %obj)
{
   Inspector.inspect(%obj);
   InspectorNameEdit.setValue(%obj.getName());
}

function EditorTree::onAddSelection(%this, %obj)
{
   EWorldEditor.selectObject(%obj);
}
function EditorTree::onRemoveSelection(%this, %obj)
{
   EWorldEditor.unselectObject(%obj);
}
function EditorTree::onSelect(%this, %obj)
{
   EWorldEditor.selectObject(%obj);
}

function EditorTree::onUnselect(%this, %obj)
{
   EWorldEditor.unselectObject(%obj);
}

function ETContextPopup::onSelect(%this, %index, %value)
{
   switch(%index)
   {
      case 0:
         EditorTree.contextObj.delete();
   }
}

//------------------------------------------------------------------------------

function Editor::open(%this)
{
   // prevent the mission editor from opening while the GuiEditor is open.
   if(Canvas.getContent() == GuiEditorGui.getId())
      return;

   Canvas.setContent(EditorGui);
   
   clickShowObjTextBnt();//默认隐字
}

function Editor::close(%this, %gui)
{
   Canvas.setContent(%gui);
   if(isObject(MessageHud))
      MessageHud.close();
}

$RelightCallback = "";

function EditorLightingComplete()
{
   $lightingMission = false;
   RelightStatus.visible = false;
   
   if ($RelightCallback !$= "")
   {
      eval($RelightCallback);
   }
   
   $RelightCallback = "";
}

function updateEditorLightingProgress()
{
   RelightProgress.setValue(($SceneLighting::lightingProgress));
   if ($lightingMission)
      $lightingProgressThread = schedule(1, 0, "updateEditorLightingProgress");
}

function Editor::lightScene(%this, %callback, %forceAlways)
{
   if ($lightingMission)
      return;
      
   $lightingMission = true;
   $RelightCallback = %callback;
   RelightStatus.visible = true;
   RelightProgress.setValue(0);
   Canvas.repaint();  
   lightScene("EditorLightingComplete", %forceAlways);
   updateEditorLightingProgress();
} 

function setShowObject()
{
	 if(UIShowObjText.IsStateOn())
	 {
	 	  EWorldEditor.RenderObjText = false;
	    EWorldEditor.RenderObjHandle= false;
	 }
	 else
	 {
	 	  EWorldEditor.RenderObjText = true;
	    EWorldEditor.RenderObjHandle= true;
	 }
}

function setRenderShadows()
{
	if(UIRenderShadows.IsStateOn())
	  $Editor::enableRenderShadows = true;
	else
	  $Editor::enableRenderShadows = false;
}

function setRenderBrightness()
{
	if(UIRenderBrightness.IsStateOn())
	  setenableBrightness(true);
	else
	  setenableBrightness(false);
}

function setRenderNpc()
{
	if(UIRenderNPC.IsStateOn())
	  $Editor::RenderNPC = false;
	else
	  $Editor::RenderNPC = true;
}

function setRenderStatic()
{
	if(UIRenderStatic.IsStateOn())
	  $Editor::RenderStatic = false;
	else
	  $Editor::RenderStatic = true;
}     


function clickBrightnessBnt()
{
	if(!UIRenderBrightness.IsStateOn())
	  UIRenderBrightness.setSTateOn(true);  
	else   
		UIRenderBrightness.setSTateOn(false);
		
  setRenderBrightness();
} 

function clickShowObjTextBnt()
{
	if(!UIShowObjText.IsStateOn())
	   UIShowObjText.setStateOn(true);
	   
	setShowObject();
}

function ShowEditorSaveBnt()
{
	EditorSaveBnt.setActive(true);
}
