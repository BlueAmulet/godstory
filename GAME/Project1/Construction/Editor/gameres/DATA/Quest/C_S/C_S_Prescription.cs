//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//用于配方制造物品时的触发脚本
//程序会先判定玩家是否拥有足够的材料，然后触发进度条，当读条结束后才触发本脚本
//==================================================================================
//制造前置判断
//配方前置脚本判断属性中填写为“1”时调用
function ProduceCheck(%Player, %SerialId)
{
	if(%SerialId <= 0)
	   return 9999;

	 return 0;
}

function ProduceAll(%Player, %PresId)
{
	//Player	玩家对象
	//PresId	配方编号

	//GetPrescriptionData(%PresId,%Num);	读取配方数据
	//%Num，行数

	//echo("进入配方制作");
	//判断配方编号位数，必需为9位
	if(strlen(%PresId) == 9)
		{

			//判断配方编号第2位，依照规则执行相应脚本
			//第2位为1
			if(GetSubStr(%PresId,1,1) $= "1")
			{
				//echo("进入通用合成公式");
				Produce_Make_Normal(%Player, %PresId);		//通用合成公式
			}
			//第2位为2
			if(GetSubStr(%PresId,1,1) $= "2")
			{
				//echo("进入特殊合成公式");
				Produce_Make_Special(%Player, %PresId);		//特殊合成公式
			}
			//第2位为3
			if(GetSubStr(%PresId,1,1) $= "3")
			{
				//echo("进入独立配方合成公式");
				eval("Produce_"@%PresId@"(%Player, %PresId);");	//执行相应配方编号的独立函数
			}
		}
}

//通用制造公式
function Produce_Make_Normal(%Player, %PresId)
{
	//制造需求
	//材料
	%Materials[1] = GetPrescriptionData(%PresId, 10);
	%Materials[2] = GetPrescriptionData(%PresId, 12);
	%Materials[3] = GetPrescriptionData(%PresId, 14);
	%Materials[4] = GetPrescriptionData(%PresId, 16);
	%Materials[5] = GetPrescriptionData(%PresId, 18);
	%Materials[6] = GetPrescriptionData(%PresId, 20);
	//数量
	%Number[1] = GetPrescriptionData(%PresId, 11);
	%Number[2] = GetPrescriptionData(%PresId, 13);
	%Number[3] = GetPrescriptionData(%PresId, 15);
	%Number[4] = GetPrescriptionData(%PresId, 17);
	%Number[5] = GetPrescriptionData(%PresId, 19);
	%Number[6] = GetPrescriptionData(%PresId, 21);
	//活力
	%Vigor = GetPrescriptionData(%PresId, 4);
	//钞票
	%Money = GetPrescriptionData(%PresId, 7);

	//给于的道具
	%Item = GetPrescriptionData(%PresId, 22);
	//给于的数量
	%Item_Num = GetPrescriptionData(%PresId, 24);

	//判断玩家是否有这些数量的材料
	for(%i = 1; %i < 7; %i++)
	{
		if(%Materials[%i] > 0 && %Number[%i] > 0)
		{
			if(%Player.GetItemCount(%Materials[%i]) < %Number[%i])
				{
					//材料数量不够，合成失败
					return 4103;
				}
		}
	}
	//判断玩家是否有足够的活力
	if(%Player.GetVigor() < %Vigor)
		return 4103;	//材料数量不够，合成失败

	//判断玩家是否有足够的钞票
	if(%Player.GetMoney(1) < %Money)
		return 4103;	//材料数量不够，合成失败

	//直接给道具
	if(AddItemToInventory(%Player.GetPlayerID(), %Item, %Item_Num))
		{
			for(%i = 1;%i < 7;%i++)
			{
				if(%Materials[%i] !$= "0" && %Number[%i] !$= "0")
					DelItemFromInventory(%Player.GetPlayerID(), %Materials[%i], %Number[%i]);
			}
			//添加生活技能熟练度
			addLivingSkillGuerdon(%Player,GetPrescriptionData(%PresId, 6),1);
			//扣除活力值
			%Player.AddVigor(- %Vigor);
			//扣钱
			%Player.ReduceMoney(%Money, 1);

			return 0;	//制作成功
		}
		else
			{
				//道具无法给于，制造失败
				//可能情况：道具唯一属性；背包满；玩家处于无法获得道具的状态下
				return 4103;
			}
}

