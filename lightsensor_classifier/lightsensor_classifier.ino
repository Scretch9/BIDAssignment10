#define LOWEST_VALUE 600
#define HIGHEST_VALUE 1100

#define LED_LEFT 6
#define LED_RIGHT 9
// pins
int sensorLeft = A4;
int sensorRight = A5;

//constants
const int histogramSize = 200;
const int maxSensorValue = 1000;

// flags
bool foundMaxima = false;

//global variables
int valueHistogram[histogramSize];
int intoHistogramSize = 1000;

//Values that occured most often, therefore indicating a threshold
int whiteValue;
int greyValue;
int blackValue;

int upperThreshold;
int lowerThreshold;

int sampleCount = 0;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  pinMode(sensorLeft, INPUT);
  pinMode(sensorRight, INPUT);

  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);

  //Serial.println("Start measuring!");
}

//TODO
// separate sampling for each sensor

void loop() {

  //start sampling values
  if (sampleCount < intoHistogramSize) {
    goForward();
    //put both sensor values into the histogram
    //intoHistogram(analogRead(sensorLeft));
    //delay(5);
    intoHistogram(analogRead(sensorLeft));
    delay(10);
    sampleCount += 1;
    //Serial.println(analogRead(sensorRight));
  } else {
    goForward();
    if (!foundMaxima) {
      Serial.println("finding maxima...");
      delay(10);
      findMaxima();
      computeThresholds();
      foundMaxima = true;

      Serial.print("upper threshold: "); Serial.println(upperThreshold);
      Serial.print("lower threshold: "); Serial.println(lowerThreshold);
      Serial.print("white: "); Serial.println(whiteValue);
      Serial.print("grey: "); Serial.println(greyValue);
      Serial.print("black: "); Serial.println(blackValue);
    } else {

      int measureLeft = intoHistogram(analogRead(sensorLeft));
      int measureRight = intoHistogram(analogRead(sensorRight));
      int left = classifyValue(measureLeft);
      int right = classifyValue(measureRight);
      Serial.print("left: "); Serial.print(left);
      Serial.print("right: "); Serial.println(right);
      driveControl(left,right);
      delay(1000);
      // code goes here
      // check both photodiodes and calculate the direction
      // if one photodiode has a different value,
    }

  }
}

void driveControl(int left,int right) {
  if (left == 0 && right == 2) {
    goLeft();
  }
  if (left == 1 && right == 0) {
    goLeft();
  }
  if (left == 2 && right == 1) {
    goLeft();
  }

  if (left == 0 && right == 1) {
    goRight();
  }
  if (left == 1 && right == 2) {
    goRight();
  }
  if (left == 2 && right == 0) {
    goRight();
  }
}

void lampsOff() {
  digitalWrite(LED_RIGHT, LOW);
  digitalWrite(LED_LEFT, LOW);
}

void goForward() {
  digitalWrite(LED_RIGHT, HIGH);
  digitalWrite(LED_LEFT, HIGH);
}

void goBackward() {
  digitalWrite(LED_RIGHT, HIGH);
  digitalWrite(LED_LEFT, HIGH);
  delay(500);
  digitalWrite(LED_RIGHT, LOW);
  digitalWrite(LED_LEFT, LOW);
}

void goLeft() {
  digitalWrite(LED_RIGHT, LOW);
  digitalWrite(LED_LEFT, HIGH);
}

void goRight() {
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, HIGH);
}

int classifyValue(int value) {
  if (value < lowerThreshold) {
    Serial.println("0");
    return 0;
  } else if (value < upperThreshold) {
    Serial.println("1");
    return 1;
  } else {
    Serial.println("2");
    return 2;
  }
}

int intoHistogram(int measuredValue) {
  //int i = (int) ((measuredValue * histogramSize) / maxSensorValue);
  int i = map(measuredValue, LOWEST_VALUE, HIGHEST_VALUE, 0 , histogramSize);
  valueHistogram[i] = valueHistogram[i] + 1;
  Serial.println(i);
  return i;
}



void findMaxima() {
  int localMax[(int) histogramSize / 2] = {0};
  int pointerToCurrentLocalMax = 0;
  int histogramPeaks[3] = {};


  //save all local maxima in localMax[]
  for (int i = 1; i < histogramSize; i++) {
    if (valueHistogram[i - 1] < valueHistogram[i] && valueHistogram[i] > valueHistogram[i + 1]) {
      localMax[pointerToCurrentLocalMax] = i;
      pointerToCurrentLocalMax ++;
    }
  }


  //find the three highest local maxima
  for (int j = 0; j < 3; ++j)
  {
    int _max = 0;
    int index = 0;
    for (int i = 1; i < (int) (histogramSize / 2); i++) {
      if (valueHistogram[localMax[i]] > _max) {
        _max = valueHistogram[localMax[i]];
        index = i;
      }
    }
    histogramPeaks[j] = localMax[index];
    valueHistogram[localMax[index]] = 0;
  }

  blackValue = 10000;
  whiteValue = 0;
  int sum = 0;
  // associate three maxima with grey, white and black
  for (int i = 0; i < 3; ++i)
  {
    Serial.println(histogramPeaks[i]);
    delay(10);
    sum += histogramPeaks[i];
    if (histogramPeaks[i] < blackValue) blackValue = histogramPeaks[i];
    if (histogramPeaks[i] > whiteValue) whiteValue = histogramPeaks[i];
  }

  greyValue =  sum - whiteValue -  blackValue;

  Serial.println("Found Maxima!");
  delay(10);
}

void computeThresholds() {
  upperThreshold = (int)((whiteValue + greyValue) / 2);
  lowerThreshold = (int)((blackValue + greyValue) / 2);
}
