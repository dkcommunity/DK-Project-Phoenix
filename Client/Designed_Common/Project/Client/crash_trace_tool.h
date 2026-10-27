#ifndef __CRASH_TRACE_TOOL_H__
#define __CRASH_TRACE_TOOL_H__

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <share.h>
#include <string.h>

// ---------------------------------------------------------------------------
// 2026-05-18 KRITIK YAPI DUZELTMESI:
// ENABLED flag'leri INLINE BODY'DEN ONCE tanimlanmali. Eski yapida #ifndef
// guard'li define'lar dosyanin ALT kisminda kaliyordu -> preprocessor linear
// parse -> inline body line 25'teki `#if CT_TRACE_ENABLED || CT_DONE_ENABLED`
// evaluate edildiginde macro'lar HENUZ TANIMSIZ -> false -> body BOS kaliyordu.
// Sonuc: CT_Done(...) cagriliyordu ama CrashTrace_Impl body'si NO-OP idi
// (sadece `(void)format`). Disk log son aylardir hic yazilmamis olabilir.
//
// Asagidaki #ifndef + #define ALT kisimda da var (tarihce yorumlu) -- onlar
// `#ifndef` guard ile no-op (zaten burada tanimli). Asagiyi dokunmadik
// (yorumlari korumak icin).
// ---------------------------------------------------------------------------
#ifndef CT_TRACE_ENABLED
// 2026-05-20 KAPATILDI (debug bitti, kural 9.4): Client.exe erken exit teshisi
// tamamlandi. CT_Done/CrashTrace log'lari init flow'unu izledi. Production
// temizligi icin 0'a dondu. Tum call yerleri korundu (kural 4.1 silme yok).
#define CT_TRACE_ENABLED 0
#endif

#ifndef CT_DONE_ENABLED
// 2026-05-18 v5: 0 production (kural 9.4). Tum debug session'lari basari ile
// kapatildi:
//   * Domain Exp loader root cause: getProperty().c_str() dangling pointer
//   * Level-up text truncation root cause: race-specific strX too far right
//     (MTopView.cpp:12393 g_pGameMessage render block)
//   * EXP bar percentage display bug: raw goal_exp vs computed %
// Tum CT_Done call yerleri korundu (kural 4.1 silme yok), sadece flag 0.
// 2026-05-20 KAPATILDI (debug bitti, kural 9.4): Client.exe erken exit teshisi
// tamamlandi. Init flow log'lari analiz edildi. Production temizligi icin 0.
// 2026-09-01 GECICI 1 (kural 9.2/9.4): dx11gpu ilk kosuda siyah ekran —
// D3D11Backend/QuadPipeline/SpriteCommands init + Present akisi izlendi.
// Root cause: dxcpl.exe (DirectX Control Panel) HKCU\Software\Microsoft\
// Direct3D\Application0 kaydi FeatureLevelLimit=0x9100 (9_1) ile Client.exe
// path'ini kilitliyordu -> D3D11CreateDeviceAndSwapChain DXGI_ERROR_UNSUPPORTED.
// Kayit silindi, fix dogrulandi (FL 11_1) -> flag geri 0 (production).
// 2026-09-01 GECICI 1 (kural 9.2/9.4): 3 kullanici raporu teshisi —
//   1. Ouster MilitaryDlg skill popup LEARN butonu gorunmuyor
//      (OusterLearnBtn ENTER/EXIT izleri, VS_UI_GameCommon2.cpp)
//   2. Logout/Login sonrasi harita objeleri yuklenmiyor
//      (CTypePack "NOT initialized"/"split open failed" izleri)
//   3. Gamma/GammaBox fix dogrulamasi (D3D11Backend::SetGammaRamp izi)
// Teshis bitince flag geri 0'a cekilecek.
// 2026-09-01 GERI 0 (ayni gun): kullanici FPS 3-10'a dustu bildirdi —
// bilinen sebep (910 site x her frame disk I/O, bkz. yukaridaki tarihce).
// Gamma izi dogrulandi (SetGammaRamp step=97). Kalan 2 teshis (Ouster LEARN
// butonu + harita objeleri) izole CT_OBJ kategorisine tasindi (asagida) —
// ayri dosya, yalniz nadir/throttle'li noktalar, FPS etkisi yok.
// 2026-09-02 GECICI 1 -> GERI 0 (ayni gun, kural 9.2/9.4): fullscreen
// calismiyor raporu teshis edildi. Root cause: D3D11Backend::Init DXGI
// exclusive SetFullscreenState(TRUE) = 0x887A0022 NOT_CURRENTLY_AVAILABLE
// -> sessizce windowed devam. Fix: Reborn ApplyWindowMode paritesi
// (ChangeDisplaySettings CDS_FULLSCREEN + WS_POPUP borderless). Dogrulandi:
// "ApplyBorderlessFullscreen ON 1280x960 OK" + display mode 1280x960.
#define CT_DONE_ENABLED 0
#endif

