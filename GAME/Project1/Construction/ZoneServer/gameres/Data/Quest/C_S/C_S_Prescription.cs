//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//�����䷽������Ʒʱ�Ĵ����ű�
//��������ж�����Ƿ�ӵ���㹻�Ĳ��ϣ�Ȼ�󴥷���������������������Ŵ������ű�
//==================================================================================
//����ǰ���ж�
//�䷽ǰ�ýű��ж���������дΪ��1��ʱ����
function ProduceCheck(%Player, %SerialId)
{
	if(%SerialId <= 0)
	   return 9999;

	 return 0;
}

function ProduceAll(%Player, %PresId)
{
	//Player	��Ҷ���
	//PresId	�䷽���

	//GetPrescriptionData(%PresId,%Num);	��ȡ�䷽����
	//%Num������

	//echo("�����䷽����");
	//�ж��䷽���λ��������Ϊ9λ
	if(strlen(%PresId) == 9)
		{

			//�ж��䷽��ŵ�2λ�����չ���ִ����Ӧ�ű�
			//��2λΪ1
			if(GetSubStr(%PresId,1,1) $= "1")
			{
				//echo("����ͨ�úϳɹ�ʽ");
				Produce_Make_Normal(%Player, %PresId);		//ͨ�úϳɹ�ʽ
			}
			//��2λΪ2
			if(GetSubStr(%PresId,1,1) $= "2")
			{
				//echo("��������ϳɹ�ʽ");
				Produce_Make_Special(%Player, %PresId);		//����ϳɹ�ʽ
			}
			//��2λΪ3
			if(GetSubStr(%PresId,1,1) $= "3")
			{
				//echo("��������䷽�ϳɹ�ʽ");
				eval("Produce_"@%PresId@"(%Player, %PresId);");	//ִ����Ӧ�䷽��ŵĶ�������
			}
		}
}

//ͨ�����칫ʽ
function Produce_Make_Normal(%Player, %PresId)
{
	//��������
	//����
	%Materials[1] = GetPrescriptionData(%PresId, 10);
	%Materials[2] = GetPrescriptionData(%PresId, 12);
	%Materials[3] = GetPrescriptionData(%PresId, 14);
	%Materials[4] = GetPrescriptionData(%PresId, 16);
	%Materials[5] = GetPrescriptionData(%PresId, 18);
	%Materials[6] = GetPrescriptionData(%PresId, 20);
	//����
	%Number[1] = GetPrescriptionData(%PresId, 11);
	%Number[2] = GetPrescriptionData(%PresId, 13);
	%Number[3] = GetPrescriptionData(%PresId, 15);
	%Number[4] = GetPrescriptionData(%PresId, 17);
	%Number[5] = GetPrescriptionData(%PresId, 19);
	%Number[6] = GetPrescriptionData(%PresId, 21);
	//����
	%Vigor = GetPrescriptionData(%PresId, 4);
	//��Ʊ
	%Money = GetPrescriptionData(%PresId, 7);

	//���ڵĵ���
	%Item = GetPrescriptionData(%PresId, 22);
	//���ڵ�����
	%Item_Num = GetPrescriptionData(%PresId, 24);

	//�ж�����Ƿ�����Щ�����Ĳ���
	for(%i = 1; %i < 7; %i++)
	{
		if(%Materials[%i] > 0 && %Number[%i] > 0)
		{
			if(%Player.GetItemCount(%Materials[%i]) < %Number[%i])
				{
					//���������������ϳ�ʧ��
					return 4103;
				}
		}
	}
	//�ж�����Ƿ����㹻�Ļ���
	if(%Player.GetVigor() < %Vigor)
		return 4103;	//���������������ϳ�ʧ��

	//�ж�����Ƿ����㹻�ĳ�Ʊ
	if(%Player.GetMoney(1) < %Money)
		return 4103;	//���������������ϳ�ʧ��

	//ֱ�Ӹ�����
	if(AddItemToInventory(%Player.GetPlayerID(), %Item, %Item_Num))
		{
			for(%i = 1;%i < 7;%i++)
			{
				if(%Materials[%i] !$= "0" && %Number[%i] !$= "0")
					DelItemFromInventory(%Player.GetPlayerID(), %Materials[%i], %Number[%i]);
			}
			//��������������
			addLivingSkillGuerdon(%Player,GetPrescriptionData(%PresId, 6),1);
			//�۳�����ֵ
			%Player.AddVigor(- %Vigor);
			//��Ǯ
			%Player.ReduceMoney(%Money, 1);

			return 0;	//�����ɹ�
		}
		else
			{
				//�����޷����ڣ�����ʧ��
				//�������������Ψһ���ԣ�����������Ҵ����޷���õ��ߵ�״̬��
				return 4103;
			}
}

//����ϳɹ�ʽ
function Produce_Make_Special()
{

}

