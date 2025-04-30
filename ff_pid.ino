#include <PID_v1.h>

// 定義引腳
const int encoderPin = 2;//接編碼器
const int motorPin1 = 3;//控制電機方向
const int motorPin2 = 4;//控制電機方向
const int pwmPin = 5;//控制電機速度

// PID變數
double setpoint, input, output;
//目標值、(目前的)測量值、算出來的結果


// PID參數
double Kp = 0.0, Ki = 0.0, Kd = 0.0;
//自己微調，太高的數值會讓你的自走車跟吸毒一樣


// 創建PID
PID PID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// 速度和時間變數
volatile long encoderCount = 0;//計算脈衝數量
long lastTime = 0;//儲存上次計算時間
double rpm = 0.0;//儲存轉速

// 中斷服務程序(ISR)
void encoderISR() {
  encoderCount++;//每發出一個脈衝encoderCount+1
}

void setup() {
  // 初始化引腳
  pinMode(encoderPin, INPUT_PULLUP);//編碼器
  pinMode(motorPin1, OUTPUT);//電機方向
  pinMode(motorPin2, OUTPUT);//電機方向
  pinMode(pwmPin, OUTPUT);//馬達速度

  // 初始化串口
  Serial.begin(9600);//連接鮑率9600

  // 初始化編碼器中斷
  attachInterrupt(digitalPinToInterrupt(encoderPin), encoderISR, RISING);
  //有上升訊號觸發ISR

  // 設置初始值
  setpoint = 100;  // 目標轉速(可以自己改)(RPM)
  PID.SetMode(AUTOMATIC);
  PID.SetOutputLimits(0, 255);//輸出限制(0-255)
}

void loop() {
  // 計算當前轉速
  long currentTime = millis(); //獲取當前時間
  long elapsedTime = currentTime - lastTime; //現在時間-上次計算時間
  if (elapsedTime >= 100) {  // 每100毫秒更新一次(每100MS算一次，然後重設計數器和計時器)
    rpm = (encoderCount / 20.0) * 60.0;  // 根據編碼器脈衝計算轉速，20是每轉脈衝數
    encoderCount = 0;
    lastTime = currentTime;

    // 設置PID輸入值
    input = rpm;

    // 計算PID輸出
    PID.Compute();

    // 控制電機
    analogWrite(pwmPin, output);
    if (output > 0) {
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
    } else {
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
    }
    //理論上不會等於零所以我偷懶

    // 輸出當前轉速和PID輸出到串口
    Serial.print("轉速(RPM) = ");
    Serial.print(rpm);
    Serial.print(" Output = ");
    Serial.println(output);
  }
}
