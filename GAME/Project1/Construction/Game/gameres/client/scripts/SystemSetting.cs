//===========================================================================================================
// �ĵ�˵��:ϵͳ���ò���
// ����ʱ��:2009-3-24
// ������: soar
//=========================================================================================================== 

function CloseAllGui()
{
  if($IsDirty != 0)
  {
  	for(%i = 0; %i <= $CloseNum; %i++)
  	{
  		eval($CloseData[%i]);
  	}
  	$IsDirty = 0;
  	return false;
  }
  else
  	return true;
}

//----------------------ϵͳ����----------------------
//----------------------------------------------------
$OpenSysSetOnce = 0;
$WindowModel = 0; //0Ϊ���� 1Ϊȫ��
$ScreenPlayerNum = 0; //ͬ������
$SkillEffect = 0;   //������Ч(0����ʾ,1��ʾ)
$WeaponEffect = 0;   //װ����Ч(0����ʾ,1��ʾ)
$MusicVolume = 0;    //��������
$SoundVolume = 0;    //��Ч����
$Brightness  = 0;    //����
$Contrast    = 0;    //�Աȶ�

$TexturePrecision = 0; //��ͼ����
$ShadowDetail = 0;     //��Ӱϸ��
$SceneDetail  = 0;     //����ϸ��

$StaticAnimation = 0;  //�������
$FSBrightness = 0;     //ȫ������
$WaterReflaction = 0;  //ˮ�淴��
$CoastEffect =0 ;      //����Ч��

function OpenSysSettingWnd()
{
	SystemSettingWndGui.setVisible(1);
	GameMainWndGui_Layer3.PushToBack(SystemSettingWndGui);
	SystemMenuGui.setvisible(0);
	$IsDirty++;
	if($OpenSysSetOnce==0) 
	   $OpenSysSetOnce=1;
	else
	   return;
	SysSettingMenu_Resolving.clear();
	SysSettingMenu_FSAA.clear();
	getDisplayModelList();
	SysSettingMenu_FSAA.add("32λ  0x",1);
	SysSettingMenu_FSAA.add("32λ  2x",2);
	SysSettingMenu_FSAA.add("32λ  4x",3);
	SysSettingMenu_FSAA.add("32λ  8x",4);
	SysSettingMenu_FSAA.add("32λ  16x",5);
	SysSettingMenu_FSAA.setSelected(1);
	%CurrentModel = getCurrentModel();
	SysSettingMenu_Resolving.setSelected(SysSettingMenu_Resolving.findtext(%CurrentModel));
	getDisplayModelState();
	getEntironmentState();
}

function CloseSysSettingWnd()
{
	SystemSettingWndGui.setVisible(0);
	$IsDirty--;
}

function getInitModelState(%IsFull,%playCount,%IsSkillEffect,%IsWeaponEffect,%BrightnessVal,%ContrastVal)
{
	$WindowModel=%IsFull;
	$ScreenPlayerNum = %playCount;
	$SkillEffect = %IsSkillEffect;
	$WeaponEffect = %IsWeaponEffect;
	$Brightness = %BrightnessVal;
	$Contrast = %ContrastVal;
	
	if(%IsFull == 0)
	  SysSettingCheckBtn_Window.setStateOn(1);
	else
	  SysSettingCheckBtn_FullScreen.setStateOn(1);
	SysSetSlider_ScreenPlayer.setValue(%playCount);
	SysSettingCheckBtn_SkillEffect.SetStateOn(%IsSkillEffect);
	SysSettingCheckBtn_WeaponEffect.setStateOn(%IsWeaponEffect);
	SysSetSlider_Lighteness.setValue(%BrightnessVal);
	SysSetSlider_Contrast.setValue(%ContrastVal);
}

function getInitEntironmentValue(%Texture,%Shadow,%Scene,%Animation,%FullFSB,%Water,%Coast)
{
	$TexturePrecision = %Texture; 
  $ShadowDetail = %Shadow;     
  $SceneDetail  = %Scene;     

  $StaticAnimation = %Animation;  
  $FSBrightness = %FullFSB;     
  $WaterReflaction = %Water;  
  $CoastEffect =%Coast;
  
  SysSetSlider_TexturePrecision.setValue(%Texture);
  SysSetSlider_ShadowDetail.setValue(%Shadow);
  SysSetSlider_SceneDetail.setValue(%Scene);
  
  SysSettingCheckBtn_StaticAnimation.setStateOn(%Animation);
  SysSettingCheckBtn_FSBrightness.setStateOn(%FullFSB);
  SysSettingCheckBtn_WaterReflaction.setStateOn(%Water);
  SysSettingCheckBtn_CoastEffect.setStateOn(%Coast);
  
  //��Ч
  SysSetSlider_SoundVolume.setValue(1.0);
  SysSetSlider_MusicVolume.setValue(1.0);     
}

