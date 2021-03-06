if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[PlayerInfo_Deleted]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[PlayerInfo_Deleted]
GO

CREATE TABLE [dbo].[PlayerInfo_Deleted] (
	[id] [int] IDENTITY (1, 1) NOT NULL ,
	[AccountId] [int] NOT NULL ,
	[PlayerId] [int] NOT NULL ,
	[PlayerName] [varchar] (20) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[ActiveData] [datetime] NOT NULL ,
	[DeleteDate] [datetime] NOT NULL ,
	[PlayerFunction] [int] NOT NULL ,
	[PlayerOnlineTime] [int] NOT NULL ,
	[DispData] [image] NOT NULL ,
	[MainDAta] [image] NOT NULL ,
	[Job] [int] NOT NULL ,
	[Sex] [tinyint] NOT NULL ,
	[Race] [int] NOT NULL ,
	[Lev] [int] NOT NULL ,
	[Money] [int] NOT NULL ,
	[ZoneId] [int] NOT NULL ,
	[TriggerNo] [int] NOT NULL ,
	[FactionId] [int] NOT NULL ,
	[FactionJob] [int] NOT NULL ,
	[FactionExp] [int] NOT NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

ALTER TABLE [dbo].[PlayerInfo_Deleted] ADD 
	CONSTRAINT [DF_PlayerInfo_Deleted_DeleteDate] DEFAULT (getdate()) FOR [DeleteDate],
	CONSTRAINT [PK_PlayerInfo_Deleted] PRIMARY KEY  CLUSTERED 
	(
		[id]
	)  ON [PRIMARY] 
GO

