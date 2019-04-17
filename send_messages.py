#!/usr/bin/env python3.7

import requests
import time

Service_address = "127.0.0.1"
Service_port = 3000



message = "hello there"
encrypt_mode = 1	# 1 for fast. i.e. encrypt using the latest key. 2 for secure only use 1 key once.

how_many_times = 20


url1 = "http://" + Service_address + ":" + str(Service_port) + "/encrypt"
data = {'message':message, 'encrypt_mode': encrypt_mode}

for i in range(0,how_many_times,1):
	print("sending")
	 
	r = requests.post(url = url1, data = data) 
	time.sleep(0.5)
