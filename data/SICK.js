function gencard_ODOD2000(valuesensor){
    loadcardOD2000++;
    if(loadcardOD2000 == 1){
        html = "<div class='col-14 cardproduct'>\
        <Center>\
            <h2><span>OD2000</span> </h2>\
            <div class=\"container overflow-hidden text-center\">\
            <div class=\"row gy-3\">\
                <div class=\"col-5\">\
                <div class=\"statecard\">Distance:</div>\
                </div>\
                <div class=\"col-6\">\
                <div class=\"statecard\" id=\"cardvalueOD2000\"></div>\
                </div>\
            </div>\
            </Center>\
        </div>\
        </div><br></br>";

        document.getElementById("addcard").innerHTML = html;
    }
    document.getElementById("cardvalueOD2000").innerHTML = (valuesensor).toFixed(2) +"mm";

}
function gencard_WTM10L(valuesensor){
    loadcard_WTM10L++;
    if(loadcard_WTM10L == 1){
        html += "<div class='col-14 cardproduct'>\
        <Center>\
           <h2><span></span>WTM10L</h2>\
            <div class=\"container overflow-hidden text-center\">\
            <div class=\"row gy-3\">\
                <div class=\"col-5\">\
                <div class=\"statecard\">Distance:</div>\
                </div>\
                <div class=\"col-6\">\
                <div class=\"statecard\" id=\"cardvalueWTM10L\"></div>\
                </div>\
            </div>\
            </Center>\
        </div>\
        </div><br></br>";
        document.getElementById("addcard").innerHTML = html;
    }
    document.getElementById("cardvalueWTM10L").innerHTML = (valuesensor).toFixed(2) +"mm";
}
function gencard_MPB10(valuesensor){
    loadcard_MPB10++;
    if(loadcard_MPB10 == 1){
        html += "<div class='col-14 cardproduct'>\
                <Center>\
                    <h2><span></span>MPB10</h2>\
                    <div class=\"container overflow-hidden text-center\">\
                    <div class=\"row gy-3\">\
                        <div class=\"col-5\">\
                        <div class=\"statecard\">X:</div>\
                        </div>\
                        <div class=\"col-6\">\
                        <div class=\"statecard\" id=\"cardvalue1\"></div>\
                        </div>\
                        <div class=\"col-5\">\
                        <div class=\"statecard\">Y:</div>\
                        </div>\
                        <div class=\"col-6\">\
                        <div class=\"statecard\" id=\"cardvalue2\"></div>\
                        </div>\
                        <div class=\"col-5\">\
                        <div class=\"statecard\">Z:</div>\
                        </div>\
                        <div class=\"col-6\">\
                        <div class=\"statecard\" id=\"cardvalue3\"></div>\
                        </div>\
                        <div class=\"col-5\">\
                        <div class=\"statecard\">Temperature:</div>\
                        </div>\
                        <div class=\"col-6\">\
                        <div class=\"statecard\" id=\"cardvalue4\"></div>\
                        </div>\
                    </div>\
                    </Center>\
                </div>\
                </div><br></br>";
        document.getElementById("addcard").innerHTML = html;      
    }
    document.getElementById("cardvalue1").innerHTML = (valuesensor[3]*180/3.14).toFixed(2) +"°";
    document.getElementById("cardvalue2").innerHTML = (valuesensor[2]*180/3.14).toFixed(2) +"°";
    document.getElementById("cardvalue3").innerHTML = (valuesensor[1]*180/3.14).toFixed(2) +"°";
    document.getElementById("cardvalue4").innerHTML = (valuesensor[0]).toFixed(2) +"°C";
}