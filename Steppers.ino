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
#include <AFMotor.h> // biblioteca para o motor shield L293D
#include <TM1637Display.h> //biblioteca para o display TM1637
#include <EEPROM.h> //biblioteca para gravar/ler dados na memória EEPROM
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
int pino_botao = 2;

//variavel para a leitura do estado do push button
int estado = 0;

//armazena se o botao foi pressionado ou nao
int guarda_estado;

//pino do botao de reset, A0
int pino_botao_reset = 14;

//variavel para a leitura do estado do push button de reset
int estado_reset = 0;

//armazena se o botao de reset foi pressionado ou nao
int guarda_estado_reset;

//variavel que armazena numero de passos que o motor vai executar, descomentar
//caso queira um angulo diferente de 360

double n_passos;

// Define os parametros do motor 1
AF_Stepper motor_de_passo_1(passos_total, porta_motor_1);
// Define os parametros do motor 2
AF_Stepper motor_de_passo_2(passos_total, porta_motor_2);

//Definicao do pino de clock do display, pode ser esse porque so eh usado quando
//o shield esta controlando servo motores
const int CLK = 10;
//Definicao do pino DIO do display
const int DIO = 13;

//cria objeto do Display TM1637
TM1637Display display(CLK, DIO);

//endereco da memoria em que se salva o contador, nao mexer para obter armazenamento maximo de 0 a 255
int endereco = 0;

//variavel para leitura do valor armazenado na memoria EEPROM
unsigned int contador;

////////////////////////////////////////////////////////////////////////////////
//          Etapa de Configuracao: executada vez somente no inicio            //
////////////////////////////////////////////////////////////////////////////////

void setup(){
  //Definicao da velocidade de rotacao dos motores em RPM; o limite depende do motor
  motor_de_passo_1.setSpeed(10); //10RPM, 1/6 Hz
  motor_de_passo_2.setSpeed(10); //10RPM, 1/6 Hz
  //Define o pino pino_botao como entrada pullup, ativando o resistor interno
  //para evitar jitter, futuação entre 0 e 1 no valor do pino
  pinMode(pino_botao, INPUT_PULLUP);
  //Define o pino pino_botao de reset como entrada pullup, ativando o resistor interno
  //para evitar jitter, futuação entre 0 e 1 no valor do pino
  pinMode(pino_botao_reset, INPUT_PULLUP);
  //Inicia o programa com o botão sem ter sido apertado; mudar para 1 se quiser
  // que o programa execute uma volta sem apaertar o botao no inicio
  guarda_estado = 0;

  //inicia o contador como 0, mas e substituido pelo valor na memoria depois
  contador = 0;

  n_passos = passos_total; //modificar caso nao queira mais volta de 360

  // seta o brilho do display no maximo
  display.setBrightness(0x0a);

  //Inicializa a porta serial; nao precisa mexer aqui
  Serial.begin(9600);
}

////////////////////////////////////////////////////////////////////////////////
//   Etapa de Execucao: programa em si, executado enquanto o arduino estiver  //
//                                   ligado                                   //
////////////////////////////////////////////////////////////////////////////////

void loop(){
  //le a porta de entrada do botao para saber se foi pressionado ou nao
  estado = digitalRead(pino_botao);

  estado_reset = digitalRead(pino_botao_reset);

  contador = EEPROM.read(endereco);

  //exibe o numero de acionamentos no display
  display.showNumberDec(contador);

  //formula: numero_de_passos = angulo/(360/passos_total)

  //Mostra no serial monitor o numero de passos calculados, descomentar se voce
  //quiser saber quantos passos o motor vai executar, mas nao faz sentido para
  // uma volta completa.
	  //Serial.print("Numero de passos : ");
  //Serial.println(numero_de_passos);

  if(estado == HIGH){//Se o botao nao estiver sendo pressionado
    guarda_estado = 1; //o botao foi pressionado
    delay(100);// delay para evitar flutuacao no valor do botao
  }

  if (guarda_estado==1){ //o botao foi pressionado

	contador ++; //aumenta o numero de acionamentos registrado

	if (contador>255){// caso exceda o limite de 255, o contador volta para 1, alterar para outro valor caso desejador
		contador = 1;
	}
	EEPROM.write(endereco, contador); //guarda na memoria o novo numero de acionamentos
	delay(20);
        // ja mostra o numero, comentar caso queira que so apareca depois de
        //girar os motores
        display.showNumberDec(contador);

        motor_de_passo_1.step(n_passos, FORWARD, SINGLE);
        motor_de_passo_1.release();
	delay(100); // atraso entre o movimento de um motor e do outro em milissegundos
        motor_de_passo_2.step(n_passos, FORWARD, SINGLE);
        motor_de_passo_2.release();
        delay(100);//Atraso entre um ciclo de maquina e outro em milissegundos.
	guarda_estado = 0;
  } else {
    delay(10);
  }

 if(estado_reset == HIGH){//Se o botao nao estiver sendo pressionado
    guarda_estado_reset = 1; //o botao foi pressionado
    delay(100);// delay para evitar flutuacao no valor do botao
  }

  if (guarda_estado_reset ==1){ //o botao foi pressionado

        contador = 0; //zera o contador
	EEPROM.write(endereco, contador);
        // ja mostra o numero 0
        display.showNumberDec(contador);

	guarda_estado_reset = 0;
	delay(100);

  } else {
    delay(10);
  }

}
