// Basic Arduino & ESP8266 webserver
//
// uses AltSoftSerial download from https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
// this can be replaced with the normal software serial
 
 
#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
// Arduino pin 08 for RX
// Arduino Pin 09 for TX
 
SoftwareSerial espSerial(4,5);
 
 
const bool printReply = true;
const char line[] = "-----\n\r";
int loopCount=0;
 
char html[50];
char command[20];
char reply[500]; // you wouldn't normally do this
 
char ipAddress [20];
char name[30];
int lenHtml = 0;
char temp[5];
 
 
void setup()
{
      Serial.begin(115200);
      Serial.println("Start\r\n\r\n");
      

 
      espSerial.begin(115200); // your ESP8266 module's baud rate might be different
 
 
      // reset the ESP8266
      Serial.println("reset the module"); 
      espSerial.print("AT+RST\r\n");
      getReply( 2000 );

      espSerial.print("AT+GMR");
      getReply( 2000 );
  
      // configure as a station
      Serial.println("Change to station mode"); 
      espSerial.print("AT+CWMODE=1\r\n");
      getReply( 1500 );
 
 
      // connect to the network. Uses DHCP. ip will be assigned by the router.
      Serial.println("Connect to a network ");
 
      // Enter the SSID and password for your own network
      espSerial.print("AT+CWJAP=\"SSIS_Name\",\"PW\"\r\n");
      getReply( 6000 );
 
 
      // get ip address
      Serial.println("Get the ip address assigned ny the router"); 
      espSerial.print("AT+CIFSR\r\n");
      getReply( 1000 );
 
 
      // parse ip address.
      int len = strlen( reply ); 
      bool done=false;
      bool error = false;
      int pos = 0;
      while (!done)
      {
           if ( reply[pos] == 10) { done = true;} 
           pos++;
           if (pos > len) { done = true;  error = true;}
      }
 
      if (!error)
      {
            int buffpos = 0;
            done = false;
            while (!done)
            {
               if ( reply[pos] == 13 ) { done = true; }
               else { ipAddress[buffpos] = reply[pos];    buffpos++; pos++;   }
            }
            ipAddress[buffpos] = 0;
      }
      else { strcpy(ipAddress,"ERROR"); }
 
 
 
      // configure for multiple connections
      Serial.println("Set for multiple connections"); 
      espSerial.print("AT+CIPMUX=1\r\n");
      getReply( 1500 );
 
 
      // start server on port 80
      Serial.println("Start the server"); 
      espSerial.print("AT+CIPSERVER=1,80\r\n");
      getReply( 1500 );
 
      Serial.println("");
 
 
      Serial.println("Waiting for page request");
      Serial.print("Connect to "); Serial.println(ipAddress);
      Serial.println("");
}
 
 
void loop()
{
      if(espSerial.available()) // check if the ESP8266 is sending data
      {
          // this is the +IPD reply - it is quite long. 
          // normally you would not need to copy the whole message in to a variable you can copy up to "HOST" only
          // or you can just search the data character by character as you read the serial port.
          getReply( 2000 );      
 
 
          bool foundIPD = false;
          for (int i=0; i<strlen(reply); i++)
          {
               if (  (reply[i]=='I') && (reply[i+1]=='P') && (reply[i+2]=='D')   ) { foundIPD = true;    }
          }
 
 
          if ( foundIPD  )  
          {
 
              loopCount++;
              // Serial.print( "Have a request.  Loop = ");  Serial.println(loopCount); Serial.println(""); 
 
 
              // check to see if we have a name - look for name=
              bool haveName = false;
              int nameStartPos = 0;
              for (int i=0; i<strlen(reply); i++)
              {
                   if (!haveName) // just get the first occurrence of name
                   {
                         if (  (reply[i]=='n') && (reply[i+1]=='a') && (reply[i+2]=='m') && (reply[i+3]=='e')  && (reply[i+4]=='=') ) 
                         { 
                             haveName = true;
                             nameStartPos = i+5;
                         }
                   }     
              }
 
              // get the name - copy everything from nameStartPos to the first space character
              if (haveName)
              {
                    int tempPos = 0;
                    bool finishedCopying = false;
                    for (int i=nameStartPos; i<strlen(reply); i++)
                    {
                         if ( (reply[i]==' ') && !finishedCopying )  { finishedCopying = true;   } 
                         if ( !finishedCopying )                     { name[tempPos] = reply[i];   tempPos++; }
                    }              
                    name[tempPos] = 0;
              }
 
              if (haveName) { Serial.print( "name = ");  Serial.println(name); Serial.println(""); }
              else          { Serial.println( "no name entered");   Serial.println("");           }
 
 
 
              // start sending the HTML
 
 
              strcpy(html,"<html><head></head><body>");
              strcpy(command,"AT+CIPSEND=0,25\r\n");
              espSerial.print(command);
              getReply( 2000 );          
              espSerial.print(html);
              getReply( 2000 );                      
 
              strcpy(html,"<h1>ESP8266 Webserver</h1>");
              strcpy(command,"AT+CIPSEND=0,26\r\n");
              espSerial.print(command);
              getReply( 2000 );         
              espSerial.print(html);
              getReply( 2000 );          
 
              strcpy(html,"<p>Served by Arduino and ESP8266</p>");
              strcpy(command,"AT+CIPSEND=0,36\r\n");
              espSerial.print(command);
              getReply( 2000 );          
              espSerial.print(html);
              getReply( 2000 );           
 
              strcpy(html,"<p>Request number ");
              itoa( loopCount, temp, 10);
              strcat(html,temp);
              strcat(html,"</p>");
 
              // need the length of html
              int lenHtml = strlen( html );
 
              strcpy(command,"AT+CIPSEND=0,");
              itoa( lenHtml, temp, 10);
              strcat(command, temp);
              strcat(command, "\r\n");
              espSerial.print(command);
              getReply( 2000 );          
              espSerial.print(html);
              getReply( 2000 );                       
 
 
 
             if (haveName)
             {
                  // write name
                  strcpy(html,"<p>Your name is "); strcat(html, name ); strcat(html,"</p>");
 
                  // need the length of html for the cipsend command
                  lenHtml = strlen( html );
                  strcpy(command,"AT+CIPSEND=0,"); itoa( lenHtml, temp, 10); strcat(command, temp); strcat(command, "\r\n");
                  espSerial.print(command);
                  getReply( 2000 );          
                  espSerial.print(html);
                  getReply( 2000 );                           
             }
 
 
              strcpy(html,"<form action=\""); strcat(html, ipAddress); strcat(html, "\" method=\"GET\">"); strcat(command, "\r\n");
 
              lenHtml = strlen( html );
              itoa( lenHtml, temp, 10);
              strcpy(command,"AT+CIPSEND=0,"); 
              itoa( lenHtml, temp, 10); 
              strcat(command, temp);  
              strcat(command, "\r\n");
 
              espSerial.print(command);
              getReply( 2000 );          
              espSerial.print(html);
              getReply( 2000 );          
 
              strcpy(html,"Name:<br><input type=\"text\" name=\"name\">");
              strcpy(command,"AT+CIPSEND=0,40\r\n");
              espSerial.print(command);
              getReply( 2000 );         
              espSerial.print(html);
              getReply( 2000 );         
 
              strcpy(html,"<input type=\"submit\" value=\"Submit\"></form>");
              strcpy(command,"AT+CIPSEND=0,43\r\n");
              espSerial.print(command);
              getReply( 2000 );       
              espSerial.print(html);
              getReply( 2000 );       
 
              strcpy(html,"</body></html>");
              strcpy(command,"AT+CIPSEND=0,14\r\n");
              espSerial.print(command);
              getReply( 2000 ); 
              espSerial.print(html);
              getReply( 2000 ); 
 
              // close the connection
              espSerial.print( "AT+CIPCLOSE=0\r\n" );
              getReply( 1500 );            
 
 
              Serial.println("last getReply 1 ");
              getReply( 1500 );  
 
              Serial.println("last getReply 2 ");
              getReply( 1500 ); 
 
 
 
          } // if(espSerial.find("+IPD"))
      } //if(espSerial.available())
 
 
      delay (100);
 
      // drop to here and wait for next request.
 
}
 
 
void getReply(int wait)
{
    int tempPos = 0;
    long int time = millis();
    while( (time + wait) > millis())
    {
        while(espSerial.available())
        {
            char c = espSerial.read(); 
            if (tempPos < 500) { reply[tempPos] = c; tempPos++;   }
        }
        reply[tempPos] = 0;
    } 
 
    if (printReply) { Serial.println( reply );  Serial.println(line);     }
}

