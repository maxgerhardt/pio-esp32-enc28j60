/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino WIZnet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 moidified 12 Feb 2022
 by Maximilian Gerhardt
 */
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

#define ETH_SPI_BUS HSPI
#define ETH_MOSI 13
#define ETH_MISO 12
#define ETH_SCLK 14
#define ETH_CS 15

SPIClass ethernetSPI(ETH_SPI_BUS);

void setup()
{
    // You can use Ethernet.init(pin) to configure the CS pin and SPI
    // Init with specific pin settings from above.
    Ethernet.init(ETH_CS, &ethernetSPI, ETH_SCLK, ETH_MISO, ETH_MOSI);

    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Ethernet WebServer Example");

    // start the Ethernet connection (DHCP) and the server:
    Serial.println("Trying to recognize hardware and join via DHCP...");
    int ret = Ethernet.begin(mac);
    Serial.println("Got out of Ethernet.begin with ret = " + String(ret));

    if (ret == 0) {
        // Check for Ethernet hardware present
        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
            Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
            while (true)
            {
                delay(1); // do nothing, no point running without Ethernet hardware
            }
        }
        if (Ethernet.linkStatus() == LinkOFF)
        {
            Serial.println("Ethernet cable is not connected.");
        }
        while(Ethernet.linkStatus() == LinkOFF) {}
        Serial.println("Ethernet cable got connected, retrying.");
        if(Ethernet.begin(mac) == 0) {
            Serial.println("Ethernet.begin() failed.");
        } else {
            Serial.println("Ethernet.begin() ok.");
        }
    }

    // start the server
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
}

void loop()
{
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client)
    {
        Serial.println("new client");
        // an HTTP request ends with a blank line
        bool currentLineIsBlank = true;
        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
                Serial.write(c);
                // if you've gotten to the end of the line (received a newline
                // character) and the line is blank, the HTTP request has ended,
                // so you can send a reply
                if (c == '\n' && currentLineIsBlank)
                {
                    // send a standard HTTP response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close"); // the connection will be closed after completion of the response
                    client.println("Refresh: 5");        // refresh the page automatically every 5 sec
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");
                    // output the value of each analog input pin, 8 channels for now
                    for (int analogChannel = 0; analogChannel < 8; analogChannel++)
                    {
                        int sensorReading = analogRead(analogInputToDigitalPin(analogChannel));
                        client.print("analog input ");
                        client.print(analogChannel);
                        client.print(" is ");
                        client.print(sensorReading);
                        client.println("<br />");
                    }
                    client.println("</html>");
                    break;
                }
                if (c == '\n')
                {
                    // you're starting a new line
                    currentLineIsBlank = true;
                }
                else if (c != '\r')
                {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
}
