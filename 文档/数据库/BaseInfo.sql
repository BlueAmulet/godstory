if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[BaseInfo]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[BaseInfo]
GO

CREATE TABLE [dbo].[BaseInfo] (
	[AccountId] [int] NOT NULL ,
	[AccountName] [varchar] (20) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[IsAdult] [tinyint] NOT NULL ,
	[Mode] [tinyint] NOT NULL ,
	[Status] [tinyint] NOT NULL ,
	[GMFlag] [tinyint] NOT NULL ,
	[LoginStatus] [tinyint] NOT NULL ,
	[LoginTime] [datetime] NOT NULL ,
	[LogoutTime] [datetime] NOT NULL ,
	[LoginIP] [varchar] (20) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[LoginError] [int] NOT NULL ,
	[TotalOnlineTime] [int] NOT NULL ,
	[AccountFunction] [int] NOT NULL ,
	[PointNum] [int] NOT NULL ,
	[dTotalOnlineTime] [int] NOT NULL ,
	[dTotalOfflineTime] [int] NOT NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[BaseInfo] ADD 
	CONSTRAINT [DF_BaseInfo_IsAdult] DEFAULT (0) FOR [IsAdult],
	CONSTRAINT [DF_BaseInfo_Mode] DEFAULT (9) FOR [Mode],
	CONSTRAINT [DF_BaseInfo_Status] DEFAULT (0) FOR [Status],
	CONSTRAINT [DF_BaseInfo_GMFlag] DEFAULT (0) FOR [GMFlag],
	CONSTRAINT [DF_BaseInfo_LoginStatus] DEFAULT (0) FOR [LoginStatus],
	CONSTRAINT [DF_BaseInfo_LoginTime] DEFAULT (getdate()) FOR [LoginTime],
	CONSTRAINT [DF_BaseInfo_LogoutTime] DEFAULT (getdate()) FOR [LogoutTime],
	CONSTRAINT [DF_BaseInfo_LoginIP] DEFAULT ('') FOR [LoginIP],
	CONSTRAINT [DF_BaseInfo_LoginError] DEFAULT (0) FOR [LoginError],
	CONSTRAINT [DF_BaseInfo_TotalOnlineTime] DEFAULT (0) FOR [TotalOnlineTime],
	CONSTRAINT [DF_BaseInfo_AccountFunction] DEFAULT (0) FOR [AccountFunction],
	CONSTRAINT [DF_BaseInfo_PointNum] DEFAULT (0) FOR [PointNum],
	CONSTRAINT [DF_BaseInfo_dTotalOnlineTime] DEFAULT (0) FOR [dTotalOnlineTime],
	CONSTRAINT [DF_BaseInfo_dTotalOfflineTime] DEFAULT (0) FOR [dTotalOfflineTime],
	CONSTRAINT [PK_BaseInfo] PRIMARY KEY  CLUSTERED 
	(
		[AccountId]
	)  ON [PRIMARY] 
GO

