/*

  Communication Board Software for Nextion HMI
  7.05.2021
  YUNUS EMRE UZMEZ

*/

/********************************************************************************/

char gelen_ham_data_bms[152];
char gelen_ham_data_chcksm_bms[3];
char gelen_dogrulanmis_data_bms[152];
int gelen_data_boyut_kontrolu_bms = 0;
String chcksm_hesaplanan_bms = "";
String gelen_chcksm_bms = "";

uint8_t calculate_checksum(uint8_t *array, uint16_t len);
void convert_2_int(int sayi, uint8_t *temp);

uint8_t chcksum_bms = 0;
int data_bms[34];

const byte numChars_bms = 152;
uint8_t receivedChars_bms[numChars_bms];                        //BMS CHECKSUM
int veri_bms[34];
boolean newData_bms = false;
int flag_bms = 0 ;
int i_bms = 0 ;
static boolean recvInProgress_bms = false;
byte bitis_bms = 0xff;

void recvWithStartEndMarkers_bms();
void showNewData_bms() ;
void debug_checksum_data_bms();


/********************************************************************************/

void nextion_yolla(String etiket, String deger);                  //NEXTION
void nextion_bitis();
void verileri_gonder();

/********************************************************************************/

char gelen_ham_data_motor_driver[17];
char gelen_ham_data_chcksm_motor_driver[3];
char gelen_dogrulanmis_data_motor_driver[17];
int gelen_data_boyut_kontrolu_motor_driver = 0;
String chcksm_hesaplanan_motor_driver = "";
String gelen_chcksm_motor_driver = "";

uint8_t chcksum_motor_driver = 0;
int data_motor_driver[2];

const byte numChars_motor_driver = 17;
uint8_t receivedChars_motor_driver[numChars_motor_driver];        //MOTOR DRIVER CHECKSUM
int veri_motor_driver[2];
boolean newData_motor_driver = false;
int flag_motor_driver = 0 ;
int i_motor_driver = 0 ;
static boolean recvInProgress_motor_driver = false;
byte bitis_motor_driver = 0xff;


void recvWithStartEndMarkers_motor_driver();
void showNewData_motor_driver() ;
void debug_checksum_data_motor_driver();


/*******************************************************************************************/

int ledState = LOW;
unsigned long previousMillis = 0;
void blink_led(unsigned long timeInterval, uint8_t led);        //BLINK
const int testLed = 39;

/*******************************************************************************************/

void send_all_data_checksum();                  //SEND DATA
int allData[36];

/*******************************************************************************************/


void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);

  pinMode(testLed, OUTPUT);
  digitalWrite(testLed, HIGH);

}

void loop() {

  recvWithStartEndMarkers_bms();
  showNewData_bms();
  //debug_checksum_data_bms();

  recvWithStartEndMarkers_motor_driver();
  showNewData_motor_driver();
  //debug_checksum_data_motor_driver();
  
  //send_all_data_checksum();
}

/********************************************************************************/

void blink_led(unsigned long timeInterval, uint8_t led) {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= timeInterval) {

    previousMillis = currentMillis;

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    digitalWrite(led, ledState);
  }

}

/********************************************************************************/

void recvWithStartEndMarkers_bms() {

  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial3.available() > 0 && newData_bms == false) {
    rc = Serial3.read();

    if (recvInProgress_bms == true) {
      if (rc != endMarker & rc != startMarker ) {
        receivedChars_bms[ndx] = rc;
        ndx++;


        if (ndx >= numChars_bms) {
          ndx = numChars_bms - 1;
        }
      }
      else {
        receivedChars_bms[ndx] = '\0';
        recvInProgress_bms = false;
        ndx = 0;
        newData_bms = true;
        i_bms = 0;
        if (rc == startMarker)
        {
          flag_bms = 1;

        }
      }
    }

    else if (rc == startMarker) {
      recvInProgress_bms = true;
    }
  }
}

/*******************************************************************************************************/


