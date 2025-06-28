// Caso seja identificada uma rampa, esta função será executada
// Ela ajusta a velocidade dos motores de acordo com a direção (subida ou descida)
void rampa(bool subida) {
  Serial.println("Rampa");       // Exibe no monitor serial que o robô está em uma rampa
  desligaLeds();                 // Desliga os LEDs indicadores (opcional/estético)

  // Caso esteja em uma subida E o sensor ultrassônico da esquerda detecte parede a menos de 10 cm
  if (subida && ultrassonico_cm('E') < 10) {
    velAvancar = 200;           // Aumenta a velocidade de avanço para compensar a subida
    velCurva = 170;             // Reduz um pouco a velocidade de curva
    desligaLeds();              // Garante que os LEDs estejam desligados
    segue_linha(true);          // Continua seguindo a linha com o modo "forçado/ajustado"
    velAvancar = 165;           // Restaura a velocidade de avanço para o valor padrão
    velCurva = 180;             // Restaura a velocidade de curva para o valor padrão
  }

  // Caso esteja em uma descida (subida == false)
  else if (!subida) {
    velAvancar = 100;           // Reduz bastante a velocidade de avanço para não descer rápido demais
    velCurva = 140;             // Ajusta a curva para uma velocidade mais controlada
  }

  // Independente do caso acima, ao final as velocidades são padronizadas novamente
  velAvancar = 165;             // Define a velocidade de avanço padrão
  velCurva = 180;               // Define a velocidade de curva padrão

  // OBS: Este trecho garante que a velocidade padrão seja restaurada, mas ignora os ajustes temporários acima.
  // A lógica do comentário ("enquanto os dois ultrassônicos identificarem parede...") não está implementada aqui.
}
