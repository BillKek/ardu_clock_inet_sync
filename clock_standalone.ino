#include <time.h>

// WARNING
// hardcoded_wifi ap name and password ! search first SSID & PASSWORD .
// hardcoded_time server  ! search wifi.ipConfig(UDP, "192.168.0.1", 123);


void data_to_indicator(unsigned short h1,unsigned short h2,unsigned short t,unsigned short m1,unsigned short m2);
extern short pin2segs[29];

#define standalone_sketch_here


// pre setup
#define SSID       "replace_to_your_wifi_access_point_name"
#define PASSWORD   "password_of_your_wifi_access_point_name"

#define PIN_RESET    97


#ifndef standalone_sketch_here
#include <uartWIFI.h>
#else
///////////////////////////////////////////////////////////// from uartWIFI.h

/*
ESP8266 library

Created by Stan Lee(Lizq@iteadstudio.com)
2014/10/8

Modified version
V1.0  released the first version of ESP8266 library



*/

#ifndef __UARTWIFI_H__
#define __UARTWIFI_H__
#include <Arduino.h>
//#include "NilRTOS.h"
#include <SoftwareSerial.h>

#define _DBG_RXPIN_ 2
#define _DBG_TXPIN_ 3

#define debugBaudRate 115200


//#define UNO     //uncomment this line when you use it with UNO board
#define MEGA    //uncomment this line when you use it with MEGA board


#define DEBUG


#ifdef UNO
#define _cell Serial
#define DebugSerial mySerial

#endif  
#ifdef MEGA
#define _cell Serial1
#define DebugSerial Serial
#endif  
    

    
    
#ifdef UNO
extern SoftwareSerial mySerial;

#endif


//The way of encrypstion
#define    OPEN          0
#define    WEP           1
#define    WAP_PSK       2
#define    WAP2_PSK      3
#define    WAP_WAP2_PSK  4

//Communication mode 
#define    TCP     1
#define    tcp     1
#define    UDP     0
#define    udp     0

#define    OPEN    1
#define    CLOSE   0

//The type of initialized WIFI
#define    STA     1
#define    AP      2
#define    AP_STA  3

#define SERIAL_TX_BUFFER_SIZE 128
#define SERIAL_RX_BUFFER_SIZE 128





class WIFI
{
  public:

    void begin(void);
  
  //Initialize port
  bool Initialize(byte mode, String ssid, String pwd, byte chl = 1, byte ecn = 2);
  boolean ipConfig(byte type, String addr, int port, boolean a = 0, byte id = 0);
  
  boolean Send(String str);  //send data in sigle connection mode
  boolean SendRaw(unsigned char* buff, long len);/// added !!!!!!!!!!!!!!!!!!!!!!!!!!!!
  boolean Send(byte id, String str);  //send data int multiple connection mode
    
  int ReceiveMessage(char *buf, long maxlen);
  
    //String begin(void);
    /*=================WIFI Function Command=================*/
    void Reset(void);    //reset the module
  bool confMode(byte a);   //set the working mode of module
  boolean confJAP(String ssid , String pwd);    //set the name and password of wifi 
  boolean confSAP(String ssid , String pwd , byte chl , byte ecn);       //set the parametter of SSID, password, channel, encryption in AP mode.
  
    String showMode(void);   //inquire the current mode of wifi module
    String showAP(void);   //show the list of wifi hotspot
    String showJAP(void);  //show the name of current wifi access port
    boolean quitAP(void);    //quit the connection of current wifi
    String showSAP(void);     //show the parameter of ssid, password, channel, encryption in AP mode

    /*================TCP/IP commands================*/
    String showStatus(void);    //inquire the connection status
    String showMux(void);       //show the current connection mode(sigle or multiple)
    boolean confMux(boolean a);    //set the connection mode(sigle:0 or multiple:1)
    boolean newMux(byte type, String addr, int port);   //create new tcp or udp connection (sigle connection mode)
    boolean newMux(byte id, byte type, String addr, int port);   //create new tcp or udp connection (multiple connection mode)(id:0-4) 
    void closeMux(void);   //close tcp or udp (sigle connection mode)
    void closeMux(byte id); //close tcp or udp (multiple connection mode)
    String showIP(void);    //show the current ip address
    boolean confServer(byte mode, int port);  //set the parameter of server
  
  String m_rev;

};

#endif

///////////////////////////////////////////////////////////// from uartWIFI.cpp

//#include "uartWIFI.h"

#ifdef UNO

SoftwareSerial mySerial(_DBG_RXPIN_,_DBG_TXPIN_);

#endif