// Safe header-only logger with file sharing support to prevent locking issues
//
// 2026-05-16 FPS FIX: Direkt CrashTrace_Impl(...) cagrilari (orn. MCreature.cpp
// MPlayer.cpp Bug3.b death instrumentation) define guard'lari bypass ediyordu.
// Her death event = _fsopen+fwrite+fflush+fclose disk I/O. CT_TRACE_ENABLED ve
// CT_DONE_ENABLED ikisi de 0 ise body'i bos birak — compile-time'da disk I/O
// hicbir cagri yerinde gerceklesmesin. (Macro guard cagri yerini, fonksiyon
// guard tum direkt _Impl cagrilarini kapsar.)
// 2026-05-18: `inline` -> `static inline` (TU-local). Phoenix VS_UI_GameCommon.cpp
// gibi dosyalar 4 farkli CMake target'inde derleniyor (Designed_Chs + Designed_Common
// her biri lib/exe variant) -> her .obj kendi CrashTrace_Impl emit ediyor -> linker
// /OPT:ICF comdat dedup -> eski body cache'ten seciliyor -> yeni edit'ler binary'e
// yansimamiyor (562 LNK4006 warning'i bu pattern'in semptomu). static inline ile
// her TU kendi local kopya tutar, dedup yok, her TU yeni header'la derlendiginde
// yeni body'sini emit eder ve kullanir.
static inline void CrashTrace_Impl(const char* format, ...) {
#if CT_TRACE_ENABLED || CT_DONE_ENABLED
    // 2026-05-18 PATH FIX: Eskiden `_fsopen("crash_trace.log", ...)` relative
    // path → Client'in CWD'sine bagli. VS Debugger Local Windows Debugger ile
    // baslatildiginda CWD genelde $(ProjectDir) (build/Designed_Chs/Project/),
    // GameData/ degil → log oraya yazilirdi, kullanici goremezdi.
    // Cozum: GetModuleFileNameA ile Client.exe'nin bulundugu dizine sabitle
    // (GameData/), CWD bagimsiz. Cached static, ilk cagrida hesapla.
    static char logPath[MAX_PATH] = {0};
    if (logPath[0] == 0) {
        if (GetModuleFileNameA(NULL, logPath, MAX_PATH) > 0) {
            char* lastSlash = strrchr(logPath, '\\');
            if (lastSlash) {
                strcpy(lastSlash + 1, "crash_trace.log");
            } else {
                strcpy(logPath, "crash_trace.log");
            }
        } else {
            strcpy(logPath, "crash_trace.log");  // fallback
        }
    }
    // Open for append, but allow other processes/TUs to read/write (_SH_DENYNO)
    FILE* fp = _fsopen(logPath, "a", _SH_DENYNO);
    if (!fp) return;

    static DWORD startTime = 0;
    if (startTime == 0) startTime = GetTickCount();

    char buf[4096];
    va_list args;
    va_start(args, format);
    _vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    fprintf(fp, "[%u][TID:%u] %s\n", GetTickCount() - startTime, GetCurrentThreadId(), buf);
    fflush(fp);
    fclose(fp);
#else
    (void)format;  // unused param warning'i sustur
#endif
}

