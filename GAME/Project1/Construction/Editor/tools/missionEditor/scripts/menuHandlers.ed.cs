//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

$Pref::MissionEditor::FileSpec = "PowerEngine Mission Files (*.mis)|*.mis|All Files (*.*)|*.*|";

//////////////////////////////////////////////////////////////////////////
// File Menu Handlers
//////////////////////////////////////////////////////////////////////////

function EditorFileMenu::onMenuSelect(%this)
{
   %editingHeightfield = ETerrainEditor.isVisible() && EHeightField.isVisible();
   %this.enableItem(8, %editingHeightfield);
   %this.enableItem(2, ETerrainEditor.isDirty || ETerrainEditor.isMissionDirty || EWorldEditor.isDirty);
}

//////////////////////////////////////////////////////////////////////////

function EditorQuitMission()
{  
   if(ETerrainEditor.isMissionDirty || ETerrainEditor.isDirty || EWorldEditor.isDirty)
   {
      MessageBoxYesNo("Mission Modified", "Would you like to save your changes before quiting?", "EditorDoQuitMission(true);", "EditorDoQuitMission(false);");
   }
   else
      EditorDoQuitMission(false);
}

function EditorDoQuitMission(%saveFirst)
{
   if(%saveFirst)
   {
      EditorSaveMissionMenu();
      if (isObject( MainMenuGui ))
         Editor.close("MainMenuGui");
      else if (isObject( UnifiedMainMenuGui ))
         Editor.close("UnifiedMainMenuGui");
      disconnect();
   }
   else
   {
      if (isObject( MainMenuGui ))
         Editor.close("MainMenuGui");
      else if (isObject( UnifiedMainMenuGui ))
         Editor.close("UnifiedMainMenuGui");
      disconnect();
   }
   $CheckStartMission = false;
}

function EditorNewMission()
{
   %saveFirst = false;
   if(ETerrainEditor.isMissionDirty || ETerrainEditor.isDirty || EWorldEditor.isDirty)
   {
      error(knob);
      %saveFirst = MessageBox("Mission Modified", "Would you like to save changes to the current mission \"" @
         $Server::MissionFile @ "\" before creating a new mission?", "SaveDontSave", "Question") == $MROk;
   }
      
   if(%saveFirst)
      EditorSaveMission();

   // Clear dirty flags first to avoid duplicate dialog box from EditorOpenMission()
   EWorldEditor.isDirty = false;
   ETerrainEditor.isDirty = false;
   ETerrainEditor.isMissionDirty = false;
   
   EditorOpenMission("gameres/data/missions/newMission.mis");
   EWorldEditor.isDirty = true;
   ETerrainEditor.isDirty = true;
   EditorGui.saveAs = true;
}

function EditorSaveMissionMenu()
{
   if(EditorGui.saveAs)
      EditorSaveMissionAs();
   else
      EditorSaveMission();
}

function EditorSaveMission()
{
   // just save the mission without renaming it

   // first check for dirty and read-only files:
   if((EWorldEditor.isDirty || ETerrainEditor.isMissionDirty) && !isWriteableFileName($Server::MissionFile))
   {
      MessageBox("Error", "Mission file \""@ $Server::MissionFile @ "\" is read-only.", "Ok", "Stop");
      return false;
   }
  
   // now write the terrain and mission files out:
   if(ETerrainEditor.isDirty)
   {
      // Find all of the terrain files
      initContainerTypeSearch($TypeMasks::TerrainObjectType);

      while ((%terrainObject = containerSearchNext()) != 0)
      {
         if(EditorGui.saveAs)
            %terrainObject.saveAs($Server::MissionFile);
         else
            %terrainObject.save(%terrainObject.terrainFile);
      }
   }
   ETerrainEditor.SaveNavigationMesh($Server::MissionFile);
   if(EWorldEditor.isDirty || ETerrainEditor.isMissionDirty)
   {
      MissionGroup.save($Server::MissionFile);
      MissionGroup.GhostAlwaysSave($Server::MissionFile);
   }
    
   EWorldEditor.isDirty = false;
   ETerrainEditor.isDirty = false;
   ETerrainEditor.isMissionDirty = false;
   EditorGui.saveAs = false;

   // �ļ�ֻ������ʾ
   if(ETerrainEditor.isDirty)
   {
      // Find all of the terrain files
      initContainerTypeSearch($TypeMasks::TerrainObjectType);

      while ((%terrainObject = containerSearchNext()) != 0)
      {
         if (!isWriteableFileName(%terrainObject.terrainFile))
         {
            if (MessageBox("Error", "Terrain file \""@ %terrainObject.terrainFile @ "\" is read-only.", "Ok", "Stop") == $MROk)
               continue;
            else
               return false;
         }
      }
   }

   return true;
}

