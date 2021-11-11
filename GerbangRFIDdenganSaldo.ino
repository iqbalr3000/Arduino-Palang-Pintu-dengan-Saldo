//logika : jika kartu yang ditempel terdaftar/ memiliki id sama, maka sistem akan mengecek jumlah saldo yang dimiliki,
//         jika saldo yang dimiliki mencukupi maka buzzer akan berbunyi terdapat pesan berhasil pada lcd, dan palang akan terbuka lalu nilai kunci akan berubah menjadi 1,
//         setelah palang terbuka pengguna dapat melaju dan palang akan tertutup kembali dan nilai kunci akan kembali menjadi 0,
//         jika saldo kurang maka akan keluar pesan saldo kurang,
//         jika kartu yang ditempel tidak terdaftar maka akan muncul pesan error dapa lcd dan pengguna diminta menempelkan kartu yang lain,

//nilai kunci digunakan untuk mengunci agar setelah menggunakan kartu pertama tidak dapat menggunakan kartu yang lain sebelum
//pengguna kartu pertama melewati palang, kartu yang ditempelkan tidak terdaftar, atau kartu pertama tidak memiliki saldo yang cukup.

//saldo yang digunakan masih statis, sehingga jika saldo sudah habis maka program harus di restart agar saldo dapat
//kembali seperti semula, saldo dapat diatur sesuai keinginan

//menggunakan library spi, mfrc522, liquidCrystal, dan servo
#include<SPI.h>
#include<MFRC522.h>
#include<LiquidCrystal_I2C.h>
#include<Servo.h>

//inisiasi variable mfrc522
#define SS_PIN 10 //sesuaikan dengan yang di arduino
#define RST_PIN 9 //sesuaikan dengan yang di arduino

MFRC522 mfrc522(SS_PIN, RST_PIN);

//inisialisasi variable servo
int outServo = 6;
Servo myservo;

// Motor A pins
#define A1 5
#define A2 3

//inisialisasi variable lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);

//inisiasi variabel tambahan
int buzzer = 8; //sesuaikan dengan yang di arduino
int infrared = 7; //sesuaikan dengan yang di arduino
String uidString;
int kunci = 0;
int saldo = 10000; //saldo kartu pertama, ubah sesuai keinginan
int saldo2 = 10000; //saldo kartu kedua, udah sesuai keinginan

void setup() {
  //setup baudrate yang digunakan untuk komunikasi microcontroller dengan komputer
  Serial.begin(9600);
  SPI.begin();
  lcd.begin();
  mfrc522.PCD_Init();
  myservo.attach(outServo);

  //setup pin mode
  pinMode(buzzer, OUTPUT);
  pinMode(infrared, INPUT);

  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);

  //setup lcd ketika mulai
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEMPELKAN KARTU!");
}

void loop() {
  //Baca Data
  if (mfrc522.PICC_IsNewCardPresent() && kunci == 0) { //sensor membaca apakah terdapat kartu yang di tempelkan dan kunci = 0,
    readRFID();//menjalan fungsi readRFID
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TAG UID : ");
    lcd.setCursor(0, 1);
    lcd.print(uidString);

    if (uidString == "8A-37-5A-1A") { //id kartu pertama yang terdaftar | dapat diubah
      if (saldo >= 5000) {
        buzzerBunyi();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("BERHASIL!");
        delay(500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PALANG DIBUKA");
        lcd.setCursor(0, 1);
        lcd.print("SILAKAN MASUK");
        kunci = 1;
        saldo -= 5000;
        buka();
      } else {
        buzzerBunyiGagal();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Saldo Tidak Cukup!");
        delay(500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TEMPELKAN KARTU!");
      }
    } else if (uidString == "5-86-56-BF") { //id kartu kedua yang terdaftar | dapat diubah
      if (saldo2 >= 5000) {
        buzzerBunyi();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("BERHASIL!");
        delay(500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PALANG DIBUKA");
        lcd.setCursor(0, 1);
        lcd.print("SILAKAN MASUK");
        kunci = 1;
        saldo2 -= 5000;
        buka();
      } else {
        buzzerBunyiGagal();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Saldo Tidak Cukup!");
        delay(500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TEMPELKAN KARTU!");
      }
    } else {
      buzzerBunyiGagal();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GAGAL!");
      delay(500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("TEMPELKAN KARTU!");
    }

  }

  infraredNyala();//memanggil fungsi infraredNyala

}

void readRFID() {
  mfrc522.PICC_ReadCardSerial();
  //pembatas bisa dirubah sesuai keinginan, disini yang digunakan "-"
  uidString = String(mfrc522.uid.uidByte[0], HEX) + "-" +
              String(mfrc522.uid.uidByte[1], HEX) + "-" +
              String(mfrc522.uid.uidByte[2], HEX) + "-" +
              String(mfrc522.uid.uidByte[3], HEX);
  uidString.toUpperCase();
}

void buzzerBunyi() {
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(100);
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
}

void buzzerBunyiGagal() {
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
}

void infraredNyala() {
  int readInfrared = digitalRead(infrared);

  if (readInfrared == 0 && kunci == 1) {
    buzzerBunyi();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PALANG DITUTUP");
    delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TEMPELKAN KARTU!");
    kunci = 0;
    tutup();
  }

}

//function untuk buka
void buka() {
  analogWrite(A1, 0);
  analogWrite(A2, 210);
  delay(100);
  analogWrite(A2, 0);
}

//function untuk tutup
void tutup() {
  analogWrite(A1, 210);
  analogWrite(A2, 0);
  delay(100);
  analogWrite(A1, 0);
}
