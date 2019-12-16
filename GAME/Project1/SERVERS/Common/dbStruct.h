#ifndef DB_STRUCT_H
#define DB_STRUCT_H

#include <hash_map>
#include <string>
#include <assert.h>

//#include "BackWorker.h"
#include "base/bitStream.h"

class CDBOperator;
class CDBConnPool;

#define CLASS_OFFSET(x,cls) ((int)((const char *)&(((cls *)1)->x)-(const char *)1))

enum DBType
{
	DB_TYPE_INDEX,
	DB_TYPE_INT,
    DB_TYPE_BIGINT,
    DB_TYPE_FLOAT,
	DB_TYPE_STRING,
	DB_TYPE_BINARY,
	DB_TYPE_BOOL,
	DB_TYPE_DATETIME,
};

extern void _db_field_pack( Base::BitStream& packet, const int& value, int count );
extern void _db_field_pack( Base::BitStream& packet, const float& value, int count );
extern void _db_field_pack( Base::BitStream& packet, const std::string& value, int count );

extern void _db_field_unpack( Base::BitStream& packet, int& value, int count );
extern void _db_field_unpack( Base::BitStream& packet, float& value, int count );
extern void _db_field_unpack( Base::BitStream& packet, std::string& value, int count );

extern void _db_field_pack( Base::BitStream& packet, const U64& value, int count );
extern void _db_field_unpack( Base::BitStream& packet, U64& value, int count );

extern void _tostr( int& value, std::string& str );
extern void _fromstr( int& value, std::string& str );

extern void _tostr( float& value, std::string& str );
extern void _fromstr( float& value, std::string& str );

extern void _tostr( std::string& value, std::string& str );
extern void _fromstr( std::string& value, std::string& str );

extern void _tostr( U64& value, std::string& str );
extern void _fromstr( U64& value, std::string& str );

struct dbField
{
	dbField( const char* dbName, DBType dbType, int bitCount = 0 )
	{
		mDBName = dbName;
		mDBType = dbType;
		mBitCount = bitCount;
	}

	std::string mDBName;
	DBType mDBType;
	int mBitCount;

	virtual void pack( Base::BitStream& packet ) = 0;
	virtual void unpack( Base::BitStream& packet ) = 0;

	virtual std::string toString() = 0;
	virtual void fromString( std::string str ) = 0;

};

struct dbStruct
{
    typedef std::vector<dbField*> FIELD_MAP;

	dbStruct( const char* structName, const char* dbName )
	{
		mStructName     = structName;
		mDBName         = dbName;
	}

	void AddField(dbField* pField) 
	{
        assert(0 != pField);
		m_fields.push_back(pField);
	}

    void pack(Base::BitStream& packet)
    {
        for (size_t i = 0; i < m_fields.size(); ++i)
        {
            m_fields[i]->pack(packet);
        }
    }

    void unpack( Base::BitStream& packet )
    {
        for (size_t i = 0; i < m_fields.size(); ++i)
        {
            m_fields[i]->unpack(packet);
        }
    }

    void operator = (const dbStruct& other)
    {
        assert(mStructName == other.mStructName);

        mDBName     = other.mDBName;
        mStructName = other.mStructName;
    }

	FIELD_MAP		m_fields;
	std::string		mDBName;
	std::string		mStructName;
};

class CLogWrapBase;

/************************************************************************/
/* 存放所有的结构体信息
/************************************************************************/
class CLogInfoContainer
{
public:
    typedef stdext::hash_map< std::string,CLogWrapBase*> LOG_WRAPS_MAP;

    static CLogInfoContainer* Instance(void)
    {
        static CLogInfoContainer local;
        return &local;
    }
    
    /************************************************************************/
    /* 用来获取和释放name对应的结构体实例
    /************************************************************************/
    dbStruct* Alloc(const char* name);
    void      Release(dbStruct* ptr);

    LOG_WRAPS_MAP& GetWraps(void) {return m_wraps;}

    //internal method
    void    _Register(const char* name,CLogWrapBase* pWrap)
    {
        m_wraps[name] = pWrap;
    }

    void    _UnRegister(const char* name)
    {
        LOG_WRAPS_MAP::iterator iter = m_wraps.find(name);

        if (iter == m_wraps.end())
            return;

        m_wraps.erase(iter);
    }
private:
    LOG_WRAPS_MAP m_wraps;
};

