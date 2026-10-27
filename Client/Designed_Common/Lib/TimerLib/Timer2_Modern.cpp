// =============================================================================
//  Timer2_Modern.cpp — Project Phoenix modern C++17 implementation
//  -----------------------------------------------------------------------------
//  Orijinal header: Designed_Common/Lib/ui/timer2.h  (KJTINC, 2000.06.15)
//
//  Amaç:
//   - Eski Timer.lib / Timer.dll binary bağımlılığını kaldırmak.
//   - Aynı public API'yi (C_TIMER2, gC_timer2) korumak; çağrı noktalarında
//     hiçbir değişiklik gerekmeden drop-in çalışmak.
//   - İç implementasyonu modern C++17 ile temizlemek (std::chrono kullanarak
//     zaman ölçümünü daha güvenilir hâle getirmek).
//
//  Not:
//   - Header'a DOKUNULMADI (binary-uyumluluk için S_TIMERUNIT/S_TIMERQUEUE
//     POD yapıları ve üye değişkenler aynı). Sadece .cpp tarafı yazıldı.
//   - CrashTrace kancaları (kullanıcının tutarlı tanı pattern'i) kritik
//     yerlere yerleştirildi.
//   - Callback imzası void(*)(void) aynı bırakıldı — ileride std::function'a
//     geçmek header'ı değiştirir, bu FAZ 2.2 kapsamında ayrı ele alınır.
// =============================================================================

#include "Timer2.h"
#include "../../Project/Client/crash_trace_tool.h"

#include <windows.h>
#include <chrono>
#include <cstring>
#include <new>

// -----------------------------------------------------------------------------
// Global timer yöneticisi — header'daki "extern C_TIMER2 gC_timer2;" tanımı
// -----------------------------------------------------------------------------
C_TIMER2 gC_timer2;

namespace {

// Steady clock tabanlı millisecond ticker — GetTickCount() yerine kullanılır.
// Orijinal API DWORD (32-bit) döndürdüğü için value mask ile sınırlandırılmış.
inline DWORD PhoenixTickCount()
{
    using namespace std::chrono;
    const auto now = steady_clock::now().time_since_epoch();
    const auto ms  = duration_cast<milliseconds>(now).count();
    return static_cast<DWORD>(ms & 0xFFFFFFFFu);
}

} // namespace

// -----------------------------------------------------------------------------
// Yapıcı / Yıkıcı
// -----------------------------------------------------------------------------
C_TIMER2::C_TIMER2()
    : m_id_generator(0)
{
    m_timer_queue.size         = 0;
    m_timer_queue.pS_timerunit = nullptr;
    CT_Done("C_TIMER2::ctor — modern impl hazir");
}

C_TIMER2::~C_TIMER2()
{
    delete[] m_timer_queue.pS_timerunit;
    m_timer_queue.pS_timerunit = nullptr;
    m_timer_queue.size         = 0;
}

// -----------------------------------------------------------------------------
// Execute(S_TIMERUNIT*) — özel yardımcı, tek bir timer'ın callback'ini çalıştırır
// -----------------------------------------------------------------------------
void C_TIMER2::Execute(S_TIMERUNIT* pS_timerunit)
{
    if (!pS_timerunit || !pS_timerunit->fp_proc) {
        return;
    }
    // Callback geri çağrısı
    pS_timerunit->fp_proc();
}

// -----------------------------------------------------------------------------
// Execute() — tüm aktif timer'ları tara, vadesi gelenlerin callback'lerini tetikle
// -----------------------------------------------------------------------------
void C_TIMER2::Execute()
{
    const DWORD now = PhoenixTickCount();

    for (int i = 0; i < m_timer_queue.size; ++i) {
        S_TIMERUNIT& u = m_timer_queue.pS_timerunit[i];

        if (u.bl_pause) {
            continue;
        }

        // DWORD subtraction wraparound-safe — 32-bit unsigned modular aritmetik
        const DWORD elapsed = now - u.dw_prev_tickcount;
        if (elapsed >= u.dw_millisec) {
            u.dw_prev_tickcount = now;
            Execute(&u);
        }
    }
}

// -----------------------------------------------------------------------------
// Refresh — timer'ın başlangıç tick'ini şimdiye çek (sıfırdan başlat)
// -----------------------------------------------------------------------------
void C_TIMER2::Refresh(timer_id_t tid)
{
    const DWORD now = PhoenixTickCount();
    for (int i = 0; i < m_timer_queue.size; ++i) {
        if (m_timer_queue.pS_timerunit[i].tid == tid) {
            m_timer_queue.pS_timerunit[i].dw_prev_tickcount = now;
            return;
        }
    }
}

