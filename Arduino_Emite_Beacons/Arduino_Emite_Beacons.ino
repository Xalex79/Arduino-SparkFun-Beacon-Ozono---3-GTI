// -*- mode: c++ -*-
// ----------------------------------------------------
/**
  @file Arduino_Emite_Beacons.ino
  @brief Implementación de un iBeacon básico usando un SparkFun nRF52840 Mini
  @author Jordi Bataller Mascarell
  @author Alejandro Rosado Jiménez
  @version 0.3 - En Desarrollo
  @date 25-09-2024
  
  Este archivo contiene la implementación de un iBeacon básico utilizando
  el módulo Bluefruit nRF52 en un SparkFun nRF52840 Mini. El dispositivo  
  actúa como un beacon BLE (Bluetooth Low Energy), transmitiendo datos de 
  identificación y mediciones de sensores.
  
  Características principales:
  - Configuración y inicio del advertising BLE (Bluetooth Low Energy)
  - Lectura de sensores de Ozono ozono y temperatura (en ppm y ºC respectivamente)
  - Cálculo y corrección de concentración de Ozono -> En Proceso de Ajuste
  
  Adaptado de https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/adafruitbluefruit
 */
// ----------------------------------------------------
#include <bluefruit.h>
BLEUart bleuart;


#define PIN_VOzono 5 //pin nº 1 Sensor Ozono
#define PIN_Vref 28 //pin nº 2 Sensor Ozono
#define PIN_Vtemp 29 //pin nº 3 Sensor Ozono

//Pines 6 y 7 del sensor usados como GND y VCC 3.3v respectivamente 


const double SensibilidadSensor = -35.35; //Según el QR del sensor
const int GananciaSensor = 499; //499 kV/A
const int BIASsensor = -0.025; //-25 mV

//Struct para almacenar los valores de temperatura y GasOzonoCorregido de una medicion 
struct MedicionSensor {
  double temperatura;
  double GasOzonoCorregido;
};

// ----------------------------------------------------
// setup
// ----------------------------------------------------
void setup() {
   //
   //
   //
   Serial.begin(115200);
   while ( !Serial ) delay(10);

   Serial.println("-----------------------\n");
   Serial.println(" PRUEBAS iBeacon  ");
   Serial.println("-----------------------\n");

   //
   // Initialise the Bluefruit module
   //
   Serial.println("Initialise the Bluefruit nRF52 module");
   Bluefruit.begin();

   //
   // Set the advertised device name (keep it short!)
   //
   Serial.println("Setting Device Name to SoyBACON " );

   Bluefruit.setName("SoyBACON");
   Bluefruit.ScanResponse.addName();

   // Obtener la primera medición de los sensores
   MedicionSensor medicion = obtenerMedicionSensor();

   //
   // Iniciar advertising con los valores de la medición
   //
   startAdvertising(medicion);

   //
   //
   //
   Serial.println("\nAdvertising");

} // setup()


/**
  @brief Proceso de advertising BLE (Bluetooth Low Energy) con los valores de la primera medición
  @author Alejandro Rosado Jiménez
  
  @param medicion Estructura que contiene la temperatura y concentración de Ozono corregida
  
  medicion: MedicionSensor -> startAdvertising() -> void
  
  Esta función configura y inicia el proceso de advertising BLE (Bluetooth Low Energy).
  Configura el paquete de advertising con flags, la potencia de transmisión,
  el nombre del dispositivo y los datos de beacon.
 */
void startAdvertising(MedicionSensor medicion) {

   Serial.println( " startAdvertising() " );

   Serial.println( " Bluefruit.Advertising.stop(); ");
   Bluefruit.Advertising.stop(); // Detiene el advertising por si acaso antes de empezar

   //
   // Advertising packet
   //
   Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
   Bluefruit.Advertising.addTxPower();

   // Incluye el nombre
   Serial.println( " Bluefruit.Advertising.addName(); " );
   Bluefruit.Advertising.addName();

   //
   //
   //
   Serial.println( " Bluefruit.Advertising.setBeacon( elBeacon ); ");

   uint8_t beaconUUID[16] = {
     'E', 'P', 'S', 'G', '-', 'G', 'T', 'I',
     '-', 'A', 'L', 'E', 'X', '-', '3', 'A'
     };
   
   //Esta línea era para probar la transmisión antes de tener medidas reales
   //BLEBeacon elBeacon( beaconUUID, 12, 34, 73 ); //El beacon en Android lo detecta correctamente
   
   // Añadir los datos de concentración de ozono "corregida" y temperatura al beacon en ppm y ºC respectivamente
   BLEBeacon elBeacon(beaconUUID, medicion.GasOzonoCorregido, medicion.temperatura, 73);
   
   elBeacon.setManufacturer( 0x004c ); // aple id
   Bluefruit.Advertising.setBeacon( elBeacon );

   //
   //
   //
   Bluefruit.Advertising.restartOnDisconnect(true);
   Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
   Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
   Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds

   Serial.println( " Bluefruit.Advertising.start(0);" );

} // startAdvertising ()


// ----------------------------------------------------
// ----------------------------------------------------
namespace Loop {
   int cont = 0;
   unsigned long lastMeasurementTime = 0; // Tiempo de la última medición
   const unsigned long measurementInterval = 60000; // Intervalo de 1 minuto
};
// ....................................................
void loop() {

   using namespace Loop;

   cont++;

   // Comprobar si ha pasado un minuto
   unsigned long currentMillis = millis();
   if (currentMillis - lastMeasurementTime >= measurementInterval) {
      lastMeasurementTime = currentMillis; // Actualizar el tiempo de la última medición

      // Obtener una nueva medición de los sensores
      MedicionSensor medicion = obtenerMedicionSensor();

      // Reiniciar advertising con los nuevos valores de medición
      startAdvertising(medicion);
   }

   delay(3000);

   Serial.print( " ** loop cont=" );
   Serial.println( cont );
} // ()
// ----------------------------------------------------
// ----------------------------------------------------