function getResolutionList(%text,%i)
{
	SysSettingMenu_Resolving.add(%text,%i);
}

function getFSAAList(%text,%i)
{
	SysSettingMenu_FSAA.add(%text,%i);
}

function Quiet()
{	
	if(SysSettingCheckBtn_Quiet.IsStateOn())
	{
	   sfxSetBackChannelGroupVolume(0,0);
	   sfxSetBackChannelGroupVolume(1,0);
	   sfxSetBackChannelGroupVolume(2,0);
	   sfxSetBackChannelGroupVolume(3,0);
	}
	else
	{
		if(SysSettingCheckBtn_OpenMusic.IsStateOn())
		{
			sfxSetBackChannelGroupVolume(0,SysSetSlider_MusicVolume.getValue());
		}
		
		if(SysSettingCheckBtn_OpenSound.IsStateOn())
		{
			%Value=SysSetSlider_SoundVolume.getValue();
	    sfxSetBackChannelGroupVolume(1,%Value);
	    sfxSetBackChannelGroupVolume(2,%Value);
	    sfxSetBackChannelGroupVolume(3,%Value);
		}
	}
}

function OpenMusic()
{
	if(SysSettingCheckBtn_Quiet.IsStateOn())
      return;
  if(SysSettingCheckBtn_OpenMusic.IsStateOn())
    sfxSetBackChannelGroupVolume(0,SysSetSlider_MusicVolume.getValue());
  else
    sfxSetBackChannelGroupVolume(0,0);
}
function OpenSound()
{
	if(SysSettingCheckBtn_Quiet.IsStateOn())
      return;
  if(SysSettingCheckBtn_OpenSound.IsStateOn())
		{
			%Value=SysSetSlider_SoundVolume.getValue();
	    sfxSetBackChannelGroupVolume(1,%Value);
	    sfxSetBackChannelGroupVolume(2,%Value);
	    sfxSetBackChannelGroupVolume(3,%Value);
		}
	  else
	  {
	  	sfxSetBackChannelGroupVolume(1,0);
	    sfxSetBackChannelGroupVolume(2,0);
	    sfxSetBackChannelGroupVolume(3,0);
	  }   
  
}

function MusicVolume()
{
	//�������ϻ��߿�������û�й���
	if(SysSettingCheckBtn_Quiet.IsStateOn() || !SysSettingCheckBtn_OpenMusic.IsStateOn())
	  return;
	%Value=SysSetSlider_MusicVolume.getValue();
	sfxSetBackChannelGroupVolume(0,%Value);
}

function SoundVolume()
{
	//�������ϻ��߿�����Чû�й���
	if(SysSettingCheckBtn_Quiet.IsStateOn() || !SysSettingCheckBtn_OpenMusic.IsStateOn())
	  return;	
	%Value=SysSetSlider_SoundVolume.getValue();
	sfxSetBackChannelGroupVolume(1,%Value);
	sfxSetBackChannelGroupVolume(2,%Value);
	sfxSetBackChannelGroupVolume(3,%Value);
}

function ClickMusicVolumeBtn(%AddOrSub)
{
	  %Value=SysSetSlider_MusicVolume.getValue();
	  if(%AddOrSub ==0)
	   {
	   	SysSetSlider_MusicVolume.setValue(%Value-0.1);   	
	   	sfxSetBackChannelGroupVolume(0,%Value-0.1);
	   }
	  else
	   {
	   	SysSetSlider_MusicVolume.setValue(%Value+0.1);
	  	sfxSetBackChannelGroupVolume(0,%Value+0.1);
	   }
}

