var onUpdate = 0;
function settingmodbus() {
    document.getElementById("cardmodbus").style.display = "block";
    document.getElementById("cardio").style.display = "none";
    document.getElementById("cardhome").style.display = "none"
    document.getElementById("cardshowfile").style.display = "none"
}
function send_modbus() {
    var output = "";
    var modbustype_input = document.getElementById('typemodbus').value;
    var baud_input;
    var port_input;
    var rtumode;
    var ethip;
    var gw;
    var sn;
    var dns;
    var tcpmode;
    if (modbustype_input == "") {
        alert("chua chon mode modbus");
    }
    else if (modbustype_input == "0") {
        baud_input = document.getElementById('input_baud').value;
        port_input = document.getElementById('port').value;
        rtumode = document.getElementById('rtumode').value;
        if (baud_input == "") {
            alert("chua nhap baudrate");
        }
        else {
            output = "{'Command':'settingModbus','baud':'" + baud_input + "','serial':'" + port_input + "','mbmaster':'" + rtumode + "','modbustype':'" + modbustype_input + "'}";
        }
    }
    else if (modbustype_input == "1") {
        ethip = document.getElementById('input_ethip').value;
        gw = document.getElementById('input_gw').value;
        sn = document.getElementById('input_sn').value;
        dns = document.getElementById('input_dns').value
        tcpmode = document.getElementById('tcpmode').value;
        if (ethip == "") {
            alert("Chua nhap Ethernet IP");
        }
        else if (gw == "") {
            alert("Chua nhap Gateway");
        }
        else if (sn == "") {
            alert("Chua nhap Subnet");
        }
        else if (dns == "") {
            alert("Chua nhap DNS");
        }
        else {
            output = "{'Command':'settingModbus','ethip':'" + ethip + "','gw':'" + gw + "','sn':'" + sn + "','dns':'" + dns + "','mbclient':'" + tcpmode + "','modbustype':'" + modbustype_input + "'}";
        }
    }
    else {

        baud_input = document.getElementById('input_baud').value;
        port_input = document.getElementById('port').value;
        rtumode = document.getElementById('rtumode').value;
        ethip = document.getElementById('input_ethip').value;
        gw = document.getElementById('input_gw').value;
        sn = document.getElementById('input_sn').value;
        dns = document.getElementById('input_dns').value
        tcpmode = document.getElementById('tcpmode').value;
        if (baud_input == "") {
            alert("chua nhap baudrate");
        }
        else if (ethip == "") {
            alert("Chua nhap Ethernet IP");
        }
        else if (gw == "") {
            alert("Chua nhap Gateway");
        }
        else if (sn == "") {
            alert("Chua nhap Subnet");
        }
        else if (dns == "") {
            alert("Chua nhap DNS");
        }
        else {
            output = "{'Command':'settingModbus','baud':'" + baud_input + "','serial':'" + port_input + "','mbmaster':'" + rtumode + "','ethip':'" + ethip + "','gw':'" + gw + "','sn':'" + sn + "','dns':'" + dns + "','mbclient':'" + tcpmode + "','modbustype':'" + modbustype_input + "'}";
        }
    }
    console.log(output);
    websocket.send(output);
}

const select = document.getElementById('typemodbus');
select.addEventListener('change', function handleChange(event) {
    console.log(event.target.value);

    var configRTU = "<span><div>Baud Rate: <input type=\"text\" id=\"input_baud\" size=\"20\" placeholder=\"115200\"></div><br></span>\
    <span><div>Serial port: <select id=\"port\" value=\"1\"><option value=\"1\" selected=\"\">Serial2</option><option value=\"0\">Serial1</option></select></div><br></span>\
    <span><div>Modbus Mode: <select id=\"rtumode\" value=\"1\"><option value=\"1\" selected=\"\">Master</option><option value=\"0\">Slave</option></select></div><br></span>";
    var configTCP = "<span><div>Ethernet IP : <input type=\"text\" id=\"input_ethip\" size=\"20\" placeholder=\"192.168.1.100\"></div><br></span>\
    <span><div>Ethernet Gateway : <input type=\"text\" id=\"input_gw\" size=\"20\" placeholder=\"192.168.1.1\"></div><br></span>\
    <span><div>Ethernet Subnet : <input type=\"text\" id=\"input_sn\" size=\"20\" placeholder=\"255.255.255.0\"></div><br></span>\
    <span><div>Ethernet DNS : <input type=\"text\" id=\"input_dns\" size=\"20\" placeholder=\"8.8.8.8\"></div><br></span>\
    <span><div>TCP Mode: <select id=\"tcpmode\"><option value=\"1\">Client</option><option value=\"0\">Server</option></select></div><br></span>";
    if (document.getElementById('typemodbus').value == "0") {
        document.getElementById("modedisplay").innerHTML = configRTU;
    }
    else if (document.getElementById('typemodbus').value == "1") {
        document.getElementById("modedisplay").innerHTML = configTCP;
    }
    else if (document.getElementById('typemodbus').value == "2") {
        document.getElementById("modedisplay").innerHTML = configTCP + configRTU;
    }
    else {
        document.getElementById("modedisplay").innerHTML = "";
    }
});

