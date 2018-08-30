const int motor_1_a   	= 3;
const int motor_1_b   	= 5;
const int motor_2_a		= 6;
const int motor_2_b		= 9;
const int sensor		= 2;
const int SENSOR_MAX	= 300;
const int SENSOR_MIN	= 50;

//Dados da rede
const double w1[5] = 
  	{ -20.9891, 3.44292, 0.570445, 8.65519, 2.11751 };
  
const double w2[2][5] = 
	{
    	{ -9.53031, -3.55946, -0.271678, 6.04317, -0.666103 },
    	{ -8.13692,  1.64008,  0.812694, 3.35845, -2.507660 }
 	};

const double b1[5] = 
	{ 0.289591, -1.32474, 0.313235, -7.61457, -0.410876};
  

const double b2[2] = 
	{ 3.07798, 0.90375 };

const int layerSize[3] = {1, 5, 2};


//---------------

double sigmoid(double x)
{
  return 1 / (1 + exp(-x));
}

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

void parar_motor_1()
{
  digitalWrite(motor_1_a, LOW);
  digitalWrite(motor_1_b, LOW);  
}

void parar_motor_2()
{
  digitalWrite(motor_2_a, LOW);
  digitalWrite(motor_2_b, LOW);  
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

void calculateOutput(double dist, double * Vout1, double * Vout2)
{
  double net[5];
  int neuron, k;
  double v1, v2;
  v1 = 0;
  v2 = 0;
  for (neuron = 0; neuron < layerSize[1]; neuron++)
  {
    net[neuron] = dist * w1[neuron] + b1[neuron];
    net[neuron] = sigmoid (net[neuron]);
  }
  
  for (k = 0; k < layerSize[1]; k++)
    v1 = v1 + net[k] * w2[0][k];
  v1 = v1 + b2[0];
  
  for (k = 0; k < layerSize[1]; k++)
    v2 = v2 + net[k] * w2[1][k];
  v2 = v2 + b2[1];
  
  v1 = sigmoid(v1);
  v2 = sigmoid(v2);
  
  v1 = (v1 * 510) -255;
  v2 = (v2 * 510) -255;
  
  *Vout1 = v1;
  *Vout2 = v2;
}

void setup()
{
  pinMode(motor_1_a, OUTPUT);
  pinMode(motor_1_b, OUTPUT);
  pinMode(motor_2_a, OUTPUT);
  pinMode(motor_2_b, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  double dist = distancia() / 300;
  double v1, v2;
  calculateOutput(dist, &v1, &v2);
  if (v1 > 0) acelerar_motor_1(v1);
  else rear_motor_1(v1 * (-1));
  if (v2 > 0) acelerar_motor_2(v2);
  else rear_motor_2(v2 * (-1));
  
}