//====================================================
//BEGIN
// SENSOR E BLYNK
//====================================================
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

//Declaramos os pinos que vamos a usar
int analogPin = A0;  //Analogico A5 para o sensor
//====================================================
// SENSOR E BLYNK
//END
//====================================================

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

bool calculando = false;

// recebe quantidade de milisegundos do bip que o buzz irá fazer
void buzzBip(int milis) {
  digitalWrite(BUZZ_PIN, HIGH);
  //delay(milis);
  delay(50);
  digitalWrite(BUZZ_PIN, LOW);
}

void setup() {
  //===============================================
  //BEGIN
  // BLYNK
  //===============================================
  // Debug console
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(100L, myTimerEvent);
  pinMode(analogPin, INPUT);  //Configuramos o pino analogico A5 como entrada
  //===============================================
  // BLYNK
  //END
  //===============================================

  pinMode(BUZZ_PIN, OUTPUT);
  lcd.init();  // INICIALIZA O DISPLAY LCD
  lcd.clear();
  lcd.backlight();  // HABILITA O BACKLIGHT (LUZ DE FUNDO)
  /*
  lcd.setCursor(0, 0); //SETA A POSIÇÃO EM QUE O CURSOR INCIALIZA(LINHA 1)
  lcd.print("# PETIT  CEFET #"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
  //lcd.print("# CEFET  PETIT #"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
  lcd.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR INCIALIZA(LINHA 1)
  lcd.print(" VENHA CALCULAR "); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
 */
}

//Declaramos uma variável para armazenar o valor que nos devolve o sensor
int val = 0;
int mov = 0;  //0 = parado, 1 = movimentou
unsigned long ini;
unsigned long now;
int istimeout = 0;

int numSen = 0;
int senhas[10];  //> 10.000
char senhadig[10];

void buzzBip(int milis);
void movimentou(int numSen, int senhas[], char senhadig[]);

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, val);
  val = analogRead(analogPin);  //Lemos o valor analogico que nos devolve o sensor
  Serial.println(val);          //Obtemos o valor do sensor por serial
  if (val > 100 and mov == 0)   //Si o valor é maior que 100
  {
    mov = 1;
    buzzBip(500);
    movimentou(numSen, senhas, senhadig);
  }
}

char wrongpass[] = { { 'S' }, { 'e' }, { 'n' }, { 'h' }, { 'a' }, { ' ' }, { 'i' }, { 'n' }, { 'c' }, { 'o' }, { 'r' }, { 'r' }, { 'e' }, { 't' }, { 'a' }, { '!' }, '\0' };
char rightpass[] = { { 'S' }, { 'e' }, { 'n' }, { 'h' }, { 'a' }, { ' ' }, { 'c' }, { 'o' }, { 'r' }, { 'r' }, { 'e' }, { 't' }, { 'a' }, { '!' }, '\0' };
int recebesenha(char senhadig[]);

void printLinha2int(int num);
void printLinha1(char vet[]);

void checktimeout() {
  now = millis();
  if (now - ini >= 10000) {
    msgintruso();
    istimeout = 1;
  }
}

void msgintruso() {
  Blynk.logEvent("intruso", "Possivel pessoa desconhecida!!");
}

void msgconhecido() {
  Blynk.logEvent("conhecido", "Possível pessoa conhecida.");
}

