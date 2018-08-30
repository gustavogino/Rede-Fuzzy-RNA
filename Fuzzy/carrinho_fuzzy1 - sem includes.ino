


const int motor_1_a   	= 3;
const int motor_1_b   	= 5;
const int motor_2_a		= 6;
const int motor_2_b		= 9;
const int sensor		= 2;
const int SENSOR_MAX	= 300;
const int SENSOR_MIN	= 50;


//---------------

double distancia()
{
  double tempo;
  
  // Emite sinal
  pinMode(sensor, OUTPUT);
  digitalWrite(sensor, HIGH);
  delayMicroseconds(100);
  digitalWrite(sensor, LOW);
  // Configura sensor para receber sinal
  pinMode(sensor, INPUT);
  tempo = pulseIn(sensor, HIGH);
  return tempo / 2 / 29.4;
}

void acelerar_motor_1(double velocidade)
{
  digitalWrite(motor_1_b, LOW);
  analogWrite(motor_1_a, velocidade);
}

void rear_motor_1(double velocidade)
{
  digitalWrite(motor_1_a, LOW);
  analogWrite(motor_1_b, velocidade);
}

void acelerar_motor_2(double velocidade)
{
  digitalWrite(motor_2_b, LOW);
  analogWrite(motor_2_a, velocidade);
}

void rear_motor_2(double velocidade)
{
  digitalWrite(motor_2_a, LOW);
  analogWrite(motor_2_b, velocidade);
}

