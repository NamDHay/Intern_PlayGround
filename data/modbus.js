// Hàm mở modal Card Slave
function openAddCardSlave() {
  var modal = new bootstrap.Modal(document.getElementById('cardadd'));
  modal.show();
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

const selecttypemodbus = document.getElementById('typemodbus');
selecttypemodbus.addEventListener('change', function handleChange(event) {

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
      slave_object = "{\"Command\":\"SlaveArray\",\"Slave\":[{\"slaveType\":\"" + (slave_type) + "\",\"ID\":\"" + id_address + "\",\"writeStart\":\"" + wsaddres_input + "\",\"writeEnd\":\"" + weaddres_input + "\",\"readStart\":\"" + rsaddres_input + "\",\"readEnd\":\"" + readdres_input + "\"}]}";
      numSlaveRTU++;
    }
    else if (slave_type == "1") {
      slave_object = "{\"Command\":\"SlaveArray\",\"Slave\":[{\"slaveType\":\"" + (slave_type) + "\",\"ID\":\"" + ip_address + "\",\"writeStart\":\"" + wsaddres_input + "\",\"writeEnd\":\"" + weaddres_input + "\",\"readStart\":\"" + rsaddres_input + "\",\"readEnd\":\"" + readdres_input + "\"}]}";
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
      <p class=\"state\">ID: <span id=\"slave" + i + "\">%ID%</span></p>\
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
  var ID = slave_obj.Slave[0].ID;
  var ws = slave_obj.Slave[0].writeStart;
  var we = slave_obj.Slave[0].writeEnd;
  var rs = slave_obj.Slave[0].readStart;
  var re = slave_obj.Slave[0].readEnd;

  document.getElementById('headerNo' + (numSlave - 1)).innerHTML = (numSlave - 1);
  document.getElementById('slave' + (numSlave - 1)).innerHTML = ID;
  document.getElementById('rs' + (numSlave - 1)).innerHTML = rs;
  document.getElementById('re' + (numSlave - 1)).innerHTML = re;
  document.getElementById('ws' + (numSlave - 1)).innerHTML = ws;
  document.getElementById('we' + (numSlave - 1)).innerHTML = we;
}
function loadBoardSlave(jsonValue) {
  console.log(jsonValue);
  var keys = JSON.parse(jsonValue);
  numSlave = keys.Data.Slave.length;
  addSlaveCard();
  for (var i = 0; i < numSlave; i++) {
    var ID = keys.Data.Slave[i].id;
    var ws = keys.Data.Slave[i].ws;
    var we = keys.Data.Slave[i].we;
    var rs = keys.Data.Slave[i].rs;
    var re = keys.Data.Slave[i].re;
    document.getElementById('headerNo' + i).innerHTML = i;
    document.getElementById('slave' + i).innerHTML = ID;
    document.getElementById('rs' + i).innerHTML = rs;
    document.getElementById('re' + i).innerHTML = re;
    document.getElementById('ws' + i).innerHTML = ws;
    document.getElementById('we' + i).innerHTML = we;
  }
}
function clearSlave() {
  var json_send = "{\"Command\":\"clearSlave\"}";
  console.log(json_send);
  websocket.send(json_send);
  document.getElementById("slavecard").innerHTML = ""
}
function loadTable(jsonValue) {
  var key = JSON.parse(jsonValue);
  for (var i = 0; i < numSlave; i++) {
    var slave = key.Data[i].ID;
    document.getElementById("slave" + i).innerHTML = slave;
    var rs = document.getElementById('rs' + i).innerText;
    var re = document.getElementById('re' + i).innerText;
    var length = re - rs + 1;
    for (var j = 0; j < length; j++) {
      if (key.Data[i].Data[j] == null) break;
      var type = "<select id=\"Type" + i + "_" + j + "\" onchange=\"editModbusDataType('" + i + '_' + j + "')\"><option value=0 %0%>WORD</option><option value=1 %1%>COIL</option><option value=2 %2%>DWORD</option><option value=3 %3%>FLOAT</option><option value=4 %4%>CHAR</option></select>";
      if (key.Data[i].Data[j] == "0") { type = type.replace("%0%", "selected"); type = type.replace("%1%", ""); type = type.replace("%2%", ""); type = type.replace("%3%", ""); type = type.replace("%4%", ""); }
      if (key.Data[i].Data[j] == "1") { type = type.replace("%1%", "selected"); type = type.replace("%0%", ""); type = type.replace("%2%", ""); type = type.replace("%3%", ""); type = type.replace("%4%", ""); }
      if (key.Data[i].Data[j] == "2") { type = type.replace("%2%", "selected"); type = type.replace("%1%", ""); type = type.replace("%0%", ""); type = type.replace("%3%", ""); type = type.replace("%4%", ""); }
      if (key.Data[i].Data[j] == "3") { type = type.replace("%3%", "selected"); type = type.replace("%1%", ""); type = type.replace("%2%", ""); type = type.replace("%0%", ""); type = type.replace("%4%", ""); }
      if (key.Data[i].Data[j] == "4") { type = type.replace("%4%", "selected"); type = type.replace("%1%", ""); type = type.replace("%2%", ""); type = type.replace("%0%", ""); type = type.replace("%3%", ""); }
      document.getElementById("SelectType" + i + "_" + j).innerHTML = type;
    }
  }
  loading = 1;
}
function genTable() {
  var card_table_html = "";
  var stt = 1;
  var length;
  var rs;
  var re;
  firstload = 1;
  if (firstload == 1) {
    firstload = 0;
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
      rs = document.getElementById('rs' + i).innerText;
      re = document.getElementById('re' + i).innerText;
      length = re - rs + 1;
      for (var j = 0; j < length; j++) {
        stt++;
        var type = "<div id=\"SelectType" + i + "_" + j + "\"></div>";
        card_table_html += "<tr><td>" + stt + "</td><td><div id=\"address" + i + "_" + j + "\">NULL</div></td><td>" + type + "</td><td><div id=\"value" + i + "_" + j + "\">NULL</div></td><td><div id=\"editSlaveDataBtn" + i + "_" + j + "\" style=\"display: block;\"><button class=\"buttoncuatao\" onclick=\"editModbusData('" + i + '_' + j + "')\">EDIT</button></div></td></tr>";
      }
      card_table_html += "</tbody></table></div>";
      if (i % 2 == 1) {
        card_table_html += "</div><br>";
      }
      stt = 1;
    }
    document.getElementById("slaveTable").innerHTML = card_table_html;
    for (var m = 0; m < numSlave; m++) {
      rs = document.getElementById('rs' + m).innerText;
      re = document.getElementById('re' + m).innerText;
      length = re - rs + 1;
      for (var n = 0; n < length; n++) {
        document.getElementById("address" + m + "_" + n).innerHTML = Number(rs) + n;
      }
    }
  }
}
function loaddata(jsonValue) {
  var key = JSON.parse(jsonValue);
  for (var i = 0; i < numSlave; i++) {
    if (key.Data[i].connect == "0") {
      document.getElementById("slaveConnect" + i).style = 'background-color: rgb(255, 0, 0)';
      document.getElementById("table" + i).style = 'display: none;';
      continue;
    }
    else {
      document.getElementById("slaveConnect" + i).style = 'background-color: rgb(0, 255, 0)';
      document.getElementById("table" + i).style = 'display: block;';
      for (var j = 0; j < key.Data[i].Data.length; j++) {
        if (loading == 1 && onUpdate == 0) {
          var type = document.getElementById("Type" + i + "_" + j).value;
          console.log("type: " + type);
          var value =[key.Data[i].Data[j]];
          console.log("value: " + value);
          var processedValue = selectTypeData(type, value);
          console.log("processedValue: " + processedValue);
          // document.getElementById("value" + i + "_" + j).innerHTML = processedValue;
        }
      }
    }
  }
}
function editModbusDataType(a) {
  var index = a.split("_");
  var id = index[0];
  var address = index[1];
  var slaveAddress = document.getElementById("address" + id + "_" + address).innerHTML;
  var type = document.getElementById("Type" + id + "_" + address).value;
  var key = JSON.parse(jsontableID);
  key.Data[id].Data[address] = type;
  var fbstring = JSON.stringify(key);
  document.getElementById("datatableid").value = fbstring;
}
function editModbusData(a) {
  onUpdate = 1;
  var index = a.split("_");
  var id = index[0];
  var address = index[1];
  var slaveAddress = document.getElementById("address" + id + "_" + address).innerHTML;
  var type = document.getElementById("Type" + id + "_" + address).value;
  var value = document.getElementById("value" + id + "_" + address).innerHTML;
  document.getElementById("editSlaveDataBtn" + id + "_" + address).style.display = "none";
  document.getElementById("value" + id + "_" + address).innerHTML = "<div><input type=\"text\" id=\"updateBox\" size=\"20\" placeholder=\"" + value + "\" onblur=\"\"></div><br></br>";
  document.getElementById("updateBox").focus();
  document.getElementById("updateBox").addEventListener("keyup", function (event) {
    if (event.key == "Enter") {
      updateDataBox(id + '_' + slaveAddress + '_' + type);
      event.preventDefault();
      document.getElementById("editSlaveDataBtn" + id + "_" + address).style.display = "block";
    }
  });
}

function updateDataBox(a) {
  onUpdate = 0;
  var index = a.split("_");
  var id = index[0];
  var address = index[1];
  var type = index[2];
  var length;
  if (type == "0") length = 1;
  if (type == "1") length = 1;
  if (type == "2") length = 2;
  if (type == "3") length = 2;
  if (type == "4") length = 20;
  var value = document.getElementById("updateBox").value;
  var json_send = "{\"Command\":\"editModbusData\",\"slaveID\":\"" + id + "\",\"address\":\"" + address + "\",\"length\":\"" + length + "\",\"value\":\"" + value + "\"}";
  console.log(json_send);
  // websocket.send(json_send);
}

function word_to_coil(wordArr, coilArr) {
  for (let i = 0; i < wordArr.length; i++) {
    for (let bit = 0; bit < 16; bit++) {
      coilArr[i * 16 + bit] = (wordArr[i] >> bit) & 1;
    }
  }
}
function coil_to_word(coilArr, wordArr) {
  for (let i = 0; i < wordArr.length; i++) {
    wordArr[i] = 0;
    for (let bit = 0; bit < 16; bit++) {
      wordArr[i] |= (coilArr[i * 16 + bit] << bit);
    }
  }
}


function word_to_char_string(wordArr, charArr) {
  for (let i = 0; i < wordArr.length; i++) {
    let word = wordArr[i];
    charArr[2 * i] = String.fromCharCode(word & 0xFF);         // Byte thấp
    charArr[2 * i + 1] = String.fromCharCode((word >> 8) & 0xFF); // Byte cao
  }
}

// Hàm chuyển đổi từ chuỗi ký tự thành mảng từ 16 bit
function char_string_to_word(charArr, wordArr) {
  for (let i = 0; i < wordArr.length; i++) {
    let lowByte = charArr[2 * i].charCodeAt(0);  // Byte thấp
    let highByte = charArr[2 * i + 1].charCodeAt(0); // Byte cao
    wordArr[i] = (lowByte << 8) | highByte; // Ghép nối hai byte thành một từ 16 bit
  }
}


function word_to_float(wordArr, floatArr) {
  for (let i = 0; i < wordArr.length / 2; i++) {
    let combined = (wordArr[2 * i] | (wordArr[2 * i + 1] << 16));
    floatArr[i] = int32ToFloat(combined);
  }
}

function float_to_word(floatArr, wordArr) {
  for (let i = 0; i < floatArr.length; i++) {
    // Chuyển đổi float thành số 32 bit
    let combined = floatToInt32(floatArr[i]);
    // Tách số 32 bit thành hai từ 16 bit
    wordArr[2 * i] = combined & 0xFFFF;            // Byte thấp
    wordArr[2 * i + 1] = (combined >> 16) & 0xFFFF; // Byte cao
  }
}


  // Hàm chuyển đổi số 32 bit thành float
function int32ToFloat(int32) {
  let buffer = new ArrayBuffer(4);
  new DataView(buffer).setUint32(0, int32);
  return new DataView(buffer).getFloat32(0);
}

  // Hàm chuyển đổi float thành số 32 bit
function floatToInt32(float) {
  let buffer = new ArrayBuffer(4);
  new DataView(buffer).setFloat32(0, float);
  return new DataView(buffer).getUint32(0);
}

function word_to_dword(wordArr, dwordArr) {
  for (let i = 0; i < wordArr.length / 2; i++) {
    dwordArr[i] = (wordArr[2 * i] | (wordArr[2 * i + 1] << 16));
  }
}


function dword_to_word(dwordArr, wordArr) {
  for (let i = 0; i < dwordArr.length; i++) {
    wordArr[2 * i] = dwordArr[i] & 0xFFFF;            // Byte thấp
    wordArr[2 * i + 1] = (dwordArr[i] >> 16) & 0xFFFF; // Byte cao
  }
}

function selectTypeData(typedata, value) {
  switch (typedata) {
    case 0:
      return value.join(','); // Chuyển đổi giá trị thành chuỗi phân cách bằng dấu phẩy
    case 1:
      let coilArr = new Array(value.length * 16).fill(0);
      word_to_coil(value, coilArr);
      return coilArr.join(','); // Chuyển đổi mảng thành chuỗi phân cách bằng dấu phẩy

    case 2:
      let dwordArr = new Array(value.length / 2).fill(0);
      word_to_dword(value, dwordArr);
      return dwordArr.join(','); // Chuyển đổi mảng thành chuỗi phân cách bằng dấu phẩy

    case 3:
      let floatArr = new Array(value.length / 2).fill(0);
      word_to_float(value, floatArr);
      return floatArr.join(','); // Chuyển đổi mảng thành chuỗi phân cách bằng dấu phẩy

    case 4:
      let charArr = new Array(value.length * 2).fill('');
      word_to_char_string(value, charArr);
      return charArr.join(''); // Chuyển đổi mảng thành chuỗi không có phân cách

    default:
      throw new Error("Invalid type data"); // Xử lý trường hợp không hợp lệ
  }
}
