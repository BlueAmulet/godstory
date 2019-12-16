//-----------------------------------------------------------------------------
// PowerEngine                                                                 
// Copyright (C) FireRain.com, Inc.                                            
//-----------------------------------------------------------------------------

$appName 			= "���ɴ�";
$defaultGame 	= "gameres";
$isDedicated 	= false;
$dirCount 		= 1;
$userDirs 		= $defaultGame;

function createCanvas(%windowTitle)
{
   // ������Ļ��Ⱦ����
   %foo = new GuiCanvas(Canvas);   
   // ���ô��ڱ���
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
// ���������в���
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
					     			error("����: ����������в���! [-connect IP��ַ]");
					        	
		    		//--------------------
		    		case "-log":
		        		%argUsed[%i]++;
		         		if (%hasNextArg)
								{
		            		// ���ؿ���̨��־
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
		            	error("����: ����������в���! [-log <Mode: 0,1,2>]");
		
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
      			error("����: ����������в���: " @ $Game::argv[%i]);
		}
}

//------------------------------------------------------------------------------
// �������Ŀ¼��main.cs�ű�
function loadDirs(%dirPath)
{
   %dirPath = nextToken(%dirPath, token, ";");
   if (%dirPath !$= "")
      loadDirs(%dirPath);

   if(exec(%token @ "/main.cs") != true)
   {
      error("����: �����ҵ���ԴĿ¼: " @ %token );
      $dirCount--;
   }
}

// ���������в���
ParseCommandLine();

// ����Ŀ¼����Դ�ͽű�
setModPaths($userDirs);

// �������Ŀ¼��main.cs�ű�
loadDirs($userDirs);

if($dirCount == 0) 
{
   enableWinConsole(true);
   error("����: �����ҵ���ԴĿ¼!");
   quit();
}

onStart();