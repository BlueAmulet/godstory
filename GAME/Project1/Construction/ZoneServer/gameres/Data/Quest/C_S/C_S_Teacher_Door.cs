function Mission20001(%Npc, %Player, %State, %Conv)
{
		%Mid = "20001";											               //������
		%PlayerID = %Player.GetPlayerID();	               //��ȡ��ҵı��

		if(%Player.GetFamily()==1){%NpcA ="401011006"; %NpcZ ="401011006";%NpcC ="410104001";} //��ϼ(ʥ)//������(͵����)
  	if(%Player.GetFamily()==2){%NpcA ="401006006"; %NpcZ ="401006006";%NpcC ="410204001";} //�ز�(��)//�޺�(͵����)
		if(%Player.GetFamily()==3){%NpcA ="401007006"; %NpcZ ="401007006";%NpcC ="410304001";} //����(��)//���ŵ���(͵����)
	  if(%Player.GetFamily()==4){%NpcA ="401005006"; %NpcZ ="401005006";%NpcC ="410404001";} //ī��(��)//���(͵����)
	 	if(%Player.GetFamily()==5){%NpcA ="401009006"; %NpcZ ="401009006";%NpcC ="410504001";} //ڤʦ(��)//���Ĺ���(͵����)
		if(%Player.GetFamily()==6){%NpcA ="401010006"; %NpcZ ="401010006";%NpcC ="410604001";} //����(��)//ɽ��(͵����)
	  if(%Player.GetFamily()==7){%NpcA ="401008006"; %NpcZ ="401008006";%NpcC ="410704001";} //����(��)//�������(͵����)
	  if(%Player.GetFamily()==8){%NpcA ="401004006"; %NpcZ ="401004006";%NpcC ="410804001";} //��ħ(ħ)//սħ(͵����)

	if(%State ==0)
		{	
				if(%Npc.GetDataID() $= %NpcA)
					{
						 if(%Player.GetMissionFlag(%Mid,300)==0)
								%Conv.AddOption(510 @ %Mid, 510); //��ʾʦ�ž���ѡ��
						 else
						 		%Conv.AddOption(550 @ %Mid, 550); //�������
					} 
			  if(%Npc.GetDataID() $= %NpcC){%Conv.AddOption(520 @ %Mid, 520);}//��ʾ͵���ĵ���ѡ�� 	
			  	
				if(%Player.GetMissionFlag(%Mid,300) == 1) //�м����������ж�
				  {
						for(%i = 0; %i < 9; %i++)
							 {
								 if(%Player.GetMissionFlag(%Mid, 1150 + %i) $= %Npc.GetDataID())
									 {
										 if(%Player.GetMissionFlag(%Mid, 1350 + %i) == 0)
											 {
												 %Conv.AddOption(530 @ %Mid, 530);//��ʾ�м�Ի�ѡ��
																	
													break;
											 }
										}
							 }
					 }
					 
		}			
			
		else if(%State ==510)
			   return Mission20001_TiaoJian(%Npc, %Player, %State, %Conv);	//���������ж�

		else if((%State ==530)||(%State ==531))
		     return ZhiJianGuoCheng(%Npc, %Player, %State, %Conv); 			 //�м���̶Ի�
		     
		else if((%State ==550)||(%State ==551)||(%State ==552))
		     return WanChengRenWu(%Npc, %Player, %State, %Conv); 				 //�������
		     
		else if((%State ==520)||(%State ==521)) 
		     return TouLanDiZi(%Npc, %Player, %State, %Conv); 					 //͵���ĵ��� 
			   
}

function Mission20001_TiaoJian(%Npc, %Player, %State, %Conv)
{
	 %Mid = "20001";													//������
	 %PlayerID = %Player.GetPlayerID();	      //��ȡ��ҵı�� 
	 %SmBuff = %Player.GetBuffCount(32005,1); //ʦ����ȴ
	 
	 if(%Player.GetMissionFlag(%Mid,300)!=0){%Conv.SetText( 501 @ %Mid);}
   else if(%Player.GetLevel() < 10 ){%Conv.SetText(502 @ %Mid);}
   else if(%Player.GetAcceptedMission() >= 20){%Conv.SetText(503 @ %Mid);}
   else if(%Player.GetCycleMissionTimes(%Mid) == 50){%Conv.SetText(504 @ %Mid);}
   else if(%SmBuff ==1){%Conv.SetText(505 @ %Mid);}
   else return Mission20001_LeiXing(%Npc, %Player, %State, %Conv);	//������������						  
}

