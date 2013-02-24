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

#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <avr/wdt.h>

#define USERNAME "root"
#define PASSWORD "1234"
#define LOOP_SECONDS 10
#define VERSION "1" // protocol version string.

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte ownMAC[MAC_SIZE] = {  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 }; // our own MAC address
char hexMAC[HEX_MAC_SIZE];
char serverName[] = "85.17.23.70"; // the server we connect to, in order to know which macs to send in our wake-on-lan packet

EthernetClient serverClient;

/*
typedef struct GET_REPLY_STRUCT {
  MAC_LIST *macs
  int machineID;
} GET_REPLY;
*/

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // start the Ethernet connection:
  while (Ethernet.begin(ownMAC) == 0)
    Serial.println("Failed to configure Ethernet using DHCP. Retrying...");

  // give the Ethernet shield a second to initialize:
  delay(1000);
}


// contact server for mac adresses, send PACKET_COUNT magic packets for each received mac adress
void loop()
{
  
  boolean ok;
  if (serverClient.connect(serverName, 80)) {
    Serial.println("Connected");
    serverClient.print("GET /api/poll/?user=" USERNAME "&password=" PASSWORD);
    serverClient.print(" HTTP/1.0\r\n\r\n");

    while(!serverClient.available())
      delay(500);
    Serial.println("Got server response");
    if (dropHeader(serverClient)) {
      if(serverClient.available()) {
        //get_reply = (GET_REPLY)malloc(sizeof(GET_REPLY)
        //ok = takeMAC(receivedMAC, serverClient);
        
        serverClient.stop();
        if (ok) {
          fillHexMac(receivedMAC, hexMAC);
          Serial.print("  Got MAC address from server: ");
          Serial.println(hexMAC);
          makePkt(receivedMAC, magicpacket);
          sendPkt(magicpacket, wolClient);
          Serial.println("  Magic packets sent");
          sendConfirm(hexMAC, serverClient);
          Serial.println("  MAC Confirmed");
        } else
          Serial.println("couldn't parse MAC address from server response");
      } else {
        serverClient.stop();
        Serial.println("  No MAC address from server");
      }
    } else
      Serial.println("Couldn't parse header from server");
  } else
    Serial.println("Connection failed");
  delay(LOOP_SECONDS * 1000);
}


// -------------------------------------- PARSE GET REPLY ----------------------------------- //
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

// ------------------------------------- /PARSE GET REPLY ----------------------------------- //


// ------------------------------------- PARSE REPLY --------------------------------------- //
// take the string "XXXXXXXXXXXX\n" from the client, where mac is a 6-byte (12-nibble) hex number, and fill the mac array with these bytes
boolean takeMAC(byte *mac, EthernetClient client)
{
  char c;
  byte hi, low;
  int i, j;
  for (i = 0; i < 6; i++)
    if (client.available())
    {
      hi = client.read();
      if (client.available())
      {
        low = client.read();
        if (validHexChar(hi) && validHexChar(low))
          mac[i] = (byte)(16 * hexVal(hi) + hexVal(low));
        else
        {
          Serial.print("invalid hex char: ");
          Serial.print((char)hi);
          Serial.println();
          Serial.print((char)low);
          Serial.println();
          return false;
        }  
      } 
      else       
        return false;
    }
    else
      return false;
  return (client.available() && client.read() == '\n');
}

// just check for two '\n' chars, separated by another char. This will be the "\r\n\r\n" marker beginning separating the header from the body
boolean dropHeader(EthernetClient client)
{
  boolean finishHeader = false;
  char c;
  char prev[] = {' ', ' '};
  if (client.available())
  {
    c = client.read();
    while (client.available() && !finishHeader)
    {
      prev[1] = prev[0];
      prev[0] = c;
      c = client.read();
      finishHeader = prev[1] == '\n' && c == '\n';
    }
  }
  return finishHeader;
}



// -------------------------------------------- SEND CONFIRM --------------------------------------//

// Expected parameters - username, password, clear (list of MAC's, separated by "x")
boolean sendConfirm(char *hex, EthernetClient client)
{
  if (client.connect(serverName, 8000)) {
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

