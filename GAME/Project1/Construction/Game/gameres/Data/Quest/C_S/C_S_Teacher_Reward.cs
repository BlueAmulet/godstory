//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端与服务端共用的脚本函数，任务奖励信息
//==================================================================================

//任务结束，奖励与设置信息
function TeacherMissionReward(%Player, %Mid)
{
	%PlayerID          = %Player.GetPlayerID();
	%MissionData       = "MissionData_" @ %Mid;
	%TeacherRewardData = "TeacherRewardData_" @ %Mid;

	//扣除任务获取道具
	
	if( (%Player.GetMissionFlag(%Mid,300) == 2)||(%Player.GetMissionFlag(%Mid,300) == 4) )

		for(%i = 0; %i < 9; %i++)
		{
			%ItemGet = %Player.GetMissionFlag(%Mid, 2100 + %i);
			
			%GetNum  = %Player.GetMissionFlag(%Mid, 2200 + %i);

			if( (%ItemGet $= "")&&(%GetNum $= "") )
				
				break;
				
			else
				
				DelItemFromInventory(%PlayerID, %ItemGet, %GetNum);
		}

	//扣除接任务时给的道具
	
	if(%MissionData.ItemAdd !$= "0")
		
		for(%i = 0; %i < 9; %i++)
		
		{
			%ItemAdd    = GetWord(%MissionData.ItemAdd,    %i * 2);
			%AddNum     = GetWord(%MissionData.ItemAdd,    %i * 2 + 1);
			%ItemDelete = GetWord(%MissionData.ItemDelete, %i);

			if( (%ItemAdd $= "")&&(%AddNum $= "") )
				
				break;

			if(%ItemDelete == 1)
				
				DelItemFromInventory(%PlayerID, %ItemAdd, %AddNum);
		}

	//扣除任务使用道具
	
	if(%MissionData.ItemUse !$= "0")
		
		for(%i = 0; %i < 9; %i++)
		{
			%ItemUse = GetWord(%MissionData.ItemUse, %i * 2);
			%UseNum  = GetWord(%MissionData.ItemUse, %i * 2 + 1);

			if( (%ItemUse $= "")&&(%UseNum $= "") )
				
				break;
				
			else
				
				DelItemFromInventory(%PlayerID, %ItemUse, %UseNum);
		}

	//给经验
		%Hs = %player.GetMissionFlag(%Mid,100);   //取任务环数
		%Cs = %player.GetMissionFlag(%Mid,200);   //取任务次数
		%Zs = %player.GetCycleMissionTimes(20001); //获取任务总数
		%Lx = %player.GetMissionFlag(%Mid,300);   //获取任务类型
		%Buff = %player.GetBuffCount(32005,2);    //门宗光辉
		
		if((%Lx ==1)){%Exp = 50* %Hs* %Cs;} //计算任务类型1的奖励
		if((%Lx ==2)){%Exp = 60* %Hs* %Cs;} //计算任务类型2的奖励
		if((%Lx ==3)){%Exp = 70* %Hs* %Cs;} //计算任务类型3的奖励	
		if((%Lx ==4)){%Exp = 80* %Hs* %Cs;} //计算任务类型4的奖励
		if((%Lx ==5)){%Exp = 90* %Hs* %Cs;} //计算任务类型5的奖励
		if((%Lx ==7)){%Exp = 100* %Hs* %Cs;} //计算任务类型7的奖励		
			
		if((%Zs <= 20)||(%Buff ==1))
			
				%Player.AddExp(2 * %Exp);
				
		else if ((%Zs <= 20)&&(%Buff ==1))
			  %Player.AddExp(4 * %Exp);
			  
		else 
			  %Player.AddExp(%Exp);
				
		
	//给金钱

	//给元宝

	//给积分

	//给声望

	//给选择性道具
	
	if(%MissionRewardData.ItemChose !$= "0")
		
		for(%i = 0; %i < 9; %i++)
		
		{
			%ItemChose = GetWord(%MissionRewardData.ItemChose, %i * 2);
			%ChoseNum  = GetWord(%MissionRewardData.ItemChose, %i * 2 + 1);

			if( (%ItemChose $= "")&&(%ChoseNum $= "") )
				
				break;
				
			else
				
				AddItemToInventory(%PlayerID, %ItemChose, %ChoseNum);
		}

	//必给的道具
	
 		%Zd = %Player.GetCycleMissionTimes(%Mid);
 		
 		if(%Zd ==10)
 			{
 				%ItemAdd = %Player.PutItem(105010001,10);//肉包子
				%ItemAdd = %Player.AddItem();
				if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>背包满了</t>",%Player);
						SendOneScreenMessage(2,"背包满了", %Player);
							
						return;
				 }
 			}
 			
 		if(%Zd ==20)
 			{
 				%ItemAdd = %Player.PutItem(105012111,3);//力道酒
				%ItemAdd = %Player.AddItem();
				if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>背包满了</t>",%Player);
						SendOneScreenMessage(2,"背包满了", %Player);
							
						return;
				 }
 			}
 			
 		if(%Zd ==30)
 			{
 				%ItemAdd = %Player.PutItem(105012122,3);//体魄酒
				%ItemAdd = %Player.AddItem();
				if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>背包满了</t>",%Player);
						SendOneScreenMessage(2,"背包满了", %Player);
							
						return;
				 }
 			}
 			
 		if(%Zd ==40)
 			{
 				%ItemAdd = %Player.PutItem(105020104,5);//门宗令
				%ItemAdd = %Player.AddItem();
				if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>背包满了</t>",%Player);
						SendOneScreenMessage(2,"背包满了", %Player);
							
						return;
				 }
 			}
 		
 		if(%Zd ==50)
 		  {
 		  	 %ItemAdd = %Player.PutItem(108020263,1);//门宗令
				 %ItemAdd = %Player.AddItem();
					if(!%ItemAdd)
						{
							SendOneChatMessage(0,"<t>背包满了</t>",%Player);
							SendOneScreenMessage(2,"背包满了", %Player);
							
							return;
						}
 		  }	 

	//给技能
	
	if(%MissionRewardData.Skill !$= "0")
		
		for(%i = 0; %i < 9; %i++)
		{
			%Skill = GetWord(%MissionRewardData.Skill, %i);

			if(%Skill $= "")
				
				break;
				
			else
				%Player.AddSkill(%Skill);
		}

	//给状态
	
	//给特效
	
	%Player.AddPacket( $SP_Effect[ 2 ] );
	
	//设置任务完成
	
	%Hs = %Player.GetMissionFlag(%Mid,100); //取任务环数旗标
	%Cs = %Player.GetMissionFlag(%Mid,200); //取任务次数旗标
  %Player.DelMission(%Mid);
	%Player.SetFlagsByte(1,%Hs);//设置任务次数旗标
	%Player.SetFlagsByte(2,%Cs); //设置任务环数旗标


		
}

