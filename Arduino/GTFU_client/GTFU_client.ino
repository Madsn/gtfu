/*
  Mikkel Madsen - m@madsn.net
  Connect GND on arduino to GND on motherboard, pwron + from motherboard to pin D2.
  Connect powerbutton positive lead from case to D2 as well, and GND button lead to GND on arduino or motherboard.
 */

#include <SPI.h>
#include <Ethernet.h>
#include <avr/wdt.h>

#define LOOP_SECONDS 10
#define VERSION "1" // protocol version string.

#define MOBO_PWR_PIN 2
#define LED_PIN 13

#define SERVERURL "zpc.dk"
#define NAME "balls"
#define PASSWORD "jabadoo"
#define VERBOSE true
//byte server[] = { 85, 17, 23, 70 };
//byte ip[] = { 192, 168, 0, 10 };

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte ownMAC[6] = {  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 }; // our own MAC address

EthernetClient serverClient;

void setup() {
  
  pinMode(MOBO_PWR_PIN, OUTPUT);
  digitalWrite(MOBO_PWR_PIN, HIGH);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  initializeEthernet();
}

void initializeEthernet()
{
  // start the Ethernet connection:
  Serial.println("Fetching IP via DHCP");
  while (Ethernet.begin(ownMAC) == 0)
    Serial.println("Failed to configure Ethernet using DHCP. Retrying...");
    
  // Ethernet.begin(ownMAC, ip);
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop()
{
  int action;
  if (serverClient.connect(SERVERURL, 80)) {
    Serial.println("Connected");
    Serial.println("GET " SERVERURL "/api/poll/?name=" NAME "&password=" PASSWORD);
    serverClient.print("GET http://" SERVERURL "/api/poll/?name=" NAME "&password=" PASSWORD);
/*    serverClient.print(NAME);
    serverClient.print("&password=");
    serverClient.print(PASSWORD); */
    serverClient.print(" HTTP/1.0\r\n\r\n");

    for(int i=0;i<20;i++){
      if(serverClient.available())
        break;
      delay(500);
      Serial.println("waiting for available data");
    }
    Serial.println("Got server response");
    if (dropHeader(serverClient, VERBOSE)) {
      if(serverClient.available()) {
        action = parseReply(serverClient, VERBOSE);
        serverClient.stop();
        switch (action)
          {
            case 0:
              // do nothing
              Serial.println("\ndo nothing");
              break;
            case 1:
              // wake
              powerOn();
              break;
            case 2:
              // restart
              restart();
              break;
            case 3:
              // turn off
              powerOff();
              break;
        }
      } else {
        serverClient.stop();
      }
    } else {
        Serial.println("Couldn't parse header from server");
        serverClient.stop();
      }
  } else {
    Serial.println("Connection failed");
    if (serverClient.connect("google.com", 80) == false) {
      int renewResult; 
      int attempts = 0;
      do {
        Serial.println("attempting to renew IP via dhcp");
        renewResult = Ethernet.maintain(); // renew dhcp
        Serial.print("renewal result code: ");
        Serial.println(renewResult);
        attempts++;
        delay(10000);
      }while(renewResult != 1 && renewResult != 3 && attempts < 20); //http://arduino.cc/en/Reference/EthernetMaintain
      if (attempts >= 20)
        initializeEthernet();
    }else {
      delay(1000);
      serverClient.stop();
    }
  }
  serverClient.stop();
  delay(LOOP_SECONDS * 1000);
}

// ------------------------------------- PARSE REPLY --------------------------------------- //
// 
int parseReply(EthernetClient client, boolean verbose)
{
  char c;
  int action = 0;
  if(client.available()){
    c = client.read();
    action = int(c)-48;
    if (verbose){
      Serial.print("Action:");
      Serial.println(action);
      Serial.println("reply:"); 
      Serial.print(c);  
    }
  }
  while(client.available()){
    c = client.read();       
    if(verbose)
      Serial.print(c);
  }
  return action;
}

// just check for two '\n' chars, separated by another char. This will be the "\r\n\r\n" marker beginning separating the header from the body
boolean dropHeader(EthernetClient client, boolean verbose)
{
  boolean finishHeader = false;
  char c;
  char prev[] = {' ', ' '};
  if (client.available())
  {
    c = client.read();
    if(verbose)
      Serial.print(c);
    while (client.available() && !finishHeader)
    {
      prev[1] = prev[0];
      prev[0] = c;
      c = client.read();
      if(verbose)
        Serial.print(c);
      finishHeader = prev[1] == '\n' && c == '\n';
    }
  }
  return finishHeader;
}


// -------------------------------------------- PC INTERFACE FUNCTIONS ----------------------------//
void powerOn()
{
  Serial.println("\nPowering on");
  digitalWrite(MOBO_PWR_PIN, LOW); // low = button pressed
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(MOBO_PWR_PIN, HIGH); 
}

void restart()
{
  powerOff();
  delay(6000); // wait 
  powerOn();
}

void powerOff()
{
  Serial.println("\nShutting down");
  digitalWrite(MOBO_PWR_PIN, LOW); // low = button pressed
  digitalWrite(LED_PIN, HIGH);
  delay(10000); // hold down power button
  digitalWrite(LED_PIN, LOW);
  digitalWrite(MOBO_PWR_PIN, HIGH);
}
// -------------------------------------------- /PC INTERFACE FUNCTIONS ---------------------------//
