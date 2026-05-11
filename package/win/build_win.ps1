param(
    [string]$ProjectDir = (Get-Location).Path,
    [string]$Version = ""
)

$ErrorActionPreference = "Stop"

if (-not $Version) {
    $cmakeFile = Join-Path $ProjectDir "CMakeLists.txt"
    if (-not (Test-Path $cmakeFile)) {
        Write-Error "CMakeLists.txt not found: $cmakeFile"
        exit 1
    }
    $content = Get-Content $cmakeFile -Raw
    if ($content -match "project\(.*VERSION ([0-9.]+)") {
        $Version = $matches[1]
        Write-Host "Version extracted from CMakeLists.txt: $Version"
    } else {
        Write-Error "Cannot extract version from CMakeLists.txt"
        exit 1
    }
}

Write-Host "=============================="
Write-Host "Version: $Version"
Write-Host "ProjectDir: $ProjectDir"
Write-Host "=============================="

$issFile = Join-Path $ProjectDir "package\win\install.iss"
if (-not (Test-Path $issFile)) {
    Write-Error "Inno Setup script not found: $issFile"
    exit 1
}

$outputDir = Join-Path $ProjectDir "Install"
New-Item -ItemType Directory -Force -Path $outputDir | Out-Null

Write-Host "Running Inno Setup compiler..."
& iscc "/DProjectDir=$ProjectDir" "/DMyAppVersion=$Version" "/O$outputDir" "$issFile"

if ($LASTEXITCODE -ne 0) {
    Write-Error "Inno Setup packaging failed, exit code: $LASTEXITCODE"
    exit 1
}

Write-Host "=============================="
Write-Host "Packaging complete!"
Write-Host "Output: $outputDir"
Write-Host "=============================="