function EditorSaveMissionAs()
{
   %defaultFileName = $Server::MissionFile;
   if( %defaultFileName $= "" )
      %defaultFileName = expandFilename("~/data/missions/untitled.mis");

   %dlg = new SaveFileDialog()
   {
      Filters        = $Pref::MissionEditor::FileSpec;
      DefaultPath    = $Pref::MissionEditor::LastPath;
      DefaultFile    = %defaultFileName;
      ChangePath     = false;
      OverwritePrompt   = true;
   };
         
   %ret = %dlg.Execute();
   if(%ret)
   {
      $Pref::MissionEditor::LastPath = filePath( %dlg.FileName );
      %missionName = %dlg.FileName;
   }
   
   %dlg.delete();
   
   if(! %ret)
      return;
      
   ETerrainEditor.isDirty = true;
   EWorldEditor.isDirty = true;
   EditorGui.saveAs = true;
   %saveMissionFile = $Server::MissionFile;
   //%saveTerrName = ETerrainEditor.getActiveTerrain().terrainFile;

   $Server::MissionFile = %missionName;
   //ETerrainEditor.getActiveTerrain().terrainFile = filePath(%missionName) @ "/" @ fileBase(%missionName) @ ".ter";

   if(!EditorSaveMission())
   {
      $Server::MissionFile = %saveMissionFile;
      //ETerrainEditor.getActiveTerrain().terrainFile = %saveTerrName;
   }
}

function EditorOpenMission(%filename)
{
   if(ETerrainEditor.isMissionDirty || ETerrainEditor.isDirty || EWorldEditor.isDirty)
   {
      // "EditorSaveBeforeLoad();", "getLoadFilename(\"*.mis\", \"EditorDoLoadMission\");"
      if(MessageBox("�������޸�", "�Ƿ񽫸��ı��浽\"" @
         $Server::MissionFile @ "\" ��?", SaveDontSave, Question) == $MROk)
      {
         if(! EditorSaveMission())
            return;
      }
   }

//   if(%filename $= "")
//   {
//      %defaultFileName = $Server::MissionFile;
//      if( %defaultFileName $= "" )
//         %defaultFileName = expandFilename("~/data/missions/untitled.mis");
//
//      %dlg = new OpenFileDialog()
//      {
//         Filters        = $Pref::MissionEditor::FileSpec;
//         DefaultPath    = $Pref::MissionEditor::LastPath;
//         DefaultFile    = %defaultFileName;
//         ChangePath     = false;
//         MustExist      = true;
//      };
//            
//      %ret = %dlg.Execute();
//      if(%ret)
//      {
//         $Pref::MissionEditor::LastPath = filePath( %dlg.FileName );
//         %filename = %dlg.FileName;
//      }
//      
//      %dlg.delete();
//      
//      if(! %ret)
//         return;
//   }
//   
//   // close the current editor, it will get cleaned up by MissionCleanup
//   Editor.close();
//
//   loadMission( %filename, true ) ;
//
//   // recreate and open the editor
//   Editor::create();
//   MissionCleanup.add(Editor);
//   EditorGui.loadingMission = true;
//   Editor.open();
   //����Missionѡ���б�
   $CheckStartMission = false;
   $dblclickMissionFile = 0;
   EditorGui.add(FileSelectWindow_Ctrl);
   EditorGui.pushToBack(FileSelectWindow_Ctrl);  
   FileSelectWindow_Ctrl.SetVisible(true);
   MissionDlg_ButtonCtl1.SetText("ȡ��");
   MissionDlg_ButtonCtl1.Command = "FileSelectWindow_Ctrl.SetVisible(false);";
   MissionDlg_ButtonCtl2.SetText("��");
   MissionDlg_ButtonCtl2.Command = "if($CheckStartMission == false) openMissionFile();";
}
//���¼���ѡ��ĳ���
function openMissionFile()
{
  $CheckStartMission = true;
  $dblclickMissionFile = 1;
	%id = SM_missionList.getSelectedId();
	%filename = getField(SM_missionList.getRowTextById(%id), 1);
	if(%filename !$= "")
		{
			FileSelectWindow_Ctrl.SetVisible(false);
			// close the current editor, it will get cleaned up by MissionCleanup
      Editor.close();
   
      loadMission( %filename, true ) ;
   
      // recreate and open the editor
      Editor::create();
      MissionCleanup.add(Editor);
      EditorGui.loadingMission = true;
      Editor.open();
		}
}

