# ==========================================================================
# Project Phoenix — CMake Configure Script
# --------------------------------------------------------------------------
# Desktop Commander tarafından çağrılmak için yazıldı. VS Dev Shell'i import
# eder, cmake configure'u vcpkg toolchain ile çalıştırır ve tüm çıktıyı
# build\configure.log'a yazar.
#
# Kullanım:
#   powershell -NoProfile -ExecutionPolicy Bypass -File scripts\dc-configure.ps1
#   powershell -NoProfile -ExecutionPolicy Bypass -File scripts\dc-configure.ps1 -Clean
#
# -Clean flag'i verilmezse build/ klasörü KORUNUR ve incremental rebuild
# mümkün olur. CMake cache'ini kendi yönetir, build artifact'lar kalır.
# ==========================================================================

param(
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'

$projectRoot = 'F:\Project Phoenix'
$buildDir    = Join-Path $projectRoot 'build'
$vsRoot      = 'C:\Program Files\Microsoft Visual Studio\2022\Community'
$cmakeExe    = Join-Path $vsRoot 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
$toolchain   = 'D:/DarkEden_Phoenix/vcpkg/scripts/buildsystems/vcpkg.cmake'

Write-Host "[dc-configure] Project root: $projectRoot"
Write-Host "[dc-configure] cmake        : $cmakeExe"
Write-Host "[dc-configure] toolchain    : $toolchain"
Write-Host "[dc-configure] Clean mode   : $Clean"

# Build klasörü — sadece -Clean istenirse siliniyor. Aksi hâlde korunuyor
# ki incremental build mümkün olsun.
if ($Clean -and (Test-Path $buildDir)) {
    Write-Host "[dc-configure] -Clean geldi, build/ klasörü siliniyor..."
    Remove-Item $buildDir -Recurse -Force
}
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# VS Dev Shell ortamını import et (x86 hedef, x64 host)
Import-Module (Join-Path $vsRoot 'Common7\Tools\Microsoft.VisualStudio.DevShell.dll')
Enter-VsDevShell `
    -VsInstallPath $vsRoot `
    -DevCmdArguments '-arch=x86 -host_arch=x64' `
    -SkipAutomaticLocation | Out-Null

Set-Location $buildDir
$configLog = Join-Path $buildDir 'configure.log'
$errLog    = Join-Path $buildDir 'configure.err'

Write-Host "[dc-configure] CMake configure başlatılıyor..."
# Tek string olarak argüman geçiyoruz ki Start-Process tırnakları korusun;
# aksi halde array parse'ı "Visual Studio 17 2022" kelimesini parçalıyor.
$cmakeArgs = '.. -G "Visual Studio 17 2022" -A Win32 ' +
             "-DCMAKE_TOOLCHAIN_FILE=`"$toolchain`" " +
             '-DVCPKG_TARGET_TRIPLET=x86-windows'

$proc = Start-Process `
    -FilePath $cmakeExe `
    -ArgumentList $cmakeArgs `
    -Wait -NoNewWindow -PassThru `
    -RedirectStandardOutput $configLog `
    -RedirectStandardError  $errLog

Write-Host "[dc-configure] Exit code: $($proc.ExitCode)"
Write-Host "[dc-configure] stdout log: $configLog ($((Get-Item $configLog).Length) bytes)"
Write-Host "[dc-configure] stderr log: $errLog ($((Get-Item $errLog).Length) bytes)"

# Kısa özet
if ($proc.ExitCode -ne 0) {
    Write-Host ''
    Write-Host '=== STDERR (son 40 satır) ==='
    Get-Content $errLog -Tail 40
    Write-Host ''
    Write-Host '=== STDOUT (son 30 satır) ==='
    Get-Content $configLog -Tail 30
    exit $proc.ExitCode
}

Write-Host ''
Write-Host '=== STDOUT (son 30 satır) ==='
Get-Content $configLog -Tail 30
exit 0
