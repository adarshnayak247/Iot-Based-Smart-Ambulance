#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

int pulsePin = A0;
int blinkPin = 13;

volatile int BPM;
volatile int Signal;
volatile int IBI = 600;
volatile boolean Pulse = false;
volatile boolean QS = false;
static boolean serialVisual = true;

volatile int rate[10];
volatile unsigned long sampleCounter = 0;
volatile unsigned long lastBeatTime = 0;
volatile int P = 512;
volatile int T = 512;
volatile int thresh = 525;
volatile int amp = 100;
volatile boolean firstBeat = true;
volatile boolean secondBeat = false;

void setup(void)
{
    pinMode(10, INPUT);
    pinMode(11, INPUT);
    Serial.println("Dallas Temperature IC Control Library Demo");
    sensors.begin();
    Serial.print("Locating devices...");
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount(), DEC);
    Serial.println(" devices.");
    Serial.print("Parasite power is: ");
    if (sensors.isParasitePowerMode())
        Serial.println("ON");
    else
        Serial.println("OFF");

    if (!sensors.getAddress(insideThermometer, 0))
        Serial.println("Unable to find address for Device 0");

    Serial.print("Device 0 Address: ");
    printAddress(insideThermometer);
    Serial.println();
    sensors.setResolution(insideThermometer, 9);

    Serial.print("Device 0 Resolution: ");
    Serial.print(sensors.getResolution(insideThermometer), DEC);
    Serial.println();

    pinMode(blinkPin, OUTPUT);
    Serial.begin(115200);
    interruptSetup();
}

float t;
int Bp, ECG;

void printTemperature(DeviceAddress deviceAddress)
{
    float tempC = sensors.getTempC(deviceAddress);
    t = tempC;
    if (tempC == DEVICE_DISCONNECTED_C)
    {
        Serial.println("Error: Could not read temperature data");
        return;
    }
}

void printAddress(DeviceAddress deviceAddress)
{
    // Implement this function if needed
}

void interruptSetup()
{
    TCCR2A = 0x02;
    TCCR2B = 0x06;
    OCR2A = 0X7C;
    TIMSK2 = 0x02;
    sei();
}

void serialOutput()
{
    if (serialVisual == true)
    {
        arduinoSerialMonitorVisual('-', Signal);
    }
    else
    {
        sendDataToSerial('S', Signal);
    }
}

void serialOutputWhenBeatHappens()
{
    if (serialVisual == true)
    {
        // ASCII Art Madness
        Bp = BPM;
    }
    else
    {
        sendDataToSerial('B', BPM);
        sendDataToSerial('Q', IBI);
    }
}

void arduinoSerialMonitorVisual(char symbol, int data)
{
    const int sensorMin = 0;
    const int sensorMax = 1024;
    int sensorReading = data;
    int range = map(sensorReading, sensorMin, sensorMax, 0, 11);
    // Add visualization logic if needed
}

void loop(void)
{
    sensors.requestTemperatures();
    printTemperature(insideThermometer);
    serialOutput();

    if (QS == true)
    {
        serialOutputWhenBeatHappens();
        QS = false;
    }

    if ((digitalRead(10) == 1) || (digitalRead(11) == 1))
    {
    }
    else
    {
        ECG = analogRead(A1);
    }

    delay(100);
    delay(20);

    if (t > 0 && Bp > 0 && ECG > 0)
    {
        delay(100);
        Serial.println(t);
        Serial.println(Bp);
        Serial.println(ECG);
        delay(20);
    }
}

void sendDataToSerial(char symbol, int data)
{
    // Implement this function if needed
}

ISR(TIMER2_COMPA_vect)
{
    cli();
    Signal = analogRead(pulsePin);
    sampleCounter += 2;
    int N = sampleCounter - lastBeatTime;

    if (Signal < thresh && N > (IBI / 5) * 3)
    {
        if (Signal < T)
        {
            T = Signal;
        }
    }

    if (Signal > thresh && Signal > P)
    {
        P = Signal;
    }

    if (N > 250)
    {
        if ((Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3))
        {
            Pulse = true;
            digitalWrite(blinkPin, HIGH);
            IBI = sampleCounter - lastBeatTime;
            lastBeatTime = sampleCounter;

            if (secondBeat)
            {
                secondBeat = false;
                for (int i = 0; i <= 9; i++)
                {
                    rate[i] = IBI;
                }
            }

            if (firstBeat)
            {
                firstBeat = false;
                secondBeat = true;
                sei();
                return;
            }

            word runningTotal = 0;
            for (int i = 0; i <= 8; i++)
            {
                rate[i] = rate[i + 1];
                runningTotal += rate[i];
            }
            rate[9] = IBI;
            runningTotal += rate[9];
            runningTotal /= 10;
            BPM = 60000 / runningTotal;
            QS = true;
        }
    }

    if (Signal < thresh && Pulse == true)
    {
        digitalWrite(blinkPin, LOW);
        Pulse = false;
        amp = P - T;
        thresh = amp / 2 + T;
        P = thresh;
        T = thresh;
    }

    if (N > 2500)
    {
        thresh = 512;
        P = 512;
        T = 512;
        lastBeatTime = sampleCounter;
        firstBeat = true;
        secondBeat = false;
    }
    sei();
}
