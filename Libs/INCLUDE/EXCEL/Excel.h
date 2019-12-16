#pragma once

#include <comutil.h>
#include <string>

class CExcelImpl;

class CExcel
{
private:
    //disable copying
    CExcel(const CExcel& other);
    const CExcel& operator = (const CExcel& ohter);

    CExcelImpl* m_impl; //p-impl implemention
public:
	CExcel(void);
	//static CExcelApp& _GetInstance();
	~CExcel(void);

//公共接口函数
public:
	//////////////////////////////////////////////////////////////////////////
	//remark:		打开一张EXCEL表格
	//params:		1
	//par1:			打开文件路径
	//ret:			打开成功:true 打开失败:false
	//////////////////////////////////////////////////////////////////////////
	bool Open(const char* filePath);

	//////////////////////////////////////////////////////////////////////////
	//remark:		关闭当前打开的EXCEL表格
	//params:		0
	//ret:			void
	//////////////////////////////////////////////////////////////////////////
	void Close();

	//////////////////////////////////////////////////////////////////////////
	//remark:		保存当前打开的EXCEL文件
	//params:		1
	//par1:			保存文件路径
	//ret:			void
	//////////////////////////////////////////////////////////////////////////
	void SaveAs(const char* filePath);

	//////////////////////////////////////////////////////////////////////////
	//remark:		清空当前打开EXCEL文件的内容
	//params:		0
	//ret:			void
	//////////////////////////////////////////////////////////////////////////
	void Reset();

	//////////////////////////////////////////////////////////////////////////
	//remark:		EXCEL是否被初始化
	//params:		0
	//ret:			已经初始化:true，未初始化:false
	//////////////////////////////////////////////////////////////////////////
	bool IsInitialized();
	
	//////////////////////////////////////////////////////////////////////////
	//remark:		获得EXCEL应用名
	//params:		0
	//ret:			EXCEL应用名
	//////////////////////////////////////////////////////////////////////////
    std::string GetAppName();

	//////////////////////////////////////////////////////////////////////////
	//remark:		设置EXCEL文件标题
	//params:		0
	//ret:			void
	//////////////////////////////////////////////////////////////////////////
	void SetCaption(const char* capName);

	//////////////////////////////////////////////////////////////////////////
	//remark:		获得EXCEL文件标题
	//params:		0
	//ret:			EXCEL文件标题
	//////////////////////////////////////////////////////////////////////////
	std::string GetCaption();

	//////////////////////////////////////////////////////////////////////////
	//remark:		设置指定sheet单元格内容
	//params:		4
	//par1:			sheet编号，可以是sheet名字字符串，也可以是sheet index，从1开始
	//par2:			单元格行index，从1开始
	//par3:			单元格列index，从1开始
	//par4:			被设置的值
	//ret:			成功:true , 失败:false
	//////////////////////////////////////////////////////////////////////////
	bool SetItem(const _variant_t& sheetIndex, const _variant_t& rowIndex, const _variant_t& colIndex, const _variant_t& val);

	//////////////////////////////////////////////////////////////////////////
	//remark:		设置当前sheet单元格内容
	//params:		3
	//par1:			单元格行index，从1开始
	//par2:			单元格列index，从1开始
	//par3:			被设置的值
	//ret:			void
	//////////////////////////////////////////////////////////////////////////
	void SetItem(const _variant_t& rowIndex, const _variant_t& colIndex, const _variant_t& val);

	//////////////////////////////////////////////////////////////////////////
	//remark:		获得当前sheet单元格内容
	//params:		2
	//par1:			单元格行index，从1开始
	//par2:			单元格列index，从1开始
	//ret:			单元格值
	//////////////////////////////////////////////////////////////////////////
	_variant_t GetItem(const _variant_t& rowIndex, const _variant_t& colIndex);

