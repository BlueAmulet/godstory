/*--------------------------------------------------------------------------------------------------------------
/ 功能描述:	创建角色
/---------------------------------------------------------------------------------------------------------------*/
CREATE PROCEDURE  [dbo].[CreatePlayer]
@AccountId  INT,
@PlayerName	VARCHAR(20),
@AreaId INT,
@dbVersion INT
AS

DECLARE @PlayerId INT
DECLARE @Count INT

BEGIN TRANSACTION	
/*检查角色名称*/
SELECT @Count = Count( PlayerId ) FROM PlayerInfo WHERE PlayerName = @PlayerName
IF @Count > 0 
	GOTO ERRORNAME

/*插入角色信息，其他值为默认*/
INSERT INTO PlayerInfo (AccountId,PlayerName,AreaId, dbVersion) VALUES (@AccountId,@PlayerName, @AreaId, @dbVersion)
IF @@ERROR<>0
	GOTO ERRORLABLE

/*查询出生成的角色Id*/
SELECT @PlayerId=PlayerId FROM PlayerInfo WHERE PlayerName=@PlayerName
IF @@ERROR<>0
	GOTO ERRORLABLE

COMMIT TRANSACTION
SELECT 0,@PlayerId
RETURN 0

ERRORNAME:
ROLLBACK TRANSACTION
SELECT -2
RETURN -2

ERRORLABLE:
ROLLBACK TRANSACTION
SELECT -1
RETURN -1
GO