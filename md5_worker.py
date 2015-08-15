import subprocess
import sys
import datetime
import random
import threading
from flask import Flask

tot = 0
md5min = 0xffffffffffffffffffffffffffffffff
md5max = 0
bitmax = 0
bitmin = 128
basemin = 128
tmax = 0
logs = []
T = []

app = Flask(__name__)

class t(threading.Thread):
	def __init__(self, rank, basestr):
		self.rank = str(rank)
		self.basestr = basestr
		threading.Thread.__init__(self)

	def run(self):
		self.process = subprocess.Popen(['./md5.run', self.rank, self.basestr], shell=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		if self.rank == 1:
			self.tstart = datetime.datetime.now()
		while True:
			self.nextline = self.process.stdout.readline()
			if self.nextline == '' and self.process.poll() != None:
				break
			if self.nextline[0] == '0':
				global md5max
				if md5max < int(self.nextline.split('||')[1], 16):
					md5max = int(self.nextline.split('||')[1], 16)
					logs.append('0||' + self.rank + '||' + self.nextline)
			elif self.nextline[0] == '1':
				global md5min
				if md5min > int(self.nextline.split('||')[1], 16):
					md5min = int(self.nextline.split('||')[1], 16);
					logs.append('1||' + self.rank + '||' + self.nextline)
			elif self.nextline[0] == '2':
				global bitmax
				if bitmax < int(self.nextline.split('||')[3]):
					bitmax = int(self.nextline.split('||')[3]);
					logs.append('2||' + self.rank + '||' + self.nextline)
			elif self.nextline[0] == '3':
				global bitmin
				if bitmin > int(self.nextline.split('||')[3]):
					bitmin = int(self.nextline.split('||')[3]);
					logs.append('3||' + self.rank + '||' + self.nextline)
			elif self.nextline[0] == '4':
				global basemin
				if basemin > int(self.nextline.split('||')[3]):
					basemin = int(self.nextline.split('||')[3]);
					logs.append('4||' + self.rank + '||' + self.nextline)
			elif self.nextline[0] == '5':
				global tmax
				if tmax < int(self.nextline.split('||')[3]):
					tmax = int(self.nextline.split('||')[3]);
					logs.append('5||' + self.rank + '||' + self.nextline)
			elif self.nextline[0] == '6':
				global tmin
				if tmin > int(self.nextline.split('||')[3]):
					tmin = int(self.nextline.split('||')[3]);
					logs.append('6||' + self.rank + '||' + self.nextline)
			elif self.nextline[0] == '9':
				global tot, tstart, logs
				tot += 10000000
				elapsed = datetime.datetime.now() - tstart
				speed = tot / (elapsed.seconds + elapsed.microseconds / 1000000.0)
				if self.rank == 1:
					elapsed = datetime.datetime.now() - self.tstart
					speed_this = 10000000 / (elapsed.seconds + elapsed.microseconds / 1000000.0)
					logs.append('9||' + str(tot) + '||' + str(int(speed)) + '||' + str(int(speed_this)))
					self.tstart = date.datetime.now()
				#else:
					#logs.append('l||' + str(tot) + '||' + str(int(speed)))

@app.route('/init/<int:workercnt>')
def index(workercnt):
	basestr = list("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
	random.shuffle(basestr)
	basestr = ''.join(basestr)
	global tstart
	tstart = datetime.datetime.now()
	global T
	for i in range(1,workercnt+1):
		T[i] = t(i, basestr)
		T[i].start()
		basestr = basestr[1:]
	return 'done'

@app.route('/log')
def log():
	global logs
	result = ''
	for log in logs:
		result = result + log + '\n'
	logs = []
	return result

@app.route('/stop')
def stop():
	global T
	for i in T:
		i.stop()
		del i

if __name__ == '__main__':
	app.run(debug=False, port=7000)