var loadcardOD2000 = 0;
var loadcard_WTM10L = 0;
var loadcard_MPB10 = 0;
var loadcard_CSS = 0;
var loadcard_PBS = 0;
function gencard_ODOD2000(message) {
    loadcardOD2000++;
    var key = Object.keys(message);
    var value = message[key].value;
    console.log(value);
    if (loadcardOD2000 == 1) {
        html += "<div class='col-14 cardproduct'>\
        <Center>\
            <h2><span>" + key + "</span> </h2>\
            <div class=\"container overflow-hidden text-center\">\
                <div class=\"row gy-3\">\
                    <div class=\"col-5\">\
                        <div class=\"statecard\">Distance:</div>\
                    </div>\
                    <div class=\"col-6\">\
                        <div class=\"statecard\" id=\"cardvalue"+ key + "\"></div>\
                    </div>\
                </div><br>\
                <div class=\"row gy-3\">\
                    <div class=\"col-5\">\
                        <span class=\"statecard\"> Q1 </span><svg class=\"bi\" width=\"24\" height=\"24\" id=\""+ key + "_Q1\"\
                        style = \"color: rgb(255, 0, 0);\" >\
                        <use xlink:href=\"#icon-connect\"></use>\
                        </svg>\
                    </div>\
                    <div class=\"col-5\">\
                        <span class=\"statecard\"> Q2 </span><svg class=\"bi\" width=\"24\" height=\"24\" id=\""+ key + "_Q2\"\
                        style = \"color: rgb(255, 0, 0);\" >\
                        <use xlink:href=\"#icon-connect\"></use>\
                        </svg>\
                    </div>\
                </div>\
            </div>\
        </Center>\
        </div><br></br>";

        document.getElementById("addcard").innerHTML = html;
    }
    if ((value[0]).toFixed(2) == "2130.00")
        document.getElementById("cardvalue" + key).innerHTML = "Out Range";
    else
        document.getElementById("cardvalue" + key).innerHTML = (value[0]).toFixed(2) + "mm";
    document.getElementById(key + "_Q1").style = 'background-color: rgb(0,0,0)';
    document.getElementById(key + "_Q2").style = 'background-color: rgb(0,0,0)';
    if ((value[1]).toFixed(0) == "1")
        document.getElementById(key + "_Q1").style = 'background-color: rgb(0,255,0)';
    if ((value[2]).toFixed(0) == "1")
        document.getElementById(key + "_Q2").style = 'background-color: rgb(0,255,0)';
}
function gencard_PBS(message) {
    var key = Object.keys(message);
    var value = message[key].value;
    console.log(value);
    loadcard_PBS++;
    if (loadcard_PBS == 1) {
        html += "<div class='col-14 cardproduct'>\
        <Center>\
            <h2><span>" + key + "</span> </h2>\
            <div class=\"container overflow-hidden text-center\">\
                <div class=\"row gy-3\">\
                    <div class=\"col-5\">\
                        <div class=\"statecard\">Pressure:</div>\
                    </div>\
                    <div class=\"col-6\">\
                        <div class=\"statecard\" id=\"cardvalue" + key + "\"></div>\
                    </div>\
                </div><br>\
                <div class=\"row gy-3\">\
                    <div class=\"col-5\">\
                        <span class=\"statecard\"> Q1 </span><svg class=\"bi\" width=\"24\" height=\"24\" id=\"" + key + "_Q1\"\
                        style = \"color: rgb(255, 0, 0);\" >\
                        <use xlink:href=\"#icon-connect\"></use>\
                        </svg>\
                    </div>\
                    <div class=\"col-5\">\
                        <span class=\"statecard\"> Q2 </span><svg class=\"bi\" width=\"24\" height=\"24\" id=\"" + key + "_Q2\"\
                        style = \"color: rgb(255, 0, 0);\" >\
                        <use xlink:href=\"#icon-connect\"></use>\
                        </svg>\
                    </div>\
                </div>\
            </div>\
        </Center>\
        </div><br></br>";

        document.getElementById("addcard").innerHTML = html;
    }
    if ((value[0]).toFixed(2) == "2130.00")
        document.getElementById("cardvalue" + key).innerHTML = "Out Range";
    else
        document.getElementById("cardvalue" + key).innerHTML = (value[0]).toFixed(3) + "bar";
    document.getElementById(key + "_Q1").style = 'background-color: rgb(0,0,0)';
    document.getElementById(key + "_Q2").style = 'background-color: rgb(0,0,0)';
    if ((value[1]).toFixed(0) == "1")
        document.getElementById(key + "_Q1").style = 'background-color: rgb(0,255,0)';
    if ((value[2]).toFixed(0) == "1")
        document.getElementById(key + "_Q2").style = 'background-color: rgb(0,255,0)';
}
function gencard_WTM10L(message) {
    var key = Object.keys(message);
    var value = message[key].value;
    console.log(value);
    loadcard_WTM10L++;
    if (loadcard_WTM10L == 1) {
        html += "<div class='col-14 cardproduct'>\
        <Center>\
            <h2><span>" + key + "</span></h2>\
            <div class=\"container overflow-hidden text-center\">\
            <div class=\"row gy-3\">\
                <div class=\"col-5\">\
                    <div class=\"statecard\">Distance:</div>\
                </div>\
                <div class=\"col-6\">\
                    <div class=\"statecard\" id=\"cardvalue" + key + "\"></div>\
                </div>\
            </div><br>\
            <div class=\"row gy-3\">\
                <div class=\"col-5\">\
                    <span class=\"statecard\"> Q1 </span><svg class=\"bi\" width=\"24\" height=\"24\" id=\"" + key + "_Q1\"\
                    style = \"color: rgb(255, 0, 0);\" >\
                    <use xlink:href=\"#icon-connect\"></use>\
                    </svg>\
                </div>\
                <div class=\"col-5\">\
                    <span class=\"statecard\"> Q2 </span><svg class=\"bi\" width=\"24\" height=\"24\" id=\"" + key + "_Q2\"\
                    style = \"color: rgb(255, 0, 0);\" >\
                    <use xlink:href=\"#icon-connect\"></use>\
                    </svg>\
                </div>\
            </div>\
        </Center>\
        </div>\
        </div><br></br>";
        document.getElementById("addcard").innerHTML = html;
    }
    if ((value[0]) == "32767")
        document.getElementById("cardvalue" + key).innerHTML = "Out Range";
    else
        document.getElementById("cardvalue" + key).innerHTML = (value[0]) + "mm";
    document.getElementById(key + "_Q1").style = 'background-color: rgb(0,0,0)';
    document.getElementById(key + "_Q2").style = 'background-color: rgb(0,0,0)';
    if ((value[1]).toFixed(0) == "1")
        document.getElementById(key + "_Q1").style = 'background-color: rgb(0,255,0)';
    if ((value[2]).toFixed(0) == "1")
        document.getElementById(key + "_Q2").style = 'background-color: rgb(0,255,0)';
}
function gencard_CSS(message) {
    var key = Object.keys(message);
    var value = message[key].value;
    console.log(value);
    loadcard_CSS++;
    if (loadcard_CSS == 1) {
        html += "<div class='col-14 cardproduct'>\
        <Center>\
           <h2><span>" + key + "</span></h2>\
            <div class=\"container overflow-hidden text-center\">\
            <div class=\"row gy-3\">\
                <div class=\"col-5\">\
                <div class=\"statecard\">Color:</div>\
                </div>\
                <div class=\"col-6\">\
                <svg class=\"bi\" width=\"30\" height=\"30\" id=\"cardColor" + key + "\"\
                style = \"color: rgb(255, 0, 0);\" >\
                <use xlink:href=\"#icon-connect\"></use>\
                </svg>\
                </div>\
                <div class=\"col-5\">\
                <div class=\"statecard\">CMV:</div>\
                </div>\
                <div class=\"col-6\">\
                <div class=\"statecard\" id=\"cardvalue" + key + "\"></div>\
                </div>\
            </div><br>\
            <div class=\"row gy-3\">\
                <div class=\"col-5\">\
                    <span class=\"statecard\"> Q1 </span><svg class=\"bi\" width=\"24\" height=\"24\" id=\"" + key + "_Q1\"\
                    style = \"color: rgb(255, 0, 0);\" >\
                    <use xlink:href=\"#icon-connect\"></use>\
                    </svg>\
                </div>\
                <div class=\"col-5\">\
                    <span class=\"statecard\"> Q2 </span><svg class=\"bi\" width=\"24\" height=\"24\" id=\"" + key + "_Q2\"\
                    style = \"color: rgb(255, 0, 0);\" >\
                    <use xlink:href=\"#icon-connect\"></use>\
                    </svg>\
                </div>\
            </div>\
            </Center>\
        </div>\
        </div><br></br>";
        document.getElementById("addcard").innerHTML = html;
    }
    var R = (value[0]);
    var G = (value[1]);
    var B = (value[2]);
    document.getElementById("cardColor" + key).style = 'background-color: rgb(' + R + ',' + G + ',' + B + ')';
    document.getElementById("cardvalue" + key).innerHTML = (value[3]);
    document.getElementById(key + "_Q1").style = 'background-color: rgb(0,0,0)';
    document.getElementById(key + "_Q2").style = 'background-color: rgb(0,0,0)';
    if ((value[4]).toFixed(0) == "1")
        document.getElementById(key + "_Q1").style = 'background-color: rgb(0,255,0)';
    if ((value[5]).toFixed(0) == "1")
        document.getElementById(key + "_Q2").style = 'background-color: rgb(0,255,0)';
}
function gencard_MPB10(message) {
    var key = Object.keys(message);
    var value = message[key].value;
    console.log(value);
    loadcard_MPB10++;
    if (loadcard_MPB10 == 1) {
        html += "<div class='col-14 cardproduct'>\
                <Center>\
                    <h2><span>" + key + "</span></h2>\
                    <div class=\"container overflow-hidden text-center\">\
                    <div class=\"row gy-3\">\
                        <div class=\"col-5\">\
                        <div class=\"statecard\">X:</div>\
                        </div>\
                        <div class=\"col-6\">\
                        <div class=\"statecard\" id=\"card" + key + "value1\"></div>\
                        </div>\
                        <div class=\"col-5\">\
                        <div class=\"statecard\">Y:</div>\
                        </div>\
                        <div class=\"col-6\">\
                        <div class=\"statecard\" id=\"card" + key + "value2\"></div>\
                        </div>\
                        <div class=\"col-5\">\
                        <div class=\"statecard\">Z:</div>\
                        </div>\
                        <div class=\"col-6\">\
                        <div class=\"statecard\" id=\"card" + key + "value3\"></div>\
                        </div>\
                        <div class=\"col-5\">\
                        <div class=\"statecard\">Temperature:</div>\
                        </div>\
                        <div class=\"col-6\">\
                        <div class=\"statecard\" id=\"card" + key + "value4\"></div>\
                        </div>\
                    </div>\
                    </Center>\
                </div>\
                </div><br></br>";
        document.getElementById("addcard").innerHTML = html;
    }
    document.getElementById("card" + key + "value1").innerHTML = (value[3] * 180 / 3.14).toFixed(2) + "°";
    document.getElementById("card" + key + "value2").innerHTML = (value[2] * 180 / 3.14).toFixed(2) + "°";
    document.getElementById("card" + key + "value3").innerHTML = (value[1] * 180 / 3.14).toFixed(2) + "°";
    document.getElementById("card" + key + "value4").innerHTML = (value[0]).toFixed(2) + "°C";
}