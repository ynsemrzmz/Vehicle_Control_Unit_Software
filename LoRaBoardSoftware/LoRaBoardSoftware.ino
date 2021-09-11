
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
int veri_motor_driver[17];
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

void send_all_data_checksum();
void debug_send_data();
int allData[36];

char tx_buff[162];                  //SEND DATA
uint8_t checksum = 0;

void init_timer_interrupt();

uint8_t calculate_checksum_giden(uint8_t *array, uint16_t len);

/*******************************************************************************************/


void setup() {

  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(9600);
  Serial3.begin(115200);

  pinMode(testLed, OUTPUT);
  digitalWrite(testLed, HIGH);

  init_timer_interrupt();

}

void loop() {

  recvWithStartEndMarkers_bms();
  showNewData_bms();
  //debug_checksum_data_bms();

  recvWithStartEndMarkers_motor_driver();
  showNewData_motor_driver();
  //debug_checksum_data_motor_driver();


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
        }

        Serial.println();*/

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

      
      //allData[34] = data_motor_driver[0];
      //allData[35] = data_motor_driver[1];


      /*int us = 0;

        for (us = 0; us < 2 ; us++) {
        Serial.print(data_motor_driver[us]);
        if (us < 1 )
        Serial.print(" ");
        }

        Serial.println();*/


    }

    chcksum_motor_driver = 0;
    gelen_chcksm_motor_driver = "";
    chcksm_hesaplanan_motor_driver = "";
    newData_motor_driver = false;
  }

}

/*******************************************************************************************************************************************/

