t <html><head><title>TIME info</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("time.cgx", 500);
t function plotADGraph() {
t  adVal = document.getElementById("ad_value_fecha").value;
# t  document.getElementById("ad_table").style.width = (tableSize + '%');
t  document.getElementById("ad_value").value= (adVal);
t }
t  function periodicUpdateAd() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(formUpdate,plotADGraph);
t   ad_elTime = setTimeout(periodicUpdateAd, formUpdate.period);
t  }
t  else
t   clearTimeout(ad_elTime);
t }
t </script></head>
i pg_header.inc
t <h2 align="center"><br>Visualizacion de la informacion de la fecha y hora</h2>
t <p><font size="2">This page allows you to monitor AD input value in numeric
t  and graphics form. Periodic screen update is based on <b>xml</b> technology.
t  This results in smooth flicker-free screen update.<br><br>
t  Turn potentiometer on an evaluation board clockwise or counterclockwise
t  and observe the change of AD value on the screen.</font></p>
t <form action="time.cgi" method="post" name="time">
t <input type="hidden" value="time" name="pg">
t <table border=0 width=99%><font size="3">
t <tr style="background-color: #aaccff">
t  <th width=15%>Item</th>
t  <th width=15%>hora/fecha</th>
# t  <th width=15%>Fecha</th>
#  <th width=55%>Bargraph</th></tr>
t <tr><td><img src="pabb.gif">TIME:</td>
t   <td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c i 1  size="40" id="ad_value_fecha" value="%s"></td>
#t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
#c i 2  size="10" id="ad_value_fecha" value="%s"></td>
# <td height=50><table bgcolor="#FFFFFF" border="2" cellpadding="0" cellspacing="0" width="100%"><tr>
# g 3 <td><table id="ad_table" style="width: %d%%" border="0" cellpadding="0" cellspacing="0">
# <tr><td bgcolor="#0000FF">&nbsp;</td></tr></table></td></tr></table></td></tr>
t </font></table>
t <p align=center>
t <input type=button value="Refresh" onclick="updateMultiple(formUpdate,plotADGraph)">
t Periodic:<input type="checkbox" id="adChkBox" onclick="periodicUpdateAd()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period
