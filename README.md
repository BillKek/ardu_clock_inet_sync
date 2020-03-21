# ardu_clock_inet_sync
clock on arduino mega (based on 7 segments indicator) with esp8266 time synchronization with a time server (wifi, udp to port 123)

# used libraries
Used ESP8266 library (integrated to this project with little modification). Library created by Stan Lee(Lizq@iteadstudio.com) 2014/10/8

# used hardware
Used and tested on MEGA Arduino board (MEGA2560 Mega 2560 R3 REV3 ATmega2560-16AU CH340G) bought on Ali for 666 rubels (10 bucks). 
It has chinese replacement of USB-UART chip. 

Project theoretically can be modified into UNO board by changing define MEGA to UNO, and manually remap some pins, but UNO board has too small quantity of IO pins, and so it's not recommended.

WiFi module is small ESP8266 with basic default firmware for AT-commands.

7-segments indicators is LED-based for 5 volts levels with common cathode.

# hardware connections

ESP8266 connected to MEGA's RX1 TX1 UART port, powered by MEGA's 5V, EPS's reset pin connected to MEGA's A0 port.

ESP8266's CH_PD pin shorted to ESP8266's VCC.

7-segments indicators connected to MEGA's GPIO ports which are presented in array `pin2segs`. 

| indexes of array pin2segs | GPIO port | pin of 7-segments indicator |
| ------------------------- | --------- | --------------------------- |
| 0 | 53 | A of first indicator |
| 1 | 52 | B of first indicator |
| 2 | 51 | C of first indicator |
| 3 | 50 | D of first indicator |
| 4 | 49 | E of first indicator |
| 5 | 48 | F of first indicator |
| 6 | 47 | G of first indicator |
| 7 | 46 | A of second\next\#2 indicator |
| 8 | 45 | B of second\next\#2 indicator |
| 9 | 44 | C of second\next\#2 indicator |
| 10 | 43 | D of second\next\#2 indicator |
| 11 | 42 | E of second\next\#2 indicator |
| 12 | 41 | F of second\next\#2 indicator |
| 13 | 40 | G of second\next\#2 indicator |
| 14 | 39 | dot or colon |
| 15 | 38 | A of third\3th\#3 indicator |
| 16 | 37 | B of third\3th\#3 indicator |
| 17 | 36 | C of third\3th\#3 indicator |
| 18 | 35 | D of third\3th\#3 indicator |
| 19 | 34 | E of third\3th\#3 indicator |
| 20 | 33 | F of third\3th\#3 indicator |
| 21 | 32 | G of third\3th\#3 indicator |
| 22 | 31 | A of fourth\4th\#4 indicator |
| 23 | 30 | B of fourth\4th\#4 indicator |
| 24 | 29 | C of fourth\4th\#4 indicator |
| 25 | 28 | D of fourth\4th\#4 indicator |
| 26 | 27 | E of fourth\4th\#4 indicator |
| 27 | 26 | F of fourth\4th\#4 indicator |
| 28 | 24 | G of fourth\4th\#4 indicator |


# setup and customizing 

Time zone is hardcoded into source-string:

`long tmHour = txTm_s_32 % 24 + 3; // 3 is msk`

Wi-Fi AP name and password is hardcoded into defines SSID & PASSWORD.

Time-Server IP address and port for UPD connection is hardcoded into source-string:

`wifi.ipConfig(UDP, "192.168.0.1", 123);`

It was Time-server in home internet router "mikrotik". It was necessary to install an additional package for this router to activate home time-server. Non-home external time-servers is also fits and works well at first sight.