function Mission20001_LeiXing(%Npc, %Player, %State, %Conv)
{

	 %Mid = "20001";														//������
	 %PlayerID = %Player.GetPlayerID();	      //��ȡ��ҵı��
	 //%Lx = %Player.GetMissionFlag(%Mid,300);  //��ȡ��������
	 //%Mz = %Player.GetFamily();                //��ȡ���ְҵ
	 //%Lv = %Player.GetLevel();   						  //��ȡ��ҵȼ�
	 
	 %Hs = %Player.GetMissionFlag(%Mid,100);  //��ȡ������
	 %Cs = %Player.GetMissionFlag(%Mid,200);  //��ȡ�������
	 %Zs = %Player.GetCycleMissionTimes(%Mid); //��ȡ��������
	 %FHs = %Player.GetFlagsByte(1);           //������������
	 %FCs = %Player.GetFlagsByte(2);           //�������������
	 
		if(((%Cs==0)&&(%Hs==0))&&((%FCs==0)&&(%FHs==0)))
			{ 
				%Player.SetMissionFlag(%Mid,100,1);
			  %Player.SetMissionFlag(%Mid,200,1);
			  echo("���� ="@%Player.GetMissionFlag(%Mid,100));
			  echo("���� ="@%Player.GetMissionFlag(%Mid,200));        
			}
			
		else if(((%Cs==10)&&(%Hs==5))||((%FCs==10)&&(%FHs==5))) 
				  {
				  	%Player.SetMissionFlag(%Mid,100,1);
			  		%Player.SetMissionFlag(%Mid,200,1);
				  }
				  			
		else if(%FCs < 10)			                  	
					{
						%Player.SetMissionFlag(%Mid,100,%FHs);
						%Player.SetMissionFlag(%Mid,200,%FCs + 1);
						%Player.SetFlagsByte(1,0);
						%Player.SetFlagsByte(2,0);
															                  	
					} //�����������
												                  
		else     	
				 {
				 	  %Player.SetMissionFlag(%Mid,100,%FHs + 1);
					  %Player.SetMissionFlag(%Mid,200,1);
						%Player.SetFlagsByte(1,0);
						%Player.SetFlagsByte(2,0);
				 }//����������
							
	  if(%Player.GetMissionFlag(%Mid,200) != 0) //���ݴ��������Ƿ񸽼ӵȼ�
			{		

				if((%Player.GetMissionFlag(%Mid,200) == 1)||(%Player.GetMissionFlag(%Mid,200) == 2)){%Lv = %Player.GetLevel();}
				if(%Player.GetMissionFlag(%Mid,200) == 3){%Lv = %Player.GetLevel() + 1;}
				if(%Player.GetMissionFlag(%Mid,200) == 4){%Lv = %Player.GetLevel() + 2;}
				if(%Player.GetMissionFlag(%Mid,200) == 5){%Lv = %Player.GetLevel() + 3;}
				if(%Player.GetMissionFlag(%Mid,200) == 6){%Lv = %Player.GetLevel() + 4;}
				if(%Player.GetMissionFlag(%Mid,200) == 7){%Lv = %Player.GetLevel() + 5;}
				if(%Player.GetMissionFlag(%Mid,200) == 8){%Lv = %Player.GetLevel() + 6;}
				if(%Player.GetMissionFlag(%Mid,200) == 9){%Lv = %Player.GetLevel() + 7;}
				if(%Player.GetMissionFlag(%Mid,200) == 10){%Lv = %Player.GetLevel() + 8;}
					
				echo("%Lv ="@%Lv);
			}	
					
		if((%Lv >= 10)&&(%Lv <= 29))
			{
					echo("%S="@%Player.GetMissionFlag(%Mid,200));
					if( (%Player.GetMissionFlag(%Mid,200) == 1)||(%Player.GetMissionFlag(%Mid,200) == 2) ){%LeiXing = 1;}//����
					if( (%Player.GetMissionFlag(%Mid,200) > 2)&&(%Player.GetMissionFlag(%Mid,200) < 10) )
						{
							if(%Player.GetMissionFlag(%Mid,200) == 3){%A =1;%B =2;}//����,����
							if( (%Player.GetMissionFlag(%Mid,200) == 4)||(%Player.GetMissionFlag(%Mid,200) == 8) ){%A =2;%B =4;}//����,����
							if(%Player.GetMissionFlag(%Mid,200) == 5){%A =3;%B =4;}//Ѳ��,����
							if(%Player.GetMissionFlag(%Mid,200) == 6){%A =1;%B =3;}//����,Ѳ��
							if(%Player.GetMissionFlag(%Mid,200) == 7){%A =1;%B =4;}//����,����
							if(%Player.GetMissionFlag(%Mid,200) == 9){%A =2;%B =3;}//����,Ѳ��
					
							%Lx = GetRandom(1,2);
							if(%Lx ==1){%LeiXing = %A;}
							if(%Lx ==2){%LeiXing = %B;}
						}
					if(%Player.GetMissionFlag(%Mid,200) == 10){%LeiXing = 3;}//Ѳ��	
						
					echo("%LeiXing="@%LeiXing);
			 }
			 
		if( (%Lv >= 30)&&(%Lv <= 59) )
			{
					if(%Player.GetMissionFlag(%Mid,200) == 1){%LeiXing = 1;}//����
					if( (%Player.GetMissionFlag(%Mid,200) > 1)&&(%Player.GetMissionFlag(%Mid,200) < 10) )
						{	
							if(%Player.GetMissionFlag(%Mid,200) == 2){%A =1;%B =2;}//����,����
							if(%Player.GetMissionFlag(%Mid,200) == 3){%A =2;%B =4;}//����,����
							if(%Player.GetMissionFlag(%Mid,200) == 4){%A =3;%B =4;}//Ѳ��,����	
							if( (%Player.GetMissionFlag(%Mid,200) == 5)||(%Player.GetMissionFlag(%Mid,200) == 9) ){%A =3;%B =7;}//Ѳ��,����	
							if(%Player.GetMissionFlag(%Mid,200) == 6){%A =4;%B =7;}//����,����	
							if(%Player.GetMissionFlag(%Mid,200) == 7){%A =1;%B =4;}//����,����	
							if(%Player.GetMissionFlag(%Mid,200) == 8){%A =2;%B =3;}//����,Ѳ��	
							
							%Lx = GetRandom(1,2);
							if(%Lx ==1){%LeiXing = %A;}
							if(%Lx ==2){%LeiXing = %B;}
						}
					if(%Player.GetMissionFlag(%Mid,200) == 10){%LeiXing = 5;}//Ѳ��	
			}
					
	 if(%LeiXing == 1)//����
		 {
			 if((%Lv >= 10)&&(%Lv <= 15)){%SongXin = 1;%S =1;}
			 if((%Lv >= 16)&&(%Lv <= 19)){%SongXin = 2;%S =1;}
			 if((%Lv >= 20)&&(%Lv <= 29)){%SongXin = 3;%S =1;}
			 if((%Lv >= 30)&&(%Lv <= 35)){%SongXin = 4;%S =1;}
			 if((%Lv >= 36)&&(%Lv <= 49)){%SongXin = 5;%S =1;}
			 if((%Lv >= 50)&&(%Lv <= 55)){%SongXin = 6;%S =1;}
       
			 %SX = GetRandomNum(%S,1,%SongXin);
			        		
			 %SxID1 = GetWord(%SX,1);
			 %SxID2 = GetWord(%SX,2);
			 %SxID3 = GetWord(%SX,3);
			 %SxID4 = GetWord(%SX,4);
			   
			 echo("%SxID1 ="@%SxID1);
			 echo("%SxID2 ="@%SxID2);
			 echo("%SxID3 ="@%SxID3);
			 echo("%SxID4 ="@%SxID4);     		
			      
			 if((%SxID1 == 1)||(%SxID2 == 1)||(%SxID3 == 1)||(%SxID4 == 1))
			 	{
			 		 echo("111111111");
			 		 eval("%MissionData = $SongXin_1_"@ %Player.GetFamily() @ ";");	
			 				
			 		 %SongXinID = GetRandom(1,%MissionData);  
			 		 
			 			echo("%SongXinID= "@%SongXinID);	
			 				
			 		 if(%SxNpc1 == 0){eval("%SxNpc1 = $SongXin_1_"@ %Player.GetFamily() @ "[%SongXinID]" @ ";");}
			 		 else if(%SxNpc1 != 0){eval("%SxNpc2 = $SongXin_1_"@ %Player.GetFamily() @ "[%SongXinID]" @ ";");}
			 				
			 		 echo("%SxNpc1= "@%SxNpc1);
			 	}
			 					
			 if((%SxID1 == 2)||(%SxID2 == 2)||(%SxID3 == 2)||(%SxID4 == 2))
			 	{
			 		 %MissionData = $SongXin_2;
			 		 %SongXinID = GetRandom(1,%MissionData);
			 				
			 		 if(%SxNpc1 == 0){%SxNpc1 = $SongXin_2[%SongXinID];}
			      else if(%SxNpc1 != 0){%SxNpc2 = $SongXin_2[%SongXinID];}	
			 	}
			 	
			 if((%SxID1 == 3)||(%SxID2 == 3)||(%SxID3 == 3)||(%SxID4 == 3))
			 	{
			 		 %MissionData = $SongXin_3;
			 		 %SongXinID = GetRandom(1,%MissionData);
			 				
			 		 if(%SxNpc1 == 0){%SxNpc1 = $SongXin_3[%SongXinID];}
			      else if(%SxNpc1 != 0){%SxNpc2 = $SongXin_3[%SongXinID];}	
			 	}
			 	
			 if((%SxID1 == 4)||(%SxID2 == 4)||(%SxID3 == 4)||(%SxID4 == 4))
			 	{
			 		 %MissionData = $SongXin_4;
			 		 %SongXinID = GetRandom(1,%MissionData);
			 				
			 		 if(%SxNpc1 == 0){%SxNpc1 = $SongXin_4[%SongXinID];}
			      else if(%SxNpc1 != 0){%SxNpc2 = $SongXin_4[%SongXinID];}	
			 	}
			 	
			 if((%SxID1 == 5)||(%SxID2 == 5)||(%SxID3 == 5)||(%SxID4 == 5))
			 	{
			 		 %MissionData = $SongXin_5;
			 		 %SongXinID = GetRandom(1,%MissionData);
			 				
			 		 if(%SxNpc1 == 0){%SxNpc1 = $SongXin_5[%SongXinID];}
			      else if(%SxNpc1 != 0){%SxNpc2 = $SongXin_5[%SongXinID];}	
			 	}
			 	
			 if((%SxID1 == 6)||(%SxID2 == 6)||(%SxID3 == 6)||(%SxID4 == 6))
			 	{
			 		 %MissionData = $SongXin_4;
			 		 %SongXinID = GetRandom(1,%MissionData);
			 				
			 		 if(%SxNpc1 == 0){%SxNpc1 = $SongXin_4[%SongXinID];}
			      else if(%SxNpc1 != 0){%SxNpc2 = $SongXin_4[%SongXinID];}	
			 	}
       
			 if(%SxNpc1 > 0)
			 	{				  	
			 		 %Player.SetMissionFlag(%Mid,1150,%SxNpc1);		//����Ŀ����
			 		 %Player.SetMissionFlag(%Mid,1250,1);					//����Ŀ�괥������
			 		 %Player.SetMissionFlag(%Mid,1350,0); 				//����Ŀ�괥����ǰ����
			 	}
			 if(%SxNpc2 > 0)
			 	{
			 		%Player.SetMissionFlag(%Mid,1151,%SxNpc2);		//����Ŀ����
			 		%Player.SetMissionFlag(%Mid,1251,1);					//����Ŀ�괥������
			 		%Player.SetMissionFlag(%Mid,1351,0); 				//����Ŀ�괥����ǰ����
			 	}
		   
			 	  %Player.SetMissionFlag(%Mid,300,1);             //���������������
			 		%Player.SetCycleMission(%Mid, %Zs+1);            //������������
			 		%Zd = %Player.GetCycleMissionTimes(%Mid); 
			 		%Player.SetMissionFlag(%Mid,310,%Zd);           //���������������
		 }
					
	 if(%LeiXing == 2)//����
		 {
				if((%Lv >= 10)&&(%Lv <= 15)){%PeiYu = 1;}
				if((%Lv >= 16)&&(%Lv <= 19)){%PeiYu = 1;}
				if((%Lv >= 20)&&(%Lv <= 29)){%PeiYu = 2;}
				if((%Lv >= 30)&&(%Lv <= 35)){%PeiYu = 3;}
				if((%Lv >= 36)&&(%Lv <= 49)){%PeiYu = 4;}
				if((%Lv >= 50)&&(%Lv <= 55)){%PeiYu = 5;}

				if(%PeiYu == 1)
					{ 
						 eval("%PeiYuData = $PeiYu_1_" @ %Player.GetFamily() @ ";");	
				     %PeiYuID = GetRandom(1,%PeiYuData);					
						 eval("%PY = $PeiYu_1_" @ %Player.GetFamily() @ "[%PeiYuID]" @ ";"); 
						 echo("%PeiYuData ="@%PeiYuData);
				  	 echo("%PY="@%PY);
					 }
					 
				if(%PeiYu == 2)	
					{ 
						%PeiYuData = $PeiYu_2;
						%PeiYuID = GetRandom(1,%PeiYuData);
						%PY = $PeiYu_2[%PeiYuID];
						
						echo("%PeiYuData ="@%PeiYuData);
				  	echo("%PY="@%PY);
				  }
				if(%PeiYu == 3)	
					{ 
						%PeiYuData = $PeiYu_3;
						%PeiYuID = GetRandom(1,%PeiYuData);
						%PY = $PeiYu_3[%PeiYuID];
						
						echo("%PeiYuData ="@%PeiYuData);
				  	echo("%PY="@%PY);
				  }
				if(%PeiYu == 4)	
					{ 
						%PeiYuData = $PeiYu_4;
						%PeiYuID = GetRandom(1,%PeiYuData);
						%PY = $PeiYu_4[%PeiYuID];
						
						echo("%PeiYuData ="@%PeiYuData);
				  	echo("%PY="@%PY);
				  }
				if(%PeiYu == 5)	
					{ 
						%PeiYuData = $PeiYu_5;
						%PeiYuID = GetRandom(1,%PeiYuData);
						%PY = $PeiYu_5[%PeiYuID];
						
						echo("%PeiYuData ="@%PeiYuData);
				  	echo("%PY="@%PY);
				  }

					%Player.SetMissionFlag(%Mid,2100,%PY);		//����Ŀ����
					%Player.SetMissionFlag(%Mid,2200,1);			//����Ŀ�괥������
					%Player.SetMissionFlag(%Mid,2300,0); 			//����Ŀ�괥����ǰ����
					
					echo("%PY ="@%Player.GetMissionFlag(%Mid,2100));
					
					%Player.SetMissionFlag(%Mid,300,2);       //��������������
					%Player.SetCycleMission(%Mid, %Zs+1);     //������������
					%Zd = %Player.GetCycleMissionTimes(%Mid); 
					%Player.SetMissionFlag(%Mid,310,%Zd);     //���������������  
		 }
						
   if(%LeiXing == 3)//Ѳ��
		 {
			 if((%Lv >= 10)&&(%Lv <= 15)){%XunLuo = 1;}
			 if((%Lv >= 16)&&(%Lv <= 19)){%XunLuo = 1;}
			 if((%Lv >= 20)&&(%Lv <= 29)){%XunLuo = 1;}
			 if((%Lv >= 30)&&(%Lv <= 35)){%XunLuo = 2;}
			 if((%Lv >= 36)&&(%Lv <= 49)){%XunLuo = 3;}
			 if((%Lv >= 50)&&(%Lv <= 55)){%XunLuo = 4;}
		   
			 if(%XunLuo == 1)
			 	 { 
			 		 eval("%XunLuoData = $XunLuo_1_" @ %Player.GetFamily() @ ";");	
			     %PeiYuID = GetRandom(1,%XunLuoData);					
			 		 eval("%XL = $XunLuo_1_" @ %Player.GetFamily() @ "[%XunLuoID]" @ ";");
			 	 }
			 if(%XunLuo == 2)
			 	 { 
				 		%XunLuoData = $XunLuo_2;
				 		%XunLuoID = GetRandom(1,%XunLuoData);
				 		%XL = $XunLuo_2[%XunLuoID];
			   }
			 if(%XunLuo == 3)
			 	 { 
				 	 %XunLuoData = $XunLuo_3;
				 	 %XunLuoID = GetRandom(1,%XunLuoData);
				 	 %XL = $XunLuo_3[%XunLuoID];
			   }
			 if(%XunLuo == 4)
			 	 { 
				 	 %XunLuoData = $XunLuo_4;
				 	 %XunLuoID = GetRandom(1,%XunLuoData);
				 	 %XL = $XunLuo_4[%XunLuoID];
			   }

			 	%Player.SetMissionFlag(%Mid,1150,%XL);		  //����Ŀ����
			 	%Player.SetMissionFlag(%Mid,1250,1);				//����Ŀ�괥������
			 	%Player.SetMissionFlag(%Mid,1350,0); 				//����Ŀ�괥����ǰ����
			 	
			 	echo("%XL ="@%Player.GetMissionFlag(%Mid,3100));
			 	
			 	%Player.SetMissionFlag(%Mid,300,3);             //��������������
			 	%Player.SetCycleMission(%Mid, %Zs+1);            //������������
			 	%Zd = %player.GetCycleMissionTimes(%Mid); 
			 	%Player.SetMissionFlag(%Mid,310,%Zd);           //���������������  
		 }
                
   if(%LeiXing == 4)//����
		 {
				if((%Lv >= 10)&&(%Lv <= 15)){%GouWu = 1;}
				if((%Lv >= 16)&&(%Lv <= 19)){%GouWu = 2;}
				if((%Lv >= 20)&&(%Lv <= 29)){%GouWu = 3;}
				if((%Lv >= 30)&&(%Lv <= 35)){%GouWu = 4;}
				if((%Lv >= 36)&&(%Lv <= 49)){%GouWu = 5;}
				if((%Lv >= 50)&&(%Lv <= 55)){%GouWu = 6;}
											  	
		  	if(%GouWu == 1)
			 	  { 
							%GouWuData = $GouWu_1;
							%GouWuID = GetRandom(1,%GouWuData);
							%GW = $GouWu_1[%GouWuID];
							echo("%GW="@%GW);
				  }
				if(%GouWu == 2)
			 	  { 
							%GouWuData = $GouWu_2;
							%GouWuID = GetRandom(1,%GouWuData);
							%GW = $GouWu_2[%GouWuID];
							echo("%GW="@%GW);
				  }
				if(%GouWu == 3)
			 	  { 
							%GouWuData = $GouWu_3;
							%GouWuID = GetRandom(1,%GouWuData);
							%GW = $GouWu_3[%GouWuID];
							echo("%GW="@%GW);
				  }
				if(%GouWu == 4)
			 	  { 
						%GouWuData = $GouWu_4;
						%GouWuID = GetRandom(1,%GouWuData);
						%GW = $GouWu_4[%GouWuID];
						echo("%GW="@%GW);
				  }
				if(%GouWu == 5)
			 	  { 
						%GouWuData = $GouWu_5;
						%GouWuID = GetRandom(1,%GouWuData);
						%GW = $GouWu_5[%GouWuID];
						echo("%GW="@%GW);
				  }
				if(%GouWu == 6)
			 	  { 
						%GouWuData = $GouWu_6;
						%GouWuID = GetRandom(1,%GouWuData);
						%GW = $GouWu_6[%GouWuID];
						echo("%GW="@%GW);
				  }
	
				%Player.SetMissionFlag(%Mid,2100,%GW);		//����Ŀ����
				%Player.SetMissionFlag(%Mid,2200,1);			//����Ŀ�괥������
				%Player.SetMissionFlag(%Mid,2300,0); 			//����Ŀ�괥����ǰ����

				%Player.SetMissionFlag(%Mid,300,4);       //��������������
				%Player.SetCycleMission(%Mid, %Zs+1);     //������������
				%Zd = %player.GetCycleMissionTimes(%Mid); 
				%Player.SetMissionFlag(%Mid,310,%Zd);     //���������������  
		 }            
          
   if(%LeiXing == 5)//��ɱ
		 {
		 	  %Cs = %Player.GetMissionFlag(%Mid,200);  //��ȡ�������
		 	  
				if((%Lv >= 30)&&(%Lv <= 35)){%JiSha = 1;}
				if((%Lv >= 36)&&(%Lv <= 49)){%JiSha = 2;}
				if((%Lv >= 50)&&(%Lv <= 55)){%JiSha = 3;}
				
				if(%JiSha == 1)
			 	  {
			 	  	 %JiShaData = $JiSha_1;
						 %JiShaID = GetRandom(1,%JiShaData);
				     %JS = $JiSha_1[%JiShaID];
					}								 
				if(%JiSha == 2)
			 	  {
			 	  	 %JiShaData = $JiSha_2;
						 %JiShaID = GetRandom(1,%JiShaData);
				     %JS = $JiSha_2[%JiShaID];
					}
				if(%JiSha == 3)
			 	  {
			 	  	 %JiShaData = $JiSha_3;
						 %JiShaID = GetRandom(1,%JiShaData);
				     %JS = $JiSha_3[%JiShaID];
					}		
									
      	%JSNum = %Cs * 2;
				%Player.SetMissionFlag(%Mid,3100,%JS);		//����Ŀ����
				%Player.SetMissionFlag(%Mid,3200,%JSNum);				//����Ŀ�괥������
				%Player.SetMissionFlag(%Mid,3300,0); 				//����Ŀ�괥����ǰ����
					
				echo("%JS ="@%Player.GetMissionFlag(%Mid,3100));
					
				%Player.SetMissionFlag(%Mid,300,5);             //��������������
				%Player.SetCycleMission(%Mid, %Zs+1);            //������������
				%Zd = %player.GetCycleMissionTimes(%Mid); 
				%Player.SetMissionFlag(%Mid,310,%Zd);           //���������������  
		 }   
   if(%LeiXing == 7)//����
		 {
				if((%Lv >= 30)&&(%Lv <= 35)){%BuShou = 1;}
				if((%Lv >= 36)&&(%Lv <= 49)){%BuShou = 2;}
				if((%Lv >= 50)&&(%Lv <= 55)){%BuShou = 3;}
					
				if(%BuShou == 1)
			 	  {
			 	  	%BuShouData = $BuShou_1;
						%BuShouID = GetRandom(1,%BuShouData);
						%BS = $BuShou_1[%BuShouID];
					}
					
				if(%BuShou == 2)
			 	  {
			 	  	%BuShouData = $BuShou_2;
						%BuShouID = GetRandom(1,%BuShouData);
						%BS = $BuShou_2[%BuShouID];
					}
					 
				if(%BuShou == 3)
			 	  {
			 	  	%BuShouData = $BuShou_3;
						%BuShouID = GetRandom(1,%BuShouData);
						%BS = $BuShou_3[%BuShouID];
					}   
					
				%Player.SetMissionFlag(%Mid,2100,%BS);		  //����Ŀ����
				%Player.SetMissionFlag(%Mid,2200,1);				//����Ŀ�괥������
				%Player.SetMissionFlag(%Mid,2300,0); 				//����Ŀ�괥����ǰ����
					
				echo("%BS ="@%Player.GetMissionFlag(%Mid,2100));
					
				%Player.SetMissionFlag(%Mid,300,7);         //��������������
				%Player.SetCycleMission(%Mid, %Zs+1);       //������������
				%Zd = %player.GetCycleMissionTimes(%Mid); 
				%Player.SetMissionFlag(%Mid,310,%Zd);       //���������������  
		 }   
					  
	   %Player.UpdateMission(%Mid);					              //����������Ϣ
		 %Player.AddPacket( $SP_Effect[ 1 ] );	            //���ͽ���������Ч
		 %Conv.SetText(%LeiXing + 100  @ %Mid);	            //��ʾ������Ի�
		 %Conv.AddOption(1,531);														//ѡ������Ի�	

}