//////////////////////////////////////////////////////////////////////////
// Edit Menu Handlers
//////////////////////////////////////////////////////////////////////////

function EditorEditMenu::onMenuSelect(%this)
{
   if(EWorldEditor.isVisible())
   {
      //%this.enableItem(7, true); // Select All
      %this.enableItem(5, EWorldEditor.canPasteSelection()); // Paste
      
      %canCutCopy = EWorldEditor.getSelectionSize() > 0;

      %this.enableItem(3, %canCutCopy); // Cut
      %this.enableItem(4, %canCutCopy); // Copy

   }
   else if(ETerrainEditor.isVisible())
   {
      %this.enableItem(3, false); // Cut
      %this.enableItem(4, false); // Copy
      %this.enableItem(5, false); // Paste
      //%this.enableItem(7, false); // Select All
      //%this.enableItem(8, false); // Select Nonde
   }
}

//////////////////////////////////////////////////////////////////////////

function EditorMenuEditUndo()
{
   if(EWorldEditor.isVisible())
      EWorldEditor.undo();
   else if(ETerrainEditor.isVisible())
      ETerrainEditor.undo();
}

function EditorMenuEditRedo()
{
   if(EWorldEditor.isVisible())
      EWorldEditor.redo();
   else if(ETerrainEditor.isVisible())
      ETerrainEditor.redo();
}

function EditorMenuEditCopy()
{
   if(! EWorldEditor.isVisible())
      return;
      
   EWorldEditor.copySelection();
}

function EditorMenuEditCut()
{
   if(! EWorldEditor.isVisible())
      return;
      
   EWorldEditor.copySelection();
   EWorldEditor.deleteSelection();
   //Inspector.uninspect();
}

function EditorMenuEditPaste()
{
   if(! EWorldEditor.isVisible())
      return;
   
   EWorldEditor.pasteSelection();
}

// Note: The original editor didn't implement these either, but leaving stubs since
// we will want to implement them at some point in the future.
function EditorMenuEditSelectAll()
{
}

function EditorMenuEditSelectNone()
{
}

//////////////////////////////////////////////////////////////////////////
// Window Menu Handler
//////////////////////////////////////////////////////////////////////////