const slaveslect = document.getElementById('slavetype');
slaveslect.addEventListener('change', function handleChange(event) {
    console.log(event.target.value);
    var colorIP = "<div>IP:</div>\
                    <CENTER><input type=\"text\" class=\"state\" id=\"input_slaveip\" size=\"15\" placeholder=\"Slave IP\"></CENTER>\
                    <div>Start Write Address: <CENTER><input type=\"text\" class=\"state\" id=\"input_write_start\" size=\"15\" placeholder=\"Write Start-Address\"></CENTER></div>\
                    <div>End Write Address: <CENTER><input type=\"text\" class=\"state\" id=\"input_write_end\" size=\"15\" placeholder=\"Write End-Address\"></CENTER></div>\
                    <div>Start Read Address: <CENTER><input type=\"text\" class=\"state\" id=\"input_read_start\" size=\"15\" placeholder=\"Read Start-Address\"></CENTER></div>\
                    <div>End Read Address: <CENTER><input type=\"text\" class=\"state\" id=\"input_read_end\" size=\"15\" placeholder=\"Read End-Address\"></CENTER></div>";
    var colorID = "<div>ID:</div>\
                    <CENTER><input type=\"text\" class=\"state\" id=\"input_slaveid\" size=\"15\" placeholder=\"Slave ID\"></CENTER>\
                    <div>Start Write Address: <CENTER><input type=\"text\" class=\"state\" id=\"input_write_start\" size=\"15\" placeholder=\"Write Start-Address\"></CENTER></div>\
                    <div>End Write Address: <CENTER><input type=\"text\" class=\"state\" id=\"input_write_end\" size=\"15\" placeholder=\"Write End-Address\"></CENTER></div>\
                    <div>Start Read Address: <CENTER><input type=\"text\" class=\"state\" id=\"input_read_start\" size=\"15\" placeholder=\"Read Start-Address\"></CENTER></div>\
                    <div>End Read Address: <CENTER><input type=\"text\" class=\"state\" id=\"input_read_end\" size=\"15\" placeholder=\"Read End-Address\"></CENTER></div>";
    if (document.getElementById('slavetype').value == "0") {
        document.getElementById("colorIPD").innerHTML = colorID;
    }
    else if (document.getElementById('slavetype').value == "1") {
        document.getElementById("colorIPD").innerHTML = colorIP;
    }
    else {
        document.getElementById("colorIPD").innerHTML = "";
    }
});
var numSlave = 0;
var numSlaveTCP = 0;
var numSlaveRTU = 0;
function addSlave() {

    var slave_type = document.getElementById('slavetype').value;

    if (slave_type == "") {
        alert("chua chon mode modbus");
    }
    else if (slave_type == "0") {
        var id_address = document.getElementById('input_slaveid').value;
        if (id_address == "") {
            alert("chua nhap Dia chi ID");
        }
    }
    else {
        var ip_address = document.getElementById('input_slaveip').value;
        if (ip_address == "") {
            alert("chua nhap Dia chi IP");
        }
    }
    var wsaddres_input = document.getElementById('input_write_start').value;
    var weaddres_input = document.getElementById('input_write_end').value;
    var rsaddres_input = document.getElementById('input_read_start').value;
    var readdres_input = document.getElementById('input_read_end').value;
    if (rsaddres_input == "") {
        alert("chua nhap Dia chi doc dau tien");
    }
    else if (readdres_input == "") {
        alert("chua nhap Dia chi doc ket thuc");
    }
    else if (wsaddres_input == "") {
        alert("chua nhap Dia chi ghi dau tien");
    }
    else if (weaddres_input == "") {
        alert("chua nhap Dia chi ghi ket thuc");
    }
    else {
        var slave_object = "";
        if (slave_type == "0") {
            slave_object = "{\"Command\":\"SlaveArray\",\"SlaveArray\":[{\"slaveType\":\"" + (slave_type) + "\",\"ID\":\"" + id_address + "\",\"writeStart\":\"" + wsaddres_input + "\",\"writeEnd\":\"" + weaddres_input + "\",\"readStart\":\"" + rsaddres_input + "\",\"readEnd\":\"" + readdres_input + "\"}]}";
            numSlaveRTU++;
        }
        else if (slave_type == "1") {
            slave_object = "{\"Command\":\"SlaveArray\",\"SlaveArray\":[{\"slaveType\":\"" + (slave_type) + "\",\"ID\":\"" + ip_address + "\",\"writeStart\":\"" + wsaddres_input + "\",\"writeEnd\":\"" + weaddres_input + "\",\"readStart\":\"" + rsaddres_input + "\",\"readEnd\":\"" + readdres_input + "\"}]}";
            numSlaveTCP++;
        }
        numSlave = numSlaveRTU + numSlaveTCP;
    }
    addSlaveCard();
    initData(slave_object);
    websocket.send(slave_object);
}