function ZhiJianGuoCheng(%Npc, %Player, %State, %Conv)
{
	%Mid = "20001";
	
	if(%State ==530)
		{
			for(%i = 0; %i < 9; %i++)
				 {
						if(%Player.GetMissionFlag(%Mid, 1150 + %i) $= %Npc.GetDataID())
							if(%Player.GetMissionFlag(%Mid, 1350 + %i) == 0)
								{
									%Conv.SetText(535 @ %Mid);	//��ʾ����Ի������öԻ��ı���ţ�
									%Conv.AddOption(1,531);//ѡ������Ի�
									%Player.SetMissionFlag(%Mid, 1350 + %i, 1,true); 				//����,Ŀ�괥����ǰ����
																	
									break;
								}
				 }
		}
				 
	if(%State ==531)
		%Conv.SetType(4);	//�رնԻ�
						
}

function WanChengRenWu(%Npc, %Player, %State, %Conv)
{
		%Mid = "20001";
		%Lv = %Player.GetLevel();   //��ȡ��ҵȼ�
		
		 if(%State ==550)
		 	 {
			   if(%Player.GetMissionFlag(%Mid,300) == 1)
				        {
				        	for(%i = 0; %i < 1; %i++)
				        	   {
							   		 		if(!%Player.GetMissionFlag(%Mid,1350 + %i) == 1)
							   			 		{
					               		%Conv.SetText(509 @ %Mid);
								   			  	 break;
							   			 		}
							   			 	else
							   			 		{
							   			 			%Conv.SetText(555 @ %Mid);	//��ʾ������Ի�
														%Conv.AddOption(3,551);    //ѡ�������
							   			 		}
							   		 }
							  }
					   		
				else if(%Player.GetMissionFlag(%Mid,300) == 5)
						   {	
						   		for(%i = 0; %i < 1; %i++)
						   		   {
						   					if(!%Player.GetMissionFlag(%Mid,3300 + %i) == 1)
						   						{
						   				 	    %Conv.SetText(509 @ %Mid);
						   							break;
						   						}
						   					else
							  			   	{
							  			   		 %Conv.SetText(555 @ %Mid);	//��ʾ������Ի�
														 %Conv.AddOption(3,551);    //ѡ�������
							  			   	}
			
						   				}
						   	}	
				   	
					else if((%Player.GetMissionFlag(%Mid,300) == 2)||(%Player.GetMissionFlag(%Mid,300) == 4))
						   		{	
						   		  for(%i = 0; %i < 1; %i++)
						   		 		 {
													%ItemID = %Player.GetMissionFlag(%Mid,2100+ %i);
													%ItemNum = %Player.GetMissionFlag(%Mid,2200 + %i);
										   		if(%Player.GetItemCount(%ItemID) < %ItemNum)	
										   			{
										   				%Conv.SetText(509 @ %Mid);
										   				break;
							
							  			   		}
							  			   	else
							  			   		{
							  			   			%Conv.SetText(555 @ %Mid);	//��ʾ������Ի�
															%Conv.AddOption(3,551);    //ѡ�������
							  			   		}
				  			   		 }	
				  		     }	

			 }
		   
		 if(%State ==551)
			 {	
			    
				    TeacherMissionReward(%Player,20001);	//����������
				  
						%Conv.SetText(508 @ %Mid);	          //��ʾ��������Ի�
						%Conv.AddOption(540 @ %Mid, 552);     //��ʾ��������԰�
						%Conv.AddOption(10,0);								//�ر�
					
			 }
		 if(%State ==552)
		 	 {
					Mission20001_TiaoJian(%Npc, %Player, %State, %Conv);	//������������		 	 	
		 	 }
		 	 
}


