//-----------------------------------------------------------------------------
// D3D11SpriteCommands.h
//
// FULL DX11 gecis Faz G1 (2026-09-01) — Blt* komut katmani ("dx11gpu" modu).
//
// Sorun: dx11gpu modunda basit sprite yollari GPU quad olarak cizilirken,
// henuz portlanmamis yollar (IndexSprite, effect blend, GDI text, raw
// GetSurfacePointer) CPU composition DIB'ine yazmaya devam eder. Iki dunya
// arasindaki CIZIM SIRASI korunmazsa z-order bozulur (or. isim yazisi
// tile quad'inin altinda kalir).
//
// Cozum — frame-sirali komut listesi + checkpoint segment'leri:
//   - Frame basinda composition'in kopyasi alinir: BASELINE (taban katman,
//     legacy "composition kalicidir" semantigi) + PREV (son checkpoint
//     aninindaki durum).
//   - CPU yollari composition'a yazar ve MarkDirty(rect) ile bolgeyi bildirir.
//   - GPU quad kaydi geldiginde dirty varsa once CHECKPOINT: composition'in
//     PREV'den FARKLI pikselleri (o checkpoint'ten beri CPU'nun yazdiklari)
//     alpha=255 olarak RGBA segment texture'ina cikarilir, PREV guncellenir,
//     listeye SEGMENT komutu eklenir. Sonra QUAD komutu eklenir.
//   - Present: komutlar SIRAYLA (segment = alpha'li fullscreen quad, quad =
//     sprite texture'i) KALICI accumulation RT'ye cizilir, o da backbuffer'a
//     opak basilir.
//   - Segment butcesi asilirsa (checkpoint firtinasi) frame OVERFLOW moduna
//     gecer: kalan tum cizimler CPU'ya duser (dogruluk korunur, hiz duser),
//     frame sonu final segment hepsini toplar.
//
// 2026-09-01 KALICI BACKBUFFER SEMANTIGI (accumulation RT):
//   Legacy ekranlarin cogu (login, server select, char list) arka plani BIR
//   KEZ cizer ve backbuffer'in frame'ler arasi kalici olduguna guvenir.
//   dx11gpu'da GPU'ya yonlenen Blt'ler composition'a yazilmadigi icin bu
//   icerik yalniz cizildigi frame'de var olur — ekran yeniden cizilmeyen
//   frame'de arka plan kaybolur (siyah/karanlik ekran + flicker).
//   Cozum: komutlar dogrudan backbuffer'a degil, frame'ler arasi YASAYAN
//   accumulation render target'ina cizilir (ilk frame'de composition ile
//   tohumlanir). Backbuffer her Present'ta accum'un opak kopyasini alir —
//   legacy "kalici backbuffer" davranisinin birebir GPU karsiligi.
//
// Bu katman sprite formatini BILMEZ — CSpriteSurface.cpp decode edip
// D3D11SpriteTextureCache'e RGBA verir, buraya SRV + rect + renk gelir.
// dx11gpu disinde tum API'ler no-op/false dondurur; d3d9 ve dx11 modlari
// bit degismeden calisir.
//-----------------------------------------------------------------------------

#ifndef __PHOENIX_D3D11_SPRITE_COMMANDS_H__
#define __PHOENIX_D3D11_SPRITE_COMMANDS_H__

