//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//�ͻ������жԻ�������ı�����ͨ�Ի�
//==================================================================================


//��ȡ�Ի��ı�������·�����ű������
function GetDialogText(%Npc, %Player, %DialogID, %Param)
{
	//������ƭ������ʾ
	if($Fang_Pian_Fang_Dao == 0)
		Fang_Pian_Fang_Dao();

//	echo("����������������������������������������������������������");
//	echo("%Param ======="@%Param);
//	echo("%DialogID = " @ %DialogID);
//	echo("%Param = " @ %Param);//�Ի����Ӳ���

//	if(%Param == 1)
//		{
//			echo("����������������������������������������������������������");
//			echo("%Param ======="@%Param);
//			if($Mid_Accepted_Num != %Player.GetAcceptedMission())
//				%Num = %Player.GetAcceptedMission();
//			for(%i = 0; %i < %Num; %i++)
//			{
//
//			}
//		}
	//����DialogID�ĳ��ȣ������ִ˶Ի�������

	//ͨ����Ի�
	if(strlen(%DialogID) <= 7)
		return Get_TY_Dialog(%Npc, %Player, %DialogID, %Param);
	//9λ��ŵ���ضԻ�
	if(strlen(%DialogID) == 9)
		{
			//��������ID�����ֱ�����ͣ�Ҳ�����ݱ����λ�������жϱ������

			//�����λ��������ʶ��ȡֵ��ΧΪ1-9��Ŀǰ��ȷ����
			//1������
			//2������
			//3��״̬
			//4��NPC&Monster
			//5���ٻ������ܣ�
			//
			//6.7.9.10��δȷ��
			//8������

			if(GetSubStr(%DialogID,0,1) $= "1"){return Get_Item_Dialog(%Npc, %Player, %DialogID, %Param);}
			if(GetSubStr(%DialogID,0,1) $= "2"){return Get_Skill_Dialog(%Npc, %Player, %DialogID, %Param);}
			if(GetSubStr(%DialogID,0,1) $= "3"){return Get_Buff_Dialog(%Npc, %Player, %DialogID, %Param);}
			if(GetSubStr(%DialogID,0,1) $= "4"){return Get_Npc_Dialog(%Npc, %Player, %DialogID, %Param);}
			if(GetSubStr(%DialogID,0,1) $= "8"){return Get_Area_Dialog(%Npc, %Player, %DialogID, %Param);}
			if(GetSubStr(%DialogID,0,1) $= "9"){return Get_Fly_Dialog(%Npc, %Player, %DialogID, %Param);}

		}
	//������Ի�
	if(strlen(%DialogID) == 8)
		{
			%Tid = GetSubStr(%DialogID,0,3);
			%Mid = GetSubStr(%DialogID,3,5);

			%kind       	= GetMissionKind(%Mid);
			%Mid_LeiXin 	= $MissionKind[ %kind, 2];

			//��������Ի�
			if(%Tid == 100) return $Icon[1] @ $Get_Dialog_GeShi[31203] @ " ��ȡ��" @ %Mid_LeiXin @ "��" @ GetMission_Name(%Player , %Mid , 1) @ "</t>";
			if(%Tid == 900)
				{
					if(CanDoThisMission(%Player, %Mid, 2, 0, 0) $= "")
						%Mid_Over = "����ɣ�";
					else
						%Mid_Over = "";

					return $Icon[2] @ $Get_Dialog_GeShi[31204] @ " ������" @ %Mid_LeiXin @ "��" @ GetMission_Name(%Player , %Mid , 1) @ %Mid_Over @ "</t>";
				}
			if(%Tid == 999)
				{
					%Step = $Get_Dialog_GeShi[31203] @ GetMission_Name(%Player , %Mid , 1) @ "(δ���)</t>";

					if(%Player.IsAcceptedMission(%Mid)){%Step = $Get_Dialog_GeShi[31202] @ GetMission_Name(%Player , %Mid , 1) @ "(�ѽ���)</t>";}
					if(%Player.IsFinishedMission(%Mid)){%Step = $Get_Dialog_GeShi[31204] @ GetMission_Name(%Player , %Mid , 1) @ "(�Ѿ����)</t>";}

					return %Step;
				}

			//10001,2+4,��������+������,��ʾ�������ʱ�Ի�
			if(%Tid == 110) return Get_Mis_Dialog(%Npc, %Player, %Mid, 2) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 4);

			//10009,��ʾ���񽻸�����������ʱ�ĶԻ�
			if(%Tid == 199) return Get_Mis_Dialog(%Npc, %Player, %Mid, 2) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 4);

			//10009,��ʾ������;NPC�ĶԻ���ѡ��
			if( (%Tid >= 200)&&(%Tid < 300) ) return eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, %Tid);");	//"��ʾ������;NPC�ĶԻ�ѡ��";

			//20001,5+4,�������+������,��ʾ���񽻸�ʱ�Ի�
			if(%Tid == 910) return Get_Mis_Dialog(%Npc, %Player, %Mid, 5) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 4);

			//20002,,�����������Ի�,��ʾ����������ڣ����е�׷������
//			if(%Tid == 20002)	return GetMissionNeedText(%Player, %Mid, 9999);

			//20003,,����׷�ٴ����ȸ����֣�����Ŀ��+������
			if(%Tid == 800)
				{
					if(%Mid == 20001)
						return Mission20001Dialog(%Npc, %Player, %Mid, %Tid);
					else if(%Mid == 20002)
						return Mission20002Dialog(%Npc, %Player, %Mid, %Tid);
					else if(%Mid == 20003)
						return Mission20003Dialog(%Npc, %Player, %Mid, %Tid);
					else
						return $Get_Dialog_GeShi[31201] @ Get_Mis_Dialog(%Npc, %Player, %Mid, 1) @ GetMissionNeedText(%Player, %Mid, 9999) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 4) @ "</t>";
				}

			//20010,3+1+6+2+4,�����Ѷ�+Ŀ��+����+����+����,���������ʹ��
			if(%Tid == 888)
				{
					if(%Mid == 20001)
						return Mission20001Dialog(%Npc, %Player, %Mid, %Tid);
					else if(%Mid == 20002)
						return Mission20002Dialog(%Npc, %Player, %Mid, %Tid);
					else if(%Mid == 20003)
						return Mission20003Dialog(%Npc, %Player, %Mid, %Tid);
					else
						return $Get_Dialog_GeShi[31201] @ Get_Mis_Dialog(%Npc, %Player, %Mid, 3) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 1) @ GetMissionNeedText(%Player, %Mid, 9999) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 2) @ Get_Mis_Dialog(%Npc, %Player, %Mid, 4) @ "</t>";
				}

//			%T10001 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10001);");	//"����Ŀ��";
//			%T10002 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10002);");	//"��������";
//			%T10004 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10004);");	//"������";
//			%T10005 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10005);");	//"������ɶԻ�";

//			%T20011 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 20011);");	//"�����������Ի�1";
//			%T20012 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 20012);");	//"�����������Ի�2";
//			%T20013 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 20013);");	//"�����������Ի�3";
//			%T20014 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 20014);");	//"�����������Ի�4";
//			%T20015 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 20015);");	//"�����������Ի�4";

//			echo("%T10001 = " @ %T10001);
//			echo("%T10002 = " @ %T10002);
//			echo("%T10003 = " @ %T10003);
//			echo("%T10004 = " @ %T10004);
//			echo("%T10005 = " @ %T10005);
//			echo("%T10006 = " @ %T10006);

			//20011,,�����������Ի�,��ʾ������׷�ٴ�����ɫ����
//			if(%Tid == 20011) return $Get_Dialog_GeShi[31201] @ %T20011;
//			if(%Tid == 20012) return $Get_Dialog_GeShi[31201] @ %T20012;
//			if(%Tid == 20013) return $Get_Dialog_GeShi[31201] @ %T20013;
//			if(%Tid == 20014) return $Get_Dialog_GeShi[31201] @ %T20014;
//			if(%Tid == 20015) return $Get_Dialog_GeShi[31201] @ %T20015;

//			if(%Tid == 20011){return %T20011 @ "<b/>";}
//			if(%Tid == 20012){return %T20012 @ "<b/>";}
//			if(%Tid == 20013){return %T20013 @ "<b/>";}
//			if(%Tid == 20014){return %T20014 @ "<b/>";}
//			if(%Tid == 20015){return %T20015 @ "<b/>";}

			//�Զ��������ż���������������ȥ����ִ����Ӧ�ĺ�����Ѱ��ʣ��Ի�
			%MDFunction = "Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, %Tid);";
			return eval(%MDFunction);
		}
	return "GetDialogText == ���� Npc��"@%Npc@"��Player��"@%Player@"��DialogID��"@%DialogID@"��Id��"@%Param@"��";
}

