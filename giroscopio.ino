const int filterSize = 10;      // Tamanho do buffer para filtro de média móvel (quanto maior, mais suavizado será o sinal)
float pitchBuffer[filterSize];  // Vetor que armazena os valores recentes de pitch para cálculo da média
int bufferIndex = 0;            // Índice atual do buffer (posição a ser sobrescrita)

// Função para calibrar o giroscópio/acelerômetro
void calibraGyro() {
  selecionaSensor('G');             // Seleciona o sensor do tipo giroscópio (caso seja um sistema multiplexado)
  Wire.begin();                     // Inicia a comunicação I2C
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);                 // Acessa o registrador PWR_MGMT_1 do MPU6050
  Wire.write(0);                    // Escreve 0 para acordar o sensor (tirar do modo sleep)
  Wire.endTransmission(true);
  Serial.println("Giroscopio conectado");

  delay(100);  // Espera um pouco para o sensor estabilizar após inicialização

  // Variáveis auxiliares para acumular leituras
  long accX_sum = 0;
  long accY_sum = 0;
  long accZ_sum = 0;
  const int numReadings = 100;  // Número de leituras para média de calibração

  // Realiza múltiplas leituras para calcular média dos valores de repouso (offsets)
  for (int i = 0; i < numReadings; i++) {
    selecionaSensor('G');  // Seleciona novamente o sensor

    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B);               // Endereço inicial dos registradores de aceleração
    Wire.endTransmission(false);   // Recomeça a transmissão (repeated start)
    Wire.requestFrom(MPU6050_ADDR, 6, true);  // Solicita 6 bytes (accX, accY, accZ)

    if (Wire.available() == 6) {  // Confirma se os 6 bytes foram recebidos
      accX = Wire.read() << 8 | Wire.read();  // Combina os 2 bytes em um valor de 16 bits
      accY = Wire.read() << 8 | Wire.read();
      accZ = Wire.read() << 8 | Wire.read();

      accX_sum += accX;  // Soma os valores para média posterior
      accY_sum += accY;
      accZ_sum += accZ;
    }

    delay(10);  // Pequena pausa entre leituras para estabilidade
  }

  // Calcula os valores médios (offsets) com base nas leituras
  accX_offset = accX_sum / numReadings;
  accY_offset = accY_sum / numReadings;
  accZ_offset = accZ_sum / numReadings;

  // Inicializa o buffer de pitch com zeros
  for (int i = 0; i < filterSize; i++) {
    pitchBuffer[i] = 0.0;
  }
}

// Função para calcular a inclinação do robô (pitch) em graus
float inclinacao() {
  selecionaSensor('G');  // Seleciona o sensor giroscópio/acelerômetro

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);               // Endereço inicial para leitura dos dados do acelerômetro
  Wire.endTransmission(false);   // Recomeça a transmissão (repeated start)
  Wire.requestFrom(MPU6050_ADDR, 6, true);  // Solicita os 6 bytes necessários (accX, accY, accZ)

  if (Wire.available() == 6) {  // Verifica se os dados foram recebidos com sucesso
    accX = Wire.read() << 8 | Wire.read();  // Lê e combina os bytes altos e baixos de accX
    accY = Wire.read() << 8 | Wire.read();  // accY
    accZ = Wire.read() << 8 | Wire.read();  // accZ

    // Subtrai os valores de offset para obter leituras relativas
    float accX_rel = accX - accX_offset;
    float accY_rel = accY - accY_offset;
    float accZ_rel = accZ - accZ_offset;

    // Calcula o ângulo de inclinação (pitch) com base na trigonometria
    float newPitch = atan2(-accX_rel, sqrt(accY_rel * accY_rel + accZ_rel * accZ_rel)) * 180 / PI;

    // Define uma zona morta para ignorar pequenas oscilações (ruído)
    const float deadZone = 2;  // Ângulo mínimo necessário para considerar que houve mudança

    // Atualiza o valor de pitch somente se houve uma variação relevante
    if (fabs(newPitch - pitch) > deadZone) {
      // Armazena o novo valor no buffer circular
      pitchBuffer[bufferIndex] = newPitch;
      bufferIndex = (bufferIndex + 1) % filterSize;  // Avança circularmente no buffer

      // Calcula a média dos valores no buffer (filtro de média móvel)
      float pitchSum = 0.0;
      for (int i = 0; i < filterSize; i++) {
        pitchSum += pitchBuffer[i];
      }

      pitch = pitchSum / filterSize;  // Novo valor suavizado de pitch
    }
  }

  return pitch;  // Retorna o valor final de inclinação
}
