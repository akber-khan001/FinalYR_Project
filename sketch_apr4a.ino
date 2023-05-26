#include<SoftwareSerial.h>
int sensor;
int data;
int Light_status=10;
int threshold=70;

SoftwareSerial esp8266(3,4);

#define SSID "Rayaan"
#define PASS "12345678"

String sendAT(String command,const int timeout){
  String response="";
  esp8266.print(command);
  long int time = millis();
  while((time + timeout)>millis()){
    while(esp8266.available()){
      char c = esp8266.read();
      response += c;
    }
  }
  Serial.print(response);
  return response;
}

void connectwifi(){
  sendAT("AT\r\n",1000);
  sendAT("AT+CWMODE=1\r\n",1000);
  sendAT("AT+CWJAP=\""SSID"\",\""PASS"\"\r\n",2000);
  while(!esp8266.find("OK")){
    //wait for connection
  }
  sendAT("AT+CIFSR\r\n",1000);
  sendAT("AT+CIPMUX=0\r\n",1000);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  esp8266.begin(9600);
  sendAT("AT+RST\r\n",2000);
  connectwifi();
  pinMode(8,OUTPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
  sensor = analogRead(A0);
  data = map(sensor,0,1023,0,100);

  if(data<threshold){
    digitalWrite(8,HIGH);
    Light_status = 100;
  }  
  else{
    digitalWrite(8,LOW);
    Light_status=10;
  }
  String sensor_value = String(data);
  Serial.print("Light intensity:");
  Serial.println(data);

  String Light_status1 = String(Light_status);
  Serial.print("Light_status");
  Serial.println(Light_status);

  String threshold1 = String(threshold);
  Serial.print("Threshold:");
  Serial.println(threshold);

  updateTS(sensor_value,Light_status1,threshold1);
  delay(3000);
}

void updateTS(String S,String L,String T){
  Serial.println("");
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n",1000);
  delay(2000);
  String cmdlen;
  String cmd = "GET /update?key=OKO770XKEEPOQK58&field1="+L+"&field2="+S+"&field3="+T+"\r\n";
  cmdlen= cmd.length();
  sendAT("AT+CIPSEND="+cmdlen+"\r\n",2000);
  esp8266.print(cmd);
  Serial.print("");
  sendAT("AT+CIPCLOSER\r\n",2000);
  Serial.println("");
  delay(1000);

}
