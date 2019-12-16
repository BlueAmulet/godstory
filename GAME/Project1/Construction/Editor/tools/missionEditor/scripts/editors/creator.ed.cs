//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

function Creator::onWake(%this)
{
   Creator.init();
}

function Creator::init( %this )
{
   //%this.clear();
   $InstantGroup = "MissionGroup";

   // ---------- INTERIORS
//   %base = %this.insertItem( 0, "室内物件" );
//
//   // walk all the interiors and add them to the correct group
//   %interiorId = "";
//   %file = findFirstFile( "*.dif" );
//
//   while( %file !$= "" )
//   {
//      %file = makeRelativePath(%file, getMainDotCSDir());
//      
//      // Determine which group to put the file in
//      // and build the group hierarchy as we go
//      %split    = strreplace(%file, "/", " ");
//      %dirCount = getWordCount(%split)-1;
//      %parentId = %base;
//
//      for(%i=0; %i<%dirCount; %i++)
//      {
//         %parent = getWords(%split, 0, %i);
//         if( %parent $= "" )
//            continue;
//
//         // if the group doesn't exist create it
//         if ( !%interiorId[%parent] )
//            %interiorId[%parent] = %this.insertItem( %parentId, getWord(%split, %i));
//         %parentId = %interiorId[%parent];
//      }
//
//      // Add the file to the group
//      %create = "createInterior(" @ "\"" @ %file @ "\"" @ ");";
//      %this.insertItem( %parentId, fileBase( %file ), %create,"Interior" );
//
//      %file = findNextFile( "*.dif" );
//   }


   // ---------- SHAPES - add in all the shapes now...
//   %base = %this.insertItem(0, "Shapes");
//   %dataGroup = "DataBlockGroup";
//
//   for(%i = 0; %i < %dataGroup.getCount(); %i++)
//   {
//      %obj = %dataGroup.getObject(%i);
//      // echo ("Obj: " @ %obj.getName() @ " - " @ %obj.category );
//      if(%obj.category !$= "" || %obj.category != 0)
//      {
//         %id = %this.findItemByName(%obj.category);
//           if (%id  == 0)
//           {
//            %grp = %this.insertItem(%base,%obj.category);
//            %this.insertItem(%grp, %obj.getName(), %obj.getClassName() @ "::create(" @ %obj.getName() @ ");","Item");
//         }
//         else
//         {
//            %this.insertItem(%id, %obj.getName(), %obj.getClassName() @ "::create(" @ %obj.getName() @ ");","Item");
//         }
//      }
//   }


   // ---------- Static Shapes
   %base = %this.insertItem( 0, "静态物件","","",0,0,0,"" );

   // walk all the statics and add them to the correct group
   %staticId = "";
   %path = "gameres/data/shapes/" @ "*.mod";     
   %file = findFirstFile( %path );
   while( %file !$= "" )
   {
      %file = makeRelativePath(%file, "gameres/data/shapes");
      
      // Determine which group to put the file in
      // and build the group hierarchy as we go
      %split    = strreplace(%file, "/", " ");
      %dirCount = getWordCount(%split)-1;
      %parentId = %base;

      for(%i=0; %i<%dirCount; %i++)
      {
         %parent = getWords(%split, 0, %i);
         if( %parent $= "" )
            continue;
         
         // if the group doesn't exist create it
         if ( !%staticId[%parent] )
            %staticId[%parent] = %this.insertItem( %parentId, getWord(%split, %i),"","",0,0,0,"");
         %parentId = %staticId[%parent];
      }
      // Add the file to the group
      %create = "TSStatic::create(\"" @ %file @ "\");";
      %this.insertItem( %parentId, fileBase( %file ), %create,"TSStatic" ,0,0,0,"");

      %file = findNextFile( %path );
   }


   // *** OBJECTS - do the objects now...
   %objGroup[0] = "Environment";
   %objGroup[1] = "Mission";
   %objGroup[2] = "System";

   %Environment_Item[0]  = "Water";
   %Environment_Item[1]  = "SFXEmitter";
   %Environment_Item[2]  = "Precipitation";
   %Environment_Item[3]  = "ParticleEmitter";
   %Environment_Item[4]  = "fxShapeReplicator";
   %Environment_Item[5]  = "fxFoliageReplicator";
   %Environment_Item[6] = "sgLightObject";
   %Environment_Item[7] = "sgDecalProjector";
   %Environment_Item[8] = "fxFireReplicator";
   %Environment_Item[9] = "VolumeFog";

   %Mission_Item[0] = "Path";
   %Mission_Item[1] = "PathMarker";
   %Mission_Item[2] = "Trigger";
   %Mission_Item[3] = "TSLogicMarkNode";

   %System_Item[0] = "SimGroup";


   //显示名称
   %objGroupName[0] = "环境";
   %objGroupName[1] = "关卡";
   %objGroupName[2] = "其它";

   %Environment_ItemName[0]  = "水";
   %Environment_ItemName[1]  = "声音";
   %Environment_ItemName[2]  = "天气";
   %Environment_ItemName[3]  = "粒子";
   %Environment_ItemName[4]  = "随机模型";
   %Environment_ItemName[5]  = "随机植物";
   %Environment_ItemName[6]  = "光源";
   %Environment_ItemName[7]  = "地表贴花";
   %Environment_ItemName[8]  = "随机火";
   %Environment_ItemName[9]  = "体积雾";
   
   %Mission_ItemName[0] = "路径";
   %Mission_ItemName[1] = "路点";
   %Mission_ItemName[2] = "触发器";
   %Mission_ItemName[3] = "逻辑标记点";

   %System_ItemName[0] = "自定义目录";

   // objects group
   %base = %this.insertItem(0, "系统","","",0,0,0,"");

   // create 'em
   for(%i = 0; %objGroup[%i] !$= ""; %i++)
   {
      %grp = %this.insertItem(%base, %objGroupName[%i],"","",0,0,0,"");

      %groupTag = "%" @ %objGroup[%i] @ "_Item";
      %groupTagName = "%" @ %objGroup[%i] @ "_ItemName";

      %done = false;
      for(%j = 0; !%done; %j++)
      {
         eval("%itemTag = " @ %groupTag @ %j @ ";");
      	 eval("%itemName = " @ %groupTagName @ %j @ ";");
      
         if(%itemTag $= "")
            %done = true;
         else
         {
            echo("itemTag:" @ %itemTag @ " j:" @ %j);
            %this.insertItem(%grp, %itemName, "ObjectBuilderGui.build" @ %itemTag @ "();",%itemTag,0,0,0,"");
         }
      }
   }
   
   //// ---------- MonsterObject shape
   //%base = %this.insertItem( 0, "怪物" );
   //%num  = GetMonsterNum();
   //for(%i = 0 ; %i < %num; %i++ )
   //{
   //		%mid = GetTMonsterID(%i);
   //		if( %mid !$= "" )
   //		{
   //			 %create = "MonsterObject::create(\"" @ %i @ "\");";
   //   	 %this.insertItem( %base, %mid, %create,"MonsterObject" );
   //		}
   //}

   // ---------- Npc
   %base = %this.insertItem( 0, "Npc","","",0,0,0,"" );
   
   // ---------- MonsterObject shape
   %base1 = %this.insertItem( 0, "怪物","","",0,0,0,"" );
   
   // 2:宝箱 3:材料 4:物件 5:可攻击物件 
   %base2 = %this.insertItem( 0, "怪物(其它)","","",0,0,0,"" );
   
   // 6:宠物 7:骑乘   
   %base3 = %this.insertItem( 0, "宠物-骑乘" ,"","",0,0,0,"");
   
   %num  = GetNpcNum();
   for(%i = 0 ; %i < %num; %i++ )
   {
      %mid = GetTNpcID(%i);
      if( %mid !$= "" )
      {
         %create = "NpcObject::create(\"" @ %i @ "\");";

         %name = GetNpcDataName(%mid);
         %SPACE = " ";
            
         if( %mid < 410000000 )
      	    %this.insertItem( %base, %mid @ %SPACE @ %name, %create,"NpcObject",0,0,0,"" );
      	 else if( %mid < 420000000 )
      	    %this.insertItem( %base1, %mid @ %SPACE @ %name, %create,"NpcObject",0,0,0,"" );
      	 else if(%mid < 460000000)
      	    %this.insertItem( %base2, %mid @ %SPACE @ %name, %create,"NpcObject",0,0,0,"" );
      	 else if(%mid < 480000000)
      	    %this.insertItem( %base3, %mid @ %SPACE @ %name, %create,"NpcObject",0,0,0,"" );
      }
   }

    // 触发类物件  
   %base4 = %this.insertItem( 0, "触发类物件" ,"","",0,0,0,"");
   %num = GetCollectionObjectNum();
   for(%i = 0 ; %i < %num; %i++ )
   {
      %mid = GetTCollectionObjectID(%i);
      if( %mid !$= "" )
      {
         %create = "CollectionObject::create(\"" @ %i @ "\");";

         %name = GetCollectionObjectDataName(%mid);
         %SPACE = " ";
      	 %this.insertItem( %base4, %mid @ %SPACE @ %name, %create,"CollectionObject",0,0,0,"" );
      }
   }
}

