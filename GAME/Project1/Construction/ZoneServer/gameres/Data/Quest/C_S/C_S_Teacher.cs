function Mission20001Dialog(%Npc, %Player, %Mid, %Tid)
{	
		%Hs = %Player.GetMissionFlag(%Mid,100); //获取任务旗标环数
		%Cs = %Player.GetMissionFlag(%Mid,200); //获取任务旗标次数
		%Lx = %Player.GetMissionFlag(%Mid,300); //获取任务类型
		%Zs = %Player.GetMissionFlag(%Mid,310); //获取任务总数
		%Buff = %Player.GetBuffCount(32005,2);   //获取门宗光辉
		%Mz = %Player.Getfamily();               //获取玩家职业
		%Lv = %Player.GetLevel();                //获取玩家等级
		
	  if(%Player.GetMissionFlag(%Mid,300) == 1)//送信
	  	{
				%SongXinID1 = %Player.GetMissionFlag(%Mid,1150);
				%SongXinID2 = %Player.GetMissionFlag(%Mid,1151);
				if(%SongXinID1 > 0){%SongXin1 ="【"@GetNpcData(%SongXinID1,1)@"】";} 
				if(%SongXinID2 > 0){%SongXin2 ="【"@GetNpcData(%SongXinID2,1)@"】";}
				%SongXin = %SongXin1@%SongXin2;
				
				%Exp = 50* %Hs* %Cs;
				%Sw = 5*%Hs* %Cs;
			  %tt = 111;
		  }

		if(%Player.GetMissionFlag(%Mid,300) == 2)//培育
			{
				%PeiyuID1 = %Player.GetMissionFlag(%Mid,2100);
				%PeiyuID2 = %Player.GetMissionFlag(%Mid,2101);
				if(%PeiyuID1 > 0){%Peiyu1 ="【"@GetItemData(%PeiyuID1,1)@"】";} 
				if(%PeiyuID2 > 0){%Peiyu2 ="【"@GetItemData(%PeiyuID2,1)@"】";}
				%Peiyu = %Peiyu1@%Peiyu2;
				
				%Exp = 60* %Hs* %Cs;
				%Sw = 6*%Hs* %Cs;
				%tt = 112;
			}
			
		if(%Player.GetMissionFlag(%Mid,300) == 3)//巡逻
			{
				%XunluoID1 = %Player.GetMissionFlag(%Mid,1150);
				%XunluoID2 = %Player.GetMissionFlag(%Mid,1151);
				if(%XunluoID1 > 0){%Xunluo1 ="【"@GetNpcData(%XunluoID1,1)@"】";} 
				if(%XunluoID2 > 0){%Xunluo2 ="【"@GetNpcData(%XunluoID2,1)@"】";}
				%Xunluo = %Xunluo1@%Xunluo2;
				
				%Exp = 70* %Hs* %Cs;
				%Sw = 7*%Hs* %Cs;
				%tt = 113;
			}
		if(%Player.GetMissionFlag(%Mid,300) == 4)	//购物
			{
				%GouWuID1 = %Player.GetMissionFlag(%Mid,2100);
				%GouWuID2 = %Player.GetMissionFlag(%Mid,2101);
				if(%GouWuID1 > 0){%GouWu1 ="【"@GetItemData(%GouWuID1,1)@"】";} 
				if(%GouWuID2 > 0){%GouWu2 ="【"@GetItemData(%GouWuID2,1)@"】";}
				%GouWu = %GouWu1@%GouWu2;
				%Exp = 80* %Hs* %Cs;
				%Sw = 8*%Hs* %Cs;
				%tt = 114;
			}
			
		if(%Player.GetMissionFlag(%Mid,300) == 5)	//击杀
			{
				%JiShaID1 = %Player.GetMissionFlag(%Mid,3100);
				%JiShaID2 = %Player.GetMissionFlag(%Mid,3101);
				if(%JiShaID1 > 0){%JiSha1 ="【"@GetNpcData(%JiShaID1,1)@"】";} 
				if(%JiShaID2 > 0){%JiSha2 ="【"@GetNpcData(%JiShaID2,1)@"】";}
				%JiSha = %JiSha1@%JiSha2;
				
				%Exp = 90* %Hs* %Cs;
				%Sw = 8*%Hs* %Cs;
				%tt = 115;
			}
			
		if(%Player.GetMissionFlag(%Mid,300) == 7)	//捕兽
			{
				%BuShouID1 = %Player.GetMissionFlag(%Mid,2100);
				%BuShouID2 = %Player.GetMissionFlag(%Mid,2101);
				if(%BuShouID1 > 0){%BuShou1 ="【"@GetItemData(%BuShouID1,1)@"】";} 
				if(%BuShouID2 > 0){%BuShou2 ="【"@GetItemData(%BuShouID2,1)@"】";}
				%BuShou = %BuShou1@%BuShou2;
				%Exp = 100* %Hs* %Cs;
				%Sw = 8*%Hs* %Cs; 
				%tt = 117;
			}
	
	  if((%Zs <= 20)||(%Buff==1)){%u = "此次任务为双倍经验";} //显示双倍经验提示
	    	
		if ((%Zs <= 20)&&(%Lx == 1)){%Tmoney = 2 * 50 * %Cs;}
		if ((%Zs <= 20)&&(%Lx == 2)){%Tmoney = 2 * 60 * %Cs;}
		if ((%Zs <= 20)&&(%Lx == 3)){%Tmoney = 2 * 70 * %Cs;}
		if ((%Zs <= 20)&&(%Lx == 4)){%Tmoney = 2 * 80 * %Cs;}
		if ((%Zs <= 20)&&(%Lx == 5)){%Tmoney = 2 * 80 * %Cs;}
		if ((%Zs <= 20)&&(%Lx == 7)){%Tmoney = 2 * 80 * %Cs;}
					
		if ((%Zs > 20)&&(%Lx == 1)){%Tmoney = 50 * %Cs;}
		if ((%Zs > 20)&&(%Lx == 2)){%Tmoney = 60 * %Cs;}
		if ((%Zs > 20)&&(%Lx == 3)){%Tmoney = 70 * %Cs;}
		if ((%Zs > 20)&&(%Lx == 4)){%Tmoney = 80 * %Cs;}
		if ((%Zs > 20)&&(%Lx == 5)){%Tmoney = 90 * %Cs;}
		if ((%Zs > 20)&&(%Lx == 7)){%Tmoney = 100 * %Cs;}
								
		%TPngA = isFile("gameres/gui/images/GUIWindow25_1_005.png");//任务描述
		%TPngB = isFile("gameres/gui/images/GUIWindow25_1_006.png");//任务目标
		%TPngC = isFile("gameres/gui/images/GUIWindow25_1_007.png");//任务奖励
		%TPngD = isFile("gameres/gui/images/GUIWindow25_1_008.png");//任务完成
		%TPngE = isFile("gameres/gui/images/GUIWindow25_1_009.png");//任务难度
	
		if(%TPngA == 1){%TPng_A = "<i s='gameres/gui/images/GUIWindow25_1_005.png' w='16' h='16'/>";}else{%TPng_A = "任务描述</t>";}
		if(%TPngB == 1){%TPng_B = "<i s='gameres/gui/images/GUIWindow25_1_006.png' w='16' h='16'/>";}else{%TPng_B = "任务目标</t>";}
		if(%TPngC == 1){%TPng_C = "<i s='gameres/gui/images/GUIWindow25_1_007.png' w='16' h='16'/>";}else{%TPng_C = "任务奖励</t>";}
		if(%TPngD == 1){%TPng_D = "<i s='gameres/gui/images/GUIWindow25_1_008.png' w='16' h='16'/>";}else{%TPng_D = "任务完成</t>";}
		if(%TPngE == 1){%TPng_E = "<i s='gameres/gui/images/GUIWindow25_1_009.png' w='16' h='16'/>";}else{%TPng_E = "任务难度 </t>";}

	 switch(%Tid)
	 {
	  case 10002: return %TPng_A @ "<b/>"@ //任务描述
	  	                 		"<t>你目前的师门做到了第"@%Hs@"环第"@%Cs@"次。</t><b/>"@
	  	                 		 $Get_Dialog_GeShi[31206] @%u@"</t><b/><b/>";
	  	           
		case 101:	return 	%TPng_A @"<b/>"@	//送信-接任务描述
													"<t>你去捎个口信给</t>"@%SongXin@"<t>。</t><b/>"@
										  		"<t>你目前的师门做到了第"@%Hs@"环第"@%Cs@"次。</t><b/>"@
										  		$Get_Dialog_GeShi[31206] @%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										 		 	$Get_Dialog_GeShi[31203]@"经验： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203]@"声望： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			               			
	  case 102: return 	%TPng_A @"<b/>"@	//培育-接任务描述
										  		"<t>你去采集一些</t>"@%Peiyu@"<t>来。</t><b/>"@
										 			"<t>你目前的师门做到了第"@%Hs@"环第"@%Cs@"次。</t><b/>"@
										 			$Get_Dialog_GeShi[31206]@%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										  		$Get_Dialog_GeShi[31203] @"经验： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203] @"声望： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			                
		case 103:	return  %TPng_A @"<b/>"@	//巡逻-接任务描述
										 			"<t>你去找</t>"@%Xunluo@"<t>地方看看是否安全。</t><b/>"@
										  		"<t>你目前的师门做到了第"@%Hs@"环第"@%Cs@"次。</t><b/>"@
										  		$Get_Dialog_GeShi[31206] @%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										  		$Get_Dialog_GeShi[31203] @"经验： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203] @"声望： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			               			
		case 104: return  %TPng_A @"<b/>"@	//购物-接任务描述
										  		"<t>你去买一些</t>"@%GouWu@"<t>来。</t><b/>"@
										  		"<t>你目前的师门做到了第"@%Hs@"环第"@%Cs@"次。</t><b/>"@
										  		$Get_Dialog_GeShi[31206]@%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										  		$Get_Dialog_GeShi[31203] @ "经验： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203] @ "声望： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
		
		case 105: return  %TPng_A @"<b/>"@	//击杀-接任务描述
										  		"<t>你去杀死一些</t>"@%JiSha@"<t>看看。</t><b/>"@
										  		"<t>你目前的师门做到了第"@%Hs@"环第"@%Cs@"次。</t><b/>"@
										  		$Get_Dialog_GeShi[31206]@%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										  		$Get_Dialog_GeShi[31203] @ "经验： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203] @ "声望： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			               			
	 case 107: return  %TPng_A @"<b/>"@	//捕兽-接任务描述
										  		"<t>你去抓一只</t>"@%BuShou@"<t>给我。</t><b/>"@
										  		"<t>你目前的师门做到了第"@%Hs@"环第"@%Cs@"次。</t><b/>"@
										  		$Get_Dialog_GeShi[31206]@%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										  		$Get_Dialog_GeShi[31203] @ "经验： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203] @ "声望： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			               			      	
	  case 111: return 	%TPng_B @"<b/>"@	//送信-任务目标
		                  	"<t>你去捎个口信给</t>"@%SongXin@"<t>。</t><b/>";
		                  	
	  case 112: return	%TPng_B @"<b/>"@	//培育-任务目标
		                  	"<t>你去采集一些</t>"@%Peiyu@"<t>来。</t><b/>";	                  	
		                  
	  case 113: return	%TPng_B @"<b/>"@	//巡逻-任务目标
	  								 		"<t>你去找</t>"@%Xunluo@"<t>地方看看是否安全。</t><b/>";
	  								 
	  case 114: return	%TPng_B @"<b/>"@	//购物-任务目标
		                  		"<t>你去买一些</t>"@%GouWu@"<t>来。</t><b/>";
		                  
		case 115: return  %TPng_B @"<b/>"@	//击杀-任务目标
		                  "<t>你去杀死一些</t>"@%JiSha@"<t>看看。</t><b/>";
		                  
	  case 117: return  %TPng_B @"<b/>"@	//击杀-任务目标
		                  "<t>你去抓一只</t>"@%BuShou@"<t>给我。</t><b/>";
	
	  case 535: return %TPng_D @"<b/>"@   //任务完成
	  	               "<t>好的,我知道了。</t>";
	  	
		case 555: return %TPng_D @"<b/>"@   //任务完成
										    "<t>不错，不错，这么快就完成。</t><b/><b/>"@
										 %TPng_C @ "<b/>"@ 
									      $Get_Dialog_GeShi[31203] @ "经验： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@																																																																																																													
			                  $Get_Dialog_GeShi[31203] @ "声望： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";	    
		case 999:
										if(%Zs !=50)
		                  {
		                     return "</t>" @ %TPng_C @ "<b/>"@ //任务奖励
			               		 $Get_Dialog_GeShi[31203] @"经验： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               		 $Get_Dialog_GeShi[31203] @ "声望： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			               	} 
			             if(%Zs ==50)
			               {	
			               		return "</t>" @ %TPng_C @ "<b/>"@ //任务奖励
					             $Get_Dialog_GeShi[31203] @ "经验： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
					             $Get_Dialog_GeShi[31203] @ "声望： </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>"@	
					             $Get_Dialog_GeShi[31203] @ "固定任务奖励： </t><b/>"@ GetFixedMissionItemText();
									   }
									   
		//任务追踪处的热感文字，任务目标+任务奖励
		
		case 800: return $Get_Dialog_GeShi[31201] @ Mission20001Dialog(%Npc, %Player, %Mid, %tt) @ GetMissionNeedText(%Player, %Mid, 9999) @Mission20001Dialog(%Npc, %Player, %Mid, 999) @ "</t>";
		
	//任务难度+目标+需求+描述+奖励,任务界面内使用	
	
		case 888: return $Get_Dialog_GeShi[31201] @ Get_Mis_Dialog(%Npc, %Player, %Mid, 3) @
										 Mission20001Dialog(%Npc, %Player, %Mid, %tt)@"<b/>"@
										 GetMissionNeedText(%Player, %Mid, 9999) @ 
										 Mission20001Dialog(%Npc, %Player, %Mid, 10002) @"<b/>"@ 
										 Mission20001Dialog(%Npc, %Player, %Mid, 999) @ "</t>";
	}
	
	switch(%Tid)
	 {
		 case 501: return "你已经接受过师门任务.";
		 case 502: return "你的等级未达到10级,无法领取师门任务.";
		 case 503: return "你身上接受的任务数量已满.";
		 case 504: return "你今天的师门任务已经做完";
		 case 505: return "你的师门还处于冷却中,10分钟后在来";
		 case 506: return "<t>你将被扣除【</t>"@ $Get_Dialog_GeShi[31206] @ %Tmoney @"</t><t>】的金钱是否真的决定使用?</t>";
		 case 507: return "<t>你没事逗我玩是不,你师门任务都没接,我怎么让你偷懒.</t>";
		 case 508: return "<t>这么快就完成了任务,不错不错，是否还继续？</t>";
		 case 509: return "<t>你的师门任务尚未完成.</t>";
		 case 511: return "<t>没钱也想偷懒！</t>";
		 			
		 case 510: return  $Icon[1] @ $Get_Dialog_GeShi[31203] @"<t>接取 - 循环:[10] 师门尽忠(10级)</t>";
	   case 520: return  $Icon[1] @ $Get_Dialog_GeShi[31204] @"<t>偷懒的弟子</t>";
	   case 530: return  $Icon[1] @ $Get_Dialog_GeShi[31204] @"<t>师门尽忠</t>"; 
	   case 540: return  $Icon[1] @ $Get_Dialog_GeShi[31204] @"<t>继续任务</t>";	
	   case 550: return  $Icon[1] @ $Get_Dialog_GeShi[31204] @"<t>交付 - 循环：[10] 师门尽忠(10级)</t>";		
	  	
	 }
	
	return "Mission" @%Mid@ "Dialog == 错误 Npc【"@%Npc@"】Player【"@%Player@"】Mid【"@%Mid@"】Tid【"@%Tid@"】";

}