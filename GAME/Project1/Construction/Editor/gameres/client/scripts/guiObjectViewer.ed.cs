LoadDataBlocks("~/data/dataBlocks/particles/");

$LinkPoint_1 = 0;
$LinkPoint_2 = 0;
$LinkShapeName = 0;

ShapeTabBook.selectPage(0);
ShapeInfoTabBook.selectPage(0);

LinkPoint_2.add("linkpoint",0);
for(%i = 0; %i < 20; %i++)
{	
	if(%i<9)
	{
		LinkPoint_2.add("link0" @ %i+1 @ "point", %i+1);
	}
	else
	{
		LinkPoint_2.add("link" @ %i+1 @ "point", %i+1);
	}
}


function LineMode()
{
	EditorObjectView.setFillMode(1);
}

function PointMode()
{
	EditorObjectView.setFillMode(2);
}

function CommonMode()
{
	EditorObjectView.setFillMode(0);
}

function OpenBounds()
{
	if(BoundsButton.IsStateOn())
	{
		EditorObjectView.showBounds(1);
	}
	else
	{
		EditorObjectView.showBounds(0);
	}
}

function OpenCollision()
{
	if(CollisionButton.IsStateOn())
	{
		EditorObjectView.showCollection(1);
	}
	else
	{
		EditorObjectView.showCollection(0);
	}
}

function OpenLightWay()
{
	if(LightWayButton.IsStateOn())
	{
		EditorObjectView.showLightDir(1);
	}
	else
	{
		EditorObjectView.showLightDir(0);
	}
}

function RevertCamera()
{
	EditorObjectView.resetCamera();
}

function LinkCamera()
{
	EditorObjectView.LinkCamera();
}
//function addLinkNode(%linkName, %index)
//{
//	LinkPoint_1.add(%linkName, %index);
//}
function addLinkNode()
{
	 LinkPoint_1.clear();
	 %base = 0;
	 EditorObjectView.currentNodeSearch();
	 while(1)
	 {
	 	 %file = EditorObjectView.getNext(); 
      if(%file $= "")
      	break;
      LinkPoint_1.add(%file, %base);
      %base++;
	 }	
}
function OpenLinkPointInput()
{
	//EditorObjectView.showLinkNode($LinkShapeName);
	addLinkNode();
	LinkPoint_1.setText(LinkPoint_1.getTextById(0));
	LinkPoint_2.setText(LinkPoint_2.getTextById(0));
	LinkPointInputWnd.setVisible(1);
}

function LinkPointOK()
{
  $LinkPoint_1 = LinkPoint_1.getText();
  $LinkPoint_2 = LinkPoint_2.getText();
  LinkShape($LinkShapeName,$LinkPoint_1,$LinkPoint_2);
  LinkPointInputWnd.setVisible(0);
  LinkPoint_1.setText("");
  LinkPoint_2.setText("");
}

function LinkPointClose()
{
  LinkPointInputWnd.setVisible(0);
  LinkPoint_1.setText("");
  LinkPoint_2.setText("");
}

function PlayAction()
{
	if(PlayActionButton.IsStateOn())
	{
		EditorObjectView.playAnimation(1);
	}
	else
	{
		EditorObjectView.playAnimation(0);
	}
}

function ActionLoop()
{
	if(ActionLoopButton.IsStateOn())
	{
		EditorObjectView.loopAnimation(1);
	}
	else
	{
		EditorObjectView.loopAnimation(0);
	}
}

function SelectActionClose()
{
	SelectActionWnd.setVisible(0);
}

function RefLoadShapeTree()
{
	setModPaths("gameres");
	LoadShapeTree.clear();
	LoadShapeTree.init();
	ActionTree.clear();
	ActionTree.init();
}