#include <windows.h>
#include "D3D11QuadPipeline.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace phoenix
{
namespace render
{

class D3D11SpriteCommands
{
public:
    static D3D11SpriteCommands& Instance();

    // Backend Init sonrasi (dx11gpu modunda) kurulur.
    //   pCompBits/pitch/w/h: composition DIB bilgisi (R5G6B5)
    bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
              void* pCompBits, int compPitchBytes, int width, int height);
    void Shutdown();

    // Kayit yapilabilir mi? (dx11gpu + init OK + frame overflow degil)
    bool IsActive() const;

    // Verilen pointer composition DIB'inin pixel base'i mi?
    // (CSpriteSurface hook'lari yalniz backsurface'ta devreye girer;
    //  offscreen/chrome-cache surface'lari CPU yolunda kalir.)
    bool IsCompositionTarget(const void* pBits) const
    {
        return m_pCompBits != NULL && pBits == m_pCompBits;
    }

    // CPU yolu composition'a yazdi — bolgeyi isaretle (pRect NULL = tum ekran).
    // Ekran disina tasan rect'ler kirpilir.
    void MarkDirty(const RECT* pRect);

    // 2026-09-03 flicker fix — world frame'i CIZIMIN BASINDA isaretle.
    // MTopView::DrawZone girisinden (facade GPUWorldFrame_Begin uzerinden)
    // cagrilir. Onceki tasarimda bayrak world ciziminin SONUNDA
    // (RecordWorldBoundary/RecordLightFilter) set ediliyordu; segment
    // butcesi world cizimi SIRASINDA dolarsa ara flush baseline karari
    // yanlis veriyordu (04:32 build: baseline atlandi -> sprite izleri;
    // 05:00 build'deki prev-frame devri de world CIZILMEYEN ara Present
    // frame'lerinde baseline'i accum ustune bastirip GPU icerigini sildi ->
    // genel flicker). Dogru cozum: bayragi ilk world komutundan once set
    // etmek — baseline yalniz GERCEK world frame'lerinde, ilk flush'ta.
    void MarkWorldFrame()
    {
        if (m_bInit)
            m_bWorldFrame = true;
    }

    // 2026-09-03 R6 (minimap tasima titremesi) — bolgenin HER FRAME segment'e
    // cikmasini zorla: PREV snapshot'i bolgede gecersiz kilinir (comp'un
    // tersi yazilir) + MarkDirty. Diff mekanizmasi "degismeyen" CPU icerigini
    // emit etmeyip BASELINE kalicihgina guvenir; ama world frame'lerinde
    // tasima invalidation'i tile quad'larini baseline USTUNE cizdirir ->
    // degismeyen CPU icerigi (minimap harita alani gibi comp'a raw Blt/poke
    // ile yazilanlar) o frame'lerde gorunmez kalir (yanip sonme). ForceDirty
    // ile icerik her frame diff'e yakalanir; z-order checkpoint kurallariyla
    // korunur (ustune cizen pencere quad'i kaydedilmeden once segment cikar).
    void ForceDirty(const RECT* pRect);

    // GPU quad kaydet. Gerekirse once checkpoint segment cikarir.
    // false = kayit yapilamadi (aktif degil / overflow) → caller CPU yoluna
    // duser (ve MarkDirty cagirmalidir).
    bool RecordQuad(ID3D11ShaderResourceView* pSRV, int texW, int texH,
                    const RECT& dest, const RECT& src,
                    DWORD colorRGBA, D3D11QuadPipeline::BlendMode blend);

    // Solid renk quad'i (BltColorAlpha) — ayni checkpoint kurallari.
    bool RecordSolidQuad(const RECT& dest, DWORD colorRGBA,
                         D3D11QuadPipeline::BlendMode blend);

    // 2026-09-02 cursor-iz fix — composition tam-ekran fill'inin
    // (CDirectDrawSurface::FillSurface, or. CWaitUIUpdate'in her frame
    // "[ANTIGRAVITY] ghosting" temizligi) SIRALI GPU karsiligi.
    //
    // Sorun: kalici accum RT'de "ayni renge temizle" diff tabanli
    // checkpoint'e HIC yansimaz (composition onceki frame'de de o renkti →
    // diff bos) → menu frame'lerinde onceki frame'lerin sprite quad'lari
    // (or. mouse cursor) accum'da birikir. 1280x960'ta tam ekran artwork
    // her frame ustunu orttugu icin gorunmezdi; 1920x1080'de artwork disi
    // bolgede cursor izi olarak ortaya cikti.
    //
    // Yapilan: (1) fullscreen opak solid quad kaydedilir (accum'daki eski
    // birikim SIRALI olarak silinir), (2) onceki dirty birikimi atilir
    // (fill zaten ustunu ortecekti — legacy parite), (3) PREV snapshot fill
    // rengine resetlenir: accum silindigi icin CPU'nun bu frame yeniden
    // cizdigi ayni icerik (text vb.) diff'te tekrar cikmali; prev eski
    // kalsaydi "degismedi" sayilir, icerik kaybolurdu. Caller composition'i
    // ayni renge CPU-fill etmeye DEVAM eder (comp == prev tutarliligi +
    // raw readback paritesi). false = kayit yok → caller eski MarkDirty +
    // CPU fill yoluna duser.
    bool RecordClear(WORD color565);

    // Faz G4 (2026-09-01) — gece isik filtresi, SIRALI komut olarak.
    // MTopView world cizimini bitirip UI'dan ONCE submit eder; bu noktaya
    // kadar kaydedilen her sey (baseline + world segment/quad'lari)
    // BLEND_MULTIPLY fullscreen quad ile karartilir, sonrasi (UI/text)
    // dokunulmaz — legacy "filtre once, UI ustune" sirasinin GPU karsiligi.
    //   cells4 : fw*fh grid, her hucrede 4-bit karartma nibble'i (0-15)
    //   destW/H: karartilacak clip alani (CLIPSURFACE boyutu)
    // Bos grid'de true doner (karartma yok = basarili no-op). false =
    // kayit yapilamadi (aktif degil / segment butcesi bitti) — caller bu
    // frame karartmayi atlar (CPU apply dx11gpu'da composition'i bozar).
    bool RecordLightFilter(const BYTE* cells4, int fw, int fh,
                           int destW, int destH);

    // Faz G5 (2026-09-01) — world/UI siniri checkpoint'i. MTopView world
    // cizimini bitirdigi anda (GPULightFilter_Deactivate) cagrilir:
    // composition'daki TUM CPU birikimi (isim yazilari, bubble text,
    // hook'suz yollar) fullscreen diff ile segment'e cikar → world icerigi
    // UI quad'larinin ALTINDA katmanlanir ve gece LIGHT karartmasina girer.
    // Degisen pixel yoksa segment slotu tuketilmez (EmitSegment atlar).
    bool RecordWorldBoundary();

    // Present: baseline + komutlar sirayla cizilir. false = bu frame komut
    // katmani kullanilamadi (caller normal CPU-composition present yapmali).
    // brightness: Option menu "Aydinlatma" (gamma) — accum->backbuffer final
    // kopyasina modulate renk olarak uygulanir (0.5..1.0, 1.0 = notr).
    // D3D9 windowed post-process brightness paritesi (2026-09-01).
    bool RenderFrame(ID3D11RenderTargetView* pRTV, float brightness = 1.0f);

    // Present sonu: bir sonraki frame'in baseline/prev snapshot'ini al,
    // komut listesini sifirla. Iconic-skip frame'lerinde de cagrilir.
    void ResetFrame();

    // Istatistik (HUD / Dev_Log dogrulama)
    int GetQuadCount() const    { return m_QuadsThisFrame; }
    int GetSegmentCount() const { return m_SegmentsUsed; }
    bool WasOverflow() const    { return m_bOverflow; }

    // impl .cpp'de tanimlanir (std::vector header'a sizmasin);
    // public: cpp'deki namespace-scope vector typedef'i erisebilsin diye.
    struct Cmd;

private:
    D3D11SpriteCommands();
    ~D3D11SpriteCommands();
    D3D11SpriteCommands(const D3D11SpriteCommands&);
    D3D11SpriteCommands& operator=(const D3D11SpriteCommands&);

    // Dirty bolgeyi PREV ile karsilastirip segment texture'ina cikar.
    // false = segment butcesi bitti (caller overflow moduna gecirir).
    bool EmitSegment();

    // 2026-09-03 R4 — ara flush: birikmis komutlar (gerekirse baseline
    // dahil) accum RT'ye HEMEN cizilir, komut listesi bosaltilir ve segment
    // ring'i ayni frame icinde yeniden kullanilabilir olur. Boylece segment
    // butcesi checkpoint firtinasinda "frame ortasi CPU fallback" (overflow)
    // uretmez. Overflow'un yan etkisi kullanici-gorunur siyah bolgelerdi:
    // CPU fallback'e dusen statik menu icerigi sonraki frame'lerin
    // fullscreen diff'ine yakalanmiyor (comp==prev), yalniz BASELINE'da
    // kaliyor ve world quad'lari onu ortuyordu (dump'suz analiz,
    // Dev_Log 2026-09-03 R4). false = flush yapilamadi (init/pipeline yok).
    bool FlushToAccum();

    void SnapshotComposition();   // comp → baseline + prev

    // 2026-09-01: 12 -> 24 (oyun ici cok-pencere sahnelerinde 11 segment +
    // overflow olculdu; kesisim-testli erteleme ile birlikte guvenli tavan).
    // 2026-09-03 R4: 24 -> 8. FlushToAccum ile ring ayni frame'de yeniden
    // kullanilabildigi icin buyuk havuz gereksiz; 1920x1080'de segment
    // basina ~8MB dynamic texture — 24 slot 32-bit VA'da ~200MB tutuyordu
    // (R1 bad_alloc analizindeki adres alani baskisinin bir parcasi).
    enum { kMaxSegments = 8 };

    void* m_pCmds;                // std::vector<Cmd>*

    ID3D11Device*             m_pDevice;
    ID3D11DeviceContext*      m_pContext;

    // Composition kaynagi
    void*                     m_pCompBits;
    int                       m_CompPitch;
    int                       m_Width;
    int                       m_Height;

    // CPU snapshot'lar (R5G6B5, pitch = m_Width*2)
    WORD*                     m_pBaseline;
    WORD*                     m_pPrev;

    // 2026-09-03 FPS fix #2 — segment diff cikisi icin CPU scratch (RGBA8,
    // pitch = m_Width*4). EmitSegment diff'i buraya yazar ve YALNIZ degisen
    // piksellerin siki sinir kutusu UpdateSubresource ile GPU'ya gider.
    // Onceki tasarim her emit'te fullscreen dynamic texture'i DISCARD ile
    // map ediyordu: frame basina 12-17 segment x ~8MB rename tahsisi
    // surucunun rename havuzunu tuketip periyodik stall uretiyordu
    // (SCPERF kaniti: kotu saniyelerde blMs ~250 / flMs ~690 / rfMs ~376).
    DWORD*                    m_pSegScratch;

    // GPU kaynaklar
    ID3D11Texture2D*          m_pBaselineTex;   // RGBA8 dynamic (accum tohumu)
    ID3D11ShaderResourceView* m_pBaselineSRV;
    ID3D11Texture2D*          m_pSegTex[kMaxSegments];
    ID3D11ShaderResourceView* m_pSegSRV[kMaxSegments];

    // 2026-09-01 — kalici accumulation RT (legacy backbuffer semantigi).
    // Komutlar buraya cizilir; backbuffer her Present'ta bunun kopyasini
    // alir. Ilk RenderFrame'de composition baseline'i ile tohumlanir.
    ID3D11Texture2D*          m_pAccumTex;
    ID3D11RenderTargetView*   m_pAccumRTV;
    ID3D11ShaderResourceView* m_pAccumSRV;
    bool                      m_bAccumSeeded;

    // 2026-09-01 sprite-iz fix'i: world frame'i (MTopView zone cizimi —
    // RecordWorldBoundary/RecordLightFilter caqrildi) menu frame'inden
    // ayirir. World frame'de accum BASELINE ile sifirlanir (legacy: oyun
    // ici her frame composition tabaniyla bastan cizilir — GPU sprite
    // izleri silinir). Menu frame'inde accum kalici kalir (bir-kez-cizilen
    // arka planlar kaybolmaz). ResetFrame'de false'a doner.
    bool                      m_bWorldFrame;

    // Faz G4 — isik grid texture'i (RGBA8, keep-faktoru gri olarak; linear
    // sample edilir, BLEND_MULTIPLY ile karartir)
    ID3D11Texture2D*          m_pLightTex;
    ID3D11ShaderResourceView* m_pLightSRV;
    int                       m_LightTexW;
    int                       m_LightTexH;

    // Frame durumu
    bool                      m_bDirty;
    RECT                      m_DirtyRect;
    int                       m_SegmentsUsed;
    int                       m_QuadsThisFrame;
    bool                      m_bOverflow;
    // 2026-09-03 R4 — baseline bu frame accum'a cizildi mi (ilk
    // FlushToAccum cagirisinda karar verilir, frame'de en fazla bir kez)
    bool                      m_bBaselineApplied;
    bool                      m_bInit;
};

} // namespace render
} // namespace phoenix

#endif // __PHOENIX_D3D11_SPRITE_COMMANDS_H__
