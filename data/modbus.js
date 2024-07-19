function settingmodbus() {
    document.getElementById("cardmodbus").style.display = "block";
    document.getElementById("cardio").style.display = "none";
    document.getElementById("cardhome").style.display = "none"
    document.getElementById("cardshowfile").style.display = "none"
}

function send_modbus() {
    var output = "";
    var modbustype_input = document.getElementById('typemodbus').value;

    if (modbustype_input == "") {
        alert("chua chon mode modbus");
    }
    else if (modbustype_input == "0") {
        var baud_input = document.getElementById('input_baud').value;
        var port_input = document.getElementById('port').value;
        var rtumode = document.getElementById('rtumode').value;
        if (baud_input == "") {
            alert("chua nhap baudrate");
        }
        else {
            output = "{'Command':'settingModbus','baud':'" + baud_input + "','serial':'" + port_input + "','mbmaster':'" + rtumode + "','modbustype':'" + modbustype_input + "'}";
        }
    }
    else {
        var ethip = document.getElementById('input_ethip').value;
        var gw = document.getElementById('input_gw').value;
        var sn = document.getElementById('input_sn').value;
        var dns = document.getElementById('input_dns').value
        var tcpmode = document.getElementById('tcpmode').value;
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
    console.log(output);
    websocket.send(output);
}
function saveModbusTable() {
    var msg = [];
    var msgOut = "{\"Command\":\"mbDataType\",\"lengh\":\"" + TableDataLen + "\",\"type\":[";
    for (var i = 0; i < TableDataLen; i++) { if (document.getElementById("SelectType" + i).value != null) msg[i] = document.getElementById("SelectType" + i).value; }
    msgOut += msg[0];
    for (var i = 1; i < TableDataLen; i++) { msgOut += "," + msg[i]; }
    msgOut += "]}";
    loading = 0;
    websocket.send(msgOut);
    console.log(msgOut);
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
let slave_array = "{\"Command\":\"SlaveArray\",\"SlaveArray\":[]}";
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
        var slave_object = "{\"slaveType\":\"" + (slave_type) + "\",\"ID\":\"" + id_address + "\",\"writeStart\":\"" + wsaddres_input + "\",\"writeEnd\":\"" + weaddres_input + "\",\"readStart\":\"" + rsaddres_input + "\",\"readEnd\":\"" + readdres_input + "\"}]}";
        if (numSlave == 0) { slave_array = slave_array.replace("]}", slave_object); }
        else { slave_array = slave_array.replace("]}", "," + slave_object); }
        console.log(slave_array);
        numSlave++;
    }
    addSlaveCard();
    websocket.send(slave_array);
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
        <p class=\"state\">ID: <span id=\"slave" + i + "\">%ID%</span></p>\
        <p class=\"state\">READ</p>\
        <p class=\"state\">From <span id=\"rs" + i + "\">%S%</span> To <span id=\"re" + i + "\">%E%</span></p>\
        <p class=\"state\">WRITE</p>\
        <p class=\"state\">From <span id=\"ws" + i + "\">%S%</span> To <span id=\"we" + i + "\">%E%</span></p>\
        </div>";

        slave_card_html += card_html;
        if (i % 4 == 3) {
            slave_card_html += "</div><br>";
        }
    }
    document.getElementById("slavecard").innerHTML = slave_card_html;
    initData();
}

function initData() {
    var slave_obj = JSON.parse(slave_array);
    for (var i = 0; i < numSlave; i++) {
        var ID = slave_obj.SlaveArray[i].ID;
        var ws = slave_obj.SlaveArray[i].writeStart;
        var we = slave_obj.SlaveArray[i].writeEnd;
        var rs = slave_obj.SlaveArray[i].readStart;
        var re = slave_obj.SlaveArray[i].readEnd;

        document.getElementById('headerNo' + i).innerHTML = i;
        document.getElementById('slave' + i).innerHTML = ID;
        document.getElementById('rs' + i).innerHTML = rs;
        document.getElementById('re' + i).innerHTML = re;
        document.getElementById('ws' + i).innerHTML = ws;
        document.getElementById('we' + i).innerHTML = we;
    }
}