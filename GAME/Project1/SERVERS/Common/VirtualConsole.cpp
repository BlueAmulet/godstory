#include "VirtualConsole.h"
#include <process.h>

VirtualConsole::VirtualConsole()
{
	::AllocConsole();

	mStdInput = GetStdHandle( STD_INPUT_HANDLE );
	mStdOutput = GetStdHandle( STD_OUTPUT_HANDLE );
	mStdError = GetStdHandle( STD_ERROR_HANDLE );

	mPromote = "#>";

	for (int iIndex = 0; iIndex < MaxCmdCount; iIndex ++)
		rgCmds[iIndex][0] = '\0';

	iCmdIndex = 0;
	inpos = 0;

	mWorkThread = (HANDLE)_beginthread( workMethod, 0, this );
}

VirtualConsole::~VirtualConsole()
{
	TerminateThread( mWorkThread, 0 );
}

void VirtualConsole::process()
{
	DWORD dwInputEventCount;
	GetNumberOfConsoleInputEvents( mStdInput, &dwInputEventCount );

	if( dwInputEventCount == 0 )
		return ;

	INPUT_RECORD rec[200];
	char outbuf[512];
	int outpos = 0;

	ReadConsoleInput(mStdInput, rec, 200, &dwInputEventCount);

	for(DWORD i = 0; i < dwInputEventCount; i++)
	{			
		if(rec[i].EventType == KEY_EVENT)
		{				
			KEY_EVENT_RECORD *ke = &(rec[i].Event.KeyEvent);

			if(ke->bKeyDown)
			{
				if((wchar_t)ke->uChar.UnicodeChar > 255)
				{
					char szANSI[3];     
					WideCharToMultiByte(936, WC_COMPOSITECHECK, &(ke->uChar.UnicodeChar), -1, szANSI, 3, NULL, NULL); 
					inbuf[inpos++] = szANSI[0];
					inbuf[inpos++] = szANSI[1];
					outbuf[outpos++] = szANSI[0];
					outbuf[outpos++] = szANSI[1];
				}
				else
				{			   
					switch (ke->uChar.AsciiChar)
					{
						// If no ASCII char, check if it's a handled virtual key
					case 0:
						switch (ke->wVirtualKeyCode)
						{
							// UP ARROW
						case 0x26 :
							// Go to the previous command in the cyclic array
							if ((-- iCmdIndex) < 0)
								iCmdIndex = MaxCmdCount - 1;

							// If this command isn't empty ...
							if (rgCmds[iCmdIndex][0] != '\0')
							{
								// Obliterate current displayed text
								for (int i = outpos = 0; i < inpos; i ++)
								{
									outbuf[outpos ++] = '\b';
									outbuf[outpos ++] = ' ';
									outbuf[outpos ++] = '\b';
								}

								// Copy command into command and display buffers
								for (inpos = 0; inpos < (int)strlen(rgCmds[iCmdIndex]); inpos ++, outpos ++)
								{
									outbuf[outpos] = rgCmds[iCmdIndex][inpos];
									inbuf [inpos ] = rgCmds[iCmdIndex][inpos];
								}
							}
							// If previous is empty, stay on current command
							else if ((++ iCmdIndex) >= MaxCmdCount)
							{
								iCmdIndex = 0;
							}

							break;

							// DOWN ARROW
						case 0x28 : {
							// Go to the next command in the command array, if
							// it isn't empty
							if (rgCmds[iCmdIndex][0] != '\0' && (++ iCmdIndex) >= MaxCmdCount)
								iCmdIndex = 0;

							// Obliterate current displayed text
							for (int i = outpos = 0; i < inpos; i ++)
							{
								outbuf[outpos ++] = '\b';
								outbuf[outpos ++] = ' ';
								outbuf[outpos ++] = '\b';
							}

							// Copy command into command and display buffers
							for (inpos = 0; inpos < (int)strlen(rgCmds[iCmdIndex]); inpos ++, outpos ++)
							{
								outbuf[outpos] = rgCmds[iCmdIndex][inpos];
								inbuf [inpos ] = rgCmds[iCmdIndex][inpos];
							}
									}
									break;

									// LEFT ARROW
						case 0x25 :
							break;

							// RIGHT ARROW
						case 0x27 :
							break;

						default :
							break;
						}
						break;
					case '\b':
						if(inpos > 0)
						{
							outbuf[outpos++] = '\b';
							outbuf[outpos++] = ' ';
							outbuf[outpos++] = '\b';
							inpos--;
						}
						break;
					case '\t':
						// In the output buffer, we're going to have to erase the current line (in case
						// we're cycling through various completions) and write out the whole input
						// buffer, so (inpos * 3) + complen <= 512.  Should be OK.  The input buffer is
						// also 512 chars long so that constraint will also be fine for the input buffer.
						//{
						//	// Erase the current line.
						//	DWORD i;
						//	for (i = 0; i < inpos; i++) {
						//		outbuf[outpos++] = '\b';
						//		outbuf[outpos++] = ' ';
						//		outbuf[outpos++] = '\b';
						//	}
						//	// Modify the input buffer with the completion.
						//	DWORD maxlen = 512 - (inpos * 3);
						//	if (ke->dwControlKeyState & SHIFT_PRESSED) {
						//		inpos = Con::tabComplete(inbuf, inpos, maxlen, false);
						//	}
						//	else {
						//		inpos = Con::tabComplete(inbuf, inpos, maxlen, true);
						//	}
						//	// Copy the input buffer to the output.
						//	for (i = 0; i < inpos; i++) {
						//		outbuf[outpos++] = inbuf[i];
						//	}
						//}
						break;
					case '\n':
					case '\r':
						outbuf[outpos++] = '\r';
						outbuf[outpos++] = '\n';

						inbuf[inpos] = 0;
						outbuf[outpos] = 0;
						_printf("%s", outbuf);

						// Pass the line along to the console for execution.
				
						__raise onInput( inbuf, inpos + 1 );

						// If we've gone off the end of our array, wrap
						// back to the beginning
						if (iCmdIndex >= MaxCmdCount)
							iCmdIndex %= MaxCmdCount;

						// Put the new command into the array
						strcpy_s(rgCmds[iCmdIndex ++], 512, inbuf);

						_printf("%s", mPromote.c_str() );
						inpos = outpos = 0;
						break;
					default:
						{							 
							inbuf[inpos++] = ke->uChar.AsciiChar;
							outbuf[outpos++] = ke->uChar.AsciiChar;
						}
						break;
					}
				}
			}
		}
	}
	if(outpos)
	{
		outbuf[outpos] = 0;
		_printf("%s", outbuf);
	}
}

void VirtualConsole::_printf( const char* format, ... )
{ 
	static const int BufSize = 4096;
	static char buffer[4096];
	DWORD bytes;
	va_list args;
	va_start(args, format);
	_vsnprintf_s(buffer, BufSize, BufSize, format, args);
	// Replace tabs with carats, like the "real" console does.
	//char *pos = buffer;
	//while (*pos) {
	//	if (*pos == '\t') {
	//		*pos = '^';
	//	}
	//	pos++;
	//}

	WriteFile(mStdOutput, buffer, strlen(buffer), &bytes, NULL);
	FlushFileBuffers( mStdOutput );
}

void VirtualConsole::printf( const char* format, ... )
{
	static const int BufSize = 4096;
	static char buffer[4096];

	va_list args;
	va_start(args, format);
	_vsnprintf_s(buffer, BufSize, BufSize, format, args);

	inbuf[inpos] = 0;
	_printf( "\r%s%s%s", buffer, mPromote.c_str(), inbuf );
}

void VirtualConsole::workMethod( void* param )
{
	VirtualConsole* pThis = static_cast< VirtualConsole* >( param );

	while( 1 )
	{
		pThis->process();
		Sleep( 50 );
	}
}