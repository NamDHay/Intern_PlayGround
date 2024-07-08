var gateway = `ws://192.168.1.128/ws`;
// var gateway = `ws://172.16.16.18/ws`;
// var gateway = 'ws://' + window.location.hostname + '/ws';
var websocket;
var IsConnect;

var io_array = [1, 1, 1, 1, 1, 1, 1, 1];

var io_obj = "{\"Command\": \"getIO\",\"Data\":[]}";
let io_arr = [];



function getReadings() {
  con
  sole.log(io_array.length);
  for (var i = 0; i < io_array.length; i++) {
    console.log("Get IO: " + io_array[i]);
  }
}

const intervalId = setInterval(intervalHandle, 1000);
window.addEventListener('beforeunload', () => clearInterval(intervalId));
window.addEventListener('load', onLoad);
window.addEventListener('load', getReadings);

function initWebSocket() {
  // console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}
function onOpen(event) {
  console.log('Connection opened');
  IsConnect = true;
}
function onClose(event) {
  IsConnect = false;
  console.log('Connection closed');
  setTimeout(initWebSocket, 1000);
}
function onMessage(event) {
  var state;
  var message = JSON.parse(event.data);
  if (message.Command == "toggleLed") {
    if (message.Data == "0")
      state = "ON";
    else state = "OFF";
    document.getElementById('state').innerHTML = state;
  } else if (message.Command == "getIO") {

  } else if (message.Command == "settingWifi") {
    alert("Setting Done")
  }
}

function onLoad(event) {
  initWebSocket();
  initButton();
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
  document.getElementById('tabWifi').addEventListener('click', settingwifi);
  document.getElementById('tabIO').addEventListener('click', settingio);
  document.getElementById('tabHome').addEventListener('click', Home);

  document.getElementById('buttonSave').addEventListener('click', wifi_save);
  document.getElementById('mbButtonSave').addEventListener('click', modbus_save);
  document.getElementById('button1').addEventListener('click', io_ChangeState1);
  document.getElementById('button2').addEventListener('click', io_ChangeState2);
  document.getElementById('button3').addEventListener('click', io_ChangeState3);
  document.getElementById('button4').addEventListener('click', io_ChangeState4);
}

function intervalHandle() {
  var json_output;
  if (IsConnect == true) {
    var JSonObj = JSON.parse(io_obj);
    for (var i = 0; i < io_array.length; i++) {
      JSonObj.Data[i] = io_array[i];
      // console.log("Get IO: " + io_array[i]);
      // console.log("JSonObj.Data: " + JSonObj.Data[i]);
      // io_arr.push(io_obj);
    }
    var StringJson = JSON.stringify(JSonObj);
    console.log(StringJson);

    json_output = "{'Command':'toggleLed'}";
    websocket.send(json_output);

    // json_output = "{'Command':'getIONO','Data':'" + io_array + "'}";
    websocket.send(StringJson);
    document.getElementById('input1').innerHTML = io_array[0];
    document.getElementById('input2').innerHTML = io_array[1];
    document.getElementById('input3').innerHTML = io_array[2];
    document.getElementById('input4').innerHTML = io_array[3];
    document.getElementById('output1').innerHTML = io_array[4];
    document.getElementById('output2').innerHTML = io_array[5];
    document.getElementById('output3').innerHTML = io_array[6];
    document.getElementById('output4').innerHTML = io_array[7];
    // convertDisplay();
    var JSonObj = JSON.parse(io_obj);
    for (var i = 0; i < io_array.length; i++) {
      JSonObj.Data[i] = io_array[i];
    }
    var StringJson = JSON.stringify(JSonObj);
    console.log(StringJson);
  }//if(IsConnect == true) {
}

// function convertDisplay() {
//   if (io_array[0] == "0") input1 = "ON";
//   else input1 = "OFF";
//   if (io_array[1] == "0") input2 = "ON";
//   else input2 = "OFF";
//   if (io_array[2] == "0") input3 = "ON";
//   else input3 = "OFF";
//   if (io_array[3] == "0") input4 = "ON";
//   else input4 = "OFF";

//   if (io_array[4] == "0") output1 = "ON";
//   else output1 = "OFF";
//   if (io_array[5] == "0") output2 = "ON";
//   else output2 = "OFF";
//   if (io_array[6] == "0") output3 = "ON";
//   else output3 = "OFF";
//   if (io_array[7] == "0") output4 = "ON";
//   else output4 = "OFF";
// }

function settingwifi() {
  document.getElementById("cardwifi").style.display = "block";
  document.getElementById("cardio").style.display = "none";
  document.getElementById("cardhome").style.display = "none"
}
function settingio() {
  document.getElementById("cardwifi").style.display = "none";
  document.getElementById("cardhome").style.display = "none"
  document.getElementById("cardio").style.display = "block";
}
function Home() {
  document.getElementById("cardwifi").style.display = "none";
  document.getElementById("cardio").style.display = "none";
  document.getElementById("cardhome").style.display = "block";
}

function wifi_save() {
  var ssid_input = document.getElementById('input_ssid').value;
  var pass_input = document.getElementById('input_pass').value;
  var waddress_input = document.getElementById('input_waddress').value;
  var wgetway_input = document.getElementById('input_wgetway').value;
  var wsubnet_input = document.getElementById('input_wsubnet').value;
  var staip = document.getElementById('staip').value;
  var wmode = document.getElementById('wmode').value;
  // "{'SSID':'isoft','PASS':'i-soft@123'}"
  if (ssid_input == "") {
    alert("chua nhap ssid ");
  } else if (pass_input == "") {
    alert("chua nhap password");
  } else if (waddress_input == "") {
    alert("chua nhap waddress");
  } else if (wgetway_input == "") {
    alert("chua nhap wgetway");
  } else if (wsubnet_input == "") {
    alert("chua nhap wsubnet");
  } else {
    var json_output = "{'Command':'settingWifi','SSID':'" + ssid_input + "','PASS':'" + pass_input + "','waddress':'" + waddress_input + "','wgetway':'" + wgetway_input + "','wsubnet':'" + wsubnet_input + "','staticIP':'" + staip + "','wmode':'" + wmode + "'}";
    console.log(json_output);
    websocket.send(json_output);
  }
}

function modbus_save() {
  var slaveID = document.getElementById('slaveID').value;
  var baud = document.getElementById('Baudrate').value;
  var readStart = document.getElementById('readStart').value;
  var readEnd = document.getElementById('readEnd').value;
  var writeStart = document.getElementById('writeStart').value;
  var writeEnd = document.getElementById('writeEnd').value;
  var serial = document.getElementById('serial').value;
  var mbmaster = document.getElementById('mbmaster').value;
  // "{'SSID':'isoft','PASS':'i-soft@123'}"
  if (slaveID == "") {
    alert("chua nhap slaveID ");
  } else if (baud == "") {
    alert("chua nhap baudrate");
  } else if (readStart == "") {
    alert("chua nhap Dia chi doc dau tien");
  } else if (readEnd == "") {
    alert("chua nhap Dia chi doc ket thuc");
  } else if (writeStart == "") {
    alert("chua nhap Dia chi ghi dau tien");
  } else if (writeEnd == "") {
    alert("chua nhap Dia chi ghi ket thuc");
  } else {
    var json_output = "{'Command':'settingModbus','slaveID':'" + slaveID + "','baud':'" + baud + "','readStart':'" + readStart + "','readEnd':'" + readEnd + "','writeStart':'" + writeStart + "','writeEnd':'" + writeEnd + "','serial':'" + serial + "','mbmaster':'" + mbmaster + "'}";
    console.log(json_output);
    websocket.send(json_output);
  }

  // websocket.send("getinfo
}

var chartT = new Highcharts.Chart({
  chart: {
    renderTo: 'chart-temperature'
  },
  series: [
    {
      name: 'Heap',
      type: 'line',
      color: '#101D42',
      marker: {
        symbol: 'circle',
        radius: 3,
        fillColor: '#101D42',
      }
    },
    {
      name: 'RAM ',
      type: 'line',
      color: '#71B48D',
      marker: {
        symbol: 'triangle-down',
        radius: 3,
        fillColor: '#71B48D',
      }
    },
  ],
  title: {
    text: undefined
  },
  xAxis: {
    type: 'datetime',
    dateTimeLabelFormats: { second: '%H:%M:%S' }
  },
  yAxis: {
    title: {
      text: 'Memory'
    }
  },
  credits: {
    enabled: false
  }
});

function plotTemperature(jsonValue) {

  var keys = Object.keys(jsonValue);
  console.log(keys);
  console.log(keys.length);

  for (var i = 0; i < keys.length; i++) {
    var x = (new Date()).getTime();
    console.log(x);
    const key = keys[i];
    var y = Number(jsonValue[key]);
    console.log(y);

    if (chartT.series[i].data.length > 40) {
      chartT.series[i].addPoint([x, y], true, true, true);
    } else {
      chartT.series[i].addPoint([x, y], true, false, true);
    }
  }
}
function getReadings() {
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var myObj = JSON.parse(this.responseText);
      console.log(myObj);
      plotTemperature(myObj);
    }
  };
  // websocket.send("getinfo");
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

  source.addEventListener('new_readings', function (e) {
    console.log("new_readings", e.data);
    var myObj = JSON.parse(e.data);
    console.log(myObj);
    plotTemperature(myObj);
  }, false);
}
