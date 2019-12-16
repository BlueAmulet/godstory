//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/ShapeStack.h"
#include "t3d/shapeBase.h"


// ========================================================================================================================================
//  ShapeStack
// ========================================================================================================================================
#define Macro_isFlag(stack, flag) (bool(mStackFlags[stack] & flag))

ShapeStack::ShapeStack()
{
	dMemset(mStackFlags, 0, sizeof(mStackFlags));
	mRefreshMask = 0;
    mIsFinalShapeChanged = false;

	mStackFlags[Stack_Base] = Flags_Enabled | Flags_Valid;
	mStackFlags[Stack_Equipment] = Flags_Enabled | Flags_Valid;
	mStackFlags[Stack_Fashion] = Flags_Valid;
	mStackFlags[Stack_FamilyFashion] = Flags_Enabled | Flags_Valid;
	mStackFlags[Stack_Buff] = Flags_Enabled | Flags_IgnoreLower | Flags_Valid;
	mStackFlags[Stack_Stall] = Flags_Enabled | Flags_IgnoreLower | Flags_Valid;
}

ShapeStack::~ShapeStack()
{
}

void ShapeStack::add(Stack stack, U32 shapesSetId, U32 effectId /* = 0 */)
{
	if(stack >= MaxStacks)
		return;
	const ShapesSet* set = g_ShapesSetRepository.GetSet(shapesSetId);
	if(!set)
		return;

	if(set->shapeName && set->shapeName[0])
	{
		add(stack, -1, set->shapeName, set->skinName, 0, 0, effectId);
		// 不可重复
		effectId = 0;
	}

	for (S32 i=0; i<ShapeBase::MaxMountedImages; ++i)
		if(set->subImages[i].shapeName && set->subImages[i].shapeName[0])
		{
			add(stack, i, set->subImages[i].shapeName, set->subImages[i].skinName, set->subImages[i].mountPoint, set->subImages[i].mountPointSelf, effectId);
			// 不可重复
			effectId = 0;
		}

	mRefreshMask |= BIT(stack);
}

void ShapeStack::add(Stack stack, S32 slot, StringTableEntry shape, StringTableEntry skin, StringTableEntry lp, StringTableEntry lpSelf, U32 effectId)
{
	if(stack >= MaxStacks || slot < -1 || slot >= ShapeBase::MaxMountedImages)
		return;

	if(!(shape && shape[0]))
		return;

	if(slot == -1)
	{
		AssertFatal(!(mStacks[stack].shapeName && mStacks[stack].shapeName[0]), "add error - ShapesSet::shapeName");
		mStacks[stack].shapeName = StringTable->insert(shape);
		mStacks[stack].skinName = StringTable->insert(skin);
		mStacks[stack].effectId = effectId;
	}
	else
	{
		AssertFatal(!(mStacks[stack].subImages[slot].shapeName && mStacks[stack].subImages[slot].shapeName[0]), "add error - ShapesSet::subImages::shapeName");
		mStacks[stack].subImages[slot].shapeName = StringTable->insert(shape);
		mStacks[stack].subImages[slot].skinName = StringTable->insert(skin);
		mStacks[stack].subImages[slot].mountPoint = StringTable->insert(lp);
		mStacks[stack].subImages[slot].mountPointSelf = StringTable->insert(lpSelf);
		mStacks[stack].subImages[slot].effectId = effectId;
	}

	mRefreshMask |= BIT(stack);
}

void ShapeStack::remove(Stack stack, U32 shapesSetId)
{
	if(stack >= MaxStacks)
		return;
	const ShapesSet* set = g_ShapesSetRepository.GetSet(shapesSetId);
	if(!set)
		return;

	if(set->shapeName && set->shapeName[0])
		remove(stack, -1);

	for (S32 i=0; i<ShapeBase::MaxMountedImages; ++i)
		if(set->subImages[i].shapeName && set->subImages[i].shapeName[0])
			remove(stack, i);

	mRefreshMask |= BIT(stack);
}

void ShapeStack::remove(Stack stack, S32 slot)
{
	if(stack >= MaxStacks || slot < -1 || slot >= ShapeBase::MaxMountedImages)
		return;

	if(slot == -1)
	{
		AssertFatal(mStacks[stack].shapeName && mStacks[stack].shapeName[0], "remove error - ShapesSet::shapeName");
		mStacks[stack].shapeName = NULL;
		mStacks[stack].skinName = NULL;
		mStacks[stack].effectId = 0;
	}
	else
	{
		AssertFatal(mStacks[stack].subImages[slot].shapeName && mStacks[stack].subImages[slot].shapeName[0], "remove error - ShapesSet::subImages::shapeName");
		mStacks[stack].subImages[slot].shapeName = NULL;
		mStacks[stack].subImages[slot].skinName = NULL;
		mStacks[stack].subImages[slot].mountPoint = NULL;
		mStacks[stack].subImages[slot].mountPointSelf = NULL;
		mStacks[stack].subImages[slot].effectId = 0;
	}

	mRefreshMask |= BIT(stack);
}

