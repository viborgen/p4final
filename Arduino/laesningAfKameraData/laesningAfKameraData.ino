int VSync = 3;
int Href = 8;
int plck = 2;
const int picy = 480;
const int picx = 640;
// datapins
int d0 = A8;
int d1 = A9;
int d2 = A10;
int d3 = A11;
int d4 = 37;
int d5 = 36;
int d6 = 35;
int d7 = 34;
byte arr[(picx * 2 - 8) * 6];
const int datasize = (picx * 2 - 9) * 6;

int r = 0;
int c = 0;

int cFlag = 0;
int rFlag = 0;
int noData = 0;
unsigned long startTime;
unsigned long stopTime;

void setup()
{
  Serial.begin(115200);
  pinMode(VSync, INPUT);
  pinMode(Href, INPUT);
  pinMode(plck, INPUT);
  pinMode(d0, INPUT);
  pinMode(d1, INPUT);
  pinMode(d2, INPUT);
  pinMode(d3, INPUT);
  pinMode(d4, INPUT);
  pinMode(d5, INPUT);
  pinMode(d6, INPUT);
  pinMode(d7, INPUT);
  //DDRD = DDRD | 0;
}

void loop()
{
  int x, y;

  r = 0;

  while (!(PINE & 32)); //venter på Vsync går høj
  y = picy;
  while (PINE & 32); // venter på Vsync går lav.

  startTime = millis();
  while (y)
  {
    while (!(PINH & 32)); // venter på at HREF går høj
    x = picx * 2 - 9;
    c = 83;
    while (x)
    {
      while (!(PINE & 16));  // venter på at PCLK går høj
      arr[(1271 - x) + (1271 * r)] = (PINK & 15) | ((PINC & 15) << 4);
      while (PINE & 116);     // venter på at PLCK går lav
      x--;
    }
    while (PINH & 32); // venter på at HREF går lav
    if (picy - y > 10 && r < 5 ) r++;
    y--;
  }
  Serial.println(millis() - startTime);
  Serial.print("[");
  
  for (int i = 0; i < 1271 * 6; i++)
  {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
  Serial.println("]");
}
