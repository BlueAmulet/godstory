//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//����˺����ű���������������߻�����ĺ���
//ֻ�ڷ����ʹ�ã������������鲻��ͻ��˺���ʹ����ͬ����
//==================================================================================

//������ת��ڣ�ѡ�к�ʹ��F3���ɣ�����㲻����UE���ǾͰﲻ������

//�������������������������Npc�����Ľű�����������������������������������
//�����������������������������򴥷��ű�������������������������������������
//�����������������������뿪���򴥷��ű�������������������������������������
//�����������������������������������ű�������������������������������������
//����������������������ˢ��NPC��������������������������������������������
//����������������������ˢ�²ɼ��������������������������������������������
//������������������������ʱ�������󴥷��ű���������������������������������
//�������������������������͵�ͼ�ű�����������������������������������������
//�������������������������ֽ�ɫ��ʼ���ű�����������������������������������
//�����������������������ɼ��¼����ýű�������������������������������������
//������������������������ɫ�ɹ���¼����������������������������������������
//������������������������ɫ�л��̡߳���������������������������������������
//������������������������ɫ�л���ͼ����������������������������������������
//������������������������ɫ������������������������������������������������
//������������������������ɫ������������������������������������������������
//������������������������ɫ�����������������������������������������������
//�����������������������ض�����ĳЩ��������Ľű���������������������������
//�������������������������߼����ű�����������������������������������������
//����������������������������ָ��ʱ��ű�����������������������������������
//����������������������ѧϰ������Զ�ѧ���䷽����������������������������
//����������������������������Ե�˳ɹ�������ű�����������������������������
//�����������������������½�����������������������������������������������
//������������������������������ʱˢ�����а��������������������������������
//����������������������״̬�����󴥷���������������������������������������



//�������������������������Npc�����Ľű�����������������������������������
//���Npc�����Ľű�
function NpcObject::OnTrigger(%Npc, %Player, %State, %Param)
{
	//��ҳ�������׸�NPC
//	if($FirstBlood == "")
//		if(%Npc.GetDataID() $= 400001101)
//			$FirstBlood = %Npc;

//	echo("ID = "@%Npc.GetDataID()@"Npc = "@%Npc);

	%Conv = new Conversation();
	%Conv.SetType(1);	//�Ի���ʽ����ѡ�

	if(%State == -1)
		{
//			echo("�رնԻ��رնԻ�");
			%Conv.SetType(4);	//�رնԻ�
			%Conv.Send(%Player);
			return;
		}

	//ִ��Npc��������Ľű�
	NpcOnTrigger(%Conv, %Npc, %Player, %State, %Param);
//	if(%State == 0)
//		{
//			%Conv.SetType(1);
//			%Conv.AddOption(1,-1);//Ĭ���н����Ի�
//		}

	%Conv.Send(%Player);
}

//ִ��Npc��������Ľű�
function NpcOnTrigger(%Conv, %Npc, %Player, %State, %Param)
{
	if(%State == 0)
		%Conv.SetText(%Npc.GetDataID());//��ʾNPCĬ�϶Ի�

	//%State��������0��ֻ��8λʱ����������ű�
	if( (%State == 0)||(strlen(%State) == 8) )
		{
//			Mission_Lead(%Npc, %Player, %State, %Conv, %Param);			//ָ������ű�
			Mission_Special(%Npc, %Player, %State, %Conv, %Param);	//��������ű�
			Mission_Normal(%Npc, %Player, %State, %Conv, %Param);		//��ͨ����ű�
		}

	 //ʦ�����񣭲���
   //Mission20001(%Npc, %Player, %State , %Conv);
	//Ѱ��-����
	//Mission20002(%Npc, %Player, %State , %Conv);
	//̽��-����
	//Mission20003(%Npc, %Player, %State , %Conv);


	//%State�������ڵ���6λʱ������˵�
	if(strlen(%State) <= 6)
		Npc_Job(%Npc, %Player, %State , %Conv);		//NPC�����Խű�
//		{
//			//�Զ�����ͼ�����NPC��ż���������������ȥ����ִ����Ӧ�ĺ���
//			%HeBingFunction = "Map" @ GetZoneID() @ "Npc" @ %Npc.GetDataID() @ "(%Npc, %Player, %State , %Conv);";
//			eval(%HeBingFunction);
//		}

}

//�������������������������Npc�����Ľű�����������������������������������

//�����������������������������򴥷��ű�������������������������������������

//�������򴥷��ű�
function OnEnterTrigger(%Player,%TriggerId,%FatherTriggerID)
{
	echo("OnEnterTrigger = IN = " @ %TriggerId);

	//�Զ�����ǰ��ҵ����򱣴�����ʱ������
	$Player_Area[%Player.GetPlayerID()] = %TriggerId;

	//���ʹ���
	if($Area[%TriggerId] > 0)
		{
			if($Area[%TriggerId] < 900000)
				GoToNextMap_Normal(%Player, $Area[%TriggerId] );	//��ͨ���򴥷�
			else
				FlyMeToTheMoon(%Player, $Area[%TriggerId] );			//���е㴥��

			return;
		}

	if(%TriggerId $= "811270100")
		{
      %Player.AddTimerControl(1, 300000,1,0,1,1);
			%Player.SetTimerControl(1, 1, "105100104");
			%Player.AddBuff(390010001, 0);
			%Player.AddBuff(390020001, 0);
		}
	if(%TriggerId $= "813030100")
		{
			%Mid = 10109;

			%CopyMapID = %Player.GetLayerId();

			if(%Player.IsAcceptedMission(%Mid))
	  			{
	  				if(%Player.GetMissionFlag(%Mid, 3300) == 4)
	  					{
	  						if(%Player.GetFlagsByte(27) ==0)
	  							{
	  								%Player.SetFlagsByte(25,1);
	  								%Player.SetFlagsByte(27,1);

				  		  		SpNewNpc3(%Player, 410700024, "-1.24882 -4.7772 133.91", %CopyMapID,0,"2 2 2");
				  		  	}
				  		}
	  			}

		}

	if(%TriggerId $= "810010105")
		{
			//�����

			%Mid = 10000;
			if( (!%Player.IsAcceptedMission(%Mid))&&(!%Player.IsFinishedMission(%Mid)) )
				OnTriggerMission(%Player, %TriggerId , %Mid, 0, 0, 0, 0);
		}

	if(%TriggerId $= "810010106")
		{
			//���³�����
			%Mid = 10113;

			if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)
						OnTriggerMission(%Player, %TriggerId , %Mid, 0, 0, 0, 0);
		}

	if(%TriggerId $= "810010119")
	  {
	  	//����������
	  	%Mid =10120;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)
	  				{
	  					%Player.SetMissionFlag(%Mid, 1300, 1, true);
	  					GoToNextMap_Normal(%Player, 100119);
	  				}
	  }

	if(%TriggerId $= "811030110")
	  {
	  	//������Ӫ��
	  	%Mid =10510;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)
	  				%Player.SetMissionFlag(%Mid, 1300, 1, true);
	  }

	 if(%TriggerId $= "811030111")
	  {
	  	//������Ӫ��
	  	%Mid =10510;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1301) < 1)
	  				%Player.SetMissionFlag(%Mid, 1301, 1, true);
	  }

	  if(%TriggerId $= "811030112")
	  {
	  	//������Ӫ��

	  	%Mid =10510;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1302) < 1)
	  				%Player.SetMissionFlag(%Mid, 1302, 1, true);
	  }

	if(%TriggerId $= "811030113")
	  {
	  	//���̸�������
	  	%Mid =10522;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)

	  		%Player.SetMissionFlag(%Mid, 1300, 1, true);
	  }

	if(%TriggerId $= "811030106")
	  {
	  	//�����µ�������
	  	%Mid =10547;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1300) < 1)
	  				%Player.SetMissionFlag(%Mid, 1300, 1, true);
	  }

	if(%TriggerId $= "811030107")
	  {
	  	//�����µƹ����
	  	%Mid =10547;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1301) < 1)
	  				%Player.SetMissionFlag(%Mid, 1301, 1, true);
	  }

	if(%TriggerId $= "811030108")
	  {
	  	//�����ǵ�������
	  	%Mid =10547;

	  	if(%Player.IsAcceptedMission(%Mid))
				if(!%Player.IsFinishedMission(%Mid))
					if(%Player.GetMissionFlag(%Mid, 1302) < 1)
	  				%Player.SetMissionFlag(%Mid, 1302, 1, true);
	  }
	if(%TriggerId $= "810040100")//��ϦԨ
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410807001);		//����,Ŀ����
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//����,Ŀ�괥������
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//����,Ŀ�괥����ǰ����
					}
		}
		
	if(%TriggerId $= "810050100")//��ľ��
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410407001);		//����,Ŀ����
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//����,Ŀ�괥������
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//����,Ŀ�괥����ǰ����
					}
		}

	if(%TriggerId $= "810060100")//��������
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410207001);		//����,Ŀ����
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//����,Ŀ�괥������
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//����,Ŀ�괥����ǰ����
					}
		}

	if(%TriggerId $= "810070100")//�����ɾ�
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410307001);		//����,Ŀ����
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//����,Ŀ�괥������
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//����,Ŀ�괥����ǰ����
					}
		}
		
	if(%TriggerId $= "810080100")//���α���
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410707001);		//����,Ŀ����
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//����,Ŀ�괥������
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//����,Ŀ�괥����ǰ����
					}
		}
		
	if(%TriggerId $= "810090100")//��ڤ����
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410507001);		//����,Ŀ����
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//����,Ŀ�괥������
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//����,Ŀ�괥����ǰ����
					}
		}

	if(%TriggerId $= "810100100")//˫��ɽ
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410607001);		//����,Ŀ����
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				//����,Ŀ�괥������
						%Player.SetMissionFlag(%Mid, 1350, 0); 				//����,Ŀ�괥����ǰ����
					}
		}
		
	if(%TriggerId $= "810110100")//���ع���
		{
			%Mid = 10123;

			if(%Player.IsAcceptedMission(%Mid))
			  if(!%Player.IsFinishedMission(%Mid))
			  	{
			  		%Player.SetMissionFlag(%Mid, 1150, 410107001);		//����,Ŀ����
				  	%Player.SetMissionFlag(%Mid, 1250, 1);				   //����,Ŀ�괥������
						%Player.SetMissionFlag(%Mid, 1350, 0); 				   //����,Ŀ�괥����ǰ����
					}
		}
}