void setup()
{
  pinMode(motor_1_a, OUTPUT);
  pinMode(motor_1_b, OUTPUT);
  pinMode(motor_2_a, OUTPUT);
  pinMode(motor_2_b, OUTPUT);
  
    ///Define input da distancia
    FuzzyInput* distancia = new FuzzyInput(1);
        ///Definicao do FuzzySet da distancia
    FuzzySet* bateu = new FuzzySet(0, 0, 10, 50);
    distancia->addFuzzySet(bateu);
    FuzzySet* pertao = new FuzzySet(30, 80, 80, 110);
    distancia->addFuzzySet(pertao);
    FuzzySet* perto = new FuzzySet(90, 130, 130, 160);
    distancia->addFuzzySet(perto);
    FuzzySet* media = new FuzzySet(150, 180, 180, 210);
    distancia->addFuzzySet(media);
    FuzzySet* longe = new FuzzySet(200, 230, 230, 250);
    distancia->addFuzzySet(longe);
    FuzzySet* seguro = new FuzzySet(240, 280, 300, 300);
    distancia->addFuzzySet(seguro);
        ///Adiciona distancia ao input
    fuzzy->addFuzzyInput(distancia);
    ///------------------------------------------


    ///Definicao de outputs
    FuzzyOutput* velocidade1 = new FuzzyOutput(1);
    FuzzyOutput* velocidade2 = new FuzzyOutput(2);
        ///FuzzySet de velocidade1 e velocidade2
    FuzzySet* rear1 = new FuzzySet(-255, -255, -200, 0);
    velocidade1->addFuzzySet(rear1);
    FuzzySet* baixo1 = new FuzzySet(0, 30, 30, 70);
    velocidade1->addFuzzySet(baixo1);
    FuzzySet* medio1 = new FuzzySet(60, 100, 100, 140);
    velocidade1->addFuzzySet(medio1);
    FuzzySet* alto1 = new FuzzySet(130, 180, 200, 200);
    velocidade1->addFuzzySet(alto1);
    FuzzySet* turbo1 = new FuzzySet(200, 230, 255, 255);
    velocidade1->addFuzzySet(turbo1);

    FuzzySet* rear2 = new FuzzySet(-255, -255, -200, 0);
    velocidade2->addFuzzySet(rear2);
    FuzzySet* baixo2 = new FuzzySet(0, 30, 30, 70);
    velocidade2->addFuzzySet(baixo2);
    FuzzySet* medio2 = new FuzzySet(60, 100, 100, 140);
    velocidade2->addFuzzySet(medio2);
    FuzzySet* alto2 = new FuzzySet(130, 180, 200, 200);
    velocidade2->addFuzzySet(alto2);
    FuzzySet* turbo2 = new FuzzySet(200, 230, 255, 255);
    velocidade2->addFuzzySet(turbo2);
        ///Adiciona velocidade1 ao output
    fuzzy->addFuzzyOutput(velocidade1);
    fuzzy->addFuzzyOutput(velocidade2);
    ///------------------------------------------

    ///Definindo Regras
        ///Definindo Antecedentes
    FuzzyRuleAntecedent* if_bateu = new FuzzyRuleAntecedent();
    if_bateu->joinSingle(bateu);
    FuzzyRuleAntecedent* if_pertao = new FuzzyRuleAntecedent();
    if_pertao->joinSingle(pertao);
    FuzzyRuleAntecedent* if_perto = new FuzzyRuleAntecedent();
    if_perto->joinSingle(perto);
    FuzzyRuleAntecedent* if_media = new FuzzyRuleAntecedent();
    if_media->joinSingle(media);
    FuzzyRuleAntecedent* if_longe = new FuzzyRuleAntecedent();
    if_longe->joinSingle(longe);
    FuzzyRuleAntecedent* if_seguro = new FuzzyRuleAntecedent();
    if_seguro->joinSingle(seguro);
        ///Definindo Consequentes
    FuzzyRuleConsequent* motor1_rear = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* motor2_rear = new FuzzyRuleConsequent();
    motor1_rear->addOutput(rear1);
    motor2_rear->addOutput(rear2);
    FuzzyRuleConsequent* motor1_baixo = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* motor2_baixo = new FuzzyRuleConsequent();
    motor1_baixo->addOutput(baixo1);
    motor2_baixo->addOutput(baixo2);
    FuzzyRuleConsequent* motor1_medio = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* motor2_medio = new FuzzyRuleConsequent();
    motor1_medio->addOutput(medio1);
    motor2_medio->addOutput(medio2);
    FuzzyRuleConsequent* motor1_alto = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* motor2_alto = new FuzzyRuleConsequent();
    motor1_alto->addOutput(alto1);
    motor2_alto->addOutput(alto2);
    FuzzyRuleConsequent* motor1_turbo = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* motor2_turbo = new FuzzyRuleConsequent();
    motor1_turbo->addOutput(turbo1);
    motor2_turbo->addOutput(turbo2);

        ///Definindo regras
    FuzzyRule* fuzzyRule01 = new FuzzyRule(1, if_bateu, motor1_rear);
    FuzzyRule* fuzzyRule02 = new FuzzyRule(2, if_bateu, motor2_rear);
    fuzzy->addFuzzyRule(fuzzyRule01);
    fuzzy->addFuzzyRule(fuzzyRule02);
    FuzzyRule* fuzzyRule03 = new FuzzyRule(3, if_pertao, motor1_medio);
    FuzzyRule* fuzzyRule04 = new FuzzyRule(4, if_pertao, motor2_baixo);
    fuzzy->addFuzzyRule(fuzzyRule03);
    fuzzy->addFuzzyRule(fuzzyRule04);
    FuzzyRule* fuzzyRule05 = new FuzzyRule(5, if_perto, motor1_medio);
    FuzzyRule* fuzzyRule06 = new FuzzyRule(6, if_perto, motor2_baixo);
    fuzzy->addFuzzyRule(fuzzyRule05);
    fuzzy->addFuzzyRule(fuzzyRule06);
    FuzzyRule* fuzzyRule07 = new FuzzyRule(7, if_media, motor1_baixo);
    FuzzyRule* fuzzyRule08 = new FuzzyRule(8, if_media, motor2_medio);
    fuzzy->addFuzzyRule(fuzzyRule07);
    fuzzy->addFuzzyRule(fuzzyRule08);
    FuzzyRule* fuzzyRule09 = new FuzzyRule(9, if_longe, motor1_alto);
    FuzzyRule* fuzzyRule10 = new FuzzyRule(10, if_longe, motor2_alto);
    fuzzy->addFuzzyRule(fuzzyRule09);
    fuzzy->addFuzzyRule(fuzzyRule10);
    FuzzyRule* fuzzyRule11 = new FuzzyRule(11, if_seguro, motor1_turbo);
    FuzzyRule* fuzzyRule12 = new FuzzyRule(12, if_seguro, motor2_turbo);
    fuzzy->addFuzzyRule(fuzzyRule11);
    fuzzy->addFuzzyRule(fuzzyRule12);
    ///------------------------------------------------------------  
  
  Serial.begin(9600);
}

void loop()
{
  double dist = distancia();
  fuzzy->setInput(1, dist);
  fuzzy->fuzzify();
  double v1 = fuzzy->defuzzify(1);
  double v2 = fuzzy->defuzzify(2);
  v1 = (v1+255)/510;
  v2 = (v2+255)/510;
  if (v1 > 0) acelerar_motor_1(v1);
  else rear_motor_1(v1 * (-1));
  if (v2 > 0) acelerar_motor_2(v2);
  else rear_motor_2(v2 * (-1));
  delay(100);
}