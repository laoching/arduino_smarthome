#include <Wire.h>                                                                   //i2c 통신을 위한 라이브러리 선언
#include <Servo.h>                                                                  //서보모터 이용을 위한 라이브러리 선언
#include <SoftwareSerial.h>                                                         //software serial을 이용하기 위한 라이브러리 선언
SoftwareSerial BTSerial(13,12);                                                     //13,12번 핀을 BTSerial이라는 software serial로 정의

byte a,b,c,d,e;                                                                     //i2c통신을 위한 byte형 변수 선언
Servo servo1;                                                                       //servo1 = 창문 서보모터
Servo servo;                                                                        //servo = 블라인드 서보모터

uint8_t temp,dust,light,humi,flame;                                                 //i2c통신을 위한 변수 선언

int windowcontrol=0;                                                                //창문 제어 변수 선언
int blindcontrol=0;                                                                 //블라인드 제어 변수 선언

int servo_motor_win=2;                                                              //창문제어 서보모터 digital 2번으로 정의
int servo_motor_bli=3;                                                              //블라인드 제어 서보모터 digital 3번으로 정의
int angle=180;                                                                      //서보모터 각 변수 180도로 초기화
char i,j,k,n;                                                                       //application과 아두이노 통신을 위한 변수 선언
 
void setup() {
  Wire.begin();                                                                     //i2c 통신을 위한 라이브러리 이용
  Serial.begin(9600);                                                               //serial 보드레이드 9600으로 선언
  BTSerial.begin(9600);                                                             //software serial 보드레이드 9600으로 선언
  servo.attach(servo_motor_win);                                                    //서보모터에 창문제어 핀번호 입력
  servo1.attach(servo_motor_bli);                                                   //서보모터에 블라인드 제어 핀번호 입력

  pinMode(5, OUTPUT);                                                               //dc 핀 5,6,7,8 output으로 지정
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
}

void loop() {
  transmit();                                                                       //i2c통신 함수 사용
  dc_motor();                                                                       //dc모터 함수 사용
  servo_motor_window();                                                             //창문제어하는 서보모터 함수 사용
  servo_motor_blind();                                                              //블라인드 제어하는 서보모터 함수 사용
  light_control();                                                                  //릴레이 모듈 통한 전구제어 함수 사용
  if (BTSerial.available()) {                                                       //software serial 이용 가능하면
    Serial.write(BTSerial.read());                                                  //BTSerial로부터 값을 읽어옴
  }
  // Serial –> Data –> BT
  if (Serial.available()) {                                                         //serial 이용 가능하면
    BTSerial.write(Serial.read());                                                  //BTSerial serial에서 값 읽어옴
 }
  delay(1000);
//블루투스를 통해 값 전송
    BTSerial.print("temperature : ");                                               //사용자에게 진행상태를 보여주기 위해 출력
    BTSerial.print(temp);                                                           //온도값 
    BTSerial.print("  ");                                                           //사용자에게 진행상태를 보여주기 위해 출력  
    BTSerial.print("humidity : ");                                                  //사용자에게 진행상태를 보여주기 위해 출력  
    BTSerial.print(humi);                                                           //습도값
    BTSerial.print("  ");                                                           //사용자에게 진행상태를 보여주기 위해 출력
    BTSerial.print("dust : ");                                                      //사용자에게 진행상태를 보여주기 위해 출력
    BTSerial.print(dust);                                                           //미세먼지값  
    BTSerial.println("  ");                                                         //사용자에게 진행상태를 보여주기 위해 출력
    if(flame<100){                                                                  //불꽃감지센서값이 100밑으로 내려가면(불꽃 감지되면)
      BTSerial.println("화재발생");                                                  //사용자에게 진행상태를 보여주기 위해 출력
      BTSerial.println("화재발생");  
      BTSerial.println("화재발생"); 
      BTSerial.println("화재발생"); 
      BTSerial.println("화재발생");       
      BTSerial.println("화재발생"); 
      BTSerial.println("화재발생"); 
    }
    delay(1000);


}