//特殊合成公式
function Produce_Make_Special()
{

}

function Produce_PropsOperation(%Player, %PresId, %ResultItem, %Count)
{
	if(strlen(%ResultItem) != 9) {return 4103;}

	if(%ResultItem == 0) {return 4103;}
	//获取需要扣除的材料
	//材料
	%Materials[1] = GetPrescriptionData(%PresId, 10);
	%Materials[2] = GetPrescriptionData(%PresId, 12);
	%Materials[3] = GetPrescriptionData(%PresId, 14);
	%Materials[4] = GetPrescriptionData(%PresId, 16);
	%Materials[5] = GetPrescriptionData(%PresId, 18);
	%Materials[6] = GetPrescriptionData(%PresId, 20);
	//数量
	%Number[1] = GetPrescriptionData(%PresId, 11);
	%Number[2] = GetPrescriptionData(%PresId, 13);
	%Number[3] = GetPrescriptionData(%PresId, 15);
	%Number[4] = GetPrescriptionData(%PresId, 17);
	%Number[5] = GetPrescriptionData(%PresId, 19);
	%Number[6] = GetPrescriptionData(%PresId, 21);
	//活力
	%Vigor = GetPrescriptionData(%PresId, 4);
	//钞票
	%Money = GetPrescriptionData(%PresId, 7);

	//判断玩家是否有这些数量的材料
	for(%i = 1; %i < 7; %i++)
	{
		if(%Materials[%i] > 0 && %Number[%i] > 0)
		{
			if(%Player.GetItemCount(%Materials[%i]) < %Number[%i])
				{
					//材料数量不够，合成失败
					return 4103;
				}
		}
	}
	//判断玩家是否有足够的活力
	if(%Player.GetVigor() < %Vigor)
		return 4103;	//材料数量不够，合成失败

	//判断玩家是否有足够的钞票
	if(%Player.GetMoney(1) < %Money)
		return 4103;	//材料数量不够，合成失败

	//直接给道具
	if(AddItemToInventory(%Player.GetPlayerID(), %ResultItem, %Count))
		{
			for(%i = 1;%i < 7;%i++)
			{
				if(%Materials[%i] !$= "0" && %Number[%i] !$= "0")
					DelItemFromInventory(%Player.GetPlayerID(), %Materials[%i], %Number[%i]);
			}
			//添加生活技能熟练度
			addLivingSkillGuerdon(%Player,GetPrescriptionData(%PresId, 6),0);
			//扣除活力值
			%Player.AddVigor(- %Vigor);
			//扣钱
			%Player.ReduceMoney(%Money, 1);
		}
	else
		{
				//道具无法给于，合成失败
				//可能情况：道具唯一属性；背包满；玩家处于无法获得道具的状态下
			return 4103;
		}

		return 0;
}

function Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%ItemList_Gray,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%OddsList_Gray,%Odds_ColorList,%OddsModeList)
{
	%PurpleItemListCount 	=getWordCount(%ItemList_Purple);		//紫色道具数量
	%BlueItemListCount 		= getWordCount(%ItemList_Blue);			//蓝色道具数量
	%GreenItemListCount 	= getWordCount(%ItemList_Green);		//绿色道具数量
	%WhiteItemListCount 	= getWordCount(%ItemList_White);		//白色道具数量
    %GrayItemListCount      = getWordCount(%ItemList_Gray);         //灰色道具数量

//	%PurpleOddsListCount    = getWordCount(%OddsList_Purple);	//紫色几率数量
//	%BlueOddsListCount      = getWordCount(%OddsList_Blue);		//蓝色几率数量
//	%GreenOddsListCount 	= getWordCount(%OddsList_Green);		//绿色几率数量
//	%WhiteOddsListCount 	= getWordCount(%OddsList_White);		//白色几率数量
//	%GrayOddsListCount      = getWordCount(%OddsList_Gray);		//灰色几率数量

    %GrayOdds               = getWord(%Odds_ColorList , 0);//灰色几率
	%WhiteOdds 	            = getWord(%Odds_ColorList , 1);//白色几率
	%GreenOdds 	            = getWord(%Odds_ColorList , 2);//绿色几率
	%BlueOdds 	            = getWord(%Odds_ColorList , 3);//蓝色几率
	%PurpleOdds             = getWord(%Odds_ColorList , 4);//紫色几率

	%tmpOdds 		        = %GrayOdds + %WhiteOdds + %GreenOdds + %BlueOdds + %PurpleOdds;
  //echo("几率总和:" @ %tmpOdds);

	%Odds 			        = getWord(%OddsModeList , 0);//总几率
//echo("总几率:" @ %Odds);
//	%OddsMode 	= getWord(%OddsModeList , 1);//几率计算方式
//	%CalcMode 		= getWord(%OddsModeList , 2);//计算方式
	%OddsColorMode          = getWord(%OddsModeList,1);//没几率必出的道具颜色

	//道具几率设置出现错误.
	if(%Odds < %tmpOdds)
	{
		////echo("几率设置出错.");
		return 4100;
	}

//	if(%CalcMode == 0)//由道具几率总和计算
//	{
//		if(%OddsMode == 0)//至少出现最差的
//		{
      //echo("至少出现最差的");
			//是否需要设定出现顺序?
			%rndResult = GetRandom(1,%Odds);
			////echo("随机几率:" @ %rndResult);

			//没有道具不计算几率
			if(%PurpleItemListCount > 0)
			{
				if(%rndResult >= 1 && %rndResult <= %PurpleOdds )
				{
					////echo("紫色道具随机.");
					return ProduceGetRndItem(%ItemList_Purple , %OddsList_Purple);
				}
			}

			if(%BlueItemListCount > 0)
			{
				if(%rndResult > %PurpleOdds && %rndResult <= (%PurpleOdds + %BlueOdds))
				{
					////echo("蓝色道具随机.");
					return ProduceGetRndItem(%ItemList_Blue , %OddsList_Blue);
				}
			}

			if(%GreenItemListCount > 0)
			{
				if(%rndResult > (%PurpleOdds + %BlueOdds) && %rndResult <= (%PurpleOdds + %BlueOdds + %GreenOdds))
				{
					////echo("绿色道具随机.");
					return ProduceGetRndItem(%ItemList_Green , %OddsList_Green);
				}
			}

            //echo("白色道具随机.");
            if(%WhiteItemListCount > 0)
            {
                if(%rndResult > (%PurpleOdds + %BlueOdds + %GreenOdds) && %rndResult <= (%PurpleOdds + %BlueOdds + %GreenOdds + %WhiteOdds))
                {
      		        return ProduceGetRndItem(%ItemList_White , %OddsList_White);
                }
		    }

            //灰色道具随机
            if(%GrayItemListCount > 0)
            {
                if(%rndResult > (%PurpleOdds + %BlueOdds + %GreenOdds + %WhiteOdds) <= (%PurpleOdds + %BlueOdds + %GreenOdds + %WhiteOdds + %GrayOdds))
                {
                    return ProduceGetRndItem(%ItemList_Gray , %OddsList_Gray);
                }
            }

			//没几率的情况

            if(%OddsColorMode == 1)//灰
            {
                if(%GrayItemListCount > 0)
                {
                    return ProduceGetRndItem(%ItemList_Gray , %OddsList_Gray);
                }
            }

			if(%OddsColorMode == 2)//白
			{
				if(%WhiteItemListCount > 0)
    		{
						return ProduceGetRndItem(%ItemList_White , %OddsList_White);
				}
			}



			if(%OddsColorMode == 3)//绿
			{
				if(%GreenItemListCount > 0)
				{
						return ProduceGetRndItem(%ItemList_Green , %OddsList_Green);
				}
			}


			if(%OddsColorMode == 4)//蓝
			{
				if(%BlueItemListCount > 0)
				{
					return ProduceGetRndItem(%ItemList_Blue , %OddsList_Blue);
				}
			}


			if(%OddsColorMode == 5)//紫
			{
				if(%PurpleItemListCount > 0)
				{
					return ProduceGetRndItem(%ItemList_Purple , %OddsList_Purple);
				}
			}
		//}

//		if(%OddsMode == 1)//按照道具几率计算
//		{
//			////echo("按照道具几率计算");
//		}
//	}
//
//	if(%CalcMode == 1)//按定义几率计算
//	{
//	}

	return 0;//没有几率获得任何道具
}

