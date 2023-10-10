/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL29DcbQ7Ho"
#define BLYNK_TEMPLATE_NAME "sensor"
#define BLYNK_AUTH_TOKEN "8mJg32lIXYQWq7lEyCBBNjh3v8ubPXkx"

/* Comment this out to disable prints and save space */
// #define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "BLYNK";
char pass[] = "28350507";

BlynkTimer timer;

#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bits/stdc++.h>

#include <iostream>
//A tela possui 16 caracteres
//1234567890123456
//=? tecle =

//D8     NAO DA como entrada, mas DA como saida
//Tx==1  NAO DA como entrada, mas DA como saida
//Rx==3
//sk==10
//S3==9

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte n_rows = 4;
const byte n_cols = 4;
char keys[n_rows][n_cols] = { { '1', '2', '3', '4' }, { '4', '5', '6', '-' }, { '7', '8', '9', '*' }, { 'c', '0', '=', '/' } };
byte colPins[n_rows] = { D6, D5, D4, D3 };
byte rowPins[n_cols] = { 10, 9, 3, D7 };
Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, n_rows, n_cols);

int BUZZ_PIN = D8;

// recebe quantidade de milisegundos do bip que o buzz irá fazer
void buzzBip() {
  digitalWrite(BUZZ_PIN, HIGH);
  delay(50);
  digitalWrite(BUZZ_PIN, LOW);
}

void buzzBip(int t) {
  digitalWrite(BUZZ_PIN, HIGH);
  delay(t);
  digitalWrite(BUZZ_PIN, LOW);
}

//Declaramos os pinos que vamos a usar
int analogPin = A0;  //Analogico A5 para o sensor

//Declaramos uma variável para armazenar o valor que nos devolve o sensor
int val = 0;

void setup() {
  // Debug console
  Serial.begin(115200);

  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);

  // Setup a function to be called every second
  //timer.setInterval(100L, myTimerEvent);

  pinMode(analogPin, INPUT);  //Configuramos o pino analogico A5 como entrada
  pinMode(BUZZ_PIN, OUTPUT);

  lcd.init();  // INICIALIZA O DISPLAY LCD
  lcd.clear();
  lcd.backlight();  // HABILITA O BACKLIGHT (LUZ DE FUNDO)
}

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
int start = 0;
unsigned long ini;
unsigned long now;

void myTimerEvento() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, val);
  val = analogRead(analogPin);  //Lemos o valor analogico que nos devolve o sensor
  //Serial.println(val);          //Obtemos o valor do sensor por serial
  if (val > 100)  //Si o valor é maior que 100
  {
    if (start == 0) {
      buzzBip(500);
      ini = millis();
      start = 1;
    }
  }
  now = millis();
  if (start and now - ini == 10000) {
    buzzBip(500);
    Serial.println("TEMPO");
    //Blynk.logEvent("alerta", "Possivel pessoa desconhecida");
    //Blynk.logEvent("conhecido", "Possível pessoa conhecida.");
    start = 0;
  }
}

void escrevePrimeiraLinha(char vet[]) {
  lcd.setCursor(0, 0);
  lcd.print("                ");  //limpa
  lcd.setCursor(0, 0);            //volta o cursor
  int i = 0;
  for (; vet[i] != '\0'; i++) {
    lcd.print(vet[i]);
  }
}

void limpa() {
  lcd.setCursor(0, 1);            //SETA A POSIÇÃO EM QUE O CURSOR INCIALIZA(LINHA 1)
  lcd.print("                ");  //limpa
  lcd.setCursor(0, 0);
  lcd.print("                ");  //limpa
}

void loop() {
  Blynk.run();
  timer.run();  // Initiates BlynkTimer
  minhalogica();
}

char key;
void minhalogica() {
  key = myKeypad.getKey();
  if (key != '\0') {
    if (key == '1') {
      Blynk.logEvent("alerta", "Possivel pessoa desconhecida");
    } else if (key == '2') {
      Blynk.logEvent("intruso", "Possivel pessoa desconhecida!!");
    } else if (key == '3') {
      Blynk.logEvent("conhecido", "Possível pessoa conhecida.");
    }
    Serial.println(key);
  }
};
