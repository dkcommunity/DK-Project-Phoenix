#include "Packet/Cpackets/CGBuyTmallGoods.h"
#include "Client_PCH.h"

void CGBuyTmallGoods::read(SocketInputStream &iStream) throw(ProtocolException,
                                                             Error) {
  __BEGIN_TRY

  iStream.read(m_GoodsID);
  iStream.read(m_Kind);

  __END_CATCH
}

void CGBuyTmallGoods::write(SocketOutputStream &oStream) const
    throw(ProtocolException, Error) {
  __BEGIN_TRY

  oStream.write(m_GoodsID);
  oStream.write(m_Kind);

  __END_CATCH
}

void CGBuyTmallGoods::execute(Player *pPlayer) throw(ProtocolException, Error) {
  __BEGIN_TRY

  CGBuyTmallGoodsHandler::execute(this, pPlayer);

  __END_CATCH
}

string CGBuyTmallGoods::toString() const throw() {
  __BEGIN_TRY

  StringStream msg;
  msg << "CGBuyTmallGoods("
      << ")";
  return msg.toString();

  __END_CATCH
}