function Produce_PropsOperation(%Player, %PresId, %ResultItem, %Count)
{
	if(strlen(%ResultItem) != 9) {return 4103;}

	if(%ResultItem == 0) {return 4103;}
	//��ȡ��Ҫ�۳��Ĳ���
	//����
	%Materials[1] = GetPrescriptionData(%PresId, 10);
	%Materials[2] = GetPrescriptionData(%PresId, 12);
	%Materials[3] = GetPrescriptionData(%PresId, 14);
	%Materials[4] = GetPrescriptionData(%PresId, 16);
	%Materials[5] = GetPrescriptionData(%PresId, 18);
	%Materials[6] = GetPrescriptionData(%PresId, 20);
	//����
	%Number[1] = GetPrescriptionData(%PresId, 11);
	%Number[2] = GetPrescriptionData(%PresId, 13);
	%Number[3] = GetPrescriptionData(%PresId, 15);
	%Number[4] = GetPrescriptionData(%PresId, 17);
	%Number[5] = GetPrescriptionData(%PresId, 19);
	%Number[6] = GetPrescriptionData(%PresId, 21);
	//����
	%Vigor = GetPrescriptionData(%PresId, 4);
	//��Ʊ
	%Money = GetPrescriptionData(%PresId, 7);

	//�ж�����Ƿ�����Щ�����Ĳ���
	for(%i = 1; %i < 7; %i++)
	{
		if(%Materials[%i] > 0 && %Number[%i] > 0)
		{
			if(%Player.GetItemCount(%Materials[%i]) < %Number[%i])
				{
					//���������������ϳ�ʧ��
					return 4103;
				}
		}
	}
	//�ж�����Ƿ����㹻�Ļ���
	if(%Player.GetVigor() < %Vigor)
		return 4103;	//���������������ϳ�ʧ��

	//�ж�����Ƿ����㹻�ĳ�Ʊ
	if(%Player.GetMoney(1) < %Money)
		return 4103;	//���������������ϳ�ʧ��

	//ֱ�Ӹ�����
	if(AddItemToInventory(%Player.GetPlayerID(), %ResultItem, %Count))
		{
			for(%i = 1;%i < 7;%i++)
			{
				if(%Materials[%i] !$= "0" && %Number[%i] !$= "0")
					DelItemFromInventory(%Player.GetPlayerID(), %Materials[%i], %Number[%i]);
			}
			//��������������
			addLivingSkillGuerdon(%Player,GetPrescriptionData(%PresId, 6),0);
			//�۳�����ֵ
			%Player.AddVigor(- %Vigor);
			//��Ǯ
			%Player.ReduceMoney(%Money, 1);
		}
	else
		{
				//�����޷����ڣ��ϳ�ʧ��
				//�������������Ψһ���ԣ�����������Ҵ����޷���õ��ߵ�״̬��
			return 4103;
		}

		return 0;
}

function Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList)
{
	%PurpleItemListCount 	=getWordCount(%ItemList_Purple);		//��ɫ��������
	%BlueItemListCount 		= getWordCount(%ItemList_Blue);			//��ɫ��������
	%GreenItemListCount 	= getWordCount(%ItemList_Green);		//��ɫ��������
	%WhiteItemListCount 	= getWordCount(%ItemList_White);		//��ɫ��������
    %GrayItemListCount      = getWordCount(%ItemList_Gray);         //��ɫ��������

//	%PurpleOddsListCount    = getWordCount(%OddsList_Purple);	//��ɫ��������
//	%BlueOddsListCount      = getWordCount(%OddsList_Blue);		//��ɫ��������
//	%GreenOddsListCount 	= getWordCount(%OddsList_Green);		//��ɫ��������
//	%WhiteOddsListCount 	= getWordCount(%OddsList_White);		//��ɫ��������
//	%GrayOddsListCount      = getWordCount(%OddsList_Gray);		//��ɫ��������

    %GrayOdds               = getWord(%Odds_ColorList , 0);//��ɫ����
	%WhiteOdds 	            = getWord(%Odds_ColorList , 1);//��ɫ����
	%GreenOdds 	            = getWord(%Odds_ColorList , 2);//��ɫ����
	%BlueOdds 	            = getWord(%Odds_ColorList , 3);//��ɫ����
	%PurpleOdds             = getWord(%Odds_ColorList , 4);//��ɫ����

	%tmpOdds 		        = %GrayOdds + %WhiteOdds + %GreenOdds + %BlueOdds + %PurpleOdds;
  //echo("�����ܺ�:" @ %tmpOdds);

	%Odds 			        = getWord(%OddsModeList , 0);//�ܼ���
//echo("�ܼ���:" @ %Odds);
//	%OddsMode 	= getWord(%OddsModeList , 1);//���ʼ��㷽ʽ
//	%CalcMode 		= getWord(%OddsModeList , 2);//���㷽ʽ
	%OddsColorMode          = getWord(%OddsModeList,1);//û���ʱس��ĵ�����ɫ

	//���߼������ó��ִ���.
	if(%Odds < %tmpOdds)
	{
		////echo("�������ó���.");
		return 4100;
	}

//	if(%CalcMode == 0)//�ɵ��߼����ܺͼ���
//	{
//		if(%OddsMode == 0)//���ٳ�������
//		{
      //echo("���ٳ�������");
			//�Ƿ���Ҫ�趨����˳��?
			%rndResult = GetRandom(1,%Odds);
			////echo("�������:" @ %rndResult);

			//û�е��߲����㼸��
			if(%PurpleItemListCount > 0)
			{
				if(%rndResult >= 1 && %rndResult <= %PurpleOdds )
				{
					////echo("��ɫ�������.");
					return ProduceGetRndItem(%ItemList_Purple , %OddsList_Purple);
				}
			}

			if(%BlueItemListCount > 0)
			{
				if(%rndResult > %PurpleOdds && %rndResult <= (%PurpleOdds + %BlueOdds))
				{
					////echo("��ɫ�������.");
					return ProduceGetRndItem(%ItemList_Blue , %OddsList_Blue);
				}
			}

			if(%GreenItemListCount > 0)
			{
				if(%rndResult > (%PurpleOdds + %BlueOdds) && %rndResult <= (%PurpleOdds + %BlueOdds + %GreenOdds))
				{
					////echo("��ɫ�������.");
					return ProduceGetRndItem(%ItemList_Green , %OddsList_Green);
				}
			}

            //echo("��ɫ�������.");
            if(%WhiteItemListCount > 0)
            {
                if(%rndResult > (%PurpleOdds + %BlueOdds + %GreenOdds) && %rndResult <= (%PurpleOdds + %BlueOdds + %GreenOdds + %WhiteOdds))
                {
      		        return ProduceGetRndItem(%ItemList_White , %OddsList_White);
                }
		    }

            //��ɫ�������
            if(%GrayItemListCount > 0)
            {
                if(%rndResult > (%PurpleOdds + %BlueOdds + %GreenOdds + %WhiteOdds) <= (%PurpleOdds + %BlueOdds + %GreenOdds + %WhiteOdds + %GrayOdds))
                {
                    return ProduceGetRndItem(%ItemList_Gray , %OddsList_Gray);
                }
            }

			//û���ʵ����

            if(%OddsColorMode == 1)//��
            {
                if(%GrayItemListCount > 0)
                {
                    return ProduceGetRndItem(%ItemList_Gray , %OddsList_Gray);
                }
            }

			if(%OddsColorMode == 2)//��
			{
				if(%WhiteItemListCount > 0)
    		{
						return ProduceGetRndItem(%ItemList_White , %OddsList_White);
				}
			}



			if(%OddsColorMode == 3)//��
			{
				if(%GreenItemListCount > 0)
				{
						return ProduceGetRndItem(%ItemList_Green , %OddsList_Green);
				}
			}


			if(%OddsColorMode == 4)//��
			{
				if(%BlueItemListCount > 0)
				{
					return ProduceGetRndItem(%ItemList_Blue , %OddsList_Blue);
				}
			}


			if(%OddsColorMode == 5)//��
			{
				if(%PurpleItemListCount > 0)
				{
					return ProduceGetRndItem(%ItemList_Purple , %OddsList_Purple);
				}
			}
		//}

//		if(%OddsMode == 1)//���յ��߼��ʼ���
//		{
//			////echo("���յ��߼��ʼ���");
//		}
//	}
//
//	if(%CalcMode == 1)//�����弸�ʼ���
//	{
//	}

	return 0;//û�м��ʻ���κε���
}

function ProduceGetRndItem(%ItemList , %OddsList)
{
	//�������б��������ƥ�䣬ƽ������
	%tmpItemCount 	= getWordCount(%ItemList);
  //echo("��������:" @ %tmpItemCount);
	%tmpOddsList 		= getWordCount(%OddsList);
	//echo("��������:" @ %tmpOddsList);
	if(%tmpOddsList != %tmpItemCount)
	{
        //echo("�������ȣ�ƽ������");
		if(%tmpItemCount == 0)
		{
			%tmpItemCount = %tmpItemCount;
		}
		%rndResult = GetRandom(1 , %tmpItemCount);
        //echo("����:" @ %rndResult);
        //echo("ItemList:" @ %ItemList);
    %testResult = getWord(%ItemList , %rndResult - 1);
    //echo(%testResult);
		return getWord(%ItemList , %rndResult - 1);
	}
	else
	{
		//��ȡ�ܼ���
    //echo("��ȡ�����ܺ�.");
    %tmpOdds = 0;
		for(%i = 0 ; %i < %tmpItemCount ; %i++)
		{
			%tmpOdds = getWord(%OddsList , %i) + %tmpOdds;
		}
		//echo("�����ܺ�Ϊ:" @ %tmpOdds);
		//ȡ���ֵ
		%rndResult = GetRandom(1 , %tmpOdds);

    ////echo("����:" @ %rndResult);
		%tmpRnd = 0;

		//���һ�ȡ���ֵ�ĵ���
		for(%i = 0 ; %i < %tmpItemCount ; %i++)
		{
			%currentRnd = %tmpRnd + getWord(%OddsList , %i);
      //echo("��ǰ����:" @ %currentRnd);

			if(%rndResult > %tmpRnd && %rndResult <= %currentRnd)
			{
				return getWord(%ItemList , %i);
			}

			%tmpRnd = %currentRnd;
		}
	}

  //echo("û�ҵ��κ���Ч��Ϣ,���ؿ�ֵ.");

	//���� ����������.
	return 4103;
}

