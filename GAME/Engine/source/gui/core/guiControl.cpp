//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/codeBlock.h"
#include "platform/event.h"
#include "gfx/gBitmap.h"
#include "sim/actionMap.h"
#include "gui/core/guiCanvas.h"
#include "gui/core/guiControl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui//editor/guiEditCtrl.h"
#include "gfx/gfxDrawUtil.h"
#include "Gameplay/Team/ClientTeam.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "Util/RichTextDrawer.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
////#define STATEBLOCK
//cull none
GFXStateBlock* GuiControl::mCullNoneSB = NULL;
//------------------------------------------------------------------------------

GuiControl *GuiControl::smPrevResponder = NULL;
GuiControl *GuiControl::smCurResponder = NULL;
GuiEditCtrl*GuiControl::smEditorHandle = NULL;
bool        GuiControl::smDesignTime = false;

IMPLEMENT_CONOBJECT(GuiControl);

static const EnumTable::Enums horzEnums[] =
{
	{ GuiControl::horizResizeRight,      "right"       },
	{ GuiControl::horizResizeWidth,      "width"       },
	{ GuiControl::horizResizeLeft,       "left"        },
    { GuiControl::horizResizeCenter,     "center"      },
    { GuiControl::horizResizeRelative,   "relative"    },
	{ GuiControl::horizResizeProportion, "Proportion"  }
};
static const EnumTable gHorizSizingTable(6, &horzEnums[0]);

static const EnumTable::Enums vertEnums[] =
{
	{ GuiControl::vertResizeBottom,      "bottom"      },
	{ GuiControl::vertResizeHeight,      "height"      },
	{ GuiControl::vertResizeTop,         "top"         },
    { GuiControl::vertResizeCenter,      "center"      },
    { GuiControl::vertResizeRelative,    "relative"    },
	{ GuiControl::vertResizeProportion,  "Proportion"  }
};
static const EnumTable gVertSizingTable(6, &vertEnums[0]);

void GuiControl::onRender(Point2I offset, const RectI &updateRect)
{
	m_DirtyFlag = false;
	
   RectI ctrlRect(offset, getExtent());

   //if opaque, fill the update rect with the fill color
   if (getControlProfile()->mOpaque)
	  GFX->getDrawUtil()->drawRectFill(ctrlRect, getControlProfile()->mFillColor);

   //if there's a border, draw the border
   if (getControlProfile()->mBorder)
	  renderBorder(ctrlRect, getControlProfile());


   // Render Children
   renderChildControls(offset, updateRect);
}

bool GuiControl::renderTooltipAdv(Point2I cursorPos, const char* tipText /* = NULL */ )
{
	// Short Circuit.
	if (!mAwake) 
		return false;
	if ( dStrlen( mTooltip ) == 0 && ( tipText == NULL || dStrlen( tipText ) == 0 ) )
		return false;

	RectI r = GFX->getClipRect();

	GuiCanvas* root = getRoot();
	if( !root )
		return false;

	Point2I screensize = root->getWindowSize();

	CRichTextDrawer::Style style;
	style.fontSize = 12;
	mDrawer->setDefaultStyle( style );
	mDrawer->setContent( mTooltip ); 
 
	RectI rect;
	rect.point = cursorPos;
	rect.extent.x = mTooltipWidth; 
	rect.extent.y = mDrawer->getHeight();

	RectI b = getGlobalBounds();

	if( rect.point.y + rect.extent.y > screensize.y )
	{
		rect.point.y = screensize.y - rect.extent.y;
	}

	GFX->setClipRect( rect );

	if( !mTooltipProfile && mTooltipProfileName )
	{
		GuiControlProfile *profile = dynamic_cast<GuiControlProfile*>(Sim::findObject( mTooltipProfileName ));

		if( profile )
			setTooltipProfile(profile);
	}

	if( mTooltipProfile )
	{
		if (!mTooltipProfile->mTextureObject.isNull())
		{
			renderBorder(rect, mTooltipProfile);
		}
		else
		{
			GFX->getDrawUtil()->drawRectFill(rect, mTooltipProfile->mFillColor );
			GFX->getDrawUtil()->drawRect( rect, mTooltipProfile->mBorderColor );
		}
	}

	mDrawer->render( rect.point, rect );

	GFX->setClipRect( r );

	return true;
}

bool GuiControl::renderTooltip(Point2I cursorPos, const char* tipText )
{
   // Short Circuit.
   if (!mAwake) 
      return false;
   if ( dStrlen( mTooltip ) == 0 && ( tipText == NULL || dStrlen( tipText ) == 0 ) )
      return false;

   const char* renderTip = mTooltip;
   if( tipText != NULL )
      renderTip = tipText;

   // Need to have root.
   GuiCanvas *root = getRoot();
   if ( !root )
	   return false;

   CommonFontEX *font = getTooltipProfile()->mFont;
   //Vars used:
   //Screensize (for position check)
   //Offset to get position of cursor
   //textBounds for text extent.
   Point2I screensize = getRoot()->getWindowSize();
   Point2I offset = cursorPos; 
   Point2I textBounds;
   S32 textWidth = 0;

   // get each line of text
   const char *pTextStart = renderTip;
   const char *pTextEnd = pTextStart;
   char endOfLine = '\n';
   char *context = NULL;
   std::list<char *> textList;
   char *pText = NULL;

   while(*pTextEnd != 0)
   {
	   if (*pTextEnd == endOfLine)
	   {
		   pText = new char[pTextEnd - pTextStart + 1];
		   dMemcpy(pText, pTextStart, pTextEnd - pTextStart);
		   pText[pTextEnd - pTextStart] = 0;
		   textList.push_back(pText);
		   pTextStart = ++pTextEnd;
		   continue;
	   }

	   pTextEnd++;
   }

   if (pTextStart != pTextEnd)
   {
	   pText = new char[pTextEnd - pTextStart + 1];
	   dMemcpy(pText, pTextStart, pTextEnd - pTextStart);
	   pText[pTextEnd - pTextStart] = 0;
	   textList.push_back(pText);
   }

   std::list<char *>::iterator it = textList.begin();
   while(it != textList.end())
   {
		int width = font->getStrWidth(*it);
		if (textWidth <= width)
			textWidth = width;
		++it;
   }

   //Offset below cursor image
   offset.y += 20; // TODO: Attempt to fix?: root->getCursorExtent().y;
   //Create text bounds.
   textBounds.x = textWidth+20;
   textBounds.y = (font->getHeight() + 2) * textList.size() + 10;

   // Check position/width to make sure all of the tooltip will be rendered
   // 5 is given as a buffer against the edge
   if (screensize.x < offset.x + textBounds.x + 5)
      offset.x = screensize.x - textBounds.x - 5;

   // And ditto for the height
   if(screensize.y < offset.y + textBounds.y + 5)
      offset.y = cursorPos.y - textBounds.y - 5;
   
   RectI oldClip = GFX->getClipRect();

   // Set rectangle for the box, and set the clip rectangle.
   RectI rect(offset, textBounds);
   GFX->setClipRect(rect);

   if (!mTooltipProfile->mTextureObject.isNull())
   {
	   renderBorder(rect, mTooltipProfile);
	   //GFX->getDrawUtil()->drawBitmapStretch(mTooltipProfile->mTextureObject, rect);	   
   }
   else
   {
	   GFX->getDrawUtil()->drawRectFill(rect, mTooltipProfile->mFillColor );
	   GFX->getDrawUtil()->drawRect( rect, mTooltipProfile->mBorderColor );
   }
   // Draw the text centered in the tool tip box
   GFX->getDrawUtil()->setBitmapModulation( mTooltipProfile->mFontColor );
   Point2I start;
   int i = 0;
   for (it = textList.begin(); it != textList.end(); ++it)
   {
	   int width = font->getStrWidth(*it);

	   start.set( ( textBounds.x - width) / 2, 5 + (font->getHeight() + 2) * i);
	   //start.set( ( textBounds.x - textWidth) / 2, ( textBounds.y - font->getHeight() ) / 2 );
	   GFX->getDrawUtil()->drawText( font, start + offset, *it, &mTooltipProfile->mFontColor );
	   i++;
   }

   GFX->setClipRect( oldClip );

   it = textList.begin();
   while(it != textList.end())
   {
	   delete (*it);
	   ++it;
   }
   textList.clear();

   return true;
}

