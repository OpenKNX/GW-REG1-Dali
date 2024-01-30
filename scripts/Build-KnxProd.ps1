if(-Not (Test-Path -Path platformio.ini)) {
    Set-Location ..
}
if(-Not (Test-Path -Path platformio.ini)) {
    Write-Host "Skript aus falschem Pfad ausgeführt..."
    timeout /T 20
    exit 1
}
$projectDir = Get-Location

$files = Get-ChildItem -Path $projectDir/*.ae-manu
$filePath = $projectDir/$files[0].Name
~/bin/Kaenx.Creator.Console publish $filePath