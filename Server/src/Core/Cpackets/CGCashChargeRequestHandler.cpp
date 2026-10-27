//////////////////////////////////////////////////////////////////////////////
// Filename    : CGCashChargeRequestHandler.cpp
// Description : PayPal Cash yükleme isteği handler'ı.
//               Oyuncuyu doğrular, 32-hex tek-kullanımlık token üretir,
//               PayPalTransactions'a PENDING kayıt atar (paket web sayfasında
//               seçileceği için PackageID NULL) ve GCCashChargeURL döner.
//               Tasarım: Dev_Log/Server/paypal_cash_charge_system_design_2026-08-20.md
//////////////////////////////////////////////////////////////////////////////

#include "CGCashChargeRequest.h"

#ifdef __GAME_SERVER__
	#include "Assert.h"
	#include "GamePlayer.h"
	#include "PlayerCreature.h"
	#include "DB.h"
	#include "Properties.h"

	#include "Gpackets/GCCashChargeURL.h"

	#include <cstdlib>	// rand()
#endif	// __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGCashChargeRequestHandler::execute (CGCashChargeRequest* pPacket , Player* pPlayer)
	 throw (Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert(pGamePlayer != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
	if ( pPC == NULL )
	{
		// Sessiz yutma yok: logla + client'a hata dön
		filelog("PayPalCash.log", "[CGCashChargeRequestHandler] NULL PlayerCreature, player=%s", pGamePlayer->getID().c_str());

		GCCashChargeURL gcError;
		gcError.setResult( CASH_CHARGE_RESULT_ERROR );
		gcError.setURL( "" );
		pGamePlayer->sendPacket( &gcError );
		return;
	}

	// gameserver.conf'tan PayPal web endpoint base URL'i.
	// Key tanımlı değilse placeholder default kullanılır —
	// ClientLoginServerIP örüntüsü (LGIncomingConnectionOKHandler.cpp:100).
	string baseURL = "https://CHANGEME.local";
	if ( g_pConfig != NULL && g_pConfig->hasKey("PayPalEndpointBaseURL") )
	{
		baseURL = g_pConfig->getProperty("PayPalEndpointBaseURL");
	}

	// Çift slash üretmemek için sondaki slash'ları at: <base>/pay/?token=<token>
	while ( !baseURL.empty() && baseURL[ baseURL.size() - 1 ] == '/' )
	{
		baseURL.erase( baseURL.size() - 1 );
	}

	// Tek kullanımlık 32-hex token — WebMarket one-time-key örüntüsü
	// (CGRequestWebMarketHandler.cpp:45). Kod tabanında kripto random yok;
	// srand() main.cpp:96'da çağrılıyor, glibc rand() 31-bit döner (7 hex/çağrı).
	char szToken[33];
	static const char szHexDigits[] = "0123456789abcdef";
	unsigned int uRand = 0;
	for ( int i = 0; i < 32; i++ )
	{
		if ( i % 7 == 0 )
		{
			uRand = (unsigned int)rand();
		}
		szToken[i] = szHexDigits[ uRand & 0xF ];
		uRand >>= 4;
	}
	szToken[32] = '\0';

	const string safeAccount = ReplaceDBString( pGamePlayer->getID() );
	const string safeName    = ReplaceDBString( pPC->getName() );

	// Başarısız varsayılır; PENDING insert başarılı olursa OK işaretlenir.
	BYTE   result = CASH_CHARGE_RESULT_ERROR;
	string url;

	Statement* pStmt   = NULL;
	Result*    pResult = NULL;

	try
	{
		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getDistConnection("PLAYER_DB")->createStatement();

			// Spam koruması: süresi dolmamış PENDING kaydı 5+ ise reddet (tasarım §4 adım 2)
			pResult = pStmt->executeQuery(
				"SELECT COUNT(*) FROM PayPalTransactions WHERE Account='%s' AND Status='PENDING' AND KeyExpiresAt > NOW()",
				safeAccount.c_str() );

			int pendingCount = 0;
			if ( pResult != NULL && pResult->next() )
			{
				pendingCount = pResult->getInt(1);
			}

			if ( pendingCount >= 5 )
			{
				filelog("PayPalCash.log", "[CGCashChargeRequestHandler] too many PENDING rows, player=%s count=%d", safeAccount.c_str(), pendingCount);
			}
		else
		{
			// Paket web sayfasında seçilecek — PackageID/HowMuchPay/CashAmount
			// checkout sırasında web endpoint'i tarafından snapshot'lanır.
			// PlayerID == Account: canlı DB'de Player.PlayerID == userinfo.username
			// doğrulandı (string hesap id'si); kullanıcı şartı gereği PlayerID
			// kolonu ayrıca ve aynı değerle yazılır (2026-08-20 revizyonu).
			pStmt->executeQuery(
				"INSERT INTO PayPalTransactions (Account, PlayerID, CharName, PackageID, Status, OneTimeKey, KeyExpiresAt, CreatedAt) "
				"VALUES ('%s', '%s', '%s', NULL, 'PENDING', '%s', DATE_ADD(NOW(), INTERVAL 15 MINUTE), NOW())",
				safeAccount.c_str(),
				safeAccount.c_str(),
				safeName.c_str(),
				szToken );

				if ( pStmt->getAffectedRowCount() == 1 )
				{
					result = CASH_CHARGE_RESULT_OK;
					url    = baseURL + "/pay/?token=" + szToken;
				}
				else
				{
					filelog("PayPalCash.log", "[CGCashChargeRequestHandler] PENDING insert affected=%u, player=%s", pStmt->getAffectedRowCount(), safeAccount.c_str());
				}
			}

			SAFE_DELETE( pStmt );
		}
		END_DB( pStmt )
	}
	catch ( SQLQueryException& sqe )
	{
		// Sessiz yutma yok: hata loglanır, client'a result=1 + boş URL döner
		filelog("PayPalCash.log", "[CGCashChargeRequestHandler] DB error, player=%s : %s", safeAccount.c_str(), sqe.toString().c_str());
	}

	// URL BYTE-uzunluklu string ile gidiyor — 255'i aşarsa client'a taşınamaz
	if ( result == CASH_CHARGE_RESULT_OK && url.size() > 255 )
	{
		filelog("PayPalCash.log", "[CGCashChargeRequestHandler] URL too long (%u bytes), player=%s", (unsigned int)url.size(), safeAccount.c_str());
		result = CASH_CHARGE_RESULT_ERROR;
		url    = "";
	}

	GCCashChargeURL gcCashChargeURL;
	gcCashChargeURL.setResult( result );
	gcCashChargeURL.setURL( url );
	pGamePlayer->sendPacket( &gcCashChargeURL );

#else
	(void)pPacket;
	(void)pPlayer;
#endif	// __GAME_SERVER__

	__END_DEBUG_EX __END_CATCH
}
