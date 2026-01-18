#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

//------------- VARIABLES GLOBALES -------------
volatile bool cerrarPuerta = false; // "Bandera" para la interrupción
int cont = 0;
int cont2 = 0;
char numeros[4] = {'0','0','0','0'};
char clave[4] = {'2','0','0','6'};
char tecla;

Servo svm;
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Pines y Textos
#define touch 2
int buz = 11;
String texto1 = "INGRESE LA CLAVE";
String texto2 = "CLAVE CORRECTA";
String texto3 = "CLAVE INCORRECTA";
String texto4 = "SISTEMA BLOQUEADO";
String texto5 = "INTENTE DE NUEVO";
String texto6 = "ACCESO PERMITIDO";

// Config Teclado
const byte FILAS = 4;
const byte COLUMNAS = 4;
char TECLADO[FILAS][COLUMNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte PINESFILAS[FILAS] = {3,4,5,6};
byte PINESCOLUMNAS[COLUMNAS] = {7,8,9,10};
Keypad keypad = Keypad(makeKeymap(TECLADO), PINESFILAS, PINESCOLUMNAS, FILAS, COLUMNAS);

// ------------- INTERRUPCIÓN (CORTA) -------------
void isoCerrar() {
  cerrarPuerta = true; // Solo avisamos que hay que cerrar
}

void setup() {
  Serial.begin(9600);
  svm.attach(12);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(touch, INPUT);
  // Usamos RISING o FALLING en lugar de CHANGE para evitar que se active dos veces
  attachInterrupt(digitalPinToInterrupt(touch), isoCerrar, RISING); 
  pinMode(buz, OUTPUT);
  
  svm.write(99); 
}

void loop() {
  // 1. REVISAR SI LA INTERRUPCIÓN SE ACTIVÓ
  if (cerrarPuerta) {
    svm.write(99);
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("CERRANDO PUERTA");
    delay(1500);
    lcd.clear();
    cont = 0;
    cerrarPuerta = false; // Resetear la bandera
  }

  // 2. LÓGICA NORMAL DEL TECLADO
  if (cont == 0 && !cerrarPuerta) {
    lcd.setCursor(0,0);
    lcd.print(texto1);
    lcd.setCursor(6,1);
    lcd.print("____");
  }

  tecla = keypad.getKey();
  if (tecla) {
    numeros[cont] = tecla;
    lcd.setCursor(6 + cont, 1);
    lcd.print("*"); 
    cont++;
    
    if (cont == 4) {
      delay(300);
      if (numeros[0]==clave[0] && numeros[1]==clave[1] && 
          numeros[2]==clave[2] && numeros[3]==clave[3]) {
        svm.write(0);
        lcd.clear();
        lcd.print(texto2);
        lcd.setCursor(1,1);
        lcd.print(texto6);
        delay(3000);
        cont = 0;
        cont2 = 0;
        lcd.clear();
      } 
      else {
        digitalWrite(buz, HIGH);
        lcd.clear();
        lcd.print(texto3);
        lcd.setCursor(1,1);
        lcd.print(texto5);
        delay(2000);
        digitalWrite(buz, LOW);
        cont2++;
        cont = 0;
        lcd.clear();
        
        if (cont2 >= 3) {
          for (int i = 20; i >= 0; i--) {
            lcd.setCursor(0,0);
            lcd.print(texto4);
            lcd.setCursor(7,1);
            lcd.print(i);
            lcd.print("s ");
            delay(1000);
          }
          cont2 = 0;
          lcd.clear();
        }
      }
    }
  }
}