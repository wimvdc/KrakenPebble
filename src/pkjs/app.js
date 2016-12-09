var API_ENDPOINT = "";

Pebble.addEventListener('ready', function(e) {
  Pebble.sendAppMessage({'APP_READY': true});
});

Pebble.addEventListener('appmessage', function(dict) {
    //getKrakenData(dict.payload['LOW']);
		getKrakenData();
  
});

function getKrakenData() {
  xhrWrapper(API_ENDPOINT, 'get', null, function(req) {
    if(req.status == 200) {
      sendResultToPebble(JSON.parse(req.response));
    }
  });

}

function sendResultToPebble(json) {	
    Pebble.sendAppMessage({
      'BALANCE_EUR': parseFloat(json.ZEUR).toFixed(4),
			'BALANCE_ETH': parseFloat(json.XETH).toFixed(4),
			'LOW': parseFloat(json.XETHZEUR.l[1]).toFixed(4),
			'HIGH': parseFloat(json.XETHZEUR.h[1]).toFixed(4),
			'CURRENT': parseFloat(json.XETHZEUR.c[0]).toFixed(4)
    });
}

function xhrWrapper(url, type, data, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(xhr);
  };
  xhr.open(type, url);
  if(true) {
    xhr.setRequestHeader('Content-Type', 'application/json');
    //xhr.send(JSON.stringify(data));
		 xhr.send();
  } else {
    xhr.send();
  }
}