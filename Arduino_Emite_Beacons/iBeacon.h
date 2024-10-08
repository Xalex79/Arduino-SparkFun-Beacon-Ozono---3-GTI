#ifndef IBEACON_H
#define IBEACON_H

#include <bluefruit.h>

// Definiciones de pines
#define PIN_VOzono 5
#define PIN_Vref 28
#define PIN_Vtemp 29

// Constantes del sensor
const double SensibilidadSensor = -35.35;
const int GananciaSensor = 499;
const int BIASsensor = -0.025;

// Constantes de calibración
const float pendiente = 0.598082;
const float intercepto = 2.411722;

// Estructura para almacenar mediciones
struct MedicionSensor {
  double temperatura;
  double GasOzonoCorregido;
};

// Declaraciones de funciones
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
void startAdvertising(MedicionSensor medicion);

/**
 * @brief Obtiene una medición de los sensores de ozono y temperatura
 * @author Alejandro Rosado
 * 
 * obtenerMedicionSensor() -> MedicionSensor
 * 
 * Lee los valores de los sensores de ozono y temperatura, realiza los cálculos
 * necesarios para obtener las mediciones corregidas y las devuelve en una estructura MedicionSensor.
 */
MedicionSensor obtenerMedicionSensor();

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
double leerValorAnalogico(int pin);

/**
 * @brief Lee el valor del sensor de Ozono
 * @author Alejandro Rosado
 * 
 * leerOzono() -> double
 * 
 * Lee los valores de los pines del sensor de Ozono y calcula la diferencia de voltaje
 * entre el pin de Ozono y el pin de referencia.
 */
double leerOzono();

/**
 * @brief Lee la temperatura del sensor
 * @author Alejandro Rosado
 * 
 * leerTemperatura() -> double
 * 
 * Lee el valor del sensor de temperatura y lo convierte a grados Celsius,
 * aplicando un factor de calibración.
 */
double leerTemperatura();

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
double obtenerConcentracionOzono(double valorOzono);

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
double corregirConcentracionOzono(double temperatura, double concentracionOzono);

#endif // IBEACON_H