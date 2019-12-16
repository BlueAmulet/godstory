//�챦����
function Mission20002(%Npc, %Player, %State, %Conv)
{
		%Mid = "20002";										 //������
		%PlayerID = %Player.GetPlayerID(); //��ȡ��ҵı��
		%LV = %Player.GetLevel();          //��ҵȼ�               

		if(%State ==0)
			{
				if((%Npc.GetDataID() $= "401002118")||(%Npc.GetDataID() $= "400001118"))//Ѱ��
					{		
							%Conv.AddOption(700 @ %Mid, 700);//��ʾ�ر�ͼѡ��  						  
					}
			}
				
   if(%State ==700)
   	 {
   	 		%Conv.SetText(701 @ %Mid);      //��ʾ����Ի�
   	 		%Conv.AddOption(702 @ %Mid, 702);//��ʾ�ر�ͼѡ�� 
   	 		%Conv.AddOption(1,0);          //�����Ի�
   	 }
   	 
   if(%State ==702)
		 {
		 	 	if((%LV >= 30)&&(%LV < 40)){%ZHS = 10;}
		    if((%LV >= 40)&&(%LV < 50)){%ZHS = 20;}
		    if(%LV > 50){%ZHS = 50;}
		    
		    if(%Player.GetMissionFlag(%Mid,400)!=0){%Conv.SetText( 501 @ %Mid);}
			  else if(%Player.GetLevel() < 30 ){%Conv.SetText(502 @ %Mid);}
			  else if(%Player.GetAcceptedMission() >= 20){%Conv.SetText(503 @ %Mid);}
			  else if(%Player.GetCycleMissionTimes(%Mid) > %ZHS ){%Conv.SetText(504 @ %Mid);}
			  else if(%Player.GetMoney(1) > 200){%Conv.SetText(504 @ %Mid);}
			  else return Mission20002_LeiXing(%Npc, %Player, %State, %Conv);	//������������
			  				
			}
}


