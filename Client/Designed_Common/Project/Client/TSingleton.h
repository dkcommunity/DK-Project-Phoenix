#ifndef __TSINGLETON_H__
#define __TSINGLETON_H__

//#include <assert.h>

#pragma warning( disable : 4311 ) // 'type cast'
#pragma warning( disable : 4312 ) // 'type cast'
template<class T> 
class TSingleton
{
	static T* m_pInstance;
public:
	TSingleton()
	{
		/*assert( !m_pInstance );*/
		int offset = (int)(T*)1-(int)(TSingleton<T>*)(T*)1;
		m_pInstance = (T*)((int)this + offset );
	}
	~TSingleton() { /*assert( m_pInstance );*/ m_pInstance = 0; }
	static T* Instance() { return m_pInstance; }
};
#pragma warning( default : 4311 )
#pragma warning( default : 4312 )

template <class T> T* TSingleton<T>::m_pInstance = 0;


#endif	//__TSINGLETON_H__