#ifdef DEBUG
#define DBG(message)    DebugSerial.print(message)
#define DBGW(message)    DebugSerial.write(message)
#else
#define DBG(message)
#define DBGW(message)
#endif // DEBUG

int chlID;    //client id(0-4)

void WIFI::begin(void)
{
  boolean result = false;
  _cell.begin(115200);  //The default baud rate of ESP8266 is 
  
  DebugSerial.begin(debugBaudRate);   //The default baud rate for debugging is 
  _cell.flush();
  _cell.setTimeout(3000);
  _cell.println("AT+RST");
  DBG("AT+RST\r\n");
  result = _cell.find("eady");
  if(result)
    DBG("Module is ready\r\n");
    else
  {
    DBG("Module have no response\r\n");
    while(1);
  }

}


/*************************************************************************
//Initialize port

  mode: setting operation mode
    STA:  Station
    AP:   Access Point
    AT_STA: Access Point & Station

  chl:  channel number
  ecn:  encryption
    OPEN          0
    WEP           1
    WAP_PSK       2
    WAP2_PSK      3
    WAP_WAP2_PSK  4   

  return:
    true  - successfully
    false - unsuccessfully

***************************************************************************/
bool WIFI::Initialize(byte mode, String ssid, String pwd, byte chl, byte ecn)
{
  if (mode == STA)
  { 
    bool b = confMode(mode);
    if (!b)
    {
      return false;
    }
    Reset();
    confJAP(ssid, pwd);
  }
  else if (mode == AP)
  {
    bool b = confMode(mode);
    if (!b)
    {
      return false;
    }
    Reset();
    confSAP(ssid, pwd, chl, ecn);
  }
  else if (mode == AP_STA)
  {
    bool b = confMode(mode);
    if (!b)
    {
      return false;
    }
    Reset();
    confJAP(ssid, pwd);
    confSAP(ssid, pwd, chl, ecn);
  }
  
  return true;
}

/*************************************************************************
//Set up tcp or udp connection

  type: tcp or udp
  
  addr: ip address
  
  port: port number
  
  a:  set multiple connection
    0 for sigle connection
    1 for multiple connection
    
  id: id number(0-4)

  return:
    true  - successfully
    false - unsuccessfully

***************************************************************************/
boolean WIFI::ipConfig(byte type, String addr, int port, boolean a, byte id)
{
  boolean result = false;
  if (a == 0 )
  {
    confMux(a);
    
    long timeStart = millis();
    while (1)
    {
      long time0 = millis();
      if (time0 - timeStart > 5000)
      {
        break;
      }
    }
    result = newMux(type, addr, port);
  }
  else if (a == 1)
  {
    confMux(a);
    long timeStart = millis();
    while (1)
    {
      long time0 = millis();
      if (time0 - timeStart > 5000)
      {
        break;
      }
    }
    result = newMux(id, type, addr, port);
  }
  return result;
}

/*************************************************************************
//receive message from wifi

  buf:  buffer for receiving data
  
  chlID:  <id>(0-4)

  return: size of the buffer
  

***************************************************************************/
int WIFI::ReceiveMessage(char *buf, long maxlen)
{
  //+IPD,<len>:<data>
  //+IPD,<id>,<len>:<data>
  String data = "";
  if (_cell.available()>0)
  {
    
    unsigned long start;
    start = millis();
    char c0 = _cell.read();
    short len2 = 0;
    if (c0 == '+')
    {
      
      while (millis()-start<5000) 
      {
        if (_cell.available()>0)
        {
          char c = _cell.read();
          data += c;
          len2++;
        }
        else
        {
          if (len2 >= 59) // speedhack
            break; 
        }
        /*
        if (data.indexOf("\nOK")!=-1) // there are no OK !!!
        {
          break;
        }
        */
      }
      //Serial.println(data);
      int sLen = strlen(data.c_str());
      int i,j;
      for (i = 0; i <= sLen; i++)
      {
        if (data[i] == ':')
        {
          break;
        }
        
      }
      
      int iSize;
      //DBG(data);
      //DBG("\r\n");
      
      {     
      String _size = data.substring(4, i);
      iSize = _size.toInt();
      //DBG(iSize);
      //DBG("\r\n");
      //String str = data.substring(i+1, i+1+iSize); // substring corrupts raw string coz damned null-terminated
      long lencopy = iSize; if (lencopy > maxlen) lencopy = maxlen; memset(buf,0,maxlen); memcpy(buf,data.c_str() + i+1,lencopy);  
      
      //strcpy(buf, str.c_str());
      //DBG(str);
      }
      return iSize;
    }
  }
  
  return 0;
}

//////////////////////////////////////////////////////////////////////////


