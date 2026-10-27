// =============================================================================
//  NMCrypt_Stub.cpp — Project Phoenix NMCrypt replacement
//  -----------------------------------------------------------------------------
//  Orijinal NMCrypt.lib, Netmarble (Kore oyun portal'ı) için kapalı kaynak
//  bir şifreleme/SSO clipboard yardımcı kütüphanesiydi. Kaynak yok, binary
//  yalnızca Referans Proje/.../Designed_Common/Lib/NMCrypt.lib olarak mevcut.
//
//  Netmarble entegrasyonu zaten FAZ 0.3'te devre dışı bırakılacak (silme yok);
//  o adıma kadar Netmarble.cpp linkleniyor ve bu fonksiyonları çağırıyor.
//  Dolayısıyla Client.exe link aşamasını geçsin diye burada minimum no-op
//  stub'lar veriyoruz. Gerçek davranış yok — Netmarble yolu çağrılırsa
//  CrashTrace kancası geri dönüyor, fonksiyonel sonuç hata olacak şekilde
//  ayarlı (Netmarble'ın başarısız olması bekleniyor).
//
//  TODO(FAZ 0.3): Netmarble.cpp devre dışı bırakılınca bu dosya da kaldırılabilir.
// =============================================================================

#include "../../Project/Client/NMCrypt.h"
#include "../../Project/Client/crash_trace_tool.h"

#include <cstring>

// -----------------------------------------------------------------------------
// EncryptString — orijinali plain text'i key ile şifreleyip target'a yazardı.
// Stub: input'u aynen kopyalar (no-op). Gerçek kripto yok.
// -----------------------------------------------------------------------------
void EncryptString(const char* psSource, const char* psKey, char* psTarget)
{
	CT_Done("NMCrypt stub: EncryptString cagrildi (no-op)");
	if (psTarget == nullptr) {
		return;
	}
	if (psSource != nullptr) {
		std::strcpy(psTarget, psSource);
	} else {
		psTarget[0] = '\0';
	}
	(void)psKey;
}

// -----------------------------------------------------------------------------
// DecryptString — orijinali cipher'ı key ile deşifre ederdi.
// Stub: CRYPT_INPUT_FAIL döndürür → çağıran kod "başarısız" yolunu seçer.
// Bu Netmarble login akışının zaten kullanılmamasını sağlar (FAZ 0.3 hedefi).
// -----------------------------------------------------------------------------
int DecryptString(const char* psSource, const char* psKey, char* psTarget)
{
	CT_Done("NMCrypt stub: DecryptString cagrildi, CRYPT_INPUT_FAIL donuyor");
	(void)psSource;
	(void)psKey;
	if (psTarget != nullptr) {
		psTarget[0] = '\0';
	}
	return CRYPT_INPUT_FAIL;
}

// -----------------------------------------------------------------------------
// SetNMClipData — orijinali clipboard'a şifreli SSO verisini yazardı.
// Stub: false → başarısız. Netmarble clipboard SSO zaten kullanılmıyor.
// -----------------------------------------------------------------------------
bool SetNMClipData(const char* str, const char* key)
{
	CT_Done("NMCrypt stub: SetNMClipData cagrildi (no-op, false)");
	(void)str;
	(void)key;
	return false;
}

// -----------------------------------------------------------------------------
// GetNMClipData — orijinali clipboard'daki şifreli SSO verisini okurdu.
// Stub: false → başarısız. Netmarble login bypass edilir.
// -----------------------------------------------------------------------------
bool GetNMClipData(char* pstr, unsigned int maxlen, const char* key, bool bClearData)
{
	CT_Done("NMCrypt stub: GetNMClipData cagrildi (no-op, false)");
	(void)key;
	(void)bClearData;
	if (pstr != nullptr && maxlen > 0) {
		pstr[0] = '\0';
	}
	return false;
}
