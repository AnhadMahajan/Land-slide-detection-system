#include "Arduino.h"
#include <EMailSender.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ThingSpeak.h>
WiFiClient client;

long myChannelNumber = 2331887;
const char myWriteAPIKey[] = "HWGOZL9TD5OVR3LY";


 //Initialize the LCD with the I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo servo; // Create a servo object
int soilMoisturePin = A0;
int servoPin = D6; // Define the servo control pin on NodeMCU (e.g., D2)
int Led1 = D3;
int Led2 = D4;
int buzzer = D5;

const char* ssid = "A";
const char* password = "9218730168";

uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;

EMailSender emailSend("walia.institute026@gmail.com", "xrtgkmkizmepczmx");

uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);
        Serial.println(nSSID);
    }

    uint8_t i = 0;
    while(WiFi.status() != WL_CONNECTED && i++ < 50)
    {
        delay(200);
        Serial.print(".");
    }
    ++attempt;
    Serial.println("");
    if(i == 51) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Check if access point available or SSID and Password\r\n");
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}

void Awaits()
{
    uint32_t ts = millis();
    while (!connection_state)
    {
        delay(50);
        if (millis() > (ts + reconnect_interval) && !connection_state) {
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}

void sendEmailToRecipient(const char* recipientEmail)
{
    EMailSender::EMailMessage message;
    message.subject = "Landslide Warning";
    message.message = "The Area(KOTLA) is in danger. Any delay may put everyone life at risk.You need to take action immediately.";

    EMailSender::Response resp = emailSend.send(recipientEmail, message);

    Serial.println("Sending status to " + String(recipientEmail) + ":");
    Serial.println(resp.status);
    Serial.println(resp.code);
    Serial.println(resp.desc);

    // Add a delay between sending emails to avoid potential issues with rate limiting or other restrictions
    delay(10000); // 10-second delay
}

void setup()
{
    

    

    // List of email recipients
    

  Serial.begin(115200); // Define baud rate for serial communication
  servo.attach(servoPin);
  servo.write(155);
  delay(500); // Attach the servo to the specified pin

  lcd.begin();
  lcd.backlight();
  lcd.clear();  // Turn on the backlight
  pinMode(soilMoisturePin, INPUT); 
  pinMode(Led1, OUTPUT);
  pinMode(Led2,OUTPUT);
  pinMode(buzzer, OUTPUT);
  ThingSpeak.begin(client);
 
  


    
}
void loop()
{
    // Your loop code, if needed
  //unsigned int sensorValue = analogRead(soilMoisturePin);
  connection_state = WiFiConnect(ssid, password);
    if (!connection_state) {
        Awaits();
    }
  const char* recipients[] = {
        "anhadmahjan06@gmail.com",
        "jahnvimehra47@gmail.com",
       // "mehakwalia82@gmail.com"
    };
  while (true) {
        unsigned int sensorValue = analogRead(soilMoisturePin);
        int mappedValue = map(sensorValue, 50, 425, 100, 0);
        //Serial.println(sensorValue);
       // Serial.println(mappedValue);
       lcd.setCursor(0, 0);
       lcd.print("Soil Moisture:");
        lcd.setCursor(0, 1);
        lcd.print("Sensor 2: " + String(mappedValue) + "%");
        

        if (mappedValue > 70) {
   servo.write(0); // Rotate the servo to 90 degrees
   delay(1000); // Wait for 1 second
   digitalWrite(Led1, LOW);
    digitalWrite(Led2, LOW);
    digitalWrite(buzzer,HIGH);
    delay(500);
   // noTone(buzzer);
    // Rotate the servo back to 0 degrees
  }
  else{
   servo.write(155);
   digitalWrite(Led1, HIGH);
    digitalWrite(Led2,HIGH);
    digitalWrite(buzzer, LOW);
  }
 
 ThingSpeak.writeField(myChannelNumber, 1, mappedValue, myWriteAPIKey);

  if (sensorValue < 150) {
            for (int i = 0; i < 2; i++) {
               sendEmailToRecipient(recipients[i]);
              
            }
        }
   // Map the sensor value to the range 0-100

 // Serial.println(mappedValue);

  }
  delay(1000);
}
