var modalCardSet = new Object;
modalCardSet.active = null
modalCardSet.closefn = null;
var stateID = [];//trạng thái của Run/Stop (2/1)
var lock = [];//trạng thái của Run/Stop (0/1)

function ShowModalCardSet(closefunc) {
    modalCardSet.active.style.display = "block";
    modalCardSet.closefn = closefunc;
  }
  function CloseModalCardSet(response) {
    modalCardSet.active.style.display = 'none';
    modalCardSet.closefn(response);
  }
  
  function CardSetClose(value) {
    console.log(value); if (value == "Ok") {
      if (modalCardSet.active.getElementsByClassName("modal-title") == "Edit Application") {
        console.log("Edit App");
      } else {
        // saveSettings(cardID);
      }
    }
  }
  function CardSetup_dlg(titledlg, textdlg, closefunc) {
    modalCardSet.active = document.getElementById('settingsModal.html');
    var title = modalCardSet.active.getElementsByClassName("modal-title")[0];
    var body = modalCardSet.active.getElementsByClassName("modal-text")[0];
    title.innerHTML = titledlg;
    body.innerHTML = textdlg;
    cardID = textdlg;
    if (typeof closefunc === 'undefined') closefunc = CardSetClose;
    ShowModalCardSet(closefunc);
    loaddatasetting(textdlg);
  }
  function toggleDeleteProductList() {
    var option_product = document.getElementById("option_product");
    var blockInput = document.getElementById("blockInput");
    var deleteProductList = document.getElementById("deleteProductList");
  
    if (deleteProductList.style.display === "none") {
      deleteProductList.style.display = "block";
      option_product.style.display = "none";
      blockInput.style.display = "none";
      document.getElementById("btntoggleDeleteProductList").innerHTML = "Hide";
      displayDeleteProductList();
    } else {
      deleteProductList.style.display = "none";
      option_product.style.display = "block";
      blockInput.style.display = "block";
      document.getElementById("btntoggleDeleteProductList").innerHTML = "Show";
    }
  }
  
  function displayDeleteProductList() {
    var editForm = document.getElementById("editForm");
    editForm.style.display = "none";
  
    var productTableBody = document.getElementById("productTableBody");
    productTableBody.innerHTML = ""; // Xóa nội dung bảng sản phẩm cũ
  
    // Lấy danh sách sản phẩm từ chuỗi JSON
    var productJSON = JSON.parse(document.getElementById("dataProduct").value);
  
    // Lấy id của card
    var id = document.getElementById("cardID").innerHTML;
  
    // Hiển thị danh sách sản phẩm của card
    productJSON.Data[id].product.forEach(function (product, index) {
      var row = document.createElement("tr");
  
      // Tạo cột hiển thị tên sản phẩm
      var productCell = document.createElement("td");
      productCell.textContent = product;
  
      // Tạo cột chứa nút Edit
      var editButtonCell = document.createElement("td");
      var editButton = document.createElement("button");
      editButton.textContent = "Edit";
      editButton.className = "btn btn-warning";
      editButton.type = "button";
      editButton.onclick = function () {
      editProduct(id, index); // Truyền id của card và index của sản phẩm vào hàm chỉnh sửa sản phẩm
      };
      editButtonCell.appendChild(editButton);
  
      // Tạo cột chứa nút Xóa
      var deleteButtonCell = document.createElement("td");
      var deleteButton = document.createElement("button");
      deleteButton.textContent = "Delete";
      deleteButton.className = "btn btn-secondary";
      deleteButton.type = "button";
      deleteButton.onclick = function () {
        deleteProduct(id, index); // Truyền id của card và index của sản phẩm vào hàm xóa sản phẩm
      };
      deleteButtonCell.appendChild(deleteButton);
  
      // Thêm cột vào hàng
      row.appendChild(productCell);
      row.appendChild(editButtonCell);
      row.appendChild(deleteButtonCell);
  
      // Thêm hàng vào bảng
      productTableBody.appendChild(row);
    });
  
    SaveDataProduct();
  }
  
  function editProduct(cardID, index) {
    var productJSON = JSON.parse(document.getElementById("dataProduct").value);
    var product = productJSON.Data[cardID].product[index];
    var cycleTime = productJSON.Data[cardID].cycletime[index];
    var productSet = productJSON.Data[cardID].productset[index];
  
    // Ẩn bảng hiển thị sản phẩm
    document.getElementById("productTable").style.display = "none";
  
    // Hiển thị form chỉnh sửa sản phẩm
    var editForm = document.getElementById("editForm");
    editForm.style.display = "block";
  
  
    // Điền thông tin sản phẩm vào form chỉnh sửa
    document.getElementById("editProductName").value = product;
    document.getElementById("editCycleTime").value = cycleTime;
    document.getElementById("editProductSet").value = productSet;
  
    editForm.dataset.index = index;
  
  }
  function closeEditForm() {
    // Ẩn form chỉnh sửa sản phẩm mà không lưu thay đổi
    var editForm = document.getElementById("editForm");
    editForm.style.display = "none";
    document.getElementById("productTable").style.display = "table";
  }
  function saveProduct() {
    var id = document.getElementById("cardID").innerHTML;
    var productJSON = JSON.parse(document.getElementById("dataProduct").value);
  
    // Lấy thông tin sản phẩm từ form chỉnh sửa
    var editedProductName = document.getElementById("editProductName").value;
    var editedCycleTime = document.getElementById("editCycleTime").value;
    var editedProductSet = document.getElementById("editProductSet").value;
  
    // Lấy index từ thuộc tính data-index của form chỉnh sửa
    var index = parseInt(document.getElementById("editForm").dataset.index);
  
    // Cập nhật thông tin sản phẩm vào đối tượng JSON
    productJSON.Data[id].product[index] = editedProductName;
    productJSON.Data[id].cycletime[index] = editedCycleTime;
    productJSON.Data[id].productset[index] = editedProductSet;
  
    // Chuyển đối tượng JSON thành chuỗi JSON và lưu vào phần tử HTML
    document.getElementById("dataProduct").value = JSON.stringify(productJSON);
  
    // Hiển thị lại bảng sản phẩm
    document.getElementById("productTable").style.display = "table";
  
    // Ẩn form chỉnh sửa sản phẩm
    var editForm = document.getElementById("editForm");
    editForm.style.display = "none";
  
    document.getElementById("dataProduct").value = JSON.stringify(productJSON);
    // In ra chuỗi JSON đã được cập nhật
    console.log("Updated JSON:", JSON.stringify(productJSON));
  
    SaveDataProduct();
    loaddatasetting(id);
    displayDeleteProductList();
  }
  
  // Hàm xóa sản phẩm từ danh sách sản phẩm của card dựa trên id và index
  function deleteProduct(cardID, index) {
    var id = document.getElementById("cardID").innerHTML;
    var productJSON = JSON.parse(document.getElementById("dataProduct").value);
  
    // Xóa sản phẩm khỏi danh sách của card
    productJSON.Data[cardID].product.splice(index, 1);
    productJSON.Data[cardID].cycletime.splice(index, 1);
    productJSON.Data[cardID].productset.splice(index, 1);
  
    // Cập nhật lại chuỗi JSON chứa danh sách sản phẩm
    document.getElementById("dataProduct").value = JSON.stringify(productJSON);
  
    // Hiển thị lại danh sách sản phẩm để xóa
    loaddatasetting(id);
    displayDeleteProductList();
  }
  
  function loaddatasetting(id) {
    console.log("loaddatasetting" + id);
    var UpdateDataApp = JSON.parse(jsonAppInput);
    var jsontableid = JSON.parse(jsontableID);
    var arrayData = [AppID, poss, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6, value7] = UpdateDataApp.Data[id].app.split(",");
    document.getElementById("doiten").value = arrayData[5];
  
    var chooseSlave = document.getElementById("selectreg").value;
    if(chooseSlave == "") chooseSlave = UpdateDataApp.Data[id].app.split(",")[0];
    var jsontabledata = JSON.parse(jsontableData);
    var re = document.getElementById('re' + chooseSlave).innerText;
    var rs = document.getElementById('rs' + chooseSlave).innerText;
    var length = re - rs + 1;
    for(var j = 0; j < length; j++){
      if (value1 == Number(rs) + j) {
        document.getElementById("planInput").value = document.getElementById("value" + chooseSlave + "_" + j).innerText;
      }
      if (value2 == Number(rs) + j) {
        document.getElementById("resultInput").value = document.getElementById("value" + chooseSlave + "_" + j).innerText;
      }
      if (value3 == Number(rs) + j) {
        document.getElementById("planSetInput").value = document.getElementById("value" + chooseSlave + "_" + j).innerText;
      }
      if (value4 == Number(rs) + j) {
        document.getElementById("ProductDataSaveSelect").value = document.getElementById("value" + chooseSlave + "_" + j).innerText;
      }
      if (value5 == Number(rs) + j) {
        document.getElementById("TimeIncInput").value = document.getElementById("value" + chooseSlave + "_" + j).innerText;
      }
      if (value6 == Number(rs) + j) {
        document.getElementById("PCSInput").value = document.getElementById("value" + chooseSlave + "_" + j).innerText;
      }
    }
  
    var productData;
    var productParse = document.getElementById("dataProduct").value;
    productJSON = JSON.parse(productParse);
  
    if (productJSON.Data[id] != null) {
      productData = productJSON.Data[id].product;
  
      // Đặt giá trị vào select
      var selectElement = document.getElementById("ProductDataSaveSelect");
      selectElement.innerHTML = ''; // Xóa tất cả các option cũ
  
      var optionID = 0;
      productData.forEach(function (product) {
        var option = document.createElement("option");
        option.text = product;
        option.value = optionID;
        optionID = optionID + 1;
        selectElement.appendChild(option);
      });
  
      // Chọn giá trị phù hợp nếu có
      var productNameElement = document.getElementById("productName_" + id);
      if (productNameElement) {
        var productName = productNameElement.innerHTML;
        var selectedIndex = Array.from(selectElement.options).findIndex(option => option.text === productName);
        if (selectedIndex !== -1) {
          selectElement.selectedIndex = selectedIndex;
        }
      }
      var productID = document.getElementById("ProductDataSaveSelect").value;
      console.log("Select id:" + productID);
      var cardid = document.getElementById("cardID").innerHTML;
      console.log("card ID:" + cardid);
      var CyleTime = productJSON.Data[cardid].cycletime[productID];
      document.getElementById("TimeIncInput").value = CyleTime;
  
      var ProductSet = productJSON.Data[cardid].productset[productID];
      document.getElementById("planSetInput").value = ProductSet;
    }
    else {
      document.getElementById("ProductDataSaveSelect").innerHTML = '';
    }
    lock[id] = 1;
    stateID[id] = 1;
  }
  //add product
  function addProduct() {
    var productName = document.getElementById("newProductName").value.trim();
    if (productName === "") {
      alert("Tên sản phẩm không được để trống!");
      return;
    }
    var newProductName = document.getElementById("newProductName").value;
    var newCycleTime = document.getElementById("TimeIncInput").value;
    var newProductSet = document.getElementById("planSetInput").value;
    var selectElement = document.getElementById("ProductDataSaveSelect");
    var id = document.getElementById("cardID").innerHTML;
    var UpdateDataApp = JSON.parse(jsonApp);
    var arrayData = [AppID, poss, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6,value7] = UpdateDataApp.Data[id].app.split(",");
    productJSON = JSON.parse(document.getElementById("dataProduct").value);
    var idEnd = productJSON.Data.length;
    if (productJSON.Data[id] == null) {
      console.log("tao moi");
      var dataproduct = [];
      var datacycletime = [];
      var dataproductset = [];
      dataproduct.push(newProductName);
      datacycletime.push(newCycleTime);
      dataproductset.push(newProductSet);
      productJSON.Data.push({ id: idEnd, product: dataproduct, cycletime: datacycletime, productset: dataproductset });
  
      dataProduct = productJSON;
      console.log("newData: " + JSON.stringify(dataProduct));
      document.getElementById("dataProduct").value = JSON.stringify(dataProduct);
      var option = document.createElement("option");
      option.text = newProductName;
      option.value = 0;
      selectElement.appendChild(option);
      document.getElementById("newProductName").value = "";
    }
    else {
      // Lấy giá trị mới từ các phần tử HTML
      var newProductName = document.getElementById("newProductName").value;
      var newCycleTime = document.getElementById("TimeIncInput").value;
      var newProductSet = document.getElementById("planSetInput").value;
      // In ra các giá trị mới
      console.log("newProductName:", newProductName);
      console.log("newCycleTime:", newCycleTime);
      console.log("newProductSet:", newProductSet);
      // Lấy danh sách sản phẩm từ chuỗi JSON
      var productJSON = JSON.parse(document.getElementById("dataProduct").value);
      // Lấy số lượng sản phẩm hiện có
      var newID = productJSON.Data[id].product.length;
      // Thêm tên sản phẩm mới vào mảng product
      productJSON.Data[id].product.push(newProductName);
      // Thêm giá trị mới của cycletime và productset cho sản phẩm mới
      productJSON.Data[id].cycletime.push(newCycleTime);
      productJSON.Data[id].productset.push(newProductSet);
      // Cập nhật lại chuỗi JSON chứa danh sách sản phẩm
      document.getElementById("dataProduct").value = JSON.stringify(productJSON);
      // In ra chuỗi JSON đã được cập nhật
      console.log("Updated JSON:", JSON.stringify(productJSON));
      // Tạo một option mới để thêm vào dropdown list
      var selectElement = document.getElementById("ProductDataSaveSelect");
      var option = document.createElement("option");
      option.text = newProductName;
      option.value = newID;
      selectElement.appendChild(option);
      // Xóa giá trị của phần tử HTML sau khi đã thêm sản phẩm mới thành công
      document.getElementById("newProductName").value = "";
    }
    SaveDataProduct();
    loaddatasetting(id);
  }
  
  function SaveDataProduct() {
    SaveJson(document.getElementById('dataProduct').value,"dataProduct");
  }
  function displaySelectedProduct() {
  
    var id = document.getElementById("cardID").innerHTML;
    var UpdateDataApp = JSON.parse(jsonApp);
    var arrayData = [AppID, poss, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6,value7] = UpdateDataApp.Data[id].app.split(",");
  
    var productID = document.getElementById("ProductDataSaveSelect").value;
    console.log("Select id:" + productID);
    var cardid = document.getElementById("cardID").innerHTML;
    console.log("card ID:" + cardid);
    var CyleTime = productJSON.Data[cardid].cycletime[productID];
    document.getElementById("TimeIncInput").value = CyleTime;
  
    var ProductSet = productJSON.Data[cardid].productset[productID];
    document.getElementById("planSetInput").value = ProductSet;
  
    var sp = productJSON.Data[cardid].product[productID];
    // Hiển thị giá trị vừa chọn ra thẻ h3 có cùng ID với cardID
    console.log("id:" + id);
    console.log("ProductSet:" + ProductSet);
    console.log("CyleTime:" + CyleTime);
    if (sp.length % 2 == 0) {
      console.log("độ dài chẵn");
    }
    else {
      console.log("độ dài lẻ");
      sp = sp + " ";
    }
    
    saveSettings(cardID,document.getElementById("planSetInput").value,3);
    setTimeout(function () { saveSettings(cardID,char_string_to_word(productJSON.Data[cardid].product[productID]) ,4); }, 2000);
    setTimeout(function () { saveSettings(cardID,document.getElementById("TimeIncInput").value,5); }, 4000);
    updatevalue();
  }
  // change name card  
  function changeName() {
    var id = document.getElementById("cardID").innerHTML;
    console.log("id:" + id);
    var UpdateDataApp = JSON.parse(jsonApp);
    var arrayData = [AppID, poss, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6,value7] = UpdateDataApp.Data[id].app.split(",");
    var newName = document.getElementById("doiten").value;
    console.log("new name:" + newName);
  
    namecard = namecard.replace(namecard, newName);
    document.getElementById("namecard" + id).innerHTML = namecard;
  
    for (var i = 0; i < (app); i++) {
      if (i == id) {
        AppID = arrayData[0];
        poss = arrayData[1];
        nodeID = arrayData[3];
        netID = arrayData[4];
        selectvalue1 = arrayData[6];
        selectvalue2 = arrayData[7];
        selectvalue3 = arrayData[8];
        selectvalue4 = arrayData[9];
        selectvalue5 = arrayData[10];
        selectvalue6 = arrayData[11];
        selectvalue7 = arrayData[12];
        appData[i] = AppID + "," + poss + "," + i + "," + nodeID + "," + netID + "," + namecard + "," + selectvalue1 + "," + selectvalue2 + "," + selectvalue3 + "," + selectvalue4 + "," + selectvalue5 + "," + selectvalue6 + "," + selectvalue7;
        jsonApp = "{\"Command\":\"App\",\"Data\":[{\"app\":\"" + appData[0] + "\"}";
        if (app > 0) {
          for (var k = 1; k < (app); k++) {
            jsonApp += ",{\"app\":\"" + appData[k] + "\"}";
          }
        }
        jsonApp += "]}";
      }
    }
    console.log(jsonApp);
    document.getElementById("jsonApp").value = jsonApp;
    websocket.send(jsonApp);
  }
  // change state
  function changeState(id) {
    console.log("state id = " + stateID[id]);
    var UpdateDataApp = JSON.parse(jsonApp);
    var arrayData = [AppID, poss, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6, value7] = UpdateDataApp.Data[id].app.split(",");
    
    if (lock[id] == 0) { stateID[id] = 2; }
    if (lock[id] == 1) { stateID[id] = 1; }
    if(stateID[id]== 0){
      stateID[id] = 1;
      ocument.getElementById('buttonrun'+id).innerHTML = "Null";
      document.getElementById('State0_'+ id).innerHTML = "state:Null";
    }
    if(stateID[id]== 1 && lock[id] == 1){
      saveSettings(id, 0, 7);
      // stateID[id] = 2;
      document.getElementById('buttonrun'+id).innerHTML = "Run";
      document.getElementById('State0_'+ id).innerHTML = "state:Stop";
    }
    if(stateID[id] == 2 && lock[id] == 0){
      saveSettings(id, 1, 7);
      // stateID[id] = 1; 
      document.getElementById('State0_'+ id).innerHTML = "state:Run";
      document.getElementById('buttonrun'+id).innerHTML = "Stop";   
    }
    lock[id] = !lock[id];
    console.log("lock[id] = " + lock[id]);
  
  }
  // change reset
  function changeReset(id) {
    console.log("reset id= " + id);
    var UpdateDataApp = JSON.parse(jsonApp);
    var arrayData = [AppID, poss, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6, value7] = UpdateDataApp.Data[id].app.split(",");
    saveSettings(id,0,1);
    setTimeout(function () { saveSettings(id,0,2); }, 500);
  }
  //bỏ nút save, nhập vào thì lưu
  // Gán sự kiện onblur cho từng trường input trong modal
  document.getElementById("planInput").onblur = function () {
    console.log("planInput: " + document.getElementById("planInput").value);
  
    saveSettings(cardID,document.getElementById("planInput").value,1);
  };
  document.getElementById("resultInput").onblur = function () {
    console.log("resultInput: " + document.getElementById("resultInput").value);
    saveSettings(cardID,document.getElementById("resultInput").value,2);
  };
  document.getElementById("planSetInput").onblur = function () {
    console.log("planSetInput: " + document.getElementById("planSetInput").value);
    saveSettings(cardID,document.getElementById("planSetInput").value,3);
  };
  document.getElementById("ProductDataSaveSelect").onblur = function() {
    console.log("ProductDataSaveSelect: "+document.getElementById("ProductDataSaveSelect").value);
    // saveSettings(cardID,document.getElementById("ProductDataSaveSelect").value,4);
  };
  document.getElementById("TimeIncInput").onblur = function () {
    console.log("TimeIncInput: " + document.getElementById("TimeIncInput").value);
    saveSettings(cardID,document.getElementById("TimeIncInput").value,5);
  };
  document.getElementById("PCSInput").onblur = function () {
    console.log("PCSInput: " + document.getElementById("PCSInput").value);
    saveSettings(cardID,document.getElementById("PCSInput").value,6);
  };
    
  function saveSettings(id ,value, Address) {
    var jsonObjRegs = JSON.parse(jsonAppInput);
    var vals = [AppID, poss, id_card, nodeID, netID, namecard, value1, value2, value3, value4, value5, value6, value7] = jsonObjRegs.Data[id].app.split(',');
    var RegsAddress = vals[Address+5];
    var jsonObj = JSON.parse(jsontableData); 
    var slaveID = jsonObj.Data[vals[0]].ID;
    var rs = document.getElementById('rs' + vals[0]).innerText;
    type = document.getElementById("Type" + vals[0] + "_" + (RegsAddress - rs)).value;
    
    if (type == "0") length = 1;
    if (type == "1") length = 1;
    if (type == "2") length = 2;
    if (type == "3") length = 2;
    if (type == "4") length = 20;

    var json_send = "{\"Command\":\"editModbusData\",\"slaveID\":\"" + slaveID + "\",\"address\":\"" + RegsAddress + "\",\"length\":\"" + length + "\",\"value\":[" + value + "]}";
    console.log(json_send);
  
    websocket.send(json_send);
    json_send = "";
  }
  function SaveJson(jsonValue, name){
    var json = JSON.parse(jsonValue);
    json.Command = "SaveFile";
    var newJsonObj = {
      Command: json.Command,
      Filename: name,
      ...json
    };
    var jsonSave = JSON.stringify(newJsonObj);
    console.log(jsonSave);
    websocket.send(jsonSave);
  }
  function LoadJson(name){
    var newJsonObj = {
      Command: "LoadFile",
      Filename: name
    };
    var jsonSave = JSON.stringify(newJsonObj);
    console.log(jsonSave);
    websocket.send(jsonSave);
  }  