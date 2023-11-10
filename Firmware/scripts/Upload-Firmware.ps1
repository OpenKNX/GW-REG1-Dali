$env = $args[0]

if(-Not (Test-Path -Path platformio.ini)) {
    Set-Location ..
}
if(-Not (Test-Path -Path platformio.ini)) {
    Write-Host "Skript aus falschem Pfad ausgeführt..."
    timeout /T 20
    exit 1
}
$projectDir = Get-Location

if(!Test-Path -Path $projectDir/.pio/platformio.base.ini) {
    Write-Host "Lade benötigte inis..."
    $projectDir/scripts/Restore-First.ps1
}

if ($IsMacOS -or $IsLinux) { ~/.platformio/penv/bin/pio run -e $env }
else { ~/.platformio/penv/Scripts/pio.exe run -e $env } 

timeout /T 20