void showNewData_bms() {

  if (newData_bms == true) {

    if (flag_bms)
    {
      recvInProgress_bms = true;
      newData_bms = false ;
      flag_bms = 0;
    }

    int ham_data_kontrol = 1, ham_data_chcksm_kontrol = 0;
    int ham_data_sayac = 0, ham_data_chcksm_sayac = 0;

    for (int gelen_veri_sayac = 0; gelen_veri_sayac < sizeof(receivedChars_bms); gelen_veri_sayac++)
    {
      if (receivedChars_bms[0] == '[' && ham_data_kontrol == 1)
      {
        gelen_ham_data_bms[gelen_veri_sayac] = receivedChars_bms[gelen_veri_sayac];

        if (receivedChars_bms[gelen_veri_sayac] == ']')
        {
          ham_data_sayac = gelen_veri_sayac + 1;
          ham_data_chcksm_kontrol = 1;
        }

        if (ham_data_chcksm_kontrol == 1)
        {
          gelen_data_boyut_kontrolu_bms = gelen_veri_sayac + 1;
          if (gelen_data_boyut_kontrolu_bms == sizeof(receivedChars_bms))
          {
            gelen_data_boyut_kontrolu_bms = gelen_veri_sayac;
          }
          gelen_ham_data_chcksm_bms[gelen_veri_sayac + 1 - ham_data_sayac] = receivedChars_bms[gelen_data_boyut_kontrolu_bms];

          if (receivedChars_bms[gelen_data_boyut_kontrolu_bms] == '*')
          {
            ham_data_kontrol = 0;
            ham_data_chcksm_kontrol = 0;
            ham_data_chcksm_sayac = gelen_veri_sayac + 1 - ham_data_sayac;
          }
        }
      }
    }



    for (int chcksm_sayac = 0; chcksm_sayac < ham_data_chcksm_sayac; chcksm_sayac++)
    {
      gelen_chcksm_bms += gelen_ham_data_chcksm_bms[chcksm_sayac];
    }



    char tmp[4];
    chcksum_bms = calculate_checksum(gelen_ham_data_bms, ham_data_sayac);
    convert_2_int(chcksum_bms, tmp);
    chcksm_hesaplanan_bms  = tmp[1];
    chcksm_hesaplanan_bms += tmp[2];
    chcksm_hesaplanan_bms += tmp[3];



    if (chcksm_hesaplanan_bms == gelen_chcksm_bms)
    {
      if (gelen_ham_data_bms[0] == '[' && gelen_ham_data_bms[ham_data_sayac - 1] == ']')
      {
        for (int dogrulanmis_data_sayac = 0; dogrulanmis_data_sayac < ham_data_sayac - 2; dogrulanmis_data_sayac++)
        {
          gelen_dogrulanmis_data_bms[dogrulanmis_data_sayac] = gelen_ham_data_bms[dogrulanmis_data_sayac + 1];
        }
      }



      char* demitter = strtok(gelen_dogrulanmis_data_bms, "|");
      while (demitter != 0)
      {
        int alinan = atoi(demitter);
        data_bms[i_bms++] = alinan;
        alinan = 0;
        if (i_bms == 34)
          i_bms = 0;

        demitter = strtok(0, "|");
      }


      verileri_gonder();

      /*
      int yun = 0;
      for (yun = 0 ; yun < 34 ; yun++) {
        allData[yun] = data_motor_driver[yun];
      }*/

      /*
        int us = 0;

        for (us = 0; us < 34 ; us++) {
        Serial.print(data_bms[us]);
        if (us < 33 )
          Serial.print(" ");
        }*/

      //Serial.println();

    }

    chcksum_bms = 0;
    gelen_chcksm_bms = "";
    chcksm_hesaplanan_bms = "";
    newData_bms = false;
  }

}

/********************************************************************************************************************/

uint8_t calculate_checksum(uint8_t *array, uint16_t len) {
  uint8_t rem = 0x41;
  uint16_t  i = 0, j = 0;

  for (i = 0; i < len; i++) {

    rem = rem ^ array[i];

    for (j = 0; j < 8; j++) {

      if (rem & 0x80) {  // if leftmost (most significant) bit is set
        rem = (rem << 1) ^ 0x07;
      }

      else {
        rem = rem << 1;
      }

    }

  }

  return rem;

}

/****************************************************************************************************************/

void convert_2_int(int sayi, uint8_t *temp)
{
  uint32_t bolum, kalan;

  if (sayi < 0)
  {
    sayi = -sayi;
  }
  else if (sayi > 0) {
    sayi = sayi;
  }

  if (sayi >= 10000)
  {
    bolum = (int)sayi / 100000;
    kalan = (int)sayi % 100000;

    *temp++ = bolum + '0';

    bolum = kalan / 10000;
    kalan = kalan % 10000 ;

    *temp++ = bolum + '0';

    bolum = kalan / 1000;
    kalan = kalan % 1000;

    *temp++ = bolum + '0';

    bolum = kalan / 100;
    kalan = kalan % 100;

    *temp++ = bolum + '0';

    bolum = kalan / 10;
    kalan = kalan % 10;

    *temp++ = bolum + '0';
    *temp++ = kalan + '0';

  }

  if (sayi < 10000 && sayi >= 1000)
  {
    bolum = (int)sayi / 1000;
    kalan = (int)sayi % 1000;

    *temp++ = bolum + '0';

    bolum = kalan / 100;
    kalan = kalan % 100 ;

    *temp++ = bolum + '0';

    bolum = kalan / 10;
    kalan = kalan % 10 ;

    *temp++ = bolum + '0';
    *temp++ = kalan + '0';


  }
  if (sayi < 1000 && sayi >= 100)
  {
    *temp++ = '0';

    bolum = (int)sayi / 100;
    kalan = (int)sayi % 100;

    *temp++ = bolum + '0';

    bolum = kalan / 10;
    kalan = kalan % 10 ;

    *temp++ = bolum + '0';
    *temp++ = kalan + '0';
  }

  if (sayi < 100 && sayi >= 10)
  {

    *temp++ = '0';
    *temp++ = '0';

    bolum = (int)sayi / 10;
    kalan = (int)sayi % 10;

    *temp++ = bolum + '0' ;
    *temp++ = kalan + '0';
  }
  if (sayi < 10)
  {
    *temp++ = '0';
    *temp++ = '0';
    *temp++ = '0';
    *temp++ = bolum + '0';

  }
}

