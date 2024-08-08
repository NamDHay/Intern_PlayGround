
//global variables
// var gateway = `ws://192.168.1.254/ws`;
var gateway = `ws://192.168.0.88/ws`;
var loadcard = 0;
var loading = 0;
var websocket;
var IsConnect;
var numSlave = 0;
var numSlaveTCP = 0;
var numSlaveRTU = 0;
var io_array = [1, 1, 1, 1, 1, 1, 1, 1];
var daloadvcard = 0;
var io_obj = "{\"Command\": \"getIO\",\"id\": \"1\",\"Data\":[]}";
var jsonAppInput = "";// data de tao card
var jsontableID = "";
var jsontableData = "";
var AppID = 0;
var appData = [];
var typecard;
var distance = 0;
var x = 0;
var y = 0;
var z = 0;
var temperature = 0;
var app = 0;
var nodeID;
var netID = 0;
let jsonApp = "";
var namecard = "";
var selectvalue1;
var selectvalue2;
var selectvalue3;
var selectvalue4;
var selectvalue5;
var selectvalue6;
var selectvalue7;
var firstload = 0;
var value1 = 0;
var value2 = 0;
var value3 = 0;
var value4 = 0;
var value5 = 0;
var value6 = 0;
var value7 = 0;
var id_card = 1;
var html = "";
var dataProduct;
var preferenceslist = [];
var onUpdate = 0;
var jsonSlave = "";
var loadDone = false;
var loadproduct = 0;
const intervalId = setInterval(intervalHandle, 1000);
function intervalHandle() {
  // var json_output;
  // if (IsConnect == true) {
  //   json_output = "{'Command':'toggleLed'}";
  //   console.log(json_output);
  //   websocket.send(json_output);

  //   document.getElementById('input1').innerHTML = io_array[0];
  //   document.getElementById('input2').innerHTML = io_array[1];
  //   document.getElementById('input3').innerHTML = io_array[2];
  //   document.getElementById('input4').innerHTML = io_array[3];
  //   document.getElementById('output1').innerHTML = io_array[4];
  //   document.getElementById('output2').innerHTML = io_array[5];
  //   document.getElementById('output3').innerHTML = io_array[6];
  //   document.getElementById('output4').innerHTML = io_array[7];

  // var jsonOD2000 = "{\"OD2000\":{\"value\":65.21464539}}";
  // var jsonparseOD2000 = JSON.parse(jsonOD2000);
  // gencard_ODOD2000(jsonparseOD2000.OD2000.value);
  // var jsonMPB10 = "{\"MPB10\":{\"value\":[33,0.196717024,0.141381785,0.13805677]}}";
  // var jsonparseMPB10 = JSON.parse(jsonMPB10);
  // gencard_MPB10(jsonparseMPB10.MPB10.value);

  // var jsonWTM10L = "{\"WTM10L\":{\"value\":183}}";
  // var jsonparseWTM10L = JSON.parse(jsonWTM10L);
  // gencard_WTM10L(jsonparseWTM10L.WTM10L.value);
}
const selectwifimode = document.getElementById("staticip");
// select mode wifi
selectwifimode.addEventListener('change', function handChange(event) {
  var Enable = "<div>WEB_ADDRESS: <input type=\"text\"  id=\"waddress\" size=\"20\" value=\"192.168.4.1\"></div><br>\
                    <div>WEB_GETWAY: <input type=\"text\" id=\"wgetway\" size=\"20\" value=\"192.168.4.1\"></div><br>\
                    <div>WEB_SUBNET: <input type=\"text\"  id=\"wsubnet\" size=\"20\" value=\"255.255.255.0\"></div><br>\
                    <div>WIFI_MODE: <select id=\"wifimode\" value=\"0\"><option value=\"0\" selected=\"\">STA NORMAL</option><option value=\"1\">AP NORMAL</option><option value=\"2\">AP-STA NORMAL</option><option value=\"3\">WIFI OFF</option></select></div><br></br>";
  if (document.getElementById("staticip").value == "0") {
    document.getElementById("wifimode").innerHTML = "";
  }
  else if (document.getElementById("staticip").value == "1") {
    document.getElementById("wifimode").innerHTML = Enable;
  }
});

