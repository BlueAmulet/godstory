//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//���������ű����������������������ű�
//
//==================================================================================


function Mission_Special(%Npc, %Player, %State, %Conv, %Param)
{
	
	if(%Npc.GetDataID() $= "400001122"){Mission_Special_10103(%Npc, %Player, %State, %Conv, %Param);}	//����̨	
	if(%Npc.GetDataID() $= "400001102"){Mission_Special_10109(%Npc, %Player, %State, %Conv, %Param);}	//Ů洱���
//	if(%Npc.GetDataID() $= "401303101"){Mission_Special_10110(%Npc, %Player, %State, %Conv, %Param);}	//�����ӵĻ���

  if(%Npc.GetDataID() $= "400001007")
  	{
  		Mission_Special_10001(%Npc, %Player, %State, %Conv, %Param,10001);
  		Mission_Special_10112(%Npc, %Player, %State, %Conv, %Param,10112);
  	}//����
  	
  if(%Npc.GetDataID() $= "400001001")//��ʱ��
  	{
  		Mission_Special_10113(%Npc, %Player, %State, %Conv, %Param);
  		Mission_Special_10114(%Npc, %Player, %State, %Conv, %Param);
  	}
  if(%Npc.GetDataID() $= "400001060"){Mission_Special_10116(%Npc, %Player, %State, %Conv, %Param);}//����
   	
	if(%Npc.GetDataID() $= "400001101")//������
		{
			Mission_Special_10000(%Npc, %Player, %State, %Conv, %Param, 10000);//���������
			Mission_Special_10119(%Npc, %Player, %State, %Conv, %Param, 10119);//���������
			Mission_Special_10121(%Npc, %Player, %State, %Conv, %Param, 10121);//����������
			Mission_Special_10122(%Npc, %Player, %State, %Conv, %Param, 10122);//�˴�����
		}
  if(%Npc.GetDataID() $= "400001102"){Mission_Special_10123(%Npc, %Player, %State, %Conv, %Param,10123);}	//Ů�����
	if( (%Npc.GetDataID() $= "410107001")||(%Npc.GetDataID() $= "410207001")||(%Npc.GetDataID() $= "410307001")||(%Npc.GetDataID() $= "410407001")||
			(%Npc.GetDataID() $= "410507001")||(%Npc.GetDataID() $= "410607001")||(%Npc.GetDataID() $= "410707001")||(%Npc.GetDataID() $= "410807001") )
		{
			Mission_Special_10123(%Npc, %Player, %State, %Conv, %Param,10123);//����֮��
			Mission_Special_10123_2(%Npc, %Player, %State, %Conv, %Param, 10123);//����֮��
			Mission_Special_10124(%Npc, %Player, %State, %Conv, %Param, 10124);//�µ���ͨ
		}

	if( (%Npc.GetDataID() $= "401011002")||(%Npc.GetDataID() $= "401006002")||(%Npc.GetDataID() $= "401007002")||(%Npc.GetDataID() $= "401005002")||
			(%Npc.GetDataID() $= "401009002")||(%Npc.GetDataID() $= "401010002")||(%Npc.GetDataID() $= "401008002")||(%Npc.GetDataID() $= "401004002") )
		{
			Mission_Special_10124(%Npc, %Player, %State, %Conv, %Param, 10124);//�µ���ͨ
			Mission_Special_10125(%Npc, %Player, %State, %Conv, %Param, 10125);//��ľ
			Mission_Special_10126(%Npc, %Player, %State, %Conv, %Param, 10126);//�����������
		}

	if( (%Npc.GetDataID() $= "401011006")||(%Npc.GetDataID() $= "401006006")||(%Npc.GetDataID() $= "401007006")||(%Npc.GetDataID() $= "401005006")||
			(%Npc.GetDataID() $= "401009006")||(%Npc.GetDataID() $= "401010006")||(%Npc.GetDataID() $= "401008006")||(%Npc.GetDataID() $= "401004006") )
		{
			Mission_Special_10126(%Npc, %Player, %State, %Conv, %Param, 10126);//�����������
			Mission_Special_10127(%Npc, %Player, %State, %Conv, %Param, 10127);//����ͬ�ŵ���
		}

	if( (%Npc.GetDataID() $= "400001038")||(%Npc.GetDataID() $= "400001044")||(%Npc.GetDataID() $= "400001039")||(%Npc.GetDataID() $= "400001042")||
			(%Npc.GetDataID() $= "400001041")||(%Npc.GetDataID() $= "400001040")||(%Npc.GetDataID() $= "400001043")||(%Npc.GetDataID() $= "400001045") )
		{
			Mission_Special_10122(%Npc, %Player, %State, %Conv, %Param, 10122);//�˴�����
			Mission_Special_10127(%Npc, %Player, %State, %Conv, %Param, 10127);//����ͬ�ŵ���
			Mission_Special_10128(%Npc, %Player, %State, %Conv, %Param, 10128);//���۽�
		}
		
		
	if(%Npc.GetDataID() $= "400001059")
		{
			Mission_Special_10128(%Npc, %Player, %State, %Conv, %Param,10128);
			Mission_Special_10140(%Npc, %Player, %State, %Conv, %Param);
		}	//���۽�
	if(%Npc.GetDataID() $= "400002105"){Mission_Special_10153(%Npc, %Player, %State, %Conv, %Param);}	//������
	if(%Npc.GetDataID() $= "400002105"){Mission_Special_10161(%Npc, %Player, %State, %Conv, %Param);}	//������
	if(%Npc.GetDataID() $= "400001104"){Mission_Special_10156(%Npc, %Player, %State, %Conv, %Param);}	//�ξ���
	if(%Npc.GetDataID() $= "400002110"){Mission_Special_10186(%Npc, %Player, %State, %Conv, %Param);}	//���
	if(%Npc.GetDataID() $= "400002115"){Mission_Special_10233(%Npc, %Player, %State, %Conv, %Param);}	//������
	if(%Npc.GetDataID() $= "401102121"){Mission_Special_10303(%Npc, %Player, %State, %Conv, %Param);}	//�߶�һ
	if(%Npc.GetDataID() $= "401102122"){Mission_Special_10303(%Npc, %Player, %State, %Conv, %Param);}	//�߶���	
	if(%Npc.GetDataID() $= "401102123"){Mission_Special_10303(%Npc, %Player, %State, %Conv, %Param);}	//�߶���
  if(%Npc.GetDataID() $= "401102109"){Mission_Special_10305(%Npc, %Player, %State, %Conv, %Param);}	//���
  if(%Npc.GetDataID() $= "401102103"){Mission_Special_10316(%Npc, %Player, %State, %Conv, %Param);}	//�޴峤
  if(%Npc.GetDataID() $= "401102112"){Mission_Special_10320(%Npc, %Player, %State, %Conv, %Param);}	//Ϧ��
  if(%Npc.GetDataID() $= "401102115"){Mission_Special_10321(%Npc, %Player, %State, %Conv, %Param);}	//ħ��
  if(%Npc.GetDataID() $= "401103020"){Mission_Special_10527_1(%Npc, %Player, %State, %Conv, %Param);}	//����
  if(%Npc.GetDataID() $= "401103021"){Mission_Special_10527_2(%Npc, %Player, %State, %Conv, %Param);}	//����
  if(%Npc.GetDataID() $= "401103015"){Mission_Special_10527_3(%Npc, %Player, %State, %Conv, %Param);}	//���	
  if(%Npc.GetDataID() $= "401103017"){Mission_Special_10545(%Npc, %Player, %State, %Conv, %Param);}	//���
  if(%Npc.GetDataID() $= "401103019"){Mission_Special_10580(%Npc, %Player, %State, %Conv, %Param);}	//�Ͷ�
  if(%Npc.GetDataID() $= "401103014"){Mission_Special_10584(%Npc, %Player, %State, %Conv, %Param);}	//̴��	
  if(%Npc.GetDataID() $= "401305001"){Mission_Special_10585(%Npc, %Player, %State, %Conv, %Param);}	//̰���Ǿ���̴��
  if(%Npc.GetDataID() $= "401002106"){Mission_Special_10704(%Npc, %Player, %State, %Conv, %Param);}	//л�þ�	
  if(%Npc.GetDataID() $= "401104005"){Mission_Special_10743(%Npc, %Player, %State, %Conv, %Param);}	//�����
  if(%Npc.GetDataID() $= "401104010"){Mission_Special_10761(%Npc, %Player, %State, %Conv, %Param);}	//ɣѩ��
  if(%Npc.GetDataID() $= "401105003"){Mission_Special_10907(%Npc, %Player, %State, %Conv, %Param);}	//���˻���
  if(%Npc.GetDataID() $= "401105004"){Mission_Special_10908(%Npc, %Player, %State, %Conv, %Param);}	//�Ͷ���
  if(%Npc.GetDataID() $= "401105009"){Mission_Special_10915(%Npc, %Player, %State, %Conv, %Param);}	//ɣľ�ڴ�
								 		
}

