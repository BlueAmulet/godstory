//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//�ͻ��������˹��õĽű����������񽻽������ж�
//==================================================================================


//���񽻽������ж�
function TeacherDoorMission(%Player, %Mid, %Type, %Msg)
{

	 %TiaoJian    = "";
	 
	  %Hs = %Player.GetMissionFlag(%Mid,100); //ȡ�������
		%Cs = %Player.GetMissionFlag(%Mid,200); //ȡ�������
  	%Lx = %Player.GetMissionFlag(%Mid,300); //ȡ��������
    %SmBuff = %Player.GetBuffCount(32005,1); //ʦ����ȴ
	//��������������ж�
	if(%Type == 1)
		{  
			if(%Player.GetMissionFlag(%Mid,300)!=0)												{%TiaoJian = %TiaoJian @ "A";}				//�Ƿ��Ѿ����ܵ�ǰ����
			if(%Player.GetLevel() < 20 )											              {%TiaoJian = %TiaoJian @ "C";}				//�Ƿ�ﵽ������ܵȼ�
			if(%Player.GetAcceptedMission() >= 20)													{%TiaoJian = %TiaoJian @ "F";}				//�Ѿ����ܵ����������Ƿ�����
			if(%Player.GetCycleMissionTimes(%Mid) == 50)	                  {%TiaoJian = %TiaoJian @ "G";}				//�Ƿ�ʦ�����������50��
	    if(%SmBuff ==1)                                                 {%TiaoJian = %TiaoJian @ "H";}        //�Ƿ���ʦ����ȴ
	                                                                                                        	//�Ƿ����ɹ�����ȷ
		}
			echo("%TiaoJian = "@%TiaoJian); 
	 
	//��������������ж�
	if(%Type == 2)
		{
			if(!%Player.IsAcceptedMission(20001))				  										{%TiaoJian = %TiaoJian @ "A";}				//�Ƿ��Ѿ����ܵ�ǰ����
			if(%Player.IsFinishedMission(20001))															{%TiaoJian = %TiaoJian @ "B";}				//�Ƿ��Ѿ���ɵ�ǰ����
  
			%Lv = %Player.GetLevel();
			%Lx = %Player.GetMissionFlag(%Mid,300);
			
		
			if(((%Lv >= 20)&&(%Lv < 40))&&(%Lx ==1)){%tt = 1;}
			if(((%Lv >= 20)&&(%Lv < 40))&&(%Lx ==2)){%tt = 2;}
			if(((%Lv >= 20)&&(%Lv < 40))&&(%Lx ==3)){%tt = 1;}
			if(((%Lv >= 20)&&(%Lv < 40))&&(%Lx ==4)){%tt = 1;}			
				
			if(((%Lv >= 40)&&(%Lv < 60))&&(%Lx ==1)){%tt = 2;}
			if(((%Lv >= 40)&&(%Lv < 60))&&(%Lx ==2)){%tt = 4;}
			if(((%Lv >= 40)&&(%Lv < 60))&&(%Lx ==3)){%tt = 2;}
			if(((%Lv >= 40)&&(%Lv < 60))&&(%Lx ==4)){%tt = 1;}
				
			if(((%Lv >= 60)&&(%Lv < 80))&&(%Lx ==1)){%tt = 3;}
			if(((%Lv >= 60)&&(%Lv < 80))&&(%Lx ==2)){%tt = 4;}
			if(((%Lv >= 60)&&(%Lv < 80))&&(%Lx ==3)){%tt = 2;}
			if(((%Lv >= 60)&&(%Lv < 80))&&(%Lx ==4)){%tt = 1;} 	
			
			  
	    	for(%i = 0; %i < %tt; %i++)
				{ 
	
				   	 if((%Lx == 1)||(%Lx == 2)||(%Lx == 3))
	           {
				   			if(!%Player.GetMissionFlag(%Mid,1350 + %i) == 1)
	
				   			{
				   				%TiaoJian = %TiaoJian @ "E ";
	
				   				break;
	
				   			}
				   	}
				   		
				   	if((%Lx == 4) ||(%Lx == 6))
				   	{	
				   		if(!%Player.GetMissionFlag(%Mid,3300 + %i) == 1)
	
				   			{
				   				%TiaoJian = %TiaoJian @ "E ";
	
				   				break;
	
				   			}
				   	}	
				   	
				   	if(%Lx == 5)
				   	{	
				   		  
				   		  for(%i = 0; %i < 9; %i++)
				   		 {
									%ItemID = %Player.GetMissionFlag(%Mid,2100+ %i);
									
									%ItemNum = %Player.GetMissionFlag(%Mid,2200 + %i);
									
					   			if(%Player.GetItemCount(%ItemID) < %ItemNum)
					   				
					   			{
					   				%TiaoJian = %TiaoJian @ "E ";
					   				
					   				break;
		
		  			   		}
		  			   }	
		  		  }	
		  		  
		  		  
		  		  
	   		}
   	
  	}
  	
  //�����޷����ܻ���ɵ�ԭ����Ϣ
  if( (%TiaoJian !$= "")&&(%Msg > 0) )
  	 {
  	 	 	 echo("��������");
				 NoWayAccept(%Player,%TiaoJian, %Type, %Msg);
				 
				 echo("%Player="@%Player);
				 echo("%TiaoJian="@%TiaoJian);
				 echo("%Type="@%Type);
				 echo("%Msg="@%Msg);
		 }
				
				
	
		return %TiaoJian;
}

function NoWayAccept(%Player,%TiaoJian, %Type, %Msg)
{
	%Txt1 = "";
	%Txt2 = "";
	
	echo("������Ϣ");
	
//�ж��Ƿ�Ҫ������Ϣ
	if(%Msg == 1)
	  for(%i = 0; %i < 99; %i++)
		 {
				%Why = GetWord(%TiaoJian, %i);
		
				if(%Why $= "")
					break;
				else
					{
						if(%Why $= "A"){%Txt1 = "���Ѿ����ܹ�ʦ������";}
						if(%Why $= "B"){%Txt1 = "���Ѿ������ʦ������";}
						if(%Why $= "C"){%Txt1 = "��ĵȼ�δ�ﵽ20��,�޷���ȡʦ������";}
						if(%Why $= "D"){%Txt1 = "���ʦ�������ѳ�ʱ";}
						if(%Why $= "E"){%Txt1 = "���ʦ��������δ���.";}
						if(%Why $= "F"){%Txt1 = "�����Ͻ��ܵ�������������";}
						if(%Why $= "G"){%Txt1 = "������ʦ�������Ѿ�����";}
						if(%Why $= "H"){%Txt1 = "���ʦ�Ż�������ȴ��,���Ե�Ƭ��";}
							
						if(%Where == 1)
		
							%Txt2 = "���޷���������";
		
						else
		
							%Txt2 = "���޷���������";
							
							
						%Txt3 = %Txt1 @ %Txt2;
					  %Txt4 = "<t>" @ %Txt1 @ %Txt2 @ "</t>";
		
						//�жϵ�ǰ�Ƿ�Ϊ�ͻ��˵��ã���ϵͳ��Ϣ
						if($Now_Script == 1)
		
							{
		
								SetScreenMessage(%Txt3, $Color[2]);
								SetChatMessage(%Txt3, $Color[2]);
							}
		
						//ֱ��Echo����
		
						if(%Msg == 2)
							
							{
								
								SendOneScreenMessage(2, %Txt3, %Player);
								SendOneChatMessage(0, %Txt4, %Player);
								
							}
					 }
	     }
}