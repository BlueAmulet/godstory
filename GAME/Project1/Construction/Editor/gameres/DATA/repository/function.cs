
// ����װ��ǿ���������ļ۸�
function CalcEquipStrengthenPrice(%part, %color, %level, %times)
{
	//Gold=��ɫƷ�ʣ�ֵ��*װ����λ��ֵ��*2*1.06�ȼ�����*2ǿ������
	if(%part == 1)
		%part = 1.60;		//����
	else if(%part == 2)
		%part = 0.90;		//����
	else if(%part == 4)
		%part = 1.00;		//��Ʒ
	else
		%part = 1.80;
	
	if(%color == 2)
		%color = 1.00;	//��ɫ
	else if(%color == 3)
		%color = 1.20;	//��ɫ
	else if(%color == 4)
		%color = 1.40; 	//��ɫ
	else if(%color == 5)
		%color = 1.60;	//��ɫ
	else if(%color == 6)
		%color = 2.00; 	//��ɫ
	else
		%color = 0.0;
		
	return %color * %part * 2* mpow(1.06, %level) * mpow(2,%times);
}

// ����װ�������������ļ۸�
function CalcEquipIdentifyPrice(%part, %color, %level)
{
	//Gold=������ֵ��*��װ����ɫȡֵ��*��װ������ȡֵ��*���ȱȲ�����װ������
	if(%part == 1)
		%part = 0.4;		//����
	else if(%part == 2)
		%part = 0.1;		//����
	else if(%part == 3)
		%part = 0.2;		//��Ʒ
	else
		%part = 0.0;
		
	if(%color == 3)
		%color = 1.1;		//��ɫ
	else if(%color == 4)
		%color = 1.3;		//��ɫ
	else if(%color == 5)
		%color = 1.6;		//��ɫ
	else if(%color == 6)
		%color = 2.0;		//��ɫ
	else
		%color = 0.0;
	return 20.0 * %color * %part * mpow(1.08, %level);
}

function AddEquipStrengthenConfig(%times, %materialid, %materialnum, %DegradeRate, %succ_white, %succ_green, %succ_blue, %succ_purple, %succ_orange)
{
	%EquipStrengthenObjectID = "EquipSthengthenObject_" @ %times;
	new ScriptObject(%EquipStrengthenObjectID) {
      materialid 		= %materialid;		// ����ID
      materialnum 	= %materialnum;		// ��������
      degraderate 	= %degraderate;		// �˼�����
      succeed_white = %succ_white;		// ��ɫ�ɹ�����
      succeed_green = %succ_green;		// ��ɫ�ɹ�����
      succeed_blue 	= %succ_blue;			// ��ɫ�ɹ�����
      succeed_purple= %succ_purple;		// ��ɫ�ɹ�����
      succeed_orang = %succ_orange;		// ��ɫ�ɹ�����
   };
	$EquipStrengthenObject[%times] = %EquipStrengthenObjectID;
}

// ��ȡװ��ǿ��������
function GetEquipSthrengthenConfig(%times, %valueType)
{
	%result = 0;
	%obj = $EquipStrengthenObject[%times];
	switch(%valueType)
	{
		case 0: %result = %obj.materialid;
		case 1: %result = %obj.materialnum;
		case 2: %result = %obj.degraderate;
		case 3: %result = %obj.succeed_white;
		case 4: %result = %obj.succeed_green;
		case 5: %result = %obj.succeed_blue;
		case 6: %result = %obj.succeed_purple;
		case 7: %result = %obj.succeed_orang;
	}
    
    echo ("GetEquipSthrengthenConfig times: " @ %times);
    echo ("GetEquipSthrengthenConfig valueType: " @ %valueType);
    echo ("GetEquipSthrengthenConfig result: " @ %result);

	return %result;
}

