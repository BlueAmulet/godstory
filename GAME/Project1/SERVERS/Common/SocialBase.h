#ifndef SOCIAL_BASE_H
#define SOCIAL_BASE_H

namespace SocialType
{
	enum Type
	{
		Temp,			// ��ʱ
		Consort,		// ��ż
		Buddie,			// �ֵ�
		Master,			// ʦ��
		Prentice,		// ͽ��		
		Friend,			// ����
		Enemy,			// ����
		Mute,			// ����

		Count,
	} ;

    static const char* strType[Count] = 
    {
        "��ʱ",
        "����",
        "��ż",
        "ʦ��",
        "ͽ��",
        "�ֵ�",
        "����",
        "����",
    };
} ;

struct SocialError
{
	enum Error
	{
		None,

		Self,			// ���ܺ������Ϊ����ϵ
		MaxCount,		// ������ϵ�����ﵽ�������
		NotFound,		// Ŀ����Ҳ�����
		AutoRefused,	// �Զ��ܾ�
		Existed,		// Ŀ��������Ǵ�������ϵ�б���
		MaxChear,		// ����������
		MaxCheared,		// ������������
		ChearCooldown,	// ������ȴ��

		Unknown,		// δ֪

		Count
	};
};

static const int MaxChearCount		= 3;		// ��������
static const int MaxChearedCount	= 5;		// ��󱻼�����

static int SocialTypeMaxCount[ SocialType::Count ] = 
{
		50,				// ��ʱ
		200,			// ����
		1,				// ��ż
		1,				// ʦ��
		5,				// ͽ��
		5,				// �ֵ�
		20,				// ����
		100				// ����
} ;


struct stPlayerSocialInfo
{
	int		id;					// ID
	char	name[32];			// ����
	int		level;				// ����
	int		friendValue;		// �Ѻö�
	int		soulLevel;			// Ԫ��ȼ�
	char	mapName[8];			// ���ڵ�ͼ
	char	consortName[32];	// ��ż����
	int		family;				// ����
	char	groupName[32];		// ���
	char	status;				// ��ǰ״̬
	char	type;				// ��ϵ
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

// �������ϵ�ṹ
struct stSocialInfo
{
	int		id;					// ID
	char	name[32];			// ����
	int		level;				// ����
	int		family;				// ����
	int		type;				// ����ϵ����
	int		status;				// ״̬

	int		sex;				// �Ա�
	int		friendValue;		// ���Ѷ�

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