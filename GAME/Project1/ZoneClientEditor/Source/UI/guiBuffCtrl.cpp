
#include "guiBuffCtrl.h"
#include "Gameplay/GameObjects/Buff.h"
#include "Gameplay/GameObjects/BuffData.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/ClientGameplayState.h"
#include "console/consoleTypes.h"
#include "Gameplay/Team/ClientTeam.h"
#include "Gameplay/ClientPlayerManager.h"

IMPLEMENT_CONOBJECT(GuiBuffCtrl);

GuiBuffCtrl::GuiBuffCtrl()
{
	bHasTime = false;
	bIsNewline = false;
	mObjType = 0;
	mTeammateY = 100;
	pObjectBuff = NULL;
	mBuffFilePath = StringTable->insert("");
	mFrameName = StringTable->insert("");
	mFrameTexture = NULL;
	mBuffBoundary.set(0,0,1,1);
	mBuffNum	= 0;
	mDeBuffNum	= 0;
	mSpaceNum = 0;
	mRemainTimeExtext.set(1,1);
	mAddendExtext.set(1,1);
	mTempChangeY = 0;
	for(S32 i = 0; i < 16; i++)
	{
		mBuffTexture[i] = NULL;
		mDeBuffTexture[i] = NULL;
	}
	dMemset(mBuffAddendNum,		0, sizeof(mBuffAddendNum));
	dMemset(mDeBuffAddendNum,	0, sizeof(mDeBuffAddendNum));
	dMemset(cBuffRemainTime,	0, sizeof(cBuffRemainTime));
	dMemset(cDeBuffRemainTime,	0, sizeof(cDeBuffRemainTime));
	dMemset(cBuffRemainTimeBak,	0, sizeof(cBuffRemainTimeBak));
	dMemset(cDeBuffRemainTimeBak,	0, sizeof(cDeBuffRemainTimeBak));
	dMemset(mBuffFileName,		0, sizeof(mBuffFileName));
	dMemset(mDeBuffFileName,	0, sizeof(mDeBuffFileName));
}

void GuiBuffCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addField("BuffBoundary",          TypeRectI,   Offset(mBuffBoundary,           GuiBuffCtrl));
	addField("SpaceNum",              TypeRectI,   Offset(mSpaceNum,               GuiBuffCtrl));
	addField("BuffFilePath",          TypeString,  Offset(mBuffFilePath,           GuiBuffCtrl));
	addField("ObjType",               TypeS32,     Offset(mObjType,                GuiBuffCtrl));
	addField("HasTime",               TypeBool,    Offset(bHasTime,                GuiBuffCtrl));
	addField("FrameName",             TypeString,  Offset(mFrameName,              GuiBuffCtrl));
	addField("RemainTimeExtext",      TypePoint2I, Offset(mRemainTimeExtext,       GuiBuffCtrl));
	addField("AddendExtext",          TypePoint2I, Offset(mAddendExtext,           GuiBuffCtrl));
}

bool GuiBuffCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	mRealBuffBoundary = mBuffBoundary;

	return true;
}

void GuiBuffCtrl::onSleep()
{
	Parent::onSleep();
}

void GuiBuffCtrl::onPreRender()
{
	if (isVisible())
	{
		if (mObjType == 1)
		{
			pObjectBuff = g_ClientGameplayState->GetControlPlayer();
			if (!pObjectBuff)
				return;
		}
		else if (mObjType == 2)
		{
			Player *player = g_ClientGameplayState->GetControlPlayer();
			if (!player)
				return;

			pObjectBuff = player->GetTarget();
			if (!pObjectBuff)
				return;
		}
		else if (mObjType == 3)
		{
			if (CLIENT_TEAM->HasTeam())
			{
				for (int i = 1; i <= CLIENT_TEAM->GetPlayerList()->size()-1; i++)
				{
					pObjectBuff = PLAYER_MGR->GetLocalPlayer(CLIENT_TEAM->GetPlayerByIndex(i-1,0));
					if (!pObjectBuff)
						return;
				}
			}
		}
	}
	m_DirtyFlag = isUpDate();
}

