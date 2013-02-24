/*
  DNS and DHCP-based Web client
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen
 
 GET request format: ?version=<integer>&boardid=<36 char uuid>
     reply format: machineid=<integer>&macs=<list of 12 char macs, separated and ended by char 'X'>
    
 POST request format: version=<integer>&boardid=<36 char uuid>&machineid=<integer>
      reply format:  --- 200 OK or 405 forbidden
 */

#include <SPI.h>
#include <Ethernet.h>
#include <avr/wdt.h>

#define USERNAME "balls"
#define PASSWORD "jabadoo"
#define LOOP_SECONDS 10
#define VERSION "1" // protocol version string.

#define PWR_BUTTON_PIN 3
#define MOBO_PWR_PIN 2
#define LED_PIN 13
#define SERVERURL "zpc.dk"
byte server[] = { 85, 17, 23, 70 };
byte ip[] = { 192, 168, 0, 10 };
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte ownMAC[6] = {  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 }; // our own MAC address
//char serverName[] = "zpc.dk"; // the server we connect to, in order to know which macs to send in our wake-on-lan packet

EthernetClient serverClient;

/*
typedef struct GET_REPLY_STRUCT {
  MAC_LIST *macs
  int machineID;
} GET_REPLY;
*/

void setup() {
  
  pinMode(PWR_BUTTON_PIN, INPUT);
  pinMode(MOBO_PWR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(MOBO_PWR_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // start the Ethernet connection:
  /*
  Serial.println("Fetching IP via DHCP");
  while (Ethernet.begin(ownMAC) == 0)
    Serial.println("Failed to configure Ethernet using DHCP. Retrying...");
    */
  Ethernet.begin(ownMAC, ip);
  // give the Ethernet shield a second to initialize:
  delay(1000);
}


// contact server for mac adresses, send PACKET_COUNT magic packets for each received mac adress
void loop()
{
  
  int action;
  if (serverClient.connect(SERVERURL, 80)) {
    Serial.println("Connected");
    serverClient.print("GET http://zpc.dk/api/poll/?name=balls&password=jabadoo");
    serverClient.print(" HTTP/1.0\r\n\r\n");

    while(!serverClient.available()){
      delay(500);
      Serial.println("waiting for available data");
    }
    Serial.println("Got server response");
    if (dropHeader(serverClient, false)) {
      if(serverClient.available()) {
        action = parseReply(serverClient, true);
        serverClient.stop();
        switch (action)
          {
            case 0:
              // do nothing
              Serial.println("do nothing");
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
      }
    } else {
        Serial.println("Couldn't parse header from server");
        serverClient.stop();
      }
  } else {
    Serial.println("Connection failed");
  }
  delay(LOOP_SECONDS * 1000);
}


// -------------------------------------- PARSE GET REPLY ----------------------------------- //
/*
boolean parseGetReply(GET_REPLY *reply, EthernetClient client)
{
  if (dropHeader(client))
  {
    long machineID = 0;
    if (parseMachineID(client, &machineID))
    {
      boolean ok = true
      while (ok && client.available())
      {
        
      }
      
    }
  }
  else
    return false;
}
*/
// ------------------------------------- /PARSE GET REPLY ----------------------------------- //


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



// -------------------------------------------- SEND CONFIRM --------------------------------------//

// Expected parameters - username, password, clear (list of MAC's, separated by "x")
boolean sendConfirm(char *hex, EthernetClient client)
{
  if (client.connect(SERVERURL, 8000)) {
    char *rsp = "POST /api/wol/confirm/ HTTP/1.0\r\nHost: 192.168.1.100\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 55";
    char *data = "username=" USERNAME "&password=" PASSWORD "&clear=";
    char *endline = "\r\n";
    
    client.print(rsp);
    client.print(endline);
    client.print(endline);
    client.print(data);
    client.print(hex);
    client.print(endline);
    client.stop();
    return true;
  } else
    return false;
}
// -------------------------------------------- /SEND CONFIRM -------------------------------------//

// -------------------------------------------- START PC ------------------------------------------//
void powerOn()
{
  Serial.println("Powering on");
  digitalWrite(MOBO_PWR_PIN, LOW); // low = button pressed
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(MOBO_PWR_PIN, HIGH); 
}
// -------------------------------------------- /START PC -----------------------------------------//

// -------------------------------------------- RESTART PC ----------------------------------------//
void restart()
{
  powerOff();
  delay(6000); // wait 
  powerOn();
}
// -------------------------------------------- /RESTART PC ---------------------------------------//

// -------------------------------------------- SHUTDOWN PC ---------------------------------------//
void powerOff()
{
  Serial.println("Shutting down");
  digitalWrite(MOBO_PWR_PIN, LOW); // low = button pressed
  digitalWrite(LED_PIN, HIGH);
  delay(10000); // hold down power button
  digitalWrite(LED_PIN, LOW);
  digitalWrite(MOBO_PWR_PIN, HIGH);
}
// -------------------------------------------- /SHUTDOWN PC --------------------------------------//