window.addEventListener('beforeunload', () => clearInterval(intervalId));
window.addEventListener('load', onLoad);
function getReadings() {
  console.log(io_array.length);
  for (var i = 0; i < io_array.length; i++) {
    console.log("Get IO: " + io_array[i]);
  }
}
function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  // websocket = new WebSocket(gateway);
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}
function onOpen(event) {
  console.log('Connection opened');
  // IsConnect = true;
  Flagfile = true;
}
function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 100);
}
function onMessage(event) {
  var state;
  try {
    var message = JSON.parse(event.data);
    // console.log(message);
    // console.log('Message received: ' + event.data);
    if (message.Command == "toggleLed") {
      if (message.Data == "0")
        state = "ON";
      else state = "OFF";
      console.log(state);
      document.getElementById('state').innerHTML = state;
    }
    else if (message.Command == "getIO") { }

    else if (message.Command == "settingWifi") {
      alert("Setting Done");
    }
    else if (message.Command == "settingModbus") {
      alert("Completed setting!!!");
    }
    else if (message.Command == "ListFile") {
      tablefile(event.data);
    }

    else if (message.Command == "TableID") {
      jsontableID = document.getElementById("datatableid").value = event.data;
      // console.log(jsontableID);
      firstload = 1;
      loading = 0;
      loadTable(jsontableID);

      changeRegOptions(jsontableID);
    }
    else if (message.Command == "tableData") {
      console.log(event.data);
      if (loading == 1) {
        jsontableData = event.data;
        loaddata(jsontableData);
        addvaluecard(jsontableData);
      }
      if (daloadvcard) updatevalue();
      if (loadcard == 1) {
        buildCardJson();
        loadcard = 0;

      }
    }
    else if (message.Filename == "Application") {
      document.getElementById("jsonApp").value = event.data;
      // console.log(event.data);
      loadproduct = 1;

    }
    else if (message.Filename == "TableID") {
      jsontableID = document.getElementById("datatableid").value = event.data;
      // console.log(jsontableID);
      firstload = 1;
      loading = 0;
      loadTable(jsontableID);
      loadcard = 1;
      changeRegOptions(jsontableID);
    }
    else if (message.Filename == "DataProduct") {
      // console.log(event.data);
      document.getElementById("dataProduct").value = preferenceslist = event.data;
    }
    else if (message.Filename == "mbSlave" && loadDone == false) {
      // console.log(event.data);
      jsonSlave = document.getElementById("datatabledata").value = event.data;
      loadBoardSlave(event.data);
      genTable();
      loadDone = true;
    }
    var key = Object.keys(message);
    if (key == "OD2000") {
      gencard_ODOD2000(message);
    }
    if (key == "WTM10L") {
      gencard_WTM10L(message);
    }
    if (key == "MPB10") {
      gencard_MPB10(message);
    }
    if (key == "CSS") {
      gencard_CSS(message);
    }
    if (key == "PBS") {
      gencard_PBS(message);
    }
  } catch (e) {
    console.log(e);
    console.log(event.data);
  }
}
function onLoad(event) {
  initWebSocket();
  initButton();
  // LoadData();
}
function io_ChangeState1() {
  io_array[4] = !io_array[4];
}
function io_ChangeState2() {
  io_array[5] = !io_array[5];
}
function io_ChangeState3() {
  io_array[6] = !io_array[6];
}
function io_ChangeState4() {
  io_array[7] = !io_array[7];
}

