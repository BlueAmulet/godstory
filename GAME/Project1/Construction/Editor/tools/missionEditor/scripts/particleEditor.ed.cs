//===========================================================================================================
// 文档说明:粒子编辑器
// 创建时间:2009-05-22
// 创建人: thinking
//=========================================================================================================== 
$ParticleEditorInitialized = false;
$ParticleEditor::isOpen    = false;

function toggleParticleEditor(%val)
{
	  if(PE_SaveDLG.isVisible())
	     Canvas.popDialog(PE_SaveDLG);
	  //1粒子,2发射器,3退出
	  if(%val != 1 && %val != 2 && %val != 3)
	  	return;
	  
	  if($ParticleEditor::isOpen)
	  {
	  	 if(%val == 1)
   	   {
   	   	  //PE_ParticleWindow.setVisible(true);
   	   	  //PE_EmitterWindow.setVisible(false);
   	      //ParticleEditor.openParticlePane();
   	   }
       else if(%val == 2)
   	   {
   	   	  //PE_ParticleWindow.setVisible(false);
   	   	  PE_EmitterWindow.setVisible(true);
   	   	  ParticleEditor.openEmitterPane();
   	   }
   	   else if(%val == 3)
   	   {
   	   	  //PE_ParticleWindow.setVisible(false);
          PE_EmitterWindow.setVisible(false);
   	   	  Canvas.popDialog(ParticleEditor);
          $ParticleEditor::isOpen = false;
          $ParticleEditorInitialized = false;
   	   }
        return;
	  }
    //初始化界面
	  if(!$ParticleEditorInitialized)
	  {
	  	 $ParticleEditorInitialized = true;
	  	 
       if(%val == 1)
   	   {
   	   	  //PE_ParticleWindow.setVisible(true);
   	   	  //PE_EmitterWindow.setVisible(false);
   	      //ParticleEditor.openParticlePane();
   	   }
       else if(%val == 2)
   	   {
   	   	  //PE_ParticleWindow.setVisible(false);
   	   	  PE_EmitterWindow.setVisible(true); 
   	   	  ParticleEditor.openEmitterPane();
   	   }
   	   
   	   Canvas.pushDialog(ParticleEditor);
       $ParticleEditor::isOpen = true;
	  }
} 

function PE_EmitterEditor::updateControls(%this)
{
	 %id = PEE_EmitterSelector.getSelected();
   %data = PEE_EmitterSelector.getTextById(%id);
   
   PEE_DisplayVelocity.setText(%data.DisplayVelocity);
   PEE_Sort.setValue(%data.Sort);
   
   PEE_EmitterParticleSelector.setText(getField(%data.particles, 0));

   $ParticleEditor::currEmitter = %data;
}

