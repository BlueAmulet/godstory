//===========================================================================================================
// �ĵ�˵��:�Ҽ������˵��������
// ����ʱ��:2009-11-12
// ������: soar
//=========================================================================================================== 
//
//%type
//0���Լ�
//1��Ŀ�����
//2��NPC
//3��Ŀ����ҵĳ���
//4���Լ��ĳ���
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
			  ("MenuBtn" @%num).setText("��������");
			  ("MenuBtn" @%num).Command = "CreateTeam();HideMenu();";               
   		}
			else
			{	
			  ("MenuBtn" @%num).setText("תΪ�Ŷ�");
			  ("MenuBtn" @%num).Command = "HideMenu();";      			   
			}
			("MenuBtn" @%num).setVisible(1);
			%num++;    
			
			 %player = GetPlayer();
			 %level = %player.getLevel();	
	     if(%level >= 10)
	     {
	     	("MenuBtn" @%num).setText("��̯");
			  ("MenuBtn" @%num).Command = "stallage();HideMenu();";      			
			  ("MenuBtn" @%num).setVisible(1);
			  ("MenuBtn" @%num).setActive(1);		   
	     }	
	     else
	     {
	     	("MenuBtn" @%num).setText("��̯");
			  ("MenuBtn" @%num).Command = "stallage();HideMenu();";      			
			  ("MenuBtn" @%num).setVisible(1);	
			  ("MenuBtn" @%num).setActive(0);
	     }
	     	%num++; 	   
			 
			if(%player.getPKState())
			{				
				("MenuBtn" @%num).setText("�л�Ϊ��ƽģʽ"); 	
				("MenuBtn" @%num).Command = "ChangePKState();HideMenu();";  			    
			}
			else
			{				
				("MenuBtn" @%num).setText("�л�ΪPKģʽ"); 	
				("MenuBtn" @%num).Command = "ChangePKState();HideMenu();";  			 
			}
			("MenuBtn" @%num).setVisible(1);
			%num++;      
		case 1:
		  ("MenuBtn" @%num).setText("����");
		  ("MenuBtn" @%num).Command = "HideMenu();";  
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("����");
		  ("MenuBtn" @%num).Command = "followTarget();HideMenu();";  
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("д��"); 
		  ("MenuBtn" @%num).Command = "OpenMailWnd();HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("�����Ϊ����");
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;   
		  ("MenuBtn" @%num).setText("�鿴������Ϣ"); 
		  ("MenuBtn" @%num).Command = "SeeTargetPlayerInfo();HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("�鿴������Ϣ");
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("�鿴Ԫ����Ϣ");
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("��������");
		  ("MenuBtn" @%num).Command = "CopyTargetName();HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("����"); 
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("����"); 
		  ("MenuBtn" @%num).Command = "AddTargetInBanList();HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;		 	
			//PopUpMenuWnd_MeunList.addRow(%num++,"�������");
			//PopUpMenuWnd_MeunList.addRow(%num++,"�������");		
		
		case 2:
		 ("MenuBtn" @%num).setText("����"); 
		 ("MenuBtn" @%num).Command = "HideMenu();"; 
		 ("MenuBtn" @%num).setVisible(1);
		  %num++;					
		case 3:
		("MenuBtn" @%num).setText("�鿴��Ϣ");
		("MenuBtn" @%num).Command = "SeeTargetPlayerInfo();HideMenu();";  
		 ("MenuBtn" @%num).setVisible(1);
		  %num++;					
		case 4:
		 ("MenuBtn" @%num).setText("�鿴��Ϣ");
		 ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("����");
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("ѱ��");
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1); 
		  %num++;
		  ("MenuBtn" @%num).setText("�л�����ģʽ"); 
		  ("MenuBtn" @%num).Command = "HideMenu();"; 
		  ("MenuBtn" @%num).setVisible(1);
		  %num++;
		  ("MenuBtn" @%num).setText("�ջس���"); 
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