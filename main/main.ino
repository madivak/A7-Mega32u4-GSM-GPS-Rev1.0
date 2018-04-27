#define DEBUG true

char byteGPS=-1;
char linea[150] = "";
char comandoRMC[7] = "$GPRMC";
char comandoSRD[7] = "+GPSRD";
int cont=0;
int bien=0;
int bien1=0;
int conta=0;
int indices[13];
int y=0;
int x=0;

int GPS_time=200;                        // When the board gets the 30 times location information successfully and the location information will be send by sms.      
String target_phone = "+2547XXXXXXXX"; // Your phone number,be careful need to add a country code before the cellphone number

String RMC="";
String SRD="";
int GPS_position_count=0;
int datacount=0;
int datacount1=0;
void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(8,OUTPUT);
  digitalWrite(5, HIGH); 
  digitalWrite(4, LOW); 
  digitalWrite(8, LOW); 
  delay(2000);
  digitalWrite(8, HIGH);  //PWRKEY held high >2Sec to boot A7
  delay(3000);       
  digitalWrite(8, LOW);
  Serial.println("A7 Power ON!");
  sendData("AT+GPS=0",3000,DEBUG);     //Close GPS
  for(int i=0;i<2;i++){                //Make sure the GPS has been turned on
    sendData("AT+GPSRD=1",1000,DEBUG);
    Serial1.println("AT+GPS=1");
  }
  Serial.println("*********************************************************");
  Serial.println("**If don`t display 'GPS positioning....',please reboot.**");
  Serial.println("*********************************************************");
}

void loop()
{
    testgps();
    
}

void testgps()
{
  while(Serial1.available())
  {
     byteGPS=Serial1.read();  
    // Read a byte of the serial port
    if (byteGPS == -1) 
     {  /* See if the port is empty yet*/ } 
    else 
    {
     // note: there is a potential buffer overflow here!
     linea[conta]=byteGPS;        // If there is serial port data, it is put in the buffer
     conta++;
     datacount++;
     datacount1++;
                          
     Serial.print(byteGPS);    //If you delete '//', you will get the all GPS information
              
     if (byteGPS==13)
     {
      // If the received byte is = to 13, end of transmission
      // note: the actual end of transmission is <CR><LF> (i.e. 0x13 0x10)
      cont=0;
      bien=0;
      bien1=0;
      // The following for loop starts at 1, because this code is clowny and the first byte is the <LF> (0x10) from the previous transmission.
       for (int i=1;i<7;i++)     // Verifies if the received command starts with $GPR
       {
          if (linea[i]==comandoSRD[i-1])
            {bien++;}
       }
       for (int i=1;i<7;i++)     // Verifies if the received command starts with $GPR
       {
          if (linea[i]==comandoRMC[i-1])
            {bien1++;}
       }
       
   //-----------------------------------------------------------------------       
       if(bien==6) // If initial characters match "+GPSRD" string, process the data
       { 

          for (int i=8;i<datacount;i++) // i=8 is to remove the preceeding +GPSRD from GGA string
           {
              SRD+=linea[i];                   
           }
           y=1;
           Serial.print("\r\nGPSRD character count is: ");
           Serial.println(SRD.length());
       }
       if(bien1==6) // If initial characters match "+GPSRD" string, process the data
       { 
   //-----------------------------------------------------------------------
          for (int i=1;i<datacount1;i++)
           {
              RMC+=linea[i];                   
           }
           x=1;
           Serial.print("\r\nGPRMC character count is: ");
           Serial.println(RMC.length());
       }
       int z = x+y;
       if (z==2)      
       {
          sendData("AT+GPSRD=0",2000,DEBUG);
          delay(500);
          TCP_GPRS(SRD,RMC);
          RMC="";
          SRD="";
          x=0;
          y=0;
          sendData("AT+GPSRD=1",2000,DEBUG);         
       }
       else{}
       //-----------------------------------------------------------------------
       conta=0;
       datacount=0;
       datacount1=0;
       // Reset the buffer  
       for (int i=0;i<150;i++)
       {    //  
        linea[i]=' ';             
       }           
     } //byteGPS==13
   }  //else byteGPS is not null
  } //Serial1.available
} //testGPS


void sendData(String command, const int timeout, boolean debug)
{
    String response = "";    
    Serial1.println(command); 
    long int time = millis();   
    while( (time+timeout) > millis()){
      while(Serial1.available()){       
        response += (char)Serial1.read(); 
      }  
    }    
    if(debug){
      Serial.print(response);
    }    
}

 void TCP_GPRS(String GPS0, String GPS1)
{
  sendData("AT",2000,DEBUG); 
  sendData("AT+CGATT=1",2000,DEBUG);
  sendData("AT+CIPMUX=0",2000,DEBUG);
  sendData("AT+CSTT=\"APN\",\"\",\"\"",3000,DEBUG);
  sendData("AT+CIICR",3000,DEBUG); 
  sendData("AT+CIFSR",3000,DEBUG); 
  sendData("AT+CIPSTART=\"TCP\",IP,PORT",3000,DEBUG);
  sendData("AT+CIPSEND",2000,DEBUG);
  sendData("#CAR PLATE NO:[KCQ 450R]\r\r\n"+GPS0+"\r\n"+GPS1+"\r\nBUFF[13] = 1\r\r\nCar status is = 1\r\r\nText no# is :07XXXXXXXX\r",2000,DEBUG);
  delay(100);
  Serial1.println((char)26);                  //the ASCII code of the ctrl+z is 26
  delay(1000); 
  
  sendData("AT+CIPCLOSE",2000,DEBUG);
  sendData("AT+CIPSHUT",2000,DEBUG);
}

