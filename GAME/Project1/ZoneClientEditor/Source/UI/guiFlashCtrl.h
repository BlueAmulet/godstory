#ifndef _GUIFLASHCTRL_H_
#define _GUIFLASHCTRL_H_

#ifndef _GUIBUTTONCTRL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _GAMEOBJECT_H_
#include "Gameplay/GameObjects/GameObject.h"
#endif

/************************************************************************/
/* flash�ؼ�
/************************************************************************/

//ui�ؼ�
class GuiFlashCtrl : public GuiControl
{
    typedef GuiControl Parent;
public:
    GuiFlashCtrl();
    ~GuiFlashCtrl();
    DECLARE_CONOBJECT(GuiFlashCtrl);

    static void initPersistFields();

    bool onWake();
    void onSleep();
    void onRender(Point2I offset, const RectI &updateRect);

    void onMouseDown(const GuiEvent &event);
    void onMouseUp(const GuiEvent &event);
    void onMouseMove(const GuiEvent &event);
protected:
    
    //flash��Դ����
    StringTableEntry m_lastFlashName;
    StringTableEntry m_flashName;

    //�Ƿ��ϱ���
    bool             m_isBlendBk;
    
    unsigned int     m_curFlashView;
};


#endif /*_GUIFLASHCTRL_H_*/