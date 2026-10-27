//----------------------------------------------------------------------
// KeyAccelerator.cpp
//----------------------------------------------------------------------

#include "client_PCH.h"
#include <fstream>
#include "KeyAccelerator.h"
#include "CDirectInput.h"
#include "AcceleratorDef.h"
#include "crash_trace_tool.h"
//----------------------------------------------------------------------
// define functions
//----------------------------------------------------------------------
#define READ_CHECK_EOF( value, temp, bytes )		\
		{											\
			file.read((char*)&temp, bytes);			\
			if (!file.eof())						\
			{										\
				value = temp;						\
				return false;						\
			}										\
			else									\
			{										\
				return true;						\
			}										\
		}
//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
KeyAccelerator*		g_pKeyAccelerator = NULL;

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
KeyAccelerator::KeyAccelerator()
{
}

KeyAccelerator::~KeyAccelerator()
{
}

//----------------------------------------------------------------------
// Init ( max )
//----------------------------------------------------------------------
// FIX 2026-05-12: VS6 mirasi `reserve(max)` sadece capacity ayirir; size()=0
// kalir. Sonraki SetAcceleratorKey/GetKey icindeki `m_Accelerators[accel]`
// UNDEFINED BEHAVIOR (size() < accel). MSVC release vector'unde silent yazma
// olabilir ama m_Keys map'inin temizligi bozulur (find UB-read ile yanlis
// stale key erase eder). Sonuc: harf shortcut'lari (S/TAB/I/K/M/P/Q/H/B vs.)
// dispatch'te map'ten cikamayabiliyor.
// Resize ile size=max, hepsi 0 init -> tum index'ler valid.
void
KeyAccelerator::Init( int max )
{
	m_Keys.clear();
	m_Accelerators.clear();

	m_Accelerators.resize( max, 0 );
}

//----------------------------------------------------------------------
// Set AcceleratorKey ( accel, key )
//----------------------------------------------------------------------
// key�� ������ accel�� ����ȴ�..�� �ǹ�.
// accel�� key�� ����Ǿ��ִ�..�� �ǹ̵� �ǰ�..
//----------------------------------------------------------------------
void
KeyAccelerator::SetAcceleratorKey(BYTE accel, WORD key)
{
	// FIX 2026-05-12: capacity() yerine size() — resize sonrasi ikisi de >= max
	// olur, ama size() check semantik olarak dogru (yazilan element'in valid index'i).
	if (accel >= m_Accelerators.size())
	{
		return;
	}

	KEY_MAP::iterator iKey = m_Keys.find( m_Accelerators[accel] );

	if (iKey != m_Keys.end())
	{
		// �̹� ������ key�� �־��ٸ� �����Ѵ�.
		m_Keys.erase( iKey );
	}

	m_Keys[key] = accel;

	m_Accelerators[accel] = key;
}

//----------------------------------------------------------------------
// Get Accelerator ( key )
//----------------------------------------------------------------------
// Ȯ���� �� key�� �Էµ� ���
//----------------------------------------------------------------------
BYTE
KeyAccelerator::GetAccelerator(WORD key) const
{
	KEY_MAP::const_iterator iKey = m_Keys.find( key );

	if (iKey==m_Keys.end())
	{
		return 0;
	}

	return iKey->second;
}