//function PE_ParticleEditor::updateControls(%this)
//{
//	 %id = PEP_ParticleSelector.getSelected();
//   %data = PEP_ParticleSelector.getTextById(%id);
//   
//   PEP_textureName.setText(                  %data.TextureName);
//   PEP_lifetimeMS.setText(                   %data.Lifetime);
//   PEP_gravity.setText(                      %data.Gravity);
//   PEP_gravityDeviation.setText(             %data.GravityDeviation);
//   PEP_gravityEffect.setText(                %data.GravityEffect);
//   PEP_emitTimeInterval.setText(             %data.EmitTimeInterval);
//   PEP_minPosDeviation.setText(              %data.MinPosDeviation);
//   PEP_maxPosDeviation.setText(              %data.MaxPosDeviation);
//   PEP_perEmitTime.setText(                  %data.PerEmitTime);
//   PEP_perEmitNum.setText(                   %data.PerEmitNum);
//   PEP_FlickerTime.setText(                  %data.FlickerTime);
//   PEP_FlickerInterval.setText(              %data.FlickerInterval);
//   PEP_rotationDirection.setText(            %data.ParticleRot);
//   PEP_textureSplinterStageNumLength.setText(%data.TextureSplinterStageNumLength);
//   PEP_textureSplinterStageNumWidth.setText( %data.TextureSplinterStageNumWidth);
//   PEP_texRemainTime.setText(                %data.TexRemainTime);
//   //混合状态
//   switch(%data.TextureStageState)
//   {
//   	case 1:
//   	    PEP_stageState1.setValue(true);
//   	case 2:
//   	    PEP_stageState2.setValue(true);
//   	case 3:
//   	    PEP_stageState3.setValue(true);
//   	case 4:
//   	    PEP_stageState4.setValue(true);
//   	case 5:
//   	    PEP_stageState5.setValue(true);
//   	case 6:
//   	    PEP_stageState6.setValue(true);
//   }
//   
//   //tab1标签内容
//   PEP_pag1_sizeNum.setText(%data.SizeNum);
//   PEP_pag1_sizeVal.setText(%data.vecSizePoint);
//   PEP_pag1_sizeTime.setText(%data.vecSizeTimePoint);
//   //tab2
//   PEP_pag2_VelocityNum.setText(%data.VelocityNum);
//   PEP_pag2_maxVelocity.setText(%data.vecMaxVelocity);
//   PEP_pag2_minVelocity.setText(%data.vecMinVelocity);
//   PEP_pag2_velocityTime.setText(%data.vecVelocityTimePoint);
//   //tab3
//   PEP_pag3_colorNum.setText(%data.ColorNum);
//   PEP_pag3_colorTime.setText(%data.vecColorTimePoint);
//   PEP_pag3_minColor.setText(%data.vecMinColorPoint);
//   PEP_pag3_maxColor.setText(%data.vecMaxColorPoint);
//   //tab4
//   PEP_pag4_alphaNum.setText(%data.AlphaNum);
//   PEP_pag4_alphaTime.setText(%data.vecAlphaTimePoint);
//   PEP_pag4_alphaPoint.setText(%data.vecAlphaPoint);
//   //tab5
//   PEP_pag5_angleNum.setText(%data.AngleNum);
//   PEP_pag5_anglePoint.setText(%data.vecAnglePoint);
//   PEP_pag5_angleTime.setText(%data.vecAngleTimePoint);
//   //tab6
//   PEP_pag6_parNum.setText(%data.ParticleNumDeviationNum);
//   PEP_pag6_parNumVal.setText(%data.vecParticleNumDeviation);
//   PEP_pag6_parNumTime.setText(%data.vecParticleNumDeviationTimePoint);
//   //tab7
//   PEP_pag7_rotNum.setText(%data.RotNum);
//   PEP_pag7_rotTime.setText(%data.vecRotTimePoint);
//   PEP_pag7_rotX.setText(%data.vecRotXValue);
//   PEP_pag7_rotY.setText(%data.vecRotYValue);
//   PEP_pag7_rotZ.setText(%data.vecRotZValue);
//   
//   PEP_billboard.setValue(%data.Billboard);
//   PEP_textureSplinter.setValue(%data.TextureSplinter);
//   PEP_texSplinterInitRandom.setValue(%data.TexSplinterInitRandom);
//   PEP_texSplinterBroadRandom.setValue(%data.TexSplinterBroadRandom);
//   PEP_Repeat.setValue(%data.Repeat);
//   
//   $ParticleEditor::currParticle =   %data;
//}

//function ParticleEditor::openParticlePane(%this)
//{
//	 PEP_ParticleSelector.clear();
//	 
//	 %count = DatablockGroup.getCount();
//   %particleCount = 0;
//   
//    for (%i = 0; %i < %count; %i++)
//   {
//      %obj = DatablockGroup.getObject(%i);
//      if (%obj.getClassName() $= "ParticleData")
//      {
//         PEP_ParticleSelector.add(%obj.getName(), %particleCount);
//          %particleCount++;
//      }
//    }
//    
//   PEP_ParticleSelector.sort();
//   PEP_ParticleSelector.setSelected(0);
//
//   PE_ParticleEditor.updateControls();
//   $ParticleEditor::activeEditor = PE_ParticleEditor;
//}

