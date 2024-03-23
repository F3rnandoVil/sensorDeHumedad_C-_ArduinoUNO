/*☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄
☄                                                                            ☄
☄       DATOS GENERALES:                                                     ☄
☄      NOMBRE:      Sensor de Humedad                                        ☄
☄      FECHA:       23 de marzo de 2024.                                     ☄
☄      VERSIÓN:     1.2.                                                     ☄
☄                                                                            ☄
☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄☄*/


#include <LiquidCrystal_I2C.h>

#define sensorPin A0    // Pin analógico donde está conectado el sensor de humedad
#define buttonPin 6     // Pin donde está conectado el botón
#define buzzerPin 7     // Pin donde está conectado el zumbador (buzzer)
#define RGB_RED 5       // Pin para el color rojo del LED RGB
#define RGB_GREEN 3     // Pin para el color verde del LED RGB
#define RGB_BLUE 4      // Pin para el color azul del LED RGB
#define relayPin 9      // Pin donde está conectado el relé para la bomba de agua
#define sensorMin 1023  // Valor mínimo de lectura del sensor
#define sensorMax 0     // Valor máximo de lectura del sensor

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Inicialización de la pantalla LCD

byte caracter1[] = {
  B00100,
  B00100,
  B01010,
  B10001,
  B10001,
  B11011,
  B01110,
  B00100
};

byte caracter2[] = {
  B11110,
  B10010,
  B01010,
  B00110,
  B01111,
  B01001,
  B00101,
  B00010
};

bool modoAutomatico = false;  // Bandera para el modo automático

void setup() {
  Serial.begin(9600);  // Inicialización de la comunicación serial
  lcd.init();          // Inicialización de la pantalla LCD
  lcd.createChar(0, caracter1);  // Creación de caracteres personalizados para la LCD
  lcd.createChar(1, caracter2);
  pinMode(RGB_RED, OUTPUT);      // Configuración de los pines como salidas
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(relayPin, OUTPUT);
  lcd.backlight();                // Encender retroiluminación de la LCD
}

void loop() {
  int sensorValue = leerSensor();  // Leer el sensor de humedad
  int percentValue = map(sensorValue, sensorMin, sensorMax, 0, 100);  // Convertir valor analógico a porcentaje

  if (digitalRead(buttonPin) == HIGH) {  // Verificar estado del botón
    displayModeOnLCD();  // Mostrar el modo en la LCD
    modoAutomatico = !modoAutomatico; // Cambiar el estado del modo automático
    delay(200); // Pequeña demora para evitar el rebote del botón
  }

  if (modoAutomatico) {
    encenderVioleta();  // Encender el LED RGB en color violeta
    displayLcd(sensorValue, percentValue);  // Mostrar valores en la LCD
    serialPrint(sensorValue, percentValue);  // Mostrar valores por comunicación serial

    if (percentValue <= 85) {
      digitalWrite(relayPin, LOW);  // Encender la bomba de agua
      tone(7,1500,100);  // Emitir tono con el zumbador
      delay(100);
      tone(7,1200,100);
      delay(100);
      tone(7,1000,100);
    } else {
      digitalWrite(relayPin, HIGH); // Apagar la bomba de agua
    }
  } else {
    digitalWrite(relayPin, HIGH); // Apagar la bomba de agua en modo manual
    displayLcd(sensorValue, percentValue);  // Mostrar valores en la LCD
    serialPrint(sensorValue, percentValue);  // Mostrar valores por comunicación serial
    ledIndicator(percentValue);  // Controlar el color del LED RGB según la humedad
  }

  delay(500); // Pequeña pausa entre iteraciones del bucle principal
}

int leerSensor() {
  int sensorValue = analogRead(sensorPin);  // Lectura del valor del sensor de humedad

  return sensorValue;
}

// Función para mostrar valores en la LCD
void displayLcd(uint16_t data, uint16_t data2){
  lcd.clear();  // Limpiar la pantalla LCD
  lcd.setCursor(0, 0);
  lcd.write(byte(0)); // Mostrar un carácter personalizado en la primera línea
  lcd.setCursor(12, 0); // Posicionar cursor en la primera línea
  lcd.print(data2); // Mostrar porcentaje de humedad
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.write(byte(1)); // Mostrar otro carácter personalizado en la segunda línea
  lcd.setCursor(12, 1);
  lcd.print(data); // Mostrar valor analógico del sensor
}

// Función para mostrar valores por comunicación serial
void serialPrint(uint16_t data, uint16_t data2) {
  Serial.print("Analogico: ");
  Serial.println(data);
  Serial.print("Humedad del suelo: ");
  Serial.print(data2);
  Serial.println("%");
}

// Función para controlar el color del LED RGB según la humedad
void ledIndicator(uint16_t data) {
  if (data == 100) {
    // Azul completo si la humedad es del 100%
    analogWrite(RGB_RED, 0);
    analogWrite(RGB_GREEN, 0);
    analogWrite(RGB_BLUE, 255);
  } else if (data >= 75) {
    // Verde completo si la humedad es mayor o igual al 75%
    analogWrite(RGB_RED, 0);
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 0);
  } else if (data >= 50) {
    // Blanco (todo encendido) si la humedad es mayor o igual al 50%
    analogWrite(RGB_RED, 255);
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 255);
  } else if (data >= 25) {
    // Amarillo (rojo + verde) si la humedad es mayor o igual al 50%
    analogWrite(RGB_RED, 200);
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 0);
  } else {
    // Rojo completo si la humedad es menor al 25%
    analogWrite(RGB_RED, 255);
    analogWrite(RGB_GREEN, 0);
    analogWrite(RGB_BLUE, 0);
  }
}

// Función para encender el LED RGB en color violeta
void encenderVioleta() {
  int redIntensity = 128;   // Intensidad roja para formar violeta
  int greenIntensity = 0; // Intensidad verde (no se usa en violeta)
  int blueIntensity = 255;    // Intensidad azul para formar violeta

  // Establecer intensidades en los pines RGB
  analogWrite(RGB_RED, redIntensity);
  analogWrite(RGB_GREEN, greenIntensity);
  analogWrite(RGB_BLUE, blueIntensity);
}

// Función para mostrar el modo (automático o manual) en la LCD
void displayModeOnLCD() {
  lcd.clear(); // Limpiar la pantalla LCD
  lcd.setCursor(0, 0);
  if (modoAutomatico) {
    lcd.print("Modo: Manual"); // Mostrar "Modo: Manual" en la primera línea
    tone(7,1500,100); // Emitir tono con el zumbador
    delay(100);
    tone(7,1200,100);
    delay(100);
    tone(7,1000,100);
  } else {
    lcd.print("Modo: Auto"); // Mostrar "Modo: Auto" en la primera línea
    tone(7,1000,100); // Emitir tono con el zumbador
    delay(100);
    tone(7,1200,100);
    delay(100);
    tone(7,1500,100);
  }
  delay(1000); // Mostrar el modo durante 1 segundo
}
