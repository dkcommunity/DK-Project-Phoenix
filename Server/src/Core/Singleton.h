/////////////////////////////////////////////////////////////////////
//
// filename 	: Singleton.h
// written by	: Sequoia
// Description	: Singleton base class
// 				  상속 받으면 알아서 singleton 이 된다.
//
// 				  ex)
// 				  class Derived : public Singleton<Derived>
// 				  {
// 				  protected:
// 				  		Derived() {...}
// 				  		friend class Singleton<Derived>;
// 				  };
// 				  
// 				  caution)
// 				  반드시 하위 클래스에서 friend 만들어줘야 하위클래스의 생성자에 접근할 수 있다.
//	
//					2003.12.13
//					GPG1에 나온-_- 방식 추가 by sonee
//					생성자가 호출되어야 하기 때문에... 반드시 인스턴스 필요.
//
/////////////////////////////////////////////////////////////////////

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <cassert>

template<class T>
class Singleton
{
public:
	static T& Instance()
	{
		static T theInstance;

		return theInstance;
	}
};

#endif
