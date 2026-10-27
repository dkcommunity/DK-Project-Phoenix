//-----------------------------------------------------------------------------
// XAudio2Backend.cpp
//
// FULL DX11 gecis Faz S1 (2026-09-01) — gercek XAudio2 ses backend'i.
// Mimari aciklama: XAudio2Backend.h basligindaki yorum.
//-----------------------------------------------------------------------------

#include "XAudio2Backend.h"
#include "crash_trace_tool.h"

#include <windows.h>
#include <xaudio2.h>
#include <vorbis/vorbisfile.h>

#include <atomic>
#include <thread>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

#pragma comment(lib, "xaudio2.lib")

namespace phoenix {
namespace audio {

namespace {

//-----------------------------------------------------------------------------
// Sabitler
//-----------------------------------------------------------------------------
const int   kMaxVoiceSlots   = 64;     // es anli SFX voice ustsiniri
const float kMaxFreqRatio    = 4.0f;   // SetFrequencyRatio ust siniri
const int   kStreamBufCount  = 4;      // BGM ring buffer adedi
const int   kStreamBufBytes  = 64 * 1024;

//-----------------------------------------------------------------------------
// Config — audio_engine.cfg / PHOENIX_AUDIO_ENGINE ("xaudio2" → secili).
// text_engine.cfg / render_backend.cfg pattern'inin aynisi.
//-----------------------------------------------------------------------------
bool ReadEngineSelection()
{
    char envVal[32] = { 0 };
    DWORD n = GetEnvironmentVariableA("PHOENIX_AUDIO_ENGINE", envVal, sizeof(envVal));
    if (n > 0 && n < sizeof(envVal))
    {
        const bool sel = (_stricmp(envVal, "xaudio2") == 0);
        CT_Done("Phoenix audio engine (env): %s", sel ? "XAudio2" : "SoLoud");
        return sel;
    }

    char modPath[MAX_PATH] = { 0 };
    if (GetModuleFileNameA(NULL, modPath, MAX_PATH) > 0)
    {
        char* pSlash = strrchr(modPath, '\\');
        if (pSlash != NULL)
        {
            *(pSlash + 1) = '\0';
            char cfgPath[MAX_PATH];
            _snprintf(cfgPath, MAX_PATH - 1, "%saudio_engine.cfg", modPath);
            cfgPath[MAX_PATH - 1] = '\0';

            FILE* f = fopen(cfgPath, "rt");
            if (f != NULL)
            {
                char line[64] = { 0 };
                bool sel = false;
                if (fgets(line, sizeof(line), f) != NULL)
                {
                    for (int i = (int)strlen(line) - 1;
                         i >= 0 && (line[i] == '\n' || line[i] == '\r' || line[i] == ' ' || line[i] == '\t');
                         --i)
                        line[i] = '\0';
                    sel = (_stricmp(line, "xaudio2") == 0);
                }
                fclose(f);
                CT_Done("Phoenix audio engine: %s (audio_engine.cfg)", sel ? "XAudio2" : "SoLoud");
                return sel;
            }
        }
    }

    CT_Done("Phoenix audio engine: SoLoud (varsayilan — audio_engine.cfg yok)");
    return false;
}

//-----------------------------------------------------------------------------
// Equal-power pan matrisi (kaynak kanal sayisina gore 2-cikis)
//-----------------------------------------------------------------------------
void ApplyPanMatrix(IXAudio2SourceVoice* pVoice, int srcChannels, float pan)
{
    if (pVoice == NULL)
        return;
    if (pan < -1.0f) pan = -1.0f;
    if (pan >  1.0f) pan =  1.0f;

    // Equal-power: t 0..1 (0=sol, 1=sag)
    const float t = (pan + 1.0f) * 0.5f;
    const float PI_2 = 1.5707963f;
    const float l = cosf(t * PI_2);
    const float r = sinf(t * PI_2);

    float matrix[8] = { 0 };
    if (srcChannels == 1)
    {
        matrix[0] = l;   // src0 → L
        matrix[1] = r;   // src0 → R
        pVoice->SetOutputMatrix(NULL, 1, 2, matrix);
    }
    else if (srcChannels == 2)
    {
        // Stereo kaynak: pan sola gittikce sag kanal kisilir (DirectSound
        // SetPan semantigi ile ayni his)
        matrix[0] = (pan <= 0.0f) ? 1.0f : (1.0f - pan);  // srcL → L
        matrix[1] = 0.0f;                                  // srcL → R
        matrix[2] = 0.0f;                                  // srcR → L
        matrix[3] = (pan >= 0.0f) ? 1.0f : (1.0f + pan);  // srcR → R
        pVoice->SetOutputMatrix(NULL, 2, 2, matrix);
    }
}

} // namespace

//-----------------------------------------------------------------------------
// XA2Sound — parse edilmis WAV (paylasilan, refcount'lu)
//-----------------------------------------------------------------------------
struct XA2Sound
{
    WAVEFORMATEX      fmt;
    std::vector<BYTE> pcm;
    LONG              refCount;
};

//-----------------------------------------------------------------------------
// Voice slot + callback
//-----------------------------------------------------------------------------
struct VoiceSlot;

struct SlotCallback : public IXAudio2VoiceCallback
{
    VoiceSlot* pSlot;

