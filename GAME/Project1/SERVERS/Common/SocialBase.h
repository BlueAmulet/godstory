#ifndef SOCIAL_BASE_H
#define SOCIAL_BASE_H

namespace SocialType
{
	enum Type
	{
		Temp,			// 临时
		Consort,		// 配偶
		Buddie,			// 兄弟
		Master,			// 师傅
		Prentice,		// 徒弟		
		Friend,			// 好友
		Enemy,			// 仇人
		Mute,			// 屏蔽

		Count,
	} ;

    static const char* strType[Count] = 
    {
        "临时",
        "好友",
        "配偶",
        "师傅",
        "徒弟",
        "兄弟",
        "仇人",
        "屏蔽",
    };
} ;

struct SocialError
{
	enum Error
	{
		None,

		Self,			// 不能和自身成为社会关系
		MaxCount,		// 此社会关系人数达到最大上限
		NotFound,		// 目标玩家不存在
		AutoRefused,	// 自动拒绝
		Existed,		// 目标玩家已是此在社会关系列表中
		MaxChear,		// 加油数已满
		MaxCheared,		// 被加油数已满
		ChearCooldown,	// 加油冷却中

		Unknown,		// 未知

		Count
	};
};

static const int MaxChearCount		= 3;		// 最大加油数
static const int MaxChearedCount	= 5;		// 最大被加油数

static int SocialTypeMaxCount[ SocialType::Count ] = 
{
		50,				// 临时
		200,			// 好友
		1,				// 配偶
		1,				// 师傅
		5,				// 徒弟
		5,				// 兄弟
		20,				// 仇人
		100				// 屏蔽
} ;


struct stPlayerSocialInfo
{
	int		id;					// ID
	char	name[32];			// 名称
	int		level;				// 级别
	int		friendValue;		// 友好度
	int		soulLevel;			// 元神等级
	char	mapName[8];			// 所在地图
	char	consortName[32];	// 配偶名称
	int		family;				// 门宗
	char	groupName[32];		// 帮会
	char	status;				// 当前状态
	char	type;				// 关系
	int		sex;

	stPlayerSocialInfo()
	{
		memset( this, 0, sizeof( stPlayerSocialInfo ) );
	}

	template< typename _Ty >
	void WritePakcet( _Ty* pPacket )
	{
		pPacket->writeInt( id, 32 );
		pPacket->writeString( name,32 );
		pPacket->writeInt( level, 8 );
		pPacket->writeInt( friendValue, 32 );
		pPacket->writeInt( soulLevel, 8 );
		pPacket->writeString( mapName,8 );
		pPacket->writeString( consortName ,32);
		pPacket->writeInt( family, 8 );
		pPacket->writeString( groupName ,32);
		pPacket->writeInt( status, 8 );
		pPacket->writeInt( type, 8 );
		pPacket->writeInt( sex, 2 );
	}

	template< typename _Ty >
	void ReadPacket( _Ty* pPacket )
	{
		id = pPacket->readInt( 32 );
		pPacket->readString( name ,32);
		level = pPacket->readInt( 8 );
		friendValue = pPacket->readInt( 32 );
		soulLevel = pPacket->readInt( 8 );
		pPacket->readString( mapName ,8);
		pPacket->readString( consortName ,32);
		family = pPacket->readInt( 8 );
		pPacket->readString( groupName ,32);
		status = pPacket->readInt( 8 );
		type = pPacket->readInt( 8 );
		sex = pPacket->readInt( 2 );
	}

};

// 玩家社会关系结构
struct stSocialInfo
{
	int		id;					// ID
	char	name[32];			// 名称
	int		level;				// 级别
	int		family;				// 门宗
	int		type;				// 社会关系类型
	int		status;				// 状态

	int		sex;				// 性别
	int		friendValue;		// 好友度

	stSocialInfo()
	{
		memset( this, 0, sizeof( stSocialInfo ) );
	}

	stSocialInfo( const stSocialInfo& other )
	{
		copy( other );
	}

	stSocialInfo& operator = ( const stSocialInfo& other )
	{
		copy( other );
		return *this;
	}

	void copy( const stSocialInfo& other )
	{
		id = other.id;
		strcpy_s( name, other.name );
		level = other.level;
		family = other.family;
		type = other.type;
		status = other.status;
		friendValue = other.friendValue;
	}

	template< typename _Ty >
	void WritePakcet( _Ty* pPacket )
	{
		pPacket->writeInt( id, 32 );
		pPacket->writeString( name ,32);
		pPacket->writeInt( level, 8 );
		pPacket->writeInt( family, 8 );
		pPacket->writeInt( type, 8 );
		pPacket->writeInt( status, 8 );
		pPacket->writeInt( sex, 2 );
		pPacket->writeInt( friendValue, 16 );
	}

	template< typename _Ty >
	void ReadPacket( _Ty* pPacket )
	{
		id = pPacket->readInt( 32 );
		pPacket->readString( name,32 );
		level = pPacket->readInt( 8 );
		family = pPacket->readInt( 8 );
		type = pPacket->readInt( 8 );
		status = pPacket->readInt( 8 );
		sex = pPacket->readInt( 2 );
		friendValue = pPacket->readInt( 16 );
	}
};

typedef int SocialTypeCount[ SocialType::Count ];

#endif