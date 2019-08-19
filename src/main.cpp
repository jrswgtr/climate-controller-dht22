#include <ClimateController.h>
#include <DHTClimateSensor.h>

#define DHT_PIN 3

#define HEATER_PIN 4
#define COOLER_PIN 5
#define HUMIDIFIER_PIN 6
#define DE_HUMIDIFIER_PIN 7

#define POLL_INTERVAL 6000

#define TEMPERATURE_LOCK_DELAY 120000

#define HEATER_MIN 0.0
#define HEATER_MAX 30.0

#define COOLER_MIN 32
#define COOLER_MAX 100

#define HUMIDIFIER_MIN 0.0
#define HUMIDIFIER_MAX 69.9

#define DE_HUMIDIFIER_MIN 75.0
#define DE_HUMIDIFIER_MAX 100.0

ClimateController *heaterController;
ClimateController *coolerController;
ClimateController *humidifierController;
ClimateController *deHumidifierController;

long lastPollTime = 0;

void setup()
{
    Serial.begin(9600);

    Lock *temperatureLock = new DelayedLock(TEMPERATURE_LOCK_DELAY);
    Lock *humidityLock = new Lock();

    ClimateSensor *temperatureSensor = new DHTTemperatureSensor(DHT_PIN);
    ClimateSensor *humiditySensor = new DHTHumiditySensor(DHT_PIN);

    heaterController = new ClimateController(
        temperatureSensor,
        new Relay(HEATER_PIN),
        new Range(HEATER_MIN, HEATER_MAX),
        temperatureLock
    );

    coolerController = new ClimateController(
        temperatureSensor, 
        new Relay(COOLER_PIN), 
        new Range(COOLER_MIN, COOLER_MAX),
        temperatureLock
    );

    humidifierController = new ClimateController(
        humiditySensor,
        new Relay(HUMIDIFIER_PIN), 
        new Range(HUMIDIFIER_MIN, HUMIDIFIER_MAX),
        humidityLock
    );

    deHumidifierController = new ClimateController(
        humiditySensor,
        new Relay(DE_HUMIDIFIER_PIN), 
        new Range(DE_HUMIDIFIER_MIN, DE_HUMIDIFIER_MAX),
        humidityLock
    );
}

/**
 * Check if it's time to poll.
 * 
 * @param currentTime the current time in milliseconds
 * @return `true` if the `POLL_INTERVAL` time has passed since the last poll.
 */
bool isTimeToPoll(long currentTime)
{
    return lastPollTime == 0 || currentTime - lastPollTime >= POLL_INTERVAL;
}

/**
 * Poll all ClimateController instances
 */
void poll()
{
    heaterController->poll();
    coolerController->poll();
    humidifierController->poll();
    deHumidifierController->poll();
}

/**
 * Print messages to serial monitor
 */
void print()
{
    Serial.print("Temperatuur: ");
    Serial.print(heaterController->getCurrentValue());
    Serial.println("");

    Serial.print("Vochtigheid: ");
    Serial.print(humidifierController->getCurrentValue());
    Serial.println("");
}

void loop()
{
    long currentTime = millis();
    if (isTimeToPoll(currentTime)) {
        lastPollTime = currentTime;
        poll();
        print();
    }
}