const char MAIN_page[] PROGMEM = R"=====(
<HTML>

<HEAD>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width" content="user-scalable=no" initial-scale="1.0" maximum-scale="1">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="mobile-web-app-capable" content="yes">

  <TITLE>plant-o-meter</TITLE>

  <STYLE>
    * {
      margin: 0;
      padding: 0;
      outline: 0;
      /*font-family: 'Franklin Gothic Medium', 'Arial Narrow', Arial, sans-serif; */
    }

    body {
      background-color: black;
      color: black;
      /* width: 100%; */
      clear: both;
      touch-action: manipulation;
    }

    #header {
      background-color: #dc0567;
      color: white;
      font-size: 44px;
    }

    button,
    input {
      color: white;
      font-size: 24px;
      width: 50%;
      height: 50px;
      margin: 2px;
      border: solid green 2px;
      background-color: black;
    }

    input {
      text-align: center;
    }

    .data {
      color: yellow;
      font-size: 32px;
    }

    .power {
      color: white;
      font-size: 32px;
    }

    div {
      margin: 4px;
    }
  </STYLE>

</HEAD>

<BODY>
  <CENTER>

    <div id="header">
      PLANT-0-METER
    </div>

    <div id="main">
      <div id="humidity" class="data"></div>
      <div id="temperature" class="data"></div>
      <div id="soil" class="data"></div>
      <div id="power" class="power"></div>
      <div id="treshold" class="power"></div>
    </div>

    <div>
      <button type="button" onclick="ajax('lights')">
        lights
      </button>

      <button type="button" onclick="ajax('soil_reading')">
        soil reading
      </button>
    </div>

    <div>
      <input id="soil_input" placeholder="treshold 0 - 100" type="number" autocomplete="off" autocorrect="off" autocapitalize="off"
        spellcheck="false" />
      <button type="button" onclick="soil_limit()">
        submit
      </button>
    </div>

  </CENTER>

  <SCRIPT>

    function ajax(url, data) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {

        }
      };
      xhttp.open("GET", url, true);
      xhttp.send();
    }

    // state update 
    (function ajaxFn() {
      let xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
          let val = this.responseText;
          fullState(val);
        }
      };
      xhttp.open("GET", "/data", true);
      xhttp.send();
      setTimeout(ajaxFn, 1000);
    })();

    function fullState(val) {

      let str = val.split(" ", 5);
      let value = ["humidity", "temperature", "soil", "power", "treshold"];
      let symbol = ["%", "C", "%", "", "%"];

      for (let i = 0; i <= 4; i++) {
        if (str[i] !== null || str[i] !== "undefined") {
          divider(str[i], symbol[i], value[i]);
          console.log(str[i]);
        }
      }

      function divider(str, symbol, value) {
        let output = "";
        output = value + ": " + str + symbol;
        document.getElementById(value).innerHTML =
          output;
      }
    }

    // soil config
    function soil_limit() {

      let z = 0;
      let y = document.getElementById('soil_input');
      z = y.value;

      if (z < 100 && z > 0 && z !== "") {
        let xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {

          }
        }
        xhttp.open('GET', 'soil_limit?soil_value=' + z, true);
        xhttp.send();
      }
      document.getElementById('soil_input').value = "";
    }

  </SCRIPT>

</BODY>

</HTML>
)=====";
