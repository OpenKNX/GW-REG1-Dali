function buttonread(device, online, progress, context)
{
// Start read devicetype
progress.setText(device.getMessage(2));

var data = [context.Channel];
online.connect();
var resp = online.invokeFunctionProperty(164, 2, data); //invoke readdevicetype

if (resp[0] != 0) {
    // Dali Error:
    throw new Error(device.getMessage(1) + String(resp[0]));
}

var para = device.getParameterByName("deviceType");
if(resp[1] == 255)
    throw new Error(device.getMessage(14)) // Unknown DeviceType

para.value = resp[1].toString();

// Read Successfully devicetype
progress.setText(device.getMessage(3));
}

function buttonsettingsRead(device, online, progress, context) {
progress.setText(device.getMessage(10)); // Start readeing Data from EVG
online.connect();
var data = online.invokeFunctionProperty(164, 11, [context.Channel]);
online.disconnect();
progress.setText(device.getMessage(11)); // Parsing data
if(data[0] != 0)
    throw new Error("Dali Error: " + data[0]);
var errors = "";

if(data[25] & 1) errors += "Min Level, ";
else device.getParameterByName("min").value = data[1].toString();
if(data[25] & 2) errors += "Max Level, ";
else device.getParameterByName("max").value = data[2].toString();
if(data[25] & 4) errors += "Power On, ";
else {
    device.getParameterByName("poweron").value = (data[3] == 255) ? "1" : "0";
    if (data[3] < 255) device.getParameterByName("poweronlevel").value = data[3].toString();
}
if(data[25] & 8) errors += "Failure On, ";
else {
    device.getParameterByName("failureon").value = (data[4] == 255) ? "1" : "0";
    if (data[4] < 255) device.getParameterByName("failureonlevel").value = data[4].toString();
}
if(data[25] & 16) {
    device.getParameterByName("fadeTime").value = (data[5] >> 4).toString();
    device.getParameterByName("fadeRate").value = (data[5] & 15).toString();
}
if(data[25] & 64) errors += "Groups 0-7, ";
else {
    device.getParameterByName("g0").value = (data[7] & 1) ?"1" : "0";
    device.getParameterByName("g1").value = (data[7] & 2) ?"1" : "0";
    device.getParameterByName("g2").value = (data[7] & 4) ?"1" : "0";
    device.getParameterByName("g3").value = (data[7] & 8) ?"1" : "0";
    device.getParameterByName("g4").value = (data[7] & 16) ?"1" : "0";
    device.getParameterByName("g5").value = (data[7] & 32) ?"1" : "0";
    device.getParameterByName("g6").value = (data[7] & 64) ?"1" : "0";
    device.getParameterByName("g7").value = (data[7] & 128) ?"1" : "0";
}
if(data[25] & 128) errors += "Groups 8-15, ";
else {
    device.getParameterByName("g8").value = (data[8] & 1) ?"1" : "0";
    device.getParameterByName("g9").value = (data[8] & 2) ?"1" : "0";
    device.getParameterByName("g10").value = (data[8] & 4) ?"1" : "0";
    device.getParameterByName("g11").value = (data[8] & 8) ?"1" : "0";
    device.getParameterByName("g12").value = (data[8] & 16) ?"1" : "0";
    device.getParameterByName("g13").value = (data[8] & 32) ?"1" : "0";
    device.getParameterByName("g14").value = (data[8] & 64) ?"1" : "0";
    device.getParameterByName("g15").value = (data[8] & 128) ?"1" : "0";
}

device.getParameterByName("s0t").value = (data[9] == 255) ? "0" : "1";
if (data[9] < 255) device.getParameterByName("s0v").value = data[9].toString();
device.getParameterByName("s1t").value = (data[10] == 255) ? "0" : "1";
if (data[10] < 255) device.getParameterByName("s1v").value = data[10].toString();
device.getParameterByName("s2t").value = (data[11] == 255) ? "0" : "1";
if (data[11] < 255) device.getParameterByName("s2v").value = data[11].toString();
device.getParameterByName("s3t").value = (data[12] == 255) ? "0" : "1";
if (data[12] < 255) device.getParameterByName("s3v").value = data[12].toString();
device.getParameterByName("s4t").value = (data[13] == 255) ? "0" : "1";
if (data[13] < 255) device.getParameterByName("s4v").value = data[13].toString();
device.getParameterByName("s5t").value = (data[14] == 255) ? "0" : "1";
if (data[14] < 255) device.getParameterByName("s5v").value = data[14].toString();
device.getParameterByName("s6t").value = (data[15] == 255) ? "0" : "1";
if (data[15] < 255) device.getParameterByName("s6v").value = data[15].toString();
device.getParameterByName("s7t").value = (data[16] == 255) ? "0" : "1";
if (data[16] < 255) device.getParameterByName("s7v").value = data[16].toString();
device.getParameterByName("s8t").value = (data[17] == 255) ? "0" : "1";
if (data[17] < 255) device.getParameterByName("s8v").value = data[17].toString();
device.getParameterByName("s9t").value = (data[18] == 255) ? "0" : "1";
if (data[18] < 255) device.getParameterByName("s9v").value = data[18].toString();
device.getParameterByName("s10t").value = (data[19] == 255) ? "0" : "1";
if (data[19] < 255) device.getParameterByName("s10v").value = data[19].toString();
device.getParameterByName("s11t").value = (data[20] == 255) ? "0" : "1";
if (data[20] < 255) device.getParameterByName("s11v").value = data[20].toString();
device.getParameterByName("s12t").value = (data[21] == 255) ? "0" : "1";
if (data[21] < 255) device.getParameterByName("s12v").value = data[21].toString();
device.getParameterByName("s13t").value = (data[22] == 255) ? "0" : "1";
if (data[22] < 255) device.getParameterByName("s13v").value = data[22].toString();
device.getParameterByName("s14t").value = (data[23] == 255) ? "0" : "1";
if (data[23] < 255) device.getParameterByName("s14v").value = data[23].toString();
device.getParameterByName("s15t").value = (data[24] == 255) ? "0" : "1";
if (data[24] < 255) device.getParameterByName("s15v").value = data[24].toString();

if(errors != "")
    progress.setText(device.getMessage(13) + errors); // following couldnt be read
else
    progress.setText(device.getMessage(12)); // reading successfull
}

