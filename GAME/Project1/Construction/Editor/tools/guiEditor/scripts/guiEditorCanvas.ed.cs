//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
function GuiEdit(%val)
{
   if (Canvas.isFullscreen())
   {
      MessageBoxOKOld("Windowed Mode Required", "Please switch to windowed mode to access the GUI Editor.");
      return;
   }

   if(%val != 0)
      return;
   
   EWToolbarWindow.setVisible(false);
   EWorldEditorToolbar.setVisible(false);
      
   if( !isObject( GuiEditCanvas ) )
      new GuiCanvas( GuiEditCanvas );

   GuiEditorOpen(Canvas.getContent());
}

function GuiEditCanvas::onAdd( %this )
{
   %this.setWindowTitle("PowerEngine Gui Editor");
         
}

function GuiEditCanvas::onRemove( %this )
{
   if( isObject( GuiEditorGui.menuGroup() ) )
      GuiEditorGui.delete();

   // cleanup
}

function GuiEditCanvas::onCreateMenu(%this)
{
   
   if(isObject(%this.menuBar))
      return;
   
   //set up %cmdctrl variable so that it matches OS standards
   %cmdCtrl = $platform $= "macos" ? "Cmd" : "Ctrl";
   
   // Menu bar
   %this.menuBar = new MenuBar()
   {
      dynamicItemInsertPos = 3;
      
      new PopupMenu()
      {
         superClass = "MenuBuilder";
         barTitle = "�ļ�";
         internalName = "FileMenu";
         
         item[0] = "�½�����..." TAB %cmdCtrl SPC "N" TAB %this @ ".create();";
         item[1] = "���ļ�..." TAB %cmdCtrl SPC "O" TAB %this @ ".open();";
         item[2] = "-";
         item[3] = "�ָ��ļ�" TAB %cmdCtrl SPC "R" TAB  %this @ ".revert();";
         item[4] = "�����ļ�..." TAB %cmdCtrl SPC "S" TAB %this @ ".save();";
         item[5] = "-";
         item[5] = "�رս���" TAB %cmdCtrl SPC "W" TAB %this @ ".close();";
         item[5] = "-";
         item[6] = "�˳�" TAB "F10" TAB %this @ ".quit();";
      };

      new PopupMenu()
      {
         superClass = "MenuBuilder";
         barTitle = "�༭";
         internalName = "EditMenu";
         
         item[0] = "����" TAB %cmdCtrl SPC "Z" TAB "GuiEditor.undo();";
         item[1] = "����" TAB %cmdCtrl @ "-shift Z" TAB "GuiEditor.redo();";
         item[2] = "-";
         item[3] = "����" TAB %cmdCtrl SPC "X" TAB "GuiEditor.saveSelection($Gui::clipboardFile); GuiEditor.deleteSelection();";
         item[4] = "����" TAB %cmdCtrl SPC "C" TAB "GuiEditor.saveSelection($Gui::clipboardFile);";
         item[5] = "ճ��" TAB %cmdCtrl SPC "V" TAB "GuiEditor.loadSelection($Gui::clipboardFile);";
         item[6] = "ȫѡ" TAB %cmdCtrl SPC "A" TAB "GuiEditor.selectAll();";
         item[7] = "-";
         item[8] = "����ѡ��" TAB %cmdCtrl SPC "," TAB "GuiEditor.showPrefsDialog();";
      };

      new PopupMenu()
      {
         superClass = "MenuBuilder";
         barTitle = "����";
         internalName = "LayoutMenu";
         
         item[0] = "�����" TAB %cmdCtrl SPC "L" TAB "GuiEditor.Justify(0);";
         item[1] = "�Ҷ���" TAB %cmdCtrl SPC "R" TAB "GuiEditor.Justify(2);";
         item[2] = "���˶���" TAB %cmdCtrl SPC "T" TAB "GuiEditor.Justify(3);";
         item[3] = "�ײ�����" TAB %cmdCtrl SPC "B" TAB "GuiEditor.Justify(4);";
         item[4] = "-";
         item[5] = "���ж���" TAB "" TAB "GuiEditor.Justify(1);";
         item[6] = "��ֱ����" TAB "" TAB "GuiEditor.Justify(5);";
         item[7] = "ˮƽ����" TAB "" TAB "GuiEditor.Justify(6);";
         item[8] = "-";
         item[9] = "��ǰ" TAB "" TAB "GuiEditor.BringToFront();";
         item[10] = "�ú�" TAB "" TAB "GuiEditor.PushToBack();";
         item[11] = "����ѡ��" TAB "" TAB "GuiEditorTreeView.lockSelection(true);";
         item[12] = "����ѡ��" TAB "" TAB "GuiEditorTreeView.lockSelection(false);";
      };
      
      new PopupMenu()
      {
         superClass = "MenuBuilder";
         barTitle = "�ƶ�";
         internalName = "MoveMenu";
            
         item[0] = "����" TAB "Left" TAB "GuiEditor.moveSelection(-1,0);";
         item[1] = "����" TAB "Right" TAB "GuiEditor.moveSelection(1,0);";
         item[2] = "����" TAB "Up" TAB "GuiEditor.moveSelection(0,-1);";
         item[3] = "����" TAB "Down" TAB "GuiEditor.moveSelection(0,1);";
         item[4] = "-";
         item[5] = "�������" TAB "Shift Left" TAB "GuiEditor.moveSelection(-$pref::guiEditor::snap2gridsize * 2,0);";
         item[6] = "�������" TAB "Shift Right" TAB "GuiEditor.moveSelection($pref::guiEditor::snap2gridsize * 2,0);";
         item[7] = "�������" TAB "Shift Up" TAB "GuiEditor.moveSelection(0,-$pref::guiEditor::snap2gridsize * 2);";
         item[8] = "�������" TAB "Shift Down" TAB "GuiEditor.moveSelection(0,$pref::guiEditor::snap2gridsize * 2);";
      };
   };
   %this.menuBar.attachToCanvas( %this, 0 );
}