function EditorWindowMenu::onSelectItem(%this, %id, %text)
{
   // The text passed in may have the accellerator in it, so this is safer
   EditorGui.setEditor(getField(%this.item[%id], 0));
   
   %this.checkRadioItem(0, 10, %id);
   switch(%id)
   {
     case 0:
       SceneEditirButton.setStateOn(true);
	     SceneToolbarTabCtrl.SetVisible(true);
	     TerrainToolbarTabCtrl.SetVisible(false);
	     NavigationToolbarCtrl.SetVisible(false);
	     SceneToolbarTabCtrl.selectPage(0);
	     EditorWindowMenu::onSelectItem(EditorGui.menuBar->WindowMenu,2,"����������");
     case 1:
       SceneEditirButton.setStateOn(true);
	     SceneToolbarTabCtrl.SetVisible(true);
	     TerrainToolbarTabCtrl.SetVisible(false);
	     NavigationToolbarCtrl.SetVisible(false); 
       SceneToolbarTabCtrl.selectPage(1);
     case 2:
       SceneEditirButton.setStateOn(true);
	     SceneToolbarTabCtrl.SetVisible(true);
	     TerrainToolbarTabCtrl.SetVisible(false);
	     NavigationToolbarCtrl.SetVisible(false); 
       SceneToolbarTabCtrl.selectPage(0);
     case 5:
       TerrainEditirButton.setStateOn(true);
	     SceneToolbarTabCtrl.SetVisible(false);
	     TerrainToolbarTabCtrl.SetVisible(true);
	     NavigationToolbarCtrl.SetVisible(false);
       TerrainToolbarTabCtrl.selectPage(0);
     case 6:   
       TerrainEditirButton.setStateOn(true);
	     SceneToolbarTabCtrl.SetVisible(false);
	     TerrainToolbarTabCtrl.SetVisible(true);
	     NavigationToolbarCtrl.SetVisible(false);
       TerrainToolbarTabCtrl.selectPage(1);
     case 7:
       TerrainEditirButton.setStateOn(true);
	     SceneToolbarTabCtrl.SetVisible(false);
	     TerrainToolbarTabCtrl.SetVisible(true);
	     NavigationToolbarCtrl.SetVisible(false);
       TerrainToolbarTabCtrl.selectPage(2);
     case 8:
       TerrainEditirButton.setStateOn(true);
	     SceneToolbarTabCtrl.SetVisible(false);
	     TerrainToolbarTabCtrl.SetVisible(true);
	     NavigationToolbarCtrl.SetVisible(false);
       TerrainToolbarTabCtrl.selectPage(0);
       EditorActionMenu::onSelectItem(EditorGui.actionMenu,15,"������"); 
       TerrainTabBut18.setStateOn(true);
     case 10:
       NavigationEditirButton.setStateOn(true);
	     SceneToolbarTabCtrl.SetVisible(false);
	     TerrainToolbarTabCtrl.SetVisible(false);
	     NavigationToolbarCtrl.SetVisible(true); 
     default: return;
   }
}

function EditorWindowMenu::setupDefaultState(%this)
{
   %this.onSelectItem(1, getField(%this.item[1], 0));
   SceneEditirEvent();
   Parent::setupDefaultState(%this);
}

//////////////////////////////////////////////////////////////////////////
// Camera Menu Handler
//////////////////////////////////////////////////////////////////////////

function EditorCameraSpeedMenu::onSelectItem(%this, %id, %text)
{
   // CodeReview - Seriously, comment your magic numbers... -JDD
   $Camera::movementSpeed = (%id / 6.0) * 195 + 5;
   
   %this.checkRadioItem(0, 6, %id);
}

function EditorCameraSpeedMenu::setupDefaultState(%this)
{
   %this.onSelectItem(3, getField(%this.item[3], 0));
   Parent::setupDefaultState(%this);
}

//////////////////////////////////////////////////////////////////////////
// Fill Mode
//////////////////////////////////////////////////////////////////////////

function EditorRenderFillMode::onSelectItem(%this, %id, %text)
{
	SetFillMode(%id+1);
	%this.checkRadioItem(0, 2, %id);
}

function EditorRenderFillMode::setupDefaultState(%this)
{
  %this.onSelectItem(2, getField(%this.item[2], 0));
  Parent::setupDefaultState(%this);
}


//////////////////////////////////////////////////////////////////////////
// World Menu Handler
//////////////////////////////////////////////////////////////////////////