void movimentou(int numSen, int senhas[], char senhadig[]) {
  buzzBip(500);
  int wrong = 0;
  int wrongs = 0;
  ini = millis();
  int tam;
  do {
    Serial.println("vou receber");
    tam = recebesenha(senhadig);  //Recebe senha do usuario
    Serial.println("recebi");
    Serial.println(" ");
    Serial.print("Senha: ");
    Serial.print(senhadig);
    Serial.println(" ");
    if (tam == 0) {//timeout
      break;
    } else if (tam > 4 and tam < 11) {  //se TRUE, verifica se existe essa senha
      int senha = senhaCharToInt(senhadig, tam);
      Serial.println(" ");
      Serial.print("Senha int: ");
      Serial.print(senha);
      Serial.println(" ");
      wrong = verificaSenha(senha, senhas, numSen);
      Serial.println(" ");
      Serial.print("valida: ");
      Serial.println(wrong);
      if (!wrong) {
        wrong = 1;
      } else {
        wrongs = -1;
        break;
      }
    } else {  //senha invalida (incorreta)
      wrong = 1;
    }
    if (wrong) {
      wrong = 0;
      wrongs++;
      printLinha2(wrongpass);
    }
    checktimeout();
    if(istimeout){
      break;
    }
  } while (wrongs < 3);
  if (!istimeout) {
    if (wrongs == 3) {
      msgintruso();
    } else {
      msgconhecido();
      printLinha2(rightpass);
    }
  }else{
    istimeout = 0;
  }
  mov = 0;
  limpa();
}

void limpa() {
  lcd.setCursor(0, 1);            //SETA A POSIÇÃO EM QUE O CURSOR INCIALIZA(LINHA 1)
  lcd.print("                ");  //limpa
  lcd.setCursor(0, 0);
  lcd.print("                ");  //limpa
}

void printLinha2int(int num) {
  //Serial.print(num);
  //Serial.print("\n");
  lcd.setCursor(0, 1);
  lcd.print("                ");  //limpa
  int numlog;
  if (num == 0) {
    numlog = 1;
  }
  int n = int(log10(numlog) + 1);
  lcd.setCursor(16 - n, 1);  //volta o cursor
  lcd.print(num);
}

void printLinha1(char vet[]) {
  lcd.setCursor(0, 0);
  lcd.print("                ");  //limpa
  lcd.setCursor(0, 0);            //volta o cursor
  int i = 0;
  for (; vet[i] != '\0'; i++) {
    lcd.print(vet[i]);
  }
}

void printLinha2(char vet[]) {
  lcd.setCursor(0, 1);
  lcd.print("                ");  //limpa
  lcd.setCursor(0, 1);            //volta o cursor
  int i = 0;
  for (; vet[i] != '\0'; i++) {
    lcd.print(vet[i]);
  }
}

int verificaSenha(int senhas[], int numSen, int senha) {
  for (int i = 0; i < numSen; i++) {
    if (senha == senhas[i]) {
      return 1;
    }
  }
  return 0;
}

//===================================================================================================================================================
int senhaCharToInt(char senhadig[], int tam) {
  int senha = 0;
  int mult = 1;
  for (int i = tam - 1; i >= 0; i--) {
    switch (senhadig[i]) {
      case '1':
        senha += mult;
        break;
      case '2':
        senha += mult * 2;
        break;
      case '3':
        senha += mult * 3;
        break;
      case '4':
        senha += mult * 4;
        break;
      case '5':
        senha += mult * 5;
        break;
      case '6':
        senha += mult * 6;
        break;
      case '7':
        senha += mult * 7;
        break;
      case '8':
        senha += mult * 8;
        break;
      case '9':
        senha += mult * 9;
        break;
      default:
        break;
    }
    mult *= 10;
  }
  return senha;
}

//Recebe uma senha do usuario, seja a do admin para um acao ou uma nova senha..
int recebesenha(char senhadig[]) {
  int pos = 0;
  char c = '\0';
  do {
    checktimeout();
    if (istimeout) {
      return 0;
    }
    c = myKeypad.getKey();
    if (c != NULL) {
      if (c >= '0' && c <= '9') {  //se c eh um numero
        senhadig[pos] = c;
        pos++;
        senhadig[pos] = '\0';
        printLinha2(senhadig);
        buzzBip(500);
      } else if (c == 'c') {
        buzzBip(100);
        //break;
      }
    }
  } while (pos < 10);
  return pos;
}

