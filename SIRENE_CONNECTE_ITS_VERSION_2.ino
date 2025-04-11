#include <TFT_eSPI.h> 
#include <SPI.h>
#include "RTClib.h"

#define choice_button 12
#define button_increment 13
#define button_desincrement 14
#define relais_sirene 27


TFT_eSPI tft = TFT_eSPI();  
RTC_DS3231 rtc;
DateTime now;

char _buffer[11];
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
byte x_pos[7] = {80, 70, 57, 40, 49, 70, 49};

static byte previous_dow = 8;
static int prevHour = -1, prevMinute = -1, prevSecond = -1;
static String previousDate = "";
static float prevTemp = 0.0;
int heure = 0, minute = 0, sec = 0;

void setup() 
{
  setup_TFT();
  pinMode(choice_button, INPUT_PULLUP);
  pinMode(button_increment, INPUT_PULLUP); 
  pinMode(button_desincrement, INPUT_PULLUP); 
  pinMode(relais_sirene, OUTPUT);
  if (!rtc.begin()) {
    Serial.println("Impossible de trouver RTC");
    Serial.flush();
    while (1) delay(10);
  }
}

void setup_TFT()
{
  tft.init();
  tft.fillScreen(TFT_WHITE);
  int screenHeight = 240; 
  int thirdHeight = screenHeight / 3; 
  int lineThickness = 2; 

  tft.drawFastHLine(0, thirdHeight-lineThickness/2, tft.width(), TFT_BLUE);
  tft.drawFastHLine(0, thirdHeight*2-lineThickness/2, tft.width(), TFT_BLUE);
  tft.drawFastHLine(0, screenHeight-lineThickness/2, tft.width(), TFT_BLUE);

  tft.setTextColor(TFT_BLUE);     
  tft.setTextSize(3);                
  tft.setCursor(3, 10);               
  tft.print("ITS Automatic");
  tft.setCursor(80, 40);               
  tft.print("Siren");
  tft.setCursor(85, 165);      
  tft.setTextSize(3);         
  tft.print("Time:");
  tft.setCursor(20, 245);      
  tft.setTextSize(3); 
  tft.setTextColor(TFT_BLUE);        
  tft.print("Temperature:");
}

void loop() 
{
  now = rtc.now();
  heure = now.hour();
  minute = now.minute();
  sec = now.second();

  reglageManuelle();
  handleClassSchedule(heure, minute, sec);

  // Fonctions d'affichage
  displayDate(now);
  displayTime(now);
  displayDayOfWeek(now);
  displayTemperature();
  
}

void handleClassSchedule(int heure, int minute, int sec) {
  if (heure == 7 && minute == 20 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("DEBUT DE COURS");
    resetPreviousValues();
  } else if (heure == 7 && minute == 25 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("PREMIERE HEURE");
    resetPreviousValues();
  } else if (heure == 8 && minute == 25 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("DEUXIEME HEURE");
    resetPreviousValues();
  } else if (heure == 9 && minute == 10 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("TROISIEME HEURE");
    resetPreviousValues();
  } else if (heure == 10 && minute == 0 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("QUATRIEME HEURE");
    resetPreviousValues();
  } else if (heure == 10 && minute == 50 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("PREMIERE PAUSE");
    resetPreviousValues();
  } else if (heure == 11 && minute == 10 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("CINQUIEME HEURE");
    resetPreviousValues();
  } else if (heure == 12 && minute == 0 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("SIXIEME HEURE");
    resetPreviousValues();
  } else if (heure == 12 && minute == 50 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("DEUXIEME PAUSE");
    resetPreviousValues();
  } else if (heure == 13 && minute == 0 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("SEPTIEME HEURE");
    resetPreviousValues();
  } else if (heure == 13 && minute == 40 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("HUITIEME HEURE");
    resetPreviousValues();
  } else if (heure == 14 && minute == 20 && sec <= 10) {
    digitalWrite(relais_sirene, HIGH);
    clignoterEcran("FIN DE COURS");
    resetPreviousValues();
  } else {
    digitalWrite(relais_sirene, LOW);
  }
}

