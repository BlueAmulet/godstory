CREATE PROCEDURE  AddPoint
@Num     	INT,
@AccountId      INT			     	
AS
DECLARE @FreePoint INT,
@ac INT

BEGIN TRANSACTION

SELECT  @ac = Count( AccountId ) FROM BaseInfo WHERE AccountId = @AccountId
IF @ac = 0 
	GOTO ERROR_NOT_ENOUGH

UPDATE BaseInfo SET PointNum = PointNum + @Num WHERE AccountId = @AccountId
IF @@ERROR<>0 
	GOTO ERROR_NOT_ENOUGH

SELECT @FreePoint = PointNum  FROM BaseInfo WHERE AccountId = @AccountId

IF @@ERROR<>0 
	GOTO ERROR_NOT_ENOUGH

IF @FreePoint < 0
	GOTO ERROR_NOT_ENOUGH
	
COMMIT TRANSACTION
SELECT 0, @FreePoint
RETURN 0	--处理成功

ERROR_NOT_ENOUGH:
ROLLBACK TRANSACTION
SELECT -1
RETURN 1
GO
