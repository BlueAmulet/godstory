function Mission20002Dialog(%Npc, %Player, %Mid, %Tid) //Ѱ��
{
	   %XZ_QuYu =  %Player.GetMissionFlag(%Mid,8100);
		 %XZ_ZB1 =  %Player.GetMissionFlag(%Mid,8200);
		 %XZ_ZB2 =  %Player.GetMissionFlag(%Mid,8300);
		 %Zs = %Player.GetMissionFlag(%Mid,9100);
		 %XZ_ZuoBiao1 = Pos3DTo2D(GetWord(%XZ_ZB1,0), GetWord(%XZ_ZB2,1));
		 %XZ_ZuoBiao2 = GetWord(%Map_ZuoBiao1,0) @ "," @ GetWord(%Map_ZuoBiao1,1);
		 
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
     
     if(%XZ_QuYu $="811010100"){%QYMC ="����";}
     if(%XZ_QuYu $="811020100"){%QYMC ="������";}
     if(%XZ_QuYu $="811030100"){%QYMC ="���ǲ�Ұ";}
     if(%XZ_QuYu $="811040100"){%QYMC ="����ɭ��";}
      		
		 switch(%Tid)
		 {
		 	
			  case 10002: return %TPng_A @ "<b/>"@ //Ѱ��-��������
			  	                 		"<t>ǰ����"@%QYMC@"���ġ�"@%XZ_ZuoBiao2@"������ǿ����ȡ��ͼ��</t><b/>"@
			  	                 		"<t>��Ŀǰ�챦���������˵�"@%Zs@"�Ρ�</t><b/>";
			  	                 		
			  case 500:	return %TPng_A @ "<b/>"@ //Ѱ��-��������
			  	                 		"<t>ǰ����"@%QYMC@"���ġ�"@%XZ_ZuoBiao2@"������ǿ����ȡ��ͼ��</t><b/>"@
			  	                 		"<t>��Ŀǰ�챦���������˵�"@%Zs@"�Ρ�</t><b/>";                 		
			  	                 		
			  case 511: return %TPng_B @"<b/>"@	//Ѱ��-����Ŀ��
				                  	  "<t>ǰ����"@%QYMC@"���ġ�"@%XZ_ZuoBiao2@"������ǿ����ȡ��ͼ��</t><b/>";
  						   
				//����׷�ٴ����ȸ����֣�����Ŀ��+������
				
				case 800: return $Get_Dialog_GeShi[31201] @ Mission20002Dialog(%Npc, %Player, %Mid, 511) @ GetMissionNeedText(%Player, %Mid, 9999)@ "</t>";
					
			 //�����Ѷ�+Ŀ��+����+����+����,���������ʹ��	
			
				case 888: return $Get_Dialog_GeShi[31201] @ Get_Mis_Dialog(%Npc, %Player, %Mid, 3) @
												 Mission20002Dialog(%Npc, %Player, %Mid, 511)@"<b/>"@
												 GetMissionNeedText(%Player, %Mid, 9999) @ 
												 Mission20002Dialog(%Npc, %Player, %Mid, 10002) @"</t>";
		}	
		
		switch(%Tid)
		{
			 case 700: return $Icon[1] @ $Get_Dialog_GeShi[31203] @"<t>��ȡ - ѭ��:[30] �ر�ͼ(30��)</t>";  
			 case 702: return $Icon[1] @ $Get_Dialog_GeShi[31204] @"<t>�������ȥ,�������</t>";          
			 case 701: return "<t>�ұ����˼䶦����������״Ԫ,���ѵ���,���������˰���,��������һ�������ı���,�Ҿ͸����㱦ͼ������.</t>";
			 	
		}
}


function Mission20003Dialog(%Npc, %Player, %Mid, %Tid) //̽��
{
	 %TB_QuYu =  %Player.GetMissionFlag(%Mid,8400);
	 %TB_ZB1 =  %Player.GetMissionFlag(%Mid,8500);
	 %TB_ZB2 =  %Player.GetMissionFlag(%Mid,8600);
	 %TB_ZuoBiao1 = Pos3DTo2D(GetWord(%TB_ZB1,0), GetWord(%TB_ZB2,1));
	 %TB_ZuoBiao2 = GetWord(%TB_ZuoBiao1,0) @ "," @ GetWord(%TB_ZuoBiao2,1);
		 
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
	 	
	 if(%TB_QuYu $="811010100"){%QYMC ="����";}
   if(%TB_QuYu $="811020100"){%QYMC ="������";}
   if(%TB_QuYu $="811030100"){%QYMC ="���ǲ�Ұ";}
   if(%TB_QuYu $="811040100"){%QYMC ="����ɭ��";}
   	
   switch(%Tid)
		 {	  
		 			//̽��-��������
		 			
	   			case 10002: return %TPng_A @ "<b/>"@ 
				  	                 		"<t>���ŵ�ͼ������ڡ�"@%QYMC@"���ġ�"@%TB_ZuoBiao2@"�������ܴ����Ӵ�ı��ء�</t><b/>";
				  //̽��-��������	 
				                  		
				  case 600:	return %TPng_A @ "<b/>"@ 	
				  	                 		"<t>���ŵ�ͼ������ڡ�"@%QYMC@"���ġ�"@%TB_ZuoBiao2@"�������ܴ����Ӵ�ı��ء�</t><b/>";       		
				  //̽��-����Ŀ��
				  	                 		
				  case 611: return %TPng_B @"<b/>"@		
					                  	 "<t>���ŵ�ͼ������ڡ�"@%QYMC@"���ġ�"@%TB_ZuoBiao2@"�������ܴ����Ӵ�ı��ء�</t><b/>";	
					                  	 
					//����׷�ٴ����ȸ����֣�����Ŀ��+������
				
					case 800: return $Get_Dialog_GeShi[31201] @ Mission20003Dialog(%Npc, %Player, %Mid, 611) @ GetMissionNeedText(%Player, %Mid, 9999)@ "</t>";
					
			 	 //�����Ѷ�+Ŀ��+����+����+����,���������ʹ��	
			
					case 888: return $Get_Dialog_GeShi[31201] @ Get_Mis_Dialog(%Npc, %Player, %Mid, 3) @
													 Mission20003Dialog(%Npc, %Player, %Mid, 611)@"<b/>"@
													 GetMissionNeedText(%Player, %Mid, 9999) @ 
													 Mission20003Dialog(%Npc, %Player, %Mid, 10002) @"</t>"; 
													 
   }
}                 	 				                  	  	                 	