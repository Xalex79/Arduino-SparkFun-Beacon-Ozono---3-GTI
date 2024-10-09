# Arduino SparkFun Beacon Ozono - 3º GTI

Documentación del Repositorio:

Este proyecto implementa un sistema de emisión de beacons utilizando un módulo Bluefruit nRF52. El sistema está diseñado para medir la concentración de ozono y la temperatura, y a través de Bluetooth Low Energy (BLE), emite estos datos en forma de beacon.



Descripción General:


El proyecto consta de tres componentes principales:

1. Arduino_Emite_Beacons.ino: Archivo principal que contiene la lógica para inicializar el dispositivo y gestionar la emisión de datos de los sensores.

2. iBeacon.cpp: Implementación de las funciones que gestionan el proceso de advertising BLE y la obtención de datos de los sensores.

3. iBeacon.h: Definiciones de constantes, estructuras y declaraciones de funciones utilizadas en el proyecto.



Funcionamiento:

Inicialización: 
En el método setup(), se inicializa la comunicación serial y se configura el módulo Bluefruit. Se asigna un nombre al dispositivo y se obtienen las primeras mediciones de los sensores antes de iniciar el proceso de advertising BLE.

Emisión de Datos: 
En el bucle loop(), el programa realiza mediciones periódicas (cada 60 segundos) de la temperatura y la concentración de ozono, actualizando los datos que se emiten como beacon a través de BLE.

Advertising BLE: 
La función startAdvertising() configura el paquete de advertising con la información de los sensores y otros parámetros necesarios, y lo inicia para que otros dispositivos puedan detectarlo.



Estructuras y Funciones Clave:


Estructura MedicionSensor:

La estructura MedicionSensor almacena las mediciones obtenidas de los sensores:

struct MedicionSensor {
    double temperatura;             // Temperatura en grados Celsius
    double GasOzonoCorregido;      // Concentración de ozono corregida
};


Funciones Principales:

setup()

Inicializa el sistema y comienza el advertising.
loop()

Realiza las mediciones de los sensores y actualiza el beacon.
startAdvertising(MedicionSensor medicion)

Configura el proceso de advertising BLE utilizando los datos de medición.
obtenerMedicionSensor()

Obtiene las mediciones de los sensores de ozono y temperatura y devuelve una estructura MedicionSensor.


Funciones de lectura de sensores:


leerValorAnalogico(int pin): Lee el valor de un pin analógico y lo convierte a voltaje.

leerOzono(): Lee el valor del sensor de ozono.

leerTemperatura(): Lee la temperatura del sensor.


Funciones de cálculo:


obtenerConcentracionOzono(double valorOzono): Calcula la concentración de ozono.

corregirConcentracionOzono(double temperatura, double concentracionOzono): Corrige la concentración de ozono basada en la temperatura.



Ejemplo de Uso:


Configuración del Dispositivo:


Asegúrate de que el módulo Bluefruit nRF52 esté correctamente conectado al Arduino.

Carga el código en el Arduino a través del IDE.


Lectura de Datos:

Conecta un dispositivo compatible con BLE (como un teléfono móvil) y busca el dispositivo con el nombre "SoyBACON".

Escanea y observa cómo el dispositivo emite la información de concentración de ozono y temperatura en intervalos de 60 segundos.


Interpretación de Datos:

Cada beacon emitido incluirá la concentración de ozono corregida y la temperatura actual en el formato establecido.



Conclusión:

Este proyecto permite la emisión continua de datos de sensores a través de BLE, lo que lo hace útil para aplicaciones de monitoreo ambiental, como la vigilancia de la calidad del aire.
