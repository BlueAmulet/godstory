/**********************************************************************

Filename    :   FxCommand.h
Content     :   Interface to the game engine from GFx via fscommands
Created     :   11/9/2006
Authors     :   Brendan Iribe, Prasad Silva
Copyright   :   (c) 2005-2006 Scaleform Corp. All Rights Reserved.

This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING 
THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR ANY PURPOSE.

**********************************************************************/

#ifndef INC_FXCOMMAND_H
#define INC_FXCOMMAND_H

#include "GFxString.h"

struct gstring_key
{
    const char *pChar;

    gstring_key(const char *pc)
    {
        pChar = pc;
    }
    bool operator == (const gstring_key& c) const
    {
        return strcmp(pChar, c.pChar) == 0;
    }
    bool operator != (const gstring_key& c) const
    {
        return strcmp(pChar, c.pChar) != 0;
    }
};

class gstring_hashfn
{
public:

    size_t  operator()(const gstring_key& data) const
    { 
        size_t  size = strlen(data.pChar);
        return GFxString::BernsteinHashFunction(data.pChar, size);
    }
};

template <class N>
class GConstStringHash : public GTL::ghash<gstring_key, N, gstring_hashfn> {};

struct FxCommand
{
    int         Key;
    const char* Command;
};


class FxCommandHandler
{
public:

    //  Return the command map (map between command enumeration value and fscommand string)
    // This mapping can be different for objects derived from the same base class
    virtual FxCommand* GetCommandMap() = 0;

    // Process any commands from the ActionScript.
    // returns 1 if command handled, otherwise, 0 to continue processing
    virtual bool VFsCallback(class GFxMovieView* pmovie, 
                             int cmdKey, const char* cmdName, const char* args) = 0;

    virtual ~FxCommandHandler(){};
};


class FxCommandManager
{
protected:

    struct CommandNode
    {
        int  Key;
        FxCommandHandler* pClass;

        CommandNode(int k, FxCommandHandler*p)
        {
            Key = k; pClass = p;
        }
    };

    GConstStringHash<CommandNode>  CommandMap;

public:

    // Registers the command handler
    void    Register(FxCommandHandler* phandler);
    void    Unregister(FxCommandHandler* phandler);

    // Process any commands from the ActionScript.
    // returns 1 if command handled, otherwise, 0 to continue processing
    bool    FsCallback(class GFxMovieView* pmovie, const char* command, const char* args);
};


#endif//INC_GFXCOMMAND_H