    SlotCallback() : pSlot(NULL) {}

    void STDMETHODCALLTYPE OnBufferEnd(void* pContext);
    void STDMETHODCALLTYPE OnStreamEnd() {}
    void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) {}
    void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() {}
    void STDMETHODCALLTYPE OnBufferStart(void*) {}
    void STDMETHODCALLTYPE OnLoopEnd(void*) {}
    void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT hr)
    {
        CT_Done("XAudio2: OnVoiceError 0x%08X", hr);
    }
};

struct VoiceSlot
{
    IXAudio2SourceVoice* pVoice;
    SlotCallback         callback;
    XA2Sound*            pSound;       // addref'li — slot bosalana kadar yasar
    WAVEFORMATEX         fmt;          // voice'un yaratildigi format
    std::atomic<bool>    busy;
    unsigned int         generation;

    VoiceSlot() : pVoice(NULL), pSound(NULL), busy(false), generation(0)
    {
        memset(&fmt, 0, sizeof(fmt));
        callback.pSlot = this;
    }
};

void STDMETHODCALLTYPE SlotCallback::OnBufferEnd(void* /*pContext*/)
{
    // Audio thread — yalniz atomic isaret. pSound release'i main thread'de
    // (Play/Shutdown sirasinda) yapilir; deallocation callback'te YASAK.
    if (pSlot != NULL)
        pSlot->busy.store(false);
}

//-----------------------------------------------------------------------------
// XA2Stream — OGG BGM stream (feeder thread'li)
//-----------------------------------------------------------------------------
struct StreamCallback : public IXAudio2VoiceCallback
{
    HANDLE hBufferEndEvent;

    StreamCallback() : hBufferEndEvent(NULL) {}

    void STDMETHODCALLTYPE OnBufferEnd(void*)
    {
        if (hBufferEndEvent != NULL)
            SetEvent(hBufferEndEvent);
    }
    void STDMETHODCALLTYPE OnStreamEnd() {}
    void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) {}
    void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() {}
    void STDMETHODCALLTYPE OnBufferStart(void*) {}
    void STDMETHODCALLTYPE OnLoopEnd(void*) {}
    void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT hr)
    {
        CT_Done("XAudio2 stream: OnVoiceError 0x%08X", hr);
    }
};

struct XA2Stream
{
    IXAudio2SourceVoice* pVoice;
    StreamCallback       callback;
    OggVorbis_File       vf;
    FILE*                pFile;
    WAVEFORMATEX         fmt;
    bool                 loop;

    BYTE                 buffers[kStreamBufCount][kStreamBufBytes];
    int                  nextBuf;

    std::thread          feeder;
    std::atomic<bool>    quitFlag;
    std::atomic<bool>    playing;
    std::atomic<bool>    paused;
    std::atomic<bool>    ended;     // non-loop stream sonuna geldi

    XA2Stream()
        : pVoice(NULL), pFile(NULL), loop(false), nextBuf(0),
          quitFlag(false), playing(false), paused(false), ended(false)
    {
        memset(&vf, 0, sizeof(vf));
        memset(&fmt, 0, sizeof(fmt));
    }
};