//----------------------------------------------------------------------
// Get AcceleratorSimilar ( key )
//----------------------------------------------------------------------
// ctrl+I�� ������ ����Ǵ°�
// ctrl+shift+I�� ������ ������ �ȵȴ�.
// �̷� ��쿡�� ����ǰ� �ҷ���.. ����Ű�� �����ؼ� üũ�ؾ� �Ѵ�.
//----------------------------------------------------------------------
BYTE				
KeyAccelerator::GetAcceleratorSimilar(WORD key) const
{
	// �Էµ� key
	KEY_MAP::const_iterator iKey = m_Keys.find( key );

	if (iKey!=m_Keys.end())
	{
		return iKey->second;
	}

	WORD checkKey;	

	int bHasControl	= ACCEL_HAS_CONTROL(key);
	int bHasAlt		= ACCEL_HAS_ALT(key);
	int bHasShift	= ACCEL_HAS_SHIFT(key);

	//------------------------------------------------------------
	// ���� key�� ���� ���
	//------------------------------------------------------------
//	if (!bHasControl && !bHasAlt && !bHasShift)
//	{
//		return 0;
//	}

	//------------------------------------------------------------
	// ctrl����
	//------------------------------------------------------------
	if (bHasControl)
	{
		checkKey = ACCEL_REMOVE_CONTROL( key );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;
	}

	//------------------------------------------------------------
	// alt����
	//------------------------------------------------------------
	if (bHasAlt)
	{
		checkKey = ACCEL_REMOVE_ALT( key );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;
	}

	//------------------------------------------------------------
	// shift����
	//------------------------------------------------------------
	if (bHasShift)
	{
		checkKey = ACCEL_REMOVE_SHIFT( key );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;
	}

	WORD keyOnly = ACCEL_GET_KEY( key );

	//------------------------------------------------------------
	// ctrl + alt + shift �� ���
	//------------------------------------------------------------
	if (bHasControl && bHasAlt && bHasShift)
	{
		//------------------------------------------------------------
		// ctrl�� �����.
		//------------------------------------------------------------
		checkKey = ACCEL_ADD_CONTROL( keyOnly );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;	

		//------------------------------------------------------------
		// alt�� �����
		//------------------------------------------------------------
		checkKey = ACCEL_ADD_ALT( keyOnly );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;		

		//------------------------------------------------------------
		// shift�� �����
		//------------------------------------------------------------
		checkKey = ACCEL_ADD_SHIFT( keyOnly );
		iKey = m_Keys.find( checkKey );
		if (iKey!=m_Keys.end())	return iKey->second;
	}

	//------------------------------------------------------------
	// �� ������ ���
	//------------------------------------------------------------
	iKey = m_Keys.find( keyOnly );
	if (iKey!=m_Keys.end())	return iKey->second;

	return 0;
}

//----------------------------------------------------------------------
// Get Key ( accel )
//----------------------------------------------------------------------
WORD
KeyAccelerator::GetKey(BYTE accel) const
{
	// FIX 2026-05-12: capacity() yerine size() — UB read'i engelle.
	if (accel < m_Accelerators.size())
	{
		return m_Accelerators[accel];
	}

	return 0;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void				
KeyAccelerator::SaveToFile(std::ofstream& file)
{
	// FAZ 1.8 — UserOption persistence fix.
	// Eski kod: int num = m_Accelerators.capacity();
	// Sorun: MSVC std::vector<WORD>::reserve(N) bazı durumlarda capacity'yi
	// N'den büyük bir değere yuvarlayabiliyor. Load tarafı ise num == MAX_ACCELERATOR
	// eşitliğini kontrol ediyor → eşleşme sağlanamıyor → ResetUserOption()
	// çağrılıyor ve UserSet\*.set dosyalarının tümü siliniyor, VolumeMusic/PlayMusic
	// gibi tüm ayarlar defaults'a düşüyor. Save eden oyun, bir sonraki açılışta
	// hiçbir ayarı hatırlamıyordu.
	// Düzeltme: Save yazarken capacity() yerine MAX_ACCELERATOR yazıyoruz —
	// bu sayede Load tarafındaki num == MAX_ACCELERATOR karşılaştırması garantilenir.
	int num = (int)MAX_ACCELERATOR;
	CT_Done("[ACCEL-SAVE] MAX_ACCELERATOR=%d capacity()=%d writing num=%d",
		(int)MAX_ACCELERATOR, (int)m_Accelerators.capacity(), num);

	file.write((const char*)&num, 4);

	// key-accelerator'un sadece key'lerini saklamak yeterli.
	// accel'ler sabit enum değerleri, kaydetmeye gerek yok.
	// Sadece key'leri sırayla yazıyoruz.
	for (int accel=1; accel<num; accel++)
	{
		// FIX 2026-05-12: Init artik resize() kullaniyor, size() == max garantili.
		// Vector erisimi guvenli.
		WORD key = 0;
		if (accel < (int)m_Accelerators.size())
			key = m_Accelerators[accel];

		file.write((const char*)&key, 2);
	}
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
bool
KeyAccelerator::LoadFromFile(ivfstream& file)
{
	int num;
	file.read((char*)&num, 4);

	Init(MAX_ACCELERATOR);

	CT_Done("[ACCEL-LOAD] read num=%d  MAX_ACCELERATOR=%d  match=%d",
		num, (int)MAX_ACCELERATOR, (int)(num == MAX_ACCELERATOR));

	// ����Ű�� ������ ��ġ�� ���
	// ������ �´ٰ� �Ǵ�
	if(num == MAX_ACCELERATOR)
	{
		WORD key;

		for (int accel=1; accel<num; accel++)
		{
			file.read((char*)&key, 2);

			if(!file.eof())
				SetAcceleratorKey( accel , key );
		}

		return true;
	}
	else
	{
		SetDefaultAccelerator();

		return false;
	}
}
