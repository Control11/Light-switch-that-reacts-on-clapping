//***********UWAGI**************
//1.  Odzdzielne zasilanie dla przekaznika, jak czujnik i przekaznik podłaczone sa do Arduino
//    to wprowadza zmiany w odczycie gdy przekaznik jest załaczony lub wyłaczony

//2.  Zastosowanie millis() bez uzywania delay() powoduje, że na biezaco sa pobierane odczyty
//    z przerwami tylko na wykonanie zadań

//3. Duzy wplyw na odczyty ma zasilanie, czujnik musi byc zasilany ze stabilnego zrodła (z wyjscia 5V Arduino)

int dzwiek = A0;
bool stan = LOW;
bool spr_stanu = LOW;
int lampa = 8;
int button = 9;
int button_TRYB = 10;
int LEDR = 7;
int LEDG = 6;
unsigned long Time2, Time3, Weryfikacja_startowej, Weryfikacja_startowej1, Czas_probkowania = 0;
unsigned long Time = 2000; //aby nasluch nie wlaczal sie od razu po uruchomieniu
int wartosc = analogRead(dzwiek);
int wartosc_startowa = analogRead(dzwiek)-1; // trzeba odjac bo przez chwile ma wieksza wartosc (wartosc nieustalona)
int liczba_przejsc = 0;
bool TRYB = true;


void setup() {
  Serial.begin(9600);
  pinMode(dzwiek, INPUT);
  pinMode(lampa,OUTPUT);
  pinMode(button,INPUT_PULLUP); 
  pinMode(button_TRYB,INPUT_PULLUP); 
  pinMode(LEDR,OUTPUT);
  pinMode(LEDG,OUTPUT);
  digitalWrite(LEDR, TRYB);
}

