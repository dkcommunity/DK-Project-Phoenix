//////////////////////////////////////////////////////////////////////
//
// Filename    : BitAttribute.h
// Written By  : chyaya@darkeden.com
// Description : Header Of CBitAttribute Class
//
//////////////////////////////////////////////////////////////////////
#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__

//////////////////////////////////////////////////////////////////////
//
// class CBitAttribute
//
// : 각종 bool 속성을 bit연산으로 저장한다.
//
//////////////////////////////////////////////////////////// chyaya //
template<typename DATA_TYPE, typename SIZE_TYPE>
class CBitAttribute
{
public:
	inline				CBitAttribute();
	inline				CBitAttribute(DATA_TYPE attr);
	inline				~CBitAttribute();

	inline DATA_TYPE	operator = (DATA_TYPE attr);

public:
	inline void			SetNULL();

	inline void			SetAttr(DATA_TYPE attr, bool flag = true);
	inline void			ToggleAttr(DATA_TYPE attr);

	inline bool			GetAttr(DATA_TYPE attr) const;

public:
	inline void			operator |= (DATA_TYPE attr);
	inline void			operator &= (DATA_TYPE attr);

public:
	inline DATA_TYPE	GetValue() const;
	inline SIZE_TYPE	GetValueSize() const;

private:
	DATA_TYPE			m_nAttribute;
};


template<class DATA_TYPE, class SIZE_TYPE>
CBitAttribute<DATA_TYPE, SIZE_TYPE>::CBitAttribute()
{
	SetNULL();
}


template<class DATA_TYPE, class SIZE_TYPE>
CBitAttribute<DATA_TYPE, SIZE_TYPE>::CBitAttribute(DATA_TYPE attr)
{
	m_nAttribute = attr;
}


template<class DATA_TYPE, class SIZE_TYPE>
CBitAttribute<DATA_TYPE, SIZE_TYPE>::~CBitAttribute()
{

}

template<class DATA_TYPE, class SIZE_TYPE>
DATA_TYPE CBitAttribute<DATA_TYPE, SIZE_TYPE>::operator = (DATA_TYPE attr)
{
	return m_nAttribute = attr;
}


template<class DATA_TYPE, class SIZE_TYPE>
void CBitAttribute<DATA_TYPE, SIZE_TYPE>::SetNULL()
{
	m_nAttribute = 0;
}


template<class DATA_TYPE, class SIZE_TYPE>
void CBitAttribute<DATA_TYPE, SIZE_TYPE>::SetAttr(DATA_TYPE attr, bool flag)
{
	if(flag)	m_nAttribute |= attr;
	else		m_nAttribute &= ~attr;
}


template<class DATA_TYPE, class SIZE_TYPE>
void CBitAttribute<DATA_TYPE, SIZE_TYPE>::ToggleAttr(DATA_TYPE attr)
{
	SetAttribute(attr, m_nAttribute ^ attr);
}


template<class DATA_TYPE, class SIZE_TYPE>
bool CBitAttribute<DATA_TYPE, SIZE_TYPE>::GetAttr(DATA_TYPE attr) const
{
	return (m_nAttribute & attr) ? true : false;
}


template<class DATA_TYPE, class SIZE_TYPE>
void CBitAttribute<DATA_TYPE, SIZE_TYPE>::operator |= (DATA_TYPE attr)
{
	m_nAttribute |= attr;
}


template<class DATA_TYPE, class SIZE_TYPE>
void CBitAttribute<DATA_TYPE, SIZE_TYPE>::operator &= (DATA_TYPE attr)
{
	m_nAttribute &= attr;
}


template<class DATA_TYPE, class SIZE_TYPE>
DATA_TYPE CBitAttribute<DATA_TYPE, SIZE_TYPE>::GetValue() const
{
	return m_nAttribute;
}


template<class DATA_TYPE, class SIZE_TYPE>
SIZE_TYPE CBitAttribute<DATA_TYPE, SIZE_TYPE>::GetValueSize() const
{
	return sizeof(DATA_TYPE);
}

// 타이핑 수고를 줄여주는 유용한 정의
typedef CBitAttribute<unsigned char, unsigned int>	BYTE_ATTRIBUTE;
typedef CBitAttribute<unsigned short, unsigned int>	WORD_ATTRIBUTE;
typedef CBitAttribute<unsigned long, unsigned int>	DWORD_ATTRIBUTE;

#endif	//__ATTRIBUTE_H__