function Mission20002_LeiXing(%Npc, %Player, %State, %Conv)
{
			%ItemAdd = %Player.PutItem(108020260, 1); //���η�
			%ItemAdd = %Player.AddItem();
			
			if(!%ItemAdd)
				{
					SendOneChatMessage(0,"<t>��������</t>",%Player);
					SendOneScreenMessage(2,"��������", %Player);
					return;
				}
			 %Player.Reducemoney(200,1); //���ÿ۳����		
			 %Player.SetCycleMission(%Mid, %Zs+1);            //������������
			 %Zd = %Player.GetCycleMissionTimes(%Mid); 
			 %Player.SetMissionFlag(%Mid,9100,%Zd);           //���������������	
						
			 %Conv.SetType(4);	
}
			   
			   
function Trigger_108020260(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)//���η�
{
	%Mid = "20002";											               //������
	%PlayerID = %Player.GetPlayerID();                 //��ȡ��ҵı��
	%Zb1 =%Player.GetMissionFlag(%Mid,8200);           //��ȡ1�������     
	%Zb2 =%Player.GetMissionFlag(%Mid,8300);           //��ȡ2�������
	 
	if((%Zb1 == 0)&&(%Zb2 ==0)) 
		{		
			%Cbt = GetRandom(1,4);
										
			if(%Cbt ==1){eval("%HideMapData = $QuYuDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==2){eval("%HideMapData = $QuYuDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==3){eval("%HideMapData = $QuYuDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==4){eval("%HideMapData = $QuYuDiTu"@ "[%Cbt]" @ ";");}

			echo("%Cbt="@%Cbt);	
			echo("%HideMapData="@%HideMapData);
										
			if(%HideMapData $="811010100")
				{
					%Zb = GetRandom(1,3);
					if(%Zb ==1){eval("%ZuoBiao = $QuYuZuoBiao_QFB"@ "[%Zb]" @ ";");}
					if(%Zb ==2){eval("%ZuoBiao = $QuYuZuoBiao_QFB"@ "[%Zb]" @ ";");}
					if(%Zb ==3){eval("%ZuoBiao = $QuYuZuoBiao_QFB"@ "[%Zb]" @ ";");}							
				}	
			if(%HideMapData $="811020100")
				{
					%Zb = GetRandom(1,3);	  		
					if(%Zb ==1){eval("%ZuoBiao = $QuYuZuoBiao_XYL"@"[%Zb]" @ ";");}
					if(%Zb ==2){eval("%ZuoBiao = $QuYuZuoBiao_XYL"@"[%Zb]" @ ";");}
					if(%Zb ==3){eval("%ZuoBiao = $QuYuZuoBiao_XYL"@"[%Zb]" @ ";");}
													
				}
			if(%HideMapData $="811030100")
				{
					%Zb = GetRandom(1,3);
					if(%Zb ==1){eval("%ZuoBiao = $QuYuZuoBiao_WYCY"@"[%Zb]" @ ";");}
					if(%Zb ==2){eval("%ZuoBiao = $QuYuZuoBiao_WYCY"@"[%Zb]" @ ";");}
					if(%Zb ==3){eval("%ZuoBiao = $QuYuZuoBiao_WYCY"@"[%Zb]" @ ";");}
				}
			if(%HideMapData $="811040100")
				{
					%Zb = GetRandom(1,3);		
					if(%Zb ==1){eval("%ZuoBiao = $QuYuZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
					if(%Zb ==2){eval("%ZuoBiao = $QuYuZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
					if(%Zb ==3){eval("%ZuoBiao = $QuYuZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
				}
				
				echo("%Zb="@%Zb);	
				echo("%ZuoBiao="@%ZuoBiao);	
					
			  %Map_ZuoBiao2 = GetWord(%ZuoBiao,0);             //����2D�����һλ		 
			  %Map_ZuoBiao3 = GetWord(%ZuoBiao,1);             //����2D����ڶ�λ
			   
		  	echo("%Map_ZuoBiao2="@%Map_ZuoBiao2);
			  echo("%Map_ZuoBiao3="@%Map_ZuoBiao3); 
			  
			  %Player.SetMissionFlag(%Mid,8100,%HideMapData);   //�����ͼ������
			  %Player.SetMissionFlag(%Mid,8200,%Map_ZuoBiao2);  //���ֵ���������
			  %Player.SetMissionFlag(%Mid,8300,%Map_ZuoBiao3);  //���ֵ���������
			  
			  echo("����="@%Player.GetMissionFlag(%Mid,8100));
	      echo("����1="@%Player.GetMissionFlag(%Mid,8200));
	      echo("����2="@%Player.GetMissionFlag(%Mid,8300));

			  %Player.UpdateMission(%Mid);					           //����������Ϣ
				%Player.AddPacket( $SP_Effect[ 1 ] );	           //���ͽ���������Ч
				%Conv.SetText(500 @ %Mid);	                     //��ʾ������Ի�
				%Conv.AddOption(1,710);                          //ѡ������Ի�
				
		 }
		 		
  if((%Zb1 != 0)&&(%Zb2 != 0))                               //�ж��Ƿ�������
    { 
    	  %QuYu = %Player.GetMissionFlag(%Mid,8100);           //��ȡ��ͼ������
    	  %Map_ZuoBiao2 = %Player.GetMissionFlag(%Mid,8200);   //��ȡ��ͼ������
    	  %Map_ZuoBiao3 = %Player.GetMissionFlag(%Mid,8300);   //��ȡ��ͼ������
    	  
    		%WJZB = %Player.GetPosition();			         //��ȡ��ҵ�ǰ����
				%WJZB_1 = GetWord(%WJZB,0);   			         //��ҵ�ǰ�����һλ
				%WJZB_2 = GetWord(%WJZB,1);  				         //��ҵ�ǰ����ڶ�λ
				echo("%QuYu="@%QuYu);
				echo("%Map_ZuoBiao2="@%Map_ZuoBiao2);
				echo("%Map_ZuoBiao3="@%Map_ZuoBiao3);
				echo("%WJZB="@%WJZB);	
				echo("%WJZB_1="@%WJZB_1);
				echo("%WJZB_2="@%WJZB_2);
				
			if($Player_Area[%PlayerID] $= %QuYu) //�ж������Ƿ���ȷ
				{
					echo("$Player_Area[%PlayerID] = "@$Player_Area[%PlayerID]);
					
					if(((%WJZB_1 <= %Map_ZuoBiao2 + 10)&&(%WJZB_1 >= %Map_ZuoBiao2 - 10))||((%WJZB_2 <= %Map_ZuoBiao3 + 10)&&(%WJZB_2 >= %Map_ZuoBiao3 - 10))) //�ж���������
					  {
							if($New_108020208 ==0) //�ж��Ƿ��Ѿ�ˢ����
								{
									
									%Player.DelMission(%Mid);
									
									%ItemAdd = %Player.PutItem(108020260, -1); //�۳����η�
									%ItemAdd = %Player.AddItem();

									$New_108020208 = SpNewNpc(%Player,410000013,0,0); //ˢ�¹�����������
									Schedule(60000, 0, "RemoveNpc_108020208");        //����һ��ʱ�䴥��ĳ����
								}	
						} 
				}
				
		}
		
}

function RemoveNpc_108020208()
{
	$New_108020208.SafeDeleteObject();
	$New_108020208= 0;
}

function Trigger_108020261(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)//�ر�ͼ
{
	%Mid = "20003";											               //������
	%PlayerID = %Player.GetPlayerID();                 //��ȡ��ҵı��
	%WaBaoZb1 =%Player.GetMissionFlag(%Mid,8500);      //��ȡ1�������     
	%WaBaoZb2 =%Player.GetMissionFlag(%Mid,8600);      //��ȡ2�������
	 
	if((%WaBaoZb1 == 0)&&(%WaBaoZb2 ==0)) 
		{		
			%ItemAdd = %Player.PutItem(108020261, -1); //�۳��ر�ͼ
			%ItemAdd = %Player.AddItem();
			
			%ItemAdd = %Player.PutItem(108020262, 1);  //�����Ѵ򿪲ر�ͼ
			%ItemAdd = %Player.AddItem();

			if(!%ItemAdd)
				{
					SendOneChatMessage(0,"<t>��������</t>",%Player);
					SendOneScreenMessage(2,"��������", %Player);
					return;
				}
			
			%Cbt = GetRandom(1,4);
										
			if(%Cbt ==1){eval("%HideMapData = $WaBaoDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==2){eval("%HideMapData = $WaBaoDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==3){eval("%HideMapData = $WaBaoDiTu"@ "[%Cbt]" @ ";");}
			if(%Cbt ==4){eval("%HideMapData = $WaBaoDiTu"@ "[%Cbt]" @ ";");}

			if(%HideMapData $="811010100")
				{
					%Zb = GetRandom(1,3);
					if(%Zb ==1){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_QFB"@ "[%Zb]" @ ";");}
					if(%Zb ==2){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_QFB"@ "[%Zb]" @ ";");}
					if(%Zb ==3){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_QFB"@ "[%Zb]" @ ";");}							
				}	
			if(%HideMapData $="811020100")
				{
					%Zb = GetRandom(1,3);	  		
					if(%Zb ==1){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_XYL"@"[%Zb]" @ ";");}
					if(%Zb ==2){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_XYL"@"[%Zb]" @ ";");}
					if(%Zb ==3){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_XYL"@"[%Zb]" @ ";");}
													
				}
			if(%HideMapData $="811030100")
				{
					%Zb = GetRandom(1,3);
					if(%Zb ==1){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_WYCY"@"[%Zb]" @ ";");}
					if(%Zb ==2){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_WYCY"@"[%Zb]" @ ";");}
					if(%Zb ==3){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_WYCY"@"[%Zb]" @ ";");}
				}
			if(%HideMapData $="811040100")
				{
					%Zb = GetRandom(1,3);		
					if(%Zb ==1){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
					if(%Zb ==2){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
					if(%Zb ==3){eval("%WaBaoZuoBiao = $WaBaoZuoBiao_ZYSL"@ "[%Zb]" @ ";");}
				}
				
			  %WaBao_ZuoBiao2 = GetWord(%WaBaoZuoBiao,0);          //����2D�����һλ		 
			  %WaBao_ZuoBiao3 = GetWord(%WaBaoZuoBiao,1);          //����2D����ڶ�λ
			   	  
			  %Player.SetMissionFlag(%Mid,8400,%HideMapData);      //�����ͼ������
			  %Player.SetMissionFlag(%Mid,8500,%WaBao_ZuoBiao2);   //���ֵ���������
			  %Player.SetMissionFlag(%Mid,8600,%WaBao_ZuoBiao3);   //���ֵ���������
		      		
			  %Player.UpdateMission(%Mid);					               //����������Ϣ
				%Player.AddPacket( $SP_Effect[ 1 ] );	               //���ͽ���������Ч
				%Conv.SetText(600 @ %Mid);	                         //��ʾ������Ի�
				%Conv.AddOption(1,710);                              //ѡ������Ի�
			
		 }
}

function Trigger_108020262(%Conv, %Player, %Type, %State, %TriggerType, %TriggerID)//�Ѵ򿪲ر�ͼ
{
	 %Mid = "20003";										                   //������
	 %PlayerID = %Player.GetPlayerID();                    //��ȡ��ҵı��
	 %WaBaoQuYu = %Player.GetMissionFlag(%Mid,8400);       //��ȡ��ͼ������
   %WaBaoZuoBiao2 = %Player.GetMissionFlag(%Mid,8500);   //��ȡ��ͼ������
   %WaBaoZuoBiao3 = %Player.GetMissionFlag(%Mid,8600);   //��ȡ��ͼ������   	  
   %WJZB = %Player.GetPosition();			                   //��ȡ��ҵ�ǰ����
	 %WJZB_1 = GetWord(%WJZB,0);   			                   //��ҵ�ǰ�����һλ
	 %WJZB_2 = GetWord(%WJZB,1);  				                 //��ҵ�ǰ����ڶ�λ
	
	if($Player_Area[%PlayerID] $= %WaBaoQuYu) 						 //�ж������Ƿ���ȷ
		{
			echo("$Player_Area[%PlayerID] = "@$Player_Area[%PlayerID]);
					
			if(((%WJZB_1 <= %WaBaoZuoBiao2 + 10)&&(%WJZB_1 >= %WaBaoZuoBiao2 - 10))||((%WJZB_2 <= %WaBaoZuoBiao3 + 10)&&(%WJZB_2 >= %WaBaoZuoBiao3 - 10))) //�ж���������
				{
						%Player.DelMission(%Mid); //ɾ������
						%ItemAdd = %Player.PutItem(108020262, -1); //�۳��Ѵ򿪲ر�ͼ
						%ItemAdd = %Player.AddItem();
						
						%ChuFa = GetRandom(1,5); //��������¼�
						
						if(%ChuFa ==1) //�����������Ǯ
							{
								eval("%ChuFaData = $WaBao_Money" @ ";"); //ȷ���������
								%SJ_Money = GetRandom(1,%ChuFaData);  //��������е�ֵ
								eval("%MoneyID = $WaBao_Money" @ "[%SJ_Money]"@ ";");
								
								Player.Addmoney(%MoneyID,2);
							}
						if(%ChuFa ==2) //������ˢ�¹���
							{
									eval("%ChuFaData = $WaBao_Fight" @ ";");
									%SJ_Fight = GetRandom(1,%ChuFaData);  //��������е�ֵ
									eval("%FightID = $WaBao_Fight" @ "[%SJ_Fight]"@ ";");
									
									if($New_108020262 ==0) //�ж��Ƿ��Ѿ�ˢ����
										{
											$New_108020262 = SpNewNpc(%Player,%FightID,0,0); //ˢ�¹�����������
											Schedule(60000, 0, "RemoveNpc_108020262");        //����һ��ʱ�䴥��ĳ����
										}	
								
							}
						if(%ChuFa ==3) //������������Ʒ
							{
								eval("%ChuFaData = $WaBao_Item" @ ";");
								%SJ_Item = GetRandom(1,%ChuFaData);  //��������е�ֵ
								eval("%ItemID = $WaBao_Item" @ "[%SJ_Item]"@ ";");
								
								%ItemAdd = %Player.PutItem(%ItemID, 1); 
								%ItemAdd = %Player.AddItem();
										
								if(!%ItemAdd)
								  {
										SendOneChatMessage(0,"<t>��������</t>",%Player);
										SendOneScreenMessage(2,"��������", %Player);
									  return;
									}
								
							}
						if(%ChuFa ==4) //�������
							{
								eval("%ChuFaData = $WaBao_XianJing" @ ";");
								%SJ_XianJing = GetRandom(1,%ChuFaData);  //��������е�ֵ
								eval("%XianJingID = $WaBao_XianJing" @ "[%SJ_XianJing]"@ ";");
								
								%Player.AddBuff(%XianJingID);
								
							}
						if(%ChuFa ==5) //������������
							{
								eval("%ChuFaData = $WaBao_ChuanSong" @ ";");
								%SJ_ChuanSong = GetRandom(1,%ChuFaData);  //��������е�ֵ
								eval("%ChuanSongID = $WaBao_ChuanSong" @ "[%SJ_ChuanSong]"@ ";");
								
							}

				  } 
		}
	
}

function RemoveNpc_108020262()
{
	$New_108020262.SafeDeleteObject();
	$New_108020262= 0;
}

//��������������Ѱ�١���������������������������������������
//��ͼ����
$QuYuDiTu[$QuYuDiTu++] = "811010100";
$QuYuDiTu[$QuYuDiTu++] = "811020100";
$QuYuDiTu[$QuYuDiTu++] = "811030100";
$QuYuDiTu[$QuYuDiTu++] = "811040100";

//ָ����ͼ����

//����
$QuYuZuoBiao_QFB[$QuYuZuoBiao_QFB++] = "-201.459 -182.045 39.4386";
$QuYuZuoBiao_QFB[$QuYuZuoBiao_QFB++] = "-194.866 -403.906 4.24695";
$QuYuZuoBiao_QFB[$QuYuZuoBiao_QFB++] = "-336.401 -164.23 16.5954";
//������
$QuYuZuoBiao_XYL[$QuYuZuoBiao_XYL++] = "130.507 -124.276 94.9672";
$QuYuZuoBiao_XYL[$QuYuZuoBiao_XYL++] = "140.095 -0.899079 73.4717";
$QuYuZuoBiao_XYL[$QuYuZuoBiao_XYL++] = "-66.9969 31.8353 72.3157";
//���ǲ�Ұ
$QuYuZuoBiao_WYCY[$QuYuZuoBiao_WYCY++] = "67.7467 -94.3243 73.1447";
$QuYuZuoBiao_WYCY[$QuYuZuoBiao_WYCY++] = "99.0152 -13.9025 70.5331";
$QuYuZuoBiao_WYCY[$QuYuZuoBiao_WYCY++] = "-84.6463 -28.8356 80.3844";
//����ɭ��
$QuYuZuoBiao_ZYSL[$QuYuZuoBiao_ZYSL++] = "-83.7572 -94.8396 114.752";
$QuYuZuoBiao_ZYSL[$QuYuZuoBiao_ZYSL++] = "143.008 -54.2059 109.631";
$QuYuZuoBiao_ZYSL[$QuYuZuoBiao_ZYSL++] = "-28.0793 52.1808 122.411";
//��������������Ѱ�١���������������������������������������

//���������������ڱ�����������������������������������������
//��ͼ����
$WaBaoDiTu[$WaBaoDiTu++] = "811010100";
$WaBaoDiTu[$WaBaoDiTu++] = "811020100";
$WaBaoDiTu[$WaBaoDiTu++] = "811030100";
$WaBaoDiTu[$WaBaoDiTu++] = "811040100";

//ָ����ͼ����

//����
$WaBaoZuoBiao_QFB[$WaBaoZuoBiao_QFB++] = "-254.13 -442.207 3.57358";
$WaBaoZuoBiao_QFB[$WaBaoZuoBiao_QFB++] = "-166.435 -320.94 24.9156";
$WaBaoZuoBiao_QFB[$WaBaoZuoBiao_QFB++] = "-215.88 -180.382 40.2003";
//������
$WaBaoZuoBiao_XYL[$WaBaoZuoBiao_XYL++] = "23.1289 63.0308 73.2148";
$WaBaoZuoBiao_XYL[$WaBaoZuoBiao_XYL++] = "-25.5479 -37.3814 73.0522";
$WaBaoZuoBiao_XYL[$WaBaoZuoBiao_XYL++] = "116.454 39.2686 71.9175";
//���ǲ�Ұ
$WaBaoZuoBiao_WYCY[$WaBaoZuoBiao_WYCY++] = "118.946 -5.22342 74.4776";
$WaBaoZuoBiao_WYCY[$WaBaoZuoBiao_WYCY++] = "-91.5556 -103.62 70.5847";
$WaBaoZuoBiao_WYCY[$WaBaoZuoBiao_WYCY++] = "-37.8235 -93.827 71.739";
//����ɭ��
$WaBaoZuoBiao_ZYSL[$WaBaoZuoBiao_ZYSL++] = "45.4196 -47.0228 118.997";
$WaBaoZuoBiao_ZYSL[$WaBaoZuoBiao_ZYSL++] = "-54.7147 8.18625 111";
$WaBaoZuoBiao_ZYSL[$WaBaoZuoBiao_ZYSL++] = "9.46716 -129.449 120.552";

//���������������ڱ�����������������������������������������

//���������������ڱ������¼�����������������������������������������
//��Ǯ
$WaBao_Money[$WaBao_Money++] ="100";
$WaBao_Money[$WaBao_Money++] ="200";
$WaBao_Money[$WaBao_Money++] ="300";
$WaBao_Money[$WaBao_Money++] ="400";
$WaBao_Money[$WaBao_Money++] ="500";
$WaBao_Money[$WaBao_Money++] ="600";
$WaBao_Money[$WaBao_Money++] ="700";
$WaBao_Money[$WaBao_Money++] ="800";
$WaBao_Money[$WaBao_Money++] ="900";
$WaBao_Money[$WaBao_Money++] ="1000";

//����
$WaBao_Fight[$WaBao_Fight++] ="410701001";
$WaBao_Fight[$WaBao_Fight++] ="410701002";
$WaBao_Fight[$WaBao_Fight++] ="410701003";

//��Ʒ
$WaBao_Item[$WaBao_Item++] ="105010101";
$WaBao_Item[$WaBao_Item++] ="105010102";
$WaBao_Item[$WaBao_Item++] ="105010103";
$WaBao_Item[$WaBao_Item++] ="105010104";
$WaBao_Item[$WaBao_Item++] ="105010105";
$WaBao_Item[$WaBao_Item++] ="105010106";
$WaBao_Item[$WaBao_Item++] ="105010107";
$WaBao_Item[$WaBao_Item++] ="105010108";
$WaBao_Item[$WaBao_Item++] ="105010109";
$WaBao_Item[$WaBao_Item++] ="105010110";

//BOSS

//����

//����
$WaBao_XianJing[$WaBao_XianJing++] ="307020020";

//����
$WaBao_ChuanSong[$WaBao_ChuanSong++] ="45.4196 -47.0228 118.997";  
$WaBao_ChuanSong[$WaBao_ChuanSong++] ="-54.7147 8.18625 111";      
$WaBao_ChuanSong[$WaBao_ChuanSong++] ="9.46716 -129.449 120.552";  
$WaBao_ChuanSong[$WaBao_ChuanSong++] ="-91.5556 -103.62 70.5847";
$WaBao_ChuanSong[$WaBao_ChuanSong++] ="-37.8235 -93.827 71.739"; 
                                      
//���������������ڱ������¼�����������������������������������������