// ---------------------------------------------------------------------------
// CrashTrace — aktif diagnostic trace. Default: sessiz (no-op).
// FAZ 4 (2026-05-03): GHOST mode + tüm major crash'ler fixlendi, log spam'i
// kapatıldı. Yeni bir crash debug'ında CT_TRACE_ENABLED'i 1'e çevir → tüm
// CrashTrace çağrıları (910 site) tekrar log'a akar. Çağrı yerleri silinmedi
// — gelecek crash'lerde çevirip kullanılır. CT_Done ile aynı pattern.
// ---------------------------------------------------------------------------
#ifndef CT_TRACE_ENABLED
// 2026-05-12: PRODUCTION default = 0 (kapali). Onceden 1'di + her CrashTrace
// cagrisi (~910 site) her frame `_fsopen("crash_trace.log","a")` yapip 4KB
// tampon yaziyor → 60 FPS x 910 = ~54k disk I/O/s → ciddi PC lag + F12 gibi
// screen-shot tetikleyicilerinde frame stall + ntdll DbgBreakPoint
// (ntdll!RtlAssert vs RtlReportException) triggered. Yeni crash debug'inda
// 1 yap, sonra geri kapat. Compiler bos inline'i optimize edip kaldirir.
#define CT_TRACE_ENABLED 0
#endif

#if CT_TRACE_ENABLED
    #define CrashTrace(...) CrashTrace_Impl(__VA_ARGS__)
#else
    #define CrashTrace(...) ((void)0)
#endif

// ---------------------------------------------------------------------------
// CT_Done — "başarısı kanıtlanmış" (arşivlenmiş) trace kategorisi için macro.
// Fikir: Bir trace bir sorunu çözdükten sonra kod'dan silmek yerine
// CrashTrace(...)  →  CT_Done(...)  diye rename'lersin. Default: sessiz (no-op).
// Gerekirse aşağıdaki CT_DONE_ENABLED flag'ini 1'e çevir → hepsi tekrar log'a akar.
// Compiler boş inline'ı optimize edip kaldırdığı için kapalıyken runtime maliyeti sıfır.
// ---------------------------------------------------------------------------
#ifndef CT_DONE_ENABLED
// 2026-05-15: D3D9 port login+zone empirically calisir, production mode.
// CT_Done cagri yerleri (~910 site) compiler tarafindan optimize edilip
// runtime maliyeti sıfır. Yeni crash debug'inda 1 yap, sonra tekrar 0.
//
// Tarihce:
//   2026-05-04: 0 (production)
//   2026-05-14: 1 (D3D9 heap corruption debug — Fix #11..#21)
//   2026-05-15: 0 (production tekrar — FPS drop sebebi 54k disk I/O/s idi)
//   2026-05-17: 1 (GECICI — waaass crash root cause tespiti; test sonrasi 0)
//   2026-05-21: 0 (FPS drop sebebi — 910 site x ~5 cast/s = ciddi disk I/O).
//              Skill->Action->Effect/Sound dispatch trace icin yeni CT_ACT
//              kategorisi eklendi (asagida), ayri log dosyasi, dusuk overhead.
#define CT_DONE_ENABLED 0
#endif

#if CT_DONE_ENABLED
    // CrashTrace_Impl'i direkt çağır — CT_TRACE_ENABLED ayrı yönetilebilsin.
    #define CT_Done(...) CrashTrace_Impl(__VA_ARGS__)
#else
    #define CT_Done(...) ((void)0)
#endif

