With the hybrid working model, many of us now wear headphones so we can attend virtual meetings or listen to some light music while we work. This makes it difficult for others who come to talk to you to know whether you're busy in a meeting and/or just forgot to take your headphones off.

The idea for this project is to have an LED status indicator sitting somewhere noticeable at your cubicle to indicate to others whether you're free to chat or simply don't bother you at this time. This LED status indicator could be useful as well if you're working remotely to indicate that you're in a meeting but not in the office.

Design
Hardware:
	Use Arduino GPIO (5V) to control a bi-color LED (green and red) 
	Arduino gets the data from host via serial port. Once the device gets "0" (character), turns red LED ON and green LED OFF; gets "1", turn green LED ON and red LED OFF 

Software:
	Zoom Integration:
		Reads task list and attempt to find the zoom meeting process (CptHost.exe).
		If found: light turns red, if not; light turns green.
	MS Teams Integration:
		Reads MS Teams logs stored in local user folder. 
		Extract lines with regex that contains the presence status (Available, Busy, Do Not Disturb and etc.)
		If last status is "Available": light turns green
		If anything else (Busy, Away, Do Not Disturb ...): light turns red
		Short Video Demo: https://www.youtube.com/shorts/XCUdH-NWIPE

Setup:
Hardware:
	Connect Arduino to Host PC through serial port (Baud rate: 115200)
	Note the serial COM port number enumerated on the host in Device Manager

Software:
	Modify python script to update the COM port enumerated
	py -3 -m pip install pyserial
	Run the python script

Future Considerations
Currently the design requires a wired connection from the host PC to the Arduino board. For future iterations, we considered adding Bluetooth of WIFI modules so that instead of communicating through the serial port, we can communicate over WIFI or Bluetooth and the Arduino can be battery powered. This allows the user more flexibility on where the LED can be placed at their cubicle.