function Mission_Special_10000(%Npc, %Player, %State, %Conv, %Param,%Mid)
{
	if(%State == 0)
		{
			if(%Player.IsAcceptedMission(%Mid))
				if(%Player.GetLevel() < 10)
						  %Conv.AddOption(900 @ %Mid, 120 @ %Mid);//��ʾ������ѡ��
		}
		
	if((%State > 0)&&(%Mid == 10000))
		{
			%MidState = GetSubStr(%State,0,3);
			
			if(%MidState == 120)
				{
				   %Conv.SetText(%Mid);	
				}
		}  
}

function Mission_Special_10001(%Npc, %Player, %State, %Conv, %Param,%Mid)
{
	%Mid = 10001;
	
	if(%State == 0)
		{
			if(%Player.IsFinishedMission(10112))
				if(!%Player.IsFinishedMission(%Mid))
					if(!%Player.IsAcceptedMission(%Mid))
						  %Conv.AddOption(100 @ %Mid, 120 @ %Mid);//��ʾ������ѡ��
						 
			if(%Player.IsAcceptedMission(%Mid))
				 if(!%Player.IsFinishedMission(%Mid))
		   		   %Conv.AddOption(900 @ %Mid, 920 @ %Mid);//��ʾ������ѡ��
		}
		
	if((%State > 0)&&(%Mid == 10001))
		{
			%MidState = GetSubStr(%State,0,3);
			%Pet = %Player.GetSpawnedPet();

			if(%MidState == 120)
				{
					if(%Pet.GetLevel() == 0)
						  %Conv.SetText(%Mid + 1);
					else 
						{
						  %Conv.SetText(201 @ %Mid);
							%Conv.AddOption(210 @ %Mid, 210 @ %Mid);//��������
							%Conv.AddOption(300 @ %Mid, 300 @ %Mid);//�ر�
						}
							
				}
			if(%MidState == 210)
				{
					AddMissionAccepted(%Player, %Mid);	//�������������Ϣ
					%Conv.SetType(4);
				}
				
			if(%MidState == 920)
				{
           if(%Pet.GetLevel() >= 10)
           	 { 
	              AddMissionReward(%Player, %Mid, %Param);	//����������
	              %Conv.SetType(4);
             }
           else
							%Conv.SetText(%Mid);
				}
		}  
}


function Mission_Special_10103(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10103;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			
			if(%State == 0)
				{
					if(%Player.GetMissionFlag(%Mid, 1350) == 0)		
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{	
							%Player.SetMissionFlag(%Mid, 1350, 1, true); //����,Ŀ�괥����ǰ����
						  %Conv.SetType(4);
					
						}			
				}
		}
}

function Mission_Special_10109(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10109;
	
	if(%Player.IsAcceptedMission(%Mid))
			{
				if(%State == 0)
					{
						%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//����Ů���������
						%Conv.AddOption(300 @ %Mid, -1 );	//�Ե�Ƭ�̣��һ�������Ƭ��
					}
	
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						
						if(%MidState == 200)
							{
								 if((%Player.GetMissionFlag(%Mid,3100) == 0)||(%Player.GetMissionFlag(%Mid,3101) == 0))
									 {  
	
									 	  %Player.SetMissionFlag(%Mid, 1350, 1, true); 				//����,Ŀ�괥����ǰ����
											%Player.SetMissionFlag(%Mid,3100,410700022);		//����,Ŀ����
										  %Player.SetMissionFlag(%Mid,3200,4);				//����,Ŀ�괥������
											%Player.SetMissionFlag(%Mid,3300,0); 				//����,Ŀ�괥����ǰ����
											%Player.SetMissionFlag(%Mid,3101,410700024);		//����,Ŀ����
											%Player.SetMissionFlag(%Mid,3201,1);				//����,Ŀ�괥������
										  %Player.SetMissionFlag(%Mid,3301,0); 				//����,Ŀ�괥����ǰ����
										  %Player.UpdateMission(%Mid);							 //����������Ϣ
									 }
		                  %Player.SetFlagsByte(25,0);
		                  %Player.SetFlagsByte(27,0);
		                  
											GoToNextMap_CopyMap(%Player, 1303);
											%Conv.SetType(4);
	
						  }	
					}
			}
}

function Mission_Special_10112(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	%Mid = 10112;
	
	if(%Player.IsAcceptedMission(%Mid))
		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) !$= "")
			{
				if(%State == 0)
					{
		   			%Conv.AddOption(900 @ %Mid, 920 @ %Mid);//��ʾ������ѡ��
		   		}
		
				if((%State > 0)&&(%Mid == 10112))
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 920)
							{
								%Conv.SetText(%Mid);
								%Conv.AddOption(200 @ %Mid, 200 @ %Mid);//����ȥ��Ȳ���
						    %Conv.AddOption(300 @ %Mid, -1);//��һ����ȥ����
							}
							
					 if(%MidState == 200)
							{
								GoToNextMap_Normal(%Player,112401);
								%Conv.SetType(4);
							}
					}  
			}
}

function Mission_Special_10113(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10113;
	
	if(%Player.IsAcceptedMission(%Mid))
		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) !$= "")
			{
				if(%State == 0)
					{
		   			%Conv.AddOption(900 @ %Mid, 920 @ %Mid);//��ʾ������ѡ��
		   		}
		
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 920)
							{
								%Conv.SetText(%Mid);
							}
					}  
			}
}

function Mission_Special_10114(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10114;
	
	if(%Player.IsAcceptedMission(%Mid))
		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) !$= "")
			{
				if(%State == 0)
					{
		   			%Conv.AddOption(900 @ %Mid, 920 @ %Mid);//��ʾ������ѡ��
		   		}
		
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 920)
							{
								%Conv.SetText(%Mid);
							}
					}  
			}
}

