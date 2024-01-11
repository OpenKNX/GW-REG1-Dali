param ([bool]$checkoutHash=$False,[bool]$wait=$true)

# we assume, we start this script in projects "restore" directory
$oldDir = Get-Location
if(-Not (Test-Path -Path platformio.ini))
{
    if(Test-Path -Path ../platformio.ini)
    {
        Set-Location ..
    }
}

if(-Not (Test-Path -Path platformio.ini)) {
    Write-Host "Skript aus falschem Pfad ausgeführt..."
    Write-Host "Entweder aus Root- oder scripts-Ordner"
    timeout /T 20
    exit 1
}

$projectDir = Get-Location
if(-Not (Test-Path -Path lib/))
{
    New-Item -Path $projectDir -Name "lib" -ItemType "directory"
}
Set-Location lib/

$subprojects = Get-Content dependencies.txt
foreach ($subproject in $subprojects) {
    $attr = $subproject.Split()
    if ($attr[0] -ne "-------") {
        Write-Host ""
        Write-Host "Subproject $($attr[2])" -ForegroundColor Yellow

        if(-Not (Test-Path -Path lib/$attr[2]/))
        {
            Write-Host "Creating directory" -ForegroundColor Yellow
            New-Item -Path $projectDir/lib -Name $attr[2] -ItemType "directory"
        }

        Set-Location $attr[2]
        git clone $attr[3]
        if (!$?) {
            Write-Host "  FAIL: clone" -ForegroundColor Red
            Set-Location $oldDir
            exit 1
        }
        git fetch --all
        if (!$?) {
            Write-Host "  FAIL: fetch --all" -ForegroundColor Red
            Set-Location $oldDir
            exit 1
        }

        if($checkoutHash -eq $TRUE) {
            Write-Host "Checkout $attr[0]" -ForegroundColor Yellow
            git checkout $attr[0]
        } else {
            Write-Host "Checkout $attr[1]" -ForegroundColor Yellow
            git checkout $attr[1]
        }
        if (!$?) {
            Write-Host "  FAIL: checkout" -ForegroundColor Red
            Set-Location $oldDir
            exit 1
        }
        git pull --ff-only
        if (!$?) {
            Write-Host "  FAIL: pull --ff-only" -ForegroundColor Red
            Set-Location $oldDir
            exit 1
        }
        Set-Location $projectDir/lib/
    }
}

Set-Location $oldDir

if($wait -eq $True) {
    timeout /T 20
}