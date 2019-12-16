//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/GameObjects/ShapesSet.h"


// ========================================================================================================================================
//  ShapeStack
// ========================================================================================================================================
class ShapeBase;

class ShapeStack
{
public:

	// 显示优先级从低到高
	enum Stack
	{
		Stack_Base,
		Stack_Equipment,
		Stack_Fashion,
		Stack_FamilyFashion,
		Stack_Buff,
		Stack_Stall,

		MaxStacks,
	};

	enum StackFlags
	{
		Flags_Enabled			= BIT(0),	// 是否启用
		Flags_IgnoreLower		= BIT(1),	// 在启用情况下忽视层级较低的其他模型(必须要有主模型)
		Flags_Valid				= BIT(2),	// 是否有效
	};

protected:

	ShapesSet					mFinalShape;
	ShapesSet					mStacks[MaxStacks];
	U32							mStackFlags[MaxStacks];
	U32							mRefreshMask;
    bool                        mIsFinalShapeChanged;

private:
	void						add							(Stack stack, S32 slot, StringTableEntry shape, StringTableEntry skin, StringTableEntry lp, StringTableEntry lpSelf, U32 effectId);
	void						remove						(Stack stack, S32 slot);

public:

	ShapeStack();
	~ShapeStack();

	void						add							(Stack stack, U32 shapesSetId, U32 effectId = 0);
	void						remove						(Stack stack, U32 shapesSetId);

	bool						isFlags						(Stack stack, StackFlags flag);
	void						setFlags					(Stack stack, StackFlags flag, bool enable);

	void						refresh						(ShapeBase* pObj = NULL);
	inline const ShapesSet&		getShapes					() { return mFinalShape;}
	void						setShapes					(S32 slot, StringTableEntry shape, StringTableEntry skin, StringTableEntry lp, StringTableEntry lpSelf,U32 effectId);

    bool                        isFinalShapeChanged(void) const {return mIsFinalShapeChanged;}
    void                        clrFinalShapeChanged(void)      {mIsFinalShapeChanged = false;}
};
