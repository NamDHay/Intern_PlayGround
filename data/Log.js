var staffPass = "abc", adminPass = "123", AutoStopPass = "";

function closeModalUnlock() {
    var modalElement = document.getElementById('passwordModal');
    var modal = bootstrap.Modal.getInstance(modalElement);
    modal.hide();
  }
  function UnlockStaff() {
    unlockValue = 1;
    document.getElementById("tennutluu").innerHTML = "Save new staff passwords";
    document.getElementById("roleAccount").innerHTML = "Staff";
    var ids = ['dashboard', 'settingA', 'features', 'board', 'openbuttonchange', 'name_product', 'wifimode', 'btnModbus', 'btntable', 'btnIo','buttonadd','btnsetting'];
    ids.forEach(function (id) {
      var element = document.getElementById(id);
      if (element) {
        if (unlockValue == 1) {
          element.style.display = 'block';
        }
      }
    });
  }
  
  function UnlockAdmin() {
    unlockValue = 2;
    document.getElementById("tennutluu").innerHTML = "Save new admin passwords";
    document.getElementById("roleAccount").innerHTML = "Admin";
    var ids = ['dashboard', 'settingA', 'features', 'board', 'application', 'console', 'advanced', 'wifi', 'rf', 'ethernet', 'modbus', 'zicon', 'openbuttonchange', 'name_product', 'wifimode', 'settingBtnKL', 'settingBtnEX', 'btnModbus', 'btntable', 'btnIo','buttonadd','btnsetting'];
    ids.forEach(function (id) {
      var element = document.getElementById(id);
      if (element) {
        if (unlockValue == 2) {
          element.style.display = 'block';
        }
      }
    });
  }
  function LogOut() {
    unlockValue = 0;
    document.getElementById("roleAccount").innerHTML = "User";
    var ids = ['dashboard', 'settingA', 'features', 'board', 'application', 'console', 'advanced', 'wifi', 'rf', 'ethernet', 'modbus', 'zicon', 'openbuttonchange', 'changePassA', 'name_product', 'wifimode', 'settingBtnKL', 'settingBtnEX', 'btnModbus', 'btntable', 'btnIo','buttonadd','btnsetting'];
    ids.forEach(function (id) {
      var element = document.getElementById(id);
      if (element) {
        element.style.display = 'none';
      }
    });
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

    changeTab("dashboard");
  }
  function updateUI(height) {
    // $("body").width($(window).width()-4)
    if (height > 0) $('#vplab_divider').height(height)
    $("#tabs").height($(window).height())
    $("#feed_url").height($("#Stationary_chart").height())
    $("#feed_url").height($("#Moving_chart").height())
    $("#tab_console").height($(window).height())
}
function changeTab(id) {
    if (!id) return
    $(".a_tab").addClass('hidden_forced')
    $(".nav-link").removeClass('active')
    $("#" + id).addClass('nav-link py-3 active')
    $("#tab_" + id).removeClass('hidden_forced')
    updateUI($("#tab_" + id).height());
    // if(id == 'application')loadData()//hardcode

    // if (id == 'wifi') build_HTML_lookline_list('wifi', 'wifi_list_data')
    // if (id == 'ethernet') build_HTML_lookline_list('tcp', 'ethernet_list_data')
    // if (id == 'modbus') build_HTML_lookline_list('modbus', 'modbus_list_data')
    // if (id == 'application') build_HTML_lookline_list('application', 'application_list_data')
    // if (id == 'rf') build_HTML_lookline_list('rf', 'rf_list_data')
    // application_list_data
}
  function PassSave(id) {
    // try{
    console.log("pass_Save: " + document.getElementById("pass_Save").value);
    if (id == 1) {
      staffPass = document.getElementById("pass_Save").value; preferenceslist[0].Pass_staff = document.getElementById("pass_Save").value;
    }
    if (id == 2) {
      adminPass = document.getElementById("pass_Save").value;
      preferenceslist[0].Pass_admin = document.getElementById("pass_Save").value;
    }
  
    console.log("staffPass: " + staffPass);
    console.log("adminPass: " + adminPass);
    var inputElement = document.getElementById("pass_Save");
    // Đặt giá trị của input thành rỗng
    inputElement.value = "";
  
  }
  function PassSave_AutoStop() {
    console.log("PassSave_AutoStop save");
    AutoStopPass = document.getElementById("pass_Save_AutoStop").value;
    preferenceslist[0].AutoStop = document.getElementById("pass_Save_AutoStop").value;
  
  
    console.log("AutoStopPass: " + AutoStopPass);
    var inputElement = document.getElementById("pass_Save_AutoStop");
    // Đặt giá trị của input thành rỗng
    inputElement.value = "";
    // }
    // catch(e) {console.log(e);}
  }
  function PassGet_AutoStop() {
    console.log("PassGet_AutoStop get");
    try {
      void 0 == AutoStopPass ? AutoStopPass = preferenceslist[0].AutoStop : AutoStopPass = preferenceslist[0].AutoStop,
        document.getElementById("AutoStopShow").innerHTML = AutoStopPass;
  
      console.log("AutoStopPass: " + AutoStopPass);
    }
    catch (e) {
      console.log(e);
    }
    document.getElementById("dataProduct").value = preferenceslist[0].product;
  
  }
  function PassGet() {
    console.log("PassGet");
    try {
      void 0 == staffPass ? staffPass = preferenceslist[0].Pass_staff : staffPass = preferenceslist[0].Pass_staff,
        document.getElementById("staffShow").innerHTML = staffPass;
  
      void 0 == adminPass ? adminPass = preferenceslist[0].Pass_admin : adminPass = preferenceslist[0].Pass_admin,
        document.getElementById("adminShow").innerHTML = adminPass;
  
      console.log("Get staffPass: " + staffPass);
      console.log("Get adminPass: " + adminPass);
    }
    catch (e) {
      console.log(e);
    }
    document.getElementById("dataProduct").value = preferenceslist[0].product;
  
  }
  function validateAndSave() {
    var pass1 = document.getElementById("pass_Save").value;
    var pass2 = document.getElementById("pass_Save_confirm").value;
  
    if (pass1 === pass2) {
      PassSave(unlockValue);
      closeModalChange();
      document.getElementById("pass_Save").value = "";
      document.getElementById("pass_Save_confirm").value = "";
      ChangeSuccess();
    } else {
      alert("Passwords do not match. Please try again.");
      document.getElementById("pass_Save").focus();
      document.getElementById("pass_Save").value = "";
      document.getElementById("pass_Save_confirm").value = "";
  
    }
  }
  function checkPassword() {
    var password = document.getElementById('passwordInput').value;
    //Admin
    if (password == adminPass) {
      LogOut();
      closeModalUnlock();
      UnlockAdmin();
      document.getElementById("passwordInput").value = "";
      unlockAdminSuccess();
    }
    //Staff
    else if (password == staffPass) {
      LogOut();
      closeModalUnlock();
      UnlockStaff();
      document.getElementById("passwordInput").value = "";
      unlockStaffSuccess();
    }
    else {
      LogOut();
      unlockFail();
      document.getElementById("passwordInput").value = "";
    }
  }
  document.getElementById("passwordInput").addEventListener("keyup", function (event) {
    if (event.key == "Enter") {
      event.preventDefault();
      checkPassword();
    }
  });
  document.getElementById("pass_Save").addEventListener("keyup", function (event) {
    if (event.key == "Enter") {
      event.preventDefault();
      document.getElementById("pass_Save_confirm").focus();
    }
  });
  
  document.getElementById("pass_Save_confirm").addEventListener("keyup", function (event) {
    if (event.key == "Enter") {
      event.preventDefault();
      validateAndSave();
    }
  });
  function closeModalChange() {
    var modalElement = document.getElementById('passwordModalChange');
    var modal = bootstrap.Modal.getInstance(modalElement);
    modal.hide();
  }
  function unlockFail() {
    var modal = new bootstrap.Modal(document.getElementById('wrongPasswordModal'));
    modal.show();
    setTimeout(function () { modal.hide(); }, 1000);
  }
  
  function unlockAdminSuccess() {
    var modal = new bootstrap.Modal(document.getElementById('successModal'));
    modal.show();
    document.getElementById('successModaltext').innerHTML = "Login Admin Successful!";
    setTimeout(function () { modal.hide(); }, 1000);
  }
  
  function unlockStaffSuccess() {
    var modal = new bootstrap.Modal(document.getElementById('successModal'));
    modal.show();
    document.getElementById('successModaltext').innerHTML = "Login Staff Successful!";
    setTimeout(function () { modal.hide(); }, 1000);
  }
  function ChangeSuccess() {
    var modal = new bootstrap.Modal(document.getElementById('successModalChange'));
    modal.show();
    setTimeout(function () { modal.hide(); }, 1000);
  }
  function showlogin() {
    var modal = new bootstrap.Modal(document.getElementById('passwordModal'));
    document.getElementById('passwordModal').addEventListener('shown.bs.modal', function () {
      document.getElementById("passwordInput").focus();
    });
    document.getElementById("passwordInput").value = "";
    modal.show();
  }