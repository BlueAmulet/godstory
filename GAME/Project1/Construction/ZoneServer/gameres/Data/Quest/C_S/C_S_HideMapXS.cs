function Mission20002Dialog(%Npc, %Player, %Mid, %Tid) //寻踪
{
	   %XZ_QuYu =  %Player.GetMissionFlag(%Mid,8100);
		 %XZ_ZB1 =  %Player.GetMissionFlag(%Mid,8200);
		 %XZ_ZB2 =  %Player.GetMissionFlag(%Mid,8300);
		 %Zs = %Player.GetMissionFlag(%Mid,9100);
		 %XZ_ZuoBiao1 = Pos3DTo2D(GetWord(%XZ_ZB1,0), GetWord(%XZ_ZB2,1));
		 %XZ_ZuoBiao2 = GetWord(%Map_ZuoBiao1,0) @ "," @ GetWord(%Map_ZuoBiao1,1);
		 
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
     
     if(%XZ_QuYu $="811010100"){%QYMC ="清风滨";}
     if(%XZ_QuYu $="811020100"){%QYMC ="仙游岭";}
     if(%XZ_QuYu $="811030100"){%QYMC ="无忧草野";}
     if(%XZ_QuYu $="811040100"){%QYMC ="遮月森林";}
      		
		 switch(%Tid)
		 {
		 	
			  case 10002: return %TPng_A @ "<b/>"@ //寻踪-任务描述
			  	                 		"<t>前往【"@%QYMC@"】的【"@%XZ_ZuoBiao2@"】消灭强盗夺取宝图。</t><b/>"@
			  	                 		"<t>你目前异宝觅踪做到了第"@%Zs@"次。</t><b/>";
			  	                 		
			  case 500:	return %TPng_A @ "<b/>"@ //寻踪-任务描述
			  	                 		"<t>前往【"@%QYMC@"】的【"@%XZ_ZuoBiao2@"】消灭强盗夺取宝图。</t><b/>"@
			  	                 		"<t>你目前异宝觅踪做到了第"@%Zs@"次。</t><b/>";                 		
			  	                 		
			  case 511: return %TPng_B @"<b/>"@	//寻踪-任务目标
				                  	  "<t>前往【"@%QYMC@"】的【"@%XZ_ZuoBiao2@"】消灭强盗夺取宝图。</t><b/>";
  						   
				//任务追踪处的热感文字，任务目标+任务奖励
				
				case 800: return $Get_Dialog_GeShi[31201] @ Mission20002Dialog(%Npc, %Player, %Mid, 511) @ GetMissionNeedText(%Player, %Mid, 9999)@ "</t>";
					
			 //任务难度+目标+需求+描述+奖励,任务界面内使用	
			
				case 888: return $Get_Dialog_GeShi[31201] @ Get_Mis_Dialog(%Npc, %Player, %Mid, 3) @
												 Mission20002Dialog(%Npc, %Player, %Mid, 511)@"<b/>"@
												 GetMissionNeedText(%Player, %Mid, 9999) @ 
												 Mission20002Dialog(%Npc, %Player, %Mid, 10002) @"</t>";
		}	
		
		switch(%Tid)
		{
			 case 700: return $Icon[1] @ $Get_Dialog_GeShi[31203] @"<t>接取 - 循环:[30] 藏宝图(30级)</t>";  
			 case 702: return $Icon[1] @ $Get_Dialog_GeShi[31204] @"<t>打发你的拿去,快告诉我</t>";          
			 case 701: return "<t>我本是人间鼎鼎大名的武状元,落难到此,不曾想卖了包子,如果你给我一定数量的报酬,我就告诉你宝图的秘密.</t>";
			 	
		}
}


function Mission20003Dialog(%Npc, %Player, %Mid, %Tid) //探宝
{
	 %TB_QuYu =  %Player.GetMissionFlag(%Mid,8400);
	 %TB_ZB1 =  %Player.GetMissionFlag(%Mid,8500);
	 %TB_ZB2 =  %Player.GetMissionFlag(%Mid,8600);
	 %TB_ZuoBiao1 = Pos3DTo2D(GetWord(%TB_ZB1,0), GetWord(%TB_ZB2,1));
	 %TB_ZuoBiao2 = GetWord(%TB_ZuoBiao1,0) @ "," @ GetWord(%TB_ZuoBiao2,1);
		 
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
	 	
	 if(%TB_QuYu $="811010100"){%QYMC ="清风滨";}
   if(%TB_QuYu $="811020100"){%QYMC ="仙游岭";}
   if(%TB_QuYu $="811030100"){%QYMC ="无忧草野";}
   if(%TB_QuYu $="811040100"){%QYMC ="遮月森林";}
   	
   switch(%Tid)
		 {	  
		 			//探宝-任务描述
		 			
	   			case 10002: return %TPng_A @ "<b/>"@ 
				  	                 		"<t>这张地图标记着在【"@%QYMC@"】的【"@%TB_ZuoBiao2@"】处可能存在庞大的宝藏。</t><b/>";
				  //探宝-任务描述	 
				                  		
				  case 600:	return %TPng_A @ "<b/>"@ 	
				  	                 		"<t>这张地图标记着在【"@%QYMC@"】的【"@%TB_ZuoBiao2@"】处可能存在庞大的宝藏。</t><b/>";       		
				  //探宝-任务目标
				  	                 		
				  case 611: return %TPng_B @"<b/>"@		
					                  	 "<t>这张地图标记着在【"@%QYMC@"】的【"@%TB_ZuoBiao2@"】处可能存在庞大的宝藏。</t><b/>";	
					                  	 
					//任务追踪处的热感文字，任务目标+任务奖励
				
					case 800: return $Get_Dialog_GeShi[31201] @ Mission20003Dialog(%Npc, %Player, %Mid, 611) @ GetMissionNeedText(%Player, %Mid, 9999)@ "</t>";
					
			 	 //任务难度+目标+需求+描述+奖励,任务界面内使用	
			
					case 888: return $Get_Dialog_GeShi[31201] @ Get_Mis_Dialog(%Npc, %Player, %Mid, 3) @
													 Mission20003Dialog(%Npc, %Player, %Mid, 611)@"<b/>"@
													 GetMissionNeedText(%Player, %Mid, 9999) @ 
													 Mission20003Dialog(%Npc, %Player, %Mid, 10002) @"</t>"; 
													 
   }
}                 	 				                  	  	                 	