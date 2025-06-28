#include <Adafruit_TCS34725.h>   // Biblioteca para sensores de cor TCS34725
#include <Wire.h>                // Biblioteca para comunicação I2C

// Definições dos pinos dos sensores de luz (fitas coloridas no chão)
#define s1 A0   // Sensor de luz 1 - Fio verde
#define s2 A1   // Sensor de luz 2 - Fio amarelo
#define s3 A2   // Sensor de luz 3 - Fio branco
#define s4 A3   // Sensor de luz 4 - Fio roxo
#define sfe A4  // Sensor de luz extremo esquerdo - Fio azul
#define sfd A5  // Sensor de luz extremo direito - Fio azul

// Definições dos sensores ultrassônicos (F: Frente, D: Direita, E: Esquerda)
#define echoF 2   // Echo frente - Fio cinza
#define trigF 22  // Trigger frente - Fio azul

#define echoD 6   // Echo direita - Fio verde
#define trigD 24  // Trigger direita - Fio amarelo

#define echoE 3   // Echo esquerda - Fio azul
#define trigE 24  // Trigger esquerda - Fio cinza (OBS: mesmo pino do trigD, pode causar conflito)

// LEDs de feedback visual para depuração ou indicação de status
#define ledVermelho1 A13  // LED vermelho 1 - Fio verde
#define ledVerde1 A14     // LED verde 1 - Fio azul
#define ledAzul1 A15      // LED azul 1 - Fio branco

#define ledVermelho2 A10  // LED vermelho 2 - Fio verde
#define ledVerde2 A11     // LED verde 2 - Fio azul
#define ledAzul2 A12      // LED azul 2 - Fio branco

#define Preto 800  // Valor de referência para leitura de preto nos sensores de luz

#define MPU6050_ADDR 0x68  // Endereço do acelerômetro/giroscópio MPU6050

// Pinos dos motores
int MET = 11;  // Motor esquerdo - marcha à ré
int MEF = 10;  // Motor esquerdo - para frente
int velE = 8;  // Controle de velocidade - motor esquerdo

int MDT = 12;  // Motor direito - marcha à ré
int MDF = 13;  // Motor direito - para frente
int velD = 9;  // Controle de velocidade - motor direito

// Variáveis para armazenar leituras dos sensores de luz
int leituraSf = 0;
int leituraS1 = 0;
int leituraS2 = 0;
int leituraS3 = 0;
int leituraS4 = 0;
int leituraSfd = 0;
int leituraSfe = 0;

// Leituras dos sensores ultrassônicos
int ultraEValor = 0;
int ultraDValor = 0;
int ultraFValor;

// Velocidades padrão
int velAvancar = 165;
int velCurva = 180;  // Máximo sugerido: 200

// Variáveis do acelerômetro
int16_t accX, accY, accZ;                    // Leitura bruta dos eixos X, Y, Z
float accX_offset, accY_offset, accZ_offset; // Calibração de offset
float pitch = 0.0;                           // Ângulo de inclinação suavizado

// Instâncias dos sensores de cor
Adafruit_TCS34725 sensTCSD = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_420MS, TCS34725_GAIN_1X);   // Sensor cor direita
Adafruit_TCS34725 sensTCSE = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_420MS, TCS34725_GAIN_1X);   // Sensor cor esquerda
Adafruit_TCS34725 sensTCSM = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);  // Sensor cor central (vermelho)

// Seleciona um sensor no multiplexador I2C
void selecionaSensor(char lado) {
  uint8_t porta;
  if (lado == 'D') porta = 2;       // Cor direita
  else if (lado == 'E') porta = 6;  // Cor esquerda
  else if (lado == 'G') porta = 4;  // Giroscópio
  else if (lado == 'M') porta = 3;  // Cor central

  Wire.beginTransmission(0x70);     // Inicia comunicação com multiplexador
  Wire.write(1 << porta);           // Seleciona a porta correta
  Wire.endTransmission();           // Finaliza comunicação
}

// Função de configuração do sistema
void setup() {
  Serial.begin(9600);   // Inicializa comunicação serial
  Wire.begin();         // Inicializa barramento I2C

  // Define pinos de motores como saída
  pinMode(MDT, OUTPUT);
  pinMode(MDF, OUTPUT);
  pinMode(MET, OUTPUT);
  pinMode(MEF, OUTPUT);

  // Define pinos dos LEDs como saída
  pinMode(ledVerde1, OUTPUT);
  pinMode(ledAzul1, OUTPUT);
  pinMode(ledVermelho1, OUTPUT);
  pinMode(ledVerde2, OUTPUT);
  pinMode(ledAzul2, OUTPUT);
  pinMode(ledVermelho2, OUTPUT);

  // Configura sensores ultrassônicos (trigger como saída, echo como entrada)
  pinMode(trigF, OUTPUT);
  digitalWrite(trigF, LOW);
  delayMicroseconds(10);
  pinMode(echoF, INPUT);

  pinMode(trigD, OUTPUT);
  digitalWrite(trigD, LOW);
  delayMicroseconds(10);
  pinMode(echoD, INPUT);

  pinMode(trigE, OUTPUT);
  digitalWrite(trigE, LOW);
  delayMicroseconds(10);
  pinMode(echoE, INPUT);

  delay(100);  // Pequeno atraso para estabilidade

  // Inicializa sensores de cor conectados via multiplexador
  selecionaSensor('E');
  if (sensTCSE.begin()) Serial.println("Sensor da esquerda conectado");
  else Serial.println("Sensor da esquerda com problema");

  selecionaSensor('D');
  if (sensTCSD.begin()) Serial.println("Sensor da direita conectado");
  else Serial.println("Sensor da direita com problema");

  initTCS34725('M');  // Inicializa sensor de cor do meio (vermelho)

  calibraGyro();  // Calibra o giroscópio
}

// Loop principal
void loop() {
  // Lógica principal de navegação: rampa, obstáculo ou linha
  if (true) {
    if (inclinacao() < -75) rampa(true);               // Subida
    else if (inclinacao() > 60) rampa(false);          // Descida
    else if (ultrassonico_cm('F') <= 3 && ultrassonico_cm('F') > 0) {
      obstaculo('E', false);                           // Desvio de obstáculo para a esquerda
    } else {
      segue_linha(false);                              // Segue a linha normalmente
    }
  } else {
    // Bloco de testes / depuração (desativado por padrão)
    leitura();
    Serial.println(ultrassonico_cm('F'));
    //verificaCor('D');
    //verificaCor('E');
    //Serial.println(String(leituraS1) + " " + String(leituraS2) + " " + String(leituraSfe) + " " + String(leituraSfe) + " " + String(leituraS3) + " " + String(leituraS4));
    //avancar(velAvancar);
  }
}

// Verifica se chegou no final ao detectar a cor vermelha no sensor do meio
void fim() {
  if (verificaCor('M') == "Vermelho") {
    parar();          // Para os motores
    situacao('F');    // Atualiza situação para "fim"
  }
}