function Mission_Special_10116(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10116;
	
	if(%Player.IsAcceptedMission(%Mid))
		if(CanDoThisMission(%Player, %Mid, 2, 0, 0) !$= "")
			{
				
				if(%State == 0)
					{
		   			%Conv.AddOption(900 @ %Mid, 920 @ %Mid);//��ʾ������ѡ��
		   		}
		
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 920)
							{
								%Conv.SetText(%Mid);
							}
					}  
			}
}

function Mission_Special_10119(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//��δ�Ի�
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//��ʾ������;�Ի�ѡ��

			if((%State > 0)&&(%Mid == 10119))
				{
					%MidState = GetSubStr(%State,0,3);
					if( (%MidState >= 200)&&(%MidState <= 400) )
						{
							%Dlg = %MidState + GetRandom(1,3) * 10;

							%Conv.SetText(%Dlg @ %Mid);	//��ʾ����

							%Yes = GetRandom(1,2);
							if(%Yes == 1){%No = 2;%a = %MidState + 100;%b = 998;}
							if(%Yes == 2){%No = 1;%b = %MidState + 100;%a = 998;}

							%Conv.AddOption(%Dlg + %Yes @ %Mid , %a @ %Mid);//ѡ��1
							%Conv.AddOption(%Dlg + %No  @ %Mid , %b @ %Mid);//ѡ��2
						}
					if(%MidState == 500)
						{
							%Conv.SetText(%MidState @ %Mid);	//ȫ�������
							%Conv.AddOption(4 , 0 );		//����

							%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//����,Ŀ�괥����ǰ����
						}
					if(%MidState == 998)
						{
							%Conv.SetText(%MidState @ %Mid);	//�����
					    %Player.AddBuff(320110001);
							%Conv.AddOption(4 , 0 );		//����
						}
				}
		}
}

function Mission_Special_10121(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%Player.IsAcceptedMission(%Mid))
			{
				if(%State == 0)
					if((%Player.GetMissionFlag(%Mid, 3300) != 1)||(%Player.GetMissionFlag(%Mid, 3301) != 3)||(%Player.GetMissionFlag(%Mid, 2300) != 1))
					{
						%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//���Ѿ�׼����ȥ����¥��
						%Conv.AddOption(300 @ %Mid, -1 );	//�Ե�Ƭ�̣��һ�������Ƭ��
					}
	
				if((%State > 0)&&(%Mid == 10121))
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 200)
							{
								%Player.SetFlagsByte(26,0);	
								
								GoToNextMap_CopyMap(%Player, 1301);
								
								%Conv.SetType(4);
							}
					}
			}
}

function Mission_Special_10122(%Npc, %Player, %State, %Conv, %Param,%Mid)
{
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					if(%Npc.GetDataID() $= "400001042"){%Conv.AddOption(200 @ %Mid, 201 @ %Mid);}//�����ڽ�����
					if(%Npc.GetDataID() $= "400001041"){%Conv.AddOption(200 @ %Mid, 202 @ %Mid);}//�����ڽ�����
					if(%Npc.GetDataID() $= "400001039"){%Conv.AddOption(200 @ %Mid, 203 @ %Mid);}//�����ڽ�����
					if(%Npc.GetDataID() $= "400001038"){%Conv.AddOption(200 @ %Mid, 204 @ %Mid);}//ʥ���ڽ�����
					if(%Npc.GetDataID() $= "400001040"){%Conv.AddOption(200 @ %Mid, 205 @ %Mid);}//�����ڽ�����
					if(%Npc.GetDataID() $= "400001043"){%Conv.AddOption(200 @ %Mid, 206 @ %Mid);}//�����ڽ�����		
					if(%Npc.GetDataID() $= "400001044"){%Conv.AddOption(200 @ %Mid, 207 @ %Mid);}//�����ڽ�����
					if(%Npc.GetDataID() $= "400001045"){%Conv.AddOption(200 @ %Mid, 208 @ %Mid);}//ħ���ڽ�����
		
					if(%Npc.GetDataID() $= "400001101")
						if((%Player.GetMissionFlag(%Mid, 1350) ==1)&&(%Player.GetMissionFlag(%Mid, 1351) ==1)
							 &&(%Player.GetMissionFlag(%Mid, 1352) ==1)&&(%Player.GetMissionFlag(%Mid, 1353) ==1)
							 &&(%Player.GetMissionFlag(%Mid, 1354) ==1)&&(%Player.GetMissionFlag(%Mid, 1355) ==1)
							 &&(%Player.GetMissionFlag(%Mid, 1356) ==1)&&(%Player.GetMissionFlag(%Mid, 1357) ==1) )
						{%Conv.AddOption(209 @ %Mid, 209 @ %Mid);%Conv.AddOption(300 @ %Mid, 3400);}	
				}		
			if((%State > 0)&&(%Mid == 10122))
				{
					%MidState = GetSubStr(%State,0,3);

					if( (%MidState > 199)&&(%MidState < 209) )
						{
							if(%MidState == 201){%Conv.SetText(201 @ %Mid);%i=0;}
							if(%MidState == 202){%Conv.SetText(202 @ %Mid);%i=1;}
							if(%MidState == 203){%Conv.SetText(203 @ %Mid);%i=2;}
							if(%MidState == 204){%Conv.SetText(204 @ %Mid);%i=3;}
							if(%MidState == 205){%Conv.SetText(205 @ %Mid);%i=4;}
							if(%MidState == 206){%Conv.SetText(206 @ %Mid);%i=5;}
							if(%MidState == 207){%Conv.SetText(207 @ %Mid);%i=6;}
							if(%MidState == 208){%Conv.SetText(208 @ %Mid);%i=7;}
							
							%Player.SetMissionFlag(%Mid, 1350 + %i, 1, true); 				//����,Ŀ�괥����ǰ����
							%Player.UpdateMission(%Mid);							//����������Ϣ
							
						}
						
					if(%MidState ==209)
					  {
					    if(%Player.IsAcceptedMission(%Mid))
								 if(!%Player.IsFinishedMission(%Mid))
										{
											AddMissionReward(%Player, %Mid, %Param);	//����������
											NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//ִ��Npc��������Ľű�
										}
					  }	
					
				}	
		}						
														
}

