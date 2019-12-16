//==================================================================================
//ע�����нű���ͷ������ע�����ű��Ĵ���˵������
//
//
//�ͻ��������˹��õĽű���������������Ϣ
//==================================================================================

//���������������������Ϣ
function TeacherMissionReward(%Player, %Mid)
{
	%PlayerID          = %Player.GetPlayerID();
	%MissionData       = "MissionData_" @ %Mid;
	%TeacherRewardData = "TeacherRewardData_" @ %Mid;

	//�۳������ȡ����
	
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

	//�۳�������ʱ���ĵ���
	
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

	//�۳�����ʹ�õ���
	
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

	//������
		%Hs = %player.GetMissionFlag(%Mid,100);   //ȡ������
		%Cs = %player.GetMissionFlag(%Mid,200);   //ȡ�������
		%Zs = %player.GetCycleMissionTimes(20001); //��ȡ��������
		%Lx = %player.GetMissionFlag(%Mid,300);   //��ȡ��������
		%Buff = %player.GetBuffCount(32005,2);    //���ڹ��
		
		if((%Lx ==1)){%Exp = 50* %Hs* %Cs;} //������������1�Ľ���
		if((%Lx ==2)){%Exp = 60* %Hs* %Cs;} //������������2�Ľ���
		if((%Lx ==3)){%Exp = 70* %Hs* %Cs;} //������������3�Ľ���	
		if((%Lx ==4)){%Exp = 80* %Hs* %Cs;} //������������4�Ľ���
		if((%Lx ==5)){%Exp = 90* %Hs* %Cs;} //������������5�Ľ���
		if((%Lx ==7)){%Exp = 100* %Hs* %Cs;} //������������7�Ľ���		
			
		if((%Zs <= 20)||(%Buff ==1))
			
				%Player.AddExp(2 * %Exp);
				
		else if ((%Zs <= 20)&&(%Buff ==1))
			  %Player.AddExp(4 * %Exp);
			  
		else 
			  %Player.AddExp(%Exp);
				
		
	//����Ǯ

	//��Ԫ��

	//������

	//������

	//��ѡ���Ե���
	
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

	//�ظ��ĵ���
	
 		%Zd = %Player.GetCycleMissionTimes(%Mid);
 		
 		if(%Zd ==10)
 			{
 				%ItemAdd = %Player.PutItem(105010001,10);//�����
				%ItemAdd = %Player.AddItem();
				if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>��������</t>",%Player);
						SendOneScreenMessage(2,"��������", %Player);
							
						return;
				 }
 			}
 			
 		if(%Zd ==20)
 			{
 				%ItemAdd = %Player.PutItem(105012111,3);//������
				%ItemAdd = %Player.AddItem();
				if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>��������</t>",%Player);
						SendOneScreenMessage(2,"��������", %Player);
							
						return;
				 }
 			}
 			
 		if(%Zd ==30)
 			{
 				%ItemAdd = %Player.PutItem(105012122,3);//���Ǿ�
				%ItemAdd = %Player.AddItem();
				if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>��������</t>",%Player);
						SendOneScreenMessage(2,"��������", %Player);
							
						return;
				 }
 			}
 			
 		if(%Zd ==40)
 			{
 				%ItemAdd = %Player.PutItem(105020104,5);//������
				%ItemAdd = %Player.AddItem();
				if(!%ItemAdd)
					{
						SendOneChatMessage(0,"<t>��������</t>",%Player);
						SendOneScreenMessage(2,"��������", %Player);
							
						return;
				 }
 			}
 		
 		if(%Zd ==50)
 		  {
 		  	 %ItemAdd = %Player.PutItem(108020263,1);//������
				 %ItemAdd = %Player.AddItem();
					if(!%ItemAdd)
						{
							SendOneChatMessage(0,"<t>��������</t>",%Player);
							SendOneScreenMessage(2,"��������", %Player);
							
							return;
						}
 		  }	 

	//������
	
	if(%MissionRewardData.Skill !$= "0")
		
		for(%i = 0; %i < 9; %i++)
		{
			%Skill = GetWord(%MissionRewardData.Skill, %i);

			if(%Skill $= "")
				
				break;
				
			else
				%Player.AddSkill(%Skill);
		}

	//��״̬
	
	//����Ч
	
	%Player.AddPacket( $SP_Effect[ 2 ] );
	
	//�����������
	
	%Hs = %Player.GetMissionFlag(%Mid,100); //ȡ���������
	%Cs = %Player.GetMissionFlag(%Mid,200); //ȡ����������
  %Player.DelMission(%Mid);
	%Player.SetFlagsByte(1,%Hs);//��������������
	%Player.SetFlagsByte(2,%Cs); //�������������


		
}

//�������������

function AddTeacherRewardData(%Mid,%NanDu,%JiFen, %Exp, %Money, %Yuan, %Gold, %ShengWang, %ItemChose, %ItemSet, %Skill, %Buff)
{
	//ƴװ�������ݱ��
	
	%DataID = "TeacherRewardData_" @ %Mid;

	//������������
	
	new scriptObject(%DataID)
		{
			NanDu       =  %NanDu;      	//�Ѷ�
			JiFen       =  %JiFen;      	//����
			Exp         =  %Exp;        	//����
			Money       =  %Money;      	//��Ǯ
			Yuan        =  %Yuan;      		//��Ԫ
			Gold        =  %Gold;       	//Ԫ��
			ShengWang   =  %ShengWang;  	//����
			ItemChose   =  %ItemChose;  	//ѡ���Ե���
			ItemSet     =  %ItemSet;    	//�ظ�����
			Skill       =  %Skill;      	//����
			Buff        =  %Buff;       	//״̬
		};
}

//��������������

AddTeacherRewardData(20001,0,0,50,100,0,0,0,0,0,0,0);

//�����������ݣ���ʽ�ο������ɴ����������ģ��.xlsx

//AddMissionData(20001,0,"ʦ�ž���",0,20,20,0,0,0,0,0,0,0,0,0,0);
AddMissionData(20001,0,"ʦ�ž���",1001,"10 10 10 10 100",0,0,0,0,0,0,0,0,0,0);