function ProduceGetRndItem(%ItemList , %OddsList)
{
	//若两个列表的数量不匹配，平均几率
	%tmpItemCount 	= getWordCount(%ItemList);
  //echo("道具数量:" @ %tmpItemCount);
	%tmpOddsList 		= getWordCount(%OddsList);
	//echo("几率数量:" @ %tmpOddsList);
	if(%tmpOddsList != %tmpItemCount)
	{
        //echo("数量不等，平均几率");
		if(%tmpItemCount == 0)
		{
			%tmpItemCount = %tmpItemCount;
		}
		%rndResult = GetRandom(1 , %tmpItemCount);
        //echo("几率:" @ %rndResult);
        //echo("ItemList:" @ %ItemList);
    %testResult = getWord(%ItemList , %rndResult - 1);
    //echo(%testResult);
		return getWord(%ItemList , %rndResult - 1);
	}
	else
	{
		//获取总几率
    //echo("获取几率总和.");
    %tmpOdds = 0;
		for(%i = 0 ; %i < %tmpItemCount ; %i++)
		{
			%tmpOdds = getWord(%OddsList , %i) + %tmpOdds;
		}
		//echo("几率总和为:" @ %tmpOdds);
		//取随机值
		%rndResult = GetRandom(1 , %tmpOdds);

    ////echo("几率:" @ %rndResult);
		%tmpRnd = 0;

		//查找获取随机值的道具
		for(%i = 0 ; %i < %tmpItemCount ; %i++)
		{
			%currentRnd = %tmpRnd + getWord(%OddsList , %i);
      //echo("当前几率:" @ %currentRnd);

			if(%rndResult > %tmpRnd && %rndResult <= %currentRnd)
			{
				return getWord(%ItemList , %i);
			}

			%tmpRnd = %currentRnd;
		}
	}

  //echo("没找到任何有效信息,返回空值.");

	//返回 “”错误结果.
	return 4103;
}

//执行相应配方编号的独立函数
//function Produce_630000001(%Player, %PresId)
//{
//	//影响道具出现几率的其它方法暂时没写。
//	//道具列表，空格分割
//	//紫色
//	%ItemList_Purple = "101013006 101013007 101013008";
//	//蓝色
//	%ItemList_Blue = "101012004 101012005 101012006";
//	//绿色
//	%ItemList_Green = "101011004 101011005 101011006";
//	//白色
//	%ItemList_White = "101010004 101010005 101010006";
//
//	//多道具颜色几率列表，如果几率列表数量与道具列表数量不匹配，视为平分几率。计算方式：A/(A+B+C+……)
//	//紫色
//	%OddsList_Purple = "1 2 3";
//	//蓝色
//	%OddsList_Blue = "1 2 3";
//	//绿色
//	%OddsList_Green = "1 2 3";
//	//白色
//	%OddsList_White = "1 2 3";
//
//	//道具颜色几率，空格分割，顺序为白、绿、蓝、紫
//	//道具颜色几率总和不得超过总几率
//	%Odds_ColorList = "0 100 10 1";
//
//	//计算方法设定
//	//总几率 |没几率必出的颜色
//	//总几率：
//	//没几率必出的颜色：（1、2、3、4）对应（白、绿、蓝、紫），其它任何数字都代表不考虑必出的行为。
//	%OddsModeList = "1000 1";
//
//	%ResultItem = Produce_GetItem(%ItemList_Purple,%ItemList_Blue,%ItemList_Green,%ItemList_White,%OddsList_Purple,%OddsList_Blue,%OddsList_Green,%OddsList_White,%Odds_ColorList,%OddsModeList);
//
//	return Produce_Result(%Player, %PresId, %ResultItem, 1);
//}

