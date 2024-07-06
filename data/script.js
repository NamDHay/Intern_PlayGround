var xValues = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23];
var yValues = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23];
var gateway = `ws://192.168.1.128/ws`;
// var gateway = `ws://192.168.245.54/ws`;
var websocket;
window.addEventListener('load', onLoad);

function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}
function onOpen(event) {
  console.log('Connection opened');
}
function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 1);
}
function onMessage(event) {
  var state;
  if (event.data == "1") {
    state = "ON";
  }
  else {
    state = "OFF";
  }
  document.getElementById('state').innerHTML = state;

}

function onLoad(event) {
  initWebSocket();
  initButton();
}
function initButton() {
  document.getElementById('buttonSave').addEventListener('click', save);
  document.getElementById('button').addEventListener('click', toggle);
  document.getElementById('tabWifi').addEventListener('click', settingwifi);
  document.getElementById('tabIO').addEventListener('click', settingio);
  document.getElementById('tabHome').addEventListener('click', Home);
}
function settingwifi() {
  document.getElementById("cardwifi").style.display = "block";
  document.getElementById("cardio").style.display = "none";
}
function settingio() {
  document.getElementById("cardwifi").style.display = "none";
  document.getElementById("cardio").style.display = "block";
  new Chart("myChart", {
    type: "line",
    data: {
      labels: xValues,
      datasets: [{
        fill: false,
        lineTension: 0,
        backgroundColor: "rgba(0,0,255,1.0)",
        borderColor: "rgba(0,0,255,0.1)",
        data: yValues
      }]
    },
    options: {
      legend: { display: false },
      scales: {
        yAxes: [{ ticks: { min: 0, max: 24 } }],
      }
    }
  });
}
function Home() {
  document.getElementById("cardwifi").style.display = "none";
  document.getElementById("cardio").style.display = "none";
}
function save() {
  var ssid_input = document.getElementById('input_ssid').value;
  var pass_input = document.getElementById('input_pass').value;
  var waddress_input = document.getElementById('input_waddress').value;
  var wgetway_input = document.getElementById('input_wgetway').value;
  var wsubnet_input = document.getElementById('input_wsubnet').value;
  // "{'SSID':'isoft','PASS':'i-soft@123'}"
  if (ssid_input == "") {
    console.log("chua nhap ssid ");
    alert("chua nhap ssid ");
  } else if (pass_input == "") {
    console.log("chua nhap password");
    alert("chua nhap password");
  } else if (waddress_input == "") {
    console.log("chua nhap waddress");
    alert("chua nhap waddress");
  } else if (wgetway_input == "") {
    console.log("chua nhap wgetway");
    alert("chua nhap wgetway");
  } else if (wsubnet_input == "") {
    console.log("chua nhap wsubnet");
    alert("chua nhap wsubnet");
  } else {
    // var json_output = "{'SSID':'" + ssid_input + "','PASS':'" + pass_input + "'}";
    var json_output = "{'SSID':'" + ssid_input + "','PASS':'" + pass_input + "','waddress':'" + waddress_input +"','wgetway':'" + wgetway_input +"','wsubnet':'" + wsubnet_input +"'}";
    console.log(json_output);
    websocket.send(json_output);
    alert("setting success!!!");
  }
}
function toggle() {
  console.log('toggle');
  websocket.send('toggle');
}