/*************************************************************************
//reboot the wifi module



***************************************************************************/
void WIFI::Reset(void)
{
    _cell.println("AT+RST");
  unsigned long start;
  start = millis();
    while (millis()-start<5000) {                            
        if(_cell.find("ready")==true)
        {
      DBG("reboot wifi is OK\r\n");
           break;
        }
    }
}

/*********************************************
 *********************************************
 *********************************************
             WIFI Function Commands
 *********************************************
 *********************************************
 *********************************************
 */

/*************************************************************************
//inquire the current mode of wifi module

  return: string of current mode
    Station
    AP
    AP+Station

***************************************************************************/
String WIFI::showMode()
{
    String data;
    _cell.println("AT+CWMODE?");  
  unsigned long start;
  start = millis();
    while (millis()-start<2000) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1)
     {
         break;
     }
  }
    if(data.indexOf("1")!=-1)
    {
        return "Station";
    }else if(data.indexOf("2")!=-1)
    {
            return "AP";
    }else if(data.indexOf("3")!=-1)
    {
         return "AP+Station";
    }
}



/*************************************************************************
//configure the operation mode

  a:  
    1 - Station
    2 - AP
    3 - AP+Station
    
  return:
    true  - successfully
    false - unsuccessfully

***************************************************************************/

bool WIFI::confMode(byte a)
{
    String data;
     _cell.print("AT+CWMODE=");  
     _cell.println(String(a));
   unsigned long start;
  start = millis();
    while (millis()-start<2000) {
      if(_cell.available()>0)
      {
      char a =_cell.read();
      data=data+a;
      }
      if (data.indexOf("OK")!=-1 || data.indexOf("no change")!=-1)
      {
          return true;
      }
    if (data.indexOf("ERROR")!=-1 || data.indexOf("busy")!=-1)
    {
      return false;
    }
    
   }
}


/*************************************************************************
//show the list of wifi hotspot
    
  return: string of wifi information
    encryption,SSID,RSSI
    

***************************************************************************/

String WIFI::showAP(void)
{
    String data;
  _cell.flush();
    _cell.print("AT+CWLAP\r\n");  
  delay(1000);
  while(1);
    unsigned long start;
  start = millis();
    while (millis()-start<8000) {
   if(_cell.available()>0)
   {
     char a =_cell.read();
     data=data+a;
   }
     if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
     {
         break;
     }
  }
    if(data.indexOf("ERROR")!=-1)
    {
        return "ERROR";
    }
    else{
       char head[4] = {0x0D,0x0A};   
       char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
       data.replace("AT+CWLAP","");
       data.replace("OK","");
       data.replace("+CWLAP","WIFI");
       data.replace(tail,"");
     data.replace(head,"");

        return data;
        }
 }


/*************************************************************************
//show the name of current wifi access port
    
  return: string of access port name
    AP:<SSID>
    

***************************************************************************/
String WIFI::showJAP(void)
{
  _cell.flush();
    _cell.println("AT+CWJAP?");  
      String data;
    unsigned long start;
  start = millis();
    while (millis()-start<3000) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
       {
           break;
       }
    }
      char head[4] = {0x0D,0x0A};   
      char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
      data.replace("AT+CWJAP?","");
      data.replace("+CWJAP","AP");
      data.replace("OK","");
    data.replace(tail,"");
      data.replace(head,"");
      
          return data;
}


/*************************************************************************
//configure the SSID and password of the access port
    
    return:
    true  - successfully
    false - unsuccessfully
    

***************************************************************************/
boolean WIFI::confJAP(String ssid , String pwd)
{
  
    _cell.print("AT+CWJAP=");
    _cell.print("\"");     //"ssid"
    _cell.print(ssid);
    _cell.print("\"");

    _cell.print(",");

    _cell.print("\"");      //"pwd"
    _cell.print(pwd);
    _cell.println("\"");


    unsigned long start;
  start = millis();
    while (millis()-start<3000) {                            
        if(_cell.find("OK")==true)
        {
       return true;
           
        }
    }
  return false;
}
/*************************************************************************
//quite the access port
    
    return:
      true  - successfully
      false - unsuccessfully
    

***************************************************************************/

boolean WIFI::quitAP(void)
{
    _cell.println("AT+CWQAP");
    unsigned long start;
  start = millis();
    while (millis()-start<3000) {                            
        if(_cell.find("OK")==true)
        {
       return true;
           
        }
    }
  return false;

}

