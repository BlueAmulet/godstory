//===========================================================================================================
// 文档说明:右键弹出菜单界面操作
// 创建时间:2009-11-12
// 创建人: soar
//=========================================================================================================== 
//
//%type
//0：自己
//1：目标玩家
//2：NPC
//3：目标玩家的宠物
//4：自己的宠物
//
function OpenMouseRightDownWnd(%type,%pointx,%pointy)
{
	%player = getPlayer(); 
	if(%player == 0)
	  return;
	  
	$RightDownType = %type;
	%num = 0;
	for(%nCount=0 ; %nCount<14 ;%nCount++ )
	{
		("MenuBtn" @ %nCount).setVisible(false);
	}
	switch(%type)
	{
		case 0:
			if(!IsMyselfHaveTeam())
			{
			  ("MenuBtn" @%num).setText("创建队伍");
			  ("MenuBtn" @%num).Command = "CreateTeam();HideMenu();";               
   		}
			else
			{	
			  ("MenuBtn" @%num).setText("转为团队");
			  ("MenuBtn" @%num).Command = "HideMenu();";      			   
			}
			("MenuBtn" @%num).setVisible(1);
			%num++;    
			
			 %player = GetPlayer();
			 %level = %player.getLevel();	
	     if(%level >= 10)
	     {
	     	("MenuBtn" @%num).setText("摆摊");
			  ("MenuBtn" @%num).Command = "stallage();HideMenu();";      			
			  ("MenuBtn" @%num).setVisible(1);
			  ("MenuBtn" @%num).setActive(1);		   
	     }	
	     else
	     {
	     	("MenuBtn" @%num).setText("摆摊");
			  ("MenuBtn" @%num).Command = "stallage();HideMenu();";      			
			  ("MenuBtn" @%num).setVisible(1);	
			  ("MenuBtn" @%num).setActive(0);
	     }
	     	%num++; 	   
			 
			if(%player.getPKState())
			{				
				("MenuBtn" @%num).setText("切换为和平模式"); 	
				("MenuBtn" @%num).Command = "ChangePKState();HideMenu();";  			    
			}
			else
			{				
				("MenuBtn" @%num).setText("切换为PK模式"); 	
				("MenuBtn" @%num).Command = "ChangePKState();HideMenu();";  			 
			}
			("MenuBtn" @%num).setVisible(1);
			%num++;      
		case 1:
		  ("MenuBtn" @%num).setText("决斗");
		  ("MenuBtn" @%num).Command = "HideMenu();";  
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("跟随");
		  ("MenuBtn" @%num).Command = "followTarget();HideMenu();";  
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("写信"); 
		  ("MenuBtn" @%num).Command = "OpenMailWnd();HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("请求加为好友");
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;   
		  ("MenuBtn" @%num).setText("查看人物信息"); 
		  ("MenuBtn" @%num).Command = "SeeTargetPlayerInfo();HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("查看宠物信息");
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("查看元神信息");
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("复制名字");
		  ("MenuBtn" @%num).Command = "CopyTargetName();HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("交互"); 
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("屏蔽"); 
		  ("MenuBtn" @%num).Command = "AddTargetInBanList();HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;		 	
			//PopUpMenuWnd_MeunList.addRow(%num++,"请求骑宠");
			//PopUpMenuWnd_MeunList.addRow(%num++,"邀请帮派");		
		
		case 2:
		 ("MenuBtn" @%num).setText("跟随"); 
		 ("MenuBtn" @%num).Command = "HideMenu();"; 
		 ("MenuBtn" @%num).setVisible(1);
		  %num++;					
		case 3:
		("MenuBtn" @%num).setText("查看信息");
		("MenuBtn" @%num).Command = "SeeTargetPlayerInfo();HideMenu();";  
		 ("MenuBtn" @%num).setVisible(1);
		  %num++;					
		case 4:
		 ("MenuBtn" @%num).setText("查看信息");
		 ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("治疗");
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("驯服");
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("切换防御模式"); 
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("收回宠物"); 
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;			
			
	  default : return;
	 }
	%num = %num * 20 + 12;
	PopUpMenuWnd.setextent(getword(PopUpMenuWnd.getextent(),0),%num);
	PopUpMenuWnd.setposition(%pointx,%pointy);
	PopUpMenuWnd.setVisible(true);
	//Canvas.setTempWnd(PopUpMenuWnd);
}

function HideMenu()
{
	PopUpMenuWnd.setVisible(false);
	Canvas.popDialog(PopUpMenuWndCtrl);
}

function SeeTargetPlayerInfo()
{		
	if($SocialFindPlayerAction == 0)
	{
		$SocialFindPlayerAction = 2;
	}
	else
		return;
	SptSocialFindPlayerById(GetTargetPlayerId());
}