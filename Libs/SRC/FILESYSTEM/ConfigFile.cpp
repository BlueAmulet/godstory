#include "ConfigFile.h"
#include <fstream>
#include "FileDataStream.h"

#define MAX_LINE_LENGTH  256

namespace
{
    void trim(std::string& str)
    {
        static const std::string delims = " \t\r";
        str.erase(str.find_last_not_of(delims)+1); // trim right
        str.erase(0, str.find_first_not_of(delims)); // trim left
    }
}

bool CConfigFile::LoadConfig(const char* cfgFile)
{
    std::fstream* fs = new std::fstream;
    fs->open(cfgFile,std::ios_base::in | std::ios_base::binary);

    if (!fs->is_open())
    {
        delete fs;
        return false;
    }

    DataStreamPtr stream = new CFileDataStream(fs);
    return LoadConfig(stream);
}

bool CConfigFile::LoadConfig(DataStreamPtr stream)
{
    SECTIONS_MAP::iterator curSectionIter = m_cfgMap.end();

    std::string line;

    while(!stream->IsEof())
    {   
        line = stream->ReadLine();

        if (line.length() > 0 && line.at(0) != '/' && line.at(0) != '/')
        {
            if (line.at(0) == '[' && line.at(line.length()-1) == ']')
            {
                // Section
                std::string curSection = line.substr(1, line.length() - 2);            

                SECTIONS_MAP::iterator iter = m_cfgMap.find(curSection);

                if (iter == m_cfgMap.end())
                {
                    iter = m_cfgMap.insert(std::make_pair(curSection,KEYS_MAP())).first;
                }

                curSectionIter = iter;
            }
            else
            {
                //配置文件非法
                if (curSectionIter == m_cfgMap.end())
                    return false;

                //找到相应的key值
                std::string::size_type separator_pos = line.find_first_of('=',0);

                if (separator_pos != std::string::npos)
                {
                    std::string keyName = line.substr(0, separator_pos);

                    //找到相应的setting值
                    std::string::size_type nonseparator_pos = line.find_first_not_of('=', separator_pos);

                    std::string setting = (nonseparator_pos == std::string::npos) ? "" : line.substr(nonseparator_pos);

                    trim(keyName);
                    trim(setting);

                    KEYS_MAP::iterator iterKey = curSectionIter->second.find(keyName);

                    if (iterKey == curSectionIter->second.end())
                    {
                        iterKey = curSectionIter->second.insert(std::make_pair(keyName,SETTINGS())).first;
                    }

                    iterKey->second.push_back(setting);
                }
            }
        }
    }

    return true;
}

CConfigFile::SETTINGS& CConfigFile::GetSettings(std::string section,std::string key)
{
    SECTIONS_MAP::iterator iterSection = m_cfgMap.find(section);

    if (iterSection == m_cfgMap.end())
        return SETTINGS();

    KEYS_MAP::iterator iterKey = iterSection->second.find(key);

    return iterKey->second;
}

CConfigFile::KEYS_MAP& CConfigFile::GetKeys(std::string section)
{
    SECTIONS_MAP::iterator iter = m_cfgMap.find(section);

    if (iter == m_cfgMap.end())
        return KEYS_MAP();

    return iter->second;
}

CConfigFile::SECTIONS_MAP& CConfigFile::GetSections(void)
{   
    return m_cfgMap;
}