function TouLanDiZi(%Npc, %Player, %State, %Conv)
{
	 %Mid = "20001";
	 %Lx = %Player.GetMissionFlag(%Mid,300); //ȡ�������ͱ��
	 %Zs = %Player.GetCycleMissionTimes(%Mid); //��ȡ��������
   %Hs = %Player.GetMissionFlag(%Mid,100); //ȡ������
	 %Cs = %Player.GetMissionFlag(%Mid,200); //ȡ�������
	 
		if(%State ==520)
			{
			  if(%Lx == 0) 											  //��ȡ�������� 	
					{
					  %Conv.SetText(507 @ %Mid);      //��ʾ������Ի�
					  %Conv.AddOption(4,0);           //����	
					}
					        
				else	
					{	
					        
					  %Conv.SetText(506 @ %Mid);	   //��ʾ������Ի�
						%Conv.AddOption(8,521);       //ѡ�֪����
						%Conv.AddOption(4,0);          //����	
												
					}
			 }
			 				
		if(%State ==521) // ͵���ĵ���
			{
					if ((%Zs <= 20)&&(%Lx == 1)){%Tmoney =2 * 50 * %Zs;}
					if ((%Zs <=20)&&(%Lx == 2)){%Tmoney =2 * 50 * %Zs;}
					if ((%Zs <=20)&&(%Lx == 3)){%Tmoney =2 * 50 * %Zs;}
					if ((%Zs <=20)&&(%Lx == 4)){%Tmoney =2 * 100 * %Zs;}
					if ((%Zs <=20)&&(%Lx == 5)){%Tmoney =2 * 100 * %Zs;}
					if ((%Zs <=20)&&(%Lx == 7)){%Tmoney =2 * 100 * %Zs;}		
									
					if ((%Zs > 20)&&(%Lx == 1)){%Tmoney =50 * %Zs;}
					if ((%Zs > 20)&&(%Lx == 2)){%Tmoney =50 * %Zs;}
					if ((%Zs > 20)&&(%Lx == 3)){%Tmoney =50 * %Zs;}
					if ((%Zs > 20)&&(%Lx == 4)){%Tmoney =100 * %Zs;}
					
					if(%Player.GetMoney(1) < %Tmoney)
						{
							%Conv.SetText(511 @ %Mid);
						}
					else
						{	
							%Player.Reducemoney(%Tmoney,1); //����������
							%Player.DelMission(%Mid);       //����������
							%Player.SetFlagsByte(1,%Hs);    //��������������
							%Player.SetFlagsByte(2,%Cs);    //�������������
			        %Conv.SetType(4);	              //�رնԻ�
			      }
			}
}		
//�����������������š���������������������������������������������������������

