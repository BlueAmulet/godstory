//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//�����ű���ΪNPC�����Խű������е�ͼ��������
//�ű����������NPC�����������ʾ
//==================================================================================


//���й�������Ĵ����
function Npc_Job(%Npc, %Player, %State , %Conv)
{
	//NPC���ܷ��ࣺ
	//6λ��
		//��λ1���̵���
		//��λ2�����ܽ�����

	//4λ����ͼ���
	//5λ�����͵�ͼָ����


	//���⹦��
//	switch$(%Npc.GetDataID())
//	{
//		case 400001101:		//���ֹ�������	������
//			NowAdd(%Npc, %Player, %State , %Conv);

//		case 400001104:		//����
//			SpawnNewNpc(%Npc, %Player, %State , %Conv);

//	}
	//���깦��
	NpcOpenShop(%Npc, %Player, %State , %Conv);

	//���������Խ���
	NpcOpenJob(%Npc, %Player, %State , %Conv);

	//���������ԶԻ����޽���
	NpcCanDoJob(%Npc, %Player, %State , %Conv);

	//���͹���
	NpcTransportObject(%Npc, %Player, %State , %Conv);

	//���е㹦��
	NpcFly(%Npc, %Player, %State , %Conv);

	//���ָ��ְ���
	NewPlayerHelp(%Npc, %Player, %State , %Conv);

	//���صĹ���
//	PlayGame(%Npc, %Player, %State , %Conv);
}