//-----------------------------------------------------------------------------
// Backend ic durumu
//-----------------------------------------------------------------------------
namespace {

IXAudio2*               g_pXAudio2 = NULL;
IXAudio2MasteringVoice* g_pMasterVoice = NULL;
bool                    g_bInitTried = false;
bool                    g_bInitOK = false;
VoiceSlot               g_Slots[kMaxVoiceSlots];
unsigned int            g_NextGeneration = 1;

inline unsigned int MakeHandle(int slot, unsigned int gen)
{
    return ((unsigned int)(slot + 1) << 16) | (gen & 0xFFFF);
}

// Handle → slot (generation uyusmazsa NULL: slot baskasina verilmis)
VoiceSlot* SlotFromHandle(unsigned int handle)
{
    if (handle == 0)
        return NULL;
    const int slot = (int)(handle >> 16) - 1;
    if (slot < 0 || slot >= kMaxVoiceSlots)
        return NULL;
    if ((g_Slots[slot].generation & 0xFFFF) != (handle & 0xFFFF))
        return NULL;
    return &g_Slots[slot];
}

//-----------------------------------------------------------------------------
// RIFF WAV parse — PCM (8/16 bit) + IEEE float; WAVE_FORMAT_EXTENSIBLE cozulur
//-----------------------------------------------------------------------------
bool ParseWav(const BYTE* pData, size_t size, WAVEFORMATEX* pFmt,
              const BYTE** ppPcm, size_t* pPcmBytes, const char* pathForLog)
{
    if (pData == NULL || size < 44)
        return false;

    if (memcmp(pData, "RIFF", 4) != 0 || memcmp(pData + 8, "WAVE", 4) != 0)
    {
        CT_Done("XAudio2: RIFF/WAVE imzasi yok (%s)", pathForLog);
        return false;
    }

    const BYTE* pFmtChunk = NULL;
    DWORD fmtSize = 0;
    const BYTE* pDataChunk = NULL;
    DWORD dataSize = 0;

    size_t pos = 12;
    while (pos + 8 <= size)
    {
        const char* id = (const char*)(pData + pos);
        DWORD chunkSize;
        memcpy(&chunkSize, pData + pos + 4, 4);
        const BYTE* pChunk = pData + pos + 8;

        if (pos + 8 + chunkSize > size)
            chunkSize = (DWORD)(size - pos - 8);

        if (memcmp(id, "fmt ", 4) == 0)
        {
            pFmtChunk = pChunk;
            fmtSize = chunkSize;
        }
        else if (memcmp(id, "data", 4) == 0)
        {
            pDataChunk = pChunk;
            dataSize = chunkSize;
        }

        pos += 8 + chunkSize + (chunkSize & 1);   // chunk'lar word-aligned
    }

    if (pFmtChunk == NULL || pDataChunk == NULL || fmtSize < 16 || dataSize == 0)
    {
        CT_Done("XAudio2: fmt/data chunk eksik (%s)", pathForLog);
        return false;
    }

    WAVEFORMATEX fmt;
    memset(&fmt, 0, sizeof(fmt));
    memcpy(&fmt, pFmtChunk, (fmtSize < sizeof(WAVEFORMATEX)) ? fmtSize : sizeof(WAVEFORMATEX));

    // WAVE_FORMAT_EXTENSIBLE → SubFormat'in ilk WORD'u gercek tag
    if (fmt.wFormatTag == 0xFFFE && fmtSize >= 40)
    {
        WORD realTag;
        memcpy(&realTag, pFmtChunk + 24, 2);
        fmt.wFormatTag = realTag;
        fmt.cbSize = 0;
    }

    if (fmt.wFormatTag != WAVE_FORMAT_PCM && fmt.wFormatTag != 3 /*IEEE float*/)
    {
        CT_Done("XAudio2: desteklenmeyen wav format tag=%d (%s) — SoLoud fallback",
                fmt.wFormatTag, pathForLog);
        return false;
    }
    if (fmt.nChannels < 1 || fmt.nChannels > 2 || fmt.nSamplesPerSec < 4000)
    {
        CT_Done("XAudio2: anormal wav format ch=%d rate=%d (%s)",
                fmt.nChannels, fmt.nSamplesPerSec, pathForLog);
        return false;
    }

    fmt.nBlockAlign = (WORD)(fmt.nChannels * fmt.wBitsPerSample / 8);
    fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;
    fmt.cbSize = 0;

    *pFmt = fmt;
    *ppPcm = pDataChunk;
    *pPcmBytes = dataSize;
    return true;
}

//-----------------------------------------------------------------------------
// Slot edinme: bos slot > format uyumlu bos voice reuse > yeni voice
//-----------------------------------------------------------------------------
bool FormatsMatch(const WAVEFORMATEX& a, const WAVEFORMATEX& b)
{
    return a.wFormatTag == b.wFormatTag &&
           a.nChannels == b.nChannels &&
           a.nSamplesPerSec == b.nSamplesPerSec &&
           a.wBitsPerSample == b.wBitsPerSample;
}

VoiceSlot* AcquireSlot(const WAVEFORMATEX& fmt)
{
    // 1) Ayni formatta bos voice (reuse — voice create pahali)
    for (int i = 0; i < kMaxVoiceSlots; ++i)
    {
        VoiceSlot& s = g_Slots[i];
        if (s.pVoice != NULL && !s.busy.load() && FormatsMatch(s.fmt, fmt))
        {
            s.pVoice->FlushSourceBuffers();
            if (s.pSound != NULL)
            {
                XAudio2Backend::Instance().ReleaseSound(s.pSound);
                s.pSound = NULL;
            }
            return &s;
        }
    }

    // 2) Hic voice olmayan slot — yeni voice
    for (int i = 0; i < kMaxVoiceSlots; ++i)
    {
        VoiceSlot& s = g_Slots[i];
        if (s.pVoice == NULL)
        {
            HRESULT hr = g_pXAudio2->CreateSourceVoice(
                &s.pVoice, &fmt, 0, kMaxFreqRatio, &s.callback);
            if (FAILED(hr) || s.pVoice == NULL)
            {
                CT_Done("XAudio2: CreateSourceVoice FAILED 0x%08X", hr);
                s.pVoice = NULL;
                return NULL;
            }
            s.fmt = fmt;
            return &s;
        }
    }

    // 3) Farkli formatta bos voice — yik, yeniden yarat
    for (int i = 0; i < kMaxVoiceSlots; ++i)
    {
        VoiceSlot& s = g_Slots[i];
        if (s.pVoice != NULL && !s.busy.load())
        {
            s.pVoice->DestroyVoice();
            s.pVoice = NULL;
            if (s.pSound != NULL)
            {
                XAudio2Backend::Instance().ReleaseSound(s.pSound);
                s.pSound = NULL;
            }
            HRESULT hr = g_pXAudio2->CreateSourceVoice(
                &s.pVoice, &fmt, 0, kMaxFreqRatio, &s.callback);
            if (FAILED(hr) || s.pVoice == NULL)
            {
                CT_Done("XAudio2: CreateSourceVoice(re) FAILED 0x%08X", hr);
                s.pVoice = NULL;
                return NULL;
            }
            s.fmt = fmt;
            return &s;
        }
    }

    // Hepsi mesgul — legacy DirectSound davranisi gibi ses dusurulur
    return NULL;
}

//-----------------------------------------------------------------------------
// Vorbis FILE* callback'leri — OV_CALLBACKS_DEFAULT bu vorbisfile surumunde
// yok; ayrica vcpkg vorbis DLL'i farkli CRT kullandigi icin FILE* islemleri
// bizim CRT'mizde kalmali (ov_open yerine ov_open_callbacks zorunlu).
//-----------------------------------------------------------------------------
size_t OggCbRead(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    return fread(ptr, size, nmemb, (FILE*)datasource);
}
int OggCbSeek(void* datasource, ogg_int64_t offset, int whence)
{
    return _fseeki64((FILE*)datasource, offset, whence);
}
int OggCbClose(void* datasource)
{
    return fclose((FILE*)datasource);
}
long OggCbTell(void* datasource)
{
    return (long)_ftelli64((FILE*)datasource);
}
const ov_callbacks kOggFileCallbacks = { OggCbRead, OggCbSeek, OggCbClose, OggCbTell };

//-----------------------------------------------------------------------------
// Stream feeder thread
//-----------------------------------------------------------------------------
void StreamFeederProc(XA2Stream* s)
{
    while (!s->quitFlag.load())
    {
        if (!s->playing.load() || s->paused.load())
        {
            Sleep(20);
            continue;
        }

        XAUDIO2_VOICE_STATE state;
        s->pVoice->GetState(&state);

        bool submitted = false;
        while (state.BuffersQueued < kStreamBufCount && !s->quitFlag.load())
        {
            BYTE* pBuf = s->buffers[s->nextBuf];
            int filled = 0;
            while (filled < kStreamBufBytes)
            {
                int bitstream = 0;
                long n = ov_read(&s->vf, (char*)pBuf + filled,
                                 kStreamBufBytes - filled, 0, 2, 1, &bitstream);
                if (n > 0)
                {
                    filled += (int)n;
                }
                else if (n == 0)
                {
                    // EOF
                    if (s->loop)
                    {
                        if (ov_pcm_seek(&s->vf, 0) != 0)
                            break;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    CT_Done("XAudio2 stream: ov_read hata %ld", n);
                    break;
                }
            }

            if (filled <= 0)
            {
                // Non-loop bitti veya decode hatasi
                if (!s->loop)
                    s->ended.store(true);
                break;
            }

            XAUDIO2_BUFFER xb;
            memset(&xb, 0, sizeof(xb));
            xb.pAudioData = pBuf;
            xb.AudioBytes = (UINT32)filled;
            if (FAILED(s->pVoice->SubmitSourceBuffer(&xb)))
                break;

            s->nextBuf = (s->nextBuf + 1) % kStreamBufCount;
            submitted = true;
            s->pVoice->GetState(&state);
        }

        if (s->ended.load() && state.BuffersQueued == 0)
        {
            s->playing.store(false);
            continue;
        }

        // Buffer bosalana kadar bekle (event OnBufferEnd'de set edilir)
        if (!submitted || state.BuffersQueued >= kStreamBufCount)
            WaitForSingleObject(s->callback.hBufferEndEvent, 100);
    }
}

} // namespace

//-----------------------------------------------------------------------------
// XAudio2Backend
//-----------------------------------------------------------------------------

XAudio2Backend::XAudio2Backend() {}
XAudio2Backend::~XAudio2Backend() {}

XAudio2Backend& XAudio2Backend::Instance()
{
    static XAudio2Backend s_Inst;
    return s_Inst;
}

bool XAudio2Backend::IsSelected()
{
    static int s_Selected = -1;
    if (s_Selected < 0)
        s_Selected = ReadEngineSelection() ? 1 : 0;
    return s_Selected == 1;
}

bool XAudio2Backend::Init()
{
    if (g_bInitTried)
        return g_bInitOK;
    g_bInitTried = true;

    HRESULT hr = XAudio2Create(&g_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr) || g_pXAudio2 == NULL)
    {
        CT_Done("XAudio2: XAudio2Create FAILED 0x%08X", hr);
        g_pXAudio2 = NULL;
        return false;
    }