void GuiBuffCtrl::onRender( Point2I offset, const RectI &updateRect )
{
	m_DirtyFlag = false;

	if( !pObjectBuff )
		return ;

	if (bHasTime && bIsNewline)
		mTempChangeY = (mBuffBoundary.extent.y + mRemainTimeExtext.y) * 2;
	else if (bHasTime && !bIsNewline)
		mTempChangeY = mBuffBoundary.extent.y + mRemainTimeExtext.y;
	else if (!bHasTime && bIsNewline)
		mTempChangeY = mBuffBoundary.extent.y * 2;
	else if (!bHasTime && !bIsNewline)
		mTempChangeY = mBuffBoundary.extent.y;

	if (mObjType == 3)
	{
		if (CLIENT_TEAM->HasTeam())
		{
			for (int j = 1; j <= CLIENT_TEAM->GetPlayerList()->size()-1; j++ )
			{
				const BuffTable::BuffTableType& bufflist = pObjectBuff->GetBuffTable().GetBuff();
				BuffTable::BuffTableType::const_iterator it = bufflist.begin();
				for(int i = 0;it != bufflist.end();it++,i++)
				{
					if (mBuffTexture[i])
					{
						GFX->getDrawUtil()->clearBitmapModulation();
						RectI rect(offset + mRealBuffBoundary.point,mRealBuffBoundary.extent);
						GFX->getDrawUtil()->drawBitmapStretch(mBuffTexture[i],rect);
						GFX->getDrawUtil()->drawBitmapStretch(mFrameTexture,rect);
					}

					if (mBuffAddendNum[i])
					{
						GFX->getDrawUtil()->clearBitmapModulation();
						GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
						Point2I point(offset + mRealBuffBoundary.point + (mRealBuffBoundary.extent - mAddendExtext));
						renderJustifiedText(point,mAddendExtext,(char*)mBuffAddendNum[i]);
					}

					if (cBuffRemainTime[i])
					{
						GFX->getDrawUtil()->clearBitmapModulation();
						GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
						Point2I point(offset + mRealBuffBoundary.point + Point2I(0,mRealBuffBoundary.extent.y));
						renderJustifiedText(point,mRemainTimeExtext,(char*)cBuffRemainTime[i]);
					}
					mRealBuffBoundary.point.x += mRealBuffBoundary.extent.x + mSpaceNum;
					if (i == 7)
					{
						mRealBuffBoundary.point.x = mBuffBoundary.point.x;
						if (bHasTime)
							mRealBuffBoundary.point.y += mBuffBoundary.extent.y + mRemainTimeExtext.y;
						else
							mRealBuffBoundary.point.y += mBuffBoundary.extent.y;
					}
				}

				mRealBuffBoundary = mBuffBoundary;
				mRealBuffBoundary.point.y += mTempChangeY;

				const BuffTable::BuffTableType& deBufflist = pObjectBuff->GetBuffTable().GetDebuff();
				BuffTable::BuffTableType::const_iterator dit = deBufflist.begin();
				for(int i = 0;dit != deBufflist.end();dit++,i++)
				{
					if (mDeBuffTexture[i])
					{
						GFX->getDrawUtil()->clearBitmapModulation();
						RectI rect(offset + mRealBuffBoundary.point,mRealBuffBoundary.extent);
						GFX->getDrawUtil()->drawBitmapStretch(mDeBuffTexture[i],rect);
						GFX->getDrawUtil()->drawBitmapStretch(mFrameTexture,rect);
					}

					if (mDeBuffAddendNum[i])
					{
						GFX->getDrawUtil()->clearBitmapModulation();
						GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
						Point2I point(offset + mRealBuffBoundary.point + (mRealBuffBoundary.extent - mAddendExtext));
						renderJustifiedText(point,mAddendExtext,(char*)mDeBuffAddendNum[i]);
					}

					if (cDeBuffRemainTime[i])
					{
						GFX->getDrawUtil()->clearBitmapModulation();
						GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
						Point2I point(offset + mRealBuffBoundary.point + Point2I(0,mRealBuffBoundary.extent.y));
						renderJustifiedText(point,mRemainTimeExtext,(char*)cDeBuffRemainTime[i]);
					}
					mRealBuffBoundary.point.x += mRealBuffBoundary.extent.x + mSpaceNum;
					if (i == 7)
					{
						mRealBuffBoundary.point.x = mBuffBoundary.point.x;
						if (bHasTime)
							mRealBuffBoundary.point.y += mBuffBoundary.extent.y + mRemainTimeExtext.y;
						else
							mRealBuffBoundary.point.y += mBuffBoundary.extent.y;
					}
				}
				mRealBuffBoundary = mBuffBoundary;

				offset.y += mTeammateY;
			}
		}
	}
	else
	{
		const BuffTable::BuffTableType& bufflist = pObjectBuff->GetBuffTable().GetBuff();
		BuffTable::BuffTableType::const_iterator it = bufflist.begin();
		for(int i = 0;it != bufflist.end();it++,i++)
		{
			if (mBuffTexture[i])
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				RectI rect(offset + mRealBuffBoundary.point,mRealBuffBoundary.extent);
				GFX->getDrawUtil()->drawBitmapStretch(mBuffTexture[i],rect);
				GFX->getDrawUtil()->drawBitmapStretch(mFrameTexture,rect);
			}

			if (mBuffAddendNum[i])
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
				Point2I point(offset + mRealBuffBoundary.point + (mRealBuffBoundary.extent - mAddendExtext));
				renderJustifiedText(point,mAddendExtext,(char*)mBuffAddendNum[i]);
			}

			if (cBuffRemainTime[i])
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
				Point2I point(offset + mRealBuffBoundary.point + Point2I(0,mRealBuffBoundary.extent.y));
				renderJustifiedText(point,mRemainTimeExtext,(char*)cBuffRemainTime[i]);
			}
			mRealBuffBoundary.point.x += mRealBuffBoundary.extent.x + mSpaceNum;
			if (i == 7)
			{
				mRealBuffBoundary.point.x = mBuffBoundary.point.x;
				if (bHasTime)
					mRealBuffBoundary.point.y += mBuffBoundary.extent.y + mRemainTimeExtext.y;
				else
					mRealBuffBoundary.point.y += mBuffBoundary.extent.y;
			}
		}

		mRealBuffBoundary = mBuffBoundary;
		mRealBuffBoundary.point.y += mTempChangeY;

		const BuffTable::BuffTableType& deBufflist = pObjectBuff->GetBuffTable().GetDebuff();
		BuffTable::BuffTableType::const_iterator dit = deBufflist.begin();
		for(int i = 0;dit != deBufflist.end();dit++,i++)
		{
			if (mDeBuffTexture[i])
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				RectI rect(offset + mRealBuffBoundary.point,mRealBuffBoundary.extent);
				GFX->getDrawUtil()->drawBitmapStretch(mDeBuffTexture[i],rect);
				GFX->getDrawUtil()->drawBitmapStretch(mFrameTexture,rect);
			}

			if (mDeBuffAddendNum[i])
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
				Point2I point(offset + mRealBuffBoundary.point + (mRealBuffBoundary.extent - mAddendExtext));
				renderJustifiedText(point,mAddendExtext,(char*)mDeBuffAddendNum[i]);
			}
			
			if (cDeBuffRemainTime[i])
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColor );
				Point2I point(offset + mRealBuffBoundary.point + Point2I(0,mRealBuffBoundary.extent.y));
				renderJustifiedText(point,mRemainTimeExtext,(char*)cDeBuffRemainTime[i]);
			}
			mRealBuffBoundary.point.x += mRealBuffBoundary.extent.x + mSpaceNum;
			if (i == 7)
			{
				mRealBuffBoundary.point.x = mBuffBoundary.point.x;
				if (bHasTime)
					mRealBuffBoundary.point.y += mBuffBoundary.extent.y + mRemainTimeExtext.y;
				else
					mRealBuffBoundary.point.y += mBuffBoundary.extent.y;
			}
		}
		mRealBuffBoundary = mBuffBoundary;
	}
}