void loop() {

//----------------------WERYFIKACJA STARTOWEJ------------------//gdyz wystepuja zmiany napiecia zasilania, a to ciagnie za soba zmiany odczytow wiec trzeba aktualizowac punkt odniesienia
 if (millis()-Weryfikacja_startowej1>1800){    // ustawia co 1,8 s wartosc startowa (stoper resetuje sie przy nasluchu, sprawdzaniu oraz przy nastawieniu nowej startowej wartosci poprzez zmiane stanu lampy) 
      wartosc_startowa = analogRead(dzwiek);  // aby wartosc odniesienia mogla sie zaktualizowac zanim dojdzie do kolejnej inicjalizacji
      Weryfikacja_startowej1 = millis();
  }
if(spr_stanu == !stan && millis()-Weryfikacja_startowej>700){ //gdy zapali lub zgasi lampe (zmiany napiecia i odczytow) i gdy minie minimum 400 ms ustawia nowa wartosc startowa
     wartosc_startowa = analogRead(dzwiek);                   // trzeba poczekac az troszke sie unormuja odczyty dlatego czeka 400 ms
     Weryfikacja_startowej = millis();                           
     Weryfikacja_startowej1 = millis();
      spr_stanu = stan;
  }
//---------------------------POBRANIE ODCZYTU----------------------------------
  wartosc = analogRead(dzwiek);
 
//-----------------WYSWIETLANIE WARTOSCI------------------------- 
//  Serial.print("  Wartosc :  ");
//  Serial.print(wartosc);
//  Serial.print("  Wartosc_startowa :  ");
//  Serial.print(wartosc_startowa);
//  Serial.print("  Probka :  ");
//  Serial.print(probka);
//  Serial.print("  liczba :  ");
//  Serial.println(liczba_przejsc);

//---------------INICJALIZACJA-------------------------------------
  if(millis()-Time2 >=2000){ //Jezeli od poprzedniego sprawdzenia lub konca nasluchu minely 2 s
  if(wartosc>wartosc_startowa+20  || wartosc<wartosc_startowa-20){ //Prog wejscia, aby nie było zbyt latwo uruchomic nasłuchu 
  Serial.println("");
  Serial.println("---------- SKANOWANIE ROZPOCZĘTE ---------");
  Serial.println("");
  
      liczba_przejsc = 0; //zresetowanie liczby przejsc przez obszar sprawdzania
      Time=millis();  
      Time2=millis(); // wlaczenie stopera aby zaraz mogl kontrolowac dlugosc nasluchu (pozniej tez za ile ma sie umozliwic kolejna inicjalizacja)
     }
    }
//-------------------------------NASŁUCH--------------------------------------------- // zlicza z odpowiednia czestotliwoscia probki, ktore nie sa w obszarze startowym, dzieki temu moze wychwycic duzo probek, wiec jest wieksza kontrola nad pozniejszymi warunkami
                                                                                     //jest wiecej mozliwosci wyboru przedzialow dla ktorych bedzie dzialalo wlaczanie/wylaczanie, przy odpowiednio duzej czestotliwosci powinno byc mniej probek niz przy malej czestotliwosci (mala czestotliwosc rzadko przechodzi przez obszar startowy wiec ma duzo probek)
                                                                                     //ograniczenie czasu nasluchu czyli 1 s daje mozliwosc wykrycia odpowiednio krotkich sygnalow
    if(millis()-Time >0 && Time2-Time<=1000) { // nasluch wlacza sie zaraz po skonczeniu inicjalizacji i trwa 1 s (Time2 ciagle ma nowa wartosc podczas nasluchu, dzieki temu moze stanowic odniesienia, a Time ma wartosc nieaktualizowana) 
       Weryfikacja_startowej1 = millis(); 
        if(micros()-Czas_probkowania>=250){ // okres probkowania co 250 us
        Serial.print(" W ");              // Obecna odczytana wartosc                           
        Serial.print(wartosc);
        Serial.print(" Ws ");            // Wartosc startowa - 1  Wartosc startowa + 1
        Serial.print(wartosc_startowa-1); 
        Serial.print(" ");
        Serial.print(wartosc_startowa+1);
        Serial.print(" L ");             // Liczba probek
        Serial.println(liczba_przejsc);
        
        if(wartosc>wartosc_startowa+1 || wartosc<wartosc_startowa-1){ 
        liczba_przejsc=liczba_przejsc+1;  // liczenie z czestotliwoscia probkowania momentow gdy sygnal jest poza obszarem startowym
       }
     Time2 = millis();  //reset stopera aby mogl kontrolowac dlugosc nasluchu, a potem jak nasluch sie skonczy tez do tego za ile ma sie umozliwic kolejna inicjalizacja
    Czas_probkowania = micros(); //reset czasu probkowania
     }
    }
 //-------------------SPRAWDZENIE---------------------------------------- // wlaczanie lub wylaczanie lampy na wykrycie odpowiedniego sygnalu (zakresu odczytow)
    if (liczba_przejsc >= 3 && liczba_przejsc <= 7 && millis()-Time2 > 100 && TRYB == true){//klaskanie
    Serial.println("");
    Serial.println("-------------- WYKRYTO SYGNAL ------------");
    Serial.println("-------------------- ON ------------------");
     Serial.println("");
       digitalWrite(lampa, HIGH); 
        Weryfikacja_startowej = millis(); // resetowanie stopera aby mozna bylo zaraz ustalic nowa wartosc odniesienia
       liczba_przejsc = 0; // reset licznika
    Time2 = millis(); //reset stopera do kontroli za ile ma sie umozliwic kolejna inicjalizacja
   }
    if (liczba_przejsc >= 8 && liczba_przejsc <= 11 && millis()-Time2 > 100 && TRYB == true){//klaskanie
    Serial.println("");
    Serial.println("-------------- WYKRYTO SYGNAL ------------");
    Serial.println("-------------------- OFF -----------------");
    Serial.println("");
       digitalWrite(lampa, LOW); 
        Weryfikacja_startowej = millis(); 
       liczba_przejsc = 0;
    Time2 = millis(); 
   }
       if (liczba_przejsc >= 3 && liczba_przejsc <= 7 && millis()-Time2 > 100 && TRYB == false){//klaskanie
    Serial.println("");
    Serial.println("-------------- WYKRYTO SYGNAL ------------");
    Serial.println("-------------------- ON / OFF ------------------");
     Serial.println("");
       digitalWrite(lampa, stan=!stan); 
        Weryfikacja_startowej = millis(); 
       liczba_przejsc = 0;
    Time2 = millis(); 
   }
  //-------------------------------KONIEC UZYWANIA CZUJNIKA----------------------------------------------------

 //--------------------------------WLACZANIE PRZYCISKIEM-----------------------------------------
  if (digitalRead(button) == LOW && millis()-Time3 >= 1000){ // wlaczenie lub wylaczenie przyciskiem dostepne co 1 s (ominiecie drgan stykow)
      stan=!stan;
      digitalWrite(lampa, stan); // wlacza/wylacza przekaznik (wlacz/wylacz lampe)
      Time3 = millis(); 
     Weryfikacja_startowej = millis(); 
       Time2 = millis(); 
   }
   if (digitalRead(button_TRYB) == LOW && millis()-Time3 >= 1000){ // wlaczenie lub wylaczenie przyciskiem dostepne co 1 s (ominiecie drgan stykow)
  TRYB = !TRYB;
  digitalWrite(LEDR, TRYB);
  digitalWrite(LEDG, !TRYB);
  Time3 = millis();
   }
}
