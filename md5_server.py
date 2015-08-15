import httplib
import urllib
workers = [{ip: '127.0.0.1', port: 7000, workercnt: 16}]

for worker in workers:
	conn = httplib.HTTPConnection(worker['ip'], worker['port'])
	conn.request("GET", "/init/" + str(worker['workercnt']))
	res = conn.getresponse()
	if res.read() == 'done':
		print 'Started ' + str(worker['workercnt']) + ' workers on node ' + worker['ip']