"""

Python script to extract timing from music and store it in text file
Thanks to Alex for this : https://gist.github.com/alexrdp90/5789345

Modified to take in multiple events, and use the same to provide timing to Arduino via Pyserial

TODO : Make provision to halt the timer and sending of signals in case the Arduino is "interrupted"

Edited by 
Ankit Daftery
24 June 2013

"""
import pygame
import time
import sys
import serial
from threading import Timer

SERIAL_ADDRESS = '/dev/ttyACM1'
BAUD_RATE = 9600
try:
	ser = serial.Serial(SERIAL_ADDRESS, BAUD_RATE)
except:
	print"Could not open serial port"
BACKWARD= 0
FORWARD = 1
BLINK = 2

def send_midi_event(a):
	try:
		ser.write(a)
	except:
		print 'Could not send any serial data'

def start_timing(t):
	for times in t:
		times.start()


def main(arguments):
	try:
		if (len(arguments)==2):
			infile = open("timings.txt","r")
			timers = []
			for line in infile:
				temp = list(line.split(","))
				timers.append(Timer(float(temp[0]),send_midi_event,(temp[1],)))
		pygame.display.init()
		pygame.display.set_mode((100, 100))
		pygame.mixer.init()
		pygame.mixer.music.load(arguments[1])
		print("The music will start shortly...")
		while(1):
			print "a"
			ready = ser.read(size=1)
			print ready
			if (ready=='1'):
				ser.write('2')
				break
		print("The music starts now")
		pygame.mixer.music.play()
		if(len(arguments)==3):
			f = open(arguments[2], 'w')
			while pygame.mixer.music.get_busy():
				for event in pygame.event.get():
					if event.type == pygame.QUIT:
						sys.exit(0)
					if event.type == pygame.KEYDOWN:
						diff = time.time()-start_time
						f.write("{}".format(diff)+","+chr(event.key)+"\n")
						print(diff)
			f.close()
		else:
			start_timing(timers)
			start_time = time.time()
			while pygame.mixer.music.get_busy():
				pass
		print("The music has just finished")
		pygame.display.quit()
		pygame.mixer.quit()
	except KeyboardInterrupt:
		pygame.mixer.quit()
		pygame.display.quit()
 
if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("Usage: {} <ogg_file> <output_file>".format(sys.argv[0]))
		sys.exit(1)
	if len(sys.argv) == 2 or len(sys.argv) == 3:
		main(sys.argv)
	