// ---------------------------------------------------------------------------
// CT_BD — BLOOD_DROP sprite chain debug kategorisi (2026-05-19, GECICI).
//
// Senaryo: BLOOD_DROP_GROUND (action.inf idx 1996) HP<30% trigger ile YANLIS
// sprite gosteriyor (normaleffect.efpk frameID 22-24 ground decal beklenirken
// farkli bir sprite render ediliyor). Mevcut CT_TRACE_ENABLED ve CT_DONE_ENABLED
// production'da 0 (54k disk I/O/s lag onlemi, kural 9.4). Bu debug session icin
// onlari acmadan, izole bir flag (CT_BD_ENABLED) + ayri log dosyasi
// (crash_trace_bd.log) + ayri impl function ile sprite chain'i takip ederiz:
//   PacketFunction::ExecuteActionInfoFromMainNode (entry, ai filter)
//     -> MEffectGeneratorTable::Generate (phase / EST / EGID / step)
//       -> MAttachZoneEffectGenerator::Generate (pre/post BLOOD_GROUND_2_1 hardcoded check)
//         -> MTopView::GetMaxEffectFrame (BLT_TYPE + frameID -> numFrame)
//
// Filter pattern: cagri yerinde nActionInfo==1996 veya est in [3611,3614]
// veya bltType==BLT_NORMAL && frameID in [10,40] gibi dar bir araliga skor
// gore log et — frame spam'i onler, sadece BLOOD_DROP path'i yakalar.
//
// Kullanim:
//   CT_BD("MEGT::Generate ai=%d phase=%d EST=%d EGID=%d", ai, ph, est, egid);
//
// Test sonrasi: CT_BD_ENABLED 0'a alinacak (kural 9.4 — production temizlik).
// Cagri yerleri KORUNACAK (kural 4.1 — silme yok). Kapali iken compile-time
// no-op (`((void)0)`) — runtime maliyeti sifir.
// ---------------------------------------------------------------------------
#ifndef CT_BD_ENABLED
// 2026-05-20: 0 production (kural 9.4). BLOOD_DROP_GROUND wrong sprite
// debug session basariyla kapandi. Fix: effectspritetype.inf idx 3611-3614
// -> BLT_NORMAL + FrameID 11/12/13/14 (normaleffect.efpki FSID 11-14 =
// kullanicinin teyit ettigi gercek Blood Drop ground decal sprite havuzu).
// CT_BD cagri yerleri korunuyor (kural 4.1) — gelecek BLOOD_DROP debug'inda
// 1 yap, sonra geri 0. Phoenix CHS deployment'inda BLT_SHADOW path neredeyse
// bos (shadoweffect.efpk 1 FrameSet), DrawEffect bounds guard'lar kalici
// koruma sagliyor (MTopView.cpp:22380).
// 2026-08-29: Darkness overlay round-2 debug (Blt4444Color fix dogrulama).
// 2026-08-30: GCAdd* factory name max=100 dogrulandi (571 burrow DC).
// Cagri yerleri duruyor; production 0 (kural 9.4).
#define CT_BD_ENABLED 0
#endif

// Ayri impl function: CrashTrace_Impl ile karismaz, kendi log dosyasi
// (crash_trace_bd.log), kendi body guard'i (CT_BD_ENABLED). Bu sayede:
//   - CT_TRACE_ENABLED=0 olsa bile CT_BD calisir
//   - Existing 910 CT site'inin baskisini paylasmaz (ayri dosya)
//   - Test sonrasi sadece CT_BD_ENABLED'i 0'la, body bos olur (zero cost)
static inline void CrashTrace_BD_Impl(const char* format, ...) {
#if CT_BD_ENABLED
    static char logPath[MAX_PATH] = {0};
    if (logPath[0] == 0) {
        if (GetModuleFileNameA(NULL, logPath, MAX_PATH) > 0) {
            char* lastSlash = strrchr(logPath, '\\');
            if (lastSlash) {
                strcpy(lastSlash + 1, "crash_trace_bd.log");
            } else {
                strcpy(logPath, "crash_trace_bd.log");
            }
        } else {
            strcpy(logPath, "crash_trace_bd.log");
        }
    }
    FILE* fp = _fsopen(logPath, "a", _SH_DENYNO);
    if (!fp) return;

    static DWORD startTime = 0;
    if (startTime == 0) startTime = GetTickCount();

    char buf[4096];
    va_list args;
    va_start(args, format);
    _vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    fprintf(fp, "[%u][TID:%u] %s\n", GetTickCount() - startTime, GetCurrentThreadId(), buf);
    fflush(fp);
    fclose(fp);
#else
    (void)format;  // unused param warning sustur
#endif
}

