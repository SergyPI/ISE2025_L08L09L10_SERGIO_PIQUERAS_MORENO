t <html><head><title>RTC Control</title></head>
t <h2 align=center><br>RTC Module Control</h2>
t <p><font size="2">Esta pagina es para configurar el RTC <b>LCD Module</b>.
t  Simply enter the date and time which you want to set and click <b>Send</b>.<br><br>
t  This Form uses a <b>POST</b> method to send data to a Web server.</font></p>
t <form action=rtc.cgi method=post name=cgi>
t <input type=hidden value="rtc" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
t <tr><td><img src=pabb.gif>Año</td>
c h 1 <td><input type=number name=Año size=4 maxlength=4 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Mes</td>
c h 2 <td><input type=number name=Mes size=2 maxlength=2 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Dia</td>
c h 3 <td><input type=number name=Dia size=2 maxlength=2 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Hora</td>
c h 4 <td><input type=number name=Hora size=2 maxlength=2 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Minutos</td>
c h 5 <td><input type=number name=Minutos size=2 maxlength=2 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Segundos</td>
c h 6 <td><input type=number name=Segundos size=2 maxlength=2 value="%s"></td></tr>
t </font></table>
t <p align=center>
t <input type=submit name=set value="Send" id="sbm">
t <input type=reset value="Undo">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
