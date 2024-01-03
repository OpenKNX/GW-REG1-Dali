# KNX Dali GW
This Firmware is for use with [REG1 Base](https://github.com/OpenKNX/OpenKNX/wiki/REG1-Base) with the dali application pcb.  

## Cloning this Repo
Open a PowerShell and run this script:
```
iex (iwr "https://raw.githubusercontent.com/OpenKNX/GW-REG1-Dali/master/Firmware/scripts/New-Clone.ps1").Content
```

It will clone this repo and run Restore-First.ps1

## Supported DT
|DeviceType|Description|Supported|
|---|---|---|
|DT0|Leuchtstofflampen|ja*|
|DT1|Notbeleuchtung|ja*|
|DT2|Entladungslampen|ja*|
|DT3|NV-Halogenlampen|ja*|
|DT4|Glühlampen|ja|
|DT5|0-10V Netzteil|ja|
|DT6|LED|ja|
|DT7|Schaltrelais|ja|
|DT8|Farb/Temperatur LED|ja|
|DT9|Sequenzer|nein|

\* nur schalten und dimmen, keine Sonderfunktionen  