function DelectShape()
{
	EditorObjectView.deleteCurrentShape();
	ClearShapeInfo();
	ShapePreTree.clear();
	CollisionObjectNum.setText("0");
	DetailNum.setText("0");
	MeshNum.setText("0");
	FNodeName.setText("");
	SelfNodeName.setText("");
	CollisionObjectPath.setText("");
	MAXPath.setText("");
	ActionPath.setText("");   
	OpenShapePreTree();
	ShowShapeInfo();
}

function ShowShapeInfo()
{
	%BitMapNum = EditorObjectView.getCurrentShapeTextureCount();
	for(%i = 0; %i < %BitMapNum; %i++)
	{
		BitMapList.addRow(%i,EditorObjectView.getCurrentShapeTextureName(%i));
	}
	BitMapList.sort(0);
	
	%ShapeNodeNum = EditorObjectView.getCurrentShapeNodeCount();
	for(%i = 0; %i < %ShapeNodeNum; %i++)
	{
		LinkPointList.addRow(%i,EditorObjectView.getCurrentShapeNodeName(%i));
	}
	LinkPointList.sort(0);
	
	
	CollisionObjectNum.setText(EditorObjectView.getCurrentShapeCollisionCount());

	DetailNum.setText(EditorObjectView.getCurrentShapeDetailCount());
	
	%DetailCount = EditorObjectView.getCurrentShapeDetailCount();
	for(%i = 0; %i < %DetailCount; %i++)
	{
		%MeshCount += EditorObjectView.getCurrentShapeMeshCount(i);
		MeshNum.setText(%MeshCount);
	}
	
	FNodeName.setText(EditorObjectView.getCurrentShapeMountNodeName());
	SelfNodeName.setText(EditorObjectView.getCurrentShapeParentNodeName());
	
	OpenDetailAndMeshTreee();
}

function ClearShapeInfo()
{
	BitMapList.clear();
	LinkPointList.clear();
	DetailAndMeshTree.clear();
}

function OpenLight()
{
	if(OpenLightButton.IsStateOn())
	{
		EditorObjectView.lightEnable(1);
		LightColor.setText(EditorObjectView.getLightColor());
	  AmbientColor.setText(EditorObjectView.getAmbientColor());
	  
	  %file = EditorObjectView.getLightColor(); 
    if(%file $= "")
      break;
	  %split    = strreplace(%file, "/", " ");
    for(%i = 0; %i < 3; %i++)
    {
    	if(%i $= 0)
    	{
    		Light_R.setText(getWord(%split, %i));
    	}
    	else if(%i $= 1)
    	{
    		Light_G.setText(getWord(%split, %i));
    	}
    	else
    	{
    		Light_B.setText(getWord(%split, %i));
    	}
    }
    
    %file1 = EditorObjectView.getAmbientColor(); 
    if(%file1 $= "")
      break;
	  %split1    = strreplace(%file1, "/", " ");
    for(%i = 0; %i < 3; %i++)
    {
    	if(%i $= 0)
    	{
    		Ambient_R.setText(getWord(%split1, %i));
    	}
    	else if(%i $= 1)
    	{
    		Ambient_G.setText(getWord(%split1, %i));
    	}
    	else
    	{
    		Ambient_B.setText(getWord(%split1, %i));
    	}
    }
	}
	else
	{
		EditorObjectView.lightEnable(0);
		LightColor.setText("光线颜色");
	  AmbientColor.setText("环境光颜色");
	}
}

function AppLight()
{
	EditorObjectView.setLightColor(Light_R.getText(),Light_G.getText(),Light_B.getText());
	EditorObjectView.setAmbientColor(Ambient_R.getText(),Ambient_G.getText(),Ambient_B.getText());
	setBgColor(Bg_R.getText(),Bg_G.getText(),Bg_B.getText());
	
}

function updateActionVolume(%index)
{
	EditorObjectView.setTimeScale(%index);
}

//-------------------------------------------------模型树----------------------------------------------------
function LoadShapeTree::onWake(%this)
{
   LoadShapeTree.init();
}

