//===========================================================================================================
// 文档说明:播放所有界面音效
// 创建时间:2009-7-23
// 创建人: Batcel
//=========================================================================================================== 

//点击按钮的音效
function PlayClickButtonSound()
{
	PlayOnceSound(U_001);
}

//打开/关闭界面的音效
function PlayOpenOrCloseGuiSound()
{
	PlayOnceSound(U_002);
}

//聊天频道的消息(1系统 2密聊)
function PlayChatChannelSound(%Type)
{
	if(%Type==1)
	   PlayOnceSound(U_003);
	else if(%Type==2)
	   PlayOnceSound(U_010);
}

//收到各种邀请消息(组队,交易,EMS)
function PlayKindOfInviteSound(%Type)
{
	switch(%Type)
	{
		case 1:PlayOnceSound(U_004);
		case 2:
		case 3:
	}
}
//各种失败的音效(组队,交易,创建人物等等)
function PlayKindOfFailedSound(%Type)
{
	switch(%Type)
	{
		case 1:PlayOnceSound(U_005);
		case 2:
		case 3:
	}
}

//各种操作错误的音效
function PlayOperationErrorSound()
{
	PlayOnceSound(U_006);
}

//加入队伍的音效(自己加入和队员加入)
function PlayJoinToTeamSound()
{
	PlayOnceSound(U_007);
}

//加载资源完成后进入到游戏
function PlayEnterIntoGameSound()
{
	PlayOnceSound(U_008);
}

//选择角色
function PlayChooseActorSound()
{
	PlayOnceSound(U_009);
}

//买卖道具(NPC买卖,玩家摆摊购买)
function PlayBuyItemSound()
{
	PlayOnceSound(U_011);
}

//使用食物恢复HP
function PlayAddHpSound()
{
	PlayOnceSound(U_012);
}

//增加MP
function PlayAddMpSound()
{
	PlayOnceSound(U_013);
}

//使用卷轴
function PlayUsedReelSound()
{
	PlayOnceSound(U_014);
}

//使用道具
function PlayUsedItemSound()
{
	PlayOnceSound(U_015);
}

//销毁道具
function PlayDestroyItemSound()
{
	PlayOnceSound(U_016);
}

//更换道具(1防具 2武器 3首饰)
function PlayExchangeItemSound(%type)
{
	switch(%type)
	{
	  case 1: PlayOnceSound(U_017);
	  case 2: PlayOnceSound(U_018);
	  case 3: PlayOnceSound(U_019);
  }
}

//怪物掉落物品包
function PlayMonsterDropItemSound()
{
	PlayOnceSound(U_020);
}

//任务(1接取,2完成)
function PlayMissionSound(%type)
{
	if(%type $=1)
	 PlayOnceSound(U_021);
	else
	 PlayOnceSound(U_022);
}

//生活技能合成道具
function PlayLiveSkillItemSound()
{
	PlayOnceSound(U_023);
}

//获得新的技能
function PlayGetNewSkillSound()
{
	PlayOnceSound(U_024);
}
//人物升级
function PlayPlayerLeverUpSound()
{
	PlayOnceSound(U_025);
}

//元神修为提升
function PlaySoulAdvanceSound()
{
	PlayOnceSound(U_026);
} 
//宠物升级
function PlayPetLevelUpSound()
{
	PlayOnceSound(U_027);
}
//装备强化(1为成功 2失败)
function PlayItemConsolidateSound(%type)
{
	if(%type $=1)
	  PlayOnceSound(U_028);
	else
	  PlayOnceSound(U_029);
}