//�������������������ڣ�ʥ������������������������������������������
//10��-15��
$SongXin_1_1[$SongXin_1_1++] = "410107001";
$SongXin_1_1[$SongXin_1_1++] = "410106001";
$SongXin_1_1[$SongXin_1_1++] = "410105001";
$SongXin_1_1[$SongXin_1_1++] = "401011001";
$SongXin_1_1[$SongXin_1_1++] = "401011002";
$SongXin_1_1[$SongXin_1_1++] = "401011003";
$SongXin_1_1[$SongXin_1_1++] = "401011004";
$SongXin_1_1[$SongXin_1_1++] = "401011005";
//��������������������(��)����������������������������������������
//10��-15��
$SongXin_1_2[$SongXin_1_2++] = "410207001";
$SongXin_1_2[$SongXin_1_2++] = "410206001";
$SongXin_1_2[$SongXin_1_2++] = "410205001";
$SongXin_1_2[$SongXin_1_2++] = "401006001";
$SongXin_1_2[$SongXin_1_2++] = "401006002";
$SongXin_1_2[$SongXin_1_2++] = "401006003";
$SongXin_1_2[$SongXin_1_2++] = "401006004";
$SongXin_1_2[$SongXin_1_2++] = "401006005";
//��������������������(��)����������������������������������������
//10��-15��
$SongXin_1_3[$SongXin_1_3++] = "410307001";
$SongXin_1_3[$SongXin_1_3++] = "410306001";
$SongXin_1_3[$SongXin_1_3++] = "410305001";
$SongXin_1_3[$SongXin_1_3++] = "401007001";
$SongXin_1_3[$SongXin_1_3++] = "401007002";
$SongXin_1_3[$SongXin_1_3++] = "401007003";
$SongXin_1_3[$SongXin_1_3++] = "401007004";
$SongXin_1_3[$SongXin_1_3++] = "401007005";
//���������������ɻ���(��)����������������������������������������
//10��-15��
$SongXin_1_4[$SongXin_1_4++] = "410407001";
$SongXin_1_4[$SongXin_1_4++] = "410406001";
$SongXin_1_4[$SongXin_1_4++] = "410405001";
$SongXin_1_4[$SongXin_1_4++] = "401005001";
$SongXin_1_4[$SongXin_1_4++] = "401005002";
$SongXin_1_4[$SongXin_1_4++] = "401005003";
$SongXin_1_4[$SongXin_1_4++] = "401005004";
$SongXin_1_4[$SongXin_1_4++] = "401005005";
//�����������������Ľ�(��)����������������������������������������
//10��-15��
$SongXin_1_5[$SongXin_1_5++] = "410507001";
$SongXin_1_5[$SongXin_1_5++] = "410506001";
$SongXin_1_5[$SongXin_1_5++] = "410505001";
$SongXin_1_5[$SongXin_1_5++] = "401009001";
$SongXin_1_5[$SongXin_1_5++] = "401009002";
$SongXin_1_5[$SongXin_1_5++] = "401009003";
$SongXin_1_5[$SongXin_1_5++] = "401009004";
$SongXin_1_5[$SongXin_1_5++] = "401009005";
$SongXin_1_5[$SongXin_1_5++] = "401009007";
$SongXin_1_5[$SongXin_1_5++] = "401009008";
//��������������ɽ����(��)����������������������������������������
//10��-15��
$SongXin_1_6[$SongXin_1_6++] = "410607001";
$SongXin_1_6[$SongXin_1_6++] = "410606001";
$SongXin_1_6[$SongXin_1_6++] = "410605001";
$SongXin_1_6[$SongXin_1_6++] = "401010001";
$SongXin_1_6[$SongXin_1_6++] = "401010002";
$SongXin_1_6[$SongXin_1_6++] = "401010003";
$SongXin_1_6[$SongXin_1_6++] = "401010004";
$SongXin_1_6[$SongXin_1_6++] = "401010005";
//�����������������鹬(��)����������������������������������������
//10��-15��
$SongXin_1_7[$SongXin_1_7++] = "410707001";
$SongXin_1_7[$SongXin_1_7++] = "410706001";
$SongXin_1_7[$SongXin_1_7++] = "410705001";
$SongXin_1_7[$SongXin_1_7++] = "401008001";
$SongXin_1_7[$SongXin_1_7++] = "401008002";
$SongXin_1_7[$SongXin_1_7++] = "401008003";
$SongXin_1_7[$SongXin_1_7++] = "401008004";
$SongXin_1_7[$SongXin_1_7++] = "401008005";
//����������������ħ��(ħ)����������������������������������������
//10��-15��
$SongXin_1_8[$SongXin_1_8++] = "410807001";
$SongXin_1_8[$SongXin_1_8++] = "410806001";
$SongXin_1_8[$SongXin_1_8++] = "410805001";
$SongXin_1_8[$SongXin_1_8++] = "401004001";
$SongXin_1_8[$SongXin_1_8++] = "401004002";
$SongXin_1_8[$SongXin_1_8++] = "401004003";
$SongXin_1_8[$SongXin_1_8++] = "401004004";
$SongXin_1_8[$SongXin_1_8++] = "401004005";
//��������������������ͨ�á���������������������������������������
//16��-19��

//�����
$SongXin_2[$SongXin_2++] = "400001001";
$SongXin_2[$SongXin_2++] = "400001002";
$SongXin_2[$SongXin_2++] = "400001003";
$SongXin_2[$SongXin_2++] = "400001004";
$SongXin_2[$SongXin_2++] = "400001005";
$SongXin_2[$SongXin_2++] = "400001006";
$SongXin_2[$SongXin_2++] = "400001007";
$SongXin_2[$SongXin_2++] = "400001008";
$SongXin_2[$SongXin_2++] = "400001009";
$SongXin_2[$SongXin_2++] = "400001011";
//�˴�����
$SongXin_2[$SongXin_2++] = "410107001";
$SongXin_2[$SongXin_2++] = "410106001";
$SongXin_2[$SongXin_2++] = "410105001";
$SongXin_2[$SongXin_2++] = "401011001";
$SongXin_2[$SongXin_2++] = "401011002";
$SongXin_2[$SongXin_2++] = "401011003";
$SongXin_2[$SongXin_2++] = "401011004";
$SongXin_2[$SongXin_2++] = "401011005";
$SongXin_2[$SongXin_2++] = "410207001";
$SongXin_2[$SongXin_2++] = "410206001";
$SongXin_2[$SongXin_2++] = "410205001";
$SongXin_2[$SongXin_2++] = "401006001";
$SongXin_2[$SongXin_2++] = "401006002";
$SongXin_2[$SongXin_2++] = "401006003";
$SongXin_2[$SongXin_2++] = "401006004";
$SongXin_2[$SongXin_2++] = "401006005";
$SongXin_2[$SongXin_2++] = "410307001";
$SongXin_2[$SongXin_2++] = "410306001";
$SongXin_2[$SongXin_2++] = "410305001";
$SongXin_2[$SongXin_2++] = "401007001";
$SongXin_2[$SongXin_2++] = "401007002";
$SongXin_2[$SongXin_2++] = "401007003";
$SongXin_2[$SongXin_2++] = "401007004";
$SongXin_2[$SongXin_2++] = "401007005";
$SongXin_2[$SongXin_2++] = "410407001";
$SongXin_2[$SongXin_2++] = "410406001";
$SongXin_2[$SongXin_2++] = "410405001";
$SongXin_2[$SongXin_2++] = "401005001";
$SongXin_2[$SongXin_2++] = "401005002";
$SongXin_2[$SongXin_2++] = "401005003";
$SongXin_2[$SongXin_2++] = "401005004";
$SongXin_2[$SongXin_2++] = "401005005";
$SongXin_2[$SongXin_2++] = "410507001";
$SongXin_2[$SongXin_2++] = "410506001";
$SongXin_2[$SongXin_2++] = "410505001";
$SongXin_2[$SongXin_2++] = "401009001";
$SongXin_2[$SongXin_2++] = "401009002";
$SongXin_2[$SongXin_2++] = "401009003";
$SongXin_2[$SongXin_2++] = "401009004";
$SongXin_2[$SongXin_2++] = "401009005";
$SongXin_2[$SongXin_2++] = "401009007";
$SongXin_2[$SongXin_2++] = "401009008";
$SongXin_2[$SongXin_2++] = "410607001";
$SongXin_2[$SongXin_2++] = "410606001";
$SongXin_2[$SongXin_2++] = "410605001";
$SongXin_2[$SongXin_2++] = "401010001";
$SongXin_2[$SongXin_2++] = "401010002";
$SongXin_2[$SongXin_2++] = "401010003";
$SongXin_2[$SongXin_2++] = "401010004";
$SongXin_2[$SongXin_2++] = "401010005";
$SongXin_2[$SongXin_2++] = "410707001";
$SongXin_2[$SongXin_2++] = "410706001";
$SongXin_2[$SongXin_2++] = "410705001";
$SongXin_2[$SongXin_2++] = "401008001";
$SongXin_2[$SongXin_2++] = "401008002";
$SongXin_2[$SongXin_2++] = "401008003";
$SongXin_2[$SongXin_2++] = "401008004";
$SongXin_2[$SongXin_2++] = "401008005";
$SongXin_2[$SongXin_2++] = "410807001";
$SongXin_2[$SongXin_2++] = "410806001";
$SongXin_2[$SongXin_2++] = "410805001";
$SongXin_2[$SongXin_2++] = "401004001";
$SongXin_2[$SongXin_2++] = "401004002";
$SongXin_2[$SongXin_2++] = "401004003";
$SongXin_2[$SongXin_2++] = "401004004";
$SongXin_2[$SongXin_2++] = "401004005";

//20��-29��

//����
$SongXin_3[$SongXin_3++] = "400002102";
$SongXin_3[$SongXin_3++] = "400002114";
$SongXin_3[$SongXin_3++] = "400002109";
$SongXin_3[$SongXin_3++] = "400002104";
$SongXin_3[$SongXin_3++] = "400002112";
//������
$SongXin_3[$SongXin_3++] = "401102110";
$SongXin_3[$SongXin_3++] = "401102111";
$SongXin_3[$SongXin_3++] = "401102112";
$SongXin_3[$SongXin_3++] = "401102113";
$SongXin_3[$SongXin_3++] = "401102114";
$SongXin_3[$SongXin_3++] = "401102117";
$SongXin_3[$SongXin_3++] = "401102118";
$SongXin_3[$SongXin_3++] = "401102101";
$SongXin_3[$SongXin_3++] = "401102106";
$SongXin_3[$SongXin_3++] = "401102103";

//30��-35��

//�����
$SongXin_4[$SongXin_4++] = "400001001";
$SongXin_4[$SongXin_4++] = "400001002";
$SongXin_4[$SongXin_4++] = "400001003";
$SongXin_4[$SongXin_4++] = "400001004";
$SongXin_4[$SongXin_4++] = "400001005";
$SongXin_4[$SongXin_4++] = "400001006";
$SongXin_4[$SongXin_4++] = "400001007";
$SongXin_4[$SongXin_4++] = "400001008";
$SongXin_4[$SongXin_4++] = "400001009";
$SongXin_4[$SongXin_4++] = "400001011";

//���ľ�
$SongXin_4[$SongXin_4++] = "401002010";
$SongXin_4[$SongXin_4++] = "401002028";
$SongXin_4[$SongXin_4++] = "401002101";
$SongXin_4[$SongXin_4++] = "401002102";
$SongXin_4[$SongXin_4++] = "401002104";
$SongXin_4[$SongXin_4++] = "401002106";
$SongXin_4[$SongXin_4++] = "401002108";

