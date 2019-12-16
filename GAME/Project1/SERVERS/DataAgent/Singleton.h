template < class T >
class TSingleton
{
public:
	static T* GetInstance()
	{
		static T instance;
		
		return &instance;
	}
protected:
	TSingleton() {;}
};