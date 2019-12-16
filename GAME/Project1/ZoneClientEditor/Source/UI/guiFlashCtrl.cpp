#include <d3dx9math.h>
#include "gfx/gfxFlashManager.h"
#include "guiFlashCtrl.h"
#include <assert.h>
#include "console/consoleTypes.h"
#include "gfx\gfxDevice.h"
#include "gfx/D3D9/pc/gfxPCD3D9Device.h"

#ifdef new
#undef new
#endif

IMPLEMENT_CONOBJECT(GuiFlashCtrl);

GuiFlashCtrl::GuiFlashCtrl()
{
    m_flashName     = StringTable->insert("");
    m_lastFlashName = StringTable->insert("");
    m_curFlashView  = 0;
    m_isBlendBk     = true;
}

GuiFlashCtrl::~GuiFlashCtrl()
{
    GFX->GetFlashMgr()->Unload(m_curFlashView);
}

void GuiFlashCtrl::initPersistFields()
{
    Parent::initPersistFields();

    addField("flashName",  TypeString,  Offset(m_flashName,GuiFlashCtrl));
    addField("blendBk",    TypeBool,	Offset(m_isBlendBk,GuiFlashCtrl));
}

bool GuiFlashCtrl::onWake()
{
    if (!Parent::onWake())
        return false;
    
    return true;
}

void GuiFlashCtrl::onSleep()
{
    Parent::onSleep();
}

void GuiFlashCtrl::onRender( Point2I offset, const RectI &updateRect )
{
    if (0 == m_curFlashView || m_lastFlashName != m_flashName)
    {
        if (0 != m_curFlashView)
            GFX->GetFlashMgr()->Unload(m_curFlashView);

        m_curFlashView  = GFX->GetFlashMgr()->Load(m_flashName,m_isBlendBk);
        m_lastFlashName = m_flashName;
    }

    if (0 == m_curFlashView)
        return;

    gfxFlashManager* pFlashMgr = GFX->GetFlashMgr();
    const Point2I& extent = getExtent();
    pFlashMgr->Render(m_curFlashView,extent.x,extent.y,offset.x, offset.y,extent.x,extent.y);
}

void GuiFlashCtrl::onMouseDown(const GuiEvent &event)
{
    GFX->GetFlashMgr()->OnLMouseDown(m_curFlashView,event.mousePoint.x,event.mousePoint.y);
}

void GuiFlashCtrl::onMouseUp(const GuiEvent &event)
{
    GFX->GetFlashMgr()->OnLMouseUp(m_curFlashView,event.mousePoint.x,event.mousePoint.y);
}

void GuiFlashCtrl::onMouseMove(const GuiEvent &event)
{
    GFX->GetFlashMgr()->OnMouseMove(m_curFlashView,event.mousePoint.x,event.mousePoint.y);
}