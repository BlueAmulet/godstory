#ifndef __AUTONPCSELL_H__
#define __AUTONPCSELL_H__

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

class ItemShortcut;
class Player;
class Stream;

class AutoNpcSellCache
{
public:
	typedef Vector<ItemShortcut*> AutoNpcSellData;
	AutoNpcSellCache();
	~AutoNpcSellCache();
	void loadCache(Player* player);
	void saveCache(Player* player);
	void setUpdate() { mUpdatedCache = true;}
	void clear();
	bool read(Player* player, Stream* stream);
	bool write(Player* player, Stream* stream);
private:
	AutoNpcSellData mData;		// 自动售卖数据
	bool mLoadedCache;			// 是否已经载入过本地数据
	bool mUpdatedCache;			//
};

extern AutoNpcSellCache* g_AutoNpcSellCache;

#endif//__AUTONPCSELL_H__