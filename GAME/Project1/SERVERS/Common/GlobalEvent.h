#ifndef GLOBAL_EVENT_H
#define GLOBAL_EVENT_H

struct stGlobalEvent
{
	unsigned char id;
	int			time;
	char		type;

	static const int MaxEventCount = 256;
};

#endif