AddEquipStrengthenConfig(1, 116010001,	1,   0,	10000, 	10000, 	10000, 	10000, 	10000); 
AddEquipStrengthenConfig(2, 116010001,	2,  4000,	10000, 	9000, 	9000, 	9000, 	9000 ); 
AddEquipStrengthenConfig(3, 116010001,	3,	5000,	9000, 	9000, 	8000, 	8000, 	8000 ); 
AddEquipStrengthenConfig(4, 116010002,	1,	6000,	9000,	8000,	7000,	7000,	7000 ); 
AddEquipStrengthenConfig(5, 116010002,	2,	7000,	8000, 	8000, 	7000, 	6000, 	6000 );
AddEquipStrengthenConfig(6, 116010002,	3, 8000,	8000, 	7000, 	6000, 	6000, 	5000 ); 
AddEquipStrengthenConfig(7, 116010003,	1, 9000,	7000, 	7000, 	6000, 	5000, 	4000 ); 
AddEquipStrengthenConfig(8, 116010003,	2, 10000,	7000, 	6000, 	5000, 	4000, 	3000 ); 
AddEquipStrengthenConfig(9, 116010003,	3, 10000,	6000, 	6000, 	4000, 	3000, 	2000 ); 
AddEquipStrengthenConfig(10,116010004,	1, 10000,	6000, 	5000, 	3000, 	2000, 	1000 ); 
AddEquipStrengthenConfig(11,116010004,	2, 10000,	5000,	4000,  	900,  	900,  	900  ); 
AddEquipStrengthenConfig(12,116010004,	3, 10000,	5000, 	4000, 	800, 	800, 	800  ); 
AddEquipStrengthenConfig(13,116010005,	1, 10000,	4000, 	3000, 	700, 	700, 	700  ); 
AddEquipStrengthenConfig(14,116010005,	2, 10000,	4000, 	3000, 	600, 	600, 	600  ); 
AddEquipStrengthenConfig(15,116010005,	3, 10000,	4000, 	3000, 	500,  500,  500      ); 

//////////// ժȡ��Ƕ��ʯ������غ��� ///////////////////////////////////////////
function AddEquipGemUnmountConfig(%GemLevel, %MaterialId, %MaterialNum)
{
	%EquipGemUnmountObjectID = "EquipGemUnmountObjectID_" @ %GemLevel;
	new ScriptObject(%EquipGemUnmountObjectID) {
      materialid 		= %MaterialId;		// ����ID
      materialnum 	= %MaterialNum;		// ��������
  };
  $EquipGemUnmountObject[%GemLevel] = %EquipGemUnmountObjectID;
}

AddEquipGemUnmountConfig(1, 116040001, 2);
AddEquipGemUnmountConfig(2, 116040002, 2);
AddEquipGemUnmountConfig(3, 116040003, 2);
AddEquipGemUnmountConfig(4, 116040004, 2);
AddEquipGemUnmountConfig(5, 116040005, 2);
AddEquipGemUnmountConfig(6, 116040006, 2);
AddEquipGemUnmountConfig(7, 116040007, 2);
AddEquipGemUnmountConfig(8, 116040008, 2);
AddEquipGemUnmountConfig(9, 116040009, 2);
AddEquipGemUnmountConfig(10, 116040010, 2);

function GetEquipGemUnmountConfig(%GemLevel, %valueType)
{
	%obj = $EquipGemUnmountObject[%GemLevel];
	switch(%valueType)
	{
		case 0: return %obj.MaterialId;
		case 1: return %obj.MaterialNum;
	}
	
	return 0;
}

////////////////////װ����Ƕ��ʯ��ɫ������ϵ��////////////////////////////////
function GemIsColorMatch(%type, %color)
{
	if (%type == 1)					//��ɫϵ:	��ɫ����ɫ����ɫ
	{
		switch(%color)
		{
			case 1001:	return 1;		//��ɫ
			case 1004:	return 1;		//��ɫ
			case 1006:	return 1; 	//��ɫ
		}
	}
	else if (%type == 2)		//��ɫϵ:	��ɫ����ɫ����ɫ
	{
		switch(%color)
		{
			case 1002:	return 1;		//��ɫ
			case 1005:	return 1;		//��ɫ
			case 1004:	return 1; 	//��ɫ
		}
	}
	else if (%type == 3)		//��ɫϵ: ��ɫ����ɫ����ɫ
	{
		switch(%color)
		{
			case 1003:	return 1;		//��ɫ
			case 1006:	return 1;		//��ɫ
			case 1005:	return 1; 	//��ɫ
		}
	}
	
	return 0;
}