function createInterior(%name)
{
   %obj = new InteriorInstance()
   {
      position = "0 0 0";
      rotation = "0 0 0";
      interiorFile = %name;
   };

   return(%obj);
}

//function Creator::onAction(%this)
//{
//   %this.currentSel = -1;
//   %this.currentRoot = -1;
//   %this.currentObj = -1;

  // %sel = %this.getSelected();
  // if(%sel == -1 || %this.isGroup(%sel) || !$missionRunning)
  //    return;

   // the value is the callback function..
  // if(%this.getValue(%sel) $= "")
  //    return;

//   %this.currentSel = %sel;
//   %this.currentRoot = %this.getRootGroup(%sel);

  // %this.create(%sel);
//}

function Creator::onSelect(%this)
{
  // Creator.clearSelection();
}

function Creator::onInspect(%this,%obj)
{
   if(!$missionRunning)
      return;

   %objId = eval(%this.getItemValue(%obj));

   // drop it from the editor - only SceneObjects can be selected...
   //Creator.removeSelection(%obj);

   //EditorTree.clearSelection();
   EWorldEditor.clearSelection();
   EWorldEditor.selectObject(%objId);
   EWorldEditor.dropSelection();
}

function Creator::create(%this, %sel)
{
   // create the obj and add to the instant group
   %obj = eval(%this.getItemValue(%sel));

   if(%obj == -1)
      return;

//   %this.currentObj = %obj;

   $InstantGroup.add(%obj);

   // drop it from the editor - only SceneObjects can be selected...
   EWorldEditor.clearSelection();
   EWorldEditor.selectObject(%obj);
   EWorldEditor.dropSelection();
}