function EditorWorldMenu::onMenuSelect(%this)
{
   %selSize = EWorldEditor.getSelectionSize();
   %lockCount = EWorldEditor.getSelectionLockCount();
   %hideCount = EWorldEditor.getSelectionHiddenCount();
   
   %this.enableItem(0, %lockCount < %selSize);  // Lock Selection
   %this.enableItem(1, %lockCount > 0);  // Unlock Selection
   
   %this.enableItem(3, %hideCount < %selSize);  // Hide Selection
   %this.enableItem(4, %hideCount > 0);  // Show Selection
   
   %this.enableItem(6, %selSize > 1);  // Align bounds
   %this.enableItem(7, %selSize > 1);  // Align center
   
   %this.enableItem(9, %selSize > 0);  // Reset Selected Rotation
   %this.enableItem(10, %selSize > 0);  // Reset Selected Scale

   %this.enableItem(15, %selSize > 0);  // Add to instant group
   %this.enableItem(13, %selSize > 0);  // Camera To Selection
   %this.enableItem(14, %selSize > 0 && %lockCount == 0);  // Reset Transforms
   %this.enableItem(16, %selSize > 0 && %lockCount == 0);  // Drop Selection
   %this.enableItem(12, %lockCount == 0);  // Delete Selection
   
}

//////////////////////////////////////////////////////////////////////////

function EditorDropTypeMenu::onSelectItem(%this, %id, %text)
{
   // CodeReview - What the heck does this do?  Comment it.
   EWorldEditor.dropType = getField(%this.item[%id], 2);
   
   %this.checkRadioItem(0, 6, %id);
}

function EditorDropTypeMenu::setupDefaultState(%this)
{
   // CodeReview - What the heck does this do?  Comment it.
   %this.onSelectItem(4, getField(%this.item[4], 0));
   Parent::setupDefaultState(%this);
}

//////////////////////////////////////////////////////////////////////////

function EditorAlignBoundsMenu::onSelectItem(%this, %id, %text)
{
   // Have the editor align all selected objects by the selected bounds.
   EWorldEditor.alignByBounds(getField(%this.item[%id], 2));
}

function EditorAlignBoundsMenu::setupDefaultState(%this)
{
   // Allow the parent to set the menu's default state
   Parent::setupDefaultState(%this);
}

//////////////////////////////////////////////////////////////////////////

function EditorAlignCenterMenu::onSelectItem(%this, %id, %text)
{
   // Have the editor align all selected objects by the selected axis.
   EWorldEditor.alignByAxis(getField(%this.item[%id], 2));
}

function EditorAlignCenterMenu::setupDefaultState(%this)
{
   // Allow the parent to set the menu's default state
   Parent::setupDefaultState(%this);
}

//////////////////////////////////////////////////////////////////////////
// Brush Menu Handler
//////////////////////////////////////////////////////////////////////////

function EditorBrushMenu::onSelectItem(%this, %id, %text)
{
   if(%id >= 0 && %id <= 1)
      %this.checkRadioItem(0, 1, %id);
      
   if(%id >= 3 && %id <= 4)
      %this.checkRadioItem(3, 4, %id);
   switch(%id)
   {
   	  case 0:
   	  	TerrainTabBut5.setStateOn(true);//���α�ˢ
   	  case 1:
   	  	TerrainTabBut6.setStateOn(true);//Բ�α�ˢ
   	  case 3:
   	  	TerrainTabBut7.setStateOn(true);//���ˢ
   	  case 4:
   	  	TerrainTabBut8.setStateOn(true);//Ӳ��ˢ
   	  default: return;
   }
   
   // Pass off to parent for handling of actual action
   Parent::onSelectItem(%this, %id, %text);
}

function EditorBrushMenu::setupDefaultState(%this)
{
   %this.onSelectItem(1, getField(%this.item[1], 0));
   %this.onSelectItem(3, getField(%this.item[3], 0));
   TerrainTabBut6.setStateOn(true);//Բ�α�ˢ
   TerrainTabBut7.setStateOn(true);//���ˢ
   Parent::setupDefaultState(%this);
}