void transmit(){                                                                    //i2c통신을 위한 함수 선언
  Wire.requestFrom(8,5);                                                            //mega에서 선언한 8번 주소로 5바이트 배열 전달 받음

  a=Wire.read();                                                                    //a에 0번 배열 값 받음
  b=Wire.read();                                                                    //b에 1번 배열 값 받음
  c=Wire.read();                                                                    //c에 2번 배열 값 받음
  d=Wire.read();                                                                    //d에 3번 배열 값 받음
  e=Wire.read();                                                                    //e에 4번 배열 값 받음
  temp =a;                                                                          //온도 변수에 a변수 값 입력
  humi = b;                                                                         //습도 변수에 b변수 값 입력
  dust = c;                                                                         //미세먼지 변수에 c변수 값 입력
  light = d;                                                                        //조도 변수에 d변수 값 입력
  flame = e;                                                                        //불꽃감지 변수에 e변수 값 입력
    Serial.print("온도 : ");                                                        //사용자에게 진행상태를 보여주기 위해 출력
    Serial.print(temp);                                                             //온도 값 출력
    Serial.print("   ");                                                            //사용자에게 진행상태를 보여주기 위해 출력
    Serial.print("미세먼지 : ");                                                     //사용자에게 진행상태를 보여주기 위해 출력 
    Serial.print(dust);                                                             //미세먼지 값 출력
    Serial.print("   ");                                                            //사용자에게 진행상태를 보여주기 위해 출력  
    Serial.print("빛 감지 : ");                                                      //사용자에게 진행상태를 보여주기 위해 출력  
    Serial.println(light);                                                          //조도센서 값 출력

     delay(5000);
     delay(2000);
}
void dc_motor(){                                                                    //dc모터 함수 선언
  k = BTSerial.read();                                                              //블루투스로 읽어온 값을 k에 저장
  if(temp>=28||k=='5'){                                                             //온도가 28도 이상이거나 블루투스로 5를 전달 받으면
  digitalWrite(5, HIGH);                                                            // HIGH LOW HIGH -> 앞으로 바람 나옴
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  }
  else if(temp<28||k=='6'){                                                         //온도가 28도 미만이거나 블루투스로 6을 전달 받으면
  digitalWrite(5, LOW);                                                             //dc모터 정지 
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  }
  delay(500);
}
void servo_motor_window(){                                                          //창문 제어 서보모터 함수 선언
     j = BTSerial.read();                                                           //블루투스로 읽어온 값을 j에 저장
    if(dust>=50||j=='2'){                                                           //미세먼지농도가 50이상이거나 블루투스로 2를 전달 받으면
    angle=0;                                                                        //서보모터 0도로 회전 = 창문 닫음
    servo.write(angle);                                                             
      delay(500);
  }

  else if(dust<50||j=='1'){                                                         //미세먼지농도 50아래거나 블루투스로 1을 전달 받으면
    angle=150;                                                                      //서보모터 150도로 회전
    servo.write(angle); 
  }
    delay(500);

}
void servo_motor_blind(){                                                           //블라인드 제어 서보모터 함수 선언
   i = BTSerial.read();                                                             //블루투스로 읽어온 값을 i에 저장
   if(light==1||i=='3'){                                                            //빛 감지되거나 블루투스로 3을 전달 받으면
    angle=0;                                                                        //서보모터 0도로 회전 = 내림
    servo1.write(angle);    
      delay(500);
  }

  else if(light==0||i=='4'){                                                        //빛 없거나 블루투스로 4를 전달 받으면
    angle=180;                                                                      //서보모터 180도로 회전 = 올림
    servo1.write(angle); 
  }
     delay(500);
  
}
//릴레이 모듈 핀 8번
void light_control(){                                                               //전구 제어를 위한 함수 선언
  n = BTSerial.read();                                                              //블루투스로 읽어온 값을 n에 저장
  if(n=='7'){                                                                       //블루투스로 7을 받으면 전구 끔
  digitalWrite(8,LOW);
    }
  else if(n=='8'){                                                                  //블루투스로 8을 받으면 전구 킴
    digitalWrite(8,HIGH);
  }
  delay(500);
}


 
