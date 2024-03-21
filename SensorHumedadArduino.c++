#include <LiquidCrystal_I2C.h>

#define sensorPin A0 // Se asume que el sensor está conectado al pin analógico A0
#define buttonPin 6  // Pin donde está conectado el botón
#define buzzerPin 7  // Pin donde está conectado el zumbador
#define RGB_RED 5
#define RGB_GREEN 3
#define RGB_BLUE 4
#define relayPin 9
#define sensorMin 1023   // Valor mínimo de lectura del sensor
#define sensorMax 0 // Valor máximo de lectura del sensor

LiquidCrystal_I2C lcd(0x27, 16, 2);

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

bool modoAutomatico = false; // Bandera para el modo automático

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.createChar(0, caracter1);
  lcd.createChar(1, caracter2);
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(relayPin, OUTPUT);
  lcd.backlight();

}

void loop() {
  int sensorValue = analogRead(sensorPin);
  int percentValue = map(sensorValue, sensorMin, sensorMax, 0, 100);
  
  // Lectura del estado del botón
  if (digitalRead(buttonPin) == HIGH) {
    displayModeOnLCD();
    modoAutomatico = !modoAutomatico; // Cambia el estado del modo automático
    delay(200); // Pequeña demora para evitar el rebote del botón
  }

  if (modoAutomatico) {
    
    // Si estamos en modo automático, controlar el relé según el porcentaje de humedad
   	encenderVioleta();
    displayLcd(sensorValue, percentValue);
    serialPrint(sensorValue, percentValue);
   	
    if (percentValue <= 85) {
      digitalWrite(relayPin, LOW); // Encender el relé
      tone(7,1500,100);
      delay(100);
      tone(7,1200,100);
      delay(100);
      tone(7,1000,100);
    } else {
      digitalWrite(relayPin, HIGH); // Apagar el relé
    }
  } else {
    // Si estamos en modo manual, ejecutar las funciones de LCD, impresión serial y control LED
    digitalWrite(relayPin, HIGH);
    displayLcd(sensorValue, percentValue);
    serialPrint(sensorValue, percentValue);
    ledIndicator(percentValue);
  }

  delay(500);
}

void displayLcd(uint16_t data, uint16_t data2){
  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.write(byte(0)); 
  lcd.setCursor(12, 0); 
  lcd.print(data2); 
  lcd.print("%");

  lcd.setCursor(0, 1); 
  lcd.write(byte(1));
  lcd.setCursor(12, 1); 
  lcd.print(data); 
}

void serialPrint(uint16_t data, uint16_t data2) {
  Serial.print("Analogico: ");
  Serial.println(data);
  Serial.print("Humedad del suelo: ");
  Serial.print(data2);
  Serial.println("%");
}

void ledIndicator(uint16_t data) {
  if (data == 100) {
    analogWrite(RGB_RED, 0);
    analogWrite(RGB_GREEN, 0);
    analogWrite(RGB_BLUE, 255); // Encender completamente el azul
  } else if (data >= 75) {
    analogWrite(RGB_RED, 0);
    analogWrite(RGB_GREEN, 255); // Encender completamente el verde
    analogWrite(RGB_BLUE, 0);
  } else if (data >= 50) {
    analogWrite(RGB_RED, 200); // Encender completamente el rojo
    analogWrite(RGB_GREEN, 255);
    analogWrite(RGB_BLUE, 0); // Cambiar a rojo
  } else if (data >= 25) {
    analogWrite(RGB_RED, 200); // Encender completamente el rojo
    analogWrite(RGB_GREEN, 0); // Encender completamente el verde
    analogWrite(RGB_BLUE, 255); // Cambiar a amarillo (rojo + verde)
  } else {
    analogWrite(RGB_RED, 255); // Encender completamente el rojo
    analogWrite(RGB_GREEN, 0);
    analogWrite(RGB_BLUE, 0);
  }
}



void encenderVioleta() {
  // Definir los valores de intensidad para rojo y azul (sin verde para formar violeta)
  int redIntensity = 128;
  int greenIntensity = 255;
  int blueIntensity = 0;

  // Establecer los pines RGB con los valores de intensidad correspondientes
  analogWrite(RGB_RED, redIntensity);
  analogWrite(RGB_GREEN, greenIntensity);
  analogWrite(RGB_BLUE, blueIntensity);
}


void displayModeOnLCD() {
  lcd.clear(); 
  lcd.setCursor(0, 0);
  if (modoAutomatico) {
    lcd.print("Modo: Manual");
    tone(7,1500,100);
    delay(100);
    tone(7,1200,100);
    delay(100);
    tone(7,1000,100);
  } else {
    lcd.print("Modo: Auto");
    tone(7,1000,100); 
    delay(100);
    tone(7,1200,100);
    delay(100);
    tone(7,1500,100);
  }
  delay(1000); // Mostrar el modo durante 1 segundo
}