void send_all_data_checksum() {

  uint8_t temp_buff[4]; //Checksum için gerekli değişkenler

  tx_buff[0] = '<';
  tx_buff[1] = '[';

  convert_2_int(data_bms[0] * 10, temp_buff);
  tx_buff[2] = temp_buff[0]; //basamak basamak ayrilan sayinin 1000'ler basamagi
  tx_buff[3] = temp_buff[1]; //basamak basamak ayrilan sayinin 100'ler basamagi
  tx_buff[4] = temp_buff[2]; //basamak basamak ayrilan sayinin 10'ler basamagi              //hucre 0
  tx_buff[5] = '|'; //ayirma biti

  convert_2_int(data_bms[1] * 10, temp_buff);
  tx_buff[6] = temp_buff[0];
  tx_buff[7] = temp_buff[1];                                                      //hucre 1
  tx_buff[8] = temp_buff[2];
  tx_buff[9] = '|'; //ayirma biti

  convert_2_int(data_bms[2] * 10, temp_buff);
  tx_buff[10] = temp_buff[0];
  tx_buff[11] = temp_buff[1];                                         //hucre 2
  tx_buff[12] = temp_buff[2];
  tx_buff[13] = '|'; //ayirma biti

  convert_2_int(data_bms[3] * 10, temp_buff);
  tx_buff[14] = temp_buff[0];
  tx_buff[15] = temp_buff[1];                                         //hucre 3
  tx_buff[16] = temp_buff[2];
  tx_buff[17] = '|'; //ayirma biti

  convert_2_int(data_bms[4] * 10, temp_buff);
  tx_buff[18] = temp_buff[0];
  tx_buff[19] = temp_buff[1];
  tx_buff[20] = temp_buff[2];                                             //hucre 4
  tx_buff[21] = '|'; //ayirma biti

  convert_2_int(data_bms[5] * 10, temp_buff);
  tx_buff[22] = temp_buff[0];
  tx_buff[23] = temp_buff[1];                                         //hucre 5
  tx_buff[24] = temp_buff[2];
  tx_buff[25] = '|'; //ayirma biti

  convert_2_int(data_bms[6] * 10, temp_buff);
  tx_buff[26] = temp_buff[0];
  tx_buff[27] = temp_buff[1];
  tx_buff[28] = temp_buff[2];                                               //hucre 6
  tx_buff[29] = '|'; //ayirma biti

  convert_2_int(data_bms[7] * 10, temp_buff);
  tx_buff[30] = temp_buff[0];
  tx_buff[31] = temp_buff[1];                                     //hucre 7
  tx_buff[32] = temp_buff[2];
  tx_buff[33] = '|'; //ayirma biti

  convert_2_int(data_bms[8] * 10, temp_buff);
  tx_buff[34] = temp_buff[0];
  tx_buff[35] = temp_buff[1];
  tx_buff[36] = temp_buff[2];                                   //hucre 8
  tx_buff[37] = '|'; //ayirma biti

  convert_2_int(data_bms[9] * 10, temp_buff);
  tx_buff[38] = temp_buff[0];
  tx_buff[39] = temp_buff[1];                                     //hucre 9
  tx_buff[40] = temp_buff[2];
  tx_buff[41] = '|'; //ayirma biti

  convert_2_int(data_bms[10] * 10, temp_buff);
  tx_buff[42] = temp_buff[0];
  tx_buff[43] = temp_buff[1];
  tx_buff[44] = temp_buff[2];                                     //hucre 10
  tx_buff[45] = '|'; //ayirma biti

  convert_2_int(data_bms[11] * 10, temp_buff);
  tx_buff[46] = temp_buff[0];
  tx_buff[47] = temp_buff[1];
  tx_buff[48] = temp_buff[2];                                   //hucre 11
  tx_buff[49] = '|'; //ayirma biti

  convert_2_int(data_bms[12] * 10, temp_buff);
  tx_buff[50] = temp_buff[0];
  tx_buff[51] = temp_buff[1];
  tx_buff[52] = temp_buff[2];                                       //hucre 12
  tx_buff[53] = '|'; //ayirma biti

  convert_2_int(data_bms[13] * 10, temp_buff);
  tx_buff[54] = temp_buff[0];
  tx_buff[55] = temp_buff[1];
  tx_buff[56] = temp_buff[2];                                     //hucre 13
  tx_buff[57] = '|'; //ayirma biti

  convert_2_int(data_bms[14] * 10, temp_buff);
  tx_buff[58] = temp_buff[0];
  tx_buff[59] = temp_buff[1];
  tx_buff[60] = temp_buff[2];                                     //hucre 14
  tx_buff[61] = '|'; //ayirma biti

  convert_2_int(data_bms[15] * 10, temp_buff);
  tx_buff[62] = temp_buff[0];
  tx_buff[63] = temp_buff[1];
  tx_buff[64] = temp_buff[2];                                   //hucre 15
  tx_buff[65] = '|'; //ayirma biti

  convert_2_int(data_bms[16] * 10, temp_buff);
  tx_buff[66] = temp_buff[0];
  tx_buff[67] = temp_buff[1];
  tx_buff[68] = temp_buff[2];                                     //hucre 16
  tx_buff[69] = '|'; //ayirma biti

  convert_2_int(data_bms[17] * 10, temp_buff);
  tx_buff[70] = temp_buff[0];
  tx_buff[71] = temp_buff[1];
  tx_buff[72] = temp_buff[2];                                         //hucre 17
  tx_buff[73] = '|'; //ayirma biti

  convert_2_int(data_bms[18] * 10, temp_buff);
  tx_buff[74] = temp_buff[0];
  tx_buff[75] = temp_buff[1];                                     //hucre 18
  tx_buff[76] = temp_buff[2];
  tx_buff[77] = '|'; //ayirma biti

  convert_2_int(data_bms[19] * 10, temp_buff);
  tx_buff[78] = temp_buff[0];
  tx_buff[79] = temp_buff[1];                                   //hucre 19
  tx_buff[80] = temp_buff[2];
  tx_buff[81] = '|'; //ayirma biti

  convert_2_int(data_bms[20] * 10, temp_buff);
  tx_buff[82] = temp_buff[0];
  tx_buff[83] = temp_buff[1];                                //hucre 20
  tx_buff[84] = temp_buff[2];
  tx_buff[85] = '|'; //ayirma biti

  convert_2_int(data_bms[21] * 10, temp_buff);
  tx_buff[86] = temp_buff[0];
  tx_buff[87] = temp_buff[1];                                //hucre 21
  tx_buff[88] = temp_buff[2];
  tx_buff[89] = '|'; //ayirma biti

  convert_2_int(data_bms[22] * 10, temp_buff);
  tx_buff[90] = temp_buff[0];
  tx_buff[91] = temp_buff[1];                                  //min Hücre
  tx_buff[92] = temp_buff[2];
  tx_buff[93] = '|'; //ayirma biti

  convert_2_int(data_bms[23] * 10, temp_buff);
  tx_buff[94] = temp_buff[0];
  tx_buff[95] = temp_buff[1];                                  //max Hücre
  tx_buff[96] = temp_buff[2];
  tx_buff[97] = '|'; //ayirma biti

  convert_2_int(data_bms[24] , temp_buff);
  tx_buff[98] = temp_buff[0];
  tx_buff[99] = temp_buff[1];
  tx_buff[100] = temp_buff[2];                                  //toplamHucre
  tx_buff[101] = temp_buff[3];
  tx_buff[102] = '|'; //ayirma biti

  convert_2_int(data_bms[25], temp_buff);
  tx_buff[103] = temp_buff[0];
  tx_buff[104] = temp_buff[1];                                  //sicaklik 1
  tx_buff[105] = temp_buff[2];
  tx_buff[106] = temp_buff[3];
  tx_buff[107] = '|'; //ayirma biti

  convert_2_int(data_bms[26], temp_buff);
  tx_buff[108] = temp_buff[0];
  tx_buff[109] = temp_buff[1];
  tx_buff[110] = temp_buff[2];                                      //sicaklik 2
  tx_buff[111] = temp_buff[3];
  tx_buff[112] = '|'; //ayirma biti

  convert_2_int(data_bms[27], temp_buff);
  tx_buff[113] = temp_buff[0];
  tx_buff[114] = temp_buff[1];
  tx_buff[115] = temp_buff[2];
  tx_buff[116] = temp_buff[3];                                        //sicaklik 3
  tx_buff[117] = '|'; //ayirma biti

  convert_2_int(data_bms[28], temp_buff);
  tx_buff[118] = temp_buff[0];
  tx_buff[119] = temp_buff[1];
  tx_buff[120] = temp_buff[2];                                          //sicaklik 4
  tx_buff[121] = temp_buff[3];
  tx_buff[122] = '|'; //ayirma biti

  convert_2_int(data_bms[29], temp_buff);
  tx_buff[123] = temp_buff[0];
  tx_buff[124] = temp_buff[1];                                        //max Sicaklik
  tx_buff[125] = temp_buff[2];
  tx_buff[126] = temp_buff[3];
  tx_buff[127] = '|'; //ayirma biti

  convert_2_int(data_bms[30], temp_buff);
  tx_buff[128] = temp_buff[0];
  tx_buff[129] = temp_buff[1];                                      //akim
  tx_buff[130] = temp_buff[2];
  tx_buff[131] = temp_buff[3];
  tx_buff[132] = '|'; //ayirma biti

  convert_2_int(data_bms[31] * 10 , temp_buff);
  tx_buff[133] = temp_buff[0];                                  //harcanan enerji wh
  tx_buff[134] = temp_buff[1];
  tx_buff[135] = temp_buff[2];
  tx_buff[136] = temp_buff[3];
  tx_buff[137] = '|';

  convert_2_int(data_bms[32], temp_buff);
  tx_buff[138] = temp_buff[0];                                  //kalan enerji wh
  tx_buff[139] = temp_buff[1];
  tx_buff[140] = temp_buff[2];
  tx_buff[141] = temp_buff[3];
  tx_buff[142] = '|';

  convert_2_int(data_bms[33] , temp_buff);     //kalan enerji yüzdesi
  tx_buff[143] = temp_buff[1];
  tx_buff[144] = temp_buff[2];
  tx_buff[145] = temp_buff[3];
  tx_buff[146] = '|';

  convert_2_int(data_motor_driver[0] , temp_buff);     //hız
  tx_buff[147] = temp_buff[0];
  tx_buff[148] = temp_buff[1];
  tx_buff[149] = temp_buff[2];
  tx_buff[150] = temp_buff[3];
  tx_buff[151] = '|';

  convert_2_int(data_motor_driver[1] , temp_buff);     //motor sıcaklığı
  tx_buff[152] = temp_buff[0];
  tx_buff[153] = temp_buff[1];
  tx_buff[154] = temp_buff[2];
  tx_buff[155] = temp_buff[3];
  tx_buff[156] = ']';

  uint8_t chcksm_array[4];
  checksum = calculate_checksum_giden(tx_buff, 157);
  convert_2_int(int(checksum), chcksm_array);

  tx_buff[157] = chcksm_array[1];
  tx_buff[158] = chcksm_array[2];
  tx_buff[159] = chcksm_array[3];

  tx_buff[160] = '*'; //checksum bitiş biti
  tx_buff[161] = '>'; //bitis biti

  for (int i = 0; i < 162 ; i++)
  {
    Serial2.print(tx_buff[i]);
  }
  Serial2.println();


}