    hr = g_pXAudio2->CreateMasteringVoice(&g_pMasterVoice);
    if (FAILED(hr) || g_pMasterVoice == NULL)
    {
        CT_Done("XAudio2: CreateMasteringVoice FAILED 0x%08X", hr);
        g_pXAudio2->Release();
        g_pXAudio2 = NULL;
        g_pMasterVoice = NULL;
        return false;
    }

    g_bInitOK = true;
    CT_Done("XAudio2: init OK");
    return true;
}

void XAudio2Backend::Shutdown()
{
    StopAll();

    for (int i = 0; i < kMaxVoiceSlots; ++i)
    {
        VoiceSlot& s = g_Slots[i];
        if (s.pVoice != NULL)
        {
            s.pVoice->DestroyVoice();
            s.pVoice = NULL;
        }
        if (s.pSound != NULL)
        {
            ReleaseSound(s.pSound);
            s.pSound = NULL;
        }
        s.busy.store(false);
    }

    if (g_pMasterVoice != NULL)
    {
        g_pMasterVoice->DestroyVoice();
        g_pMasterVoice = NULL;
    }
    if (g_pXAudio2 != NULL)
    {
        g_pXAudio2->Release();
        g_pXAudio2 = NULL;
    }
    g_bInitTried = false;
    g_bInitOK = false;
}

