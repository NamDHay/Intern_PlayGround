function SICKMessageHandler(eventdata) {
    var message = JSON.parse(eventdata.data);
    if(message.OD2000.value != null){
        console.log("Received OD2000 distance: ", message.OD2000.value);
    }
    if(message.MPB10.value != null){
        console.log("Received MPB10 value: ", message.OD2000.value);
    }
    if(message.WTM10L.value != null){
        console.log("Received WTM10L value: ", message.WTM10L.value);
    }
}