// Called before onSleep when the canvas content is changed
function GuiEditCanvas::onDestroyMenu(%this)
{
   if( !isObject( %this.menuBar ) )
      return;

   // Destroy menus      
   while( %this.menuBar.getCount() != 0 )
      %this.menuBar.getObject(0).delete();
   
   if (isObject(%this.menubar))
   {
      %this.menuBar.removeFromCanvas( %this );
      %this.menuBar.delete();
   }
}

//
// Menu Operations
//
function GuiEditCanvas::create( %this )
{
   GuiEditorStartCreate();
}

function GuiEditCanvas::open( %this )
{
   %openFileName = GuiBuilder::getOpenName();
   if( %openFileName $= "" )
      return;   

   // Make sure the file is valid.
   if ((!isFile(%openFileName)) && (!isFile(%openFileName @ ".cmp")))
      return;
   
   // Load up the level.
   exec(%openFileName);
   
   // The level file should have contained a scenegraph, which should now be in the instant
   // group. And, it should be the only thing in the group.
   if (!isObject(%guiContent))
   {
      MessageBox("PowerEngine", "You have loaded a Gui file that was created before this version.  It has been loaded but you must open it manually from the content list dropdown", "Ok", "Information" );   
      return 0;
   }
  
   GuiEditorOpen(%guiContent);   
}
function GuiEditCanvas::save( %this )
{
   %currentObject = GuiEditorContent.getObject( 0 );
   
   if( %currentObject == -1 )
      return;
   
   if( %currentObject.getName() !$= "" )
      %name =  %currentObject.getName() @ ".gui";
   else
      %name = "Untitled.gui";
      
   %currentFile = %currentObject.getScriptFile();
   if( %currentFile $= "")
   {
      if( $Pref::GuiEditor::LastPath !$= "" )
         %currentFile = $Pref::GuiEditor::LastPath @ %name;
      else
         %currentFile = expandFileName( %name );
   }
   else
      %currentFile = expandFileName(%currentFile);
   // get the filename
   %filename = GuiBuilder::getSaveName(%currentFile);
   
   if(%filename $= "")
      return;
      
   // Save the Gui
   if( isWriteableFileName( %filename ) )
   {
      //
      // Extract any existent PowerEngineScript before writing out to disk
      //
      %fileObject = new FileObject();
      %fileObject.openForRead( %filename );      
      %skipLines = true;
      %beforeObject = true;
      // %var++ does not post-increment %var, in PowerEnginescript, it pre-increments it,
      // because ++%var is illegal. 
      %lines = -1;
      %beforeLines = -1;
      %skipLines = false;
      while( !%fileObject.isEOF() )
      {
         %line = %fileObject.readLine();
         if( %line $= "//--- OBJECT WRITE BEGIN ---" )
            %skipLines = true;
         else if( %line $= "//--- OBJECT WRITE END ---" )
         {
            %skipLines = false;
            %beforeObject = false;
         }
         else if( %skipLines == false )
         {
            if(%beforeObject)
               %beforeNewFileLines[ %beforeLines++ ] = %line;
            else
               %newFileLines[ %lines++ ] = %line;
         }
      }      
      %fileObject.close();
      %fileObject.delete();
     
      %fo = new FileObject();
      %fo.openForWrite(%filename);
      
      // Write out the captured PowerEngineScript that was before the object before the object
      for( %i = 0; %i < %beforeLines; %i++)
         %fo.writeLine( %beforeNewFileLines[ %i ] );
         
      %fo.writeLine("//--- OBJECT WRITE BEGIN ---");
      %fo.writeObject(%currentObject, "%guiContent = ");
      %fo.writeLine("//--- OBJECT WRITE END ---");
      
      // Write out captured PowerEngineScript below Gui object
      for( %i = 1; %i <= %lines; %i++ )
         %fo.writeLine( %newFileLines[ %i ] );
               
      %fo.close();
      %fo.delete();
      
   }
   else
      MessageBox("PowerEngine", "There was an error writing to file '" @ %currentFile @ "'. The file may be read-only.", "Ok", "Error" );   
}
function GuiEditCanvas::revert( %this )
{
}
function GuiEditCanvas::close( %this )
{
}

function GuiEditCanvas::onWindowClose(%this)
{
   %this.quit();
}

function GuiEditCanvas::quit( %this )
{
   %this.close();
   GuiGroup.add(GuiEditorGui);
   // we must not delete a window while in its event handler, or we foul the event dispatch mechanism
   %this.schedule(10, delete);
   
   Canvas.setContent(GuiEditor.lastContent);
   EWToolbarWindow.setVisible(true);
   EWorldEditorToolbar.setVisible(true);
}