bool GuiBuffCtrl::isUpDate()
{
	if (!pObjectBuff)
		return false;

	bool dirty = m_DirtyFlag;

	if (!mFrameTexture)
	{
		if (mFrameName[0] != '\0')
		{
			mFrameTexture = GFXTexHandle(mFrameName,&GFXDefaultStaticDiffuseProfile,true);
			dirty = true;
		}
	}

	//----------------------------------------------BUFF---------------------------------------------------
	char buffFileName[256];

	const BuffTable::BuffTableType& bufflist = pObjectBuff->GetBuffTable().GetBuff();
	BuffTable::BuffTableType::const_iterator it = bufflist.begin();
	if (mBuffNum != bufflist.size())
		dirty = true;
	mBuffNum = bufflist.size();
	for(int i = 0;it != bufflist.end();it++,i++)
	{
		if (mObjType == 3)
		{
			if (i > 7)
				break;
		}
		else
		{
			if (i > 7)
				bIsNewline = true;
			else
				bIsNewline = false;
		}

		sprintf( buffFileName, "%s%s", mBuffFilePath, (*it)->m_BuffData->GetIcon());

		if ( dStrcmp((char*)mBuffFileName[i],(char*)buffFileName) )
		{
			dStrcpy(mBuffFileName[i],256,buffFileName);

			mBuffTexture[i] = GFXTexHandle(mBuffFileName[i],&GFXDefaultStaticDiffuseProfile,true);
			dirty = true;
		}
		
		if (!mBuffTexture[i])
		{
			sprintf( buffFileName, "%s%s", mBuffFilePath, "icon_unknown");

			if (dStrcmp((char*)mBuffFileName[i],(char*)buffFileName))
			{
				dStrcpy(mBuffFileName[i],256,buffFileName);

				mBuffTexture[i] = GFXTexHandle(mBuffFileName[i],&GFXDefaultStaticDiffuseProfile,true);
				dirty = true;
			}
		}

		if (!mBuffTexture[i])
			return false;
		
		if ((*it)->m_BuffData->IsFlags(BuffData::Flags_ShowCount))
		{
			char mTempAddendNum[32] = {0};
			dSprintf(mTempAddendNum,sizeof(mTempAddendNum),"%d",(*it)->m_Count);
			if (!mBuffAddendNum[i])
			{
				dSprintf(mBuffAddendNum[i],sizeof(mBuffAddendNum[i]),"%s",mTempAddendNum);
				dirty = true;
			}
			if (dStrcmp((char*)mBuffAddendNum[i],(char*)mTempAddendNum))
			{
				dSprintf(mBuffAddendNum[i],sizeof(mBuffAddendNum[i]),"%s",mTempAddendNum);
				dirty = true;
			}
		}
		else
		{
			dSprintf(mBuffAddendNum[i],sizeof(mBuffAddendNum[i])," ");
		}
		
		if (bHasTime)
		{
			S32 lastTime = 0;
			S32 timeTemp = 0;
			S32 timeNum = 0;
			if((*it)->m_BuffData->IsFlags(BuffData::Flags_Infinity) || !(*it)->m_BuffData->IsFlags(BuffData::Flags_ShowLimitTime))
			{
				if (dStrcmp((char*)cBuffRemainTime[i],"N/A"))
				{
					dSprintf(cBuffRemainTime[i], sizeof(cBuffRemainTime[i]), "%s", "N/A");
					dirty = true;
				}
			}
			else
			{
				lastTime = (*it)->m_RemainTime;
				timeTemp = (*it)->m_RemainTime - Platform::getVirtualMilliseconds();
				if (timeTemp != lastTime)
				{
					if (timeTemp < 60000)
					{
						timeNum = (int)mFloor(timeTemp / 1000) + 1;
						if (timeNum < 0)
						{
							timeNum = 0;
						}
						dSprintf(cBuffRemainTime[i], sizeof(cBuffRemainTime[i]), "%d%s", timeNum ,"S");
					}
					else if (timeTemp >= 60000 && timeTemp < 3600000)
					{
						timeNum = (int)mFloor(timeTemp / 1000 / 60) + 1;
						if (timeNum < 0)
						{
							timeNum = 0;
						}
						dSprintf(cBuffRemainTime[i], sizeof(cBuffRemainTime[i]), "%d%s",timeNum ,"M");
					}
					else if (timeTemp >= 3600000)
					{
						timeNum = (int)mFloor(timeTemp / 1000 / 60 / 60) + 1;
						if (timeNum < 0)
						{
							timeNum = 0;
						}
						dSprintf(cBuffRemainTime[i], sizeof(cBuffRemainTime[i]), "%d%s",timeNum ,"H");
					}

					if(dStrcmp((char*)cBuffRemainTime[i],(char*)cBuffRemainTimeBak[i]))
					{
						dirty = true;
						dStrcpy(cBuffRemainTimeBak[i],32,cBuffRemainTime[i]);
					}
				}
				lastTime = timeTemp;
			}
		}
	}

	//----------------------------------------------DEBUFF-------------------------------------------------
	char deBuffFileName[256];
	const BuffTable::BuffTableType& deBufflist = pObjectBuff->GetBuffTable().GetDebuff();
	BuffTable::BuffTableType::const_iterator dit = deBufflist.begin();
	if (mDeBuffNum != deBufflist.size())
		dirty = true;
	mDeBuffNum = deBufflist.size();
	for(int i = 0;dit != deBufflist.end();dit++,i++)
	{
		if (mObjType == 3)
		{
			if (i > 7)
				break;
		}

		sprintf( deBuffFileName, "%s%s", mBuffFilePath, (*dit)->m_BuffData->GetIcon());

		if ( dStrcmp((char*)mDeBuffFileName[i],(char*)deBuffFileName))
		{
			dStrcpy(mDeBuffFileName[i],256,deBuffFileName);
			mDeBuffTexture[i] = GFXTexHandle(mDeBuffFileName[i],&GFXDefaultStaticDiffuseProfile,true);
			dirty = true;
		}

		if (!mDeBuffTexture[i])
		{
			sprintf( deBuffFileName, "%s%s", mBuffFilePath, "icon_unknown");
			if (dStrcmp((char*)mDeBuffFileName[i],(char*)deBuffFileName))
			{
				dStrcpy(mDeBuffFileName[i],256,deBuffFileName);
				mDeBuffTexture[i] = GFXTexHandle(mDeBuffFileName[i],&GFXDefaultStaticDiffuseProfile,true);
				dirty = true;
			}
		}

		if (!mDeBuffTexture[i])
			return false;

		if ((*dit)->m_BuffData->IsFlags(BuffData::Flags_ShowCount))
		{
			char mTempAddendNum[32] = {0};
			dSprintf(mTempAddendNum,sizeof(mTempAddendNum),"%d",(*dit)->m_Count);
			if (!mDeBuffAddendNum[i])
			{
				dSprintf(mDeBuffAddendNum[i],sizeof(mDeBuffAddendNum[i]),"%s",mTempAddendNum);
				dirty = true;
			}
			if (dStrcmp((char*)mDeBuffAddendNum[i],(char*)mTempAddendNum))
			{
				dSprintf(mDeBuffAddendNum[i],sizeof(mDeBuffAddendNum[i]),"%s",mTempAddendNum);
				dirty = true;
			}
		}
		else
		{
			dSprintf(mDeBuffAddendNum[i],sizeof(mDeBuffAddendNum[i])," ");
		}

		if (bHasTime)
		{
			S32 lastTime = 0;
			S32 timeTemp = 0;
			S32 timeNum = 0;
			if((*dit)->m_BuffData->IsFlags(BuffData::Flags_Infinity) || !(*dit)->m_BuffData->IsFlags(BuffData::Flags_ShowLimitTime))
			{
				if (dStrcmp((char*)cDeBuffRemainTime[i],"N/A"))
				{
					dSprintf(cDeBuffRemainTime[i], sizeof(cDeBuffRemainTime[i]), "%s", "N/A");
					dirty = true;
				}
			}
			else
			{
				lastTime = (*dit)->m_RemainTime;
				timeTemp = (*dit)->m_RemainTime - Platform::getVirtualMilliseconds();
				if (timeTemp != lastTime)
				{
					if (timeTemp < 60000)
					{
						timeNum = (int)mFloor(timeTemp / 1000) + 1;
						if (timeNum < 0)
						{
							timeNum = 0;
						}
						dSprintf(cDeBuffRemainTime[i], sizeof(cDeBuffRemainTime[i]), "%d%s", timeNum ,"S");
					}
					else if (timeTemp >= 60000 && timeTemp < 3600000)
					{
						timeNum = (int)mFloor(timeTemp / 1000 / 60) + 1;
						if (timeNum < 0)
						{
							timeNum = 0;
						}
						dSprintf(cDeBuffRemainTime[i], sizeof(cDeBuffRemainTime[i]), "%d%s",timeNum ,"M");
					}
					else if (timeTemp >= 3600000)
					{
						timeNum = (int)mFloor(timeTemp / 1000 / 60 / 60) + 1;
						if (timeNum < 0)
						{
							timeNum = 0;
						}
						dSprintf(cDeBuffRemainTime[i], sizeof(cDeBuffRemainTime[i]), "%d%s",timeNum ,"H");
					}

					if(dStrcmp((char*)cBuffRemainTime[i],(char*)cBuffRemainTimeBak[i]))
					{
						dirty = true;
						dStrcpy(cBuffRemainTimeBak[i],32,cBuffRemainTime[i]);
					}
				}
				lastTime = timeTemp;
			}
		}
	}
	
	return dirty;
}