class CLogWrapBase
{
public:
    CLogWrapBase(const char* name):
        m_name(name)
	{
        CLogInfoContainer::Instance()->_Register(name,this);
	}

    virtual ~CLogWrapBase(void)
    {
        CLogInfoContainer::Instance()->_UnRegister(m_name.c_str());
    }

    const char* GetName(void) const {return m_name.c_str();}
    
    /************************************************************************/
    /* 子类通过重载这个方法，提供具体的实现
    /************************************************************************/
    virtual dbStruct* Alloc(void) = 0;
    virtual void      Release(dbStruct* ptr) = 0;
private:
    std::string m_name;
};

inline dbStruct* CLogInfoContainer::Alloc(const char* name)
{
    LOG_WRAPS_MAP::iterator iter = m_wraps.find(name);

    if (iter == m_wraps.end())
        return 0;

    return iter->second->Alloc();
}

inline void      CLogInfoContainer::Release(dbStruct* ptr)
{
    if (0 == ptr)
        return;

    LOG_WRAPS_MAP::iterator iter = m_wraps.find(ptr->mStructName.c_str());

    if (iter == m_wraps.end())
        return;

    iter->second->Release(ptr);
}

#define DECL_DB_TABLE( p, n )  \
	typedef p _Type; \
	p() : dbStruct( #p, n) { } \

#define DECL_DB_FIELD( c, p, t, l ) \
	struct DB_FIELD_##c : public dbField \
	{ \
	public:\
		DB_FIELD_##c()\
		: dbField( #c, t, l ) \
	{ _Type* pParent = (_Type*)((char*)this - CLASS_OFFSET(c,_Type)); pParent->AddField( this );} \
	p _inst;\
	void operator = ( p dest ) { _inst = dest;} \
	operator p(void) {return _inst;} \
	void pack( Base::BitStream& packet )    { _db_field_pack( packet, _inst, mBitCount );}\
	void unpack( Base::BitStream& packet )  { _db_field_unpack( packet, _inst, mBitCount );}\
	std::string toString()                  { std::string str; _tostr( _inst, str ); return str; } \
	void        fromString(std::string str) { _fromstr( _inst, str ); } \
	}; \
	DB_FIELD_##c c;

template<typename _Ty>
class CLogWrap:
    public CLogWrapBase
{
public:
    CLogWrap(const char* name):
      CLogWrapBase(name)
      {
      }

    //method from CLogWrapBase
    virtual dbStruct* Alloc(void)            {return new _Ty;}
    virtual void      Release(dbStruct* ptr) {delete ptr;}
};

#define DB_STRUCT(p) \
	struct p; \
	static CLogWrap<p> dsb_##p( #p); \
	struct p : public dbStruct

#include "Common/log/LogTypes.h"

#ifdef NTJ_LOGSERVER
// database access object
class dbStructDAO
{
public:
	dbStructDAO()
	{
		mConn = NULL;
	}

	bool attachDbPool( CDBConnPool* connPool )
	{
        if (0 == connPool)
            return false;

		mConn = connPool;

        return __InitDBTables();
	}

	static dbStructDAO* getInstance()
	{
		static dbStructDAO instance;
		return &instance;
	}

	void insert( dbStruct* pStruct );

    /************************************************************************/
    /* 从数据库读取一种类型的log列表,一次必须读取所有的列信息
    /************************************************************************/
    template<typename _Ty>
    bool    read(const char* logTableName,const char* sqlQuery,std::vector<_Ty>& logList)
    {
        if (0 == logTableName || 0 == mConn || 0 == sqlQuery)
            return false;
        
        CDBOperator optr(mConn);

        optr->SQL(sqlQuery);
        
        _Ty tmpData;
        
        while(optr->More())
        {
            //读取1条记录
            for(size_t i = 0; i < tmpData.m_fields.size(); ++i)
            {
                dbField* pField = tmpData.m_fields[i];
                pField->fromString(optr->GetString());
            }

            logList.push_back(tmpData);
        }
        
        return true;
    }
private:
    /************************************************************************/
    /* 初始化所有LOG结构体的数据库表
    /************************************************************************/
    bool __InitDBTables(void);
    
    /************************************************************************/
    /* 创建所有数据结构的模板表
    /************************************************************************/
    void __CreateTemplateDB(dbStruct* pStruct);

	CDBConnPool* mConn;
};

#endif

#endif