/****************************************************************************************************************/

void nextion_yolla(String etiket, String deger) {
  Serial2.print(etiket + ".val=" + deger);
  nextion_bitis();
}

/****************************************************************************************************************/


void nextion_bitis() {
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}


/****************************************************************************************************************/


void verileri_gonder() {

  nextion_yolla("n0", String(data_bms[0]));
  nextion_yolla("n1", String(data_bms[1]));
  nextion_yolla("n2", String(data_bms[2]));
  nextion_yolla("n3", String(data_bms[3]));
  nextion_yolla("n4", String(data_bms[4]));
  nextion_yolla("n5", String(data_bms[5]));
  nextion_yolla("n6", String(data_bms[6]));
  nextion_yolla("n7", String(data_bms[7]));
  nextion_yolla("n8", String(data_bms[8]));
  nextion_yolla("n9", String(data_bms[9]));
  nextion_yolla("n10", String(data_bms[10]));
  nextion_yolla("n11", String(data_bms[11]));
  nextion_yolla("n12", String(data_bms[12]));
  nextion_yolla("n13", String(data_bms[13]));
  nextion_yolla("n14", String(data_bms[14]));
  nextion_yolla("n15", String(data_bms[15]));
  nextion_yolla("n16", String(data_bms[16]));
  nextion_yolla("n17", String(data_bms[17]));
  nextion_yolla("n18", String(data_bms[18]));
  nextion_yolla("n19", String(data_bms[19]));
  nextion_yolla("n20", String(data_bms[20]));
  nextion_yolla("n21", String(data_bms[21]));
  nextion_yolla("mh", String(data_bms[22]));
  nextion_yolla("ih", String(data_bms[23]));
  nextion_yolla("g", String(data_bms[24]));
  nextion_yolla("x0", String(data_bms[25]));
  nextion_yolla("x1", String(data_bms[26]));
  nextion_yolla("x2", String(data_bms[27]));
  nextion_yolla("x3", String(data_bms[28]));
  nextion_yolla("ms", String(data_bms[29]));
  nextion_yolla("a", String(data_bms[30]));
  nextion_yolla("he", String(data_bms[31]));
  nextion_yolla("e", String(data_bms[32]));
  nextion_yolla("y", String(data_bms[33]));
  nextion_yolla("h", String(data_motor_driver[0]));
  nextion_yolla("mt", String(data_motor_driver[1]));

}


/***********************************************************************************************************************/

void debug_checksum_data_bms() {


  if (Serial3.available())  {

    String debugData = " ";

    char c = Serial3.read();

    if (c == '<') {
      debugData = Serial3.readStringUntil('>');

      Serial.println(debugData);

    }
    else {
      debugData += c;
    }

  }

}

/*********************************************************************************************************/

void debug_checksum_data_motor_driver() {

  if (Serial1.available())  {

    String debugData = " ";

    char c = Serial1.read();

    if (c == '<') {
      debugData = Serial1.readStringUntil('>');

      Serial.println(debugData);

    }
    else {
      debugData += c;
    }

  }
}

/***********************************************************************************************************/

void recvWithStartEndMarkers_motor_driver() {

  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial1.available() > 0 && newData_motor_driver == false) {
    rc = Serial1.read();

    if (recvInProgress_motor_driver == true) {
      if (rc != endMarker & rc != startMarker ) {
        receivedChars_motor_driver[ndx] = rc;
        ndx++;


        if (ndx >= numChars_motor_driver) {
          ndx = numChars_motor_driver - 1;
        }
      }
      else {
        receivedChars_motor_driver[ndx] = '\0';
        recvInProgress_motor_driver = false;
        ndx = 0;
        newData_motor_driver = true;
        i_motor_driver = 0;
        if (rc == startMarker)
        {
          flag_motor_driver = 1;

        }
      }
    }

    else if (rc == startMarker) {
      recvInProgress_motor_driver = true;
    }
  }
}

