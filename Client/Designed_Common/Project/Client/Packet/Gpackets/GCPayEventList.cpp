#include "Packet/Gpackets/GCPayEventList.h"
#include "Client.h"
#include "Client_PCH.h"
#include "DarkAssert.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

GCPayEventList::GCPayEventList()

{
  __BEGIN_TRY

  m_ItemList.clear();

  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
GCPayEventList::~GCPayEventList() {
  __BEGIN_TRY

  std::list<PayEventInfo *>::iterator itr = m_ItemList.begin();
  std::list<PayEventInfo *>::iterator endItr = m_ItemList.end();

  for (; itr != endItr; ++itr) {
    if (*itr != NULL)
      SAFE_DELETE(*itr);
  }

  m_ItemList.clear();

  __END_CATCH
}

void GCPayEventList::read(SocketInputStream &iStream)

{
  __BEGIN_TRY

  BYTE totalNum;
  iStream.read(totalNum);
  if (totalNum > MAX_PAY_EVENT_LIST)
    throw DisconnectException("GCPayEventList : read error.");

  for (int i = 0; i < totalNum; ++i) {
    PayEventInfo *pGI = new PayEventInfo;

    iStream.read(pGI->objectID);
    iStream.read(pGI->itemClass);
    iStream.read(pGI->itemType);
    iStream.read(pGI->grade);

    BYTE optionNum;
    iStream.read(optionNum);

    pGI->optionType.clear();

    for (int j = 0; j < optionNum; ++j) {
      OptionType_t optionType;
      iStream.read(optionType);
      pGI->optionType.push_back(optionType);
    }

    iStream.read(pGI->num);
    iStream.read(pGI->timeLimit);

    iStream.read(pGI->ImageID);

    addPayEventInfo(pGI);
  }

  iStream.read(m_Count20);
  iStream.read(m_Count50);
  iStream.read(m_Count100);
  iStream.read(m_Count200);
  iStream.read(m_Count500);
  iStream.read(m_Count1000);
  iStream.read(m_Count2000);
  iStream.read(m_Count5000);

  iStream.read(m_Status20);
  iStream.read(m_Status50);
  iStream.read(m_Status100);
  iStream.read(m_Status200);
  iStream.read(m_Status500);
  iStream.read(m_Status1000);
  iStream.read(m_Status2000);
  iStream.read(m_Status5000);

  __END_CATCH
}

void GCPayEventList::write(SocketOutputStream &oStream) const

{
  __BEGIN_TRY
  __BEGIN_DEBUG

  BYTE totalNum = m_ItemList.size();
  if (totalNum > MAX_PAY_EVENT_LIST)
    throw DisconnectException("GCPayEventList :write error.");

  oStream.write(totalNum);

  std::list<PayEventInfo *>::const_iterator itr = m_ItemList.begin();
  std::list<PayEventInfo *>::const_iterator endItr = m_ItemList.end();

  for (; itr != endItr; ++itr) {
    PayEventInfo *pGI = *itr;
    Assert(pGI != NULL);

    oStream.write(pGI->objectID);
    oStream.write(pGI->itemClass);
    oStream.write(pGI->itemType);
    oStream.write(pGI->grade);

    BYTE optionNum = pGI->optionType.size();
    oStream.write(optionNum);

    std::list<OptionType_t>::const_iterator oitr = pGI->optionType.begin();
    std::list<OptionType_t>::const_iterator endoItr = pGI->optionType.end();

    for (; oitr != endoItr; ++oitr) {
      oStream.write(*oitr);
    }

    oStream.write(pGI->num);
    oStream.write(pGI->timeLimit);

    oStream.write(pGI->ImageID);
  }

  oStream.write(m_Count20);
  oStream.write(m_Count50);
  oStream.write(m_Count100);
  oStream.write(m_Count200);
  oStream.write(m_Count500);
  oStream.write(m_Count1000);
  oStream.write(m_Count2000);
  oStream.write(m_Count5000);

  oStream.write(m_Status20);
  oStream.write(m_Status50);
  oStream.write(m_Status100);
  oStream.write(m_Status200);
  oStream.write(m_Status500);
  oStream.write(m_Status1000);
  oStream.write(m_Status2000);
  oStream.write(m_Status5000);

  __END_DEBUG
  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCPayEventList::execute(Player *pPlayer)

{
  __BEGIN_TRY
  __BEGIN_DEBUG

  GCPayEventListHandler::execute(this, pPlayer);

  __END_DEBUG
  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
PacketSize_t GCPayEventList::getPacketSize() const

{
  __BEGIN_TRY
  __BEGIN_DEBUG

  PacketSize_t size = szBYTE;

  std::list<PayEventInfo *>::const_iterator itr = m_ItemList.begin();
  std::list<PayEventInfo *>::const_iterator endItr = m_ItemList.end();

  for (; itr != endItr; ++itr) {
    size += (*itr)->getPacketSize();
  }
  size += szObjectID + szObjectID + szObjectID + szObjectID + szObjectID +
          szObjectID + szObjectID + szObjectID + szObjectID + szObjectID +
          szObjectID + szObjectID + szObjectID + szObjectID + szObjectID +
          szObjectID;
  return size;

  __END_DEBUG
  __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get packet's debug std::string
//////////////////////////////////////////////////////////////////////////////
std::string GCPayEventList::toString() const

{
  __BEGIN_TRY

  StringStream msg;
  msg << "GCPayEventList(";

  std::list<PayEventInfo *>::const_iterator itr = m_ItemList.begin();
  std::list<PayEventInfo *>::const_iterator endItr = m_ItemList.end();

  for (; itr != endItr; ++itr) {
    msg << (*itr)->toString();
  }

  msg << ")";
  return msg.toString();

  __END_CATCH
}
