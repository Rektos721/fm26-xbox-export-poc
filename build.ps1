$ErrorActionPreference = "Stop"

$vcvars = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$out = Join-Path $root "bin"
New-Item -ItemType Directory -Force $out | Out-Null

$bootstrap = 'cd /d "' + $out + '" && call "' + $vcvars + '" >nul && cl /nologo /std:c++17 /EHsc /LD "' + $root + '\probe\bootstrap.cpp" /Fobootstrap_v2.obj /Febootstrap_v2.dll'
$payload = 'cd /d "' + $out + '" && call "' + $vcvars + '" >nul && cl /nologo /std:c++17 /EHsc /LD "' + $root + '\payload\payload.cpp" /Fopayload.obj /Fepayload.dll psapi.lib'
$injector = 'cd /d "' + $out + '" && call "' + $vcvars + '" >nul && cl /nologo /std:c++17 /EHsc "' + $root + '\injector\injector.cpp" /Foinjector.obj /Feinjector.exe'

cmd /c $bootstrap
if ($LASTEXITCODE -ne 0) { throw "bootstrap_v2 build failed" }
cmd /c $payload
if ($LASTEXITCODE -ne 0) { throw "payload build failed" }
cmd /c $injector
if ($LASTEXITCODE -ne 0) { throw "injector build failed" }
