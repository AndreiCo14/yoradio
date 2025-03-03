#ifndef config_h
#define config_h
#include "Arduino.h"
#include <Ticker.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <EEPROM.h>
//#include "SD.h"
#include "options.h"
#include "rtcsupport.h"
#include "../pluginsManager/pluginsManager.h"

#define EEPROM_SIZE       768
#define EEPROM_START      500
#define EEPROM_START_IR   0
#define EEPROM_START_2    10
#ifndef BUFLEN
  #define BUFLEN            170
#endif
#define PLAYLIST_PATH     "/data/playlist.csv"
#define SSIDS_PATH        "/data/wifi.csv"
#define TMP_PATH          "/data/tmpfile.txt"
#define INDEX_PATH        "/data/index.dat"

#define PLAYLIST_SD_PATH     "/data/playlistsd.csv"
#define INDEX_SD_PATH        "/data/indexsd.dat"

#ifdef DEBUG_V
#define DBGH()       { Serial.printf("[%s:%s:%d] Heap: %d\n", __PRETTY_FUNCTION__, __FILE__, __LINE__, xPortGetFreeHeapSize()); }
#define DBGVB( ... ) { char buf[200]; sprintf( buf, __VA_ARGS__ ) ; Serial.print("[DEBUG]\t"); Serial.println(buf); }
#else
#define DBGVB( ... )
#define DBGH()
#endif
#define BOOTLOG( ... ) { char buf[120]; sprintf( buf, __VA_ARGS__ ) ; Serial.print("##[BOOT]#\t"); Serial.println(buf); }
#define EVERY_MS(x)  static uint32_t tmr; bool flag = millis() - tmr >= (x); if (flag) tmr += (x); if (flag)
#define REAL_PLAYL   getMode()==PM_WEB?PLAYLIST_PATH:PLAYLIST_SD_PATH
#define REAL_INDEX   getMode()==PM_WEB?INDEX_PATH:INDEX_SD_PATH

#define MAX_PLAY_MODE   1
#define WEATHERKEY_LENGTH 58
#define MDNS_LENGTH 24
#if SDC_CS!=255
  #define USE_SD
#endif
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  #define ESP_ARDUINO_3 1
#endif
#define CONFIG_VERSION  4

enum playMode_e      : uint8_t  { PM_WEB=0, PM_SDCARD=1 };
enum BitrateFormat { BF_UNCNOWN, BF_MP3, BF_AAC, BF_FLAC, BF_OGG, BF_WAV };

void u8fix(char *src);

