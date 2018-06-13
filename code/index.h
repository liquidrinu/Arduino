const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<TITLE>plant-o-meter</TITLE>
      <meta charset="utf-8">
      <meta name="viewport" content="user-scalable=no" initial-scale="1.0" maximum-scale="1">
	</HEAD>
  <STYLE>
  body {
    background-color: black;
    color: black;
  }
  button {
    color: pink;
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