/*************************************************************************
//show the parameter of ssid, password, channel, encryption in AP mode
    
    return:
      mySAP:<SSID>,<password>,<channel>,<encryption>

***************************************************************************/
String WIFI::showSAP()
{
    _cell.println("AT+CWSAP?");  
      String data;
      unsigned long start;
  start = millis();
    while (millis()-start<3000) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
       {
           break;
       }
    }
      char head[4] = {0x0D,0x0A};   
      char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
      data.replace("AT+CWSAP?","");
      data.replace("+CWSAP","mySAP");
      data.replace("OK","");
    data.replace(tail,"");
      data.replace(head,"");
      
          return data;
}

/*************************************************************************
//configure the parameter of ssid, password, channel, encryption in AP mode
    
    return:
      true  - successfully
      false - unsuccessfully

***************************************************************************/

boolean WIFI::confSAP(String ssid , String pwd , byte chl , byte ecn)
{
    _cell.print("AT+CWSAP=");  
    _cell.print("\"");     //"ssid"
    _cell.print(ssid);
    _cell.print("\"");

    _cell.print(",");

    _cell.print("\"");      //"pwd"
    _cell.print(pwd);
    _cell.print("\"");

    _cell.print(",");
    _cell.print(String(chl));

    _cell.print(",");
    _cell.println(String(ecn));
  unsigned long start;
  start = millis();
    while (millis()-start<3000) {                            
        if(_cell.find("OK")==true )
        {
           return true;
        }
     }
   
   return false;

}


/*********************************************
 *********************************************
 *********************************************
             TPC/IP Function Command
 *********************************************
 *********************************************
 *********************************************
 */

/*************************************************************************
//inquire the connection status
    
    return:   string of connection status
      <ID>  0-4
      <type>  tcp or udp
      <addr>  ip
      <port>  port number

***************************************************************************/

String WIFI::showStatus(void)
{
    _cell.println("AT+CIPSTATUS");  
      String data;
    unsigned long start;
  start = millis();
    while (millis()-start<3000) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1)
       {
           break;
       }
    }

          char head[4] = {0x0D,0x0A};   
          char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
          data.replace("AT+CIPSTATUS","");
          data.replace("OK","");
      data.replace(tail,"");
          data.replace(head,"");
          
          return data;
}

/*************************************************************************
//show the current connection mode(sigle or multiple)
    
    return:   string of connection mode
      0 - sigle
      1 - multiple

***************************************************************************/
String WIFI::showMux(void)
{
    String data;
    _cell.println("AT+CIPMUX?");  

      unsigned long start;
  start = millis();
    while (millis()-start<3000) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1)
       {
           break;
       }
    }
          char head[4] = {0x0D,0x0A};   
          char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
          data.replace("AT+CIPMUX?","");
          data.replace("+CIPMUX","showMux");
          data.replace("OK","");
      data.replace(tail,"");
          data.replace(head,"");
          
          return data;
}

/*************************************************************************
//configure the current connection mode(sigle or multiple)
    
    a:    connection mode
      0 - sigle
      1 - multiple
      
  return:
    true  - successfully
    false - unsuccessfully
***************************************************************************/
boolean WIFI::confMux(boolean a)
{
  _cell.print("AT+CIPMUX=");
  _cell.println(a);           
  unsigned long start;
  start = millis();
  while (millis()-start<3000) {                            
        if(_cell.find("OK")==true )
        {
           return true;
        }
     }
   
   return false;
}


/*************************************************************************
//Set up tcp or udp connection  (signle connection mode)

  type: tcp or udp
  
  addr: ip address
  
  port: port number
    

  return:
    true  - successfully
    false - unsuccessfully

***************************************************************************/
boolean WIFI::newMux(byte type, String addr, int port)

{
    String data;
    _cell.print("AT+CIPSTART=");
    if(type>0)
    {
        _cell.print("\"TCP\"");
    }else
    {
        _cell.print("\"UDP\"");
    }
    _cell.print(",");
    _cell.print("\"");
    _cell.print(addr);
    _cell.print("\"");
    _cell.print(",");
//    _cell.print("\"");
    _cell.println(String(port));
//    _cell.println("\"");
    unsigned long start;
  start = millis();
  while (millis()-start<3000) { 
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("ALREAY CONNECT")!=-1 || data.indexOf("ERROR")!=-1)
     {
         return true;
     }
  }
  return false;
}
/*************************************************************************
//Set up tcp or udp connection  (multiple connection mode)

  type: tcp or udp
  
  addr: ip address
  
  port: port number
    
  id: id number(0-4)

  return:
    true  - successfully
    false - unsuccessfully

***************************************************************************/
boolean WIFI::newMux( byte id, byte type, String addr, int port)