function initButton() {
  document.getElementById('addslave').addEventListener('click', openAddCardSlave);
  document.getElementById('buttonaddslave').addEventListener('click', addSlave);
  document.getElementById('buttonsave').addEventListener('click', save);
  document.getElementById('buttonsend').addEventListener('click', send_modbus);
  document.getElementById('button1').addEventListener('click', io_ChangeState1);
  document.getElementById('button2').addEventListener('click', io_ChangeState2);
  document.getElementById('button3').addEventListener('click', io_ChangeState3);
  document.getElementById('button4').addEventListener('click', io_ChangeState4);
  document.getElementById('buttontoggle').addEventListener('click', buttontoggle);

  document.getElementById('btnModbus').addEventListener('click', settingmodbus);
  document.getElementById('btnIo').addEventListener('click', settingio);
  document.getElementById('btnHome').addEventListener('click', Home);
  document.getElementById('buttonadd').addEventListener('click', AddCard);
  document.getElementById('bntLoadcard').addEventListener('click', loadCard);
  document.getElementById('buttonsavecard').addEventListener('click', SaveCard);
  document.getElementById('buttonlog').addEventListener('click', showlogin);
  document.getElementById('bntLoadtableid').addEventListener('click', loadtableid);
  document.getElementById('bntLoadproduct').addEventListener('click', loaddataproduct);
  document.getElementById('bntLoadtabledata').addEventListener('click', loadtabledata);
  // Gắn sự kiện click cho các nút mở modal
  document.getElementById('settingBtnNV').addEventListener('click', openSettingModalNV);
  document.getElementById('buttonadd').addEventListener('click', openAddCard);
  document.getElementById('settingBtnKL').addEventListener('click', Showfile);
  document.getElementById('settingBtnEX').addEventListener('click', TabShowData);
  document.getElementById('settingBtnLog').addEventListener('click', openSettingModalLog);
  document.getElementById('settingBtnList').addEventListener('click', openSettingModalList);
  document.getElementById('settingBtnSetting').addEventListener('click', openSettingModalSetting);
  document.getElementById('btntable').addEventListener('click', TabTableData);


  document.getElementById('bntsavecard').addEventListener('click', saveCard);
  document.getElementById('bntsaveproduct').addEventListener('click', savedataproduct);
  document.getElementById('bntsavetableid').addEventListener('click', saveTableID);
  document.getElementById('bntsavetabledata').addEventListener('click', saveTableData);


  document.getElementById('bntReloadcard').addEventListener('click', ReloadJsonCard);
  document.getElementById('bntReloadproduct').addEventListener('click', ReloadJsonProduct);
  document.getElementById('bntReloadtableid').addEventListener('click', ReloadJsonTableID);
  document.getElementById('bntReloadtabledata').addEventListener('click', ReloadJsonTableData);

  document.getElementById('bnttestcard').addEventListener('click', TestCard);
  document.getElementById('bnttestproduct').addEventListener('click', TestProduct);
  document.getElementById('bnttesttableid').addEventListener('click', TestTableID);
  document.getElementById('bnttesttabledata').addEventListener('click', TestTableData);
}
//test json
function TestCard() {
  buildCardJson();
}
function TestProduct() {
  preferenceslist = document.getElementById('dataProduct').value;
}
function TestTableID() {
  jsontableID = document.getElementById("datatableid").value;
  firstload = 1;
  loading = 0;
  loadTable(jsontableID);
}
function TestTableData() {
  jsontableData = document.getElementById("datatabledata").value;
  loaddata(jsontableData);
  addvaluecard(jsontableData);
  if (daloadvcard) updatevalue();
}
// load json
function loadtableid() {
  firstload = 1;
  loading = 0;
  LoadJson("TableID");
}
function loaddataproduct() {
  LoadJson("DataProduct");
}
function loadtabledata() {
  LoadJson("mbSlave");
}
function loadCard() {
  LoadJson("Application");
}
// save json
function saveCard() {
  SaveJson(document.getElementById('jsonApp').value, "Application");
}
function saveTableID() {
  SaveJson(document.getElementById('datatableid').value, "tableID");
}
function savedataproduct() {
  SaveJson(document.getElementById('dataProduct').value, "dataProduct");
}
function saveTableData() {
  SaveJson(document.getElementById('datatabledata').value, "mbSlave");
}// save json

//reload json 
function ReloadJsonCard() {
  document.getElementById("jsonApp").value = jsonAppInput;
}
function ReloadJsonProduct() {
  document.getElementById("dataProduct").value = preferenceslist;
}
function ReloadJsonTableID() {
  document.getElementById("datatableid").value = jsontableID;
}
function ReloadJsonTableData() {
  document.getElementById("datatabledata").value = jsonSlave;
}//reload json

function buttontoggle() {
  IsConnect = !IsConnect;
}