bool GuiControl::isDirty()
{
	bool dirty = m_DirtyFlag;
	m_DirtyFlag = false;

	iterator i;
	for(i = begin(); i != end(); i++)
	{
		GuiControl *ctrl = static_cast<GuiControl *>(*i);
		if(ctrl->isDirty())
			dirty = true;
	}

	return dirty;
}

void GuiControl::renderChildControls(Point2I offset, const RectI &updateRect)
{
   // offset is the upper-left corner of this control in screen coordinates
   // updateRect is the intersection rectangle in screen coords of the control
   // hierarchy.  This can be set as the clip rectangle in most cases.
   RectI clipRect = updateRect;

   iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      if (ctrl->mVisible)
      {
         Point2I childPosition = offset + ctrl->getPosition();
         RectI childClip(childPosition, ctrl->getExtent() + Point2I(1,1));

         if (childClip.intersect(clipRect))
         {
            GFX->setClipRect( childClip );
#ifdef STATEBLOCK
				AssertFatal(mCullNoneSB, "GuiControl::renderChildControls -- mCullNoneSB cannot be NULL.");
				mCullNoneSB->apply();
#else
            GFX->setCullMode( GFXCullNone );
#endif
            ctrl->onRender(childPosition, childClip);
         }
      }
   }
}

void GuiControl::setUpdateRegion(Point2I pos, Point2I ext)
{
   Point2I upos = localToGlobalCoord(pos);
   GuiCanvas *root = getRoot();
   if (root)
   {
      root->addUpdateRegion(upos, ext);
   }
}

void GuiControl::setUpdate()
{
   setUpdateRegion(Point2I(0,0), getExtent());
   m_DirtyFlag = true;
}

GuiControl::GuiControl() : mAddGroup( NULL ),
						   mClickOut(false),
                           mLayer(0),
                           mBounds(0,0,64,64),
                           mMinExtent(8,2),
						   mTooltipWidth( 100 ),
                           mProfileName(NULL),
                           mProfile(NULL),
                           mLangTable(NULL),
                           mFirstResponder(NULL),
                           mVisible(true),
                           mActive(false),
                           mAwake(false),
                           mCanSave(true),
                           mHorizSizing(horizResizeRight),
                           mVertSizing(vertResizeBottom),
                           mTooltipProfileName(NULL),
                           mTooltipProfile(NULL),
                           mTipHoverTime(1000),
                           mIsContainer(false)
{
   mConsoleVariable     = StringTable->insert("");
   mConsoleCommand      = StringTable->insert("");
   mAltConsoleCommand   = StringTable->insert("");
   mAcceleratorKey      = StringTable->insert("");
   mLangTableName       = StringTable->insert("");
   mScriptFile          = CodeBlock::getCurrentCodeBlockName();

   mTooltip             = StringTable->insert("");

   mCanSaveFieldDictionary = false;

   mNSLinkMask = LinkSuperClassName | LinkClassName;
   m_DirtyFlag = true;
   mMustPrerender = false;

   mDrawer = new CRichTextDrawer();
}

GuiControl::~GuiControl()
{
	delete mDrawer;
}

//-----------------------------------------------------------------------------
// Persistence 
//-----------------------------------------------------------------------------
void GuiControl::initPersistFields()
{
   Parent::initPersistFields();


   // Things relevant only to the editor.
   addGroup("Gui Editing");
   addField("isContainer",       TypeBool,      Offset(mIsContainer, GuiControl));
   endGroup("Gui Editing");

   // Parent Group.
   addGroup("GuiControl");

   //addProtectedField("Profile",  TypeGuiProfile,   Offset(mProfile, GuiControl), &setProfileProt, &defaultProtectedGetFn, "");
   addField("Profile",          TypeString,       Offset(mProfileName, GuiControl));
   addField("HorizSizing",       TypeEnum,         Offset(mHorizSizing, GuiControl), 1, &gHorizSizingTable);
   addField("VertSizing",        TypeEnum,         Offset(mVertSizing, GuiControl), 1, &gVertSizingTable);

   addField("Position",          TypePoint2I,      Offset(mBounds.point, GuiControl));
   addField("Extent",            TypePoint2I,      Offset(mBounds.extent, GuiControl));
   addField("MinExtent",         TypePoint2I,      Offset(mMinExtent, GuiControl));
   addField("canSave",           TypeBool,         Offset(mCanSave, GuiControl));
   addProtectedField( "Visible", TypeBool,         Offset(mVisible, GuiControl), &setVisible, &defaultProtectedGetFn, "" );
   addField("mustPrerender",     TypeBool,         Offset(mMustPrerender, GuiControl));

   addDeprecatedField("Modal");
   addDeprecatedField("SetFirstResponder");

   addField("Variable",          TypeString,       Offset(mConsoleVariable, GuiControl));
   addField("Command",           TypeString,       Offset(mConsoleCommand, GuiControl));
   addField("AltCommand",        TypeString,       Offset(mAltConsoleCommand, GuiControl));
   addField("Accelerator",       TypeString,       Offset(mAcceleratorKey, GuiControl));
   endGroup("GuiControl");	

   addGroup("ToolTip");
   addField("tooltipprofile",    TypeString,       Offset(mTooltipProfileName, GuiControl));
   addField("tooltip",           TypeString,       Offset(mTooltip, GuiControl));
   addField("tooltipWidth",		 TypeS32,		   Offset(mTooltipWidth, GuiControl));
   addField("hovertime",         TypeS32,          Offset(mTipHoverTime, GuiControl));
   endGroup("ToolTip");


   addGroup("Localization");
   addField("langTableMod",      TypeString,       Offset(mLangTableName, GuiControl));
   endGroup("Localization");
}

//-----------------------------------------------------------------------------
// Simulation
//-----------------------------------------------------------------------------
bool GuiControl::processArguments(S32 argc, const char **argv)
{
   // argv[0] - The GuiGroup to add this control to when it's created.  
   //           this is an optional parameter that may be specified at
   //           object creation time to organize a gui control into a 
   //           subgroup of GuiControls and is useful in helping the 
   //           gui editor group and sort the existent gui's in the Sim.

   // Specified group?
   if( argc == 1 )
   {
      StringTableEntry steIntName = StringTable->insert(argv[0]);
      mAddGroup = dynamic_cast<SimGroup*>(Sim::getGuiGroup()->findObjectByInternalName( steIntName ));
      if( mAddGroup == NULL )
      {
         mAddGroup = new SimGroup();
         mAddGroup->registerObject();
         mAddGroup->setInternalName( steIntName );
         Sim::getGuiGroup()->addObject( mAddGroup );
      }
      mAddGroup->addObject(this);
   }
   return true;
}

void GuiControl::awaken()
{
   //AssertFatal(!mAwake, "GuiControl::awaken: control is already awake");
   if(mAwake)
      return;

   iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      //AssertFatal(!ctrl->isAwake(), "GuiControl::awaken: child control is already awake");
      if(!ctrl->isAwake())
         ctrl->awaken();
   }

   //AssertFatal(!mAwake, "GuiControl::awaken: should not be awake here");
   if(!mAwake)
   {
      if(!onWake())
      {
         Con::errorf(ConsoleLogEntry::General, "GuiControl::awaken: failed onWake for obj: %s", getName());
         AssertFatal(0, "GuiControl::awaken: failed onWake");
         deleteObject();
      }
   }
}

void GuiControl::sleep()
{
   AssertFatal(mAwake, "GuiControl::sleep: control is not awake");
   if(!mAwake)
      return;

   iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);

      AssertFatal(ctrl->isAwake(), "GuiControl::sleep: child control is already asleep");
      if(ctrl->isAwake())
         ctrl->sleep();
   }

   AssertFatal(mAwake, "GuiControl::sleep: should not be asleep here");
   if(mAwake)
      onSleep();
}

void GuiControl::setPrerenderFlag()
{
	mMustPrerender = true;

	GuiControl *parent = getParent();
	if( parent )
		parent->setPrerenderFlag();
}

bool GuiControl::onAdd()
{
	if(mMustPrerender)
		setPrerenderFlag();

   // Let Parent Do Work.
   if(!Parent::onAdd())
      return false;

   // Grab the classname of this object
   const char *cName = getClassName();

   // if we're a pure GuiControl, then we're a container by default.
   if(dStrcmp("GuiControl", cName) == 0)
      mIsContainer = true;

   // Add to root group.
   if( mAddGroup == NULL )
      mAddGroup = Sim::getGuiGroup();
   mAddGroup->addObject(this);

   getControlProfile();

   // Notify Script.
   if( isMethod("onAdd") )
      Con::executef(this, "onAdd");

   // Return Success.
   return true;
}

