#include "TCCWapper.h"
#include <stdio.h>

#pragma warning( disable: 4731 )			// 屏蔽EBP指针被修改警告

CTCCWarper::CTCCWarper()
{
	mTccState = tcc_new();
	tcc_set_output_type( mTccState,	TCC_OUTPUT_MEMORY );
	tcc_set_error_func( mTccState, this, _error );
	tcc_add_include_path( mTccState, "." );
	tcc_add_library_path( mTccState, "." );
}

CTCCWarper::~CTCCWarper()
{
	if( mTccState )
		tcc_delete( mTccState );
}

void CTCCWarper::_error( void* value, const char* msg )
{
	CTCCWarper* pThis = ( CTCCWarper* )value;
	pThis->mLastErrorMessage = msg;
	pThis->mIsReady = false;
}

bool CTCCWarper::check()
{
	if( !mIsReady )
		return false;

	if( tcc_relocate( mTccState ) == 0 )
		mIsReady = true;
	else
		mIsReady = false;

	return mIsReady;
}

void CTCCWarper::registerSymbol(std::string strName, void *pSymbol )
{
	if( !mTccState )
		return ;

	tcc_add_symbol( mTccState, strName.c_str(), (unsigned long)pSymbol );
}

void CTCCWarper::loadScript( const char* szScript )
{
	if( tcc_compile_string( mTccState, szScript ) == 0 )
		mIsReady = true;
	else
		mIsReady = false;

	check();
}

void CTCCWarper::compileFile( const char* fileName )
{
	tcc_delete( mTccState );
	mTccState = tcc_new();

	tcc_set_output_type( mTccState,	TCC_OUTPUT_OBJ );
	tcc_set_error_func( mTccState, this, _error );
	tcc_add_include_path( mTccState, "." );
	tcc_add_library_path( mTccState, "." );

	tcc_add_file( mTccState, fileName );

	std::string fn = fileName;
	fn = fn.substr( 0, fn.length() - 2 );
	fn += ".lib";
	tcc_output_file( mTccState, fn.c_str() );

	tcc_delete( mTccState );
	mTccState = tcc_new();

	tcc_set_output_type( mTccState,	TCC_OUTPUT_MEMORY );
	tcc_set_error_func( mTccState, this, _error );
	tcc_add_include_path( mTccState, "." );
	tcc_add_library_path( mTccState, "." );
}

void CTCCWarper::loadFile( const char* fileName )
{
	if( !mTccState )
		return ;

	if( tcc_add_file( mTccState, fileName ) == 0)
		mIsReady = true ;
	else
		mIsReady = false;

	check();
}

TCCCall CTCCWarper::_getFunction(char *szName)
{
	TCCCall call = 0;
	tcc_get_symbol( mTccState, (unsigned long*)&call, szName );
	return call;
}

int _cdecl CTCCWarper::run( char* szName, int argc, ... )
{
	TCCCall invokeCall;

	if( !szName )
		return NULL;

	if( !mIsReady )
		return 0;

	invokeCall = _getFunction( szName );

	int* pParam = &argc;
	pParam++;
	int nRet = 0;

	unsigned long pESP;
	int nLength = argc * 4;
	if( invokeCall && mIsReady )
	{
		__asm
		{
			mov eax, nLength;
			sub esp, eax;
			mov pESP, esp;
		}

		memcpy( (void*)pESP, pParam, nLength ); // 压栈, 填充参数

		__asm 
		{
			call invokeCall;
			mov nRet, eax;		// 获得返回值
			mov ebx, nLength;
			add esp, ebx;		// 恢复ESP
		}
	}

	return nRet;
}

void CTCCWarper::getSymbol( std::string strName, void* pSymbol )
{
	tcc_get_symbol( mTccState, (unsigned long*)pSymbol, strName.c_str() );
}