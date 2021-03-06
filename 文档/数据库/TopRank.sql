if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[TopRank]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[TopRank]
GO

CREATE TABLE [dbo].[TopRank] (
	[id] [int] IDENTITY (1, 1) NOT NULL ,
	[idx] [int] NOT NULL ,
	[idxValue] [int] NOT NULL ,
	[playerId] [int] NOT NULL ,
	[playerName] [nvarchar] (20) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[pValue] [int] NOT NULL ,
	[honour] [int] NOT NULL ,
	[type] [int] NOT NULL 
) ON [PRIMARY]
GO

