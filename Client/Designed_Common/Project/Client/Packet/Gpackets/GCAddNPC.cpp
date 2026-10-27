//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddNPC.cc
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GCAddNPC.h"
#include "Client_PCH.h"


//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
GCAddNPC::GCAddNPC()

{
  __BEGIN_TRY

  m_pEffectInfo = NULL;

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////
GCAddNPC::~GCAddNPC() {
  __BEGIN_TRY

  if (m_pEffectInfo != NULL) {
    delete m_pEffectInfo;
    m_pEffectInfo = NULL;
  }

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
//////////////////////////////////////////////////////////////////////
void GCAddNPC::read(SocketInputStream &iStream)

{
  __BEGIN_TRY

  int localLongName = 0;
#if __CONTENTS(__LOCALIZING_LONGNAME)
  localLongName = 12;
#endif //__LOCALIZING_LONGNAME

  // 최적화 작업시 실제 크기를 명시하도록 한다.
  iStream.read(m_ObjectID);

  BYTE szName;

  iStream.read(szName);

  if (szName == 0)
    throw InvalidProtocolException("szName == 0");

  if (szName > 100 + localLongName)
    throw InvalidProtocolException("too large name length");

  iStream.read(m_Name, szName);
  iStream.read(m_NPCID);

  iStream.read(m_SpriteType);

  iStream.read(m_MainColor);
  iStream.read(m_SubColor);

  iStream.read(m_X);
  iStream.read(m_Y);
  iStream.read(m_Dir);

  // 메네그로스 던전에 엔피시에 이펙트를 붙이면서 처리를 정확하기 위해서...
#if __CONTENTS(__MENEGROTH)
  if (m_pEffectInfo == NULL) m_pEffectInfo = new EffectInfo();
  m_pEffectInfo->read(iStream);
#endif //__MENEGROTH
  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
//////////////////////////////////////////////////////////////////////
void GCAddNPC::write(SocketOutputStream &oStream) const

{
  __BEGIN_TRY
  int localLongName = 0;
#if __CONTENTS(__LOCALIZING_LONGNAME)
  localLongName = 12;
#endif //__LOCALIZING_LONGNAME
       // 최적화 작업시 실제 크기를 명시하도록 한다.
  oStream.write(m_ObjectID);

  BYTE szName = m_Name.size();

  if (szName == 0)
    throw InvalidProtocolException("szName == 0");

  if (szName > 100 + localLongName)
    throw InvalidProtocolException("too large name length");

  oStream.write(szName);
  oStream.write(m_Name);
  oStream.write(m_NPCID);

  oStream.write(m_SpriteType);

  oStream.write(m_MainColor);
  oStream.write(m_SubColor);

  oStream.write(m_X);
  oStream.write(m_Y);
  oStream.write(m_Dir);

#if __CONTENTS(__MENEGROTH)
  if (m_pEffectInfo != NULL) m_pEffectInfo->write(oStream);
#endif //__MENEGROTH

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCAddNPC::execute(Player *pPlayer)

{
  __BEGIN_TRY

  GCAddNPCHandler::execute(this, pPlayer);

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// get packet's debug std::string
//
//////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
std::string GCAddNPC::toString() const

{
  __BEGIN_TRY

  StringStream msg;

  msg << "GCAddNPC("
      << "ObjectID:" << m_ObjectID << ",Name:" << m_Name << ",NPCID:" << m_NPCID
      << ",SpriteType:" << m_SpriteType << ",MainColor:" << m_MainColor
      << ",SubColor:" << m_SubColor << ",X:" << (int)m_X << ",Y:" << (int)m_Y
      << ",Dir: " << (int)m_Dir
#if __CONTENTS(__MENEGROTH)
      << ",EffectInfo: " << (int)m_pEffectInfo
#endif //__MENEGROTH
      << ")";

  return msg.toString();

  __END_CATCH
}
#endif