function LoadShapeTree::init( %this )
{
   //%this.clear();
   $InstantGroup = "ShapeGroup";
   
   %base = 0;

   // walk all the statics and add them to the correct group
   %staticId = "";
   %path = "gameres/" @ "*.mod";     
   %file = findFirstFile( %path );
   while( %file !$= "" )
   {
      %file = makeRelativePath(%file, "gameres");
      
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
      %create = "SelectTreeNode(\"" @ %file @ "\");";
      %this.insertItem( %parentId, fileBase( %file ), %create,"TSStatic",0,0,0,"" );

      %file = findNextFile( %path );
   } 
}

function LoadShapeTree::onInspect(%this,%obj)
{
   eval(%this.getItemValue(%obj));
}

function SelectTreeNode(%ShapeName)
{
	$LinkShapeName = "gameres/" @ %shapeName;
	if(LoadBaseShapeButton.IsStateOn())
	{
		EditorObjectView.loadBaseShape($LinkShapeName);
		ClearShapeInfo();  
	  ShapePreTree.clear();
	  OpenShapePreTree();
	  ShowShapeInfo();
	}
	else if(LinkShapeButton.IsStateOn())
	{
		OpenLinkPointInput();
		$LinkPoint_1 = 0;
		$LinkPoint_2 = 0;
	}
	CollisionObjectPath.setText($LinkShapeName);
	
	%CollisionPath = $LinkShapeName;
	%MaxPath = strreplace(%CollisionPath, ".mod", ".max");	
	%MaxFile = findFirstFile( %MaxPath );
	if(%MaxFile !$= "")
	{
		MAXPath.setText(%MaxPath);
	}
	else
	{
		MAXPath.setText("");
	}

}

function LinkShape(%Name,%Link_1,%Link_2)
{
	EditorObjectView.mountShapeToCurrentObject(%Name,%Link_1,%Link_2);
	ClearShapeInfo();  
	ShapePreTree.clear();
	OpenShapePreTree();
	ShowShapeInfo();
}
//------------------------------------------------------------------------------------------------------------

//-------------------------------------------------选择模型树----------------------------------------------------
function OpenShapePreTree()
{
	ShapePreTree.init();
}



function ShapePreTree::init( %this )
{
   //初始化数据
   ShapePreTree.clear();  
   DetailAndMeshTree.clear();
   %NodeText[0] = "";
   %NodeId[0] = 0;
   %index =0;
   
	 EditorObjectView.beginItemSearch();
   while(1)
   {
      //获取下一个文件
      %file = EditorObjectView.getNext(); 
      if(%file $= "")
      	break;

      %parentId = 0;      //初始化父结点为根结点	
      %dirCount = getWordCount(%file,"/") -1;
      //遍历树,找到对应树的父结点,否则为根结点
      for(%i=0; %i<%dirCount; %i++)
      {
         %parentText = getWords(%file, 0,%i, "/");
         if( %parentText $= "" )
            continue;
         
         if ( %NodeText[%i] $= %parentText)
            %parentId = %NodeId[%i];
      }
      //加树结点
      %create = "SelectShapePreTreeNode(\"" @ %file @ "\");";
	    %parentText = filename(%file);
	    %NodeText[%index] =  %file;
	    %NodeId[%index] = %this.insertItem( %parentId,  %parentText , %create,"TSStatic",0,0,0,"" );
	    %index++;
   }
}

function ShapePreTree::onSelect(%this,%obj)
{
   eval(%this.getItemValue(%obj));
}

function SelectShapePreTreeNode(%NodeName)
{
	EditorObjectView.selectCurrentShape(%NodeName); 
	
	ClearShapeInfo();
	ShowShapeInfo();
	%ActionPathFile = EditorObjectView.getCurrentActionFileName();
	if(%ActionPathFile $= "")
	{
		ActionPath.setText("");
	}
	else
	{
		ActionPath.setText(%ActionPathFile);
	}
	%NodeName = EditorObjectView.getCurrentShapeFileName();
	if(%NodeName $= "")
	{
		CollisionObjectPath.setText("");
	}
	else
	{
		CollisionObjectPath.setText(%NodeName);
	}
	
}
//------------------------------------------------------------------------------------------------------------