//********************开始:630000001配方脚本********************//
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
//********************结束:630000001配方脚本********************//

//********************开始:630202001配方脚本********************//
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
//********************结束:630202001配方脚本********************//

//********************开始:630203001配方脚本********************//
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
//********************结束:630203001配方脚本********************//

//********************开始:630204001配方脚本********************//
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
//********************结束:630204001配方脚本********************//

//********************开始:630205001配方脚本********************//
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
//********************结束:630205001配方脚本********************//

//********************开始:630206001配方脚本********************//
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
//********************结束:630206001配方脚本********************//

//********************开始:630302001配方脚本********************//
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
//********************结束:630302001配方脚本********************//

//********************开始:630303001配方脚本********************//
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
//********************结束:630303001配方脚本********************//

//********************开始:630304001配方脚本********************//
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
//********************结束:630304001配方脚本********************//

//********************开始:630305001配方脚本********************//
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
//********************结束:630305001配方脚本********************//

//********************开始:630306001配方脚本********************//
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
//********************结束:630306001配方脚本********************//

//********************开始:630102001配方脚本********************//
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
//********************结束:630102001配方脚本********************//

//********************开始:630103001配方脚本********************//
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
//********************结束:630103001配方脚本********************//

//********************开始:630104001配方脚本********************//
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
//********************结束:630104001配方脚本********************//

//********************开始:630105001配方脚本********************//
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
//********************结束:630105001配方脚本********************//

//********************开始:630106001配方脚本********************//
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
//********************结束:630106001配方脚本********************//

//********************开始:630602001配方脚本********************//
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
//********************结束:630602001配方脚本********************//

//********************开始:630603001配方脚本********************//
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
//********************结束:630603001配方脚本********************//

//********************开始:630604001配方脚本********************//
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
//********************结束:630604001配方脚本********************//

//********************开始:630605001配方脚本********************//
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
//********************结束:630605001配方脚本********************//

//********************开始:630606001配方脚本********************//
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
//********************结束:630606001配方脚本********************//

//********************开始:630702001配方脚本********************//
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
//********************结束:630702001配方脚本********************//

//********************开始:630703001配方脚本********************//
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
//********************结束:630703001配方脚本********************//

//********************开始:630704001配方脚本********************//
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
//********************结束:630704001配方脚本********************//

//********************开始:630705001配方脚本********************//
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
//********************结束:630705001配方脚本********************//

//********************开始:630706001配方脚本********************//
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
//********************结束:630706001配方脚本********************//

//********************开始:630502001配方脚本********************//
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
//********************结束:630502001配方脚本********************//

//********************开始:630503001配方脚本********************//
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
//********************结束:630503001配方脚本********************//

//********************开始:630504001配方脚本********************//
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
//********************结束:630504001配方脚本********************//

//********************开始:630505001配方脚本********************//
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
//********************结束:630505001配方脚本********************//

//********************开始:630506001配方脚本********************//
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
//********************结束:630506001配方脚本********************//

//********************开始:630402001配方脚本********************//
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
//********************结束:630402001配方脚本********************//

//********************开始:630403001配方脚本********************//
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
//********************结束:630403001配方脚本********************//

//********************开始:630404001配方脚本********************//
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
//********************结束:630404001配方脚本********************//

//********************开始:630405001配方脚本********************//
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
//********************结束:630405001配方脚本********************//

//********************开始:630406001配方脚本********************//
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
//********************结束:630406001配方脚本********************//

//********************开始:630901201配方脚本********************//
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
//********************结束:630901201配方脚本********************//

//********************开始:630902201配方脚本********************//
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
//********************结束:630902201配方脚本********************//

//********************开始:630903201配方脚本********************//
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
//********************结束:630903201配方脚本********************//

//********************开始:630904201配方脚本********************//
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
//********************结束:630904201配方脚本********************//

//********************开始:630905201配方脚本********************//
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
//********************结束:630905201配方脚本********************//

//********************开始:631201401配方脚本********************//
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
//********************结束:631201401配方脚本********************//

//********************开始:631202401配方脚本********************//
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
//********************结束:631202401配方脚本********************//

