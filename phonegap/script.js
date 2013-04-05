var SERVER = "http://zpc.dk/api/gtfu/";
var gotResponse = function(responseText){
    /*
    var notification = window.webkitNotifications.createNotification(
        '16x16.png',
        'Server response',
        responseText
        );
    notification.show();
    setTimeout(function(){
        notification.cancel();
    }, 3000);
*/
    console.log(responseText);
};
var sendRequest = function(cmdstate) {
    var xmlHttp = new XMLHttpRequest();
    var name = document.getElementById('name').value;
    var pass = document.getElementById('pass').value;
    var url = SERVER+"?name="+name+"&password="+pass+"&cmdstate="+cmdstate;
    xmlHttp.addEventListener("load", function(){
        gotResponse(xmlHttp.responseText);
        //alert("response: " + xmlHttp.responseText);
    });
    xmlHttp.addEventListener("error", function(){
        alert("error: " + xmlHttp.responseText);
    });
    xmlHttp.addEventListener("abort", function(){
        alert("abort: " + xmlHttp.responseText);
    });
    xmlHttp.open("GET", url, false);
    xmlHttp.send();
};

var sendPowerOn         = function(){sendRequest(1)}
,   sendSoftOff         = function(){sendRequest(2)}
,   sendForceOff        = function(){sendRequest(3)}
,   sendForceRestart    = function(){sendRequest(4)};

// store login info in local, to avoid syncing across
// different installations
var storage = window.localStorage;
document.addEventListener('DOMContentLoaded', function() {
    storage.get('name', function(val){
        if (val.name != undefined)
            document.getElementById('name').value = val.name;
    });
    storage.get('pass', function(val){
        if (val.pass != undefined)
            document.getElementById('pass').value = val.pass;
    });
    document.querySelector('#powerOn').addEventListener(
      'click', sendPowerOn);
    document.querySelector('#softOff').addEventListener(
      'click', sendSoftOff);
    document.querySelector('#forceOff').addEventListener(
      'click', sendForceOff);
    document.querySelector('#forceRestart').addEventListener(
      'click', sendForceRestart);
    document.querySelector('#name').addEventListener(
      'keyup', setName);
    document.querySelector('#name').addEventListener(
      'paste', setName);
    document.querySelector('#pass').addEventListener(
      'keyup', setPass); 
    document.querySelector('#pass').addEventListener(
      'paste', setPass); 
});


var setName = function(){
    storage.setItem({'name': document.getElementById('name').value}, function(){})
};

var setPass = function(){
    storage.setItem({'pass': document.getElementById('pass').value}, function(){})
};