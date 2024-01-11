$repo = "GW-REG1-Dali"

Write-Host "Klone das Repo $repo" -ForegroundColor Yellow

git clone https://github.com/OpenKnx/$repo

Write-Host "Führe das Skript 'Restore-Project.ps1' aus" -ForegroundColor Yellow

Set-Location $repo\Firmware\
.\scripts\Restore-Project.ps1 -checkoutHash $False -wait $False

Write-Host "Fertig" -ForegroundColor Yellow
timeout /T 20