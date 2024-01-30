if(-Not (Test-Path -Path platformio.ini)) {
    Set-Location ..
}
if(-Not (Test-Path -Path platformio.ini)) {
    Write-Host "Skript aus falschem Pfad ausgeführt..."
    timeout /T 20
    exit 1
}

./Build-KnxProd.ps1
./Upload-Firmware.ps1