//ִ����Ӧ�䷽��ŵĶ�������
//function Produce_630000001(%Player, %PresId)
//{
//	//Ӱ����߳��ּ��ʵ�����������ʱûд��
//	//�����б��ո�ָ�
//	//��ɫ
//	%ItemList_Purple = "101013006 101013007 101013008";
//	//��ɫ
//	%ItemList_Blue = "101012004 101012005 101012006";
//	//��ɫ
//	%ItemList_Green = "101011004 101011005 101011006";
//	//��ɫ
//	%ItemList_White = "101010004 101010005 101010006";
//
//	//�������ɫ�����б���������б�����������б�������ƥ�䣬��Ϊƽ�ּ��ʡ����㷽ʽ��A/(A+B+C+����)
//	//��ɫ
//	%OddsList_Purple = "1 2 3";
//	//��ɫ
//	%OddsList_Blue = "1 2 3";
//	//��ɫ
//	%OddsList_Green = "1 2 3";
//	//��ɫ
//	%OddsList_White = "1 2 3";
//
//	//������ɫ���ʣ��ո�ָ˳��Ϊ�ס��̡�������
//	//������ɫ�����ܺͲ��ó����ܼ���
//	%Odds_ColorList = "0 100 10 1";
//
//	//���㷽���趨
//	//�ܼ��� |û���ʱس�����ɫ
//	//�ܼ��ʣ�
//	//û���ʱس�����ɫ����1��2��3��4����Ӧ���ס��̡������ϣ��������κ����ֶ��������Ǳس�����Ϊ��
//	%OddsModeList = "1000 1";
//
//	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%Odds_ColorList,%OddsModeList);
//
//	return Produce_Result(%Player, %PresId, %ResultItem, 1);
//}

