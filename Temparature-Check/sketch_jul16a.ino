#include <LiquidCrystal_I2C.h>

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#define LED 13 
#define haha 4
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
LiquidCrystal_I2C lcd(0x27,16,2);
Servo myservo1;  
Servo myservo2;
int pos1, pos2, pirstate;
float temp;
void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  myservo1.attach(7);  
  myservo2.attach(8); 
  myservo1.write(90);
  myservo2.write(90);
  Serial.println("Arduino MLX90614 Testing");  
  mlx.begin();  

  lcd.begin();
  lcd.clear();
  lcd.backlight();
}
 
void loop() {
  int motion = digitalRead(haha); 
  
  if(motion == 1 && pirstate == 0 ){
    pirstate = 1;
    delay(100);
    temp = mlx.readObjectTempF()+9.0;
    Serial.println("MOTION DETECTED");
    Serial.print("Body Temperature = "); Serial.print(temp); Serial.println("*F");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("MOTION DETECTED!");
    lcd.setCursor(1,1);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print(" *F");
  
  if(temp<100&&temp>95){
    digitalWrite(LED, LOW);
    
    for(pos1 = 90; pos1 <= 180; pos1 += 1)
          {
//           lcd.clear();
//           lcd.setCursor(2,0);
//           lcd.print(temp);
//           lcd.setCursor(2,1);
//           lcd.print(" ok ");                                  
            myservo1.write(pos1);
            myservo2.write(180-pos1);         
            delay(15);
          }
            delay(5000);

           for(pos1 = 180; pos1>=90; pos1 -=1)
           {
            myservo1.write(pos1);
            myservo2.write(180-pos1);     
            delay(15);                     
           }
           
    }
    
   else if(temp<=95){
    digitalWrite(LED, LOW);
   }
   else{
    digitalWrite(LED, HIGH);
//    lcd.clear();
//    lcd.setCursor(2,0);
//    lcd.print(temp);
//    lcd.setCursor(2,1);
//    lcd.print("Keep out!");
//    lcd.setCursor(0,1);

      //lcd.clear();
//      lcd.setCursor(1,0);
//      lcd.print("Temp: ");
//      lcd.print(mlx.readObjectTempF()+9);
//      lcd.print(" *F");
      lcd.setCursor(0,0);
      lcd.print(" Access Denied! ");
   }
  Serial.println();
  delay(50);
  
  }
  else if(motion == 0){
    if(pirstate == 1){
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print(" *F");
    lcd.clear();
    lcd.setCursor(2,1);
    lcd.print("Not detected");
    
    Serial.println("MOTION ENDED");
    
    pirstate = 0;
    digitalWrite(LED, LOW);
    }
  }
}
