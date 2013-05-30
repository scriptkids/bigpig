#!/usr/bin/python2

import cgitb; cgitb.enable()

import cgi
form = cgi.FieldStorage()
name = form.getvalue('firstname')
passwd = form.getvalue('passwd')
gname = form.getvalue("gfirstname");
gpasswd = form.getvalue("gpasswd");
try:
    html1 = "<html> your username is " + name
except:
    html1 = "<html> your username is NULL "
try:
    html2 = "your passwd is " + passwd 
except:
    html2 ="your passwd is NULL"
try:
    html3 = "<html> your username is " + gname
except:
    html3 = "<html> your username is NULL "
try:
    html4 = "your passwd is " + gpasswd + "</html>"
except:
    html4 ="your passwd is NULL </html>"

print "HTTP/1.1 200 OK"
print "Content-type: text/html"
print "Content-Length: %d" %(len(html1)+len(html2)+1+len(html3)+len(html4))

#print "Transfer-Encoding: chunked"
print 



#result = eval(thing)
print html1,html2,html3,html4
print


#print "<h1> the result is length over over  %s</h1>" %(result)

#print "<h1>i think you like you like you like %s</h1>" %(thing)
#print 
#print

