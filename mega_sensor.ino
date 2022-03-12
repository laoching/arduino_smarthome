#include <dht.h>                                                            //온습도 센서 사용을 위한 라이브러리 선언
#include <SoftwareSerial.h>                                                 //software serial을 정의하기 위한 라이브러리 선언
#include <LiquidCrystal_I2C.h>                                              //lcd를 사용하기 위한 라이브러리 선언
#include <Wire.h>                                                           //센서값을 i2c통신을 통해 arduino uno로 전달하기 위한 라이브러리 선언
#define DHT11_PIN 4                                                         //온습도 센서 핀 변수를 선언하여 4로 정의
LiquidCrystal_I2C lcd(0x27, 20, 4);                                         //lcd 사용을 위한 주소값, 규격 입력
dht DHT;                                                                    //온습도 센서 라이브러리 사용변수 선언
int cnt;                                                                    //db 정렬을 위한 카운트 선언                                                                  
byte a,b,c,d;                                                               //i2c 통신을 위한 byte형 변수 선언
float buzzer = 6;                                                           //BUZZER 핀 번호 6
float state;                                                                //불꽃감지센서 상태 변수 선언
uint8_t humi;                                                               //i2c통신을 위한 습도값 변수 선언
uint8_t temp;                                                               //i2c통신을 위한 온도값 변수 선언
uint8_t light;                                                              //i2c통신을 위한 조도값 변수 선언
uint8_t flame;                                                              //i2c통신을 위한 불꽃값 변수 선언

SoftwareSerial mySerial(12,13); //RX,TX                                     //wifi 모듈 연결 핀 시리얼 정의

String ssid = "olleh_WiFi_2776";                                            //공유기 정보
String PASSWORD = "0000001138";
String host = "121.129.10.128";

void connectWifi(){                                                         //wifi 연결을 위한 함수

  String join ="AT+CWJAP=\""+ssid+"\",\""+PASSWORD+"\"";                    //at명령어를 join변수에 저장
      
  Serial.println("Connect Wifi...");                                        //사용자에게 진행상태를 보여주기 위해 출력
  mySerial.println(join);                                                   //연결을 위해 join변수 사용
  delay(10000);                                                             //연결 시간
  if(mySerial.find("OK"))                                                   //사용자에게 진행상태를 보여주기 위해 출력
  {
    Serial.print("WIFI connect\n");                                         //사용자에게 진행상태를 보여주기 위해 출력
  }else
  {
   Serial.println("connect timeout\n");                                     //사용자에게 진행상태를 보여주기 위해 출력
  }
  delay(1000);
}
  
void httpclient(String char_input){                                         //db에 센서값을 올리기 위한 함수 선언
  delay(100);
  Serial.println("connect TCP...");                                         //사용자에게 진행상태를 보여주기 위해 출력
  mySerial.println("AT+CIPSTART=\"TCP\",\""+host+"\",8787");                //AT명령어를 통해 웹서버에 접속
  delay(500);
  if(Serial.find("ERROR")) return;                                          //에러 발생 시 ERROR 출력
  
  Serial.println("Send data...");                                           //사용자에게 진행상태를 보여주기 위해 출력
  String url=char_input;                                                    //url 변수 선언
  String cmd="GET /smarthome_sensor.php?temp="+url+" HTTP/1.0\r\n\r\n";     //smarthome_sensor.php 파일을 통해 db값 웹으로 출력하는 cmd변수 선언
  mySerial.print("AT+CIPSEND=");                                            //software serial을 통해 AT명령어 입력
  mySerial.println(cmd.length());                                           //software serial을 통해 AT명령어 길이 입력
  Serial.print("AT+CIPSEND=");                                              //AT명령어를 시리얼 모니터를 통해 출력
  Serial.println(cmd.length());                                             //AT명령어 길이를 시리얼 모니터를 통해 출력
  if(mySerial.find(">"))                                                    //정상적으로 접속되면
  {
    Serial.print(">");                                                      //사용자에게 진행상태를 보여주기 위해 출력
  }else
  {
    mySerial.println("AT+CIPCLOSE");                                        //접속 불가 시
    Serial.println("connect timeout");                                      //connect timeout출력
    delay(1000);
    return;
  }
  delay(500);
       
  mySerial.println(cmd);                                                    //software serial을 통해 위에 선언한 cmd 변수 입력
  Serial.println(cmd);                                                      //cmd 변수 출력
  delay(100);
  if(Serial.find("ERROR")) return;                                          //error 발생 시 
  mySerial.println("AT+CIPCLOSE");                                          //software serial을 통해 AT+CIPCLOSE입력하여 TCP통신 종료 
  delay(100);
  }  


void setup() { 
  Serial.begin(9600);                                                       //시리얼의 보드레이트를 9600으로 설정
  mySerial.begin(9600);                                                     //software serial의 보드레이트를 9600으로 설정
  connectWifi();                                                            //connect wifi 함수 실행
   delay(500);
  int cnt = 0;                                                              //카운트 변수인 cnt를 0으로 초기화
  pinMode(8,INPUT);                                                         // 미세먼지 센서 Digital8 연결                  
  pinMode(buzzer, OUTPUT);                                                  // BUZZER를 출력으로 설정
  pinMode(10, INPUT);                                                       // 조도센서를 입력으로 설정

  lcd.init();                                                               //lcd 초기화
  lcd.backlight();                                                          //lcd backlight on

  Wire.begin(8);                                                            // i2c통신을 위한 버스 주소를 8로 지정
  Wire.onRequest(Send);                                                     // i2c통신이 가능하면 send 함수 실행

  delay(500);
}