void GuiControl::onRemove()
{
   // Only invoke script callbacks if they can be received
   if( isMethod("onRemove") )
      Con::executef(this, "onRemove");

   clearFirstResponder();

   Parent::onRemove();

  // If we are a child, notify our parent that we've been removed
  GuiControl *parent = getParent();
  if( parent )
     parent->onChildRemoved( this );
}

void GuiControl::onDeleteNotify(SimObject *object)
{
   if (object == mProfile)
      mProfile = NULL;
   if (object == mTooltipProfile)
      mTooltipProfile = NULL;
}

GuiControlProfile* GuiControl::getTooltipProfile()
{
   // We can early out if we still have a valid profile
   if (mTooltipProfile)
      return mTooltipProfile;

   // Attempt to find the profile specified
   if (mTooltipProfileName)
   {
      GuiControlProfile *profile = dynamic_cast<GuiControlProfile*>(Sim::findObject( mTooltipProfileName ));

      if( profile )
         setTooltipProfile(profile);
   }

   // If that didn't work then we use this guis profile instead
   if (!mTooltipProfile)
      setTooltipProfile(getControlProfile());

   return mTooltipProfile;
}

GuiControlProfile* GuiControl::getControlProfile()
{
   // We can early out if we still have a valid profile
   if (mProfile)
      return mProfile;

   // Attempt to find the profile specified
   if (mProfileName)
   {
      GuiControlProfile *profile = dynamic_cast<GuiControlProfile*>(Sim::findObject( mProfileName ));

      if( profile )
         setControlProfile(profile);
   }

   // If that didn't work then attempt to get <Classname>Profile
   if (!mProfile)
   {
      // Grab the classname of this object
      const char *cName = getClassName();

      // Ensure the classname is a valid name...
      if( cName && cName[0] )
      {
         S32 pos = 0;

         for( pos = 0; pos <= dStrlen( cName ); pos++ )
            if( !dStrncmp( cName + pos, "Ctrl", 4 ) )
               break;

         if( pos != 0 )
         {
            char buff[255];
            dStrncpy( buff, sizeof(buff), cName, pos );
            buff[pos] = '\0';
            dStrcat( buff, sizeof(buff), "Profile\0" );

            GuiControlProfile *profile = dynamic_cast<GuiControlProfile*>(Sim::findObject( buff ));

            if( profile )
               setControlProfile(profile);
         }
      }
   }

   // If we still don't have a profile then we default to GuiDefaultProfile
   if(!mProfile)
   {
      GuiControlProfile *profile = dynamic_cast<GuiControlProfile*>(Sim::findObject( "GuiDefaultProfile" ));

      AssertISV(profile != NULL, avar("GuiControlProfile: unable to find specified profile (%s) and GuiDefaultProfile does not exist!", mProfileName));

      if( profile )
         setControlProfile(profile);
   }

   return mProfile;
}

bool GuiControl::onWake()
{
   AssertFatal( !mAwake, "GuiControl::onWake: control is already awake" );
   if( mAwake )
      return false;

   // [tom, 4/18/2005] Cause mLangTable to be refreshed in case it was changed
   mLangTable = NULL;

   //set the flag
   mAwake = true;

   //getTooltipProfile();

   //set the layer
   GuiCanvas *root = getRoot();
   //AssertFatal(root, "Unable to get the root Canvas.");
   GuiControl *parent = getParent();
   if (parent && parent != root)
      mLayer = parent->mLayer;

   //make sure the first responder exists
   if (! mFirstResponder)
      mFirstResponder = findFirstTabable();

   //increment the profile
   getControlProfile()->incRefCount();

   mDrawer->setWidth( 200 );
   mDrawer->setContent( mTooltip );

   // Only invoke script callbacks if we have a namespace in which to do so
   // This will suppress warnings
   if( isMethod("onWake") )
      Con::executef(this, "onWake");
   
   return true;
}

void GuiControl::onSleep()
{
   AssertFatal(mAwake, "GuiControl::onSleep: control is not awake");
   if(!mAwake)
      return;

   //decrement the profile referrence
   if( getControlProfile() != NULL )
      getControlProfile()->decRefCount();
   clearFirstResponder();
   mouseUnlock();

   // Only invoke script callbacks if we have a namespace in which to do so
   // This will suppress warnings
   if( isMethod("onSleep") )
      Con::executef(this, "onSleep");

   // Set Flag
   mAwake = false;
}

void GuiControl::onChildAdded( GuiControl *child )
{
   // Base class does not make use of this
}

void GuiControl::onChildRemoved( GuiControl *child )
{
   // Base does nothing with this
}

void GuiControl::addObject(SimObject *object)
{
   GuiControl *ctrl = dynamic_cast<GuiControl *>(object);
   if(object->getGroup() == this)
      return;

	Parent::addObject(object);

   AssertFatal(!ctrl->isAwake(), "GuiControl::addObject: object is already awake before add");
   if(mAwake)
      ctrl->awaken();

  // If we are a child, notify our parent that we've been removed
  GuiControl *parent = ctrl->getParent();
  if( parent )
     parent->onChildAdded( ctrl );


}

void GuiControl::removeObject(SimObject *object)
{
   AssertFatal(mAwake == static_cast<GuiControl*>(object)->isAwake(), "GuiControl::removeObject: child control wake state is bad");
   if (mAwake)
      static_cast<GuiControl*>(object)->sleep();
	Parent::removeObject(object);
}

GuiControl *GuiControl::getParent()
{
	SimObject *obj = getGroup();
	if (GuiControl* gui = dynamic_cast<GuiControl*>(obj))
      return gui;
   return 0;
}

GuiCanvas *GuiControl::getRoot()
{
   GuiControl *root = NULL;
	GuiControl *parent = getParent();
   while (parent)
   {
      root = parent;
      parent = parent->getParent();
   }
   if (root)
      return dynamic_cast<GuiCanvas*>(root);
   else
      return NULL;
}

void GuiControl::inspectPreApply()
{
   if(smDesignTime && smEditorHandle)
      smEditorHandle->controlInspectPreApply(this);
   
   // The canvas never sleeps
   if(mAwake && dynamic_cast<GuiCanvas*>(this) == NULL )
   {
      onSleep(); // release all our resources.
      mAwake = true;
   }
}

void GuiControl::inspectPostApply()
{
   // Shhhhhhh, you don't want to wake the canvas!
   if(mAwake && dynamic_cast<GuiCanvas*>(this) == NULL )
   {
      mAwake = false;
      bool isContainer = mIsContainer;
      onWake();
      mIsContainer = isContainer;
   }
   
   if(smDesignTime && smEditorHandle)
      smEditorHandle->controlInspectPostApply(this);
}

//-----------------------------------------------------------------------------
// Coordinates
//-----------------------------------------------------------------------------
Point2I GuiControl::localToGlobalCoord(const Point2I &src)
{
   Point2I ret = src;
   ret += getPosition();
   GuiControl *walk = getParent();
   while(walk)
   {
      ret += walk->getPosition();
      walk = walk->getParent();
   }
   return ret;
}

Point2I GuiControl::globalToLocalCoord(const Point2I &src)
{
   Point2I ret = src;
   ret -= getPosition();
   GuiControl *walk = getParent();
   while(walk)
   {
      ret -= walk->getPosition();
      walk = walk->getParent();
   }
   return ret;
}

//-----------------------------------------------------------------------------
// Internationalization
//-----------------------------------------------------------------------------
LangTable * GuiControl::getGUILangTable()
{
	if(mLangTable)
		return mLangTable;

	if(mLangTableName && *mLangTableName)
	{
		mLangTable = (LangTable *)getModLangTable((const UTF8*)mLangTableName);
		return mLangTable;
	}

	GuiControl *parent = getParent();
	if(parent)
		return parent->getGUILangTable();

	return NULL;
}

const UTF8 * GuiControl::getGUIString(S32 id)
{
	LangTable *lt = getGUILangTable();
	if(lt)
		return lt->getString(id);

	return NULL;
}

