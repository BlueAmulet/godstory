#ifndef CLIENT_CHANNEL_MGR_H
#define CLIENT_CHANNEL_MGR_H

#include <stdio.h>

struct stChannelPage
{
	int nChannelCount;

	bool channels[20];


	stChannelPage()
	{
		nChannelCount = 0;
	}
};

struct stChannnelMgr
{
	int nChannelPageCount;

	stChannelPage pages[10];

	bool globalChannels[20];

	stChannnelMgr()
	{
		nChannelPageCount = 0;
	}

	void Write()
	{
		FILE* fp;
		errno_t err = fopen_s(&fp,  "Config/channel.dat", "wb+" );
		if( err != 0 )
			return ;
		fwrite( this, 1, sizeof( stChannnelMgr), fp );
		fclose( fp );
	}	

	void Read()
	{
		FILE* fp;
		errno_t err = fopen_s(&fp,  "Config/channel.dat", "rb" );
		if( err != 0 )
			return ;
		fread( this, 1, sizeof(stChannnelMgr), fp );
		fclose( fp );
	}
};

extern stChannnelMgr g_ChannelMgr;
#define CLIENT_CHANNEL (&g_ChannelMgr);

#endif