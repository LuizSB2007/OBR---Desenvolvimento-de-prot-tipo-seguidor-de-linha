void segue_linha(bool subida) {
  leitura();                   //Faz as leituras dos sensores
  if (!subida) cruzamento();   //Verifica se há um cruzamento, caso houver executa a sequência correta
  if (!subida) situacao('L');  //Indica a situação que o robô se encontra através dos dois leds RGBs
  if (!subida) arena('R');
  if (!subida) fim();
  //Serial.println(String(leituraS1) + " " + String(leituraS2) + " " + String(leituraSf) + " "  + String(leituraS3) + " " + String(leituraS4));

  if (leituraSfd > Preto + 110 && leituraSfe > Preto + 110) avancar(velAvancar);         //Verifica se o sensor da frente está sobre a linha, caso sim, o robô manterá sua posição e seguirá em frente
  else if (leituraS2 < Preto - 100 && leituraS3 > Preto - 100) curvaDir(velCurva - 20);  //Verifica se o sensor da direita está sobre a linha, caso sim, realiza uma curva para a direita
  else if (leituraS2 > Preto - 100 && leituraS3 < Preto - 100) curvaEsq(velCurva - 20);  //Verifica se o sensor da esquerda está sobre a linha, caso sim, realiza uma curva para a esquerda
  else avancar(velAvancar);                                                              //Caso nenhuma das verifições acima forem verdadeiras o robô manterá sua posição e seguirá reto

  delay(5);  //Espera existente para possibilitar a execução mínima de um dos comandos acima
}