//Funcao um (1) para o menu
//digite a senha admin
//Senha incorreta //sai do menu
//Senha do admin:
char senhadm[] = { { 'S' }, { 'e' }, { 'n' }, { 'h' }, { 'a' }, { ' ' }, { 'd' }, { 'o' }, { ' ' }, { 'a' }, { 'd' }, { 'm' }, { 'i' }, { 'n' }, { ':' }, '\0' };
int funcao1(char senhadig[], int senhas[]) {
  limpa();                          //Limpa o display
  printLinha1(senhadm);             //escreve na primeira linha "Senha do admin:"
  int tam = recebesenha(senhadig);  //Recebe a senha do usuario (senha admin esperada)
  int senha = senhaCharToInt(senhadig, tam);
  return senha == senhas[0];  //Verifica se a senha Adm esta correta
}

//Funcao dois (2) para o menu
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= FUNCAO 2 -=-=-=
// digite o cod da senha (tem q exibir as senhas) ESCOLHER SENHA
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= FUNCAO 2 -=-=-=
int funcao2(int senhas[]) {
  //printLinha1();

  char c = printSenhasRetOp(senhas);
  int codigo = 0;
  switch (c) {
    case '1':
      codigo += 1;
      break;
    case '2':
      codigo += 2;
      break;
    case '3':
      codigo += 3;
      break;
    case '4':
      codigo += 4;
      break;
    case '5':
      codigo += 5;
      break;
    case '6':
      codigo += 6;
      break;
    case '7':
      codigo += 7;
      break;
    case '8':
      codigo += 8;
      break;
    case '9':
      codigo += 9;
      break;
    default:
      break;
  }
  return codigo;
}

void printSenhaPosicao(int senhas[], int pos, int linha) {
  lcd.setCursor(0, linha - 1);
  lcd.print("                ");  //limpa
  lcd.setCursor(0, linha - 1);
  lcd.print(pos);
  lcd.print(" - ");
  lcd.print(senhas[pos]);
}

//Escolha um codg:
char coddeseja[] = { { 'E' }, { 's' }, { 'c' }, { 'o' }, { 'l' }, { 'h' }, { 'a' }, { ' ' }, { 'u' }, { 'm' }, { ' ' }, { 'c' }, { 'o' }, { 'd' }, { 'g' }, { ':' }, '\0' };
char printSenhasRetOp(int senhas[]) {
  unsigned long ini = millis();
  const long meio = 5000;
  unsigned long now = ini + meio;
  char myKey = '\0';
  int pos = 0;
  int pant = -1;
  do {
    if ((now - ini) >= meio) {
      ini = millis();
      now = millis();
      //ALTERA O POS
      if (senhas[pos] == 0) {
        pos = 0;
      }
      //FAZ AS IMPRESSOES NO DISPLAY
      if (pos > 0) {
        //== PRINT L1 INT
        printSenhaPosicao(senhas, pos, 1);  //Imprime na linha 1 a senha de codigo pos
      } else {
        printLinha1(coddeseja);  //Imprime a mensagmem na linha 1 falando pra escolher um codigo
        pos--;                   //Vai faze imprimir no display a posicao 0 no codigo abaixo
      }
      //== PRINT L2 INT
      printSenhaPosicao(senhas, pos + 1, 2);  //Imprime na linha 2 a senha de codigo pos+1
      if (pos == 0) {
        pos = 1;
      } else {
        pos += 2;
      }
      myKey = myKeypad.getKey();
    }
    if (myKey != '\0') {
      buzzBip(500);
      break;
    }
    now = millis();
  } while (true);
  return myKey;
}

int verificaSenha(int senha, int senhas[], int numsenha) {
  for (int i = 0; i < numsenha; i++) {
    if (senha == senhas[i]) {
      return 1;
    }
  }
  return 0;
}

