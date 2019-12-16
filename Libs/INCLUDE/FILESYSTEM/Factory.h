#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <string>

/************************************************************************/
/* 抽象工厂接口类                                                                     */
/************************************************************************/
template< typename T > 
class IFactory
{
public:
    virtual ~IFactory(void) {};

    virtual const std::string& GetType(void) const = 0;
    
    virtual T* CreateInstance( const std::string& name ) = 0;    

    virtual void DestroyInstance( T* ) = 0;    
};

#endif /*_FACTORY_H_*/