//���ָ��ְ���
function NewPlayerHelp(%Npc, %Player, %State , %Conv)
{
//--------------------------------------NewPlayerHelp--------State == 0----��----------------------
	switch(%State)
	{
		case 0:
			//�����NPC
			switch$(%Npc.GetDataID())
			{
				case "400001062":		//���ְ�������
					%Conv.AddOption(2210, 2210);	//�����򿪱���
					%Conv.AddOption(2270, 2270);	//������ɶ���
					%Conv.AddOption(2280, 2280);	//��Ϸ�ȼ�һ��
					%Conv.AddOption(2220, 2220);	//�����鿴����
					%Conv.AddOption(2000, 2000);	//���࡭��

				case "400001101":%Conv.AddOption(3400, 3400);	//���ֹ�������	������//���ڼ���ָ��
				case "400001001":%Conv.AddOption(4201, 4201); //����ѧϰ����ҩ��	����
				case "400001002":%Conv.AddOption(4202, 4202); //����ѧϰ���ʳ��	����
				case "400001003":%Conv.AddOption(4203, 4203); //����ѧϰ��������	����
				case "400001004":%Conv.AddOption(4204, 4204); //����ѧϰ�����¿�	����
				case "400001005":%Conv.AddOption(4205, 4205); //����ѧϰ�ӹ�����	����
				case "400001007":
					%Conv.AddOption(4206, 4206); //�����������	����
					%Conv.AddOption(4207, 4207); //����ιʳ����	����
				case "400001008":%Conv.AddOption(4208, 4208); //���޹���˵��	����
				case "400001009":%Conv.AddOption(4209, 4209); //����������	����
				case "400001011":%Conv.AddOption(4210, 4210); //������̯	����
				case "400001017":%Conv.AddOption(4211, 4211); //�����ڿ�	����
				case "400001018":%Conv.AddOption(4212, 4212); //������ľ	����
				case "400001019":%Conv.AddOption(4213, 4213); //��������	����
				case "400001020":%Conv.AddOption(4214, 4214); //������ֲ	����
				case "400001021":%Conv.AddOption(4215, 4215); //������ҩ	����
				case "400001022":%Conv.AddOption(4216, 4216); //��������	����
				case "400001023":%Conv.AddOption(4217, 4217); //������������	����
				case "400001024":%Conv.AddOption(4218, 4218); //���������¿�	����
				case "400001025":%Conv.AddOption(4219, 4219); //�����ӹ�����	����
				case "400001026":%Conv.AddOption(4220, 4220); //������������	����
				case "400001027":%Conv.AddOption(4221, 4221); //�������ɵ�ҩ	����
				case "400001028":%Conv.AddOption(4222, 4222); //�������ʳ��	����
				case "400001029":%Conv.AddOption(4223, 4223); //��������Ҿ�	����
				case "400001030":%Conv.AddOption(4224, 4224); //����������	����
				case "400001046":%Conv.AddOption(4225, 4225); //�����չ��Ԫ��	����
				case "400001049":%Conv.AddOption(4226, 4226); //������ö���	����
				case "400001059":%Conv.AddOption(4227, 4227); //�����Ĺ���	����
				case "400001060":%Conv.AddOption(4228, 4228); //˫�������˵��	����

				case "400001038":															//ʥ���ڽ�����	����ʹ
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "400001039":															//�����ڽ�����	����
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "400001040":															//�����ڽ�����	��Х��
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "400001041":															//�����ڽ�����	���
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "400001042":															//�����ڽ�����	����
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "400001043":															//�����ڽ�����	������
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "400001044":															//�����ڽ�����	����٤��
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "400001045":															//ħ���ڽ�����	ħ��
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��

			}
			//���ľ�NPC
			switch$(%Npc.GetDataID())
			{
				case "401002001":%Conv.AddOption(4201, 4201);	//����ѧϰ����ҩ��	����
				case "401002002":%Conv.AddOption(4202, 4202);	//����ѧϰ���ʳ��	����
				case "401002003":%Conv.AddOption(4203, 4203);	//����ѧϰ��������	����
				case "401002004":%Conv.AddOption(4204, 4204);	//����ѧϰ�����¿�	����
				case "401002005":%Conv.AddOption(4205, 4205);	//����ѧϰ�ӹ�����	����
				case "401002007":%Conv.AddOption(4206, 4206);	//�����������	����
				case "401002007":%Conv.AddOption(4207, 4207);	//����ιʳ����	����
				case "401002008":%Conv.AddOption(4208, 4208);	//���޹���˵��	����
				case "401002009":%Conv.AddOption(4209, 4209);	//����������	����
				case "401002010":%Conv.AddOption(4210, 4210);	//������̯	����
				case "401002011":%Conv.AddOption(4211, 4211);	//�����ڿ�	����
				case "401002012":%Conv.AddOption(4212, 4212);	//������ľ	����
				case "401002013":%Conv.AddOption(4213, 4213);	//��������	����
				case "401002014":%Conv.AddOption(4214, 4214);	//������ֲ	����
				case "401002015":%Conv.AddOption(4215, 4215);	//������ҩ	����
				case "401002016":%Conv.AddOption(4216, 4216);	//��������	����
				case "401002017":%Conv.AddOption(4217, 4217);	//������������	����
				case "401002018":%Conv.AddOption(4218, 4218);	//���������¿�	����
				case "401002019":%Conv.AddOption(4219, 4219);	//�����ӹ�����	����
				case "401002020":%Conv.AddOption(4220, 4220);	//������������	����
				case "401002021":%Conv.AddOption(4221, 4221);	//�������ɵ�ҩ	����
				case "401002022":%Conv.AddOption(4222, 4222);	//�������ʳ��	����
				case "401002023":%Conv.AddOption(4223, 4223);	//��������Ҿ�	����
				case "401002024":%Conv.AddOption(4224, 4224);	//����������	����
				case "401002037":%Conv.AddOption(4225, 4225); //�����չ��Ԫ��	����
				case "401002040":%Conv.AddOption(4226, 4226); //������ö���	����
				case "401002042":%Conv.AddOption(4227, 4227); //�����Ĺ���	����
				case "401002043":%Conv.AddOption(4228, 4228); //˫�������˵��	����

				case "401002029":															//ʥ���ڽ�����	ӭ��ʹ
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "401002030":															//�����ڽ�����	����
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "401002031":															//�����ڽ�����	�׽�
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "401002032":															//�����ڽ�����	����
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "401002033":															//�����ڽ�����	����
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "401002034":															//�����ڽ�����	��С��
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "401002035":															//�����ڽ�����	Ц����
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��
				case "401002036":															//ħ���ڽ�����	��ħ
					%Conv.AddOption(4010, 4010);	//���ڱ���
					%Conv.AddOption(4020, 4020);	//������ɫ
					%Conv.AddOption(4030, 4030);	//��������
					%Conv.AddOption(4040, 4040);	//����ʥ��

				case "410107001":               //������(ʥ��
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
					  		%Conv.AddOption(41010, 3800);

				case "410207001":               //������������
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(%Player.GetSex() == 1)
								if(!%Player.IsAcceptedMission(10123))
									%Conv.AddOption(41020, 3800);

				case "410307001":								//�赤ϼ���ɣ�
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
					    	%Conv.AddOption(41030, 3800);

				case "410407001":								//��ĸ������
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(%Player.GetSex() == 2)
								if(!%Player.IsAcceptedMission(10123))
									%Conv.AddOption(41040, 3800);

				case "410507001":								//��������
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
								%Conv.AddOption(41050, 3800);

				case "410607001":								//��ʯ���֣�
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
								%Conv.AddOption(41060, 3800);

				case "410707001":								//�ļ�������
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
								%Conv.AddOption(41070, 3800);

				case "410807001":								//ħ��ħ��
					if(%Player.GetLevel() > 9)
						if(%Player.GetFamily()==0)
							if(!%Player.IsAcceptedMission(10123))
								%Conv.AddOption(41080, 3800);

			}
//				deflt:
//					i (%Npc.GetDataID() $= "401011003")||			//ʥ���ڴ���ʹ
//						"(%Npc.GetDataID() $= "401007003")||     //�����ڴ���ʹ
//							(%Npc.GetDataID() $= "401010003")||     //�����ڴ���ʹ
//							(%Npc.GetDataID() $= "401009003")||     //�����ڴ���ʹ
//							(%Npc.GetDataID() $= "401005003")||     //�����ڴ���ʹ
//							(%Npc.GetDataID() $= "401008003")||     //�����ڴ���ʹ
//							(%Npc.GetDataID() $= "401006003")||     //�����ڴ���ʹ
//							(%Npc.GetDataID() $= "401004003") )     //ħ���ڴ���ʹ
//						{
//							%Conv.SetText(3501);					//��Ҫɶ��������
//							%Conv.AddOption(3500, 3500);	//�鿴�����ڵ�NPCλ��
//							%Conv.AddOption(3510, 3510);	//��Ҫȥ��������
//							%Conv.AddOption(3520, 3520);	//��Ҫ�س�ȥ
//							%Conv.AddOption(3600, 0);	//�����������
//							%Conv.AddOption(3700, 0);	//��Ҫ��ȡ��������
//							%Conv.AddOption(3530, 3530);	//ûʲô��Ҫ�����ģ�лл
//						}
//					if( (%Npc.GetDataID() $= "401011002")||			//ʥ���ڼ���ʹ
//							(%Npc.GetDataID() $= "401007002")||     //�����ڼ���ʹ
//							(%Npc.GetDataID() $= "401010002")||     //�����ڼ���ʹ
//							(%Npc.GetDataID() $= "401009002")||     //�����ڼ���ʹ
//							(%Npc.GetDataID() $= "401005002")||     //�����ڼ���ʹ
//							(%Npc.GetDataID() $= "401008002")||     //�����ڼ���ʹ
//							(%Npc.GetDataID() $= "401006002")||     //�����ڼ���ʹ
//							(%Npc.GetDataID() $= "401004002") )     //ħ���ڼ���ʹ
//						{
//							%Conv.AddOption(3200, 0);	//�鿴���ڼ���
//						}
	}

//--------------------------------------NewPlayerHelp--------State == 0----��----------------------
//--------------------------------------NewPlayerHelp-------State > 0----��----------------------
	//��������顱
	switch(%State)
	{
		case 2000:
//			%Conv.SetText(2100);	//��������
			%Conv.AddOption(2210, 2210);	//�����򿪱���
			%Conv.AddOption(2220, 2220);	//�����鿴����
			%Conv.AddOption(2230, 2230);	//�����鿴��ͼ
			%Conv.AddOption(2240, 2240);	//������ȡ˫������
			%Conv.AddOption(2250, 2250);	//�����洢�ֿ�
			%Conv.AddOption(2260, 2260);	//������Ӻ���
			%Conv.AddOption(2270, 2270);	//������ɶ���
			%Conv.AddOption(2280, 2280);	//��Ϸ�ȼ�һ��
			%Conv.AddOption(4 , 0 );//����

		default:
			if( (%State >= 2210)&&(%State <= 2280) )
				{
					%Conv.SetText(%State + 1);
					%Conv.AddOption(4 , 2000);//����
				}
	}

	//���ڰ���
	switch(%State)
	{
		case 3400:
			%Conv.SetText(3410);						//������ӵ������������ϣ�����������ư����£�����ά��������
			%Conv.AddOption(3411 , 3420 );	//�ư����£��������������ư������ǻ�����ʲô
			%Conv.AddOption(3412 , 3450 );	//ά��������ǿ�߱������������
			%Conv.AddOption(4 , 0 );//����

		case 3420:
			%Conv.SetText(%State);											//�����������ѡ����ϣ�����һ���ǵ��������ǲ���������
			%Conv.AddOption(%State + 1 , %State + 10 );	//�����������������޴���
			%Conv.AddOption(%State + 2 , %State + 20 );	//���������������Ŀ�����˪��ƽ������
			%Conv.AddOption(4 , 3400 );//����

		case 3450:
			%Conv.SetText(%State);											//�����Ѷ�ս����Ը��Ϊì���ǻ�Ϊ��
			%Conv.AddOption(%State + 1 , %State + 10 );	//ì����Ը��Ϊӭ�����˵�����
			%Conv.AddOption(%State + 2 , %State + 20 );	//�ܣ���Ը��Ϊ�ֵ������Ķ�
			%Conv.AddOption(4 , 3400 );//����

		case 3430:
			%Conv.SetText(%State);									//����ս�ܣ�����������Ļ���ս��ɳ����
			%Conv.AddOption(%State + 1 , 3496 );		//������ģ�ʤ���˳��£����ӱ���10�겻��
			%Conv.AddOption(%State + 2 , 3498 );		//ս��ɳ����ֻ��ս��������ս��
			%Conv.AddOption(4 , 3420 );//����

		case 3440:
			%Conv.SetText(%State);									//��ƽ֮ʱ�����ϲ���������Ի���ϰ��ǿ��
			%Conv.AddOption(%State + 1 , 3493 );		//�������ԣ��������������Ϊ���Ǵ���
			%Conv.AddOption(%State + 2 , 3491 );		//ϰ��ǿ�����ڵ�ǿ��������Ļ���
			%Conv.AddOption(4 , 3420 );//����

		case 3460:
			%Conv.SetText(%State);									//��������֮�֣����ϣ������Ծ����ǹ��䲻��
			%Conv.AddOption(%State + 1 , 3494 );		//����Ծ��������Լ�ʵ��������Ծ��кβ���
			%Conv.AddOption(%State + 2 , 3495 );		//���䲻������ʡһ����ʡһ������ʡһ������ʡһ����
			%Conv.AddOption(4 , 3450 );//����

		case 3470:
			%Conv.SetText(%State);									//��Ը��ǰ��Ϊǿ������ϻ����ں�Ϊ��ʵ������
			%Conv.AddOption(%State + 1 , 3492 );		//���ϣ����Ǳ����뱣���˵���ֱ������
			%Conv.AddOption(%State + 2 , 3497 );		//��������ԸĬĬ���ų�Ϊ������������ĺ�Ԯ
			%Conv.AddOption(4 , 3450 );//����

		case 3500:
			%Conv.SetText(3502);		//�鿴�����ڵ�NPCλ��
			%Conv.AddOption(4 , 0 );//����

		case 3510:
			%Conv.SetText(3519);		//��Ҫȥ��������
			if(GetZoneID() !$= "1011"){%Conv.AddOption(1011, 101101);}	//���������ڵ�ͼ"������(ʥ)"���ع���
			if(GetZoneID() !$= "1007"){%Conv.AddOption(1006, 100601);}	//���������ڵ�ͼ"������(��)"��������
			if(GetZoneID() !$= "1010"){%Conv.AddOption(1007, 100701);}	//���������ڵ�ͼ"������(��)"�����ɾ�
			if(GetZoneID() !$= "1009"){%Conv.AddOption(1005, 100501);}	//���������ڵ�ͼ"�ɻ���(��)"��ľ��
			if(GetZoneID() !$= "1005"){%Conv.AddOption(1009, 100901);}	//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
			if(GetZoneID() !$= "1008"){%Conv.AddOption(1010, 101001);}	//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
			if(GetZoneID() !$= "1006"){%Conv.AddOption(1008, 100801);}	//���������ڵ�ͼ"���鹬(��)"���α���
			if(GetZoneID() !$= "1004"){%Conv.AddOption(1004, 100401);}	//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
			%Conv.AddOption(4 , 0 );//����

		case 3520:
			%Conv.SetText(3529);		//��Ҫ�س�ȥ
			%Conv.AddOption(1001, 100103 );	//�����������
			%Conv.AddOption(4 , 0 );//����

		case 3530:			//ûʲô��Ҫ�����ģ�лл
			%Conv.SetType(4);

		case 3800:			//��Ҫ������

			%ItemAdd = %Player.PutItem(105100102,1);
			%ItemAdd = %Player.AddItem();

			if(!%ItemAdd)
				{
					SendOneChatMessage(0,"<t>��������</t>",%Player);
					SendOneScreenMessage(2,"��������", %Player);

					return;
				}
			if(%Npc.GetDataID() $= "410107001"){%Player.SetFamily(1);}//"������(ʥ)
			if(%Npc.GetDataID() $= "410207001"){%Player.SetFamily(2);}//"������(��)
			if(%Npc.GetDataID() $= "410307001"){%Player.SetFamily(3);}//"������(��)
			if(%Npc.GetDataID() $= "410407001"){%Player.SetFamily(4);}//"�ɻ���(��)
			if(%Npc.GetDataID() $= "410507001"){%Player.SetFamily(5);}//"���Ľ�(��)
			if(%Npc.GetDataID() $= "410607001"){%Player.SetFamily(6);}//"ɽ����(��)
			if(%Npc.GetDataID() $= "410707001"){%Player.SetFamily(7);}//"���鹬(��)
			if(%Npc.GetDataID() $= "410807001"){%Player.SetFamily(8);}//"��ħ��(ħ)

			%Conv.SetType(4);	//�رնԻ�

		default:
			if( (%State > 3490)&&(%State < 3499) )
				{
					%Conv.SetText(%State);	//��ѡ�����XXX

					%Conv.AddOption(3401, 3400 );	//��ʾ������;�Ի�ѡ��
					%Conv.AddOption(4 , 0 );//����
				}

	}

	//����
	switch(%State)
	{
		case 4010:		//���ڱ���
			if(%Npc.GetDataID() $= "400001038"){%Conv.SetText(4011);}     //ʥ���ڽ�����	����ʹ
			if(%Npc.GetDataID() $= "400001044"){%Conv.SetText(4012);}     //�����ڽ�����	����٤��
			if(%Npc.GetDataID() $= "400001039"){%Conv.SetText(4013);}     //�����ڽ�����	����
			if(%Npc.GetDataID() $= "400001042"){%Conv.SetText(4014);}     //�����ڽ�����	����
			if(%Npc.GetDataID() $= "400001041"){%Conv.SetText(4015);}     //�����ڽ�����	���
			if(%Npc.GetDataID() $= "400001040"){%Conv.SetText(4016);}     //�����ڽ�����	��Х��
			if(%Npc.GetDataID() $= "400001043"){%Conv.SetText(4017);}     //�����ڽ�����	������
			if(%Npc.GetDataID() $= "400001045"){%Conv.SetText(4018);}     //ħ���ڽ�����	ħ��
			%Conv.AddOption(4,0);//����

		case 4020:		//������ɫ
			if(%Npc.GetDataID() $= "400001038"){%Conv.SetText(4021);}     //ʥ���ڽ�����	����ʹ
			if(%Npc.GetDataID() $= "400001044"){%Conv.SetText(4022);}     //�����ڽ�����	����٤��
			if(%Npc.GetDataID() $= "400001039"){%Conv.SetText(4023);}     //�����ڽ�����	����
			if(%Npc.GetDataID() $= "400001042"){%Conv.SetText(4024);}     //�����ڽ�����	����
			if(%Npc.GetDataID() $= "400001041"){%Conv.SetText(4025);}     //�����ڽ�����	���
			if(%Npc.GetDataID() $= "400001040"){%Conv.SetText(4026);}     //�����ڽ�����	��Х��
			if(%Npc.GetDataID() $= "400001043"){%Conv.SetText(4027);}     //�����ڽ�����	������
			if(%Npc.GetDataID() $= "400001045"){%Conv.SetText(4028);}     //ħ���ڽ�����	ħ��
			%Conv.AddOption(4,0);//����

		case 4030:		//��������
			if(%Npc.GetDataID() $= "400001038"){%Conv.SetText(4031);}     //ʥ���ڽ�����	����ʹ
			if(%Npc.GetDataID() $= "400001044"){%Conv.SetText(4032);}     //�����ڽ�����	����٤��
			if(%Npc.GetDataID() $= "400001039"){%Conv.SetText(4033);}     //�����ڽ�����	����
			if(%Npc.GetDataID() $= "400001042"){%Conv.SetText(4034);}     //�����ڽ�����	����
			if(%Npc.GetDataID() $= "400001041"){%Conv.SetText(4035);}     //�����ڽ�����	���
			if(%Npc.GetDataID() $= "400001040"){%Conv.SetText(4036);}     //�����ڽ�����	��Х��
			if(%Npc.GetDataID() $= "400001043"){%Conv.SetText(4037);}     //�����ڽ�����	������
			if(%Npc.GetDataID() $= "400001045"){%Conv.SetText(4038);}     //ħ���ڽ�����	ħ��
			%Conv.AddOption(4,0);//����

		case 4040:		//����ʥ��
			if(%Npc.GetDataID() $= "400001038"){%Conv.SetText(4041);}     //ʥ���ڽ�����	����ʹ
			if(%Npc.GetDataID() $= "400001044"){%Conv.SetText(4042);}     //�����ڽ�����	����٤��
			if(%Npc.GetDataID() $= "400001039"){%Conv.SetText(4043);}     //�����ڽ�����	����
			if(%Npc.GetDataID() $= "400001042"){%Conv.SetText(4044);}     //�����ڽ�����	����
			if(%Npc.GetDataID() $= "400001041"){%Conv.SetText(4045);}     //�����ڽ�����	���
			if(%Npc.GetDataID() $= "400001040"){%Conv.SetText(4046);}     //�����ڽ�����	��Х��
			if(%Npc.GetDataID() $= "400001043"){%Conv.SetText(4047);}     //�����ڽ�����	������
			if(%Npc.GetDataID() $= "400001045"){%Conv.SetText(4048);}     //ħ���ڽ�����	ħ��
			%Conv.AddOption(4,0);//����

	}

	//����
	if( (%State > 4200)&&(%State < 4300) )
		{
			%Conv.SetText(%State + 100);
			%Conv.AddOption(4,0);//����
		}
//--------------------------------------NewPlayerHelp-------State > 0----��----------------------
}

