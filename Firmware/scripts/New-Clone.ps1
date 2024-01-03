Write-Host "Klone das Repo GW-REG1-Dali" -ForegroundColor Yellow

git clone https://github.com/OpenKnx/GW-REG1-Dali

Write-Host "Führe das Skript 'Restore-First.ps1' aus" -ForegroundColor Yellow

.\GW-REG1-Dali\Firmware\scripts\Restore-First.ps1

Write-Host "Fertig" -ForegroundColor Yellow