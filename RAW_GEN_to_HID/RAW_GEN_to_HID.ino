#define PIN_UP     4
#define PIN_DOWN   5
#define PIN_LEFT   6
#define PIN_RIGHT  7
#define PIN_B      15
#define PIN_C      17
#define PIN_SEL    16

void setup() {
  Serial.begin(115200);

  pinMode(PIN_UP, INPUT);
  pinMode(PIN_DOWN, INPUT);
  pinMode(PIN_LEFT, INPUT);
  pinMode(PIN_RIGHT, INPUT);
  pinMode(PIN_B, INPUT);
  pinMode(PIN_C, INPUT);

  pinMode(PIN_SEL, OUTPUT);
}

void loop() {
  bool up, down, left, right;
  bool a, b, c, start;

  digitalWrite(PIN_SEL, HIGH);
  delayMicroseconds(50);

  up    = !digitalRead(PIN_UP);
  down  = !digitalRead(PIN_DOWN);
  left  = !digitalRead(PIN_LEFT);
  right = !digitalRead(PIN_RIGHT);
  b     = !digitalRead(PIN_B);
  c     = !digitalRead(PIN_C);

  
  digitalWrite(PIN_SEL, LOW);
  delayMicroseconds(50);

  a     = !digitalRead(PIN_B);
  start = !digitalRead(PIN_C);

  Serial.print("U:"); Serial.print(up);
  Serial.print(" D:"); Serial.print(down);
  Serial.print(" L:"); Serial.print(left);
  Serial.print(" R:"); Serial.print(right);
  Serial.print(" A:"); Serial.print(a);
  Serial.print(" B:"); Serial.print(b);
  Serial.print(" C:"); Serial.print(c);
  Serial.print(" START:"); Serial.println(start);

  delay(100);
}