//��ȡͨ����Ի�
function Get_TY_Dialog(%Npc, %Player, %DialogID, %Param)
{
//	echo("��ȡ������Ի� = " @ %DialogID);
	switch(%DialogID)
	{
		case 0: return $Get_Dialog_GeShi[31401] @ "��ӭ���������ɴ������磡</t><b/><b/>";

		case 1:	return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "�����Ի�</t>";
		case 2:	return "��������";
		case 3:	return $Icon[1] @ $Get_Dialog_GeShi[31204] @ "�������</t>";
		case 4:	return $Icon[4] @ "<t>����</t>";
		case 5:	return "��";
		case 6:	return "��ı����������Ų����ˣ�������һ�°�";
		case 7:	return "��Ʒ���ݴ���";
		case 8:	return "֪����";
		case 9:	return "�鿴��Ʒ";
		case 10:return $Icon[4] @ $Get_Dialog_GeShi[31204] @"�ر�</t>";
		case 11:return $Icon[4] @ "<t>�뿪����</t>";
		case 12:return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "��Ը����</t>";
		case 13:return "��������̨";
		case 14:return "���Ҹı�ĳ�������״̬";
		case 15:return "ֱ�ӽ���";
		case 16:return "ֱ�����";
		case 17:return "ǿ�Ʒ���";
		case 18:return "���ó�δ���״̬";
		case 19:return "����������";
		case 20:return "���»�����";
		case 21:return "��֧�߾��顿";
		case 22:return "��ÿ��ѭ����";
		case 23:return "���������";
		case 24:return "��ָ������";
		case 25:return "����������";
		case 26:return "Ŀǰ���޴�������";
		case 27:return "����Դ������ʲô��";
		case 28:return "�����������ɣ�ϣ��";
		case 29:return "�����������һ�£�9�룩��";
		case 30:return "�����ɣ�С���棡";
		case 31:return "����Ŷ����û���㹻�Ļ��ҡ���";
		case 32:return "��δ���ţ������ڴ���";
		case 33:return $Icon[5] @ "<t>�������</t>";
		case 34:return "��������������������㣬�޷������׶�";
		case 35:return $Icon[5] @ "<t>����ָ��</t>";
		case 36:return $Icon[5] @ "<t>ʹ��</t>";
		case 37:return $Icon[5] @ "<t>����</t>";
	}

	switch(%DialogID)
	{
		case 200:	return "����";
		case 201:	return "��1��";
		case 202:	return "��2��";
		case 203:	return "��3��";
		case 204:	return "��4��";
		case 205:	return "��5��";
		case 206:	return "��6��";
		case 207:	return "��7��";
		case 208:	return "��8��";
		case 209:	return "��9��";
		case 210:	return "��10��";


		case 299:	return "��1��";
		case 298:	return "��2��";
		case 297:	return "��3��";
		case 296:	return "��1����";
		case 295:	return "��2����";
		case 294:	return "��3����";
		case 293: return "ʥ";
		case 292: return "��";
		case 291: return "��";
		case 290: return "��";
		case 289: return "��";
		case 288: return "��";
		case 287: return "��";
		case 286: return "ħ";

		case 309:return "����������ӻ���";
		case 310:return "ʥּ����������ˣ�����ȥ����";
		case 311:return $Icon[4] @ "<t>�����뿪������</t>";
		case 312:return $Icon[4] @ "<t>��λ�á�" @ GetItemData(105100021, 1) @ "��</t>";
		case 313:return "��û�С�" @ GetItemData(105100021, 1) @ "�����޷�ʹ�á�" @ GetItemData(105030012, 1) @ "��</t>";
		case 314:return $Icon[4] @ "<t>��λ�á�" @ GetItemData(105100022, 1) @ "��</t>";
		case 315:return "��û�С�" @ GetItemData(105100022, 1) @ "�����޷�ʹ�á�" @ GetItemData(105030013, 1) @ "��</t>";
		case 316:return $Icon[5] @ "<t>����Ů�����</t>";
		case 317:return $Icon[5] @ "<t>���˼���</t>";
		case 318:return $Icon[5] @ "<t>��Ӽ���</t>";
		case 319:return $Get_Dialog_GeShi[31401] @ "<t>������ʯ���ΰ�Ů����</t><b/><b/>" @
										$Get_Dialog_GeShi[31213] @ "<t>���˼��ݣ�</t><b/>" @
										$Get_Dialog_GeShi[31206] @ "<t> ��ÿ��ֻ�ܼ���һ��</t><b/>" @
										$Get_Dialog_GeShi[31206] @ "<t> ��ÿ�μ�����Ҫ����5����" @ GetItemData(105102018, 1) @ "��</t><b/><b/>" @
										$Get_Dialog_GeShi[31213] @ "<t>��Ӽ��ݣ�</t><b/>" @
										$Get_Dialog_GeShi[31206] @ "<t> ��ÿ��ֻ�ܼ���һ��</t><b/>" @
										$Get_Dialog_GeShi[31206] @ "<t> ������һ����Ҫ5����" @ GetItemData(105102018, 1) @ "��</t><b/>" @
										$Get_Dialog_GeShi[31206] @ "<t> ��ֻ���ɶӳ�������ݣ����ж�Ա�����ڶӳ����</t><b/>";

	}

	//�����������ڵ�ͼ����
	if( (%DialogID >= 1000)&&(%DialogID < 2000) )
		return $Icon[10] @ "<t>��������" @ $Mission_Map[ %DialogID ] @ "��</t>";

	switch(%DialogID)
	{
		case 2000:	return $Icon[6] @ "<t>���࡭��</t>";
		case 2210:	return $Icon[6] @ "<t>�����򿪱���</t>";
		case 2220:	return $Icon[6] @ "<t>�����鿴����</t>";
		case 2230:	return $Icon[6] @ "<t>�����鿴��ͼ</t>";
		case 2240:	return $Icon[6] @ "<t>������ȡ˫������</t>";
		case 2250:	return $Icon[6] @ "<t>�����洢�ֿ�</t>";
		case 2260:	return $Icon[6] @ "<t>������Ӻ���</t>";
		case 2270:	return $Icon[6] @ "<t>������ɶ���</t>";
		case 2280:	return $Icon[6] @ "<t>��Ϸ�ȼ�һ��</t>";

		case 2211:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "ʹ���ȼ���Alt+A�������Դ򿪱������Ҽ���������еĵ��ߣ������Խ���ʹ�û�װ��</t><b/>";
		case 2221:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "ʹ���ȼ���Alt+Q�������Բ鿴���񣬵�������»��ߵ��ַ������Խ���Ѱ��</t><b/>";
		case 2231:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "ʹ���ȼ���Tab�������Բ鿴��ǰ��ͼ��ʹ���ȼ���Alt+M�������Բ鿴ȫ����ͼ</t><b/>";
		case 2241:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "������ǵ�npc���ǶԻ�����������ȡ˫�����飬Ҳ���Դ�����˫������</t><b/>";
		case 2251:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "���������ǵ�Ǯׯ�ƹ񴦣������Դ洢�ֿ�</t><b/>";
		case 2261:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "ѡ����Ҫ��ӵ���Һ���Ŀ�����ϵ����Ҽ���ѡ�С���Ӻ��ѡ����ɣ�����ʹ�á�Alt+F���������ѽ��棬ѡ����Ӳ�������Է���������</t><b/>";
		case 2271:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @ "ʹ���ȼ���Alt+T������������棬��������󼴿�����ɶ��飬����ѡ��Ŀ��󣬵����Ҽ�ѡ�����������飬�ɹ����Զ���ɶ���</t><b/>";
		case 2281:	return $Get_Dialog_GeShi[31401] @ Get_TY_Dialog(%Npc, %Player, %DialogID - 1, %Param) @ "</t><b/><b/>" @ $Get_Dialog_GeShi[31201] @
											 "<t>��Alt+C��  �������ԣ�������Ϣ��</t><b/>" @
											 "<t>��Alt+S��  ְҵ����</t><b/>" @
											 "<t>��Alt+A��  ���˰���</t><b/>" @
											 "<t>��Alt+T��  ����</t><b/>" @
											 "<t>��Alt+Q��  �����ѽ�����</t><b/>" @
											 "<t>��Alt+X��  ����(�ٻ���)</t><b/>" @
											 "<t>��Tab��    ��ǰ��ͼ</t><b/>" @
											 "<t>��Alt+M��  ���ͼ</t><b/>" @
											 "<t>��Alt+F��  �罻��ϵ�����ˣ�</t><b/>" @
											 "<t>��Alt+G��  ���</t><b/>" @
											 "<t>��Alt+W��  ����Ԫ����Ϣ��</t><b/>" @
											 "<t>��Alt+D��  ���������죩</t><b/>" @
											 "<t>��Alt+H��  ����</t><b/>" @
											 "<t>��Alt+V��  �̳�</t><b/>" @
											 "<t>��Alt+E��  �������ţ��ռ��䣩</t><b/>" @
											 "<t>��Alt+B��  AC</t><b/>" @
											 "<t>��Alt+N��  ������</t><b/>" @
											 "<t>��Alt+L��  ������Ϸ</t><b/>" @
											 "<t>��Alt+I��  ��׭¼</t><b/>";

	}
	switch(%DialogID)
	{
		case 3400:	return $Icon[6] @ " <t>���ڼ���ָ��</t>";
		case 3401:	return $Icon[4] @ " <t>�������о��о��ɡ���</t>";

		case 3410:	return $Get_Dialog_GeShi[31401] @ "������ӵ������������ϣ�����������ư�����  ά������</t>";
		case 3411:	return $Icon[4] @ " <t>�ư����£���������Ͱ���֮��</t>";
		case 3412:	return $Icon[4] @ " <t>ά��������ǿ�߱������������</t>";

		case 3420:	return $Get_Dialog_GeShi[31401] @ "�����������ѡ����ϣ�����һ���� ������ ���� ��������</t>";
		case 3421:	return $Icon[4] @ " <t>�����������������޴���</t>";
		case 3422:	return $Icon[4] @ " <t>���������������Ŀ�����˪��ƽ������</t>";

		case 3430:	return $Get_Dialog_GeShi[31401] @ "����ս�ܣ�����������Ļ���ս��ɳ��</t>";
		case 3431:	return $Icon[4] @ " <t>������ģ����ӱ���10�겻��</t>";
		case 3432:	return $Icon[4] @ " <t>ս��ɳ����ֻ��ս��������ս��</t>";

		case 3440:	return $Get_Dialog_GeShi[31401] @ "��ƽ֮ʱ�����ϲ���������Ի���ϰ��ǿ��</t>";
		case 3441:	return $Icon[4] @ " <t>�������ԣ��������������Ϊ���Ǵ���</t>";
		case 3442:	return $Icon[4] @ " <t>ϰ��ǿ�����ڵ�ǿ��������Ļ���</t>";

		case 3450:	return $Get_Dialog_GeShi[31401] @ "�����Ѷ�ս����Ը��Ϊì���ǻ�Ϊ�ܣ�</t>";
		case 3451:	return $Icon[4] @ " <t>ì����Ը��Ϊӭ�����˵�����</t>";
		case 3452:	return $Icon[4] @ " <t>�ܣ���Ը��Ϊ�ֵ������Ķ�</t>";

		case 3460:	return $Get_Dialog_GeShi[31401] @ "��������֮�֣����ϣ������Ծ����ǹ��䲻��</t>";
		case 3461:	return $Icon[4] @ " <t>�����Լ�ʵ��������Ծ��кβ���</t>";
		case 3462:	return $Icon[4] @ " <t>���䲻������ʡһ����ʡһ��</t>";

		case 3470:	return $Get_Dialog_GeShi[31401] @ "��Ը��ǰ��Ϊǿ������ϻ����ں�Ϊ��ʵ��������</t>";
		case 3471:	return $Icon[4] @ " <t>���ϣ���ϲ����ֱ�ӵ�����</t>";
		case 3472:	return $Icon[4] @ " <t>��������Ը��Ϊ������������ĺ�Ԯ</t>";

		case 3501:	return "Do U Need Help ��";
		case 3502:	return "����NPCλ�����£�";
		case 3500:	return "�鿴�����ڵ�NPCλ��";
		case 3510:	return "��Ҫȥ��������";
		case 3520:	return "��Ҫ�س�ȥ";
		case 3529:	return "��Ҫ�س�ȥ������ǣ�XX��";
		case 3530:	return "ûʲô��Ҫ�����ģ�лл";

		case 3600:	return "�����������";
		case 3700:	return "��Ҫ��ȡ��������";

		case 4000:	return "���ҿ���������ʣ�";

		default:
			if( (%DialogID > 3490)&&(%DialogID < 3499) )
				return $Get_Dialog_GeShi[31401] @ "<t>�������ѡ�����ʺ���������ǣ�</t><b/><b/><t>   ��"@$FamilyName[%DialogID - 3490, 1]@"��</t></t>";

	}
	switch(%DialogID)
	{
		case 4010: return $Icon[6] @ " <t>���ڱ���</t>";
    case 4011: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4012: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4013: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4014: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4015: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4016: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4017: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4018: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";

		case 4020: return $Icon[6] @ " <t>������ɫ</t>";
    case 4021: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4022: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4023: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4024: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4025: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4026: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4027: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4028: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";

    case 4030: return $Icon[6] @ " <t>��������</t>";
    case 4031: return $Get_Dialog_GeShi[31401] @ "������裺�������</t><b/>"@"<t>���������������������������ԣ�һ��ֻ��һ������֪ƣ�벻��ˮ�ݣ��¿�����ǧ���Ͽ�̤�ƶ��У���ʥ�߲�������</t>";
    case 4032: return $Get_Dialog_GeShi[31401] @ "������裺��������</t><b/>"@"<t>���ն���������������ܷ𷨽̻�������ͨ�����������Ծ޴�֮���˷���ƶ��С�</t>";
    case 4033: return $Get_Dialog_GeShi[31401] @ "������裺���¹</t><b/>"@"<t>����������¹��������������һ��ʮ�ɡ�����Ϊ���������������ɽӽ������¹��ͷ����������Ϊ���ۣ���ΪԽ��ɫ��Խ���ޡ�</t>";
    case 4034: return $Get_Dialog_GeShi[31401] @ "������裺������</t><b/>"@"<t>Ϊ������ɢ�䷲�����������Ի����������Ի�Ϊ�飬����֮��Ψһ���������δ��ѱ����������������Ը��ٻ��������¡�</t>";
    case 4035: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4036: return $Get_Dialog_GeShi[31401] @ "������裺��Ϭ</t><b/>"@"<t>˫��ɽ��������Ϭ�������ʣ���Ƥ���ɻ�۵�������������ᱻ��ʯ���ǣ����麩���׼�Ԧ����ǧ�����ܳ���ɽ��֮�С�</t>";
    case 4037: return $Get_Dialog_GeShi[31401] @ "�����ڴ�</t>";
    case 4038: return $Get_Dialog_GeShi[31401] @ "������裺ħ����Ȯ</t><b/>"@"<t>��Ԩ���е����������ħ����Ⱦ������б�����ɱ�κο����Ļ����ѱ����ȮҲΪħ�ڵ��ӱ���֮�Σ����е�����ɥ�ڴˡ�</t>";

    case 4040: return $Icon[6] @ " <t>����ʥ��</t>";
    case 4041: return $Get_Dialog_GeShi[31401] @ "���ع���</t><b/>"@"<t>���ع���ΡΡ�۾ᣬ��ʮ�����գ������ء���ʮ����������ڴ����ۣ�һ���޽��������ء���ɽ��һ����ʥ����֮�أ��ơ���ڣ��</t>";
    case 4042: return $Get_Dialog_GeShi[31401] @ "������</t><b/>"@"<t>������λ�ڼ������죬�������������������գ�������������̨����⡣һ�����Ӳ�ײ���죬ը��һƬƬ��ɫ���ģ��ڿ��л���Ʈɢ��</t>";
    case 4043: return $Get_Dialog_GeShi[31401] @ "�����ɾ�</t><b/>"@"<t>�����ɾ�������ˮ�������˶�����ӿ�Ĵ��ٲ�����һ���µ�����Ӧ�������֮λ��������������ܲ�Ǭ���������������Ž������</t>";
    case 4044: return $Get_Dialog_GeShi[31401] @ "��ľ��</t><b/>"@"<t>�������µ���ľ�֣���һ�ò������Ϊ�ģ����ܰٻ���ӵ�����������ľ�����ϹŶ����������֦�������Ũ����������Ȼ�����л���֮�ܡ�</t>";
    case 4045: return $Get_Dialog_GeShi[31401] @ "��ڤ����</t><b/>"@"<t>��ڤ���������¼���֮�أ��ޱ��޼ʣ��������󣬻�Ȫ���ڡ��˵�����ȴ��Ҷ���й�ȴ��ů�⡣���������̨Ϊ�ģ��������಻����</t>";
    case 4046: return $Get_Dialog_GeShi[31401] @ "˫��ɽ</t><b/>"@"<t>���嶫��֮��˫��ɽ����ɽ��Ᾱ��죬��ɽ�����һ����ΰ�Ĺ�������޲��Ǵ���������������磬�����º�ɽΪһ�塣</t>";
    case 4047: return $Get_Dialog_GeShi[31401] @ "���α���</t><b/>"@"<t>���α���λ����ԯ���ݱ��棬������������֮�þ���������������λù������������һ�������������</t>";
    case 4048: return $Get_Dialog_GeShi[31401] @ "��ϦԨ</t><b/>"@"<t>��ϦԨλ�ڴ���ڲ�������ħһ�ƻ��ɣ���Ϊ������֮�գ��������أ���Ϊ��Ϧ����������һ����ħ��Ϊ����������</t>";



	}

	switch(%DialogID)
	{
		case 4201: return $Icon[6] @ "<t>����ѧϰ����ҩ��</t>";
		case 4202: return $Icon[6] @ "<t>����ѧϰ���ʳ��</t>";
		case 4203: return $Icon[6] @ "<t>����ѧϰ��������</t>";
		case 4204: return $Icon[6] @ "<t>����ѧϰ�����¿�</t>";
		case 4205: return $Icon[6] @ "<t>����ѧϰ�ӹ�����</t>";
		case 4206: return $Icon[6] @ "<t>�����������</t>";
		case 4207: return $Icon[6] @ "<t>����ιʳ����</t>";
		case 4208: return $Icon[6] @ "<t>���޹���˵��</t>";
		case 4209: return $Icon[6] @ "<t>����������</t>";
		case 4210: return $Icon[6] @ "<t>������̯</t>";
		case 4211: return $Icon[6] @ "<t>�����ڿ�</t>";
		case 4212: return $Icon[6] @ "<t>������ľ</t>";
		case 4213: return $Icon[6] @ "<t>��������</t>";
		case 4214: return $Icon[6] @ "<t>������ֲ</t>";
		case 4215: return $Icon[6] @ "<t>������ҩ</t>";
		case 4216: return $Icon[6] @ "<t>��������</t>";
		case 4217: return $Icon[6] @ "<t>������������</t>";
		case 4218: return $Icon[6] @ "<t>���������¿�</t>";
		case 4219: return $Icon[6] @ "<t>�����ӹ�����</t>";
		case 4220: return $Icon[6] @ "<t>������������</t>";
		case 4221: return $Icon[6] @ "<t>�������ɵ�ҩ</t>";
		case 4222: return $Icon[6] @ "<t>�������ʳ��</t>";
		case 4223: return $Icon[6] @ "<t>��������Ҿ�</t>";
		case 4224: return $Icon[6] @ "<t>����������</t>";
		case 4225: return $Icon[6] @ "<t>�����չ��Ԫ��</t>";
		case 4226: return $Icon[6] @ "<t>������ö���</t>";
		case 4227: return $Icon[6] @ "<t>�����Ĺ���</t>";
		case 4228: return $Icon[6] @ "<t>˫�������˵��</t>";

		case 4301: return "����ҩƷ��һ�Ÿ�����ַ��ӵ�ѧ�ʣ�20��ʱ��ϰ�á���ҩ��������ȡҩ�ģ�30��ʱ��ѧ�ᡰ����������ҩ����Ϊ��ҩ��";
		case 4302: return "����ʳΪ�죬Ҫ���ʳ����������Ӧ��ʳ�ģ�ʳ����Դ�кܶ࣬�������ģ����Եģ���ֲ�����ġ�";
		case 4303: return "��������ǰ���������Ĳ���ѡ��30����ѧϰ���ڿ󡰺͡����조���ż��ܣ��Ϳ��Խ��������ˣ�";
		case 4304: return "����ʱҲӦ����ǿʳ�ķ�����������ķ�����Ҳ����Ҫ��10��ʱ����ѧϰ�����ԡ���ò��ϣ�15��ȥѧϰ���÷족�Ϳ���������";
		case 4305: return "���εĿ��Ը�������ܶ��޷������Ч�����������εĻ�ú����⣬��ֻ����40��ʱѧϰ�����ա�������ȫ������";
		case 4306: return "60�����¿���ȥ�����޹ȡ��в�׽���ޣ�����һ��Ҫ���ϡ���������Ŷ����ͬ�������в�ͬ��ר������ʹͬһ������Ҳ�гɳ��Ĳ�ͬ";
		case 4307: return "��������޿��֡���������ʱ������޷��ٻ�������ս���ˣ���ʱ�����Ҫ������߻�������ʳ����ι�����ˣ����ι���Ϳ�����";
		case 4308: return "�����ڴ�!";
		case 4309: return "�����ڴ�!";
		case 4310: return "������ǽ��׸ۿڣ��ſ��Խ��а�̯����̯�İ취�ܼ򵥣��㿪��Ǭ���������棬�����̯�Ϳ��Է�����Ҫ���ۻ����չ��Ķ�����";
		case 4311: return "ѧ���ˡ��ڿ��Ժ󣬻�Ҫ���򡰿�䡱�����ڿ�׼���׵����Ժ�ȥ������Ұ��Ŀ����������ؾͿ����ˡ�";
		case 4312: return "ѧ���ˡ���ľ���Ժ󣬻�Ҫ���򡰸�ͷ�����ܷ�ľ��׼���׵����Ժ�ȥ������Ұ��ķ�ľ������ľ�ľͿ����ˡ�";
		case 4313: return "ѧ���ˡ����㡱�Ժ󣬻�Ҫ������͡����ܵ��㣬׼���׵����Ժ�������߾���һ���������򣬵����Ⱥ�Ϳ����ˡ�";
		case 4314: return "ѧ���ˡ���ֲ���Ժ󣬻�Ҫ�������ӡ��ſ�����ֲ��������߾��и��أ��ڸ�����ѡ��Ҫ�ֵĶ����Ϳ����ˣ�Ҫע����ֲ��Ҫʱ��Ŷ��";
		case 4315: return "ѧ���ˡ���ҩ���Ժ󣬻�Ҫ�����������ſ��Բɼ�������������һ���Ĳ�ҩ���򣬵���Ϳ����ˡ�";
		case 4316: return "ѧ���ˡ����ԡ��Ժ󣬻�Ҫ�����Թ����ſ������ԣ��������Ϳ������Ե�һ�������";
		case 4317: return "�����������������ѣ���Ϊ�ܶ���������췽�����ǲ������ġ���ֻҪ������ǵġ��䷽������ֻҪ׼���ò��ϾͿ�����������õ�������";
		case 4318: return "�����¿��ܼ򵥣���ӵ���䷽��׼���ò��ϣ�������ʲô����ʲô��";
		case 4319: return "���ȣ���Ҫ������ε������䷽�������䷽������Ĳ���ֻҪ�ռ������Ͳ��������ˡ�";
		case 4320: return "��������������ܸ���";
		case 4321: return "����㹻��ҩ�ĺ���Ҫѧϰ���ֵ�ҩ���䷽�������䷽�ϵı����ͻ���������������ĵ�ҩ��";
		case 4322: return "���ʳ��Ҫ����ȥ������ᷢ��ʳ�ﲢ��ֻ�ǳ伢���������������벻����Ч�������磬��ʱ���ڰٶ����֣���Ȼ��ǰ������Ҫ����䷽��";
		case 4323: return "����ӵ�ж���֮����ᷢ��һ���õļҾߣ���������֮·�ϴ������洦�Ƕ�ô�������������˾�Ԫ�غ���ĳ�������Լ���þ�����";
		case 4324: return "�����ڴ�!";
		case 4325: return "�����õ�����չ��Ԫ��Ļ�Ԫ�����㽫�������ڻ�Ԫ������Ŀո�λ����Ϳ�����չ�ˡ�!";
		case 4326: return "�����ڴ�!";
		case 4327: return "һ���õ�װ�����Լ������ԣ�������������ͨʱ�޷���֪�ġ�����һ�� ������������Ϳ��Ը����ṩ���й�����������������!";
		case 4328: return "�����ڴ�!";

	}

  switch(%DialogID)
	{
		case 10000: return "<t>С���ɣ���ĵȼ���û�е�10�������ٽ��������Һ�������ı����أ�</t><b/>"@ $Get_Dialog_GeShi[31206] @"<t>����ʾ:alt+q���Դ�����˵�,���Ѿ����ܵ�������Խ��в鿴,����Ѱ�����ٵ��������.ͬʱ�������ڿɽ������в鿴û�н��ܵ�������.tab�����Դ򿪵�ͼ,�鿴npc�ķ�λ��Ұ��ķ���,��Ұ���ɱ����Ҳ���Կ��ٵĵ���10��Ŷ!��</t>";
		case 10001: return "��ĳ��ﻹδ����10��,��ȥ������������.";
		case 10002: return "<t>��ĳ����������û�п�������û�г�������ô��������أ�</t><b/>"@ $Get_Dialog_GeShi[31206] @"<t>����ʾ���򿪱�����ʹ�þ����Ĵ���÷�ӡ���㼴�ɻ��һֻ���޻�飩</t>";
		case 10112: return "�㻹ûץס����,��ȥ���ץ��.";
		case 10113:	return "<t>�㻹û�б����أ����޷����������ҩ��Ч��˵����!</t><b/>"@ $Get_Dialog_GeShi[31206] @"<t>����ʾ���㿪������ʹ������õ��ı����������Ϳ�����������ˣ�</t>";
		case 10114:	return "������ȵ��������������ҩ������ȥ���ɺײ�.";
    case 10116:	return "ǰ�����³ؾͿ�����ȡ����������.";
    case 10140: return "��ĵȼ�δ�ﵽ10��,�����ľ�������,����������Կ�,������·���������ť��";
    case 41010: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "��Ը����������</t>";
    case 41020: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "��Ը���뼫������</t>";
    case 41030: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "��Ը���������ɾ�</t>";
    case 41040: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "��Ը������ľ��</t>";
    case 41050: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "��Ը������ڤ����</t>";
    case 41060: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "��Ը����˫��ɽ</t>";
    case 41070: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "��Ը�������α���</t>";
    case 41080: return $Icon[4] @ $Get_Dialog_GeShi[31204] @ "��Ը������ϦԨ</t>";
	}

	switch(%DialogID)
	{
		case 20000: return $Get_Dialog_GeShi[31401] @ "Ŀ���ͼ��δ���š���</t><b/>";
		case 99999:	return $Get_Dialog_GeShi[31401] @ "<t>��ӭ���������ɴ��������磡</t><b/><b/>";

	}
	switch(%DialogID)
	{
		case 101111: return "�㻹û�в�׽����"@GetItemData(118010001,1)@"����ȷ��Ҫ�뿪������";//��һ�β�׽
		case 101651: return "�㻹û��ȫ����ɡ�������ҹ��������ȷ��Ҫ�뿪������";//������ҹ��������
	}
	switch(%DialogID)
	{
		case 200001: return $Icon[5] @ "<t>װ������</t>";
		case 200003: return $Icon[9] @ "<t>����ѡ�����</t>";
		case 200004: return $Icon[9] @ "<t>����ѧϰ����</t>";
		case 200005: return $Icon[5] @ "<t>װ��ǿ��</t>";
		case 200006: return $Icon[5] @ "<t>װ�����</t>";
		case 200007: return $Icon[7] @ "<t>����Ǯׯ</t>";
		case 200008: return $Icon[5] @ "<t>�����޸�װ��</t>";
		case 200009: return $Icon[5] @ "<t>����;��޸�</t>";

		case 200109: return $Icon[9] @ "<t>�����ڿ���ѧϰ����</t>";
		case 200110: return $Icon[9] @ "<t>������ľ����ѧϰ����</t>";
		case 200111: return $Icon[9] @ "<t>�������㼼��ѧϰ����</t>";
		case 200112: return $Icon[9] @ "<t>������ֲ����ѧϰ����</t>";
		case 200113: return $Icon[9] @ "<t>������ҩ����ѧϰ����</t>";
		case 200114: return $Icon[9] @ "<t>�������Լ���ѧϰ����</t>";
		case 200101: return $Icon[9] @ "<t>�������켼��ѧϰ����</t>";
		case 200102: return $Icon[9] @ "<t>�����÷켼��ѧϰ����</t>";
		case 200103: return $Icon[9] @ "<t>�������ռ���ѧϰ����</t>";
		case 200104: return $Icon[9] @ "<t>�������似��ѧϰ����</t>";
		case 200105: return $Icon[9] @ "<t>������������ѧϰ����</t>";
		case 200106: return $Icon[9] @ "<t>������⿼���ѧϰ����</t>";
		case 200107: return $Icon[9] @ "<t>������������ѧϰ����</t>";
		case 200108: return $Icon[9] @ "<t>������������ѧϰ����</t>";

		case 200201: return $Icon[5] @ "<t>�������</t>";
		case 200202: return $Icon[5] @ "<t>����������</t>";
		case 200203: return $Icon[5] @ "<t>��������</t>";
		case 200204: return $Icon[5] @ "<t>���ﻹͯ</t>";
		case 200205: return $Icon[5] @ "<t>���ﷱֳ</t>";

		case 200211: return $Icon[5] @ "<t>��������</t>";
		case 200212: return $Icon[5] @ "<t>��������</t>";
		case 200213: return $Icon[5] @ "<t>����ǿ��</t>";
		case 200214: return $Icon[5] @ "<t>��������</t>";
		case 200215: return $Icon[5] @ "<t>�����ϳ�</t>";

		case 200221: return $Icon[5] @ "<t>��ʾ��</t>";
		case 200222: return $Icon[5] @ "<t>ͨ����</t>";
		case 200223: return $Icon[5] @ "<t>�����</t>";
		case 200224: return $Icon[5] @ "<t>�������</t>";
		case 200225: return $Icon[5] @ "<t>��Ҫ���</t>";
	}
	switch(%DialogID)
	{
		case 300000: return "�������(����)";
		case 300001: return "�����þ���(����)";
		case 300002: return "�����ý�Ǯ(����)";
		case 300003: return $Icon[5] @ "<t>��ȡ����״̬(С��30��)</t>";
		case 300004: return $Icon[5] @ "<t>��ȡ�¹ⱦ��(����)</t>";
		case 300005: return $Icon[5] @ "<t>�����ܣ������</t>";
		case 300006: return "��Ҫ��ʦ";
		case 300007: return "��Ҫ��ͽ";
		case 300008: return "����ʦ���ȼ�";

		case 300011: return "�����1��(����)";
		case 300012: return "�����10��(����)";
		case 300013: return "�����50��(����)";
		case 300014: return "�����120��(����)";

		case 300015: return $Icon[5] @ "<t>�����Ѫ���շѣ�</t>";
		case 300016: return $Icon[5] @ "<t>�����Ѫ����ѣ�</t>";
		case 300017: return "��һ�ڲ����շѱ�׼���£�ÿ100HP�շ�1Ǯ���������1Ǯ�����ܱ��˰ɣ�";
		case 300018: return $Icon[5] @ "<t>��һ��</t>";
		case 300019: return "��Ҳ������㣬��ϧ����δ���ţ�";
		case 300020: return "��̨��������ܰ���û���Ⱑ��";

		case 300021: return $Icon[5] @ "<t>ǰ������</t>";
		case 300022: return $Icon[5] @ "<t>�Ѻö�ѭ��</t>";
		case 300023: return $Icon[5] @ "<t>��ȡ˫������</t>";
		case 300024: return $Icon[5] @ "<t>����˫������</t>";
		case 300025: return $Icon[4] @ "<t>����ǵ�����</t>";
		case 300026: return $Icon[5] @ "<t>�����ܣ���������-����</t>";
		case 300027: return $Icon[5] @ "<t>�����ܣ���������-����</t>";
		case 300028: return $Icon[5] @ "<t>���������ܣ���������-����</t>";
		case 300029: return $Icon[5] @ "<t>���������ܣ���������-����</t>";
		case 300030: return $Icon[5] @ "<t>�����µķ�֧���ܣ���������</t>";
		case 300031: return "<t>���������ܣ����������������ڿ���������һ���������ܣ�</t><b/>" @
												"<t>��������-����</t>" @
												"<t>��������-����</t>" @
												"<t>������������һ���������ܺ󣬽���������������һ����������</t>";
		case 300032: return $Icon[5] @ "<t>���������ܣ���������-�����꡿</t>";
		case 300033: return $Icon[5] @ "<t>���������ܣ���������-����ѩ��</t>";
		case 300034: return $Icon[5] @ "<t>���������ܣ������</t>";
		case 300035: return $Icon[5] @ "<t>���������ܣ������</t>";
		case 300036: return $Icon[4] @ "<t>����Ů�</t>";


		case 399999:	return $Get_Dialog_GeShi[41401] @ "��������Ҫ�˽�һЩ����������</t><b/>"@
												 $Get_Dialog_GeShi[41402] @ "�����[����]��ť����ݼ�Alt+Q���ɲ鿴�Ѿ����ܵ�����������Ϣ</t><b/>"@
												 $Get_Dialog_GeShi[41402] @ "��������̵ġ���ݼ�Alt+H���ɲ鿴��Ϸ�Ĳ�������˵��</t><b/>"@
												 $Get_Dialog_GeShi[41401] @ "ͬʱ�����ǽ����ķݾ�ϲ�������͸�����</t><b/>"@
												 $Get_Dialog_GeShi[41401] @ "���ĵ�һ�������ǣ���������������Դ򿪱�������ݼ�Alt+A���鿴�����</t><b/>"@
												 $Get_Dialog_GeShi[41401] @ "�ٹ�5���ӣ�����������͵ĵڶ��ݾ�ϲ����</t><b/><b/>"@
												 $Get_Dialog_GeShi[31401] @ "����ǰ�ġ����������ȴ�����������֮·Ϊ����������������������ע���ķ���֮�ˣ�</t><b/>";
	}

	switch(%DialogID)
	{
//		case 400010: return "�򿪲����̵�";
//		case 400012: return "����Ƕ�����̵�";

		case 400001: return $Icon[8] @ "<t>���е��߲��ԣ�	400001</t>";
		case 400002: return $Icon[8] @ "<t>��Ƕ�����̵꣺	400002</t>";
		case 400003: return $Icon[8] @ "<t>1--80���������ԣ�	400003</t>";
		case 400004: return $Icon[8] @ "<t>1--80�����߲��ԣ�	400004</t>";
		case 400005: return $Icon[8] @ "<t>1--80����Ʒ���ԣ�	400005</t>";
		case 400006: return $Icon[8] @ "<t>ǿ�������̵꣺	400006</t>";
		case 400007: return $Icon[8] @ "<t>���������̵꣺	400007</t>";
		case 400008: return $Icon[8] @ "<t>ģ�Ͳ����̵꣺	400008</t>";
		case 400009: return $Icon[8] @ "<t>��������̵꣺  400009</t>";
		case 400010: return $Icon[8] @ "<t>�����̵꣺  400010</t>";

		case 402011: return $Icon[8] @ "<t>�������̵�</t>";
		case 402021: return $Icon[8] @ "<t>�򿪷����̵�</t>";
		case 402031: return $Icon[8] @ "<t>�������̵�</t>";
		case 402041: return $Icon[8] @ "<t>���ӻ��̵�</t>";
		case 402042: return $Icon[8] @ "<t>��ʳ���̵�</t>";
		case 402051: return $Icon[8] @ "<t>��ҩƷ�̵�</t>";
		case 402061: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 402071: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 402081: return $Icon[8] @ "<t>�򿪹����̵�</t>";

		case 402101: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 402111: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 402131: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 402141: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 402151: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 402161: return $Icon[8] @ "<t>�����޵����̵�</t>";
		case 402181: return $Icon[8] @ "<t>�򿪼������̵�</t>";

//		case       :" �򿪹����̵�</t>";
//		case       :" �򿪹����̵�</t>";
//		case       :" �򿪷����̵�</t>";
//		case       :" ������װ���̵�</t>";
//		case       :" ������̵�</t>";

		case 405011: return $Icon[8] @ "<t>�������̵�</t>";
		case 405021: return $Icon[8] @ "<t>�򿪷����̵�</t>";
		case 405031: return $Icon[8] @ "<t>�������̵�</t>";
		case 405041: return $Icon[8] @ "<t>���ӻ��̵�</t>";
		case 405042: return $Icon[8] @ "<t>��ʳ���̵�</t>";
		case 405051: return $Icon[8] @ "<t>��ҩƷ�̵�</t>";
		case 405061: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 405071: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 405081: return $Icon[8] @ "<t>�򿪹����̵�</t>";

		case 405101: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 405111: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 405131: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 405141: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 405151: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 405161: return $Icon[8] @ "<t>�����޵����̵�</t>";
		case 405162: return $Icon[8] @ "<t>������װ���̵�</t>";
		case 405171: return $Icon[8] @ "<t>������̵�</t>";
		case 405181: return $Icon[8] @ "<t>�򿪼������̵�</t>";
		case 405191: return $Icon[8] @ "<t>�򿪷����̵�</t>";
		case 405201: return $Icon[8] @ "<t>�򿪹����̵�</t>";
		case 405211: return $Icon[8] @ "<t>�򿪹����̵�</t>";


		case 411171: return $Icon[8] @ "<t>�����̵�</t>";
		case 412171: return $Icon[8] @ "<t>�����̵�</t>";
		case 413171: return $Icon[8] @ "<t>�����̵�</t>";
		case 414171: return $Icon[8] @ "<t>�����̵�</t>";
		case 415171: return $Icon[8] @ "<t>�����̵�</t>";
		case 416171: return $Icon[8] @ "<t>�����̵�</t>";
		case 417171: return $Icon[8] @ "<t>�����̵�</t>";
		case 418171: return $Icon[8] @ "<t>�����̵�</t>";

	}

	return "Get_TY_Dialog == ���� Npc��"@%Npc@"��Player��"@%Player@"��DialogID��"@%DialogID@"��Id��"@%Param@"��";
}
//��ȡ������ضԻ�
function Get_Item_Dialog(%Npc, %Player, %DialogID, %Param)
{
	if(%DialogID > 0)
		{
			switch$(%DialogID)
			{
				case "108020048":
					%Txt = "��δ�����롾"@GetItemData(%DialogID,1)@"����ص������޷�ʹ�ô���Ʒ";
					if( (%Player.IsAcceptedMission(10113))||(%Player.IsAcceptedMission(10114)) )
						{
							if(%Player.GetMissionFlag(10113, 1300) == 0){%Txt = GetItemData(%DialogID,1) @ "�ڿտ���Ҳ����δ��������֮����";}
							if(%Player.GetMissionFlag(10113, 1300) == 1){%Txt = GetItemData(%DialogID,1) @ "�����ģ��Ѿ�����������֮����";}
							if(%Player.IsAcceptedMission(10114)){%Txt = GetItemData(%DialogID,1) @ "�����ģ��Ѿ�����������֮��������ȥ�ҡ�</t><c cid='" @ Get_TraceButton() @ "' cmd='\"findpath(400001060);\",\"" @ $Get_Dialog_GeShi[50000] @ "����</t>\",\"GuiMissionTraceButtonProfile6\"' cf='createButton' /><t>�����������";}
						}
					return $Get_Dialog_GeShi[31201] @ "<t>���ǡ�" @ GetItemData(%DialogID,1) @ "������˵���ڡ�</t>" @ "<c cid='" @ Get_TraceButton() @ "' cmd='\"findpath("@%DialogID@");\",\"" @ $Get_Dialog_GeShi[50000] @ "���³���</t>\",\"GuiMissionTraceButtonProfile6\"' cf='createButton' />" @ "<t>������������֮������</t><b/><b/><t>" @ %Txt @ "</t></t>";

				case "105089001":
					return $Get_Dialog_GeShi[31201] @ "<t>���Ѿ�ѧ���ˡ�����������ܣ�������ܽ���������ĵȼ��������������������㵽��10����20����30��������ʱ��������ҿ����ɡ�</t></t><b/><b/>" @
								 $Get_Dialog_GeShi[31206] @ "<t>��Alt+S���������ܽ��棬��ս�����ܵ��ۺϼ��������ҵ�������������ܣ���������Ҽ�ץȡ��Ŀ�������ڿ�����Ͻ���ʹ��</t>";

				case "108020060":
					return $Get_Dialog_GeShi[31206] @ "<t>С�氮�ޣ�</t><b/>" @
								 "<t>����壬�����С��������飬�����������˿̻���������£��п���ǧ�������������ʮ���꣬���кκ���</t><b/>" @
								 "<t>�ӽ���������һ�ˣ������࣬�㴨���㲻ס��Ω�������ٱ���</t></t><b/>";

				case "108020048":return "�������ӡ�Ѿ��������ˣ�ȥ������ �����ܰ�������ӡ��";

//				case "100000001":return "����Ի�����µ�������Ʒ������鿴��Ʒ";
//				case "100000002":return "����Դ�������Ʒ������������ĳ����Ʒ������鿴��Ʒ����";
//				case "100000003":return "����Ҫ������������Ʒ������ѡ����ĳ����Ʒ������鿴��Ʒ����";
//				case "100000004":return "����Ի�����µ�������Ʒ������鿴��Ʒ";

				case "108020208":return "���ţ����ţħ���������ϵۣ�";

				case "105099001":return $Get_Dialog_GeShi[31401] @ "<t>Ӧ��ش����˶��������飬�ǲ��ɶ�õ�����鱦�������������������澫��������Ҳ���Խ��ף�����Ϊ��������Ϸ�����</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�㵱ǰ�ı������鴦�ڣ���"@GetItemData(%DialogID,1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�������������������ɽ���Ϊ����"@GetItemData(105099002,1)@"��</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ���������˴�����֮һ</t></t><b/>";
				case "105099002":return $Get_Dialog_GeShi[31401] @ "<t>Ӧ��ش����˶��������飬�ǲ��ɶ�õ�����鱦�������������������澫��������Ҳ���Խ��ף�����Ϊ��������Ϸ�����</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�㵱ǰ�ı������鴦�ڣ���"@GetItemData(%DialogID,1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�������������������ɽ���Ϊ����"@GetItemData(105099003,1)@"��</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ���������״̬</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ������ȼ��ﵽ13��</t></t><b/>";
				case "105099003":return $Get_Dialog_GeShi[31401] @ "<t>Ӧ��ش����˶��������飬�ǲ��ɶ�õ�����鱦�������������������澫��������Ҳ���Խ��ף�����Ϊ��������Ϸ�����</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�㵱ǰ�ı������鴦�ڣ���"@GetItemData(%DialogID,1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�������������������ɽ���Ϊ����"@GetItemData(105099004,1)@"��</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> �����һλʦ��</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ������ȼ��ﵽ17��</t></t><b/>";
				case "105099004":return $Get_Dialog_GeShi[31401] @ "<t>Ӧ��ش����˶��������飬�ǲ��ɶ�õ�����鱦�������������������澫��������Ҳ���Խ��ף�����Ϊ��������Ϸ�����</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�㵱ǰ�ı������鴦�ڣ���"@GetItemData(%DialogID,1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�������������������ɽ���Ϊ����"@GetItemData(105099005,1)@"��</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ��Ԫ��ȼ��ﵽ5��</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ������ȼ��ﵽ23��</t></t><b/>";
				case "105099005":return $Get_Dialog_GeShi[31401] @ "<t>Ӧ��ش����˶��������飬�ǲ��ɶ�õ�����鱦�������������������澫��������Ҳ���Խ��ף�����Ϊ��������Ϸ�����</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�㵱ǰ�ı������鴦�ڣ���"@GetItemData(%DialogID,1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�������������������ɽ���Ϊ����"@GetItemData(105099006,1)@"��</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ����������һ������</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ������ȼ��ﵽ28��</t></t><b/>";
				case "105099006":return $Get_Dialog_GeShi[31401] @ "<t>Ӧ��ش����˶��������飬�ǲ��ɶ�õ�����鱦�������������������澫��������Ҳ���Խ��ף�����Ϊ��������Ϸ�����</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�㵱ǰ�ı������鴦�ڣ���"@GetItemData(%DialogID,1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�������������������ɽ���Ϊ����"@GetItemData(105099007,1)@"��</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> �����һλ����</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ������ȼ��ﵽ35��</t></t><b/>";
				case "105099007":return $Get_Dialog_GeShi[31401] @ "<t>Ӧ��ش����˶��������飬�ǲ��ɶ�õ�����鱦�������������������澫��������Ҳ���Խ��ף�����Ϊ��������Ϸ�����</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�㵱ǰ�ı������鴦�ڣ���"@GetItemData(%DialogID,1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�������������������ɽ���Ϊ����"@GetItemData(105099008,1)@"��</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ��ӵ�ж���ͽ��</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ��ӵ�ж�����"@GetItemData(105102015, 1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ������ȼ��ﵽ55��</t></t><b/>";
				case "105099008":return $Get_Dialog_GeShi[31401] @ "<t>Ӧ��ش����˶��������飬�ǲ��ɶ�õ�����鱦�������������������澫��������Ҳ���Խ��ף�����Ϊ��������Ϸ�����</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�㵱ǰ�ı������鴦�ڣ���"@GetItemData(%DialogID,1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�������������������ɽ���Ϊ����"@GetItemData(105099009,1)@"��</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ��ӵ���Լ��Ķ���</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ��ӵ�ж�����"@GetItemData(105102016, 1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ������ȼ��ﵽ65��</t></t><b/>";
				case "105099009":return $Get_Dialog_GeShi[31401] @ "<t>Ӧ��ش����˶��������飬�ǲ��ɶ�õ�����鱦�������������������澫��������Ҳ���Խ��ף�����Ϊ��������Ϸ�����</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�㵱ǰ�ı������鴦�ڣ���"@GetItemData(%DialogID,1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�������������������ɽ���Ϊ����"@GetItemData(105099002,1)@"��</t><b/><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ����ʦ��ͽ�ܴﵽ10��</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ��ӵ�ж�����"@GetItemData(105102017, 1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31206] @ "<t> ������ȼ��ﵽ75��</t></t><b/>";
				case "105099010":return $Get_Dialog_GeShi[31401] @ "<t>Ӧ��ش����˶��������飬�ǲ��ɶ�õ�����鱦�������������������澫��������Ҳ���Խ��ף�����Ϊ��������Ϸ�����</t><b/><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>�㵱ǰ�ı������鴦�ڣ���"@GetItemData(%DialogID,1)@"��</t><b/>" @
																$Get_Dialog_GeShi[31213] @ "<t>���������Ѿ��������ռ��׶�</t><b/>";

				case "105030012":return "���ˡ�" @ GetItemData(%DialogID,1) @ "������Ҫ����1����" @ GetItemData(105100021, 1) @ "���ſɿ�����";//��ɫ����
				case "105030013":return "���ˡ�" @ GetItemData(%DialogID,1) @ "������Ҫ����1����" @ GetItemData(105100022, 1) @ "���ſɿ�����";//��ɫ����
				case "105030014":return "���ˡ�" @ GetItemData(%DialogID,1) @ "������ͨ�����ϻ�֪��Ʒ�ʣ�ͬʱҲ���ô������顣";//����

				case "105028011":return "���ˡ�" @ GetItemData(%DialogID,1) @ "����ʹ�ú����1Сʱ����߲�����������õľ��顣";//���������Һ
				case "105028013":return "���ˡ�" @ GetItemData(%DialogID,1) @ "����ʹ�ú����2Сʱ����߲�����������õľ��顣";//�м������Һ
				case "105028014":return "���ˡ�" @ GetItemData(%DialogID,1) @ "����ʹ�ú����3Сʱ����߲�����������õľ��顣";//�߼������Һ

				case "105100021":return "���ˡ�" @ GetItemData(%DialogID,1) @ "������ͨ����ֻ�á�";//��¶Һ
				case "105100022":return "���ˡ�" @ GetItemData(%DialogID,1) @ "������Ԫ���̳ǳ��ۡ�";//��¶Һ


				default:
					//����������϶Ի�
					if( (GetSubStr(%DialogID,0,4) == 1051)&&(GetSubStr(%DialogID,3,6) >= 100001)&&(GetSubStr(%DialogID,3,6) <= 100999) )
						{
//							echo("DialogID = "@%DialogID);
							%Player.ClearMissionItem();
							%Item_Num_All = $Item_Bao[%DialogID];

							%NumAll = GetWord(%Item_Num_All, 0);		//����ڵ�������
							%Type   = GetWord(%Item_Num_All, 1);		//�������

							//ѡ������ߵĴ������
							if(%Type == 3)
								{
									for(%i = 0;%i < %NumAll;%i++)
									{

										%Item = GetWord($Item_Bao[%DialogID, %i + 1], 0);
										%Num  = GetWord($Item_Bao[%DialogID, %i + 1], 1);

//										echo("Item = "@%i@" = "@%Item@" = "@%Num);
										%Player.AddItemToMission(0, %Item, %Num);
									}
									%Txt1 = "�򿪴˰���������Դ�������Ʒ������ѡ����ĳ����Ʒ��</t><b/><b/>" @ GetOptionalMissionItemText();
//									echo("%Txt1%Txt1 ===="@%Txt1);
								}
								else
									{
//										echo("Type1 = "@%Type);
										if(%Type == 4)
											{
												//�������������ߵĴ������
												%Why = GetWord(%Item_Num_All, 2);		//�������

												if(%Why == 1){%ListNum = %Player.GetFamily();%ListSex = %Player.GetSex();}	//���������ж�
												if(%Why == 2){%ListNum = %Player.GetLevel();}		//�ȼ������ж�

												%Item_Num_Add = $Item_Bao[%DialogID, %ListNum, %ListSex];

//												echo("Item_Num_Add = "@%Item_Num_Add);

												for(%ii = 0;%ii < 9;%ii++)
												{
													%Item = GetWord(%Item_Num_Add, %ii * 2);
													%Num  = GetWord(%Item_Num_Add, %ii * 2 + 1);

//													echo("Item = "@%ii@" = "@%Item@" = "@%Num);

													if( (%Item $= "")&&(%Num $= "") )
														break;
													else
														%Player.AddItemToMission(1, %Item, %Num);
												}
											}
											else
												{
//													echo("Type2 = "@%Type);
													//�ظ����ߵĴ������
													//��������ߵĴ������
													for(%iii = 0;%iii < %NumAll;%iii++)
													{
														%Item = GetWord($Item_Bao[%DialogID, %iii + 1], 0);
														%Num  = GetWord($Item_Bao[%DialogID, %iii + 1], 1);

														%Player.AddItemToMission(1, %Item, %Num);

//														echo("i = "@%i@" "@%Item@" "@%Num);
													}
												}

										if(%Type == 1){%Txt1 = "�򿪴˰������㽫���������Ʒ��</t><b/><b/>" @ GetFixedMissionItemText();}
										if(%Type == 2){%Txt1 = "�򿪴˰������㽫��������Ʒ������������ĳ����Ʒ��</t><b/><b/>" @ GetFixedMissionItemText();}
										if(%Type == 4){%Txt1 = "�򿪴˰������㽫���������Ʒ��</t><b/><b/>" @ GetFixedMissionItemText();}
									}

								return $Get_Dialog_GeShi[31401] @ %Txt1;
						}

			}
		}

	return "Get_Item_Dialog == ���� Npc��"@%Npc@"��Player��"@%Player@"��DialogID��"@%DialogID@"��Id��"@%Param@"��";
}
//��ȡ������ضԻ�
function Get_Skill_Dialog(%Npc, %Player, %DialogID, %Param)
{
	return "Get_Skill_Dialog == ���� Npc��"@%Npc@"��Player��"@%Player@"��DialogID��"@%DialogID@"��Id��"@%Param@"��";
}
//��ȡ״̬�Ի�
function Get_Buff_Dialog(%Npc, %Player, %DialogID, %Param)
{
	return "Get_Buff_Dialog == ���� Npc��"@%Npc@"��Player��"@%Player@"��DialogID��"@%DialogID@"��Id��"@%Param@"��";
}
//��ȡNpc�������ضԻ�
function Get_Npc_Dialog(%Npc, %Player, %DialogID, %Param)
{
//	echo("��ȡNpc�������ضԻ�");
//	echo(%Npc.GetDataID() @ " = " @ %DialogID);

	if(%Npc.GetDataID() $= %DialogID)
		{
			switch$(%Npc.GetDataID())
			{
				case "400001102":return $Get_Dialog_GeShi[31401] @ "<t>��ؽ���������������</t><b/>" @
																													 "<t>Ǭ����ƽ������ʼ�֣�</t><b/>" @
																													 "<t>����Զȥ����ϻ��磬</t><b/>" @
																													 "<t>ãȻ������Ȼ������</t><b/>" @
																													 "<t>Ů洵������������ˣ�</t><b/>" @
																													 "<t>����������������ء�</t><b/>";
				case "410107001":return $Get_Dialog_GeShi[31401] @ "���ˡ�"@GetNpcData(%Npc.GetDataID(),1)@"��ְ�ơ�"@$FamilyName[1,1]@"��</t>";	//"������(ʥ)
				case "410207001":return $Get_Dialog_GeShi[31401] @ "���ˡ�"@GetNpcData(%Npc.GetDataID(),1)@"��ְ�ơ�"@$FamilyName[2,1]@"��</t>";	//"������(��)
				case "410307001":return $Get_Dialog_GeShi[31401] @ "���ˡ�"@GetNpcData(%Npc.GetDataID(),1)@"��ְ�ơ�"@$FamilyName[3,1]@"��</t>";	//"������(��)
				case "410407001":return $Get_Dialog_GeShi[31401] @ "���ˡ�"@GetNpcData(%Npc.GetDataID(),1)@"��ְ�ơ�"@$FamilyName[4,1]@"��</t>";	//"�ɻ���(��)
				case "410507001":return $Get_Dialog_GeShi[31401] @ "���ˡ�"@GetNpcData(%Npc.GetDataID(),1)@"��ְ�ơ�"@$FamilyName[5,1]@"��</t>";	//"���Ľ�(��)
				case "410607001":return $Get_Dialog_GeShi[31401] @ "���ˡ�"@GetNpcData(%Npc.GetDataID(),1)@"��ְ�ơ�"@$FamilyName[6,1]@"��</t>";	//"ɽ����(��)
				case "410707001":return $Get_Dialog_GeShi[31401] @ "���ˡ�"@GetNpcData(%Npc.GetDataID(),1)@"��ְ�ơ�"@$FamilyName[7,1]@"��</t>";	//"���鹬(��)
				case "410807001":return $Get_Dialog_GeShi[31401] @ "���ˡ�"@GetNpcData(%Npc.GetDataID(),1)@"��ְ�ơ�"@$FamilyName[8,1]@"��</t>";	//"��ħ��(ħ)

				case "400001072":return "��ëһ�ڣ�����һ�飬�������٣����ڲ���";	//ҽ����Ѫ
			//������������
				case "401401001":return $Get_Dialog_GeShi[31200] @ "<t>��������һ��ˮ�����������Һ��˲�����</t><b/>" @
																								 "<t>��ôҲ����һ������������������˲��˱�</t><b/>" @
																								 "<t>���ǰѰ�����̫�������ֺ���һ���ĸо�</t><b/>" @
																								 "<t>��������ǰ����ŵ�Բŷ���ˮ�����ķ�������</t><b/>" @
																								 "<t>��Ȼ�����������������Ӳ���������</t><b/>" @
																								 "<t>������ߵ���ķ��Ҷ�����˼尾</t><b/>" @
																								 "<t>��Ȼ��֪����֪����������Ư������</t><b/>" @
																								 "<t>������һ����һ��Ц���������ҩ</t><b/>" @
																								 "<t>�������еĺ����еļ���</t><b/><b/>" @
																								 "<t>��������һ��ˮ�����������Һ��˲�����</t><b/>" @
																								 "<t>��ôҲ����һ������������������˲��˱�</t><b/>" @
																								 "<t>���ǰѰ�����̫�������ֺ���һ���ĸо�</t><b/>" @
																								 "<t>��������ǰ����ŵ�Բŷ���ˮ�����ķ�������</t><b/>" @
																								 "<t>��Ȼ�����������������Ӳ���������</t><b/>" @
																								 "<t>������ߵ���ķ��Ҷ�����˼尾</t><b/>" @
																								 "<t>��Ȼ��֪����֪����������Ư������</t><b/>" @
																								 "<t>������һ����һ��Ц���������ҩ</t><b/>" @
																								 "<t>�������еĺ����еļ���</t><b/>" @
																								 "<t>��Ȼ�����������������Ӳ���������</t><b/>" @
																								 "<t>������ߵ���ķ��Ҷ�����˼尾</t><b/>" @
																								 "<t>��Ȼ��֪����֪����������Ư������</t><b/><b/>" @
																								 "<t>������һ����һ��Ц���������ҩ</t><b/>" @
																								 "<t>�������еĺ�</t><b/>" @
																								 "<t>���еļ���</t><b/></t>";

				case "400001019":return "��ͷ�Ҵ�����ʮ�꣬�������м��ء�������������ζ���ǲ����ʳ�ģ����к����ĳ��������㡢�����ȣ�������������Ϊ���ܵ������˵�����˴�ˮ�׵�����ϡ�����������أ����Ƿ�Ҫѧϰ����֮���أ��������������֮·�°빦����";//����Npc
				case "400001022":return "�ҽ�������֮����ʮ�꣬ʲô�������޶��Թ�����Щ�ɶ��Ǳ��ﰡ���޹ǿ��������������������ʳ�ģ���Ƥ��������װ����ʵ���������ؼ��������Ű������Ƿ�Ҫѧϰ����֮���أ��������������֮·�°빦����";//����Npc
				case "400001021":return "��ҩ֮�������Ϲţ�ֻҪ��ʶ�𣬻�ɼ������֮������Կ���ҩ��ѧ���˲�ҩ��ᷢ�����¶��Ǳ�����������Ҫ��Ҫѧѧ��";//��ҩNpc
				case "400001020":return "��ɱ�С������ֲ֮�����ϹŴ�����ũ֮���Գ��ٲݣ���Ϊ�о��������飬�����д������о������ֲ������������ֲ֮������������Ľᾧ����������ֲ֮����������ǳ�����ض��޷����������ﶼ����ͨ����ֲ�������������Ķ��˰ɣ�";//��ֲNpc
				case "400001018":return "��ľ֮�������Ƽ򵥣���ʵ��Ҫ���յĶ���̫���ˣ�ͬһ��ľ���ᷥľ���˿��������ľ�����ľ�����ῳ���ľ���һ����ľ��Ҫ���ñ������ľ��������ͬʱ��Ҫ������ľ������Ϊ����ʧȥ�������м�ļ�ֵ���ر𰡡�Ҫ��Ҫѧϰ���ⷥľ֮������";//��ľNpc
				case "400001017":return "˵���ɿ�����Ʒ�裬��Ʒζɬ�����ζŨ����Ȥ�޷����ٺ٣������Ҳ����������Ժ����֪���ˣ���ɿ�֮�����г��߻��������򸻼�һ�����ĸ�û�úô��أ�";//�ɿ�Npc

				default:
					if($NpcRandomDialog[%Npc.GetDataID(),1] !$= "")
						{
							%D = 1;
							if($NpcRandomDialog[%Npc.GetDataID(),2] !$= ""){%D = 2;}
							if($NpcRandomDialog[%Npc.GetDataID(),3] !$= ""){%D = 3;}

							return $NpcRandomDialog[%Npc.GetDataID(),GetRandom(1,%D)];
						}
					else
						return $Get_Dialog_GeShi[31401] @ "��ӭ���������ɴ���������</t><b/><b/>";
			}

		}
		else
			return "Get_Npc_Dialog == ��Ų��� NpcID��"@%Npc.GetDataID()@"��Player��"@%Player@"��DialogID��"@%DialogID@"��Id��"@%Param@"��";

	return "Get_Npc_Dialog == ���� Npc��"@%Npc@"��Player��"@%Player@"��DialogID��"@%DialogID@"��Id��"@%Param@"��";
}
//��ȡ������ضԻ�
//function Get_Area_Dialog(%Npc, %Player, %DialogID, %Param)
//{
//	if(%DialogID $= "813030100")
//		return $Get_Dialog_GeShi[31201] @ "���¾��׵Ŀռ䶸Ȼ����̫�׽��ǵ�������ɱ���֤꣬������</t><b/><b/>" @
//					 $Get_Dialog_GeShi[31206] @ "��������֮�꣬����֮�ǣ���̫�׽���֤���Լ���ʵ����</t>";
//
//	return "Get_Area_Dialog == ���� Npc��"@%Npc@"��Player��"@%Player@"��DialogID��"@%DialogID@"��Id��"@%Param@"��";
//}

//��ȡ���е���ضԻ�
function Get_Fly_Dialog(%Npc, %Player, %DialogID, %Param)
{
	%FlyID = GetSubStr(%DialogID,1,8);

	switch$(%FlyID)
	{
		case "10010101":	return $Icon[11] @ "<t>���½�����д�������</t>";
		case "10010102":	return $Icon[11] @ "<t>����ͷ���д��͵�����</t>";
		case "10010103":	return $Icon[11] @ "<t>�����Ƿ��д�������</t>";
		case "10010104":	return $Icon[11] @ "<t>�ξ��η��д��͵�����</t>";
		case "11010101":	return $Icon[11] @ "<t>�����Ƿ��д��͵������</t>";
		case "11010102":	return $Icon[11] @ "<t>����ͷ���д�������</t>";
		case "11010103":	return $Icon[11] @ "<t>�ξ��η��д�������</t>";
		case "11270001":	return $Icon[11] @ "<t>��������д��͵������</t>";
		case "10010005":	return $Icon[11] @ "<t>����Ƿ��д��͵����ľ�</t>";
		case "10010006":	return $Icon[11] @ "<t>���ľ����д�������</t>";
		case "10020001":	return $Icon[11] @ "<t>����Ƿ��д�������</t>";
		case "10020002":	return $Icon[11] @ "<t>���ľ����д��͵������</t>";

	}

	return "Get_Fly_Dialog == ���� Npc��"@%Npc@"��Player��"@%Player@"��DialogID��"@%DialogID@"��Id��"@%Param@"��";
}
//��ȡ�������õ�����ͨ������
function Get_Mis_Dialog(%Npc, %Player, %Mid, %Tid)
{
	%PngA = isFile("gameres/gui/images/GUIWindow25_1_005.png");//��������
	%PngB = isFile("gameres/gui/images/GUIWindow25_1_006.png");//����Ŀ��
	%PngC = isFile("gameres/gui/images/GUIWindow25_1_007.png");//������
	%PngD = isFile("gameres/gui/images/GUIWindow25_1_008.png");//�������
	%PngE = isFile("gameres/gui/images/GUIWindow25_1_009.png");//�����Ѷ�

	if(%PngA == 1){%Png_A = "<i s='gameres/gui/images/GUIWindow25_1_005.png' w='16' h='16'/>";}else{%Png_A = "��������</t>";}
	if(%PngB == 1){%Png_B = "<i s='gameres/gui/images/GUIWindow25_1_006.png' w='16' h='16'/>";}else{%Png_B = "����Ŀ��</t>";}
	if(%PngC == 1){%Png_C = "<i s='gameres/gui/images/GUIWindow25_1_007.png' w='16' h='16'/>";}else{%Png_C = "������</t>";}
	if(%PngD == 1){%Png_D = "<i s='gameres/gui/images/GUIWindow25_1_008.png' w='16' h='16'/>";}else{%Png_D = "�������</t>";}
	if(%PngE == 1){%Png_E = "<i s='gameres/gui/images/GUIWindow25_1_009.png' w='16' h='16'/>";}else{%Png_E = "�����Ѷ� </t>";}

	//����ͨ������
	if(%Tid == 1)
		{
			%T10001 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10001);");	//"����Ŀ��";
			%Txt = %Png_B @ "<b/>" @ %T10001;
		}
	if(%Tid == 2)
		{
			%TxtID = %Player.GetMissionFlag(%Mid, 4000);

			if(%TxtID < 1)
				%TxtID = 10002;

			%T10002 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, %TxtID);");	//"��������";
			%Txt = %Png_A @ "<b/>" @ %T10002;
		}
	if(%Tid == 3)
		{
			%Txt = %Png_E @ GetMission_NanDu(%Mid);	//"�����Ѷ�";
		}
	if(%Tid == 4)
		{
			%Player.ClearMissionItem();

			if(%Mid == 20001)
				{
					%T999 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 999);");	//"������";

					return %T999;
				}

			%MissionRewardData = "MissionRewardData_" @ %Mid;

			if(%MissionRewardData.JiFen     != 0){%JiFen = $Get_Dialog_GeShi[31212] @ "���֣� </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %MissionRewardData.JiFen @ " </t><t>]</t><b/>";}else{%JiFen = "";}
			if(%MissionRewardData.Exp       != 0){%Exp   = $Get_Dialog_GeShi[31212] @ "���飺 </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %MissionRewardData.Exp   @ " </t><t>]</t><b/>";}else{%Exp   = "";}
			if(%MissionRewardData.Gold      != 0){%Gold  = $Get_Dialog_GeShi[31212] @ "Ԫ���� </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %MissionRewardData.Gold  @ " </t><t>]</t><b/>";}else{%Gold  = "";}

			//��Ԫ����
			if(%MissionRewardData.Money     != 0)
				{
//					echo("��Ԫ���� = "@ %MissionRewardData.Money);

					%a = %MissionRewardData.Money;
					%b = 9 - strlen(%a);
					%c = "1";
					for(%i = 0;%i < %b;%i++)
						%c = %c @ "0";

					%Money = %c @ %a;

//					%Money = 100000000 + %MissionRewardData.Money;
//					%Money = 123456789 + %MissionRewardData.Money;
//					echo("Money = "@ %Money);
//					echo("Money a = "@ GetSubStr(%Money,1,4));
//					echo("Money b = "@ GetSubStr(%Money,5,2));
//					echo("Money c = "@ GetSubStr(%Money,7,2));
//
//					%Money_A = $Get_Dialog_GeShi[31201] @ GetSubStr(%Money,1,4) @ "</t>";	//���
//					%Money_B = $Get_Dialog_GeShi[31201] @ GetSubStr(%Money,5,2) @ "</t>";	//����
//					%Money_C = $Get_Dialog_GeShi[31201] @ GetSubStr(%Money,7,2) @ "</t>";	//ͭ��

//					echo("Money_A = "@ %Money_A);
//					echo("Money_B = "@ %Money_B);
//					echo("Money_C = "@ %Money_C);

//					%Money = 100000 + %MissionRewardData.Money;
//					echo("Money = "@ %Money);

					%Money_A = GetSubStr(%Money,1,5) + 0;	//���
					%Money_B = GetSubStr(%Money,6,2) + 0;	//����
					%Money_C = GetSubStr(%Money,8,2) + 0;	//ͭ��

//					echo("Money_A = "@ %Money_A);
//					echo("Money_B = "@ %Money_B);
//					echo("Money_C = "@ %Money_C);

					%Png1 = isFile("gameres/gui/images/GUIWindow31_1_024.png"); //���
					%Png2 = isFile("gameres/gui/images/GUIWindow31_1_025.png"); //����
					%Png3 = isFile("gameres/gui/images/GUIWindow31_1_026.png"); //ͭ��

					%Txt = $Get_Dialog_GeShi[31201];

					if(%Money_A > 0)
						{
							if(%Png1 == 1)
								%Show_A = %Txt @ %Money_A @ "</t><i s='gameres/gui/images/GUIWindow31_1_024.png' w='18' h='18'/><t> </t>";
							else
								%Show_A = %Txt @ %Money_A @ "�� </t>";
						}
						else
							%Show_A = "";

					if(%Money_B > 0)
						{
							if(%Png2 == 1)
								%Show_B = %Txt @ %Money_B @ "</t><i s='gameres/gui/images/GUIWindow31_1_025.png' w='18' h='18'/><t> </t>";
							else
								%Show_B = %Txt @ %Money_B @ "�� </t>";
						}
						else
							%Show_B = "";

					if(%Money_C > 0)
						{
							if(%Png2 == 1)
								%Show_C = %Txt @ %Money_C @ "</t><i s='gameres/gui/images/GUIWindow31_1_026.png' w='18' h='18'/><t> </t>";
							else
								%Show_C = %Txt @ %Money_C @ "ͭ </t>";
						}
						else
							%Show_C = "";

					%Money = $Get_Dialog_GeShi[31212] @ "��Ԫ�� </t><t>[ </t>" @ %Show_A @ %Show_B @ %Show_C @ "<t>]</t><b/>";

//					echo("%Money = "@%Money);
				}
				else
					%Money = "";

			//��Ԫ����
			if(%MissionRewardData.Yuan     != 0)
				{
//					echo("��Ԫ������ = "@ %MissionRewardData.Yuan);

					%a = %MissionRewardData.Yuan;
					%b = 9 - strlen(%a);
					%c = "1";
					for(%i = 0;%i < %b;%i++)
						%c = %c @ "0";

					%Yuan = %c @ %a;

//					echo("%Yuan = "@%Yuan);
					%Yuan_A = GetSubStr(%Yuan,1,5) + 0;	//���
					%Yuan_B = GetSubStr(%Yuan,6,2) + 0;	//����
					%Yuan_C = GetSubStr(%Yuan,8,2) + 0;	//ͭ��

//					echo("Yuan_A = "@ %Yuan_A);
//					echo("Yuan_B = "@ %Yuan_B);
//					echo("Yuan_C = "@ %Yuan_C);

					%Png1 = isFile("gameres/gui/images/GUIWindow31_1_020.png"); //���
					%Png2 = isFile("gameres/gui/images/GUIWindow31_1_021.png"); //����
					%Png3 = isFile("gameres/gui/images/GUIWindow31_1_022.png"); //ͭ��

//					echo("Png1 = "@ %Png1);
//					echo("Png2 = "@ %Png2);
//					echo("Png3 = "@ %Png3);

					%Txt = $Get_Dialog_GeShi[31201];

					if(%Yuan_A > 0)
						{
							if(%Png1 == 1)
								%Show_A = %Txt @ %Yuan_A @ "</t><i s='gameres/gui/images/GUIWindow31_1_020.png' w='18' h='18'/><t> </t>";
							else
								%Show_A = %Txt @ %Yuan_A @ "�� </t>";
						}
						else
							%Show_A = "";

					if(%Yuan_B > 0)
						{
							if(%Png2 == 1)
								%Show_B = %Txt @ %Yuan_B @ "</t><i s='gameres/gui/images/GUIWindow31_1_021.png' w='18' h='18'/><t> </t>";
							else
								%Show_B = %Txt @ %Yuan_B @ "�� </t>";
						}
						else
							%Show_B = "";

					if(%Yuan_C > 0)
						{
							if(%Png2 == 1)
								%Show_C = %Txt @ %Yuan_C @ "</t><i s='gameres/gui/images/GUIWindow31_1_022.png' w='18' h='18'/><t> </t>";
							else
								%Show_C = %Txt @ %Yuan_C @ "ͭ </t>";
						}
						else
							%Show_C = "";

//					echo("Show_A = "@ %Show_A);
//					echo("Show_B = "@ %Show_B);
//					echo("Show_C = "@ %Show_C);

					%Yuan = $Get_Dialog_GeShi[31212] @ "��Ԫ�� </t><t>[ </t>" @ %Show_A @ %Show_B @ %Show_C @ "<t>]</t><b/>";

//					echo("%Yuan = "@%Yuan);
				}
				else
					%Yuan = "";

			//��������
			if(%MissionRewardData.ShengWang !$= "0")
				{
					%ShengWang = $Get_Dialog_GeShi[31212] @ "������ </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %MissionRewardData.ShengWang @ "</t><t> ]</t><b/>";
				}
				else
					%ShengWang = "";

			//ѡ���Ե���
			if(%MissionRewardData.ItemChose !$= "0")
				{
					%Set = 0;
					%I_Num = 9;

					//�ж��Ա�����
					if( (%Mid == 10103)||(%Mid == 10106)||(%Mid == 10163) )
						{
							%I_Num = 3;

							if(%Player.GetSex() == 2)
								%Set = 3;
						}
					//�ж���������
					if(%Mid == 10127)
						{
							%I_Num = 2;

							if(%Player.GetFamily() == 1){%Set = 0;}
							if(%Player.GetFamily() == 2){%Set = 2;}
							if(%Player.GetFamily() == 3){%Set = 4;}
							if(%Player.GetFamily() == 4){%Set = 6;}
							if(%Player.GetFamily() == 5){%Set = 8;}
							if(%Player.GetFamily() == 6){%Set = 10;}
							if(%Player.GetFamily() == 7){%Set = 12;}
							if(%Player.GetFamily() == 8){%Set = 14;}

						}

					for(%i = 0; %i < %I_Num; %i++)
					{
						%Item = GetWord(%MissionRewardData.ItemChose, (%i + %Set) * 2);
						%Num  = GetWord(%MissionRewardData.ItemChose, (%i + %Set) * 2 + 1);

						echo("Mid=[ "@%Mid@" ]=Set=[ "@%Set@" ]=i=[ "@%i@" ]=Item=[ "@%Item@" ]=Num=[ "@%Num@" ]");

						if( (%Item $= "")&&(%Num $= "") )
							break;
						else
							%Player.AddItemToMission(0, %Item, %Num);
					}

					%ItemChose = $Get_Dialog_GeShi[31212] @ "��ѡ��������</t><b/>" @ GetOptionalMissionItemText();
				}
				else
					%ItemChose = "";

			//�ظ�����
			if(%MissionRewardData.ItemSet !$= "0")
				{
					for(%i = 0; %i < 9; %i++)
					{
						%Item = GetWord(%MissionRewardData.ItemSet, %i * 2);
						%Num  = GetWord(%MissionRewardData.ItemSet, %i * 2 + 1);

						if( (%Item $= "")&&(%Num $= "") )
							break;
						else
							%Player.AddItemToMission(1, %Item, %Num);
					}

					%ItemSet = $Get_Dialog_GeShi[31212] @ "�̶���������</t><b/>" @ GetFixedMissionItemText();
				}
				else
					%ItemSet = "";

			//����
			if(%MissionData.Skill !$= "0")
				{
				}
				else
					%Skill = "";

			//״̬
			if(%MissionData.Buff !$= "0")
				{
				}
				else
					%Buff = "";

			%Reward = %JiFen @ %Exp @ %Money @ %Yuan @ %Gold @ %ShengWang @ %ItemChose @ %ItemSet @ %Skill @ %Buff;

			if(%Reward !$= "")
				%Txt = %Png_C @ "<b/>" @ %Reward;
			else
				%Txt = "";
		}
	if(%Tid == 5)
		{
			%T10005 = eval("Mission" @ %Mid @ "Dialog" @ "(%Npc, %Player, %Mid, 10005);");	//"������ɶԻ�";
			%Txt = %Png_D @ "<b/>" @ %T10005;
		}

	if(%Txt !$= "")
		return $Get_Dialog_GeShi[31201] @ %Txt @ "<b/>";

	return "Get_Mis_Dialog == ���� Tid��"@%Tid@"��";
}