struct theme_t {
  uint16_t background;
  uint16_t meta;
  uint16_t metabg;
  uint16_t metafill;
  uint16_t title1;
  uint16_t title2;
  uint16_t digit;
  uint16_t div;
  uint16_t weather;
  uint16_t vumax;
  uint16_t vumin;
  uint16_t clock;
  uint16_t clockbg;
  uint16_t seconds;
  uint16_t dow;
  uint16_t date;
  uint16_t heap;
  uint16_t buffer;
  uint16_t ip;
  uint16_t vol;
  uint16_t rssi;
  uint16_t bitrate;
  uint16_t volbarout;
  uint16_t volbarin;
  uint16_t plcurrent;
  uint16_t plcurrentbg;
  uint16_t plcurrentfill;
  uint16_t playlist[5];
};
struct config_t
{                                      //адреса EEPROM
  uint16_t  config_set; //must be 4262 //500
  uint16_t  version;//502
  uint8_t   volume;//504
  int8_t    balance;//505
  int8_t    trebble;//506
  int8_t    middle;//507
  int8_t    bass;//508
  uint16_t  lastStation;//510
  uint16_t  countStation;//512
  uint8_t   lastSSID;//514
  bool      audioinfo;//515
  uint8_t   smartstart;//516
  int8_t    tzHour;//517
  int8_t    tzMin;//518
  uint16_t  timezoneOffset;//520
  bool      vumeter;//522
  uint8_t   softapdelay;//523
  bool      flipscreen;//524
  bool      invertdisplay;//525
  bool      numplaylist;//526
  bool      fliptouch;//527
  bool      dbgtouch;//528
  bool      dspon;//529
  uint8_t   brightness;//530
  uint8_t   contrast;//531
  char      sntp1[35];//532-566
  char      sntp2[35];//567-601
  bool      showweather;//602
  char      weatherlat[10];//603-612
  char      weatherlon[10];//613-622
  char      weatherkey[WEATHERKEY_LENGTH];//623-680
  uint16_t  _reserved;//682
  uint16_t  lastSdStation;//684
  bool      sdsnuffle;//686
  uint8_t   volsteps;//687
  uint16_t  encacc;//688
  uint8_t   play_mode;  //0 WEB, 1 SD //690
  uint8_t   irtlp;//691
  bool      btnpullup;//692
  uint16_t  btnlongpress;//694
  uint16_t  btnclickticks;//696
  uint16_t  btnpressticks;//698
  bool      encpullup;//700
  bool      enchalf;//701
  bool      enc2pullup;//702
  bool      enc2half;//703
  bool      forcemono;//704
  bool      i2sinternal;//705
  bool      rotate90;//706
  bool      screensaverEnabled;//707
  uint16_t  screensaverTimeout;//709
  bool      screensaverBlank;//710
  bool      screensaverPlayingEnabled;//711
  uint16_t  screensaverPlayingTimeout;//713
  bool      screensaverPlayingBlank;//714
  char      mdnsname[24];//738
  bool      skipPlaylistUpDown;//739
};

#if IR_PIN!=255
struct ircodes_t
{
  unsigned int ir_set; //must be 4224
  uint64_t irVals[20][3];
};
#endif

struct station_t
{
  char name[BUFLEN];
  char url[BUFLEN];
  char title[BUFLEN];
  uint16_t bitrate;
  int  ovol;
};

struct neworkItem
{
  char ssid[30];
  char password[40];
};