function Mission_Special_10123(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					if(%Npc.GetDataID() $= "400001102")
						{
							if(%Player.GetSex() == 1)
								{
									%Conv.AddOption(201 @ %Mid, 201 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
									%Conv.AddOption(202 @ %Mid, 202 @ %Mid);//���������ڵ�ͼ"������(��)"��������
									%Conv.AddOption(203 @ %Mid, 203 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
									%Conv.AddOption(205 @ %Mid, 205 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
									%Conv.AddOption(206 @ %Mid, 206 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
									%Conv.AddOption(207 @ %Mid, 207 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
									%Conv.AddOption(208 @ %Mid, 208 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
							  }
							else
								{
									%Conv.AddOption(201 @ %Mid, 201 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
									%Conv.AddOption(203 @ %Mid, 203 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
									%Conv.AddOption(204 @ %Mid, 204 @ %Mid);//���������ڵ�ͼ"�ɻ���(��)"��ľ��
									%Conv.AddOption(205 @ %Mid, 205 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
									%Conv.AddOption(206 @ %Mid, 206 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
									%Conv.AddOption(207 @ %Mid, 207 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
									%Conv.AddOption(208 @ %Mid, 208 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
							  }
						}
					if(%Player.GetMissionFlag(%Mid, 1150) $= %Npc.GetDataID() )
						{
							%Player.SetMissionFlag(%Mid, 1350, 1); 				//����,Ŀ�괥����ǰ����
							%Player.UpdateMission(%Mid);							//����������Ϣ
						}
				}

			if((%State > 0)&&(%Mid == 10123))
				{
					%MidState = GetSubStr(%State,0,3);

					if( (%MidState > 200)&&(%MidState < 209) )
						{
							if(%MidState == 201){%NpcX = 410107001;%Map = 101101;}//"������(ʥ)"���ع���
							if(%MidState == 202){%NpcX = 410207001;%Map = 100601;}//"������(��)"��������
							if(%MidState == 203){%NpcX = 410307001;%Map = 100701;}//"������(��)"�����ɾ�
							if(%MidState == 204){%NpcX = 410407001;%Map = 100501;}//"�ɻ���(��)"��ľ��
							if(%MidState == 205){%NpcX = 410507001;%Map = 100901;}//"���Ľ�(��)"��ڤ����
							if(%MidState == 206){%NpcX = 410607001;%Map = 101001;}//"ɽ����(��)"˫��ɽ
							if(%MidState == 207){%NpcX = 410707001;%Map = 100801;}//"���鹬(��)"���α���
							if(%MidState == 208){%NpcX = 410807001;%Map = 100401;}//"��ħ��(ħ)"��ϦԨ

							%Conv.SetType(4);
							%Player.SetMissionFlag(%Mid, 1150, %NpcX);		//����,Ŀ����
							%Player.SetMissionFlag(%Mid, 1250, 1);				//����,Ŀ�괥������
							%Player.SetMissionFlag(%Mid, 1350, 0); 				//����,Ŀ�괥����ǰ����
							%Player.UpdateMission(%Mid);							//����������Ϣ
							GoToNextMap_Normal(%Player, %Map);
						}
					if(%MidState == 998)
						{
							%Conv.SetText(%MidState @ %Mid);	//���ݣ���ͼδ�������
							%Conv.AddOption(4 , 0 );		//����
						}
				}
		}
}

function Mission_Special_10123_2(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
			  {
			 	 	if(%Npc.GetDataID() $= "410107001"){%Conv.AddOption(41010, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 301 @ %Mid);}//������(ʥ)
			 	 	if(%Npc.GetDataID() $= "410207001"){%Conv.AddOption(41020, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 302 @ %Mid);}//������(��)
			 	 	if(%Npc.GetDataID() $= "410307001"){%Conv.AddOption(41030, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 303 @ %Mid);}//������(��)
			 	 	if(%Npc.GetDataID() $= "410407001"){%Conv.AddOption(41040, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 304 @ %Mid);}//�ɻ���(��)
			 	 	if(%Npc.GetDataID() $= "410507001"){%Conv.AddOption(41050, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 305 @ %Mid);}//���Ľ�(��)
			 	 	if(%Npc.GetDataID() $= "410607001"){%Conv.AddOption(41060, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 306 @ %Mid);}//ɽ����(��)
			 	 	if(%Npc.GetDataID() $= "410707001"){%Conv.AddOption(41070, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 307 @ %Mid);}//���鹬(��)
			 	 	if(%Npc.GetDataID() $= "410807001"){%Conv.AddOption(41080, 200 @ %Mid);%Conv.AddOption(200 @ %Mid, 308 @ %Mid);}//��ħ��(ħ)					
			 }
				
			if((%State > 0)&&(%Mid == 10123))
				{
					%MidState = GetSubStr(%State,0,3);
					
					if(%MidState == 200)
						{
							%ItemAdd = %Player.PutItem(105100102,1);
							%ItemAdd = %Player.AddItem();
							
							if(!%ItemAdd)
								{
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);
				
									return;
								}
								
							AddMissionReward(%Player, 10123, 0);	
							if(%Npc.GetDataID() $= "410107001"){%Player.SetFamily(1);}//"������(ʥ)
							if(%Npc.GetDataID() $= "410207001"){%Player.SetFamily(2);}//"������(��)
							if(%Npc.GetDataID() $= "410307001"){%Player.SetFamily(3);}//"������(��)
							if(%Npc.GetDataID() $= "410407001"){%Player.SetFamily(4);}//"�ɻ���(��)
							if(%Npc.GetDataID() $= "410507001"){%Player.SetFamily(5);}//"���Ľ�(��)
							if(%Npc.GetDataID() $= "410607001"){%Player.SetFamily(6);}//"ɽ����(��)
							if(%Npc.GetDataID() $= "410707001"){%Player.SetFamily(7);}//"���鹬(��)
							if(%Npc.GetDataID() $= "410807001"){%Player.SetFamily(8);}//"��ħ��(ħ)
						}
						
					if( (%MidState > 300)&&(%MidState < 309) )
						{  
							if(%MidState == 301) //ʥ
								if(%Player.GetSex() == 1)
								{
									%Conv.AddOption(202 @ %Mid, 402 @ %Mid);//���������ڵ�ͼ"������(��)"��������
									%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
									%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
									%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
									%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
									%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
							  }
							else
								{
									%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
									%Conv.AddOption(204 @ %Mid, 404 @ %Mid);//���������ڵ�ͼ"�ɻ���(��)"��ľ��
									%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
									%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
									%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
									%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
							  }
							  
						  if(%MidState == 302)//��
								if(%Player.GetSex() == 1)
								{
									%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
									%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
									%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
									%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
									%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
									%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
							  }
							else
								{
									%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
									%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
									%Conv.AddOption(204 @ %Mid, 404 @ %Mid);//���������ڵ�ͼ"�ɻ���(��)"��ľ��
									%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
									%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
									%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
									%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
							  }
							  
							  if(%MidState == 303)//��
									if(%Player.GetSex() == 1)
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//���������ڵ�ͼ"������(��)"��������
										%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
										%Conv.AddOption(204 @ %Mid, 404 @ %Mid);//���������ڵ�ͼ"�ɻ���(��)"��ľ��
										%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
								  }
								  
							if(%MidState == 304)//��
								if(%Player.GetSex() == 1)
								  {
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//���������ڵ�ͼ"������(��)"��������
                    %Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
										%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
										%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
										%Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
								  }		  		
								  
							if(%MidState == 305)//��
								if(%Player.GetSex() == 1)
								  {
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//���������ڵ�ͼ"������(��)"��������
                    %Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
										%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
                    %Conv.AddOption(204 @ %Mid, 404 @ %Mid);//���������ڵ�ͼ"�ɻ���(��)"��ľ��
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
								  }
						 if(%MidState == 306)//��
								if(%Player.GetSex() == 1)
								  {
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//���������ڵ�ͼ"������(��)"��������
                    %Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
                    %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
										%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
                    %Conv.AddOption(204 @ %Mid, 404 @ %Mid);//���������ڵ�ͼ"�ɻ���(��)"��ľ��
									  %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
								  }
						if(%MidState == 307)//��
								if(%Player.GetSex() == 1)
								  {
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//���������ڵ�ͼ"������(��)"��������
                    %Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
                    %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
										%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
                    %Conv.AddOption(204 @ %Mid, 404 @ %Mid);//���������ڵ�ͼ"�ɻ���(��)"��ľ��
									  %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
										%Conv.AddOption(208 @ %Mid, 408 @ %Mid);//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
								  }
								  
						if(%MidState == 308)//ħ
								if(%Player.GetSex() == 1)
								  {
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
                    %Conv.AddOption(202 @ %Mid, 402 @ %Mid);//���������ڵ�ͼ"������(��)"��������
                    %Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
                    %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
								  }
								else
									{
										%Conv.AddOption(201 @ %Mid, 401 @ %Mid);//���������ڵ�ͼ"������(ʥ)"���ع���
										%Conv.AddOption(203 @ %Mid, 403 @ %Mid);//���������ڵ�ͼ"������(��)"�����ɾ�
                    %Conv.AddOption(204 @ %Mid, 404 @ %Mid);//���������ڵ�ͼ"�ɻ���(��)"��ľ��
									  %Conv.AddOption(205 @ %Mid, 405 @ %Mid);//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
										%Conv.AddOption(206 @ %Mid, 406 @ %Mid);//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
										%Conv.AddOption(207 @ %Mid, 407 @ %Mid);//���������ڵ�ͼ"���鹬(��)"���α���
								  }	
						}  
						if( (%MidState > 400)&&(%MidState < 409) )
							 {
									if(%MidState == 401){%NpcX = 410107001;%Map = 101101;}//"������(ʥ)"���ع���
									if(%MidState == 402){%NpcX = 410207001;%Map = 100601;}//"������(��)"��������
									if(%MidState == 403){%NpcX = 410307001;%Map = 100701;}//"������(��)"�����ɾ�
									if(%MidState == 404){%NpcX = 410407001;%Map = 100501;}//"�ɻ���(��)"��ľ��
									if(%MidState == 405){%NpcX = 410507001;%Map = 100901;}//"���Ľ�(��)"��ڤ����
									if(%MidState == 406){%NpcX = 410607001;%Map = 101001;}//"ɽ����(��)"˫��ɽ
									if(%MidState == 407){%NpcX = 410707001;%Map = 100801;}//"���鹬(��)"���α���
									if(%MidState == 408){%NpcX = 410807001;%Map = 100401;}//"��ħ��(ħ)"��ϦԨ
					
									%Conv.SetType(4);
									%Player.SetMissionFlag(%Mid, 1150, %NpcX);		//����,Ŀ����
									%Player.SetMissionFlag(%Mid, 1250, 1);				//����,Ŀ�괥������
									%Player.SetMissionFlag(%Mid, 1350, 0); 				//����,Ŀ�괥����ǰ����
									%Player.UpdateMission(%Mid);							//����������Ϣ
									GoToNextMap_Normal(%Player, %Map);
							 }
						if(%MidState == 998)
							{
								%Conv.SetText(%MidState @ %Mid);	//���ݣ���ͼδ�������
								%Conv.AddOption(4 , 0 );		//����
							}

			   }

	  }												
}


function Mission_Special_10124(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%State == 0)
		{
			//������
			if(%Player.IsFinishedMission(%Mid - 1))
				if(!%Player.IsAcceptedMission(%Mid))
					if(!%Player.IsFinishedMission(%Mid))
						{
							%Yes = 0;

							if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "410107001") ){%Yes = 1;} //ʥNPC
							if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "410207001") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "410307001") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "410407001") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "410507001") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "410607001") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "410707001") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "410807001") ){%Yes = 1;} //ħNPC

							if(%Yes == 1)
								%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//��ʾ������ѡ��
						}

			//������
			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					{
						%No = 0;

						if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011002") ){%No = 1;} //ʥNPC
						if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004002") ){%No = 1;} //ħNPC

						if(%No == 1)
							%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//��ʾ������ѡ��
					}
		}

	if((%State > 0)&&(%Mid == 10124))
		{
			%MidState = GetSubStr(%State,0,3);

			switch(%MidState)
			{
				case 110:
					if(%Player.IsFinishedMission(%Mid - 1))
						if(!%Player.IsAcceptedMission(%Mid))
							if(!%Player.IsFinishedMission(%Mid))
								{
									AddMissionAccepted(%Player, %Mid);	//�������������Ϣ
									%Conv.SetType(4);
								}

				case 910:
					if(%Player.IsAcceptedMission(%Mid))
						if(!%Player.IsFinishedMission(%Mid))
							{
								AddMissionReward(%Player, %Mid, %Param);	//����������

								NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//ִ��Npc��������Ľű�
								
								if(%Mid == 10124)
									{
											%Conv.SetType(4);

										//��������ѡ�����
										if( (%Player.GetClasses(0) == 0)||(%Player.GetClasses(1) == 0) )
											{
												%A = %Player.GetClasses(0);
												%B = %Player.GetClasses(1);
												%C = %Player.GetClasses(2);
												%D = %Player.GetClasses(3);
												OpenSkillSelect(%Player,%A,%B,%C,%D);
											}
										//��������ѧϰ����
										if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
											{
												%A = %Player.GetClasses(0);
												%B = %Player.GetClasses(1);
												%C = %Player.GetClasses(2);
												%D = %Player.GetClasses(3);
												%E = %Player.GetClasses(4);
												OpenSkillStudy(%Player,%A,%B,%C,%D,%E);
											}
									}
							}
			 }
		}
}

