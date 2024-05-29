#include <DHT.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ir_Daikin.h>

#define DHTPIN 4 // what pin we're connected to
DHT dht(DHTPIN, DHT22);

const char *ssid = "Winternet is coming";
const char *password = "Nim3niNuSti3Parola";

const char *mqtt_server = "65.108.235.54";

WiFiClient wifiClient;
PubSubClient client(wifiClient);
long lastMsg = 0;

float targetTemperature = 1000;
bool coolerOn = false;

const uint16_t kIrLed = 2;
IRDaikinESP ac(kIrLed);

void
setup()
{
	Serial.begin(115200);
	Serial.println("Main code started");

	setup_wifi();

	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);

	dht.begin();

	ac.begin();
}

void
callback(char *topic, byte *message, unsigned int length)
{
	String messageTemp;
	for (int i = 0; i < length; i++) {
		messageTemp += (char)message[i];
	}

	// Feel free to add more if statements to control more GPIOs with MQTT

	// If a message is received on the topic esp32/output, you check if the
	// message is either "on" or "off". Changes the output state according to
	// the message
	if (String(topic) == "esp32/targetTemperature") {
		Serial.print("Changing target temperature to ");
		int _targetTemperature = messageTemp.toInt();

		if (_targetTemperature < 0 || _targetTemperature > 100) {
			Serial.println("Invalid target temperature");
			return;
		}

		Serial.println(_targetTemperature);

		// set target temperature
		targetTemperature = (float)_targetTemperature;
	}
}

void
setup_wifi()
{
	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

void
reconnect()
{
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect("ESP8266Client")) {
			Serial.println("connected");
			// Subscribe
			client.subscribe("esp32/targetTemperature");
			Serial.println("Subscribed to esp32/targetTemperature");
		} else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

void
turnCoolerOn()
{
	Serial.println("Turning cooler on");

	// Set up what we want to send. See ir_Daikin.cpp for all the options.
	ac.on();
	ac.setFan(1);
	ac.setMode(kDaikinCool);
	ac.setTemp(20);
	ac.setSwingVertical(false);
	ac.setSwingHorizontal(false);

	ac.disableOffTimer();

	// Now send the IR signal.
#if SEND_DAIKIN
	ac.send();
#endif // SEND_DAIKIN

	coolerOn = true;
}

void
turnCoolerOff()
{
	Serial.println("Turning cooler off");

	ac.off();

#if SEND_DAIKIN
	ac.send();
#endif // SEND_DAIKIN

	coolerOn = false;
}

void
loop()
{
	if (!client.connected()) {
		reconnect();
	}
	client.loop();

	long now = millis();
	if (now - lastMsg > 5000) {
		lastMsg = now;

		// Reading temperature or humidity takes about 250 milliseconds!
		// Sensor readings may also be up to 2 seconds 'old' (its a very slow
		// sensor)
		float h = dht.readHumidity();
		// Read temperature as Celsius (the default)
		float t = dht.readTemperature();

		// Check if any reads failed and exit early (to try again).
		if (isnan(h) || isnan(t)) {
			Serial.println("Failed to read from DHT sensor!");
			return;
		}

		// Compute heat index in Celsius (isFahreheit = false)
		float hic = dht.computeHeatIndex(t, h, false);

		// check if we need to turn on the cooler
		if (t > targetTemperature) {
			if (!coolerOn) {
				turnCoolerOn();
			}
		} else {
			if (coolerOn) {
				turnCoolerOff();
			}
		}

		Serial.print("Humidity: ");
		Serial.print(h);
		Serial.print(" %   ");
		Serial.print("Temperature: ");
		Serial.print(t);
		Serial.print(" *C   ");
		Serial.print("Heat index: ");
		Serial.print(hic);
		Serial.print(" *C   ");
		Serial.print("Target temperature: ");
		Serial.print(targetTemperature);
		Serial.print(" *C   ");
		Serial.print("Cooler on: ");
		Serial.println(coolerOn);

		client.publish("esp32/humidity", String(h).c_str());
		client.publish("esp32/temperature", String(t).c_str());
		client.publish("esp32/heatindex", String(hic).c_str());
		client.publish("esp32/coolerOn", String(coolerOn).c_str());
	}
}