function EditorBrushSizeMenu::onSelectItem(%this, %id, %text)
{
   %size = getField(%this.item[%id], 2);
   
   ETerrainEditor.brushSize = %size;
   ETerrainEditor.setBrushSize(%size, %size);
   
   %this.checkRadioItem(0, 5, %id);
}

function EditorBrushSizeMenu::setupDefaultState(%this)
{
   %this.onSelectItem(3, getField(%this.item[3], 0));
   Parent::setupDefaultState(%this);
}

//////////////////////////////////////////////////////////////////////////
// Action Menu Handler
//////////////////////////////////////////////////////////////////////////

// FIXME: Tidy this up
function EditorActionMenu::onSelectItem(%this, %id, %text)
{
	 EditorGui-->TerrainPainter.setVisible(false);
	 EditorGui-->TerraformEditor.setVisible(false);
	 EditorGui-->HeightfieldWindow.setVisible(false);
	 EditorGui-->TextureEditor.setVisible(false);
	 EditorGui-->TexPrevWindow.setVisible(false);
	 TerrainToolbarTabCtrl.selectPage(0); 
   %this.checkRadioItem(0, 16, %id);
   
   %item = getField(%this.item[%id], 0);
   
   switch$(%item)
   {
      case "ѡ��":
         ETerrainEditor.currentMode = "select";
         ETerrainEditor.selectionHidden = false;
         ETerrainEditor.renderVertexSelection = true;
         ETerrainEditor.setAction("select");
      case "���ѡ��":
         ETerrainEditor.currentMode = "deselect";
         ETerrainEditor.selectionHidden = false;
         ETerrainEditor.renderVertexSelection = true;
         ETerrainEditor.setAction("deselect");
      case "����ѡ��":
         ETerrainEditor.currentMode = "adjust";
         ETerrainEditor.selectionHidden = false;
         ETerrainEditor.setAction("adjustHeight");
         ETerrainEditor.currentAction = brushAdjustHeight;
         ETerrainEditor.renderVertexSelection = true;
      case "���ѡ��":
         ETerrainEditor.currentMode = "clear";
         ETerrainEditor.selectionHidden = true;
         ETerrainEditor.renderVertexSelection = true;
         ETerrainEditor.setAction("clear");
         %this.onSelectItem(0, getField(%this.item[0], 0));
      default:
         ETerrainEditor.currentMode = "paint";
         ETerrainEditor.selectionHidden = true;
         ETerrainEditor.setAction(ETerrainEditor.currentAction);
         switch$(%item)
         {
            case "���Ӹ߶�":
               ETerrainEditor.currentAction = raiseHeight;
               ETerrainEditor.renderVertexSelection = true;
               TerrainTabBut10.setStateOn(true);
            case "������":
               ETerrainEditor.currentAction = paintMaterial;
               ETerrainEditor.renderVertexSelection = true;
               TerrainTabBut18.setStateOn(true);
               EditorGui::setEditor(EditorGui,"�ر�����޸���");
            case "�������":
               ETerrainEditor.currentAction = clearMaterials;
               ETerrainEditor.renderVertexSelection = true;
               TerrainTabBut17.setStateOn(true);
            case "���͸߶�":
               ETerrainEditor.currentAction = lowerHeight;
               ETerrainEditor.renderVertexSelection = true;
               TerrainTabBut11.setStateOn(true);
            case "���ø߶�":
               ETerrainEditor.currentAction = setHeight;
               ETerrainEditor.renderVertexSelection = true;
               TerrainTabBut12.setStateOn(true);
            case "�����߶�":
               ETerrainEditor.currentAction = brushAdjustHeight;
               ETerrainEditor.renderVertexSelection = true;
               TerrainTabBut9.setStateOn(true);
            case "��Ϊƽ��":
               ETerrainEditor.currentAction = flattenHeight;
               ETerrainEditor.renderVertexSelection = true;
               TerrainTabBut13.setStateOn(true);
            case "ƽ������":
               ETerrainEditor.currentAction = smoothHeight;
               ETerrainEditor.renderVertexSelection = true;
               TerrainTabBut14.setStateOn(true);
            case "�ڳ��ر�":
               ETerrainEditor.currentAction = setEmpty;
               ETerrainEditor.renderVertexSelection = false;
               TerrainTabBut15.setStateOn(true);
            case "���ر�":
               ETerrainEditor.currentAction = clearEmpty;
               ETerrainEditor.renderVertexSelection = false;
               TerrainTabBut16.setStateOn(true);
            case "�������":
               ETerrainEditor.currentAction = clearShadowAction;
               ETerrainEditor.renderVertexSelection = false;
               TerrainTabBut19.setStateOn(true);
            case "������":
               ETerrainEditor.currentAction = shadowAction;
               ETerrainEditor.renderVertexSelection = false;
               TerrainTabBut20.setStateOn(true);
         }
         if(ETerrainEditor.currentMode $= "select")
            ETerrainEditor.processAction(ETerrainEditor.currentAction);
         else if(ETerrainEditor.currentMode $= "deselect")
            ETerrainEditor.processAction(ETerrainEditor.currentAction);
         else if(ETerrainEditor.currentMode $= "paint")
            ETerrainEditor.setAction(ETerrainEditor.currentAction);
         else if(ETerrainEditor.currentMode $= "clear")
            %this.onSelectItem(0, getField(%this.item[0], 0));
   }
}