//�˴�����
$SongXin_4[$SongXin_4++] = "410107001";
$SongXin_4[$SongXin_4++] = "410106001";
$SongXin_4[$SongXin_4++] = "410105001";
$SongXin_4[$SongXin_4++] = "401011001";
$SongXin_4[$SongXin_4++] = "401011002";
$SongXin_4[$SongXin_4++] = "401011003";
$SongXin_4[$SongXin_4++] = "401011004";
$SongXin_4[$SongXin_4++] = "401011005";
$SongXin_4[$SongXin_4++] = "410207001";
$SongXin_4[$SongXin_4++] = "410206001";
$SongXin_4[$SongXin_4++] = "410205001";
$SongXin_4[$SongXin_4++] = "401006001";
$SongXin_4[$SongXin_4++] = "401006002";
$SongXin_4[$SongXin_4++] = "401006003";
$SongXin_4[$SongXin_4++] = "401006004";
$SongXin_4[$SongXin_4++] = "401006005";
$SongXin_4[$SongXin_4++] = "410307001";
$SongXin_4[$SongXin_4++] = "410306001";
$SongXin_4[$SongXin_4++] = "410305001";
$SongXin_4[$SongXin_4++] = "401007001";
$SongXin_4[$SongXin_4++] = "401007002";
$SongXin_4[$SongXin_4++] = "401007003";
$SongXin_4[$SongXin_4++] = "401007004";
$SongXin_4[$SongXin_4++] = "401007005";
$SongXin_4[$SongXin_4++] = "410407001";
$SongXin_4[$SongXin_4++] = "410406001";
$SongXin_4[$SongXin_4++] = "410405001";
$SongXin_4[$SongXin_4++] = "401005001";
$SongXin_4[$SongXin_4++] = "401005002";
$SongXin_4[$SongXin_4++] = "401005003";
$SongXin_4[$SongXin_4++] = "401005004";
$SongXin_4[$SongXin_4++] = "401005005";
$SongXin_4[$SongXin_4++] = "410507001";
$SongXin_4[$SongXin_4++] = "410506001";
$SongXin_4[$SongXin_4++] = "410505001";
$SongXin_4[$SongXin_4++] = "401009001";
$SongXin_4[$SongXin_4++] = "401009002";
$SongXin_4[$SongXin_4++] = "401009003";
$SongXin_4[$SongXin_4++] = "401009004";
$SongXin_4[$SongXin_4++] = "401009005";
$SongXin_4[$SongXin_4++] = "401009007";
$SongXin_4[$SongXin_4++] = "401009008";
$SongXin_4[$SongXin_4++] = "410607001";
$SongXin_4[$SongXin_4++] = "410606001";
$SongXin_4[$SongXin_4++] = "410605001";
$SongXin_4[$SongXin_4++] = "401010001";
$SongXin_4[$SongXin_4++] = "401010002";
$SongXin_4[$SongXin_4++] = "401010003";
$SongXin_4[$SongXin_4++] = "401010004";
$SongXin_4[$SongXin_4++] = "401010005";
$SongXin_4[$SongXin_4++] = "410707001";
$SongXin_4[$SongXin_4++] = "410706001";
$SongXin_4[$SongXin_4++] = "410705001";
$SongXin_4[$SongXin_4++] = "401008001";
$SongXin_4[$SongXin_4++] = "401008002";
$SongXin_4[$SongXin_4++] = "401008003";
$SongXin_4[$SongXin_4++] = "401008004";
$SongXin_4[$SongXin_4++] = "401008005";
$SongXin_4[$SongXin_4++] = "410807001";
$SongXin_4[$SongXin_4++] = "410806001";
$SongXin_4[$SongXin_4++] = "410805001";
$SongXin_4[$SongXin_4++] = "401004001";
$SongXin_4[$SongXin_4++] = "401004002";
$SongXin_4[$SongXin_4++] = "401004003";
$SongXin_4[$SongXin_4++] = "401004004";
$SongXin_4[$SongXin_4++] = "401004005";

//36��-49��
//���ǲ�Ұ
$SongXin_5[$SongXin_5++] = "401103008";
$SongXin_5[$SongXin_5++] = "401103009";
$SongXin_5[$SongXin_5++] = "401103010";
$SongXin_5[$SongXin_5++] = "401103011";
$SongXin_5[$SongXin_5++] = "401103012";
$SongXin_5[$SongXin_5++] = "401103013";
$SongXin_5[$SongXin_5++] = "401103014";
$SongXin_5[$SongXin_5++] = "401103015";
$SongXin_5[$SongXin_5++] = "401103016";
$SongXin_5[$SongXin_5++] = "401103017";
$SongXin_5[$SongXin_5++] = "401103018";
$SongXin_5[$SongXin_5++] = "401103019";
$SongXin_5[$SongXin_5++] = "401103020";
$SongXin_5[$SongXin_5++] = "401103021";
//����ɭ��
$SongXin_5[$SongXin_5++] = "401104001";
$SongXin_5[$SongXin_5++] = "401104005";
$SongXin_5[$SongXin_5++] = "401104007";
$SongXin_5[$SongXin_5++] = "401104008";
$SongXin_5[$SongXin_5++] = "401104011";
$SongXin_5[$SongXin_5++] = "401104014";
$SongXin_5[$SongXin_5++] = "401104018";

//��������������Ѳ�ߡ���������������������������������������������������

//�������������������ڣ�ʥ������������������������������������������
//10��-15��
$XunLuo_1_1[$XunLuo_1_1++] = "410106001";
$XunLuo_1_1[$XunLuo_1_1++] = "410105001";
$XunLuo_1_1[$XunLuo_1_1++] = "401011001";
$XunLuo_1_1[$XunLuo_1_1++] = "401011002";
$XunLuo_1_1[$XunLuo_1_1++] = "401011003";
//��������������������(��)����������������������������������������
//10��-15��
$XunLuo_1_2[$XunLuo_1_2++] = "410206001";
$XunLuo_1_2[$XunLuo_1_2++] = "410205001";
$XunLuo_1_2[$XunLuo_1_2++] = "401006001";
$XunLuo_1_2[$XunLuo_1_2++] = "401006002";
$XunLuo_1_2[$XunLuo_1_2++] = "401006003";
//��������������������(��)����������������������������������������
//10��-15��
$XunLuo_1_3[$XunLuo_1_3++] = "410307001";
$XunLuo_1_3[$XunLuo_1_3++] = "410306001";
$XunLuo_1_3[$XunLuo_1_3++] = "401007001";
$XunLuo_1_3[$XunLuo_1_3++] = "401007002";
$XunLuo_1_3[$XunLuo_1_3++] = "401007003";
//���������������ɻ���(��)����������������������������������������
//10��-15��
$XunLuo_1_4[$XunLuo_1_4++] = "400001001";
$XunLuo_1_4[$XunLuo_1_4++] = "400001002";
$XunLuo_1_4[$XunLuo_1_4++] = "400001003";
$XunLuo_1_4[$XunLuo_1_4++] = "400001004";
$XunLuo_1_4[$XunLuo_1_4++] = "400001005";
//�����������������Ľ�(��)����������������������������������������
//10��-15��
$XunLuo_1_5[$XunLuo_1_5++] = "410507001";
$XunLuo_1_5[$XunLuo_1_5++] = "410506001";
$XunLuo_1_5[$XunLuo_1_5++] = "401009001";
$XunLuo_1_5[$XunLuo_1_5++] = "401009002";
$XunLuo_1_5[$XunLuo_1_5++] = "401009003";
//��������������ɽ����(��)����������������������������������������
//10��-15��
$XunLuo_1_6[$XunLuo_1_6++] = "400001001";
$XunLuo_1_6[$XunLuo_1_6++] = "400001002";
$XunLuo_1_6[$XunLuo_1_6++] = "400001003";
$XunLuo_1_6[$XunLuo_1_6++] = "400001004";
$XunLuo_1_6[$XunLuo_1_6++] = "400001005";
//�����������������鹬(��)����������������������������������������
//10��-15��
$XunLuo_1_7[$XunLuo_1_7++] = "400001001";
$XunLuo_1_7[$XunLuo_1_7++] = "400001002";
$XunLuo_1_7[$XunLuo_1_7++] = "400001003";
$XunLuo_1_7[$XunLuo_1_7++] = "400001004";
$XunLuo_1_7[$XunLuo_1_7++] = "400001005";
//����������������ħ��(ħ)����������������������������������������
//10��-15��
$XunLuo_1_8[$XunLuo_1_8++] = "400001001";
$XunLuo_1_8[$XunLuo_1_8++] = "400001002";
$XunLuo_1_8[$XunLuo_1_8++] = "400001003";
$XunLuo_1_8[$XunLuo_1_8++] = "400001004";
$XunLuo_1_8[$XunLuo_1_8++] = "400001005";

//��������������������ͨ�á���������������������������������������
//30��-35��
$XunLuo_2[$XunLuo_2++] = "400001001";
$XunLuo_2[$XunLuo_2++] = "400001002";
$XunLuo_2[$XunLuo_2++] = "400002101";
$XunLuo_2[$XunLuo_2++] = "400002102";
$XunLuo_2[$XunLuo_2++] = "400002103";
                                       
//36��-49��                            
$XunLuo_3[$XunLuo_3++] = "401102101";
$XunLuo_3[$XunLuo_3++] = "401102103";
$XunLuo_3[$XunLuo_3++] = "401102106";
$XunLuo_3[$XunLuo_3++] = "401102107";
$XunLuo_3[$XunLuo_3++] = "401102108";
                                       
//50��-55��                            
$XunLuo_4[$XunLuo_4++] = "401103008";
$XunLuo_4[$XunLuo_4++] = "401103009";
$XunLuo_4[$XunLuo_4++] = "401103013";
$XunLuo_4[$XunLuo_4++] = "401103014";
$XunLuo_4[$XunLuo_4++] = "401103015";
                                       