{

    _cell.print("AT+CIPSTART=");
    _cell.print("\"");
    _cell.print(String(id));
    _cell.print("\"");
    if(type>0)
    {
        _cell.print("\"TCP\"");
    }
  else
    {
        _cell.print("\"UDP\"");
    }
    _cell.print(",");
    _cell.print("\"");
    _cell.print(addr);
    _cell.print("\"");
    _cell.print(",");
//    _cell.print("\"");
    _cell.println(String(port));
//    _cell.println("\"");
    String data;
    unsigned long start;
  start = millis();
  while (millis()-start<3000) { 
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("ALREAY CONNECT")!=-1 )
     {
         return true;
     }
  }
  return false;
  

}
/*************************************************************************
//send data in sigle connection mode

  str:  string of message

  return:
    true  - successfully
    false - unsuccessfully

***************************************************************************/
boolean WIFI::Send(String str)
{
    _cell.print("AT+CIPSEND=");
//    _cell.print("\"");
    _cell.println(str.length());
//    _cell.println("\"");
    unsigned long start;
  start = millis();
  bool found;
  while (millis()-start<5000) {                            
        if(_cell.find(">")==true )
        {
      found = true;
           break;
        }
     }
   if(found)
    _cell.print(str);
  else
  {
    closeMux();
    return false;
  }


    String data;
    start = millis();
  while (millis()-start<5000) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("SEND OK")!=-1)
     {
         return true;
     }
  }
  return false;
}

boolean WIFI::SendRaw(unsigned char* buff, long len)
{
    _cell.print("AT+CIPSEND=");
//    _cell.print("\"");
    _cell.println(len);
//    _cell.println("\"");
    unsigned long start;
  start = millis();
  bool found;
  while (millis()-start<5000) {                            
        if(_cell.find(">")==true )
        {
      found = true;
           break;
        }
     }
   if(found)
   _cell.write((byte*)buff,len);
   /*
    for (int i = 0;i<len;i++)
      {
      _cell.write(buff[i]);
      delay(1);
      }*/
  else
  {
    closeMux();
    return false;
  }


    String data;
    start = millis();
  while (millis()-start<5000) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("SEND OK")!=-1)
     {
         return true;
     }
  }
  return false;
}

/*************************************************************************
//send data in multiple connection mode

  id:   <id>(0-4)
  
  str:  string of message

  return:
    true  - successfully
    false - unsuccessfully

***************************************************************************/
boolean WIFI::Send(byte id, String str)
{
    _cell.print("AT+CIPSEND=");

    _cell.print(String(id));
    _cell.print(",");
    _cell.println(str.length());
    unsigned long start;
  start = millis();
  bool found;
  while (millis()-start<5000) {                          
        if(_cell.find(">")==true )
        {
      found = true;
           break;
        }
     }
   if(found)
    _cell.print(str);
  else
  {
    closeMux(id);
    return false;
  }


    String data;
    start = millis();
  while (millis()-start<5000) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("SEND OK")!=-1)
     {
         return true;
     }
  }
  return false;
}

/*************************************************************************
//Close up tcp or udp connection  (sigle connection mode)


***************************************************************************/
void WIFI::closeMux(void)
{
    _cell.println("AT+CIPCLOSE");

    String data;
    unsigned long start;
  start = millis();
  while (millis()-start<3000) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("Linked")!=-1 || data.indexOf("ERROR")!=-1 || data.indexOf("we must restart")!=-1)
     {
         break;
     }
  }
}


/*************************************************************************
//Set up tcp or udp connection  (multiple connection mode)
    
  id: id number(0-4)

***************************************************************************/
void WIFI::closeMux(byte id)
{
    _cell.print("AT+CIPCLOSE=");
    _cell.println(String(id));
    String data;
    unsigned long start;
  start = millis();
  while (millis()-start<3000) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("Link is not")!=-1 || data.indexOf("Cant close")!=-1)
     {
         break;
     }
  }

}

/*************************************************************************
//show the current ip address
    
  return: string of ip address

***************************************************************************/
String WIFI::showIP(void)
{
    String data;
    unsigned long start;
    //DBG("AT+CIFSR\r\n");
  for(int a=0; a<3;a++)
  {
  _cell.println("AT+CIFSR");  
  start = millis();
  while (millis()-start<3000) {
     while(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("AT+CIFSR")!=-1)
     {
         break;
     }
  }
  if(data.indexOf(".") != -1)
  {
    break;
  }
  data = "";
  }
  //DBG(data);
  //DBG("\r\n");
    char head[4] = {0x0D,0x0A};   
    char tail[7] = {0x0D,0x0D,0x0A};        
    data.replace("AT+CIFSR","");
    data.replace(tail,"");
    data.replace(head,"");
  
    return data;
}

