//--------------------------------------------------------------------------------
//
// Filename   : WeatherManager.cpp
// Written By : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "WeatherManager.h"
#include "GameTime.h"
#include "TimeManager.h"
#include "WeatherInfo.h"
#include "LogClient.h"
#include "DarkLightInfo.h"
#include "Zone.h"
#include "PKZoneInfoManager.h"
#include "Gpackets/GCChangeDarkLight.h"
#include "Gpackets/GCChangeWeather.h"
#include "Gpackets/GCLightning.h"

//--------------------------------------------------------------------------------
//
// destructor
//
// �����̳��� ��� ��ü���� DB�� ������ ��, �����Ѵ�.
//
//--------------------------------------------------------------------------------
WeatherManager::~WeatherManager ()
	throw (Error)
{
    __BEGIN_TRY
    __END_CATCH
}


//--------------------------------------------------------------------------------
// initialize current zone's weather, darklevel, lightlevel
//--------------------------------------------------------------------------------
void WeatherManager::init ()
	throw (Error)
{
	__BEGIN_TRY

	//--------------------------------------------------------------------------------
	// ������ ������ �����Ѵ�.
	//--------------------------------------------------------------------------------

	// ������ ������� �˱� ���ؼ�, GameTime ��ü�� �����´�.
	GameTime gametime = g_pTimeManager->getGameTime();

	// �̹����� ���� ������ �޾ƿ´�.
	//const WeatherInfo & weatherInfo = g_pWeatherInfoManager->getWeatherInfo(gametime.getMonth());

	// ���̽��� ������, ������ ������ �����Ѵ�.
	//m_TodayWeather = weatherInfo.getWeather(Dice(1,100));

	//--------------------------------------------------------------------------------
	// ���� ��¥�� �����Ѵ�.
	//--------------------------------------------------------------------------------
	
	// ���ӽð��� time_t �� �޾ƿ´�.
	time_t gmtime = g_pTimeManager->getgametime();

	// tm ��Ʈ��ó�� ��ȯ, ��/��/�� ���� �˾Ƴ���.
	tm ltm;
	localtime_r( &gmtime, &ltm );
	//struct tm* ptm = localtime(&gmtime);

	// ���� �ð����� ���ϱ��� ���� �ʸ� ����Ѵ�.
	int dSec = (23 - ltm.tm_hour)* 3600 + (59 - ltm.tm_min)* 60 + (60 - ltm.tm_sec);

	// (���� ���� �ð� / 5) �� ���� �ǽð��� ���ϸ�, ������ ���� �ð��� ���´�.
	time_t currentTime = time(0);
	m_Tomorrow = currentTime + dSec / 5;

	//--------------------------------------------------------------------------------
	// ������ ����, ��������, ������������ð��� �����Ѵ�.
	//--------------------------------------------------------------------------------
	// ��/���� �� Ȯ���� ������ ������ �����Ѵ�.
	
	m_TodayWeather = WEATHER_CLEAR;
	m_Probability = 0;
	m_CurrentWeather = WEATHER_CLEAR; 
	m_WeatherLevel = 0;

	m_NextWeatherChangingTime = m_Tomorrow;
	m_NextLightning = m_Tomorrow;

	/*
	if (m_TodayWeather == WEATHER_CLEAR) 
	{
		// ���� ���� ������ ����.
		m_Probability = 0;
		m_CurrentWeather = WEATHER_CLEAR; 
		m_WeatherLevel = 0;

		m_NextWeatherChangingTime = m_Tomorrow;
		m_NextLightning = m_Tomorrow;
	}
	else 
	{
		m_Probability = Dice(3,100) / 3;		// �� ���� �� Ȯ��
		m_CurrentWeather = (Dice(1,100) < m_Probability) ? m_TodayWeather : WEATHER_CLEAR ;
		m_WeatherLevel = (m_CurrentWeather != WEATHER_CLEAR) ? Dice(3,20) / 3 : 0;

		// ��/���� �ִ� 20 ��(�ǽð�)���� ������.
		// ����, ���� ���� ���� �ð��� ���� �ð� + (1-20min)* 60sec �İ� �ȴ�.
		// �ϴ� �׽�Ʈ�� ���ؼ� �ֱ⸦ ª��
		//m_NextWeatherChangingTime = time(0) + Dice(1,20)* 60;
		m_NextWeatherChangingTime = time(0) + 60;	

		// �ϴ� �׽�Ʈ�� ���ؼ� �ֱ⸦ ª��
		//m_NextLightning = time(0) + 60;
		m_NextLightning = time(0) + 10;
	}
	*/

	//--------------------------------------------------------------------------------
	// ������ ���� ���� ��ӱ� ����, ���� ����ð��� �����Ѵ�.
	//--------------------------------------------------------------------------------
	DarkLightInfo* pDIInfo = g_pDarkLightInfoManager->getCurrentDarkLightInfo( m_pZone );
	m_pZone->setDarkLevel(pDIInfo->getDarkLevel());
	m_pZone->setLightLevel(pDIInfo->getLightLevel());

	// ���� �ð����� ���� 10�д밡 ���ӽð� �� �� ������ ����Ѵ�.
	dSec = (9 - ltm.tm_min / 10)* 60 + (60 - ltm.tm_sec);
	m_Next10Min = currentTime + dSec / 5;

	__END_CATCH
}