//------------------------------------------detail/Mesh对象的树-----------------------------------------------
function OpenDetailAndMeshTreee()
{
	DetailAndMeshTree.init();
}

function DetailAndMeshTree::init( %this )
{
	 DetailAndMeshTree.clear();

	 %base = 0;
   %staticId = "";

	 EditorObjectView.beginMeshSearch();
   while(1)
   {
      %file = EditorObjectView.getNext(); 
      if(%file $= "")
      	break;
      
      // Determine which group to put the file in
      // and build the group hierarchy as we go
      %split    = strreplace(%file, "/"," ");
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
	    //%create = "SelectShapePreTreeNode(\"" @ %file @ "\");";
	    %parent = filename(%file);
	    %staticId[%parent] = %this.insertItem( %parentId, filename(%file), %create,"TSStatic",0,0,0,"" );
	  }
}
//----------------------------------------------------------------------------------------------------

//-------------------------------------------------动画树---------------------------------------------
function ActionTree::onWake(%this)
{
   ActionTree.init();
}

function ActionTree::init( %this )
{
   //%this.clear();
   $InstantGroup = "ShapeGroup";
   
   //%base = %this.insertItem( 0, "模型文件" );

   // walk all the statics and add them to the correct group
   %staticId = "";
   %path = "gameres/" @ "*.ani";     
   %file = findFirstFile( %path );
   while( %file !$= "" )
   {
      %file = makeRelativePath(%file, "gameres");
      
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
      %create = "SelectActionNode(\"" @ %file @ "\");";
      %this.insertItem( %parentId, fileBase( %file ), %create,"TSStatic",0,0,0,"" );

      %file = findNextFile( %path );
   }  
}

function ActionTree::onSelect(%this,%obj)
{
   eval(%this.getItemValue(%obj));
}  

function SelectActionNode(%ActionName)
{
	%Name = "gameres/" @ %ActionName;
	EditorObjectView.loadCurrentAction(%Name);
	ActionPath.setText(%Name);
	echo(EditorObjectView.getTimeScale());
	ActionVolume.setValue(EditorObjectView.getTimeScale());
}

//----------------------------------------------------------------------------------------------

//---------------------------------------------BitMapList---------------------------------------
function LinkPointList::SelectIt()
{
	%name = LinkPointList.getRowTextById(LinkPointList.getSelectedId());
	EditorObjectView.selectNode(%name);
}
//----------------------------------------------------------------------------------------------

//-------------------------------------------------粒子树----------------------------------------------------
function RefLoadPartTree()
{
	setModPaths("gameres");
	LoadPartTree.clear();
	LoadPartTree.init();
}

function LoadPartTree::onWake(%this)
{
   LoadPartTree.init();
}

function LoadPartTree::init( %this )
{
   $InstantGroup = "ShapeGroup";
   
   %base = 0;

   // walk all the statics and add them to the correct group
   %staticId = "";
   %path = "gameres/data/datablocks/particles/" @ "*.cs";     
   %file = findFirstFile( %path );
   while( %file !$= "" )
   {
      %file = makeRelativePath(%file, "gameres");
      
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
      %create = "SelectPartNode(\"" @ %file @ "\");";
      %this.insertItem( %parentId, fileBase( %file ), %create,"TSStatic",0,0,0,"" );

      %file = findNextFile( %path );
   }  
}

function LoadPartTree::onInspect(%this,%obj)
{
   eval(%this.getItemValue(%obj));
}

function SelectPartNode(%PartName)
{
	$LinkPartName = "gameres/" @ %partName;
	if(LoadBasePartButton.IsStateOn())
	{
		EditorObjectView.loadBasePart($LinkPartName);
	}
	else if(LinkPartButton.IsStateOn())
	{
		ParticleOpenLinkPointInput();
		$LinkPoint_1 = 0;
	}
}