//********************��ʼ:630000001�䷽�ű�********************//
function Produce_630000001(%Player, %PresId)
{
	%ItemList_Gray = "";
	%ItemList_White = "";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "101013006 101013007 101013008";

	%OddsList_Gray = "";
	%OddsList_White = "1 2 3";
	%OddsList_Green = "1 2 3";
	%OddsList_Blue = "1 2 3";
	%OddsList_Purple = "1 2 3";

	%Odds_ColorList = "0 0 100 10 1";

	%OddsModeList = "1000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630000001�䷽�ű�********************//

//********************��ʼ:630202001�䷽�ű�********************//
function Produce_630202001(%Player, %PresId)
{
	%ItemList_Gray = "101020022";
	%ItemList_White = "101020022";
	%ItemList_Green = "101021222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630202001�䷽�ű�********************//

//********************��ʼ:630203001�䷽�ű�********************//
function Produce_630203001(%Player, %PresId)
{
	%ItemList_Gray = "101020023";
	%ItemList_White = "101020023";
	%ItemList_Green = "101021223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630203001�䷽�ű�********************//

//********************��ʼ:630204001�䷽�ű�********************//
function Produce_630204001(%Player, %PresId)
{
	%ItemList_Gray = "101020024";
	%ItemList_White = "101020024";
	%ItemList_Green = "101021224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630204001�䷽�ű�********************//

//********************��ʼ:630205001�䷽�ű�********************//
function Produce_630205001(%Player, %PresId)
{
	%ItemList_Gray = "101020025";
	%ItemList_White = "101020025";
	%ItemList_Green = "101021225";
	%ItemList_Blue = "101022225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630205001�䷽�ű�********************//

//********************��ʼ:630206001�䷽�ű�********************//
function Produce_630206001(%Player, %PresId)
{
	%ItemList_Gray = "101020026";
	%ItemList_White = "101020026";
	%ItemList_Green = "101021226";
	%ItemList_Blue = "101022226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630206001�䷽�ű�********************//

//********************��ʼ:630302001�䷽�ű�********************//
function Produce_630302001(%Player, %PresId)
{
	%ItemList_Gray = "101030022";
	%ItemList_White = "101030022";
	%ItemList_Green = "101031222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630302001�䷽�ű�********************//

//********************��ʼ:630303001�䷽�ű�********************//
function Produce_630303001(%Player, %PresId)
{
	%ItemList_Gray = "101030023";
	%ItemList_White = "101030023";
	%ItemList_Green = "101031223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630303001�䷽�ű�********************//

//********************��ʼ:630304001�䷽�ű�********************//
function Produce_630304001(%Player, %PresId)
{
	%ItemList_Gray = "101030024";
	%ItemList_White = "101030024";
	%ItemList_Green = "101031224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630304001�䷽�ű�********************//

//********************��ʼ:630305001�䷽�ű�********************//
function Produce_630305001(%Player, %PresId)
{
	%ItemList_Gray = "101030025";
	%ItemList_White = "101030025";
	%ItemList_Green = "101031225";
	%ItemList_Blue = "101032225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630305001�䷽�ű�********************//

//********************��ʼ:630306001�䷽�ű�********************//
function Produce_630306001(%Player, %PresId)
{
	%ItemList_Gray = "101030026";
	%ItemList_White = "101030026";
	%ItemList_Green = "101031226";
	%ItemList_Blue = "101032226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630306001�䷽�ű�********************//

//********************��ʼ:630102001�䷽�ű�********************//
function Produce_630102001(%Player, %PresId)
{
	%ItemList_Gray = "101010022";
	%ItemList_White = "101010022";
	%ItemList_Green = "101011222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630102001�䷽�ű�********************//

//********************��ʼ:630103001�䷽�ű�********************//
function Produce_630103001(%Player, %PresId)
{
	%ItemList_Gray = "101010023";
	%ItemList_White = "101010023";
	%ItemList_Green = "101011223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630103001�䷽�ű�********************//

//********************��ʼ:630104001�䷽�ű�********************//
function Produce_630104001(%Player, %PresId)
{
	%ItemList_Gray = "101010024";
	%ItemList_White = "101010024";
	%ItemList_Green = "101011224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630104001�䷽�ű�********************//

//********************��ʼ:630105001�䷽�ű�********************//
function Produce_630105001(%Player, %PresId)
{
	%ItemList_Gray = "101010025";
	%ItemList_White = "101010025";
	%ItemList_Green = "101011225";
	%ItemList_Blue = "101012225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630105001�䷽�ű�********************//

//********************��ʼ:630106001�䷽�ű�********************//
function Produce_630106001(%Player, %PresId)
{
	%ItemList_Gray = "101010026";
	%ItemList_White = "101010026";
	%ItemList_Green = "101011226";
	%ItemList_Blue = "101012226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630106001�䷽�ű�********************//

//********************��ʼ:630602001�䷽�ű�********************//
function Produce_630602001(%Player, %PresId)
{
	%ItemList_Gray = "101060022";
	%ItemList_White = "101060022";
	%ItemList_Green = "101061222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630602001�䷽�ű�********************//

//********************��ʼ:630603001�䷽�ű�********************//
function Produce_630603001(%Player, %PresId)
{
	%ItemList_Gray = "101060023";
	%ItemList_White = "101060023";
	%ItemList_Green = "101061223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630603001�䷽�ű�********************//

//********************��ʼ:630604001�䷽�ű�********************//
function Produce_630604001(%Player, %PresId)
{
	%ItemList_Gray = "101060024";
	%ItemList_White = "101060024";
	%ItemList_Green = "101061224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630604001�䷽�ű�********************//

//********************��ʼ:630605001�䷽�ű�********************//
function Produce_630605001(%Player, %PresId)
{
	%ItemList_Gray = "101060025";
	%ItemList_White = "101060025";
	%ItemList_Green = "101061225";
	%ItemList_Blue = "101062225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630605001�䷽�ű�********************//

//********************��ʼ:630606001�䷽�ű�********************//
function Produce_630606001(%Player, %PresId)
{
	%ItemList_Gray = "101060026";
	%ItemList_White = "101060026";
	%ItemList_Green = "101061226";
	%ItemList_Blue = "101062226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630606001�䷽�ű�********************//

//********************��ʼ:630702001�䷽�ű�********************//
function Produce_630702001(%Player, %PresId)
{
	%ItemList_Gray = "101070022";
	%ItemList_White = "101070022";
	%ItemList_Green = "101071222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630702001�䷽�ű�********************//

//********************��ʼ:630703001�䷽�ű�********************//
function Produce_630703001(%Player, %PresId)
{
	%ItemList_Gray = "101070023";
	%ItemList_White = "101070023";
	%ItemList_Green = "101071223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630703001�䷽�ű�********************//

//********************��ʼ:630704001�䷽�ű�********************//
function Produce_630704001(%Player, %PresId)
{
	%ItemList_Gray = "101070024";
	%ItemList_White = "101070024";
	%ItemList_Green = "101071224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630704001�䷽�ű�********************//

//********************��ʼ:630705001�䷽�ű�********************//
function Produce_630705001(%Player, %PresId)
{
	%ItemList_Gray = "101070025";
	%ItemList_White = "101070025";
	%ItemList_Green = "101071225";
	%ItemList_Blue = "101072225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630705001�䷽�ű�********************//

//********************��ʼ:630706001�䷽�ű�********************//
function Produce_630706001(%Player, %PresId)
{
	%ItemList_Gray = "101070026";
	%ItemList_White = "101070026";
	%ItemList_Green = "101071226";
	%ItemList_Blue = "101072226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630706001�䷽�ű�********************//

//********************��ʼ:630502001�䷽�ű�********************//
function Produce_630502001(%Player, %PresId)
{
	%ItemList_Gray = "101050022";
	%ItemList_White = "101050022";
	%ItemList_Green = "101051222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630502001�䷽�ű�********************//

//********************��ʼ:630503001�䷽�ű�********************//
function Produce_630503001(%Player, %PresId)
{
	%ItemList_Gray = "101050023";
	%ItemList_White = "101050023";
	%ItemList_Green = "101051223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630503001�䷽�ű�********************//

//********************��ʼ:630504001�䷽�ű�********************//
function Produce_630504001(%Player, %PresId)
{
	%ItemList_Gray = "101050024";
	%ItemList_White = "101050024";
	%ItemList_Green = "101051224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630504001�䷽�ű�********************//

//********************��ʼ:630505001�䷽�ű�********************//
function Produce_630505001(%Player, %PresId)
{
	%ItemList_Gray = "101050025";
	%ItemList_White = "101050025";
	%ItemList_Green = "101051225";
	%ItemList_Blue = "101052225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630505001�䷽�ű�********************//

//********************��ʼ:630506001�䷽�ű�********************//
function Produce_630506001(%Player, %PresId)
{
	%ItemList_Gray = "101050026";
	%ItemList_White = "101050026";
	%ItemList_Green = "101051226";
	%ItemList_Blue = "101052226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630506001�䷽�ű�********************//

//********************��ʼ:630402001�䷽�ű�********************//
function Produce_630402001(%Player, %PresId)
{
	%ItemList_Gray = "101040022";
	%ItemList_White = "101040022";
	%ItemList_Green = "101041222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630402001�䷽�ű�********************//

//********************��ʼ:630403001�䷽�ű�********************//
function Produce_630403001(%Player, %PresId)
{
	%ItemList_Gray = "101040023";
	%ItemList_White = "101040023";
	%ItemList_Green = "101041223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630403001�䷽�ű�********************//

//********************��ʼ:630404001�䷽�ű�********************//
function Produce_630404001(%Player, %PresId)
{
	%ItemList_Gray = "101040024";
	%ItemList_White = "101040024";
	%ItemList_Green = "101041224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630404001�䷽�ű�********************//

//********************��ʼ:630405001�䷽�ű�********************//
function Produce_630405001(%Player, %PresId)
{
	%ItemList_Gray = "101040025";
	%ItemList_White = "101040025";
	%ItemList_Green = "101041225";
	%ItemList_Blue = "101042225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630405001�䷽�ű�********************//

//********************��ʼ:630406001�䷽�ű�********************//
function Produce_630406001(%Player, %PresId)
{
	%ItemList_Gray = "101040026";
	%ItemList_White = "101040026";
	%ItemList_Green = "101041226";
	%ItemList_Blue = "101042226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630406001�䷽�ű�********************//

//********************��ʼ:630901201�䷽�ű�********************//
function Produce_630901201(%Player, %PresId)
{
	%ItemList_Gray = "102020021";
	%ItemList_White = "102020021";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 0 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630901201�䷽�ű�********************//

//********************��ʼ:630902201�䷽�ű�********************//
function Produce_630902201(%Player, %PresId)
{
	%ItemList_Gray = "102020022";
	%ItemList_White = "102020022";
	%ItemList_Green = "102021222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630902201�䷽�ű�********************//

//********************��ʼ:630903201�䷽�ű�********************//
function Produce_630903201(%Player, %PresId)
{
	%ItemList_Gray = "102020023";
	%ItemList_White = "102020023";
	%ItemList_Green = "102021223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630903201�䷽�ű�********************//

//********************��ʼ:630904201�䷽�ű�********************//
function Produce_630904201(%Player, %PresId)
{
	%ItemList_Gray = "102020024";
	%ItemList_White = "102020024";
	%ItemList_Green = "102021224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630904201�䷽�ű�********************//

//********************��ʼ:630905201�䷽�ű�********************//
function Produce_630905201(%Player, %PresId)
{
	%ItemList_Gray = "102020025";
	%ItemList_White = "102020025";
	%ItemList_Green = "102021225";
	%ItemList_Blue = "102022225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630905201�䷽�ű�********************//

//********************��ʼ:631201401�䷽�ű�********************//
function Produce_631201401(%Player, %PresId)
{
	%ItemList_Gray = "102050021";
	%ItemList_White = "102050021";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 0 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631201401�䷽�ű�********************//

//********************��ʼ:631202401�䷽�ű�********************//
function Produce_631202401(%Player, %PresId)
{
	%ItemList_Gray = "102050022";
	%ItemList_White = "102050022";
	%ItemList_Green = "102051222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631202401�䷽�ű�********************//

//********************��ʼ:631203401�䷽�ű�********************//
function Produce_631203401(%Player, %PresId)
{
	%ItemList_Gray = "102050023";
	%ItemList_White = "102050023";
	%ItemList_Green = "102051223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631203401�䷽�ű�********************//

//********************��ʼ:631204401�䷽�ű�********************//
function Produce_631204401(%Player, %PresId)
{
	%ItemList_Gray = "102050024";
	%ItemList_White = "102050024";
	%ItemList_Green = "102051224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631204401�䷽�ű�********************//

//********************��ʼ:631205401�䷽�ű�********************//
function Produce_631205401(%Player, %PresId)
{
	%ItemList_Gray = "102050025";
	%ItemList_White = "102050025";
	%ItemList_Green = "102051225";
	%ItemList_Blue = "102052225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631205401�䷽�ű�********************//

//********************��ʼ:630801601�䷽�ű�********************//
function Produce_630801601(%Player, %PresId)
{
	%ItemList_Gray = "102010021";
	%ItemList_White = "102010021";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 0 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630801601�䷽�ű�********************//

//********************��ʼ:630802601�䷽�ű�********************//
function Produce_630802601(%Player, %PresId)
{
	%ItemList_Gray = "102010022";
	%ItemList_White = "102010022";
	%ItemList_Green = "102011222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630802601�䷽�ű�********************//

//********************��ʼ:630803601�䷽�ű�********************//
function Produce_630803601(%Player, %PresId)
{
	%ItemList_Gray = "102010023";
	%ItemList_White = "102010023";
	%ItemList_Green = "102011223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630803601�䷽�ű�********************//

//********************��ʼ:630804601�䷽�ű�********************//
function Produce_630804601(%Player, %PresId)
{
	%ItemList_Gray = "102010024";
	%ItemList_White = "102010024";
	%ItemList_Green = "102011224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630804601�䷽�ű�********************//

//********************��ʼ:630805601�䷽�ű�********************//
function Produce_630805601(%Player, %PresId)
{
	%ItemList_Gray = "102010025";
	%ItemList_White = "102010025";
	%ItemList_Green = "102011225";
	%ItemList_Blue = "102012225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:630805601�䷽�ű�********************//

//********************��ʼ:631401801�䷽�ű�********************//
function Produce_631401801(%Player, %PresId)
{
	%ItemList_Gray = "102070021";
	%ItemList_White = "102070021";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 0 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631401801�䷽�ű�********************//

//********************��ʼ:631402801�䷽�ű�********************//
function Produce_631402801(%Player, %PresId)
{
	%ItemList_Gray = "102070022";
	%ItemList_White = "102070022";
	%ItemList_Green = "102071222";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 3000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631402801�䷽�ű�********************//

//********************��ʼ:631403801�䷽�ű�********************//
function Produce_631403801(%Player, %PresId)
{
	%ItemList_Gray = "102070023";
	%ItemList_White = "102070023";
	%ItemList_Green = "102071223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631403801�䷽�ű�********************//

//********************��ʼ:631404801�䷽�ű�********************//
function Produce_631404801(%Player, %PresId)
{
	%ItemList_Gray = "102070024";
	%ItemList_White = "102070024";
	%ItemList_Green = "102071224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631404801�䷽�ű�********************//

//********************��ʼ:631405801�䷽�ű�********************//
function Produce_631405801(%Player, %PresId)
{
	%ItemList_Gray = "102070025";
	%ItemList_White = "102070025";
	%ItemList_Green = "102071225";
	%ItemList_Blue = "102072225";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 500 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631405801�䷽�ű�********************//

//********************��ʼ:631102001�䷽�ű�********************//
function Produce_631102001(%Player, %PresId)
{
	%ItemList_Gray = "102040022";
	%ItemList_White = "102040022";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 0 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631102001�䷽�ű�********************//

//********************��ʼ:631103001�䷽�ű�********************//
function Produce_631103001(%Player, %PresId)
{
	%ItemList_Gray = "102040023";
	%ItemList_White = "102040023";
	%ItemList_Green = "102041223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631103001�䷽�ű�********************//

//********************��ʼ:631104001�䷽�ű�********************//
function Produce_631104001(%Player, %PresId)
{
	%ItemList_Gray = "102040024";
	%ItemList_White = "102040024";
	%ItemList_Green = "102041224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631104001�䷽�ű�********************//

//********************��ʼ:631105001�䷽�ű�********************//
function Produce_631105001(%Player, %PresId)
{
	%ItemList_Gray = "102040025";
	%ItemList_White = "102040025";
	%ItemList_Green = "102041225";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631105001�䷽�ű�********************//

//********************��ʼ:631106001�䷽�ű�********************//
function Produce_631106001(%Player, %PresId)
{
	%ItemList_Gray = "102040026";
	%ItemList_White = "102040026";
	%ItemList_Green = "102041226";
	%ItemList_Blue = "102042226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631106001�䷽�ű�********************//

//********************��ʼ:631302001�䷽�ű�********************//
function Produce_631302001(%Player, %PresId)
{
	%ItemList_Gray = "102060022";
	%ItemList_White = "102060022";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 0 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631302001�䷽�ű�********************//

//********************��ʼ:631303001�䷽�ű�********************//
function Produce_631303001(%Player, %PresId)
{
	%ItemList_Gray = "102060023";
	%ItemList_White = "102060023";
	%ItemList_Green = "102061223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631303001�䷽�ű�********************//

//********************��ʼ:631304001�䷽�ű�********************//
function Produce_631304001(%Player, %PresId)
{
	%ItemList_Gray = "102060024";
	%ItemList_White = "102060024";
	%ItemList_Green = "102061224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631304001�䷽�ű�********************//

//********************��ʼ:631305001�䷽�ű�********************//
function Produce_631305001(%Player, %PresId)
{
	%ItemList_Gray = "102060025";
	%ItemList_White = "102060025";
	%ItemList_Green = "102061225";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631305001�䷽�ű�********************//

//********************��ʼ:631306001�䷽�ű�********************//
function Produce_631306001(%Player, %PresId)
{
	%ItemList_Gray = "102060026";
	%ItemList_White = "102060026";
	%ItemList_Green = "102061226";
	%ItemList_Blue = "102062226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631306001�䷽�ű�********************//

//********************��ʼ:631002001�䷽�ű�********************//
function Produce_631002001(%Player, %PresId)
{
	%ItemList_Gray = "102030022";
	%ItemList_White = "102030022";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 0 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631002001�䷽�ű�********************//

//********************��ʼ:631003001�䷽�ű�********************//
function Produce_631003001(%Player, %PresId)
{
	%ItemList_Gray = "102030023";
	%ItemList_White = "102030023";
	%ItemList_Green = "102031223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631003001�䷽�ű�********************//

//********************��ʼ:631004001�䷽�ű�********************//
function Produce_631004001(%Player, %PresId)
{
	%ItemList_Gray = "102030024";
	%ItemList_White = "102030024";
	%ItemList_Green = "102031224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631004001�䷽�ű�********************//

//********************��ʼ:631005001�䷽�ű�********************//
function Produce_631005001(%Player, %PresId)
{
	%ItemList_Gray = "102030025";
	%ItemList_White = "102030025";
	%ItemList_Green = "102031225";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631005001�䷽�ű�********************//

//********************��ʼ:631006001�䷽�ű�********************//
function Produce_631006001(%Player, %PresId)
{
	%ItemList_Gray = "102030026";
	%ItemList_White = "102030026";
	%ItemList_Green = "102031226";
	%ItemList_Blue = "102032226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631006001�䷽�ű�********************//

//********************��ʼ:631502001�䷽�ű�********************//
function Produce_631502001(%Player, %PresId)
{
	%ItemList_Gray = "103010022";
	%ItemList_White = "103010022";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 0 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631502001�䷽�ű�********************//

//********************��ʼ:631503001�䷽�ű�********************//
function Produce_631503001(%Player, %PresId)
{
	%ItemList_Gray = "103010023";
	%ItemList_White = "103010023";
	%ItemList_Green = "103011223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631503001�䷽�ű�********************//

//********************��ʼ:631504001�䷽�ű�********************//
function Produce_631504001(%Player, %PresId)
{
	%ItemList_Gray = "103010024";
	%ItemList_White = "103010024";
	%ItemList_Green = "103011224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631504001�䷽�ű�********************//

//********************��ʼ:631505001�䷽�ű�********************//
function Produce_631505001(%Player, %PresId)
{
	%ItemList_Gray = "103010025";
	%ItemList_White = "103010025";
	%ItemList_Green = "103011225";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631505001�䷽�ű�********************//

//********************��ʼ:631506001�䷽�ű�********************//
function Produce_631506001(%Player, %PresId)
{
	%ItemList_Gray = "103010026";
	%ItemList_White = "103010026";
	%ItemList_Green = "103011226";
	%ItemList_Blue = "103012226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631506001�䷽�ű�********************//

//********************��ʼ:631602001�䷽�ű�********************//
function Produce_631602001(%Player, %PresId)
{
	%ItemList_Gray = "103020022";
	%ItemList_White = "103020022";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 0 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631602001�䷽�ű�********************//

//********************��ʼ:631603001�䷽�ű�********************//
function Produce_631603001(%Player, %PresId)
{
	%ItemList_Gray = "103020023";
	%ItemList_White = "103020023";
	%ItemList_Green = "103021223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631603001�䷽�ű�********************//

//********************��ʼ:631604001�䷽�ű�********************//
function Produce_631604001(%Player, %PresId)
{
	%ItemList_Gray = "103020024";
	%ItemList_White = "103020024";
	%ItemList_Green = "103021224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631604001�䷽�ű�********************//

//********************��ʼ:631605001�䷽�ű�********************//
function Produce_631605001(%Player, %PresId)
{
	%ItemList_Gray = "103020025";
	%ItemList_White = "103020025";
	%ItemList_Green = "103021225";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631605001�䷽�ű�********************//

//********************��ʼ:631606001�䷽�ű�********************//
function Produce_631606001(%Player, %PresId)
{
	%ItemList_Gray = "103020026";
	%ItemList_White = "103020026";
	%ItemList_Green = "103021226";
	%ItemList_Blue = "103022226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631606001�䷽�ű�********************//

//********************��ʼ:631702001�䷽�ű�********************//
function Produce_631702001(%Player, %PresId)
{
	%ItemList_Gray = "103030022";
	%ItemList_White = "103030022";
	%ItemList_Green = "";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 0 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631702001�䷽�ű�********************//

//********************��ʼ:631703001�䷽�ű�********************//
function Produce_631703001(%Player, %PresId)
{
	%ItemList_Gray = "103030023";
	%ItemList_White = "103030023";
	%ItemList_Green = "103031223";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 5000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631703001�䷽�ű�********************//

//********************��ʼ:631704001�䷽�ű�********************//
function Produce_631704001(%Player, %PresId)
{
	%ItemList_Gray = "103030024";
	%ItemList_White = "103030024";
	%ItemList_Green = "103031224";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631704001�䷽�ű�********************//

//********************��ʼ:631705001�䷽�ű�********************//
function Produce_631705001(%Player, %PresId)
{
	%ItemList_Gray = "103030025";
	%ItemList_White = "103030025";
	%ItemList_Green = "103031225";
	%ItemList_Blue = "";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 9000 0 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631705001�䷽�ű�********************//

//********************��ʼ:631706001�䷽�ű�********************//
function Produce_631706001(%Player, %PresId)
{
	%ItemList_Gray = "103030026";
	%ItemList_White = "103030026";
	%ItemList_Green = "103031226";
	%ItemList_Blue = "103032226";
	%ItemList_Purple = "";

	%OddsList_Gray = "";
	%OddsList_White = "";
	%OddsList_Green = "";
	%OddsList_Blue = "";
	%OddsList_Purple = "";

	%Odds_ColorList = "100 0 8000 1900 0";

	%OddsModeList = "10000 2";

	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList);

	return Produce_PropsOperation(%Player, %PresId, %ResultItem, 1);
}
//********************����:631706001�䷽�ű�********************//
