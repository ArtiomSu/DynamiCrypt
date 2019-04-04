
#wget -S --header="Accept-Encoding: gzip, deflate" --header='Accept-Charset: UTF-8' --header='Content-Type: application/json' -O response.json --post-data '{"service_name": "ss1","address_of_this_tpm": "127.0.0.1","port_of_this_tpm": 8001}' 127.0.0.1:3000/form_get_service_name &> /dev/null
# this posts data to the nodejs app routes will use later to send multiple decrypt messages 
# this is handy to see the difference between encryption modes
# since keys are sometimes generated faster than you can type and click send in the form

wget -S --header="Accept-Encoding: gzip, deflate" --header='Accept-Charset: UTF-8' --header='Content-Type: application/json' -O /dev/null --post-data '{"service_name": "ss1","address_of_this_tpm": "127.0.0.1","port_of_this_tpm": 8001}' 127.0.0.1:3000/form_get_service_name &> /dev/null

