//////////////////////////////////////////////////////////////////////
//
// Filename    : DataCaching.h
// Written By  : chyaya@darkeden.com
// Description : Header Of CDataCaching Class
//
//////////////////////////////////////////////////////////////////////
#ifndef __DATACACHING_H__
#define __DATACACHING_H__

#include <map>
#include <limits>
#include <vector>
#include <list>
#include <functional>

#undef max

#pragma warning(disable:4786)

template<typename KEY_TYPE>
struct ICacheData
{
	typedef KEY_TYPE key_type;

	virtual void Create(KEY_TYPE key) = 0;
	virtual void Destory() = 0;
};

//////////////////////////////////////////////////////////////////////
//
// class CDataCaching
//
// : 데이터를 캐싱하는 간단한 클래스
//   CPackageCache를 만들다가 걍 탬플릿을 빼는게 나을 것 같아서 만들었음
//   배보다 배꼽이 큰거 같아서 일단 간단간단
//
//////////////////////////////////////////////////////////// chyaya //
template<typename DATA_TYPE>
class CDataCaching
{
public:
	CDataCaching();
	CDataCaching(int maxSize);
	virtual ~CDataCaching();

	typedef typename DATA_TYPE::key_type KEY_TYPE;
	typedef unsigned int TIME_TYPE;

	struct TimeCacheData {
		DATA_TYPE	Data;
		TIME_TYPE	LastAccessTime;

		TimeCacheData() { LastAccessTime = 0; }
		~TimeCacheData() { Data.Destory(); LastAccessTime = 0; }
	};

	typedef std::list<TimeCacheData>		DATA_LIST;
	typedef typename DATA_LIST::iterator	DATA_ITER;
	typedef std::vector<DATA_ITER>			DATAITER_VEC;
	typedef std::map<KEY_TYPE, DATA_ITER>	CACHING_MAP;

public:
	DATA_TYPE*	GetCacheData(const KEY_TYPE& key);
	bool		CacheDataExist(const KEY_TYPE& key);

	void		DeleteCacheData(const KEY_TYPE& key);
	void		DeleteLRU();

	void		ClearCacheData();

	void		SetSize(int size);

protected:
	DATA_ITER	GetNewData();

protected:
	CACHING_MAP		m_CachingMap;

	DATA_LIST		m_DataList;
	DATAITER_VEC	m_DataIterVec;

	int				m_MaxSize;
};

template<typename DATA_TYPE>
CDataCaching<DATA_TYPE>::CDataCaching()
	: m_MaxSize(0)
{

}

template<typename DATA_TYPE>
CDataCaching<DATA_TYPE>::CDataCaching(int maxSize)
	: m_MaxSize(maxSize)
{
	
}

template<typename DATA_TYPE>
CDataCaching<DATA_TYPE>::~CDataCaching()
{
	ClearCacheData();
}

template<typename DATA_TYPE>
inline DATA_TYPE* CDataCaching<DATA_TYPE>::GetCacheData(const KEY_TYPE& key)
{
	CACHING_MAP::iterator pos = m_CachingMap.find(key);

	DATA_ITER FindDataIter = m_DataList.end();

	if(pos == m_CachingMap.end())
	{
		DATA_ITER CacheDataIter = GetNewData();
		CacheDataIter->Data.Create(key);
		CacheDataIter->LastAccessTime = GetTickCount();

		m_CachingMap.insert(CACHING_MAP::value_type(key, CacheDataIter));

		FindDataIter = m_CachingMap[key];
	}
	else
	{
		FindDataIter = pos->second;
	}

	FindDataIter->LastAccessTime = GetTickCount();

	return &FindDataIter->Data;
}

template<typename DATA_TYPE>
inline bool CDataCaching<DATA_TYPE>::CacheDataExist(const KEY_TYPE& key)
{
	CACHING_MAP::iterator pos = m_CachingMap.find(key);
	return pos != m_CachingMap.end();
}

template<typename DATA_TYPE>
inline void CDataCaching<DATA_TYPE>::DeleteCacheData(const KEY_TYPE& key)
{
	CACHING_MAP::iterator pos = m_CachingMap.find(key);

	if(pos != m_CachingMap.end())
	{
		pos->second->Data.Destory();
		m_DataIterVec.push_back(pos->second);
		m_CachingMap.erase(pos);
	}
}


template<typename DATA_TYPE>
inline void CDataCaching<DATA_TYPE>::DeleteLRU()
{
	CACHING_MAP::iterator LeastTimePos = m_CachingMap.end();
	TIME_TYPE LeastTime = std::numeric_limits<TIME_TYPE>::max();

	CACHING_MAP::iterator pos = m_CachingMap.begin();
	for(; pos != m_CachingMap.end(); ++pos)
	{
		DWORD time = pos->second->LastAccessTime;
		if(time < LeastTime)
		{
			LeastTimePos	= pos;
			LeastTime		= time;
		}
	}

	if(LeastTimePos != m_CachingMap.end())
	{
		// 캐시에서 삭제한뒤 사용 가능한 상태로 등록
		LeastTimePos->second->Data.Destory();
		m_DataIterVec.push_back(LeastTimePos->second);
		m_CachingMap.erase(LeastTimePos);
	}
}


template<typename DATA_TYPE>
inline void CDataCaching<DATA_TYPE>::ClearCacheData()
{
	// TimeCacheData 소멸자가 호출되며
	// 자동으로 Destory가 호출된다.
	m_DataList.clear();
	m_DataIterVec.clear();
	m_CachingMap.clear();
}


template<typename DATA_TYPE>
inline void CDataCaching<DATA_TYPE>::SetSize(int size)
{
	ClearCacheData();
	m_MaxSize = size;
}


template<typename DATA_TYPE>
inline typename CDataCaching<DATA_TYPE>::DATA_ITER
CDataCaching<DATA_TYPE>::GetNewData()
{
	// DataVec의 크기가 m_MaxSize보다 작다면
	// 새로운 데이터를 만든다.
	if((int)m_DataList.size() < m_MaxSize)
	{
		m_DataList.push_front(TimeCacheData());
		m_DataIterVec.push_back(m_DataList.begin());
	}

	// 남은 데이터가 없다면 LRU
	if(m_DataIterVec.empty())
	{
		DeleteLRU();
	}

	assert(!m_DataIterVec.empty());

	// 새로운 데이터를 얻었다.
	DATA_ITER NewDataIter = m_DataIterVec.back();
	m_DataIterVec.pop_back();

	NewDataIter->LastAccessTime = GetTickCount();

	return NewDataIter;
}


#endif	//__DATACACHING_H__
