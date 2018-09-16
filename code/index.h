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
      outline: none;
    }

    body {
      background-color: black;
      color: black;
      clear: both;
      touch-action: manipulation;
    }

    center {
      max-width: 600px;
      margin-left: auto;
      margin-right: auto;
    }

    #header {
      background-color: #dc0567;
      color: white;
      font-size: 44px;
      font-family: palantino;
      margin-bottom: 5%;
    }

    #main {
      margin-top: 4px;
      margin-bottom: 2%;
      width: 90%;
    }

    button,
    input {
      color: white;
      font-size: 22px;
      height: 42px;
      margin: 2px;
      border: solid #dc0567 2px;
      background-color: black;
    }

    .controls {
      margin-top: 2%;
      padding-top: 2%;
      margin-bottom: 2%;
    }

    .controls button {
      color: white;
      font-size: 22px;
      width: 90%;
      height: 48px;
      border: solid #dc0567 2px;
      background-color: black;
    }
    
    .controls div button {
      display: inline-block;
      width: 45%;
      font-size: 22px;
    }
   

    input {
      text-align: center;
    }

    .data {
      font-family: palantino;
      /*color: #057e84;*/
      color: #078d93;
      font-size: 36px;
      text-align: left;
    }

    .power {
      font-family: verdana;
      color: white;
      font-size: 20px;
      text-align: left;
      padding-top: 1%;
      margin-bottom: 1%;
    }

    div {
      margin: 4px;
    }

    .config div {
      clear: none;
    }

    #soil_input {
      width: 60%;
      color: gray;
      border: solid 2px grey;
      font-size: 16px;
    }

    #pump_input {
      width: 60%;
      color: gray;
      border: solid 2px grey;
      font-size: 16px;
    }

    .config div button {
      width: 30%;
      font-size: 16px;
    }

    .colorOn {
      color: rgb(57, 214, 57);
    }

    .colorOff {
      color: red;
    }

    /* ANIMATIONS */

    .blinkBtn {
      animation: blinky 0.2s linear infinite;
    }

    @keyframes blinky {
      80% {
        opacity: 0;
      }
    }

    #breaker {
      width: 100%;
      background-color: purple;
      height: 4px;
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
      <div id="breaker"></div>
      <div id="display" class="power"></div>
      <div id="lights" class="power"></div>
      <div id="treshold" class="power"></div>
    </div>

    <div class="controls">
      <div>
        <button type="button" id="displayBtn" onclick="ajaxBtn('display', 'displayBtn')">
          display
        </button>

        <button type="button" id="lightBtn" onclick="ajaxBtn('lights', 'lightBtn')">
          lights
        </button>
      </div>

      <button type="button" id="soilBtn" onclick="ajaxBtn('soil_reading', 'soilBtn')">
        soil reading
      </button>

      <button type="button" id="pumpBtn" onclick="ajaxBtn('pump', 'pumpBtn')">
        pump water
      </button>

    </div>

    <div class="config">

      <div>
        <input id="soil_input" placeholder="treshold 0 - 100" type="number" autocomplete="off" autocorrect="off"
          autocapitalize="off" spellcheck="false" />
        <button type="button" onclick="soil_limit()">
          submit
        </button>
      </div>
    </div>

  </CENTER>

  <SCRIPT>

    function ajaxBtn(url, id) {
      document.getElementById(id).style.backgroundColor = '#dc0567';
      document.getElementById(id).classList.add('blinkBtn');

      let xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
          if (this.responseText == "done") {
            document.getElementById(id).style.backgroundColor = 'black';
            document.getElementById(id).classList.remove('blinkBtn');
            document.getElementById("pumpBtn").innerHTML = 'pump water';
          }
          // pump safety lock 
          if (this.responseText == "locked") {
            document.getElementById("pumpBtn").style.backgroundColor = 'black';
            document.getElementById("pumpBtn").style.color = 'white';
            document.getElementById("pumpBtn").innerHTML = '* LOCKED *';
            document.getElementById(id).classList.remove('blinkBtn');
          }

        }
      };
      xhttp.open("POST", url, true);
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

      let str = val.split(" ", 6);
      let value = ["humidity", "temperature", "soil", "display", "lights", "treshold"];
      let symbol = ["%", "C", "%", "", "", "%"];

      for (let i = 0; i < value.length; i++) {
        if (str[i] !== null || str[i] !== "undefined") {
          divider(str[i], symbol[i], value[i]);
        }
      }

      function divider(str, symbol, value) {
        let output = "";
        let colorClass = "";

        if (value === "display" || value === "lights") {
          if (str === "on") {
            output = value + ": ";
            colorClass = "<span class='colorOn'>" + str + "</span>";
            output += colorClass;
            document.getElementById(value).innerHTML =
              output;
          } else {
            output = value + ": ";
            colorClass = "<span class='colorOff'>" + str + "</span>";
            output += colorClass;
            document.getElementById(value).innerHTML =
              output;
          }
        } else {
          output = value + ": " + str + symbol;
          document.getElementById(value).innerHTML =
            output;
        }
      }
    }

    // soil config
    function soil_limit() {

      let z = 0;
      let y = document.getElementById('soil_input');

      z = y.value;

      if (z < 99 && z > 9 && z !== "") {
        let xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {
          }
        }
        xhttp.open('GET', 'soilmem?soil_value=' + z, true);
        xhttp.send();
      }
      document.getElementById('soil_input').value = "";
    }

    // pump config
    function pump_limit() {

      let z = 0;
      let y = document.getElementById('pump_input');

      z = y.value;

      if (z < 100 && z > 0 && z !== "") {
        let xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {
          }
        }
        xhttp.open('GET', 'pumpmem?pump_value=' + z, true);
        xhttp.send();
      }
      document.getElementById('pump_input').value = "";
    }

  </SCRIPT>

</BODY>

</HTML>
)=====";
