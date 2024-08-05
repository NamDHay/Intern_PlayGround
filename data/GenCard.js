// Hàm mở modal card App
function openAddCard() {
    var modal = new bootstrap.Modal(document.getElementById('cardaddApp'));
    modal.show();
}
// Select type card
// const selectcard = document.getElementById('selectcard');
//     selectcard.addEventListener('change', function handleChange(event) {
//         var chooseCard = document.getElementById("selectcard").value;
//         if(chooseCard == "0"){
//             document.getElementById("plc").style.display = "block"
//             document.getElementById("od2000").style.display = "none";
//             document.getElementById("wtmdistance").style.display = "none";
//             document.getElementById("wtmxyz").style.display = "none";
//         }
//         if(chooseCard == "1"){
//             document.getElementById("plc").style.display = "none"
//             document.getElementById("od2000").style.display = "block";
//             document.getElementById("wtmdistance").style.display = "none";
//             document.getElementById("wtmxyz").style.display = "none";
//         }
//         if(chooseCard == "2"){
//             document.getElementById("plc").style.display = "none"
//             document.getElementById("od2000").style.display = "none";
//             document.getElementById("wtmdistance").style.display = "block";
//             document.getElementById("wtmxyz").style.display = "none";
//         }
//         if(chooseCard == "3"){
//             document.getElementById("plc").style.display = "none"
//             document.getElementById("od2000").style.display = "none";
//             document.getElementById("wtmdistance").style.display = "none";
//             document.getElementById("wtmxyz").style.display = "block";
//         }
        