//////////// װ�������غ��� ///////////////////////////////////////////
function GetEquipPunchHoleCostMoney(%level, %openHoleNum, %allowNum)
{
	if (%openHoleNum >= %allowNum)
		return 0;
	%money = 100 * %level * (%openHoleNum+1);
	return %money;
}

function GetEquipPunchHoleSuceessRate(%nHoles)
{
	switch(%nHoles)
	{
		case 0:		return 6000;		// 60%
		case 1:		return 3000;		// 30%
		case 2:		return 500;		  // 5%
	}
	
	return 0;
}

//////////// ����ϵͳ ///////////////////////////////////////////
function GetIncreaseInsightSuccessRate(%nInsight)
{
	switch(%nInsight)
	{
	case 1:		return 10000;
	case 2:		return 9000;
	case 3: 	return 8000;
	case 4: 	return 7000;
	case 5: 	return 6000;
	case 6: 	return 5000;
	case 7: 	return 4000;
	case 8: 	return 3000;
	case 9: 	return 2000;
	case 10: 	return 1000;
	default:	return 0;
	}
}

function GetGenGuLevelByInsight(%nInsight)
{
	if (%nInsight >= 0 && %nInsight <= 2)
		return 1;
	if (%nInsight >= 3 && %nInsight <= 5)
		return 2;
	if (%nInsight >= 6 && %nInsight <= 8)
		return 3;
	if (%nInsight >= 9 && %nInsight <= 10)
		return 4;
		
	return 0;
}

function IsInsightMatchGenGuDan(%nInsight, %nGenGuLevel)
{
	%nLevel = GetGenGuLevelByInsight(%nInsight);
	if (%nLevel == 0)
		return 0;
	
	return %nLevel == %nGenGuLevel;
}

function GetDecreasedInsightLevel(%nInsight)
{
	%nLevel = GetGenGuLevelByInsight(%nInsight);
	
	if ( (%nLevel == 1 && %nInsight == 0) ||
	     (%nLevel == 2 && %nInsight == 3) ||
	     (%nLevel == 3 && %nInsight == 6) ||
	     (%nLevel == 4 && %nInsight == 9))
	{
		return %nInsight;
	}
	
	return %nInsight - 1;
}

function PetLianHua_GetGenGuDanId(%nGenGu)
{
	if (%nGenGu <= 2)
		return 116070001;
	else if (%nGenGu > 2 && %nGenGu <= 5)
		return 116070002;
	else if (%nGenGu > 5 && %nGenGu <= 8)
		return 116070003;
	else if (%nGenGu > 8)
		return 116070004;
	else
		return 0;
}

function PetIdentify_GetCostMoney()
{
	return 1;
}

function PetInsight_GetCostMoney()
{
	return 1;
}

function PetLianHua_GetCostMoney()
{
	return 1;
}

function PetHuanTong_GetCostMoney()
{
	return 1;	
}

//��ÿۼ�װ���Ĳ�λ
function getRepairEquipPos(%type, %num)
{
	if(%type == 1)//����
	{
		if(%num > 0 && %num < 61)
		 return 11; //����
		else if(%num > 60 && %num < 86)
		 return 1; //ͷ��
		else if(%num > 85 && %num <= 100)
		 return 4; //�粿
	}
	else if(%type == 2) //װ��
	{
		if(%num > 0 && %num < 13)
		 return 3; //��
		else if(%num > 12 && %num < 43)
		 return 5; //����
		else if(%num > 42 && %num < 61)
		 return 6; //��
		else if(%num > 60 && %num < 86)
		 return 8; //��
		else if(%num > 85 && %num <= 100)
		 return 7; //��
	}
}

//����۸�
function getRepairPrice(%max,%curr,%maxCurr,%level)
{
	%base = 35;
    echo(%base*%max*(1-%curr/%maxCurr)/%level);
	return %base*%max*(1-%curr/%maxCurr)/%level;
}
//����ʧ����
function getRepairFailRate(%level)
{
	if(%level < 60)
		return 0;
	else if(%level >= 60)
	 return 50;
}

function getMaxRepairPrice(%max,%maxCurr)
{
	%base = 3000;
   echo(%base*(2*%max-%maxCurr));
	return %base*(2*%max-%maxCurr);
}