void GuiBuffCtrl::onMouseDown( const GuiEvent &event )
{
	mClickOut = true;

	if (!pObjectBuff)
		return;
	
	Point2I local_position = globalToLocalCoord(event.mousePoint);
	setChickOut(local_position);
	
}

void GuiBuffCtrl::onRightMouseDragged(const GuiEvent &event)
{
	mClickOut = true;

	if (!pObjectBuff)
		return;
	
	Point2I local_position = globalToLocalCoord(event.mousePoint);
	setChickOut(local_position); 

	Parent::onRightMouseDragged(event);
}

void GuiBuffCtrl::onMouseMove( const GuiEvent &event )
{
	Con::executef("CloseBuffHover");

	if (!pObjectBuff)
		return;

	char buff[1024] = "\n";
	RectI rect;
	RectI deRect;
	Point2I local_position = globalToLocalCoord(event.mousePoint);
	if (mObjType == 3)
	{
		if (CLIENT_TEAM->HasTeam())
		{
			for (int j = 1; j <= CLIENT_TEAM->GetPlayerList()->size()-1; j++)
			{
				const BuffTable::BuffTableType& bufflist = pObjectBuff->GetBuffTable().GetBuff();
				BuffTable::BuffTableType::const_iterator it = bufflist.begin();
				for(int i = 0;it != bufflist.end();it++,i++)
				{
					rect.set(mBuffBoundary.point.x + (mBuffBoundary.extent.x + mSpaceNum) * i,mBuffBoundary.point.y + (j-1) * mTeammateY,mBuffBoundary.extent.x,mBuffBoundary.extent.y);
					if (i > 7)
						break;
					if (rect.pointInRect(local_position))
					{
						char point[32];
						dSprintf(point, sizeof(point), "%d %d", event.mousePoint.x + 20,event.mousePoint.y + 20);
						if (bufflist[i]->m_BuffData)
						{
							g_ItemManager->insertText(bufflist[i]->m_BuffData,buff);
							Con::executef("ShowBuffHover",point,buff);
							return;
						}
					}
				}
				const BuffTable::BuffTableType& deBufflist = pObjectBuff->GetBuffTable().GetDebuff();
				BuffTable::BuffTableType::const_iterator dit = deBufflist.begin();
				for(int i = 0;dit != deBufflist.end();dit++,i++)
				{
					deRect.set(mBuffBoundary.point.x + (mBuffBoundary.extent.x + mSpaceNum) * i,mTempChangeY + (j-1) * mTeammateY,mBuffBoundary.extent.x,mBuffBoundary.extent.y);
					if (i > 7)
						break;
					if (rect.pointInRect(local_position))
					{
						char point[32];
						dSprintf(point, sizeof(point), "%d %d", event.mousePoint.x + 20,event.mousePoint.y + 20);
						if (deBufflist[i]->m_BuffData)
						{
							g_ItemManager->insertText(deBufflist[i]->m_BuffData,buff);
							Con::executef("ShowBuffHover",point,buff);
							return;
						}
					}
				}
			}
		}
	}
	else
	{
		const BuffTable::BuffTableType& bufflist = pObjectBuff->GetBuffTable().GetBuff();
		BuffTable::BuffTableType::const_iterator it = bufflist.begin();
		for(int i = 0;it != bufflist.end();it++,i++)
		{
			rect.set(mBuffBoundary.point.x + (mBuffBoundary.extent.x + mSpaceNum) * i,mBuffBoundary.point.y,mBuffBoundary.extent.x,mBuffBoundary.extent.y);
			if (i > 7)
			{
				rect.point.x -= (mBuffBoundary.extent.x + mSpaceNum) * 8;
				if (bHasTime)
					rect.point.y += mBuffBoundary.extent.y + mRemainTimeExtext.y;
				else
					rect.point.y += mBuffBoundary.extent.y;
			}
			if (rect.pointInRect(local_position))
			{
				char point[32];
				dSprintf(point, sizeof(point), "%d %d", event.mousePoint.x + 20,event.mousePoint.y + 20);
				if (bufflist[i]->m_BuffData)
				{
					g_ItemManager->insertText(bufflist[i]->m_BuffData,buff);
					Con::executef("ShowBuffHover",point,buff); 
					return;
				}
			}
		}
		const BuffTable::BuffTableType& deBufflist = pObjectBuff->GetBuffTable().GetDebuff();
		BuffTable::BuffTableType::const_iterator dit = deBufflist.begin();
		for(int i = 0;dit != deBufflist.end();dit++,i++)
		{
			deRect.set(mBuffBoundary.point.x + (mBuffBoundary.extent.x + mSpaceNum) * i,mTempChangeY,mBuffBoundary.extent.x,mBuffBoundary.extent.y);
			if (i > 7)
			{
				deRect.point.x -= (mBuffBoundary.extent.x + mSpaceNum) * 8;
				if (bHasTime)
					deRect.point.y += mBuffBoundary.extent.y + mRemainTimeExtext.y;
				else
					deRect.point.y += mBuffBoundary.extent.y;
			}
			if (deRect.pointInRect(local_position))
			{
				char point[32];
				dSprintf(point, sizeof(point), "%d %d", event.mousePoint.x + 20,event.mousePoint.y + 20);
				if (deBufflist[i]->m_BuffData)
				{
					g_ItemManager->insertText(deBufflist[i]->m_BuffData,buff);
					Con::executef("ShowBuffHover",point,buff);
					return;
				}
			}
		}
	}
}

