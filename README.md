# DynamiCrypt

This project uses tree parity machines to generate keys dynamically to use with AES encryption.

This repo contains an API that allows data to be encrypted and decrypted dynamically. A sync server that manages the tree parity machines. An example NodeJS App that consumes the API and provides a basic messaging app like interface. Finally some python scripts that interact with the NodeJs app so you dont have to fill in the forms manually.

To execute the API and the sync server run the following (API and sync server are part of the same proccess)<br/>
cd into DynamiCrypt/DynamiCrypt and sure you can see all .cpp files when you ls<br/>
g++ *.cpp -lboost_system -lpthread -lboost_thread -lboost_program_options -lcryptopp -lpistache -o DynamiCrypt<br/><br/>

If there are errors while compiling please install boost libraries and Pistache<br/>
https://www.boost.org/<br/>
https://github.com/oktal/pistache<br/>

<br/>Also when syncing the sync server will spawn a gnome-terminal instance to write info about the keys received.<br/>
If you dont have gnome-terminal installed you can edit the source code to use whichever terminal you want or you can tell DynamiCrypt to write everything into one terminal by adding. <br/>
--PRINT_KEYS_TO_EXTERNAL_GNOME_TERMINAL 0<br/>
like so:<br/>
./DynamiCrypt --listen-port 8001 --api-port 9081 --PRINT_KEYS_TO_EXTERNAL_GNOME_TERMINAL 0<br/>
For more toggle switches and options you can type in:<br/>
./DynamiCrypt --help <br/>
or simply ./DynamiCrypt will print the same help message since it needs the listen-port and api-port to start executing.<br/><br/>

Then once there are no errors or warnings after compiling open up two terminals in the same directory where you compiled the binary.
In the first terminal execute:<br/>
./DynamiCrypt --listen-port 8001 --api-port 9081<br/>
And in the second terminal execute:<br/>
./DynamiCrypt --listen-port 8002 --api-port 9082<br/>

if there are issues with starting you have something running on the ports specified so just change them to what you like.<br/>

Next the NodeJs apps will need to be started up.<br/>
cd into DynamiCrypt/NodeApp/<br/>
run: <br/>
npm install <br/>
To install all the node modules<br/>

open up two terminals again<br/>
In the first terminal execute:<br/>
./run 3000<br/>
And in the second terminal execute:<br/>
./run 4000<br/><br/>
This will execute two nodejs apps one on port 3000 and the other on port 4000.<br/>
You can open up the web-browser to see the apps<br/>
http://127.0.0.1:3000/<br/>
http://127.0.0.1:4000/<br/><br/>

Now you can either fill out the forms manually to register the node apps with the apis or you can use my python script quick_setup.py which will fill out the forms you need for setup.<br/>
If using script make sure that the ports and ips are correct inside the script. The default ones provided should work if you used the same ports as I specified above.<br/>

<br/>
If you decide to fill them out manually you can do the following<br/>
In the browser at http://127.0.0.1:3000/<br/>
you can fill out the form providing any service name you want<br/>
The API port will be 9081 if you are following the same setup.<br/>
And the API address will be local host you can put in 127.0.0.1<br/><br/>

In the browser at http://127.0.0.1:4000/ fill out the same form<br/>
you can fill out the form providing any service name you want<br/>
The API port will be 9082 if you are following the same setup.<br/>
And the API address will be local host you can put in 127.0.0.1<br/><br/>

The next form you only need to fill out in one of the browser windows as it will send the needed info to the other NodeJs app regardless.<br/>
So lets use the window pointing at http://127.0.0.1:3000/<br/>
This will ask for the info of the nodeJS app that is running on port 4000<br/>
For port fill in 4000<br/>
for address fill in 127.0.0.1<br/><br/>

Now you should see a send message box in both of the browsers. At this time the tree parity machines did not begin syncronising to start sync press the green start sync button at the top of one of the browsers it should be visible in the browser accessing port 3000.
Now the tree parity machines will begin to syncronise and you will see varius output displayed in the first two terminals. You will also see two other terminals spawn and display keys aswell as the time it took to obtain the key.<br/><br/>

You can type and send messages by typing into the message box. You will see your message in plaintext and encrypted form in both of the browser windows.<br/>
You can send messages automatically by using another script called send_messages.py.<br/>
Again make sure to change the variables in the script to suit your setup. The script is handy for testing different ecryption options i.e. fast or secure.

### Demo
[![Demo](https://img.youtube.com/vi/LsR4XsGrDCY/0.jpg)](https://www.youtube.com/watch?v=LsR4XsGrDCY)



