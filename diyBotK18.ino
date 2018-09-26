#include <diyBotLib.h>
#include <Arduino.h>

const int vitesse = 60;

//variables
volatile unsigned long nbseconde = 0;
volatile bool isTraitement = false;

int distance = 0;
int anglerepos = 90;
bool isObstacle = false;

//obj
DiyBot bot;

//ISR Te=40us
ISR(TIMER1_COMPA_vect)
{
  nbseconde++;
  if (nbseconde > 50000 && isTraitement == false)
  {
    isTraitement = true;
    nbseconde = 0;
  }
  bot.turretAction();
}
//
void testLed()
{
  bot.setLed(true);
  delay(50);
  bot.setLed(false);
  delay(100);
}
//
void testMotor()
{
  bot.setMotorSpeeds(vitesse, vitesse, 2000);
  bot.setMotorStop(1000);

  bot.setMotorSpeeds(-vitesse, vitesse, 500);
  bot.setMotorStop(1000);

  bot.setMotorSpeeds(vitesse, -vitesse, 500);
  bot.setMotorStop(1000);

  bot.setMotorSpeeds(-vitesse, -vitesse, 2000);
  bot.setMotorStop(1000);
}
//
void testTete()
{
  bot.turretWriteAngle(90, 1000);
  bot.turretWriteAngle(170, 1000);
  bot.turretWriteAngle(90, 1000);
  bot.turretWriteAngle(10, 1000);
  bot.turretWriteAngle(90, 1000);
}
//
void testDistance()
{
  distance = bot.turretReadDistance_mm();
  //
  Serial.print(F("dist= "));
  Serial.println(distance);
  //
  if (distance < 100)
  {
    bot.setLed(true);
  }
  else
  {
    bot.setLed(false);
  }
  //
  delay(500);
}
//
uint8_t chercheAngleRepos()
{
  uint8_t angle = 0;
  int droite = 0;
  int gauche = 0;

  bot.turretWriteAngle(0, 500);
  droite = bot.turretReadDistance_mm();
  delay(300);

  bot.turretWriteAngle(180, 500);
  droite = bot.turretReadDistance_mm();
  delay(300);
  //
  if (gauche < droite)
    angle = 180;
  else
    angle = 0;
  return angle;
}
//
void cherchePositionLibre()
{
  int droite = 0;
  int gauche = 0;

  //
  if (isObstacle)
  {
    bot.turretWriteAngle(0, 500);
    droite = bot.turretReadDistance_mm();
    delay(300);

    bot.turretWriteAngle(180, 500);
    gauche = bot.turretReadDistance_mm();
    delay(300);
    //
    if (droite < gauche)
    {
      bot.setMotorSpeeds(-vitesse, vitesse, 500);
    }
    else
    {
      bot.setMotorSpeeds(vitesse, -vitesse, 500);
    }
    //
    bot.turretWriteAngle(90, 200);
    isObstacle = false;
  }
}
//
void suivreMur()
{
  int erreur = 0;
  static int erreur_av = 0;
  int vitesseGauche = 0;
  int vitesseDroite = 0;
  int dis = 0;
  int sortie = 0;

  dis = bot.turretReadDistance_mm();
  if (dis > 200)
    dis = 200;

  dis = dis / 10;

  erreur = 12 - dis;
  sortie = 4 * erreur - erreur_av;
  //
  if (anglerepos == 180)
  {
    vitesseGauche = vitesse + sortie;
    vitesseDroite = vitesse - sortie;
  }
  else
  {
    vitesseGauche = vitesse - sortie;
    vitesseDroite = vitesse + sortie;
  }
  //mem
  erreur_av = erreur;

  bot.setMotorSpeeds(vitesseGauche, vitesseDroite);
}
//
void action()
{
  if (isTraitement == false)
  {
    suivreMur();
  }
  else
  {
    bot.setMotorStop(0);
    bot.turretWriteAngle(90, 500);

    if (bot.turretReadDistance_mm() <= 150)
    {
      isObstacle = true;
      cherchePositionLibre();

      isObstacle = false;
    }
    //
    bot.turretWriteAngle(anglerepos, 200);
    isTraitement = false;
  }
}
//***************
void setup()
{
  Serial.begin(9600);
  //
  bot.begin();
  //
  bot.turretWriteAngle(90, 200);

  bot.waitButtonPressAndReleased();
  //
  anglerepos = chercheAngleRepos();
  delay(500);
}

//****************
void loop()
{
  action();
}
