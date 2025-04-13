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
char daysOfTheWeek[7][12] = { "Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi" };
byte x_pos[7] = { 80, 70, 57, 40, 49, 70, 49 };

static byte previous_dow = 8;
static int prevHour = -1, prevMinute = -1, prevSecond = -1;
static String previousDate = "";
static float prevTemp = 0.0;
int heure = 0, minute = 0, sec = 0;

void setup() {
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

void setup_TFT() {
  tft.init();
  tft.setRotation(1);  // Paysage
  tft.fillScreen(TFT_WHITE);

  tft.setTextColor(TFT_BLUE);
  tft.setTextSize(4);

  // --- Titre centré ---
  String title = "Lycee Kwesu";
  int16_t titleX = (tft.width() - tft.textWidth(title)) / 2;
  tft.setCursor(titleX, 10);
  tft.print(title);

  // --- Ligne après le titre ---
  tft.drawFastHLine(0, 45, tft.width(), TFT_BLUE);

  // --- Jour centré ---
  /* String day = "Saturday";
  int16_t dayX = (tft.width() - tft.textWidth(day)) / 2;
  tft.setCursor(dayX, 60);
  tft.print(day); */

  // --- Date centrée ---
  /* String date = "01-01-2020";
  int16_t dateX = (tft.width() - tft.textWidth(date)) / 2;
  tft.setCursor(dateX, 100);
  tft.print(date); */

  // --- Ligne après la date ---
  tft.drawFastHLine(0, 140, tft.width(), TFT_BLUE);

  // --- Texte Heure centré ---
  String heureLabel = "Heure :";
  int16_t heureLabelX = (tft.width() - tft.textWidth(heureLabel)) / 2;
  tft.setCursor(heureLabelX, 150);
  tft.print(heureLabel);

  // --- Heure centrée ---
  /* String heure = "01:01:40";
  int16_t heureX = (tft.width() - tft.textWidth(heure)) / 2;
  tft.setCursor(heureX, 190);
  tft.print(heure); */

  // --- Ligne après l’heure ---
  tft.drawFastHLine(0, 230, tft.width(), TFT_BLUE);

  // --- Température centrée ---
  String tempLabel = "Temperature :";
  int16_t tempLabelX = (tft.width() - tft.textWidth(tempLabel)) / 2;
  tft.setCursor(tempLabelX, 240);
  tft.print(tempLabel);

  /* String temp = "26.25*C";
  int16_t tempX = (tft.width() - tft.textWidth(temp)) / 2;
  tft.setCursor(tempX, 280);
  tft.print(temp); */
}

void loop() {
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

  tft.setTextSize(4);  // Taille du texte à 4

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



bool debounce(int pin) {
  if (digitalRead(pin) == LOW) {
    delay(50);
    if (digitalRead(pin) == LOW) {
      return true;
    }
  }
  return false;
}

byte edit(int parameter, int x, int y) {
  char text[3];
  sprintf(text, "%02u", parameter);

  tft.setTextSize(4);  // Taille du texte à 4
  tft.setTextColor(TFT_BROWN, TFT_WHITE);  // Couleur du texte avec fond blanc

  while (debounce(choice_button))
    ;  // Attendre que B1 soit relâché

  while (true) {
    while (!digitalRead(button_increment)) {  // Tant que B2 est appuyé
      parameter++;
      if (x == 120 && parameter > 31) parameter = 1;    // Jour
      if (x == 192 && parameter > 12) parameter = 1;   // Mois
      if (x == 310 && parameter > 99) parameter = 0;  // Année
      if (x == 140 && parameter > 23) parameter = 0;   // Heure
      if (x == 210 && parameter > 59) parameter = 0;   // Minute

      sprintf(text, "%02u", parameter);
      tft.setCursor(x, y);
      tft.print(text);
      delay(200);
    }

    while (!digitalRead(button_desincrement)) {
      parameter--;
      if (x == 120 && parameter < 1) parameter = 31;
      if (x == 192 && parameter < 1) parameter = 12;
      if (x == 310 && parameter < 0) parameter = 99;
      if (x == 140 && parameter < 0) parameter = 23;
      if (x == 210 && parameter < 0) parameter = 59;

      sprintf(text, "%02u", parameter);
      tft.setCursor(x, y);
      tft.print(text);
      delay(200);
    }

    tft.fillRect(x, y, 50, 30, TFT_WHITE);  // Effacer le texte précédent (ajuster la taille si nécessaire)
    unsigned long previous_m = millis();
    while ((millis() - previous_m < 250) && digitalRead(choice_button) && digitalRead(button_increment) && digitalRead(button_desincrement))
      ;
    tft.setCursor(x, y);
    tft.print(text);
    previous_m = millis();
    while ((millis() - previous_m < 250) && digitalRead(choice_button) && digitalRead(button_increment) && digitalRead(button_desincrement))
      ;

    if (!digitalRead(choice_button)) {  // Si B1 est appuyé
      return parameter;                 // Retourner la valeur du paramètre et quitter
    }
  }
}


void reglageManuelle() {
  if (!digitalRead(choice_button)) {  // Si le bouton B1 est appuyé
    if (debounce(choice_button)) {    // Appel de la fonction de débouncing (s'assurer que B1 est appuyé)
      while (debounce(choice_button))
        ;  // Attendre que B1 soit relâché

      byte day = edit(now.day(), 120, 100);
      byte month = edit(now.month(), 192, 100);
      byte year = edit(now.year() - 2000, 310, 100);
      byte hour = edit(now.hour(), 140, 190);
      byte minute = edit(now.minute(), 210, 190);

      // Écrire les données de temps et de date sur la puce RTC
      rtc.adjust(DateTime(2000 + year, month, day, hour, minute, 0));

      while (debounce(choice_button))
        ;  // Attendre que B1 soit relâché
    }
  }
}


void displayDayOfWeek(DateTime now) {
  String currentDay = daysOfTheWeek[now.dayOfTheWeek()];  // Jour actuel

  // Calcul de la position horizontale pour centrer le texte
  int16_t dayX = (tft.width() - tft.textWidth(currentDay)) / 2;
  int16_t y = 60;  // Position verticale du jour de la semaine

  // Si le jour a changé, on met à jour l'affichage
  if (previous_dow != now.dayOfTheWeek()) {
    // Effacer l'affichage précédent
    tft.fillRect(0, y, tft.width(), 36, TFT_WHITE);

    // Afficher le nouveau jour de la semaine centré
    previous_dow = now.dayOfTheWeek();
    tft.setTextColor(TFT_BLUE);
    tft.setTextSize(4);  // Taille du texte
    tft.setCursor(dayX, y);
    tft.print(currentDay);
  }
}

void displayDate(DateTime now) {
  String dayString = String(now.day());
  String monthString = String(now.month());

  // Ajouter un 0 devant le jour/mois si nécessaire
  if (now.day() < 10) dayString = "0" + dayString;
  if (now.month() < 10) monthString = "0" + monthString;

  String currentDate = dayString + "-" + monthString + "-" + String(now.year());

  if (currentDate != previousDate) {
    // Largeur estimée d’un caractère à setTextSize(4)
    int charWidth = 24;
    int textLength = currentDate.length();
    int totalWidth = textLength * charWidth;
    int startX = (tft.width() - totalWidth) / 2;
    int y = 100;

    // Effacer l'affichage précédent
    tft.fillRect(startX, y, totalWidth, 36, TFT_WHITE);

    // Afficher la nouvelle date
    tft.setCursor(startX, y);
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

  // Paramètres d'affichage
  tft.setTextColor(TFT_BROWN);
  tft.setTextSize(4);  // Confirmé

  // Largeur estimée d’un caractère avec setTextSize(4)
  int charWidth = 24;  // Taille plus grande que TextSize(3)

  // Position de départ centrée pour "HH:MM:SS"
  int totalWidth = 8 * charWidth;
  int startX = (tft.width() - totalWidth) / 2;
  int y = 190;

  // Affichage heure
  if (currentHour != prevHour) {
    sprintf(_buffer, "%02u:", currentHour);
    tft.fillRect(startX, y, charWidth * 3, 36, TFT_WHITE);  // Hauteur ajustée pour TextSize(4)
    tft.setCursor(startX, y);
    tft.print(_buffer);
    prevHour = currentHour;
  }

  // Affichage minutes
  if (currentMinute != prevMinute) {
    sprintf(_buffer, "%02u:", currentMinute);
    tft.fillRect(startX + charWidth * 3, y, charWidth * 3, 36, TFT_WHITE);
    tft.setCursor(startX + charWidth * 3, y);
    tft.print(_buffer);
    prevMinute = currentMinute;
  }

  // Affichage secondes
  if (currentSecond != prevSecond) {
    sprintf(_buffer, "%02u", currentSecond);
    tft.fillRect(startX + charWidth * 6, y, charWidth * 2, 36, TFT_WHITE);
    tft.setCursor(startX + charWidth * 6, y);
    tft.print(_buffer);
    prevSecond = currentSecond;
  }
}


void displayTemperature() {
  float temp_float = rtc.getTemperature();

  if (abs(temp_float - prevTemp) >= 0.1) {
    // Effacer la zone entière de température
    tft.fillRect(0, 280, tft.width(), 40, TFT_WHITE);

    // Préparer la température à afficher
    String temp_string = String(temp_float, 2);  // Ex: "26.25"

    // Taille texte
    tft.setTextSize(4);
    tft.setTextColor(TFT_BROWN);

    // Calcul de la largeur totale : texte + espace + cercle + "C"
    int temp_width = tft.textWidth(temp_string);
    int total_width = temp_width + 12 + tft.textWidth("C");  // cercle et espace après

    // Point de départ pour centrer tout
    int16_t startX = (tft.width() - total_width) / 2;
    int16_t y = 280;

    // Affichage température
    tft.setCursor(startX, y);
    tft.print(temp_string);

    // Cercle pour ° (petit rond)
    int16_t circleX = startX + temp_width + 4;
    tft.drawCircle(circleX, y + 8, 3, TFT_BROWN);

    // Affichage du C
    tft.setCursor(circleX + 8, y);
    tft.print("C");

    // Màj de la valeur précédente
    prevTemp = temp_float;
  }
}
