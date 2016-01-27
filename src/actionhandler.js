var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

var temp_mode = ".";
var options = {};
var initialized = false;
console.log('options ' + options);
console.log('temp_mode ' + temp_mode);

if (temp_mode==".") { temp_mode = "C"; }

function locationSuccess(pos) {
  // Construct URL
  var apikey = "c33a3bee9eea2a4f11713c6bbc879951";
  var loc = navigator.language;
  console.log('loc = ' + loc);
  console.log('loc.length = ' + loc.length);
  if (loc.length > 2 ){
    loc = loc.substr(0, 2);
  }
  console.log('loc = ' + loc);
  console.log('loc.length = ' + loc.length);
  var url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
      pos.coords.latitude + "&lon=" + pos.coords.longitude + "&lang=" + loc +
      "&APPID=" + apikey;
  //console.log('url = ' + url);
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);
      console.log("Temperature(C) is " + temperature);
      var farenheit = Math.round(temperature*1.8+32);
      console.log("Temperature(F) is " + farenheit);
      
      // Conditions
      var name = json.name;      
      console.log("City is " + name);
            
      // Condition ID
      var conditionid = json.weather[0].id;
      var id = "";
      switch(conditionid){      
      //Gewitter
        case 200: id = "O";
        break;
        case 201: id = "O";
        break;
        case 202: id = "O";
        break;
        case 210: id = "O";
        break;
        case 211: id = "O";
        break;
        case 212: id = "O";
        break;
        case 221: id = "O";
        break;
        case 230: id = "O";
        break;
        case 231: id = "O";
        break;
        case 232: id = "O";
        break;
      
        //Niesel
        case 300: id = "Q";
        break;
        case 301: id = "Q";
        break;
        case 302: id = "Q";
        break;
        case 310: id = "Q";
        break;
        case 311: id = "Q";
        break;
        case 312: id = "Q";
        break;
        case 313: id = "Q";
        break;
        case 314: id = "Q";
        break;
        case 321: id = "Q";
        break;
      
        //Regen
        case 500: id = "R";
        break;
        case 501: id = "R";
        break;
        case 502: id = "R";
        break;
        case 503: id = "R";
        break;
        case 504: id = "R";
        break;
        case 511: id = "R";
        break;
        case 520: id = "R";
        break;
        case 521: id = "R";
        break;
        case 522: id = "R";
        break;	
        case 531: id = "R";
        break;
      
        //Schnee
        case 600: id = "W";
        break;
        case 601: id = "W";
        break;
        case 602: id = "W";
        break;
        case 611: id = "W";
        break;
        case 612: id = "W";
        break;
        case 615: id = "W";
        break;
        case 616: id = "W";
        break;
        case 620: id = "W";
        break;
        case 621: id = "W";
        break;
        case 622: id = "W";
        break;
      
        //Nebel
        case 701: id = "M";
        break;
        case 711: id = "M";
        break;
        case 721: id = "M";
        break;
        case 731: id = "M";
        break;
        case 741: id = "M";
        break;
        case 751: id = "M";
        break;
        case 761: id = "M";
        break;
        case 762: id = "M";
        break;
        
        //Sturm
        case 771: id = "F";
        break;
        case 781: id = "F";
        break;
      
        //Klar
        case 800: id = "B";
        break;
      
        //Bewoelkt
        case 801: id = "H";
        break;
        case 802: id = "N";
        break;
        case 803: id = "Y";
        break;
        case 804: id = "Y";
        break;
        
        //Extremes
        case 900: id = "F";
        break;
        case 901: id = "F";
        break;
        case 902: id = "F";
        break;
        case 903: id = "E";
        break;
        case 904: id = "B";
        break;
        case 905: id = "F";
        break;
        case 906: id = "X";
        break;
        
        
        case 951: id = "J";
        break;
        case 952: id = "F";
        break;
        case 953: id = "F";
        break;
        case 954: id = "F";
        break;
        case 955: id = "F";
        break;
        case 956: id = "F";
        break;
        case 957: id = "F";
        break;
        case 958: id = "F";
        break;
        case 959: id = "F";
        break;
        case 960: id = "F";
        break;
        case 961: id = "F";
        break;
        case 962: id = "F";
        break;
        default:	id = "G";
        break;   
      }

      console.log('Condition ID is ' + conditionid);
      console.log('Icon is ' + id);
      console.log('temp_mode pre key transfer' + temp_mode);
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TEMPERATURE": temperature,
        "KEY_CONDITIONS": name,
        "KEY_CONDITIONS_ID": id,
        "KEY_FARENHEIT": farenheit,
        "KEY_TEMP_MODE": temp_mode
      };
      console.log('temp_mode after key transfer' + temp_mode);
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 30000, maximumAge: 90000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
    initialized = true;
    // Get the initial weather
    getWeather();
  }
);

Pebble.addEventListener('showConfiguration', function(e) {
    //Load the remote config page
    Pebble.openURL('http://rusche.it/pebble/metroweather/metroconfig.html?'+encodeURIComponent(JSON.stringify(options)));
});

Pebble.addEventListener('webviewclosed',
  function(e) {
    //Get JSON dictionary
    if (e.response.charAt(0) == "{" && e.response.slice(-1) == "}" && e.response.length > 5) {
    options = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration window returned: ' + JSON.stringify(options));
    temp_mode = options.temp_mode;
      getWeather();
    } else {
    console.log("Cancelled");
    }
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  }                     
);