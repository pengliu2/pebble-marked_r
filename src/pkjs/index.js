//=======================================================================================================
// Yahoo! Weather implementation
//=======================================================================================================
var xhrRequest = function (url, type, callback) {
  
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);    
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSucess(pos){    
  var lat = pos.coords.latitude.toFixed(2);
  var lon = pos.coords.longitude.toFixed(2);
  
  var units = 'metric';
  var key = '3fdbf20b1e5ff07c60cb0004cbed6bb4';
  var api = 'http://api.openweathermap.org/data/2.5/weather?lon=' + lon + '&lat=' + lat + '&units=' + units + '&appid=' + key;
  
  // Send request to Openweather
  xhrRequest(encodeURI(api), 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      if( json.main !== undefined) {
        var temp = json.main.temp.toFixed(0);
        var cond = json.weather[0].main;
        var dictionary = {
          'WEATHER': cond + " " + temp + '°C'
        };
  
        // Send to Pebble
        Pebble.sendAppMessage(dictionary,
          function() {
            console.log('Weather info sent to Pebble successfully!');
          },
          function(e) {
            console.log('Error sending weather info to Pebble!');
            console.log(JSON.stringify(e));
          }
        );
      }
    }
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  
  watchId = navigator.geolocation.getCurrentPosition(
    locationSucess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );  
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    
    var dict = e.payload;
    
    if('RequestData' in dict){getWeather();}
       
  }                     
);