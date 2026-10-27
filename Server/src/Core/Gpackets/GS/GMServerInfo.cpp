//////////////////////////////////////////////////////////////////////
// 
// Filename    : GMServerInfo.cpp 
// Written By  : reiot@ewestsoft.com
// Description : 
// 
//////////////////////////////////////////////////////////////////////

// include files
#include "GMServerInfo.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
GMServerInfo::GMServerInfo () 
     throw ()
{
	__BEGIN_TRY

	m_ZoneCount = 0;

	__END_CATCH
}
	
//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GMServerInfo::~GMServerInfo () 
    throw ()
{
	__BEGIN_TRY

	m_ZoneUserList.clear();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GMServerInfo::execute ( Player * pPlayer ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
	GMServerInfoHandler::execute( this );
		
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////////////
void GMServerInfo::read ( Datagram & iDatagram ) 
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY

//	cout << " GMServerInfo::read " << endl;
	iDatagram.read(m_WorldID);
	iDatagram.read(m_ServerID);

	ZONEUSERDATA zoneuser_data;

	iDatagram.read(m_ZoneCount);
//	cout << " Read m_ZoneCount : " <<(WORD)m_ZoneCount << endl;


	for (WORD s=0; s<m_ZoneCount; s++)
	{
		iDatagram.read(zoneuser_data.ZoneID);
		iDatagram.read(zoneuser_data.UserNum);


        //Read Race Count 20071210 kim sung yong
	    iDatagram.read(zoneuser_data.RaceNum[0]);
		iDatagram.read(zoneuser_data.RaceNum[1]);
        iDatagram.read(zoneuser_data.RaceNum[2]);

//      cout<< "Reading RaceNum[0] : " << zoneuser_data.RaceNum[0] << "   Reading RaceNum[1] : " << zoneuser_data.RaceNum[1] << "   Reading RaceNum[2] : " << zoneuser_data.RaceNum[2] << endl;

		m_ZoneUserList.push_back(zoneuser_data);
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////////////
void GMServerInfo::write ( Datagram & oDatagram ) 
     const throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	oDatagram.write(m_WorldID);
	oDatagram.write(m_ServerID);

	oDatagram.write(m_ZoneCount);
//	cout << " Write m_ZoneCount : " << (WORD)m_ZoneCount <<endl;
	
	list<ZONEUSERDATA>::const_iterator zoneuser_itr = m_ZoneUserList.begin();
	for (; zoneuser_itr != m_ZoneUserList.end(); zoneuser_itr++)
	{
		ZONEUSERDATA zoneuser_data = *zoneuser_itr;
		oDatagram.write(zoneuser_data.ZoneID);
		oDatagram.write((WORD)zoneuser_data.UserNum);

        //Write Race Count 20071210 kim sung yong
        oDatagram.write((WORD)zoneuser_data.RaceNum[0]);
		oDatagram.write((WORD)zoneuser_data.RaceNum[1]);
        oDatagram.write((WORD)zoneuser_data.RaceNum[2]);
//        cout<< "Write RaceNum[0] : " << zoneuser_data.RaceNum[0] << "   Write RaceNum[1] : " << zoneuser_data.RaceNum[1] << "   Write RaceNum[2] : " << zoneuser_data.RaceNum[2] << endl;
	}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string GMServerInfo::toString () 
	const throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "[GMServerInfo]"
		<< "ZoneNum:" << (int)m_ZoneCount
		<< ", ZoneUser(";

	int total = 0;

	// 성서전 참여 인원 표시용
	int RaceNumForHolyWar[3] = {0,};
	int TotalNumForHolyWar = 0;

	list<ZONEUSERDATA>::const_iterator zoneuser_itr = m_ZoneUserList.begin();
	for (; zoneuser_itr != m_ZoneUserList.end(); zoneuser_itr++)
	{
		ZONEUSERDATA zoneuser_data = *zoneuser_itr;
		msg << (int)zoneuser_data.ZoneID << ":"
			<< (int)zoneuser_data.UserNum << ", "
			<< (int)zoneuser_data.RaceNum[0] << ", "
			<< (int)zoneuser_data.RaceNum[1] << ", "
			<< (int)zoneuser_data.RaceNum[2] << ", ";
				
	    total += zoneuser_data.UserNum;

		switch( zoneuser_data.ZoneID)
		{
			case 75 :
			case 1201 :
			case 1202 :
			case 1203 :
			case 1204 :
			case 71 :
			case 72 :
			case 73 :
			case 74 :
			case 1205 :
			case 1206 :
				RaceNumForHolyWar[0] += zoneuser_data.RaceNum[0];
				RaceNumForHolyWar[1] += zoneuser_data.RaceNum[1];
				RaceNumForHolyWar[2] += zoneuser_data.RaceNum[2];

				TotalNumForHolyWar += zoneuser_data.UserNum;
				break;
			default :
				break;
		}

	}

	msg << "), Total( " << total << " )";

	msg << "\n";
	msg << "[HolyWarRaceNum]" 
		<< "TotalNum : "
		<< TotalNumForHolyWar
		<< ",Slayer : "
		<< RaceNumForHolyWar[0]
		<< ",Vampire : "
		<< RaceNumForHolyWar[1]
		<< ",Ousters : "
		<< RaceNumForHolyWar[2];
		 


	return msg.toString();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void GMServerInfo::addZoneUserData(ZoneID_t ZoneID, DWORD UserNum, vector<uint> RaceNum )
	    throw()
{
	__BEGIN_TRY

	ZONEUSERDATA zoneuser_data;
	zoneuser_data.ZoneID = ZoneID;
	zoneuser_data.UserNum = UserNum;

    //Race Count 20071210 kim sung yong
    zoneuser_data.RaceNum[0] = RaceNum[0];
//    cout << "addZoneUserData()... RaceNum[0] val : " << RaceNum[0] << endl;
	zoneuser_data.RaceNum[1] = RaceNum[1];
//    cout << "addZoneUserData()... RaceNum[1] val : " << RaceNum[1] << endl;
    zoneuser_data.RaceNum[2] = RaceNum[2];
//    cout << "addZoneUserData()... RaceNum[2] val : " << RaceNum[2] << endl;

	m_ZoneUserList.push_back(zoneuser_data);

	m_ZoneCount++;

//	cout << " GMServerInfo::addZoneUserData ZoneCount : " << (WORD)m_ZoneCount << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void GMServerInfo::popZoneUserData(ZONEUSERDATA& rData)
	    throw()
{
	__BEGIN_TRY

	ZONEUSERDATA zoneuser_data = m_ZoneUserList.front();

	rData.ZoneID = zoneuser_data.ZoneID;
	rData.UserNum = zoneuser_data.UserNum;

	rData.RaceNum[0] = zoneuser_data.RaceNum[0];
	rData.RaceNum[1] = zoneuser_data.RaceNum[1];
	rData.RaceNum[2] = zoneuser_data.RaceNum[2];


	m_ZoneUserList.pop_front();
	m_ZoneCount--;

	__END_CATCH
}
