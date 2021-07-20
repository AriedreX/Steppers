////////////////////////////////////////////////////////////////////////////////
//Programa para controlar 2 motores de passo 28BYJ-48 com Arduino  Uno e Motor//
//Shield L293D                                                                //
////////////////////////////////////////////////////////////////////////////////
//Versão: v0.1                                                                //
//data da ultima alteracao: 13/03/2019                                        //
//Autor : Antonio Galiza (angacego@poli.ufrj.br)                              //
////////////////////////////////////////////////////////////////////////////////
//                           Bibliotecas carregadas                           //
////////////////////////////////////////////////////////////////////////////////
#include <AFMotor.h>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                            Variaveis importantes                           //
////////////////////////////////////////////////////////////////////////////////

// Um motor de passo de 4 bobinas pode ser acionado em 4 ou 8 passos. O modo de
//4 passos exige menos passos para dar uma volta completa, uma vez que aciona
//somente 2 das 4 bobinas do motor. É menos preciso, mas fornece um torque maior
// que o modo de 8 passos

//modo de 4 passos
double passos_total = 2048;
//modo de 8 passos_total; comentar a variavel acima e descomentar a de baixo
//double passos_total = 4096;

//Portas para os motores
//Valor 1 para motor de passo em M1/M2 e 2 para motor de passo em M3/M4
int porta_motor_1 = 1;
int porta_motor_2 = 2;
//Pino Digital para o push button
int pino_botao = 4;

//armazena o valor do botao no ciclo de maquina anterior
int valor_passado_botao;

//variavel que armazena numero de passos que o motor vai executar, descomentar
//caso queira um angulo diferente de 360

double n_passos;

// Define os parametros do motor 1
AF_Stepper motor_de_passo_1(passos_total, porta_motor_1);
// Define os parametros do motor 2
AF_Stepper motor_de_passo_2(passos_total, porta_motor_2);

////////////////////////////////////////////////////////////////////////////////
//          Etapa de Configuracao: executada vez somente no inicio            //
////////////////////////////////////////////////////////////////////////////////

void setup(){
  //Definicao da velocidade de rotacao dos motores em RPM; o limite depende do motor
  motor_de_passo_1.setSpeed(10); //10RPM, 1/6 Hz
  motor_de_passo_2.setSpeed(10); //10RPM, 1/6 Hz
  //Define o pino pino_botao como entrada pullup, ativando o resistor interno
  //para evitar jitter, futuação entre 0 e 1 no valor do pino
  pinMode(pino_botao, INPUT);
  //Inicia o programa com o botão sem ter sido apertado; mudar para 1 se quiser
  // que o programa execute uma volta sem apaertar o botao no inicio
  valor_passado_botao = 0;
  //Inicializa a porta serial; nao precisa mexer aqui
  Serial.begin(9600);
}

////////////////////////////////////////////////////////////////////////////////
//   Etapa de Execucao: programa em si, executado enquanto o arduino estiver  //
//                                   ligado                                   //
////////////////////////////////////////////////////////////////////////////////

void loop(){
  n_passos = passos_total; //modificar caso nao queira mais volta de 360
  //formula: numero_de_passos = angulo/(360/passos_total)

  //Mostra no serial monitor o numero de passos calculados, descomentar se voce
  //quiser saber quantos passos o motor vai executar, mas nao faz sentido para
  // uma volta completa.
  //Serial.print("Numero de passos : ");
  //Serial.println(numero_de_passos);

  if(digitalRead(pino_botao) == LOW){//Se o botao nao estiver sendo pressionado
      if (valor_passado_botao){ //caso o botao tenha sido pressionado antes, indicando que o botao foi pressionado e solto

        //Move o motor 1. Use FORWARD para sentido horario e BACKWARD para anti-horario
        //use SINGLE para modo de acionamento de 4 passos e DOUBLE para o de 8, mas se
        //lembre de mudar o numero_de_passos para 4096
        motor_de_passo_1.step(n_passos, FORWARD, SINGLE);
        motor_de_passo_1.release();

        delay(10); // atraso entre o movimento de um motor e do outro em milissegundos

        //Move o motor 2. Use FORWARD para sentido horario e BACKWARD para anti-horario
        //use SINGLE para modo de acionamento de 4 passos e DOUBLE para o de 8, mas se
        //lembre de mudar o numero_de_passos para 4096
        motor_de_passo_2.step(n_passos, FORWARD, SINGLE);
        motor_de_passo_2.release();

        //Libera a variavel de memoria, dizendo que o botao ainda nao foi pressionado
        valor_passado_botao = 0;
        delay(500);//Atraso entre um ciclo de maquina e outro em milissegundos.

      }//Caso o botao nao tenha sido pressionado, o programa faz nada e passa ao
      // proximo ciclo de maquina

  }else{ //Caso o botao seja pressionado
      valor_passado_botao = 1; //registra que o botao foi pressionado
  }

}
