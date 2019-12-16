//--- OBJECT WRITE BEGIN ---
//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
// Create: thinking
// Date:   13/9/2009
//-----------------------------------------------------------------------------
//打断
function cancleFetchTimeProgress()
{
	//技能
	if(guiVocalProgressBar.isVisible())
	{                                    
		VocalProgressBarCtrl.setProgressStyle(2);
		VocalProgressBarCtrl.setShowProgressLayer2(true);
		VocalProgressBarCtrl.setText("被打断");
		$ScheduleEventId = Schedule(500,0,"CloseProgress");
	}
	//公共
	if(guiVocalProgressBar1.isVisible())
	{
		VocalProgressBarCtrl1.setProgressStyle(2);
		VocalProgressBarCtrl1.setShowProgressLayer2(true);
		VocalProgressBarCtrl1.setText("被打断");
		$ScheduleEventId = Schedule(500,0,"CloseProgress");
	}
	
}
//关闭
function CloseProgress()
{
	if(guiVocalProgressBar.isVisible())
	  closeSkillProgress();
	
	if(guiVocalProgressBar1.isVisible())
	  closeCommonProgress();

	clearVocalStatus();
}
//结束
function CloseFetchTimeProgress()
{
	%closeTime = 500;
	$ScheduleEventId = Schedule(%closeTime,0,"CloseProgress");
	
	if(guiVocalProgressBar.isVisible())
	  VocalProgressBarCtrl.setFadeOutTime(%closeTime);//设置淡入时间
	  
	if(guiVocalProgressBar1.isVisible())
	  VocalProgressBarCtrl1.setFadeOutTime(%closeTime);
}
//开始
function InitFetchTimeProgress(%time,%type,%name)
{
	if(isEventPending($ScheduleEventId))
	{
		cancel($ScheduleEventId);
		CloseProgress();
	}
	
	//设置吟唱
	setVocalStatus(true,%type);
	
	//获取吟唱状态
	%status = getVocalStatus();
	//获取吟唱类型
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

//开始
function InitFetchTimeAnimProgress(%time,%type,%name,%action)
{
	if(isEventPending($ScheduleEventId))
	{
		cancel($ScheduleEventId);
		CloseProgress();
	}
	
	//设置吟唱
	setVocalStatus(true,%type,%action);
	
	//获取吟唱状态
	%status = getVocalStatus();
	//获取吟唱类型
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

//技能吟唱条
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

//公共吟唱条（如生活技能等）
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

//需要恢复的按钮操作
function recoveryButtonState(%statusType)
{
  switch(%statusType)
  {
  	case 1:    //合成、制造
  	setProduceButton(1);
  	case 3:		// 召唤宠物
  	PetInfoWnd_RefreshBtn();
  	case 6:		// 召唤骑宠
  	PetInfoWnd_RefreshMountBtn();
  }
}
//--- OBJECT WRITE END ---