function ClickSoundVolumeBtn(%AddOrSub)
{
	%Value=SysSetSlider_SoundVolume.getValue();
	  if(%AddOrSub ==0)
	   {
	   	SysSetSlider_SoundVolume.setValue(%Value-0.1);   	
	   	sfxSetBackChannelGroupVolume(1,%Value-0.1);
	   	sfxSetBackChannelGroupVolume(2,%Value-0.1);
	   	sfxSetBackChannelGroupVolume(3,%Value-0.1);
	   }
	  else
	   {
	   	SysSetSlider_SoundVolume.setValue(%Value+0.1);
	  	sfxSetBackChannelGroupVolume(1,%Value+0.1);
	   	sfxSetBackChannelGroupVolume(2,%Value+0.1);
	   	sfxSetBackChannelGroupVolume(3,%Value+0.1);
	   }
}


function SystemSettingOk()
{
	%IsFull =0;
	if(SysSettingCheckBtn_Window.IsStateOn())
	   %IsFull = 0;
	else
	   %IsFull = 1;
		$WindowModel= %IsFull;
	
	  //ͬ������
	 if($ScreenPlayerNum != SysSetSlider_ScreenPlayer.getValue())
	  {	  	
	  	$ScreenPlayerNum = SysSetSlider_ScreenPlayer.getValue();
	  	setVisiblePlayerCount($ScreenPlayerNum);
	  }	
	
	  //������Ч
	 if($SkillEffect != SysSettingCheckBtn_SkillEffect.IsStateOn())
	  {
	  	$SkillEffect = SysSettingCheckBtn_SkillEffect.IsStateOn();
	  	setSkillEffect($SkillEffect);
	  }
	  
	  //װ����Ч
	 if($WeaponEffect != SysSettingCheckBtn_WeaponEffect.IsStateOn())
	  {
	  	$WeaponEffect = SysSettingCheckBtn_WeaponEffect.IsStateOn();
	  	setWeaponEffect($WeaponEffect);
	  }
	  
	  //����
	 if($Brightness != SysSetSlider_Lighteness.getValue())
	  {
	   $Brightness = SysSetSlider_Lighteness.getValue();
	   	setBrightness($Brightness);
	  }
	  
	  //�Աȶ�
	 if($Contrast != SysSetSlider_Contrast.getValue())
	 {
	 	$Contrast = SysSetSlider_Contrast.getValue();
	 	setContrast($Contrast);
	 }
	 
	 //��ͼ����
	 if($TexturePrecision != SysSetSlider_TexturePrecision.getValue())
	 {
	 	$TexturePrecision=SysSetSlider_TexturePrecision.getValue();
	 	setTexFilterType($TexturePrecision);
	 }
	 
	 //��Ӱϸ��
	 if($ShadowDetail != SysSetSlider_ShadowDetail.getValue())
	  {
	  	$ShadowDetail =  SysSetSlider_ShadowDetail.getValue();
	  	setShadowLevel($ShadowDetail);
	  }
	 
	 //����ϸ��
	 if($SceneDetail !=SysSetSlider_SceneDetail.getValue())
	  {
	  	$SceneDetail = SysSetSlider_SceneDetail.getValue();
	  	setTinyStaticLevel($SceneDetail);
	  }  
	  
	  //�������
	  if($StaticAnimation != SysSettingCheckBtn_StaticAnimation.IsStateOn())
	  {
	  	$StaticAnimation = SysSettingCheckBtn_StaticAnimation.IsStateOn();
	  	setStaticAnimation($StaticAnimation);
	  }
	  
	  //ȫ������
	  if($FSBrightness != SysSettingCheckBtn_FSBrightness.IsStateOn())
	  {
	  	$FSBrightness = SysSettingCheckBtn_FSBrightness.IsStateOn();
	  	setenableBrightness($FSBrightness);
	  }
	  
	  //ˮ�淴��
	  if($WaterReflaction != SysSettingCheckBtn_WaterReflaction.IsStateOn())
	  {
	  	$WaterReflaction = SysSettingCheckBtn_WaterReflaction.IsStateOn();
	  	setWaterReflaction($WaterReflaction);
	  }
	  
	  //����Ч��(��ʱû��)
	  if($CoastEffect !=SysSettingCheckBtn_CoastEffect.IsStateOn())
	  {
	  	$CoastEffect =SysSettingCheckBtn_CoastEffect.IsStateOn();
	  	
	  }
}



function  GameSettingOK()
{
	
}


//ȷ������
function SettingOK()
{
	 SystemSettingOk();
	 GameSettingOK();
	 CloseSysSettingWnd();
}

function SettingCancel()
{
	SystemSettingCancel();
	GameSetttingCancel();
	CloseSysSettingWnd();
}