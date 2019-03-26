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
#include <ESP8266WiFi.h>
#include <Blynk.h>
#include <BlynkSimpleEsp8266.h>

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

/*PIN INPUT PULSANTI*/
int forward=14;  //pin input tasto enter1
int up=12;  //pin input tasto up
int down=13;  //pin input tasto down

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
int mod2=0;  //variabile utilizzata per scorrere i parametri
int mod3=0;  //variabile utilizzata per scorrere i parametri
int mod8=0;  //variabile utilizzata per scorrere i parametri
int mod15=0;  //variabile utilizzata per scorrere i parametri
int mod31=0;  //variabile utilizzata per scorrere i parametri
int mod60=0;  //variabile utilizzata per scorrere i parametri
int mod81=0;  //variabile utilizzata per scorrere i parametri

/*VISUALIZZAZIONI A DISPLAY*/
int printed=0;  //variabile utilizzata per scrivere una sola volta un menù o un opzione

/*LUNGHEZZA PREDEFINITA ARRAY*/
int size2=2;  //lunghezza array necessaria per settare le variabili disabilitazione al valore corrente
int size3=3;  //lunghezza array necessaria per settare le variabili disabilitazione al valore corrente
int size8=8;  //lunghezza array necessaria per settare le variabili al valore corrente
int size15=15;  //lunghezza array necessaria per calcolare la temperatura media
int size31=31;  //lunghezza array necessaria per settare le variabili al valore corrente
int size60=60;  //lunghezza array necessaria per settare le variabili al valore corrente
int size81=81;  //lunghezza array necessaria per settare le variabili al valore corrente

/*ARRAY CON PARAMETRI MODIFICABILI*/
unsigned long standbyidle[31]={240,300,360,420,480,540,600,660,720,780,840,900,960,1020,1080,1140,1200,1500,1800,2100,2400,2700,3000,3600,5400,7200,10800,14400,21600,43200,86400};  //array con valori di tempo di standby in millisecondi
float toptemp[81]={-35,-30,-25,-20,-15,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,10.5,11,11.5,12,12.5,13,13.5,14,14.5,15,15.5,16,16.5,17,17.5,18,18.5,19,19.5,20,20.5,21,21.5,22,22.5,23,23.5,24,24.5,25,25.5,26,26.5,27,27.5,28,28.5,29,29.5,30,31,32,33,34,35};  //array con valori di temperatura massima camera di fermentazione in gradi celsius
unsigned long coldstartup[60]={30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240,250,260,270,280,290,300,315,330,345,360,375,390,405,420,435,450,465,480,495,510,525,540,555,570,585,600,630,660,690,720,750,780,810,840,870,900,1800,3600};  //array con valori di tempo di accensione raffreddamento in millisecondi
unsigned long coldidle[31]={240,300,360,420,480,540,600,660,720,780,840,900,960,1020,1080,1140,1200,1500,1800,2100,2400,2700,3000,3600,5400,7200,10800,14400,21600,43200,86400};  //array con valori di tempo di standby raffreddamento in millisecondi
float bottomtemp[81]={-35,-30,-25,-20,-15,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,10.5,11,11.5,12,12.5,13,13.5,14,14.5,15,15.5,16,16.5,17,17.5,18,18.5,19,19.5,20,20.5,21,21.5,22,22.5,23,23.5,24,24.5,25,25.5,26,26.5,27,27.5,28,28.5,29,29.5,30,31,32,33,34,35};  //array con valori di temperatura minima camera di fermentazione in gradi celsius
unsigned long heatstartup[60]={30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240,250,260,270,280,290,300,315,330,345,360,375,390,405,420,435,450,465,480,495,510,525,540,555,570,585,600,630,660,690,720,750,780,810,840,870,900,1800,3600};  //array con valori di tempo di accensione riscaldamento in millisecondi
unsigned long heatidle[31]={240,300,360,420,480,540,600,660,720,780,840,900,960,1020,1080,1140,1200,1500,1800,2100,2400,2700,3000,3600,5400,7200,10800,14400,21600,43200,86400};  //array con valori di tempo di standby riscaldamento in millisecondi
int controltemp[8]={10,15,20,25,30,35,40,45};  //array con valori di temperatura centralina in gradi celsius
unsigned long clockstate[8]={15,30,60,120,240,480,960,1920};  //array con valori di tempo per modificare clock invio dati al cloud
unsigned long mainmenu[8]={20,30,45,60,90,120,240,2400};  //array con valori di tempo da attendere prima di tornare alla home del display in millisecondi
unsigned long backlight[8]={20,30,45,60,90,120,240,2400};  //array con valori di tempo di accensione illuminazione display lcd in millisecondi

/*VARIABILI PER IMPOSTARE PARAMETRI*/
unsigned long sbidle=600;  //variabile utilizzata per impostare il tempo di standby generico del raffreddamento e del riscaldamento
float maxtemp=19;  //variabile utilizzata per impostare la temperatura massima della camera di fermentazione in gradi celsius
unsigned long cstartup=70;  //variabile utilizzata per impostare il tempo di accensione del raffreddamento
unsigned long cidle=600;  //variabile utilizzata per impostare il tempo di standby del raffreddamento
float mintemp=18;  //variabile utilizzata per impostare la temperatura minima della camera di fermentazione in gradi celsius
unsigned long hstartup=60;  //variabile utilizzata per impostare il tempo di accensione del riscaldamento
unsigned long hidle=600;  //variabile utilizzata per impostare il tempo di standby del riscaldamento
int maxcputemp=30;  //variabile utilizzata per impostare la temperatura massima della centralina in gradi celsius
unsigned long clocktime=240;  //variabile utilizzata per definire il nuovo clock intervallo eventi
unsigned long home=30;  //variabile utilizzata per tornare alla home dopo un certo tempo
unsigned long blon=60;  //variabile utilizzata per impostare il tempo di accensione illuminazione display lcd

/*ARRAY ACCENSIONE FORZATA E DISABILITAZIONE*/
int coldenable[2]={0,1};  //array con valori di force start
int heatenable[2]={0,1};  //array con valori di force start
int fermentationfanenable[3]={0,1,2};  //array con valori di force start
int cpufanenable[3]={0,1,2};  //array con valori di force start