//�����������������������������򴥷��ű�������������������������������������

//�����������������������뿪���򴥷��ű�������������������������������������
//�뿪���򴥷��ű�
function OnLeaveTrigger(%Player,%TriggerId,%FatherTriggerID)
{

}

//�����������������������뿪���򴥷��ű�������������������������������������


//�����������������������������������ű�������������������������������������
//�������������ű�
function Player::KilledMonster(%Player, %Monster, %SharePlayerNum, %MaxConnection)
{
	//��ȡ����ı��
	%MonsterID = %Monster.GetDataID();
	//��ȡ��ҵı��
	%PlayerID = %Player.GetPlayerID();
	//��ȡ��ҵȼ�
	%P_Lv[1] = %Player.GetLevel();
	//��ȡ��ǰ����ȼ�
	%Pet = %Player.GetSpawnedPet();
	if(%Pet > 0)
		{
			%P_Lv[2] = %Pet.GetLevel();
			%x = 3;
		}
		else
			%x = 2;

	//��ȡ����ȼ�
	%MonLv = %Monster.GetLevel();
	//��ȡ����Ļ�������
	if(GetNpcData(%MonsterID, 16) $= "0")
		%Exp = $Monster_Exp[ %MonLv, 1 ];
	else
		%Exp = GetNpcData(%MonsterID, 16);

	//��ȡ����Ļ������ﾭ��
	%Exp2 = $Monster_Exp[ %MonLv, 2 ];

	//ɱ�ֵĸ��ֽ���
	//����������
		//�ȼ�������ʽ
		for(%i = 1;%i < %x;%i++)
		{
			%Lv = %P_Lv[%i] - %MonLv;

			if($Lv_Exp[ %Lv ] $= "")	//����ȼ�����������������ʽ�ڵĿ���
				%Y[%i] = 0;
			else
				%Y[%i] = $Lv_Exp[ %Lv ];
		}

		//�ж��Ƿ�Ϊ���˼������
		if(%Y[1] > 0)
			{
				//���״̬�ӳ�
				%PlayerBuff = 1 + %Player.GetBuffCount(39110, 1) * 0.5 + %Player.GetBuffCount(39110, 2) * 2 + %Player.GetBuffCount(39110, 3) * 3;

				if(%SharePlayerNum == 0)
					{
						%Player_Exp = %Exp * %Y[1] * %PlayerBuff;
//						SendOneChatMessage(0, "<t>�����þ��飽�������������"@%Exp@"���ȼ�����"@%Y@"��״̬�ӳ�"@%PlayerBuff@"��</t>", %Player);
					}
					else
						{
							//����Χ�ڵĶ��������ӳ�
							%S = $SharePlayerNum[ %SharePlayerNum ];

							//����Χ�ڵĶ�������ϵ�ӳ�
							%N = $MaxConnection[ %MaxConnection ];

							%Player_Exp = (%Exp * %S / %SharePlayerNum * ( 1 + %N ) ) * %Y[1] * %PlayerBuff;
//						SendOneChatMessage(0, "<t>�����þ��飽�������������"@%Exp@"�����Ѽӳ�"@%S@"/��������"@%SharePlayerNum@"����1+��������ϵ�ӳ�"@%N@"���������ȼ�����"@%Y@"��״̬�ӳ�"@%PlayerBuff@"��</t>", %Player);
						}

				//��Ҿ���
				%Player.AddExp(%Player_Exp);
			}

	echo("���["@%Player.GetPlayerName()@"]ɱ���˹���["@GetNpcData(%MonsterID,1)@"]ID = "@%MonsterID@"��þ���="@%Player_Exp);

	//���ﾭ��
	if( (%Y[2] > 0)&&(%Pet > 0) )
		{
			//����״̬�ӳ�
			%PetBuff = 1 + %Player.GetBuffCount(39111, 1) * 0.5 + %Player.GetBuffCount(39111, 2) * 2 + %Player.GetBuffCount(39111, 3) * 3;

			%Player.AddPetExp(%Player.GetSpawnedPetSlot(), (%Exp2 * %Y[2] * %PetBuff) );
		}
//	SendOneChatMessage(0, "<t>�����þ��飽�������������"@%Exp@"���ȼ�����"@%Y@"��״̬�ӳ�"@%PetBuff@"��</t>", %Player);

//	%Player.AddPetExp(%slot, %exp);
//	%Player.AddPetLevel(%slot, %level);
//	%Player.GetSpawnedPetSlot();


	//10����������Զ�����
	if(%Player.GetLevel() < 10)
		if(%Player.GetExp() >= %Player.GetLevelExp())
			{
				%Player.AddExp(- %Player.GetLevelExp());
				%Player.AddLevel(1);
			}

	%MonsterKill_Mid = $MissionData_MonsterKill[%MonsterID];

	//�Զ�ƥ�䵱ǰ��ŵĹ��������Ӧ����ִ��ɱ�ּ���
	for(%a = 0; %a < %MonsterKill_Mid; %a++)
	{
		%Mid = $MissionData_MonsterKill[%MonsterID, %a + 1];

//		echo("%Mid = "@%Mid);

		if(%Player.IsAcceptedMission(%Mid))
			for(%i = 0; %i < 50; %i++)
			{
//				echo("Yes = 1");
				if(%Player.GetMissionFlag(%Mid, 3100 + %i) > 0)
					{
//						echo("Yes = 2");
						if(%Player.GetMissionFlag(%Mid, 3100 + %i) $= %MonsterID)
							{
//								echo("Yes = 3");
								%MaxFlag = %Player.GetMissionFlag(%Mid, 3200 + %i);
								%MinFlag = %Player.GetMissionFlag(%Mid, 3300 + %i);

//								echo("%MaxFlag = "@%MaxFlag);
//								echo("%MinFlag = "@%MinFlag);

								if( %MinFlag < %MaxFlag )
									{
//										echo("Yes = 4");
										%Player.SetMissionFlag(%Mid, 3300 + %i, %MinFlag + 1, true); //����,���ﵱǰ����
									}
							}
					}
					else
						break;
			}
	}

	if((%MonsterID $="410400037")||(%MonsterID $="410400041"))
	  {
	  		if(%Player.IsAcceptedMission(10749))
	  			{
	  					 %Flag = %Player.GetFlagsByte(10);

		  			   if(%Flag == 5)

				  		{
				  				SpNewNpc3(%Player, 410400043, "44.3506 -23.5513 119.717", %CopyMapID,0,"2 2 2");
				  				%Player.SetFlagsByte(10,%Flag++);
				  		}
		  				else

		  				{
				  				%Player.SetFlagsByte(10,%Flag++);
		  				}

	  	    }
	  }

	 if((%MonsterID $="410700022")||(%MonsterID $="410700023")||(%MonsterID $="410700024"))
	   {
	  	  %Mid = 10109;

	  		if(%Player.IsAcceptedMission(%Mid))
	  			{
						for(%i = 0; %i < 50; %i++)
						   {
								if(%Player.GetMissionFlag(%Mid, 3100 + %i) > 0)
									{

										if(%Player.GetMissionFlag(%Mid, 3100 + %i) $= %MonsterID)
											{

												%MaxFlag = %Player.GetMissionFlag(%Mid, 3200 + %i);
												%MinFlag = %Player.GetMissionFlag(%Mid, 3300 + %i);

												if( %MinFlag < %MaxFlag )
													{
														%Player.SetMissionFlag(%Mid, 3300 + %i, %MinFlag + 1, true); //����,���ﵱǰ����
													}
											}
									 }
										else
											break;
						   }

				   }

	    }

	   if(%MonsterID $="410700022")
	     {
	  	    %Mid = 10109;
	        %CopyMapID = %Player.GetLayerId();

	        if(%Player.IsAcceptedMission(%Mid))
	        	{
		  				if(%Player.GetMissionFlag(%Mid, 3300) == 4)
		  					{
		  						if(%Player.GetFlagsByte(25) ==0)
		  							{
		  	              %Player.SetFlagsByte(25,1);

						  				SpNewNpc3(%Player, 410700024, "-1.24882 -4.7772 133.91", %CopyMapID,0,"2 2 2");

						  			}
						  	}
						}
			}

	 if((%MonsterID $="410900001")||(%MonsterID $="410900002"))
	   {
	  	  %Mid = 10121;
	  	  %CopyMapID = %Player.GetLayerId();

	  	  if(%Player.IsAcceptedMission(%Mid))
	  			{
	  				if((%Player.GetMissionFlag(%Mid, 3300) == 1)&&(%Player.GetMissionFlag(%Mid, 3301) == 3))
	  					{
	  						if(%Player.GetFlagsByte(26) ==0)
	  							{
	  								%Player.SetFlagsByte(26,1);

				  		  		SpNewNpc3(%Player, 401301001, "-0.234988 -11.3923 140.351", %CopyMapID,0,"2 2 2");
				  		  	}
				  		}
	  			}
	  }

	if( (%MonsterID $="410100001")||(%MonsterID $="410200001")||(%MonsterID $="410300001")||(%MonsterID $="410400001")||
			(%MonsterID $="410500001")||(%MonsterID $="410600001")||(%MonsterID $="410700001")||(%MonsterID $="410800001") )
		{
			%Mid = 10125;

			if(%Player.IsAcceptedMission(%Mid))
				{
					for(%i = 0; %i < 50; %i++)
					{
						if(%Player.GetMissionFlag(%Mid, 3100 + %i) > 0)
							{

								if(%Player.GetMissionFlag(%Mid, 3100 + %i) $= %MonsterID)
									{

										%MaxFlag = %Player.GetMissionFlag(%Mid, 3200 + %i);
										%MinFlag = %Player.GetMissionFlag(%Mid, 3300 + %i);

										if( %MinFlag < %MaxFlag )
											{
												%Player.SetMissionFlag(%Mid, 3300 + %i, %MinFlag + 1, true); //����,���ﵱǰ����
											}
									}
							 }
								else
									break;
					 }
				}
		}

	 if(%MonsterID $="410500064"){$New_108020016_1 =0;}
   if(%MonsterID $="410500065"){$New_108020016_2 =0;}
   if(%MonsterID $="410500066"){$New_108020016_3 =0;}

	//�������񴥷�

	for(%a = 0; %a < 1; %a++)
	{
		%Mid = 20001;

//		echo("%Mid = "@%Mid);

		if(%Player.IsAcceptedMission(%Mid))
			for(%i = 0; %i < 50; %i++)
			{
//				echo("Yes = 1");
				if(%Player.GetMissionFlag(%Mid, 3100 + %i) > 0)
					{
//						echo("Yes = 2");
						if(%Player.GetMissionFlag(%Mid, 3100 + %i) $= %MonsterID)
							{
//								echo("Yes = 3");
								%MaxFlag = %Player.GetMissionFlag(%Mid, 3200 + %i);
								%MinFlag = %Player.GetMissionFlag(%Mid, 3300 + %i);

//								echo("%MaxFlag = "@%MaxFlag);
//								echo("%MinFlag = "@%MinFlag);

								if( %MinFlag < %MaxFlag )
									{
//										echo("Yes = 4");
										%Player.SetMissionFlag(%Mid, 3300 + %i, %MinFlag + 1, true); //����,���ﵱǰ����
									}
							}
					}
					else
						break;
			}
	}

}

