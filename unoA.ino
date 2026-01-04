/*
  UNO A Left Controller
  Sends speed and direction commands to UNO B over a dedicated serial link
  Prints status responses to Serial Monitor
*/

#include <SoftwareSerial.h>

static const int LINK_RX_PIN = 11; // UNO A RX from UNO B TX
static const int LINK_TX_PIN = 12; // UNO A TX to UNO B RX
static const int STATUS_LED_PIN = 13;

SoftwareSerial linkSerial(LINK_RX_PIN, LINK_TX_PIN);

int targetSpeed = 0;     // 0 to 255
int targetDir = 1;       // 1 forward, 0 reverse
unsigned long lastSendMs = 0;

void setup()
{
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  Serial.begin(115200);
  while (!Serial) { }

  linkSerial.begin(19200);

  Serial.println("UNO A ready");
  Serial.println("Type a number 0 to 255 then Enter for speed");
  Serial.println("Type d then Enter to toggle direction");
}

static void sendCommand(int speedValue, int dirValue)
{
  if (speedValue < 0) speedValue = 0;
  if (speedValue > 255) speedValue = 255;

  digitalWrite(STATUS_LED_PIN, HIGH);

  // Protocol: S,<speed>,<dir>\n
  linkSerial.print("S,");
  linkSerial.print(speedValue);
  linkSerial.print(",");
  linkSerial.print(dirValue);
  linkSerial.print("\n");

  digitalWrite(STATUS_LED_PIN, LOW);
}

static void readResponses()
{
  while (linkSerial.available() > 0)
  {
    String line = linkSerial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0)
    {
      Serial.print("UNO B: ");
      Serial.println(line);
    }
  }
}

static void handleSerialInput()
{
  if (Serial.available() <= 0) return;

  String s = Serial.readStringUntil('\n');
  s.trim();
  if (s.length() == 0) return;

  if (s == "d" || s == "D")
  {
    targetDir = (targetDir == 1) ? 0 : 1;
    Serial.print("Direction now: ");
    Serial.println(targetDir == 1 ? "forward" : "reverse");
    return;
  }

  int v = s.toInt();
  if (v < 0) v = 0;
  if (v > 255) v = 255;
  targetSpeed = v;

  Serial.print("Target speed set to: ");
  Serial.println(targetSpeed);
}

void loop()
{
  handleSerialInput();
  readResponses();

  unsigned long now = millis();
  if (now - lastSendMs >= 150)
  {
    lastSendMs = now;
    sendCommand(targetSpeed, targetDir);
  }
}
