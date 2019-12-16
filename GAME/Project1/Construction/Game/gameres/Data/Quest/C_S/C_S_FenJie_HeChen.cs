//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//������ҷֽ����ʱ������˽ű�����
//==================================================================================


//==================================================================================
//����˽ű�����(�ֽ�)
//serverItemSplit(%itemId)
//�ֽ�󴫻ز��ϲ�����ʽ����
//"itemId quantity"@ TAB @ "itemId quantity"......���8��
//TAB�ű�Ĭ��֧��,��ʵ����\t
//���ֽܷⷵ��0
//(�ϳɽű�)
//serverItemCompose(%player,%itemInfolist);
//�ɹ�����itemID,���ɹ�����0
//
//getServerComposeInfo();
//��ͨ�ϳɷ���0,����ϳɷ���%itemInfolist
//
//%itemInfolist ��ʽ
//"mItemIdTag" @ TAB @ "itemID Quantity"......
//"itemID,Quantity"���8��,����һ��
//
//serverItemCompose(%player,%itemInfolist);
//�ɹ�����itemID ,Quantity,���ɹ�����0

//openItemSplitWnd(%playerId);//�ֽ�
//openItemComposeWnd(%playerId,%itemInfo);//�ϳ�
//%itemInfo = "106020101\t106020102 5\t106020103 100"


//$Fen[11110000] = "123456789 1 10"@ TAB @ "123456789 1 20"@ TAB @ "12345679 1 30";
//strcmpBySet(%str1,%str2,%set)�Ƚ������ַ��������ز���ֵ��%str1=%itemInfolist
//==================================================================================

//==============================�ϳ���ڵ�==============================//
function ServerItemSplit(%Player, %ItemId, %ItemNum)
{
  if(%player == 0 || %itemId == 0 || %itemNum == 0)
		return 0;
  
  //2�ǵ��ߴ���
  %itemType = GetItemData(%ItemId, 2);
  %itemSubType = GetItemData(%ItemId, 3);
  %itemQuality = GetItemData(%ItemId, 19);
  %itemLevel = GetItemData(%ItemId, 24);
  
  %result = 0;
  
  //���������ߡ���Ʒ�ֽ�
  if(%itemType == 1 || %itemType == 2 || %itemType == 3)
  {
  	%result = getEquipmentResult(%player, %itemQuality, %itemLevel);

  	//�����Ҫ����������ߣ��ϲ�����Ľ����
  	//��ȡ���⸽��
  	%extraResult = getEquipmentExtra(%player, %itemType, %itemSubType, %itemLevel);
  	
  	if(%extraResult != 0)
  	{
  		if(%result != 0){%result = %result @ "|" @ %extraResult;}
  		else{%result = %extraResult;}
  	}
  	
  	return %result;
  }
  
  return 0;
}