#if CT_BD_ENABLED
    #define CT_BD(...) CrashTrace_BD_Impl(__VA_ARGS__)
#else
    #define CT_BD(...) ((void)0)
#endif

// ---------------------------------------------------------------------------
// CT_ACT — Skill->Action->Effect/Sound dispatch zinciri trace (2026-05-21, GECICI).
//
// Senaryo: Gun Shot Guidance (ve diger skill'ler) yanlis ses + yanlis effect
// olusturuyor. Enum drift hipotezleri verify edildi -> drift YOK. Asil root
// cause dispatch zincirinde: skill cast handler -> ExecuteActionInfoFromMainNode
// -> MActionInfoTable lookup -> ACTION_INFO_NODE fields -> MEffectGenerator
// + PlaySound.
//
// FPS-safe: Sadece skill cast event'lerinde tetiklenir (frame-based degil).
// Her cast = ~3-4 log entry. Saniyede maks 20-30 log (5 cast/s varsayarsa).
// Ayri log dosyasi (crash_trace_act.log), CT_TRACE/CT_DONE'dan tamamen izole.
// CT_DONE_ENABLED=0 ile birlikte FPS drop sebebi olan 910 site no-op olur,
// sadece bu 3-4 CT_ACT site aktif kalir.
//
// Kullanim:
//   CT_ACT("EAINF ai=%d tgt=%d nodes=%d", nActionInfo, tgtID, nodeCount);
//
// Test sonrasi: CT_ACT_ENABLED=0 (kural 9.4 production temizlik). Cagri
// yerleri korunur (kural 4.1 — silme yok). Kapali iken compile-time no-op.
// ---------------------------------------------------------------------------
#ifndef CT_ACT_ENABLED
// 2026-05-21 GECICI ACILDI — Plasma + GSG aim debug. Test sonrasi 0.
// (Onceki KAPATILDI notu: 2026-05-21 Skill->Action->Effect/Sound dispatch
// trace tamamlandi, bulgular Dev_Log/sessions/2026-05-21.md'de.)
#define CT_ACT_ENABLED 0
#endif

static inline void CrashTrace_ACT_Impl(const char* format, ...) {
#if CT_ACT_ENABLED
    static char logPath[MAX_PATH] = {0};
    if (logPath[0] == 0) {
        if (GetModuleFileNameA(NULL, logPath, MAX_PATH) > 0) {
            char* lastSlash = strrchr(logPath, '\\');
            if (lastSlash) {
                strcpy(lastSlash + 1, "crash_trace_act.log");
            } else {
                strcpy(logPath, "crash_trace_act.log");
            }
        } else {
            strcpy(logPath, "crash_trace_act.log");
        }
    }
    FILE* fp = _fsopen(logPath, "a", _SH_DENYNO);
    if (!fp) return;

    static DWORD startTime = 0;
    if (startTime == 0) startTime = GetTickCount();

    char buf[4096];
    va_list args;
    va_start(args, format);
    _vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    fprintf(fp, "[%u][TID:%u] %s\n", GetTickCount() - startTime, GetCurrentThreadId(), buf);
    fflush(fp);
    fclose(fp);
#else
    (void)format;
#endif
}

#if CT_ACT_ENABLED
    #define CT_ACT(...) CrashTrace_ACT_Impl(__VA_ARGS__)
#else
    #define CT_ACT(...) ((void)0)
#endif

