//===========================================================================================================
// �ĵ�˵��:ȫ�ֱ�������
// ����ʱ��:2009-07-08
// ������: thinking
//=========================================================================================================== 
//����ű�
$GUI::GUIDirectory = "gameres/gui/";
//����ͼƬ
$GUI::GUISkinDirectory = $GUI::GUIDirectory @ "images/";
//����Ŀ¼
$GUI::FontCacheDirectory = "fonts/";
//�������д���
$hideAllGuiWindow = false;

$createPlayer_Sex = 0;
$createPlayer_Data = 0;
//$createPlayer_Face = 0;
//$createPlayer_Hair = 0;
$createPlayer_Apparel = 0;
//��������_��
$PlayerFaceShape_Boy[0] = 21091;
$PlayerFaceShape_Boy[1] = 21092; 
$PlayerFaceShape_Boy[2] = 21093;
$PlayerFaceShape_Boy[3] = 21094;
$PlayerFaceShape_Boy[4] = 21095;
$PlayerFaceShape_Boy[5] = 21096;

//������������_��
$PlayerFaceName_Boy[0] = "���1";
$PlayerFaceName_Boy[1] = "���2";
$PlayerFaceName_Boy[2] = "���3";
$PlayerFaceName_Boy[3] = "���4";
$PlayerFaceName_Boy[4] = "���5";
$PlayerFaceName_Boy[5] = "���6";
			
//��������_Ů
$PlayerFaceShape_Girl[0] = 21591;
$PlayerFaceShape_Girl[1] = 21592;
$PlayerFaceShape_Girl[2] = 21593;
$PlayerFaceShape_Girl[3] = 21594;
$PlayerFaceShape_Girl[4] = 21595;
$PlayerFaceShape_Girl[5] = 21596;

//������������_Ů
$PlayerFaceName_Girl[0] = "���1";
$PlayerFaceName_Girl[1] = "���2";
$PlayerFaceName_Girl[2] = "���3";
$PlayerFaceName_Girl[3] = "���4";
$PlayerFaceName_Girl[4] = "���5";
$PlayerFaceName_Girl[5] = "���6";
	
//���﷢������_��
$PlayerHairName_Boy[0] = "����1";
$PlayerHairName_Boy[1] = "����2";
$PlayerHairName_Boy[2] = "����3";
$PlayerHairName_Boy[3] = "����4";
$PlayerHairName_Boy[4] = "����5";
$PlayerHairName_Boy[5] = "����6";
		
//���﷢������_Ů
$PlayerHairName_Girl[0] = "����1";
$PlayerHairName_Girl[1] = "����2";
$PlayerHairName_Girl[2] = "����3";
$PlayerHairName_Girl[3] = "����4";
$PlayerHairName_Girl[4] = "����5";
$PlayerHairName_Girl[5] = "����6";	

//���﷢��_��
$PlayerHairShape_Boy[0] = 31091;
$PlayerHairShape_Boy[1] = 31092;
$PlayerHairShape_Boy[2] = 31093;
$PlayerHairShape_Boy[3] = 31094;
$PlayerHairShape_Boy[4] = 31095;
$PlayerHairShape_Boy[5] = 31096;

//���﷢��_Ů
$PlayerHairShape_Girl[0] = 31591;
$PlayerHairShape_Girl[1] = 31592;
$PlayerHairShape_Girl[2] = 31593;
$PlayerHairShape_Girl[3] = 31594;
$PlayerHairShape_Girl[4] = 31595;
$PlayerHairShape_Girl[5] = 31596;

//���﷢ɫ
$PlayerHairColorShape[0] = "a";
$PlayerHairColorShape[1] = "b";
$PlayerHairColorShape[2] = "c";
$PlayerHairColorShape[3] = "d";
$PlayerHairColorShape[4] = "e";
$PlayerHairColorShape[5] = "f";

//���﷢ɫ����
$PlayerHairColorName[0] = "��ɫ1";
$PlayerHairColorName[1] = "��ɫ2";
$PlayerHairColorName[2] = "��ɫ3";
$PlayerHairColorName[3] = "��ɫ4";
$PlayerHairColorName[4] = "��ɫ5";
$PlayerHairColorName[5] = "��ɫ6";