function Mission_Special_10125(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%State == 0)
		{
			//������
			if(%Player.IsFinishedMission(%Mid - 1))
				if(!%Player.IsAcceptedMission(%Mid))
					if(!%Player.IsFinishedMission(%Mid))
						{
							%Yes = 0;

							if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011002") ){%Yes = 1;} //ʥNPC
							if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004002") ){%Yes = 1;} //ħNPC

							if(%Yes == 1)
								%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//��ʾ������ѡ��
						}

			//������
			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					{
						%No = 0;

						if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011002") ){%No = 1;} //ʥNPC
						if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008002") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004002") ){%No = 1;} //ħNPC

						if((%No == 1)&&(%Player.GetMissionFlag(%Mid, 3300) == 3))
							%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//��ʾ������ѡ��
					}
		}

	if((%State > 0)&&(%Mid == 10125))
		{
			%MidState = GetSubStr(%State,0,3);

			switch(%MidState)
			{
				case 110:
					if(%Player.IsFinishedMission(%Mid - 1))
						if(!%Player.IsAcceptedMission(%Mid))
							if(!%Player.IsFinishedMission(%Mid))
								{
									if( (%Player.GetFamily() == 1)){%NpcX = 410100001;}
									if( (%Player.GetFamily() == 2)){%NpcX = 410200001;}
									if( (%Player.GetFamily() == 3)){%NpcX = 410300001;}
									if( (%Player.GetFamily() == 4)){%NpcX = 410400001;}
									if( (%Player.GetFamily() == 5)){%NpcX = 410500001;}
									if( (%Player.GetFamily() == 6)){%NpcX = 410600001;}
									if( (%Player.GetFamily() == 7)){%NpcX = 410700001;}
									if( (%Player.GetFamily() == 8)){%NpcX = 410800001;}
										
				           echo("%NpcX ="@%NpcX);
				           %Player.SetMissionFlag(%Mid, 3100, %NpcX);		//����,Ŀ����
									 %Player.SetMissionFlag(%Mid, 3200, 3);				//����,Ŀ�괥������
									 %Player.SetMissionFlag(%Mid, 3300, 0); 				//����,Ŀ�괥����ǰ���� 
					 				 %Player.UpdateMission(%Mid);							//����������Ϣ
					 				 
									AddMissionAccepted(%Player, %Mid);	//�������������Ϣ
									%Conv.SetType(4);
								}

				case 910:
					if(%Player.IsAcceptedMission(%Mid))
						if(!%Player.IsFinishedMission(%Mid))
							{
								AddMissionReward(%Player, %Mid, %Param);	//����������

								NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//ִ��Npc��������Ľű�
							}
			}
		}
}