//-----------------------------------------------------------------------------
// Sizing Methods
//-----------------------------------------------------------------------------
bool GuiControl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	//[2009/11/11][thinking]:设置一行的宽度
	if(mProfile && mProfile->mFont && getRoot())
		mProfile->mFont->SetLineDis(getRoot()->getExtent().x);

   const Point2I minExtent = getMinExtent();
   Point2I actualNewExtent = Point2I(getMax(minExtent.x, newExtent.x),
      getMax(minExtent.y, newExtent.y));

   // only do the child control resizing stuff if you really need to.
   const RectI bounds = getBounds();
   
   // If we didn't size anything, return false to indicate such
   bool extentChanged = (actualNewExtent != bounds.extent);
   bool positionChanged = (newPosition != bounds.point);
   if (!extentChanged && !positionChanged ) 
      return false;

   // Update Extent
   if( extentChanged )
   {
      //call set update both before and after
      setUpdate();
      iterator i;
      for(i = begin(); i != end(); i++)
      {
         GuiControl *ctrl = static_cast<GuiControl *>(*i);
         ctrl->parentResized(RectI(bounds.point, bounds.extent), RectI(newPosition,actualNewExtent));
      }

      mBounds.extent = actualNewExtent;

      GuiControl *parent = getParent();
      if (parent)
         parent->childResized(this);
      setUpdate();
   }

   // Update Position
   if( positionChanged )
      mBounds.point = newPosition;
   
   // We sized something
   if( extentChanged )
      return true;

   // Note : We treat a repositioning as no sizing happening
   //  because parent's should really not need to know when they
   //  have moved, as it should not affect any child sizing since
   //  all child bounds are relative to the parent's 0,0
   return false;

}
bool GuiControl::setPosition( const Point2I &newPosition )
{
   return resize( newPosition, mBounds.extent );
}

bool GuiControl::setExtent( const Point2I &newExtent )
{
   return resize( mBounds.point, newExtent );
}

bool GuiControl::setBounds( const RectI &newBounds )
{
   return resize( newBounds.point, newBounds.extent );
}

void GuiControl::setLeft( S32 newLeft )
{
   resize( Point2I( newLeft, mBounds.point.y), mBounds.extent );
}

void GuiControl::setTop( S32 newTop )
{
   resize( Point2I( mBounds.point.x, newTop ), mBounds.extent );
}

void GuiControl::setWidth( S32 newWidth )
{
   resize( mBounds.point, Point2I( newWidth, mBounds.extent.y ) );
}

void GuiControl::setHeight( S32 newHeight )
{
   resize( mBounds.point, Point2I( mBounds.extent.x, newHeight ) );
}

void GuiControl::childResized(GuiControl *child)
{
   child;
   // default to do nothing...
}

void GuiControl::parentResized(const RectI &oldParentRect, const RectI &newParentRect)
{
    Point2I newPosition = getPosition();
    Point2I newExtent = getExtent();
	Point2I tempPosition = newPosition;
	Point2I tempExtent = newExtent;

	S32 deltaX = newParentRect.extent.x - oldParentRect.extent.x;
 	S32 deltaY = newParentRect.extent.y - oldParentRect.extent.y;

	if (mHorizSizing == horizResizeCenter)
	   newPosition.x = (newParentRect.extent.x - getWidth()) >> 1;
	else if (mHorizSizing == horizResizeWidth)
		newExtent.x += deltaX;
	else if (mHorizSizing == horizResizeLeft)
      newPosition.x += deltaX;
    else if (mHorizSizing == horizResizeRelative && oldParentRect.extent.x != 0)
    {
       S32 newLeft = (newPosition.x * newParentRect.extent.x) / oldParentRect.extent.x;
       S32 newRight = ((newPosition.x + newExtent.x) * newParentRect.extent.x) / oldParentRect.extent.x;

       newPosition.x = newLeft;
       newExtent.x = newRight - newLeft;
    }
	else if (mHorizSizing == horizResizeProportion && oldParentRect.extent.x != 0)
	{
		S32 newLeft = (newPosition.x * newParentRect.extent.x) / oldParentRect.extent.x;
		S32 newRight = ((newPosition.x + newExtent.x) * newParentRect.extent.x) / oldParentRect.extent.x;

		if (newLeft > newParentRect.extent.x / 2)
			newPosition.x = newLeft + (newRight - newLeft - newExtent.x);
		else
			newPosition.x = newLeft;
	}

	if (mVertSizing == vertResizeCenter)
	   newPosition.y = (newParentRect.extent.y - getHeight()) >> 1;
	else if (mVertSizing == vertResizeHeight)
		newExtent.y += deltaY;
	else if (mVertSizing == vertResizeTop)
      newPosition.y += deltaY;
    else if(mVertSizing == vertResizeRelative && oldParentRect.extent.y != 0)
    {
       S32 newTop = (newPosition.y * newParentRect.extent.y) / oldParentRect.extent.y;
       S32 newBottom = ((newPosition.y + newExtent.y) * newParentRect.extent.y) / oldParentRect.extent.y;

       newPosition.y = newTop;
       newExtent.y = newBottom - newTop;
    }
	else if (mVertSizing == vertResizeProportion && oldParentRect.extent.y != 0)
	{
		S32 newTop = (newPosition.y * newParentRect.extent.y) / oldParentRect.extent.y;
		S32 newBottom = ((newPosition.y + newExtent.y) * newParentRect.extent.y) / oldParentRect.extent.y;

		if (newTop > newParentRect.extent.y / 2)
			newPosition.y = newTop + (newBottom - newTop - newExtent.y);
		else
			newPosition.y = newTop;
	}
    // Resizing Re factor [9/18/2006]
    // Only resize if our minExtent is satisfied with it.
    Point2I minExtent = getMinExtent();
    if( newExtent.x >= minExtent.x && newExtent.y >= minExtent.y )
    {
	 //   if (newPosition.x < 0 || (newPosition.x + newExtent.x) > newParentRect.extent.x)
		//	
		//if (newPosition.y < 0 || (newPosition.y + newExtent.y) > newParentRect.extent.y)
		//	
        resize(newPosition, newExtent);
    }
}

//-----------------------------------------------------------------------------
// Render Methods
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Dirty Rects
//-----------------------------------------------------------------------------

void GuiControl::preRender()
{
   //AssertFatal(mAwake, "GuiControl::preRender: control is not awake");
   if(!mAwake || (!mVisible && !mMustPrerender) )
      return;

   iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
	  ctrl->preRender();
   }
   onPreRender();
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiControl::setTooltipProfile(GuiControlProfile *prof)
{
   if (prof == mTooltipProfile)
      return;

   if (mAwake && mTooltipProfile)
      mTooltipProfile->decRefCount();

   // Clear the delete notification we previously set up
   if (mTooltipProfile)
      clearNotify(mTooltipProfile);

   mTooltipProfile = prof;
   if (mAwake && mTooltipProfile)
      mTooltipProfile->incRefCount();

   // Make sure that the new profile will notify us when it is deleted
   if (mTooltipProfile)
      deleteNotify(mTooltipProfile);

   // force an update when the profile is changed
   if (mAwake)
   {
      sleep();
      awaken();
   }
}

void GuiControl::setControlProfile(GuiControlProfile *prof)
{
   AssertFatal(prof, "GuiControl::setControlProfile: invalid profile");

   if(prof == mProfile)
      return;

   bool skipAwaken = false;

   if (mProfile == NULL)
      skipAwaken = true;

   if(mAwake && mProfile)
      mProfile->decRefCount();

   // Clear the delete notification we previously set up
   if (mProfile)
      clearNotify(mProfile);

   mProfile = prof;
   if(mAwake)
      mProfile->incRefCount();

   // Make sure that the new profile will notify us when it is deleted
   if (mProfile)
      deleteNotify(mProfile);

   // force an update when the profile is changed
   if(mAwake && !skipAwaken)
   {
      sleep();
      awaken();
   }
}

bool GuiControl::setProfileProt(void* obj, const char* data)
{
   GuiControlProfile *prof = dynamic_cast<GuiControlProfile *>(Sim::findObject(data));
   if(prof == NULL)
      return false;
   GuiControl* ctrl = static_cast<GuiControl*>(obj);

   // filter through our setter, for consistency
   ctrl->setControlProfile(prof);

   // ask the console not to set the data, because we've already set it
   return false;
}
void GuiControl::onPreRender()
{
   // do nothing.
#ifdef NTJ_EDITOR
	m_DirtyFlag = true;
#endif
	
}
//-----------------------------------------------------------------------------
//	checks up the parent hierarchy - if anyone above us is not savable returns false
//	otherwise, returns true. 
//-----------------------------------------------------------------------------
bool	GuiControl::getCanSaveParent()
{
   GuiControl *walk = this;
   while(walk)
   {
      if(!walk->getCanSave())
         return false;

      walk = walk->getParent();
   }

   return true;
}

//-----------------------------------------------------------------------------
//	Can we Save to a PowerEngineScript file?
//-----------------------------------------------------------------------------
bool GuiControl::getCanSave()
{
   return mCanSave;
}