//===================================================================================================================================================
//===================================================================================================================================================
/*
  Menu
//#######################################################################################################################################################
//--------------------------------------------------------------------------------------------------------------------------------------------------- 1 -
//#######################################################################################################################################################
****cadastrar senha - OPCAO 1
//====================================================== FUNCAO 1 ======
      digite a senha admin                          
        Senha incorreta //sai do menu         
//====================================================== FUNCAO 1 ====== Retorna 1 se a senha tiver correta (prossegue) e 0 se não (break o menu)
        digite a nova senha 
          ERRO: Min 6 nums 
          Concluido
//#######################################################################################################################################################
//--------------------------------------------------------------------------------------------------------------------------------------------------- 2 -
//#######################################################################################################################################################
****remover senha   - OPCAO 2
//====================================================== FUNCAO 1 ======
      digite a senha admin                          
        Senha incorreta //sai do menu         
//====================================================== FUNCAO 1 ====== Retorna 1 se a senha tiver correta (prossegue) e 0 se não (break o menu)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= FUNCAO 2 -=-=-=
        digite o cod da senha (tem q exibir as senhas) ESCOLHER SENHA
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= FUNCAO 2 -=-=-=
          confirme (0 OU 1)
            senha removida //(op 1)
            //SAI DO MENU (op 0)
//#######################################################################################################################################################
//--------------------------------------------------------------------------------------------------------------------------------------------------- 3 -
//#######################################################################################################################################################
****mudar senha   - OPCAO 3
//====================================================== FUNCAO 1 ======
      digite a senha admin                          
        Senha incorreta //sai do menu         
//====================================================== FUNCAO 1 ====== Retorna 1 se a senha tiver correta (prossegue) e 0 se não (break o menu)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= FUNCAO 2 -=-=-=
        digite o cod da senha (tem q exibir as senhas) ESCOLHER SENHA
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= FUNCAO 2 -=-=-=
          digite a nova senha
            confirme
              senha alterada //caso não seja a admin
              Mudar senhaAdm? //(0 ou 1)SE FOR A SENHA ADMIN
//#######################################################################################################################################################
//--------------------------------------------------------------------------------------------------------------------------------------------------- 4 -
//#######################################################################################################################################################
****sair    - OPCAO 4
//#######################################################################################################################################################
//#######################################################################################################################################################
*/

//===================================================================================================================================================
//===================================================================================================================================================
//===================================================================================================================================================
//===================================================================================================================================================
//===================================================================================================================================================
//=========================================================================================================== LOOP
//===================================================================================================================================================

int tam = 100;
char vet[100];
float nums[100];
float vetOp[100];
int cont = 0;
bool clear = false;
float result = 0;
char myKey = '\0';


//1) Inserir senha
char um[] = { { '1' }, { ')' }, { ' ' }, { 'I' }, { 'n' }, { 's' }, { 'e' }, { 'r' }, { 'i' }, { 'r' }, { ' ' }, { 's' }, { 'e' }, { 'n' }, { 'h' }, { 'a' }, '\0' };
//2) Remover senha
char dois[] = { { '2' }, { ')' }, { ' ' }, { 'R' }, { 'e' }, { 'm' }, { 'o' }, { 'v' }, { 'e' }, { 'r' }, { ' ' }, { 's' }, { 'e' }, { 'n' }, { 'h' }, { 'a' }, '\0' };
//3) Mudar senha
char tres[] = { { '3' }, { ')' }, { ' ' }, { 'M' }, { 'u' }, { 'd' }, { 'a' }, { 'r' }, { ' ' }, { 's' }, { 'e' }, { 'n' }, { 'h' }, { 'a' }, '\0' };
//4) Sair do Menu
char quatro[] = { { '4' }, { ')' }, { ' ' }, { 'S' }, { 'a' }, { 'i' }, { 'r' }, { ' ' }, { 'd' }, { 'o' }, { ' ' }, { 'M' }, { 'e' }, { 'n' }, { 'u' }, '\0' };
//dois[0] = '5';