function Mission_Special_10126(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%State == 0)
		{
			//������
			if(%Player.IsFinishedMission(%Mid - 1))
				if(!%Player.IsAcceptedMission(%Mid))
					if(!%Player.IsFinishedMission(%Mid))
						{
							%Yes = 0;

							if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011002") ){%Yes = 1;} //ʥNPC
							if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008002") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004002") ){%Yes = 1;} //ħNPC

							if(%Yes == 1)
								%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//��ʾ������ѡ��
						}

			//������
			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					{
						%No = 0;

						if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011006") ){%No = 1;} //ʥNPC
						if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006006") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007006") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005006") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009006") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010006") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008006") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004006") ){%No = 1;} //ħNPC

						if(%No == 1)
							%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//��ʾ������ѡ��
					}
		}

	if((%State > 0)&&(%Mid == 10126))
		{
			%MidState = GetSubStr(%State,0,3);

			switch(%MidState)
			{
				case 110:
					if(%Player.IsFinishedMission(%Mid - 1))
						if(!%Player.IsAcceptedMission(%Mid))
							if(!%Player.IsFinishedMission(%Mid))
								{
									AddMissionAccepted(%Player, %Mid);	//�������������Ϣ
                  %Conv.SetType(4);
									echo("������ܲ��� = "@%Mid);
								}

				case 910:
					if(%Player.IsAcceptedMission(%Mid))
						if(!%Player.IsFinishedMission(%Mid))
							{
								AddMissionReward(%Player, %Mid, %Param);	//����������

								NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//ִ��Npc��������Ľű�
							}
			}
		}
}


function Mission_Special_10127(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%State == 0)
		{
			//������
			if(%Player.IsFinishedMission(%Mid - 1))
				if(!%Player.IsAcceptedMission(%Mid))
					if(!%Player.IsFinishedMission(%Mid))
						{
							%Yes = 0;

							if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "401011006") ){%Yes = 1;} //ʥNPC
							if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "401006006") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "401007006") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "401005006") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "401009006") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "401010006") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "401008006") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "401004006") ){%Yes = 1;} //ħNPC

							if(%Yes == 1)
								%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//��ʾ������ѡ��
						}

			//������
			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					{
						%No = 0;

						if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "400001038") ){%No = 1;} //ʥNPC
						if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "400001044") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "400001039") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "400001042") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "400001041") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "400001040") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "400001043") ){%No = 1;} //��NPC
						if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "400001045") ){%No = 1;} //ħNPC

						if(%No == 1)
							%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//��ʾ������ѡ��
					}
		}

	if((%State > 0)&&(%Mid == 10127))
		{
			%MidState = GetSubStr(%State,0,3);

			switch(%MidState)
			{
				case 110:
					if(%Player.IsFinishedMission(%Mid - 1))
						if(!%Player.IsAcceptedMission(%Mid))
							if(!%Player.IsFinishedMission(%Mid))
								{
									AddMissionAccepted(%Player, %Mid);	//�������������Ϣ
									%Conv.SetType(4);
								}

				case 910:
					if(%Player.IsAcceptedMission(%Mid))
						if(!%Player.IsFinishedMission(%Mid))
							{
								AddMissionReward(%Player, %Mid, %Param);	//����������

								NpcOnTrigger(%Conv, %Npc, %Player, 0, %Param);	//ִ��Npc��������Ľű�
							}
			}
		}
}


function Mission_Special_10128(%Npc, %Player, %State, %Conv, %Param, %Mid)
{
	if(%State == 0)
		{
			//������
			if(%Player.IsFinishedMission(%Mid - 1))
				if(!%Player.IsAcceptedMission(%Mid))
					if(!%Player.IsFinishedMission(%Mid))
						{
							%Yes = 0;

							if( (%Player.GetFamily() == 1)&&(%Npc.GetDataID() $= "400001038") ){%Yes = 1;} //ʥNPC
							if( (%Player.GetFamily() == 2)&&(%Npc.GetDataID() $= "400001044") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 3)&&(%Npc.GetDataID() $= "400001039") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 4)&&(%Npc.GetDataID() $= "400001042") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 5)&&(%Npc.GetDataID() $= "400001041") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 6)&&(%Npc.GetDataID() $= "400001040") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 7)&&(%Npc.GetDataID() $= "400001043") ){%Yes = 1;} //��NPC
							if( (%Player.GetFamily() == 8)&&(%Npc.GetDataID() $= "400001045") ){%Yes = 1;} //ħNPC

							if(%Yes == 1)
								%Conv.AddOption(100 @ %Mid, 110 @ %Mid , 1);//��ʾ������ѡ��
						}

			//������
			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Npc.GetDataID() $= "400001059")
						%Conv.AddOption(900 @ %Mid, 910 @ %Mid , 2);//��ʾ������ѡ��
		}

	if((%State > 0)&&(%Mid == 10128))
		{
			%MidState = GetSubStr(%State,0,3);

			switch(%MidState)
			{
				case 110:
					if(%Player.IsFinishedMission(%Mid - 1))
						if(!%Player.IsAcceptedMission(%Mid))
							if(!%Player.IsFinishedMission(%Mid))
								{
									AddMissionAccepted(%Player, %Mid);	//�������������Ϣ
									%Conv.SetType(4);
								}

				case 910:
					if(%Player.IsAcceptedMission(%Mid))
						if(!%Player.IsFinishedMission(%Mid))
							{
								AddMissionReward(%Player, %Mid, %Param);	//����������
								//��װ����������

								%Conv.SetType(4);
								OpenIdentify(%Player);
							}
			}
		}
}

function Mission_Special_10140(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10140;
	
	if(%State == 0)
		{
			if(%Player.IsFinishedMission(10128))
				if(%Player.GetLevel() < 11)
						  %Conv.AddOption(100 @ %Mid, 120 @ %Mid);//��ʾ������ѡ��
		}
		
	if(%State > 0)
		{
			%MidState = GetSubStr(%State,0,3);
			%Pet = %Player.GetSpawnedPet();
					
			if(%MidState == 120)
				{
				   %Conv.SetText(%Mid);	
				}
		}  
}

function Mission_Special_10153(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10153;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//���Ѿ�׼����ȥ�������
					%Conv.AddOption(300 @ %Mid, -1 );	//�Ե�Ƭ�̣��һ�������Ƭ��
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						//GoToNextMap_Normal(%Player, 100104);
						FlyMeToTheMoon(%Player, 11010101);
				}
		}
}


function Mission_Special_10156(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10156;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//��׼����ȥ����������
					%Conv.AddOption(300 @ %Mid, -1 );	//�Ե�Ƭ�̣��һ�������Ƭ��
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						//GoToNextMap_Normal(%Player, 110104);
						FlyMeToTheMoon(%Player, 10010104);
				}
		}
}


function Mission_Special_10161(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10161;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//��׼������
					%Conv.AddOption(300 @ %Mid, -1 );	//�Ե�Ƭ�̣��һ�������Ƭ��
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_CopyMap(%Player,1302);
						%Conv.SetType(4);
				}
		}
}


function Mission_Special_10186(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10186;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//��δ�Ի�
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//�����뽲

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							%Conv.SetText(300 @ %Mid);	//�԰�
							%Conv.AddOption(4 , 0 );		//����

							%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//����,Ŀ�괥����ǰ����
						}
				}
		}
}

