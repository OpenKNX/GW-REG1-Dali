# we assume, we start this script in projects "restore" directory
if(Test-Path -Path platformio.ini)
{

} else {
    if(Test-Path -Path ../platformio.ini)
    {
        Set-Location ..
    } else {
        if(Test-Path -Path GW-REG1-Dali/Firmware/platformio.ini)
        {
            Set-Location GW-REG1-Dali/Firmware/
        }
    }
}

if(-Not (Test-Path -Path platformio.ini)) {
    Write-Host "Skript aus falschem Pfad ausgeführt..."
    timeout /T 20
    exit 1
}
$projectDir = Get-Location

if(-Not (Test-Path -Path .pio/))
{
    New-Item -Path $projectDir -Name ".pio" -ItemType "directory"
}

if(Test-Path -Path $projectDir/.pio/platformio.base.ini) {
    Write-Host "Lösche 'platformio.rp2040.ini'"
    Remove-Item $projectDir/.pio/platformio.base.ini
}

if(Test-Path -Path $projectDir/.pio/platformio.rp2040.ini) {
    Write-Host "Lösche 'platformio.rp2040.ini'"
    Remove-Item $projectDir/.pio/platformio.rp2040.ini
}

Write-Host "Lade 'platformio.base.ini' herunter..."
Invoke-WebRequest https://raw.githubusercontent.com/OpenKNX/OGM-Common/v1/platformio.base.ini -OutFile $projectDir/.pio/platformio.base.ini
Write-Host "Lade 'platformio.rp2040.ini' herunter..."
Invoke-WebRequest https://raw.githubusercontent.com/OpenKNX/OGM-Common/v1/platformio.rp2040.ini -OutFile $projectDir/.pio/platformio.rp2040.ini
Write-Host "Fertig!"

timeout /T 20