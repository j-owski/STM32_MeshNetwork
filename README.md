# Mesh-Network over USART

This project uses the MicroController `STM32F401` and the RealTime Operating System `FreeRTOS` to implement a Mesh Network over USART.
A PC can connect over a terminal program with the Microcontroller (for best experience please use `TeraTerm`) and send messages over the terminal interface to the other connected PCs.

```
		send:           ╔════╗      ┌───┐        ┌───┐                              ╔════╗  reveive:
		PC2:"Hello PC2" ║PC 1║______|STM|________|STM|                 ┌───┐        ║PC 3║  PC2:"Hi there!"
						║    ║      └───┘        └───┘                 |STM|________║    ║
						╚════╝       /             /                   └───┘        ╚════╝
									/             |                  ___/              
								   /              /          ┌───┐__/                      
								  /              |       ____|STM|___                     
reveive:        ╔════╗           /              ┌───┐___/    └───┘   \                   ╔════╗
PC1:"Hello PC2" ║PC 2║        ┌───┐       ______|STM|                  \      ┌───┐      ║PC 4║
send:           ║    ║________|STM|_____/       └───┘                   \_____|STM|______║    ║
PC3:"Hi there!" ╚════╝        └───┘                                           └───┘      ╚════╝
```

The RealTime operating system allows the software to be decupled of the hardware layer underneath. So theoretically this program can be ported to other MicroControllers that implement FreeRTOS. This also allows to let different types of Microcontroller to talk to one and another over USART.

Here is the terminal initial interface view:
```
 __  ___  __   __    __
|__)  |  /  \ (_    |__)  _  _  .  _ |  |_
| \   |  \__/ __)   |    |  (_) | (- |( |_
                                /

by:  ~ Arseven Can
     ~ Abo Galila Mohamed
     ~ Ostrowski John

see /help for further information

> Please type a new ID (1..99): 1
Your ID is: 01
PC01 connected!

> /help
To send a message you need to first define your own ID (1..99)
Then you can send a message with:
XX: your_message
(XX is the ID of the receiver)

other possible commands:
/blink    -> blink the LED
/release  -> to release ID
/showDB   -> to show the database
/resetDB  -> reset the database
/hello    -> ask neighbor for update
/clear    -> clear terminal screen
/credits  -> show creators
/help     -> show this message again
```
We can see that we first have to enter our ID. This can be a value between 1 and 99. The program will notice if this ID already exists in the network and will prompt you to choose another ID number.

If another PC connects to the network the terminal will notify you about that.

You can send messages with the command: `XX: your message` where as `XX` stands for the ID of the receiver ID.
This project is only a proof of concept and the program will sadly only allow the text to be 40 characters long.

![](Doc/example.png)

## How it works
The routing is done on the basics of a very simple distance vector routing principle similar to RIPv1. It uses the PC ID, the hop count and the ingoing interface to determine the route of a message.

<p align="center">
  <img src="Doc/RTOS Projekt v6.jpg"="350" title="Diagram: How it works">
</p>

## How to install

### Flash compiled hex to a STM32F401re
- Download the compiled project `STM32F401xx.hex` for the STM32F401 from `Build`-folder
- Download the `STM32 ST-LINK utility` from the ST website
- On the same webpage you can download the driver for the STM32 called `ST-LINK/V2`
- Install the driver
- Open the ST-Link utility and open the file `STM32F401xx.hex`
- Now connect the STM32 to your PC and in your ST-Link utility click on `Connect`
- Make sure that the write memory map is choosen by clicking `Target`:`Target memory compare with file`
- Now you can flash the chip with `Target`:`Program & Verify` and click on `Start`
- Download the terminal program `TeraTerm`, open it and select `Setup`:`Serial port`<br>
  Port:your_COM_port, Baud rate:400000, Data:8bit, Parity:none, Stop:1bit, Flow control:none
- Click on OK and reset the STM32 by pressing the black button
- You should be greeted by a startup screen

### Build a Mesh-Network
Usart 1: PA9  (TX) and PA10 (RX)<br>
Usart 6: PA11 (TX) and PA12 (RX)<br>
You can use these Pins to connect on STM32 to another.<br>
Connect the TX from one Usart to the RX of the other & build your own Mesh-Network :)

## Compile yourself
I used `Keil uVision V5.23.0.0` to compile this project. Load all source files and `FreeRTOS V10.0.1` into your project. If you have a different MicroController you can change the hardware configuration in the `hardware.c` and `hardware.h`. If your courios how the program works behind the scence you can activate the `debug` messages in `main.h` (debug can be set from 0 to 4, depending how much you want to see).<br>
Happy hacking!
