#include <Wire.h>
#include <DS3231.h>
#include <Adafruit_ADS1015.h>
#include <LiquidCrystal_I2C.h>
#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>

#define I2C_RTC_ADDR 0x68  //indirizzo modulo rtc
#define I2C_ADC_ADDR 0x48  //indirizzo modulo adc
#define I2C_LCD_ADDR 0x27  //indirizzo lcd
#define BACKLIGHT_PIN 3  //schermo lcd acceso
#define En_pin 2  //schermo lcd
#define Rw_pin 1  //schermo lcd
#define Rs_pin 0  //schermo lcd
#define D4_pin 4  //schermo lcd
#define D5_pin 5  //schermo lcd
#define D6_pin 6  //schermo lcd
#define D7_pin 7  //schermo lcd

RTClib RTC;
Adafruit_ADS1115 ads(I2C_ADC_ADDR);  //indirizzo modulo adc
LiquidCrystal_I2C lcd(I2C_LCD_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);  // indirizzo display lcd

/*PIN PULSANTI*/
int forward=2;  //pin tasto enter1
int up=3;  //pin tasto up
int down=4;  //pin tasto down

/*INPUT PULSANTI*/
int val_forward;  //variabile utilizzata per sapere l input del pulsante 
int val_up;  //variabile utilizzata per sapere l input del pulsante 
int val_down;  //variabile utilizzata per sapere l input del pulsante

/*STATO PULSANTI*/
int forwardstate;  //variabile utilizzata per sapere lo stato del pulsante
int upstate;  //variabile utilizzata per sapere lo stato del pulsante
int downstate;  //variabile utilizzata per sapere lo stato del pulsante

/*VARIABILI NAVIGAZIONE MENU*/
int enter1=0;  //variabile utilizzata per entrare e confermare
int stats=0;  //variabile utilizzata per scorrere menu

/*VARIABILI PER MODIFICA PARAMETRI*/
int mod1=0;  //variabile utilizzata per scorrere i parametri
int mod2=0;  //variabile utilizzata per scorrere i parametri
int mod3=0;  //variabile utilizzata per scorrere i parametri
int mod4=0;  //variabile utilizzata per scorrere i parametri
int mod5=0;  //variabile utilizzata per scorrere i parametri

/*VISUALIZZAZIONI A DISPLAY*/
int printed=0;  //variabile utilizzata per scrivere una sola volta un menù o un opzione

/*LUNGHEZZA PREDEFINITA ARRAY*/
int size1=64;  //lunghezza array necessaria per settare le variabili al valore corrente
int size2=16;  //lunghezza array necessaria per settare le variabili al valore corrente
int size3=4;  //lunghezza array necessaria per settare le variabili al valore corrente
int size4=3;  //lunghezza array necessaria per settare le variabili al valore corrente
int size5=15;  //lunghezza array necessaria per calcolare la temperatura media

/*ARRAY CON PARAMETRI MODIFICABILI*/
unsigned long standbyidle[16]={480,540,600,660,720,780,840,900,960,1020,1080,1140,1200,1500,1800,2400};  //array con valori di tempo di standby in millisecondi
float toptemp[64]={0,1,2,3,4,5,5.33,5.66,6,6.33,6.66,7,7.33,7.66,8,8.33,8.66,9,9.33,9.66,10,10.33,10.66,11,11.33,11.66,12,12.33,12.66,13,13.33,13.66,14,14.33,14.66,15,15.33,15.66,16,16.33,16.66,17,17.33,17.66,18,18.33,18.66,19,19.33,19.66,20,20.33,20.66,21,21.33,21.66,22,22.33,22.66,23,23.33,23.66,24,25};  //array con valori di temperatura massima camera di fermentazione in gradi celsius
unsigned long coldstartup[16]={40,50,60,70,80,90,100,110,120,130,140,150,180,360,720,1440};  //array con valori di tempo di accensione raffreddamento in millisecondi
unsigned long coldidle[16]={480,540,600,660,720,780,840,900,960,1020,1080,1140,1200,1500,1800,2400};  //array con valori di tempo di standby raffreddamento in millisecondi
float bottomtemp[64]={0,1,2,3,4,5,5.33,5.66,6,6.33,6.66,7,7.33,7.66,8,8.33,8.66,9,9.33,9.66,10,10.33,10.66,11,11.33,11.66,12,12.33,12.66,13,13.33,13.66,14,14.33,14.66,15,15.33,15.66,16,16.33,16.66,17,17.33,17.66,18,18.33,18.66,19,19.33,19.66,20,20.33,20.66,21,21.33,21.66,22,22.33,22.66,23,23.33,23.66,24,25};  //array con valori di temperatura minima camera di fermentazione in gradi celsius
unsigned long heatstartup[16]={40,50,60,70,80,90,100,110,120,130,140,150,180,360,720,1440};  //array con valori di tempo di accensione riscaldamento in millisecondi
unsigned long heatidle[16]={480,540,600,660,720,780,840,900,960,1020,1080,1140,1200,1500,1800,2400};  //array con valori di tempo di standby riscaldamento in millisecondi
int controltemp[4]={20,30,40,50};  //array con valori di temperatura centralina in gradi celsius
unsigned long mainmenu[4]={30,45,60,120};  //array con valori di tempo da attendere prima di tornare alla home del display in millisecondi
unsigned long backlight[4]={5,15,60,2400};  //array con valori di tempo di accensione illuminazione display lcd in millisecondi

/*VARIABILI PER IMPOSTARE PARAMETRI*/
unsigned long sbidle=600;  //variabile utilizzata per impostare il tempo di standby generico del raffreddamento e del riscaldamento
float maxtemp=19;  //variabile utilizzata per impostare la temperatura massima della camera di fermentazione in gradi celsius
unsigned long cstartup=70;  //variabile utilizzata per impostare il tempo di accensione del raffreddamento
unsigned long cidle=600;  //variabile utilizzata per impostare il tempo di standby del raffreddamento
float mintemp=18;  //variabile utilizzata per impostare la temperatura minima della camera di fermentazione in gradi celsius
unsigned long hstartup=60;  //variabile utilizzata per impostare il tempo di accensione del riscaldamento
unsigned long hidle=600;  //variabile utilizzata per impostare il tempo di standby del riscaldamento
int maxcputemp=30;  //variabile utilizzata per impostare la temperatura massima della centralina in gradi celsius
unsigned long home=30;  //variabile utilizzata per tornare alla home dopo un certo tempo
unsigned long blon=60;  //variabile utilizzata per impostare il tempo di accensione illuminazione display lcd

