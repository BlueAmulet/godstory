//异宝觅踪

//任务交接条件判断
function HideMapDoorMission(%Player, %Mid, %Type, %Msg)
{
	 %TiaoJian = "";
	
	//接受任务的条件判断
	if(%Type == 1)
		{  
			if(%Player.IsAcceptedMission(%Mid)!=0)												  {%TiaoJian = %TiaoJian @ "A";}				//是否已经接受当前任务
			if(%Player.GetLevel() < 30 )											              {%TiaoJian = %TiaoJian @ "C";}				//是否达到任务接受等级
			if(%Player.GetAcceptedMission() >= 20)													{%TiaoJian = %TiaoJian @ "D";}				//已经接受的任务数量是否已满
			if(%Player.GetCycleMissionTimes(%Mid) == 10)	                  {%TiaoJian = %TiaoJian @ "E";}				//是否师门任务已完成50个
			if(%Player.GetMoney >=200)                                      {%TiaoJian = %TiaoJian @ "F";}        //是否有足够钱扣除
		}
			echo("%TiaoJian = "@%TiaoJian); 
	 
	//交付任务的条件判断
	if(%Type == 2)
	{
			if(!%Player.IsAcceptedMission(20002))				  										{%TiaoJian = %TiaoJian @ "A";}				//是否已经接受当前任务
			if(%Player.IsFinishedMission(20002))															{%TiaoJian = %TiaoJian @ "B";}				//是否已经完成当前任务
  
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
				if(%Why $= "A"){%Txt1 = "你已经接受过师门任务";}
				if(%Why $= "B"){%Txt1 = "你已经完成了师门任务";}
				if(%Why $= "C"){%Txt1 = "你的等级未达到30级,无法领取异宝觅踪";}
				if(%Why $= "D"){%Txt1 = "你身上接受的任务数量已满";}
				if(%Why $= "E"){%Txt1 = "你今天的师门任务已经做完";}
				if(%Why $= "F"){%Txt1 = "你的金钱不足,无法接却任务";}
				if(%Where == 1)

					%Txt2 = "，无法接受任务";

				else

					%Txt2 = "，无法交付任务";

				//判断当前是否为客户端调用，发系统消息
				if($Now_Script == 1)

					{

						SetScreenMessage(%Txt1 @ %Txt2, $Color[2]);

						SetChatMessage(%Txt1 @ %Txt2, $Color[2]);
					}

				//直接Echo出来

				if(%Msg == 2)
					
					{
						
						SendOneChatMessage(0, "<t>" @ %Txt1 @ %Txt2 @ "</t>", %Player);
						SendOneScreenMessage(2, %Txt1 @ %Txt2, %Player);
						
					}
			}
	}
}