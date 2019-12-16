//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端所有任务对话，依照规则格式填写
//==================================================================================

//任务，模板对话
//function MissionXXXXXDialog(%Npc, %Player, %Mid, %Tid)
//{
//	switch(%Tid)
//	{
//		case 10001:	return "任务目标</t></t><b/>";
//		case 10002:	return "任务描述</t></t><b/>";
//		case 10005:	return "任务完成对话</t></t><b/>";
//
//		case 10009:	return "接受任务条件不满足时对话</t></t><b/>";
//		case 20009: return "任务交付条件不满足时对话</t></t><b/>";
//	}
//	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
//}
//
//function MissionXXXXXDialog(%Npc, %Player, %Mid, %Tid)
//{
//	switch(%Tid)
//	{
//		case 10001:	return "<t>0000000</t><b/>";
//		case 10002:	return "<t>1111111</t><b/>" @
//											 "<t>2222222</t><b/>";
//		case 10005:	return "<t>3333333</t><b/>";
//	}
//	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";
//}

//对话太多，现在按照任务分类：

//【门宗任务】

//【章回任务】
//第一编号为【卷】，每一张地图为一卷，每一卷的任务编号间隔数为200，第二编号段为【章】，每一卷暂定为三章，每一章的任务编号间隔数为40，附章间隔为20，最后多余20个编号。
//目前编号段规划，最大可至24卷，24*3=72章，即可以满足72张地图所需的章回任务，视后期任务需求，可挪用一部分的支线任务编号段
//Exec("./C_Dialog_Mission_ZhangHui_00.cs");		//序章
Exec("./C_Dialog_Mission_ZhangHui_01.cs");		//卷1
Exec("./C_Dialog_Mission_ZhangHui_02.cs");		//卷2
Exec("./C_Dialog_Mission_ZhangHui_03.cs");		//卷3
Exec("./C_Dialog_Mission_ZhangHui_04.cs");		//卷4
Exec("./C_Dialog_Mission_ZhangHui_05.cs");		//卷5
//Exec("./Client/C_Dialog_Mission_ZhangHui_06.cs");		//卷6
//Exec("./Client/C_Dialog_Mission_ZhangHui_07.cs");		//卷7
//Exec("./Client/C_Dialog_Mission_ZhangHui_08.cs");		//卷8
//Exec("./Client/C_Dialog_Mission_ZhangHui_09.cs");		//卷9
//Exec("./Client/C_Dialog_Mission_ZhangHui_10.cs");		//卷10
//Exec("./Client/C_Dialog_Mission_ZhangHui_11.cs");		//卷11
//Exec("./Client/C_Dialog_Mission_ZhangHui_12.cs");		//卷12
Exec("./C_Dialog_Mission_ZhiXian_01.cs");		//支线卷1
//Exec("./C_Dialog_Mission_ZhiXian_02.cs");		//支线卷2
//Exec("./C_Dialog_Mission_ZhiXian_03.cs");		//支线卷3
//【支线剧情】

//【每日循环】

//【帮会任务】

//【指引任务】
Exec("./C_Dialog_Mission_ZhiYing.cs");

//【其它任务】