//��������_��								   
$PlayerBodyShape_Boy[0] = "ply01_001_3";
$PlayerBodyShape_Boy[1] = "ply01_002_3";
$PlayerBodyShape_Boy[2] = "ply01_003_3";	
//�����װ_��								   
$PlayerApparelShape_Boy[0] = "102029001";
$PlayerApparelShape_Boy[1] = "102029002";
$PlayerApparelShape_Boy[2] = "102029003";	
//�����װ����_��	                                       
$PlayerApparelName_Boy[0] = "��װ1";
$PlayerApparelName_Boy[1] = "��װ2";
$PlayerApparelName_Boy[2] = "��װ3";
//��������_Ů
$PlayerBodyShape_Girl[0] = "ply02_001_3";
$PlayerBodyShape_Girl[1] = "ply02_002_3";
$PlayerBodyShape_Girl[2] = "ply02_003_3";	
//�����װ_Ů								   
$PlayerApparelShape_Girl[0] = "102029004";
$PlayerApparelShape_Girl[1] = "102029005";
$PlayerApparelShape_Girl[2] = "102029006";	                                     
//�����װ����_Ů                                       
$PlayerApparelName_Girl[0] = "��װ1";
$PlayerApparelName_Girl[1] = "��װ2";
$PlayerApparelName_Girl[2] = "��װ3";		

$FirstFaceNum = 0;			
$FirstHairNum = 0;
$FirstHColorNum = 0;
$FirstApparelNum = 0;			 

$FaceMaxNum = 5;
$HairMaxNum = 5;
$HColorMaxNum = 5;
$ApparelMaxNum = 2;
  
//ʹ�õ�ģ������
$UseShapeType = 0;
//����ͼƬҳ��
$ShapeImageNum = 0;
$ShapeImageMaxNum = 0;
$LivingCurrentSelectSeries = 0;                  //ѡ���������
$SelectPrescriptionID = 0;                       //ѡ�����䷽ID
$DoingTreeItemId =0;                             //���������䷽����ItemId

$SocialFindPlayerAction = 0;
//�Զ�����
$AutoSellSlots	= 0;
$AutoListSelectedIndex = -1;