//--------------------------------------------------------------------------------
// ���� �ð��� �Ǹ� ������ �˾Ƽ� �ٲ��ش�. ���� heartbeat ���� ȣ��Ǿ�� �Ѵ�.
//--------------------------------------------------------------------------------
void WeatherManager::heartbeat () 
	throw (Error)
{
	// ��� �ʵ尡 �ƴ϶�� �ƹ� �͵� �� �ʿ䰡 ����.
	if (m_pZone->getZoneType() != ZONE_NORMAL_FIELD) return;

	// PK ���� �ƹ��͵� �� �ʿ䰡 ����.
	if ( g_pPKZoneInfoManager->isPKZone( m_pZone->getZoneID() ) )
		return;

	time_t currentTime = time(0);

	//--------------------------------------------------------------------------------
	// �Ϸ簡 ������ ���, ������ ������ �ٲٰ�, ������ Ȯ���� ����Ѵ�.
	// ������ ������ �ٲ����, m_NextWeatherChangingTime �� �Ѿ��
	// �ʾҴٸ� ������ �ٲ��� �ʴ´ٴ� �� ������ ��.
	//--------------------------------------------------------------------------------
	if (currentTime > m_Tomorrow) 
	{
		// ������ ������� �˱� ���ؼ�, GameTime ��ü�� �����´�.
		GameTime gametime = g_pTimeManager->getGameTime();

		// �̹����� ���� ������ �޾ƿ´�.
		const WeatherInfo & weatherInfo = g_pWeatherInfoManager->getWeatherInfo(gametime.getMonth());

		// ���̽��� ������, ������ ������ �����Ѵ�.
		m_TodayWeather = weatherInfo.getWeather(Dice(1,100));

		// ��/���� �� Ȯ���� �����Ѵ�.
		// ���� ������ ������������ð��� �����߸� ����ȴ�.
		// ���� ���⼭ �������� �ʿ�� ����.
		if (m_TodayWeather == WEATHER_CLEAR) 
		{
			// ���� ���� ������ ����.
			m_Probability = 0;
		} 
		else 
		{
			m_Probability = Dice(3,100) / 3;
		}

		// ���ӽð��� time_t �� �޾ƿ´�.
		time_t gmtime = g_pTimeManager->getgametime();

		// tm ��Ʈ��ó�� ��ȯ, ��/��/�� ���� �˾Ƴ���.
		tm ltm;
		localtime_r( &gmtime, &ltm );
		//struct tm* ptm = localtime(&gmtime);

		// ���� �ð����� ���ϱ��� ���� �ʸ� ����Ѵ�.
		int dSec = (23 - ltm.tm_hour)* 3600 + (59 - ltm.tm_min)* 60 + (59 - ltm.tm_sec);

		// (���� ���� �ð� / 5) �� ���� �ǽð��� ���ϸ�, ������ ���� �ð��� ���´�.
		m_Tomorrow = currentTime + dSec / 5;
	}

	//--------------------------------------------------------------------------------
	// ������ �ð��� �ʰ��� ���, ������ �ٲ���� �Ѵ�.
	//--------------------------------------------------------------------------------
	if (currentTime > m_NextWeatherChangingTime) 
	{
		if (m_TodayWeather == WEATHER_CLEAR) 
		{
			// ���� ���� ���, ���ϱ��� �ƹ��� ���� ��ȭ�� �߻����� �ʴ´�.
			m_CurrentWeather = WEATHER_CLEAR;
			m_WeatherLevel = 0;
			m_NextWeatherChangingTime = m_Tomorrow;
			m_NextLightning = m_Tomorrow / 2;
		} 
		else 
		{
			m_CurrentWeather = (Dice(1,100) < m_Probability) ? m_TodayWeather : WEATHER_CLEAR ;
			m_WeatherLevel = (m_CurrentWeather!=WEATHER_CLEAR) ? Dice(3,20) / 3 : 0;

			// �ϴ� �׽�Ʈ�� ���ؼ� �ֱ⸦ ª��
			//m_NextWeatherChangingTime = time(0) + Dice(1,20)* 60;
			m_NextWeatherChangingTime = time(0) + 60;

			// �ϴ� �׽�Ʈ�� ���� �ֱ⸦ ª��
			//m_NextLightning = time(0) + 60;
			m_NextLightning = time(0) + 20;
		}

		GCChangeWeather gcChangeWeather;
		gcChangeWeather.setWeather(m_CurrentWeather);
		gcChangeWeather.setWeatherLevel(m_WeatherLevel);

		StringStream msg;
		msg << "ZONE[" << m_pZone->getZoneID() << "] : " << gcChangeWeather.toString();
		log(LOG_DEBUG_MSG, "", "", msg.toString());

		m_pZone->broadcastPacket(&gcChangeWeather , NULL);
	}

	//--------------------------------------------------------------------------------
	// ������ ������ '��' �̸鼭, ������ �ð��� �ʰ��� ���, ���� üũ�� ���ش�.
	//--------------------------------------------------------------------------------
	if (m_CurrentWeather == WEATHER_RAINY && currentTime > m_NextLightning) 
	{
		// 1d100 ���̽��� ������, (���� ����* 5 - 30) ���� �۴ٸ�
		// ������ ģ ������ �����Ѵ�. ���� ������ �ִ� 20 �̹Ƿ�,
		// Ȯ���� �ִ� 70% ���� ������ �� �ִ�.
		// �ϴ� ������ �� ���� ������ ġ���� �Ѵ�..
		if (Dice(1 , 100) < (uint)max(0 , m_WeatherLevel* 5 - 30)) 
		{
			GCLightning gcLightning;
			gcLightning.setDelay(Dice(1,5));
			m_pZone->broadcastPacket(&gcLightning , NULL);
		}

		// ���� ���� üũ �ð��� �����Ѵ�.
		// (������ ���� ��쿡�� ���������� �ʿ����� ����.)
		m_NextLightning += 60;
	}

	//--------------------------------------------------------------------------------
	// ���� �ð� 10�и��� ���� ���� ��ӱ� ������ �ٲ��ش�.
	//--------------------------------------------------------------------------------
	if (currentTime > m_Next10Min) 
	{
		DarkLightInfo* pDIInfo = g_pDarkLightInfoManager->getCurrentDarkLightInfo( m_pZone );

		DarkLevel_t darkLevel = pDIInfo->getDarkLevel();
		LightLevel_t lightLevel = pDIInfo->getLightLevel();

		// Dark veya light degistiyse referanstaki gibi raw degerleri broadcast et.
		if (darkLevel != m_pZone->getDarkLevel() || lightLevel != m_pZone->getLightLevel())
		{
			m_pZone->setDarkLevel(darkLevel);
			m_pZone->setLightLevel(lightLevel);

			GCChangeDarkLight gcChangeDarkLight;
			gcChangeDarkLight.setDarkLevel(darkLevel);
			gcChangeDarkLight.setLightLevel(lightLevel);

			// Vampire invert: DarkLightInfo gece DarkLevel=14/15 olabilir.
			// Clamp yoksa (DARK_MAX=13)-15 BYTE underflow (254) → client
			// SetDarkBits>=16 reject → Vampire gündüz karanlığında takılır.
			// fillCreatureDarkLight / PacketUtil ile aynı clamp.
			GCChangeDarkLight gcChangeDarkLight2;
			gcChangeDarkLight2.setDarkLevel(
				(DarkLevel_t)max(0, (int)DARK_MAX - (int)darkLevel));
			gcChangeDarkLight2.setLightLevel(
				(LightLevel_t)min(13, (int)LIGHT_MAX - (int)lightLevel));

			m_pZone->broadcastDarkLightPacket(&gcChangeDarkLight, &gcChangeDarkLight2, NULL);

			//cout << "(DarkLevel/LightLevel) : (" << (int)darkLevel << "," << (int)lightLevel << ") at " << g_pTimeManager->getGameTime().toString() << endl;
		}
		else
		{
			//cout << "(DarkLevel/LightLevel) : (" << (int)darkLevel << "," << (int)lightLevel << ") at " << g_pTimeManager->getGameTime().toString() << endl;
		}

		m_Next10Min += 120;
	}
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string WeatherManager::toString () const 
	throw ()
{
	StringStream msg;

	msg << "WeatherManager(";
	msg << "    ������ ���� : " << Weather2String[ m_TodayWeather ];
	msg << "      ���� ���� : " << Weather2String[ m_CurrentWeather ];
	msg << "��/���� �� Ȯ�� : " << (long)m_Probability << "%";
	msg << "      ���� ���� : " << (long)m_WeatherLevel;

	time_t currentTime = time(0);

	msg << "       ����ð� : " << (long)ctime(&currentTime);
	msg << "  ���ӻ��� ���� : " << (long)ctime(&m_Tomorrow);
	msg << "������������ð�: " << (long)ctime(&m_NextWeatherChangingTime);
	msg << "��������üũ�ð�: " << (long)ctime(&m_NextLightning);

	return msg.toString();
}
