int led1 = 4;
int led2 = 7;
int led3 = 8;
int led4 = 12;
int led5 = 14;
int led6 = 15;
int led7 = 16;
int led8 = 17;
int dim1 = 6;
int dim2 = 9;
int dim3 = 10;
int dim4 = 11;
int mot1 = 9;
int mot2 = 5;
int swi = 2;

int i = 0;
int way = 0;


void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(led8, OUTPUT);
  pinMode(dim1, OUTPUT);
  pinMode(dim2, OUTPUT);
  pinMode(dim3, OUTPUT);
  pinMode(dim4, OUTPUT);
  pinMode(mot1, OUTPUT);
  pinMode(mot2, OUTPUT);
  pinMode(swi, INPUT);
  pinMode(13, OUTPUT);
}

void loop() {
      digitalWrite(mot1, HIGH);
      digitalWrite(mot2, HIGH);
      delay(5);
  
}