//-----------------------------------------------------------------------------
//	Sets whether we can save out to a file (PowerEngineScript)
//-----------------------------------------------------------------------------
void GuiControl::setCanSave(bool bCanSave)
{
   mCanSave	=	bCanSave;
}

//-----------------------------------------------------------------------------
//	checks out mCanSave flag, if true just passes along to our parent, 
//	if false, then we return without writing.  Note, also, that 
//	if our parent is not writeable, then we should not be writable...
//-----------------------------------------------------------------------------
void GuiControl::write(Stream &stream, U32 tabStop, U32 flags)
{
   //note: this will return false if either we, or any of our parents, are non-save controls
   bool bCanSave	=	getCanSaveParent();
   StringTableEntry steName = mAddGroup->getInternalName();
   if(bCanSave && mAddGroup && (steName != NULL) && (steName != StringTable->insert("null")) && getName() )
   {
      MutexHandle handle;
      handle.lock(mMutex);

      // export selected only?
      if((flags & SelectedOnly) && !isSelected())
      {
         for(U32 i = 0; i < size(); i++)
            (*this)[i]->write(stream, tabStop, flags);

         return;

      }

      stream.writeTabs(tabStop);
      char buffer[1024];
      dSprintf(buffer, sizeof(buffer), "new %s(%s,%s) {\r\n", getClassName(), getName() ? getName() : "", mAddGroup->getInternalName());
      stream.write(dStrlen(buffer), buffer);
      writeFields(stream, tabStop + 1);

      if(size())
      {
         stream.write(2, "\r\n");
         for(U32 i = 0; i < size(); i++)
            (*this)[i]->write(stream, tabStop + 1, flags);
      }

      stream.writeTabs(tabStop);
      stream.write(4, "};\r\n");
   }
   else
      Parent::write( stream, tabStop, flags );

}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//	Make Sure Child 1 is Ordered Just Under Child 2.
//-----------------------------------------------------------------------------
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

const char *GuiControl::getScriptValue()
{
   return NULL;
}

void GuiControl::setScriptValue(const char *value)
{
   value;
}

void GuiControl::setConsoleVariable(const char *variable)
{
   if (variable)
   {
      mConsoleVariable = StringTable->insert(variable);
   }
   else
   {
      mConsoleVariable = StringTable->insert("");
   }
}
  
//-----------------------------------------------------------------------------
//	finds and returns the first immediate child of ours whose
//	internal name matches the passed String. returns Null if not found.
//-----------------------------------------------------------------------------
void GuiControl::setConsoleCommand(const char *newCmd)
{
   if (newCmd)
      mConsoleCommand = StringTable->insert(newCmd);
   else
      mConsoleCommand = StringTable->insert("");
}

const char * GuiControl::getConsoleCommand()
{
	return mConsoleCommand;
}

void GuiControl::setSizing(S32 horz, S32 vert)
{
	mHorizSizing = horz;
	mVertSizing = vert;
}


void GuiControl::setVariable(const char *value)
{
   if (mConsoleVariable[0])
      Con::setVariable(mConsoleVariable, value);
}

void GuiControl::setIntVariable(S32 value)
{
   if (mConsoleVariable[0])
      Con::setIntVariable(mConsoleVariable, value);
}

void GuiControl::setFloatVariable(F64 value)
{
   if (mConsoleVariable[0])
      Con::setFloatVariable(mConsoleVariable, value);
}

const char * GuiControl::getVariable()
{
   if (mConsoleVariable[0])
      return Con::getVariable(mConsoleVariable);
   else return NULL;
}

S32 GuiControl::getIntVariable()
{
   if (mConsoleVariable[0])
      return Con::getIntVariable(mConsoleVariable);
   else return 0;
}