//ͳһ�رս��淽��
$IsDirty = 0;
$CloseNum = 0;
$CloseData[$CloseNum]   = "ClosePlayerInfo();";                        //������Ϣ����
$CloseData[$CloseNum++] = "CloseAllBag();";                            //��������_�ϰ�
$CloseData[$CloseNum++] = "CloseSingleBag();";                         //��������_�ְ�
$CloseData[$CloseNum++] = "CloseMissionWnd();";                        //�������
$CloseData[$CloseNum++] = "ClosePetInfoWnd();";                        //�������
$CloseData[$CloseNum++] = "CloseRelationWnd();";                       //��ϵ����
$CloseData[$CloseNum++] = "CloseSeeInfoWnd();";                        //��ϵ����_�鿴������Ϣ
$CloseData[$CloseNum++] = "CloseFriendCheerWnd();";                    //��ϵ����_����
$CloseData[$CloseNum++] = "CloseSettingWnd();";                        //��ϵ����_����
$CloseData[$CloseNum++] = "CloseFriendHintWnd();";                     //��ϵ����_��Ӻ���
$CloseData[$CloseNum++] = "CloseStallWnd();";                          //��̯����
$CloseData[$CloseNum++] = "CloseStallInfoWnd();";                      //��̯����_̯λ��Ϣ
$CloseData[$CloseNum++] = "CloseLookupWnd();";                         //��̯����_������Ʒ
$CloseData[$CloseNum++] = "CloseLookupReturnWnd();";                   //��̯����_������Ʒ������Ϣ
$CloseData[$CloseNum++] = "CloseChangePriceWnd();";                    //��̯����_���ü۸�
$CloseData[$CloseNum++] = "CloseSetStallWnd();";                       //��̯����_����̯λ
$CloseData[$CloseNum++] = "CloseSetTradeItemWnd();";                   //��̯����_���ù���or�չ��۸�
$CloseData[$CloseNum++] = "CloseStallSeeWnd();";                       //��̯����_�鿴Ŀ��̯λ
$CloseData[$CloseNum++] = "CloseBankWnd();";                           //�ֿ����
$CloseData[$CloseNum++] = "CloseBankWnd_InputWnd();";                  //�ֿ����_��ȡ����
$CloseData[$CloseNum++] = "CloseTeamWnd();";                           //�������
$CloseData[$CloseNum++] = "CloseTeamInfoWnd();";                       //������Ϣ����
$CloseData[$CloseNum++] = "CloseInviteJoinTeamWnd();";                 //�������_�������
$CloseData[$CloseNum++] = "CloseSkillWnd();";                          //���ܽ���
$CloseData[$CloseNum++] = "CloseSkillSelect();";                       //���ܽ���_����ϵѡ��
$CloseData[$CloseNum++] = "CloseSkillStudy();";                        //���ܽ���_����ѧϰ
$CloseData[$CloseNum++] = "onCancelIdentify();";                       //��Ʒ����
$CloseData[$CloseNum++] = "cancelSplit();";                            //��Ʒ���
$CloseData[$CloseNum++] = "cancelDestroyItem();";                      //��Ʒ����
$CloseData[$CloseNum++] = "CloseBigViewMap();";                        //��ǰ��ͼ
//$CloseData[$CloseNum++] = "StopFindPath();";                           //Ѱ����ʾ��
$CloseData[$CloseNum++] = "CloseSysSettingWnd();";                     //ϵͳ���ý���
$CloseData[$CloseNum++] = "CloseNpcShopWnd();";                        //�̵����
$CloseData[$CloseNum++] = "CloseShopHelpWnd();";                       //�������ֽ���
$CloseData[$CloseNum++] = "CloseShopWnd_BatchSellWnd();";              //�����������
$CloseData[$CloseNum++] = "CloseShopWnd_NotarizeWnd();";               //ȷ���������
$CloseData[$CloseNum++] = "CloseTradeWnd();";                          //���׽���
$CloseData[$CloseNum++] = "CloseTradeHelpWnd();";                      //���װ�������
$CloseData[$CloseNum++] = "CloseTradeNoteWnd();";                      //���׼�¼����
$CloseData[$CloseNum++] = "CloseTradeInputMoneyWnd();";                //���׽���_����Ի���
$CloseData[$CloseNum++] = "CloseNpcDialog();";                         //NPC�Ի���
$CloseData[$CloseNum++] = "EquipEmbedGemWnd_CloseWnd();";              //װ����Ƕ����
$CloseData[$CloseNum++] = "EquipPunchHoleWnd_Close();";                //װ����׽���
$CloseData[$CloseNum++] = "EquipStrengthenWnd_Close();";        			 //װ��ǿ������
$CloseData[$CloseNum++] = "CloseDialog_OK_Cancel();";        			     //�����Ի���
$CloseData[$CloseNum++] = "CloseMisssionHover();";        			       //����׷�ٽ����ȸ�
$CloseData[$CloseNum++] = "CloseStudyLivingSkillWnd();";               //�����_ѧϰ
$CloseData[$CloseNum++] = "ClosePrescription();";                      //�����_����
$CloseData[$CloseNum++] = "ClosePetInfoGui_StudyWnd();";               //�������н���
$CloseData[$CloseNum++] = "ClsePetCheckupWnd();";                      //�����������
$CloseData[$CloseNum++] = "ClsePetStrengthenWnd();";                   //����ǿ������
$CloseData[$CloseNum++] = "ClsePetDecompoundWnd();";                   //������������
$CloseData[$CloseNum++] = "ClsePetHelpWnd();";                         //�����б����
$CloseData[$CloseNum++] = "ClosePetBackToYoungWnd();";                 //���޻�ͯ
$CloseData[$CloseNum++] = "CloseMailWnd();";                           //�ʼ�����
$CloseData[$CloseNum++] = "ClosePlayerUpgradeHintWnd();";              //����������ʾ
$CloseData[$CloseNum++] = "CloseXianZhuanWnd();";                      //��׭¼
$CloseData[$CloseNum++] = "CloseRankWnd();";                           //���а�
$CloseData[$CloseNum++] = "CloseSystemMenu();";                        //ϵͳ�˵�
$CloseData[$CloseNum++] = "CloseBussnessCity();";                      //�̳ǽ���
$CloseData[$CloseNum++] = "CloseSoulGeniusWnd();";                     //Ԫ����Ϣ����

$ScheduleEventId = 0;      //�����¼�ID
$pitchOnIndex = -1;				 //��̯��ѡ��index

$RightDownType = 0;        //�Ҽ������˵�����