if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[PlayerInfo]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[PlayerInfo]
GO

CREATE TABLE [dbo].[PlayerInfo] (
	[AccountId] [int] NOT NULL ,
	[PlayerId] [int] IDENTITY (1, 1) NOT NULL ,
	[PlayerName] [varchar] (30) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[ActiveDate] [datetime] NOT NULL ,
	[PlayerFunction] [int] NOT NULL ,
	[PlayerOnlineTime] [int] NOT NULL ,
	[DispData] [image] NOT NULL ,
	[MainData] [image] NOT NULL ,
	[Job] [int] NOT NULL ,
	[Sex] [int] NOT NULL ,
	[Race] [int] NOT NULL ,
	[Lev] [int] NOT NULL ,
	[Money] [int] NOT NULL ,
	[ZoneId] [int] NOT NULL ,
	[TriggerNo] [int] NOT NULL ,
	[FactionId] [int] NOT NULL ,
	[FactionJob] [int] NOT NULL ,
	[FactionExp] [int] NOT NULL ,
	[AreaId] [int] NOT NULL ,
	[OldAreaId] [int] NOT NULL ,
	[forceXName] [tinyint] NOT NULL ,
	[lastLogin] [datetime] NULL ,
	[lastLogout] [datetime] NULL ,
	[dbVersion] [int] NOT NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

ALTER TABLE [dbo].[PlayerInfo] ADD 
	CONSTRAINT [DF_PlayerInfo_AccountId] DEFAULT (0) FOR [AccountId],
	CONSTRAINT [DF_PlayerInfo_ActiveDate] DEFAULT (getdate()) FOR [ActiveDate],
	CONSTRAINT [DF_PlayerInfo_PlayerFunction] DEFAULT (0) FOR [PlayerFunction],
	CONSTRAINT [DF_PlayerInfo_PlayerOnlineTime] DEFAULT (0) FOR [PlayerOnlineTime],
	CONSTRAINT [DF_PlayerInfo_DispData] DEFAULT (0x00) FOR [DispData],
	CONSTRAINT [DF_PlayerInfo_MainData] DEFAULT (0x00) FOR [MainData],
	CONSTRAINT [DF_PlayerInfo_Job] DEFAULT (0) FOR [Job],
	CONSTRAINT [DF_PlayerInfo_Sex] DEFAULT (0) FOR [Sex],
	CONSTRAINT [DF_PlayerInfo_Race] DEFAULT (0) FOR [Race],
	CONSTRAINT [DF_PlayerInfo_Lev] DEFAULT (0) FOR [Lev],
	CONSTRAINT [DF_PlayerInfo_Money] DEFAULT (0) FOR [Money],
	CONSTRAINT [DF_PlayerInfo_ZoneId] DEFAULT (0) FOR [ZoneId],
	CONSTRAINT [DF_PlayerInfo_TriggerNo] DEFAULT (0) FOR [TriggerNo],
	CONSTRAINT [DF_PlayerInfo_FactionId] DEFAULT (0) FOR [FactionId],
	CONSTRAINT [DF_PlayerInfo_FactionJob] DEFAULT (0) FOR [FactionJob],
	CONSTRAINT [DF_PlayerInfo_FactionExp] DEFAULT (0) FOR [FactionExp],
	CONSTRAINT [DF_PlayerInfo_OldAreaId] DEFAULT (0) FOR [OldAreaId],
	CONSTRAINT [DF_PlayerInfo_forceXName] DEFAULT (0) FOR [forceXName],
	CONSTRAINT [DF_PlayerInfo_lastLogin] DEFAULT (getdate()) FOR [lastLogin],
	CONSTRAINT [DF_PlayerInfo_lastLogout] DEFAULT (getdate()) FOR [lastLogout],
	CONSTRAINT [DF_PlayerInfo_dbVersion] DEFAULT (0) FOR [dbVersion],
	CONSTRAINT [PK_PlayerInfo] PRIMARY KEY  CLUSTERED 
	(
		[PlayerId]
	)  ON [PRIMARY] 
GO