unsigned long pulse = 0;                                                    //미세먼지 측정값을 pulse로 나타내기 위한 변수 초기화
         double ugm3 = 0;                                                   //미세먼지 측정값 출력 변수 선언
         uint8_t dust = 0;                                                  //i2c통신을 위한 미세먼지 변수 선언
          
void loop() {
  
  //불꽃감지 + 부저
  state = analogRead(A2);                                                   //state변수에 불꽃 감지 센서값 입력받음
  flame = state;                                                            //i2c 통신을 위한 불꽃감지 센서값 변수 flame선언
  noTone(buzzer);                                                           //초기에 BUZZER를 출력하지 않기.
  if (flame<100){                                                           //불꽃감지 센서의 값이 100 이하일때(불꽃이 감지 되었을 때)
    tone(buzzer, 500, 3000);                                                //500:음의 높낮이, 3000:부저 지속 시간
    delay(100);                
  }
  else {                                                                    //불꽃감지 센서의 값이 100이상 일때 (불꽃이 감지 되지 않았을 때)    
    noTone(buzzer);                                                         //BUZZER OFF
    delay(100);  
  }
  
  // 조도
  light = digitalRead(10);                                                  //10번핀으로 들어오는 값을 light에 저장 -> 0이면 빛 인식 / 1이면 빛 없음
 

  //미세먼지
  pulse = pulseIn(8,LOW,20000);                                             //미세먼지 측정 값 pulse 형태로 확인 가능하도록 설정
       ugm3 = pulse2ugm3(pulse);
       dust = ugm3;                                                         //i2c 통신을 위한 미세먼지 변수 선언

  //온습도
  DHT.read11(DHT11_PIN);                                                    //4번핀 값을 DHT라이브러리에 입력
  humi = DHT.humidity;                                                      //humi변수에 습도값 저장
  temp = DHT.temperature;                                                   //temp변수에 온도값 저장

  //아두이노 -> db 데이터 전송
  String str_output = String(temp)+"&humi="+String(humi)+"&dust="+String(dust)+"&flame="+String(flame)+"&light="+String(light)+"&cnt="+String(cnt);
  delay(1000);
  httpclient(str_output);                                                   //httpclient함수에 str_output변수 입력
  delay(1000);
   while (mySerial.available())                                             //software serial 사용 가능 시
   {
    char response = mySerial.read();                                        //Myserial의 값을 읽어 response에 저장
    Serial.write(response);                                                 //Myserial의 값을 출력
    if(response=='\r') Serial.print('\n');                                  //response가 캐리지리턴=엔터입력이면 시리얼 모니터 한줄 뜀        
    }
   Serial.println("\n==================================\n");                //사용자 확인 쉽도록 구분 줄 출력
  delay(2000);
  //lcd 출력
  lcd.setCursor(0,0);                                                       //lcd 커서 좌표 0,0으로 설정
  lcd.print("Temperature : ");                                              //사용자에게 확인 쉽도록 출력
  lcd.print(temp);                                                          //온도값 출력
  lcd.print("C");                                                           //기호 출력
    
  lcd.setCursor(0,1);                                                       //lcd 커서 좌표 0,1으로 설정
  lcd.print("Humidity : ");                                                 //사용자에게 진행상태를 보여주기 위해 출력
  lcd.print(humi);                                                          //습도값 출력
  lcd.print("%");                                                           //기호 출력

  lcd.setCursor(0,2);                                                       //lcd 커서 좌표 0,2으로 설정
  lcd.print("light : ");                                                    //사용자에게 진행상태를 보여주기 위해 출력
  lcd.print(light);                                                         //조도센서 값 출력
  
  lcd.setCursor(0,3);                                                       //lcd 커서 좌표 0,3으로 설정
  lcd.print("dust : ");                                                     //사용자에게 진행상태를 보여주기 위해 출력
  lcd.print(dust);                                                          //미세먼지 값 출력
  lcd.print(" ug/m^3");                                                     //단위 출력
  
  cnt = cnt +1;                                                             //loop 한번 돌면 카운트변수에 1 더함
}

//센서값 아두이노 메가 -> 우노 전달
void Send(){
  byte arr[5];                                                              //byte형 변수 arr선언

  arr[0] = temp&0xFF;                                                       //0번째 = 온도값 
  arr[1] = humi&0xFF;                                                       //1번째 = 습도값 
  arr[2] = dust&0xFF;                                                       //2번째 = 먼지값
  arr[3] = light&0xFF;                                                      //3번째 = 조도값
  arr[4] = flame&0xFF;                                                      //4번째 = 불꽃감지

  Wire.write(arr,5);                                                        //i2c통신으로 배열 전송
  delay(100);
}

//미세먼지 센서 pulse 출력 부분
float pulse2ugm3(unsigned long pulse){
    float value = (pulse-1400)/14.0;                                        //데이터 시트를 참고하여 임의로 설정한 계산 식
        if(value > 300){                                                    //미세먼지 측정 값이 300을 넘기면 측정이 안되기 때문에
           value = 0;                                                       //0으로 초기화
        }
       return value;                                                        //value 출력
        }