/***********************************************************************************************************/


void showNewData_motor_driver() {

  if (newData_motor_driver == true) {

    if (flag_motor_driver)
    {
      recvInProgress_motor_driver = true;
      newData_motor_driver = false ;
      flag_motor_driver = 0;
    }

    int ham_data_kontrol = 1, ham_data_chcksm_kontrol = 0;
    int ham_data_sayac = 0, ham_data_chcksm_sayac = 0;

    for (int gelen_veri_sayac = 0; gelen_veri_sayac < sizeof(receivedChars_motor_driver); gelen_veri_sayac++)
    {
      if (receivedChars_motor_driver[0] == '[' && ham_data_kontrol == 1)
      {
        gelen_ham_data_motor_driver[gelen_veri_sayac] = receivedChars_motor_driver[gelen_veri_sayac];

        if (receivedChars_motor_driver[gelen_veri_sayac] == ']')
        {
          ham_data_sayac = gelen_veri_sayac + 1;
          ham_data_chcksm_kontrol = 1;
        }

        if (ham_data_chcksm_kontrol == 1)
        {
          gelen_data_boyut_kontrolu_motor_driver = gelen_veri_sayac + 1;
          if (gelen_data_boyut_kontrolu_motor_driver == sizeof(receivedChars_motor_driver))
          {
            gelen_data_boyut_kontrolu_motor_driver = gelen_veri_sayac;
          }
          gelen_ham_data_chcksm_motor_driver[gelen_veri_sayac + 1 - ham_data_sayac] = receivedChars_motor_driver[gelen_data_boyut_kontrolu_motor_driver];

          if (receivedChars_motor_driver[gelen_data_boyut_kontrolu_motor_driver] == '*')
          {
            ham_data_kontrol = 0;
            ham_data_chcksm_kontrol = 0;
            ham_data_chcksm_sayac = gelen_veri_sayac + 1 - ham_data_sayac;
          }
        }
      }
    }



    for (int chcksm_sayac = 0; chcksm_sayac < ham_data_chcksm_sayac; chcksm_sayac++)
    {
      gelen_chcksm_motor_driver += gelen_ham_data_chcksm_motor_driver[chcksm_sayac];
    }



    char tmp[4];
    chcksum_motor_driver = calculate_checksum(gelen_ham_data_motor_driver, ham_data_sayac);
    convert_2_int(chcksum_motor_driver, tmp);
    chcksm_hesaplanan_motor_driver  = tmp[1];
    chcksm_hesaplanan_motor_driver += tmp[2];
    chcksm_hesaplanan_motor_driver += tmp[3];



    if (chcksm_hesaplanan_motor_driver == gelen_chcksm_motor_driver)
    {
      if (gelen_ham_data_motor_driver[0] == '[' && gelen_ham_data_motor_driver[ham_data_sayac - 1] == ']')
      {
        for (int dogrulanmis_data_sayac = 0; dogrulanmis_data_sayac < ham_data_sayac - 2; dogrulanmis_data_sayac++)
        {
          gelen_dogrulanmis_data_motor_driver[dogrulanmis_data_sayac] = gelen_ham_data_motor_driver[dogrulanmis_data_sayac + 1];
        }
      }



      char* demitter = strtok(gelen_dogrulanmis_data_motor_driver, "|");
      while (demitter != 0)
      {
        int alinan = atoi(demitter);
        data_motor_driver[i_motor_driver++] = alinan;
        alinan = 0;
        if (i_motor_driver == 2)
          i_motor_driver = 0;

        demitter = strtok(0, "|");
      }

      verileri_gonder();

      //allData[34] = data_motor_driver[0];
      //allData[35] = data_motor_driver[1];

      /*
        Serial.print(" ");
        int us = 0;

        for (us = 0; us < 2 ; us++) {
        Serial.print(data_motor_driver[us]);
        if (us < 1 )
          Serial.print(" ");
        }

        Serial.println(); */


    }

    chcksum_motor_driver = 0;
    gelen_chcksm_motor_driver = "";
    chcksm_hesaplanan_motor_driver = "";
    newData_motor_driver = false;
  }

}

/*******************************************************************************************************************************************/

void send_all_data_checksum() {

  int z = 0;

  for (z = 0; z < 35 ; z++ ) {
    
    Serial.print(allData[z]);
    if(z < 34)
      Serial.print(" ");
      
  }

}
