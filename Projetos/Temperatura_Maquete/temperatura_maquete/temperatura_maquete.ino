#define PIN_TERMISTOR 4

const float R_NOMINAL = 10000.0; // Resistência nominal (10kΩ)
const float T_NOMINAL = 25.0;    // Temperatura nominal (25°C)
const float BETA = 3950.0;       // Constante Beta do termistor
const float SERIES_RESISTOR = 10000.0; // Resistor do divisor (10kΩ)

void setup() {
  Serial.begin(9600);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
}

void loop() {
  int adcValue = analogRead(PIN_TERMISTOR);
  if (adcValue == 0) {
    Serial.println("Erro: ADC Value 0, verifique o circuito.");
    delay(1000);
    return;
  }

  // Cálculo da resistência do termistor
  float resistance = SERIES_RESISTOR / ((4095.0 / adcValue) - 1.0);

  // Fórmula de Steinhart-Hart simplificada
  float temperatura = resistance / R_NOMINAL; // (R/Ro)
  temperatura = log(temperatura);            // ln(R/Ro)
  temperatura = temperatura / BETA;          // 1/Beta * ln(R/Ro)
  temperatura = temperatura + (1.0 / (T_NOMINAL + 273.15)); // + (1/To)
  temperatura = 1.0 / temperatura;           // Inverte para Kelvin
  temperatura = temperatura - 273.15;       // Converte para Celsius

  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");
  delay(1000);
}