/*************************************************************************
////set the parameter of server

  mode:
    0 - close server mode
    1 - open server mode
    
  port: <port>
    
  return:
    true  - successfully
    false - unsuccessfully

***************************************************************************/

boolean WIFI::confServer(byte mode, int port)
{
    _cell.print("AT+CIPSERVER=");  
    _cell.print(String(mode));
    _cell.print(",");
    _cell.println(String(port));

    String data;
    unsigned long start;
  start = millis();
  boolean found = false;
  while (millis()-start<3000) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("no charge")!=-1)
     {
    found = true;
         break;
     }
  }
  return found;
}


#endif


/*
ESP8266 library
When you use with UNO board, uncomment the follow line in uartWIFI.h.
#define UNO
When you use with MEGA board, uncomment the follow line in uartWIFI.h.
#define MEGA
Connection:
When you use it with UNO board, the connection should be like these:
ESP8266_TX->D0
ESP8266_RX->D1
ESP8266_CH_PD->3.3V
ESP8266_VCC->3.3V
ESP8266_GND->GND
FTDI_RX->D3      //The baud rate of software serial can't be higher that 19200, so we use software serial as a debug port
FTDI_TX->D2
When you use it with MEGA board, the connection should be like these:
ESP8266_TX->RX1(D19)
ESP8266_RX->TX1(D18)
ESP8266_CH_PD->3.3V
ESP8266_VCC->3.3V
ESP8266_GND->GND
When you want to output the debug information, please use DebugSerial. For example,
DebugSerial.println("hello");
Note: The size of message from ESP8266 is too big for arduino sometimes, so the library can't receive the whole buffer because  
the size of the hardware serial buffer which is defined in HardwareSerial.h is too small.
Open the file from \arduino\hardware\arduino\avr\cores\arduino\HardwareSerial.h.
See the follow line in the HardwareSerial.h file.
#define SERIAL_BUFFER_SIZE 64
The default size of the buffer is 64. Change it into a bigger number, like 256 or more.
*/

////////////////////////////// MAIN CODE BEGIN////// MAIN CODE BEGIN////// MAIN CODE BEGIN////// MAIN CODE BEGIN////// MAIN CODE BEGIN////// MAIN CODE BEGIN

//#include "uartWIFI.h"
#include <SoftwareSerial.h>
WIFI wifi;

typedef struct
{
  unsigned long arrbytes[15]; // [0]=0x1b , zeros... NTP req ver3
} TSendBufferData;
TSendBufferData SendBufferData; // [0]=0x1b , zeros... NTP req ver3

typedef struct
{
//uint8_t unknowbyte; // defacto

uint32_t antialign;
  //uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
                           // li.   Two bits.   Leap indicator.
                           // vn.   Three bits. Version number of the protocol.
                           // mode. Three bits. Client will pick mode 3 for client.

  //uint8_t stratum;         // Eight bits. Stratum level of the local clock.
  //uint8_t poll;            // Eight bits. Maximum interval between successive messages.
  //uint8_t precision;       // Eight bits. Precision of the local clock.

  uint32_t rootDelay;      // 32 bits. Total round trip delay time.
  uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
  uint32_t refId;          // 32 bits. Reference clock identifier.

  uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
  uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

  uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
  uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

  uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
  uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

  uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
  uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.


uint32_t bulk[40/4];

} Trx_time1;              // Total: 384 bits or 48 bytes.  ver3?

Trx_time1 rx_time1;


void outmaindata(int n)
{
  String S;
  S = "--"+String(n);
  DebugSerial.println(S);
}

uint32_t my_ntohl(uint32_t netlong) // from https://codereview.stackexchange.com/questions/149717/implementation-of-c-standard-library-function-ntohl 
{
    union {
        uint16_t num;
        uint8_t bytes[2];
    } endian_test = { .bytes = { 0x01, 0x00 }};

    if (endian_test.num == 0x0001) {
        netlong = (netlong << 24) | ((netlong & 0xFF00ul) << 8) |
            ((netlong & 0xFF0000ul) >> 8) | (netlong >> 24);
    }

    return netlong;
}