void GuiBuffCtrl::onMouseLeave( const GuiEvent &event )
{
	Con::executef("CloseBuffHover");
}

void GuiBuffCtrl::onRightMouseUp( const GuiEvent &event )
{
	// 只对玩家自己有效
	if(mObjType != 1)
		return;

	RectI rect;
	Point2I local_position = globalToLocalCoord(event.mousePoint);

	const BuffTable::BuffTableType& bufflist = pObjectBuff->GetBuffTable().GetBuff();
	BuffTable::BuffTableType::const_iterator it = bufflist.begin();
	for(int i = 0;it != bufflist.end();it++,i++)
	{
		rect.set(mBuffBoundary.point.x + (mBuffBoundary.extent.x + mSpaceNum) * i,mBuffBoundary.point.y,mBuffBoundary.extent.x,mBuffBoundary.extent.y);
		if (i > 7)
		{
			rect.point.x -= (mBuffBoundary.extent.x + mSpaceNum) * 8;
			if (bHasTime)
				rect.point.y += mBuffBoundary.extent.y + mRemainTimeExtext.y;
			else
				rect.point.y += mBuffBoundary.extent.y;
		}
		if (rect.pointInRect(local_position) && bufflist[i]->m_BuffData)
		{
			// 找到buff了
			Player* player = g_ClientGameplayState->GetControlPlayer();
			if(player)
				player->HandleRemoveRequest(bufflist[i]->m_BuffData->GetBuffId(), i);
			return;
		}
	}
}