//�����̵�
function NpcOpenShop(%Npc, %Player, %State , %Conv)
{
	switch(%State)
	{
		case 0:
			switch$(%Npc.GetDataID())
			{
				case "400001001":%Conv.AddOption(402051, 402051);	//��ҩƷ�̵�
				case "400001002":%Conv.AddOption(402042, 402042);	//��ʳ���̵�
				case "400001003":%Conv.AddOption(402011, 402011);	//�������̵�
				case "400001004":%Conv.AddOption(402021, 402021);	//�򿪷����̵�
				case "400001005":%Conv.AddOption(402031, 402031);	//�������̵�
//				case "400001006":%Conv.AddOption(      ,       );	//�򿪷����̵�
				case "400001007":%Conv.AddOption(402161, 402161);	//�����޵����̵�
//				case "400001007":%Conv.AddOption(      ,       );	//������װ���̵�
//				case "400001009":%Conv.AddOption(      ,       );	//������̵�
				case "400001011":%Conv.AddOption(402041, 402041);	//���ӻ��̵�
				case "400001017":%Conv.AddOption(402111, 402111);	//�򿪹����̵�
				case "400001018":%Conv.AddOption(402111, 402111);	//�򿪹����̵�
				case "400001019":%Conv.AddOption(402111, 402111);	//�򿪹����̵�
//				case "400001020":%Conv.AddOption(      ,       );	//�򿪹����̵�
				case "400001021":%Conv.AddOption(402111, 402111);	//�򿪹����̵�
				case "400001022":%Conv.AddOption(402111, 402111);	//�򿪹����̵�
				case "400001023":%Conv.AddOption(402061, 402061);	//�򿪹����̵�
				case "400001024":%Conv.AddOption(402071, 402071);	//�򿪹����̵�
				case "400001025":%Conv.AddOption(402081, 402081);	//�򿪹����̵�
//				case "400001026":%Conv.AddOption(      ,       );	//�򿪹����̵�
				case "400001027":%Conv.AddOption(402141, 402141);	//�򿪹����̵�
				case "400001028":%Conv.AddOption(402131, 402131);	//�򿪹����̵�
				case "400001029":%Conv.AddOption(402151, 402151);	//�򿪹����̵�
				case "400001030":%Conv.AddOption(402101, 402101);	//�򿪹����̵�
//				case "400001031":%Conv.AddOption(      ,       );	//�򿪹����̵�
				case "400001059":%Conv.AddOption(402181, 402181);	//�������̵�

				case "401011004":%Conv.AddOption(411171, 411171); //ʥ��������
				case "401006004":%Conv.AddOption(412171, 412171); //����������
				case "401007004":%Conv.AddOption(413171, 413171); //����������
				case "401005004":%Conv.AddOption(414171, 414171); //����������
				case "401009004":%Conv.AddOption(415171, 415171); //����������
				case "401010004":%Conv.AddOption(416171, 416171); //����������
				case "401008004":%Conv.AddOption(417171, 417171); //����������
				case "401004004":%Conv.AddOption(418171, 418171); //ħ��������

				case "401002001":%Conv.AddOption(405051, 405051);	//��ҩƷ�̵�
				case "401002002":%Conv.AddOption(405042, 405042);	//��ʳ���̵�
				case "401002003":%Conv.AddOption(405011, 405011);	//�������̵�
				case "401002004":%Conv.AddOption(405021, 405021);	//�򿪷����̵�
				case "401002005":%Conv.AddOption(405031, 405031);	//�������̵�
				case "401002006":%Conv.AddOption(405191, 405191);	//�򿪷����̵�
				case "401002007":
					%Conv.AddOption(405161, 405161);	//�����޵����̵�
					%Conv.AddOption(405162, 405162);	//������װ���̵�
				case "401002009":%Conv.AddOption(405171, 405171);	//������̵�
				case "401002010":%Conv.AddOption(405041, 405041);	//���ӻ��̵�
				case "401002011":%Conv.AddOption(405111, 405111);	//�򿪹����̵�
				case "401002012":%Conv.AddOption(405111, 405111);	//�򿪹����̵�
				case "401002013":%Conv.AddOption(405111, 405111);	//�򿪹����̵�
				case "401002014":%Conv.AddOption(405111, 405111);	//�򿪹����̵�
				case "401002015":%Conv.AddOption(405111, 405111);	//�򿪹����̵�
				case "401002016":%Conv.AddOption(405111, 405111);	//�򿪹����̵�
				case "401002017":%Conv.AddOption(405061, 405061);	//�򿪹����̵�
				case "401002018":%Conv.AddOption(405071, 405071);	//�򿪹����̵�
				case "401002019":%Conv.AddOption(405081, 405081);	//�򿪹����̵�
				case "401002020":%Conv.AddOption(405201, 405201);	//�򿪹����̵�
				case "401002021":%Conv.AddOption(405141, 405141);	//�򿪹����̵�
				case "401002022":%Conv.AddOption(405131, 405131);	//�򿪹����̵�
				case "401002023":%Conv.AddOption(405151, 405151);	//�򿪹����̵�
				case "401002024":%Conv.AddOption(405101, 405101);	//�򿪹����̵�
				case "401002025":%Conv.AddOption(405211, 405211);	//�򿪹����̵�
				case "401002042":%Conv.AddOption(405181, 405181);	//�򿪼������̵�

//				case "400001107":		//̯λ-�����̵�
//					%Conv.AddOption(400001, 400001);	//"<t>���е��߲��ԣ�	400001</t>";
//					%Conv.AddOption(400002, 400002);	//"<t>��Ƕ�����̵꣺	400002</t>";
//					%Conv.AddOption(400003, 400003);	//"<t>1--80���������ԣ�	400003</t>";
//					%Conv.AddOption(400004, 400004);	//"<t>1--80�����߲��ԣ�	400004</t>";
//					%Conv.AddOption(400005, 400005);	//"<t>1--80����Ʒ���ԣ�	400005</t>";
//					%Conv.AddOption(400006, 400006);	//"<t>ǿ�������̵꣺	400006</t>";
//					%Conv.AddOption(400007, 400007);	//"<t>���������̵꣺	400007</t>";
//					%Conv.AddOption(400008, 400008);	//"<t>ģ�Ͳ����̵꣺	400008</t>";
//					%Conv.AddOption(400009, 400009);	//"<t>��������̵꣺  400009</t>";
//					%Conv.AddOption(400010, 400010);	//"<t>�����̵꣺  400010</t>";
			}
	}

	if( (%State >= 400000)&&(%State < 500000) )
		OpenNpcShop(%Player,%State);
}

