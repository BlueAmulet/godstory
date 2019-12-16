#include "dblib/dblib.h"
#include "dbStruct.h"

void _db_field_pack( Base::BitStream& packet, const int& value, int count )
{
	packet.writeInt( value, count );
}

void _db_field_pack( Base::BitStream& packet, const float& value, int count )
{
    packet.writeBits(Base::Bit32,&value);
}

void _db_field_pack( Base::BitStream& packet, const std::string& value, int count )
{
	packet.writeString( value.c_str(), count );
}

void _db_field_unpack( Base::BitStream& packet, int& value, int count )
{
	value = packet.readInt( count );
}

void _db_field_unpack( Base::BitStream& packet, float& value, int count )
{
    packet.readBits(Base::Bit32,&value);
}

void _db_field_unpack( Base::BitStream& packet, std::string& value, int count )
{
	char szBuf[256];
	packet.readString( szBuf ,256);
	value = szBuf;
}

void _db_field_pack( Base::BitStream& packet, const U64& value, int count )
{
    packet.writeBits(count,&value);
}

void _db_field_unpack( Base::BitStream& packet, U64& value, int count )
{
    packet.readBits(count,&value);
}


#include <string>


extern void _tostr( int& value, std::string& str )
{
	static char f[256];
	sprintf_s( f,sizeof(f),"%d", value );
	str = f;
}

extern void _tostr( float& value, std::string& str )
{
    static char f[256];
    sprintf_s( f,sizeof(f),"%f", value );
    str = f;
}

extern void _tostr( std::string& value, std::string& str )
{
    static char f[1024];
    sprintf_s( f,sizeof(f),"'%s'", value.c_str());

	str = f;
}

extern void _fromstr( int& value, std::string& str )
{
	value = atoi( str.c_str() );
}

extern void _fromstr( float& value, std::string& str )
{
    value = atof( str.c_str() );
}

extern void _fromstr( std::string& value, std::string& str )
{
	value = str;
}

void _tostr( U64& value, std::string& str )
{
    static char f[1024];
    _i64toa_s(value,f,1024,10);

    str = f;
}

void _fromstr( U64& value, std::string& str )
{
    value = _atoi64(str.c_str());
}

#ifdef NTJ_LOGSERVER

void dbStructDAO::insert( dbStruct* pStruct )
{
	CDBOperator optr( mConn );

	std::stringstream ssField;
	std::stringstream ssValue;

    std::string dot = "";

    for (size_t i = 0; i < pStruct->m_fields.size(); ++i)
    {
        dbField* pField = pStruct->m_fields[i];
        
        ssField << dot;
        ssValue << dot;
        ssField << "[" << pField->mDBName << "]";
        ssValue << pField->toString();

        dot = ",";
    }

	optr->SQL( "INSERT INTO %s (%s,[time]) VALUES (%s,getdate())", pStruct->mDBName.c_str(), ssField.str().c_str(), ssValue.str().c_str() );
    optr->Exec();
}

/************************************************************************/
/* 遍历所有的数据库结构体，并调用相应的存储过程AlterLogTable
/************************************************************************/
bool dbStructDAO::__InitDBTables(void)
{
    if(0 == mConn)
        return false;

    printf("log table initialzing... \n\r");

    CLogInfoContainer::LOG_WRAPS_MAP& map = CLogInfoContainer::Instance()->GetWraps();

    for(CLogInfoContainer::LOG_WRAPS_MAP::iterator iter = map.begin(); iter != map.end(); ++iter)
    {
        dbStruct* pStruct = iter->second->Alloc();

        if (0 == pStruct)
            continue;

        __CreateTemplateDB(pStruct);

        iter->second->Release(pStruct);
    }

    printf("log table inited! \n\r");
    return true;
}

/************************************************************************/
/* 创建模板表，模板表可以用来同步当前的结构信息到数据库表
/************************************************************************/
void dbStructDAO::__CreateTemplateDB(dbStruct* pStruct)
{
    CDBOperator optr( mConn );

    std::string ssField;

    std::string dot = "";

    if (0 == pStruct->m_fields.size())
        return;

    for(size_t i = 0; i < pStruct->m_fields.size(); ++i)
    {
        dbField* pField = pStruct->m_fields[i];

        ssField += dot;
        ssField += "[";
        ssField += pField->mDBName;
        ssField += "] ";

        switch( pField->mDBType )
        {
        case DB_TYPE_INT:
        case DB_TYPE_DATETIME:
            ssField += "int";
            break;
        case DB_TYPE_FLOAT:
            ssField += "float";
            break;
        case DB_TYPE_BIGINT:
            ssField += "bigint";
            break;
        case DB_TYPE_STRING:
            {
                static char bufFmt[256] = {0};
                sprintf_s(bufFmt,sizeof(bufFmt),"varchar(%d)",pField->mBitCount);
                ssField += bufFmt;
            }
            break;
        }
        
        dot = ",";
    }

    ssField += ",";
    ssField += "[time] datetime";
    
    std::string tableName = pStruct->mDBName + "_template";

    //初始化模板表
    optr->SQL("EXEC CreateLogTemplateTable '%s','(%s)'",tableName.c_str(), ssField.c_str());
    optr->Exec();

    //同步当前的表结构
    optr->SQL("EXEC AlterLogTable %s,%s",tableName.c_str(),pStruct->mDBName.c_str());
    optr->Exec();
}

#endif