// ---------------------------------------------------------------------------
// CT_BAT — Transform to Bat (VAMPIRE_GHOST 813) debug kategorisi (2026-08-17, GECICI).
//
// Senaryo: Transform to Bat skill'i kullanildiginda karakter VAMPIRE_GHOST (813)
// creature type'ina donusuyor ama hareket etmiyor. FlyingCreature flag'i ve
// FlyingHeight set ediliyor mu, movement path'i çalışıyor mu debug etmek icin.
//
// FPS-safe: Sadece transform event'inde tetiklenir (frame-based degil).
// Her transform = ~3-4 log entry. Saniyede maks 10-20 log.
// Ayri log dosyasi (crash_trace_bat.log), CT_TRACE/CT_DONE/CT_ACT/CT_BD/CT_HOTKEY/CT_ITEM'dan tamamen izole.
// CT_DONE_ENABLED=0 ile birlikte FPS drop sebebi olan 910 site no-op olur,
// sadece bu CT_BAT site'lari aktif kalir.
//
// Kullanim:
//   CT_BAT("TransformToBat: type=%d flying=%d height=%d", type, flying, height);
//   CT_BAT("SetFlyingCreature: type=%d m_MoveType=%d m_Z=%d", type, moveType, z);
//   CT_BAT("ActionMove: type=%d moveType=%d dest=(%d,%d)", type, moveType, destX, destY);
//
// Test sonrasi: CT_BAT_ENABLED=0 (kural 9.4 production temizlik). Cagri
// yerleri korunur (kural 4.1 — silme yok). Kapali iken compile-time no-op.
// ---------------------------------------------------------------------------
#ifndef CT_BAT_ENABLED
// 2026-08-17: Transform to Bat movement debug icin acildi.
// 2026-08-19: Fix dogrulandi (IsVampire tribe restorasyonu, kullanici testi OK) —
// kural 9.4 geregi production'da kapali. Cagri yerleri korunur (kural 4.1), debug'da 1'e cevrilir.
#define CT_BAT_ENABLED 0
#endif

// Ayri impl function: CrashTrace_Impl ile karismaz, kendi log dosyasi
// (crash_trace_bat.log), kendi body guard'i (CT_BAT_ENABLED). Bu sayede:
//   - CT_TRACE_ENABLED=0 olsa bile CT_BAT calisir
//   - Existing 910+ CT site'inin baskisini paylasmaz (ayri dosya)
//   - Test sonrasi sadece CT_BAT_ENABLED'i 0'la, body bos olur (zero cost)
static inline void CrashTrace_BAT_Impl(const char* format, ...) {
#if CT_BAT_ENABLED
    static char logPath[MAX_PATH] = {0};
    if (logPath[0] == 0) {
        if (GetModuleFileNameA(NULL, logPath, MAX_PATH) > 0) {
            char* lastSlash = strrchr(logPath, '\\');
            if (lastSlash) {
                strcpy(lastSlash + 1, "crash_trace_bat.log");
            } else {
                strcpy(logPath, "crash_trace_bat.log");
            }
        } else {
            strcpy(logPath, "crash_trace_bat.log");
        }
    }
    FILE* fp = _fsopen(logPath, "a", _SH_DENYNO);
    if (!fp) return;

    static DWORD startTime = 0;
    if (startTime == 0) startTime = GetTickCount();

    char buf[4096];
    va_list args;
    va_start(args, format);
    _vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    fprintf(fp, "[%u][TID:%u] %s\n", GetTickCount() - startTime, GetCurrentThreadId(), buf);
    fflush(fp);
    fclose(fp);
#else
    (void)format;  // unused param warning sustur
#endif
}

#if CT_BAT_ENABLED
    #define CT_BAT(...) CrashTrace_BAT_Impl(__VA_ARGS__)
#else
    #define CT_BAT(...) ((void)0)
#endif