void resetPreviousValues() {
  previous_dow = -1;
  prevTemp = -100.0;
  prevHour = -1;
  prevMinute = -1;
  prevSecond = -1;
  previousDate = " ";
}

void clignoterEcran(String message) {
  unsigned long startMillis = millis();
  unsigned long currentMillis = millis();

  tft.setTextSize(3);

  // Calculer la largeur du texte
  int16_t textWidth = tft.textWidth(message);

  // Ajouter une marge de 10 pixels à gauche et à droite
  int16_t margin = 10;

  // Calculer la position du texte pour qu'il soit centré horizontalement
  int x = (tft.width() - textWidth) / 2;

  // Calculer la position verticale (centrée)
  int y = (tft.height() - (8 * 4)) / 2;  // La hauteur du texte est approximativement 8 pixels * taille du texte

  while (currentMillis - startMillis < 10 * 1000) {
    tft.fillScreen(TFT_BLUE);
    tft.setCursor(x - margin, y);
    tft.setTextColor(TFT_WHITE);
    tft.print(message);
    delay(1000);
    tft.fillScreen(TFT_RED);
    tft.setCursor(x - margin, y);
    tft.setTextColor(TFT_WHITE);
    tft.print(message);
    delay(1000);
    currentMillis = millis();
  }
  setup_TFT();
}


bool debounce(int pin) 
{
  if (digitalRead(pin) == LOW) {
    delay(50);
    if (digitalRead(pin) == LOW) {
      return true;
    }
  }
  return false;
}

byte edit(int parameter, int x, int y) 
{
  char text[3];
  sprintf(text, "%02u", parameter);

  tft.setTextColor(TFT_BROWN, TFT_WHITE); // Couleur du texte avec fond blanc

  while (debounce(choice_button)); // Attendre que B1 soit relâché

  while (true) {
    while (!digitalRead(button_increment)) {  // Tant que B2 est appuyé
      parameter++;
      if (x == 0 && parameter > 31) parameter = 1;  // Jour
      if (x == 72 && parameter > 12) parameter = 1; // Mois
      if (x == 192 && parameter > 99) parameter = 0; // Année
      if (x == 25 && parameter > 23) parameter = 0;  // Heure
      if (x == 95 && parameter > 59) parameter = 0;  // Minute

      sprintf(text, "%02u", parameter);
      tft.setCursor(x, y);
      tft.print(text);
      delay(200);
    }

    while (!digitalRead(button_desincrement)) {
      parameter--;
      if (x == 0 && parameter < 1) parameter = 31;
      if (x == 72 && parameter < 1) parameter = 12;
      if (x == 192 && parameter < 0) parameter = 99;
      if (x == 25 && parameter < 0) parameter = 23;
      if (x == 95 && parameter < 0) parameter = 59;

      sprintf(text, "%02u", parameter);
      tft.setCursor(x, y);
      tft.print(text);
      delay(200);  
    }

    tft.fillRect(x, y, 50, 30, TFT_WHITE);  // Effacer le texte précédent (ajuster la taille si nécessaire)
    unsigned long previous_m = millis();
    while ((millis() - previous_m < 250) && digitalRead(choice_button) && digitalRead(button_increment) && digitalRead(button_desincrement));
    tft.setCursor(x, y);
    tft.print(text);
    previous_m = millis();
    while ((millis() - previous_m < 250) && digitalRead(choice_button) && digitalRead(button_increment) && digitalRead(button_desincrement));

    if (!digitalRead(choice_button)) {  // Si B1 est appuyé
      return parameter; // Retourner la valeur du paramètre et quitter
    }
  }
}

void reglageManuelle()
{
  if (!digitalRead(choice_button)) {  // Si le bouton B1 est appuyé
    if (debounce(choice_button)) {    // Appel de la fonction de débouncing (s'assurer que B1 est appuyé)
      while (debounce(choice_button));  // Attendre que B1 soit relâché

      byte day    = edit(now.day(), 0, 120);
      byte month  = edit(now.month(), 72, 120);
      byte year   = edit(now.year() - 2000, 192, 120);
      byte hour   = edit(now.hour(), 25, 200);
      byte minute = edit(now.minute(), 95, 200);
      
      // Écrire les données de temps et de date sur la puce RTC
      rtc.adjust(DateTime(2000 + year, month, day, hour, minute, 0));
      
      while(debounce(choice_button));  // Attendre que B1 soit relâché
    }
  }
}