//���͵�ͼ
function NpcTransportObject(%Npc, %Player, %State , %Conv )
{
//	echo("zzzzzzzzz%State = "@%State);
	switch(%State)
	{
		case 0:
			//�Ի����͹���
			switch$(%Npc.GetDataID())
			{
				case "400001037":
					if(GetZoneID() $= "1002"){%Conv.AddOption(1001, 100103 );}	//�����������
					if(GetZoneID() $= "1001"){%Conv.AddOption(1002, 100203 );}	//���������ľ�

				case "400004037":
					if(GetZoneID() $= "1401"){%Conv.AddOption(1001, 100106 );}	//���͵������

				case "401011003":%Conv.AddOption(1001, 100103 );	//�����������
				case "411004600":%Conv.AddOption(1001, 100106 );	//33���ž�����ͬ־�����������
				case "401124001":%Conv.AddOption(1001, 10111 + 500000 );	//��ȴ����������
				case "401302001":%Conv.AddOption(11  , 10165 + 500000 );	//�뿪�ֹ�������������

				case "400002105":
					if( (%Player.IsAcceptedMission(10162))||(%Player.IsAcceptedMission(10163))||(%Player.IsAcceptedMission(10164))||(%Player.IsAcceptedMission(10165)) )
						%Conv.AddOption(1302, 130201 );	//����ҹ������

				case "400001007":%Conv.AddOption(1124, 112401 );	//����ȥ���

//				case "400001102":
//					if((%Player.IsFinishedMission(10109))&&(!%Player.IsFinishedMission(10110)))
//						%Conv.AddOption(1303, 130301);	//���ͽ�Ů�����

				case "401301001":%Conv.AddOption(1001,100119);

				case "401303101":%Conv.AddOption(1001,100119);

				case "401102112":
					if((%Player.IsFinishedMission(10320))&&(!%Player.IsFinishedMission(10325)))
				 		%Conv.AddOption(1102, 110204 );	//����ˮī���춥

				case "401102115":
					if((%Player.IsFinishedMission(10321))&&(!%Player.IsFinishedMission(10325)))
						%Conv.AddOption(1304, 130401 );	//����ˮī��������ǰ��
				 	  %Conv.AddOption(1102, 110206 );  //��Ϧ�����

				case "401103014":
					if((%Player.IsFinishedMission(10584))&&(!%Player.IsFinishedMission(10589)))
						%Conv.AddOption(1305, 130501 );	//���������Ծ�

				case "401305001":%Conv.AddOption(1103, 110308);//���������Ծ�
		  	case "401304001":%Conv.AddOption(1102, 110206);//����ˮī����
				case "401103001":%Conv.AddOption(13, 110302 );//�����·����͵��Ϸ�
				case "401103002":%Conv.AddOption(13, 110303 );//�����Ϸ����͵��·�
				case "401103003":%Conv.AddOption(13, 110304 );//�����·����͵��Ϸ�
				case "401103004":%Conv.AddOption(13, 110305 );//�����Ϸ����͵��·�

				case "401401001":
					%Conv.AddOption(1001, 100106 );	//�����������
					%Conv.AddOption(1002, 100203 );	//���������ľ�
					%Conv.AddOption(1011, 101101 );	//���������ڵ�ͼ"������(ʥ)"���ع���
					%Conv.AddOption(1006, 100601 );	//���������ڵ�ͼ"������(��)"��������
					%Conv.AddOption(1007, 100701 );	//���������ڵ�ͼ"������(��)"�����ɾ�
					%Conv.AddOption(1005, 100501 );	//���������ڵ�ͼ"�ɻ���(��)"��ľ��
					%Conv.AddOption(1009, 100901 );	//���������ڵ�ͼ"���Ľ�(��)"��ڤ����
					%Conv.AddOption(1010, 101001 );	//���������ڵ�ͼ"ɽ����(��)"˫��ɽ
					%Conv.AddOption(1008, 100801 );	//���������ڵ�ͼ"���鹬(��)"���α���
					%Conv.AddOption(1004, 100401 );	//���������ڵ�ͼ"��ħ��(ħ)"��ϦԨ
			}
	}

	if(strlen(%State) == 6)
		{
//			echo("yyyyyyyy%State = "@%State);
			if(%State == 200000)
				{
					%Conv.SetText(%State);
					%Conv.AddOption(4 , 0 );//����
				}
			if(GetSubStr(%State,0,1) $= "1")
				{
					echo("xxxxxxxxxx%State = "@%State);
					%Conv.SetType(4);	//�رնԻ�

					if(GetSubStr(%State,1,1) $= "3")
						GoToNextMap_CopyMap( %Player, GetSubStr(%State,0,4) );
					else
						GoToNextMap_Normal(%Player, %State);
				}

			//��Ҫ�ж������Ƿ����������������������߸�������ȷ�ϲſɴ���
			if(GetSubStr(%State,0,1) $= "5")
				{
					%YesToo = 0;
					%Mid = %State - 500000;

					//��һ�β�׽
					if(%Mid == 10111)
						{
							%Map = 100101;

							if(%Player.IsAcceptedMission(%Mid))
								if(%Player.GetItemCount(%Player.GetMissionFlag(%Mid, 2100)) < %Player.GetMissionFlag(%Mid, 2200))
									%YesToo = 1;
						}

					//������ҹ��������
					if(%Mid == 10165)
						{
							%Map = 110104;

							if(!%Player.IsFinishedMission(%Mid))
								%YesToo = 1;
						}

					if(%YesToo == 1)
						{
							%Conv.SetText(%Mid @ "1");	//��������������
							%Conv.AddOption(GetSubStr(%Map,0,4), %Map );	//���ͳ�ȥ
						}
						else
							{
								%Conv.SetType(4);	//�رնԻ�
								GoToNextMap_Normal(%Player, %Map);
							}
				}
		}
}

