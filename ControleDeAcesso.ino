/*
 * -------------------------------Controle de Acesso com RFID-----------------------------
 * 
 * CARTÃO MASTER: 226 84 18 187
 * 
 * INFORMAÇÕES DO CIRCUITO:
 * Pinos do sensor -> Pinos do Arduino
 *  3.3v           ->       3.3v
 *  GND            ->       GND
 *  RST            ->       pin 9
 *  IRQ            ->       off
 *  MISO           ->       pin 12
 *  MOSI           ->       pin 11
 *  SCK            ->       pin 13
 *  SDA            ->       pin 10
 * 
 */

/*
PADRÃO PARA ENVIAR POR BLUETOOTH

"000 000 000 000 liberado|negado 00:00:00"

*/


#include <MFRC522.h>
#include <SPI.h>
#include <EEPROM.h>

#define SS_PIN 10
#define RST_PIN 9
#define RED_LED 8
#define YEL_LED 7
#define WRI_LED 6
#define BRAN_BUTTON 3
#define AZUL_BUTTON 4
#define VERM_BUTTON 5

const int pinosButtons[3] = {3, 4, 5};

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;


String uidString;
byte uid[4];
const byte cardMaster[4] = {226, 84, 18, 187};


void setup(){
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(RED_LED, OUTPUT);
  pinMode(YEL_LED, OUTPUT);
  pinMode(WRI_LED, OUTPUT);
  for(int i=0; i<3; i++){
    pinMode(pinosButtons[i], INPUT);
  }
  
  Serial.println("Aproxime o cartão.");
  delay(500);

}


void loop(){
  
  
  //  ESPERA POR UM CARTÃO E QUANDO ENCONTRA ARMAZENA SEU ENDEREÇO.
  readRFID();

  
  if(busca()){  // CADASTRADO                
    if(comparaMaster()){                 //MASTER: CADASTRAR OU REMOVER UM CARTÃO.
      /*
       * PARTE DOS BOTÕES
       * 
       * BOTÃO VERMELHO:  CANCELAR OPERAÇÃO E VOLTAR AO INICIO DO LOOP
       * 
       * BOTÃO AZUL:      REMOVER O PRÓXIMO CARTÃO LIDO
       * 
       * BOTÃO BRANCO:    CADASTRAR O PRÓXIMO CARTÃO LIDO
       */
       digitalWrite(WRI_LED, HIGH);           //Luz para informar que está no modo master.fica acesa até findar.
       
       Serial.println("Modo Master.");
       Serial.println("Escolha uma opção:");
       Serial.println("BOTÃO BRANCO - CADASTRAR O PRÓXIMO CARTÃO LIDO");
       Serial.println("BOTÃO AZUL - REMOVER O PRÓXIMO CARTÃO LIDO");
       Serial.println("BOTÃO VERMELHO - CANCELAR OPERAÇÃO");
       
       
       int Button = esperaButtons();
       if(Button == 0){       // BRANCO
        Serial.println("Modo de cadastro. Aproxime o cartão que deseja cadastrar.");
        readRFID();
        addCard();
        digitalWrite(WRI_LED, LOW);
        
       }
       else if(Button == 1){    //AZUL
        Serial.println("Modo de remoção. Aproxime o cartão que deseja remover.");
        readRFID();
        removerCard();
        digitalWrite(WRI_LED, LOW);
        
       }
       else if(Button == 2){    //VERMELHO
        Serial.println("Operação cancelada.");
        digitalWrite(WRI_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        delay(2000);
        digitalWrite(RED_LED, LOW);
        return;
       }
       
    }else{                              //CARTÃO CADASTRADO
      digitalWrite(YEL_LED, HIGH);
      /* ABRIR A PORTA*/
      delay(5000);
      digitalWrite(YEL_LED, LOW);
    }
  }else{                               //NÃO CADASTRADO
    digitalWrite(RED_LED, HIGH);
      /* ABRIR A PORTA*/
      delay(5000);
      digitalWrite(RED_LED, LOW);
  }
  
  
}



/*
 * Armazena o UID nas variáveia globais uid(byte) e uidString(String)
 */
void readRFID(){
  boolean loop = false;
  do{
    if( rfid.PICC_IsNewCardPresent() ){
      loop = true;
      rfid.PICC_ReadCardSerial();
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      // Check is the PICC of Classic MIFARE type
      if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
          piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
          piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("Tag incompatível com o leitor."));
        return;
      }

      uidString = String(rfid.uid.uidByte[0])+" "+String(rfid.uid.uidByte[1])+" "+String(rfid.uid.uidByte[2])+ " "+String(rfid.uid.uidByte[3]);
      for(int i=0; i<4; i++){
        uid[i] = rfid.uid.uidByte[i];
      }
    
      // Halt PICC
      rfid.PICC_HaltA();

      // Stop encryption on PCD
      rfid.PCD_StopCrypto1();

    }
    delay(100);
  }while(loop == false);
}