bool ShapeStack::isFlags(Stack stack, StackFlags flag)
{
	if(stack >= MaxStacks)
		return false;

	return Macro_isFlag(stack, flag);
}

void ShapeStack::setFlags(Stack stack, StackFlags flag, bool enable)
{
	if(stack >= MaxStacks)
		return;

	if(enable)
	{
		if(Macro_isFlag(stack, flag))
			return;
		mStackFlags[stack] |= flag;
	}
	else
	{
		if(!Macro_isFlag(stack, flag))
			return;
		mStackFlags[stack] &= ~flag;
	}

	mRefreshMask |= BIT(stack);
}

void ShapeStack::refresh(ShapeBase* pObj)
{
	if(!mRefreshMask)
		return;

	dMemset(&mFinalShape, 0, sizeof(ShapesSet));
	// 遍历每一层堆栈
	for (S32 i=(S32)MaxStacks-1; i>=0; --i)
	{
		if(!isFlags((Stack)i, ShapeStack::Flags_Enabled))
		{
			mRefreshMask &= ~BIT(i);
			continue;
		}
		if((mRefreshMask & BIT(i)) || isFlags((Stack)i, ShapeStack::Flags_Valid))
		{
			bool valid = false;
			if(mStacks[i].shapeName && mStacks[i].shapeName[0])
			{
				if(!(mFinalShape.shapeName && mFinalShape.shapeName[0]))
				{
					mFinalShape.shapeName = StringTable->insert(mStacks[i].shapeName);
					mFinalShape.skinName = StringTable->insert(mStacks[i].skinName);
					mFinalShape.effectId = mStacks[i].effectId;

                    mIsFinalShapeChanged = true;
				}
				valid |= true;
			}
			for (S32 j=0; j<ShapeBase::MaxMountedImages; ++j)
			{
				if(mStacks[i].subImages[j].shapeName && mStacks[i].subImages[j].shapeName[0])
				{
					if(!(mFinalShape.subImages[j].shapeName && mFinalShape.subImages[j].shapeName[0]))
					{
						mFinalShape.subImages[j].shapeName = mStacks[i].subImages[j].shapeName;
						mFinalShape.subImages[j].skinName = mStacks[i].subImages[j].skinName;
						mFinalShape.subImages[j].mountPoint = mStacks[i].subImages[j].mountPoint;
						mFinalShape.subImages[j].mountPointSelf = mStacks[i].subImages[j].mountPointSelf;
						mFinalShape.subImages[j].effectId = mStacks[i].subImages[j].effectId;

                        mIsFinalShapeChanged = true;
					}
					valid |= true;
				}
			}

			setFlags((Stack)i, ShapeStack::Flags_Valid, valid);
			mRefreshMask &= ~BIT(i);
		}

		if(!isFlags((Stack)i, ShapeStack::Flags_Valid))
			continue;

		if(mStacks[i].shapeName && mStacks[i].shapeName[0] && isFlags((Stack)i, ShapeStack::Flags_IgnoreLower))
			break;
	}
	if(pObj)
		pObj->setShapeShifting(isFlags(Stack_Buff, Flags_Valid) && isFlags(Stack_Buff, Flags_Enabled));
}

void ShapeStack::setShapes(S32 slot, StringTableEntry shape, StringTableEntry skin, StringTableEntry lp, StringTableEntry lpSelf,U32 effectId)
{
#ifndef NTJ_SERVER
	if(slot < -1 || slot >= ShapeBase::MaxMountedImages)
		return;

    mIsFinalShapeChanged = true;

	if(slot == -1)
	{
		mFinalShape.shapeName = StringTable->insert(shape);
		mFinalShape.skinName  = StringTable->insert(skin);
        mFinalShape.effectId  = effectId;
	}
	else
	{
		mFinalShape.subImages[slot].shapeName  = StringTable->insert(shape);
		mFinalShape.subImages[slot].skinName   = StringTable->insert(skin);
		mFinalShape.subImages[slot].mountPoint = StringTable->insert(lp);
		mFinalShape.subImages[slot].mountPointSelf = StringTable->insert(lpSelf);
        mFinalShape.subImages[slot].effectId   = effectId;
	}
#endif
}

