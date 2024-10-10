#include "iBeacon.h"

BLEUart bleuart;

/**
 * @brief Configura el dispositivo y inicia el advertising
 * @author Alejandro Rosado
 * 
 * setup() -> void
 * 
 * Inicializa la comunicación serial, configura el módulo Bluefruit,
 * obtiene una primera medición de los sensores y inicia el advertising.
 */
void setup() {
  Serial.begin(115200);

  Serial.println("-----------------------\n");
  Serial.println(" PRUEBAS iBeacon  ");
  Serial.println("-----------------------\n");

  Serial.println("Initialise the Bluefruit nRF52 module");
  Bluefruit.begin();

  Serial.println("Setting Device Name to SoyBACON ");
  Bluefruit.setName("SoyBACON");
  Bluefruit.ScanResponse.addName();

  MedicionSensor medicion = obtenerMedicionSensor();
  startAdvertising(medicion);

  Serial.println("\nAdvertising");
}

namespace Loop {
  int cont = 0;
  unsigned long lastMeasurementTime = 0;
  const unsigned long measurementInterval = 60000;
}

/**
 * @brief Función principal que se ejecuta continuamente
 * @author Alejandro Rosado
 * 
 * loop() -> void
 * 
 * Realiza mediciones periódicas y actualiza el advertising con los nuevos datos.
 * También lleva un conteo de las iteraciones del loop.
 */
void loop() {
  using namespace Loop;

  cont++;

  unsigned long currentMillis = millis();
  if (currentMillis - lastMeasurementTime >= measurementInterval) {
    lastMeasurementTime = currentMillis;

    MedicionSensor medicion = obtenerMedicionSensor();
    startAdvertising(medicion);
  }

  delay(3000);

  Serial.print(" ** loop cont=");
  Serial.println(cont);
}