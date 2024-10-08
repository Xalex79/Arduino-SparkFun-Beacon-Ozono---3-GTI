#include "iBeacon.h"

/**
 * @brief Inicia el proceso de advertising BLE con los valores de medición
 * @author Alejandro Rosado
 * 
 * @param medicion Estructura que contiene la temperatura y concentración de Ozono corregida
 * 
 * medicion: MedicionSensor -> startAdvertising() -> void
 * 
 * Configura y inicia el proceso de advertising BLE. Configura el paquete de advertising
 * con flags, la potencia de transmisión, el nombre del dispositivo y los datos de beacon.
 */
void startAdvertising(MedicionSensor medicion) {
  Serial.println(" startAdvertising() ");
  Bluefruit.Advertising.stop();
  
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();
  
  uint8_t beaconUUID[16] = {
    'E', 'P', 'S', 'G', '-', 'G', 'T', 'I',
    '-', 'A', 'L', 'E', 'X', '-', '3', 'A'
  };
  
  BLEBeacon elBeacon(beaconUUID, medicion.GasOzonoCorregido, medicion.temperatura, 73);
  elBeacon.setManufacturer(0x004c);
  Bluefruit.Advertising.setBeacon(elBeacon);
  
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);
}

/**
 * @brief Obtiene una medición de los sensores de ozono y temperatura
 * @author Alejandro Rosado
 * 
 * obtenerMedicionSensor() -> MedicionSensor
 * 
 * Lee los valores de los sensores de ozono y temperatura, realiza los cálculos
 * necesarios para obtener las mediciones corregidas y las devuelve en una estructura MedicionSensor.
 */
MedicionSensor obtenerMedicionSensor() {
  double temperatura = leerTemperatura();
  double valorOzono = leerOzono();
  double concentracionOzono = obtenerConcentracionOzono(valorOzono);
  double concentracionCorregida = corregirConcentracionOzono(temperatura, concentracionOzono) * 10;

  MedicionSensor medicion;
  medicion.temperatura = round(temperatura);
  medicion.GasOzonoCorregido = concentracionCorregida;

  Serial.print("Concentración de Ozono corregida: ");
  Serial.print(concentracionCorregida);
  Serial.print(" ppm x 10. Concentración real medida: ");
  Serial.print(concentracionCorregida / 10);
  Serial.println(" ppm");

  Serial.print("Temperatura: ");
  Serial.print(round(temperatura));
  Serial.print("ºC redondeados. Temperatura real medida: ");
  Serial.print(temperatura);
  Serial.println("ºC");

  return medicion;
}

/**
 * @brief Lee el valor analógico de un pin y lo convierte a voltaje
 * @author Alejandro Rosado
 * 
 * @param pin Número del pin analógico a leer
 * 
 * pin: int -> leerValorAnalogico() -> double
 * 
 * Lee el valor digitalizado del pin analógico especificado y lo convierte a voltios.
 */
double leerValorAnalogico(int pin) {
  return ((analogRead(pin) * 3.3) / 1023) + BIASsensor;
}

/**
 * @brief Lee el valor del sensor de Ozono
 * @author Alejandro Rosado
 * 
 * leerOzono() -> double
 * 
 * Lee los valores de los pines del sensor de Ozono y calcula la diferencia de voltaje
 * entre el pin de Ozono y el pin de referencia.
 */
double leerOzono() {
  double VOzono = leerValorAnalogico(PIN_VOzono);
  double Vref = leerValorAnalogico(PIN_Vref);
  return abs(VOzono - Vref);
}

/**
 * @brief Lee la temperatura del sensor
 * @author Alejandro Rosado
 * 
 * leerTemperatura() -> double
 * 
 * Lee el valor del sensor de temperatura y lo convierte a grados Celsius,
 * aplicando un factor de calibración.
 */
double leerTemperatura() {
  double Vtemp = leerValorAnalogico(PIN_Vtemp);
  double factorCalibacion = 26.10 / 18.87;
  return (Vtemp * 125 / 10) * factorCalibacion;
}

/**
 * @brief Calcula la concentración de ozono
 * @author Alejandro Rosado
 * 
 * @param valorOzono Valor leído del sensor de Ozono
 * 
 * valorOzono: double -> obtenerConcentracionOzono() -> double
 * 
 * Calcula la concentración de ozono a partir del valor leído del sensor,
 * utilizando la sensibilidad y ganancia del sensor.
 */
double obtenerConcentracionOzono(double valorOzono) {
  double M = (SensibilidadSensor * GananciaSensor * 0.000001);
  return abs(valorOzono / M);
}

/**
 * @brief Corrige la concentración de Ozono basada en la temperatura
 * @author Alejandro Rosado
 * 
 * @param temperatura Temperatura ambiente actual
 * @param concentracionOzono Concentración de Ozono sin corregir
 * 
 * temperatura: double, concentracionOzono: double -> corregirConcentracionOzono() -> double
 * 
 * Aplica correcciones a la concentración de Ozono basadas en la temperatura actual,
 * incluyendo correcciones por desplazamiento de cero y sensibilidad.
 */
double corregirConcentracionOzono(double temperatura, double concentracionOzono) {
  double zeroShift = (temperatura > 30) ? (temperatura - 30) * 0.0066 : 0;
  double spanCorreccion = 1 + (temperatura - 20) * 0.003;
  double concentracionCorregida = (concentracionOzono - zeroShift) * spanCorreccion;
  return abs(pendiente * concentracionCorregida * intercepto);
}