F64 GuiControl::getFloatVariable()
{
   if (mConsoleVariable[0])
      return Con::getFloatVariable(mConsoleVariable);
   else return 0.0f;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

bool GuiControl::cursorInControl()
{
   GuiCanvas *root = getRoot();
   if (! root) return false;

   Point2I pt = root->getCursorPos();
   Point2I extent = getExtent();
   Point2I offset = localToGlobalCoord(Point2I(0, 0));
   if (pt.x >= offset.x && pt.y >= offset.y &&
      pt.x < offset.x + extent.x && pt.y < offset.y + extent.y)
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool GuiControl::pointInControl(const Point2I& parentCoordPoint)
{
   const RectI &bounds = getBounds();
   S32 xt = parentCoordPoint.x - bounds.point.x;
   S32 yt = parentCoordPoint.y - bounds.point.y;
   return xt >= 0 && yt >= 0 && xt < bounds.extent.x && yt < bounds.extent.y;
}

GuiControl* GuiControl::findHitControl(const Point2I &pt, S32 initialLayer)
{
   iterator i = end(); // find in z order (last to first)
   while (i != begin())
   {
      i--;
	  GuiControl *ctrl = static_cast<GuiControl *>(*i);

	  if( !ctrl )
		  continue;

      if (initialLayer >= 0 && ctrl->mLayer > initialLayer)
      {
         continue;
      }
      else if (ctrl->mVisible && ctrl->pointInControl(pt))
      {
         Point2I ptemp = pt - ctrl->getPosition();
         GuiControl *hitCtrl = ctrl->findHitControl(ptemp);

         if(hitCtrl->getControlProfile()->mModal)
            return hitCtrl;

		 //hitCtrl->mClickOut = false;
      }
   }
   return this;
}

GuiControl* GuiControl::onHitControlMouseDown(const GuiEvent &event, const Point2I &pt, Point2I offset, RectI clipRect, S32 initialLayer)
{
	iterator i = end(); // find in z order (last to first)
	while (i != begin())
	{
		i--;
		GuiControl *ctrl = static_cast<GuiControl *>(*i);

		if( !ctrl )
			continue;

		if (initialLayer >= 0 && ctrl->mLayer > initialLayer)
		{
			continue;
		}
		else if (ctrl->mVisible/* && ctrl->pointInControl(pt)*/)
		{
			Point2I childPosition = offset + ctrl->getPosition();
			RectI childRect( childPosition, ctrl->getExtent() + Point2I(1,1) );
			childRect.intersect( clipRect );
			
			if( childRect.pointInRect( pt ) )
			{
				//Point2I ptemp = pt- ctrl->getPosition();
				GuiControl *hitCtrl = ctrl->onHitControlMouseDown( event, pt, childPosition, childRect );
			
				if(hitCtrl->getControlProfile()->mModal  && !hitCtrl->mClickOut )
					return hitCtrl;
			}
			//hitCtrl->mClickOut = false;
		}
	}
	
	onMouseDown( event );
	return this;
}

GuiControl* GuiControl::onHitControlMouseRightDown(const GuiEvent &event, const Point2I &pt, Point2I offset, RectI clipRect, S32 initialLayer)
{
	iterator i = end(); // find in z order (last to first)
	while (i != begin())
	{
		i--;
		GuiControl *ctrl = static_cast<GuiControl *>(*i);

		if( !ctrl )
			continue;

		if (initialLayer >= 0 && ctrl->mLayer > initialLayer)
		{
			continue;
		}
		else if (ctrl->mVisible/* && ctrl->pointInControl(pt)*/)
		{
			Point2I childPosition = offset + ctrl->getPosition();
			RectI childRect( childPosition, ctrl->getExtent() + Point2I(1,1) );
			childRect.intersect( clipRect );

			if( childRect.pointInRect( pt ) )
			{
				//Point2I ptemp = pt- ctrl->getPosition();
				GuiControl *hitCtrl = ctrl->onHitControlMouseRightDown( event, pt, childPosition, childRect );

				if(hitCtrl->getControlProfile()->mModal  && !hitCtrl->mClickOut )
					return hitCtrl;
			}
			//hitCtrl->mClickOut = false;
		}
	}

	onRightMouseDown( event );
	return this;
}

GuiControl* GuiControl::onHitControlMouseMove(const GuiEvent &event, const Point2I &pt, Point2I offset, RectI clipRect, S32 initialLayer)
{
	iterator i = end(); // find in z order (last to first)
	while (i != begin())
	{
		i--;
		GuiControl *ctrl = static_cast<GuiControl *>(*i);

		if( !ctrl )
			continue;

		if (initialLayer >= 0 && ctrl->mLayer > initialLayer)
		{
			continue;
		}
		else if (ctrl->mVisible/* && ctrl->pointInControl(pt)*/)
		{
			Point2I childPosition = offset + ctrl->getPosition();
			RectI childRect( childPosition, ctrl->getExtent() + Point2I(1,1) );
			childRect.intersect( clipRect );

			if( childRect.pointInRect( pt ) )
			{
				//Point2I ptemp = pt- ctrl->getPosition();
				GuiControl *hitCtrl = ctrl->onHitControlMouseMove( event, pt, childPosition, childRect );

				if(!hitCtrl->mClickOut )
					return hitCtrl;
			}
			//hitCtrl->mClickOut = false;
		}
	}
	GuiControl* pCtrl = getRoot()->mLastMoveMouseControl;
	if( pCtrl )
	{
		if( pCtrl != this )
		{
			pCtrl->onMouseLeave( event );
			onMouseEnter( event );
			getRoot()->mLastMoveMouseControl = this;
		}
	}
	else
		getRoot()->mLastMoveMouseControl = this;

	
	onMouseMove( event );
	return this;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

bool GuiControl::isMouseLocked()
{
   GuiCanvas *root = getRoot();
   return root ? root->getMouseLockedControl() == this : false;
}

void GuiControl::mouseLock(GuiControl *lockingControl)
{
   GuiCanvas *root = getRoot();
   if (root)
      root->mouseLock(lockingControl);
}

void GuiControl::mouseLock()
{
   GuiCanvas *root = getRoot();
   if (root)
      root->mouseLock(this);
}

void GuiControl::mouseUnlock()
{
   GuiCanvas *root = getRoot();
   if (root)
      root->mouseUnlock(this);
}

bool GuiControl::onInputEvent(const InputEventInfo &event)
{
	// Do nothing by default...
   return( false );
}

void GuiControl::onMouseUp(const GuiEvent &event)
{
}

void GuiControl::onMouseDown(const GuiEvent &event)
{
}

void GuiControl::onMouseMove(const GuiEvent &event)
{
   //if this control is a dead end, make sure the event stops here
   if ( !mVisible || !mAwake )
      return;

   ////pass the event to the parent
   //GuiControl *parent = getParent();
   //if ( parent )
   //   parent->onMouseMove( event );
}

void GuiControl::onMouseDragged(const GuiEvent &event)
{
}

void GuiControl::onMouseEnter(const GuiEvent &)
{
}

void GuiControl::onMouseLeave(const GuiEvent &)
{
}

bool GuiControl::onMouseWheelUp( const GuiEvent &event )
{
   //if this control is a dead end, make sure the event stops here
   if ( !mVisible || !mAwake )
      return true;

   //pass the event to the parent
   GuiControl *parent = getParent();
   if ( parent )
      return parent->onMouseWheelUp( event );
   else
      return false;
}

bool GuiControl::onMouseWheelDown( const GuiEvent &event )
{
   //if this control is a dead end, make sure the event stops here
   if ( !mVisible || !mAwake )
      return true;

   //pass the event to the parent
   GuiControl *parent = getParent();
   if ( parent )
      return parent->onMouseWheelDown( event );
   else
      return false;
}

void GuiControl::onRightMouseDown(const GuiEvent &)
{
}

void GuiControl::onRightMouseUp(const GuiEvent &)
{	
}

void GuiControl::onRightMouseDragged(const GuiEvent &)
{
}

void GuiControl::onMiddleMouseDown(const GuiEvent &)
{
}

void GuiControl::onMiddleMouseUp(const GuiEvent &)
{
}

void GuiControl::onMiddleMouseDragged(const GuiEvent &)
{
}


GuiControl* GuiControl::findFirstTabable()
{
   GuiControl *tabCtrl = NULL;
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      tabCtrl = ctrl->findFirstTabable();
      if (tabCtrl)
      {
         mFirstResponder = tabCtrl;
         return tabCtrl;
      }
   }

   //nothing was found, therefore, see if this ctrl is tabable
   return ( mProfile != NULL ) ? ( ( mProfile->mTabable && mAwake && mVisible ) ? this : NULL ) : NULL;
}

GuiControl* GuiControl::findLastTabable(bool firstCall)
{
   //if this is the first call, clear the global
   if (firstCall)
      smPrevResponder = NULL;

   //if this control is tabable, set the global
   if (mProfile->mTabable)
      smPrevResponder = this;

   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      ctrl->findLastTabable(false);
   }

   //after the entire tree has been traversed, return the last responder found
   mFirstResponder = smPrevResponder;
   return smPrevResponder;
}

GuiControl *GuiControl::findNextTabable(GuiControl *curResponder, bool firstCall)
{
   //if this is the first call, clear the global
   if (firstCall)
      smCurResponder = NULL;

   //first find the current responder
   if (curResponder == this)
      smCurResponder = this;

   //if the first responder has been found, return the very next *tabable* control
   else if ( smCurResponder && mProfile->mTabable && mAwake && mVisible && mActive )
      return( this );

   //loop through, checking each child to see if it is the one that follows the firstResponder
   GuiControl *tabCtrl = NULL;
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      tabCtrl = ctrl->findNextTabable(curResponder, false);
      if (tabCtrl) break;
   }
   mFirstResponder = tabCtrl;
   return tabCtrl;
}

GuiControl *GuiControl::findPrevTabable(GuiControl *curResponder, bool firstCall)
{
   if (firstCall)
      smPrevResponder = NULL;

   //if this is the current reponder, return the previous one
   if (curResponder == this)
      return smPrevResponder;

   //else if this is a responder, store it in case the next found is the current responder
   else if ( mProfile->mTabable && mAwake && mVisible && mActive )
      smPrevResponder = this;

   //loop through, checking each child to see if it is the one that follows the firstResponder
   GuiControl *tabCtrl = NULL;
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      tabCtrl = ctrl->findPrevTabable(curResponder, false);
      if (tabCtrl) break;
   }
   mFirstResponder = tabCtrl;
   return tabCtrl;
}

void GuiControl::onLoseFirstResponder()
{
	// Since many controls have visual cues when they are the firstResponder...
	setUpdate();

   if( isMethod( "onLoseFirstResponder" ) )
      Con::executef(this, "onLoseFirstResponder" );
}

bool GuiControl::ControlIsChild(GuiControl *child)
{
   //function returns true if this control, or one of it's children is the child control
   if (child == this)
      return true;

   //loop through, checking each child to see if it is ,or contains, the firstResponder
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      if (ctrl->ControlIsChild(child)) return true;
   }

   //not found, therefore false
   return false;
}

bool GuiControl::isFirstResponder()
{
   GuiCanvas *root = getRoot();
   return root && root->getFirstResponder() == this;
}

void GuiControl::setFirstResponder( GuiControl* firstResponder )
{
   if ( firstResponder && firstResponder->getControlProfile()->mCanKeyFocus )
      mFirstResponder = firstResponder;

   GuiControl *parent = getParent();
   if ( parent )
      parent->setFirstResponder( firstResponder );

   if( isFirstResponder() && isMethod( "onGainFirstResponder" ) )
      Con::executef(this, "onGainFirstResponder" );
}

void GuiControl::setFirstResponder()
{
	if ( mAwake && mVisible )
	{
	   GuiControl *parent = getParent();
	   if (getControlProfile()->mCanKeyFocus == true && parent != NULL )
      {
         parent->setFirstResponder(this);

         // Since many controls have visual cues when they are the firstResponder...
         this->setUpdate();

         // Call Script.
         if( isFirstResponder() && isMethod( "onGainFirstResponder" ) )
            Con::executef(this, "onGainFirstResponder" );
      }
	}
}

void GuiControl::clearFirstResponder()
{
   GuiControl *parent = this;
   while((parent = parent->getParent()) != NULL)
   {
      if(parent->mFirstResponder == this)
         parent->mFirstResponder = NULL;
      else
         break;
   }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiControl::buildAcceleratorMap()
{
   //add my own accel key
   addAcceleratorKey();

   //add all my childrens keys
   iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      ctrl->buildAcceleratorMap();
   }
}

void GuiControl::addAcceleratorKey()
{
   //see if we have an accelerator
   if (mAcceleratorKey == StringTable->insert(""))
      return;

   EventDescriptor accelEvent;
   ActionMap::createEventDescriptor(mAcceleratorKey, &accelEvent);

   //now we have a modifier, and a key, add them to the canvas
   GuiCanvas *root = getRoot();
   if (root)
      root->addAcceleratorKey(this, 0, accelEvent.eventCode, accelEvent.flags);
}

void GuiControl::acceleratorKeyPress(U32 index)
{
   index;
   onAction();
}