//�����������������������������������ű�������������������������������������


//����������������������ˢ��NPC��������������������������������������������
//ˢ��Npc
function SpNewNpc(%Player, %NpcID, %Pos, %CopyMapID)
{
	if(%CopyMapID !$= "0")
		%obj = SpawnNpc(%NpcID, %CopyMapID);
	else
		%obj = SpawnNpc(%NpcID);

	if(%Pos !$= "0")
		%obj.SetPosition(%Pos);
	else
		%obj.SetPosition(%Player.GetPosition());

	return %obj;
}

//ˢ��Npc���д��������
function SpNewNpc2(%Player, %NpcID, %Pos, %CopyMapID, %Rot)
{
	if(%CopyMapID !$= "0")
		%obj = SpawnNpc(%NpcID, %CopyMapID);
	else
		%obj = SpawnNpc(%NpcID);

	//�ж��������
	if(%Pos $= "0")
		%Pos = %Player.GetPosition();

	//�жϳ������
	if(%Rot !$= "0")
		%obj.SetPosition(%Pos, %Rot);
	else
		%obj.SetPosition(%Pos);

	return %obj;
}

//ˢ��Npc���д�������������Ŵ����
function SpNewNpc3(%Player, %NpcID, %Pos, %CopyMapID, %Rot, %Scale)
{
	if(%CopyMapID !$= "0")
		%obj = SpawnNpc(%NpcID, %CopyMapID);
	else
		%obj = SpawnNpc(%NpcID);

	//�ж��������
	if(%Pos $= "0")
		%Pos = %Player.GetPosition();

	//�жϳ������
	if(%Rot !$= "0")
		%obj.SetPosition(%Pos, %Rot);
	else
		%obj.SetPosition(%Pos);

	//�ж�ģ�ͷŴ����
	if(%Scale !$= "0")
		%obj.SetScale(%Scale);

	return %obj;
}
//����������������������ˢ��NPC��������������������������������������������

//��ʱ���棬ÿ����ҵĵ�ǰ�����ţ���Ŷ
$Player_Area = 0;

//����������������������ˢ�²ɼ��������������������������������������������
function SpNewCj(%Player, %ID, %Pos, %Rot)
{
//	echo("����������������������ˢ�²ɼ��������������������������������������������");

	%obj = new CollectionObject() {
         canSaveDynamicFields = "1";
         Enabled = "1";
         position = %Pos;	//����"-111.301 -226.354 50.3384"
         rotation = %Rot;	//����"1 0 0 0"
         scale = "1 1 1";
         objBox = "0 0 0 1 1 1";
         dataBlockId = %ID;	//�ɼ�����"552000001"
      };

	return %obj;
}
//����������������������ˢ�²ɼ��������������������������������������������

//������������������������ʱ�������󴥷��ű���������������������������������
//��ʱ�������󴥷��ű�
function OnTimerControlTriggered(%Player, %Index)
{
	switch(%Index)
	{
		case 1:
			%Player.PutItem(105100104, 1);
			%Player.AddItem();
			%Player.AddTimerControl(2,900000,1,0,1,1);
			%Player.SetTimerControl(2,1,"105100105");

		case 2:
			%Player.PutItem(105100105, 1);
			%Player.AddItem();
			%Player.AddTimerControl(3,1800000,1,0,1,1);
			%Player.SetTimerControl(3,1,"105100106");

		case 3:
			%Player.PutItem(105100106, 1);
			%Player.AddItem();
			%Player.AddTimerControl(4,3600000,1,0,1,1);
			%Player.SetTimerControl(4,1,"105100107");

		case 4:
			%Player.PutItem(105100107, 1);
			%Player.AddItem();
	}
}

//������������������������ʱ�������󴥷��ű���������������������������������

//�������������������������͵�ͼ�ű�����������������������������������������

//������ͨ��ͼ�ű�
function GoToNextMap_Normal(%Player, %MapId)
{
	//����ڸ����ڣ���ʹ�÷���֮ǰ��������
	if(GetSubStr(GetZoneID(), 1, 1) $= "3")
		SptCopymap_RemovePlayer( %Player.GetPlayerID() );
	else
		eval($TransportObject[ %MapId ]);
}

//���͸�����ͼ�ű�
function GoToNextMap_CopyMap( %Player, %MapID )
{
	if(GetSubStr(GetZoneID(), 1, 1) $= "3")
		SptCopymap_RemovePlayer( %Player.GetPlayerID() );
	else
		{
		  if($TransportObject[ %MapId ] !$= "")
		  	SptCopyMap_TransportPlayer(%Player.GetPlayerID(), %MapID, $TransportObject[ %MapId ], 0);
		}
}

