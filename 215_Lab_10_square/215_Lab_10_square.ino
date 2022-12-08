/*--------- 
  SQUARE 
  --------- 
  ESE 215 - final project 
*/ 

#include <SPI.h>
#include <SD.h>
File myFile;
#define SD_ChipSelectPin 4

void setup() { 
  //bottom motor 
  pinMode(5, OUTPUT); //pwm 
  pinMode(6, OUTPUT); //digital = dir 
  //top motor 
  pinMode(9, OUTPUT); //pwm 
  pinMode(8, OUTPUT); //digital = dir 

  // cahnge pins 12 -> 8, 10 -> 9


  Serial.begin(9600);
  Serial.setTimeout(1);

  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD fail");
    return;
  } else {
    Serial.println("SD Found"); 
  }

  // name of file
  myFile = SD.open("AJ1000.txt");

  // start at (0, 0) always
  digitalWrite(6, HIGH); 
  analogWrite(5, 0);
  digitalWrite(8, HIGH); 
  analogWrite(9, 0);
  delay(10000);
} 
/* Setup alternating movement, 
  by giving enough delay and only changing 
  the set voltage for one motor at a time. 
*/ 

// save the point it was at last
int prevX = 0;
int prevY = 0;

void plot(float x, float y) {
  int lastX = prevX;
  int lastY = prevY;
  float xDist = x - lastX;
  float yDist = y - lastY;
  int delayOffset = 25;
  // points between 0 and 508
  // if x is in the first quadrent dir is high so less than 254
  // if in second quadrent dir is low and in a range of 0 to 254

  // if the slope is 0 the move right to the point (straight line)
  float ySlope;
  float xSlope;
  if (xDist == 0) {
    ySlope = yDist / abs(yDist);
    xSlope = 0;
  } else if (yDist == 0) {
    ySlope = 0;
    xSlope = xDist / abs(xDist);
  } else {
    ySlope = (yDist / abs(yDist)) * abs(yDist / xDist);
    xSlope = xDist / abs(xDist);
  }
  
  // start at last point and increase by the y componet of the slope for y and x component for x
  float xMove = lastX; // declare x so they run at the same time
  float yMove = lastY;
  float thresh = 0.25;
  // ((xMove >= (x-thresh)) && xMove <= (x+thresh)) || ((yMove >= (x-thresh)) && (yMove <= (y+thresh)))
  // (xMove != x) || (yMove != y)
  while (((xMove <= (x-thresh)) || (xMove >= (x+thresh))) || ((yMove <= (y-thresh)) || (yMove >= (y+thresh)))) {
    //Serial.println("WHILE");
    yMove += ySlope;
    if (yMove <= 254) {
      digitalWrite(8, HIGH); 
      analogWrite(9, (int) yMove);
    } else {
      digitalWrite(8, LOW); 
      analogWrite(9, ((int) yMove) - 254);
    }
    delay(10);

    // incrament the x by 1 or negative 1 depending on the ySlope
    xMove += xSlope;
    if (xMove <= 254) {
      digitalWrite(6, HIGH); 
      analogWrite(5, (int) xMove);
    } else {
      digitalWrite(6, LOW); 
      analogWrite(5, ((int) xMove) - 254);
    }
    delay(10);

    //print to the point
    Serial.print(xMove);
    Serial.print(" ");
    Serial.print(ySlope);
    Serial.print(" ");
    Serial.println(yMove);
    
  }
  
  // distance formula to find time between points
  float dist = xDist*xDist + yDist*yDist;
  float runTime = sqrt(dist);

  // print the point
  Serial.print("(");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(") delay: ");
  Serial.print(runTime * delayOffset); 
  Serial.println();

  // account for the time it takes for the pen to move
  if ((xDist == 0) || (yDist == 0)) {
    delay(runTime * delayOffset);
  } else {
    delay(500);
  }
  
  // wait aftre first move to lower pen
  if ((prevX == 0) && (prevY == 0)) {
    Serial.println("pause to screw");
    delay(5000);
  }

  prevX = x;
  prevY = y;
}

void loop() { 

  // high 255 = -0V
  // high 0 = -12V
  // low 0 = 0V 
  // low 255 = 12V

  // pin 6 x-axis
  // pin 8 y-axis

  /*
  // for serial communicaiton
  Serial.println("ready");
  String point;
  while(true) {
    point = Serial.readString();
    if ((!point.equals("ready")) || (!point.equals("pp"))) {
      Serial.println("pp");
      break;
    }
  }
  */

  String line = "";
  while (myFile.available()) {
    char letter = (char) myFile.read();
    if (letter != '\n') {
      line.concat(letter);
    } else {
      Serial.println(line);
      LineToPoint(line);
      line = "";
    }
  }
  Serial.println(line);
  LineToPoint(line);
  myFile.close();
  Serial.println("Done");
  while(1);
} 

void LineToPoint(String point) {
  // turn the raw line with ploted point
  // format: 000 000/
  String numbers[2];
  numbers[0] = point.substring(0, point.indexOf(" "));
  numbers[1] = point.substring(point.indexOf(" ") + 1, point.length() - 1);
  int dataX = numbers[0].toInt();
  int dataY = numbers[1].toInt();
  Serial.print(dataX);
  Serial.print(" ");
  Serial.println(dataY);
  plot(dataX + 50, dataY + 50);
  delay(500);
}