class Config {
  public:
    config_t store;
    station_t station;
    theme_t   theme;
#if IR_PIN!=255
    int irindex;
    uint8_t irchck;
    ircodes_t ircodes;
#endif
    BitrateFormat configFmt = BF_UNCNOWN;
    neworkItem ssids[5];
    uint8_t ssidsCount;
    uint16_t sleepfor;
    uint32_t sdResumePos;
    bool     emptyFS;
    uint16_t vuThreshold;
    uint16_t screensaverTicks;
    uint16_t screensaverPlayingTicks;
    bool     isScreensaver;
  public:
    Config() {};
    //void save();
#if IR_PIN!=255
    void saveIR();
#endif
void saveSTORE();//Сохранение в EEPROM всех настроек
    void init();
    void loadTheme();
    uint8_t setVolume(uint8_t val);
    void saveVolume();
    void setTone(int8_t bass, int8_t middle, int8_t trebble);
    void setBalance(int8_t balance);
    uint8_t setLastStation(uint16_t val);
    uint8_t setCountStation(uint16_t val);
    uint8_t setLastSSID(uint8_t val);
    void setTitle(const char* title);
    void setStation(const char* station);
    bool parseCSV(const char* line, char* name, char* url, int &ovol);
    bool parseJSON(const char* line, char* name, char* url, int &ovol);
    bool parseWsCommand(const char* line, char* cmd, char* val, uint8_t cSize);
    bool parseSsid(const char* line, char* ssid, char* pass);
    void loadStation(uint16_t station);
    bool initNetwork();
    bool saveWifi();
    bool saveWifiFromNextion(const char* post);
    void setSmartStart(uint8_t ss);
    void setBitrateFormat(BitrateFormat fmt) { configFmt = fmt; }
    void initPlaylist();
    void indexPlaylist();
    #ifdef USE_SD
      void initSDPlaylist();
      void changeMode(int newmode=-1);
    #endif
    uint16_t lastStation(){
      return getMode()==PM_WEB?store.lastStation:store.lastSdStation;
    }
    void lastStation(uint16_t newstation){				//Сохранение последней станции
      if(getMode()==PM_WEB) saveValue(&store.lastStation, newstation);
      else saveValue(&store.lastSdStation, newstation);
    }
    uint8_t fillPlMenu(int from, uint8_t count, bool fromNextion=false);
    char * stationByNum(uint16_t num);
    void setTimezone(int8_t tzh, int8_t tzm);
    void setTimezoneOffset(uint16_t tzo);
    uint16_t getTimezoneOffset();
    void setBrightness(bool dosave=false);
    void setDspOn(bool dspon, bool saveval = true);
    void sleepForAfter(uint16_t sleepfor, uint16_t sleepafter=0);
    void bootInfo();
    void doSleepW();
    void setSnuffle(bool sn);
    uint8_t getMode() { return store.play_mode/* & 0b11*/; }
    void initPlaylistMode();
    void reset();
    bool spiffsCleanup();
    FS* SDPLFS(){ return _SDplaylistFS; }
    #if RTCSUPPORTED
      bool isRTCFound(){ return _rtcFound; };
    #endif
    template <typename T>
    size_t getAddr(const T *field) const {
      return (size_t)((const uint8_t *)field - (const uint8_t *)&store) + EEPROM_START;
    }
    template <typename T>
    void saveValue(T *field, const T &value, bool commit=true, bool force=false){			//Сохранение в EEPROM
Serial.print("=saveValue1 "); Serial.print(getAddr(field));
//В оригинале все раскомментировано
//      if(*field == value && !force) {Serial.println(""); return;} //Если новое значение равно старому, то выходим
      *field = value;//Присвоение нового значения (должно быть, что бы обновлялись слайдеры эквалайзера и еще может что...)
//      size_t address = getAddr(field);
///      EEPROM.put(address, value);
//Serial.print(" =WRITE "); Serial.print(value);
//      if(commit){
///        EEPROM.commit();
//Serial.println(" =EEPROM.commit=1");
//      }
    }
    void saveValue(char *field, const char *value, size_t N, bool commit=true, bool force=false) {	//Сохранение в EEPROM строк
Serial.print("=saveValue2 "); Serial.print(getAddr(field)); //Serial.print(" "); Serial.print(field); Serial.print(" "); Serial.print(value);
//В оригинале все раскомментировано
//      if (strcmp(field, value) == 0 && !force) {Serial.println(""); return;} //Если новое значение строки равно старому, то выходим
      strlcpy(field, value, N);//Присвоение нового значения (нужно ли???)
//      size_t address = getAddr(field);
//      size_t fieldlen = strlen(field);
//Serial.print(" =WRITE ");
//      for (size_t i = 0; i <=fieldlen ; i++) {
///        EEPROM.write(address + i, field[i]);
//Serial.print(field[i]);
//      }
//      if(commit){
///        EEPROM.commit();
//Serial.println(" =EEPROM.commit=2");
//      }
    }
    uint32_t getChipId(){
      uint32_t chipId = 0;
      for(int i=0; i<17; i=i+8) {
        chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
      }
      return chipId;
    }
  private:
    template <class T> int eepromWrite(int ee, const T& value);
    template <class T> int eepromRead(int ee, T& value);
    bool _bootDone;
    #if RTCSUPPORTED
      bool _rtcFound;
    #endif
    FS* _SDplaylistFS;
    void setDefaults();
    Ticker   _sleepTimer;
    static void doSleep();
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    void _setupVersion();
    void _initHW();
    bool _isFSempty();
    uint16_t _randomStation(){
      randomSeed(esp_random() ^ millis());
      uint16_t station = random(1, store.countStation);
      return station;
    }
    char _stationBuf[BUFLEN/2];
};

extern Config config;
#if DSP_HSPI || TS_HSPI || VS_HSPI
extern SPIClass  SPI2;
#endif

#endif
