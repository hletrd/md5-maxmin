import subprocess
import sys
import datetime
import random
import threading
import httplib
import urllib
import json

tot = 0
md5min = 0xffffffffffffffffffffffffffffffff
md5max = 0
md5bitmin = 512
md5bitmax = 0
nick = "HLETRD_OpenCL"
chunk = 20000 * 16384

class t(threading.Thread):
	def __init__(self, rank, basestr):
		self.rank = str(rank)
		self.basestr = basestr
		threading.Thread.__init__(self)

	def run(self):
		global nick
		self.process = subprocess.Popen(['./md5_cl_py.run', self.basestr, str(0), self.rank], shell=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		while True:
			self.nextline = self.process.stdout.readline()
			if self.nextline == '' and self.process.poll() != None:
				print 'End: ' + self.rank
				break
			#print >> sys.stderr, self.nextline + "\n"
			try:
				if self.nextline[0] == '0':
					global md5max
					if md5max < int(self.nextline.split('||')[1], 16):
						md5max = int(self.nextline.split('||')[1], 16)
						print 'Worker #' + self.rank + ' found max: ' + self.nextline.split('||')[1] + ' (' + self.nextline.split('||')[2] + ')'
						sys.stdout.flush()
						conn = httplib.HTTPConnection('0xf.kr')
						conn.request("GET", "/md5/update.php?c=max&t=" + urllib.quote_plus(self.nextline.split('||')[2]) + "&n=" + nick)
						res = conn.getresponse()
				elif self.nextline[0] == '1':
					global md5min
					if md5min > int(self.nextline.split('||')[1], 16):
						md5min = int(self.nextline.split('||')[1], 16);
						print 'Worker #' + self.rank + ' found min: ' + self.nextline.split('||')[1] + ' (' + self.nextline.split('||')[2] + ')'
						sys.stdout.flush()
						conn = httplib.HTTPConnection('0xf.kr')
						conn.request("GET", "/md5/update.php?c=min&t=" + urllib.quote_plus(self.nextline.split('||')[2]) + "&n=" + nick)
						res = conn.getresponse()
				elif self.nextline[0] == '2':
					global md5bitmax
					if md5bitmax > int(self.nextline.split('||')[3]):
						md5bitmax = int(self.nextline.split('||')[3]);
						print 'Worker #' + self.rank + ' found bitmax: ' + self.nextline.split('||')[1] + ' (' + self.nextline.split('||')[2] + ')'
						sys.stdout.flush()
						conn = httplib.HTTPConnection('0xf.kr')
						conn.request("GET", "/md5/update.php?c=bitmax&t=" + urllib.quote_plus(self.nextline.split('||')[2]) + "&n=" + nick)
						res = conn.getresponse()
				elif self.nextline[0] == '3':
					global md5bitmin
					if md5bitmin > int(self.nextline.split('||')[3]):
						md5bitmin = int(self.nextline.split('||')[3]);
						print 'Worker #' + self.rank + ' found bitmin: ' + self.nextline.split('||')[1] + ' (' + self.nextline.split('||')[2] + ')'
						sys.stdout.flush()
						conn = httplib.HTTPConnection('0xf.kr')
						conn.request("GET", "/md5/update.php?c=bitmin&t=" + urllib.quote_plus(self.nextline.split('||')[2]) + "&n=" + nick)
						res = conn.getresponse()
				elif self.nextline[0] == '9':
					global tot
					tot += chunk
					elapsed = datetime.datetime.now() - tstart
					speed = tot / (elapsed.seconds + elapsed.microseconds / 1000000.0)
					print str(tot) + ' hashes processed, ' + str(int(speed/10000)/100.00) + 'M hashes / sec'
					sys.stdout.flush()
			except ValueError:
				print 'ValueError'
			except IndexError:
				pass
			except:
				pass

cnt = 2 #int(raw_input("Input number of workers[4]: ") or 4)
basestr = list("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
random.shuffle(basestr)
basestr = ''.join(basestr)
tstart = datetime.datetime.now()
print 'Initializing with ' + str(cnt) + ' workers'
sys.stdout.flush()

for i in range(1,cnt):
	T = t(i, basestr[0:17])
	T.start()
	basestr = basestr[1:]