//��ȡװ���ֽ���
function getEquipmentResult(%player, %itemQuality, %itemLevel)
{
	//����ҳ����ֽ�װ���ȼ�10�������ʼ��롣
	%level = %player.getlevel();
	%factor = 1;
	
	//��ɫ
	if(%itemQuality == 3)
	{
		//if(%level - %itemLevel >= 10){%factor = 2;}
		
		if(%itemLevel <= 10)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 5)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 5)		{return "116010001 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)	{return "116010001 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 80)	{return "116010001 1";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 20)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)	{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)	{return "116010001 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)	{return "116010001 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 60)	{return "116010001 1";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 30)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)	{return "116010002 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)	{return "116010001 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)	{return "116010001 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 40)	{return "116010001 1";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 40)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 5)		{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)	{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)	{return "116010001 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 40)	{return "116010001 2";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 50)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)	{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 25)	{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 25)	{return "116010001 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)	{return "116010001 2";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 60)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)	{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)	{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)	{return "116010001 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)	{return "116010001 2";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 70)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 5)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)	{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)	{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)	{return "116010001 3";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 80)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)	{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 25)	{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)	{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 25)	{return "116010001 3";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 90)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)	{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)	{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)	{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)	{return "116010001 3";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 100)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)	{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 35)	{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 5)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)	{return "116010001 3";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
	}
	
	//��ɫ
	if(%itemQuality == 4)
	{
		//if(%level - %itemLevel >= 10){%factor = 2;}
		
		if(%itemLevel <= 10)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 40)		{return "116010001 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 50)		{return "116010001 2";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 20)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010001 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 40)		{return "116010001 2";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 30)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 50)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010001 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010001 2";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 40)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 5)			{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)		{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 45)		{return "116010001 3";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 50)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 40)		{return "116010001 3";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 60)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 35)		{return "116010001 3";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 70)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 1)			{return "116010003 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 5)			{return "116010002 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010001 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010001 4";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 80)
		{
			%rndResult = GetRandom(1 , 200 * %factor);
			if(%rndResult <= 3)			{return "116010003 1";}
			%rndResult = GetRandom(1 , 200 * %factor);
			if(%rndResult <= 20)		{return "116010002 3";}
			%rndResult = GetRandom(1 , 200 * %factor);
			if(%rndResult <= 40)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 200 * %factor);
			if(%rndResult <= 50)		{return "116010001 1";}
			%rndResult = GetRandom(1 , 200 * %factor);
			if(%rndResult <= 30)		{return "116010001 4";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 90)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 2)			{return "116010003 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)		{return "116010002 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 25)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010001 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)		{return "116010001 4";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		
		if(%itemLevel <= 100)
		{
			%rndResult = GetRandom(1 , 200 * %factor);
			if(%rndResult <= 5)			{return "116010003 1";}
			%rndResult = GetRandom(1 , 200 * %factor);
			if(%rndResult <= 10)		{return "116010002 3";}
			%rndResult = GetRandom(1 , 200 * %factor);
			if(%rndResult <= 30)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 200 * %factor);
			if(%rndResult <= 60)		{return "116010001 1";}
			%rndResult = GetRandom(1 , 200 * %factor);
			if(%rndResult <= 40)		{return "116010001 4";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
	}
	
	//��ɫ
	if(%itemQuality == 5)
	{
		//if(%level - %itemLevel >= 10){%factor = 2;}
		
		if(%itemLevel <= 10)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 40)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 60)		{return "116010001 3";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 20)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 70)		{return "116010001 3";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 30)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)		{return "116010001 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 90)		{return "116010001 3";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 40)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 5)			{return "116010002 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010001 4";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 50)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)		{return "116010002 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 35)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 25)		{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010001 4";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 60)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)		{return "116010002 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 40)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010002 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)		{return "116010001 4";}
			
			//���û���ʣ�����1��1������
			return "116010001 1";
		}
		
		if(%itemLevel <= 70)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 2)			{return "116010003 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 5)			{return "116010002 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)		{return "116010002 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 25)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010002 1";}
			
			//���û���ʣ�����1��2������
			return "116010002 1";
		}
		
		if(%itemLevel <= 80)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 3)			{return "116010003 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)		{return "116010002 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010002 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 25)		{return "116010002 1";}
			
			//���û���ʣ�����1��2������
			return "116010002 1";
		}
		
		if(%itemLevel <= 90)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 4)			{return "116010003 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)		{return "116010002 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 25)		{return "116010002 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010002 1";}
			
			//���û���ʣ�����1��2������
			return "116010002 1";
		}
		
		if(%itemLevel <= 100)
		{
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 5)			{return "116010003 1";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 20)		{return "116010002 4";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 30)		{return "116010002 3";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 10)		{return "116010002 2";}
			%rndResult = GetRandom(1 , 100 * %factor);
			if(%rndResult <= 15)		{return "116010002 1";}
			
			//���û���ʣ�����1��2������
			return "116010002 1";
		}
	}
	
	return 0;
}