//���е㹦��
function FlyMeToTheMoon(%Player, %FlyID)
{
	%MountId = 720000101;

//	if(%FlyID == 10010101){%MountId = 720000101;} //���½�����д�������
//	if(%FlyID == 10010102){%MountId = 720000101;} //����ͷ���д��͵�����
//	if(%FlyID == 10010103){%MountId = 720000101;} //�����Ƿ��д�������
//	if(%FlyID == 10010104){%MountId = 720000101;} //�ξ��η��д��͵�����
//	if(%FlyID == 11010101){%MountId = 720000101;} //�����Ƿ��д��͵������
//	if(%FlyID == 11010102){%MountId = 720000101;} //����ͷ���д�������
//	if(%FlyID == 11010103){%MountId = 720000101;} //�ξ��η��д�������
//	if(%FlyID == 11270001){%MountId = 720000101;} //��������д��͵������
//	if(%FlyID == 10010005){%MountId = 720000101;} //����Ƿ��д��͵����ľ�
//	if(%FlyID == 10010006){%MountId = 720000101;} //���ľ����д�������
//	if(%FlyID == 10020001){%MountId = 720000101;} //����Ƿ��д�������
//	if(%FlyID == 10020002){%MountId = 720000101;} //���ľ����д��͵������

	if( (%FlyID > 0)&&(%MountId > 0) )
		%Player.SetFlyPath(%FlyID, %MountId);
}

//�������������������������͵�ͼ�ű�����������������������������������������

//��ʱţ��
function WuDi(%Player)
{
	%Player.AddBuff(340010120 ,0);
	%Player.AddBuff(340020120 ,0);
	%Player.AddBuff(340030120 ,0);
	%Player.AddBuff(340040120 ,0);
	%Player.AddBuff(340050120 ,0);
	%Player.AddBuff(340060120 ,0);
	%Player.AddBuff(340070120 ,0);
	%Player.AddBuff(313050100 ,0);
	%Player.AddBuff(313090050 ,0);
//	%Player.AddBuff(309100050 ,0);
	%Player.AddBuff(309110050 ,0);
	%Player.AddBuff(309130050 ,0);
	%Player.AddBuff(313120070 ,0);

	%Player.AddBuff(313150005 ,0);
	%Player.AddBuff(307250005 ,0);
	%Player.AddBuff(313180020 ,0);
	%Player.AddBuff(309700100 ,0);

	%Player.AddBuff(307100100 ,0);
	%Player.AddBuff(313010124 ,0);
	%Player.AddBuff(309010050 ,0);

	if(%Player.GetLevel() > 0)
		Schedule(9000, 0, "WuDi", %Player);
}
//��������
function JiaSu(%Player)
{
	%Player.AddBuff(309100050 ,0);

	if(%Player.GetLevel() > 0)
		Schedule(9000, 0, "JiaSu", %Player);
}

//�������������������������ֽ�ɫ��ʼ���ű�����������������������������������
//���ֽ�ɫ��ʼ���ű���ÿ����ɫֻ�ڵ�һ�ν�����Ϸ����ʱ������
function NewbieInit(%Player)
{
	//��Ҵ����½�ɫ�����ϼ�Я������
	%Player.PutItem(105100001, 1);	//����֮��
	%Player.AddItem();

	//�����Ʒ��װ����
	%Player.AddItemToEquip(102048010);	//�ֲ���  �粿
	%Player.AddItemToEquip(102078010);	//��Ь    �Ų�
	%Player.AddItemToEquip(102058010);	//����    �ֲ�

	//��Ӽ���
	%Player.AddSkill(200330001);	//��Ծ����
	%Player.AddSkill(200030001);	//��׽����

	//��������
	LearnLivingSkill(%Player,507011001);	//��⿼���
	LearnLivingSkill(%Player,520011001);	//ĳ�������
	LearnLivingSkill(%Player,520011002);	//��ü���-����

	//�����Ʒ/����/�����/����/�����󵽿����
	%Player.AddObjectToPanel(3, 507011001, 7);//8����⿼���
	%Player.AddObjectToPanel(2, 200330001, 8);//9����Ծ����
	%Player.AddObjectToPanel(2, 200030001, 9);//0����׽����


//	%isSucceed = %player.addObjectToPanel(%type,%id,%num);
//	%type Ϊ1-��Ʒ 2-���� 3-����� 4-���� 5-���
//	%id Ϊ��Ӧ����ID
//	%num Ϊ�����λ�ã�0��ʼ��0=1,1=2...

}
//�������������������������ֽ�ɫ��ʼ���ű�����������������������������������


//�����������������������ɼ��¼����ýű�������������������������������������
//�ɼ�����ǰ���жϽű�
function EventTriggerCondition(%Player, %nEventTriggerId)
{
//	echo("EventTriggerCondition in =======" @ %nEventTriggerId);
	
	switch$(%nEventTriggerId)
	{
		case "552000061":		//���������顿����״�ɼ���
			%Txt = "";
			if($FamilyName[%Player.GetFamily(), 3] $= GetZoneID())
				{
					if(%Player.GetLevel() >= 30)
						{
							if($HuoDon_CWTS == 1)
								return true;
							else
								%Txt = "���������顿��Ѿ�����";
						}
						else
							%Txt = "���ĵȼ�������������Ҫ30�����ܲ������ڵ�Ů�������ӡ";
				}
				else
					%Txt = "�����ܲ����������ڵ�Ů�������ӡ������Լ����ڲ���";
			
			if(%Txt !$= "")
				{
					SendOneChatMessage(0, %text, %Player);
					SendOneScreenMessage(2, %text, %Player);
				}

		case 2:		//�ű�����
			return false;
		
		default:
			return true;
	}
}
//�ɼ��ɹ�����ýű�
function OnServerTriggerEvent(%Player, %EventID)
{
//	echo("%EventID = "@%EventID);
	switch$(%EventID)
	{
		case "552000027":Mission_Event_10532(%Player, %EventID);			//����1
		case "552000028":Mission_Event_10581(%Player, %EventID);			//����2
		case "552000038":Mission_Event_10518(%Player, %EventID);			//��ɫ���
		case "552000039":Mission_Event_10519(%Player, %EventID);			//��ɫ���
		case "552000040":Mission_Event_10520(%Player, %EventID);			//��ɫ���
		case "552000041":Mission_Event_10530(%Player, %EventID);			//������
		case "552000042":Mission_Event_10532_1(%Player, %EventID);		//��˻����к�
		case "552000043":Mission_Event_10542(%Player, %EventID);			//��˻�
		case "552000044":Mission_Event_10544(%Player, %EventID);			//������������
		case "552000047":Mission_Event_10711(%Player, %EventID);			//�������֦
		case "552000048":Mission_Event_10903_1(%Player, %EventID);		//���
		case "552000049":Mission_Event_10907(%Player, %EventID);			//����ʬ��
		case "552000050":Mission_Event_10915(%Player, %EventID);			//�⵶
		case "552000051":Mission_Event_10903_2(%Player, %EventID);		//����
		case "552000054":Mission_Event_10114(%Player, %EventID);			//�ɺײ�
		case "552000055":Mission_Event_10116(%Player, %EventID);			//��������
			
		case "552000061":		//���������顿����״�ɼ���
			if(%Player.GetBuffCount(20015) > 0)
				%Exp = 10;
			else
				%Exp = 20;
			
			%Player.AddExp($Monster_Exp[%Player.GetLevel() ,1] / %Exp);
			%Player.AddBuff(390230001, 0);
			
			if(%Player.GetBuffCount(39023) >= 1200)
				if(%Player.CompareFlagsDate(100,0,0,1,0,0,0))
					if(AddItemToInventory(%Player.GetPlayerID(), 105030014, 1))
						%Player.SaveFlagsDate(100);
								
		default:
			if((%EventID $= "552000022")||(%EventID $= "552000023")||(%EventID $= "552000024")){Mission_Event_10787(%Player, %EventID);}//��ʯ
	}

	//�Զ��ɼ������ڵ���
	%Item_and_Jilv = $Item_CaiJi[%EventID];

//	echo("============================================");
//	echo("%Item_and_Jilv = "@%Item_and_Jilv);

	//���ñ���
	%Item_Num = 0;//���п��ܸ��ڵĵ�������
	%Jilv_All = 0;//���м����ܺ�

	//�Ӳɼ�������л�ȡ����
//	echo("---------------------");
	for(%i = 0;%i < 9;%i++)
	{
		%Item[%i + 1] = GetWord(%Item_and_Jilv, %i * 2);
		%Jilv[%i + 1] = GetWord(%Item_and_Jilv, %i * 2 + 1);

		if((%Item[%i + 1] !$= "")&&(%Jilv[%i + 1] !$= ""))
			{
				%Item_Num = %Item_Num + 1;	//���ܸ��ڵĵ���+1
				%Jilv_All = %Jilv_All + %Jilv[%i + 1];	//�����ܺ�����

//				echo("%i = "@%i@" = "@%Item[%i + 1]@" = "@%Jilv[%i + 1]);
			}
			else
				break;
	}

	if( (%Item_Num > 0)&&(%Jilv_All > 0) )
		{
//			echo("%Item_Num = "@%Item_Num);
//			echo("%Jilv_All = "@%Jilv_All);

			//���������
			%a = GetRandom(1,%Jilv_All);

//			echo("%a = "@%a);

			%Jilv_Num = 0;

//			echo("+++++++++++++++++++++");
			for(%ii = 1;%ii <= %Item_Num;%ii++)
			{
				%Jilv_Num = %Jilv_Num + %Jilv[%ii];

//				echo("%ii = "@%ii@" = "@%Jilv_Num);

				if(%a <= %Jilv_Num)
					{
//						echo("%Item = "@%Item[%ii]);
						%Player.PutItem(%Item[%ii],1);
						break;
					}
					else
						continue;
			}

			//���͵���
			%Player.AddItem();

			//�۳���Ӧ����

		}
}

