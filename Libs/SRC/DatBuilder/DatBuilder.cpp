



//********************
//create by zl 09.2.27
//********************

#include "DatBuilder.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "Shlwapi.h"

using namespace std;

COperateData::COperateData()
{
	setlocale(LC_ALL, ""); 
	m_hWnd = NULL;
	m_ConfigList.clear()   ;
	m_IexFlieList.clear()  ;    //iex列表
	m_IniFlieList.clear()  ;    //int列表
	m_DataFlieList.clear() ;   //Data列表
	//
	ZeroMemory(m_Path, sizeof(m_Path));             //生成文件的路径
	ZeroMemory(m_FileName, sizeof(m_FileName));     //要生成的文件名
	ZeroMemory(m_FileName2, sizeof(m_FileName2));   //
	//
	ReadDataInit();
}
//
COperateData::~COperateData()
{
	ReadDataClose();
}
//
string   COperateData::GetFileTitleName(const char* name)
{
  const	char* pos = (char*)strrchr(name,'.');

	char  tchar[MAX_FILE_PATH];
	ZeroMemory(tchar, sizeof(tchar));
	if(!pos)
		return 0;
	//

	strncpy(tchar, name, strlen(name)-strlen(pos));
	string  result;
	//

	if( strlen(pos) == 0)
		return result;
	//
	if(pos)
	{
		result = tchar;
	}
	return result;

}
//得到文件的扩展名
string  COperateData::GetFileExtenName(const char* name) 
{
	const char* pos = (char*)strrchr(name, '.');
	if(!pos)
		return 0;
	//
	string  result;
	//
	++pos;

	if( strlen(pos) == 0)
		return result;
	//
	if(pos)
	{
		result = pos;
	}
	return result;
}
//遍历所有的文件
void  COperateData::RansackFile(const char* lpPath)   
{
	//
	string  tStr;
	FCheck  tchk;
	char    tchar[max_strLen];
	char    szFind[MAX_FILE_PATH];
	WIN32_FIND_DATA FindFileData;

	ZeroMemory(szFind, sizeof(szFind));
	strcpy(szFind, lpPath);
	strcat(szFind, "\\*.*");

	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	//
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	m_IexFlieList.clear();
	m_IniFlieList.clear();
	m_DataFlieList.clear();

	//当前目录下的所有文件
	while(TRUE)
	{

		//如果不是文件目录
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{

			if((strlen(FindFileData.cFileName) == 0) || (FindFileData.cFileName[0] == '.'))
			{
				if(!FindNextFile(hFind, &FindFileData))     
					break;
				continue;
			}
			//
			tStr = GetFileExtenName(FindFileData.cFileName);
			//iex文件
			if( _stricmp(tStr.c_str(),"iex") ==0 )
			{
				ZeroMemory(tchar,sizeof(tchar));
				sprintf(tchar, "%s\\%s", lpPath, FindFileData.cFileName);
				tchk.path = tchar;
				tchk.title = GetFileTitleName(FindFileData.cFileName);	
				m_IexFlieList.push_back(tchk);
			}
			//ini文件
			if( _stricmp(tStr.c_str(),"ini") ==0 )
			{
				ZeroMemory(tchar,sizeof(tchar));
				sprintf(tchar, "%s\\%s", lpPath, FindFileData.cFileName);
				tchk.path = tchar;
				tchk.title = GetFileTitleName(FindFileData.cFileName);		
				m_IniFlieList.push_back(tchk);
			}
			//data文件
			if( _stricmp(tStr.c_str(),"dat") ==0 )
			{
				ZeroMemory(tchar,sizeof(tchar));
				sprintf(tchar, "%s\\%s", lpPath, FindFileData.cFileName);
				tchk.path = tchar;
				tchk.title = GetFileTitleName(FindFileData.cFileName);		
				m_DataFlieList.push_back(tchk);
			}
		}

		if(!FindNextFile(hFind, &FindFileData))     
			break;
	}
	::FindClose(hFind);

}
//输出Ddata文件
void  COperateData::BuildData(const char* fiex)     
{
	//*****************************
	//读取二进制文件时一定要严格按照数据的大小进行读取
	//读取和写入一定要一一对应
	//******************************
	char tfilepath[MAX_FILE_PATH];
	Cconfig tcfg;
	char tchr[max_strLen];
	char tchr2[256];
	U16  strSize = 0;
	int  tIndex  = 0;
	S8 tempS8 = 0;
	S16 tempS16 = 0;
	S32 tempS32 = 0;
	S8 tempU8 = 0;
	S16 tempU16 = 0;
	S32 tempU32 = 0;
	F32 tempF32 = 0.0;
	F32 tempF64 = 0.0;

	ZeroMemory(tfilepath, sizeof(tfilepath));
	strcpy(tfilepath, m_Path);
	strcat(tfilepath, m_FileName);
	bool benum = false;  //检查enum字段值是否有效

	iMainKeyVt.clear();
	sMainKeyVt.clear();
	//
	FILE* fp = fopen(tfilepath, "w+b");  //生成了data的2进制文件

	if(!fp)
	{
		ShowErrorMessage("生成data文件失败！");
		return;
	}	

	//先把ini放入data文件,结尾用@标记(在没的字串前加上字串的长度
	vector<Cconfig>::iterator pvt  = m_ConfigList.begin();
	vector<elementData>::iterator pvt2 ;
	if( int( m_ConfigList.size() ) == 0 )
	{
		ShowErrorMessage("没有配置文件的数据！");
		return;
	}
	//
	for(;pvt != m_ConfigList.end(); ++ pvt,++tIndex)
	{
		tcfg.Init();
		ZeroMemory(tchr, sizeof(tchr));
		tcfg = *pvt;
		sprintf(tchr, "[%s]\n", tcfg.m_ColName);
		strSize = strlen(tchr);
		//写列名
		fwrite(tchr, sizeof(char), strSize, fp);
		//写数据类型
		switch(tcfg.m_DType)
		{
		case  DType_string: 
			{
				sprintf(tchr, "%s\n","type = string");
			};break;
		case  DType_S8:
			{
				sprintf(tchr, "%s\n","type = S8");
			};break;
		case  DType_S16:
			{
				sprintf(tchr, "%s\n","type = S16");
			};break;
		case  DType_S32:
			{
				sprintf(tchr, "%s\n","type = S32");
			};break;
		case  DType_U8:
			{
				sprintf(tchr, "%s\n","type = U8");
			};break;
		case  DType_U16:
			{
				sprintf(tchr, "%s\n","type = U16");
			};break;
		case  DType_U32:
			{
				sprintf(tchr, "%s\n","type = U32");
			};break;
		case  DType_enum8:
			{
				sprintf(tchr, "%s\n","type = enum8");
			};break;
		case  DType_enum16:
			{
				sprintf(tchr, "%s\n","type = enum16");
			};break;
		case DType_F32:
			{	
				sprintf(tchr, "%s\n","type = F32");
			};break;
		case DType_F64:
			{	
				sprintf(tchr, "%s\n","type = F64");
			};break;
		default:
			break;
		}
		//
		strSize = strlen(tchr);
		fwrite(tchr, sizeof(char), strSize, fp);
		//如果是主键
		if(tIndex == MainKeyIndex)
		{
			sprintf(tchr, "%s\n","mKey = true");
			strSize = strlen(tchr);
			fwrite(tchr, sizeof(char), strSize, fp);
		}
		//写enum项
		if(tcfg.m_DType == DType_enum8 || tcfg.m_DType == DType_enum16)
		{
			ZeroMemory(tchr,sizeof(tchr));
			//
			sprintf(tchr, "body = %d\n", tcfg.elem_num);

			for( pvt2 = tcfg.m_enumVT.begin(); pvt2 != tcfg.m_enumVT.end(); ++ pvt2 )
			{
				ZeroMemory(tchr2, sizeof(tchr2));
				sprintf(tchr2, "%s %d\n", pvt2->elem_name.c_str(), pvt2->elem_value);
				strcat(tchr, tchr2);
			}
			//strcat(tchr,"\n");
			strSize = strlen(tchr);
			fwrite(tchr, sizeof(char), strSize, fp);
		}
	}
	//最后写上标记
	ZeroMemory(tchr, sizeof(tchr));
	sprintf(tchr, "%s", "@\n");
	strSize = strlen(tchr);
	fwrite(tchr, sizeof(char), strSize, fp);


	/*************************
	写列的数据
	格式:

	文件首部:
	1、总记录数(int)
	2、总字段数(int)
	3、各字段类型，每个类型用一个字节(sizeof(U8))表示

	文件内容:
	1、字段长度(U16)（只有字符串需要长度）
	2、字段内容
	*************************/
	int RecordCount = 0;
	int ColumCount  = 0;
	ifstream fstm(fiex);
	char* bpos; 
	char* epos;
	elementData tedata;
	//
	while(!fstm.eof())
	{
		ZeroMemory(tchr, sizeof(tchr));
		fstm.getline(tchr, sizeof(tchr));
		StrTrim(tchr, " ");
		if(tchr[0] == 9 || strlen(tchr) == 0 )
			break;
		++ RecordCount;
	}
	-- RecordCount;
	fstm.close();

	ColumCount = (int) m_ConfigList.size();
	//
	fwrite(&RecordCount, sizeof(U32), 1, fp);
	fwrite(&ColumCount, sizeof(U32), 1, fp);
	for (pvt = m_ConfigList.begin(); pvt != m_ConfigList.end(); ++pvt)
	{
		tcfg = *pvt;
		fwrite((U8 *)&tcfg.m_DType, sizeof(U8), 1, fp);
	}
	//
	ifstream fstm2(fiex);

	fstm2.getline(tchr,sizeof(tchr));  //第一列不用写入
	//
	for(int i = 0 ; i < RecordCount ; ++ i )
	{
		ZeroMemory(tchr,sizeof(tchr));
		fstm2.getline(tchr,sizeof(tchr));  
		bpos = tchr;
		//
		pvt = m_ConfigList.begin();
		//
		tIndex = 0;
		bool bUseDefaultValues = false;
		for(;pvt != m_ConfigList.end(); ++ pvt,++tIndex)
		{
			tcfg = *pvt;
			//得到字段内容
			ZeroMemory(tchr2, sizeof(tchr2));
			epos = strchr(bpos,9);
			//
			if( bpos && epos )
				strncpy(tchr2, bpos, strlen(bpos)-strlen(epos));
			else 
				strcpy(tchr2, bpos);

			switch(tcfg.m_DType)
			{
			case DType_string:
				{
					//checkMainKey
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值不能为空!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(0,tchr2))
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值必须是唯一的!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
					
					if (bUseDefaultValues)
					{
						strSize = 0;
					}
					else
					{						
						strSize = strlen(tchr2);
					}
					//写字段长度(只有字符串类型数据需要写长度)
					fwrite(&strSize, sizeof(U16), 1, fp);
					//写字段内容
					fwrite(tchr2, sizeof(char), strSize, fp);
				}
				break;

			case DType_U8:
				{
					if (bUseDefaultValues)
					{
						tempU8 = 0;
					}
					else
					{
						tempS32 = atoi(tchr2);
						if(tempS32 < 0 || tempS32 > 0xFF)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: <%s>字段的值越界!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						tempU8 = tempS32;
					}				
					fwrite(&tempU8, sizeof(U8), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值不能为空!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempU8, ""))
						{
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值必须是唯一的!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_S8:
				{
					if (bUseDefaultValues)
					{
						tempS8 = 0;
					}
					else
					{
						tempS32 = atoi(tchr2);
						if(tempS32 < -0xFF/2 || tempS32 >= 0xFF/2)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: <%s>字段的值越界!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						tempS8 = tempS32;
					}		
					fwrite(&tempS8, sizeof(S8), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值不能为空!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempS8, ""))
						{
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值必须是唯一的!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_U16:
				{
					if (bUseDefaultValues)
					{
						tempU16 = 0;
					}
					else
					{
						tempS32 = atoi(tchr2);
						if(tempS32 < 0 || tempS32 >= 0xFFFF)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: <%s>字段的值越界!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						tempU16 = tempS32;
					}
					fwrite(&tempU16, sizeof(U16), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值不能为空!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempU16, ""))
						{
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值必须是唯一的!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_S16:
				{
					if (bUseDefaultValues)
					{
						tempS16 = 0;
					}
					else
					{
						tempS32 = atoi(tchr2);
						if(tempS32 < -0xFFFF/2 || tempS32 >= 0xFFFF/2)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: <%s>字段的值越界!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						tempS16 = tempS32;
					}
					fwrite(&tempS16, sizeof(S16), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值不能为空!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempS16, ""))
						{
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值必须是唯一的!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_U32:
				{
					if (bUseDefaultValues)
					{
						tempU32 = 0;
					}
					else
					{
						tempU32 = atoi(tchr2);
					}
					fwrite(&tempU32, sizeof(U32), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值不能为空!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempU32, ""))
						{							
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值必须是唯一的!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_S32:
				{
					if (bUseDefaultValues)
					{
						tempS32 = 0;
					}
					else
					{
						tempS32 = atoi(tchr2);
					}
					fwrite(&tempS32, sizeof(S32), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值不能为空!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempS32, ""))
						{
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s文件:\n%d行: 主键<%s>字段的值必须是唯一的!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_enum8:
				{
					if (bUseDefaultValues)
					{
						ZeroMemory(ErrorChr,sizeof(ErrorChr));
						sprintf(ErrorChr,"%s文件:\n%d行: <%s>字段值为空!", fiex, i+1, tcfg.m_ColName);
						ShowErrorMessage(ErrorChr);
						return;
					}
					tempS32 = 0;
					benum = false;
					for(pvt2 = tcfg.m_enumVT.begin(); pvt2 != tcfg.m_enumVT.end(); ++ pvt2,++tempS32)
					{
						tedata = *pvt2;
						if(_stricmp(tedata.elem_name.c_str(), tchr2) == 0 )
						{
							benum = true;
							break;
						}
					}
					//
					if(!benum)
					{
						ZeroMemory(ErrorChr, sizeof(ErrorChr));
						sprintf(ErrorChr,"%s文件:\n%d行: <%s>字段中在有无效的enum值!", fiex, i+1, tcfg.m_ColName);
						ShowErrorMessage(ErrorChr);
						fclose(fp);
						fstm.close();
						return;
					}
					//
					fwrite(&tcfg.m_enumVT[tempS32].elem_value, sizeof(U8), 1, fp);
				}
				break;

			case DType_enum16:
				{
					if (bUseDefaultValues)
					{
						ZeroMemory(ErrorChr,sizeof(ErrorChr));
						sprintf(ErrorChr,"%s文件:\n%d行: <%s>字段值为空!", fiex, i+1, tcfg.m_ColName);
						ShowErrorMessage(ErrorChr);
						return;
					}
					tempS32 = 0;
					benum = false;
					for(pvt2 = tcfg.m_enumVT.begin(); pvt2 != tcfg.m_enumVT.end(); ++ pvt2,++tempS32)
					{
						tedata = *pvt2;
						if(_stricmp(tedata.elem_name.c_str(), tchr2) == 0 )
						{
							benum = true;
							break;
						}
					}
					//
					if(!benum)
					{
						ZeroMemory(ErrorChr, sizeof(ErrorChr));
						sprintf(ErrorChr,"%s文件:\n%d行: <%s>字段中在有无效的enum值!", fiex, i+1, tcfg.m_ColName);
						ShowErrorMessage(ErrorChr);
						fclose(fp);
						fstm.close();
						return;
					}
					//
					fwrite(&tcfg.m_enumVT[tempS32].elem_value, sizeof(U16), 1, fp);
				}
				break;

			case DType_F32:
				{
					if (bUseDefaultValues)
					{
						tempF32 = 0.0;
					}
					else
					{
						tempF32 = (float)atof(tchr2);
					}
					fwrite(&tempF32, sizeof(F32), 1, fp);
				}
				break;

			case DType_F64:
				{
					if (bUseDefaultValues)
					{
						tempF64 = 0.0;
					}
					else
					{
						tempF64 = (float)atof(tchr2);
					}
					fwrite(&tempF64, sizeof(F64), 1, fp);
				}
				break;
			}

			if (bUseDefaultValues)
				continue;

			bpos += strlen(tchr2);
			if(strlen(bpos) == 0 )
			{
				bUseDefaultValues = true;
			}
			else
				++bpos;

			fflush(fp);
		}
	}

	//
	fclose(fp);
	fstm.close();
} 

//建立EXL和INI
void  COperateData::OutExlINi()                  
{
	char FilePath[MAX_FILE_PATH]; //当前工作路径
	char temp[256];
	char FiexPath[MAX_FILE_PATH], FiniPath[MAX_FILE_PATH];//FileExName[MAX_FILE_PATH],; //文件的扩展名,iex文件的路径,ini文件的路径
	char* pos ;
	bool  biex = false, bini=false;
	vector<FCheck>::iterator pvt,pvt2;
	FCheck tfchk;

	//
	ZeroMemory(FilePath, MAX_FILE_PATH);
	ZeroMemory(temp, MAX_FILE_PATH);
	ZeroMemory(FiexPath, MAX_FILE_PATH);
	ZeroMemory(FiniPath, MAX_FILE_PATH);
	ZeroMemory(m_Path, sizeof(m_Path));
	ZeroMemory(m_FileName, sizeof(m_FileName));
	ZeroMemory(m_FileName2, sizeof(m_FileName2));

	//设置文件名(包括路径)
	sprintf(temp, "%s", __argv[0]);
	pos = strrchr(temp, '\\');
	strncpy(FilePath, temp, strlen(temp)-strlen(pos));
	strcpy(m_Path, FilePath); //设置公用的路径
	//GetCurrentDirectory(fsize,curPath);


	//查找相关文件
	RansackFile(FilePath);
	//

	//判断是否缺少文件
	if( m_DataFlieList.size() == 0 )
	{
		ShowErrorMessage("缺少dat格式文件!");
		return;
	}

	//
	pvt = m_DataFlieList.begin();
	for(U32 i = 0; i < m_DataFlieList.size(); ++ i )
	{
		try
		{
			sprintf(m_FileName, "\\%s_x.ini", pvt->title.c_str());
			sprintf(m_FileName2, "\\%s_x.iex", pvt->title.c_str());
			ParseData(m_DataFlieList[i].path.c_str());
		}
		catch (...)
		{
			ZeroMemory(ErrorChr, sizeof(ErrorChr));
			sprintf(ErrorChr, "解析%s.dat失败！", pvt->title.c_str());
			ShowErrorMessage(ErrorChr);
			return;
		}
		++pvt;
	}
}

//建立Data文件
void  COperateData::OutData()						
{
	char FilePath[256]; //当前工作路径
	char temp[256];
	char FiexPath[256], FiniPath[256];//FileExName[256],; //文件的扩展名,iex文件的路径,ini文件的路径
	char* pos ;
	bool  biex = false, bini=false;
	vector<FCheck>::iterator pvt, pvt2;
	FCheck tfchk;
	//
	ZeroMemory(ErrorChr, sizeof(ErrorChr));
	ZeroMemory(FilePath, 256);
	ZeroMemory(temp, 256);
	ZeroMemory(FiexPath, 256);
	ZeroMemory(FiniPath, 256);
	ZeroMemory(m_Path, sizeof(m_Path));
	ZeroMemory(m_FileName, sizeof(m_FileName));
	ZeroMemory(m_FileName2, sizeof(m_FileName2));

	//设置文件名(包括路径)
	sprintf(temp, "%s", __argv[0]);
	pos = strrchr(temp, '\\');
	strncpy(FilePath, temp, strlen(temp)-strlen(pos));
	strcpy(m_Path, FilePath); //设置公用的路径

	//查找相关文件
	RansackFile(FilePath);

	//判断是否缺少文件
	if((m_IexFlieList.size() == 0) || (m_IniFlieList.size() == 0 )||  (m_IniFlieList.size() != m_IexFlieList.size()) )
	{
		ShowErrorMessage("没有找到相关文件");
		return;
	}

	pvt = m_IexFlieList.begin();
	//
	for(;pvt != m_IexFlieList.end(); ++ pvt)
	{
		pvt2 = m_IniFlieList.begin();
		biex = FALSE;
		for(;pvt2 != m_IniFlieList.end(); ++ pvt2)
		{

			//
			tfchk = (*pvt2); 
			if(_stricmp(tfchk.title.c_str(), pvt->title.c_str()) ==0)
			{
				biex = TRUE;
				break;
			}
		}
		//
		if(!biex)
		{
			sprintf(ErrorChr, "缺少%s.iex文件", pvt->title.c_str());
			ShowErrorMessage(ErrorChr);
			return;
		}
	}
	//
	pvt = m_IniFlieList.begin();
	for(;pvt != m_IniFlieList.end(); ++ pvt)
	{
		pvt2 = m_IexFlieList.begin();
		biex = FALSE;
		for(;pvt2 != m_IexFlieList.end(); ++ pvt2)
		{
			//
			tfchk = (*pvt2); 
			if(_stricmp(tfchk.title.c_str(), pvt->title.c_str())== 0)
			{
				biex = TRUE;
				break;
			}
		}
		//
		if(!biex)
		{
			sprintf(ErrorChr, "缺少%s.ini文件", pvt->title.c_str());
			ShowErrorMessage(ErrorChr);
			return;
		}
	}
	//

	//建立必要的解析信息
	for(size_t i = 0 ; i < m_IexFlieList.size() ; ++ i )
	{
		try
		{
			sprintf(m_FileName, "\\%s.dat", m_IniFlieList[i].title.c_str());
			BulidConfigVector(m_IniFlieList[i].path.c_str());
			BuildData(m_IexFlieList[i].path.c_str());
		}
		catch (...)
		{
			//Cstr.Format("生成%s.dat文件失败！",m_IniFlieList[i].title);
			sprintf(ErrorChr, "生成%s.dat文件失败！", m_IniFlieList[i].title.c_str());
			ShowErrorMessage(ErrorChr);
			return;
		}
	}
}
//建立必要的解析信息
void  COperateData::BulidConfigVector(const char* fini) 
{
	ifstream fstm(fini);
	char temp[max_strLen];
	char tstr[256];
	int  e_val = 0, e_num = 0;

	string  tstring;
	Cconfig  tconfig;
	//
	elementData tedata; 
	//
	m_ConfigList.clear();
	MainKeyType  = DType_none;
	MainKeyIndex = -1;
	//
	while(!fstm.eof())
	{
		ZeroMemory(temp, sizeof(temp));
		tconfig.Init();
		//
		fstm.getline(temp, max_strLen);

		if(strcmp(temp,"@") == 0 || strlen(temp) == 0 )  //如果是从data那读,这个是结束标记
			break;
		//
		char* mpos = strstr(temp,"mKey");
		//
		if(mpos)
		{
			mpos = NULL;
			if(MainKeyIndex >= 0)
			{
				ZeroMemory(ErrorChr, sizeof(ErrorChr));
				sprintf(ErrorChr, "%s文件中只能有一个主键", fini);
				ShowErrorMessage(ErrorChr);
				throw;
				return;
			}
			else
			{
				MainKeyIndex = m_ConfigList.size() - 1 ;  
				if( MainKeyIndex >= 0)
					MainKeyType = m_ConfigList[MainKeyIndex].m_DType;   //得到主键的类型
				continue;
			}

		}

		//进行解析
		char* bpos = strchr(temp, '[');
		char* epos = strchr(temp, ']');

		if(bpos && epos) //列名
		{
			++ bpos;
			strncpy(tconfig.m_ColName, bpos, strlen(bpos)-strlen(epos));
		} 

		//读列的数据类型
		fstm.getline(temp, max_strLen);

		//
		bpos = strstr(temp, "type");    //类型


		if(bpos)
		{
			bpos = strchr(temp, '=');
			++ bpos;
			StrTrim(bpos, " ");

			ZeroMemory(tstr, sizeof(tstr));
			strcpy(tstr, bpos);
			if(_stricmp(tstr, "string")==0)
			{
				tconfig.m_DType = DType_string;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"S8")==0)
			{
				tconfig.m_DType = DType_S8;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"S16")==0)
			{
				tconfig.m_DType = DType_S16;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"S32")==0)
			{
				tconfig.m_DType = DType_S32;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"U8") == 0 )
			{
				tconfig.m_DType = DType_U8;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"U16") == 0 )
			{
				tconfig.m_DType = DType_U16;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"U32") == 0 )
			{
				tconfig.m_DType = DType_U32;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"F32") == 0 )
			{
				tconfig.m_DType = DType_F32;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"F64") == 0 )
			{
				tconfig.m_DType = DType_F64;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if (_stricmp(tstr,"enum8") == 0)
				tconfig.m_DType = DType_enum8;
			else if (_stricmp(tstr,"enum16") == 0)
				tconfig.m_DType = DType_enum16;
		}

		//如果是enum类型的话,就还有enum项
		ZeroMemory(temp, sizeof(temp));
		fstm.getline(temp, max_strLen);
		epos = strstr(temp, "body");
		//
		if(epos)   
		{
			// 得到枚举类型元素的数量
			epos = strchr(temp, '=');
			++ epos;            
			StrTrim(epos, " ");
			e_num = atoi(epos);
			//
			tconfig.elem_num = e_num;
			for(int i = 0 ; i< e_num ; ++ i )
			{
				ZeroMemory(temp, sizeof(temp));
				fstm.getline(temp, max_strLen); 
				bpos = temp;
				epos = strchr(temp,' ');
				//字符串值
				ZeroMemory(tstr, sizeof(tstr));
				strncpy(tstr, temp, strlen(bpos)-strlen(epos));
				tedata.elem_name  = tstr;
				//逻辑值
				e_val = atoi(epos);
				tedata.elem_value = e_val; 
				tconfig.m_enumVT.push_back(tedata);
			}
			m_ConfigList.push_back(tconfig);
		}
		//

	}
	//
	fstm.close();
}
//
void  COperateData::ParseData(const char*  filename)
{
	BulidConfigVector(filename);

	vector<Cconfig>::iterator pvt  = m_ConfigList.begin();
	vector<elementData>::iterator  pvt2 ;
	char tfilepath[256];
	Cconfig tcfg;
	char tchr[max_strLen];
	char tchr2[256];
	int  strSize = 0;
	int  tIndex  = 0;

	//先生成配置文件
	ZeroMemory(tfilepath, sizeof(tfilepath));
	strcpy(tfilepath, m_Path);
	strcat(tfilepath, m_FileName);

	FILE* fp = fopen(tfilepath, "w+t"); 
	if(!fp)
	{
		ShowErrorMessage("打开文件失败！");
		return;
	}


	if( int( m_ConfigList.size() ) == 0 )
	{
		ShowErrorMessage("没有配置文件的数据！");
		return;
	}

	//
	for(;pvt != m_ConfigList.end(); ++ pvt,++ tIndex)
	{
		tcfg.Init();
		ZeroMemory(tchr, sizeof(tchr));
		tcfg = *pvt;
		sprintf(tchr,"[%s]\n", tcfg.m_ColName);
		strSize = strlen(tchr);
		//写列名
		fwrite(tchr, sizeof(char), strSize, fp);
		//写数据类型
		switch(tcfg.m_DType)
		{
		case  DType_string: 
			{
				sprintf(tchr,"%s\n","type = string");
			};break;
		case  DType_S8:
			{
				sprintf(tchr,"%s\n","type = S8");
			};break;
		case  DType_S16:
			{
				sprintf(tchr,"%s\n","type = S16");
			};break;
		case  DType_S32:
			{
				sprintf(tchr,"%s\n","type = S32");
			};break;
		case  DType_U8:
			{
				sprintf(tchr,"%s\n","type = U8");
			};break;
		case  DType_U16:
			{
				sprintf(tchr,"%s\n","type = U16");
			};break;
		case  DType_U32:
			{
				sprintf(tchr,"%s\n","type = U32");
			};break;
		case  DType_enum8:
			{
				sprintf(tchr,"%s\n","type = enum8");
			};break;
		case  DType_enum16:
			{
				sprintf(tchr,"%s\n","type = enum16");
			};break;
		case DType_F32:
			{
				sprintf(tchr,"%s\n","type = F32");
			};break;
		case DType_F64:
			{
				sprintf(tchr,"%s\n","type = F64");
			};break;
		default:
			break;
		}
		//
		strSize = strlen(tchr);
		fwrite(tchr, sizeof(char), strSize, fp);
		//如果是主键
		if(tIndex == MainKeyIndex)
		{
			sprintf(tchr, "%s\n", "mKey = true");
			strSize = strlen(tchr);
			fwrite(tchr, sizeof(char), strSize, fp);
		}

		//写enum项
		if(tcfg.m_DType == DType_enum8 || tcfg.m_DType == DType_enum16)
		{
			ZeroMemory(tchr, sizeof(tchr));
			//
			sprintf(tchr, "body = %d\n", tcfg.elem_num);

			for( pvt2 = tcfg.m_enumVT.begin(); pvt2 != tcfg.m_enumVT.end(); ++ pvt2 )
			{
				ZeroMemory(tchr2, sizeof(tchr2));
				sprintf(tchr2, "%s %d\n", pvt2->elem_name.c_str(), pvt2->elem_value);
				strcat(tchr, tchr2);
			}
			//strcat(tchr,"\n");
			strSize = strlen(tchr);
			fwrite(tchr, sizeof(char), strSize, fp);
		}
	}
	//
	fclose(fp);

	//再生成iex文件
	ZeroMemory(tfilepath, sizeof(tfilepath));
	strcpy(tfilepath, m_Path);
	strcat(tfilepath, m_FileName2);

	fp = fopen(tfilepath, "w+t");
	//先定位数据
	FILE* fp2 = fopen(filename, "rb");

	if(!fp || !fp2)
	{
		ShowErrorMessage("打开文件失败！");
		return;
	}

	int RecordCount = 0;
	int ColumCount  = 0;
	int temp        = 0;
	S8 fieldS8      = 0;
	S16 fieldS16    = 0;
	S32 fieldS32    = 0;
	U8 fieldU8      = 0;
	U16 fieldU16    = 0;
	U32 fieldU32    = 0;
	F32 fieldF32	= 0.0f;
	F64 fieldF64	= 0.0f;

	if(fp2)
	{
		while(1)
		{
			ZeroMemory(tchr, sizeof(tchr));
			fread(tchr, sizeof(char), 1, fp2);
			if(strcmp(tchr, "@")==0)  //找到数据文件的开头
			{
				fread(tchr, sizeof(char), 1, fp2);  //这个是换行字符
				break;
			}
		}
		fread(&RecordCount, sizeof(int), 1, fp2);
		fread(&ColumCount, sizeof(int), 1, fp2);
	}

	fseek(fp2, ColumCount, SEEK_CUR);

	//写列头
	pvt = m_ConfigList.begin();
	ZeroMemory(tchr, sizeof(tchr));
	//
	temp = 1;
	for(;pvt != m_ConfigList.end(); ++ pvt)
	{
		tcfg = *pvt;
		strcat(tchr, tcfg.m_ColName);
		if(temp < ColumCount)
			tchr[strlen(tchr)] = 9;
		++ temp;
	}
	strcat(tchr, "\n");
	fwrite(tchr, sizeof(char), strlen(tchr), fp);
	//写具体内容
	/*************************
	写列的数据
	格式:
	头文件:
	1、总记录数(int)
	2、总字段数(int)
	

	字段格式:
	1、字段长度(int)
	2、字读数据类型(int->2,string->1,enum->3)
	3、字段内容(int,string)
	*************************/
	for(int i =0 ; i < RecordCount; ++ i )
	{
		pvt = m_ConfigList.begin();
		ZeroMemory(tchr,sizeof(tchr));
		temp = 1;
		for(;pvt != m_ConfigList.end(); ++pvt, ++temp)
		{
			ZeroMemory(tchr2, sizeof(tchr2));
			tcfg =  (*pvt);

			switch (tcfg.m_DType)
			{
			case DType_string: 
				{
					fread(&strSize, sizeof(U16), 1, fp2);
					fread(tchr2, sizeof(char), strSize, fp2);
				};break;
			case DType_S8:
				{
					fread(&fieldS8, sizeof(S8), 1, fp2);
					sprintf(tchr2, "%d", fieldS8);
				};break;
			case DType_S16:
				{
					fread(&fieldS16, sizeof(S16), 1, fp2);
					sprintf(tchr2, "%d", fieldS16);
				};break;
			case DType_S32:
				{
					fread(&fieldS32, sizeof(S32), 1, fp2);
					sprintf(tchr2, "%d", fieldS32);
				};break;
			case DType_U8:
				{
					fread(&fieldU8, sizeof(U8), 1, fp2);
					sprintf(tchr2, "%u", fieldU8);
				};break;
			case DType_U16:
				{
					fread(&fieldU16, sizeof(U16), 1, fp2);
					sprintf(tchr2, "%u", fieldU16);
				};break;
			case DType_U32:
				{
					fread(&fieldU32, sizeof(U32), 1, fp2);
					sprintf(tchr2, "%u", fieldU32);
				};break;
			case DType_enum8:
				{
					fread(&fieldU8, sizeof(U8), 1, fp2);
					pvt2 = tcfg.m_enumVT.begin();
	
					for(; pvt2 != tcfg.m_enumVT.end(); ++ pvt2)
					{
						
						if(fieldU8 == pvt2->elem_value)
						{
							sprintf(tchr2,"%s",pvt2->elem_name.c_str());
							break;
						}
					}
				};break;
			case DType_enum16:
				{
					fread(&fieldU16, sizeof(U16), 1, fp2);
					pvt2 = tcfg.m_enumVT.begin();

					for(; pvt2 != tcfg.m_enumVT.end(); ++ pvt2)
					{
						if(fieldU16 == pvt2->elem_value)
						{
							sprintf(tchr2, "%s", pvt2->elem_name.c_str());
							break;
						}
					}
				};break;
			case DType_F32:
				{
					fread(&fieldF32, sizeof(F32), 1, fp2);
					sprintf(tchr2, "%f", fieldF32);
				};break;
			case DType_F64:
				{
					fread(&fieldF64, sizeof(F64), 1, fp2);
					sprintf(tchr2, "%lf", fieldF64);
				};break;
			}
			strcat(tchr, tchr2);
			if(temp < ColumCount)
				tchr[strlen(tchr)] = 9;	
		}
		strcat(tchr, "\n");
		fwrite(tchr, sizeof(char), strlen(tchr), fp);
	}
	//
	fclose(fp);
	fclose(fp2);
}
//**************************************************************
//读取data中
//的数据
//**************************************************************
void  COperateData::ReadDataInit()
{
	RecordNum = 0 ;
	ColumNum  = 0 ;
	readstep  = 0 ;
	pfdata = NULL;
}