function settingmodbus() {
  document.getElementById("cardmodbus").style.display = "block";
  document.getElementById("cardio").style.display = "none";
  document.getElementById("cardhome").style.display = "none"
  document.getElementById("cardshowfile").style.display = "none"
  document.getElementById("cardshowdata").style.display = "none";
  document.getElementById("cardaddApp").style.display = "none";
  document.getElementById("cardtable").style.display = "none";

  document.getElementById("tabmodbus").style.display = "block";
  document.getElementById("tabHome").style.display = "none";
  document.getElementById("tabIO").style.display = "none";
  document.getElementById("tabtabledata").style.display = "none";

  document.getElementById("btnModbus").style.display = "none";
  document.getElementById("btnHome").style.display = "block";
  document.getElementById("btnIo").style.display = "block";
  document.getElementById("btntable").style.display = "block";


}
function settingio() {
  document.getElementById("cardmodbus").style.display = "none";
  document.getElementById("cardhome").style.display = "none"
  document.getElementById("cardio").style.display = "block";
  document.getElementById("cardshowfile").style.display = "none"
  document.getElementById("cardshowdata").style.display = "none";
  document.getElementById("cardaddApp").style.display = "none";
  document.getElementById("cardtable").style.display = "none";

  document.getElementById("tabmodbus").style.display = "none";
  document.getElementById("tabHome").style.display = "none";
  document.getElementById("tabIO").style.display = "block";
  document.getElementById("tabtabledata").style.display = "none";

  document.getElementById("btnModbus").style.display = "block";
  document.getElementById("btnHome").style.display = "block";
  document.getElementById("btnIo").style.display = "none";
  document.getElementById("btntable").style.display = "block";
}
function Home() {
  document.getElementById("cardmodbus").style.display = "none";
  document.getElementById("cardio").style.display = "none";
  document.getElementById("cardhome").style.display = "block";
  document.getElementById("cardshowfile").style.display = "none"
  document.getElementById("cardshowdata").style.display = "none";
  document.getElementById("cardaddApp").style.display = "none";
  document.getElementById("cardtable").style.display = "none";

  document.getElementById("tabmodbus").style.display = "none";
  document.getElementById("tabHome").style.display = "block";
  document.getElementById("tabIO").style.display = "none";
  document.getElementById("tabtabledata").style.display = "none";
  document.getElementById("btnHome").style.display = "none";

  document.getElementById("btnModbus").style.display = "block";
  document.getElementById("btnHome").style.display = "none";
  document.getElementById("btnIo").style.display = "block";
  document.getElementById("btntable").style.display = "block";
}
function TabTableData() {
  document.getElementById("cardmodbus").style.display = "none";
  document.getElementById("cardio").style.display = "none";
  document.getElementById("cardhome").style.display = "none";
  document.getElementById("cardshowfile").style.display = "none"
  document.getElementById("cardshowdata").style.display = "none";
  document.getElementById("cardaddApp").style.display = "none";
  document.getElementById("cardtable").style.display = "block";

  document.getElementById("tabmodbus").style.display = "none";
  document.getElementById("tabHome").style.display = "none";
  document.getElementById("tabIO").style.display = "none";
  document.getElementById("tabtabledata").style.display = "block";

  document.getElementById("btnModbus").style.display = "block";
  document.getElementById("btnHome").style.display = "block";
  document.getElementById("btnIo").style.display = "block";
  document.getElementById("btntable").style.display = "none";
}
function Showfile() {
  document.getElementById("cardmodbus").style.display = "none";
  document.getElementById("cardio").style.display = "none";
  document.getElementById("cardhome").style.display = "none";
  document.getElementById("cardshowfile").style.display = "block"
  document.getElementById("cardshowdata").style.display = "none";
  document.getElementById("cardaddApp").style.display = "none";
  document.getElementById("cardtable").style.display = "none";

  document.getElementById("tabmodbus").style.display = "none";
  document.getElementById("tabHome").style.display = "none";
  document.getElementById("tabIO").style.display = "none";
  document.getElementById("tabtabledata").style.display = "none";

  document.getElementById("btnModbus").style.display = "block";
  document.getElementById("btnHome").style.display = "block";
  document.getElementById("btnIo").style.display = "block";
  document.getElementById("btntable").style.display = "block";

}
function TabShowData() {
  document.getElementById("cardmodbus").style.display = "none";
  document.getElementById("cardio").style.display = "none";
  document.getElementById("cardhome").style.display = "none";
  document.getElementById("cardshowfile").style.display = "none";
  document.getElementById("cardshowdata").style.display = "block";
  document.getElementById("cardaddApp").style.display = "none";
  document.getElementById("cardtable").style.display = "none";

  document.getElementById("tabmodbus").style.display = "none";
  document.getElementById("tabHome").style.display = "none";
  document.getElementById("tabIO").style.display = "none";
  document.getElementById("tabtabledata").style.display = "none";

  document.getElementById("btnModbus").style.display = "block";
  document.getElementById("btnHome").style.display = "block";
  document.getElementById("btnIo").style.display = "block";
  document.getElementById("btntable").style.display = "block";
}
function AddCard() {
  document.getElementById("cardmodbus").style.display = "none";
  document.getElementById("cardio").style.display = "none";
  document.getElementById("cardhome").style.display = "block";
  document.getElementById("cardshowfile").style.display = "none";
  document.getElementById("cardshowdata").style.display = "none";
  document.getElementById("cardaddApp").style.display = "block";
  document.getElementById("cardtable").style.display = "none";
}
function save() {
  var ssid_input = document.getElementById('input_ssid').value;
  var pass_input = document.getElementById('input_pass').value;
  var waddress_input = document.getElementById('waddress').value;
  var wgetway_input = document.getElementById('wgetway').value;
  var wsubnet_input = document.getElementById('wsubnet').value;
  var staticip_input = document.getElementById('staticip').value;
  var wifimode_input = document.getElementById('wifimode').value;
  if (ssid_input == "") {
    console.log("SSID Emty");
    alert("SSID Emty");
  }
  else if (pass_input == "") {
    console.log("Pass Emty");
    alert("Pass Emty");
  }
  else if (waddress_input == "") {
    console.log("Web Address Emty");
    alert("Web Address Emty");
  }
  else if (wgetway_input == "") {
    console.log("Web Getway Emty");
    alert("Web Getway Emty");
  }
  else if (wsubnet_input == "") {
    console.log("Web Subnet Emty");
    alert("Web Subnet Emty");
  }
  else {
    var json_output = "{'Command':'settingWifi','SSID':'" + ssid_input + "','PASS':'" + pass_input + "','waddress':'" + waddress_input + "','wgetway':'" + wgetway_input + "','wsubnet':'" + wsubnet_input + "','staip':'" + staticip_input + "','wmode':'" + wifimode_input + "'}";
    console.log(json_output);
    websocket.send(json_output);
  }
}
function openSettingModalNV() {
  var modal = new bootstrap.Modal(document.getElementById('settingModalNV'));
  modal.show();
}