void displayDayOfWeek(DateTime now) 
{
  if (previous_dow != now.dayOfTheWeek()) {
    // Effacer l'affichage précédent
    tft.setTextColor(TFT_WHITE, TFT_WHITE);
    tft.setCursor(x_pos[previous_dow], 85);
    tft.setTextSize(3);
    tft.print(daysOfTheWeek[previous_dow]);

    // Afficher le nouveau jour de la semaine
    previous_dow = now.dayOfTheWeek();
    tft.setTextColor(TFT_BROWN);
    tft.setCursor(x_pos[previous_dow], 85);
    tft.setTextSize(3);
    tft.print(daysOfTheWeek[now.dayOfTheWeek()]);
  }
}

void displayDate(DateTime now) 
{
  String dayString = String(now.day());
  String monthString = String(now.month());

  // Ajouter un 0 devant le jour si inférieur à 10
  if (now.day() < 10) {
    dayString = "0" + dayString;
  }

  // Ajouter un 0 devant le mois si inférieur à 10
  if (now.month() < 10) {
    monthString = "0" + monthString;
  }

  String currentDate = dayString + "-" + monthString + "-" + String(now.year());

  if (currentDate != previousDate) {
    // Effacer l'affichage précédent
    tft.fillRect(0, 120, 200, 30, TFT_WHITE);

    // Afficher la nouvelle date
    tft.setCursor(0, 120);
    tft.setTextColor(TFT_BROWN);
    tft.setTextSize(4);
    tft.print(currentDate);

    previousDate = currentDate;
  }
}

void displayTime(DateTime now) {

  int currentHour = now.hour();
  int currentMinute = now.minute();
  int currentSecond = now.second();

  // Effacer et réécrire seulement si les valeurs ont changé
  if (currentHour != prevHour) {
    sprintf(_buffer, "%02u:", currentHour);
    tft.fillRect(25, 200, 60, 30, TFT_WHITE); // Effacer l'ancienne heure
    tft.setCursor(25, 200);
    tft.setTextColor(TFT_BROWN);
    tft.setTextSize(4);
    tft.printf(_buffer);
    prevHour = currentHour;
  }

  if (currentMinute != prevMinute) {
    sprintf(_buffer, "%02u:", currentMinute);
    tft.fillRect(95, 200, 60, 30, TFT_WHITE); // Effacer l'ancienne minute
    tft.setCursor(95, 200);
    tft.setTextColor(TFT_BROWN);
    tft.setTextSize(4);
    tft.print(_buffer);
    prevMinute = currentMinute;
  }

  if (currentSecond != prevSecond) {
    sprintf(_buffer, "%02u", currentSecond);
    tft.fillRect(165, 200, 60, 30, TFT_WHITE); // Effacer l'ancienne seconde
    tft.setCursor(165, 200);
    tft.setTextColor(TFT_BROWN);
    tft.setTextSize(4);
    tft.print(_buffer);
    prevSecond = currentSecond;
  }
}

void displayTemperature() 
{
  float temp_float = rtc.getTemperature();
  if (abs(temp_float - prevTemp) >= 0.1) {
    // Effacer l'affichage précédent
    tft.fillRect(30, 280, 200, 30, TFT_WHITE);

    // Afficher la nouvelle température
    int temp_int = int(temp_float);
    String temp_string = String(temp_float);
    tft.setCursor(40, 280);
    tft.setTextColor(TFT_BROWN);
    tft.setTextSize(4);
    tft.print(temp_string);

    // Dessiner le symbole °C
    int16_t x = 70 + temp_string.length() * 18; 
    int16_t y = 290;
    tft.drawCircle(x, y - 8, 2, TFT_BROWN); 
    tft.setCursor(x + 6, 280);
    tft.print("C");

    prevTemp = temp_float;
  }
}