void GuiControl::acceleratorKeyRelease(U32 index)
{
   index;
   //do nothing
}

bool GuiControl::onKeyDown(const GuiEvent &event)
{
   //pass the event to the parent
   GuiControl *parent = getParent();
   if (parent)
      return parent->onKeyDown(event);
   else
      return false;
}

bool GuiControl::onKeyRepeat(const GuiEvent &event)
{
   // default to just another key down.
   return onKeyDown(event);
}

bool GuiControl::onKeyUp(const GuiEvent &event)
{
   //pass the event to the parent
   GuiControl *parent = getParent();
   if (parent)
      return parent->onKeyUp(event);
   else
      return false;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiControl::onAction()
{
   if (! mActive)
      return;

   //execute the console command
   if (mConsoleCommand && mConsoleCommand[0])
   {
      execConsoleCallback();
   }
   else
      Con::executef(this, "onAction");
}

void GuiControl::onMessage(GuiControl *sender, S32 msg)
{
   sender;
   msg;
}

void GuiControl::messageSiblings(S32 message)
{
   GuiControl *parent = getParent();
   if (! parent) return;
   GuiControl::iterator i;
   for(i = parent->begin(); i != parent->end(); i++)
   {
      GuiControl *ctrl = dynamic_cast<GuiControl *>(*i);
      if (ctrl != this)
         ctrl->onMessage(this, message);
   }
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiControl::onDialogPush()
{
   // Notify Script.
   if( isMethod("onDialogPush") )
      Con::executef(this, "onDialogPush");

}

void GuiControl::onDialogPop()
{
   // Notify Script.
   if( isMethod("onDialogPop") )
      Con::executef(this, "onDialogPop");
}

void GuiControl::parentSetVisible(bool value)
{
	iterator i;

	if(onParentSetVisible(value))
		return;

	for(i = begin(); i != end(); i++)
	{
		GuiControl *ctrl = static_cast<GuiControl *>(*i);
		ctrl->parentSetVisible(value);
	}
}

//------------------------------------------------------------------------------
void GuiControl::setVisible(bool value)
{
	if(mVisible!=value)
		parentSetVisible(value);

	mVisible = value;
   iterator i;
   setUpdate();
   onSetVisible(value);
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      ctrl->clearFirstResponder();
	}

	GuiControl *parent = getParent();
	if (parent)
	   parent->childResized(this);
}


void GuiControl::makeFirstResponder(bool value)
{
   if ( value )
      //setFirstResponder(this);
      setFirstResponder();
   else
      clearFirstResponder();
}

void GuiControl::setActive( bool value )
{
   mActive = value;

   if ( !mActive )
      clearFirstResponder();

   if ( mVisible && mAwake )
      setUpdate();
}

void GuiControl::getScrollLineSizes(U32 *rowHeight, U32 *columnWidth)
{
	// default to 10 pixels in y, 30 pixels in x
	*columnWidth = 30;
	*rowHeight = 30;
}

void GuiControl::renderJustifiedText(Point2I offset, Point2I extent, const char *text)
{
   CommonFontEX *font = getControlProfile()->mFont;
   S32 textWidth = font->getStrWidth(( UTF8*)text);
   Point2I start;

   // align the horizontal
   switch( getControlProfile()->mAlignment )
   {
      case GuiControlProfile::RightJustify:
         start.set( extent.x - textWidth, 0 );
         break;
      case GuiControlProfile::CenterJustify:
         start.set( ( extent.x - textWidth) / 2, 0 );
         break;
      default:
         // GuiControlProfile::LeftJustify
         start.set( 0, 0 );
         break;
   }
 
   // If the text is longer then the box size, (it'll get clipped) so
   // force Left Justify

   if( textWidth > extent.x )
      start.set( 0, 0 );

   // center the vertical
   if(font->getHeight() > extent.y)
      start.y = 0 - ((font->getHeight() - extent.y) / 2) ;
   else
      start.y = ( extent.y - font->getHeight() ) / 2;

   ColorI color;
   GFX->getDrawUtil()->getBitmapModulation().getColor(&color);

   if (mProfile->mFontOutline == ColorI(0,0,0))
	   GFX->getDrawUtil()->drawText( font, start + offset, text, &color );
   else
	   GFX->getDrawUtil()->drawTextOutline(font, start + offset, text, &color,true,mProfile->mFontOutline);
}

void GuiControl::renderJustifiedTextN(Point2I offset,Point2I extent,const char *text,U32 len)
{
	char tempText[256]={0,};
	dStrncpy(tempText, sizeof(tempText), text, len);
	CommonFontEX *font = getControlProfile()->mFont;
	S32 textWidth = font->getStrWidth(( UTF8*)tempText);

	if (textWidth > extent.x)
	{
		U32 ilen;
		U32 iWidth = font->getStrNWidth("...",3);
		ilen = font->getBreakPos(tempText,len,extent.x - iWidth,false);
		dStrncpy(tempText, sizeof(tempText), text, ilen);
		tempText[ilen] = 0;
		dStrcat(tempText, sizeof(tempText), "...");
	}

	renderJustifiedText(offset,extent,tempText);
}

void GuiControl::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent)
{
#ifdef _XBOX
   return;
#endif

   lastGuiEvent;

   if( !getRoot() )
      return;

   if(getRoot()->mCursorChanged != -1 && !isMouseLocked())
   {
      // We've already changed the cursor, 
      // so set it back before we change it again.

      PlatformWindow *pWindow = static_cast<GuiCanvas*>(getRoot())->getPlatformWindow();
      AssertFatal(pWindow != NULL,"GuiControl without owning platform window!  This should not be possible.");
      PlatformCursorController *pController = pWindow->getCursorController();
      AssertFatal(pController != NULL,"PlatformWindow without an owned CursorController!");

      pController->popCursor();

      // We haven't changed it
      getRoot()->mCursorChanged = -1;
   }
}

StringTableEntry GuiControl::getScriptFile()
{
   return mScriptFile;
}

const char* GuiControl::execConsoleCallback()
{
   if (mConsoleCommand && mConsoleCommand[0])
   {
      Con::setVariable("$ThisControl", avar("%d",getId()));
      return Con::evaluate(mConsoleCommand, false);
   }
   return "";
}

const char* GuiControl::execAltConsoleCallback()
{
   if(mAltConsoleCommand && mAltConsoleCommand[0])
   {
      Con::setVariable("$ThisControl", avar("%d",getId()));
      return Con::evaluate(mAltConsoleCommand, false);
   }
   return "";
}

bool GuiControl::onGamepadButtonDown(const GuiEvent &event)
{
   return onKeyDown(event);
}

bool GuiControl::onGamepadButtonUp(const GuiEvent &event)
{
   return onKeyUp(event);
}

bool GuiControl::onGamepadAxisUp(const GuiEvent &event)
{
   //pass the event to the parent
   GuiControl *parent = getParent();
   if (parent)
   {
      return parent->onGamepadAxisUp(event);
   }
   else
   {
      return false;
   }
}

bool GuiControl::onGamepadAxisDown(const GuiEvent &event)
{
   //pass the event to the parent
   GuiControl *parent = getParent();
   if (parent)
   {
      return parent->onGamepadAxisDown(event);
   }
   else
   {
      return false;
   }
}

bool GuiControl::onGamepadAxisLeft(const GuiEvent &event)
{
   //pass the event to the parent
   GuiControl *parent = getParent();
   if (parent)
   {
      return parent->onGamepadAxisLeft(event);
   }
   else
   {
      return false;
   }
}

bool GuiControl::onGamepadAxisRight(const GuiEvent &event)
{
   //pass the event to the parent
   GuiControl *parent = getParent();
   if (parent)
   {
      return parent->onGamepadAxisRight(event);
   }
   else
   {
      return false;
   }
}

bool GuiControl::onGamepadTrigger(const GuiEvent &event)
{
   //pass the event to the parent
   GuiControl *parent = getParent();
   if (parent)
   {
      return parent->onGamepadTrigger(event);
   }
   else
   {
      return false;
   }
}

bool GuiControl::execChildMethod(const char *className,const char *classMethod)
{
	if (!className || !className[0])
		return false;

	if(!classMethod || !classMethod[0])
		return false;

	iterator i;
	for (i = begin(); i != end(); i++)
	{
		GuiControl* ctrl = dynamic_cast<GuiControl*>(*i);
		if (ctrl && dStrcmp(ctrl->getClassName(),className) == 0) // 查找对应类名
		{
			if(dStrcmp(Con::executef(ctrl,classMethod),"1") == 0) // 执行是否成功
				continue;
			else
				return false;
		}
		else
		{
			continue;
		}
	}
	return true;
}


