#define TCS34725_ADDR 0x29     // Endereço I2C padrão do sensor TCS34725
#define COMMAND_BIT 0x80       // Bit usado para indicar comando no protocolo I2C

#define TAM 5                  // Tamanho do array para média móvel (histórico)

int historicoR_M[TAM] = { 0 }, pos_M = 0;  // Array para histórico do sensor médio e posição atual

const int thresholdVermelhoM = 400;         // Limiar para detectar vermelho no sensor médio
const int clearMin = 90;                     // Valor mínimo do canal "clear" para considerar leitura válida

// Valores de referência para os sensores esquerdo e direito (branco, vermelho, verde)
int brancoERef = 1500;
int vermelhoERef = 400;
int verdeERef = 650;

int brancoDRef = 1500;
int vermelhoDRef = 400;
int verdeDRef = 800;

// Ajuste dos valores referenciais para compensar aumento por conexão do cabo (~10-15%)
float brancoETol = brancoERef * 0.95;
float vermelhoETol = vermelhoERef * 0.85;
float verdeETol = verdeERef * 0.85;

float brancoDTol = brancoDRef * 0.95;
float vermelhoDTol = vermelhoDRef * 0.85;
float verdeDTol = verdeDRef * 0.85;

// Função que escreve um valor em um registrador do sensor via I2C
void writeRegister(byte reg, byte value) {
  Wire.beginTransmission(TCS34725_ADDR);     // Inicia comunicação com sensor
  Wire.write(COMMAND_BIT | reg);              // Envia comando com o endereço do registrador
  Wire.write(value);                          // Envia valor a ser escrito
  Wire.endTransmission();                     // Finaliza comunicação
}

// Função para ler 16 bits (dois bytes) de um registrador do sensor
uint16_t read16bitRegister(byte reg) {
  Wire.beginTransmission(TCS34725_ADDR);    // Inicia comunicação com sensor
  Wire.write(COMMAND_BIT | reg);             // Envia comando com o endereço do registrador
  Wire.endTransmission();                    // Finaliza envio
  Wire.requestFrom(TCS34725_ADDR, (uint8_t)2);  // Solicita 2 bytes de dados

  uint8_t low = Wire.read();                 // Lê byte menos significativo
  uint8_t high = Wire.read();                // Lê byte mais significativo
  return (high << 8) | low;                  // Combina bytes em uint16_t
}

// Inicializa o sensor TCS34725 para o sensor especificado ('E' ou 'D')
void initTCS34725(char sensor) {
  selecionaSensor(sensor);                    // Seleciona o sensor (função não mostrada aqui)
  writeRegister(0x00, 0x01);                  // Liga o sensor (Power ON)
  delay(3);                                   // Aguarda estabilização
  writeRegister(0x00, 0x03);                  // Habilita o sensor RGBC (leitura de cores)
  writeRegister(0x01, 0xFF);                  // Define tempo de integração (exposição) - 2.4ms
  writeRegister(0x0F, 0x03);                  // Define ganho do sensor - 60x
  delay(10);                                  // Aguarda para garantir configuração
  Serial.println("Sensor " + String(sensor) + " conectado");  // Confirmação no serial
}

// Calcula média móvel simples para suavizar leituras (array circular)
int mediaMovel(int *array, int nova, int &pos) {
  array[pos] = nova;                          // Insere novo valor na posição atual
  pos = (pos + 1) % TAM;                      // Atualiza posição (circular)
  int soma = 0;
  for (int i = 0; i < TAM; i++) soma += array[i];  // Soma todos os valores do array
  return soma / TAM;                          // Retorna média simples
}

// Lê os valores RGBC do sensor especificado
void lerRGB(char sensor, uint16_t &r, uint16_t &g, uint16_t &b, uint16_t &c) {
  selecionaSensor(sensor);                     // Seleciona sensor
  r = read16bitRegister(0x16);                 // Lê componente vermelho
  g = read16bitRegister(0x18);                 // Lê componente verde
  b = read16bitRegister(0x1A);                 // Lê componente azul
  c = read16bitRegister(0x14);                 // Lê componente clear (intensidade total)
}

