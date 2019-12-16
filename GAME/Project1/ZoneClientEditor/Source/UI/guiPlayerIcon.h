#ifndef _GUIPLAYERICON_H_
#define _GUIPLAYERICON_H_
#include "UI/guiControlExt.h"


class GuiPlayerIconPopUpMenu;
//==================================================================================================================================
//   GuiIconBase
//==================================================================================================================================
class GuiIconBase:public GuiControlExt
{
    typedef GuiControlExt Parent;
private:
    

public:
    GuiIconBase();
    DECLARE_CONOBJECT(GuiIconBase);

    static void initPersistFields();
    
    void setCountType(S32 countType);
    void IsShowTextInfo(S32 isShow);
    virtual void IsUseMouseMethod(S32 isUse);

    void onPreRender();
    bool onWake();
    void onSleep();
   
    virtual bool updateData(){return 1;}
    virtual void onRenderExt(Point2I offset, const RectI &updateRect);

    static GuiPlayerIconPopUpMenu *m_pPlayerIconPopUpMenu;

protected:
    RectI m_BackgroundBoundary;           //����ͼ
    RectI m_BarBackgroundBoundary;        //����
    RectI m_IconBoundary;                 //ͷ��
    RectI m_NameBoundary;                 //����
    RectI m_LevelBoundary;                //�ȼ�
    RectI m_LevelBackBoundary;            //�ȼ�����ͼ
    RectI m_HpBarBoundary;                //Hp
    RectI m_MpBarBoundary;                //Mp    
    RectI m_CaptainBoundary;              //�ӳ���־
    

    StringTableEntry m_BackgroundFileName;
    GFXTexHandle m_BackgroundTexture;

    StringTableEntry m_BarBackgroundFileName;
    GFXTexHandle m_BarBackgroundTexture;

    char m_IconFileName[128];
    GFXTexHandle m_IconTexture;

    StringTableEntry m_HpBarFileName;
    GFXTexHandle m_HpBarTextrue;

    StringTableEntry m_MpBarFileName;
    GFXTexHandle m_MpBarTexture;   

    StringTableEntry m_CaptainFileName;
    GFXTexHandle m_CaptainTexture;

    StringTableEntry m_LevelBackFileName;
    GFXTexHandle m_LevelBackTexture;

    ColorI m_NameColor;
    char m_NameInfo[64];

    char m_HpInfo[32];
    char m_MpInfo[32];
   /* char m_PpInfo[32];*/
    U32 m_LevelInfo;

    F32 m_HpPercentum;
    F32 m_MpPercentum;

   
    bool m_IsShowInfo;
    S32 m_CountType;                                 //������ʾ���ͣ�Ѫ���ȵ����֣�

    bool m_MouseOnHp;
    bool m_MouseOnMp;
    bool m_MouseOnPp;
    bool m_IsShowTextInfo;
    bool m_IsUseMouseMethod;
    S32 m_TempSex;
   
};

//==================================================================================================================================
//   GuiPlayerIcon
//==================================================================================================================================


class GuiPlayerIcon : public GuiIconBase//public GuiControlExt
{   
    typedef GuiIconBase Parent;
private:

public:
	GuiPlayerIcon();
	DECLARE_CONOBJECT(GuiPlayerIcon);

	static void initPersistFields();
	bool updateData();
	void onMouseDown(const GuiEvent &event);
	void onMouseMove(const GuiEvent &event);
	void onRightMouseDown(const GuiEvent &event);
	
	void onRenderExt(Point2I offset, const RectI &updateRect);

	static void selectSelfIcon();

	static void setHeadIcon(S32 objSex,S32 objFace,char* iconFileName);

	//void resize(const Point2I &newPosition, const Point2I &newExtent);

protected:

    RectI m_FamilyBoundary;               //���ڱ�־
    RectI m_BileBarBoundary;              //ŭ��

	StringTableEntry m_BileBarFileName;
	GFXTexHandle m_BileBarTexture;	

	StringTableEntry m_FamilyFileName[9];
	GFXTexHandle m_FamilyTexture;

	char m_PpInfo[32];

	F32 m_BilePercentum;

	S32 m_TempFamily;
};

//==================================================================================================================================
//   GuiObjectIcon
//==================================================================================================================================
class GuiObjectIcon : public GuiIconBase//GuiControlExt
{   
    typedef GuiIconBase Parent;
private:

public:
	GuiObjectIcon();
	DECLARE_CONOBJECT(GuiObjectIcon);

	static void initPersistFields();
	
	bool updateData();
	//void onMouseDown(const GuiEvent &event);
	void onMouseMove(const GuiEvent &event);
	void onRightMouseDown(const GuiEvent &event);
	
	void onRenderExt(Point2I offset, const RectI &updateRect);
	
	void setTargetNull();
	//void resize(const Point2I &newPosition, const Point2I &newExtent);

protected:

    RectI m_FamilyBoundary;               //���ڱ�־

	StringTableEntry m_IngroupFileName;
	GFXTexHandle m_IngroupTexture;

	S32 m_ObjectType;

	bool m_ShowTarget;
	bool m_ShowPet;

	GameObject* pObject;
	Player* pPlayer;
	char m_TempIcon[128];

};

//==================================================================================================================================
//   GuiTeammateIcon
//==================================================================================================================================
class GuiTeammateIcon : public GuiIconBase//public GuiControl
{
	//typedef GuiControl Parent;

    typedef GuiIconBase Parent;
private:

public:
	GuiTeammateIcon();
	DECLARE_CONOBJECT(GuiTeammateIcon);

	static void initPersistFields();
	
	bool updateData();
	void onMouseDown(const GuiEvent &event);
	void onMouseMove(const GuiEvent &event);

    void onRenderExt(Point2I offset, const RectI &updateRect){return;}
	void onRender(Point2I offset, const RectI &updateRect);
	
	void IsUseMouseMethod(S32 isUse);

	static void selectTeamIcon(U32 index);

	//void resize(const Point2I &newPosition, const Point2I &newExtent);

protected:	
	RectI m_TeammateBoundary[5];          //��Ա����    

	ColorI m_NameColor;
	char m_NameInfo[5][64];

	char m_HpInfo[5][32];
	char m_MpInfo[5][32];
	U32 m_LevelInfo[5];

	F32 m_HpPercentum[5];
	F32 m_MpPercentum[5];

	bool m_MouseOnHp[5];
	bool m_MouseOnMp[5];	

	RectI hpBoundary[5];
	RectI mpBoundary[5];

};

#endif

//==================================================================================================================================
//   GuiPlayerIconPopUpMenu
//==================================================================================================================================
class GuiPlayerIconPopUpMenu : public GuiControl
{
     typedef GuiControl Parent;
public:
    GuiPlayerIconPopUpMenu();
    DECLARE_CONOBJECT(GuiPlayerIconPopUpMenu);

    void onMouseDown(const GuiEvent &event);
    void onRightMouseDown(const GuiEvent &event);
};