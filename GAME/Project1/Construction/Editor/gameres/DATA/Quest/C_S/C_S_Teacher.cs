function Mission20001Dialog(%Npc, %Player, %Mid, %Tid)
{	
		%Hs = %Player.GetMissionFlag(%Mid,100); //��ȡ������껷��
		%Cs = %Player.GetMissionFlag(%Mid,200); //��ȡ����������
		%Lx = %Player.GetMissionFlag(%Mid,300); //��ȡ��������
		%Zs = %Player.GetMissionFlag(%Mid,310); //��ȡ��������
		%Buff = %Player.GetBuffCount(32005,2);   //��ȡ���ڹ��
		%Mz = %Player.Getfamily();               //��ȡ���ְҵ
		%Lv = %Player.GetLevel();                //��ȡ��ҵȼ�
		
	  if(%Player.GetMissionFlag(%Mid,300) == 1)//����
	  	{
				%SongXinID1 = %Player.GetMissionFlag(%Mid,1150);
				%SongXinID2 = %Player.GetMissionFlag(%Mid,1151);
				if(%SongXinID1 > 0){%SongXin1 ="��"@GetNpcData(%SongXinID1,1)@"��";} 
				if(%SongXinID2 > 0){%SongXin2 ="��"@GetNpcData(%SongXinID2,1)@"��";}
				%SongXin = %SongXin1@%SongXin2;
				
				%Exp = 50* %Hs* %Cs;
				%Sw = 5*%Hs* %Cs;
			  %tt = 111;
		  }

		if(%Player.GetMissionFlag(%Mid,300) == 2)//����
			{
				%PeiyuID1 = %Player.GetMissionFlag(%Mid,2100);
				%PeiyuID2 = %Player.GetMissionFlag(%Mid,2101);
				if(%PeiyuID1 > 0){%Peiyu1 ="��"@GetItemData(%PeiyuID1,1)@"��";} 
				if(%PeiyuID2 > 0){%Peiyu2 ="��"@GetItemData(%PeiyuID2,1)@"��";}
				%Peiyu = %Peiyu1@%Peiyu2;
				
				%Exp = 60* %Hs* %Cs;
				%Sw = 6*%Hs* %Cs;
				%tt = 112;
			}
			
		if(%Player.GetMissionFlag(%Mid,300) == 3)//Ѳ��
			{
				%XunluoID1 = %Player.GetMissionFlag(%Mid,1150);
				%XunluoID2 = %Player.GetMissionFlag(%Mid,1151);
				if(%XunluoID1 > 0){%Xunluo1 ="��"@GetNpcData(%XunluoID1,1)@"��";} 
				if(%XunluoID2 > 0){%Xunluo2 ="��"@GetNpcData(%XunluoID2,1)@"��";}
				%Xunluo = %Xunluo1@%Xunluo2;
				
				%Exp = 70* %Hs* %Cs;
				%Sw = 7*%Hs* %Cs;
				%tt = 113;
			}
		if(%Player.GetMissionFlag(%Mid,300) == 4)	//����
			{
				%GouWuID1 = %Player.GetMissionFlag(%Mid,2100);
				%GouWuID2 = %Player.GetMissionFlag(%Mid,2101);
				if(%GouWuID1 > 0){%GouWu1 ="��"@GetItemData(%GouWuID1,1)@"��";} 
				if(%GouWuID2 > 0){%GouWu2 ="��"@GetItemData(%GouWuID2,1)@"��";}
				%GouWu = %GouWu1@%GouWu2;
				%Exp = 80* %Hs* %Cs;
				%Sw = 8*%Hs* %Cs;
				%tt = 114;
			}
			
		if(%Player.GetMissionFlag(%Mid,300) == 5)	//��ɱ
			{
				%JiShaID1 = %Player.GetMissionFlag(%Mid,3100);
				%JiShaID2 = %Player.GetMissionFlag(%Mid,3101);
				if(%JiShaID1 > 0){%JiSha1 ="��"@GetNpcData(%JiShaID1,1)@"��";} 
				if(%JiShaID2 > 0){%JiSha2 ="��"@GetNpcData(%JiShaID2,1)@"��";}
				%JiSha = %JiSha1@%JiSha2;
				
				%Exp = 90* %Hs* %Cs;
				%Sw = 8*%Hs* %Cs;
				%tt = 115;
			}
			
		if(%Player.GetMissionFlag(%Mid,300) == 7)	//����
			{
				%BuShouID1 = %Player.GetMissionFlag(%Mid,2100);
				%BuShouID2 = %Player.GetMissionFlag(%Mid,2101);
				if(%BuShouID1 > 0){%BuShou1 ="��"@GetItemData(%BuShouID1,1)@"��";} 
				if(%BuShouID2 > 0){%BuShou2 ="��"@GetItemData(%BuShouID2,1)@"��";}
				%BuShou = %BuShou1@%BuShou2;
				%Exp = 100* %Hs* %Cs;
				%Sw = 8*%Hs* %Cs; 
				%tt = 117;
			}
	
	  if((%Zs <= 20)||(%Buff==1)){%u = "�˴�����Ϊ˫������";} //��ʾ˫��������ʾ
	    	
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
								
		%TPngA = isFile("gameres/gui/images/GUIWindow25_1_005.png");//��������
		%TPngB = isFile("gameres/gui/images/GUIWindow25_1_006.png");//����Ŀ��
		%TPngC = isFile("gameres/gui/images/GUIWindow25_1_007.png");//������
		%TPngD = isFile("gameres/gui/images/GUIWindow25_1_008.png");//�������
		%TPngE = isFile("gameres/gui/images/GUIWindow25_1_009.png");//�����Ѷ�
	
		if(%TPngA == 1){%TPng_A = "<i s='gameres/gui/images/GUIWindow25_1_005.png' w='16' h='16'/>";}else{%TPng_A = "��������</t>";}
		if(%TPngB == 1){%TPng_B = "<i s='gameres/gui/images/GUIWindow25_1_006.png' w='16' h='16'/>";}else{%TPng_B = "����Ŀ��</t>";}
		if(%TPngC == 1){%TPng_C = "<i s='gameres/gui/images/GUIWindow25_1_007.png' w='16' h='16'/>";}else{%TPng_C = "������</t>";}
		if(%TPngD == 1){%TPng_D = "<i s='gameres/gui/images/GUIWindow25_1_008.png' w='16' h='16'/>";}else{%TPng_D = "�������</t>";}
		if(%TPngE == 1){%TPng_E = "<i s='gameres/gui/images/GUIWindow25_1_009.png' w='16' h='16'/>";}else{%TPng_E = "�����Ѷ� </t>";}

	 switch(%Tid)
	 {
	  case 10002: return %TPng_A @ "<b/>"@ //��������
	  	                 		"<t>��Ŀǰ��ʦ�������˵�"@%Hs@"����"@%Cs@"�Ρ�</t><b/>"@
	  	                 		 $Get_Dialog_GeShi[31206] @%u@"</t><b/><b/>";
	  	           
		case 101:	return 	%TPng_A @"<b/>"@	//����-����������
													"<t>��ȥ�Ӹ����Ÿ�</t>"@%SongXin@"<t>��</t><b/>"@
										  		"<t>��Ŀǰ��ʦ�������˵�"@%Hs@"����"@%Cs@"�Ρ�</t><b/>"@
										  		$Get_Dialog_GeShi[31206] @%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										 		 	$Get_Dialog_GeShi[31203]@"���飺 </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203]@"������ </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			               			
	  case 102: return 	%TPng_A @"<b/>"@	//����-����������
										  		"<t>��ȥ�ɼ�һЩ</t>"@%Peiyu@"<t>����</t><b/>"@
										 			"<t>��Ŀǰ��ʦ�������˵�"@%Hs@"����"@%Cs@"�Ρ�</t><b/>"@
										 			$Get_Dialog_GeShi[31206]@%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										  		$Get_Dialog_GeShi[31203] @"���飺 </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203] @"������ </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			                
		case 103:	return  %TPng_A @"<b/>"@	//Ѳ��-����������
										 			"<t>��ȥ��</t>"@%Xunluo@"<t>�ط������Ƿ�ȫ��</t><b/>"@
										  		"<t>��Ŀǰ��ʦ�������˵�"@%Hs@"����"@%Cs@"�Ρ�</t><b/>"@
										  		$Get_Dialog_GeShi[31206] @%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										  		$Get_Dialog_GeShi[31203] @"���飺 </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203] @"������ </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			               			
		case 104: return  %TPng_A @"<b/>"@	//����-����������
										  		"<t>��ȥ��һЩ</t>"@%GouWu@"<t>����</t><b/>"@
										  		"<t>��Ŀǰ��ʦ�������˵�"@%Hs@"����"@%Cs@"�Ρ�</t><b/>"@
										  		$Get_Dialog_GeShi[31206]@%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										  		$Get_Dialog_GeShi[31203] @ "���飺 </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203] @ "������ </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
		
		case 105: return  %TPng_A @"<b/>"@	//��ɱ-����������
										  		"<t>��ȥɱ��һЩ</t>"@%JiSha@"<t>������</t><b/>"@
										  		"<t>��Ŀǰ��ʦ�������˵�"@%Hs@"����"@%Cs@"�Ρ�</t><b/>"@
										  		$Get_Dialog_GeShi[31206]@%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										  		$Get_Dialog_GeShi[31203] @ "���飺 </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203] @ "������ </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			               			
	 case 107: return  %TPng_A @"<b/>"@	//����-����������
										  		"<t>��ȥץһֻ</t>"@%BuShou@"<t>���ҡ�</t><b/>"@
										  		"<t>��Ŀǰ��ʦ�������˵�"@%Hs@"����"@%Cs@"�Ρ�</t><b/>"@
										  		$Get_Dialog_GeShi[31206]@%u@"</t><b/><b/>"@
										  %TPng_C @ "<b/>"@ 
										  		$Get_Dialog_GeShi[31203] @ "���飺 </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               			$Get_Dialog_GeShi[31203] @ "������ </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			               			      	
	  case 111: return 	%TPng_B @"<b/>"@	//����-����Ŀ��
		                  	"<t>��ȥ�Ӹ����Ÿ�</t>"@%SongXin@"<t>��</t><b/>";
		                  	
	  case 112: return	%TPng_B @"<b/>"@	//����-����Ŀ��
		                  	"<t>��ȥ�ɼ�һЩ</t>"@%Peiyu@"<t>����</t><b/>";	                  	
		                  
	  case 113: return	%TPng_B @"<b/>"@	//Ѳ��-����Ŀ��
	  								 		"<t>��ȥ��</t>"@%Xunluo@"<t>�ط������Ƿ�ȫ��</t><b/>";
	  								 
	  case 114: return	%TPng_B @"<b/>"@	//����-����Ŀ��
		                  		"<t>��ȥ��һЩ</t>"@%GouWu@"<t>����</t><b/>";
		                  
		case 115: return  %TPng_B @"<b/>"@	//��ɱ-����Ŀ��
		                  "<t>��ȥɱ��һЩ</t>"@%JiSha@"<t>������</t><b/>";
		                  
	  case 117: return  %TPng_B @"<b/>"@	//��ɱ-����Ŀ��
		                  "<t>��ȥץһֻ</t>"@%BuShou@"<t>���ҡ�</t><b/>";
	
	  case 535: return %TPng_D @"<b/>"@   //�������
	  	               "<t>�õ�,��֪���ˡ�</t>";
	  	
		case 555: return %TPng_D @"<b/>"@   //�������
										    "<t>����������ô�����ɡ�</t><b/><b/>"@
										 %TPng_C @ "<b/>"@ 
									      $Get_Dialog_GeShi[31203] @ "���飺 </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@																																																																																																													
			                  $Get_Dialog_GeShi[31203] @ "������ </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";	    
		case 999:
										if(%Zs !=50)
		                  {
		                     return "</t>" @ %TPng_C @ "<b/>"@ //������
			               		 $Get_Dialog_GeShi[31203] @"���飺 </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
			               		 $Get_Dialog_GeShi[31203] @ "������ </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>";
			               	} 
			             if(%Zs ==50)
			               {	
			               		return "</t>" @ %TPng_C @ "<b/>"@ //������
					             $Get_Dialog_GeShi[31203] @ "���飺 </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Exp @ " </t><t>]</t><b/>"@
					             $Get_Dialog_GeShi[31203] @ "������ </t><t>[ </t>" @ $Get_Dialog_GeShi[31201] @ %Sw @ " </t><t>]</t><b/>"@	
					             $Get_Dialog_GeShi[31203] @ "�̶��������� </t><b/>"@ GetFixedMissionItemText();
									   }
									   
		//����׷�ٴ����ȸ����֣�����Ŀ��+������
		
		case 800: return $Get_Dialog_GeShi[31201] @ Mission20001Dialog(%Npc, %Player, %Mid, %tt) @ GetMissionNeedText(%Player, %Mid, 9999) @Mission20001Dialog(%Npc, %Player, %Mid, 999) @ "</t>";
		
	//�����Ѷ�+Ŀ��+����+����+����,���������ʹ��	
	
		case 888: return $Get_Dialog_GeShi[31201] @ Get_Mis_Dialog(%Npc, %Player, %Mid, 3) @
										 Mission20001Dialog(%Npc, %Player, %Mid, %tt)@"<b/>"@
										 GetMissionNeedText(%Player, %Mid, 9999) @ 
										 Mission20001Dialog(%Npc, %Player, %Mid, 10002) @"<b/>"@ 
										 Mission20001Dialog(%Npc, %Player, %Mid, 999) @ "</t>";
	}
	
	switch(%Tid)
	 {
		 case 501: return "���Ѿ����ܹ�ʦ������.";
		 case 502: return "��ĵȼ�δ�ﵽ10��,�޷���ȡʦ������.";
		 case 503: return "�����Ͻ��ܵ�������������.";
		 case 504: return "������ʦ�������Ѿ�����";
		 case 505: return "���ʦ�Ż�������ȴ��,10���Ӻ�����";
		 case 506: return "<t>�㽫���۳���</t>"@ $Get_Dialog_GeShi[31206] @ %Tmoney @"</t><t>���Ľ�Ǯ�Ƿ���ľ���ʹ��?</t>";
		 case 507: return "<t>��û�¶������ǲ�,��ʦ������û��,����ô����͵��.</t>";
		 case 508: return "<t>��ô������������,�������Ƿ񻹼�����</t>";
		 case 509: return "<t>���ʦ��������δ���.</t>";
		 case 511: return "<t>ûǮҲ��͵����</t>";
		 			
		 case 510: return  $Icon[1] @ $Get_Dialog_GeShi[31203] @"<t>��ȡ - ѭ��:[10] ʦ�ž���(10��)</t>";
	   case 520: return  $Icon[1] @ $Get_Dialog_GeShi[31204] @"<t>͵���ĵ���</t>";
	   case 530: return  $Icon[1] @ $Get_Dialog_GeShi[31204] @"<t>ʦ�ž���</t>"; 
	   case 540: return  $Icon[1] @ $Get_Dialog_GeShi[31204] @"<t>��������</t>";	
	   case 550: return  $Icon[1] @ $Get_Dialog_GeShi[31204] @"<t>���� - ѭ����[10] ʦ�ž���(10��)</t>";		
	  	
	 }
	
	return "Mission" @%Mid@ "Dialog == ���� Npc��"@%Npc@"��Player��"@%Player@"��Mid��"@%Mid@"��Tid��"@%Tid@"��";

}