# ==========================================================================
# Project Phoenix — CMake Build Script
# --------------------------------------------------------------------------
# dc-configure.ps1'den sonra çağrılır. Client target'ını Release modunda
# derler; tüm çıktıyı build\build.log'a yazar. Özet bilgileri döker.
# ==========================================================================

$ErrorActionPreference = 'Stop'

$projectRoot = 'F:\Project Phoenix'
$buildDir    = Join-Path $projectRoot 'build'
$vsRoot      = 'C:\Program Files\Microsoft Visual Studio\2022\Community'
$cmakeExe    = Join-Path $vsRoot 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
$target      = 'Client'
if ($args.Count -gt 0) { $target = $args[0] }
$config      = 'Release'

if (-not (Test-Path (Join-Path $buildDir 'DarkEdenLegacyModern.sln'))) {
    Write-Host "dc-build: .sln bulunamadi. Once configure calistirin."
    exit 2
}

Import-Module (Join-Path $vsRoot 'Common7\Tools\Microsoft.VisualStudio.DevShell.dll')
Enter-VsDevShell `
    -VsInstallPath $vsRoot `
    -DevCmdArguments '-arch=x86 -host_arch=x64' `
    -SkipAutomaticLocation | Out-Null

Set-Location $buildDir

$outLog = Join-Path $buildDir 'build.log'
$errLog = Join-Path $buildDir 'build.err'

# Paralel derleme: kullanıcı 8c/16t sisteme sahip, 12 thread ayrılıyor,
# 4 thread diğer işlere boş bırakılıyor. --parallel CMake 3.12+ ile
# MSBuild'in /m:N parametresini set eder.
$parallelJobs = 12

$cmakeArgs = "--build . --config $config --target $target --parallel $parallelJobs --verbose"

Write-Host ("dc-build: target={0} config={1} parallel={2}" -f $target, $config, $parallelJobs)
Write-Host ("dc-build: out log = {0}" -f $outLog)

$proc = Start-Process `
    -FilePath $cmakeExe `
    -ArgumentList $cmakeArgs `
    -Wait -NoNewWindow -PassThru `
    -RedirectStandardOutput $outLog `
    -RedirectStandardError  $errLog

$exitCode = $proc.ExitCode
$outSize = 0
$errSize = 0
if (Test-Path $outLog) { $outSize = (Get-Item $outLog).Length }
if (Test-Path $errLog) { $errSize = (Get-Item $errLog).Length }

Write-Host ("dc-build: exit={0}  stdout={1}B  stderr={2}B" -f $exitCode, $outSize, $errSize)

# Hata sayimi
$lnkMatches = @()
$cerrMatches = @()
if ($outSize -gt 0) {
    $lnkMatches  += Select-String -Path $outLog -Pattern 'error LNK\d+' -AllMatches -ErrorAction SilentlyContinue
    $cerrMatches += Select-String -Path $outLog -Pattern 'error C\d+'   -AllMatches -ErrorAction SilentlyContinue
}
if ($errSize -gt 0) {
    $lnkMatches  += Select-String -Path $errLog -Pattern 'error LNK\d+' -AllMatches -ErrorAction SilentlyContinue
    $cerrMatches += Select-String -Path $errLog -Pattern 'error C\d+'   -AllMatches -ErrorAction SilentlyContinue
}
$lnkCount = 0
foreach ($m in $lnkMatches) { if ($m) { $lnkCount += $m.Matches.Count } }
$cerrCount = 0
foreach ($m in $cerrMatches) { if ($m) { $cerrCount += $m.Matches.Count } }

Write-Host ("dc-build: error counts  LNK={0}  C={1}" -f $lnkCount, $cerrCount)

# Client.exe olustu mu?
$exePath = Join-Path $projectRoot 'GameData\Client.exe'
if (Test-Path $exePath) {
    $info = Get-Item $exePath
    Write-Host ("dc-build: Client.exe FOUND at {0} ({1} bytes, {2})" -f $info.FullName, $info.Length, $info.LastWriteTime)
} else {
    Write-Host "dc-build: Client.exe NOT found (link step likely failed)"
}

Write-Host ""
Write-Host "=== BUILD LOG (last 50 lines) ==="
if ($outSize -gt 0) { Get-Content $outLog -Tail 50 }

if ($errSize -gt 0) {
    Write-Host ""
    Write-Host "=== BUILD ERR (last 30 lines) ==="
    Get-Content $errLog -Tail 30
}

exit $exitCode
