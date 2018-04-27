
#define DEBUG true

char byteGPS=-1;
char linea[150] = "";
//char comandoGPR[7] = "$GPRMC";
char comandoGPR[7] = "+GPSRD";
int cont=0;
int bien=0;
int conta=0;
int indices[13];

int GPS_time=200;                        // When the board gets the 30 times location information successfully and the location information will be send by sms.      
String target_phone = "+2547XXXXXXXX"; // Your phone number,be careful need to add a country code before the cellphone number

//String GPS_position="";
String RMC="";
int GPS_position_count=0;
int datacount=0;
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
     {       
    // See if the port is empty yet
      } 
    else 
    {
     // note: there is a potential buffer overflow here!
     linea[conta]=byteGPS;        // If there is serial port data, it is put in the buffer
     conta++;
     datacount++;
                          
     Serial.print(byteGPS);    //If you delete '//', you will get the all GPS information
     if (byteGPS==13){
      // If the received byte is = to 13, end of transmission
      // note: the actual end of transmission is <CR><LF> (i.e. 0x13 0x10)
      cont=0;
      bien=0;
      // The following for loop starts at 1, because this code is clowny and the first byte is the <LF> (0x10) from the previous transmission.
       for (int i=1;i<7;i++)     // Verifies if the received command starts with $GPR
       {
          if (linea[i]==comandoGPR[i-1]){
           bien++;
         }
       }
       if(bien==6)
       { 
        
        // If yes, continue and process the data
        //Data Partitioning
   //-----------------------------------------------------------------------
          
          for (int i=1;i<datacount;i++)
           {
              RMC+=linea[i];                   
           }
          sendData("AT+GPSRD=0",2000,DEBUG);
          Serial.print("GPRMC character count is: ");
          Serial.println(datacount);
          delay(500);
          TCP_GPRS(RMC);
  //-----------------------------------------------------------------------          
          GPS_position_count++;
          if(GPS_position_count==GPS_time)
            {
              GPS_position_count=0;          //Reset count
              SendTextMessage(RMC); 
            }
  //-----------------------------------------------------------------------  
          RMC="";
          sendData("AT+GPSRD=1",2000,DEBUG);         
       }
       //GPS_position="";
       conta=0;
       datacount=0;
       // Reset the buffer  
       for (int i=0;i<150;i++)
       {    //  
        linea[i]=' ';             
       }           
     }
   }  
  }
}

void SendTextMessage(String message)
{ 
  sendData("AT+CMGF=1",5000,DEBUG);            //Set the SMS in text mode
  delay(100);
  sendData("AT+CMGS="+target_phone,2000,DEBUG);//send sms message to the cellphone , be careful need to add a country code before the cellphone number
  delay(100);
  sendData("#CAR PLATE NO:[KCQ 450R]\r\n$GPGGA,173700.000,,S,,E,1,03,3.2,1586.7,M,,M,,0000*63"+message,2000,DEBUG);
  delay(100);
  Serial1.println((char)26);                  //the ASCII code of the ctrl+z is 26
  delay(1000);
  sendData("",2000,DEBUG);                     //Clear serial data
  delay(100);
}

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

void checkbuffer(const int timeout)
 {
    long int time = millis();
    while( (time+timeout) > millis())
    {
      while(Serial1.available())
      {       
        Serial.write(Serial1.read());
      }  
    }
 }

 void TCP_GPRS(String GPS)
{
  
  sendData("AT",2000,DEBUG); 
  sendData("AT+CGATT=1",2000,DEBUG);
  sendData("AT+CIPMUX=0",2000,DEBUG);
  sendData("AT+CSTT=\"APN\",\"\",\"\"",3000,DEBUG);
  sendData("AT+CIICR",3000,DEBUG); 
  sendData("AT+CIFSR",3000,DEBUG); 
  sendData("AT+CIPSTART=\"TCP\",IP,PORT",3000,DEBUG);
  sendData("AT+CIPSEND",2000,DEBUG);
  sendData("#CAR PLATE NO:[KCQ 450R]\r\n$GPGGA,173700.000,,S,,E,1,03,3.2,1586.7,M,,M,,0000*63\r\n"+GPS+"\r\nBUFF[13] = 1\r\nCar status is = 1\r\nText no# is :07XXXXXXXX",2000,DEBUG);
  delay(100);
  Serial1.println((char)26);                  //the ASCII code of the ctrl+z is 26
  delay(1000); 
  
  sendData("AT+CIPCLOSE",2000,DEBUG);
  sendData("AT+CIPSHUT",2000,DEBUG);
}
