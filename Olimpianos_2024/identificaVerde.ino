// Função principal para detectar a cor verde e tomar decisões com base nela
bool verde() {
  // Verifica se a inclinação está fora do intervalo aceitável (-60° a +60°)
  if (inclinacao() < -60 || inclinacao() > 60) return false;

  // Lê o estado dos sensores de cor da direita e da esquerda
  const bool verdeD = verde_D();
  const bool verdeE = verde_E();

  // Verifica se ao menos um dos sensores detecta a cor verde
  if ((verdeD || verdeE)) {
    situacao('V');  // Atualiza estado para "Verde" (leitura de verde detectada)

    // Caso os dois sensores detectem verde, o robô deve fazer uma meia-volta
    if (verdeD && verdeE) {
      situacao('R');  // Atualiza estado para "Retorno" (meia-volta)
      Serial.println("180");  // Loga no serial monitor que fará um giro de 180°

      curvaDir(velCurva);  // Começa a girar para a direita
      delay(1800);         // Gira por 1,8 segundos

      leitura();  // Atualiza as leituras dos sensores

      // Verifica se está sobre a linha preta, caso não, continua girando até encontrar
      if (leituraSfe < Preto + 100) {
        while (leituraSfe < Preto - 100) {
          curvaDir(velCurva);
          leitura();
        }
        delay(100);  // Pausa curta após encontrar a linha
      }

      parar();  // Interrompe o movimento
    }

    // Se apenas o sensor da direita detectar verde
    else if (verdeD) {
      situacao('D');  // Atualiza o estado para indicar verde na direita

      avancar(velAvancar);  // Avança um pouco
      delay(450);           // Avança por 450ms
      parar();

      curvaDir(velCurva);  // Realiza uma curva para a direita
      delay(700);          // Curva por 700ms

      leitura();  // Atualiza as leituras

      // Verifica se encontrou a linha preta, senão continua girando
      if (leituraSfd < Preto + 50) {
        while (leituraSfd < Preto + 50) {
          curvaDir(velCurva);
          leitura();
        }
      }

      parar();  // Interrompe o movimento
    }

    // Se apenas o sensor da esquerda detectar verde
    else if (verdeE) {
      situacao('E');  // Atualiza o estado para indicar verde na esquerda

      avancar(velAvancar);  // Avança um pouco
      delay(300);           // Avança por 300ms
      parar();

      curvaEsq(velCurva);  // Realiza uma curva para a esquerda
      delay(500);          // Curva por 500ms

      leitura();  // Atualiza as leituras

      // Verifica se encontrou a linha preta, senão continua girando
      if (leituraSfd < Preto + 50) {
        while (leituraSfd < Preto) {
          curvaEsq(velCurva);
          leitura();
        }
      }

      parar();  // Interrompe o movimento
    }

    return true;  // Retorna verdadeiro indicando que houve detecção de verde
  }

  return false;  // Nenhum verde detectado
}

// Verifica se o sensor de cor da direita detecta verde
bool verde_D() {
  bool temVerde = false;
  verificaCor('D') == "Verde" ? temVerde = true : temVerde = false;
  return temVerde;
}

// Verifica se o sensor de cor da esquerda detecta verde
bool verde_E() {
  bool temVerde = false;
  verificaCor('E') == "Verde" ? temVerde = true : temVerde = false;
  return temVerde;
}