struct tm rtc_time;
int outdata = 0;
void setup()
{
  
  memset(&SendBufferData,0,sizeof(SendBufferData));
  SendBufferData.arrbytes[0] = 0x1b; // [0]=0x1b , zeros... NTP req ver3  // from https://github.com/ru-kernelnewbies-org/Network/blob/master/sockets/UDP/ntp_client.c 
  
  Serial.begin(115200);
  Serial1.begin(115200);
  
  delay(1000);
  data_to_indicator(8,8,1,8,8);
  outmaindata(outdata++);
  delay(1000);
  DebugSerial.println("1..");
  outmaindata(outdata++);
  delay(1000);
  DebugSerial.println("1..");
  outmaindata(outdata++);
  //wifi.begin(&Serial1, &Serial, PIN_RESET);
  wifi.begin();
  outmaindata(outdata++);
  bool b = wifi.Initialize(STA, SSID, PASSWORD);
  outmaindata(outdata++);
  if(!b)
  {
    DebugSerial.println("Init error");
  }
  delay(8000);  //make sure the module can have enough time to get an IP address 
  outmaindata(outdata++);
  String ipstring  = wifi.showIP();
  outmaindata(outdata++);
  DebugSerial.println(ipstring);    //show the ip address of module
  
  //delay(5000);
  wifi.ipConfig(UDP, "192.168.0.1", 123);  //Connect to your time server (in your router)
  outmaindata(outdata++);
  
  DebugSerial.println("setup done..");
  wifi.SendRaw((unsigned char*)&SendBufferData,sizeof(SendBufferData));
  outmaindata(outdata++);
  //pinMode(13,OUTPUT);
  for (int i = 0; i< 29; i++)
  {
    pinMode(pin2segs[i],OUTPUT);
  }

  outmaindata(outdata++);


  outdata = 200;

  
  
    
}

#define NTP_TIMESTAMP_DELTA (2208988800ull) // from https://github.com/ru-kernelnewbies-org/Network/blob/master/sockets/UDP/ntp_client.c 
unsigned long previousMillis = 0;
unsigned long sec1previousMillis = 0;
unsigned long long cpusecondx1000 = 0;


  unsigned long currentMillis=0;
  unsigned long lastMillis = 0;


void timestep(void)
{

        currentMillis = millis();
      if (currentMillis > lastMillis)
        {
          cpusecondx1000 = cpusecondx1000 + (unsigned long long)(currentMillis-lastMillis); // time inc
        }
      lastMillis = currentMillis;


}


long lastSecond = 30;
char tick_for_indicator = 0;

void loop()
{

  //char buf[100];
  int iLen = wifi.ReceiveMessage((unsigned char*)&rx_time1,sizeof(rx_time1));
  if (iLen>0) outmaindata(iLen);
  if(iLen >= 48)
    {
      unsigned long txTm_s_32;
      txTm_s_32 = rx_time1.txTm_s;
      //ntohl(&(txTm_s_32)); 
      //DebugSerial.println(txTm_s_32); 
      txTm_s_32 = my_ntohl(txTm_s_32);
      //unsigned long long test64;
      //test64 = txTm_s_32;
      //test64 = test64 - NTP_TIMESTAMP_DELTA;
      //String sctime = ctime((long unsigned int*) &test64);  
      //rtc_time = txTm_s_32;
      //DebugSerial.println(txTm_s_32);  
      txTm_s_32 = txTm_s_32 - NTP_TIMESTAMP_DELTA; // from https://github.com/ru-kernelnewbies-org/Network/blob/master/sockets/UDP/ntp_client.c 


      long long diff = (long long)txTm_s_32*(long long)1000 - (long long)cpusecondx1000;
      if (abs(diff)>10000)
        cpusecondx1000 = (long long)1000*(long long)txTm_s_32;
      else
        cpusecondx1000 = (long long)cpusecondx1000 + (long long)diff/(long long)1000;

      tick_for_indicator=1;
      
      long tmSecond = txTm_s_32 % 60;  // from https://github.com/PaulStoffregen/Time/blob/master/Time.cpp  
      txTm_s_32 /= 60; // now it is minutes 
      long tmMinute = txTm_s_32 % 60;
      txTm_s_32 /= 60; // now it is hours
      long tmHour = txTm_s_32 % 24 + 3; // 3 is msk

              
      String sctime = String(tmHour)+":"+String(tmMinute)+":"+String(tmSecond);       
      DebugSerial.println(sctime);  
      iLen = 0;
    }
   
  timestep();


  if ((currentMillis - previousMillis >= 15000)&&(lastSecond > 10)&&(lastSecond<50)) // work around wifi and esp lags 
  {
    previousMillis = currentMillis;
  wifi.SendRaw((unsigned char*)&SendBufferData,sizeof(SendBufferData));
  //outmaindata(outdata++);
  timestep();
  }

  if (currentMillis - sec1previousMillis >= 1000) 
  {
    sec1previousMillis = currentMillis;

    long cpusecondx1 = (unsigned long long)cpusecondx1000/(unsigned long long)1000;

      long tmSecond = cpusecondx1 % 60;  // from https://github.com/PaulStoffregen/Time/blob/master/Time.cpp  
      lastSecond = tmSecond;
      cpusecondx1 /= 60; // now it is minutes 
      long tmMinute = cpusecondx1 % 60;
      cpusecondx1 /= 60; // now it is hours
      long tmHour = cpusecondx1 % 24 + 3; // 3 is msk

      tmHour = tmHour % 24; // 0..23 only 

      static char tick_for_indicator2=0;
      if (tick_for_indicator) tick_for_indicator2 = (tick_for_indicator2 + 1) % 2;
      data_to_indicator(tmHour/10,tmHour%10,tick_for_indicator2,tmMinute/10,tmMinute%10);
      tick_for_indicator = 0;
      String sctime = String(tmHour/10)+String(tmHour%10)+":"+String(tmMinute/10)+String(tmMinute%10)+":"+String(tmSecond/10)+String(tmSecond%10);       
      DebugSerial.println(">"+sctime);  
    
  }



 
  
//      digitalWrite(13, HIGH);
//      digitalWrite(13, LOW);
//      outmaindata(outdata++);
}