/*******************************************************************************************************************************************/

void debug_send_data() {

  Serial.print(data_bms[0]);
  Serial.print(" ");
  Serial.print(data_bms[1]);
  Serial.print(" ");
  Serial.print(data_bms[2]);
  Serial.print(" ");
  Serial.print(data_bms[3]);
  Serial.print(" ");
  Serial.print(data_bms[4]);
  Serial.print(" ");
  Serial.print(data_bms[5]);
  Serial.print(" ");
  Serial.print(data_bms[6]);
  Serial.print(" ");
  Serial.print(data_bms[7]);
  Serial.print(" ");
  Serial.print(data_bms[8]);
  Serial.print(" ");
  Serial.print(data_bms[9]);
  Serial.print(" ");
  Serial.print(data_bms[10]);
  Serial.print(" ");
  Serial.print(data_bms[11]);
  Serial.print(" ");
  Serial.print(data_bms[12]);
  Serial.print(" ");
  Serial.print(data_bms[13]);
  Serial.print(" ");
  Serial.print(data_bms[14]);
  Serial.print(" ");
  Serial.print(data_bms[15]);
  Serial.print(" ");
  Serial.print(data_bms[16]);
  Serial.print(" ");
  Serial.print(data_bms[17]);
  Serial.print(" ");
  Serial.print(data_bms[18]);
  Serial.print(" ");
  Serial.print(data_bms[19]);
  Serial.print(" ");
  Serial.print(data_bms[20]);
  Serial.print(" ");
  Serial.print(data_bms[21]);
  Serial.print(" ");
  Serial.print(data_bms[22]);
  Serial.print(" ");
  Serial.print(data_bms[23]);
  Serial.print(" ");
  Serial.print(data_bms[24]);
  Serial.print(" ");
  Serial.print(data_bms[25]);
  Serial.print(" ");
  Serial.print(data_bms[26]);
  Serial.print(" ");
  Serial.print(data_bms[27]);
  Serial.print(" ");
  Serial.print(data_bms[28]);
  Serial.print(" ");
  Serial.print(data_bms[29]);
  Serial.print(" ");
  Serial.print(data_bms[30]);
  Serial.print(" ");
  Serial.print(data_bms[31]);
  Serial.print(" ");
  Serial.print(data_bms[32]);
  Serial.print(" ");
  Serial.print(data_bms[33]);
  Serial.print(" ");
  Serial.print(data_motor_driver[0]);
  Serial.print(" ");
  Serial.print(data_motor_driver[1]);
  Serial.print('\n');

}

/*******************************************************************************************************************************************/

void init_timer_interrupt() {

  //set timer4 interrupt at 1Hz
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR4A = 15624 / 1; // = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR4B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE4A);

  sei();//allow interrupts

}

/*******************************************************************************************************************************************/

ISR(TIMER4_COMPA_vect) {

  //String data = "<[372|371|371|371|369|371|369|371|369|371|375|371|369|374|367|371|371|368|371|372|367|371|375|367|8164|2785|2737|2785|2688|2785|0024|0000|2300|100|3445|4141]194*>";
  send_all_data_checksum();
  //Serial.print(data);
  //Serial.println();
}
//084
/*******************************************************************************************************************************************/

uint8_t calculate_checksum_giden(uint8_t *array, uint16_t len) {
  uint8_t rem = 0x41;
  uint16_t  i = 1, j = 0;

  for (i = 1; i < len; i++) {

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