//��ȡװ���ֽ������
function getEquipmentExtra(%player, %itemType, %itemSubType, %itemLevel)
{
	%rndResult = GetRandom(1 , 100);
	
	%ItemId = 0;
	
	if(%itemLevel <= 39)
	{
		if((%itemSubType == 101 || %itemSubType == 104 || %itemSubType == 105) 												&& %rndResult <= 10){return "106020401 1";}//Զ������
		if((%itemSubType == 102 || %itemSubType == 103 || %itemSubType == 106 || %itemSubType == 107) && %rndResult <= 10){return "106020501 1";}//��������
		if((%itemSubType == 301 || %itemSubType == 302 || %itemSubType == 303) 												&& %rndResult <= 10){return "106050401 1";}//���κϳɲ���
		if((%itemSubType == 204 || %itemSubType == 205 || %itemSubType == 206 || %itemSubType == 207) && %rndResult <= 10){return "106080501 1";}//�ӹ���Ƥ
		if((%itemSubType == 201 || %itemSubType == 202 || %itemSubType == 203) 												&& %rndResult <= 10){return "106080401 1";}//�ϳ���Ƥ
	}
	
	if(%itemLevel <= 59)
	{
		if((%itemSubType == 101 || %itemSubType == 104 || %itemSubType == 105) 												&& %rndResult <= 10){return "106020402 1";}//Զ������
		if((%itemSubType == 102 || %itemSubType == 103 || %itemSubType == 106 || %itemSubType == 107) && %rndResult <= 10){return "106020502 1";}//��������
		if((%itemSubType == 301 || %itemSubType == 302 || %itemSubType == 303) 												&& %rndResult <= 10){return "106050402 1";}//���κϳɲ���
		if((%itemSubType == 204 || %itemSubType == 205 || %itemSubType == 206 || %itemSubType == 207) && %rndResult <= 10){return "106080502 1";}//�ӹ���Ƥ
		if((%itemSubType == 201 || %itemSubType == 202 || %itemSubType == 203) 												&& %rndResult <= 10){return "106080402 1";}//�ϳ���Ƥ
	}
	
	if(%itemLevel <= 79)
	{
		if((%itemSubType == 101 || %itemSubType == 104 || %itemSubType == 105) 												&& %rndResult <= 10){return "106020403 1";}//Զ������
		if((%itemSubType == 102 || %itemSubType == 103 || %itemSubType == 106 || %itemSubType == 107) && %rndResult <= 10){return "106020503 1";}//��������
		if((%itemSubType == 301 || %itemSubType == 302 || %itemSubType == 303) 												&& %rndResult <= 10){return "106050403 1";}//���κϳɲ���
		if((%itemSubType == 204 || %itemSubType == 205 || %itemSubType == 206 || %itemSubType == 207) && %rndResult <= 10){return "106080503 1";}//�ӹ���Ƥ
		if((%itemSubType == 201 || %itemSubType == 202 || %itemSubType == 203) 												&& %rndResult <= 10){return "106080403 1";}//�ϳ���Ƥ
	}
	
	if(%itemLevel <= 99)
	{
		if((%itemSubType == 101 || %itemSubType == 104 || %itemSubType == 105) 												&& %rndResult <= 10){return "106020404 1";}//Զ������
		if((%itemSubType == 102 || %itemSubType == 103 || %itemSubType == 106 || %itemSubType == 107) && %rndResult <= 10){return "106020504 1";}//��������
		if((%itemSubType == 301 || %itemSubType == 302 || %itemSubType == 303) 												&& %rndResult <= 10){return "106050404 1";}//���κϳɲ���
		if((%itemSubType == 204 || %itemSubType == 205 || %itemSubType == 206 || %itemSubType == 207) && %rndResult <= 10){return "106080504 1";}//�ӹ���Ƥ
		if((%itemSubType == 201 || %itemSubType == 202 || %itemSubType == 203) 												&& %rndResult <= 10){return "106080404 1";}//�ϳ���Ƥ
	}
	
	if(%itemLevel <= 119)
	{
		if((%itemSubType == 101 || %itemSubType == 104 || %itemSubType == 105) 												&& %rndResult <= 10){return "106020405 1";}//Զ������
		if((%itemSubType == 102 || %itemSubType == 103 || %itemSubType == 106 || %itemSubType == 107) && %rndResult <= 10){return "106020505 1";}//��������
		if((%itemSubType == 301 || %itemSubType == 302 || %itemSubType == 303) 												&& %rndResult <= 10){return "106050405 1";}//���κϳɲ���
		if((%itemSubType == 204 || %itemSubType == 205 || %itemSubType == 206 || %itemSubType == 207) && %rndResult <= 10){return "106080505 1";}//�ӹ���Ƥ
		if((%itemSubType == 201 || %itemSubType == 202 || %itemSubType == 203) 												&& %rndResult <= 10){return "106080405 1";}//�ϳ���Ƥ
	}
	
	return 0;
}

//==============================�ϳ���ڵ�==============================//
function serverItemCompose(%player,%itemInfo)
{
  if(%player == 0 || %itemInfo $= "")
     return 0;
     
  %stringCount = getWordCount(%itemInfo,"|\n");
  
  if(%stringCount <= 1)
    return 0;
  
  %itemTag = getWord(%itemInfo,0,"|\n");
  
  //%itemTag == 0 ��ͨ�ϳɣ�����Ϊ����ϳ�
  if(%itemTag == 0)
  {
	  %materialCount = %stringCount -1;
	  
  	if(%materialCount ==1)
  	{
  	    %tempInfo = getWord(%itemInfo,1,"|\n");
  			%itemId   = getWord(%tempInfo,0);
  			%quantity = getWord(%tempInfo,1);

				if(%itemId == 105020001 && %quantity == 3){return "105020002 1";}
				if(%itemId == 105020002 && %quantity == 3){return "105020003 1";}
				
				return 0;
  	}
  	else if(%materialCount == 2)
  	{
  	}
  	else if(%materialCount == 3)
  	{
  	}
  	else if(%materialCount == 4)
  	{
  	}
  	else if(%materialCount == 5)
  	{
  	}
  	else if(%materialCount == 6)
  	{
  	}
  	else if(%materialCount == 7)
  	{
  	}
  	else if(%materialCount == 8)
  	{
  	}
  	else
  	{
  	   return 0;
  	}
  }
  else
  {
  	return %itemTag @ " 1";
  	
//  	switch(%itemTag)
//  	{
//  		case 105020002:
//  		 return %itemTag @" 1";
//  		 
//  		default: return 0;
//  	}
  }
  
  return 0;
}