function EditorActionMenu::setupDefaultState(%this)
{
   %this.onSelectItem(5, getField(%this.item[5], 0));
   TerrainTabBut9.setStateOn(true);//�����߶�
   Parent::setupDefaultState(%this);
}


//
// EditorNaviGridMenu
//

function EditorNaviGridBrushSizeMenu::onSelectItem(%this, %id, %text)
{
   %size = getField(%this.item[%id], 2);
   
   ETerrainEditor.brushSize = %size;
   ETerrainEditor.setBrushSize(%size, %size);
   
   %this.checkRadioItem(0, 5, %id);
}

function SetupEditorNaviGridMenu()
{
	 ETerrainEditor.currentAction = PaintNavigationGrid;
   ETerrainEditor.setAction(ETerrainEditor.currentAction);
   EditorGui.NaviGridMenu.checkRadioItem(0, 12, 0);
   NaviGridTabBut1.setStateOn(true);
}

function EditorNaviGridMenu::onSelectItem(%this, %id, %text)
{ 
   %item = getField(%this.item[%id], 0);
   switch$(%item)
   {
      case "ȫ��ɾ������":
         ETerrainEditor.DeleteNavigationMesh();
      case "�Զ���������":
         ETerrainEditor.GeneralNavigationMesh();
      case "�Ż�������":
         ETerrainEditor.OptimizeNavigationMesh();
         ETerrainEditor.SaveNavigationMesh($Server::MissionFile);
      case "��ʾ�Ż�����":
        ETerrainEditor.ShowOptimizeNavigationMesh();
      case "����Z���":
        ETerrainEditor.EnableNavigationZBuffer();    
      default:
         switch$(%item)
         {
            case "���Ƶ�������":
               ETerrainEditor.currentAction = PaintNavigationGrid;
               NaviGridTabBut1.setStateOn(true);
            case "ɾ����������":
               ETerrainEditor.currentAction = RemoveNavigationGrid;
               NaviGridTabBut2.setStateOn(true);
            case "��Ƭɾ������":
               ETerrainEditor.currentAction = RemoveTreeNavigationGrid;
               NaviGridTabBut3.setStateOn(true);
            case "�����߶�":
               ETerrainEditor.currentAction = AdjustNavigationGrid;
               NaviGridTabBut4.setStateOn(true);	
            case "�и����":
               ETerrainEditor.currentAction = SplitCellNavigationGrid;
               NaviGridTabBut6.setStateOn(true);	
            case "Ѱ������":
               ETerrainEditor.currentAction = TestPathAction;
               NaviGridTabBut5.setStateOn(true);
         }
         %this.checkRadioItem(0, 12, %id);
   }
   ETerrainEditor.setAction(ETerrainEditor.currentAction);
}
