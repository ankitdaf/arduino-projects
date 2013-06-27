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
from collections import deque

SERIAL_ADDRESS = '/dev/ttyACM0'
BAUD_RATE = 9600

timing = deque([])

try:
	ser = serial.Serial(SERIAL_ADDRESS, BAUD_RATE)
except:
	ser = None
	print"Could not open serial port"


def send_midi_event(a):
	# print "Sending "+a For debugging
	try:
		ser.write(a)
	except:
		print 'Could not send any serial data'


def main(arguments):
	try:
		if (len(arguments)==2):
			infile = open("difftimings.txt","r")
			for line in infile:
				temp = list(line.split(","))
				timing.append((float(temp[0]),temp[1]))
		pygame.display.init()
		pygame.display.set_mode((100, 100))
		pygame.mixer.init()
		pygame.mixer.music.load(arguments[1])
		print("The music will start shortly...")
		while(1):	# Try handshaking until correct
			ready = ser.read(size=1)
			if (ready=='1'):
				ser.write('2')
				break
		print("The music starts now")
		pygame.mixer.music.play()
		start_time = time.time()
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
			prev_time = time.time()
			while pygame.mixer.music.get_busy():
				now = time.time()
				diff = now-prev_time
				if(diff-timing[0][0]>=0):
					prev_time = now
					send_midi_event(timing[0][1])
					timing.popleft()
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
	
