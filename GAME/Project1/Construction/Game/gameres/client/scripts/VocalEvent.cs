//--- OBJECT WRITE BEGIN ---
//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
// Create: thinking
// Date:   13/9/2009
//-----------------------------------------------------------------------------
//���
function cancleFetchTimeProgress()
{
	//����
	if(guiVocalProgressBar.isVisible())
	{                                    
		VocalProgressBarCtrl.setProgressStyle(2);
		VocalProgressBarCtrl.setShowProgressLayer2(true);
		VocalProgressBarCtrl.setText("�����");
		$ScheduleEventId = Schedule(500,0,"CloseProgress");
	}
	//����
	if(guiVocalProgressBar1.isVisible())
	{
		VocalProgressBarCtrl1.setProgressStyle(2);
		VocalProgressBarCtrl1.setShowProgressLayer2(true);
		VocalProgressBarCtrl1.setText("�����");
		$ScheduleEventId = Schedule(500,0,"CloseProgress");
	}
	
}
//�ر�
function CloseProgress()
{
	if(guiVocalProgressBar.isVisible())
	  closeSkillProgress();
	
	if(guiVocalProgressBar1.isVisible())
	  closeCommonProgress();

	clearVocalStatus();
}
//����
function CloseFetchTimeProgress()
{
	%closeTime = 500;
	$ScheduleEventId = Schedule(%closeTime,0,"CloseProgress");
	
	if(guiVocalProgressBar.isVisible())
	  VocalProgressBarCtrl.setFadeOutTime(%closeTime);//���õ���ʱ��
	  
	if(guiVocalProgressBar1.isVisible())
	  VocalProgressBarCtrl1.setFadeOutTime(%closeTime);
}
//��ʼ
function InitFetchTimeProgress(%time,%type,%name)
{
	if(isEventPending($ScheduleEventId))
	{
		cancel($ScheduleEventId);
		CloseProgress();
	}
	
	//��������
	setVocalStatus(true,%type);
	
	//��ȡ����״̬
	%status = getVocalStatus();
	//��ȡ��������
	%vocalType = getVocalStatusType();
	
	if(%status && %vocalType == %type)
	{
		 if(%type == 7)
	   {
	     openSkillProgress(%time,%name);
	   }
	   else
	   {
		   if(%name $= "")
		     %name = getVocalStatusName();
		  
	     openCommonProgress(%time,%name);
	   }
	}
}

//��ʼ
function InitFetchTimeAnimProgress(%time,%type,%name,%action)
{
	if(isEventPending($ScheduleEventId))
	{
		cancel($ScheduleEventId);
		CloseProgress();
	}
	
	//��������
	setVocalStatus(true,%type,%action);
	
	//��ȡ����״̬
	%status = getVocalStatus();
	//��ȡ��������
	%vocalType = getVocalStatusType();
	
	if(%status && %vocalType == %type)
	{
		 if(%type == 7)
	   {
	     openSkillProgress(%time,%name);
	   }
	   else
	   {
		   if(%name $= "")
		     %name = getVocalStatusName();
		  
	     openCommonProgress(%time,%name);
	   }
	}
}

//����������
function openSkillProgress(%time,%name)
{
	GameMainWndGui_Layer5.pushToBack(guiVocalProgressBar);
  guiVocalProgressBar.setVisible(true);
  
  VocalProgressBarCtrl.setText(%name);
  VocalProgressBarCtrl.setProgressStyle(0);
  VocalProgressBarCtrl.startProgress(%time);
  VocalProgressBarCtrl.setFadeOutTime(0);
}

function closeSkillProgress()
{
	guiVocalProgressBar.setVisible(false);
	VocalProgressBarCtrl.setShowProgressLayer2(false);
	VocalProgressBarCtrl.setText("");
	VocalProgressBarCtrl.endProgress();
}

//������������������ܵȣ�
function openCommonProgress(%time,%name)
{
	GameMainWndGui_Layer5.pushToBack(guiVocalProgressBar1);
  guiVocalProgressBar1.setVisible(true);
  
  VocalProgressBarCtrl1.setText(%name);
  VocalProgressBarCtrl1.setProgressStyle(0);
  VocalProgressBarCtrl1.startProgress(%time);
  VocalProgressBarCtrl1.setFadeOutTime(0);
}

function closeCommonProgress()
{
	guiVocalProgressBar1.setVisible(false);
	VocalProgressBarCtrl1.setShowProgressLayer2(false);
	VocalProgressBarCtrl1.setText("");
	VocalProgressBarCtrl1.endProgress();
}

//��Ҫ�ָ��İ�ť����
function recoveryButtonState(%statusType)
{
  switch(%statusType)
  {
  	case 1:    //�ϳɡ�����
  	setProduceButton(1);
  	case 3:		// �ٻ�����
  	PetInfoWnd_RefreshBtn();
  	case 6:		// �ٻ����
  	PetInfoWnd_RefreshMountBtn();
  }
}
//--- OBJECT WRITE END ---