function addSlaveCard() {
    var slave_card_html = "";
    var card_html = "";
    for (var i = 0; i < numSlave; i++) {
        if (i % 4 == 0) {
            slave_card_html += "<div class=\"row justify-content-around\">";
        }
        card_html = "<div class=\"col-2 cardcuatao\">\
        <h5 class=\"state\">\
        <CENTER> No: <span id=\"headerNo" + i + "\">%NO%</span></CENTER>\
        </h5>\
        <p class=\"state\">ID: <span id=\"slavecardid" + i + "\">%ID%</span></p>\
        <p class=\"state\">READ</p>\
        <p class=\"state\">From <span id=\"rs" + i + "\">%S%</span> To <span id=\"re" + i + "\">%E%</span></p>\
        <p class=\"state\">WRITE</p>\
        <p class=\"state\">From <span id=\"ws" + i + "\">%S%</span> To <span id=\"we" + i + "\">%E%</span></p>\
        <CENTER><svg class=\"bi\" width=\"24\" height=\"24\" id=\"slaveConnect" + i + "\"\
        style = \"color: rgb(255, 0, 0);\" >\
        <use xlink:href=\"#icon-connect\"></use>\
        </svg></CENTER>\
        </div > ";

        slave_card_html += card_html;
        if (i % 4 == 3) {
            slave_card_html += "</div><br>";
        }
    }
    document.getElementById("slavecard").innerHTML = slave_card_html;
}
function initData(jsonValue) {
    console.log(jsonValue);
    var slave_obj = JSON.parse(jsonValue);
    var ID = slave_obj.SlaveArray[0].ID;
    var ws = slave_obj.SlaveArray[0].writeStart;
    var we = slave_obj.SlaveArray[0].writeEnd;
    var rs = slave_obj.SlaveArray[0].readStart;
    var re = slave_obj.SlaveArray[0].readEnd;

    document.getElementById('headerNo' + (numSlave - 1)).innerHTML = (numSlave - 1);
    document.getElementById('slavecardid' + (numSlave - 1)).innerHTML = ID;
    document.getElementById('rs' + (numSlave - 1)).innerHTML = rs;
    document.getElementById('re' + (numSlave - 1)).innerHTML = re;
    document.getElementById('ws' + (numSlave - 1)).innerHTML = ws;
    document.getElementById('we' + (numSlave - 1)).innerHTML = we;
} 
    var keys = JSON.parse(jsonValue);
    numSlave = keys.SlaveArray.length;
    addSlaveCard();
    for (var i = 0; i < numSlave; i++) {
        var ID = keys.SlaveArray[i].ID;
        var ws = keys.SlaveArray[i].writeStart;
        var we = keys.SlaveArray[i].writeEnd;
        var rs = keys.SlaveArray[i].readStart;
        var re = keys.SlaveArray[i].readEnd;

        document.getElementById('headerNo' + i).innerHTML = i;
        document.getElementById('slave' + i).innerHTML = ID;
        document.getElementById('rs' + i).innerHTML = rs;
        document.getElementById('re' + i).innerHTML = re;
        document.getElementById('ws' + i).innerHTML = ws;
        document.getElementById('we' + i).innerHTML = we;
    }
}
var loading = 0;
var firstload = 0;
function loadTable(jsonValue) {
    var card_table_html = "";
    var stt = 0;
    if (firstload == 1) {
        firstload = 0;
        var key = JSON.parse(jsonValue);
        for (var i = 0; i < numSlave; i++) {
            if (i % 2 == 0) {
                card_table_html += "<div class=\"row justify-content-around\">";
            }
            card_table_html += "<div id=\"table" + i + "\" class=\"col-10 cardcuatao\" style=\"display:none\">\
            <h5 class=\"state\">\
            <CENTER> No: " + i + "</CENTER>\
            </h5>\
            <CENTER><span class=\"state\">ID: <div class=\"state\" id=\"slave" + i + "\">%ID%</div></span></CENTER>\
            <table class=\"table \"><thead class=\"thead-dark\"><th>STT</th><th>Address</th><th>Type</th><th>Data</th></thead><tbody>\
            ";
            for (var j = 0; j < key.Data[i].Data.length; j++) {
                if (key.Data[i].Data[j][0] == null) break;
                stt++;
                var type = "<select id=\"SelectType" + i + "_" + j + "\" onchange=\"editModbusDataType('" + i + '_' + j + "')\"><option value=0 %0%>WORD</option><option value=1 %1%>COIL</option><option value=2 %2%>DWORD</option><option value=3 %3%>FLOAT</option></select>";
                if (key.Data[i].Data[j][1] == "0") { type = type.replace("%0%", "selected"); type = type.replace("%1%", ""); type = type.replace("%2%", ""); type = type.replace("%3%", ""); }
                if (key.Data[i].Data[j][1] == "1") { type = type.replace("%1%", "selected"); type = type.replace("%0%", ""); type = type.replace("%2%", ""); type = type.replace("%3%", ""); }
                if (key.Data[i].Data[j][1] == "2") { type = type.replace("%2%", "selected"); type = type.replace("%1%", ""); type = type.replace("%0%", ""); type = type.replace("%3%", ""); }
                if (key.Data[i].Data[j][1] == "3") { type = type.replace("%3%", "selected"); type = type.replace("%1%", ""); type = type.replace("%2%", ""); type = type.replace("%0%", ""); }

                card_table_html += "<tr><td>" + stt + "</td><td><div id=\"address" + i + "_" + j + "\">NULL</div></td><td>" + type + "</td><td><div id=\"value" + i + "_" + j + "\">NULL</div></td><td><button class=\"buttoncuatao\" onclick=\"editModbusData('" + i + '_' + j + "')\">EDIT</button></td></tr>";
            }
            card_table_html += "</tbody></table></div>";
            if (i % 2 == 1) {
                card_table_html += "</div><br>";
            }
        }
        document.getElementById("slaveTable").innerHTML = card_table_html;
        for (var i = 0; i < numSlave; i++) {
            document.getElementById("slave" + i).innerHTML = key.Data[i].ID;
            for (var j = 0; j < key.Data[i].Data.length; j++) {
                if (key.Data[i].Data[j][0] == null) break;
                document.getElementById("address" + i + "_" + j).innerHTML = key.Data[i].Data[j][0];
            }
        }
        loading = 1;
    }
}
function loaddata(jsonValue) {
    var key = JSON.parse(jsonValue);
    for (var i = 0; i < numSlave; i++) {
        if (key.Data[i].connect == "0") { 
            document.getElementById("slaveConnect" + i).style = 'background-color: rgb(255, 0, 0)'; 
            document.getElementById("table" + i).style = 'display: none;';
            continue; }
        else {
            document.getElementById("slaveConnect" + i).style = 'background-color: rgb(0, 255, 0)'; 
            document.getElementById("table" + i).style = 'display: block;';

            for (var j = 0; j < key.Data[i].Data.length; j++) {
                if (loading == 1) { if (onUpdate == 0) document.getElementById("value" + i + "_" + j).innerHTML = key.Data[i].Data[j]; }
            }
        }
    }
}
function editModbusDataType(a) {
    var index = a.split("_");
    var id = index[0];
    var address = index[1];
    var slaveAddress = document.getElementById("address" + id + "_" + address).innerHTML;
    var type = document.getElementById("SelectType" + id + "_" + address).value;
    var json_send = "{\"Command\":\"editModbusDataType\",\"slaveID\":\"" + id + "\",\"address\":\"" + slaveAddress + "\",\"type\":\"" + type + "\"}";
    console.log(json_send);
    websocket.send(json_send);
}
function editModbusData(a) {
    onUpdate = 1;
    var index = a.split("_");
    var id = index[0];
    var address = index[1];
    var slaveAddress = document.getElementById("address" + id + "_" + address).innerHTML;
    var type = document.getElementById("SelectType" + id + "_" + address).value;
    var value = document.getElementById("value" + id + "_" + address).innerHTML;
    document.getElementById("value" + id + "_" + address).innerHTML = "<div><input type=\"text\" id=\"updateBox\" size=\"20\" placeholder=\"" + value + "\" onblur=\"updateDataBox('" + id + '_' + slaveAddress + '_' + type + "')\"></div><br></br>";
}
function updateDataBox(a) {
    onUpdate = 0;
    var index = a.split("_");
    var id = index[0];
    var address = index[1];
    var type = index[2];
    var value = document.getElementById("updateBox").value;
    var json_send = "{\"Command\":\"editModbusData\",\"slaveID\":\"" + id + "\",\"address\":\"" + address + "\",\"type\":\"" + type + "\",\"value\":\"" + value + "\"}";
    console.log(json_send);
    websocket.send(json_send);
}
function clearSlave() {
    var json_send = "{\"Command\":\"clearSlave\"}";
    console.log(json_send);
    websocket.send(json_send);
    document.getElementById("slavecard").innerHTML = ""
}
