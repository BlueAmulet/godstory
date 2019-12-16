//===========================================================================================================
// �ĵ�˵��:�������н�����Ч
// ����ʱ��:2009-7-23
// ������: Batcel
//=========================================================================================================== 

//�����ť����Ч
function PlayClickButtonSound()
{
	PlayOnceSound(U_001);
}

//��/�رս������Ч
function PlayOpenOrCloseGuiSound()
{
	PlayOnceSound(U_002);
}

//����Ƶ������Ϣ(1ϵͳ 2����)
function PlayChatChannelSound(%Type)
{
	if(%Type==1)
	   PlayOnceSound(U_003);
	else if(%Type==2)
	   PlayOnceSound(U_010);
}

//�յ�����������Ϣ(���,����,EMS)
function PlayKindOfInviteSound(%Type)
{
	switch(%Type)
	{
		case 1:PlayOnceSound(U_004);
		case 2:
		case 3:
	}
}
//����ʧ�ܵ���Ч(���,����,��������ȵ�)
function PlayKindOfFailedSound(%Type)
{
	switch(%Type)
	{
		case 1:PlayOnceSound(U_005);
		case 2:
		case 3:
	}
}

//���ֲ����������Ч
function PlayOperationErrorSound()
{
	PlayOnceSound(U_006);
}

//����������Ч(�Լ�����Ͷ�Ա����)
function PlayJoinToTeamSound()
{
	PlayOnceSound(U_007);
}

//������Դ��ɺ���뵽��Ϸ
function PlayEnterIntoGameSound()
{
	PlayOnceSound(U_008);
}

//ѡ���ɫ
function PlayChooseActorSound()
{
	PlayOnceSound(U_009);
}

//��������(NPC����,��Ұ�̯����)
function PlayBuyItemSound()
{
	PlayOnceSound(U_011);
}

//ʹ��ʳ��ָ�HP
function PlayAddHpSound()
{
	PlayOnceSound(U_012);
}

//����MP
function PlayAddMpSound()
{
	PlayOnceSound(U_013);
}

//ʹ�þ���
function PlayUsedReelSound()
{
	PlayOnceSound(U_014);
}

//ʹ�õ���
function PlayUsedItemSound()
{
	PlayOnceSound(U_015);
}

//���ٵ���
function PlayDestroyItemSound()
{
	PlayOnceSound(U_016);
}

//��������(1���� 2���� 3����)
function PlayExchangeItemSound(%type)
{
	switch(%type)
	{
	  case 1: PlayOnceSound(U_017);
	  case 2: PlayOnceSound(U_018);
	  case 3: PlayOnceSound(U_019);
  }
}

//���������Ʒ��
function PlayMonsterDropItemSound()
{
	PlayOnceSound(U_020);
}

//����(1��ȡ,2���)
function PlayMissionSound(%type)
{
	if(%type $=1)
	 PlayOnceSound(U_021);
	else
	 PlayOnceSound(U_022);
}

//����ܺϳɵ���
function PlayLiveSkillItemSound()
{
	PlayOnceSound(U_023);
}

//����µļ���
function PlayGetNewSkillSound()
{
	PlayOnceSound(U_024);
}
//��������
function PlayPlayerLeverUpSound()
{
	PlayOnceSound(U_025);
}

//Ԫ����Ϊ����
function PlaySoulAdvanceSound()
{
	PlayOnceSound(U_026);
} 
//��������
function PlayPetLevelUpSound()
{
	PlayOnceSound(U_027);
}
//װ��ǿ��(1Ϊ�ɹ� 2ʧ��)
function PlayItemConsolidateSound(%type)
{
	if(%type $=1)
	  PlayOnceSound(U_028);
	else
	  PlayOnceSound(U_029);
}