//�������ܽ���
function NpcOpenJob(%Npc, %Player, %State , %Conv)
{
//--------------------------------------------NpcOpenJob-------------State == 0----��----------------------
	switch(%State)
	{
		case 0:
			//�����NPC
			switch$(%Npc.GetDataID())
			{
				case "400001006":
					%Conv.AddOption(200211, 200211);		//��������	���ܽ���
					%Conv.AddOption(200212, 200212);		//��������	���ܽ���
					%Conv.AddOption(200213, 200213);		//����ǿ��	���ܽ���
					%Conv.AddOption(200214, 200214);		//��������	���ܽ���
					%Conv.AddOption(200215, 200215);		//�����ϳ�	���ܽ���

				case "400001008":
					%Conv.AddOption(200201, 200201);   //���޼���	���ܽ���
					%Conv.AddOption(200202, 200202);   //����������	���ܽ���
					%Conv.AddOption(200203, 200203);   //��������	���ܽ���
					%Conv.AddOption(200204, 200204);   //���޻�ͯ	���ܽ���
					%Conv.AddOption(200205, 200205);   //���޷�ֳ	���ܽ���

				case "400001017": %Conv.AddOption(200109, 200109);		//�����ڿ���	���ܽ���
				case "400001018": %Conv.AddOption(200110, 200110);		//������ľ����	���ܽ���
				case "400001019": %Conv.AddOption(200111, 200111);		//�������㼼��	���ܽ���
				case "400001020": %Conv.AddOption(200112, 200112);		//������ֲ����	���ܽ���
				case "400001021": %Conv.AddOption(200113, 200113);		//������ҩ����	���ܽ���
				case "400001022": %Conv.AddOption(200114, 200114);		//�������Լ���	���ܽ���

				case "400001023": %Conv.AddOption(200101, 200101);		//�������켼��	���ܽ���
				case "400001024": %Conv.AddOption(200102, 200102);		//�����÷켼��	���ܽ���
				case "400001025": %Conv.AddOption(200103, 200103);		//�������ռ���	���ܽ���
				case "400001026": %Conv.AddOption(200104, 200104);		//�������似��	���ܽ���
				case "400001027": %Conv.AddOption(200105, 200105);		//������������	���ܽ���
				case "400001028": %Conv.AddOption(200106, 200106);		//������⿼���	���ܽ���
				case "400001029": %Conv.AddOption(200107, 200107);		//������������	���ܽ���
				case "400001030": %Conv.AddOption(200108, 200108);		//������������	���ܽ���
				case "400001031": %Conv.AddOption(200115, 200115);		//������������	���ܽ���

				case "400001046": %Conv.AddOption(200007, 200007);		//�򿪻�Ԫ��	���ܽ���
				case "400001035": %Conv.AddOption(200221, 200221);		//��ʾ��	���ܽ���
				case "400001036": %Conv.AddOption(200222, 200222);		//ͨ����	���ܽ���
				case "400001047": %Conv.AddOption(200223, 200223);		//�����	���ܽ���
				case "400001048": %Conv.AddOption(200224, 200224);		//�������	���ܽ���
				case "400001050": %Conv.AddOption(200225, 200225);		//��Ҫ���	���ܽ���
				case "400001059":
					%Conv.AddOption(200001, 200001);		//����װ��	���ܽ���
					%Conv.AddOption(200005, 200005);		//װ��ǿ��	���ܽ���
					%Conv.AddOption(200006, 200006);		//װ�����	���ܽ���

				case "400001003":		//������	���ϴ�
					%Conv.AddOption(200008, 200008);		//���������//�����޸�װ��
					%Conv.AddOption(200009, 200009);		//���������//����;��޸�
			}
			//���ľ�NPC
			switch$(%Npc.GetDataID())
			{
				case "401002006":
					%Conv.AddOption(200211, 200211);		//��������	���ܽ���
					%Conv.AddOption(200212, 200212);		//��������	���ܽ���
					%Conv.AddOption(200213, 200213);		//����ǿ��	���ܽ���
					%Conv.AddOption(200214, 200214);		//��������	���ܽ���
					%Conv.AddOption(200215, 200215);		//�����ϳ�	���ܽ���

				case "401002008":
					%Conv.AddOption(200201, 200201);   //���޼���	���ܽ���
					%Conv.AddOption(200202, 200202);   //����������	���ܽ���
					%Conv.AddOption(200203, 200203);   //��������	���ܽ���
					%Conv.AddOption(200204, 200204);   //���޻�ͯ	���ܽ���
					%Conv.AddOption(200205, 200205);   //���޷�ֳ	���ܽ���

				case "401002011": %Conv.AddOption(200109, 200109);		//�����ڿ���	���ܽ���
				case "401002012": %Conv.AddOption(200110, 200110);		//������ľ����	���ܽ���
				case "401002013": %Conv.AddOption(200111, 200111);		//�������㼼��	���ܽ���
				case "401002014": %Conv.AddOption(200112, 200112);		//������ֲ����	���ܽ���
				case "401002015": %Conv.AddOption(200113, 200113);		//������ҩ����	���ܽ���
				case "401002016": %Conv.AddOption(200114, 200114);		//�������Լ���	���ܽ���

				case "401002017": %Conv.AddOption(200101, 200101);		//�������켼��	���ܽ���
				case "401002018": %Conv.AddOption(200102, 200102);		//�����÷켼��	���ܽ���
				case "401002019": %Conv.AddOption(200103, 200103);		//�������ռ���	���ܽ���
				case "401002020": %Conv.AddOption(200104, 200104);		//�������似��	���ܽ���
				case "401002021": %Conv.AddOption(200105, 200105);		//������������	���ܽ���
				case "401002022": %Conv.AddOption(200106, 200106);		//������⿼���	���ܽ���
				case "401002023": %Conv.AddOption(200107, 200107);		//������������	���ܽ���
				case "401002024": %Conv.AddOption(200108, 200108);		//������������	���ܽ���
				case "401002025": %Conv.AddOption(200115, 200115);		//������������	���ܽ���

				case "401002037": %Conv.AddOption(200007, 200007);		//�򿪻�Ԫ��	���ܽ���
				case "401002026": %Conv.AddOption(200221, 200221);		//��ʾ��	���ܽ���
				case "401002027": %Conv.AddOption(200222, 200222);		//ͨ����	���ܽ���
				case "401002038": %Conv.AddOption(200223, 200223);		//�����	���ܽ���
				case "401002039": %Conv.AddOption(200224, 200224);		//�������	���ܽ���
				case "401002041": %Conv.AddOption(200225, 200225);		//��Ҫ���	���ܽ���
				case "401002042":
					%Conv.AddOption(200001, 200001);		//����װ��	���ܽ���
					%Conv.AddOption(200005, 200005);		//װ��ǿ��	���ܽ���
					%Conv.AddOption(200006, 200006);		//װ�����	���ܽ���

				case "401002003":		//������
					%Conv.AddOption(200008, 200008);		//���������//�����޸�װ��
					%Conv.AddOption(200009, 200009);		//���������//����;��޸�
			}
			//������ͼNPC
			switch$(%Npc.GetDataID())
			{
				case "401011002":			//ʥ���ڼ���ʹ
					if(%Player.GetFamily() == 1)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//ѡ���1ϵ����
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//ѡ���2ϵ����
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���3ϵ����
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���4ϵ����
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���5ϵ����

							//��������ѧϰ����
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401007002":			//�����ڼ���ʹ
					if(%Player.GetFamily() == 3)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//ѡ���1ϵ����
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//ѡ���2ϵ����
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���3ϵ����
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���4ϵ����
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���5ϵ����

							//��������ѧϰ����
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401010002":			//�����ڼ���ʹ
					if(%Player.GetFamily() == 6)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//ѡ���1ϵ����
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//ѡ���2ϵ����
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���3ϵ����
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���4ϵ����
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���5ϵ����

							//��������ѧϰ����
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401009002":			//�����ڼ���ʹ
					if(%Player.GetFamily() == 5)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//ѡ���1ϵ����
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//ѡ���2ϵ����
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���3ϵ����
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���4ϵ����
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���5ϵ����

							//��������ѧϰ����
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401005002":			//�����ڼ���ʹ
					if(%Player.GetFamily() == 4)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//ѡ���1ϵ����
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//ѡ���2ϵ����
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���3ϵ����
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���4ϵ����
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���5ϵ����

							//��������ѧϰ����
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401008002":			//�����ڼ���ʹ
					if(%Player.GetFamily() == 7)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//ѡ���1ϵ����
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//ѡ���2ϵ����
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���3ϵ����
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���4ϵ����
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���5ϵ����

							//��������ѧϰ����
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401006002":			//�����ڼ���ʹ
					if(%Player.GetFamily() == 2)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//ѡ���1ϵ����
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//ѡ���2ϵ����
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���3ϵ����
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���4ϵ����
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���5ϵ����

							//��������ѧϰ����
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
				case "401004002":			//ħ���ڼ���ʹ
					if(%Player.GetFamily() == 8)
						{
							if( (%Player.GetClasses(0) == 0)&&(%Player.GetFamily() > 0  ) ){%Conv.AddOption(200003, 200003);}	//ѡ���1ϵ����
							if( (%Player.GetClasses(0) > 0)&&(%Player.GetClasses(1) == 0)&&(%Player.GetLevel() >= 40 ) ){%Conv.AddOption(200003, 200003);}	//ѡ���2ϵ����
							if( (%Player.GetClasses(1) > 0)&&(%Player.GetClasses(2) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���3ϵ����
							if( (%Player.GetClasses(2) > 0)&&(%Player.GetClasses(3) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���4ϵ����
							if( (%Player.GetClasses(3) > 0)&&(%Player.GetClasses(4) == 0)&&(%Player.GetLevel() >= 999) ){%Conv.AddOption(200003, 200003);}	//ѡ���5ϵ����

							//��������ѧϰ����
							if( (%Player.GetClasses(0) != 0)||(%Player.GetClasses(1) != 0) )
								%Conv.AddOption(200004, 200004);
						}
			}
	}
//-----------------------------------------NpcOpenJob----------------State == 0----��----------------------

//--------------------------------------------NpcOpenJob-------------State > 0----��----------------------
	switch(%State)
	{
		case 200001:OpenIdentify(%Player);//����װ���������ܴ��ڷ���˽ű�
		case 200003:OpenSkillSelect(%Player, %Player.GetClasses(0), %Player.GetClasses(1), %Player.GetClasses(2), %Player.GetClasses(3));//��������ѡ�����
		case 200004:OpenSkillStudy(%Player, %Player.GetClasses(0), %Player.GetClasses(1), %Player.GetClasses(2), %Player.GetClasses(3), %Player.GetClasses(4));//��������ѧϰ����
		case 200005:%Player.OpenEquipStrengthenDialog(%Npc);	//�����ִ�У���ǿ���б�
		case 200006:%Player.OpenEquipPunchHoleDialog(%Npc);		//�����ִ�У��򿪴�׽���
		case 200007:%Player.OpenBank(0);											//�����ִ�У��򿪲ֿ����
		case 200008:%Player.OpenRepair(1);										//�����ִ�У����������1 Ϊ�����޸� 2Ϊ��ǰ����;��޸�
		case 200009:%Player.OpenRepair(2);										//�����ִ�У����������1 Ϊ�����޸� 2Ϊ��ǰ����;��޸�
		case 200109:OpenLivingSkillStudy(%Player,9 );	//400001017	�ڿ���ʦ
		case 200110:OpenLivingSkillStudy(%Player,10);	//400001018	��ľ����ʦ
		case 200111:OpenLivingSkillStudy(%Player,11);	//400001019	���㼼��ʦ
		case 200112:OpenLivingSkillStudy(%Player,12);	//400001020	��ֲ����ʦ
		case 200113:OpenLivingSkillStudy(%Player,13);	//400001021	��ҩ����ʦ
		case 200114:OpenLivingSkillStudy(%Player,14);	//400001022	���Լ���ʦ
		case 200101:OpenLivingSkillStudy(%Player,1 );	//400001023	���켼��ʦ
		case 200102:OpenLivingSkillStudy(%Player,2 );	//400001024	�÷켼��ʦ
		case 200103:OpenLivingSkillStudy(%Player,3 );	//400001025	���ռ���ʦ
		case 200104:OpenLivingSkillStudy(%Player,4 );	//400001026	���似��ʦ
		case 200105:OpenLivingSkillStudy(%Player,5 );	//400001027	��������ʦ
		case 200106:OpenLivingSkillStudy(%Player,6 );	//400001028	��⿼���ʦ
		case 200107:OpenLivingSkillStudy(%Player,7 );	//400001029	��������ʦ
		case 200108:OpenLivingSkillStudy(%Player,8 );	//400001030	��������ʦ
		case 200201:%Player.PetOperation_OpenPetIdentify(%Npc);	//�������
		case 200202:%Player.PetOperation_OpenPetInsight(%Npc);	//����������
		case 200203:%Player.PetOperation_OpenPetLianHua(%Npc);	//��������
		case 200204:%Player.PetOperation_OpenPetHuanTong(%Npc);	//���ﻹͯ
		case 200205:	//���ﷱֳ

		case 200211:	//��������
		case 200212:  //��������
		case 200213:  //����ǿ��
		case 200214:  //��������
		case 200215:  //�����ϳ�

		case 200221:	//��ʾ��	���ܽ���
		case 200222:  //ͨ����	���ܽ���
		case 200223:  //�����	���ܽ���
		case 200224:  //�������	���ܽ���
		case 200225:  //��Ҫ���	���ܽ���
	}
//-----------------------------------------NpcOpenJob----------------State > 0----��----------------------
}

//���������ԶԻ����޽���
function NpcCanDoJob(%Npc, %Player, %State , %Conv)
{
//--------------------------------------NpcCanDoJob------------State == 0----��----------------------
	switch(%State)
	{
		case 0:
			//�����NPC
			switch$(%Npc.GetDataID())
			{
				case "400001101":
					%Conv.AddOption(300003, 300003);	//��ȡ����״̬(С��30��)
//					%Conv.AddOption(300004, 300004);	//"��ȡ�¹ⱦ��(����)";

					if(%Player.IsFinishedMission(10108))
						{
							//�����
							if(%Player.GetSkillLevel(20004) > 0)
								{
									if( (%Player.GetLevel() >= 10)&&(%Player.GetLevel() < 20) )
										if(%Player.GetSkillLevel(20004) == 1)
											%Conv.AddOption(300034, 300034);	//"���������ܣ������2";

									if(%Player.GetLevel() >= 20)
										if(%Player.GetSkillLevel(20004) == 2)
											%Conv.AddOption(300034, 300034);	//"���������ܣ������3";
								}
								else
									%Conv.AddOption(300005, 300005);	//"�����ܣ������";

							//��������
							if( (%Player.GetSkillLevel(20004) == 3)&&(%Player.GetLevel() >= 30) )
								{
									if( (%Player.GetSkillLevel(20005) == 0)&&(%Player.GetSkillLevel(20006) == 0) )
										%Conv.AddOption(300030, 300030);	//"�����µķ�֧���ܣ���������";
									else
										{
											if( (%Player.GetLevel() >= 40)&&(%Player.GetLevel() < 50) )
												{
													if(%Player.GetSkillLevel(20005) <= 1){%Conv.AddOption(300028, 300026);}	//"���������ܣ���������-����2";
													if(%Player.GetSkillLevel(20006) <= 1){%Conv.AddOption(300029, 300027);}	//"���������ܣ���������-����2";
												}
											if( (%Player.GetLevel() >= 50)&&(%Player.GetLevel() < 60) )
												{
													if(%Player.GetSkillLevel(20005) <= 2){%Conv.AddOption(300028, 300026);}	//"���������ܣ���������-����3";
													if(%Player.GetSkillLevel(20005) <= 2){%Conv.AddOption(300029, 300027);}	//"���������ܣ���������-����3";
												}
											if(%Player.GetLevel() >= 60)
												{
													if(%Player.GetSkillLevel(20005) <= 3){%Conv.AddOption(300032, 300026);}	//"���������ܣ���������-����";
													if(%Player.GetSkillLevel(20005) <= 3){%Conv.AddOption(300033, 300027);}	//"���������ܣ���������-��ѩ";
												}
										}
								}
						}
						
					%Conv.AddOption(300025, 300025);	//����ǵ�����

				case "400001072":
					%Conv.AddOption(300015, 300015);	//�����Ѫ
					%Conv.AddOption(300016, 300016);	//�����Ѫ

				case "400001049":%Conv.AddOption(300021, 300021);	//ǰ������
				case "400001050":%Conv.AddOption(300022, 300022);	//�Ѻö�ѭ��
				case "400001060":
					%Conv.AddOption(300023, 300023);	//��ȡ˫������
					%Conv.AddOption(300024, 300024);	//����˫������

				case "400001102":%Conv.AddOption(300036, 300036);	//����Ů�

			}
			//���ľ�NPC
			switch$(%Npc.GetDataID())
			{
				case "401002040":%Conv.AddOption(300021, 300021);	//ǰ������
				case "401002041":%Conv.AddOption(300022, 300022);	//�Ѻö�ѭ��
				case "401002043":
					%Conv.AddOption(300023, 300023);	//��ȡ˫������
					%Conv.AddOption(300024, 300024);	//����˫������
			}
	}
//--------------------------------------NpcCanDoJob------------State == 0----��----------------------
//--------------------------------------NpcCanDoJob---------State > 0----��----------------------
	switch(%State)
	{
		case 300003:
			%Player.AddBuff(390010001, 0);
			%Player.AddBuff(390020001, 0);
			%Conv.SetType(4);

		case 300004:
			%Player.PutItem(108020208,1);
			%Player.AddItem();
			%Conv.SetType(4);

		case 300005:
			%Player.AddSkill(200040001);
			%Conv.SetType(4);

		case 300021:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//��δ���ţ������ڴ���//����	//ǰ������
		case 300022:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//��δ���ţ������ڴ���//����	//�Ѻö�ѭ��
		case 300023:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//��δ���ţ������ڴ���//����	//��ȡ˫������
		case 300024:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//��δ���ţ������ڴ���//����	//����˫������
		case 300025:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//��δ���ţ������ڴ���//����	//����ǵ�����
		case 300036:%Conv.SetText(32);%Conv.AddOption(4 , 0 );	//��δ���ţ������ڴ���//����	//����Ů�

		case 300026:
			if(%Player.GetSkillLevel(20004) == 3)
				{
					//��������-����1
					if(%Player.GetLevel() >= 30)
						if(%Player.GetSkillLevel(20005) == 0)
							%Player.AddSkill(200050001);
					//��������-����2
					if(%Player.GetLevel() >= 40)
						if(%Player.GetSkillLevel(20005) == 1)
							%Player.AddSkill(200050002);
					//��������-����3
					if(%Player.GetLevel() >= 50)
						if(%Player.GetSkillLevel(20005) == 2)
							%Player.AddSkill(200050003);
					//��������-����
					if(%Player.GetLevel() >= 60)
						if(%Player.GetSkillLevel(20005) == 3)
							%Player.AddSkill(200050004);
				}
			%Conv.SetType(4);

		case 300027:
			if(%Player.GetSkillLevel(20004) == 3)
				{
					//��������-����1
					if(%Player.GetLevel() >= 30)
						if(%Player.GetSkillLevel(20006) == 0)
							%Player.AddSkill(200060001);
					//��������-����2
					if(%Player.GetLevel() >= 40)
						if(%Player.GetSkillLevel(20006) == 1)
							%Player.AddSkill(200060002);
					//��������-����3
					if(%Player.GetLevel() >= 50)
						if(%Player.GetSkillLevel(20006) == 2)
							%Player.AddSkill(200060003);
					//��������-��ѩ
					if(%Player.GetLevel() >= 60)
						if(%Player.GetSkillLevel(20006) == 3)
							%Player.AddSkill(200060004);
				}
			%Conv.SetType(4);
			
		case 300030:
			%Conv.SetText(300031);
			%Conv.AddOption(300026, 300026);	//"�����ܣ���������-����1";
			%Conv.AddOption(300027, 300027);	//"�����ܣ���������-����1";

		case 300034:
			if(%Player.GetLevel() >= 10)
				if(%Player.GetSkillLevel(20004) == 1)
					%Player.AddSkill(200040002);	//"���������ܣ������2";

			if(%Player.GetLevel() >= 20)
				if(%Player.GetSkillLevel(20004) == 2)
					%Player.AddSkill(200040003);	//"���������ܣ������3";
					
		%Conv.SetType(4);
	}

	//ҽ����Ѫ
	switch(%State)
	{
		case 300015:
			%Conv.SetText(300017);	//��һ�ڲ���
			%Conv.AddOption(300018, 300018);	//�����Ѫ
			%Conv.AddOption(4 , 0 );//����

		case 300016:
			%Conv.SetText(300019);	//����δ���ţ�
			%Conv.AddOption(4 , 0 );//����

		case 300018:
			%Conv.AddOption(4 , 0 );//����
			%HP = %Player.GetMaxHP() - %Player.GetHP();
			if(%HP > 0)
				{
					%Money = 1 + %HP / 100;
					if(%Player.GetMoney(1) > %Money)
						{
							%Player.ReduceMoney(%Money, 1);
							%Player.AddBuff(390210001, 0);
							%Conv.SetType(4);
							return;
						}
						else
							%Conv.SetText(31);	//��Ǯ����
 				}
 				else
 					%Conv.SetText(300020);	//��̨��������ܰ�����

	}

//--------------------------------------NpcCanDoJob---------State > 0----��----------------------
}

//����
//function SpawnNewNpc(%Npc, %Player, %State , %Conv)
//{
//	switch(%State)
//	{
//		case 0:
//			if(%Npc.GetDataID() $= "400001104")
//				if( ($SpNewNpc[1] == 0)||($SpNewNpc[2] == 0)||($SpNewNpc[3] == 0)||($SpNewNpc[4] == 0)||($SpNewNpc[5] == 0) )
//					%Conv.AddOption(4000, 4000);	//���ҿ���������ʣ�
//		case 4000:
//			if( ($SpNewNpc[1] == 0)||($SpNewNpc[2] == 0)||($SpNewNpc[3] == 0)||($SpNewNpc[4] == 0)||($SpNewNpc[5] == 0) )
//				{
//					$SpNewNpc[1] = SpNewNpc(%Player, 400001108, "-254.78 -407.974 100.176",0);
//					$SpNewNpc[2] = SpNewNpc(%Player, 400001109, "-254.705 -409.852 100.15",0);
//					$SpNewNpc[3] = SpNewNpc(%Player, 400001109, "-254.14 -408.14 100.15",0);
//					$SpNewNpc[4] = SpNewNpc(%Player, 400001110, "-255.516 -408.267 100.176",0);
//					$SpNewNpc[5] = SpNewNpc(%Player, 400001110, "-257.149 -410.206 100.176",0);
//				}
//	}
//}

//���е㹦��
function NpcFly(%Npc, %Player, %State , %Conv)
{
	switch(%State)
	{
		case 0:
			switch$(%Npc.GetDataID())
			{
				case "400001113":
					%Conv.AddOption( 9 @ 10010102, 310001);		//����ͷ���д��͵�����
					%Conv.AddOption( 9 @ 10010005, 310005);		//����Ƿ��д��͵����ľ�
//				case "400001104":%Conv.AddOption( 9 @ 10010104, 310002);		//�ξ��η��д��͵�����
//				case "400002105":%Conv.AddOption( 9 @ 11010101, 310003);		//�����Ƿ��д��͵������
				case "411004600":%Conv.AddOption( 9 @ 11270001, 310004);		//��������д��͵������
				case "401002028":%Conv.AddOption( 9 @ 10020002, 310006);		//���ľ����д��͵������
			}
	}

	//���е㹦��
	switch(%State)
	{
		case 310001:FlyMeToTheMoon(%Player, 10010102);
//		case 310002:FlyMeToTheMoon(%Player, 10010104);
//		case 310003:FlyMeToTheMoon(%Player, 11010101);
		case 310004:FlyMeToTheMoon(%Player, 11270001);
		case 310005:FlyMeToTheMoon(%Player, 10010005);
		case 310006:FlyMeToTheMoon(%Player, 10020002);
	}
}


//function NowAdd(%Npc, %Player, %State, %Conv)
//{
//
//	switch(%State)
//	{
//		case 0:
//			%Conv.AddOption(300004, 300004);	//"��ȡ�¹ⱦ��(����)";

//			%Conv.AddOption(300008, 300008);	//"��Ҫ��ʦ";
//			%Conv.AddOption(300006, 300006);	//"��Ҫ��ʦ";
//			%Conv.AddOption(300007, 300007);	//"��Ҫ��ͽ";

//
//		case 300006:
//			CreateRelation(%Player.GetPlayerID(),3);
//			%Conv.SetType(4);
//
//		case 300007:
//			CreateRelation(%Player.GetPlayerID(),4);
//			%Conv.SetType(4);
//
//		case 300008:
//			AddMasterLevel(%Player.GetPlayerID(),1);
//			%Conv.SetType(4);

//	}
//}


//���صĹ���
function PlayGame(%Npc, %Player, %State , %Conv)
{
	switch(%State)
	{
		case 0:
			switch$(%Npc.GetDataID())
			{
				case "400001102":	//Ů�����
					%Conv.AddOption(316, 316);	//"����Ů�����";
			}

	}

	//Ů洲���
	switch(%State)
	{
		case 316:
			%Conv.SetText(319);	//����˵��
			%Conv.AddOption(317, 317);	//"���˼���";
			%Conv.AddOption(318, 318);	//"��Ӽ���";
		case 317:


		case 318:

	}
}