/*ARRAY ACCENSIONE FORZATA*/
int forcecold[3]={0,1,2};  //array con valori di force start
int forceheat[3]={0,1,2};  //array con valori di force start
int forcechamberfan[3]={0,1,2};  //array con valori di force start
int forcecpufan[3]={0,1,2};  //array con valori di force start

/*VARIABILI ACCENSIONE FORZATA*/
int fcold=1;  //variabile utilizzata per abilitare/disabilitare/forzare raffreddamento
int fheat=1;  //variabile utilizzata per abilitare/disabilitare/forzare riscaldamento
int fff=1;  //variabile utilizzata per abilitare/disabilitare/forzare ventilazione camera di fermentazione
int fcf=1;  //variabile utilizzata per abilitare/disabilitare/forzare ventilazione cpu

/*COSTANTI ACCENSIONE FORZATA*/
unsigned long fstartup=1800;  //costante utilizzata per impostare il tempo di accensione raffreddamento/riscaldamento in accensione forzata
unsigned long fidle=900;  //costante utilizzata per impostare il tempo di standby raffreddamento/riscaldamento in accensione forzata

/*STATO DEL SISTEMA*/
int mode=5;  //variabile utilizzata per sapere lo stato del sistema

/*VARIABILE TEMPO SISTEMA ED EVENTI*/
unsigned long intervaltime;  //variabile per memorizzare il tempo e calcolare gli intervalli

/*ESEGUI OGNI 1 SECONDO*/
unsigned long clock1=1;  //intervallo di tempo tra rilevazioni di temperatura
unsigned long onesec=0;  //variabile utilizzata per memorizzare il momento dell ultima rilevazione di temperatura

/*ESEGUI OGNI 15 SECONDI*/
unsigned long clock15=15;  //intervallo di tempo tra misurazioni a display
unsigned long fifteensec=0;  //variabile utilizzata per memorizzare il momento dell ultima misurazione visualizzata a display

/*TIMER*/
unsigned long target=0;  //variabile utilizzata come timer accensioni/standby raffreddamento
unsigned long target1=0;  //variabile utilizzata come timer accensioni/standby riscaldamento
unsigned long target2=0;  //variabile utilizzata come timer accensioni/standby raffreddamento/riscaldamento in accensione forzata
unsigned long light=0;  //variabile utilizzata come timer accensioni illuminazione display lcd
unsigned long menu=0;  //variabile utilizzata come timer per tornare al menu principale

/*ACCENSIONI*/
byte refrigeration=LOW;  //stato raffreddamento spento
byte heating=LOW;  //stato riscaldamento spento
byte cpufan=LOW;  //ventilazione centralina pwm spenta
byte ventilation=LOW;  //ventilazione camera di fermentazione pwm spenta

/*CONTATORI*/
int countcoldstartup=0;  //contatore accensioni raffreddamento
int countcoldidle=0;  //contatore stanbdy raffreddamento
int countheatstartup=0;  //contatore accensioni riscaldamento
int countheatidle=0;  //contatore standby riscaldamento
int countstandbyidle=0;  //contatore standby generico
int countforcedcoldstartup=0;  //contatore accensioni forzate raffreddamento
int countforcedheatstartup=0;  //contatore accensioni forzate riscaldamento
int countforcedidle=0;  //contatore standby forzato generico
int countforcedfermentationfanstartup=0;  //contatore accensioni ventilazione camera fermentazione
int countforcedcpufanstartup=0;  //contatore accensioni ventilazione cpu

/*RILEVAZIONE SEGNALE ANALOGICO*/
int16_t Val1;  //valore analogico sonda di temperatura
int Val2;  //valore analogico sensore di temperatura

/*COSTANTI DEL CIRCUITO*/
float supply=4.995;  //tensione alimentazione sonda temperatura e ADC
float r0=100;  //costante di resistenza sonda pt100 a temperatura 0 gradi centigradi
float r1=1000;  //valore resistenza in serie alla sonda temperatura

/*COSTANTI SONDA DI TEMPERATURA*/
float mline=0.380999999;  //coeffiente angolare curva sonda di temperatura

/*VARIABILI PER IL CALCOLO TEMPERATURA CAMERA DI FERMENTAZIONE*/
float voltage;  //variabile per il calcolo del voltaggio sonda di temperatura
float resistance;  //variabile per il calcolo della resistenza sonda di temperatura

/*RISOLUZIONE IN VOLT GAIN ADC 30 BITS*/
//float resolution1=0.000187505722220526749473555;  //GAIN TWOTHIRDS
//float resolution1=0.000125003814813684499649037;  //GAIN ONE
//float resolution1=0.000062501907406842249824518;  //GAIN TWO
float resolution1=0.000031250953703421124912259;  //GAIN FOUR
//float resolution1=0.000015625476851710562456129;  //GAIN EIGHT
//float resolution1=0.000007812738425855281228064;  //GAIN SIXTEEN

/*RISOLUZIONE IN VOLT ARDUINO*/
float resolution2=0.48779296875;  //=((supply*1000)/1024)/10mV LM35DZ

