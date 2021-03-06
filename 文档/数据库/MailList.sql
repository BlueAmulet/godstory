if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[MailList]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[MailList]
GO

CREATE TABLE [dbo].[MailList] (
	[id] [int] IDENTITY (1, 1) NOT NULL ,
	[sender] [int] NOT NULL ,
	[sender_name] [nvarchar] (40) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[money] [int] NULL ,
	[itemId] [int] NULL ,
	[item_count] [int] NULL ,
	[msg] [nvarchar] (512) COLLATE Chinese_PRC_CI_AS NULL ,
	[send_time] [int] NOT NULL ,
	[recver] [int] NOT NULL ,
	[readed] [int] NULL ,
	[received] [int] NULL ,
	[isSystem] [int] NULL ,
	[title] [nvarchar] (128) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[itemInfo] [image] NOT NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

ALTER TABLE [dbo].[MailList] ADD 
	CONSTRAINT [DF_MailList_readed] DEFAULT (0) FOR [readed],
	CONSTRAINT [DF_MailList_received] DEFAULT (0) FOR [received],
	CONSTRAINT [DF_MailList_isSystem] DEFAULT (0) FOR [isSystem],
	CONSTRAINT [DF_MailList_itemInfo] DEFAULT (0x00) FOR [itemInfo],
	CONSTRAINT [PK_MailList] PRIMARY KEY  CLUSTERED 
	(
		[id]
	)  ON [PRIMARY] 
GO

