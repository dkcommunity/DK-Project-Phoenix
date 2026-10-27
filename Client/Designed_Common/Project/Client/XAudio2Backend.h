//-----------------------------------------------------------------------------
// XAudio2Backend.h
//
// FULL DX11 gecis Faz S1 (2026-09-01) — gercek XAudio2 ses backend'i.
//
// Kullanici karari: SoLoud yerine XAudio2 (E:'deki Reborn hedef mimarisi;
// Reborn'daki XAudio2Backend iskelet oldugu icin burada sifirdan gercek
// implementasyon). SoLoud kodu SILINMEZ — audio_engine.cfg ("xaudio2" /
// "soloud") ile secilir, regresyonda aninda geri donus (kural 4.1).
//
// Mimari:
//   SFX : XA2Sound (RIFF WAV parse → PCM + WAVEFORMATEX, refcount'lu)
//         + source-voice havuzu (format uyumlu slot reuse, OnBufferEnd ile
//         bosalan slot isaretlenir). Handle = slot<<16 | generation.
//   BGM : XA2Stream (OGG → libvorbis ov_read, 4'lu ring buffer, feeder
//         thread; loop/pause/volume). S3'te BGMManager buna baglanir.
//
// Cagiran katman (DXLib\CDirectSound.cpp S2'de) PhoenixSoundEntry
// sozlesmesini korur — ~256 PlaySound call-site'i dokunulmaz.
//-----------------------------------------------------------------------------

#ifndef __XAUDIO2_BACKEND_H__
#define __XAUDIO2_BACKEND_H__

namespace phoenix {
namespace audio {

// Opak tipler — XAudio2/vorbis header'lari bu dosyaya sizmasin
struct XA2Sound;
struct XA2Stream;

class XAudio2Backend
{
public:
    static XAudio2Backend& Instance();

    // audio_engine.cfg / PHOENIX_AUDIO_ENGINE "xaudio2" mi? (init denemez)
    static bool IsSelected();

    bool Init();        // IXAudio2 + mastering voice (idempotent)
    void Shutdown();
    bool IsInit() const;

    //-------------------------------------------------------------------
    // SFX — parse edilmis WAV + voice havuzu
    //-------------------------------------------------------------------
    XA2Sound*    LoadWavFile(const char* path);       // RIFF parse (PCM/float)
    // Ham PCM'den ses (CDirectSound::CreateBuffer paritesi — 8/16 bit)
    XA2Sound*    CreateFromPcm(const void* pcm, unsigned int bytes,
                               unsigned int channels, unsigned int sampleRate,
                               unsigned int bitsPerSample);
    int          GetSampleRate(const XA2Sound* snd) const;
    XA2Sound*    AddRefSound(XA2Sound* snd);
    void         ReleaseSound(XA2Sound* snd);

    // volumeLinear 0..1, pan -1..+1, freqRatio 1.0 = orijinal hiz.
    // Donus: handle (0 = fail).
    unsigned int Play(XA2Sound* snd, bool loop, float volumeLinear,
                      float pan, float freqRatio);
    void         Stop(unsigned int handle);
    bool         IsPlaying(unsigned int handle) const;
    void         SetVolume(unsigned int handle, float volumeLinear);
    void         SetPan(unsigned int handle, float pan);
    void         SetFreqRatio(unsigned int handle, float ratio);
    // loop=false: ExitLoop (mevcut tur bitince durur). loop=true ve caliyorsa:
    // bastan loop'lu resubmit (XAudio2 submit sonrasi loop acamaz — nadir yol).
    bool         SetLooping(unsigned int handle, bool loop);
    void         StopAll();

    //-------------------------------------------------------------------
    // BGM — OGG streaming
    //-------------------------------------------------------------------
    XA2Stream*   OpenOggStream(const char* path, bool loop);
    void         CloseStream(XA2Stream* stream);
    bool         StreamPlay(XA2Stream* stream, float volumeLinear);
    void         StreamStop(XA2Stream* stream);
    void         StreamPause(XA2Stream* stream, bool pause);
    void         StreamSetVolume(XA2Stream* stream, float volumeLinear);
    bool         StreamIsPlaying(XA2Stream* stream) const;

private:
    XAudio2Backend();
    ~XAudio2Backend();
    XAudio2Backend(const XAudio2Backend&);
    XAudio2Backend& operator=(const XAudio2Backend&);
};

} // namespace audio
} // namespace phoenix

#endif // __XAUDIO2_BACKEND_H__