void  COperateData::ReadDataClose()
{

	if(pfdata)
	{
		fclose(pfdata);
		pfdata = NULL;
	}
}

//读取data文件
bool  COperateData::readDataFile(const char* name)
{
	//先定位数据
	pfdata = fopen(name, "rb");
	if(!pfdata)
		return FALSE;

	char tchr[max_strLen];

	if(pfdata)
	{
		while(1)
		{
			ZeroMemory(tchr, sizeof(tchr));
			fread(tchr, sizeof(char), 1, pfdata);
			if(strcmp(tchr, "@")==0)  //找到数据文件的开头
			{
				fread(tchr, sizeof(char), 1, pfdata);  //这个是换行字符
				break;
			}
		}

		fread(&RecordNum, sizeof(int), 1, pfdata);   //得到记录总数
		fread(&ColumNum, sizeof(int), 1, pfdata);    //得到列的总数
	}
	//
	readstep = RecordNum * ColumNum;  
	return TRUE;
}

//得到具体的内容
bool  COperateData::GetData(RData& readsct)
{
	/****************************
	格式:
	头文件:
	1、总记录数(int)
	2、总字段数(int)
	字段格式:
	1、字段长度(int)
	2、字读数据类型(int->2,string->1,enum->3,float->4)
	3、字段内容(int,string)
	*************************/
	if( (readstep == 0) || (!pfdata))
		return FALSE;

	int iSize = 0 ; //字符串类型数据的长度

	ZeroMemory(&readsct, sizeof(readsct));
	fread(&readsct.m_Type, sizeof(U8), 1, pfdata);  //字读数据类型
	//
	switch (readsct.m_Type)
	{
	case DType_string: 
		{
			fread(&iSize, sizeof(U16), 1, pfdata); 
			fread(readsct.m_string, sizeof(char), iSize, pfdata);
		};break;

	case DType_S8:
	case DType_U8:
	case DType_enum8:
		{
			fread(&readsct.m_S8, sizeof(S8), 1, pfdata);
		};break;

	case DType_S16:
	case DType_U16:
	case DType_enum16:
		{
			fread(&readsct.m_S16, sizeof(S16), 1, pfdata);
		};break;

	case DType_S32:
	case DType_U32:
		{
			fread(&readsct.m_S32, sizeof(S32), 1, pfdata);
		};break;

	case DType_F32:
		{
			fread(&readsct.m_F32, sizeof(F32), 1, pfdata);			
		};break;

	case DType_F64:
		{
			fread(&readsct.m_F64, sizeof(F64), 1, pfdata);			
		};break;
	}	
	//	
	readstep --;
	//
	return TRUE;
}
//
void  COperateData::ShowErrorMessage(const char* chr)  //显示错误信息
{
	if(m_hWnd)
	{
		MessageBox(m_hWnd, chr, "DatBuilder错误信息", MB_OK);
	}
}

//检查主键的唯一性
bool  COperateData::CheckmKeyExclusive(S32 itemp, string stemp)           
{	
	//
	if(MainKeyIndex == -1 || MainKeyType == DType_none) //没有主键的文件
		return false;

	vector<int>::iterator pint;
	vector<string>::iterator pstr;
	bool result = false;

	switch(MainKeyType)
	{
	case DType_S8:
	case DType_S16:
	case DType_U32:
	case DType_U8:
	case DType_U16:
	case DType_S32:
		{
			pint = iMainKeyVt.begin();
			for(; pint != iMainKeyVt.end(); ++pint)
			{
				if((*pint) == itemp)
				{
					result = true;
					break;
				}
			}
			if(!result)
				iMainKeyVt.push_back(itemp);
		}
		break;

	case DType_string:
		{
			//pstr = find(sMainKeyVt.end(),sMainKeyVt.end(),itemp);
			pstr = sMainKeyVt.begin();
			for(; pstr != sMainKeyVt.end(); ++ pstr)
			{
				if(_stricmp(pstr->c_str(), stemp.c_str()) == 0)
				{
					result = true;
					break;
				}
			}
			if(!result)
				sMainKeyVt.push_back(stemp);
		}
		break;
	default:
		break;
	}

	return result;
}
