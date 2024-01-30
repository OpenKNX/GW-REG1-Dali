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

if($env -eq $null)
{
    Write-Host "No environment specified. Will search for release..."
    $content = Get-Content $projectDir/platformio.custom.ini -raw
    if($content -match '\[env:(release_.*)\]')
    {
        Write-Host "Found release $($Matches.1)"
        $env = $Matches.1
    } else {
        Write-Host "Didnt found host"
        timeout /T 20
        exit 1
    }
}

if ($IsMacOS -or $IsLinux) { ~/.platformio/penv/bin/pio run -e $env }
else { ~/.platformio/penv/Scripts/pio.exe run -t upload -e $env } 

timeout /T 20