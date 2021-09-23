#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>           // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define RST_PIN         D3           // Configurable, see typical pin layout above
#define SS_PIN          D8          // Configurable, see typical pin layout above
IPAddress server_addr(54,163,134,196);  // IP of the MySQL *server* here
char user[] = "admin";              // MySQL user login username
char password[] = "Minecraftturtle123!";        // MySQL user login password
char ssid[] = "Rfidtags";         // your SSID
char pass[] = "vclc7973";     // your SSID Password
WiFiClient client;                 // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;

const long utcOffsetInSeconds = -18000;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, utcOffsetInSeconds);
String timer;
char timerarray[9];
char location[] = "ClassT231";
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
String uidstring;
char uid[12];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);                                           // Initialize serial communications with the PC
  SPI.begin(); //connect to RC522
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password/*, default_db*/))
    Serial.println("MySQL Connected");
  else
    Serial.println("MySQL Failed");
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  timeClient.begin();
  Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  if(WL_CONNECTED && conn.connected())
        digitalWrite(LED_BUILTIN, HIGH);   //off
        delay(100);                        //wait 0.1 seconds
        digitalWrite(LED_BUILTIN, LOW);    //on 
        delay(100);                      
        digitalWrite(LED_BUILTIN, HIGH);  
        delay(100); 
        digitalWrite(LED_BUILTIN, LOW);   
        delay(100);                      
        digitalWrite(LED_BUILTIN, HIGH);  
        delay(100); 
        digitalWrite(LED_BUILTIN, LOW);   
        delay(100);                      
        digitalWrite(LED_BUILTIN, HIGH);  
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  timeClient.update();
  uidstring = String(mfrc522.uid.uidByte[0], HEX) + " " + String(mfrc522.uid.uidByte[1], HEX) + " " + String(mfrc522.uid.uidByte[2], HEX) + " " + String(mfrc522.uid.uidByte[3], HEX);
  uidstring.toUpperCase();
  uidstring.toCharArray(uid, 12);
  timer = (timeClient.getFormattedTime());
  timer.toCharArray(timerarray, 9);
  char INSERT_SQL[50];
  sprintf(INSERT_SQL, "INSERT INTO Classrooms_Prod.%s (timedate, rfid_id, location) VALUES ('%s', '%s', '%s')", location, timerarray, uid, location);
  Serial.println(INSERT_SQL);
  if (conn.connected())
    cursor->execute(INSERT_SQL);
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
}