function Mission_Special_10233(%Npc, %Player, %State, %Conv, %Param)
{
  %Mid = 10233;

	if(%Player.IsAcceptedMission(%Mid))
		{
				if(%State == 0)
					if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//��δ�Ի�
						%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//�����뽲
	
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 200)
							{
								%Conv.SetText(201 @ %Mid);	//�԰�
								%Conv.AddOption(202 @ %Mid, 203 @ %Mid );	
							}
						if(%MidState ==203)
							{
								%Conv.SetText(203 @ %Mid);	//�԰�
								%Conv.AddOption(204 @ %Mid, 205 @ %Mid );	
								
							}
						if(%MidState ==205)
							{
								%Conv.SetText(205 @ %Mid);	//�԰�
								%Conv.AddOption(4 , 0 );		//����
								
								%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//����,Ŀ�괥����ǰ����
							}
					}
		  }
}

function Mission_Special_10303(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10303;
	%PlayerID = %Player.GetPlayerID();

	if(%State == 0)
		{	
				if(%Npc.GetDataID() $= "401102121")
					{
					
						%Conv.SetText(200 @ %Mid);	//�߶�һ
					
					}
					
				if(%Npc.GetDataID() $= "401102122")
					{
						
					  %Conv.SetText(201 @ %Mid);	//�߶���
			
				  }
				  
				if(%Npc.GetDataID() $= "401102123")	
					{	
				 	
						%Conv.SetText(202 @ %Mid);	//�߶���
				
						if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020093)==0))
							{
									%ItemAdd = %Player.PutItem(108020093, 1);
									%ItemAdd = %Player.AddItem();
									
									if(!%ItemAdd)
								    {
											SendOneChatMessage(0,"<t>��������</t>",%Player);
											SendOneScreenMessage(2,"��������", %Player);
											%Conv.SetType(4);
											return;
								    }	
							} 
          }
     }   
}

function Mission_Special_10305(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10305;
	%PlayerID = %Player.GetPlayerID();
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
			{	
				if(%player.GetItemCount(108020095)==1){%Conv.AddOption(200 @ %Mid, 200 @ %Mid );}
				if(%player.GetItemCount(108020096)==1){%Conv.AddOption(201 @ %Mid, 201 @ %Mid );}
				if(%player.GetItemCount(108020097)==1){%Conv.AddOption(202 @ %Mid, 202 @ %Mid );}
		    if((%player.GetItemCount(108020095)==0)&&(%player.GetItemCount(108020096)==0)&&(%player.GetItemCount(108020097)==0)&&(%player.GetItemCount(108020118)==0))
		    {
		    	%Conv.SetText(203 @ %Mid);
		    	%Conv.AddOption(204 @ %Mid, 204 @ %Mid );
		    }
		    
		  }  	
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 200)
							{
								%Conv.SetText(210 @ %Mid);	//�԰�
								%Conv.AddOption(4,0 );		//����
								
								%ItemAdd = %Player.PutItem(108020095, -1);
								%ItemAdd = %Player.AddItem();
							}
						if(%MidState == 201)	
							{
								%Conv.SetText(220 @ %Mid);	//�԰�
								%Conv.AddOption(4,0 );		//����	
								
								%ItemAdd = %Player.PutItem(108020096, -1);
								%ItemAdd = %Player.AddItem();
							}
						if(%MidState == 202)
							{
								%Conv.SetText(230 @ %Mid);	//�԰�
								%Conv.AddOption(4,0 );		//����
								
								%ItemAdd = %Player.PutItem(108020097, -1);
								%ItemAdd = %Player.AddItem();
							}
						if(%MidState == 204)
							{
								%ItemAdd = %Player.PutItem(108020118, 1);
								%ItemAdd = %Player.AddItem();
								
									if(!%ItemAdd)
								  {
										SendOneChatMessage(0,"<t>��������</t>",%Player);
										SendOneScreenMessage(2,"��������",%Player);
										
										return;
								  }
								  %Conv.SetType(4);
						  }
					}
		  }				
}

function Mission_Special_10316(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10316;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//��δ�Ի�
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//�峤�뽲

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							%Conv.SetText(201 @ %Mid);	//�԰�
							%Conv.AddOption(4 , 0 );		//�����Ի�
							%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//����,Ŀ�괥����ǰ����
						}
				}
		}
}	

function Mission_Special_10320(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10320;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//��׼������
					%Conv.AddOption(300 @ %Mid, -1 );	//�Ե�Ƭ�̣��һ�������Ƭ��
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_Normal(%Player, 110204);
				}
		}
}

function Mission_Special_10321(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10321;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//��׼������
					%Conv.AddOption(300 @ %Mid, -1 );	//�Ե�Ƭ�̣��һ�������Ƭ��
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_CopyMap( %Player, 1304 );
				}
		}
}

function Mission_Special_10527_1(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10527;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			 if(%State == 0)
				{
					if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//��δ�Ի�
					%Conv.AddOption(201 @ %Mid, 200 @ %Mid );	//����
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					
					if(%MidState == 200)
						{	
							%Conv.SetText(200 @ %Mid);	//�԰�
							%Conv.AddOption(1,-1);		//�����Ի�		
							%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//����,Ŀ�괥����ǰ����

						}

				}
		}
}

function Mission_Special_10527_2(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10527;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			
			if(%State == 0)
				{
					if(%Player.GetMissionFlag(%Mid, 1351) == 0)		
					%Conv.AddOption(201 @ %Mid, 300 @ %Mid );	//����
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 300)
						{	
							%Conv.SetText(300 @ %Mid);	//�԰�
							%Conv.AddOption(1,-1);		//�����Ի�	
							%Player.SetMissionFlag(%Mid, 1351, 1, true); //����,Ŀ�괥����ǰ����
						}				
				}
		}
}

function Mission_Special_10527_3(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10527;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			
			if(%State == 0)
				{
					if(%Player.GetMissionFlag(%Mid, 1352) == 0)		
					%Conv.AddOption(201 @ %Mid, 400 @ %Mid );	//����
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 400)
						{	
						
							%Conv.SetText(400 @ %Mid);	//�԰�
							%Conv.AddOption(1,-1);		//�����Ի�	
							%Player.SetMissionFlag(%Mid, 1352, 1, true); //����,Ŀ�괥����ǰ����
					
						}			
				}
		}
}


function Mission_Special_10545(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10545;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//��׼������
					%Conv.AddOption(300 @ %Mid, -1 );	//�Ե�Ƭ�̣��һ�������Ƭ��
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_Normal(%Player, 110306);
				}
		}
}

function Mission_Special_10580(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10580;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//��׼������
					%Conv.AddOption(300 @ %Mid, -1 );	//�Ե�Ƭ�̣��һ�������Ƭ��
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_Normal(%Player, 110307);
				}
		}
}	

function Mission_Special_10584(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10584;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//��׼������
					%Conv.AddOption(300 @ %Mid, -1 );	//�Ե�Ƭ�̣��һ�������Ƭ��
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						GoToNextMap_CopyMap( %Player, 1305 );
				}
		}
}