//Senha a incluir:
char senhainc[] = { { 'S' }, { 'e' }, { 'n' }, { 'h' }, { 'a' }, { ' ' }, { 'a' }, { ' ' }, { 'i' }, { 'n' }, { 'c' }, { 'l' }, { 'u' }, { 'i' }, { 'r' }, { ':' }, '\0' };

const long meio = 5000;
int sit = 0;
int numOp = 2;
int ant = 1;

int menu = 0;
int inicia = 0;



///O QUE ERA O LOOP
//CODIGO TA AQUI
int op;
void minhaLogica() {
  if (inicia == 0) {
    inicia++;
    senhas[0] = 12356;
    numSen++;
    int i = 1;
    for (; i < 10; i++) {
      senhas[i] = 0;
    }
  }
  myKey = myKeypad.getKey();
  if (myKey != '\0') {
    Serial.println(myKey);
    buzzBip(500);
    if (myKey == 'c') {  //APERTAR O BOTÃO DO MENU
                         //==================================================================================================
                         //==================================================== MENU ========================================
                         //==================================================================================================
      menu = 1;
      do {
        now = millis();
        sit = (now - ini) / meio;
        sit = sit % numOp;
        Serial.print("DENTRO: ");
        Serial.println(sit);
        if (sit == 0 and ant != 0) {
          printLinha1(um);
          printLinha2(dois);
          ant = 0;
        } else if (sit == 1 and ant != 1) {
          printLinha1(tres);
          printLinha2(quatro);
          ant = 1;
        }
        myKey = myKeypad.getKey();
        if (myKey != '\0') {
          buzzBip(500);
          if (myKey == 'c') {
            break;
          }
        }
        int senha;
        switch (myKey) {
          //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          //===========================================================================================================
          //=================================================== OPCAO 1 ===============================================
          //===========================================================================================================
          //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          case '1':
            menu = funcao1(senhadig, senhas);
            if (menu) {  //Chama a função que recebe a senha adm e confere se esta correta
              limpa();
              printLinha1(senhainc);
              int tam = recebesenha(senhadig);  //Recebe a senha do usuario (senha admin esperada)
              if (tam > 4 and tam < 11) {       //se TRUE, insere
                senhas[numSen] = senhaCharToInt(senhadig, tam);
                numSen++;
              }
              menu = 0;
            }
            break;
          //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          //===========================================================================================================
          //=================================================== OPCAO 2 ===============================================
          //===========================================================================================================
          //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          case 2:
            // comando(s)
            menu = funcao1(senhadig, senhas);  //Chama a função que recebe a senha adm e confere se esta correta
            if (menu) {
              int cod = funcao2(senhas);
              //senha removida //(op 1)
              //cod escolhido: X
              //RemoverSenha 0?1
              //SAI DO MENU (op 0)
              do {
                myKey = myKeypad.getKey();
              } while (myKey == '\0');
              if (myKey == '0') {
                menu = 0;
              } else {
                do {
                  senhas[cod] = senhas[cod + 1];
                } while (senhas[cod + 1] != 0);
              }
            }
            break;
          //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          //===========================================================================================================
          //=================================================== OPCAO 3 ===============================================
          //===========================================================================================================
          //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          case 3:
            // comando(s)
            funcao1(senhadig, senhas);  //Chama a função que recebe a senha adm e confere se esta correta
            break;
          //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          //===========================================================================================================
          //=================================================== OPCAO 4 ===============================================
          //===========================================================================================================
          //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          case 4:
            // comando(s)
            break;
          default:
            // comando(s)
            break;
        }
      } while (menu);
      //==================================================================================================
      //========================================================================= FIM MENU ===============
      //==================================================================================================
      ant = 1;
      limpa();
    }
  }
}

void loop() {
  Blynk.run();
  timer.run();  // Initiates BlynkTimer

  minhaLogica();
}
