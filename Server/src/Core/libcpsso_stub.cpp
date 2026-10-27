// FAZ 4 - libcpsso stub: orijinal Korean operator CP SSO library yok.
// Bu fonksiyon CLLogin.cpp icinde sadece m_Cpsso (cookie) alanini parse etmek
// icin cagriliyor. Modern login flow'unda CP cookie kullanmadigimiz icin
// her zaman false donduruyoruz — login bu path'ten gecmesin, normal flow calissin.

extern "C" {
bool GetCPCookieValue(const char* szCpCookie, const char* szVar, char* pstrRtnValue)
{
    if (pstrRtnValue) pstrRtnValue[0] = 0;
    return false;
}
}