function ParticleEditor::openEmitterPane(%this)
{
	%count = DatablockGroup.getCount();
   %emitterCount = 0;
   %particleCount = 0;
   
   PEE_EmitterSelector.clear();
   PEE_EmitterParticleSelector.clear();
   
   for(%i = 0; %i < %count; %i++)
   {
   	  %obj = DatablockGroup.getObject(%i);
   	  if (%obj.getClassName() $= "ParticleEmitterData")
      {
         PEE_EmitterSelector.add(%obj.getName(), %emitterCount);
         %emitterCount++;
      }
      if (%obj.getClassName() $= "ParticleData")
      {
         PEE_EmitterParticleSelector.add(%obj.getName(), %particleCount);
         %particleCount++;
      }
   }

   PEE_EmitterSelector.sort();
   PEE_EmitterParticleSelector.sort();

   PEE_EmitterSelector.setSelected(0);
   
	 PE_EmitterEditor.updateControls();
   $ParticleEditor::activeEditor = PE_EmitterEditor;
}

//function PE_ParticleEditor::updateParticle(%this)
//{
//	 for(%i = 1; %i<8; %i++)
//	 {
//	 	  %bRet = ("PEP_pag" @ %i).execChildMethod("GuiNumericEditCtrl","checkNumberValid");
//	 	  if(!%bRet)
//	 	  {
//	 	  	 Canvas.popDialog(PE_SaveDLG);
//   		   MessageBoxOK("出错了","请认真检查数据！！！");
//   	     return;
//	 	  }
//	 }
//	 
//	 $ParticleEditor::currParticle.TextureName      = PEP_textureName.getValue();
//	 $ParticleEditor::currParticle.Lifetime         = PEP_lifetimeMS.getValue();
//	 $ParticleEditor::currParticle.Gravity          = PEP_gravity.getValue();
//	 $ParticleEditor::currParticle.GravityDeviation = PEP_gravityDeviation.getValue();
//	 $ParticleEditor::currParticle.GravityEffect    = PEP_gravityEffect.getValue();
//	 $ParticleEditor::currParticle.EmitTimeInterval = PEP_emitTimeInterval.getValue();
//	 $ParticleEditor::currParticle.MinPosDeviation  = PEP_minPosDeviation.getValue();
//	 $ParticleEditor::currParticle.MaxPosDeviation  = PEP_maxPosDeviation.getValue();
//	 $ParticleEditor::currParticle.PerEmitTime      = PEP_perEmitTime.getValue();
//	 $ParticleEditor::currParticle.PerEmitNum       = PEP_perEmitNum.getValue();
//	 $ParticleEditor::currParticle.FlickerTime      = PEP_FlickerTime.getValue();
//	 $ParticleEditor::currParticle.FlickerInterval  = PEP_FlickerInterval.getValue();
//	 $ParticleEditor::currParticle.ParticleRot      = PEP_rotationDirection.getValue();
//	 $ParticleEditor::currParticle.TextureSplinterStageNumLength = PEP_textureSplinterStageNumLength.getValue();
//	 $ParticleEditor::currParticle.TextureSplinterStageNumWidth  = PEP_textureSplinterStageNumWidth.getValue();
//	 $ParticleEditor::currParticle.TexRemainTime    = PEP_texRemainTime.getValue();
//	 //混合状态
//	 if(PEP_stageState1.getValue())
//	   $ParticleEditor::currParticle.TextureStageState = 1;
//	 else if(PEP_stageState2.getValue())
//	   $ParticleEditor::currParticle.TextureStageState = 2;
//	 else if(PEP_stageState3.getValue())
//	   $ParticleEditor::currParticle.TextureStageState = 3;
//	 else if(PEP_stageState4.getValue())
//	   $ParticleEditor::currParticle.TextureStageState = 4;
//	 else if(PEP_stageState5.getValue())
//	   $ParticleEditor::currParticle.TextureStageState = 5;
//	 else if(PEP_stageState5.getValue())
//	   $ParticleEditor::currParticle.TextureStageState = 6;
//	 
//	 //tab1标签内容
//	 $ParticleEditor::currParticle.SizeNum          = PEP_pag1_sizeNum.getValue();
//	 $ParticleEditor::currParticle.vecSizePoint     = PEP_pag1_sizeVal.getValue();
//	 $ParticleEditor::currParticle.vecSizeTimePoint = PEP_pag1_sizeTime.getValue();
//	 //tab2
//	 $ParticleEditor::currParticle.VelocityNum          = PEP_pag2_VelocityNum.getValue();
//	 $ParticleEditor::currParticle.vecMaxVelocity       = PEP_pag2_maxVelocity.getValue();
//	 $ParticleEditor::currParticle.vecMinVelocity       = PEP_pag2_minVelocity.getValue();
//	 $ParticleEditor::currParticle.vecVelocityTimePoint = PEP_pag2_velocityTime.getValue();
//	 //tab3
//	 $ParticleEditor::currParticle.ColorNum          = PEP_pag3_colorNum.getValue();
//	 $ParticleEditor::currParticle.vecColorTimePoint = PEP_pag3_colorTime.getValue();
//	 $ParticleEditor::currParticle.vecMinColorPoint  = PEP_pag3_minColor.getValue();
//	 $ParticleEditor::currParticle.vecMaxColorPoint  = PEP_pag3_maxColor.getValue();
//	 //tab4
//	 $ParticleEditor::currParticle.AlphaNum          = PEP_pag4_alphaNum.getValue();
//	 $ParticleEditor::currParticle.vecAlphaTimePoint = PEP_pag4_alphaTime.getValue();
//	 $ParticleEditor::currParticle.vecAlphaPoint     = PEP_pag4_alphaPoint.getValue();
//	 //tab5
//	 $ParticleEditor::currParticle.AngleNum          = PEP_pag5_angleNum.getValue();
//	 $ParticleEditor::currParticle.vecAnglePoint     = PEP_pag5_anglePoint.getValue();
//	 $ParticleEditor::currParticle.vecAngleTimePoint = PEP_pag5_angleTime.getValue();
//	 //tab6
//	 $ParticleEditor::currParticle.ParticleNumDeviationNum          = PEP_pag6_parNum.getValue();
//	 $ParticleEditor::currParticle.vecParticleNumDeviation          = PEP_pag6_parNumVal.getValue();
//	 $ParticleEditor::currParticle.vecParticleNumDeviationTimePoint = PEP_pag6_parNumTime.getValue();
//	 //tab7
//	 $ParticleEditor::currParticle.RotNum          = PEP_pag7_rotNum.getValue();
//	 $ParticleEditor::currParticle.vecRotTimePoint = PEP_pag7_rotTime.getValue();
//	 $ParticleEditor::currParticle.vecRotXValue    = PEP_pag7_rotX.getValue();
//	 $ParticleEditor::currParticle.vecRotYValue    = PEP_pag7_rotY.getValue();
//	 $ParticleEditor::currParticle.vecRotZValue    = PEP_pag7_rotZ.getValue();
//	 	
//	 $ParticleEditor::currParticle.Billboard              = PEP_billboard.getValue();
//	 $ParticleEditor::currParticle.TextureSplinter        = PEP_textureSplinter.getValue();
//	 $ParticleEditor::currParticle.TexSplinterInitRandom  = PEP_texSplinterInitRandom.getValue();
//	 $ParticleEditor::currParticle.TexSplinterBroadRandom = PEP_texSplinterBroadRandom.getValue();
//	 $ParticleEditor::currParticle.Repeat                 = PEP_Repeat.getValue();
//	 	
//	 $ParticleEditor::currParticle.reload();  
//}