// ---------------------------------------------------------------------------
// CT_OBJ — Ouster LEARN buton + harita objesi sprite pack teshisi
// (2026-09-01, GECICI).
//
// Senaryo 1: Ouster MilitaryDlg skill popup'inda LEARN butonu gorunmuyor
// (Slayer/Vampire OK). VS_UI_GameCommon2.cpp ShowButtonWidget LEARN_ID
// dalinda ENTER/EXIT izi (1 sn throttle, yalniz popup acikken).
// Senaryo 2: Logout/Login sonrasi harita objeleri bazen yuklenmiyor.
// CTypePack/CTypePack2 Get() failure path'leri (dummy dondurme, split open
// fail, NOT-initialized) — yalniz HATA durumunda tetiklenir.
//
// FPS-safe: Global CT_DONE_ENABLED=1 acmak 910 site x her frame disk I/O
// yapip FPS'i 3-10'a dusurdu (2026-09-01 kullanici raporu). Bu kategori
// CT_BD/CT_ACT/CT_ITEM pattern'iyle izole: ayri flag, ayri dosya
// (crash_trace_obj.log), yalniz nadir/throttle'li call site'lar.
//
// Test sonrasi: CT_OBJ_ENABLED=0 (kural 9.4). Cagri yerleri korunur
// (kural 4.1). Kapali iken compile-time no-op.
// ---------------------------------------------------------------------------
#ifndef CT_OBJ_ENABLED
// 2026-09-01: Ouster LEARN buton + harita objeleri teshisi icin acildi.
// 2026-09-04 KAPATILDI (kural 9.4, LIVE hazirlik): Oyuncular hicbir log
// dosyasi gormemeli. Cagri yerleri korunuyor (kural 4.1) — yeni debug'da 1.
#define CT_OBJ_ENABLED 0
#endif

static inline void CrashTrace_OBJ_Impl(const char* format, ...) {
#if CT_OBJ_ENABLED
    static char logPath[MAX_PATH] = {0};
    if (logPath[0] == 0) {
        if (GetModuleFileNameA(NULL, logPath, MAX_PATH) > 0) {
            char* lastSlash = strrchr(logPath, '\\');
            if (lastSlash) {
                strcpy(lastSlash + 1, "crash_trace_obj.log");
            } else {
                strcpy(logPath, "crash_trace_obj.log");
            }
        } else {
            strcpy(logPath, "crash_trace_obj.log");
        }
    }
    FILE* fp = _fsopen(logPath, "a", _SH_DENYNO);
    if (!fp) return;

    static DWORD startTime = 0;
    if (startTime == 0) startTime = GetTickCount();

    char buf[4096];
    va_list args;
    va_start(args, format);
    _vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    fprintf(fp, "[%u][TID:%u] %s\n", GetTickCount() - startTime, GetCurrentThreadId(), buf);
    fflush(fp);
    fclose(fp);
#else
    (void)format;  // unused param warning sustur
#endif
}

#if CT_OBJ_ENABLED
    #define CT_OBJ(...) CrashTrace_OBJ_Impl(__VA_ARGS__)
#else
    #define CT_OBJ(...) ((void)0)
#endif

// ---------------------------------------------------------------------------
// CT_HOTKEY — DIK_R / GRADE1 Rank hotkey trace (2026-05-25, GECICI).
//
// Senaryo: Rank System hotkey'i kaynakta aktif gorunuyor, fakat runtime'da
// tusun input filtresinde mi, accelerator dispatch'te mi, yoksa RankBonus
// availability guard'inda mi durdugunu net gormek gerekiyor.
//
// FPS-safe: Sadece DIK_R path'inde ve GRADE1 hotkey cagri zincirinde log yazar.
// Ayri log dosyasi: crash_trace_hotkey.log.
//
// Test sonrasi: CT_HOTKEY_ENABLED=0 (kural 9.4 production temizlik). Cagri
// yerleri korunur (kural 4.1). Kapali iken compile-time no-op.
// ---------------------------------------------------------------------------
#ifndef CT_HOTKEY_ENABLED
#define CT_HOTKEY_ENABLED 0
#endif