/*RILEVAZIONI TEMPERATURA MEDIA CAMERA FERMENTAZIONE*/
float aft[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  //array per le rilevazioni di temperatura fermentazione ogni secondo per 15 secondi
float fermtemp;  //valore di temperatura della camera di fermentazione in gradi celsius rilevato ogni secondo
float fermtemptotal;  //variabile utilizzata per la somma delle temperature misurate contenute nell array aft[i] e necessario per calcolare la media
float fermtempaverage=18;  //variabile utilizzata per memorizzare la temperatura media della camera di fermentazione

/*RILEVAZIONI TEMPERATURA MEDIA CPU*/
float act[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  //array per le rilevazioni di temperatura centralina ogni secondo per 15 secondi
float cputemp;  //valore di temperatura della centralina in gradi celsius rilevato ogni secondo
float cputemptotal;  //variabile utilizzata per la somma delle temperature misurate contenute nell array act[q] e necessario per calcolare la media
float cputempaverage=25;  //variabile utilizzata per memorizzare la temperatura media della centralina

void setup(){
  pinMode(2,INPUT);  //imposta il pin digitale come input
  pinMode(3,INPUT);  //imposta il pin digitale come input
  pinMode(4,INPUT);  //imposta il pin digitale come input
  pinMode(8,OUTPUT);  //imposta il pin digitale come input
  pinMode(9,OUTPUT);  //imposta il pin digitale come input
  pinMode(10,OUTPUT);  //imposta il pin digitale come input
  pinMode(11,OUTPUT);  //imposta il pin digitale come input
  Serial.begin(9600);
  Wire.begin();
  ads.begin();  //inizializza il modulo ads
  ads.setGain(GAIN_FOUR);
  lcd.begin(20,4);  //inizializza il display
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);  //accendi illuminazione lcd
  lcd.setBacklight(HIGH);  //accendi illuminazione lcd
  lcd.home();  //vai alla home dell lcd
  delay(2000);  //attendi 2 secondi
  lcd.setCursor(6,1);  //setta il cursore in posizione
  lcd.print("Lovibrau");  //scrivi "Lovibrau"
  lcd.setCursor(4,2);  //setta il cursore in posizione
  lcd.print("fermentation");  //scrivi "fermentation"
  delay(4000);  //attendi 4 secondi
  lcd.clear();  //pulisci lo schermo
}

void loop(){
  /*ISTRUZIONE PER MEMORIZZARE IL TEMPO DEL SISTEMA*/
  DateTime now=RTC.now();
  intervaltime=now.unixtime();
  /*ISTRUZIONE PER USCITA A TEMPO DEI MENU*/
  if((long)(intervaltime-menu)>=0){  //se il tempo trascorso è maggiore di menu
    enter1=0;  //resetta la variabile
    stats=0;  //resetta la variabile
    mod1=0;  //resetta la variabile
    mod2=0;  //resetta la variabile
    mod3=0;  //resetta la variabile
    mod4=0;  //resetta la variabile
    mod5=0;  //resetta la variabile
    printed=0;  //resetta la variabile
  }
  /*COMANDI PER SCORRERE MENU E PARAMETRI*/
  val_up=digitalRead(up);  //leggi lo stato del pulsante up
  val_down=digitalRead(down);  //leggi lo stato del pulsante down
  if(val_up!=upstate){  //se l input del pulsante è diverso dalla variabile di stato
    upstate=val_up;  //salva l input del pulsante nella variabile di stato
    if(upstate==HIGH){  //se lo stato del pulsante è high
      if(enter1==0){  //se enter1 è uguale a zero
        stats++;  //incrementa la variabile
      }
      if(enter1!=0){  //se enter1 è diverso da zero
        mod1++;  //incrementa la variabile
        mod2++;  //incrementa la variabile
        mod3++;  //incrementa la variabile
        mod4++;  //incrementa la variabile
        mod5++;  //incrementa la variabile
      }
      lcd.setBacklight(HIGH);  //accendi illuminazione lcd
      light=intervaltime+blon;  //imposta timer illuminazione display lcd
      printed=0;  //resetta la variabile
    }
  }
  if(val_down!=downstate){  //se l input del pulsante è diverso dalla variabile di stato
    downstate=val_down;  //salva l input del pulsante nella variabile di stato
    if(downstate==HIGH){  //se lo stato del pulsante è high
      if(enter1==0){  //se enter1 è uguale a zero
        stats--;  //decrementa la variabile
      }
      if(enter1!=0){  //se enter1 è diverso da zero
        mod1--;  //decrementa la variabile
        mod2--;  //decrementa la variabile
        mod3--;  //decrementa la variabile
        mod4--;  //decrementa la variabile
        mod5--;  //decrementa la variabile
      }
      lcd.setBacklight(HIGH);  //accendi illuminazione lcd
      light=intervaltime+blon;  //imposta timer illuminazione display lcd
      printed=0;  //resetta la variabile
    }
  }
  if(stats>15||stats<0){
    stats=0;
  }
  /*SCHERMATA PRINCIPALE*/
  if(enter1==0&&stats==0&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 0 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MAIN MENU           ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("Temperature:");  //scrivi
    lcd.setCursor(12,1);  //posiziona il cursore a
    if(fermtempaverage==0){
      lcd.print(fermtemp,2);
    }
    else{
      lcd.print(fermtempaverage,2);  //scrivi la variabile fermtempaverage con due cifre dopo la virgola
    }
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Setpoints:");  //scrivi
    lcd.setCursor(10,2);  //posiziona il cursore a
    lcd.print(mintemp,1);  //scrivi
    lcd.setCursor(14,2);  //posiziona il cursore a
    lcd.print("-");  //scrivi
    lcd.setCursor(15,2);  //posiziona il cursore a
    lcd.print(maxtemp,1);  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print("Status:");  //scrivi
    if(mode==1){  //se lo stato del sistema è uguale a 1
      lcd.setCursor(7,3);  //posiziona il cursore a
      lcd.print("cold on");  //scrivi
    }
    else if(mode==2){  //se lo stato del sistema è uguale a 2
      lcd.setCursor(7,3);  //posiziona il cursore a
      lcd.print("heat on");  //scrivi
    }
    else if(mode==3){  //se lo stato del sistema è uguale a 3
      lcd.setCursor(7,3);  //posiziona il cursore a
      lcd.print("cooling");  //scrivi
    }
    else if(mode==4){  //se lo stato del sistema è uguale a 4
      lcd.setCursor(7,3);  //posiziona il cursore a
      lcd.print("warming");  //scrivi
    }
    else if(mode==5){  //se lo stato del sistema è uguale a 5
      lcd.setCursor(7,3);  //posiziona il cursore a
      lcd.print("off");  //scrivi
    }
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  /*VISUALIZZAZIONE MENU*/
  if(enter1==0&&stats==1&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 1  e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("COUNTERS STATS      ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to visualize  ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("                    ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print("                    ");  //scrivi
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==0&&stats==2&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 2 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Standby idle time   ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(sbidle);  //scrivi la variabile sbidle
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==3&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 3 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Max temperature     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(maxtemp,2);  //scrivi la variabile maxtemp con due cifre dopo la virgola
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==4&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 4 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cold startup time   ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(cstartup);  //scrivi la variabile cstartup
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==5&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 5 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cold idle time      ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(cidle);  //scrivi la variabile cidle
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==6&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 6 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Min temperature     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(mintemp,2);  //scrivi la variabile mintemp con due cifre dopo la virgola
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==7&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 7 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Heat startup time   ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(hstartup);  //scrivi la variabile hstartup
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==8&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 8 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Heat idle time      ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(hidle);  //scrivi la variabile hidle
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==9&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 9 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("CPU temperature     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(maxcputemp,1);  //scrivi la variabile maxcputemp con una cifra dopo la virgola
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==10&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 10 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Exit from menu time ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(home);  //scrivi la variabile sbidle
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==11&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 11 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Backlight on time   ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(blon);  //scrivi la variabile blon
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==12&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 12 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("SERVICE STATUS      ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cold startup        ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(fcold);  //scrivi la variabile fcold
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==13&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 13 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("SERVICE STATUS      ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Heat startup        ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(fheat);  //scrivi la variabile fcold
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==14&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 14 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("SERVICE STATUS      ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Chamber fan startup ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(fff);  //scrivi la variabile fcold
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==0&&stats==15&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 15 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("SERVICE STATUS      ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("CPU fan startup     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(fcf);  //scrivi la variabile fcold
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  /*COMANDI PER ENTRARE NELLA MODALITA' VISUALIZZA STATISTICHE CONTATORI*/  //necessario perché il blocco VISUALIZZAZIONE MENU viene eseguito una sola volta
  if(enter1==0&&stats==1){  //se enter1 è uguale a 0 stats è uguale a 1
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=1;  //setta la variabile a 1
        printed=0;  //resetta la variabile
      }
    }
  }
  /*COMANDI PER ENTRARE NELLA MODALITA' MODIFICA*/  //necessario perché il blocco VISUALIZZAZIONE MENU viene eseguito una sola volta
  if(enter1==0&&stats==2){  //se enter1 è uguale a 0 stats è uguale a 2
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        for(int i=0;i<(size2-1);i++){
          if(standbyidle[i]==sbidle){
            mod2=i;
            break;
          }
        }
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=2;  //setta la variabile a 2
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==3){  //se enter1 è uguale a 0 stats è uguale a 3
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size1-1);i++){
          if(toptemp[i]==maxtemp){
            mod1=i;
            break;
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=3;  //setta la variabile a 3
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==4){  //se enter1 è uguale a 0 stats è uguale a 4
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        for(int i=0;i<(size2-1);i++){
          if(coldstartup[i]==cstartup){
            mod2=i;
            break;
          }
        }
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=4;  //setta la variabile a 4
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==5){  //se enter1 è uguale a 0 stats è uguale a 5
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        for(int i=0;i<(size2-1);i++){
          if(coldidle[i]==cidle){
            mod2=i;
            break;
          }
        }
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=5;  //setta la variabile a 5
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==6){  //se enter1 è uguale a 0 stats è uguale a 6
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size1-1);i++){
          if(bottomtemp[i]==mintemp){
            mod1=i;
            break;
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=6;  //setta la variabile a 6
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==7){  //se enter1 è uguale a 0 stats è uguale a 7
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        for(int i=0;i<(size2-1);i++){
          if(heatstartup[i]==hstartup){
            mod2=i;
            break;
          }
        }
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=7;  //setta la variabile a 7
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==8){  //se enter1 è uguale a 0 stats è uguale a 8
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        for(int i=0;i<(size2-1);i++){
          if(heatidle[i]==hidle){
            mod2=i;
            break;
          }
        }
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=8;  //setta la variabile a 8
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==9){  //se enter1 è uguale a 0 stats è uguale a 9
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        for(int i=0;i<(size3-1);i++){
          if(controltemp[i]==maxcputemp){
            mod3=i;
            break;
          }
        }
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=9;  //setta la variabile a 9
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==10){  //se enter1 è uguale a 0 stats è uguale a 10
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        for(int i=0;i<(size3-1);i++){
          if(mainmenu[i]==home){
            mod3=i;
            break;
          }
        }
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=10;  //setta la variabile a 10
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==11){  //se enter1 è uguale a 0 stats è uguale a 11
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        for(int i=0;i<(size3-1);i++){
          if(backlight[i]==blon){
            mod3=i;
            break;
          }
        }
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=11;  //setta la variabile a 11
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==12){  //se enter1 è uguale a 0 stats è uguale a 12
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        for(int i=0;i<(size4-1);i++){
          if(forcecold[i]==fcold){
            mod4=i;
            break;
          }
        }
        mod5=0;  //resetta la variabile
        enter1=12;  //setta la variabile a 12
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==13){  //se enter1 è uguale a 0 stats è uguale a 13
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        for(int i=0;i<(size4-1);i++){
          if(forceheat[i]==fheat){
            mod4=i;
            break;
          }
        }
        mod5=0;  //resetta la variabile
        enter1=13;  //setta la variabile a 13
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==14){  //se enter1 è uguale a 0 stats è uguale a 14
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        for(int i=0;i<(size4-1);i++){
          if(forcechamberfan[i]==fff){
            mod4=i;
            break;
          }
        }
        mod5=0;  //resetta la variabile
        enter1=14;  //setta la variabile a 14
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==15){  //se enter1 è uguale a 0 stats è uguale a 15
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l'input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        for(int i=0;i<(size4-1);i++){
          if(forcecpufan[i]==fcf){
            mod4=i;
            break;
          }
        }
        mod5=0;  //resetta la variabile
        enter1=15;  //setta la variabile a 15
        printed=0;  //resetta la variabile
      }
    }
  }
  /*VISUALIZZAZIONE MENU STATISTICHE CONTATORI*/
  if(enter1==1&&stats==1&&printed==0){  //se enter1 è uguale a 1 stats è uguale a 1 printed è uguale a 0
    if(mod5==0){
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod5>9||mod5<0){  //se la variabile mod5 è maggiore di 9 o minore di 0
        mod5=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Cold startups       ");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countcoldstartup);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod5==1){
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod5>9||mod5<0){  //se la variabile mod5 è maggiore di 9 o minore di 0
        mod5=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Cold idles          ");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countcoldidle);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod5==2){
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod5>9||mod5<0){  //se la variabile mod5 è maggiore di 9 o minore di 0
        mod5=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Heat startups       ");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countheatstartup);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod5==3){
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod5>9||mod5<0){  //se la variabile mod5 è maggiore di 9 o minore di 0
        mod5=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Heat idles          ");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countheatidle);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod5==4){
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod5>9||mod5<0){  //se la variabile mod5 è maggiore di 9 o minore di 0
        mod5=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Standby idles       ");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countstandbyidle);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod5==5){
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod5>9||mod5<0){  //se la variabile mod5 è maggiore di 9 o minore di 0
        mod5=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Forced cold startups");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countforcedcoldstartup);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod5==6){
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod5>9||mod5<0){  //se la variabile mod5 è maggiore di 9 o minore di 0
        mod5=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Forced heat startups");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countforcedheatstartup);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod5==7){
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod5>9||mod5<0){  //se la variabile mod5 è maggiore di 9 o minore di 0
        mod5=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Forced idles        ");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countforcedidle);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod5==8){
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod5>9||mod5<0){  //se la variabile mod5 è maggiore di 9 o minore di 0
        mod5=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Forced ffan startups");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countforcedfermentationfanstartup);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod5==9){
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod5>9||mod5<0){  //se la variabile mod5 è maggiore di 9 o minore di 0
        mod5=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Forced cfan startups");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countforcedcpufanstartup);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
  }
  /*VISUALIZZAZIONE MENU DI MODIFICA*/
  if(enter1==2&&stats==2&&printed==0){  //se enter1 è uguale a 2 stats è uguale a 2 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod2>15||mod2<0){  //se la variabile mod2 è maggiore di 15 o minore di 0
      mod2=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Standby idle time   ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(standbyidle[mod2]);  //scrivi l indice dell array standbyidle corrispondente al valore mod2
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==3&&stats==3&&printed==0){  //se enter1 è uguale a 3 stats è uguale a 3 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod1>63||mod1<0){  //se la variabile mod1 è maggiore di 63 o minore di 0
      mod1=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Max temperature     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(toptemp[mod1],2);  //scrivi l indice dell array toptemp corrispondente al valore mod1 con due cifre dopo la virgola
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==4&&stats==4&&printed==0){  //se enter1 è uguale a 4 stats è uguale a 4 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod2>15||mod2<0){  //se la variabile mod2 è maggiore di 4 o minore di 0
      mod2=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cold startup time   ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(coldstartup[mod2]);  //scrivi l indice dell array coldstartup corrispondente al valore mod2
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==5&&stats==5&&printed==0){  //se enter1 è uguale a 5 stats è uguale a 5 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod2>15||mod2<0){  //se la variabile mod2 è maggiore di 4 o minore di 0
      mod2=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cold idle time      ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(coldidle[mod2]);  //scrivi l indice dell array coldidle corrispondente al valore mod2
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==6&&stats==6&&printed==0){  //se enter1 è uguale a 6 stats è uguale a 6 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod1>63||mod1<0){  //se la variabile mod1 è maggiore di 63 o minore di 0
      mod1=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Min temperature     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(bottomtemp[mod1],2);  //scrivi l indice dell array bottomtemp corrispondente al valore mod1 con due cifre dopo la virgola
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==7&&stats==7&&printed==0){  //se enter1 è uguale a 7 stats è uguale a 7 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod2>15||mod2<0){  //se la variabile mod2 è maggiore di 4 o minore di 0
      mod2=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Heat startup time   ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(heatstartup[mod2]);  //scrivi l indice dell array heatstartup corrispondente al valore mod2
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==8&&stats==8&&printed==0){  //se enter1 è uguale a 8 stats è uguale a 8 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod2>15||mod2<0){  //se la variabile mod2 è maggiore di 4 o minore di 0
      mod2=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Heat idle time      ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(heatidle[mod2]);  //scrivi l indice dell array heatidle corrispondente al valore mod2
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==9&&stats==9&&printed==0){  //se enter1 è uguale a 9 stats è uguale a 9 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod3>3||mod3<0){  //se la variabile mod3 è maggiore di 4 o minore di 0
      mod3=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Control temperature ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(controltemp[mod3]);  //scrivi l indice dell array controltemp corrispondente al valore mod3
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==10&&stats==10&&printed==0){  //se enter1 è uguale a 10 stats è uguale a 10 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod3>3||mod3<0){  //se la variabile mod3 è maggiore di 3 o minore di 0
      mod3=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY  PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Exit from menu time ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(mainmenu[mod3]);  //scrivi l indice dell array mainmenu corrispondente al valore mod3
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==11&&stats==11&&printed==0){  //se enter1 è uguale a 11 stats è uguale a 11 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod3>3||mod3<0){  //se la variabile mod3 è maggiore di 4 o minore di 0
      mod3=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY  PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("LCD backlight time  ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(backlight[mod3]);  //scrivi l indice dell array backlight corrispondente al valore mod3
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==12&&stats==12&&printed==0){  //se enter1 è uguale a 12 stats è uguale a 12 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod4>2||mod4<0){  //se la variabile mod4 è maggiore di 2 o minore di 0
      mod4=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("STARTUPS FORCING   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("0:disable 1:off 2:on");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cold startup        ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(forcecold[mod4]);  //scrivi l indice dell array forcecold corrispondente al valore mod4
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==13&&stats==13&&printed==0){  //se enter1 è uguale a 13 stats è uguale a 13 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod4>2||mod4<0){  //se la variabile mod4 è maggiore di 2 o minore di 0
      mod4=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("STARTUPS FORCING   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("0:disable 1:off 2:on");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Heat startup        ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(heatstartup[mod4]);  //scrivi l indice dell array heatstartup corrispondente al valore mod4
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==14&&stats==14&&printed==0){  //se enter1 è uguale a 14 stats è uguale a 14 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod4>2||mod4<0){  //se la variabile mod4 è maggiore di 2 o minore di 0
      mod4=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("STARTUPS FORCING   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("0:disable 1:off 2:on");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Chamber fan startup ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(forcechamberfan[mod4]);  //scrivi l indice dell array forcechamberfan corrispondente al valore mod4
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  if(enter1==15&&stats==15&&printed==0){  //se enter1 è uguale a 15 stats è uguale a 15 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod4>2|mod4<0){  //se la variabile mod4 è maggiore di 2 o minore di 0
      mod4=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("STARTUPS FORCING   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("0:disable 1:off 2:on");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("CPU fan startup     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(forcecpufan[mod4]);  //scrivi l indice dell array forcecpufan corrispondente al valore mod4
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    Serial.println(enter1);
    Serial.println(stats);
    Serial.println(mod1);
    Serial.println(mod2);
    Serial.println(mod3);
    Serial.println(mod4);
  }
  /*COMANDI PER USCIRE DAL MENU VISUALIZZAZIONE STATISTICHE CONTATORI*/  //necessario perché il blocco VISUALIZZAZIONE MENU STATISTICHE CONTATORI viene eseguito solo una volta
  else if(enter1==1){  //se enter1 è uguale a 1
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=1;  //setta la variabile a 1
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  /*COMANDI PER CONFERMARE MODIFICA PARAMETRI*/  //necessario perché il blocco VISUALIZZAZIONE MENU DI MODIFICA viene eseguito solo una volta
  else if(enter1==2){  //se enter1 è uguale a 2
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        sbidle=standbyidle[mod2];  //salva il valore corrispondente alla variabile mod2 contenuto nell array standbyidle nella variabile sbidle
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=2;  //setta la variabile a 2
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==3){  //se enter1 è uguale a 3
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        maxtemp=toptemp[mod1];  //salva il valore corrispondente alla variabile mod1 contenuto nell array toptemp nella variabile maxtemp
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=3;  //setta la variabile a 3
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==4){  //se enter1 è uguale a 4
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        cstartup=coldstartup[mod2];  //salva il valore corrispondente alla variabile mod2 contenuto nell array coldstartup nella variabile cstartup
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=4;  //setta la variabile a 4
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==5){  //se enter1 è uguale a 5
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        cidle=coldidle[mod2];  //salva il valore corrispondente alla variabile mod2 contenuto nell array coldidle nella variabile cidle
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=5;  //setta la variabile a 5
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==6){  //se enter1 è uguale a 6
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        mintemp=bottomtemp[mod1];//salva il valore corrispondente alla variabile mod1 contenuto nell array bottomtemp nella variabile mintemp
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=6;  //setta la variabile a 6
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==7){  //se enter1 è uguale a 7
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        hstartup=heatstartup[mod2];  //salva il valore corrispondente alla variabile mod2 contenuto nell array heatstartup nella variabile hstartup
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=7;  //setta la variabile a 7
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==8){  //se enter1 è uguale a 8
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        hidle=heatidle[mod2];  //salva il valore corrispondente alla variabile mod2 contenuto nell array heatidle nella variabile hidle
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=8;  //setta la variabile a 8
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==9){  //se enter1 è uguale a 9
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        maxcputemp=controltemp[mod3];  //salva il valore corrispondente alla variabile mod3 contenuto nell array controltemp nella variabile maxcputemp
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=9;  //setta la variabile a 9
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==10){  //se enter1 è uguale a 10
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        home=mainmenu[mod3];  //salva il valore corrispondente alla variabile mod3 contenuto nell array mainmenu nella variabile home
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=10;  //setta la variabile a 10
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==11){  //se enter1 è uguale a 11
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        blon=backlight[mod3];  //salva il valore corrispondente alla variabile mod3 contenuto nell array backlight nella variabile blon
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=11;  //setta la variabile a 11
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==12){  //se enter1 è uguale a 12
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        fcold=forcecold[mod4];  //salva il valore corrispondente alla variabile mod4 contenuto nell array forcecold nella variabile fcold
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=12;  //setta la variabile a 12
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==13){  //se enter1 è uguale a 13
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        fheat=forceheat[mod4];  //salva il valore corrispondente alla variabile mod3 contenuto nell array forceheat nella variabile fheat
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=13;  //setta la variabile a 13
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==14){  //se enter1 è uguale a 14
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        fff=forcechamberfan[mod4];  //salva il valore corrispondente alla variabile mod4 contenuto nell array forcechamberfan nella variabile fff
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=14;  //setta la variabile a 14
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==15){  //se enter1 è uguale a 15
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell'input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        fcf=forcecpufan[mod4];  //salva il valore corrispondente alla variabile mod4 contenuto nell array forcecpufan nella variabile fcf
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=15;  //setta la variabile a 15
        mod1=0;  //resetta la variabile
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod4=0;  //resetta la variabile
        mod5=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  /*ISTRUZIONE PER SPEGNERE ILLUMINAZIONE DISPLAY LCD*/
  if((long)(intervaltime-light)>=0){  //se tempo trascorso è maggiore del timer light
    lcd.setBacklight(LOW);
  }
  /*OPERAZIONI DA COMPIERE OGNI TOT SECONDI*/
  if(intervaltime-onesec>=clock1){
    onesec=intervaltime;  //esegui ogni 1 secondo
    voltage=0;  //resetta il valore
    resistance=0;  //resetta il valore
    Val1=ads.readADC_SingleEnded(3);  //rileva il valore analogico sonda di temperatura
    voltage=resolution1*Val1;  //calcola il voltaggio
    resistance=-((voltage*r1)/(voltage-supply));  //calcola la resistenza
    fermtemp=((resistance-r0)/mline);  //calcola la temperatura
    for(int i=0;i<(size5-1);i++){
      aft[i]=aft[i+1];  //ordina i valori spostandoli dall ultimo al primo bit
    }
    aft[size5-1]=fermtemp;  //assegna i valori rilevati all array dall ultimo bit
    Val2=analogRead(A0);  //rileva il valore analogico sensore di temperatura
    cputemp=resolution2*Val2;  //calcola la temperatura
    for(int q=0;q<(size5-1);q++){
      act[q]=act[q+1];  //ordina i valori spostandoli dall ultimo al primo bit
    }
    act[size5-1]=cputemp;  //assegna i valori rilevati all array dall ultimo bit
  }
  if(intervaltime-fifteensec>=clock15){
    fifteensec=intervaltime;  //esegui ogni 15 secondi
    if(aft[0]!=0){  //condizione di avvio per evitare errori
      fermtemptotal=0;  //resetta la somma dei valori
      for(int i=0;i<size5;i++){
        fermtemptotal=fermtemptotal+aft[i];  //fai la somma dei valori rilevati
      }
      fermtempaverage=fermtemptotal/(float)size5;  //calcola la media dei valori rilevati
      if(enter1==0&&stats==0&&printed==1){  //ogni 15 secondi e se enter1 è uguale a 0 aggiorna i dati a display
        printed=0;  //setta la variabile a 1 per visualizzare una sola volta
      }
    }
    if(act[0]!=0){  //condizione di avvio per evitare errori
      cputemptotal=0;  //resetta la somma dei valori
      for(int q=0;q<size5;q++){
        cputemptotal=cputemptotal+act[q];  //fai la somma dei valori rilevati
      }
      cputempaverage=cputemptotal/(float)size5;  //calcola la media dei valori rilevati
    }
  }
  /*ISTRUZIONI PER ACCENDERE/SPEGNERE RAFFREDDAMENTO/RISCALDAMENTO*/
  if((long)(intervaltime-target)>=0){  //se tempo trascorso è maggiore del timer target
    if(refrigeration==HIGH||heating==HIGH){  //se il raffreddamento o il riscaldamento sono accesi
      if(refrigeration==HIGH){  //se il raffreddamento è acceso
        refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
        digitalWrite(8,refrigeration);  //spegni il raffreddamento
        if(fcold==0||fcold==1){
          target=intervaltime+cidle;  //imposta il timer di standby raffreddamento
          countcoldidle++;  //incrementa la variabile
        }
        else if(fcold==2){
          target=intervaltime+fidle;
          countforcedidle++;  //incrementa la variabile
        }
        mode=3;  //setta il valore dello stato del sistema a 3
        if(fff==0||fff==1){
          ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
          digitalWrite(10,ventilation);  //spegni la ventilazione
        }
        else if(fff==2){
          ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
          digitalWrite(10,ventilation);  //accendi la ventilazione
        }
      }
      else if(heating==HIGH){  //se il riscaldamento è acceso
        heating=LOW;  //cambia lo stato del riscaldamento in spento
        digitalWrite(9,heating);  //spegni il riscaldamento
        if(fheat==0||fheat==1){
          target=intervaltime+hidle;  //imposta il timer di standby riscaldamento
          countheatidle++;  //incrementa la variabile
        }
        else if(fheat==2){
          target=intervaltime+fidle;  //imposta il timer di standby forzato
          countforcedidle++;  //incrementa la variabile
        }
        mode=4;  //setta il valore dello stato del sistema a 4
        if(fff==0||fff==1){
          ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
          digitalWrite(10,ventilation);  //spegni la ventilazione
        }
        else if(fff==2){
          ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
          digitalWrite(10,ventilation);  //accendi la ventilazione
        }
      }
    }
    else if(refrigeration==LOW&&heating==LOW){  //se il raffreddamento e il riscaldamento sono spenti
      if(fermtempaverage<maxtemp&&fermtempaverage>mintemp){  //se la temperatura media è compresa nell'intervallo impostato
        if(fcold==0||fcold==1||fheat==0||fheat==1){
          refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
          digitalWrite(8,refrigeration);  //spegni il raffreddamento
          heating=LOW;  //cambia lo stato del riscaldamento in spento
          digitalWrite(9,heating);  //spegni il riscaldamento
          target=intervaltime+sbidle;  //imposta il timer di standby
          mode=5;  //setta il valore dello stato del sistema a 5
          countstandbyidle++;  //incrementa la variabile
          if(fff==0||fff==1){
            ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
            digitalWrite(10,ventilation);  //spegni la ventilazione
          }
          else if(fff==2){
            ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
            digitalWrite(10,ventilation);  //accendi la ventilazione
          }
        }
        else if(fcold==2){
          refrigeration=HIGH;  //cambia lo stato del raffreddamento in acceso
          digitalWrite(8,refrigeration);  //accendi il raffreddamento
          heating=LOW;  //cambia lo stato del riscaldamento in spento
          digitalWrite(9,heating);  //spegni il riscaldamento
          target=intervaltime+fstartup;  //imposta il timer di accensione forzata
          mode=1;  //setta il valore dello stato del sistema a 1
          countforcedcoldstartup++;  //incrementa la variabile
          if(fff==0){
            ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
            digitalWrite(10,ventilation);  //spegni la ventilazione
          }
          else if(fff==1||fff==2){
            ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
            digitalWrite(10,ventilation);  //accendi la ventilazione
            if(fff==1){
              countforcedfermentationfanstartup++;  //incrementa la variabile
            }
          }
        }
        else if(fheat==2){
          refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
          digitalWrite(8,refrigeration);  //spegni il raffreddamento
          heating=HIGH;  //cambia lo stato del riscaldamento in acceso
          digitalWrite(9,heating);  //accendi il riscaldamento
          target=intervaltime+fstartup;  //imposta il timer di accensione forzata
          mode=2;  //setta il valore dello stato del sistema a 2
          countforcedheatstartup++;  //incrementa la variabile
          if(fff==0){
            ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
            digitalWrite(10,ventilation);  //spegni la ventilazione
          }
          else if(fff==1||fff==2){
            ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
            digitalWrite(10,ventilation);  //accendi la ventilazione
            if(fff==1){
              countforcedfermentationfanstartup++;  //incrementa la variabile
            }
          }
        }
      }
      else if(fermtempaverage>maxtemp){  //se la temperatura media è superiore alla temperatura massima impostata
        if(fcold!=0){
          if(fcold==1){
            refrigeration=HIGH;  //cambia lo stato del raffreddamento in acceso
            digitalWrite(8,refrigeration);  //accendi il raffreddamento
            heating=LOW;  //cambia lo stato del riscaldamento in spento
            digitalWrite(9,heating);  //spegni il riscaldamento
            target=intervaltime+cstartup;  //imposta il timer di accensione raffreddamento
            countcoldstartup++;  //incrementa la variabile
          }
          else if(fcold==2){
            refrigeration=HIGH;  //cambia lo stato del raffreddamento in acceso
            digitalWrite(8,refrigeration);  //accendi il raffreddamento
            heating=LOW;  //cambia lo stato del riscaldamento in spento
            digitalWrite(9,heating);  //spegni il riscaldamento
            target=intervaltime+fstartup;  //imposta il timer di accensione forzata
            countforcedcoldstartup++;  //incrementa la variabile
          }
          mode=1;  //setta il valore dello stato del sistema a 1
          if(fff==0){
            ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
            digitalWrite(10,ventilation);  //spegni la ventilazione
          }
          else if(fff==1||fff==2){
            ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
            digitalWrite(10,ventilation);  //accendi la ventilazione
            if(fff==1){
              countforcedfermentationfanstartup++;  //incrementa la variabile
            }
          }
        }
        else{
          if(fheat==0||fheat==1){
            refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
            digitalWrite(8,refrigeration);  //spegni il raffreddamento
            heating=LOW;  //cambia lo stato del riscaldamento in spento
            digitalWrite(9,heating);  //spegni il riscaldamento
            target=intervaltime+sbidle;  //imposta il timer di standby
            mode=5;  //setta il valore dello stato del sistema a 5
            countstandbyidle++;  //incrementa la variabile
            if(fff==0||fff==1){
              ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
              digitalWrite(10,ventilation);  //spegni la ventilazione
            }
            else if(fff==2){
              ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
              digitalWrite(10,ventilation);  //accendi la ventilazione
            }
          }
          else if(fheat==2){
            refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
            digitalWrite(8,refrigeration);  //spegni il raffreddamento
            heating=HIGH;  //cambia lo stato del riscaldamento in acceso
            digitalWrite(9,heating);  //accendi il riscaldamento
            target=intervaltime+fstartup;  //imposta il timer di accensione forzata
            mode=2;  //setta il valore dello stato del sistema a 2
            countforcedheatstartup++;  //incrementa la variabile
            if(fff==0){
              ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
              digitalWrite(10,ventilation);  //spegni la ventilazione
            }
            else if(fff==1||fff==2){
              ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
              digitalWrite(10,ventilation);  //accendi la ventilazione
              if(fff==1){
                countforcedfermentationfanstartup++;  //incrementa la variabile
              }
            }
          }
        }
      }
      else if(fermtempaverage<mintemp){  //se la temperatura media è inferiore alla temperatura minima impostata
        if(fheat!=0){
          if(fheat==1){
            refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
            digitalWrite(8,refrigeration);  //spegni il raffreddamento
            heating=HIGH;  //cambia lo stato del riscaldamento in acceso
            digitalWrite(9,heating);  //accendi il riscaldamento
            target=intervaltime+hstartup;  //imposta il timer di accensione riscaldamento
            countheatstartup++;  //incrementa la variabile
          }
          else if(fheat==2){
            refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
            digitalWrite(7,refrigeration);  //spegni il raffreddamento
            heating=HIGH;  //cambia lo stato del riscaldamento in acceso
            digitalWrite(8,heating);  //accendi il riscaldamento
            target=intervaltime+fstartup;  //imposta il timer di accensione forzata
            countforcedheatstartup++;  //incrementa la variabile
          }
          mode=2;  //setta il valore dello stato del sistema a 2
          if(fff==0){
            ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
            digitalWrite(10,ventilation);  //spegni la ventilazione
          }
          else if(fff==1||fff==2){
            ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
            digitalWrite(10,ventilation);  //accendi la ventilazione
            if(fff==1){
              countforcedfermentationfanstartup++;  //incrementa la variabile
            }
          }
        }
        else{
          if(fcold==0||fcold==1){
            refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
            digitalWrite(8,refrigeration);  //spegni il raffreddamento
            heating=LOW;  //cambia lo stato del riscaldamento in spento
            digitalWrite(9,heating);  //spegni il riscaldamento
            target=intervaltime+sbidle;  //imposta il timer di standby
            mode=5;  //setta il valore dello stato del sistema a 5
            countstandbyidle++;  //incrementa la variabile
            if(fff==0||fff==1){
              ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
              digitalWrite(10,ventilation);  //spegni la ventilazione
            }
            else if(fff==2){
              ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
              digitalWrite(10,ventilation);  //accendi la ventilazione
            }
          }
          else if(fcold==2){
            refrigeration=HIGH;  //cambia lo stato del raffreddamento in acceso
            digitalWrite(8,refrigeration);  //accendi il raffreddamento
            heating=LOW;  //cambia lo stato del riscaldamento in spento
            digitalWrite(9,heating);  //spegni il riscaldamento
            target=intervaltime+fstartup;  //imposta il timer di accensione forzata
            mode=1;  //setta il valore dello stato del sistema a 1
            countforcedcoldstartup++;  //incrementa la variabile
            if(fff==0){
              ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
              digitalWrite(10,ventilation);  //spegni la ventilazione
            }
            else if(fff==1||fff==2){
              ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
              digitalWrite(10,ventilation);  //accendi la ventilazione
              if(fff==1){
                countforcedfermentationfanstartup++;  //incrementa la variabile
              }
            }
          }
        }
      }
    }
  }
  /*ISTRUZIONI PER ACCENDERE/SPEGNERE LA VENTILAZIONE CPU*/
  if(fcf==0){  //se la variabile è uguale a zero
    cpufan=LOW;  //cambia lo stato della ventilazione cpu in spento
    digitalWrite(11,cpufan);  //spegni la ventilazione cpu
  }
  else if(fcf==1){  //se la variabile è uguale a uno
    if(cputempaverage<=maxcputemp){  //se la temperatura media della centralina è inferiore alla temperatura massima impostata
      cpufan=LOW;  //cambia lo stato della ventilazione cpu in spento
      digitalWrite(11,cpufan);  //spegni la ventilazione cpu
    }
    else{
      cpufan=HIGH;  //cambia lo stato della ventilazione cpu in acceso
      digitalWrite(11,cpufan);  //accendi la ventilazione cpu
      countforcedcpufanstartup++;  //incrementa la variabile
    }
  }
  else if(fcf==2){  //se la variabile è uguale a due
    cpufan=HIGH;  //cambia lo stato della ventilazione cpu in acceso
    digitalWrite(11,cpufan);  //accendi la ventilazione cpu
  }
}