function PE_EmitterEditor::updateEmitter(%this)
{
	 $ParticleEditor::currEmitter.DisplayVelocity = PEE_DisplayVelocity.getValue();
	 $ParticleEditor::currEmitter.Sort            = PEE_Sort.getValue();
	 	
	 $ParticleEditor::currEmitter.particles = PEE_EmitterParticleSelector.getText();
}

function PE_EmitterEditor::onNewEmitter(%this)
{
   ParticleEditor.updateEmitterNode();
   PE_EmitterEditor.updateControls();
}

function ParticleEditor::updateEmitterNode()
{
   $ParticleEditor::clientEmitterNode.setEmitterDataBlock(PEE_EmitterSelector.getText().getId());
}

//function PE_ParticleEditor::onNewParticle(%this)
//{
//   PE_ParticleEditor.updateControls();
//}

function PE_EmitterEditor::save(%this)
{
   //we'll save to a "particles" folder for now
   %mod = "gameres/data/DATABLOCKS/PARTICLES/Emitter";
   
   if (%mod $= "")
   {
      warn("Warning: No mod detected, saving in common.");
      %mod = "common";
   }
   	  
   %filename = %mod @ "/" @ $ParticleEditor::currEmitter @ ".cs";
   $ParticleEditor::currEmitter.save(%filename);

   //update file to define datablock...
   %file = new FileObject();
   %file.openForRead(%filename);
   %line = "";
   
   while(!%file.isEOF())
      %line = %line @ %file.readLine() @ "\r\n";
   %file.delete();
   
   %line = strreplace(%line, "//--- SERVER OBJECT WRITE BEGIN ---\r\nnew", "//--- SERVER OBJECT WRITE BEGIN ---\r\ndatablock");
   
   %file = new FileObject();
   %file.openForWrite(%filename);
   %file.writeLine(%line);
   %file.delete();
}

