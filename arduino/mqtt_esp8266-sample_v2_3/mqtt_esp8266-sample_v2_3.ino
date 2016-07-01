/*
 esp8266 send to Matrix LED. Weather Code(UTF-8)
 DHT11 value, http send
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 4     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "";
const char* password = "";

const char* host = "api.thingspeak.com";
String mAPI_KEY="";
const char* mClient_id = "cliennt-arduino-0630B";
byte localserver[] = {192, 168, 10, 116 };
//const char* mqtt_server = "test.mosquitto.org";
char mTopicIn[]="item-kuc-arc-f/device-1/matrix_sample_v2";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
uint32_t mTimerTmp;
uint32_t mTimerPost;
char msg[50];
int value = 0;
int mMaxMatrix=20;
String mWDAT="";
String mHtypOne="01";
String mHtypScr="02";

int mSTAT=0;
int mSTAT_1_WDAT=1;
int mSTAT_2_Temp=2;
int mSTAT_3_WDAT=3;
int mSTAT_4_Hum =4;

//
void proc_http(String sTemp, String sHum){
//Serial.print("connecting to ");
//Serial.println(host);  
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      String url = "/update?key="+ mAPI_KEY + "&field1="+ sTemp +"&field2=" + sHum;        
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" + 
        "Connection: close\r\n\r\n");
      delay(10);      
      int iSt=0;
      while(client.available()){
          String line = client.readStringUntil('\r');
//Serial.print(line);
      }    
}

//
String conv_u8code(String sOne)
{
  String sRet="";
  if(sOne=="0"){ sRet ="EFbc90"; }
  if(sOne=="1"){ sRet ="EFbc91"; }
  if(sOne=="2"){ sRet ="EFbc92"; }
  if(sOne=="3"){ sRet ="EFbc93"; }
  if(sOne=="4"){ sRet ="EFbc94"; }
  if(sOne=="5"){ sRet ="EFbc95"; }
  if(sOne=="6"){ sRet ="EFbc96"; }
  if(sOne=="7"){ sRet ="EFbc97"; }
  if(sOne=="8"){ sRet ="EFbc98"; }
  if(sOne=="9"){ sRet ="EFbc99"; }
  if(sOne==":"){ sRet ="EFbc9a"; }
  if(sOne=="%"){ sRet ="EFbc85"; }
  if(sOne=="C"){ sRet ="EFbca3"; }
  return sRet;  
} 
//
void send_u8code(String src){
  int iWait=0;
  int iCt=0;
  String sRet="";
  int iLen= src.length();
  for (int i=0; i< iLen; i++){
    String sOne= src.substring( i, i+1 );
    sOne= conv_u8code(sOne);
    Serial.print( sOne );
    iCt++;
    if(iCt >=2){  
        delay(100);
        iCt=0;
    }
  }
  iWait= ( (iLen +2) * 1200);
  mTimerTmp = millis()+ iWait;
  Serial.println();
//Serial.println( sTemp );
}

//
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  dht.begin();
  
  setup_wifi();
//  client.setServer(mqtt_server, 1883);
  client.setServer(localserver, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  mSTAT=mSTAT_1_WDAT;
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  String sTopi=String( mTopicIn );
  String sTopi_in =String( topic );
  //String sLine="";
  mWDAT="";
  String sCRLF="\r\n";  
  if( sTopi.equals( sTopi_in ) ){
    //Serial.print("011");
    for (int i=0;i<length;i++) {
//      Serial.print((char)payload[i]);
      String sPay= String( (char)payload[i] );
      mWDAT += sPay;
    }
   }
}

//
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect( mClient_id )) {
      Serial.println("connected");
      client.subscribe(mTopicIn);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//
void sendMain_proc(){
  int iTemp=0;
  int iHum =0;
  int iWait=0;
  if (millis() > mTimerTmp) {
      //mTimerTmp = millis()+ 3000;
      Serial.print("011");
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      if (isnan(h) || isnan(t) ) {
          mTimerTmp = millis()+ 3000;
          Serial.println("Failed to read from DHT sensor!");
          return;
      }      
      if((mSTAT==mSTAT_1_WDAT) || (mSTAT==mSTAT_3_WDAT)){
          Serial.print( mHtypOne );
          Serial.println( mWDAT );
          mTimerTmp = millis()+ 5000;
          mSTAT =mSTAT+1;
      }
      else if(mSTAT==mSTAT_2_Temp){
          Serial.print( mHtypScr );
          iTemp =(int)t;
          String sTemp=String(iTemp) +"C";
//Serial.print( "sTemp.len=" );
//Serial.println( iLenTemp );
          send_u8code(sTemp);
          mSTAT =mSTAT_3_WDAT;
      }
      else if(mSTAT==mSTAT_4_Hum){
          Serial.print( mHtypScr );
          iHum = (int)h;
          String sHum= String(iHum) +"%";
          send_u8code(sHum);
          mSTAT =mSTAT_1_WDAT;
      }
  }  
}

//
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  sendMain_proc();
  if (millis() > mTimerPost) {
      mTimerPost= millis()+ 60000;
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      int iTemp =(int)t; 
      int iHum =(int)h; 
      if (isnan(h) || isnan(t) ) {
          Serial.println("Failed to read from DHT sensor!");
          return;
      }     
      proc_http(String(iTemp), String(iHum) );
      delay(100);
  }  
  
}