function MonsterObject::create(%index)
{
		%obj = new MonsterObject()
		{
			 dataBlockId  = GetTMonsterID(%index);
		};
		return(%obj);
}

function NpcObject::create(%index)
{
   %obj = new NpcObject()
   {
      dataBlockId  = GetTNpcID(%index);
   };
   return(%obj);
}

function CollectionObject::create(%index)
{
   %obj = new CollectionObject()
   {
      dataBlockId  = GetTCollectionObjectID(%index);
   };
   return(%obj);
}

//
function TSStatic::create(%shapeName)
{
   %obj = new TSStatic()
   {
      shapeName = "gameres/data/shapes/" @ %shapeName;
   };
   return(%obj);
}

function TSStatic::damage(%this)
{
   // prevent console error spam
}


//function Creator::getRootGroup(%sel)
//{
//   if(%sel == -1 || %sel == 0)
//      return(-1);
//
//   %parent = %this.getParent(%sel);
//   while(%parent != 0 || %parent != -1)
//   {
//      %sel = %parent;
//      %parent = %this.getParent(%sel);
//   }
//
//   return(%sel);
//}
//
//function Creator::getLastItem(%rootGroup)
//{
//   %traverse = %rootGroup + 1;
//   while(%this.getRootGroup(%traverse) == %rootGroup)
//      %traverse++;
//   return(%traverse - 1);
//}
//
//function Creator::createNext(%this)
//{
//   if(%this.currentSel == -1 || %this.currentRoot == -1 || %this.currentObj == -1)
//      return;
//
//   %sel = %this.currentSel;
//   %this.currentSel++;
//
//   while(%this.currentSel != %sel)
//   {
//      if(%this.getRootGroup(%this.currentSel) != %this.currentRoot)
//         %this.currentSel = %this.currentRoot + 1;
//
//      if(%this.isGroup(%this.currentSel))
//         %this.currentSel++;
//      else
//         %sel = %this.currentSel;
//   }
//
//   //
//   %this.currentObj.delete();
//   %this.create(%sel);
//}
//
//function Creator::createPrevious(%this)
//{
//   if(%this.currentSel == -1 || %this.currentGroup == -1 || %this.currentObj == -1)
//      return;
//
//   %sel = %this.currentSel;
//   %this.currentSel--;
//
//   while(%this.currentSel != %sel)
//   {
//      if(%this.getRootGroup(%this.currentSel) != %this.currentRoot)
//         %this.currentSel = getLastItem(%this.currentRoot);
//
//      if(%this.isGroup(%this.currentSel))
//         %this.currentSel--;
//      else
//         %sel = %this.currentSel;
//   }
//
//   //
//   %this.currentObj.delete();
//   %this.create(%sel);
//}
