const char MAIN_page[] PROGMEM = R"=====(
<HTML>

<HEAD>
  <meta charset="utf-8">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="mobile-web-app-capable" content="yes">
  <meta name="viewport" content="user-scalable=no" initial-scale="1.0" maximum-scale="1">

  <TITLE>plant-o-meter</TITLE>

  <STYLE>
    body {
      background-color: black;
      color: black;
    }

    button {
      color: white;
      font-size: 80px;
      width: 50%;
      height: 200px;
      border: solid green 6px;
      background-color: black;
    }

    .data {
      color: yellow;
      font-size: 80px;
    }

    .power {
      color: white;
      font-size: 80px;
    }

    div {
      margin: 2px;
    }
  </STYLE>

</HEAD>

<BODY>
  <CENTER>

    <div id="main">
      <div id="humidity" class="data"></div>
      <div id="temperature" class="data"></div>
      <div id="soil" class="data"></div>
      <div id="power" class="power"></div>
    </div>

    <button type="button" onclick="lightState()">
      lights
    </button>

  </CENTER>

  <SCRIPT>
    function lightState() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {

        }
      };
      xhttp.open("GET", "lights", true);
      xhttp.send();
    }

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

      let str = val.split(" ", 4);
      let value = ["humidity", "temperature", "soil", "power"];
      let symbol = ["%", "C", "%", ""];

      for (let i = 0; i <= 3; i++) {
        if (str[i] !== null || str[i] !== undefined) {
          divider(str[i], symbol[i], value[i]);
        }
      }

      function divider(str, symbol, value) {
        let output = "";
        output = value + ": " + str + symbol;
        document.getElementById(value).innerHTML =
          output;
      }
    }

  </SCRIPT>

</BODY>

</HTML>
)=====";
