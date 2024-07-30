// Hàm mở modal card App
function openAddCard() {
    var modal = new bootstrap.Modal(document.getElementById('cardaddApp'));
    modal.show();
}
//HTML card
function AddCardBody() {
    html += "<div class='col-14 cardproduct'>\
          <Center>\
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
                <div class=\"col-4\"><button id=\"buttonrun "+ id_card + "\" class=\"buttonA\" onclick='changeState(" + id_card + ")'>Run</button></div>\
                <div class=\"col-4\"><button id=\"buttonreset "+ id_card + "\" class=\"buttonA\" onclick='changeReset(" + id_card + ")'>Reset</button></div>\
              </div>\
            </Center>\
          </div>\
        </div><br></br>";
}
//select reg from table
function addvaluecard(jsonValue) {
    selectvalue2 = document.getElementById('selectvalue2').value;
    selectvalue1 = document.getElementById('selectvalue1').value;
    selectvalue3 = document.getElementById('selectvalue3').value;
    selectvalue4 = document.getElementById('selectvalue4').value;
    selectvalue5 = document.getElementById('selectvalue5').value;
    selectvalue6 = document.getElementById('selectvalue6').value;
    selectvalue7 = document.getElementById('selectvalue7').value;
    var jsontabledata = JSON.parse(jsonValue);// data
    var jsontableid = JSON.parse(jsontableID);// id

    var chooseSlave = document.getElementById("selectreg").value;
    if (chooseSlave != "") {
        for (var j = 0; j < jsontableid.Data[chooseSlave].Data.length; j++) {
            if (selectvalue1 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("value1").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (selectvalue2 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("value2").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (selectvalue3 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("value3").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (selectvalue4 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("value4").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (selectvalue5 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("value5").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (selectvalue6 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("value6").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (selectvalue7 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("value7").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
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
                for (var j = 0; j < jsontableid.Data[chooseSlave].Data.length; j++) {
                    // value = addresstbale[i];
                    if (jsontableid.Data[chooseSlave].Data[j][0] == null) break;
                    value = jsontableid.Data[chooseSlave].Data[j][0];
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
    appData[app] = AppID + "," + poss + "," + app + "," + nodeID + "," + netID + "," + namecard + "," + selectvalue1 + "," + selectvalue2 + "," + selectvalue3 + "," + selectvalue4 + "," + selectvalue5 + "," + selectvalue6 + "," + selectvalue7;
    jsonApp = "{\"Command\":\"App\",\"Data\":[{\"app\":\"" + appData[0] + "\"}";
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
        poss = JSON.parse(jsonAppInput).Data[i].app.split(",")[1];
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
        appData[i] = AppID + "," + poss + "," + i + "," + nodeID + "," + netID + "," + namecard + "," + selectvalue1 + "," + selectvalue2 + "," + selectvalue3 + "," + selectvalue4 + "," + selectvalue5 + "," + selectvalue6 + "," + selectvalue7;
        jsonApp = "{\"Command\":\"App\",\"Data\":[{\"app\":\"" + appData[0] + "\"}";
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
    var jsontableid = JSON.parse(jsontableID);
    for (var i = 0; i < appLen; i++) {
        var arrayData = [AppID, poss, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6, value7] = jsonObj.Data[i].app.split(",");
        var chooseSlave = document.getElementById("selectreg").value;
        if (chooseSlave == "") chooseSlave = arrayData[0];
        var jsontabledata = JSON.parse(jsontableData);
        for (var j = 0; j < jsontableid.Data[chooseSlave].Data.length; j++) {
            if (value1 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("card" + i + "value1").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (value2 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("card" + i + "value2").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (value3 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("card" + i + "value3").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (value4 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("card" + i + "value4").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (value5 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("card" + i + "value5").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
            }
            if (value6 == jsontableid.Data[chooseSlave].Data[j][0]) {
                document.getElementById("card" + i + "value6").innerHTML = jsontabledata.Data[chooseSlave].Data[j];
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
        var arrayData = [AppID, poss, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6, value7] = jsonObj.Data[i].app.split(",");
        AddCardBody();
    }
    document.getElementById("addcard").innerHTML = html;

    for (var i = 0; i < appLen; i++) {
        document.getElementById("namecard" + i).innerHTML = jsonObj.Data[i].app.split(",")[5];
    }
    daloadvcard = 1;
    updatevalue();
}