//function PE_ParticleEditor::save(%this)
//{
//   //we'll save to a "particles" folder for now
//   %mod = "gameres/data/DATABLOCKS/PARTICLES/Particle";
//   
//   if (%mod $= "")
//   {
//      warn("Warning: No mod detected, saving in common.");
//      %mod = "common";
//   }
//   
//   %filename = %mod @ "/" @ $ParticleEditor::currParticle @ ".cs";
//   $ParticleEditor::currParticle.save(%filename);
//   	
//   //update file to define datablock...
//   %file = new FileObject();
//   %file.openForRead(%filename);
//   %line = "";
//   
//   while(!%file.isEOF())
//      %line = %line @ %file.readLine() @ "\r\n";
//   %file.delete();
//   
//   %line = strreplace(%line, "//--- SERVER OBJECT WRITE BEGIN ---\r\nnew", "//--- SERVER OBJECT WRITE BEGIN ---\r\ndatablock");
//   
//   %file = new FileObject();
//   %file.openForWrite(%filename);
//   %file.writeLine(%line);
//   %file.delete();
//   //exec(%filename);
//}

function PE_EmitterEditor::cloneSave(%this)
{
   Canvas.pushDialog(PE_SaveDLG); 
   PE_DLGText.setText("发射器名称");   
   PE_DLGButton1.command = "PE_EmitterEditor.EmitterSave();"; 
   
}
function PE_EmitterEditor::EmitterSave(%this)
{
   //we'll save to a "particles" folder for now
   %mod = "gameres/data/DATABLOCKS/PARTICLES/Emitter";
   
   if (%mod $= "")
   {
      warn("Warning: No mod detected, saving in common.");
      %mod = "common";
   } 
   
   %name = PE_DLGTextEdit.getText();
   if(%name $= "")
   {
   	  Canvas.popDialog(PE_SaveDLG);
   	  return;  
   }
   
   if(%this.findObject(%name))
   	{
   		 Canvas.popDialog(PE_SaveDLG);
   		 MessageBoxOK("对象重名","对象已经存在,请另选名字");
   	   return;
   	}
   
   %filename = %mod @ "/" @ %name @ ".cs"; 
   $ParticleEditor::currEmitter.save(%filename);
   Canvas.popDialog(PE_SaveDLG);
   //update file to define datablock...
   %file = new FileObject();
   %file.openForRead(%filename);
   %line = "";
   
   while(!%file.isEOF())
      %line = %line @ %file.readLine() @ "\r\n";
   %file.delete();
   
   %line = strreplace(%line, "//--- SERVER OBJECT WRITE BEGIN ---\r\nnew", "//--- SERVER OBJECT WRITE BEGIN ---\r\ndatablock");
   %line = strreplace(%line, $ParticleEditor::currEmitter, %name);
   
   %file = new FileObject();
   %file.openForWrite(%filename);
   %file.writeLine(%line);
   %file.delete();
   
   exec(%filename);
   
   %count = DatablockGroup.getCount();
   %emitterCount = 0; 
   for(%i = 0; %i < %count; %i++)
   {
   	  %obj = DatablockGroup.getObject(%i);
   	  if (%obj.getClassName() $= "ParticleEmitterData")
         %emitterCount++;
   }
   
   PEE_EmitterSelector.add(%name,%emitterCount-1);
   PEE_EmitterSelector.setSelected(%emitterCount-1);
   PE_EmitterEditor.onNewEmitter(); 
}