void GuiControl::resetStateBlock()
{
	//mCullNoneSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mCullNoneSB);
}


void GuiControl::releaseStateBlock()
{
	if (mCullNoneSB)
	{
		mCullNoneSB->release();
	}
}

void GuiControl::init()
{
	if (mCullNoneSB == NULL)
	{
		mCullNoneSB = new GFXD3D9StateBlock;
		mCullNoneSB->registerResourceWithDevice(GFX);
		mCullNoneSB->mZombify = &releaseStateBlock;
		mCullNoneSB->mResurrect = &resetStateBlock;

		resetStateBlock();
	}
}

void GuiControl::shutdown()
{
	SAFE_DELETE(mCullNoneSB);
}


ConsoleMethod( GuiControl, findHitControl, S32, 4, 4, "returns the Id of the control at the point")
{
   Point2I pos(dAtoi(argv[2]), dAtoi(argv[3]));
   GuiControl *hit = object->findHitControl(pos);
   return hit ? hit->getId() : 0;
}

ConsoleMethod(GuiControl, isFirstResponder, bool, 2, 2, "returns true if this control is the first responder,")
{
   return object->isFirstResponder();
}

ConsoleMethod(GuiControl, setFirstResponder, void , 2, 2, "Sets this control as the first responder")
{
   object->setFirstResponder();
}

ConsoleMethod( GuiControl, getScriptFile, const char*, 2, 2, "GuiControl.getScriptFile() - gets the script file the control was created in")
{
   return object->getScriptFile();

}
ConsoleMethod( GuiControl, setCanSave, void, 3,3,"Sets whether this control can serialize itself to the hard disk")
{
   object->setCanSave( dAtob( argv[2] ) );
}


ConsoleMethod(GuiControl, pointInControl, bool, 4,4,"returns true if the point is in the control, point is in parent coords")
{
   Point2I kPoint(dAtoi(argv[2]), dAtoi(argv[3]));
   return object->pointInControl(kPoint);
}


ConsoleMethod( GuiControl, addGuiControl, void, 3, 3, "S32 controlId")
{

   GuiControl *ctrl = dynamic_cast<GuiControl *>(Sim::findObject(argv[2]));
   if(ctrl)
   {
      object->addObject(ctrl);
   }

}

ConsoleMethod(GuiControl, reorderChild, void, 4,4," (child1, child2) uses simset reorder to push child 1 after child 2 - both must already be child controls of this control")
{
   GuiControl* pControl = dynamic_cast<GuiControl*>(Sim::findObject(dAtoi(argv[2])));
   GuiControl* pTarget	 = dynamic_cast<GuiControl*>(Sim::findObject(dAtoi(argv[3])));

   if(pControl && pTarget)
   {
      object->reOrder(pControl,pTarget);
   }
}

ConsoleMethod( GuiControl, getRoot, S32, 2, 2, "returns the Id of the parent canvas.")
{
   GuiCanvas *root = object->getRoot();
   if(root)
      return root->getId();
   return 0;
}

ConsoleMethod( GuiControl, getParent, S32, 2, 2, "returns the Id of the parent control")
{

   GuiControl* pParent		= object->getParent();
   if(pParent)
   {
      return pParent->getId();
   }

   return 0;

}

ConsoleMethod( GuiControl, setValue, void, 3, 3, "(string value)")
{
   object->setScriptValue(argv[2]);
}

ConsoleMethod( GuiControl, getValue, const char*, 2, 2, "")
{
   return object->getScriptValue();
}

ConsoleMethod( GuiControl, setActive, void, 3, 3, "(bool active)")
{
   object->setActive(dAtob(argv[2]));
}

ConsoleMethod( GuiControl, isActive, bool, 2, 2, "")
{
   return object->isActive();
}

ConsoleMethod( GuiControl, setVisible, void, 3, 3, "(bool visible)")
{
	bool oldVisible = object->mVisible;
	object->setVisible(dAtob(argv[2]));

	if(dAtob(argv[2]) == false && oldVisible == true)
		Con::executef("activeChatWnd");
}

ConsoleMethod( GuiControl, makeFirstResponder, void, 3, 3, "(bool isFirst)")
{
   object->makeFirstResponder(dAtob(argv[2]));
}

ConsoleMethod( GuiControl, isVisible, bool, 2, 2, "")
{
   return object->isVisible();
}

ConsoleMethod( GuiControl, isAwake, bool, 2, 2, "")
{
   return object->isAwake();
}

ConsoleMethod( GuiControl, setProfile, void, 3, 3, "(GuiControlProfile p)")
{
   GuiControlProfile * profile;

   if(Sim::findObject(argv[2], profile))
      object->setControlProfile(profile);
}

ConsoleMethod( GuiControl, resize, void, 6, 6, "(int x, int y, int w, int h)")
{
   Point2I newPos(dAtoi(argv[2]), dAtoi(argv[3]));
   Point2I newExt(dAtoi(argv[4]), dAtoi(argv[5]));
   object->resize(newPos, newExt);
}

ConsoleMethod( GuiControl, getPosition, const char*, 2, 2, "")
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I &pos = object->getPosition();
   dSprintf(retBuffer, 64, "%d %d", pos.x, pos.y);
   return retBuffer;
}
ConsoleMethod( GuiControl, getCenter, const char*, 2, 2, " returns center of control, as space seperated ints")
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I pos = object->getPosition();
   const Point2I ext = object->getExtent();
   Point2I center(pos.x + ext.x/2, pos.y + ext.y/2);
   dSprintf(retBuffer, 64, "%d %d", center.x, center.y);
   return retBuffer;
}

ConsoleMethod( GuiControl, setCenter, void, 4, 4, " sets control position, by center - coords are local not global")
{
   const Point2I ext = object->getExtent();
   Point2I newpos(dAtoi(argv[2])-ext.x/2, dAtoi(argv[3])-ext.y/2);
   object->setPosition(newpos);
}

ConsoleMethod( GuiControl, getGlobalCenter, const char*, 2, 2, " returns center of control, as space seperated ints")
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I tl(0,0);
   Point2I pos		 =	object->localToGlobalCoord(tl);
   const Point2I ext = object->getExtent();
   Point2I center(pos.x + ext.x/2, pos.y + ext.y/2);
   dSprintf(retBuffer, 64, "%d %d", center.x, center.y);
   return retBuffer;
}

ConsoleMethod( GuiControl, getGlobalPosition, const char*, 2, 2, "")
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I pos(0,0);
   Point2I gPos	=	object->localToGlobalCoord(pos);

   dSprintf(retBuffer, 64, "%d %d", gPos.x, gPos.y);
   return retBuffer;
}
ConsoleMethod( GuiControl, setPositionGlobal, void, 4, 4, "int x,y in global screen space")
{
   //see if we can turn the x/y into ints directly, 
   Point2I gPos(dAtoi(argv[2]), dAtoi(argv[3]));
   Point2I lPosOffset	=	object->globalToLocalCoord(gPos);
   
   lPosOffset += object->getPosition();
   
   object->setPosition(lPosOffset);
}

ConsoleMethod( GuiControl, setPosition, void, 4, 4, "int x,y in local space")
{
   //see if we can turn the x/y into ints directly, 
   Point2I lPos(dAtoi(argv[2]), dAtoi(argv[3]));
   object->setPosition(lPos);
}

ConsoleMethod( GuiControl, getExtent, const char*, 2, 2, "Get the width and height of the control.")
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I &ext = object->getExtent();
   dSprintf(retBuffer, 64, "%d %d", ext.x, ext.y);
   return retBuffer;
}

ConsoleMethod( GuiControl, setExtent, void, 4, 4, " sets the width & height of the control.")
{
   Point2I kExt(dAtoi(argv[2]), dAtoi(argv[3]));
   object->setExtent(kExt);
}

ConsoleMethod( GuiControl, getMinExtent, const char*, 2, 2, "Get the minimum allowed size of the control.")
{
   char *retBuffer = Con::getReturnBuffer(64);
   Point2I minExt = object->getMinExtent();
   dSprintf(retBuffer, 64, "%d %d", minExt.x, minExt.y);
   return retBuffer;
}

ConsoleMethod(GuiControl,execChildMethod,bool,4,4,"遍历子控件，执行指定类名下的指定方法")
{
	return object->execChildMethod(argv[2],argv[3]);
}