//添加任务奖励数据

function AddTeacherRewardData(%Mid,%NanDu,%JiFen, %Exp, %Money, %Yuan, %Gold, %ShengWang, %ItemChose, %ItemSet, %Skill, %Buff)
{
	//拼装任务数据编号
	
	%DataID = "TeacherRewardData_" @ %Mid;

	//创建任务数据
	
	new scriptObject(%DataID)
		{
			NanDu       =  %NanDu;      	//难度
			JiFen       =  %JiFen;      	//积分
			Exp         =  %Exp;        	//经验
			Money       =  %Money;      	//金钱
			Yuan        =  %Yuan;      		//金元
			Gold        =  %Gold;       	//元宝
			ShengWang   =  %ShengWang;  	//声望
			ItemChose   =  %ItemChose;  	//选择性道具
			ItemSet     =  %ItemSet;    	//必给道具
			Skill       =  %Skill;      	//技能
			Buff        =  %Buff;       	//状态
		};
}

//创建任务奖励数据

AddTeacherRewardData(20001,0,0,50,100,0,0,0,0,0,0,0);

//创建任务数据，格式参看《神仙传》任务设计模板.xlsx

//AddMissionData(20001,0,"师门尽忠",0,20,20,0,0,0,0,0,0,0,0,0,0);
AddMissionData(20001,0,"师门尽忠",1001,"10 10 10 10 100",0,0,0,0,0,0,0,0,0,0);