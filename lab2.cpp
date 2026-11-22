const int NTC_PIN       = A0;
const int TEMP_OUT_PIN  = 3;
const int OR_OUT_PIN    = 4;

const int LED_RED_PIN   = 5;
const int LED_GREEN_PIN = 6;
const int LED_BLUE_PIN  = 9;

const int SERIAL_RATE     = 9600;
const int SETTLE_DELAY_US = 50;
const int ALERT_BLINK_MS  = 500;
const int NORMAL_BLINK_MS = 500;

const float TEMP_WARN_C   = 35.0;
const float TEMP_DANGER_C = 50.0;

const int   ADC_MAX       = 1023;
const int   ADC_MIN_SAFE  = 1;
const float R_SERIES_OHM  = 4700.0;
const float R0_OHM        = 10000.0;
const float BETA_COEFF    = 3950.0;
const float KELVIN_OFFSET = 273.15;
const float T0_CELSIUS    = 25.0;
const float T0_KELVIN     = T0_CELSIUS + KELVIN_OFFSET;

const int COLOR_OFF      = 0;
const int COLOR_ORANGE_G = 165;
const int COLOR_MAX      = 255;

void setLedColor(int red, int green, int blue)
{
  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_BLUE_PIN, blue);
}

void blinkLed(int red, int green, int blue, int delayMs)
{
  setLedColor(red, green, blue);
  delay(delayMs);
  setLedColor(COLOR_OFF, COLOR_OFF, COLOR_OFF);
  delay(delayMs);
}

void blinkStatusLed(float tempC, bool isFlame)
{
  if (isFlame || tempC > TEMP_DANGER_C) {
    blinkLed(COLOR_MAX, COLOR_OFF, COLOR_OFF, ALERT_BLINK_MS);
    return;
  }
  if (tempC >= TEMP_WARN_C) {
    blinkLed(COLOR_MAX, COLOR_ORANGE_G, COLOR_OFF, ALERT_BLINK_MS);
    return;
  }
  blinkLed(COLOR_MAX, COLOR_MAX, COLOR_MAX, NORMAL_BLINK_MS);
}

void initPins()
{
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);

  pinMode(TEMP_OUT_PIN, OUTPUT);
  pinMode(OR_OUT_PIN, INPUT);
  pinMode(NTC_PIN, INPUT);
}

int readNtcAdc()
{
  int rawAdc = analogRead(NTC_PIN);
  if (rawAdc < ADC_MIN_SAFE) {
    return ADC_MIN_SAFE;
  }
  return rawAdc;
}

float calculateNtcResistance(int adcValue)
{
  return R_SERIES_OHM * ((float)ADC_MAX / (float)adcValue - 1.0);
}

float calculateNtcTempKelvin(float ntcResistance)
{
  float invT = (1.0 / T0_KELVIN) + (1.0 / BETA_COEFF) * log(ntcResistance / R0_OHM);
  return 1.0 / invT;
}

float kelvinToCelsius(float tempKelvin)
{
  return tempKelvin - KELVIN_OFFSET;
}

float getTemperatureCelsius()
{
  int adcValue = readNtcAdc();
  float ntcResistance = calculateNtcResistance(adcValue);
  float tempKelvin = calculateNtcTempKelvin(ntcResistance);
  return kelvinToCelsius(tempKelvin);
}

void resetTempOut()
{
  digitalWrite(TEMP_OUT_PIN, LOW);
  delayMicroseconds(SETTLE_DELAY_US);
}

bool readOrOutState()
{
  return (digitalRead(OR_OUT_PIN) == HIGH);
}

bool isWarningTemp(float tempC)
{
  return tempC > TEMP_WARN_C;
}

void writeTempOut(bool isHigh)
{
  digitalWrite(TEMP_OUT_PIN, isHigh ? HIGH : LOW);
  delayMicroseconds(SETTLE_DELAY_US);
}

void setup()
{
  Serial.begin(SERIAL_RATE);
  initPins();
  analogReference(DEFAULT);
}

void loop()
{
  float currentTempC = getTemperatureCelsius();

  resetTempOut();
  bool flameDetected = readOrOutState();

  writeTempOut(isWarningTemp(currentTempC));
  Serial.println(currentTempC);
  blinkStatusLed(currentTempC, flameDetected);
}