void GuiBuffCtrl::setChickOut( Point2I localPoint )
{

	RectI rect;
	RectI deRect;
	if (mObjType == 3)
	{
		if (CLIENT_TEAM->HasTeam())
		{
			for (int j = 1; j <= CLIENT_TEAM->GetPlayerList()->size()-1; j++)
			{
				const BuffTable::BuffTableType& bufflist = pObjectBuff->GetBuffTable().GetBuff();
				BuffTable::BuffTableType::const_iterator it = bufflist.begin();
				for(int i = 0;it != bufflist.end();it++,i++)
				{
					rect.set(mBuffBoundary.point.x + (mBuffBoundary.extent.x + mSpaceNum) * i,mBuffBoundary.point.y + (j-1) * mTeammateY,mBuffBoundary.extent.x,mBuffBoundary.extent.y);
					if (i > 7)
						break;
					if (rect.pointInRect(localPoint))
					{
						mClickOut = false;
						return;
					}
				}
				const BuffTable::BuffTableType& deBufflist = pObjectBuff->GetBuffTable().GetDebuff();
				BuffTable::BuffTableType::const_iterator dit = deBufflist.begin();
				for(int i = 0;dit != deBufflist.end();dit++,i++)
				{
					deRect.set(mBuffBoundary.point.x + (mBuffBoundary.extent.x + mSpaceNum) * i,mTempChangeY + (j-1) * mTeammateY,mBuffBoundary.extent.x,mBuffBoundary.extent.y);
					if (i > 7)
						break;
					if (deRect.pointInRect(localPoint))
					{
						mClickOut = false;
						return;
					}
				}
			}
		}
	}
	else
	{
		const BuffTable::BuffTableType& bufflist = pObjectBuff->GetBuffTable().GetBuff();
		BuffTable::BuffTableType::const_iterator it = bufflist.begin();
		for(int i = 0;it != bufflist.end();it++,i++)
		{
			rect.set(mBuffBoundary.point.x + (mBuffBoundary.extent.x + mSpaceNum) * i,mBuffBoundary.point.y,mBuffBoundary.extent.x,mBuffBoundary.extent.y);
			if (i > 7)
			{
				rect.point.x -= (mBuffBoundary.extent.x + mSpaceNum) * 8;
				if (bHasTime)
					rect.point.y += mBuffBoundary.extent.y + mRemainTimeExtext.y;
				else
					rect.point.y += mBuffBoundary.extent.y;
			}
			if (rect.pointInRect(localPoint))
			{
				mClickOut = false;
				return;
			}
		}
		const BuffTable::BuffTableType& deBufflist = pObjectBuff->GetBuffTable().GetDebuff();
		BuffTable::BuffTableType::const_iterator dit = deBufflist.begin();
		for(int i = 0;dit != deBufflist.end();dit++,i++)
		{
			deRect.set(mBuffBoundary.point.x + (mBuffBoundary.extent.x + mSpaceNum) * i,mTempChangeY,mBuffBoundary.extent.x,mBuffBoundary.extent.y);
			if (i > 7)
			{
				deRect.point.x -= (mBuffBoundary.extent.x + mSpaceNum) * 8;
				if (bHasTime)
					deRect.point.y += mBuffBoundary.extent.y + mRemainTimeExtext.y;
				else
					deRect.point.y += mBuffBoundary.extent.y;
			}
			if (deRect.pointInRect(localPoint))
			{
				mClickOut = false;
				return;
			}
		}
	}
}