void toportpins(long allsegs);

unsigned short dig2segments(unsigned short dig);

void data_to_indicator(unsigned short h1,unsigned short h2,unsigned short t,unsigned short m1,unsigned short m2)
{
  static unsigned short h1segs;
  static unsigned short h2segs;
  static unsigned short m1segs;
  static unsigned short m2segs;
  static unsigned short dotseg;
  static long allsegs = 0;
  if (h1 == 0) h1 = 10; // clean x0:00
  h1segs = dig2segments(h1);
  h2segs = dig2segments(h2);
  m1segs = dig2segments(m1);
  m2segs = dig2segments(m2);  
  if (t) dotseg = 1; else dotseg = 0;
  allsegs = (long)h1segs + (((long)h2segs)<<7) + (((long)dotseg)<<(7+7)) + (((long)m1segs)<<(7+7+1)) + (((long)m2segs)<<(7+7+1+7)); // all = 29 signals
  //DebugSerial.println(allsegs, BIN); 
  //DebugSerial.println(allsegs); 
  toportpins(allsegs);
}

unsigned short segmbits(unsigned short a,unsigned short b,unsigned short c,unsigned short d,unsigned short e,unsigned short f,unsigned short g)
{
  return ((unsigned short)(a + (b<<1) + (c<<2) + (d<<3) + (e<<4) + (f<<5) + (g<<6)));
}

unsigned short dig2segments(unsigned short dig)
{
  // abcdefg = 1111111 demux
  //-a-
  //f b
  //-g-
  //e c
  //-d-
  switch(dig)
  {
    case 0: return(segmbits(1,1,1,1,1,1,0));
    case 1: return(segmbits(0,1,1,0,0,0,0));
    case 2: return(segmbits(1,1,0,1,1,0,1));
    case 3: return(segmbits(1,1,1,1,0,0,1));
    case 4: return(segmbits(0,1,1,0,0,1,1));
    case 5: return(segmbits(1,0,1,1,0,1,1));
    case 6: return(segmbits(0,0,1,1,1,1,1));
    case 7: return(segmbits(1,1,1,0,0,0,0));
    case 8: return(segmbits(1,1,1,1,1,1,1));
    case 9: return(segmbits(1,1,1,1,0,1,1));
    case 10:return(segmbits(0,0,0,0,0,0,0));
    default:return(segmbits(0,1,1,0,1,1,1));
  }
  return -1;
}

short pin2segs[29] = {
53,
52,
51,
50,
49,
48,
47,
46,
45,
44,
43,
42,
41,
40,
39,
38,
37,
36,
35,
34,
33,
32,
31,
30,
29,
28,
27,
26,
24
};
long allsegs_old =0;
long allsegs_changed = 0xFFFFFFFF;
long test_port_cnt = 0;
// #define test_port_cnt_deff 1
void toportpins(long allsegs)
{// all = 29 signals
long mask = 1;
test_port_cnt++;
for (int i = 0; i< 29; i++)
  {
        #ifndef test_port_cnt_deff
    if (allsegs_changed & mask)
      {
        if (allsegs & mask)
          {
            digitalWrite(pin2segs[i], HIGH);
          }
        else
          {
            digitalWrite(pin2segs[i], LOW);
          }
      }
        #else
          if ((test_port_cnt/2) % 2)
            {
            digitalWrite(pin2segs[i], HIGH);
            }
          else
            {
            digitalWrite(pin2segs[i], LOW);
            }
        #endif
    mask = mask << 1;
  }
allsegs_changed = allsegs ^ allsegs_old;
allsegs_old = allsegs; 
//      digitalWrite(13, HIGH);
//      digitalWrite(13, LOW);
}
