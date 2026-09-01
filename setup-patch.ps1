# setup-patch.ps1 - 将探测到的工具链路径写入 .vscode/c_cpp_properties.json
# 由 setup.bat 调用: powershell -File setup-patch.ps1 -GccBin "D:\...\bin"
param(
    [Parameter(Mandatory = $true)][string]$GccBin
)

$f = Join-Path $PSScriptRoot '.vscode\c_cpp_properties.json'
if (-not (Test-Path -LiteralPath $f)) {
    Write-Error "missing: $f"
    exit 1
}

$p = ($GccBin.TrimEnd('\', '/') -replace '\\', '/') + '/arm-none-eabi-gcc.exe'
$c = Get-Content -Raw -LiteralPath $f
if ($c -notmatch '"compilerPath"') {
    Write-Error 'compilerPath not found in c_cpp_properties.json'
    exit 1
}

$c = $c -replace '"compilerPath"\s*:\s*"[^"]*"', ('"compilerPath": "' + $p + '"')
Set-Content -LiteralPath $f -Value $c -Encoding UTF8
Write-Output "compilerPath updated to: $p"
