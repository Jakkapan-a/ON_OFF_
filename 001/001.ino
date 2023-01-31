#include <BUTTON.h>
#include <PINOUT.h>

BUTTON BTN_Start(42);
BUTTON Selector_Sw(43);
BUTTON Diff_1(46);   // Red
BUTTON Diff_2(48);   // Orange
BUTTON Diff_3(50);   //  Yellow
BUTTON Diff_4(52);   // Green
BUTTON Diff_5(45);   // Blue
BUTTON Diff_6(47);   // Purple
BUTTON Diff_7(49);   // Gray
BUTTON Diff_8(51);   // White
BUTTON Diff_9(53);   // Black
BUTTON Diff_10(44);  // Brown

PINOUT RELAY_1(23);  // B+
PINOUT RELAY_2(25);  // ACC
PINOUT RELAY_3(27);  // Alram
PINOUT RELAY_4(29);  // Mode Auto Manual

// A0
#define LED_STATE 8
#define LED_Moving 9

unsigned long period_cs = 1000;  // Timer 1000 = 1 sce
unsigned long last_time_cs;

unsigned long period_ms = 10;  // Timer 1000 = 1 millisec
unsigned long last_time_ms;

int total_timer = 0;
int current_timer = 0;
bool isStart = false;
bool isStop = false;
int PWM = 0;
int PWM_MAX = 10;
int step_run = 0;

bool toggle = false;
// Stop
int LED_MAX = 255;
int LED_COUNT = 0;

int stop_timer = 0;
bool exit_state = false;
int exit_process = 0;
// int stop_timer_max = 10;
void time_count() {
  /// @brief 1ms timer
  if (millis() - last_time_cs > period_cs) {
    // This will only run once every 1000ms
    // Print diff
    // Serial.println("Diff: " + String(checkDiff()));
    current_timer++;
    exit_process = 0;
    if (isStart) {
      if (PWM >= PWM_MAX -1) {
        PWM = 0;
        step_run++;
        Serial.println("Current timer: " + String(current_timer) +", Total :"+String(total_timer));
        switch (step_run) {
          case 1:
            RELAY_2.on(); // ACC ON
            toggle = true;
            break;
          default:
            if (current_timer >= total_timer) {
              isStop = true;
              isStart = false;
              current_timer = 0;
              total_timer = 0;
              step_run = 0;
              stop_timer = 0;
              RELAY_2.off();  // ACC OFF
            } else {
              toggle = !toggle;
              if (toggle) {
                RELAY_2.on();
              } else {
                RELAY_2.off();
              }
            }

            break;
        }
      } else {
        PWM++;
      }
    }
    if (!isStart && isStop) {
      stop_timer++;
      if (stop_timer == 5) {
        RELAY_1.off();  // B+ OFF
        RELAY_3.on();   // Alram ON
      } else if (stop_timer >= 10) {
        RELAY_3.off();  // Alram OFF
        isStop = false;
        stop_timer = 0;
      }
    }
    //
    last_time_cs = millis();
  } else if (millis() < 1000) {
    last_time_cs = millis();
  }
  // timer 10ms
  if (millis() - last_time_ms > period_ms) {
    // This will only run once every 10ms
    if (isStart && !isStop) {
      if (toggle) {
        LED_COUNT += 5;

      } else {
        LED_COUNT -= 5;
      }

      if (LED_COUNT >= LED_MAX) {
        LED_COUNT = LED_MAX;
      }
      if(LED_COUNT <= 0){
        LED_COUNT = 0;
      }
      analogWrite(LED_STATE, LED_MAX);
      analogWrite(LED_Moving, LED_COUNT);

    } else {
      analogWrite(LED_STATE, 0);
      analogWrite(LED_Moving, 0);
    }

    last_time_ms = millis();
  } else if (millis() < 1000) {
    last_time_ms = millis();
  }
}

int checkDiff() {
  // Reserve
  if (!Diff_10.getState()) {
    return 10;
  } else if (!Diff_9.getState()) {
    return 9;
  } else if (!Diff_8.getState()) {
    return 8;
  } else if (!Diff_7.getState()) {
    return 7;
  } else if (!Diff_6.getState()) {
    return 6;
  } else if (!Diff_5.getState()) {
    return 5;
  } else if (!Diff_4.getState()) {
    return 4;
  } else if (!Diff_3.getState()) {
    return 3;
  } else if (!Diff_2.getState()) {
    return 2;
  } else if (!Diff_1.getState()) {
    return 1;
  } else {
    return 0;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  RELAY_1.off();
  RELAY_2.off();
  RELAY_3.off();
  RELAY_4.off();

  pinMode(LED_STATE, OUTPUT);
  pinMode(LED_Moving, OUTPUT);

  // Off Led state
  analogWrite(LED_STATE,0);
  analogWrite(LED_Moving, 0);
}
bool isStarted = false;
void loop() {

  if (!Selector_Sw.getState()) {
    if (BTN_Start.isPressed() && !isStart) {
      Serial.println("Start Button Pressed");
      isStart = true;
      isStarted = true;
    }
    if (isStart && isStarted) {
      total_timer = (checkDiff() * 10) * 60;
      // Serial.println("Total timer : "+ String(total_timer));
      if (total_timer == 0) {
        isStart = false;
        isStop = true;
        return;
      }
      current_timer = 0;
      isStart = true;
      isStop = false;
      step_run = 0;
      isStarted = false;
      RELAY_1.on();
    }
    RELAY_4.on();
  }else if(!isStart && !isStop) {
    RELAY_4.off();
  }else if (BTN_Start.isPressed() && exit_state && isStart) {
        exit_state = false;
        exit_process++;
        Serial.println("Stop :"+String(exit_process));
        if(exit_process >=3){
            isStop = true;
            isStart = false;
            current_timer = 0;
            total_timer = 0;
            step_run = 0;
            stop_timer = 0;
            RELAY_4.on();
            RELAY_2.off();  // ACC OFF
        }
    }else {
      exit_state =true;
    }


  time_count();
}
