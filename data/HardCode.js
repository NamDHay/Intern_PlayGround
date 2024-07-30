function LoadData(){
    jsonSlave = document.getElementById("datatabledata").value = "{\"Command\":\"LoadFile\",\"Filename\":\"mbSlave\",\"Data\":{\"numSlave\":3,\"Slave\":[{\"id\":\"192.168.1.100\",\"rs\":6096,\"re\":6119,\"ws\":6096,\"we\":6119},{\"id\":\"1\",\"rs\":6120,\"re\":6129,\"ws\":6120,\"we\":6129},{\"id\":\"192.168.1.100\",\"rs\":6139,\"re\":6158,\"ws\":6139,\"we\":6158}]}}";
    loadBoardSlave(jsonSlave);
    
    document.getElementById("datatableid").value = jsontableID = "{\"Command\":\"LoadFile\",\"Filename\":\"TableID\",\"Data\":[{\"ID\":\"192.168.1.100\",\"Data\":[[6096,0],[6097,0],[6098,0],[6099,0],[6100,0],[6101,0],[6102,0],[6103,0],[6104,0],[6105,0],[6106,0],[6107,0],[6108,0],[6109,0],[6110,0],[6111,0],[6112,0],[6113,0],[6114,0],[6115,0],[6116,0],[6117,0],[6118,0],[6119,0]]},{\"ID\":\"1\",\"Data\":[[6120,0],[6121,0],[6122,0],[6123,0],[6124,0],[6125,0],[6126,0],[6127,0],[6128,0],[6129,0]]},{\"ID\":\"192.168.1.100\",\"Data\":[[6139,0],[6140,0],[6141,0],[6142,0],[6143,0],[6144,0],[6145,0],[6146,0],[6147,0],[6148,0],[6149,0],[6150,0],[6151,0],[6152,0],[6153,0],[6154,0],[6155,0],[6156,0],[6157,0],[6158,0]]}]}";
    try{

        firstload = 1;
        loading = 0;
        loadTable(jsontableID);
        loadcard = 1;
        changeRegOptions(jsontableID);
        
        if (loading == 1) {
            loaddata(jsontableData);
            // addvaluecard(jsontableData);
        }
    }catch (e) {console.log("Error loading");}
    // if(daloadvcard) updatevalue();
    // if(loadcard == 1) {buildCardJson();loadcard = 0; }
    jsonAppInput = document.getElementById("jsonApp").value = "{\"Command\":\"LoadFile\",\"Filename\":\"Application\",\"Data\":[{\"app\":\"1,0,0,1,0,test2,6123,6123,6122,6128,6127,6120\"},{\"app\":\"1,0,1,1,0,test3,6123,6125,6127,6127,6126,6126\"}]}";
    
    
    
    document.getElementById("dataProduct").value = preferenceslist = "{\"Command\":\"LoadFile\",\"Filename\":\"DataProduct\",\"Data\":[{\"id\":0,\"product\":[\"test1\",\"test2\",\"test3\"],\"cycletime\":[\"800\",\"500\",\"300\"],\"productset\":[\"15\",\"20\",\"3\"]},{\"id\":1,\"product\":[\"sp1\",\"sp2\"],\"cycletime\":[\"100\",\"500\"],\"productset\":[\"10\",\"5\"]}]}";

}


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

//   var JSonObj = JSON.parse(io_obj);
//   for (var i = 0; i < io_array.length; i++) {
//     JSonObj.Data[i] = io_array[i];
//   }
//   var StringJson = JSON.stringify(JSonObj);
//   StringJson = StringJson.replaceAll("false", "0");
//   StringJson = StringJson.replaceAll("true", "1");
//   console.log(StringJson);
//   websocket.send(StringJson);
//   StringJson = "";
// }
// document.getElementById("datatableid").value = jsontableID = "{\"Command\":\"TableID\",\"Data\":[{\"ID\":\"192.168.1.100\",\"Data\":[[6096,0],[6097,0],[6098,0],[6099,0],[6100,0],[6101,0],[6102,0],[6103,0],[6104,0],[6105,0],[6106,0],[6107,0],[6108,0],[6109,0],[6110,0],[6111,0],[6112,0],[6113,0],[6114,0],[6115,0],[6116,0],[6117,0],[6118,0],[6119,0]]},{\"ID\":\"1\",\"Data\":[[6120,0],[6121,0],[6122,0],[6123,0],[6124,0],[6125,0],[6126,0],[6127,0],[6128,0],[6129,0]]}]}";
// loadTable(jsontableID);
// firstload = 1;
// numSlave = 2;
// jsontableData = "{\"Command\":\"tableData\",\"Data\":[{\"ID\":\"1\",\"connect\":1,\"Data\":[9999, 1, 0, 1, 10, 10, 1, 9999, 1, 0, 0, 10, 0, 1, 0, 5, 0, 0, 300, 999, 9999, 2, 123, 1, 0, 0, 15, 0, 800, 0, 250, 300, 15, 0, 10000, 5, 0, 0, 10, 0, 999, 15, 0, 28787, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28672, 3584, 61440, 64768, 32768, 64512, 39680, 16384, 512, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28787, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]}]}";
// loaddata(jsontableData);
// console.log("jsontableData: " + jsontableData);
}