//     });
//HTML card
function AddCardBody() {
    html += "<div class='col-14 cardproduct'>\
        <Center>\
        <div class='statecard' id=\"State0_" + id_card + "\" style='position: relative; right: 20px; text-align: right'>state:Stop</div>\
            <h2><span id=\"namecard"+ id_card + "\"></span> </h2>\
            <p class=\"statecard\">ID Card: "+ id_card + "</p>\
            <div class=\"container overflow-hidden text-center\">\
            <div class=\"row gy-3\">\
                <div class=\"col-5\">\
                <div class=\"statecard\">Plan:</div>\
                </div>\
                <div class=\"col-6\">\
                <div class=\"statecard\" id=\"card"+ id_card + "value1\"></div>\
                </div>\
                <div class=\"col-5\">\
                <div class=\"statecard\">Result:</div>\
                </div>\
                <div class=\"col-6\">\
                <div class=\"statecard\" id=\"card"+ id_card + "value2\"></div>\
                </div>\
                <div class=\"col-5\">\
                <div class=\"statecard\">Plan set:</div>\
                </div>\
                <div class=\"col-6\">\
                <div class=\"statecard\" id=\"card"+ id_card + "value3\"></div>\
                </div>\
                <div class=\"col-5\">\
                <div class=\"statecard\">Name Product:</div>\
                </div>\
                <div class=\"col-6\">\
                <div class=\"statecard\" id=\"card"+ id_card + "value4\"></div>\
                </div>\
                <div class=\"col-5\">\
                <div class=\"statecard\">Cycle Time:</div>\
                </div>\
                <div class=\"col-6\">\
                <div class=\"statecard\" id=\"card"+ id_card + "value5\"></div>\
                </div>\
                <div class=\"col-5\">\
                <div class=\"statecard\">Total Plan:</div>\
                </div>\
                <div class=\"col-6\">\
                <div class=\"statecard\" id=\"card"+ id_card + "value6\"></div>\
                </div>\
                <div class=\"col-4\"><button class=\"buttonA\" onclick=\"CardSetup_dlg('Edit Parameter',"+ id_card + ")\">Setting</button></div>\
                <div class=\"col-4\"><button id=\"buttonrun"+ id_card + "\" class=\"buttonA\" onclick='changeState(" + id_card + ")'>Run</button></div>\
                <div class=\"col-4\"><button id=\"buttonreset"+ id_card + "\" class=\"buttonA\" onclick='changeReset(" + id_card + ")'>Reset</button></div>\
            </div>\
            </Center>\
        </div>\
        </div><br></br>";
}
//select reg from table
function addvaluecard() {
    selectvalue2 = document.getElementById('selectvalue2').value;
    selectvalue1 = document.getElementById('selectvalue1').value;
    selectvalue3 = document.getElementById('selectvalue3').value;
    selectvalue4 = document.getElementById('selectvalue4').value;
    selectvalue5 = document.getElementById('selectvalue5').value;
    selectvalue6 = document.getElementById('selectvalue6').value;
    selectvalue7 = document.getElementById('selectvalue7').value;

    var chooseSlave = document.getElementById("selectreg").value;
    
    if (chooseSlave != "") {
        var re = document.getElementById('re' + chooseSlave).innerText;
        var rs = document.getElementById('rs' + chooseSlave).innerText;
        var length = re - rs + 1;
        for (var j = 0; j < length; j++) {
            if (selectvalue1 == Number(rs) + j) {
                document.getElementById("value1").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (selectvalue2 == Number(rs) + j) {
                document.getElementById("value2").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (selectvalue3 == Number(rs) + j) {
                document.getElementById("value3").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (selectvalue4 == Number(rs) + j) {
                document.getElementById("value4").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (selectvalue5 == Number(rs) + j) {
                document.getElementById("value5").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (selectvalue6 == Number(rs) + j) {
                document.getElementById("value6").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (selectvalue7 == Number(rs) + j) {
                document.getElementById("value7").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
        }
    }
}
//Show reg option from table
function changeRegOptions(jsonValue) {
    var value = 0;
    var selectreg = 0;
    var htmlcard = "";
    var htmlselectreg = "";
    var rs;
    var re;
    var length;
    var jsontableid = JSON.parse(jsonValue);
    htmlselectreg += "<option value=\"\">---Select NodeID---</option>";
    for (var i = 0; i < numSlave; i++) {
        selectreg = jsontableid.Data[i].ID;
        htmlselectreg += "<option value=" + i + ">" + selectreg + "</option>";
    }
    document.getElementById("selectreg").innerHTML = htmlselectreg;
    htmlselectreg = "";
    const selectnodeID = document.getElementById('selectreg');
    selectnodeID.addEventListener('change', function handleChange(event) {
        var chooseSlave = document.getElementById("selectreg").value;
        for (var k = 1; k < 8; k++) {
            if (chooseSlave == "") {
                htmlcard += "<option value=\"\"> </option>";
                htmlcard = "";
            } else {
                rs = document.getElementById('rs' + chooseSlave).innerText;
                re = document.getElementById('re' + chooseSlave).innerText;
                length = re - rs + 1;
                for (var j = 0; j < length; j++) {
                    
                    if (Number(rs) + j == null) break;
                    value = Number(rs) + j;
                    htmlcard += "<option value=" + value + ">" + value + "</option>";
                }
            }
            document.getElementById("selectvalue" + k).innerHTML = htmlcard;
            htmlcard = "";
        }
    });
}

//Save add card
function SaveCard() {
    var jsontableid = JSON.parse(jsontableID);
    AppID = document.getElementById("selectreg").value;
    nodeID = jsontableid.Data[document.getElementById("selectreg").value].ID;
    namecard = document.getElementById("input_namecard").value;
    typecard = document.getElementById("selectcard").value;
   
    appData[app] = AppID + "," + typecard + "," + app + "," + nodeID + "," + netID + "," + namecard + "," + selectvalue1 + "," + selectvalue2 + "," + selectvalue3 + "," + selectvalue4 + "," + selectvalue5 + "," + selectvalue6 + "," + selectvalue7;
    jsonApp = "{\"Command\":\"SaveFile\",\"Filename\":\"Application\",\"Data\":[{\"app\":\"" + appData[0] + "\"}";
    if (app > 0) {
        for (var k = 1; k < (app + 1); k++) {
            jsonApp += ",{\"app\":\"" + appData[k] + "\"}";
        }
    }

    jsonApp += "]}";
    document.getElementById("jsonApp").value = jsonApp;
    console.log(jsonApp);
    buildcard(jsonApp);
    websocket.send(jsonApp);
    app++;
    console.log("App ID:" + app);

}
// Build card from Json
function buildCardJson() {
    jsonAppInput = document.getElementById("jsonApp").value;
    jsonApp = jsonAppInput;
    app = JSON.parse(jsonAppInput).Data.length;
    for (var i = 0; i < (app); i++) {
        AppID = JSON.parse(jsonAppInput).Data[i].app.split(",")[0];
        typecard = JSON.parse(jsonAppInput).Data[i].app.split(",")[1];
        nodeID = JSON.parse(jsonAppInput).Data[i].app.split(",")[3];
        netID = JSON.parse(jsonAppInput).Data[i].app.split(",")[4];
        namecard = JSON.parse(jsonAppInput).Data[i].app.split(",")[5];
        selectvalue1 = JSON.parse(jsonAppInput).Data[i].app.split(",")[6];
        selectvalue2 = JSON.parse(jsonAppInput).Data[i].app.split(",")[7];
        selectvalue3 = JSON.parse(jsonAppInput).Data[i].app.split(",")[8];
        selectvalue4 = JSON.parse(jsonAppInput).Data[i].app.split(",")[9];
        selectvalue5 = JSON.parse(jsonAppInput).Data[i].app.split(",")[10];
        selectvalue6 = JSON.parse(jsonAppInput).Data[i].app.split(",")[11];
        selectvalue7 = JSON.parse(jsonAppInput).Data[i].app.split(",")[12];
        appData[i] = AppID + "," + typecard + "," + i + "," + nodeID + "," + netID + "," + namecard + "," + selectvalue1 + "," + selectvalue2 + "," + selectvalue3 + "," + selectvalue4 + "," + selectvalue5 + "," + selectvalue6 + "," + selectvalue7;
        jsonApp = "{\"Command\":\"SaveFile\",\"Filename\":\"Application\",\"Data\":[{\"app\":\"" + appData[0] + "\"}";
        if (i > 0) {
            for (var k = 1; k < (app); k++) {
                jsonApp += ",{\"app\":\"" + appData[k] + "\"}";
            }
        }
        jsonApp += "]}";
    }
    console.log(jsonAppInput);
    websocket.send(jsonAppInput);
    buildcard(jsonAppInput);
}
//update value card
function updatevalue() {
    var jsonObj = JSON.parse(jsonAppInput);
    var appLen = jsonObj.Data.length;
    for (var i = 0; i < appLen; i++) {
        var arrayData = [AppID, typecard, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6, value7] = jsonObj.Data[i].app.split(",");
        var chooseSlave = document.getElementById("selectreg").value;
        if (chooseSlave == "") chooseSlave = arrayData[0];
        var rs = document.getElementById('rs' + chooseSlave).innerText;
        var re = document.getElementById('re' + chooseSlave).innerText;
        var length = re - rs + 1;
        var jsontabledata = JSON.parse(jsontableData);
        for (var j = 0; j < length; j++) {
            if (value1 == Number(rs) + j) {
                document.getElementById("card" + i + "value1").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (value2 == Number(rs) + j) {
                document.getElementById("card" + i + "value2").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (value3 == Number(rs) + j) {
                document.getElementById("card" + i + "value3").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (value4 == Number(rs) + j) {
                document.getElementById("card" + i + "value4").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (value5 == Number(rs) + j) {
                document.getElementById("card" + i + "value5").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
            if (value6 == Number(rs) + j) {
                document.getElementById("card" + i + "value6").innerHTML = document.getElementById("value" + chooseSlave + "_" + j).innerText;
            }
        }
    }
}
//build card
function buildcard(jsoninput) {
    html = "";
    jsonAppInput = jsoninput;
    var jsonObj = JSON.parse(jsoninput);

    var appLen = jsonObj.Data.length;
    for (var i = 0; i < appLen; i++) {
        var arrayData = [AppID, typecard, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6, value7] = jsonObj.Data[i].app.split(",");
        AddCardBody();
    }
    document.getElementById("addcard").innerHTML = html;

    for (var i = 0; i < appLen; i++) {
        document.getElementById("namecard" + i).innerHTML = jsonObj.Data[i].app.split(",")[5];
    }
    daloadvcard = 1;
    updatevalue();
 
}
