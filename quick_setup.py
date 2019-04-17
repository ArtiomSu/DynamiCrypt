#!/usr/bin/env python3.7

import requests
import time

Service_one_name = "service1"
Service_two_name = "app2"

Service_one_address = "127.0.0.1"
Service_one_port = 3000

Service_two_address = "127.0.0.1"
Service_two_port = 4000

Service_one_API_address = "127.0.0.1"
Service_two_API_address = "127.0.0.1"

Service_one_API_port = 9081
Service_two_API_port = 9082


url1 = "http://" + Service_one_address + ":" + str(Service_one_port) + "/form_get_service_name"
data = {'Service_name':Service_one_name, 'API_port': Service_one_API_port, 'API_Address': Service_one_API_address} 

r = requests.post(url = url1, data = data) 


url1 = "http://" + Service_two_address + ":" + str(Service_two_port) + "/form_get_service_name"
data = {'Service_name':Service_two_name, 'API_port': Service_two_API_port, 'API_Address': Service_two_API_address} 

r = requests.post(url = url1, data = data) 

time.sleep(0.5)

# now send info to parnter
url2 = "http://" + Service_one_address + ":" + str(Service_one_port) + "/form_send_to_partner"
data = {'port': Service_two_port, 'address': Service_two_address} 
r = requests.post(url = url2, data = data) 

# can now press sync in the browser

