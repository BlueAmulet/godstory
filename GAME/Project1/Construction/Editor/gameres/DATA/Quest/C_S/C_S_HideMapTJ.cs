//�챦����

//���񽻽������ж�
function HideMapDoorMission(%Player, %Mid, %Type, %Msg)
{
	 %TiaoJian = "";
	
	//��������������ж�
	if(%Type == 1)
		{  
			if(%Player.IsAcceptedMission(%Mid)!=0)												  {%TiaoJian = %TiaoJian @ "A";}				//�Ƿ��Ѿ����ܵ�ǰ����
			if(%Player.GetLevel() < 30 )											              {%TiaoJian = %TiaoJian @ "C";}				//�Ƿ�ﵽ������ܵȼ�
			if(%Player.GetAcceptedMission() >= 20)													{%TiaoJian = %TiaoJian @ "D";}				//�Ѿ����ܵ����������Ƿ�����
			if(%Player.GetCycleMissionTimes(%Mid) == 10)	                  {%TiaoJian = %TiaoJian @ "E";}				//�Ƿ�ʦ�����������50��
			if(%Player.GetMoney >=200)                                      {%TiaoJian = %TiaoJian @ "F";}        //�Ƿ����㹻Ǯ�۳�
		}
			echo("%TiaoJian = "@%TiaoJian); 
	 
	//��������������ж�
	if(%Type == 2)
	{
			if(!%Player.IsAcceptedMission(20002))				  										{%TiaoJian = %TiaoJian @ "A";}				//�Ƿ��Ѿ����ܵ�ǰ����
			if(%Player.IsFinishedMission(20002))															{%TiaoJian = %TiaoJian @ "B";}				//�Ƿ��Ѿ���ɵ�ǰ����
  
	  	if( (%TiaoJian !$= "")&&(%Msg > 0) )
			
			NoWayAccept(%TiaoJian, %Type, %Msg);
				
			return %TiaoJian;
  }
  		
}

function NoWayAccept(%TiaoJian, %Where, %Msg)
{
	%Txt1 = "";
	%Txt2 = "";

	for(%i = 0; %i < 99; %i++)

	{
		%Why = GetWord(%TiaoJian, %i);

		if(%Why $= "")

			break;

		else

			{
				if(%Why $= "A"){%Txt1 = "���Ѿ����ܹ�ʦ������";}
				if(%Why $= "B"){%Txt1 = "���Ѿ������ʦ������";}
				if(%Why $= "C"){%Txt1 = "��ĵȼ�δ�ﵽ30��,�޷���ȡ�챦����";}
				if(%Why $= "D"){%Txt1 = "�����Ͻ��ܵ�������������";}
				if(%Why $= "E"){%Txt1 = "������ʦ�������Ѿ�����";}
				if(%Why $= "F"){%Txt1 = "��Ľ�Ǯ����,�޷���ȴ����";}
				if(%Where == 1)

					%Txt2 = "���޷���������";

				else

					%Txt2 = "���޷���������";

				//�жϵ�ǰ�Ƿ�Ϊ�ͻ��˵��ã���ϵͳ��Ϣ
				if($Now_Script == 1)

					{

						SetScreenMessage(%Txt1 @ %Txt2, $Color[2]);

						SetChatMessage(%Txt1 @ %Txt2, $Color[2]);
					}

				//ֱ��Echo����

				if(%Msg == 2)
					
					{
						
						SendOneChatMessage(0, "<t>" @ %Txt1 @ %Txt2 @ "</t>", %Player);
						SendOneScreenMessage(2, %Txt1 @ %Txt2, %Player);
						
					}
			}
	}
}