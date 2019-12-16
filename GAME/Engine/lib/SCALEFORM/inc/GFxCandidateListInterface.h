/**********************************************************************

Filename    :   CharacterInterface.h
Content     :   Character browser interface from GFx (Flash) to engine
Created     :   6/18/2006
Authors     :   Prasad Silva
Copyright   :   (c) 2007 Scaleform Corp. All Rights Reserved.

This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING 
THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR ANY PURPOSE.

**********************************************************************/

#ifndef INC_CHARACTERINTERFACE_H
#define INC_CHARACTERINTERFACE_H

#include "GFxCandidateListBox.h"

struct CandidateListItem : public FxCandidateListItem
{
    UInt ID;
    GFxString Name;

    CandidateListItem(char* str)
    {
        Name = str;
    }

    virtual void    operator () (GFxValue& pvalue, const GFxString& key);
};

class CandidateListBox : public FxCandidateListBox
{
public:

    CandidateListBox(){};

    FxCommand* GetCommandMap();

};

#endif

