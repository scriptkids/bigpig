#!/usr/bin/python2

#print "HTTP/1.1 200 OK"
#print "Content-type: text/html"
#print "Content-Length: 20"
print 


import cgitb; cgitb.enable()

import cgi



form = cgi.FieldStorage()
thing = form.getvalue('firstname')
print "<h1>you like %s</h1>" %(thing)