function Mission_Special_10585(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid1 = 10585;
	%Mid2 = 10586;
	%Mid3 = 10587;
	%Mid4 = 10588;
  %Mid5 = 10589;
  
	if((%Player.IsAcceptedMission(%Mid1))&&(%Player.GetMissionFlag(%Mid1, 3300) == 0))	
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid1, 200 @ %Mid1 );	
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							SpNewNpc3(%Player, 411101016, "-5.75982 -9.61186 110.746", %CopyMapID,0,"2 2 2");
							%Conv.SetType(4);
						}
				}
		}
		
	if((%Player.IsAcceptedMission(%Mid2))&&(%Player.GetMissionFlag(%Mid2, 3300) == 0))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid2, 200 @ %Mid2 );	
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
					  {
							SpNewNpc3(%Player, 410600007, "17.6165 -9.62924 110.756", %CopyMapID,0,"2 2 2");
							%Conv.SetType(4);
					  }
				}
		}
		
			
	if((%Player.IsAcceptedMission(%Mid3))&&(%Player.GetMissionFlag(%Mid3, 3300) == 0))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid3, 200 @ %Mid3 );	
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							SpNewNpc3(%Player, 410701041, "5.98371 10.1313 110.749", %CopyMapID,0,"2 2 2");
							%Conv.SetType(4);
						}
				}
				
		}
		
	if((%Player.IsAcceptedMission(%Mid4))&&(%Player.GetMissionFlag(%Mid4, 3300) == 0))
		{
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid4, 200 @ %Mid4 );	
				}
	
			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							SpNewNpc3(%Player, 410400019, "-5.58389 3.83402 110.756", %CopyMapID,0,"2 2 2");
							%Conv.SetType(4);
						}
				}
					
		}
		
 if((%Player.IsAcceptedMission(%Mid5))&&(%Player.GetMissionFlag(%Mid5, 3300) == 0))
	 {
			if(%State == 0)
				{
					%Conv.AddOption(200 @ %Mid5, 200 @ %Mid5 );	
				}

			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
					  {
							SpNewNpc3(%Player, 410200007, "5.91351 -2.7875 111.028", %CopyMapID,0,"2 2 2");
		          %Conv.SetType(4);
            }
				}
			
	}
			
}

function Mission_Special_10704(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10704;
	
	if(%Player.IsAcceptedMission(%Mid))
		{
			
			if(%State == 0)
				{
					if(%Player.GetMissionFlag(%Mid, 2300) == 0)
						{		
							%Conv.AddOption(200 @ %Mid, 200 @ %Mid );
						}	
				}

			if(%State > 0)
				{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 200)
							{	
								%Conv.SetText(201 @ %Mid);	//�԰�	
								%Conv.AddOption(1,300 @ %Mid);		//�����Ի�
							}	
						
						if(%MidState == 300)
						  {	
								if($New_10704 ==0)
									{
		           			$New_10704 = SpNewNpc(%Player,410400022,0,0);
		           			%Player.SetMissionFlag(%Mid, 1350, 1, true); //����,Ŀ�괥����ǰ����
		           			Schedule(60000, 0, "RemoveNpc_10704");
		           			%Conv.SetType(4);
									}
						
						  }
				 }
		}
}

function RemoveNpc_10704()
{
	$New_10704.SafeDeleteObject();
	$New_10704= 0;
}

function Mission_Special_10743(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10743;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//��δ�Ի�
					{
						%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	
					}
					
			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							%Conv.SetText(300 @ %Mid);	//�԰�
							%Conv.AddOption(4,0);		//����
							%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//����,Ŀ�괥����ǰ����
						}
				}
		}
}

function Mission_Special_10761(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10761;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				{
					if((%Player.GetMissionFlag(%Mid, 3300) == 0)&&(%Player.GetMissionFlag(%Mid, 3301) == 0))
						{
							%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	
				  	}
				}
				
			if(%State > 0)
				{
					%MidState = GetSubStr(%State,0,3);
					if(%MidState == 200)
						{
							if($New_10761 ==0)
							  {
						       $New_10761 = SpNewNpc(%Player,401104012,0,0);
							     %Player.SetMissionFlag(%Mid, 1300, 1, true);
							     Schedule(3000, 0, "RemoveNpc_10761");
							  }
						}
				}
		}
}

function RemoveNpc_10761()
{
	SpNewNpc(%Player,410400049,0,0);
  SpNewNpc(%Player,410400051,0,0);
	$New_10761.SafeDeleteObject();
}

function Mission_Special_10907(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10907;

	if(%Player.IsAcceptedMission(%Mid))
		{
	    if(%State ==0)
	    	{
		       if(%Player.GetMissionFlag(%Mid, 1300) == 0)
		       	 {
				       	%Conv.SetText(170 @ %Mid);	//�԰�
				       	%Conv.AddOption(1,0);		//�����Ի�
				       	%Player.SetMissionFlag(%Mid, 1300, 1, true);
				        $New_552000049_1.SafeDeleteObject();
			       }
			     if(%Player.GetMissionFlag(%Mid, 1300) == 1)
			     	 {
			     		 %Conv.SetText(171 @ %Mid);	//�԰�
			     		 %Conv.AddOption(1,0);		//�����Ի�
				       %Player.SetMissionFlag(%Mid, 1300, 2, true);
				       $New_552000049_1.SafeDeleteObject();
				     } 
			     if(%Player.GetMissionFlag(%Mid, 1300) == 2)
			     	 {
			     		 %Conv.SetText(172 @ %Mid);	//�԰�
			     		 %Conv.AddOption(1,0);		//�����Ի�
				       %Player.SetMissionFlag(%Mid, 1300, 3, true);
				       $New_552000049_1.SafeDeleteObject();
				     } 
				      
				   if(%Player.GetMissionFlag(%Mid, 1300) == 3)
				   	 {
			     		 %Conv.SetText(173 @ %Mid);	//�԰�
			     		 %Conv.AddOption(1,0);		//�����Ի�
				       %Player.SetMissionFlag(%Mid, 1300, 4, true);
				       $New_552000049_1.SafeDeleteObject();
				     }  
				   if(%Player.GetMissionFlag(%Mid, 1300) == 4)
				     {
				     	 %Conv.SetText(174 @ %Mid);	//�԰�
				     	 %Conv.AddOption(1,0);		//�����Ի�
					     %Player.SetMissionFlag(%Mid, 1300, 5, true);
					     $New_552000049_1.SafeDeleteObject();	     	 
		         }
	       }
	   }
}

function Mission_Special_10908(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10908;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//��δ�Ի�
				  {	
						%Conv.SetText(100 @ %Mid);	//�԰�
						%Conv.AddOption(101 @ %Mid, 101 @ %Mid );	
          }
          
			if(%State == 101)
				{
					%Conv.SetText(110 @ %Mid);	//�԰�
					%Conv.AddOption(111 @ %Mid, 111 @ %Mid );	
				}
				
			if(%State == 111)
				{
					%Conv.SetText(120 @ %Mid);	//�԰�
					%Conv.AddOption(121 @ %Mid, 121 @ %Mid );	
				}
			if(%State == 121)
	      {
			     %Player.SetMissionFlag(%Mid, 1350, 1, true); 				//����,Ŀ�괥����ǰ����
			     %Conv.SetType(4);     
			  }
		}
		
}

function Mission_Special_10915(%Npc, %Player, %State, %Conv, %Param)
{
	%Mid = 10915;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%State == 0)
				if(%Player.GetMissionFlag(%Mid, 1300) == 1)
					if(%Player.GetMissionFlag(%Mid, 1350) == 0)		//��δ�Ի�
						%Conv.AddOption(200 @ %Mid, 200 @ %Mid );	//�峤�뽲
	
				if(%State > 0)
					{
						%MidState = GetSubStr(%State,0,3);
						if(%MidState == 200)
							{
								%Player.SetMissionFlag(%Mid, 1350, 1, true); 				//����,Ŀ�괥����ǰ����
							}
					}
		}
}						