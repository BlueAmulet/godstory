//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GATHERMATERIAL_H_
#define _GATHERMATERIAL_H_

class Player;
class CollectionObject;
class GatherMaterial
{

public:
	GatherMaterial();
	enWarnMessage canGather(Player* pPlayer,U32 LivingSkillId);
	enWarnMessage spellLivingSkill(Player* pPlayer,CollectionObject* obj);
#ifdef NTJ_SERVER
	enWarnMessage gatherCast(Player* pPlayer,U32 LivingSkillId);
#endif
	
};
extern GatherMaterial g_GatherMaterial;

#endif
