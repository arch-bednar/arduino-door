#include<SPI.h>
#include<Servo.h>
#include<MFRC522.h>

#define greenPin 2 //dioda rgb kolor zielony
#define redPin 3 //dioda rgb kolor czerwony
#define RST_PIN 9 //Configurable, see typical pin layout above
#define SS_PIN 10 //Configurable, see typical layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo sg90;

int flaga = 1;
int rozmiar = 10;

bool otwarte = true;

int klucze1[11] = {0x3A, 0xC9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int klucze2[11] = {0x9B, 0xF3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int klucze3[11] = {0x30, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int klucze4[11] = {0x15, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int i;

void setup() {
  sg90.attach(5);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  SPI.begin();  //Init SPI bus
  mfrc522.PCD_Init();
  otworz();
  Serial.begin(9600);
}

void loop() {
//Serial.println("flaga ");
//Serial.println(flaga);

sprawdz(otwarte);
  if( ! mfrc522.PICC_IsNewCardPresent()) { //sprawdza czy wykryto kartę, jeś
    Serial.println(flaga);
//          for(int k = 0; k < 10; k++){
//              Serial.println(klucze1[k]);
//              Serial.println(klucze2[k]);
//              Serial.println( klucze3[k]);
//              Serial.println( klucze4[k]);
//              Serial.println("xxxx");
//              delay(1000);
//              }
    delay(1000);
    return;
  }

  if( ! mfrc522.PICC_ReadCardSerial()) {
//    delay(1000);
    return;
  }

    
  //////////warunek sprawdza czy przyłożona karta to brelok do dodawania i usuwania kart      
  if (mfrc522.uid.uidByte[0] == klucze1[0] && //
      mfrc522.uid.uidByte[1] == klucze2[0] && 
      mfrc522.uid.uidByte[2] == klucze3[0] && 
      mfrc522.uid.uidByte[3] == klucze4[0]) {
        Serial.println("Masz 5 sekund na przylozenie nowej karty albo przyloz brelok by wrocic");
        zaczekaj();
        delay(5000);
        if ( ! mfrc522.PICC_IsNewCardPresent()){
          Serial.println("nie wykryto nowej lub starej karty");
          return;
        }
        if( ! mfrc522.PICC_ReadCardSerial()){
          Serial.println("nie wykryto nowej lub starej karty");
          return;
        }
        
          if (mfrc522.uid.uidByte[0] == klucze1[0] && //konczy petle jesli przylozymy brelok raz jeszcze
              mfrc522.uid.uidByte[1] == klucze2[0] && 
              mfrc522.uid.uidByte[2] == klucze3[0] && 
              mfrc522.uid.uidByte[3] == klucze4[0]) {
                Serial.println("powrot");
                return;
          }

          else{  
            for( i = 1; i <= flaga; i++){
              if (mfrc522.uid.uidByte[0] == klucze1[i] && //
                  mfrc522.uid.uidByte[1] == klucze2[i] && 
                  mfrc522.uid.uidByte[2] == klucze3[i] && 
                  mfrc522.uid.uidByte[3] == klucze4[i]){
                    usun_karte(klucze1, klucze2, klucze3, klucze4, i);
                    Serial.println("usunieto karte");
                    flaga--;
                    return;
                  }
            }
            if (flaga == 3){
              Serial.println("za duzo kart");
              return;
            }
            else{
              flaga++;
              dodaj_karte(klucze1, klucze2, klucze3, klucze4, flaga, mfrc522.uid.uidByte[0], mfrc522.uid.uidByte[1], mfrc522.uid.uidByte[2], mfrc522.uid.uidByte[3]);
              Serial.println("dodano nową kartę");
              delay(1000);
              return;
            }
            
          }

        sprawdz(otwarte);
          
      }
    ///////////////////////////////



//  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  for ( i = 1; i <= flaga; i++){
    
    ////// warunek sprawdza czy karta znajduje się w bazie
      if (mfrc522.uid.uidByte[0] == klucze1[i] && //
          mfrc522.uid.uidByte[1] == klucze2[i] && 
          mfrc522.uid.uidByte[2] == klucze3[i] && 
          mfrc522.uid.uidByte[3] == klucze4[i]) {
        
            if ( otwarte == true) {
              Serial.println("otwarto");
              delay(1000);
              otworz();
              digitalWrite(greenPin, HIGH);
              digitalWrite(redPin, LOW);
              otwarte = false;
              delay(1000);
              return;
            }
            else if (otwarte == false){
              Serial.println("zamknij");
              delay(1000);
              zamknij();
              digitalWrite(greenPin, LOW);
              digitalWrite(redPin, HIGH);
              otwarte = true;
              delay(1000);
              return;
            }
       }
    //////////////
  }//for
    
  Serial.println("Odmowa dostepu");
  Serial.println("koniec petli");
  delay(1000);
} //void loop

void zamknij(){
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW);
  sg90.write(112);
}

void otworz(){
  digitalWrite(greenPin, HIGH);
  digitalWrite(redPin, LOW);
  sg90.write(15);

}

void zaczekaj(){
  digitalWrite(greenPin, HIGH);
  digitalWrite(redPin, HIGH);
}

void sprawdz(bool var){
  if(var == true){
    otworz();
  }
  else{
    zamknij();
  }
}
void dodaj_karte(int tab1[11], int tab2[11], int tab3[11], int tab4[11], int pozycja, int karta1, int karta2, int karta3, int karta4){
  tab1[pozycja] = karta1;
  tab2[pozycja] = karta2;
  tab3[pozycja] = karta3;
  tab4[pozycja] = karta4;
}

void usun_karte(int tab1[11], int tab2[11], int tab3[11], int tab4[11], int pozycja){
  for(int j = pozycja; j < 10; j++){
    tab1[j] = tab1[j+1];
    tab2[j] = tab2[j+1];
    tab3[j] = tab3[j+1];
    tab4[j] = tab4[j+1];
  }
}