function buttonsettingsWrite(device, online, progress, context) {
if(device.getParameterByName("fadeTime").value != "0" && device.getParameterByName("fadeTimeExtendedMultiplier").value != "0")
    throw new Error(device.getMessage(18)); // error 

progress.setText(device.getMessage(15)); // start

var data = [];
data.push(context.Channel);
data.push(parseInt(device.getParameterByName("min").value, 10));
data.push(parseInt(device.getParameterByName("max").value, 10));
data.push((device.getParameterByName("poweron").value == "0") ? parseInt(device.getParameterByName("poweronlevel").value, 10) : 255);
data.push((device.getParameterByName("failureon").value == "0") ? parseInt(device.getParameterByName("failureonlevel").value, 10) : 255);
var fade = parseInt(device.getParameterByName("fadeTime").value);
fade = fade << 4;
fade |= parseInt(device.getParameterByName("fadeRate").value)
data.push(fade);
data.push(0);//faderate
var groups = parseInt(device.getParameterByName("g0").value, 10);
groups |= parseInt(device.getParameterByName("g1").value, 10) << 1;
groups |= parseInt(device.getParameterByName("g2").value, 10) << 2;
groups |= parseInt(device.getParameterByName("g3").value, 10) << 3;
groups |= parseInt(device.getParameterByName("g4").value, 10) << 4;
groups |= parseInt(device.getParameterByName("g5").value, 10) << 5;
groups |= parseInt(device.getParameterByName("g6").value, 10) << 6;
groups |= parseInt(device.getParameterByName("g7").value, 10) << 7;
data.push(groups);
groups = parseInt(device.getParameterByName("g8").value, 10);
groups |= parseInt(device.getParameterByName("g9").value, 10) << 1;
groups |= parseInt(device.getParameterByName("g10").value, 10) << 2;
groups |= parseInt(device.getParameterByName("g11").value, 10) << 3;
groups |= parseInt(device.getParameterByName("g12").value, 10) << 4;
groups |= parseInt(device.getParameterByName("g13").value, 10) << 5;
groups |= parseInt(device.getParameterByName("g14").value, 10) << 6;
groups |= parseInt(device.getParameterByName("g15").value, 10) << 7;
data.push(groups);
data.push((device.getParameterByName("s0t").value == "1") ? parseInt(device.getParameterByName("s0v").value, 10) : 255);
data.push((device.getParameterByName("s1t").value == "1") ? parseInt(device.getParameterByName("s1v").value, 10) : 255);
data.push((device.getParameterByName("s2t").value == "1") ? parseInt(device.getParameterByName("s2v").value, 10) : 255);
data.push((device.getParameterByName("s3t").value == "1") ? parseInt(device.getParameterByName("s3v").value, 10) : 255);
data.push((device.getParameterByName("s4t").value == "1") ? parseInt(device.getParameterByName("s4v").value, 10) : 255);
data.push((device.getParameterByName("s5t").value == "1") ? parseInt(device.getParameterByName("s5v").value, 10) : 255);
data.push((device.getParameterByName("s6t").value == "1") ? parseInt(device.getParameterByName("s6v").value, 10) : 255);
data.push((device.getParameterByName("s7t").value == "1") ? parseInt(device.getParameterByName("s7v").value, 10) : 255);
data.push((device.getParameterByName("s8t").value == "1") ? parseInt(device.getParameterByName("s8v").value, 10) : 255);
data.push((device.getParameterByName("s9t").value == "1") ? parseInt(device.getParameterByName("s9v").value, 10) : 255);
data.push((device.getParameterByName("s10t").value == "1") ? parseInt(device.getParameterByName("s10v").value, 10) : 255);
data.push((device.getParameterByName("s11t").value == "1") ? parseInt(device.getParameterByName("s11v").value, 10) : 255);
data.push((device.getParameterByName("s12t").value == "1") ? parseInt(device.getParameterByName("s12v").value, 10) : 255);
data.push((device.getParameterByName("s13t").value == "1") ? parseInt(device.getParameterByName("s13v").value, 10) : 255);
data.push((device.getParameterByName("s14t").value == "1") ? parseInt(device.getParameterByName("s14v").value, 10) : 255);
data.push((device.getParameterByName("s15t").value == "1") ? parseInt(device.getParameterByName("s15v").value, 10) : 255);

progress.setText(device.getMessage(16)); // transmit
online.connect();
var resp = online.invokeFunctionProperty(164, 10, data);
online.disconnect();
progress.setText(device.getMessage(17)); // fin
}
function buttonautoAddr(device, online, progress, context) {
    online.connect();
    online.invokeFunctionProperty(164, 5, []);

    for (var i = 0; i < 64; i++)
    {
        var para = device.getParameterByName("ballast" + i);
        para.value = "";
    }

    var counter = 0;

    while (true) {
        if (progress.isCanceled()) {
            online.readFunctionProperty(164, 5, [255]);
            return;
        }

        var resp = online.readFunctionProperty(164, 5, [counter]);

        if (resp[0]) {
            //found ballast
            var high = "";
            if (resp[2] < 16) high = "0";
            high += resp[2].toString(16);
            if (resp[3] < 16) high += "0";
            high += resp[3].toString(16);
            if (resp[4] < 16) high += "0";
            high += resp[4].toString(16);
            var para = device.getParameterByName("ballast" + counter);
            high = "0x" + high;
            if (resp[5] < 99)
            high += " -&gt; " + resp[5];
            para.value = high;
            counter++;
        }
        if (resp[1]) {
            //no more ballasts
            break;
        }
    }

    progress.setText(counter + " Geräte gefunden");
}
function buttonassingAddr(device, online, progress, context) {
    var along = device.getParameterByName("longAddr");
    var ashort = device.getParameterByName("shortAddr");

    //assign address to device
    progress.setText(device.getMessage(5));

    var bytes = [parseInt(ashort.value)];
    for (var c = 0; c < along.value.length; c += 2)
    bytes.push(parseInt(along.value.substr(c, 2), 16));

    online.connect();

    online.invokeFunctionProperty(164, 4, bytes);

    bytes = [];
    while (true) {
        if (progress.isCanceled()) {
            online.readFunctionProperty(164, 4, [255]);
            return;
        }

        var resp = online.readFunctionProperty(164, 4, bytes);

        switch (resp[0]) {
            case 2:
                //address set successfully
                progress.setText(device.getMessage(7));
                return;

            case 10:
                //dali error
                progress.setText(device.getMessage(1));
                return;

            case 11:
                //address is already in use
                throw new Error(device.getMessage(6));

            case 12:
                //long address dont exists
                throw new Error(device.getMessage(8));

            case 13:
                //short address confirm failed
                throw new Error(device.getMessage(9));

            case 14:
                //device wont answer
                throw new Error(device.getMessage(4));

        }
    }
}
function buttonscan(device, online, progress, context) {
    online.connect();

    var data = [];
    var para2 = device.getParameterByName("onlyUnaddressed");
    data[0] = parseInt(para2.value);
    para2 = device.getParameterByName("dontRandomize");
    data[1] = parseInt(para2.value);

    //start addressing
    online.invokeFunctionProperty(164, 3, data);
    progress.setText("Suche Geräte");

    for (var i = 0; i < 64; i++)
    {
        var para = device.getParameterByName("ballast" + i);
        para.value = "";
    }

    var counter = 0;

    while (true) {
        if (progress.isCanceled()) {
            online.readFunctionProperty(164, 3, [254]);
            return;
        }

        var resp = online.readFunctionProperty(164, 3, [counter]);

        if (resp[0]) {
            //found ballast
            var high = "";
            if (resp[2] < 16) high = "0";
            high += resp[2].toString(16);
            if (resp[3] < 16) high += "0";
            high += resp[3].toString(16);
            if (resp[4] < 16) high += "0";
            high += resp[4].toString(16);
            var para = device.getParameterByName("ballast" + counter);
            high = "0x" + high;
            if (resp[5] < 99)
            high += " -&gt; " + resp[5];
            para.value = high;
            counter++;
        }
        if (resp[1]) {
            //no more ballasts
            break;
        }
    }

    progress.setText(counter + " Geräte gefunden");
}