//function PE_ParticleEditor::cloneSave(%this)
//{
//   Canvas.pushDialog(PE_SaveDLG);
//   PE_DLGText.setText("粒子名称");
//   PE_DLGButton1.command = "PE_ParticleEditor.ParticleSave();";
//}
//function PE_ParticleEditor::ParticleSave(%this)
//{
//   //we'll save to a "particles" folder for now
//   %mod = "gameres/data/DATABLOCKS/PARTICLES/Particle";
//   
//   if (%mod $= "")
//   {
//      warn("Warning: No mod detected, saving in common.");
//      %mod = "common";
//   }
//   
//   %name = PE_DLGTextEdit.getText();
//   
//   
//   if(%name $= "")
//   {
//   	  Canvas.popDialog(PE_SaveDLG);
//   	  return;  
//   }
//   
//    if(%this.findObject(%name))
//   	{
//   		 Canvas.popDialog(PE_SaveDLG);
//   		 MessageBoxOK("对象重名","对象已经存在,请另选名字");
//   	   return;
//   	}
//   
//   %filename = %mod @ "/" @ %name @ ".cs";
//   $ParticleEditor::currParticle.save(%filename);
//   Canvas.popDialog(PE_SaveDLG);
//   
//   //update file to define datablock...
//   %file = new FileObject();
//   %file.openForRead(%filename);
//   %line = "";
//   
//   while(!%file.isEOF())
//      %line = %line @ %file.readLine() @ "\r\n";
//   %file.delete();
//   
//   %line = strreplace(%line, "//--- SERVER OBJECT WRITE BEGIN ---\r\nnew", "//--- SERVER OBJECT WRITE BEGIN ---\r\ndatablock");
//   %line = strreplace(%line, $ParticleEditor::currParticle, %name);
//   
//   %file = new FileObject();
//   %file.openForWrite(%filename);
//   %file.writeLine(%line);
//   %file.delete();
//   
//   exec(%filename);
//   //当前选择项数据更新到控件
//   %count = DatablockGroup.getCount();
//   %particleCount = 0;
//    for (%i = 0; %i < %count; %i++)
//   {
//      %obj = DatablockGroup.getObject(%i);
//      if (%obj.getClassName() $= "ParticleData")
//          %particleCount++;
//    }
//    
//   PEP_ParticleSelector.add(%name,%particleCount-1);
//   PEP_ParticleSelector.setSelected(%particleCount-1);
//   PE_ParticleEditor.onNewParticle();
//}

function closeParticleEditor()
{
	if(PE_SaveDLG.isVisible())
	  Canvas.popDialog(PE_SaveDLG);
	   
	 //PE_ParticleWindow.setVisible(false);
   PE_EmitterWindow.setVisible(false);
	 Canvas.popDialog(ParticleEditor);
   $ParticleEditor::isOpen = false;
   $ParticleEditorInitialized = false;
}

//function PE_ParticleEditor::changeTexture()
//{
//	 %defaultFileName = PEP_textureName.getText();
//	 if( %defaultFilePath $= "" )
//     %defaultFilePath = "~/data/environments/particles";
//   
//   %dlg = new OpenFileDialog()
//   {
//   	  Filters        = "Image Files (*.png, *.jpg)|*.png;*.jpg|All Files (*.*)|*.*|";
//      DefaultPath    = %defaultFilePath;
//      DefaultFile    = %defaultFileName;
//      ChangePath     = false;
//      MustExist      = true;
//   };
//   
//   %ret = %dlg.Execute();
//   if(%ret)
//   {
//   	  %file = %dlg.FileName;
//   	  %file = %defaultFilePath @ "/" @ fileBase(%file);
//   }
//       
//   %dlg.delete();
//   
//   if(! %ret)
//      return;
//   
//   PEP_textureName.setText(%file);
//}