function Mission_Event_10114(%Player, %EventID)	//�ɺײ�
{
	%Mid = 10114;

	 if(%EventID $= "552000054")
	   {
	  	 if(%Player.IsAcceptedMission(%Mid))
	  	 	 {
		  	 	 if(%Player.GetItemCount(107019001)== 0)
		  	 	 	 {
			  	 	 	 	SendOneChatMessage(0,"<t>��Ҫ��ҩ����</t>", %Player);
							  SendOneScreenMessage(2,"��Ҫ��ҩ����", %Player);
		         }
					 else	if(%Player.GetMissionFlag(%Mid,1300) == 0)
					  			{
						  			 %ItemAdd = %Player.PutItem(108020267,1);
										 %ItemAdd = %Player.AddItem();

										 if(!%ItemAdd)
											 {
													SendOneChatMessage(0,"<t>��������</t>",%Player);
													SendOneScreenMessage(2,"��������", %Player);

													return;
											 }

						  			%Player.SetMissionFlag(%Mid, 1300, 1, true);
					  	  	}
		  	  }
	   }

}

function Mission_Event_10116(%Player, %EventID)	//��������
{
	%Mid = 10116;

	 if(%EventID $= "552000055")
	   {
	  	 if(%Player.IsAcceptedMission(%Mid))
	  	 	 if(%Player.GetItemCount(108020269)== 1)
		  		  if(%Player.GetMissionFlag(%Mid,2300) == 0)
		  			 {

		  			 	  %ItemAdd = %Player.PutItem(108020269,-1);
		  			 		%ItemAdd = %Player.PutItem(108020270,1);
								%ItemAdd = %Player.AddItem();

								if(!%ItemAdd)
									{
										SendOneChatMessage(0,"<t>��������</t>",%Player);
										SendOneScreenMessage(2,"��������", %Player);

										return;
								  }
                %Player.SetMissionFlag(%Mid, 1300, 1, true);
		  					%Player.SetMissionFlag(%Mid, 2300, 1, true);
		  			 }
	   }

}

function Mission_Event_10518(%Player, %EventID) //��ɫ���
{
	%Mid = 10518;

  if(%EventID $= "552000038")
  	{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020135)== 6))
				{
					if(%Player.GetMissionFlag(%Mid,1300) == 0)
						{
								if($New_552000038 ==0)
								  {
								  	%ItemAdd = %Player.PutItem(108020135, -6);
									  %ItemAdd = %Player.AddItem();

									  %Player.SetMissionFlag(%Mid, 2100, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2200, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2300, "");		//����,ITEM���䵱ǰ����-ȡ��

										$New_552000038 = SpNewNpc(%Player,401103012,"84.0266 -25.1422 70.3982",0);
										%Player.SetMissionFlag(%Mid, 1300, 1, true);
				            Schedule(60000, 0, "RemoveNpc_552000038");
			           }
			           	%Player.UpdateMission(%Mid);					//����������Ϣ
					  }
					if(%Player.GetMissionFlag(%Mid,1300) == 1)
				  	{
					  	if($New_552000038 ==0)
								{
									$New_552000038 = SpNewNpc(%Player,401103012,"84.0266 -25.1422 70.3982",0);
									%Player.SetMissionFlag(%Mid, 1300, 1, true);
					        Schedule(60000, 0, "RemoveNpc_552000038");

								}
						}
				}
		}
}

function RemoveNpc_552000038()
{
	$New_552000038.SafeDeleteObject();
	$New_552000038= 0;
}

function Mission_Event_10519(%Player, %EventID) //��ɫ���
{
	%Mid = 10519;

  if(%EventID $= "552000039")
  	{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020136)== 6))
				{
					if(%Player.GetMissionFlag(%Mid,1300) == 0)
						{
							if($New_552000039 ==0)
								{
									  %ItemAdd = %Player.PutItem(108020136, -6);
								  	%ItemAdd = %Player.AddItem();

								  	%Player.SetMissionFlag(%Mid, 2100, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2200, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2300, "");		//����,ITEM���䵱ǰ����-ȡ��

										$New_552000039 = SpNewNpc(%Player,401103024,"148.993 -15.8033 70.2211",0);
										%Player.SetMissionFlag(%Mid, 1300, 1, true);
				            Schedule(60000, 0, "RemoveNpc_552000039");
			          }
			          %Player.UpdateMission(%Mid);					//����������Ϣ
						}
				  if(%Player.GetMissionFlag(%Mid,1300) == 1)
				  	{
					  	if($New_552000039 ==0)
								{
									$New_552000039 = SpNewNpc(%Player,401103024,"148.993 -15.8033 70.2211",0);
									%Player.SetMissionFlag(%Mid, 1300, 1, true);
					        Schedule(60000, 0, "RemoveNpc_552000039");

								}
						}
				 }
	 	}
}

function RemoveNpc_552000039()
{
	$New_552000039.SafeDeleteObject();
	$New_552000039= 0;
}

function Mission_Event_10520(%Player, %EventID) //��ɫ���
{
	%Mid = 10520;

  if(%EventID $= "552000040")
  	{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020137)== 6))
				{
					if(%Player.GetMissionFlag(%Mid,1300) == 0)
						{
							if($New_552000040 ==0)
								{
									  %ItemAdd = %Player.PutItem(108020137, -6);
										%ItemAdd = %Player.AddItem();

										%Player.SetMissionFlag(%Mid, 2100, "");		//����,ITEM���-ȡ��
										%Player.SetMissionFlag(%Mid, 2200, "");		//����,ITEM��������-ȡ��
										%Player.SetMissionFlag(%Mid, 2300, "");		//����,ITEM���䵱ǰ����-ȡ��

										$New_552000040 = SpNewNpc(%Player,401103025,"128.2 5.44221 75.0257",0);
										%Player.SetMissionFlag(%Mid, 1300, 1, true);
				            Schedule(60000, 0, "RemoveNpc_552000040");
			          }
			          %Player.UpdateMission(%Mid);					//����������Ϣ
						}
					if(%Player.GetMissionFlag(%Mid,1300) == 1)
				  	{
					  	if($New_552000039 ==0)
								{
									$New_552000039 = SpNewNpc(%Player,401103025,"128.2 5.44221 75.0257",0);
									%Player.SetMissionFlag(%Mid, 1300, 1, true);
					        Schedule(60000, 0, "RemoveNpc_552000040");

								}
						}
				}
	 	}
}

function RemoveNpc_552000040()
{
	$New_552000040.SafeDeleteObject();
	$New_552000040= 0;
}

function Mission_Event_10530(%Player, %EventID)//������
{
	%Mid =10530;

	if(%EventID $= "552000041")
		{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020156)== 1))
				 if(%Player.GetMissionFlag(%Mid,1300) == 0)
				 	  %Player.SetMissionFlag(%Mid, 1300, 1, true);

		}

}

function Mission_Event_10542(%Player, %EventID)//��˻�
{
	%Mid =10542;

	if(%EventID $= "552000043")
		{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020162)== 1))
				 if(%Player.GetMissionFlag(%Mid,1300) == 0)
				  	%Player.SetMissionFlag(%Mid, 1300, 1, true);

		}

}