// -----------------------------------------------------------------------------
// Pause — timer'ı duraklat
// -----------------------------------------------------------------------------
void C_TIMER2::Pause(timer_id_t tid)
{
    for (int i = 0; i < m_timer_queue.size; ++i) {
        if (m_timer_queue.pS_timerunit[i].tid == tid) {
            m_timer_queue.pS_timerunit[i].bl_pause = 1;
            return;
        }
    }
}

// -----------------------------------------------------------------------------
// Continue — duraklatılmış timer'ı sürdür (tick başlangıcı şimdiye çekilir)
// -----------------------------------------------------------------------------
void C_TIMER2::Continue(timer_id_t tid)
{
    const DWORD now = PhoenixTickCount();
    for (int i = 0; i < m_timer_queue.size; ++i) {
        if (m_timer_queue.pS_timerunit[i].tid == tid) {
            m_timer_queue.pS_timerunit[i].bl_pause         = 0;
            m_timer_queue.pS_timerunit[i].dw_prev_tickcount = now;
            return;
        }
    }
}

// -----------------------------------------------------------------------------
// Delete — timer'ı kaldırır; tid referans ile INVALID_TID'e set edilir.
//          Queue dinamik array'ini kompakt eder.
// -----------------------------------------------------------------------------
bool C_TIMER2::Delete(timer_id_t& tid)
{
    int foundIndex = INVALID_INDEX;
    for (int i = 0; i < m_timer_queue.size; ++i) {
        if (m_timer_queue.pS_timerunit[i].tid == tid) {
            foundIndex = i;
            break;
        }
    }
    if (foundIndex == INVALID_INDEX) {
        return false;
    }

    const int newSize = m_timer_queue.size - 1;
    S_TIMERUNIT* newArr = nullptr;

    if (newSize > 0) {
        newArr = new (std::nothrow) S_TIMERUNIT[newSize];
        if (!newArr) {
            CT_Done("C_TIMER2::Delete — bellek tahsisi basarisiz!");
            return false;
        }
        // [0..foundIndex) blok kopyası
        if (foundIndex > 0) {
            std::memcpy(newArr,
                        m_timer_queue.pS_timerunit,
                        sizeof(S_TIMERUNIT) * foundIndex);
        }
        // (foundIndex..size) blok kopyası
        const int rest = m_timer_queue.size - foundIndex - 1;
        if (rest > 0) {
            std::memcpy(newArr + foundIndex,
                        m_timer_queue.pS_timerunit + foundIndex + 1,
                        sizeof(S_TIMERUNIT) * rest);
        }
    }

    delete[] m_timer_queue.pS_timerunit;
    m_timer_queue.pS_timerunit = newArr;
    m_timer_queue.size         = newSize;

    tid = INVALID_TID;
    return true;
}

// -----------------------------------------------------------------------------
// ResetSpeed — timer'ın çalışma periyodunu değiştir (milisaniye cinsinden)
// -----------------------------------------------------------------------------
void C_TIMER2::ResetSpeed(timer_id_t tid, DWORD millisec)
{
    for (int i = 0; i < m_timer_queue.size; ++i) {
        if (m_timer_queue.pS_timerunit[i].tid == tid) {
            m_timer_queue.pS_timerunit[i].dw_millisec = millisec;
            return;
        }
    }
}

// -----------------------------------------------------------------------------
// Add — yeni bir timer ekle, benzersiz tid döndür. Başarısızlıkta INVALID_TID.
// -----------------------------------------------------------------------------
timer_id_t C_TIMER2::Add(DWORD dw_millisec, void (*fp_proc)(void))
{
    if (!fp_proc) {
        CT_Done("C_TIMER2::Add — null callback, ignored");
        return INVALID_TID;
    }

    const int   newSize = m_timer_queue.size + 1;
    S_TIMERUNIT* newArr = new (std::nothrow) S_TIMERUNIT[newSize];
    if (!newArr) {
        CT_Done("C_TIMER2::Add — bellek tahsisi basarisiz!");
        return INVALID_TID;
    }

    if (m_timer_queue.pS_timerunit && m_timer_queue.size > 0) {
        std::memcpy(newArr,
                    m_timer_queue.pS_timerunit,
                    sizeof(S_TIMERUNIT) * m_timer_queue.size);
        delete[] m_timer_queue.pS_timerunit;
    }

    S_TIMERUNIT& u     = newArr[m_timer_queue.size];
    u.bl_pause         = 0;
    u.dw_prev_tickcount = PhoenixTickCount();
    u.dw_millisec      = dw_millisec;
    u.tid              = ++m_id_generator;
    u.fp_proc          = fp_proc;

    m_timer_queue.pS_timerunit = newArr;
    m_timer_queue.size         = newSize;

    return u.tid;
}
