#include "console/console.h"
#include "console/consoleTypes.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/ClientGameplayState.h"
#include "UI/guiExpCtrl.h"

IMPLEMENT_CONOBJECT(GuiExpCtrl);

GuiExpCtrl::GuiExpCtrl()
{
	mExpBoundary.set(0,0,1,1);
	mExpBackBoundary.set(0,0,1,1);
	mExpTopBoundary.set(0,0,1,1);

	mExpImageFile[0] = StringTable->insert("");
    mExpImageFile[1] = StringTable->insert("");
    mExpImageFile[2] = StringTable->insert("");
	mExpBackImageFile = StringTable->insert("");
	mExpTopImageFile = StringTable->insert("");

	mExpTexture[0] = NULL;
    mExpTexture[1] = NULL;
    mExpTexture[2] = NULL;
	mExpBackTexture = NULL;
	mExpTopTexture = NULL;

	mExpPercentum = 0.0f;

	dSprintf(m_szExpDescription,sizeof(m_szExpDescription),"");
	dSprintf(m_szExpPercent,sizeof(m_szExpPercent),"");
	dSprintf(m_szExp,sizeof(m_szExp),"");
}

void GuiExpCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField("ExpBoundary",              TypeRectI,         Offset(mExpBoundary,         GuiExpCtrl));
	addField("ExpBackBoundary",          TypeRectI,         Offset(mExpBackBoundary,     GuiExpCtrl));
	addField("ExpTopBoundary",           TypeRectI,         Offset(mExpTopBoundary,      GuiExpCtrl));
    addField("ExpImageFile",             TypeString,        Offset(mExpImageFile,        GuiExpCtrl), 3);
	//addField("ExpImageFile",             TypeString,        Offset(mExpImageFile,        GuiExpCtrl));
	addField("ExpBackImageFile",         TypeString,        Offset(mExpBackImageFile,    GuiExpCtrl));
	addField("ExpTopImageFile",          TypeString,        Offset(mExpTopImageFile,     GuiExpCtrl));

}

bool GuiExpCtrl::onWake()
{
	if (!Parent::onWake())
		return false;



	setActive(true);

	return true;
}

bool GuiExpCtrl::UpdateDate()
{
	//Player* myself
	GameObject* pObject	= g_ClientGameplayState->GetControlPlayer();

	//if (NULL == myself)
	//{
	//	return false;
	//}

	//GameObject* pObject = dynamic_cast<GameObject*>(myself);

	if (!pObject)
	{
		return false;
	}

	bool dirty = false;

	if (!mExpTexture[0])
	{
		if (mExpImageFile[0][0] != '\0')
		{
			mExpTexture[0] =  GFXTexHandle(mExpImageFile[0], &GFXDefaultStaticDiffuseProfile, true);
			dirty = true;
		}
	}
    if (!mExpTexture[1])
    {
        if (mExpImageFile[1][0] != '\0')
        {
            mExpTexture[1] =  GFXTexHandle(mExpImageFile[1], &GFXDefaultStaticDiffuseProfile, true);
            dirty = true;
        }
    }
    if (!mExpTexture[2])
    {
        if (mExpImageFile[2][0] != '\0')
        {
            mExpTexture[2] =  GFXTexHandle(mExpImageFile[2], &GFXDefaultStaticDiffuseProfile, true);
            dirty = true;
        }
    }

	if (!mExpBackTexture)
	{
		if (mExpBackImageFile[0] != '\0')
		{
			mExpBackTexture =  GFXTexHandle(mExpBackImageFile, &GFXDefaultStaticDiffuseProfile, true);
			dirty = true;
		}
	}

	if (!mExpTopTexture)
	{
		if (mExpTopImageFile[0] != '\0')
		{
			mExpTopTexture =  GFXTexHandle(mExpTopImageFile, &GFXDefaultStaticDiffuseProfile, true);
			dirty = true;
		}
	}

	//F32 ExpPercentum = float(pObject->getExp()) / (g_LevelExp[pObject->getLevel()]);
	//if(mExpPercentum != ExpPercentum)
	//{
	//	mExpPercentum = ExpPercentum;
	//	dirty = true;
	//}

	return dirty;
}

void GuiExpCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	m_DirtyFlag = UpdateDate();
    F32 tempPercent = 0;
	if (mExpBackTexture)
	{
		RectI rect(offset + mExpBackBoundary.point,mExpBackBoundary.extent);
		GFX->getDrawUtil()->clearBitmapModulation();
		GFX->getDrawUtil()->drawBitmapStretch(mExpBackTexture,rect);
	}

	if (mExpTexture[0])
	{
		GFX->getDrawUtil()->clearBitmapModulation();
        RectI rect(offset + mExpBoundary.point,mExpBoundary.extent);        
        if (mExpPercentum <= 1.0f)
            tempPercent = mExpPercentum;
        else
            tempPercent = 1.0f;
        rect.extent.x = rect.extent.x * tempPercent;
        GFX->getDrawUtil()->drawBitmapStretch(mExpTexture[0],rect);
    }

    if (mExpTexture[1])
    {
        tempPercent = 0.0f;
        GFX->getDrawUtil()->clearBitmapModulation();
        RectI rect(offset + mExpBoundary.point,mExpBoundary.extent);
        if (mExpPercentum > 1.0f)  
        {
            if (mExpPercentum <= 2.0f)            
                tempPercent = mExpPercentum - 1.0f;           
            else
                tempPercent = 1.0f;          
        } 
        rect.extent.x = rect.extent.x * tempPercent;
        GFX->getDrawUtil()->drawBitmapStretch(mExpTexture[1],rect);
    }

    if (mExpTexture[2])
    {
        tempPercent = 0.0f;
        GFX->getDrawUtil()->clearBitmapModulation();
        RectI rect(offset + mExpBoundary.point,mExpBoundary.extent);
        if (mExpPercentum > 2.0f)  
        {
            if (mExpPercentum <= 3.0f)            
                tempPercent = mExpPercentum - 2.0f;           
            else
                tempPercent = 1.0f;          
        } 
        rect.extent.x = rect.extent.x * tempPercent;
        GFX->getDrawUtil()->drawBitmapStretch(mExpTexture[2],rect);
    }

    if (mExpTopTexture)
    {
        GFX->getDrawUtil()->clearBitmapModulation(); 
        RectI rect(offset + mExpTopBoundary.point,mExpTopBoundary.extent);
        GFX->getDrawUtil()->drawBitmapStretch(mExpTopTexture,rect);
    }

    //render the children
    renderChildControls(offset, updateRect);

}

void GuiExpCtrl::setPercentum( F32 percentum )
{
    if(percentum > 3.0f)
    {
        percentum = 3.0f;
    }
    if(mExpPercentum != percentum)
    {
        mExpPercentum = percentum;
        m_DirtyFlag = true;
	}
}

ConsoleMethod(GuiExpCtrl,setPercentum,void,3,3,"obj.setPercentum(percentum)")
{
	object->setPercentum(dAtof(argv[2]));
}