function Mission_Event_10532(%Player, %EventID)// ����1
{
  	if(%EventID $= "552000027")
	  	{
	  		if((%Player.IsAcceptedMission(10532))||(%Player.IsAcceptedMission(10581)))
					{
						if((%player.GetItemCount(108020158)==0)&&(%player.GetItemCount(108020168)==0))
				       {
					     		if($New_552000027 ==0)
										{
											$New_552000027 = SpNewNpc(%Player,410600002,0,0);
											Schedule(320000, 0, "RemoveNpc_552000027");
										}
							 }
				   }
			}
}

function RemoveNpc_552000027()
{
	$New_552000027.SafeDeleteObject();
	$New_552000027 = 0;
}

function Mission_Event_10532_1(%Player, %EventID)//��˻����к�
{
	%Mid =10532;

	if(%EventID $= "552000042")
		{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020158)== 1))
				if(%Player.GetMissionFlag(%Mid,1300) == 0)
				 	 %Player.SetMissionFlag(%Mid, 1300, 1, true);

		}

}

function Mission_Event_10544(%Player, %EventID)//������������
{
	%Mid =10544;

	if(%EventID $= "552000044")
		{
			if((%Player.IsAcceptedMission(%Mid))&&(%Player.GetItemCount(108020165)== 1))
				{
					if(%Player.GetMissionFlag(%Mid,1300) == 0)
						{
							if($New_552000044 ==0)
								{
									$New_552000044 = SpNewNpc(%Player,401103022,0,0);
									%Player.SetMissionFlag(%Mid, 1300, 1, true);
									Schedule(60000, 0, "RemoveNpc_552000044");

							  }
						}
			  }
		}
}

function RemoveNpc_552000044()
{
	$New_552000044.SafeDeleteObject();
	$New_552000044 = 0;
}

function Mission_Event_10581(%Player, %EventID) // ����2
{
	%Mid =10581;

	if(%EventID $= "552000028")
		{
			if((%Player.IsAcceptedMission(10581))&&(%player.GetItemCount(108020169)==0))
	    	{
					  if(%EventID $= "552000028")
						{
						   if($New_552000028 ==0)
								 {
										$New_552000028 = SpNewNpc(%Player,410600005,0,0);
										Schedule(320000, 0, "RemoveNpc_552000028");
								 }
						}
	      }
	  }
}

function RemoveNpc_552000028()
{
	$New_552000028.SafeDeleteObject();
	$New_552000028 = 0;
}

function Mission_Event_10711(%Player, %EventID) //�������֦
{
	%Mid =10711;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%EventID $= "552000047")
				{
					%Zr = GetRandom(1,2);
					%Cs = %Player.GetMissionFlag(%Mid,1300);

					if((%Zr ==1)&&(%Cs==0)){%Player.SetMissionFlag(%Mid, 1300, 1, true);}
					if((%Zr ==1)&&(%Cs==1)){%Player.SetMissionFlag(%Mid, 1300, 2, true);}
					if((%Zr ==1)&&(%Cs==2)){%Player.SetMissionFlag(%Mid, 1300, 3, true);}
					if(%Zr ==2){SpNewNpc(%Player,410600005,0,0);}

				 }
		}

}

function Mission_Event_10787(%Player, %EventID) //��ʯ
{
	%Mid =10787;

	if(%Player.IsAcceptedMission(%Mid))
		{
			if(%EventID $= "552000022")
				{
					if(%Player.GetMissionFlag(%Mid, 1300) == 0)
						{
							if((%Player.GetMissionFlag(%Mid, 1301) == 1)&&(%Player.GetMissionFlag(%Mid, 1302) == 1))
								{
									SpNewNpc3(%Player, 410600019, "-146.504 39.5251 127.018", 0,0,"2 2 2");

									%Player.SetMissionFlag(%Mid, 1300, 1, true);
								}
					    else
					        %Player.SetMissionFlag(%Mid, 1300, 1, true);
					  }
				 }

		  if(%EventID $= "552000023")
				{
			   	if(%Player.GetMissionFlag(%Mid, 1301) == 0)
						{
							if((%Player.GetMissionFlag(%Mid, 1300) == 1)&&(%Player.GetMissionFlag(%Mid, 1302) == 1))
								{
									 SpNewNpc3(%Player, 410600019, "-146.504 39.5251 127.018", 0,0,"2 2 2");
									 %Player.SetMissionFlag(%Mid, 1301, 1, true);
								}
					    else
					       	 %Player.SetMissionFlag(%Mid, 1301, 1, true);

					 }
			  }

		 if(%EventID $= "552000024")
			  {
					if(%Player.GetMissionFlag(%Mid, 1302) == 0)
						{
								if((%Player.GetMissionFlag(%Mid, 1300) == 1)&&(%Player.GetMissionFlag(%Mid, 1301) == 1))
									{
										SpNewNpc3(%Player, 410600019, "-146.504 39.5251 127.018", 0,0,"2 2 2");
										%Player.SetMissionFlag(%Mid, 1302, 1, true);
									}
								else
										%Player.SetMissionFlag(%Mid, 1302, 1, true);
						}
				}
		}
}

function Mission_Event_10903_1(%Player, %EventID) //���
{
	%Mid =10903;

	if(%Player.IsAcceptedMission(%Mid))
		{
				if(%EventID $= "552000048")
					{
							%ItemAdd = %Player.PutItem(108020212, 1);
							%ItemAdd = %Player.AddItem();

							if(!%ItemAdd)
								{
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);

									return;
								}

					}
	}
}

function Mission_Event_10903_2(%Player, %EventID) //����
{
	%Mid =10903;

	if(%Player.IsAcceptedMission(%Mid))
	{
			if(%EventID $= "552000051")
				{
					%SJ = GetRandom(1,5);

					if(%SJ==1)
					  {
					  	%ItemAdd = %Player.PutItem(108020211, 1);
							%ItemAdd = %Player.AddItem();
							if(!%ItemAdd)
								{
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);

									return;
								}
					  }

					if(%SJ ==2)
					  {
						  if($New_552000051 ==0)
								{
									$New_552000051 = SpNewNpc(%Player,410600030,0,0);
									Schedule(320000, 0, "RemoveNpc_552000051");
								}
					  }

					if(%SJ ==3)
					  {
						  if($New_552000051 ==0)
								{
									$New_552000051 = SpNewNpc(%Player,410600030,0,0);
									Schedule(320000, 0, "RemoveNpc_552000051");
								}
					  }

					if(%SJ ==4)
					  {
						  if($New_552000051 ==0)
								{
									$New_552000051 = SpNewNpc(%Player,410600030,0,0);
									Schedule(320000, 0, "RemoveNpc_552000051");
								}
					  }

					if(%SJ ==5)
					  {
						  %ItemAdd = %Player.PutItem(108020213, 1);
							%ItemAdd = %Player.AddItem();
							if(!%ItemAdd)
								{
									SendOneChatMessage(0,"<t>��������</t>",%Player);
									SendOneScreenMessage(2,"��������", %Player);
									return;
								}
					  }
				}
	 }

}

function RemoveNpc_552000051()
{
	$New_552000051.SafeDeleteObject();
	$New_552000051= 0;
}


function Mission_Event_10907(%Player, %EventID) //����ʬ��
{
	%Mid =10907;

	if(%Player.IsAcceptedMission(%Mid))
		{
				if((%EventID $= "552000049")&&(%Player.GetItemCount(108020216)== 1))
					{
						%Zr = GetRandom(1,2);
						if(%Zr ==1)
							{
									if(($New_552000049_1==0)&&(%Player.GetMissionFlag(%Mid, 1300) != 5))
										{
											$New_552000049_1= SpNewNpc(%Player,401105003,0,0);
											Schedule(320000, 0, "RemoveNpc_552000049_1");

										}
							}

						if(%Zr ==2)
							{
								if($New_552000049_2==0)
									{
										$New_552000049_2= SpNewNpc(%Player,410500001,0,0);
										Schedule(320000, 0, "RemoveNpc_552000049_2");

									}
							}
		     }
		 }
}

function RemoveNpc_552000049_1()
{
	$New_552000049_1.SafeDeleteObject();
	$New_552000049_1= 0;
}

function RemoveNpc_552000049_2()
{
	$New_552000049_2.SafeDeleteObject();
	$New_552000049_2= 0;
}


function Mission_Event_10915(%Player, %EventID) //�⵶
{
	%Mid =10915;

	if(%Player.IsAcceptedMission(%Mid))
		{
				if((%EventID $= "552000049")&&(%Player.GetMissionFlag(%Mid, 1300) == 0))
					{
						%Player.SetMissionFlag(%Mid, 1300, 1, true);
					}
		}

}

//�����������������������ɼ��ɹ�����ýű�����������������������������������

//������������������������ɫ�ɹ���¼����������������������������������������
function SvrEventLogin(%Player)
{
}
//������������������������ɫ�ɹ���¼����������������������������������������

//������������������������ɫ�л��̡߳���������������������������������������
function SvrEventSwitchLine(%Player)
{
}
//������������������������ɫ�л��̡߳���������������������������������������

//������������������������ɫ�л���ͼ����������������������������������������
function SvrEventTransport(%Player)
{
}
//������������������������ɫ�л���ͼ����������������������������������������


//������������������������ɫ������������������������������������������������
function SvrEventLevelUp(%Player)
{
	echo("��ҡ�"@%Player.GetPlayerName()@"����������"@%Player.GetLevel()@"��");

	AddEffect(1, %Player, $SP_Effect[5]);	//����������Ч

	if(%Player.GetLevel() < 10)
		{
			if(%Player.IsAcceptedMission(10000))
				{
					%Player.SetMissionFlag(10000, 8100, %Player.GetLevel(), true);
				}
		}

  if(%Player.GetLevel() >= 10)
		{
		  if(%Player.IsAcceptedMission(10000))
				{
		     	%Player.SetMissionFlag(10000, 8100, 10, true);
		    }
		}

	if(%Player.GetLevel() == 7 ){LotRequest( %Player.GetPlayerID() );}
	if(%Player.GetLevel() == 30){AddLotRequest( %Player.GetPlayerID() );}

}
//������������������������ɫ������������������������������������������������

//������������������������ɫ������������������������������������������������
function SvrEventOnDisabled(%Player)
{
//	�����������ȥ
//	1401	8.2923 -87.8359 103.505
}
//������������������������ɫ������������������������������������������������

//������������������������ɫ�����������������������������������������������
function SvrEventOnEnabled(%Player)
{
}
//������������������������ɫ�����������������������������������������������


//�����������������������ض�����ĳЩ��������Ľű���������������������������
//�ض�����ĳЩ��������Ľű�����
function FilterShareMission(%Player)
{
	%count = %Player.GetShareMissionCount();	//��ȡ������п��Թ������������
	for(%i = 0; %i < %count; %i++)
	{
		%Mid = %Player.GetShareMission(%i);	//��ȡָ��λ�õĹ���������
		if(%Mid != -1)
			{
				//�ж��Ƿ�Ϊ�»�����
				if(GetMissionKind(%Mid) == 1)
					%Player.DelShareMission(%Mid);	//ɾ��ָ������������
			}
	}
//	%Player.addShareMission(%Mid);	//���ָ������������
}
//�����������������������ض�����ĳЩ��������Ľű���������������������������


//�������������������������߼����ű�����������������������������������������
function ManualIdentify(%Player, %index, %ItemID, %index1, %type)
{
	if($JianDingFu[ %ItemID ] !$= "")
		{
			%Itemlv = %Player.GetItemlimitlevel(%index);	//Ŀ����ߵȼ�
			if(%Itemlv <= GetWord($JianDingFu[ %ItemID ],0))
				%Player.SetIdentify(%index, GetWord($JianDingFu[ %ItemID ],1), GetWord($JianDingFu[ %ItemID ],2), %index1, %type);
			else
				%WhyNot = "�������ȼ�̫�ͣ��޷������߼���Ŀ����Ʒ";
		}
		else
			%WhyNot = "�������޷�ʹ��";

	if(%WhyNot !$= "")
		{
			SendOneChatMessage(0, "<t>" @ %WhyNot @ "</t>", %Player);
			SendOneScreenMessage(2, %WhyNot, %Player);
		}
}
//�������������������������߼����ű�����������������������������������������

//����������������������������ָ��ʱ��ű�����������������������������������

//updateTop(0);
//����������������������������ָ��ʱ��ű�����������������������������������

//����������������������ѧϰ������Զ�ѧ���䷽����������������������������
function CallbackLearnLivingSkill(%Player,%livingSkillId)
{
	echo("livingSkillId = "@%livingSkillId);

	//ѧ������ܴ����������
	%Mid = 0;
	if(%livingSkillId $= "501031001"){%Mid = 22002;}
	if(%livingSkillId $= "501061001"){%Mid = 22004;}
	if(%livingSkillId $= "501051001"){%Mid = 22006;}
	if(%livingSkillId $= "501041001"){%Mid = 22008;}
	if(%livingSkillId $= "501021001"){%Mid = 22010;}
	if(%livingSkillId $= "501011001"){%Mid = 22012;}

	if(%Mid > 0)
		if(%Player.IsAcceptedMission(%Mid))
			if(%Player.GetMissionFlag(%Mid, 8100) == 0)
				%Player.SetMissionFlag(%Mid, 8100, 1, true);

	if(%livingSkillId $= "502011001")
		{
			AddPrescription(%Player, 612000001);
			AddPrescription(%Player, 612000002);
			AddPrescription(%Player, 612000003);
			AddPrescription(%Player, 612000004);
			AddPrescription(%Player, 612000005);
			AddPrescription(%Player, 612000006);
			AddPrescription(%Player, 612000007);
			AddPrescription(%Player, 612000008);
			AddPrescription(%Player, 612000009);
			AddPrescription(%Player, 612000010);
			AddPrescription(%Player, 612000011);
			AddPrescription(%Player, 612000012);
			AddPrescription(%Player, 612000013);
			AddPrescription(%Player, 612000014);
			AddPrescription(%Player, 612000015);
			AddPrescription(%Player, 612000016);
			AddPrescription(%Player, 612000017);
			AddPrescription(%Player, 612000018);
			AddPrescription(%Player, 612000019);
			AddPrescription(%Player, 612000020);
			AddPrescription(%Player, 612000021);
			AddPrescription(%Player, 612000022);
			AddPrescription(%Player, 612000023);
			AddPrescription(%Player, 612000024);
			AddPrescription(%Player, 612000025);
			AddPrescription(%Player, 612000026);
			AddPrescription(%Player, 612000027);
			AddPrescription(%Player, 612000028);
			AddPrescription(%Player, 612000029);
			AddPrescription(%Player, 612000030);
			AddPrescription(%Player, 612000031);
			AddPrescription(%Player, 612000032);
			AddPrescription(%Player, 612000033);
			AddPrescription(%Player, 612000034);
			AddPrescription(%Player, 612000035);
			AddPrescription(%Player, 612000036);
			AddPrescription(%Player, 612000037);
			AddPrescription(%Player, 612000038);
			AddPrescription(%Player, 612000039);
			AddPrescription(%Player, 612000040);
			AddPrescription(%Player, 630102001);
			AddPrescription(%Player, 630202001);
			AddPrescription(%Player, 630302001);
			AddPrescription(%Player, 630402001);
			AddPrescription(%Player, 630502001);
			AddPrescription(%Player, 630602001);
			AddPrescription(%Player, 630702001);
		}
	if(%livingSkillId $= "503011001")
		{
			AddPrescription(%Player, 612000121);
			AddPrescription(%Player, 612000122);
			AddPrescription(%Player, 612000123);
			AddPrescription(%Player, 612000124);
			AddPrescription(%Player, 612000125);
			AddPrescription(%Player, 612000126);
			AddPrescription(%Player, 612000127);
			AddPrescription(%Player, 612000128);
			AddPrescription(%Player, 612000129);
			AddPrescription(%Player, 612000130);
			AddPrescription(%Player, 612000131);
			AddPrescription(%Player, 612000132);
			AddPrescription(%Player, 612000133);
			AddPrescription(%Player, 612000134);
			AddPrescription(%Player, 612000135);
			AddPrescription(%Player, 612000136);
			AddPrescription(%Player, 612000137);
			AddPrescription(%Player, 612000138);
			AddPrescription(%Player, 612000139);
			AddPrescription(%Player, 612000140);
			AddPrescription(%Player, 612000141);
			AddPrescription(%Player, 612000142);
			AddPrescription(%Player, 612000143);
			AddPrescription(%Player, 612000144);
			AddPrescription(%Player, 612000145);
			AddPrescription(%Player, 612000146);
			AddPrescription(%Player, 612000147);
			AddPrescription(%Player, 612000148);
			AddPrescription(%Player, 612000149);
			AddPrescription(%Player, 612000150);
			AddPrescription(%Player, 612000151);
			AddPrescription(%Player, 612000152);
			AddPrescription(%Player, 612000153);
			AddPrescription(%Player, 612000154);
			AddPrescription(%Player, 612000155);
			AddPrescription(%Player, 612000156);
			AddPrescription(%Player, 612000157);
			AddPrescription(%Player, 612000158);
			AddPrescription(%Player, 612000159);
			AddPrescription(%Player, 612000160);
			AddPrescription(%Player, 630901201);
		}
	if(%livingSkillId $= "504011001")
		{
			AddPrescription(%Player, 612000041);
			AddPrescription(%Player, 612000042);
			AddPrescription(%Player, 612000043);
			AddPrescription(%Player, 612000044);
			AddPrescription(%Player, 612000045);
			AddPrescription(%Player, 612000046);
			AddPrescription(%Player, 612000047);
			AddPrescription(%Player, 612000048);
			AddPrescription(%Player, 612000049);
			AddPrescription(%Player, 612000050);
			AddPrescription(%Player, 612000051);
			AddPrescription(%Player, 612000052);
			AddPrescription(%Player, 612000053);
			AddPrescription(%Player, 612000054);
			AddPrescription(%Player, 612000055);
			AddPrescription(%Player, 612000056);
			AddPrescription(%Player, 612000057);
			AddPrescription(%Player, 612000058);
			AddPrescription(%Player, 612000059);
			AddPrescription(%Player, 612000060);
			AddPrescription(%Player, 631502001);
			AddPrescription(%Player, 631602001);
			AddPrescription(%Player, 631702001);
		}
	if(%livingSkillId $= "505011001")
		{
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
//			AddPrescription(%Player, );
		}
	if(%livingSkillId $= "506011001")
		{
			AddPrescription(%Player, 611920101);
			AddPrescription(%Player, 611920121);
			AddPrescription(%Player, 612000101);
			AddPrescription(%Player, 612000102);
			AddPrescription(%Player, 612000103);
			AddPrescription(%Player, 612000104);
			AddPrescription(%Player, 612000105);
			AddPrescription(%Player, 612000106);
			AddPrescription(%Player, 612000107);
			AddPrescription(%Player, 612000108);
			AddPrescription(%Player, 612000109);
			AddPrescription(%Player, 612000110);
			AddPrescription(%Player, 612000111);
			AddPrescription(%Player, 612000112);
			AddPrescription(%Player, 612000113);
			AddPrescription(%Player, 612000114);
			AddPrescription(%Player, 612000115);
			AddPrescription(%Player, 612000116);
			AddPrescription(%Player, 612000117);
			AddPrescription(%Player, 612000118);
			AddPrescription(%Player, 612000119);
			AddPrescription(%Player, 612000120);
		}
	if(%livingSkillId $= "507011001")
		{
			AddPrescription(%Player, 611810101);
			AddPrescription(%Player, 611810121);
			AddPrescription(%Player, 612000081);
			AddPrescription(%Player, 612000082);
			AddPrescription(%Player, 612000083);
			AddPrescription(%Player, 612000084);
			AddPrescription(%Player, 612000085);
			AddPrescription(%Player, 612000086);
			AddPrescription(%Player, 612000087);
			AddPrescription(%Player, 612000088);
			AddPrescription(%Player, 612000089);
			AddPrescription(%Player, 612000090);
			AddPrescription(%Player, 612000091);
			AddPrescription(%Player, 612000092);
			AddPrescription(%Player, 612000093);
			AddPrescription(%Player, 612000094);
			AddPrescription(%Player, 612000095);
			AddPrescription(%Player, 612000096);
			AddPrescription(%Player, 612000097);
			AddPrescription(%Player, 612000098);
			AddPrescription(%Player, 612000099);
			AddPrescription(%Player, 612000100);
		}
}
//����������������������ѧϰ������Զ�ѧ���䷽����������������������������

//����������������������������Ե�˳ɹ�������ű�����������������������������
//LotRequest( %playerId );  // �͵ȼ���Ҵ���
//AddLotRequest( %playerId );  // �ߵȼ���ҽ��봥��// ��ҵ���30����ÿ���ض�ʱ��ȥ�������������ʱ���ݶ�30����
//RemoveLotRequest( %playerId );  // �ߵȼ���Ҵ����Ƴ�
//HandleLotRequest( %playerId, %destPlayerId ); // �����ɹ�

//�ͼ���Ҵ���
function HandleLotRequest( %PlayerID_Lv1, %PlayerID_Lv99 )
{

}
//�߼���Ҵ���
function HandleLotAck( %PlayerID_Lv99, %PlayerID_Lv1 )
{

}

//����������������������������Ե�˳ɹ�������ű�����������������������������

//�����������������������½�����������������������������������������������
function SvrGetNewbiePosition()
{
	%X = GetRandom(-2, -14);
	%Y = GetRandom(3, 15);

	return %X @ " " @ %Y @ " 130";
}
//�����������������������½�����������������������������������������������

//������������������������������ʱˢ�����а��������������������������������
function CheckServerTime()
{
	%Time = GetLocalTime();

	%YY = GetWord(%Time, 0);	//��
	%MM = GetWord(%Time, 1);	//��
	%DD = GetWord(%Time, 2);	//��
	%H = GetWord(%Time, 3);	//Сʱ
	%M = GetWord(%Time, 4);	//����
	%S = GetWord(%Time, 5);	//��

	//�������а�
	if( (GetZoneID() $= "1001")&&( (%H == 1)&&(%M == 1) ) )
		UpDateTop(0);

	//ÿ��20:00~09:00���˴����ڳ�����===========================���������顿====================
//	if(GetZoneID() $= "1001")
//		{
//			if( (%H == 19)&&(%M == 55) )		//�Ԥ��
//				{
//					%Txt = "���������顿��ÿ��20:00~9:00���������5���Ӻ�ʼ����ҿ����������ڵ������ԣ�����Ů�������ӡ����ͼ�����";
//					SendAllChatMessage(1, %Txt);//����������Ϣ
//					SendAllChatMessage(3, %Txt);//����ȫ����Ϣ
//				}
//			if( (%H == 20)&&(%M == 1) )			//���ʼ
//				{
//					%Txt = "���������顿��ÿ��20:00~9:00����Ѿ���ʼ��������������ڵ������ԣ�����Ů�������ӡ����ͼ�����";
//					SendAllChatMessage(1, %Txt);//����������Ϣ
//					SendAllChatMessage(3, %Txt);//����ȫ����Ϣ
//				}
//		}
//	if( (GetZoneID() $= "1004")||(GetZoneID() $= "1005")||(GetZoneID() $= "1006")||(GetZoneID() $= "1007")||(GetZoneID() $= "1008")||(GetZoneID() $= "1009")||(GetZoneID() $= "1010")||(GetZoneID() $= "1011") )
//		{
//			if( ( (%H >= 20)||(%H < 9) )&&($HuoDong_CWTS == 0) )		//���ʼ
//				{
//					$HuoDong_CWTS = 1;	//���ʼ��ʶ
//					%Map = GetZoneID();
//					$HuoDong_CWTS[1] = SpNewCj(0, 552000061, $HuoDon_CWTS[%Map, 1]     @ " " @ $HuoDon_CWTS[%Map, 2]     @ " " @ $HuoDon_CWTS[%Map, 3], "1 0 0 0");
//					$HuoDong_CWTS[2] = SpNewCj(0, 552000061, $HuoDon_CWTS[%Map, 1] + 5 @ " " @ $HuoDon_CWTS[%Map, 2] + 5 @ " " @ $HuoDon_CWTS[%Map, 3], "1 0 0 0");
//					$HuoDong_CWTS[3] = SpNewCj(0, 552000061, $HuoDon_CWTS[%Map, 1] + 5 @ " " @ $HuoDon_CWTS[%Map, 2] - 5 @ " " @ $HuoDon_CWTS[%Map, 3], "1 0 0 0");
//					$HuoDong_CWTS[4] = SpNewCj(0, 552000061, $HuoDon_CWTS[%Map, 1] - 5 @ " " @ $HuoDon_CWTS[%Map, 2] + 5 @ " " @ $HuoDon_CWTS[%Map, 3], "1 0 0 0");
//					$HuoDong_CWTS[5] = SpNewCj(0, 552000061, $HuoDon_CWTS[%Map, 1] - 5 @ " " @ $HuoDon_CWTS[%Map, 2] - 5 @ " " @ $HuoDon_CWTS[%Map, 3], "1 0 0 0");
//				}
//			if( (%H == 9)&&($HuoDong_CWTS == 1) )		//�����
//				{
//					$HuoDong_CWTS = 0;	//�������ʶ
//					for(%i = 1;%i < 6;%i++)
//					{
//						%DK = $HuoDong_CWTS[ %i ];
//						%DK.SafeDeleteObject();
//					}
//				}
//		}


	//��һ���Ӻ���ִ��ʱ����
	if(%S < 30)
		%Time = ( 30 - %S + 60 ) * (60 - %M);
	else
		%Time = ( 60 - %S + 30 ) * (60 - %M);

	Schedule(%Time @ "000", 0, "CheckServerTime");
}

Schedule(60000, 0, "CheckServerTime");
//������������������������������ʱˢ�����а��������������������������������

//����������������������״̬�����󴥷���������������������������������������
//function BuffOver(%Player, %BuffID)
//{
//	switch(%BuffID)
//	{
//		case 1111:
//	}
//}
//����������������������״̬�����󴥷���������������������������������������

