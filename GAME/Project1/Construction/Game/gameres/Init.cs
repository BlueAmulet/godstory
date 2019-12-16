//-----------------------------------------------------------------------------
// PowerEngine                                                                 
// Copyright (C) FireRain.com, Inc.                                            
//-----------------------------------------------------------------------------

$appName 			= "神仙传";
$defaultGame 	= "gameres";
$isDedicated 	= false;
$dirCount 		= 1;
$userDirs 		= $defaultGame;

function createCanvas(%windowTitle)
{
   // 创建屏幕渲染对象
   %foo = new GuiCanvas(Canvas);   
   // 设置窗口标题
   Canvas.setWindowTitle(%windowTitle);
   return true;
}

function uiShowMainWindow()
{
    Canvas.showWindow();
}

function pushFront(%list, %token, %delim)
{
   if (%list !$= "")
      return %token @ %delim @ %list;
   return %token;
}

function pushBack(%list, %token, %delim)
{
   if (%list !$= "")
      return %list @ %delim @ %token;
   return %token;
}

function popFront(%list, %delim)
{
   return nextToken(%list, unused, %delim);
}

//------------------------------------------------------------------------------
// 处理命令行参数
function ParseCommandLine()
{
		%logModeSpecified = false;
		for (%i = 1; %i < $Game::argc ; %i++)
		{
				%arg = $Game::argv[%i];
				%nextArg = $Game::argv[%i+1];
				%hasNextArg = $Game::argc - %i > 1;
				%logModeSpecified = false;
		
				switch$(%arg)
				{
						//--------------------
						case "-connect":
								%argUsed[%i]++;
					  		if (%hasNextArg)
					  		{
					     			$JoinGameAddress = %nextArg;
					     			%argUsed[%i+1]++;
					     			%i++;
					  		}
					  		else
					     			error("错误: 错误的命令行参数! [-connect IP地址]");
					        	
		    		//--------------------
		    		case "-log":
		        		%argUsed[%i]++;
		         		if (%hasNextArg)
								{
		            		// 开关控制台日志
		            		if (%nextArg != 0)
		            		{
		               			// Dump existing console to logfile first.
		               			%nextArg += 4;
		            		}
		            		setLogMode(%nextArg);
		            		%logModeSpecified = true;
		            		%argUsed[%i+1]++;
		            		%i++;
		         		}
		         		else
		            	error("错误: 错误的命令行参数! [-log <Mode: 0,1,2>]");
		
		    		//--------------------
		    		case "-console":
		        		enableWinConsole(true);
		         		%argUsed[%i]++;
					     
						case "-fullscreen":
					  		setFullScreen(true);
					  		%argUsed[%i]++;
					
						case "-windowed":
					  		setFullScreen(false);
					  		%argUsed[%i]++;
					
						case "-openGL":
					  		$pref::Video::displayDevice = "OpenGL";
					  		%argUsed[%i]++;
					
						case "-directX":
					  		$pref::Video::displayDevice = "D3D";
					  		%argUsed[%i]++;
					
						case "-voodoo2":
					  		$pref::Video::displayDevice = "Voodoo2";
					  		%argUsed[%i]++;
					
						case "-autoVideo":
					  		$pref::Video::displayDevice = "";
					  		%argUsed[%i]++;
		
		    		//-------------------
		    		default:
		        		%argUsed[%i]++;
		         		if($userDirs $= "")
		            		$userDirs = %arg;
		   	}
		}
		
		if($dirCount == 0) 
		{
    		$userDirs = $defaultGame;
      	$dirCount = 1;
		}
		
		// Default to a new logfile each session.
		if( !%logModeSpecified )
		{
   			if( $platform !$= "xbox" && $platform !$= "xenon" )
      			setLogMode(6);
		}
		
		// Display an error message for unused arguments
		for (%i = 1; %i < $Game::argc; %i++)
		{
   			if (!%argUsed[%i])
      			error("错误: 错误的命令行参数: " @ $Game::argv[%i]);
		}
}

//------------------------------------------------------------------------------
// 逐个加载目录下main.cs脚本
function loadDirs(%dirPath)
{
   %dirPath = nextToken(%dirPath, token, ";");
   if (%dirPath !$= "")
      loadDirs(%dirPath);

   if(exec(%token @ "/main.cs") != true)
   {
      error("错误: 不能找到资源目录: " @ %token );
      $dirCount--;
   }
}

// 解析命令行参数
ParseCommandLine();

// 搜索目录下资源和脚本
setModPaths($userDirs);

// 逐个加载目录下main.cs脚本
loadDirs($userDirs);

if($dirCount == 0) 
{
   enableWinConsole(true);
   error("错误: 不能找到资源目录!");
   quit();
}

onStart();