void addCard(){
  if(!busca()){
    
    int lugar = (EEPROM.read(1023))*4;
    if(lugar < 1020){                                //OS ÚLTIMOS QUATRO ESPAÇOS SÃO RESERVADOS PARA OUTROS FINS
      // laço para percorrer a variável uid[4]
      for(int i=0; i<4;i++){
        EEPROM.write(lugar, uid[i]);
        lugar++;
      }
  
      lugar = EEPROM.read(1023) + 1;
      EEPROM.write(1023, lugar);
      Serial.println("Cartão cadastrado.");
       
    }else{
      Serial.println("Memória cheia.");
    }
  }else{
    Serial.println("Esse cartão já está cadastrado.");
  }
}

/*
 * Confere se o cartão lido está cadastrado, se estiver retorna "true" se não retorna "false"
 */
boolean busca(){
  boolean controle = false;
  int cont = 0;
  byte aux[4];
   int testaIgualdade;
  do{
    //laço para copiar um cartão da memória para a varável
    for(int i=0; i<4;i++){
      aux[i] = EEPROM.read(cont);
      cont++;
    }

    //laço para comparar e ver se o carão copiado da memória é igual ao lido
    testaIgualdade = 0;   //caso o valor final seja 4 então o cartão da memória e o cartão lido são iguais.
    for(int i=0; i<4; i++){
      if(aux[i] == uid[i]){
        testaIgualdade++;
      }
    }

    if(testaIgualdade == 4){
      controle = true;
    }
    
  }while(controle == false && cont < 1024);

  return controle;
}

/*
 * Função que compara o cartão lido com o cartão master, se ambos forem iguais retorna true
 */
boolean comparaMaster(){
  int cont = 0;
  for(int i=0; i<4;i++){
    if(uid[i] == cardMaster[i]){
      cont++;
    }
  }
  
  if(cont == 4){
    return true;
  }else{
    return false;
  }
}

void removerCard(){
  boolean controle = false;
  int cont = 0;
  byte aux[4];
   int testaIgualdade;
  do{
    //laço para copiar um cartão da memória para a varável
    for(int i=0; i<4;i++){
      aux[i] = EEPROM.read(cont);
      cont++;
    }

    //laço para comparar e ver se o carão copiado da memória é igual ao lido
    testaIgualdade = 0;   //caso o valor final seja 4 então o cartão da memória e o cartão lido são iguais.
    for(int i=0; i<4; i++){
      if(aux[i] == uid[i]){
        testaIgualdade++;
      }
    }

    if(testaIgualdade == 4){
      controle = true;
    }
    
  }while(controle == false && cont < 1024);


  if(controle = true && !comparaMaster()){
    
    int aux = cont - 4; //recebe o primerio lugar do cartão que deve ser removido
    do{
      EEPROM.write(aux, EEPROM.read(cont));
      aux++;
      cont++;
    }while (cont < (EEPROM.read(1023)*4));

    for(int i=0; i<4; i++){
      EEPROM.write(aux, 255);
      aux++;
    }
    int aux2 = EEPROM.read(1023) - 1;
    EEPROM.write(1023, aux2); 
    Serial.println("Cartão removido.");
  }else{
    Serial.println("O Cartão Master não pode ser removido.");
  }

}

int esperaButtons(){
  boolean apert = false;
  int indexButton;
  while(!apert){
    for(int i=0; i<3; i++){
      if(digitalRead(pinosButtons[i]) == LOW){
        // i informa qual botão foi pressionado
        indexButton = i; 
        apert = true;
      }
    }
  }
  return indexButton;
}




