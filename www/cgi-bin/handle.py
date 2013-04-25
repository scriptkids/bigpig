#!/usr/bin/python2

import cgitb; cgitb.enable()

import cgi
form = cgi.FieldStorage()
name = form.getvalue('firstname')
passwd = form.getvalue('passwd')
try:
    html1 = "<html> your username is " + name
    html2 = "your passwd is " + passwd + "</html>"
except:
    html1 = "<html> your username is NULL "
    html2 ="your passwd is NULL </html>"
#print "HTTP/1.1 200 OK"
#print "Content-type: text/html"
print "Content-Length: %d" %(len(html1)+len(html2)+1)
#print "Transfer-Encoding: chunked"
print 




#result = eval(thing)
print html1,html2
print


#print "<h1> the result is length over over  %s</h1>" %(result)

#print "<h1>i think you like you like you like %s</h1>" %(thing)
#print 
#print