// Função que retorna a cor detectada pelo sensor especificado ('E', 'D' ou 'M')
char *verificaCor(char sensor) {
  uint16_t r, g, b, c, LUX;
  lerRGB(sensor, r, g, b, c);                   // Lê valores de cor e intensidade

  // Se intensidade total (clear) for muito baixa, considera preto
  if (c < clearMin) {
    Serial.print("Preto ");
    Serial.println(sensor);
    return "Preto";
  }

  // Caso sensor direito
  if (sensor == 'D') {
    selecionaSensor('D');
    // Pega dados brutos do sensor (objeto sensTCSD não mostrado aqui)
    sensTCSD.getRawData(&r, &g, &b, &c);

    LUX = sensTCSD.calculateLux(r, g, b);        // Calcula luminosidade (lux)
    float proporcaoV = (float)g / (r);           // Proporção verde/vermelho para identificação

    // Impressão para debug
    Serial.println("Proporcao: " + String(proporcaoV));
    Serial.print("Vermelho(R): ");
    Serial.print(r);
    Serial.print("   Verde(G): ");
    Serial.print(g);
    Serial.print("   Azul(B): ");
    Serial.print(b);
    Serial.print("   Intensidade: ");
    Serial.println(LUX);
    delay(200);

    // Verifica se é branco (todos componentes altos)
    if (r > brancoDTol && b > brancoDTol && g > brancoDTol) {
      Serial.println("Branco direita");
      situacao('R');                              // Marca situação (ex: para indicação)
      return "Branco";
    }
    // Verifica se é vermelho (vermelho maior que verde e acima do limiar)
    else if (r > g && r >= vermelhoDTol) {
      Serial.println("Vermelho direita");
      return "Vermelho";
    }
    // Verifica se é verde (verde maior que vermelho, acima do limiar e proporção alta)
    else if (g > r && g >= verdeETol && proporcaoV > 1.5) {
      Serial.println("Verde direita");
      return "Verde";
    } 
    else {
      Serial.println("Preto direita");
      return "Preto";
    }
  } 

  // Caso sensor esquerdo
  else if (sensor == 'E') {
    selecionaSensor('E');
    sensTCSE.getRawData(&r, &g, &b, &c);          // Pega dados brutos do sensor esquerdo

    LUX = sensTCSE.calculateLux(r, g, b);          // Calcula luminosidade
    float proporcaoV = (float)g / (r);             // Proporção verde/vermelho

    // Impressão para debug
    Serial.println("Proporcao: " + String(proporcaoV));
    Serial.print("Vermelho(R): ");
    Serial.print(r);
    Serial.print("   Verde(G): ");
    Serial.print(g);
    Serial.print("   Azul(B): ");
    Serial.print(b);
    Serial.print("   Intensidade: ");
    Serial.println(LUX);
    delay(200);

    // Identificação da cor para o sensor esquerdo, com limiares ajustados
    if (r > brancoETol && b > brancoETol && g > brancoETol) {
      Serial.println("Branco esquerda");
      situacao('R');
      return "Branco";
    }
    else if (r > g && r > vermelhoETol) {
      Serial.println("Vermelho esquerda");
      return "Vermelho";
    }
    else if (g > r && g > verdeETol && proporcaoV > 1.65) {
      Serial.println("Verde esquerda");
      return "Verde";
    } 
    else {
      Serial.println("Preto esquerda");
      return "Preto";
    }
  } 

  // Caso sensor médio
  else if (sensor == 'M') {
    int rMed = mediaMovel(historicoR_M, r, pos_M);    // Média móvel do vermelho
    float proporcaoR = (float)r / (g + b + 1);        // Proporção vermelho / (verde + azul + 1)

    // Debug (comentado)
    // Serial.print("M R:"); Serial.print(r);
    // Serial.print(" G:"); Serial.print(g);
    // Serial.print(" B:"); Serial.print(b);
    // Serial.print(" Clear:"); Serial.print(c);
    // Serial.print(" Proporção R: "); Serial.println(proporcaoR, 2);

    // Detecta vermelho quando média do vermelho ultrapassa limiar e proporções indicam predominância
    if (rMed > thresholdVermelhoM && proporcaoR > 1 && rMed > g + 100 && rMed > b + 100) {
      Serial.println("Vermelho frente");
      return "Vermelho";
    }
  }

  // Caso nenhum dos critérios seja satisfeito, considera preto
  return "Preto";
}