/*VARIABILI ACCENSIONE FORZATA E DISABILITAZIONE*/
int colde=1;  //variabile utilizzata per abilitare/disabilitare/forzare raffreddamento
int heate=1;  //variabile utilizzata per abilitare/disabilitare/forzare riscaldamento
int fff=1;  //variabile utilizzata per abilitare/disabilitare/forzare ventilazione camera di fermentazione
int fcf=1;  //variabile utilizzata per abilitare/disabilitare/forzare ventilazione cpu

/*STATO DEL SISTEMA*/
int mode=5;  //variabile utilizzata per sapere lo stato del sistema

/*VARIABILE TEMPO SISTEMA ED EVENTI*/
unsigned long intervaltime;  //variabile per memorizzare il tempo e calcolare gli intervalli

/*CLOCK OGNI 1 SECONDO*/
unsigned long clock1=1;  //intervallo di tempo tra rilevazioni di temperatura
unsigned long onesec=0;  //variabile utilizzata per memorizzare il momento dell ultima rilevazione di temperatura

/*CLOCK OGNI 15 SECONDI*/
unsigned long clock15=15;  //intervallo di tempo tra misurazioni temperatura media
unsigned long fifteensec=0;  //variabile utilizzata per memorizzare il momento dell ultima misurazione di temperatura media

/*CLOCK MODIFICABILE*/
unsigned long oldclock=240;  //intervallo di tempo tra eventi
unsigned long seconds;  //variabile utilizzata per memorizzare il momento dell ultimo evento

/*TIMER*/
unsigned long target=0;  //variabile utilizzata come timer accensioni/standby raffreddamento
unsigned long light=0;  //variabile utilizzata come timer accensioni illuminazione display lcd
unsigned long menu=0;  //variabile utilizzata come timer per tornare al menu principale

/*OUTPUT*/
int cooling=1;  //pin output raffreddamento
int warming=0;  //pin output riscaldamento
int fermentationventilation=2;  //pin output ventilazione camera di fermentazione
int cpuventilation=15;  //pin output ventilazione cpu

/*ACCENSIONI*/
byte refrigeration=LOW;  //stato raffreddamento spento
byte heating=LOW;  //stato riscaldamento spento
byte ventilation=LOW;  //ventilazione camera di fermentazione pwm spenta
byte cpufan=LOW;  //ventilazione centralina pwm spenta

/*CONTATORI*/
int countcycle=0;  //contatore cicli di processo
int countcoldstartup=0;  //contatore accensioni raffreddamento
int countcoldidle=0;  //contatore stanbdy raffreddamento
int countheatstartup=0;  //contatore accensioni riscaldamento
int countheatidle=0;  //contatore standby riscaldamento
int countstandbyidle=0;  //contatore standby generico
int countfermentationfanstartup=0;  //contatore accensioni ventilazione camera fermentazione
int countcpufanstartup=0;  //contatore accensioni ventilazione cpu

/*RILEVAZIONE SEGNALE ANALOGICO*/
int16_t Val1;  //valore analogico sonda di temperatura camera di fermentazione 
int Val2;  //valore analogico sensore di temperatura cpu

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

/*RISOLUZIONE IN VOLT ANALOGICO MICROPROCESSORE*/
float resolution2=0.48779296875;  //=((supply*1000)/1024)/10mV LM35DZ on ATMEGA/ESP8266

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

/*VARIABILI DI RETE*/
const char* ssid="Lovi wifi 2.4Ghz";  //variabile con nome rete wireless a cui connettersi
const char* password="CAGATONA89";  //variabile con password rete wireless a cui connettersi

/*VARIABILI HOST*/
const char* host="api.thingspeak.com";  //variabile con nome server a cui connettersi
const int port=80;  //variabile con numero porta da utilizzare per connettersi al server Thingspeak
String writekey="AO488TEWE7LI8LSR";  //variabile con codice di scrittura Thingspeak
String averagetemperature="field1";  //variabile con nome corrispondente field chart
String cputemperature="field2";  //variabile con nome corrispondente field chart
String cycles="field3";  //variabile con nome corrispondente field chart
String standbyidles="field4";  //variabile con nome corrispondente field chart
String coldstartups="field5";  //variabile con nome corrispondente field chart
String heatstartups="field6";  //variabile con nome corrispondente field chart
String fermentationfanstartups="field7";  //variabile con nome corrispondente field chart
String cpufanstartups="field8";  //variabile con nome corrispondente field chart

/*CODICE AUTORIZZAZIONE BLYNK*/
char auth[]="98543c5cd3264ba7b629df6406b4301a";  //variabili con codice di autorizzazione Blynk

void setup(){
  pinMode(14,INPUT);  //imposta il pin digitale come input
  pinMode(12,INPUT);  //imposta il pin digitale come input
  pinMode(13,INPUT);  //imposta il pin digitale come input
  pinMode(1,OUTPUT);  //imposta il pin digitale come output
  pinMode(0,OUTPUT);  //imposta il pin digitale come output
  pinMode(2,OUTPUT);  //imposta il pin digitale come output
  pinMode(15,OUTPUT);  //imposta il pin digitale come output
  Serial.begin(9600);  //inizializzazione monitor seriale
  Wire.pins(4,5);  //inizializza SDA SCL sui pin 4 e 5
  ads.begin();  //inizializza il modulo ads
  ads.setGain(GAIN_FOUR);  //setta la risoluzione dell adc a FOUR
  lcd.begin(20,4);  //inizializza il display
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);  //accendi illuminazione lcd
  lcd.setBacklight(HIGH);  //accendi illuminazione lcd
  lcd.home();  //vai alla home dell lcd
  WiFi.begin(ssid,password);  //inizializza e connetti il wifi
  Blynk.begin(auth,ssid,password);  //inizializza e connetti Blynk
  delay(2000);  //attendi 2 secondi
  lcd.setCursor(6,1);  //setta il cursore in posizione
  lcd.print("Lovibrau");  //scrivi "Lovibrau"
  lcd.setCursor(4,2);  //setta il cursore in posizione
  lcd.print("fermentation");  //scrivi "fermentation"
  delay(4000);  //attendi 4 secondi
  lcd.clear();  //pulisci lo schermo
}