function SelectPartNode2(%PartName)
{
	$LinkPartName = "gameres/" @ %partName;
	if(LoadBasePartButton.IsStateOn())
	{
		EditorObjectView.loadBasePart($LinkPartName);
	}
	else if(LinkPartButton.IsStateOn())
	{
    EditorObjectView.mountPartToCurrentObject($LinkPartName);
	}
}

//-----------------------------------------------------------------------------------------------------------
//-------------------------------------粒子链接对话框

function ParticleAddLinkNode()
{
	 ParticleLinkPoint_1.clear();
	 %base = 0;
	 EditorObjectView.currentNodeSearch();
	 while(1)
	 {
	 	 %file = EditorObjectView.getNext(); 
      if(%file $= "")
      	break;
      ParticleLinkPoint_1.add(%file, %base);
      %base++;
	 }	
}

function ParticleOpenLinkPointInput()
{
	//EditorObjectView.showLinkNode($LinkShapeName);
	ParticleAddLinkNode();
	ParticleLinkPoint_1.setText(ParticleLinkPoint_1.getTextById(0));
	ParticleLinkPointInputWnd.setVisible(1);
}


function ParticleLinkPointOK()
{
  $LinkPoint_1 = ParticleLinkPoint_1.getText();
  ParticleLinkShape($LinkPartName,$LinkPoint_1,$LinkPoint_2);
  ParticleLinkPointInputWnd.setVisible(0);
  ParticleLinkPoint_1.setText("");
}

function ParticleLinkPointClose()
{
  ParticleLinkPointInputWnd.setVisible(0);
  ParticleLinkPoint_1.setText("");
}

function ParticleLinkShape(%Name,%Link_1,%Link_2)
{
	//EditorObjectView.mountShapeToCurrentObject(%Name,%Link_1,%Link_2);
	EditorObjectView.mountPartToCurrentObjectByLink(%Name, %Link_1);
	ClearShapeInfo();  
	ShapePreTree.clear();
	OpenShapePreTree();
	ShowShapeInfo();
}
//------------------------------------------------------------------------------------------------------------

//-------------------------------------来自 3DS Max 的查看器相关调用----------------------------
$PreviewActive = false;
function PreviewUpdate()
{
    if ( $PreviewActive == false )
    {
        $PreviewActive = true;
        Canvas.setContent( guiObjectViewer );
    }
    
    Ambient_R.setText("1.0");
	Ambient_G.setText("1.0");
	Ambient_B.setText("1.0");
    AppLight();
    
    if ( OpenLightButton.IsStateOn() == false )
        OpenLightButton.performClick();    
    
    EditorObjectView.loopAnimation(1);
    ActionVolume.setValue(1);
    SelectTreeNode( "previewer.mod" );
    SelectActionNode( "previewer.ani" );
    
    if ( LinkPartButton.IsStateOn() == false )
        LinkPartButton.performClick();
  
    EditorObjectView.clearAllParts();
    
    %partName = "previewer.cs";
    exec( "gameres/" @ %partName );
    SelectPartNode2( %partName );
    
       //load uncompiled...
   %basepath = "gameres/";
   %path = %basepath @ "previewer_*.cs";
   echo("搜索路径" @ %path);
   %file = findFirstFile(%path);
   echo("查找到粒子文件" @ %file);
   while(%file !$= "")
   {
     %file = filePath(%file) @ "/" @ fileBase(%file) @ ".cs";
     exec(%file);
     echo("执行粒子文件" @ %file);
     SelectPartNode2( fileBase(%file) );
     %file = findNextFile(%path);
   }
   
   echo("查找粒子文件结束");
}
//----------------------------------------------------------------------------------------------

// 灯光默认打开
if ( OpenLightButton.IsStateOn() == false )
        OpenLightButton.performClick(); 
