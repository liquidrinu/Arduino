const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
      <meta charset="utf-8">
      <meta name="apple-mobile-web-app-capable" content="yes">
      <meta name="mobile-web-app-capable" content="yes">
      <meta name="viewport" content="user-scalable=no" initial-scale="1.0" maximum-scale="1">

      <TITLE>plant-o-meter</TITLE>
      
	</HEAD>
  <STYLE>
  body {
    background-color: black;
    color: black;
  }
  button {
    color: green;
    font-size: 40px;
    width: 50%;
    height: 200px;
    border: solid green 6px;
    background-color: black;
  }
  </STYLE>
<BODY>
	<CENTER>

        <button type="button" onclick="lightState()">
        lights</button>

	</CENTER>	
</BODY>
<SCRIPT>
function lightState() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      
    }
  };
  xhttp.open("GET", "lights", true);
  xhttp.send();
}
</SCRIPT>
</HTML>
)=====";
