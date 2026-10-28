# Visual Studio ile Client derleme

Bu kılavuz, DK Project Phoenix Client kaynaklarını Visual Studio 2022 veya daha yeni bir sürümle başka bir Windows bilgisayarında derlemek içindir.

> **Önemli:** `F:\Project Phoenix\build` içindeki `.sln`, `.vcxproj`, CMake cache dosyaları ve ara çıktılar mevcut bilgisayara ait yollar içerir. Bunlar taşınabilir proje girdileri değildir. Kaynak kod ve taşınabilir CMake/vcpkg tanımları ana depodaki [`Client/`](https://github.com/dkcommunity/DK-Project-Phoenix/tree/main/Client) klasöründedir. Her bilgisayarda CMake derleme klasörünü yeniden oluşturun.

## Gereksinimler

- Windows 10 veya 11
- Visual Studio 2022 veya daha yeni sürüm; **Desktop development with C++** iş yükü
- MSVC C++ derleyicisi ve Windows 10/11 SDK
- CMake 3.20 veya daha yeni sürüm
- Git
- vcpkg

Proje Win32/x86 hedefler ve bağımlılıklarını `Client/vcpkg.json` manifestinden kurar.

## Kaynağı ve bağımlılıkları hazırlama

PowerShell'de kaynak depoyu klonlayın:

```powershell
git clone https://github.com/dkcommunity/DK-Project-Phoenix.git C:\src\DK-Project-Phoenix
Set-Location C:\src\DK-Project-Phoenix\Client
$clientDir = (Get-Location).Path
```

vcpkg mevcut değilse kurup bu PowerShell oturumu için tanımlayın:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\src\vcpkg
Set-Location C:\src\vcpkg
.\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = 'C:\src\vcpkg'
Set-Location $clientDir
```

vcpkg zaten kuruluysa `VCPKG_ROOT` değişkenini mevcut kurulum dizininize ayarlayın.

Projenin SoLoud kaynak kodunu beklediği konuma sabit sürüm etiketiyle indirin:

```powershell
git clone https://github.com/jarikomppa/soloud.git .\thirdparty\soloud
git -C .\thirdparty\soloud checkout RELEASE_20200207
```

## Visual Studio çözümünü oluşturma ve derleme

Aşağıdaki örnek, Visual Studio 2022 için Win32 çözümünü oluşturur. CMake, vcpkg manifestindeki paketleri yapılandırma sırasında kurar:

```powershell
cmake -S . -B build `
  -G "Visual Studio 17 2022" `
  -A Win32 `
  "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  -DVCPKG_TARGET_TRIPLET=x86-windows

cmake --build build --config Release --target Client --parallel
```

Visual Studio'da `Client` klasörünü **Open > Folder** ile açarak CMake projesini kullanabilir veya oluşturulan `build\DarkEdenLegacyModern.sln` çözümünü açabilirsiniz. Daha yeni Visual Studio sürümlerinde, kurulu sürümün desteklediği CMake jeneratörünü seçin; örneğin ilgili `Visual Studio <sürüm> <yıl>` jeneratörü.

Çalıştırılabilir dosya ve çalışma zamanı DLL'leri CMake yapılandırmasına göre `Client\GameData` altına yazılır.

## Bilinen eksik bağımlılık: DirectX 7 SDK

Bu kaynak ağacında `Client\thirdparty\dx7sdk` bulunmuyor. CMake yapılandırması bu SDK yokken uyarı verir; ancak eski DirectX 7 başlıklarına veya kütüphanelerine ihtiyaç duyan kaynaklar derlemede hata verebilir. Bu özel depoya SDK eklenmemiştir; derlemenin tamamlanması için lisanslı ve uyumlu SDK dosyalarının ayrı olarak edinilip beklenen konuma yerleştirilmesi gerekebilir.

Bu nedenle yukarıdaki adımlar taşınabilir CMake/vcpkg kurulumunu sağlar, fakat eksik DirectX 7 dosyaları temin edilmeden temiz derlemenin başarılı olduğu garanti edilmez.

## `build` klasöründen yüklenmeyenler

`F:\Project Phoenix\build` içindeki CMake önbelleği, Visual Studio çözüm/proje çıktıları, `.vs`, `CMakeFiles`, `vcpkg_installed`, nesne dosyaları, derleme günlükleri ve önceden derlenmiş ikililer yerel makineye veya belirli bir derleme yapılandırmasına bağlıdır. Bunları başka bilgisayarda derleme girdisi olarak kullanmayın; her bilgisayarda kaynaklardan yeniden oluşturun.