bool XAudio2Backend::IsInit() const
{
    return g_bInitOK;
}

//-----------------------------------------------------------------------------
// SFX
//-----------------------------------------------------------------------------

XA2Sound* XAudio2Backend::LoadWavFile(const char* path)
{
    if (path == NULL || !g_bInitOK)
        return NULL;

    FILE* f = fopen(path, "rb");
    if (f == NULL)
    {
        CT_Done("XAudio2: wav acilamadi (%s)", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    const long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (fileSize <= 0)
    {
        fclose(f);
        return NULL;
    }

    std::vector<BYTE> fileData((size_t)fileSize);
    const size_t nRead = fread(&fileData[0], 1, (size_t)fileSize, f);
    fclose(f);
    if (nRead != (size_t)fileSize)
    {
        CT_Done("XAudio2: wav okuma eksik (%s) %u/%ld", path, (unsigned)nRead, fileSize);
        return NULL;
    }

    WAVEFORMATEX fmt;
    const BYTE* pPcm = NULL;
    size_t pcmBytes = 0;
    if (!ParseWav(&fileData[0], fileData.size(), &fmt, &pPcm, &pcmBytes, path))
        return NULL;

    XA2Sound* snd = new XA2Sound();
    snd->fmt = fmt;
    snd->pcm.assign(pPcm, pPcm + pcmBytes);
    snd->refCount = 1;
    return snd;
}

XA2Sound* XAudio2Backend::CreateFromPcm(const void* pcm, unsigned int bytes,
                                        unsigned int channels, unsigned int sampleRate,
                                        unsigned int bitsPerSample)
{
    if (pcm == NULL || bytes == 0 || !g_bInitOK)
        return NULL;
    if (channels < 1 || channels > 2 || sampleRate < 4000 ||
        (bitsPerSample != 8 && bitsPerSample != 16))
    {
        CT_Done("XAudio2: CreateFromPcm anormal format ch=%u rate=%u bits=%u",
                channels, sampleRate, bitsPerSample);
        return NULL;
    }

    XA2Sound* snd = new XA2Sound();
    memset(&snd->fmt, 0, sizeof(snd->fmt));
    snd->fmt.wFormatTag = WAVE_FORMAT_PCM;
    snd->fmt.nChannels = (WORD)channels;
    snd->fmt.nSamplesPerSec = sampleRate;
    snd->fmt.wBitsPerSample = (WORD)bitsPerSample;
    snd->fmt.nBlockAlign = (WORD)(channels * bitsPerSample / 8);
    snd->fmt.nAvgBytesPerSec = sampleRate * snd->fmt.nBlockAlign;
    snd->pcm.assign((const BYTE*)pcm, (const BYTE*)pcm + bytes);
    snd->refCount = 1;
    return snd;
}

int XAudio2Backend::GetSampleRate(const XA2Sound* snd) const
{
    return (snd != NULL) ? (int)snd->fmt.nSamplesPerSec : 0;
}

XA2Sound* XAudio2Backend::AddRefSound(XA2Sound* snd)
{
    if (snd != NULL)
        InterlockedIncrement(&snd->refCount);
    return snd;
}

void XAudio2Backend::ReleaseSound(XA2Sound* snd)
{
    if (snd == NULL)
        return;
    if (InterlockedDecrement(&snd->refCount) <= 0)
        delete snd;
}

unsigned int XAudio2Backend::Play(XA2Sound* snd, bool loop, float volumeLinear,
                                  float pan, float freqRatio)
{
    if (snd == NULL || !g_bInitOK || snd->pcm.empty())
        return 0;

    VoiceSlot* pSlot = AcquireSlot(snd->fmt);
    if (pSlot == NULL)
        return 0;   // havuz dolu — ses duser (legacy davranis)

    XAUDIO2_BUFFER xb;
    memset(&xb, 0, sizeof(xb));
    xb.pAudioData = &snd->pcm[0];
    xb.AudioBytes = (UINT32)snd->pcm.size();
    xb.Flags = XAUDIO2_END_OF_STREAM;
    if (loop)
        xb.LoopCount = XAUDIO2_LOOP_INFINITE;

    pSlot->pSound = AddRefSound(snd);
    pSlot->generation = (g_NextGeneration++) & 0xFFFF;
    if (pSlot->generation == 0)
        pSlot->generation = (g_NextGeneration++) & 0xFFFF;

    if (volumeLinear < 0.0f) volumeLinear = 0.0f;
    if (volumeLinear > 1.0f) volumeLinear = 1.0f;
    pSlot->pVoice->SetVolume(volumeLinear);
    ApplyPanMatrix(pSlot->pVoice, snd->fmt.nChannels, pan);
    if (freqRatio > 0.0f && freqRatio != 1.0f)
    {
        if (freqRatio > kMaxFreqRatio) freqRatio = kMaxFreqRatio;
        pSlot->pVoice->SetFrequencyRatio(freqRatio);
    }
    else
    {
        pSlot->pVoice->SetFrequencyRatio(1.0f);
    }

    pSlot->busy.store(true);
    HRESULT hr = pSlot->pVoice->SubmitSourceBuffer(&xb);
    if (FAILED(hr))
    {
        CT_Done("XAudio2: SubmitSourceBuffer FAILED 0x%08X", hr);
        pSlot->busy.store(false);
        ReleaseSound(pSlot->pSound);
        pSlot->pSound = NULL;
        return 0;
    }
    pSlot->pVoice->Start(0);

    const int slotIndex = (int)(pSlot - g_Slots);
    return MakeHandle(slotIndex, pSlot->generation);
}

void XAudio2Backend::Stop(unsigned int handle)
{
    VoiceSlot* s = SlotFromHandle(handle);
    if (s == NULL || s->pVoice == NULL)
        return;
    s->pVoice->Stop(0);
    s->pVoice->FlushSourceBuffers();
    s->busy.store(false);
    s->pVoice->Start(0);   // voice hazir kalsin (reuse)
}

bool XAudio2Backend::IsPlaying(unsigned int handle) const
{
    VoiceSlot* s = SlotFromHandle(handle);
    return (s != NULL && s->busy.load());
}

void XAudio2Backend::SetVolume(unsigned int handle, float volumeLinear)
{
    VoiceSlot* s = SlotFromHandle(handle);
    if (s == NULL || s->pVoice == NULL)
        return;
    if (volumeLinear < 0.0f) volumeLinear = 0.0f;
    if (volumeLinear > 1.0f) volumeLinear = 1.0f;
    s->pVoice->SetVolume(volumeLinear);
}

void XAudio2Backend::SetPan(unsigned int handle, float pan)
{
    VoiceSlot* s = SlotFromHandle(handle);
    if (s == NULL || s->pVoice == NULL)
        return;
    ApplyPanMatrix(s->pVoice, s->fmt.nChannels, pan);
}

void XAudio2Backend::SetFreqRatio(unsigned int handle, float ratio)
{
    VoiceSlot* s = SlotFromHandle(handle);
    if (s == NULL || s->pVoice == NULL)
        return;
    if (ratio <= 0.0f) ratio = 1.0f;
    if (ratio > kMaxFreqRatio) ratio = kMaxFreqRatio;
    s->pVoice->SetFrequencyRatio(ratio);
}

bool XAudio2Backend::SetLooping(unsigned int handle, bool loop)
{
    VoiceSlot* s = SlotFromHandle(handle);
    if (s == NULL || s->pVoice == NULL)
        return false;

    if (!loop)
    {
        // Mevcut loop turu bitince dursun
        s->pVoice->ExitLoop();
        return true;
    }

    // Loop acma: XAudio2 submit edilmis buffer'a loop ekleyemez —
    // caliyorsa bastan loop'lu resubmit (nadir yol, SoLoud setLooping paritesi)
    if (s->pSound == NULL || s->pSound->pcm.empty())
        return false;

    s->pVoice->Stop(0);
    s->pVoice->FlushSourceBuffers();

    XAUDIO2_BUFFER xb;
    memset(&xb, 0, sizeof(xb));
    xb.pAudioData = &s->pSound->pcm[0];
    xb.AudioBytes = (UINT32)s->pSound->pcm.size();
    xb.Flags = XAUDIO2_END_OF_STREAM;
    xb.LoopCount = XAUDIO2_LOOP_INFINITE;

    s->busy.store(true);
    if (FAILED(s->pVoice->SubmitSourceBuffer(&xb)))
    {
        s->busy.store(false);
        return false;
    }
    s->pVoice->Start(0);
    return true;
}

void XAudio2Backend::StopAll()
{
    for (int i = 0; i < kMaxVoiceSlots; ++i)
    {
        VoiceSlot& s = g_Slots[i];
        if (s.pVoice != NULL && s.busy.load())
        {
            s.pVoice->Stop(0);
            s.pVoice->FlushSourceBuffers();
            s.busy.store(false);
            s.pVoice->Start(0);
        }
    }
}

//-----------------------------------------------------------------------------
// BGM stream
//-----------------------------------------------------------------------------

XA2Stream* XAudio2Backend::OpenOggStream(const char* path, bool loop)
{
    if (path == NULL || !g_bInitOK)
        return NULL;

    FILE* f = fopen(path, "rb");
    if (f == NULL)
    {
        CT_Done("XAudio2 stream: ogg acilamadi (%s)", path);
        return NULL;
    }

    XA2Stream* s = new XA2Stream();
    s->pFile = f;
    s->loop = loop;

    if (ov_open_callbacks(f, &s->vf, NULL, 0, kOggFileCallbacks) != 0)
    {
        CT_Done("XAudio2 stream: ov_open FAILED (%s)", path);
        fclose(f);
        s->pFile = NULL;
        delete s;
        return NULL;
    }

    vorbis_info* vi = ov_info(&s->vf, -1);
    if (vi == NULL || vi->channels < 1 || vi->channels > 2)
    {
        CT_Done("XAudio2 stream: vorbis_info anormal (%s)", path);
        ov_clear(&s->vf);   // ov_clear FILE*'i da kapatir
        s->pFile = NULL;
        delete s;
        return NULL;
    }

    s->fmt.wFormatTag = WAVE_FORMAT_PCM;
    s->fmt.nChannels = (WORD)vi->channels;
    s->fmt.nSamplesPerSec = (DWORD)vi->rate;
    s->fmt.wBitsPerSample = 16;
    s->fmt.nBlockAlign = (WORD)(s->fmt.nChannels * 2);
    s->fmt.nAvgBytesPerSec = s->fmt.nSamplesPerSec * s->fmt.nBlockAlign;

    s->callback.hBufferEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (s->callback.hBufferEndEvent == NULL)
    {
        ov_clear(&s->vf);
        s->pFile = NULL;
        delete s;
        return NULL;
    }

    HRESULT hr = g_pXAudio2->CreateSourceVoice(&s->pVoice, &s->fmt, 0,
                                               2.0f, &s->callback);
    if (FAILED(hr) || s->pVoice == NULL)
    {
        CT_Done("XAudio2 stream: CreateSourceVoice FAILED 0x%08X (%s)", hr, path);
        CloseHandle(s->callback.hBufferEndEvent);
        ov_clear(&s->vf);
        s->pFile = NULL;
        delete s;
        return NULL;
    }

    // Feeder thread — playing=false ile bekler
    s->feeder = std::thread(StreamFeederProc, s);
    return s;
}

void XAudio2Backend::CloseStream(XA2Stream* s)
{
    if (s == NULL)
        return;

    s->quitFlag.store(true);
    if (s->callback.hBufferEndEvent != NULL)
        SetEvent(s->callback.hBufferEndEvent);
    if (s->feeder.joinable())
        s->feeder.join();

    if (s->pVoice != NULL)
    {
        s->pVoice->Stop(0);
        s->pVoice->FlushSourceBuffers();
        s->pVoice->DestroyVoice();
        s->pVoice = NULL;
    }
    if (s->callback.hBufferEndEvent != NULL)
    {
        CloseHandle(s->callback.hBufferEndEvent);
        s->callback.hBufferEndEvent = NULL;
    }
    ov_clear(&s->vf);   // FILE*'i da kapatir
    s->pFile = NULL;
    delete s;
}

bool XAudio2Backend::StreamPlay(XA2Stream* s, float volumeLinear)
{
    if (s == NULL || s->pVoice == NULL)
        return false;

    StreamSetVolume(s, volumeLinear);
    s->ended.store(false);
    s->paused.store(false);
    s->playing.store(true);
    s->pVoice->Start(0);
    if (s->callback.hBufferEndEvent != NULL)
        SetEvent(s->callback.hBufferEndEvent);   // feeder'i uyandir
    return true;
}

void XAudio2Backend::StreamStop(XA2Stream* s)
{
    if (s == NULL || s->pVoice == NULL)
        return;
    s->playing.store(false);
    s->pVoice->Stop(0);
    s->pVoice->FlushSourceBuffers();
    ov_pcm_seek(&s->vf, 0);
    s->nextBuf = 0;
    s->ended.store(false);
}

void XAudio2Backend::StreamPause(XA2Stream* s, bool pause)
{
    if (s == NULL || s->pVoice == NULL)
        return;
    s->paused.store(pause);
    if (pause)
        s->pVoice->Stop(0);
    else
    {
        s->pVoice->Start(0);
        if (s->callback.hBufferEndEvent != NULL)
            SetEvent(s->callback.hBufferEndEvent);
    }
}

void XAudio2Backend::StreamSetVolume(XA2Stream* s, float volumeLinear)
{
    if (s == NULL || s->pVoice == NULL)
        return;
    if (volumeLinear < 0.0f) volumeLinear = 0.0f;
    if (volumeLinear > 1.0f) volumeLinear = 1.0f;
    s->pVoice->SetVolume(volumeLinear);
}

bool XAudio2Backend::StreamIsPlaying(XA2Stream* s) const
{
    return (s != NULL && s->playing.load());
}

} // namespace audio
} // namespace phoenix
