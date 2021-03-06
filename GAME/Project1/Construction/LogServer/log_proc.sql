if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[AlterLogTable]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[AlterLogTable]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[CreateLogTemplateTable]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[CreateLogTemplateTable]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

--by butterfly
--简单的LOG数据表之间的格式拷贝,源表是一个模板结构
CREATE PROCEDURE AlterLogTable
	@sourceTableName         sysname,        --源表名
 	@targetTableName           sysname         --目标表名,默认与源表名相同
AS
	DECLARE @columnName	varchar(100)      --列名
	DECLARE @columnType	varchar(100)      --列类型
	DECLARE @cmd_temp               nvarchar(3000)   --存放临时执行的语句
	DECLARE @xtype		int
	DECLARE @typelen		varchar(100)
	DECLARE @i	 		smallInt
	DECLARE @mdfType		varchar(100)
SET NOCOUNT ON

IF (@targetTableName = @sourceTableName) 
	return;

--判断源表是否存在
IF (NOT EXISTS (SELECT * from sysobjects  WHERE id = object_id(@sourceTableName) AND OBJECTPROPERTY(id, N'IsUserTable') = 1))
	return;

--假如目标表不存在,则重命名当前表为目标表
IF (NOT EXISTS (SELECT * from sysobjects  WHERE id = object_id(@targetTableName) AND OBJECTPROPERTY(id, N'IsUserTable') = 1))
BEGIN
	EXECUTE sp_rename @sourceTableName,@targetTableName,'OBJECT'
	return
END

--获取源表的所有列
DECLARE       @src_columns table(c_no int identity,c_name varchar(100),c_xtype int,c_length int)
INSERT INTO @src_columns SELECT name,xtype,length FROM dbo.syscolumns WHERE id = object_id(@sourceTableName)

--目标表的所有列信息
DECLARE       @dst_columns table(c_no int identity,c_name varchar(100),c_xtype int,c_length int)
INSERT INTO @dst_columns SELECT name,xtype,length FROM dbo.syscolumns WHERE id = object_id(@targetTableName)

--删除目标表中多余的列
SET @i = 1
WHILE (@i <= (SELECT COUNT(c_no) FROM @dst_columns))
BEGIN
	SELECT @columnName = c_name FROM @dst_columns WHERE c_no = @i

	IF (NOT EXISTS(SELECT * from @src_columns WHERE c_name =  @columnName))
	BEGIN
		SET @cmd_temp = 'ALTER TABLE [' + @targetTableName + ']  DROP COLUMN [' + @columnName + '] '
		EXEC sp_executesql @cmd_temp
	END

	SET @i = @i + 1
END

--添加或修改目标表中的列
SET @i = 1
WHILE (@i <= (SELECT COUNT(c_no) FROM @src_columns))
BEGIN
	SELECT @columnName = c_name,@xtype = c_xtype,@typelen = c_length  FROM @src_columns WHERE c_no = @i
	
	--获取源表的列类型
	SELECT @columnType = name FROM dbo.systypes WHERE xtype = @xtype

	--检测目标表中是否存在该列
	IF (NOT EXISTS(SELECT * from @dst_columns WHERE c_name =  @columnName))
		SET @mdfType = ' ADD '
	ELSE
		SET @mdfType = ' ALTER COLUMN '

	IF (@columnType = 'varchar' OR @columnType = 'nvarchar' OR @columnType = 'sysname')
	BEGIN
		---组成varchar(?)
		SET @columnType =  'varchar'
		SET @columnType = @columnType + '(' + @typelen + ')'
	END
		
	SET @cmd_temp = 'ALTER TABLE [' + @targetTableName + '] ' + @mdfType+ ' [' + @columnName + '] '
	SET @cmd_temp =  @cmd_temp + @columnType + ' NULL'
		
	EXEC sp_executesql @cmd_temp

	SET @i = @i + 1
END

SET @cmd_temp = 'DROP TABLE [' + @sourceTableName + '] '
EXEC sp_executesql @cmd_temp
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

--by butterfly
--初始化用于结构体拷贝的模板表
CREATE PROCEDURE CreateLogTemplateTable
	@tableName         sysname,        --表名
 	@tableFiled           sysname         --字段
AS
	DECLARE @cmd_temp               nvarchar(3000)   --存放临时执行的语句
	
set nocount on

--如果表存在则删除
IF (EXISTS (SELECT * from sysobjects  WHERE id = object_id(@tableName) AND OBJECTPROPERTY(id, N'IsUserTable') = 1))
BEGIN
	SET @cmd_temp = 'DROP TABLE [' + @tableName + '] '
	EXEC sp_executesql @cmd_temp
END

--创建模板表
SET @cmd_temp = 'CREATE TABLE [' + @tableName + '] '
SET @cmd_temp = @cmd_temp + @tableFiled;

EXEC sp_executesql @cmd_temp
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

