#include <Keyboard.h>
#include <Mouse.h>

const int SERIAL_BAUD_RATE = 9600;

struct Point {
  int x;
  int y;
};

void generateCurvedPath(Point* curvedPath, const Point& startPoint, const Point& endPoint, int numSteps, int minControlDistance, int maxControlDistance) {
  // Calculate control points for the Bezier curve with random signs
  int controlDistanceX = random(minControlDistance, maxControlDistance);
  int controlDistanceY = random(minControlDistance, maxControlDistance);

  int xControl = (startPoint.x + endPoint.x) / random(2, 3) + controlDistanceX;
  int yControl = (startPoint.y + endPoint.y) / random(2, 3) + controlDistanceY;

  for (int i = 0; i <= numSteps; i++) {
    float t = static_cast<float>(i) / numSteps;
    float u = 1 - t;

    // Calculate Bezier curve coordinates
    curvedPath[i].x = u * u * startPoint.x + 2 * u * t * xControl + t * t * endPoint.x;
    curvedPath[i].y = u * u * startPoint.y + 2 * u * t * yControl + t * t * endPoint.y;
  }
}

void moveLikeHuman(int xTarget, int yTarget, int numSteps, int minDelay, int maxDelay, int minControlDistance, int maxControlDistance, int offset) {
  int numRandomPoints = random(1, 3) + 1;  // random(1, 3) Randomly determine the number of random points between 1 and 3
  Point path[numRandomPoints + 1];         // Array to store the path from (0, 0) to the target (including random points)

  path[0] = { 0, 0 };  // Set the target coordinates as the last point in the path
  // Generate random points on the straight line from (0, 0) to the target coordinates
  for (int i = 1; i < numRandomPoints; i++) {
    float t = static_cast<float>(i + 1) / (numRandomPoints + 1);  // Calculate the parameter 't' for interpolation

    // Interpolate x and y coordinates based on 't'
    int x = static_cast<int>(t * xTarget);
    int y = static_cast<int>(t * yTarget);

    // Add random offset within the specified range
    x += random(-offset, offset);
    y += random(-offset, offset);

    Point point = { x, y };
    path[i] = point;
  }

  path[numRandomPoints] = { xTarget, yTarget };  // Set the target coordinates as the last point in the path

  // Sort the random points in ascending order based on the absolute values of their coordinates
  for (int i = 0; i < numRandomPoints - 1; i++) {
    for (int j = 0; j < numRandomPoints - i - 1; j++) {
      if (abs(path[j].x) + abs(path[j].y) > abs(path[j + 1].x) + abs(path[j + 1].y)) {
        Point temp = path[j];
        path[j] = path[j + 1];
        path[j + 1] = temp;
      }
    }
  }

  // Generate curved paths between the sorted random points
  for (int i = 0; i < numRandomPoints; i++) {
    Point startPoint = path[i];
    Point endPoint = path[i + 1];

    // Calculate the distance between the start and end points in both x and y directions
    float distanceX = endPoint.x - startPoint.x;
    float distanceY = endPoint.y - startPoint.y;

    // Calculate the number of steps based on the maximum distance in either x or y direction
    int curvedNumSteps = static_cast<int>(numSteps * (max(abs(distanceX), abs(distanceY)) / max(abs(xTarget), abs(yTarget))));

    // Generate curved path between the start and end points
    Point curvedPath[curvedNumSteps + 1];

    minControlDistance = minControlDistance * -1;
    maxControlDistance = maxControlDistance * -1;

    generateCurvedPath(curvedPath, startPoint, endPoint, curvedNumSteps, minControlDistance, maxControlDistance);

    // Move the mouse step by step along the curved path
    for (int j = 0; j < curvedNumSteps; j++) {
      int deltaX = curvedPath[j + 1].x - curvedPath[j].x;
      int deltaY = curvedPath[j + 1].y - curvedPath[j].y;

      Mouse.move(deltaX, deltaY);
      int delayTime = random(minDelay, maxDelay);
      delay(delayTime);
    }
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  randomSeed(analogRead(0));  // Seed the random number generator
  Keyboard.begin();
  Mouse.begin();
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    delay(10);

    char action = data.charAt(0);

    if (action == '1') {  // sendKey 1,key,minDelay,maxDelay
      data.remove(0, 2);  // Remove the action identifier (1,) from the data string
      int commaIndex1 = data.indexOf(',');
      int commaIndex2 = data.indexOf(',', commaIndex1 + 1);

      if (commaIndex1 != -1 && commaIndex2 != -1) {
        String codeKeyString = data.substring(0, commaIndex1);
        String minDelayString = data.substring(commaIndex1 + 1, commaIndex2);
        String maxDelayString = data.substring(commaIndex2 + 1);

        int codeKey = codeKeyString.toInt();
        int minDelay = minDelayString.toInt();
        int maxDelay = maxDelayString.toInt();

        Keyboard.press(codeKey);
        delay(random(minDelay, maxDelay));
        Keyboard.release(codeKey);
      }
    }

    if (action == '2') {  // toggleKey 2,key,type,minDelay,maxDelay
      data.remove(0, 2);  // Remove the action identifier (5,) from the data string
      int commaIndex1 = data.indexOf(',');
      int commaIndex2 = data.indexOf(',', commaIndex1 + 1);
      int commaIndex3 = data.indexOf(',', commaIndex2 + 1);

      if (commaIndex1 != -1 && commaIndex2 != -1 && commaIndex3 != -1) {
        String codeKeyString = data.substring(0, commaIndex1);
        String typeString = data.substring(commaIndex1 + 1, commaIndex2);
        String minDelayString = data.substring(commaIndex2 + 1, commaIndex3);
        String maxDelayString = data.substring(commaIndex3 + 1);

        int codeKey = codeKeyString.toInt();
        bool type = typeString.toInt() != 0;
        int minDelay = minDelayString.toInt();
        int maxDelay = maxDelayString.toInt();

        if (type) {
          Keyboard.press(codeKey);
          delay(random(minDelay, maxDelay));
        } else {
          Keyboard.release(codeKey);
          delay(random(minDelay, maxDelay));
        }
      }
    }

    if (action == '3') {  // printKeys 3,text,delay,delay
      data.remove(0, 2);  // Remove the action identifier (3,) from the data string
      int commaIndex1 = data.indexOf(',');
      int commaIndex2 = data.indexOf(',', commaIndex1 + 1);
      if (commaIndex1 != -1 && commaIndex2 != -1) {
        String text = data.substring(0, commaIndex1);
        String minDelayString = data.substring(commaIndex1 + 1, commaIndex2);
        String maxDelayString = data.substring(commaIndex2);

        int minDelay = minDelayString.toInt();
        int maxDelay = maxDelayString.toInt();

        for (int i = 0; i < text.length(); i++) {
          char character = text.charAt(i);
          int keyCode = static_cast<int>(character);
          Keyboard.press(keyCode);
          delay(random(minDelay, maxDelay));
          Keyboard.release(keyCode);
          if (random(0, 1) == 0) {
            delay(random(50, 300));  // sometimes we think
          }
        }
      }
    }

    if (action == '4') {  // mouse.click 4,button,minDelay,maxDelay
      data.remove(0, 2);  // Remove the action identifier (5,) from the data string
      int commaIndex1 = data.indexOf(',');
      int commaIndex2 = data.indexOf(',', commaIndex1 + 1);

      if (commaIndex1 != -1 && commaIndex2 != -1) {
        String buttonString = data.substring(0, commaIndex1);
        String minDelayString = data.substring(commaIndex1 + 1, commaIndex2);
        String maxDelayString = data.substring(commaIndex2 + 1);

        int button = buttonString.toInt();
        int minDelay = minDelayString.toInt();
        int maxDelay = maxDelayString.toInt();

        Mouse.press(button);
        delay(random(minDelay, maxDelay));
        Mouse.release(button);
      }
    }


    if (action == '5') {  // mouse.toggle 5,button,type,delay1,delay2
      data.remove(0, 2);  // Remove the action identifier (5,) from the data string
      int commaIndex1 = data.indexOf(',');
      int commaIndex2 = data.indexOf(',', commaIndex1 + 1);
      int commaIndex3 = data.indexOf(',', commaIndex2 + 1);

      if (commaIndex1 != -1 && commaIndex2 != -1 && commaIndex3 != -1) {
        String buttonString = data.substring(0, commaIndex1);
        String typeString = data.substring(commaIndex1 + 1, commaIndex2);
        String minDelayString = data.substring(commaIndex2 + 1, commaIndex3);
        String maxDelayString = data.substring(commaIndex3 + 1);

        int button = buttonString.toInt();
        bool type = typeString.toInt() != 0;
        int minDelay = minDelayString.toInt();
        int maxDelay = maxDelayString.toInt();

        if (type) {
          Mouse.press(button);
          delay(random(minDelay, maxDelay));
        } else {
          Mouse.release(button);
          delay(random(minDelay, maxDelay));
        }
      }
    }

    if (action == '6') {  // mouse.moveTo 6,x,y
      data.remove(0, 2);  // Remove the action identifier (6,) from the data string
      int commaIndex1 = data.indexOf(',');
      if (commaIndex1 != -1) {
        String xString = data.substring(0, commaIndex1);
        String yString = data.substring(commaIndex1 + 1);

        int xTarget = xString.toInt();
        int yTarget = yString.toInt();

        Mouse.move(xTarget, yTarget, 100);
        delay(100);
      }
    }

    if (action == '7') {  // mouse.humanMoveTo 7,x,y,numSteps,minDelay,maxDelay,minControlDistance,maxControlDistance,offset
      data.remove(0, 2);  // Remove the action identifier (7,) from the data string

      int commaIndex1 = data.indexOf(',');
      int commaIndex2 = data.indexOf(',', commaIndex1 + 1);
      int commaIndex3 = data.indexOf(',', commaIndex2 + 1);
      int commaIndex4 = data.indexOf(',', commaIndex3 + 1);
      int commaIndex5 = data.indexOf(',', commaIndex4 + 1);
      int commaIndex6 = data.indexOf(',', commaIndex5 + 1);
      int commaIndex7 = data.indexOf(',', commaIndex6 + 1);

      if (commaIndex1 != -1 && commaIndex2 != -1 && commaIndex3 != -1 && commaIndex4 != -1 && commaIndex5 != -1 && commaIndex6 != -1 && commaIndex7 != -1) {
        String xString = data.substring(0, commaIndex1);
        String yString = data.substring(commaIndex1 + 1, commaIndex2);
        String stepsString = data.substring(commaIndex2 + 1, commaIndex3);
        String minDelayString = data.substring(commaIndex3 + 1, commaIndex4);
        String maxDelayString = data.substring(commaIndex4 + 1, commaIndex5);
        String minContolDistanceString = data.substring(commaIndex5 + 1, commaIndex6);
        String maxContolDistanceString = data.substring(commaIndex6 + 1, commaIndex7);
        String offsetString = data.substring(commaIndex7);

        int xTarget = xString.toInt();
        int yTarget = yString.toInt();
        int numSteps = stepsString.toInt();
        int minDelay = minDelayString.toInt();
        int maxDelay = maxDelayString.toInt();
        int minControlDistance = minContolDistanceString.toInt();
        int maxControlDistance = maxContolDistanceString.toInt();
        int offset = offsetString.toInt();

        moveLikeHuman(xTarget, yTarget, numSteps, minDelay, maxDelay, minControlDistance, maxControlDistance, offset);
      }
    }

   if (action == '8') {  // scroll 8,direction,steps && indices[1] != -1
     data.remove(0, 2);  // Remove the action identifier (8,) from the data string

     int commaIndex1 = data.indexOf(',');
     int commaIndex2 = data.indexOf(',', commaIndex1 + 1);

     int direction = data.substring(0, commaIndex1).toInt();
     int steps = data.substring(commaIndex1 + 1, commaIndex2).toInt();

     int scrollValue = direction == 0 ? -1 : 1;

     for (int i = 0; i < steps; i++) {
       Mouse.move(0, 0, scrollValue);
       delay(random(50, 150));
     }
   }

    Serial.print("ready");
  }
}