void loop(){
  /*ISTRUZIONE AVVIAMENTO BLYNK*/
  Blynk.run();  //avvia Blynk
  /*ISTRUZIONE PER MEMORIZZARE IL TEMPO DEL SISTEMA*/
  DateTime now=RTC.now();  //ottieni il tempo istantaneo dall rtc
  intervaltime=now.unixtime();  //converti in unixtime il tempo istantaneo
  /*ISTRUZIONE PER USCITA A TEMPO DEI MENU*/
  if((long)(intervaltime-menu)>=0){  //se il tempo trascorso è maggiore di menu
    enter1=0;  //resetta la variabile
    stats=0;  //resetta la variabile
    mod2=0;  //resetta la variabile
    mod3=0;  //resetta la variabile
    mod8=0;  //resetta la variabile
    mod15=0;  //resetta la variabile
    mod31=0;  //resetta la variabile
    mod60=0;  //resetta la variabile
    mod81=0;  //resetta la variabile
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
        mod2++;  //incrementa la variabile
        mod3++;  //incrementa la variabile
        mod8++;  //incrementa la variabile
        mod15++;  //incrementa la variabile
        mod31++;  //incrementa la variabile
        mod60++;  //incrementa la variabile
        mod81++;  //incrementa la variabile
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
        mod2--;  //decrementa la variabile
        mod3--;  //decrementa la variabile
        mod8--;  //decrementa la variabile
        mod15--;  //decrementa la variabile
        mod31--;  //decrementa la variabile
        mod60--;  //decrementa la variabile
        mod81--;  //decrementa la variabile
      }
      lcd.setBacklight(HIGH);  //accendi illuminazione lcd
      light=intervaltime+blon;  //imposta timer illuminazione display lcd
      printed=0;  //resetta la variabile
    }
  }
  if(stats>16||stats<0){  //se stats è maggiore di 16 o minore di 0
    stats=0;  //resetta stats
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
    if(fermtempaverage==0){  //se fermetempaverage è uguale a 0
      lcd.print(fermtemp,2);  //scrivi la variabile fermtemp con due cifre dopo la virgola
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
  }
  if(enter1==0&&stats==10&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 10 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cloud send time     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(oldclock);  //scrivi la variabile oldclock
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==0&&stats==11&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 11 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("INSTANT PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Exit from menu time ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(home);  //scrivi la variabile home
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==0&&stats==12&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 12 e printed è uguale a 0
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
  }
  if(enter1==0&&stats==13&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 13 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("SERVICE STATUS      ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cooling             ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(colde);  //scrivi la variabile colde
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==0&&stats==14&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 14 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("SERVICE STATUS      ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Heating             ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(heate);  //scrivi la variabile heate
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==0&&stats==15&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 15 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("SERVICE STATUS      ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Fermentation fan    ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(fff);  //scrivi la variabile fff
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==0&&stats==16&&printed==0){  //se enter1 è uguale a 0 stats è uguale a 16 e printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("SERVICE STATUS      ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("CPU fan             ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(fcf);  //scrivi la variabile fcf
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  /*COMANDI PER ENTRARE NELLA MODALITA VISUALIZZA STATISTICHE CONTATORI*/  //necessario perché il blocco VISUALIZZAZIONE MENU viene eseguito una sola volta
  if(enter1==0&&stats==1){  //se enter1 è uguale a 0 stats è uguale a 1
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=1;  //setta la variabile a 1
        printed=0;  //resetta la variabile
      }
    }
  }
  /*COMANDI PER ENTRARE NELLA MODALITA MODIFICA*/  //necessario perché il blocco VISUALIZZAZIONE MENU viene eseguito una sola volta
  if(enter1==0&&stats==2){  //se enter1 è uguale a 0 stats è uguale a 2
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size31-1);i++){  //scorri i valori dell array standbyidle fino a quello che corrisponde alla variabili sbidle
          if(standbyidle[i]==sbidle){  //quando trovi il valore corrispondente alla variabile sbidle nell array standbyidle
            mod31=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=2;  //setta la variabile a 2
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==3){  //se enter1 è uguale a 0 stats è uguale a 3
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size81-1);i++){  //scorri i valori dell array toptemp fino a quello che corrisponde alla variabili maxtemp
          if(toptemp[i]==maxtemp){  //quando trovi il valore corrispondente alla variabile maxtemp nell array toptemp
            mod81=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        enter1=3;  //setta la variabile a 3
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==4){  //se enter1 è uguale a 0 stats è uguale a 4
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size60-1);i++){  //scorri i valori dell array coldstartup fino a quello che corrisponde alla variabili cstartup
          if(coldstartup[i]==cstartup){  //quando trovi il valore corrispondente alla variabile cstartup nell array coldstartup
            mod60=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=4;  //setta la variabile a 4
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==5){  //se enter1 è uguale a 0 stats è uguale a 5
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size31-1);i++){  //scorri i valori dell array coldidle fino a quello che corrisponde alla variabili cidle
          if(coldidle[i]==cidle){  //quando trovi il valore corrispondente alla variabile cidle nell array coldidle
            mod31=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=5;  //setta la variabile a 5
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==6){  //se enter1 è uguale a 0 stats è uguale a 6
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se linput del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size81-1);i++){  //scorri i valori dell array bottomtemp fino a quello che corrisponde alla variabili mintemp
          if(bottomtemp[i]==mintemp){  //quando trovi il valore corrispondente alla variabile mintemp nell array bottomtemp
            mod81=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        enter1=6;  //setta la variabile a 6
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==7){  //se enter1 è uguale a 0 stats è uguale a 7
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(mod60-1);i++){  //scorri i valori dell array heatstartup fino a quello che corrisponde alla variabili hstartup
          if(heatstartup[i]==hstartup){  //quando trovi il valore corrispondente alla variabile hstartup nell array heatstartup
            mod60=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=7;  //setta la variabile a 7
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==8){  //se enter1 è uguale a 0 stats è uguale a 8
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size31-1);i++){  //scorri i valori dell array heatidle fino a quello che corrisponde alla variabili hidle
          if(heatidle[i]==hidle){  //quando trovi il valore corrispondente alla variabile hidle nell array heatidle
            mod31=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=8;  //setta la variabile a 8
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==9){  //se enter1 è uguale a 0 stats è uguale a 9
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size8-1);i++){  //scorri i valori dell array controltemp fino a quello che corrisponde alla variabili maxcputemp
          if(controltemp[i]==maxcputemp){  //quando trovi il valore corrispondente alla variabile maxcputemp nell array controltemp
            mod8=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=9;  //setta la variabile a 9
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==10){  //se enter1 è uguale a 0 stats è uguale a 10
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size8-1);i++){  //scorri i valori dell array clockstate fino a quello che corrisponde alla variabili clocktime
          if(clockstate[i]==clocktime){  //quando trovi il valore corrispondente alla variabile clocktime nell array clockstate
            mod8=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=10;  //setta la variabile a 10
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==11){  //se enter1 è uguale a 0 stats è uguale a 11
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size8-1);i++){  //scorri i valori dell array mainmenu fino a quello che corrisponde alla variabili home
          if(mainmenu[i]==home){  //quando trovi il valore corrispondente alla variabile home nell array mainmenu
            mod8=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=11;  //setta la variabile a 11
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==12){  //se enter1 è uguale a 0 stats è uguale a 12
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size8-1);i++){  //scorri i valori dell array backlight fino a quello che corrisponde alla variabili blon
          if(backlight[i]==blon){  //quando trovi il valore corrispondente alla variabile blon nell array backlight
            mod8=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=12;  //setta la variabile a 12
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==13){  //se enter1 è uguale a 0 stats è uguale a 13
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size2-1);i++){  //scorri i valori dell array coldenable fino a quello che corrisponde alla variabili colde
          if(coldenable[i]==colde){  //quando trovi il valore corrispondente alla variabile colde nell array coldenable
            mod2=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=13;  //setta la variabile a 13
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==14){  //se enter1 è uguale a 0 stats è uguale a 14
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size2-1);i++){  //scorri i valori dell array heatenable fino a quello che corrisponde alla variabili heate
          if(heatenable[i]==heate){  //quando trovi il valore corrispondente alla variabile heate nell array heatenable
            mod2=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=14;  //setta la variabile a 14
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==15){  //se enter1 è uguale a 0 stats è uguale a 15
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size3-1);i++){  //scorri i valori dell array fermentationfanenable fino a quello che corrisponde alla variabili fff
          if(fermentationfanenable[i]==fff){  //quando trovi il valore corrispondente alla variabile fff nell array fermentationfanenable
            mod3=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=15;  //setta la variabile a 15
        printed=0;  //resetta la variabile
      }
    }
  }
  if(enter1==0&&stats==16){  //se enter1 è uguale a 0 stats è uguale a 16
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è HIGH
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        for(int i=0;i<(size3-1);i++){  //scorri i valori dell array cpufanenable fino a quello che corrisponde alla variabili fcf
          if(cpufanenable[i]==fcf){  //quando trovi il valore corrispondente alla variabile fcf nell array cpufanenable
            mod3=i;  //setta la variabile al valore corrispondente
            break;  //interrompi il ciclo for
          }
        }
        mod2=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=16;  //setta la variabile a 16
        printed=0;  //resetta la variabile
      }
    }
  }
  /*VISUALIZZAZIONE MENU STATISTICHE CONTATORI*/
  if(enter1==1&&stats==1&&printed==0){  //se enter1 è uguale a 1 stats è uguale a 1 printed è uguale a 0
    if(mod8==0){  //se mod9 è uguale a 0
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
        mod8=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Cycles              ");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countcycle);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod8==1){  //se mod9 è uguale a 1
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
        mod8=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Standby idles       ");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(sbidle);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod8==2){  //se mod9 è uguale a 2
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
        mod8=0;  //resetta la variabile
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
    else if(mod8==3){  //se mod9 è uguale a 3
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
        mod8=0;  //resetta la variabile
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
    else if(mod8==4){  //se mod9 è uguale a 4
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
        mod8=0;  //resetta la variabile
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
    else if(mod8==5){  //se mod9 è uguale a 5
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
        mod8=0;  //resetta la variabile
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
    else if(mod8==6){  //se mod9 è uguale a 6
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
        mod8=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("Ferm fan startups   ");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countfermentationfanstartup);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
    else if(mod8==7){  //se mod9 è uguale a 7
      menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
      if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
        mod8=0;  //resetta la variabile
      }
      lcd.clear();  //pulisci lo schermo
      lcd.setCursor(0,0);  //posiziona il cursore a
      lcd.print("WORKING STATS       ");  //scrivi
      lcd.setCursor(0,1);  //posiziona il cursore a
      lcd.print("enter to exit       ");  //scrivi
      lcd.setCursor(0,2);  //posiziona il cursore a
      lcd.print("CPU fan startups    ");  //scrivi
      lcd.setCursor(0,3);  //posiziona il cursore a
      lcd.print(countcpufanstartup);  //scrivi la variabile
      printed=1;  //setta la variabile a 1 per visualizzare una sola volta
    }
  }
  /*VISUALIZZAZIONE MENU DI MODIFICA*/
  if(enter1==2&&stats==2&&printed==0){  //se enter1 è uguale a 2 stats è uguale a 2 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod31>30||mod31<0){  //se la variabile mod31 è maggiore di 30 o minore di 0
      mod31=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Standby idle time   ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(standbyidle[mod31]);  //scrivi l indice dell array standbyidle corrispondente al valore mod31
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==3&&stats==3&&printed==0){  //se enter1 è uguale a 3 stats è uguale a 3 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod81>80||mod81<0){  //se la variabile mod81 è maggiore di 80 o minore di 0
      mod81=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Max temperature     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(toptemp[mod81],2);  //scrivi l indice dell array toptemp corrispondente al valore mod81 con due cifre dopo la virgola
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==4&&stats==4&&printed==0){  //se enter1 è uguale a 4 stats è uguale a 4 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod60>59||mod60<0){  //se la variabile mod60 è maggiore di 59 o minore di 0
      mod60=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cold startup time   ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(coldstartup[mod60]);  //scrivi l indice dell array coldstartup corrispondente al valore mod60
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==5&&stats==5&&printed==0){  //se enter1 è uguale a 5 stats è uguale a 5 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod31>30||mod31<0){  //se la variabile mod31 è maggiore di 30 o minore di 0
      mod31=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cold idle time      ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(coldidle[mod31]);  //scrivi l indice dell array coldidle corrispondente al valore mod31
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==6&&stats==6&&printed==0){  //se enter1 è uguale a 6 stats è uguale a 6 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod81>80||mod81<0){  //se la variabile mod81 è maggiore di 80 o minore di 0
      mod81=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Min temperature     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(bottomtemp[mod81],2);  //scrivi l indice dell array bottomtemp corrispondente al valore mod81 con due cifre dopo la virgola
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==7&&stats==7&&printed==0){  //se enter1 è uguale a 7 stats è uguale a 7 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod60>59||mod60<0){  //se la variabile mod60 è maggiore di 59 o minore di 0
      mod60=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Heat startup time   ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(heatstartup[mod60]);  //scrivi l indice dell array heatstartup corrispondente al valore mod60
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==8&&stats==8&&printed==0){  //se enter1 è uguale a 8 stats è uguale a 8 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod31>30||mod31<0){  //se la variabile mod31 è maggiore di 30 o minore di 0
      mod31=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Heat idle time      ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(heatidle[mod31]);  //scrivi l indice dell array heatidle corrispondente al valore mod31
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==9&&stats==9&&printed==0){  //se enter1 è uguale a 9 stats è uguale a 9 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
      mod8=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY PARAMETERS   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Control temperature ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(controltemp[mod8]);  //scrivi l indice dell array controltemp corrispondente al valore mod8
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==10&&stats==10&&printed==0){  //se enter1 è uguale a 10 stats è uguale a 10 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
      mod8=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY  PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cloud send time     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(clockstate[mod8]);  //scrivi l indice dell array clockstate corrispondente al valore mod8
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==11&&stats==11&&printed==0){  //se enter1 è uguale a 11 stats è uguale a 11 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
      mod8=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY  PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Exit from menu time ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(mainmenu[mod8]);  //scrivi l indice dell array mainmenu corrispondente al valore mod8
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==12&&stats==12&&printed==0){  //se enter1 è uguale a 12 stats è uguale a 12 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod8>7||mod8<0){  //se la variabile mod8 è maggiore di 7 o minore di 0
      mod8=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("MODIFY  PARAMETERS  ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("enter to modify     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("LCD backlight time  ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(backlight[mod8]);  //scrivi l indice dell array backlight corrispondente al valore mod8
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==13&&stats==13&&printed==0){  //se enter1 è uguale a 13 stats è uguale a 13 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod2>1||mod2<0){  //se la variabile mod2 è maggiore di 1 o minore di 0
      mod2=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("STARTUPS ENABLE     ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("0:disable 1:off     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Cold startup        ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(coldenable[mod2]);  //scrivi l indice dell array coldenable corrispondente al valore mod2
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==14&&stats==14&&printed==0){  //se enter1 è uguale a 14 stats è uguale a 14 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod2>1||mod2<0){  //se la variabile mod2 è maggiore di 1 o minore di 0
      mod2=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("STARTUPS ENABLE     ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("0:disable 1:off     ");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Heat startup        ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(heatenable[mod2]);  //scrivi l indice dell array heatenable corrispondente al valore mod2
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==15&&stats==15&&printed==0){  //se enter1 è uguale a 15 stats è uguale a 15 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod3>2||mod3<0){  //se la variabile mod3 è maggiore di 2 o minore di 0
      mod3=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("STARTUPS FORCING   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("0:disable 1:off 2:on");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("Chamber fan startup ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(fermentationfanenable[mod3]);  //scrivi l indice dell array fermentationfanenable corrispondente al valore mod3
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  if(enter1==16&&stats==16&&printed==0){  //se enter1 è uguale a 16 stats è uguale a 16 printed è uguale a 0
    menu=intervaltime+home;  //imposta il timer di ritorno al menu principale
    if(mod3>2|mod3<0){  //se la variabile mod3 è maggiore di 2 o minore di 0
      mod3=0;  //resetta la variabile
    }
    lcd.clear();  //pulisci lo schermo
    lcd.setCursor(0,0);  //posiziona il cursore a
    lcd.print("STARTUPS FORCING   ");  //scrivi
    lcd.setCursor(0,1);  //posiziona il cursore a
    lcd.print("0:disable 1:off 2:on");  //scrivi
    lcd.setCursor(0,2);  //posiziona il cursore a
    lcd.print("CPU fan startup     ");  //scrivi
    lcd.setCursor(0,3);  //posiziona il cursore a
    lcd.print(cpufanenable[mod3]);  //scrivi l indice dell array cpufanenable corrispondente al valore mod3
    printed=1;  //setta la variabile a 1 per visualizzare una sola volta
  }
  /*COMANDI PER USCIRE DAL MENU VISUALIZZAZIONE STATISTICHE CONTATORI*/  //necessario perché il blocco VISUALIZZAZIONE MENU STATISTICHE CONTATORI viene eseguito solo una volta
  else if(enter1==1){  //se enter1 è uguale a 1
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=1;  //setta la variabile a 1
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  /*COMANDI PER CONFERMARE MODIFICA PARAMETRI*/  //necessario perché il blocco VISUALIZZAZIONE MENU DI MODIFICA viene eseguito solo una volta
  else if(enter1==2){  //se enter1 è uguale a 2
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        sbidle=standbyidle[mod31];  //salva il valore corrispondente alla variabile mod31 contenuto nell array standbyidle nella variabile sbidle
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=2;  //setta la variabile a 2
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==3){  //se enter1 è uguale a 3
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        maxtemp=toptemp[mod81];  //salva il valore corrispondente alla variabile mod81 contenuto nell array toptemp nella variabile maxtemp
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=3;  //setta la variabile a 3
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==4){  //se enter1 è uguale a 4
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        cstartup=coldstartup[mod60];  //salva il valore corrispondente alla variabile mod60 contenuto nell array coldstartup nella variabile cstartup
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=4;  //setta la variabile a 4
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==5){  //se enter1 è uguale a 5
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        cidle=coldidle[mod31];  //salva il valore corrispondente alla variabile mod31 contenuto nell array coldidle nella variabile cidle
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=5;  //setta la variabile a 5
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==6){  //se enter1 è uguale a 6
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        mintemp=bottomtemp[mod81];//salva il valore corrispondente alla variabile mod81 contenuto nell array bottomtemp nella variabile mintemp
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=6;  //setta la variabile a 6
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==7){  //se enter1 è uguale a 7
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        hstartup=heatstartup[mod60];  //salva il valore corrispondente alla variabile mod60 contenuto nell array heatstartup nella variabile hstartup
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=7;  //setta la variabile a 7
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==8){  //se enter1 è uguale a 8
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        hidle=heatidle[mod31];  //salva il valore corrispondente alla variabile mod31 contenuto nell array heatidle nella variabile hidle
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=8;  //setta la variabile a 8
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==9){  //se enter1 è uguale a 9
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        maxcputemp=controltemp[mod8];  //salva il valore corrispondente alla variabile mod8 contenuto nell array controltemp nella variabile maxcputemp
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=9;  //setta la variabile a 9
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==10){  //se enter1 è uguale a 10
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        clocktime=clockstate[mod8];  //salva il valore corrispondente alla variabile mod8 contenuto nell array clockstate nella variabile clocktime
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=10;  //setta la variabile a 10
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==11){  //se enter1 è uguale a 11
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        home=mainmenu[mod8];  //salva il valore corrispondente alla variabile mod8 contenuto nell array mainmenu nella variabile home
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=11;  //setta la variabile a 11
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==12){  //se enter1 è uguale a 12
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        blon=backlight[mod8];  //salva il valore corrispondente alla variabile mod8 contenuto nell array backlight nella variabile blon
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=12;  //setta la variabile a 12
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==13){  //se enter1 è uguale a 13
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        colde=coldenable[mod2];  //salva il valore corrispondente alla variabile mod2 contenuto nell array coldenable nella variabile colde
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=13;  //setta la variabile a 13
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==14){  //se enter1 è uguale a 14
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        heate=heatenable[mod2];  //salva il valore corrispondente alla variabile mod2 contenuto nell array heatenable nella variabile heate
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=14;  //setta la variabile a 14
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==15){  //se enter1 è uguale a 15
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        fff=fermentationfanenable[mod3];  //salva il valore corrispondente alla variabile mod3 contenuto nell array fermentationfanenable nella variabile fff
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=15;  //setta la variabile a 15
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  else if(enter1==16){  //se enter1 è uguale a 16
    val_forward=digitalRead(forward);  //leggi lo stato del pulsante forward
    if(val_forward!=forwardstate){  //se l input del pulsante è diverso dallo stato del pulsante
      forwardstate=val_forward;  //salva il valore dell input nella variabile di stato pulsante
      if(forwardstate==HIGH){  //se lo stato del pulsante è high
        fcf=cpufanenable[mod3];  //salva il valore corrispondente alla variabile mod3 contenuto nell array cpufanenable nella variabile fcf
        lcd.setBacklight(HIGH);  //accendi illuminazione lcd
        light=intervaltime+blon;  //imposta il timer di accensione display
        stats=16;  //setta la variabile a 16
        mod2=0;  //resetta la variabile
        mod3=0;  //resetta la variabile
        mod8=0;  //resetta la variabile
        mod15=0;  //resetta la variabile
        mod31=0;  //resetta la variabile
        mod60=0;  //resetta la variabile
        mod81=0;  //resetta la variabile
        enter1=0;  //resetta la variabile
        printed=0;  //resetta la variabile
      }
    }
  }
  /*ISTRUZIONE PER SPEGNERE ILLUMINAZIONE DISPLAY LCD*/
  if((long)(intervaltime-light)>=0){  //se tempo trascorso è maggiore del timer light
    lcd.setBacklight(LOW);  //spegni l lcd
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
    for(int i=0;i<(size15-1);i++){
      aft[i]=aft[i+1];  //ordina i valori spostandoli dall ultimo al primo bit
    }
    aft[size15-1]=fermtemp;  //assegna i valori rilevati all array dall ultimo bit
    // Val2=analogRead(A0);  //rileva il valore analogico sensore di temperatura Arduino
    Val2=analogRead(A0);  //rileva il valore analogico sensore di temperatura ESP8266
    cputemp=resolution2*Val2;  //calcola la temperatura
    for(int q=0;q<(size15-1);q++){
      act[q]=act[q+1];  //ordina i valori spostandoli dall ultimo al primo bit
    }
    act[size15-1]=cputemp;  //assegna i valori rilevati all array dall ultimo bit
  }
  if(intervaltime-fifteensec>=clock15){
    fifteensec=intervaltime;  //esegui ogni 15 secondi
    if(aft[0]!=0){  //condizione di avvio per evitare errori
      fermtemptotal=0;  //resetta la somma dei valori
      for(int i=0;i<size15;i++){
        fermtemptotal=fermtemptotal+aft[i];  //fai la somma dei valori rilevati
      }
      fermtempaverage=fermtemptotal/(float)size15;  //calcola la media dei valori rilevati
    }
    if(act[0]!=0){  //condizione di avvio per evitare errori
      cputemptotal=0;  //resetta la somma dei valori
      for(int q=0;q<size15;q++){
        cputemptotal=cputemptotal+act[q];  //fai la somma dei valori rilevati
      }
      cputempaverage=cputemptotal/(float)size15;  //calcola la media dei valori rilevati
    }
    if(enter1==0&&stats==0&&printed==1){  //ogni 15 secondi e se enter1 è uguale a 0 aggiorna i dati a display
      printed=0;  //setta la variabile a 1 per visualizzare una sola volta
      Blynk.virtualWrite(V0,fermtempaverage);  //scrivi su Blynk la variabile fermtempaverage
      Blynk.virtualWrite(V1,cputempaverage);  //scrivi su Blynk la variabile cputempaverage
    }
  }
  /*ISTRUZIONI PER ACCENDERE/SPEGNERE RAFFREDDAMENTO/RISCALDAMENTO*/
  if((long)(intervaltime-target)>=0){  //se tempo trascorso è maggiore del timer target
    if(refrigeration==HIGH||heating==HIGH){  //se il raffreddamento o il riscaldamento sono accesi
      if(refrigeration==HIGH){  //se il raffreddamento è acceso
        refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
        digitalWrite(cooling,refrigeration);  //spegni il raffreddamento
        target=intervaltime+cidle;  //imposta il timer di standby raffreddamento
        countcoldidle++;  //incrementa la variabile
        mode=3;  //setta il valore dello stato del sistema a 3
        if(fff==0||fff==1){  //se la variabile fff è uguale a 0 o 1
          ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
          digitalWrite(fermentationventilation,ventilation);  //spegni la ventilazione
        }
        else if(fff==2){  //se la variabile fff è uguale a 2
          ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
          digitalWrite(fermentationventilation,ventilation);  //accendi la ventilazione
        }
      }
      else if(heating==HIGH){  //se il riscaldamento è acceso
        heating=LOW;  //cambia lo stato del riscaldamento in spento
        digitalWrite(warming,heating);  //spegni il riscaldamento
        target=intervaltime+hidle;  //imposta il timer di standby riscaldamento
        countheatidle++;  //incrementa la variabile
        mode=4;  //setta il valore dello stato del sistema a 4
        if(fff==0||fff==1){  //se la variabile fff è uguale a 0 o 1
          ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
          digitalWrite(fermentationventilation,ventilation);  //spegni la ventilazione
        }
        else if(fff==2){  //se la variabile fff è uguale a 2
          ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
          digitalWrite(fermentationventilation,ventilation);  //accendi la ventilazione
        }
      }
    }
    else if(refrigeration==LOW&&heating==LOW){  //se il raffreddamento e il riscaldamento sono spenti
      if(fermtempaverage<maxtemp&&fermtempaverage>mintemp){  //se la temperatura media è compresa nell intervallo impostato
        refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
        digitalWrite(cooling,refrigeration);  //spegni il raffreddamento
        heating=LOW;  //cambia lo stato del riscaldamento in spento
        digitalWrite(warming,heating);  //spegni il riscaldamento
        target=intervaltime+sbidle;  //imposta il timer di standby
        mode=5;  //setta il valore dello stato del sistema a 5
        countstandbyidle++;  //incrementa la variabile
        if(fff==0||fff==1){  //se la variabile fff è uguale a 0 o 1
          ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
          digitalWrite(fermentationventilation,ventilation);  //spegni la ventilazione
        }
        else if(fff==2){  //se la variabile fff è uguale a 2
          ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
          digitalWrite(fermentationventilation,ventilation);  //accendi la ventilazione
        }
      }
      else if(fermtempaverage>maxtemp){  //se la temperatura media è superiore alla temperatura massima impostata
        if(colde!=0){  //se la variabile colde è diversa da 0
          refrigeration=HIGH;  //cambia lo stato del raffreddamento in acceso
          digitalWrite(cooling,refrigeration);  //accendi il raffreddamento
          heating=LOW;  //cambia lo stato del riscaldamento in spento
          digitalWrite(warming,heating);  //spegni il riscaldamento
          target=intervaltime+cstartup;  //imposta il timer di accensione raffreddamento
          countcoldstartup++;  //incrementa la variabile
          mode=1;  //setta il valore dello stato del sistema a 1
          if(fff==0){  //se la variabile fff è uguale a 0
            ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
            digitalWrite(fermentationventilation,ventilation);  //spegni la ventilazione
          }
          else if(fff==1||fff==2){  //se la variabile fff è uguale a 1 o 2
            ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
            digitalWrite(fermentationventilation,ventilation);  //accendi la ventilazione
            if(fff==1){  //se la variabile fff è uguale a 1
              countfermentationfanstartup++;  //incrementa la variabile
            }
          }
        }
        else{
          refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
          digitalWrite(cooling,refrigeration);  //spegni il raffreddamento
          heating=LOW;  //cambia lo stato del riscaldamento in spento
          digitalWrite(warming,heating);  //spegni il riscaldamento
          target=intervaltime+sbidle;  //imposta il timer di standby
          mode=5;  //setta il valore dello stato del sistema a 5
          countstandbyidle++;  //incrementa la variabile
          if(fff==0||fff==1){  //se la variabile fff è uguale a 0 o 1
            ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
            digitalWrite(fermentationventilation,ventilation);  //spegni la ventilazione
          }
          else if(fff==2){  //se la variabile fff è uguale a 2
            ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
            digitalWrite(fermentationventilation,ventilation);  //accendi la ventilazione
          }
        }
      }
      else if(fermtempaverage<mintemp){  //se la temperatura media è inferiore alla temperatura minima impostata
        if(heate!=0){  //se la variabile colde è diversa da 0
          refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
          digitalWrite(cooling,refrigeration);  //spegni il raffreddamento
          heating=HIGH;  //cambia lo stato del riscaldamento in acceso
          digitalWrite(warming,heating);  //accendi il riscaldamento
          target=intervaltime+hstartup;  //imposta il timer di accensione riscaldamento
          countheatstartup++;  //incrementa la variabile
          mode=2;  //setta il valore dello stato del sistema a 2
          if(fff==0){  //se la variabile fff è uguale a 0
            ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
            digitalWrite(fermentationventilation,ventilation);  //spegni la ventilazione
          }
          else if(fff==1||fff==2){  //se la variabile fff è uguale a 1 o 2
            ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
            digitalWrite(fermentationventilation,ventilation);  //accendi la ventilazione
            if(fff==1){  //se la variabile fff è uguale a 1
              countfermentationfanstartup++;  //incrementa la variabile
            }
          }
        }
        else{
          refrigeration=LOW;  //cambia lo stato del raffreddamento in spento
          digitalWrite(cooling,refrigeration);  //spegni il raffreddamento
          heating=LOW;  //cambia lo stato del riscaldamento in spento
          digitalWrite(warming,heating);  //spegni il riscaldamento
          target=intervaltime+sbidle;  //imposta il timer di standby
          mode=5;  //setta il valore dello stato del sistema a 5
          countstandbyidle++;  //incrementa la variabile
          if(fff==0||fff==1){  //se la variabile fff è uguale a 0 o 1
            ventilation=LOW;  //cambia lo stato della ventilazione camera di fermentazione in spento
            digitalWrite(fermentationventilation,ventilation);  //spegni la ventilazione
          }
          else if(fff==2){  //se la variabile fff è uguale a 2
            ventilation=HIGH;  //cambia lo stato della ventilazione camera di fermentazione in acceso
            digitalWrite(fermentationventilation,ventilation);  //accendi la ventilazione
          }
        }
      }
    }
    countcycle++;  //incrementa la variabile
  }
  /*ISTRUZIONI PER ACCENDERE/SPEGNERE LA VENTILAZIONE CPU*/
  if(fcf==0){  //se la variabile è uguale a zero
    cpufan=LOW;  //cambia lo stato della ventilazione cpu in spento
    digitalWrite(cpuventilation,cpufan);  //spegni la ventilazione cpu
  }
  else if(fcf==1){  //se la variabile è uguale a uno
    if(cputempaverage<=maxcputemp){  //se la temperatura media della centralina è inferiore alla temperatura massima impostata
      cpufan=LOW;  //cambia lo stato della ventilazione cpu in spento
      digitalWrite(cpuventilation,cpufan);  //spegni la ventilazione cpu
    }
    else{
      cpufan=HIGH;  //cambia lo stato della ventilazione cpu in acceso
      digitalWrite(cpuventilation,cpufan);  //accendi la ventilazione cpu
      countcpufanstartup++;  //incrementa la variabile
    }
  }
  else if(fcf==2){  //se la variabile è uguale a due
    cpufan=HIGH;  //cambia lo stato della ventilazione cpu in acceso
    digitalWrite(cpuventilation,cpufan);  //accendi la ventilazione cpu
  }
  /*ISTRUZIONI INVIO DATI AL CLOUD*/
  if(intervaltime-seconds>=oldclock){  //quando il tempo di attesa è terminato
    if(oldclock!=clocktime){  //se il clock precedente è diverso dal nuovo valore di clock
      oldclock=clocktime;  //salva il nuovo valore di clock nella variabile clocktime
    }
    seconds=intervaltime;  //salva il valore di millis nella variabile seconds
    Serial.println("");  //scrivi
    Serial.println("");  //scrivi
    Serial.print("Connecting to ");  //scrivi
    Serial.print(ssid);  //scrivi
    if(WiFi.begin(ssid,password)==0){  //se il wifi non è connesso
      Serial.println("");  //scrivi
      Serial.print("Reconnecting to ");  //scrivi
      Serial.print(ssid);  //scrivi
      for(int i=0;i<4;i++){  //prova 5 volte
        WiFi.begin(ssid,password);  //connetti al wifi
        Serial.print(".");  //scrivi
        if(WiFi.begin(ssid,password)!=0){  //se il wifi si riconnette
          break;  //interrompi il ciclo for
        }
      }
      Serial.println("");  //scrivi
      Serial.println("Error connecting to wifi");  //scrivi
      return;  //esci dall if
    }
    else{  //se il wifi è connesso
      Serial.println("");  //scrivi
      Serial.println("Wifi connected!");  //scrivi
      if(Blynk.begin(auth,ssid,password)==0){
        Serial.println("");  //scrivi
        Serial.print("Reconnecting to Blynk");  //scrivi
        for(int i=0;i<4;i++){  //prova 5 volte
          Blynk.begin(auth,ssid,password);  //connetti a Blynk
          Serial.print(".");  //scrivi
          if(Blynk.begin(auth,ssid,password)!=0){  //se Blynk si riconnette
            break;  //interrompi il ciclo for
          }
        }
        Serial.println("");  //scrivi
        Serial.println("Blynk disconnected");  //scrivi
        return;  //esci dall if
      }
      else{  //se Blynk è connesso
        Serial.println("");  //scrivi
        Serial.println("Blynk online!");  //scrivi
      }
      WiFiClient client;  //dai un nome all esp8266 per connettersi al server
      Serial.print("Connecting to ");  //scrivi
      Serial.print(host);  //scrivi
      client.connect(host,port);  //connettiti al server
      if(client.connected()==0){  //se il client non è connesso al server
        for(int i=0;i<4;i++){  //prova 5 volte
          client.connect(host,port);  //connetti al server
          Serial.print(".");  //scrivi
          if(client.connected()!=0){  //se il client si connette al server
            break;  //interrompi il ciclo for
          }
        }
        Serial.println("");  //scrivi
        Serial.println("Connection failed");  //scrivi
        return;  //esci dall if
      }
      else{  //se il client si è connesso al server
        Serial.println("");  //scrivi
        Serial.println("Connected to server!");  //scrivi
        String url="GET /update?api_key="+writekey+"&"+averagetemperature+"="+String(fermtempaverage)+"&"+cputemperature+"="+String(cputempaverage)+"&"+cycles+"="+String(countcycle)+"&"+standbyidles+"="+String(countstandbyidle)+"&"+coldstartups+"="+String(countcoldstartup)+"&"+heatstartups+"="+String(countheatstartup)+"&"+fermentationfanstartups+"="+String(countfermentationfanstartup)+"&"+cpufanstartups+"="+String(countcpufanstartup);  //crea una richiesta url con i seguenti campi
        String values;  //dichiara una stringa di valori
        values+="Fermentation temperature"+String(fermtempaverage)+"&";  //stringa corrispondente al field1 di valore fermtempaverage
        values+="CPU temperature"+String(cputempaverage)+"&";  //stringa corrispondente al field2 di valore cputempaverage
        values+="Cycles"+String(countcycle)+"&";  //stringa corrispondente al field3 di valore countcycle
        values+="Standby idles"+String(countstandbyidle)+"&";  //stringa corrispondente al field4 di valore countstandbyidle
        values+="Cold startups"+String(countcoldstartup)+"&";  //stringa corrispondente al field5 di valore countcoldstartup
        values+="Heat startups"+String(countheatstartup)+"&";  //stringa corrispondente al field6 di valore countheatstartup
        values+="Fermentation fan startups"+String(countfermentationfanstartup)+"&";  //stringa corrispondente al field7 di valore countfermentationfanstartup
        values+="CPU fan startups"+String(countcpufanstartup)+"&";  //stringa corrispondente al field8 di valore countcpufanstartup
        client.println(url);  //scrivi sul server la richiesta
        client.print("Content-Length: ");  //scrivi sul server
        client.println(values.length());  //scrivi sul server la lunghezza dei valori
        client.println();  //scrivi sul server
        client.print(values);  //scrivi sul server i valori
      }
      client.stop();  //disconnettiti dal server
      Serial.println("Disconnected from server");  //scrivi
    }
  }
}
