#ifndef _CONFIGFILE_H_
#define _CONFIGFILE_H_

#include <string>
#include <vector>
#include <hash_map>

#include "DataStream.h"

/************************************************************************/
/*²Ù×÷ÅäÖÃÎÄ¼þ                                                                      */
/************************************************************************/
class CConfigFile
{
public:
    typedef std::vector<std::string>               SETTINGS;
    typedef stdext::hash_map<std::string,SETTINGS> KEYS_MAP;
    typedef stdext::hash_map<std::string,KEYS_MAP> SECTIONS_MAP;
public:
    bool LoadConfig(const char* cfgFile);

    bool LoadConfig(DataStreamPtr stream);

    SETTINGS& GetSettings(std::string section,std::string key);

    KEYS_MAP& GetKeys(std::string section);

    SECTIONS_MAP& GetSections(void);
protected:
    SECTIONS_MAP m_cfgMap;
};

#endif /*_CONFIGFILE_H_*/