//��������������������������������������������������������������������

//�������������������ڣ�ʥ������������������������������������������
//10��-15��
$PeiYu_1_1[$PeiYu_1_1++] = "108020054";
$PeiYu_1_1[$PeiYu_1_1++] = "108020055";
$PeiYu_1_1[$PeiYu_1_1++] = "108020061";
//��������������������(��)����������������������������������������
//10��-15��
$PeiYu_1_2[$PeiYu_1_2++] = "108020054";
$PeiYu_1_2[$PeiYu_1_2++] = "108020055";
$PeiYu_1_2[$PeiYu_1_2++] = "108020061";
//��������������������(��)����������������������������������������
//10��-15��
$PeiYu_1_3[$PeiYu_1_3++] = "108020054";
$PeiYu_1_3[$PeiYu_1_3++] = "108020055";
$PeiYu_1_3[$PeiYu_1_3++] = "108020061";
//���������������ɻ���(��)����������������������������������������
//10��-15��
$PeiYu_1_4[$PeiYu_1_4++] = "108020054";
$PeiYu_1_4[$PeiYu_1_4++] = "108020055";
$PeiYu_1_4[$PeiYu_1_4++] = "108020061";
//�����������������Ľ�(��)����������������������������������������
//10��-15��
$PeiYu_1_5[$PeiYu_1_5++] = "108020054";
$PeiYu_1_5[$PeiYu_1_5++] = "108020055";
$PeiYu_1_5[$PeiYu_1_5++] = "108020061";
//��������������ɽ����(��)����������������������������������������
//10��-15��
$PeiYu_1_6[$PeiYu_1_6++] = "108020054";
$PeiYu_1_6[$PeiYu_1_6++] = "108020055";
$PeiYu_1_6[$PeiYu_1_6++] = "108020061";

//�����������������鹬(��)����������������������������������������
//10��-15��
$PeiYu_1_7[$PeiYu_1_7++] = "108020054";
$PeiYu_1_7[$PeiYu_1_7++] = "108020055";
$PeiYu_1_7[$PeiYu_1_7++] = "108020061";

//����������������ħ��(ħ)����������������������������������������
//10��-15��
$PeiYu_1_8[$PeiYu_1_8++] = "108020054";
$PeiYu_1_8[$PeiYu_1_8++] = "108020055";
$PeiYu_1_8[$PeiYu_1_8++] = "108020061";

//��������������������ͨ�á���������������������������������������
//20��-29��                          
$PeiYu_3[$PeiYu_3++] = "108020083";
$PeiYu_3[$PeiYu_3++] = "108020128";
$PeiYu_3[$PeiYu_3++] = "108020129";
$PeiYu_3[$PeiYu_3++] = "108020134";
                    
//30��-35��                          
$PeiYu_4[$PeiYu_4++] = "108020099";
$PeiYu_4[$PeiYu_4++] = "108020110";
$PeiYu_4[$PeiYu_4++] = "108020054";
$PeiYu_4[$PeiYu_4++] = "108020055";
                         
//36��-49��                          
$PeiYu_5[$PeiYu_5++] = "108020151";
$PeiYu_5[$PeiYu_5++] = "108020152";
$PeiYu_5[$PeiYu_5++] = "108020166";
$PeiYu_5[$PeiYu_5++] = "108020207";
            
//50��-55��                          
$PeiYu_6[$PeiYu_6++] = "108020183";
$PeiYu_6[$PeiYu_6++] = "108020189";
$PeiYu_6[$PeiYu_6++] = "108020191";

//�����������������������������������������������������������������
//10��-15��
$GouWu_1[$GouWu_1++] = "105020101";
//16��-19�� 
$GouWu_2[$GouWu_2++] = "105020102";
//20��-29��
$GouWu_3[$GouWu_3++] = "105020104";
//30��-35�� 
$GouWu_4[$GouWu_4++] = "105020105";
//36��-49�� 
$GouWu_5[$GouWu_5++] = "105020106";
//50��-55��  
$GouWu_6[$GouWu_6++] = "105020107";

//�����������������������������������������������������������������

//����������������ɱ������������������������������������������������
//��������������������ͨ�á���������������������������������������
//30��-35��
$JiSha_1[$JiSha_1++] = "411101012";
$JiSha_1[$JiSha_1++] = "411101014";
$JiSha_1[$JiSha_1++] = "410701034";
$JiSha_1[$JiSha_1++] = "411101015";
$JiSha_1[$JiSha_1++] = "410701035";
$JiSha_1[$JiSha_1++] = "410701036";
$JiSha_1[$JiSha_1++] = "410400001";
$JiSha_1[$JiSha_1++] = "410400003";
$JiSha_1[$JiSha_1++] = "410200001";
$JiSha_1[$JiSha_1++] = "410400007";
$JiSha_1[$JiSha_1++] = "410400009";
$JiSha_1[$JiSha_1++] = "410600001";
$JiSha_1[$JiSha_1++] = "410400013";
$JiSha_1[$JiSha_1++] = "410600003";

//36��-49��
$JiSha_2[$JiSha_2++] = "411101012";
$JiSha_2[$JiSha_2++] = "411101014";
$JiSha_2[$JiSha_2++] = "410701034";
$JiSha_2[$JiSha_2++] = "411101015";
$JiSha_2[$JiSha_2++] = "410701035";
$JiSha_2[$JiSha_2++] = "410701036";
$JiSha_2[$JiSha_2++] = "410400001";
$JiSha_2[$JiSha_2++] = "410400003";
$JiSha_2[$JiSha_2++] = "410200001";
$JiSha_2[$JiSha_2++] = "410400007";
$JiSha_2[$JiSha_2++] = "410400009";
$JiSha_2[$JiSha_2++] = "410600001";
$JiSha_2[$JiSha_2++] = "410400013";
$JiSha_2[$JiSha_2++] = "410600003";
$JiSha_2[$JiSha_2++] = "410400021"; 
$JiSha_2[$JiSha_2++] = "410400023"; 
$JiSha_2[$JiSha_2++] = "410701043"; 
$JiSha_2[$JiSha_2++] = "410400027"; 
$JiSha_2[$JiSha_2++] = "410400033"; 
$JiSha_2[$JiSha_2++] = "410400035"; 
$JiSha_2[$JiSha_2++] = "410400037"; 
$JiSha_2[$JiSha_2++] = "410400039"; 
$JiSha_2[$JiSha_2++] = "410400041"; 
$JiSha_2[$JiSha_2++] = "410400047"; 
$JiSha_2[$JiSha_2++] = "410701047"; 
$JiSha_2[$JiSha_2++] = "410600009"; 
$JiSha_2[$JiSha_2++] = "410600011"; 
$JiSha_2[$JiSha_2++] = "410400053"; 

//50��-55��
$JiSha_3[$JiSha_3++] = "410400021"; 
$JiSha_3[$JiSha_3++] = "410400023"; 
$JiSha_3[$JiSha_3++] = "410701043"; 
$JiSha_3[$JiSha_3++] = "410400027"; 
$JiSha_3[$JiSha_3++] = "410400033"; 
$JiSha_3[$JiSha_3++] = "410400035"; 
$JiSha_3[$JiSha_3++] = "410400037"; 
$JiSha_3[$JiSha_3++] = "410400039"; 
$JiSha_3[$JiSha_3++] = "410400041"; 
$JiSha_3[$JiSha_3++] = "410400047"; 
$JiSha_3[$JiSha_3++] = "410701047"; 
$JiSha_3[$JiSha_3++] = "410600009"; 
$JiSha_3[$JiSha_3++] = "410600011"; 
$JiSha_3[$JiSha_3++] = "410400053";

//�����������������ޡ�����������������������������������������������
//30��-35�� 
$BuShou_1[$BuShou_1++] = "108020273";
//36��-49�� 
$BuShou_2[$BuShou_2++] = "108020273";
//50��-55��
$BuShou_3[$BuShou_3++] = "108020273";

//�����������������ޡ�����������������������������������������������