static inline void CrashTrace_HOTKEY_Impl(const char* format, ...) {
#if CT_HOTKEY_ENABLED
    static char logPath[MAX_PATH] = {0};
    if (logPath[0] == 0) {
        if (GetModuleFileNameA(NULL, logPath, MAX_PATH) > 0) {
            char* lastSlash = strrchr(logPath, '\\');
            if (lastSlash) {
                strcpy(lastSlash + 1, "crash_trace_hotkey.log");
            } else {
                strcpy(logPath, "crash_trace_hotkey.log");
            }
        } else {
            strcpy(logPath, "crash_trace_hotkey.log");
        }
    }
    FILE* fp = _fsopen(logPath, "a", _SH_DENYNO);
    if (!fp) return;

    static DWORD startTime = 0;
    if (startTime == 0) startTime = GetTickCount();

    char buf[4096];
    va_list args;
    va_start(args, format);
    _vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    fprintf(fp, "[%u][TID:%u] %s\n", GetTickCount() - startTime, GetCurrentThreadId(), buf);
    fflush(fp);
    fclose(fp);
#else
    (void)format;
#endif
}

#if CT_HOTKEY_ENABLED
    #define CT_HOTKEY(...) CrashTrace_HOTKEY_Impl(__VA_ARGS__)
#else
    #define CT_HOTKEY(...) ((void)0)
#endif

// ---------------------------------------------------------------------------
// CT_ITEM — Item tooltip / _Item_Description_Show crash debug (2026-05-23, GECICI).
//
// Senaryo: Esya uzerine hover yapildiginda client crash (frame _Item_Description_Show).
// VS Call Stack L6324 (CGameUpdate::UpdateDraw -> gC_vs_ui.Show() return point).
// VS_UI_Description.cpp icindeki 47 mevcut CT_Done("ItemDescShow: ...") site bu
// kategoriye RENAME edildi (kural 4.1 silme yok, sadece prefix degisti).
// Global CT_DONE_ENABLED 0 kalir (910 site no-op), sadece bu 47 site aktif
// -> dusuk overhead, FPS drop yok.
//
// FPS profili: Tooltip sadece hover olunca Show edilir -> frame basina 0 veya 1
// tooltip render. Hover yoksa tum 47 site no-op (kosul bile evaluate edilmez,
// fonksiyon zaten cagrilmaz). Hover varken: 47 disk I/O / frame.
// Test sonrasi: CT_ITEM_ENABLED=0 (kural 9.4 production).
//
// Kullanim:
//   CT_ITEM("ItemDescShow: ENTER void_ptr=%p right=%ld", void_ptr, right);
//
// Cagri yerleri korunur (kural 4.1 silme yok). Kapali iken compile-time no-op.
// ---------------------------------------------------------------------------
#ifndef CT_ITEM_ENABLED
#define CT_ITEM_ENABLED 0
#endif

static inline void CrashTrace_ITEM_Impl(const char* format, ...) {
#if CT_ITEM_ENABLED
    static char logPath[MAX_PATH] = {0};
    if (logPath[0] == 0) {
        if (GetModuleFileNameA(NULL, logPath, MAX_PATH) > 0) {
            char* lastSlash = strrchr(logPath, '\\');
            if (lastSlash) {
                strcpy(lastSlash + 1, "crash_trace_item.log");
            } else {
                strcpy(logPath, "crash_trace_item.log");
            }
        } else {
            strcpy(logPath, "crash_trace_item.log");
        }
    }
    FILE* fp = _fsopen(logPath, "a", _SH_DENYNO);
    if (!fp) return;

    static DWORD startTime = 0;
    if (startTime == 0) startTime = GetTickCount();

    char buf[4096];
    va_list args;
    va_start(args, format);
    _vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    fprintf(fp, "[%u][TID:%u] %s\n", GetTickCount() - startTime, GetCurrentThreadId(), buf);
    fflush(fp);
    fclose(fp);
#else
    (void)format;
#endif
}

#if CT_ITEM_ENABLED
    #define CT_ITEM(...) CrashTrace_ITEM_Impl(__VA_ARGS__)
#else
    #define CT_ITEM(...) ((void)0)
#endif

#endif