/**
  @brief Obtener las mediciones de los sensores de ozono y temperatura corregidas
  @author Alejandro Rosado Jiménez
  
  obtenerMedicionSensor() -> MedicionSensor
  
  @returns Estructura con los valores de ozono y temperatura corregidas
 */
MedicionSensor obtenerMedicionSensor() {
    double temperatura = leerTemperatura();
    double valorOzono = leerOzono();
    double concentracionOzono = obtenerConcentracionOzono(valorOzono);
    double concentracionCorregida = corregirConcentracionOzono(temperatura, concentracionOzono) * 10; //Para tener 100ppm de resolución en vez de 10ppm

    MedicionSensor medicion;
    medicion.temperatura = round(temperatura);
    medicion.GasOzonoCorregido = concentracionCorregida;

    Serial.print("Concentración de Ozono corregida: ");
    Serial.print(concentracionCorregida); //HAY QUE CALIBRAR EL OZONO
    Serial.print(" ppm x 10. Concrentración real medida: "); //Para tener 100ppm de resolución en vez de 10ppm
    Serial.print(concentracionCorregida / 10);
    Serial.println(" ppm");

    Serial.print("Temperatura: ");
    Serial.print(round(temperatura)); //HAY QUE CALIBRAR LA TEMPERATURA
    Serial.print("ºC redondeados. Temperatura real medida: ");
    Serial.print(temperatura);
    Serial.println("ºC");
    

    return medicion;
}

/**
  @brief Lee el analógico del ADC de 12 bits y lo convierte a voltaje
  @author Alejandro Rosado Jiménez
  
  pin: int -> leerValorAnalogico() -> double
  
  @param pin El número del pin analógico a leer
  @returns El voltaje estimado a partir de la lectura analógica (después de conversión digital 2^12)
 */
double leerValorAnalogico(int pin) {
  // Se lee el valor digitalizado del pin analógico y se convierte a voltios
  //Serial.print("Valor pin: ");
  //Serial.println(analogRead(pin));
  return ((analogRead(pin) * 3.3) / 1023) + BIASsensor;
}

/**
  @brief Lee el valor del Ozono del sensor y lo convierte a un valor en voltios
  @author Alejandro Rosado Jiménez
  
  leerOzono() -> double
  
  @returns La diferencia de voltaje entre el pin VOzono del Ozono y Vref el de referencia, en voltios
 */
double leerOzono() {
  // Leer el valor del pin del Ozono y el de referencia (ambos analógicos)
  double VOzono = leerValorAnalogico(PIN_VOzono);
  double Vref = leerValorAnalogico(PIN_Vref);

  // Calcular la diferencia de voltaje
  double lecturaFinal = VOzono - Vref; 

  // Asegurarse de que la lectura no sea negativa (esto de momento no es muy preciso)
  if(lecturaFinal < 0) {
  lecturaFinal = lecturaFinal * (-1);
  }

  return lecturaFinal;
}

/**
  @brief Leer la temperatura del sensor en ºC
  @author Alejandro Rosado Jiménez
  
  leerTemperatura() -> double
  
  @returns La temperatura leída en ºC
 */
double leerTemperatura() {
  double Vtemp = leerValorAnalogico(PIN_Vtemp);

  //double ValorTemperatura = 143.1324 * Vtemp - 29.6136; 
  double ValorTemperatura =  (Vtemp * 125) / 10; //CALIBRAR CON TERMOMETRO
  return ValorTemperatura;
}

/**
  @brief Calcula la concentración de ozono a partir del valor leído
  @author Alejandro Rosado Jiménez
  
  valorOzono: double -> obtenerConcentracionOzono() -> double
  
  @param valorOzono El valor de Ozono leído del sensor
  @returns La concentración de ozono calculada
 */
double obtenerConcentracionOzono(double valorOzono) {
  double M = (SensibilidadSensor * GananciaSensor * 0.000001);
  //Serial.println(M);
  //double M = -0.017639;
  //double M = 0.007485;
  double concentracionOzono = valorOzono / M; 
  return concentracionOzono;
}

/**
  @brief Corrige la concentración de Ozono basada en la temperatura (No es relevante para nuestra aplicación, pero lo añado igual)
  @author Alejandro Rosado Jiménez
  
  temperatura: double, concentracionOzono: double -> corregirConcentracionOzono() -> double
  
  @param temperatura La temperatura ambiente actual
  @param concentracionOzono La concentración de Ozono sin corregir
  @returns La concentración de Ozono corregida por temperatura
 */
double corregirConcentracionOzono(double temperatura, double concentracionOzono) {
  // Corregir el "Zero Shift" (ppm/°C para temperaturas mayores de 30°C)
  double zeroShift = 0;
  if (temperatura > 30) {
    zeroShift = (temperatura - 30) * 0.0066;  // Aplicar corrección para desplazamiento de cero (Según la documentación, pero no es muy relevante por la imprecisión del sensor)
  }

  // Corregir la "Sensibilidad" (%/°C respecto a los 20°C)
  double spanCorreccion = 1 + (temperatura - 20) * 0.003;  // 0.3%/°C sobre 20°C (Según la documentación)

  // Aplicar las correcciones
  double concentracionCorregida = (concentracionOzono - zeroShift) * spanCorreccion;

  return concentracionCorregida;
}

// ----------------------------------------------------
// ----------------------------------------------------