	//////////////////////////////////////////////////////////////////////////
	//remark:		设置指定sheet单元格内容
	//params:		3
	//par1:			sheet编号，可以是sheet名字字符串，也可以是sheet index，从1开始
	//par2:			单元格行index，从1开始
	//par3:			单元格列index，从1开始
	//ret:			单元格值
	//////////////////////////////////////////////////////////////////////////
	_variant_t GetItem(const _variant_t& sheetIndex, const _variant_t& rowIndex, const _variant_t& colIndex);

	//////////////////////////////////////////////////////////////////////////
	//remark:		获得当前sheet的名字
	//params:		0
	//ret:			当前sheet的名字
	//////////////////////////////////////////////////////////////////////////
	std::string GetCurrentSheetName();

	//////////////////////////////////////////////////////////////////////////
	//remark:		设置当前sheet的名字
	//params:		1
	//par1:			sheet名字
	//ret:			void
	//////////////////////////////////////////////////////////////////////////
	void SetCurrentSheetName(const char* sheetName);
	
	//////////////////////////////////////////////////////////////////////////
	//remark:		指定sheet为当前sheet
	//params:		1
	//par1:			指定sheet的编号,可以是sheet名字字符串，也可以是sheet index，从1开始
	//ret:			成功:true , 失败:false
	//////////////////////////////////////////////////////////////////////////
	bool ActiveSheet(const _variant_t& sheet);

	//////////////////////////////////////////////////////////////////////////
	//remark:		获得sheet数量
	//params:		0
	//ret:			sheet数量
	//////////////////////////////////////////////////////////////////////////
	long GetSheetsCount();

	//////////////////////////////////////////////////////////////////////////
	//remark:		获得当前sheet索引编号
	//params:		0
	//ret:			sheet索引编号
	//////////////////////////////////////////////////////////////////////////
	long GetCurrentSheetIndex();

	//////////////////////////////////////////////////////////////////////////
	//remark:		插入insertedSheetName到sheet前
	//params:		2
	//par1:			插入到此sheet的前面
	//par2:			插入sheet的名字
	//ret:			成功:true , 失败:false
	//////////////////////////////////////////////////////////////////////////
	bool InsertSheetBefore(const _variant_t& sheet, const char* insertedSheetName);

	//////////////////////////////////////////////////////////////////////////
	//remark:		插入insertedSheetName到sheet后
	//params:		2
	//par1:			插入到此sheet的后面
	//par2:			插入sheet的名字
	//ret:			成功:true , 失败:false
	//////////////////////////////////////////////////////////////////////////
	bool InsertSheetAfter(const _variant_t& sheet, const char* insertedSheetName);

	//////////////////////////////////////////////////////////////////////////
	//remark:		向sheet列表push一张sheet
	//params:		1
	//par1:			插入sheet的名字
	//ret:			void
	//////////////////////////////////////////////////////////////////////////
	void PushSheet(const char* pushedSheetName);

	//////////////////////////////////////////////////////////////////////////
	//remark:		删除一张sheet
	//params:		1
	//par1:			被删除sheet,可以是sheet名字字符串，也可以是sheet index，从1开始
	//ret:			成功:true , 失败:false
	//////////////////////////////////////////////////////////////////////////
	bool DeleteSheet(const _variant_t& deletedSheet);

	//////////////////////////////////////////////////////////////////////////
	//remark:		查找一张sheet
	//params:		1
	//par1:			被查找的sheet,可以是sheet名字字符串，也可以是sheet index，从1开始
	//ret:			sheet下标索引
	//////////////////////////////////////////////////////////////////////////
	int  FindSheet(const _variant_t& sheet);

	//////////////////////////////////////////////////////////////////////////
	//remark:		获得当前工作簿名字
	//params:		0
	//ret:			工作簿名
	//////////////////////////////////////////////////////////////////////////
	std::string GetCurrentWorkBookName();

	//////////////////////////////////////////////////////////////////////////
	//remark:		获得当前工作簿数量
	//params:		0
	//ret:			工作簿数量
	//////////////////////////////////////////////////////////////////////////
	long GetWorkBooksCount();

	std::string GetCurrentRangeName();
	long GetCurrentRangeCount();
};

//#define EXCEL CExcelApp::_GetInstance()