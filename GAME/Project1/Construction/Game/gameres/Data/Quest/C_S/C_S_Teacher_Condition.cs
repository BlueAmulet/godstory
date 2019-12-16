//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//客户端与服务端共用的脚本函数，任务交接条件判断
//==================================================================================


//任务交接条件判断
function TeacherDoorMission(%Player, %Mid, %Type, %Msg)
{

	 %TiaoJian    = "";
	 
	  %Hs = %Player.GetMissionFlag(%Mid,100); //取任务次数
		%Cs = %Player.GetMissionFlag(%Mid,200); //取任务次数
  	%Lx = %Player.GetMissionFlag(%Mid,300); //取任务类型
    %SmBuff = %Player.GetBuffCount(32005,1); //师门冷却
	//接受任务的条件判断
	if(%Type == 1)
		{  
			if(%Player.GetMissionFlag(%Mid,300)!=0)												{%TiaoJian = %TiaoJian @ "A";}				//是否已经接受当前任务
			if(%Player.GetLevel() < 20 )											              {%TiaoJian = %TiaoJian @ "C";}				//是否达到任务接受等级
			if(%Player.GetAcceptedMission() >= 20)													{%TiaoJian = %TiaoJian @ "F";}				//已经接受的任务数量是否已满
			if(%Player.GetCycleMissionTimes(%Mid) == 50)	                  {%TiaoJian = %TiaoJian @ "G";}				//是否师门任务已完成50个
	    if(%SmBuff ==1)                                                 {%TiaoJian = %TiaoJian @ "H";}        //是否处于师门冷却
	                                                                                                        	//是否门派归属正确
		}
			echo("%TiaoJian = "@%TiaoJian); 
	 
	//交付任务的条件判断
	if(%Type == 2)
		{
			if(!%Player.IsAcceptedMission(20001))				  										{%TiaoJian = %TiaoJian @ "A";}				//是否已经接受当前任务
			if(%Player.IsFinishedMission(20001))															{%TiaoJian = %TiaoJian @ "B";}				//是否已经完成当前任务
  
			%Lv = %Player.GetLevel();
			%Lx = %Player.GetMissionFlag(%Mid,300);
			
		
			if(((%Lv >= 20)&&(%Lv < 40))&&(%Lx ==1)){%tt = 1;}
			if(((%Lv >= 20)&&(%Lv < 40))&&(%Lx ==2)){%tt = 2;}
			if(((%Lv >= 20)&&(%Lv < 40))&&(%Lx ==3)){%tt = 1;}
			if(((%Lv >= 20)&&(%Lv < 40))&&(%Lx ==4)){%tt = 1;}			
				
			if(((%Lv >= 40)&&(%Lv < 60))&&(%Lx ==1)){%tt = 2;}
			if(((%Lv >= 40)&&(%Lv < 60))&&(%Lx ==2)){%tt = 4;}
			if(((%Lv >= 40)&&(%Lv < 60))&&(%Lx ==3)){%tt = 2;}
			if(((%Lv >= 40)&&(%Lv < 60))&&(%Lx ==4)){%tt = 1;}
				
			if(((%Lv >= 60)&&(%Lv < 80))&&(%Lx ==1)){%tt = 3;}
			if(((%Lv >= 60)&&(%Lv < 80))&&(%Lx ==2)){%tt = 4;}
			if(((%Lv >= 60)&&(%Lv < 80))&&(%Lx ==3)){%tt = 2;}
			if(((%Lv >= 60)&&(%Lv < 80))&&(%Lx ==4)){%tt = 1;} 	
			
			  
	    	for(%i = 0; %i < %tt; %i++)
				{ 
	
				   	 if((%Lx == 1)||(%Lx == 2)||(%Lx == 3))
	           {
				   			if(!%Player.GetMissionFlag(%Mid,1350 + %i) == 1)
	
				   			{
				   				%TiaoJian = %TiaoJian @ "E ";
	
				   				break;
	
				   			}
				   	}
				   		
				   	if((%Lx == 4) ||(%Lx == 6))
				   	{	
				   		if(!%Player.GetMissionFlag(%Mid,3300 + %i) == 1)
	
				   			{
				   				%TiaoJian = %TiaoJian @ "E ";
	
				   				break;
	
				   			}
				   	}	
				   	
				   	if(%Lx == 5)
				   	{	
				   		  
				   		  for(%i = 0; %i < 9; %i++)
				   		 {
									%ItemID = %Player.GetMissionFlag(%Mid,2100+ %i);
									
									%ItemNum = %Player.GetMissionFlag(%Mid,2200 + %i);
									
					   			if(%Player.GetItemCount(%ItemID) < %ItemNum)
					   				
					   			{
					   				%TiaoJian = %TiaoJian @ "E ";
					   				
					   				break;
		
		  			   		}
		  			   }	
		  		  }	
		  		  
		  		  
		  		  
	   		}
   	
  	}
  	
  //发送无法接受或完成的原因消息
  if( (%TiaoJian !$= "")&&(%Msg > 0) )
  	 {
  	 	 	 echo("任务条件");
				 NoWayAccept(%Player,%TiaoJian, %Type, %Msg);
				 
				 echo("%Player="@%Player);
				 echo("%TiaoJian="@%TiaoJian);
				 echo("%Type="@%Type);
				 echo("%Msg="@%Msg);
		 }
				
				
	
		return %TiaoJian;
}

function NoWayAccept(%Player,%TiaoJian, %Type, %Msg)
{
	%Txt1 = "";
	%Txt2 = "";
	
	echo("发送消息");
	
//判断是否要发送消息
	if(%Msg == 1)
	  for(%i = 0; %i < 99; %i++)
		 {
				%Why = GetWord(%TiaoJian, %i);
		
				if(%Why $= "")
					break;
				else
					{
						if(%Why $= "A"){%Txt1 = "你已经接受过师门任务";}
						if(%Why $= "B"){%Txt1 = "你已经完成了师门任务";}
						if(%Why $= "C"){%Txt1 = "你的等级未达到20级,无法领取师门任务";}
						if(%Why $= "D"){%Txt1 = "你的师门任务已超时";}
						if(%Why $= "E"){%Txt1 = "你的师门任务尚未完成.";}
						if(%Why $= "F"){%Txt1 = "你身上接受的任务数量已满";}
						if(%Why $= "G"){%Txt1 = "你今天的师门任务已经做完";}
						if(%Why $= "H"){%Txt1 = "你的师门还处于冷却中,请稍等片刻";}
							
						if(%Where == 1)
		
							%Txt2 = "，无法接受任务";
		
						else
		
							%Txt2 = "，无法交付任务";
							
							
						%Txt3 = %Txt1 @ %Txt2;
					  %Txt4 = "<t>" @ %Txt1 @ %Txt2 @ "</t>";
		
						//判断当前是否为客户端调用，发系统消息
						if($Now_Script == 1)
		
							{
		
								SetScreenMessage(%Txt3, $Color[2]);
								SetChatMessage(%Txt3, $Color[2]);
							}
		
						//直接Echo出来
		
						if(%Msg == 2)
							
							{
								
								SendOneScreenMessage(2, %Txt3, %Player);
								SendOneChatMessage(0, %Txt4, %Player);
								
							}
					 }
	     }
}