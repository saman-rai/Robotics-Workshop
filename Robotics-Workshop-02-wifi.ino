/*
 * ESP32 Web Server - PWM Brightness Control with Slider
 * Robotics Club Workshop 2
 * 
 * Creates a WiFi Access Point. Connect your phone to "ESP32_Robot"
 * Open browser to 192.168.4.1 and use the slider to dim/brighten the LED.
 */

#include <WiFi.h>

// Use Access Point mode (ESP32 creates its own WiFi network)
const char* ap_ssid = "ESP32_Robot";
const char* ap_password = "12345678";  // Must be at least 8 characters

WiFiServer server(80);

// LED pin (built-in LED on most ESP32 dev boards is GPIO 2)
const int ledPin = 2;

// Current brightness percentage (0-100)
int brightnessPercent = 0;

void setup() {
  Serial.begin(115200);
  
  // Set LED pin as output (PWM capable)
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);  // Start with LED off

  // Start Access Point
  Serial.print("Setting up Access Point: ");
  Serial.println(ap_ssid);
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.println("Connect your phone to WiFi: ESP32_Robot (password: 12345678)");
  Serial.println("Then open browser to: http://192.168.4.1");

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New Client.");
    String header = "";
    String currentLine = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        header += c;
        
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // ---- Process the request ----
            
            // Check if the request contains "/set?value="
            if (header.indexOf("GET /set?value=") >= 0) {
              // Extract the value from the URL
              int valueStart = header.indexOf("value=") + 6;
              int valueEnd = header.indexOf(" ", valueStart);
              String valueStr = header.substring(valueStart, valueEnd);
              brightnessPercent = valueStr.toInt();
              
              // Constrain to 0-100
              brightnessPercent = constrain(brightnessPercent, 0, 100);
              
              // Map percentage (0-100) to PWM duty (0-255)
              int duty = map(brightnessPercent, 0, 100, 0, 255);
              analogWrite(ledPin, duty);  // <-- SIMPLE PWM
              
              Serial.print("Brightness set to: ");
              Serial.print(brightnessPercent);
              Serial.print("% (PWM: ");
              Serial.print(duty);
              Serial.println(")");
            }
            
            // ---- Send HTTP Response ----
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // ---- Build the Webpage ----
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<title>ESP32 LED Dimmer</title>");
            client.println("<style>");
            client.println("body { font-family: Arial; text-align: center; margin-top: 50px; background-color: #1a1a2e; color: #eee; }");
            client.println(".slider-container { width: 80%; max-width: 400px; margin: 30px auto; }");
            client.println("input[type=range] { width: 100%; height: 20px; -webkit-appearance: none; background: #333; border-radius: 10px; outline: none; }");
            client.println("input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 40px; height: 40px; background: #4CAF50; border-radius: 50%; cursor: pointer; }");
            client.println(".value-display { font-size: 48px; font-weight: bold; margin: 20px; color: #4CAF50; }");
            client.println("h1 { color: #4CAF50; }");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 12px 30px; text-decoration: none; font-size: 20px; border-radius: 5px; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            
            client.println("<h1>ESP32 LED Dimmer</h1>");
            client.println("<h3>Robotics Club Workshop</h3>");
            
            client.print("<div class=\"value-display\">");
            client.print(brightnessPercent);
            client.println("%</div>");
            
            client.println("<div class=\"slider-container\">");
            client.print("<input type=\"range\" min=\"0\" max=\"100\" value=\"");
            client.print(brightnessPercent);
            client.println("\" class=\"slider\" id=\"brightnessSlider\" oninput=\"updateValue(this.value)\">");
            client.println("</div>");
            
            client.println("<p><em>Move the slider to change LED brightness</em></p>");
            client.println("<p><a href=\"/\"><button class=\"button\">Refresh</button></a></p>");
            
            // JavaScript to handle slider updates
            client.println("<script>");
            client.println("function updateValue(val) {");
            client.println("  document.querySelector('.value-display').innerHTML = val + '%';");
            client.println("  fetch('/set?value=' + val);");
            client.println("}");
            client.println("</script>");
            
            client.println("</body></html>");
            client.println();
            
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.\n");
  }
}