//********************开始:631203401配方脚本********************//
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
//********************结束:631203401配方脚本********************//

//********************开始:631204401配方脚本********************//
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
//********************结束:631204401配方脚本********************//

//********************开始:631205401配方脚本********************//
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
//********************结束:631205401配方脚本********************//

//********************开始:630801601配方脚本********************//
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
//********************结束:630801601配方脚本********************//

//********************开始:630802601配方脚本********************//
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
//********************结束:630802601配方脚本********************//

//********************开始:630803601配方脚本********************//
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
//********************结束:630803601配方脚本********************//

//********************开始:630804601配方脚本********************//
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
//********************结束:630804601配方脚本********************//

//********************开始:630805601配方脚本********************//
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
//********************结束:630805601配方脚本********************//

//********************开始:631401801配方脚本********************//
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
//********************结束:631401801配方脚本********************//

//********************开始:631402801配方脚本********************//
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
//********************结束:631402801配方脚本********************//

//********************开始:631403801配方脚本********************//
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
//********************结束:631403801配方脚本********************//

//********************开始:631404801配方脚本********************//
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
//********************结束:631404801配方脚本********************//

//********************开始:631405801配方脚本********************//
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
//********************结束:631405801配方脚本********************//

//********************开始:631102001配方脚本********************//
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
//********************结束:631102001配方脚本********************//

//********************开始:631103001配方脚本********************//
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
//********************结束:631103001配方脚本********************//

//********************开始:631104001配方脚本********************//
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
//********************结束:631104001配方脚本********************//

//********************开始:631105001配方脚本********************//
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
//********************结束:631105001配方脚本********************//

//********************开始:631106001配方脚本********************//
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
//********************结束:631106001配方脚本********************//

//********************开始:631302001配方脚本********************//
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
//********************结束:631302001配方脚本********************//

//********************开始:631303001配方脚本********************//
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
//********************结束:631303001配方脚本********************//

//********************开始:631304001配方脚本********************//
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
//********************结束:631304001配方脚本********************//

//********************开始:631305001配方脚本********************//
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
//********************结束:631305001配方脚本********************//

//********************开始:631306001配方脚本********************//
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
//********************结束:631306001配方脚本********************//

//********************开始:631002001配方脚本********************//
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
//********************结束:631002001配方脚本********************//

//********************开始:631003001配方脚本********************//
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
//********************结束:631003001配方脚本********************//

//********************开始:631004001配方脚本********************//
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
//********************结束:631004001配方脚本********************//

//********************开始:631005001配方脚本********************//
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
//********************结束:631005001配方脚本********************//

//********************开始:631006001配方脚本********************//
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
//********************结束:631006001配方脚本********************//

//********************开始:631502001配方脚本********************//
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
//********************结束:631502001配方脚本********************//

//********************开始:631503001配方脚本********************//
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
//********************结束:631503001配方脚本********************//

//********************开始:631504001配方脚本********************//
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
//********************结束:631504001配方脚本********************//

//********************开始:631505001配方脚本********************//
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
//********************结束:631505001配方脚本********************//

//********************开始:631506001配方脚本********************//
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
//********************结束:631506001配方脚本********************//

//********************开始:631602001配方脚本********************//
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
//********************结束:631602001配方脚本********************//

//********************开始:631603001配方脚本********************//
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
//********************结束:631603001配方脚本********************//

//********************开始:631604001配方脚本********************//
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
//********************结束:631604001配方脚本********************//

//********************开始:631605001配方脚本********************//
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
//********************结束:631605001配方脚本********************//

//********************开始:631606001配方脚本********************//
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
//********************结束:631606001配方脚本********************//

//********************开始:631702001配方脚本********************//
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
//********************结束:631702001配方脚本********************//

//********************开始:631703001配方脚本********************//
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
//********************结束:631703001配方脚本********************//

//********************开始:631704001配方脚本********************//
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
//********************结束:631704001配方脚本********************//

//********************开始:631705001配方脚本********************//
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
//********************结束:631705001配方脚本********************//

//********************开始:631706001配方脚本********************//
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
//********************结束:631706001配方脚本********************//
