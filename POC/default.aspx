<% @Page Language="C#" %>
<% 
var cf=Request.ClientCertificate["SUBJECTCN"].Split('/')[0];
%>
<!DOCTYPE html>
<html>
<body>
<p>Benvenuto <%=Request.ClientCertificate["SUBJECTG"]%> <%=Request.ClientCertificate["SUBJECTSN"]%></p>
<p>CF: <%=cf%></p>

</body>
</html>