// Hàm mở modal cho EX
function openSettingModalEX() {
  var modal = new bootstrap.Modal(document.getElementById('settingModalEX'));
  modal.show();
}
// Hàm mở modal cho Log
function openSettingModalLog() {
  var modal = new bootstrap.Modal(document.getElementById('settingModalLog'));
  loadLog();
  modal.show();
}
// Hàm mở modal cho List
function openSettingModalList() {
  var modal = new bootstrap.Modal(document.getElementById('settingModalList'));
  modal.show();
}
// Hàm mở modal cho Settings
function openSettingModalSetting() {
  var modal = new bootstrap.Modal(document.getElementById('settingModalSetting'));
  modal.show();
}

function SelectTab(evt, cityName) {
  var i, tabcontent, tablinks;
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = "none";
  }
  tablinks = document.getElementsByClassName("tablinks");
  for (i = 0; i < tablinks.length; i++) {
    tablinks[i].className = tablinks[i].className.replace(" active", "");
  }
  document.getElementById(cityName).style.display = "block";
  evt.currentTarget.className += " active";
}

if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function (e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function (e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('message', function (e) {
    console.log("message", e.data);
  }, false);
}
//table file
function tablefile(jsonValue) {
  var TableHTML = "";
  TableHTML = "<table class=\"table table-borderless \"><thead class=\"thead-dark\"><th>STT</th><th>Time</th><th>File Name</th><th>Use Space</th></thead><tbody>";
  var file = JSON.parse(jsonValue);
  var stt = 0;
  var total;
  var use = 0;
  var free = 0;
  for (var i = 0; i < file.ListFile.length; i++) {
    stt++;
    var h = new Date().getHours();
    var m = new Date().getMinutes();
    var s = new Date().getSeconds();
    const space = file.ListFile[i].space;
    var thoigian = h + ":" + m + ":" + s;
    var name = file.ListFile[i].name;
    total = file.ListFile[i].total;
    TableHTML += "<tr><td>" + stt + "</td><td>" + thoigian + "</td><td>" + name + "</td><td>" + (space / 1024).toFixed(2) + "KB" + "</td></tr>";
    use += Number(file.ListFile[i].space);
    free = total - use;
  }
  TableHTML += "</tbody></table>";
  document.getElementById("TableFile").innerHTML = TableHTML;
  document.getElementById("total").innerHTML = (total / 1024).toFixed(2) + "KB";
  document.getElementById("use").innerHTML = (use / 1024).toFixed(2) + "KB";
  document.getElementById("free").innerHTML = (free / 1024).toFixed(2) + "KB";
}