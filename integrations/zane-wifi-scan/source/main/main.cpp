/*
 * ZANE systems Kft. - Sticky Wi-Fi Tools
 * Seeed reTerminal Sticky (ESP32-S3, SSD1677 3.97" ePaper)
 *
 * Lapozhato menu, 7 eszkoz:
 *   1  Wi-Fi scanner    (SSID/BSSID/RSSI/CH/titkositas/rejtett/gyarto/eszleles)
 *   2  Channel analyzer (2.4 GHz oszlopdiagram, fekvo nezet)
 *   3  AP inspector     (beacon keret 802.11 mezoi)
 *   4  Packet monitor   (promiscuous mod, kerettipus-statisztika)
 *   5  Client discovery (egy kivalasztott AP kliensei, fix csatornan)
 *   6  Probe analyzer   (kliensek probe requestjei, hatterben csatorna-ugralassal)
 *   7  RSSI meter       (egy kivalasztott AP terero-meroje, sav + min/max/avg + history)
 *
 * Minden kepernyo-felirat ANGOL. A legkisebb hasznalt betumeret 9 pt
 * (FreeSans9pt7b) - a beepitett apro font (setTextSize) sehol nincs hasznalva.
 *
 * A kijelzo- es energiakezelo reteg a bevalt WiFi-scanner sketchbol szarmazik:
 *   - canvas a panel NATIV 800x480 elrendezeseben, setRotation(1) portrehoz
 *   - direkt SSD1677 driver, gate limit 479, horizontalis tukrozes a transzferben
 *   - 1 = feher (PAPER), 0 = fekete (INK)
 *   - tapzar (PWR_HOLD/LOCK) + deep sleep EXT1 ebresztessel (OK/DOWN)
 *
 * Arduino IDE: Board ESP32S3 Dev Module, USB CDC On Boot = DISABLED,
 *              PSRAM OPI, arduino-esp32 3.x (ledcAttach miatt).
 */

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include "esp_wifi.h"

#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include <driver/gpio.h>

#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

// Betuk: a legkisebb 9 pt (>= 9 a kovetelmeny)
#define F_SMALL  (&FreeSans9pt7b)
#define F_SMALLB (&FreeSansBold9pt7b)
#define F_MED    (&FreeSans12pt7b)
#define F_MEDB   (&FreeSansBold12pt7b)

// ============================================================
//  Pinout (reTerminal Sticky)
// ============================================================
#define PIN_PWR_HOLD   45
#define PIN_PWR_LOCK   46
#define PIN_CHG_EN     39   // aktiv alacsony

#define PIN_SPI_SCK    13
#define PIN_SPI_MOSI   14
#define PIN_SPI_MISO   12

#define EPD_CS         15
#define EPD_DC         16
#define EPD_RST        17
#define EPD_BUSY       18
#define EPD_PWR_EN     47

#define PIN_BTN_UP      5
#define PIN_BTN_DOWN    6
#define PIN_BTN_OK      4
#define PIN_BUZZER     48

// I2C (BQ27220 uzemanyagmero)
#define PIN_I2C_SCL     0
#define PIN_I2C_SDA     1
#define BQ27220_ADDR        0x55
#define BQ27220_REG_VOLTAGE 0x08   // akku feszultseg, mV
#define BQ27220_REG_SOC     0x2C   // toltottseg, % (adatlap-revizionkent elterhet -> voltage fallback)

// ============================================================
//  Kijelzo - a panel NATIV elrendezese
// ============================================================
#define EPD_WIDTH       800
#define EPD_HEIGHT      480
#define BYTES_PER_LINE  (EPD_WIDTH / 8)
#define EPD_SPI_FREQ    10000000UL
#define CANVAS_ROTATION 1

// A Channels lap fekvo nezetben. A 2-es ertekkel az eszkozt balra kell
// forditani (teteje balra). Ha megis forditva allna, valts 0-ra.
#define CH_ROTATION 2

GFXcanvas1  canvas(EPD_WIDTH, EPD_HEIGHT);
SPISettings epdSPI(EPD_SPI_FREQ, MSBFIRST, SPI_MODE0);

#define INK   0     // fekete tinta
#define PAPER 1     // feher papir
#define CANVAS_W 480
#define CANVAS_H 800

// Reszleges (partial) frissites - csak az RSSI meter lap hasznalja.
// Ezek a HARDVEREN HANGOLANDO ertekek, ha ghosting/nem-frissul gond van:
//   EPD_PARTIAL_MODE:   a 0x22 (Display Update) erteke differencialis modhoz (probald: 0xFF / 0xCF / 0xC7)
//   EPD_PARTIAL_BORDER: a 0x3C (border waveform) erteke partial modhoz
#define EPD_PARTIAL_MODE   0xFF
#define EPD_PARTIAL_BORDER 0x80

// ============================================================
//  Allapotgep + tipusok
// ============================================================
enum Screen { SCR_MENU, SCR_SCANNER, SCR_CHANNELS, SCR_INSPECTOR, SCR_MONITOR, SCR_CLIENTS, SCR_PROBES, SCR_RSSI };
enum WifiMode { WM_OFF, WM_STA_IDLE, WM_PROMISC };
enum BtnEvent { EV_NONE, EV_UP, EV_DOWN, EV_OK, EV_OK_LONG, EV_CHORD };

Screen   screen    = SCR_MENU;
WifiMode wifiState = WM_OFF;

const char* MENU_ITEMS[] = { "Wi-Fi scanner", "Channel analyzer",
                             "AP inspector", "Packet monitor", "Client discovery",
                             "Probe analyzer", "RSSI meter" };
const int MENU_N = 7;

RTC_DATA_ATTR int     menuSel  = 0;
RTC_DATA_ATTR uint8_t sortMode = 0;   // 0 = RSSI, 1 = channel

// ============================================================
//  Szkennelt AP-k
// ============================================================
struct ApInfo {
  char    ssid[33];
  uint8_t bssid[6];
  int8_t  rssi;
  uint8_t channel;
  uint8_t enc;
  bool    hidden;
  bool    present;      // szerepelt-e a legutobbi szkennelesben
  uint32_t firstSeen;   // millis() az elso eszleleskor
  uint32_t lastSeen;    // millis() a legutobbi eszleleskor
};
#define MAX_AP 48
ApInfo aps[MAX_AP];
int    apCount    = 0;
int    scanPage   = 0;
int    inspSel    = 0;

// ============================================================
//  Csomag-monitor allapot
// ============================================================
volatile uint32_t cTotal, cMgmt, cCtrl, cData, cBeacon, cProbeReq, cProbeResp, cDeauth;

struct LastFrame { uint8_t src[6]; uint8_t bssid[6]; int8_t rssi; uint8_t ch; uint8_t ftype; uint8_t fsub; };
LastFrame ring[8];
uint8_t   ringHead = 0;

int      curCh   = 1;
bool     hopOn   = true;
uint32_t lastHop = 0, lastRefresh = 0;
const uint32_t REFRESH_MS = 8000;   // periodikus kijelzo-frissites (monitor, kliens, probe)
const uint32_t HOP_MS     = 300;

volatile bool     capWanted = false, capDone = false;
uint8_t           capBssid[6];
uint8_t           capBuf[400];
volatile uint16_t capLen  = 0;
volatile int8_t   capRssi = 0;

// ---- Kliens-felderito allapot ----
#define MAX_CLIENTS 32
struct ClientInfo { uint8_t mac[6]; int8_t rssi; uint32_t frames; uint32_t lastSeen; };
ClientInfo clients[MAX_CLIENTS];
int      clientCount = 0;
uint8_t  cliBssid[6];               // a kivalasztott AP BSSID-je
uint8_t  cliChannel = 1;            // a kivalasztott AP csatornaja
volatile bool cliActive = false;    // a sniffer gyujtson-e klienseket
bool     cliPicking = true;         // valaszto nezet (true) vagy kliens nezet (false)
int      cliSel  = 0;               // valaszto: kijelolt AP
int      cliPage = 0;               // kliens nezet: lap

// ---- Probe request analyzer allapot ----
#define MAX_PROBERS 32
struct ProbeClient {
  uint8_t  mac[6];
  int8_t   rssi;
  uint32_t count;       // osszes probe
  uint32_t broadcast;   // broadcast probe (ures SSID)
  uint32_t directed;    // directed probe (konkret SSID)
  char     lastSsid[33];// legutobb kert SSID (directed)
  uint32_t lastSeen;
};
ProbeClient probers[MAX_PROBERS];
int      proberCount = 0;
volatile bool     probeActive = false;
volatile uint32_t probeTotal  = 0;    // osszes probe (rata-szamitashoz)
int      probePage = 0;
uint32_t probeWindowBase = 0, probeWindowStart = 0;
int      probeRate = 0;               // probe/sec (a legutobbi frissitesi ablakbol)

// ---- RSSI meter allapot ----
#define RSSI_MIN_DBM (-90)            // sav/grafikon also hatar
#define RSSI_MAX_DBM (-30)            // sav/grafikon felso hatar
#define RSSI_H10  50                  // 10 s history @ 200 ms
#define RSSI_H60  60                  // 60 s history @ 1 s
uint8_t  rssiBssid[6];                // mert AP BSSID-je
uint8_t  rssiChannel = 1;
char     rssiSsid[33] = {0};
volatile bool     rssiActive = false;
volatile int8_t   rssiLast   = 0;     // legutobbi minta (a sniffer irja)
volatile uint32_t rssiLastMs = 0;     // legutobbi minta ideje
int8_t   rssiHist10[RSSI_H10]; int rssiH10N = 0, rssiH10Head = 0;
int8_t   rssiHist60[RSSI_H60]; int rssiH60N = 0, rssiH60Head = 0;
uint32_t rssiSamp10 = 0, rssiSamp60 = 0;
uint8_t  rssiWindow  = 0;             // 0 = 10 s, 1 = 1 min
bool     rssiPicking = true;
int      rssiSel = 0;
// Reszleges (villanasmentes) frissites utemezese a mero nezethez
#define RSSI_FAST_MS    800    // reszleges frissites gyakorisaga (ms)
#define RSSI_FULL_EVERY 20     // ennyi reszleges utan egy teljes (ghost-tisztitas)
uint32_t rssiLastDraw  = 0;
const uint32_t RSSI_REFRESH_MS = 1000;   // RSSI meter: gyors reszleges frissites (nem a 8s-es kozos)

// ============================================================
//  Energiakezeles
// ============================================================
#define AWAKE_WINDOW_MS 60000UL   // 1 perc az utolso gombnyomastol
uint32_t lastActivity = 0;

void holdSystemPower(){
  pinMode(PIN_PWR_HOLD, OUTPUT); pinMode(PIN_PWR_LOCK, OUTPUT);
  digitalWrite(PIN_PWR_HOLD, HIGH); digitalWrite(PIN_PWR_LOCK, HIGH);
  pinMode(PIN_CHG_EN, OUTPUT); digitalWrite(PIN_CHG_EN, LOW);
}
void releasePinHolds(){
  gpio_deep_sleep_hold_dis();
  gpio_hold_dis((gpio_num_t)PIN_PWR_HOLD);
  gpio_hold_dis((gpio_num_t)PIN_PWR_LOCK);
  gpio_hold_dis((gpio_num_t)PIN_CHG_EN);
  gpio_hold_dis((gpio_num_t)EPD_PWR_EN);
}

// ============================================================
//  Buzzer
// ============================================================
void beep(uint16_t ms = 30){
  ledcWriteTone(PIN_BUZZER, 2500);
  delay(ms);
  ledcWriteTone(PIN_BUZZER, 0);
}

// ============================================================
//  SSD1677 alacsony szint
// ============================================================
void epdCommand(uint8_t cmd){
  SPI.beginTransaction(epdSPI);
  digitalWrite(EPD_DC, LOW); digitalWrite(EPD_CS, LOW);
  SPI.transfer(cmd);
  digitalWrite(EPD_CS, HIGH);
  SPI.endTransaction();
}
void epdData(uint8_t data){
  SPI.beginTransaction(epdSPI);
  digitalWrite(EPD_DC, HIGH); digitalWrite(EPD_CS, LOW);
  SPI.transfer(data);
  digitalWrite(EPD_CS, HIGH);
  SPI.endTransaction();
}
bool epdWaitReady(uint32_t timeout = 50000){
  uint32_t start = millis();
  while(digitalRead(EPD_BUSY) == HIGH){
    if(millis() - start > timeout) return false;
    delay(10);
  }
  return true;
}
void epdReset(){
  digitalWrite(EPD_RST, HIGH); delay(20);
  digitalWrite(EPD_RST, LOW);  delay(2);
  digitalWrite(EPD_RST, HIGH); delay(20);
}
void epdSetWindow(uint16_t xS, uint16_t yS, uint16_t xE, uint16_t yE){
  epdCommand(0x44); epdData(xS & 0xFF); epdData((xS >> 8) & 0x03); epdData(xE & 0xFF); epdData((xE >> 8) & 0x03);
  epdCommand(0x45); epdData(yS & 0xFF); epdData((yS >> 8) & 0x03); epdData(yE & 0xFF); epdData((yE >> 8) & 0x03);
}
void epdSetCursor(uint16_t x, uint16_t y){
  epdCommand(0x4E); epdData(x & 0xFF); epdData((x >> 8) & 0x03);
  epdCommand(0x4F); epdData(y & 0xFF); epdData((y >> 8) & 0x03);
}
void epdInit(){
  digitalWrite(EPD_PWR_EN, HIGH); delay(100);
  epdReset(); delay(50); epdWaitReady();
  epdCommand(0x12); epdWaitReady();
  epdCommand(0x18); epdData(0x80);
  epdCommand(0x0C); epdData(0xAE); epdData(0xC7); epdData(0xC3); epdData(0xC0); epdData(0x80);
  epdCommand(0x01); epdData(0xDF); epdData(0x01); epdData(0x02);   // gate 479
  epdCommand(0x3C); epdData(0x01);
  epdCommand(0x11); epdData(0x03);
  epdSetWindow(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1);
  epdSetCursor(0, 0);
  epdWaitReady();
}
static inline uint8_t reverseBits(uint8_t b){
  b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);
  b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
  b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
  return b;
}
void epdWriteFramebuffer(){
  uint8_t* buffer = canvas.getBuffer();
  epdSetCursor(0, 0);
  epdCommand(0x24);
  SPI.beginTransaction(epdSPI);
  digitalWrite(EPD_DC, HIGH); digitalWrite(EPD_CS, LOW);
  for(uint16_t y = 0; y < EPD_HEIGHT; y++){
    uint32_t rowStart = (uint32_t)y * BYTES_PER_LINE;
    for(int16_t xb = BYTES_PER_LINE - 1; xb >= 0; xb--)
      SPI.transfer(reverseBits(buffer[rowStart + xb]));
  }
  digitalWrite(EPD_CS, HIGH);
  SPI.endTransaction();
}
void epdRefresh(){
  epdCommand(0x22); epdData(0xF7); epdCommand(0x20);
  epdWaitReady();
}
void epdSleep(){
  epdCommand(0x10); epdData(0x03); delay(50);
}

// ---- Reszleges (villanasmentes) frissites - csak az RSSI meterhez ----
// Megkozelites: teljes keptkocka + reszleges hullamforma (0x22=0xFF), a
// controllert NEM altatjuk el a frissitesek kozott. Igy nincs ablak-matek,
// a statikus reszek nem villognak, es idonkent egy teljes frissites tisztit.
// Hangolasi pontok panelenkent: a 0xFF (reszleges) es 0x3C (border) ertek.
bool epdPartialSession = false;

void stickyShow(){
  epdPartialSession = false;   // ha reszleges session volt, az epdInit HW-resetje lezarja
  epdInit();
  epdWriteFramebuffer();
  epdRefresh();
  epdSleep();
}
void epdBeginPartialSession(){   // elso teljes frissites (alapkep) + atallas reszlegesre
  epdInit();
  epdWriteFramebuffer();
  epdCommand(0x22); epdData(0xF7); epdCommand(0x20); epdWaitReady();  // teljes: alapkep
  epdCommand(0x3C); epdData(EPD_PARTIAL_BORDER);                      // border: reszleges (nincs villanas)
  epdPartialSession = true;
}
void epdPartialUpdate(){         // gyors, villanasmentes frissites
  epdWriteFramebuffer();
  epdCommand(0x22); epdData(EPD_PARTIAL_MODE); epdCommand(0x20); epdWaitReady();
}
void epdSessionFullClean(){      // idonkenti teljes frissites a session alatt (ghost-tisztitas)
  epdWriteFramebuffer();
  epdCommand(0x22); epdData(0xF7); epdCommand(0x20); epdWaitReady();
}

// ============================================================
//  Szoveg-segedek (aranyos fontok -> getTextBounds kell)
// ============================================================
uint16_t textW(const char* t){
  int16_t x1, y1; uint16_t w, h;
  canvas.getTextBounds(t, 0, 0, &x1, &y1, &w, &h);
  return w;
}
void rightAt(const char* t, int rightX, int baseline){  // aktualis font szerint
  canvas.setCursor(rightX - (int)textW(t), baseline);
  canvas.print(t);
}
void fitW(char* t, uint16_t maxW){                      // levagas "..."-tal
  if(textW(t) <= maxW) return;
  size_t n = strlen(t);
  while(n > 4 && textW(t) > maxW) t[--n] = '\0';
  if(n > 3){ t[n-1] = '.'; t[n-2] = '.'; t[n-3] = '.'; }
}

// ============================================================
//  Gombkezeles
// ============================================================
BtnEvent pollButtons(){
  static bool upPrev = false, dnPrev = false, okPrev = false;
  static bool okLongSent = false, chordActive = false;
  static uint32_t okDownAt = 0;
  const uint32_t LONG_MS = 900;
  uint32_t now = millis();

  bool up = (digitalRead(PIN_BTN_UP)   == LOW);
  bool dn = (digitalRead(PIN_BTN_DOWN) == LOW);
  bool ok = (digitalRead(PIN_BTN_OK)   == LOW);

  if(up && dn){
    if(!chordActive){ chordActive = true; upPrev = dnPrev = true; return EV_CHORD; }
    return EV_NONE;
  }
  if(ok){
    if(!okPrev){ okPrev = true; okDownAt = now; okLongSent = false; }
    else if(!okLongSent && (now - okDownAt) >= LONG_MS){ okLongSent = true; return EV_OK_LONG; }
  } else if(okPrev){
    okPrev = false; if(!okLongSent) return EV_OK;
  }
  if(up) upPrev = true;
  else if(upPrev){ upPrev = false; if(!chordActive) return EV_UP; }
  if(dn) dnPrev = true;
  else if(dnPrev){ dnPrev = false; if(!chordActive) return EV_DOWN; }
  if(!up && !dn) chordActive = false;
  return EV_NONE;
}

// ============================================================
//  Wi-Fi mod valtas
// ============================================================
void wifiToSTA(){
  if(wifiState == WM_PROMISC) esp_wifi_set_promiscuous(false);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, false);
  wifiState = WM_STA_IDLE;
}
void wifiToPromisc(uint8_t ch);

IRAM_ATTR void snifferCb(void* buf, wifi_promiscuous_pkt_type_t type){
  const wifi_promiscuous_pkt_t* pkt = (const wifi_promiscuous_pkt_t*)buf;
  const uint8_t* p = pkt->payload;
  uint16_t len = pkt->rx_ctrl.sig_len;
  if(len < 2) return;

  uint16_t fc    = p[0] | (p[1] << 8);
  uint8_t  ftype = (fc >> 2) & 0x3;
  uint8_t  fsub  = (fc >> 4) & 0xF;

  cTotal = cTotal + 1;
  if(type == WIFI_PKT_MGMT){
    cMgmt = cMgmt + 1;
    if     (fsub == 8)  cBeacon    = cBeacon + 1;
    else if(fsub == 4)  cProbeReq  = cProbeReq + 1;
    else if(fsub == 5)  cProbeResp = cProbeResp + 1;
    else if(fsub == 12) cDeauth    = cDeauth + 1;
  } else if(type == WIFI_PKT_CTRL) cCtrl = cCtrl + 1;
  else   if(type == WIFI_PKT_DATA) cData = cData + 1;

  if(len >= 22 && (type == WIFI_PKT_MGMT || type == WIFI_PKT_DATA)){
    LastFrame f;
    memcpy(f.src,   p + 10, 6);
    memcpy(f.bssid, p + 16, 6);
    f.rssi = pkt->rx_ctrl.rssi; f.ch = pkt->rx_ctrl.channel;
    f.ftype = ftype; f.fsub = fsub;
    ring[ringHead] = f; ringHead = (ringHead + 1) & 7;
  }
  if(capWanted && !capDone && type == WIFI_PKT_MGMT && fsub == 8 && len >= 36){
    if(memcmp(p + 16, capBssid, 6) == 0){
      uint16_t n = len; if(n > sizeof(capBuf)) n = sizeof(capBuf);
      memcpy(capBuf, p, n); capLen = n; capRssi = pkt->rx_ctrl.rssi; capDone = true;
    }
  }

  // Kliens-felderites: a kivalasztott BSSID (addr3) kereteibol a nem-AP MAC
  if(cliActive && len >= 22 && (type == WIFI_PKT_MGMT || type == WIFI_PKT_DATA)){
    if(memcmp(p + 16, cliBssid, 6) == 0){            // addr3 == kivalasztott AP
      const uint8_t* a1 = p + 4;                      // cel
      const uint8_t* a2 = p + 10;                     // forras
      const uint8_t* cli = (memcmp(a2, cliBssid, 6) == 0) ? a1 : a2;  // amelyik nem az AP
      if((cli[0] & 0x01) == 0 && memcmp(cli, cliBssid, 6) != 0){      // nem broadcast/multicast, nem az AP
        int idx = -1;
        for(int i = 0; i < clientCount; i++)
          if(memcmp(clients[i].mac, cli, 6) == 0){ idx = i; break; }
        if(idx < 0){
          if(clientCount < MAX_CLIENTS) idx = clientCount++;
          else { // legregebben latott kidobasa
            int oldest = 0; uint32_t ot = clients[0].lastSeen;
            for(int i = 1; i < clientCount; i++) if(clients[i].lastSeen < ot){ ot = clients[i].lastSeen; oldest = i; }
            idx = oldest;
          }
          memcpy(clients[idx].mac, cli, 6); clients[idx].frames = 0;
        }
        clients[idx].rssi     = pkt->rx_ctrl.rssi;
        clients[idx].frames   = clients[idx].frames + 1;
        clients[idx].lastSeen = millis();
      }
    }
  }

  // Probe request analyzer: mgmt subtype 4
  if(probeActive && type == WIFI_PKT_MGMT && fsub == 4 && len >= 24){
    const uint8_t* src = p + 10;                 // addr2 = forras (a kereso kliens)
    probeTotal = probeTotal + 1;

    // SSID IE (id 0) rogton a 24. bajttol; ures hossz = broadcast probe
    bool directed = false; int ssidLen = 0;
    if(len >= 26 && p[24] == 0){
      ssidLen = p[25];
      if(ssidLen > 0 && 26 + ssidLen <= len) directed = true;
    }

    int idx = -1;
    for(int i = 0; i < proberCount; i++)
      if(memcmp(probers[i].mac, src, 6) == 0){ idx = i; break; }
    if(idx < 0){
      if(proberCount < MAX_PROBERS) idx = proberCount++;
      else { int oldest = 0; uint32_t ot = probers[0].lastSeen;
             for(int i = 1; i < proberCount; i++) if(probers[i].lastSeen < ot){ ot = probers[i].lastSeen; oldest = i; }
             idx = oldest; }
      memcpy(probers[idx].mac, src, 6);
      probers[idx].count = 0; probers[idx].broadcast = 0; probers[idx].directed = 0; probers[idx].lastSsid[0] = 0;
    }
    probers[idx].rssi     = pkt->rx_ctrl.rssi;
    probers[idx].count    = probers[idx].count + 1;
    probers[idx].lastSeen = millis();
    if(directed){
      probers[idx].directed = probers[idx].directed + 1;
      int cl = ssidLen; if(cl > 32) cl = 32;
      memcpy(probers[idx].lastSsid, p + 26, cl); probers[idx].lastSsid[cl] = 0;
    } else {
      probers[idx].broadcast = probers[idx].broadcast + 1;
    }
  }

  // RSSI meter: a kivalasztott AP altal ADOTT keretek jelszintje (addr2 == BSSID)
  if(rssiActive && len >= 16 && memcmp(p + 10, rssiBssid, 6) == 0){
    rssiLast   = pkt->rx_ctrl.rssi;
    rssiLastMs = millis();
  }
}
void wifiToPromisc(uint8_t ch){
  if(wifiState == WM_PROMISC){ esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE); curCh = ch; return; }
  WiFi.mode(WIFI_STA); WiFi.disconnect();
  esp_wifi_set_promiscuous(true);
  wifi_promiscuous_filter_t filter; filter.filter_mask = WIFI_PROMIS_FILTER_MASK_ALL;
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(&snifferCb);
  esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
  wifiState = WM_PROMISC; curCh = ch;
}

// ============================================================
//  Segedfuggvenyek
// ============================================================
const char* encStr(uint8_t e){
  switch(e){
    case WIFI_AUTH_OPEN:            return "OPEN";
    case WIFI_AUTH_WEP:             return "WEP";
    case WIFI_AUTH_WPA_PSK:         return "WPA";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-E";
    case WIFI_AUTH_WPA3_PSK:        return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:   return "WPA2/3";
    case WIFI_AUTH_WAPI_PSK:        return "WAPI";
    default:                        return "?";
  }
}

// ---- OUI -> gyarto (valogatott kezdolista; bovitheto az IEEE OUI registrybol) ----
struct OuiEntry { uint8_t oui[3]; const char* name; };
static const OuiEntry OUI_TABLE[] = {
  {{0x24,0x0A,0xC4},"Espressif"},{{0x30,0xAE,0xA4},"Espressif"},{{0x7C,0x9E,0xBD},"Espressif"},
  {{0x84,0xCC,0xA8},"Espressif"},{{0x84,0xF3,0xEB},"Espressif"},{{0xA4,0xCF,0x12},"Espressif"},
  {{0xB4,0xE6,0x2D},"Espressif"},{{0xCC,0x50,0xE3},"Espressif"},{{0xDC,0x4F,0x22},"Espressif"},
  {{0xEC,0xFA,0xBC},"Espressif"},{{0x8C,0xAA,0xB5},"Espressif"},{{0x3C,0x61,0x05},"Espressif"},
  {{0x3C,0x07,0x54},"Apple"},{{0x88,0x66,0x5A},"Apple"},{{0xA4,0x83,0xE7},"Apple"},
  {{0xF0,0x18,0x98},"Apple"},{{0x90,0xB0,0xED},"Apple"},{{0xAC,0xBC,0x32},"Apple"},
  {{0xDC,0x56,0xE7},"Apple"},{{0x68,0xAB,0xBC},"Apple"},
  {{0x5C,0x0A,0x5B},"Samsung"},{{0x8C,0x77,0x12},"Samsung"},{{0xC8,0x19,0xF7},"Samsung"},
  {{0xE8,0x50,0x8B},"Samsung"},{{0x34,0x23,0xBA},"Samsung"},{{0xF0,0x25,0xB7},"Samsung"},
  {{0x50,0xC7,0xBF},"TP-Link"},{{0x14,0xCC,0x20},"TP-Link"},{{0xA4,0x2B,0xB0},"TP-Link"},
  {{0x60,0x32,0xB1},"TP-Link"},{{0xEC,0x08,0x6B},"TP-Link"},{{0x54,0xAF,0x97},"TP-Link"},
  {{0x00,0x31,0x92},"TP-Link"},
  {{0x20,0xE5,0x2A},"Netgear"},{{0x9C,0x3D,0xCF},"Netgear"},{{0xA0,0x40,0xA0},"Netgear"},
  {{0xC4,0x04,0x15},"Netgear"},{{0x3C,0x37,0x86},"Netgear"},{{0x08,0x36,0xC9},"Netgear"},
  {{0x04,0x18,0xD6},"Ubiquiti"},{{0x24,0xA4,0x3C},"Ubiquiti"},{{0x44,0xD9,0xE7},"Ubiquiti"},
  {{0x68,0x72,0x51},"Ubiquiti"},{{0xDC,0x9F,0xDB},"Ubiquiti"},{{0xFC,0xEC,0xDA},"Ubiquiti"},
  {{0x78,0x8A,0x20},"Ubiquiti"},{{0xE0,0x63,0xDA},"Ubiquiti"},
  {{0x04,0xD4,0xC4},"Asus"},{{0x2C,0x56,0xDC},"Asus"},{{0x50,0x46,0x5D},"Asus"},
  {{0xAC,0x9E,0x17},"Asus"},{{0xD8,0x50,0xE6},"Asus"},{{0x1C,0xB7,0x2C},"Asus"},{{0x88,0xD7,0xF6},"Asus"},
  {{0x14,0xD6,0x4D},"D-Link"},{{0x78,0x54,0x2E},"D-Link"},{{0xC8,0xBE,0x19},"D-Link"},
  {{0xF0,0x7D,0x68},"D-Link"},{{0x00,0x1B,0x11},"D-Link"},
  {{0x48,0x46,0xFB},"Huawei"},{{0x5C,0x7D,0x5E},"Huawei"},{{0xE0,0x24,0x7F},"Huawei"},
  {{0x00,0x9A,0xCD},"Huawei"},{{0x10,0x47,0x80},"Huawei"},{{0x24,0xDF,0x6A},"Huawei"},
  {{0x28,0x6C,0x07},"Xiaomi"},{{0x34,0xCE,0x00},"Xiaomi"},{{0x50,0x8F,0x4C},"Xiaomi"},
  {{0x64,0x09,0x80},"Xiaomi"},{{0x8C,0xBE,0xBE},"Xiaomi"},{{0x78,0x11,0xDC},"Xiaomi"},{{0xF0,0xB4,0x29},"Xiaomi"},
  {{0x34,0x13,0xE8},"Intel"},{{0x7C,0xB0,0xC2},"Intel"},{{0xA0,0xA8,0xCD},"Intel"},
  {{0xF8,0x94,0xC2},"Intel"},{{0x00,0x1B,0x21},"Intel"},{{0x94,0xE9,0x79},"Intel"},
  {{0x4C,0x5E,0x0C},"MikroTik"},{{0x64,0xD1,0x54},"MikroTik"},{{0x6C,0x3B,0x6B},"MikroTik"},
  {{0xCC,0x2D,0xE0},"MikroTik"},{{0xDC,0x2C,0x6E},"MikroTik"},{{0xE4,0x8D,0x8C},"MikroTik"},
  {{0x08,0x55,0x31},"MikroTik"},{{0x48,0x8F,0x5A},"MikroTik"},
  {{0x00,0x04,0x0E},"AVM/FRITZ"},{{0x08,0x96,0xD7},"AVM/FRITZ"},{{0x24,0x65,0x11},"AVM/FRITZ"},
  {{0x34,0x31,0xC4},"AVM/FRITZ"},{{0x3C,0xA6,0x2F},"AVM/FRITZ"},{{0xC0,0x25,0xE9},"AVM/FRITZ"},{{0xE0,0x28,0x6D},"AVM/FRITZ"},
  {{0x00,0x1B,0x0C},"Cisco"},{{0x58,0x97,0x1E},"Cisco"},{{0x00,0x24,0x14},"Cisco"},{{0xF4,0xCF,0xE2},"Cisco"},
  {{0x00,0x0B,0x86},"Aruba/HPE"},{{0x24,0xDE,0xC6},"Aruba/HPE"},{{0x6C,0xF3,0x7F},"Aruba/HPE"},
  {{0x94,0xB4,0x0F},"Aruba/HPE"},{{0xD8,0xC7,0xC8},"Aruba/HPE"},{{0x20,0x4C,0x03},"Aruba/HPE"},
  {{0x5C,0xE2,0x8C},"Zyxel"},{{0xB0,0xB2,0xDC},"Zyxel"},{{0x00,0x19,0xCB},"Zyxel"},
  {{0x54,0x60,0x09},"Google"},{{0x94,0xEB,0x2C},"Google"},{{0xF4,0xF5,0xE8},"Google"},
  {{0xF8,0x8F,0xCA},"Google"},{{0x1C,0xF2,0x9A},"Google"},
  {{0x34,0xD2,0x70},"Amazon"},{{0x44,0x65,0x0D},"Amazon"},{{0x68,0x37,0xE9},"Amazon"},
  {{0x74,0x75,0x48},"Amazon"},{{0xF0,0x81,0x73},"Amazon"},{{0xFC,0x65,0xDE},"Amazon"},
  {{0xC8,0x3A,0x35},"Tenda"},{{0x04,0x95,0xE6},"Tenda"},{{0x50,0x2B,0x73},"Tenda"},
  {{0x00,0xE0,0x4C},"Realtek"},
};
static const int OUI_N = sizeof(OUI_TABLE) / sizeof(OUI_TABLE[0]);

const char* vendorLookup(const uint8_t* b){
  if(b[0] & 0x02) return "(local MAC)";   // lokalisan kiosztott / randomizalt cim
  for(int i = 0; i < OUI_N; i++)
    if(memcmp(OUI_TABLE[i].oui, b, 3) == 0) return OUI_TABLE[i].name;
  return "(unknown)";
}

// Eltelt ido tomor formaban: "12s" / "3m" / "1h05m"
void fmtAgo(uint32_t ms, char* out, size_t n){
  uint32_t s = ms / 1000;
  if     (s < 60)   snprintf(out, n, "%us", (unsigned)s);
  else if(s < 3600) snprintf(out, n, "%um", (unsigned)(s / 60));
  else              snprintf(out, n, "%uh%02um", (unsigned)(s / 3600), (unsigned)((s % 3600) / 60));
}
void sortAps(){
  for(int i = 1; i < apCount; i++){
    ApInfo key = aps[i]; int j = i - 1;
    while(j >= 0){
      ApInfo& x = aps[j];
      bool keyBetter;
      if(x.present != key.present) keyBetter = key.present;              // jelenlevo elore
      else if(sortMode == 0)       keyBetter = key.rssi > x.rssi;        // erosebb elore
      else                         keyBetter = key.channel < x.channel;  // kisebb csatorna elore
      if(!keyBetter) break;
      aps[j+1] = aps[j]; j--;
    }
    aps[j+1] = key;
  }
}

// ---- Akku feszultseg (BQ27220) ----
uint16_t batteryMv    = 0;
bool     batteryValid = false;
int      batteryPct   = -1;   // -1 = ismeretlen

bool bqReadWord(uint8_t reg, uint16_t* out){
  Wire.beginTransmission(BQ27220_ADDR);
  Wire.write(reg);
  if(Wire.endTransmission(false) != 0) return false;
  if(Wire.requestFrom((uint8_t)BQ27220_ADDR, (uint8_t)2) != 2) return false;
  uint8_t lo = Wire.read();
  uint8_t hi = Wire.read();
  *out = ((uint16_t)hi << 8) | lo;
  return true;
}
// Egy Li-ion cella nyitott-koru feszultsegebol becsult toltottseg (fallback)
int estimatePercentFromVoltage(uint16_t mv){
  static const uint16_t cMv[]  = {3300,3400,3500,3600,3700,3800,3900,4000,4100,4200};
  static const uint8_t  cPct[] = {   0,   5,  10,  20,  35,  50,  65,  80,  90, 100};
  const int pts = sizeof(cPct);
  if(mv <= cMv[0]) return 0;
  if(mv >= cMv[pts - 1]) return 100;
  for(int i = 1; i < pts; i++)
    if(mv < cMv[i]){
      uint16_t span = cMv[i] - cMv[i - 1];
      uint8_t  rise = cPct[i] - cPct[i - 1];
      return cPct[i - 1] + (int)(((uint32_t)(mv - cMv[i - 1]) * rise) / span);
    }
  return 100;
}
void readBattery(){
  uint16_t mv = 0, soc = 0;
  batteryValid = bqReadWord(BQ27220_REG_VOLTAGE, &mv) && mv >= 2000 && mv <= 5000;
  batteryMv = batteryValid ? mv : 0;
  if(!batteryValid){ batteryPct = -1; return; }
  // Elobb a gauge SOC-ja; ha nem ertelmes, feszultseg-alapu becsles
  if(bqReadWord(BQ27220_REG_SOC, &soc) && soc <= 100) batteryPct = (int)soc;
  else                                                batteryPct = estimatePercentFromVoltage(mv);
}
// Feszultseg + toltottseg kiirasa jobbra igazitva (fejlec jobb felso sarok)
void drawBatteryAt(int rightX, int baseline){
  readBattery();
  char b[32];
  if(!batteryValid)        snprintf(b, sizeof(b), "n/a");
  else if(batteryPct >= 0) snprintf(b, sizeof(b), "%u.%02uV %d%%", batteryMv / 1000, (batteryMv % 1000) / 10, batteryPct);
  else                     snprintf(b, sizeof(b), "%u.%02uV", batteryMv / 1000, (batteryMv % 1000) / 10);
  canvas.setFont(F_SMALL);
  rightAt(b, rightX, baseline);
}

// Fejlec (12 pt bold cim + valasztovonal) es labjegyzet (9 pt)
void tb(const char* title, const char* hint){
  canvas.fillScreen(PAPER);
  canvas.setTextColor(INK);
  canvas.setTextWrap(false);
  canvas.setFont(F_MEDB);
  canvas.setCursor(6, 26); canvas.print(title);
  drawBatteryAt(CANVAS_W - 6, 26);
  canvas.drawFastHLine(0, 36, CANVAS_W, INK);
  if(hint && hint[0]){
    canvas.setFont(F_SMALL);
    canvas.setCursor(6, CANVAS_H - 8); canvas.print(hint);
  }
}
void drawBusy(const char* msg){
  tb("Wi-Fi", "");
  canvas.setFont(F_MEDB);
  canvas.setCursor(20, 400); canvas.print(msg);
  stickyShow();
}
void doScan(){
  wifiToSTA();
  drawBusy("Scanning...");
  int n = WiFi.scanNetworks(false, true);
  uint32_t now = millis();
  int found = (n < 0) ? 0 : n;

  // Eddigi bejegyzesek: egyelore "nincs jelen", amig meg nem talaljuk oket
  for(int i = 0; i < apCount; i++) aps[i].present = false;

  for(int k = 0; k < found; k++){
    uint8_t* b = WiFi.BSSID(k);

    // Van-e mar ilyen BSSID a nyilvantartasban?
    int idx = -1;
    for(int i = 0; i < apCount; i++)
      if(memcmp(aps[i].bssid, b, 6) == 0){ idx = i; break; }

    if(idx < 0){
      if(apCount < MAX_AP){
        idx = apCount++;
      } else {
        // Tele van: a legregebben latott, MAR nem jelenlevo bejegyzes kidobasa
        int oldest = -1; uint32_t oldestT = 0xFFFFFFFFUL;
        for(int i = 0; i < apCount; i++)
          if(!aps[i].present && aps[i].lastSeen < oldestT){ oldestT = aps[i].lastSeen; oldest = i; }
        if(oldest < 0) continue;   // minden jelen van, nincs hova tenni
        idx = oldest;
      }
      aps[idx].firstSeen = now;
      memcpy(aps[idx].bssid, b, 6);
    }

    // Mezok frissitese a friss szkennelesbol
    String s = WiFi.SSID(k);
    s.toCharArray(aps[idx].ssid, 33);
    aps[idx].rssi     = WiFi.RSSI(k);
    aps[idx].channel  = WiFi.channel(k);
    aps[idx].enc      = WiFi.encryptionType(k);
    aps[idx].hidden   = (s.length() == 0);
    aps[idx].lastSeen = now;
    aps[idx].present  = true;
  }
  WiFi.scanDelete();
  sortAps();
}
// ---- Vonalkas terero-jelzo ----
#define SIG_BAR_W    6
#define SIG_BAR_GAP  3
#define SIG_COL_W    (4 * SIG_BAR_W + 3 * SIG_BAR_GAP)   // 33
void drawSignalBars(int x, int baseline, int rssi){
  int level = 0;
  if      (rssi >= -55) level = 4;
  else if (rssi >= -67) level = 3;
  else if (rssi >= -78) level = 2;
  else if (rssi >= -88) level = 1;
  int bottom = baseline + 2;
  for(int i = 0; i < 4; i++){
    int h  = 6 + i * 5;
    int bx = x + i * (SIG_BAR_W + SIG_BAR_GAP);
    int by = bottom - h;
    if(i < level) canvas.fillRect(bx, by, SIG_BAR_W, h, INK);
    else          canvas.drawRect(bx, by, SIG_BAR_W, h, INK);
  }
}
int scannerRowH(){ return 76; }
// Egy oldalra ~10 talalat; ha a betumeret miatt kevesebb fer ki, annyi
int scannerPageRows(){ int fit = (CANVAS_H - 48 - 28) / scannerRowH(); return fit > 10 ? 10 : fit; }

// ============================================================
//  Rajzolok
// ============================================================
void drawMenu(){
  tb("ZANE Wi-Fi Tools", "UP/DOWN: select   OK: open   (hold OK: back)");
  canvas.setFont(F_MEDB);
  for(int i = 0; i < MENU_N; i++){
    int top = 64 + i * 70;
    if(i == menuSel){ canvas.fillRect(4, top, CANVAS_W - 8, 46, INK); canvas.setTextColor(PAPER); }
    else canvas.setTextColor(INK);
    canvas.setCursor(16, top + 31);
    canvas.print(i + 1); canvas.print(". "); canvas.print(MENU_ITEMS[i]);
  }
  canvas.setTextColor(INK);
  stickyShow();
}
void drawScanner(){
  int rowH = scannerRowH(), per = scannerPageRows(), top = 48;
  int pages = (apCount + per - 1) / per; if(pages < 1) pages = 1;
  if(scanPage >= pages) scanPage = pages - 1;
  if(scanPage < 0)      scanPage = 0;

  char t[56]; snprintf(t, sizeof(t), "Scanner (%d)  %s  %d/%d",
                       apCount, sortMode ? "CH" : "RSSI", scanPage + 1, pages);
  tb(t, "OK: rescan   UP/DOWN: page   UP+DOWN: sort");

  int start = scanPage * per;
  uint32_t now = millis();
  for(int r = 0; r < per; r++){
    int i = start + r; if(i >= apCount) break;
    int y = top + r * rowH;
    ApInfo& a = aps[i];

    // 1. sor jobb szele: jelenlevonel terero + RSSI, eltuntnel "gone <ido>"
    canvas.setFont(F_MED);
    char rs[16];
    if(a.present){
      snprintf(rs, sizeof(rs), "%d dBm", a.rssi);
    } else {
      char ago[10]; fmtAgo(now - a.lastSeen, ago, sizeof(ago));
      snprintf(rs, sizeof(rs), "gone %s", ago);
    }
    int rssiX = CANVAS_W - 6;
    int rssiW = (int)textW(rs);
    int barsX = rssiX - rssiW - 12 - SIG_COL_W;

    // SSID (bold), a jobb oldali blokk bal szeleig levagva
    canvas.setFont(F_MEDB);
    char name[40];
    if(a.hidden) strcpy(name, "<hidden>");
    else { strncpy(name, a.ssid, 32); name[32] = 0; if(name[0] == 0) strcpy(name, "<empty>"); }
    fitW(name, barsX - 6 - 10);
    canvas.setCursor(6, y + 22); canvas.print(name);

    if(a.present) drawSignalBars(barsX, y + 22, a.rssi);
    canvas.setFont(F_MED); rightAt(rs, rssiX, y + 22);

    // 2. sor: BSSID + csatorna + biztonsag
    canvas.setFont(F_SMALL);
    char d[72];
    snprintf(d, sizeof(d), "%02X:%02X:%02X:%02X:%02X:%02X   ch %d   %s",
             a.bssid[0], a.bssid[1], a.bssid[2], a.bssid[3], a.bssid[4], a.bssid[5],
             a.channel, encStr(a.enc));
    canvas.setCursor(6, y + 44); canvas.print(d);

    // 3. sor: gyarto (OUI) + eszlelesi ido (jelenlevonel "first", eltuntnel "last")
    char ago[10]; fmtAgo(now - (a.present ? a.firstSeen : a.lastSeen), ago, sizeof(ago));
    char d2[72];
    snprintf(d2, sizeof(d2), "%s   %s %s ago", vendorLookup(a.bssid),
             a.present ? "first" : "last", ago);
    canvas.setCursor(6, y + 64); canvas.print(d2);

    canvas.drawFastHLine(0, y + rowH - 6, CANVAS_W, INK);
  }
  stickyShow();
}
void drawChannels(){
  int cnt[14] = {0}; int8_t best[14]; for(int c = 1; c <= 13; c++) best[c] = -127;
  for(int i = 0; i < apCount; i++){
    int c = aps[i].channel;
    if(c >= 1 && c <= 13){ cnt[c]++; if(aps[i].rssi > best[c]) best[c] = aps[i].rssi; }
  }
  int maxc = 1; for(int c = 1; c <= 13; c++) if(cnt[c] > maxc) maxc = cnt[c];

  // Fekvo nezet - csak erre a lapra
  canvas.setRotation(CH_ROTATION);
  const int W = canvas.width();     // 800
  const int H = canvas.height();    // 480
  canvas.fillScreen(PAPER);
  canvas.setTextColor(INK);
  canvas.setTextWrap(false);

  // Fejlec: cim balra, akku jobb felso sarok, tipp tole balra
  canvas.setFont(F_MEDB); canvas.setCursor(6, 26); canvas.print("2.4 GHz Channels");
  drawBatteryAt(W - 6, 26);
  canvas.setFont(F_SMALL); rightAt("OK: rescan", W - 90, 26);
  canvas.drawFastHLine(0, 36, W, INK);

  // Diagram
  int axisY  = H - 34;               // x-tengely (alatta a csatorna-szamok)
  int topY   = 58;                   // legmagasabb oszlop teteje
  int maxBar = axisY - topY;
  int leftM = 28, rightM = 12, usable = W - leftM - rightM;
  int slotW = usable / 13;
  int barW  = slotW * 2 / 5;         // vekony oszlopok (~40% a savbol)
  if(barW < 10) barW = 10;

  canvas.drawFastHLine(leftM, axisY, usable, INK);

  canvas.setFont(F_SMALL);
  for(int c = 1; c <= 13; c++){
    int slotX = leftM + (c - 1) * slotW;
    int barX  = slotX + (slotW - barW) / 2;
    int barH  = (maxc > 0) ? (int)((long)maxBar * cnt[c] / maxc) : 0;
    if(cnt[c] > 0 && barH < 3) barH = 3;
    if(barH > 0) canvas.fillRect(barX, axisY - barH, barW, barH, INK);

    // darabszam az oszlop felett
    if(cnt[c] > 0){
      char n[12]; snprintf(n, sizeof(n), "%d", cnt[c]);
      int w = (int)textW(n);
      canvas.setCursor(barX + barW / 2 - w / 2, axisY - barH - 6); canvas.print(n);
    }
    // csatorna-szam a tengely alatt
    char cl[12]; snprintf(cl, sizeof(cl), "%d", c);
    int wc = (int)textW(cl);
    canvas.setCursor(slotX + slotW / 2 - wc / 2, axisY + 18); canvas.print(cl);
  }

  // max ertek jobbra fent
  char mx[16]; snprintf(mx, sizeof(mx), "max %d AP", maxc);
  rightAt(mx, W - 6, 52);

  stickyShow();
  canvas.setRotation(CANVAS_ROTATION);   // vissza portreba a tobbi laphoz
}
void drawInspector(bool detail){
  tb("AP inspector", "UP/DOWN: AP   OK: beacon");
  if(apCount == 0){
    canvas.setFont(F_MED); canvas.setCursor(6, 72); canvas.print("No networks"); stickyShow(); return;
  }
  ApInfo& a = aps[inspSel];
  canvas.setFont(F_MEDB); canvas.setCursor(6, 64);
  { char name[40];
    if(a.hidden) strcpy(name, "<hidden>");
    else { strncpy(name, a.ssid, 32); name[32] = 0; if(name[0] == 0) strcpy(name, "<empty>"); }
    fitW(name, CANVAS_W - 12); canvas.print(name); }

  canvas.setFont(F_SMALL); int y = 90; char l[80];
  snprintf(l, sizeof(l), "BSSID %02X:%02X:%02X:%02X:%02X:%02X",
           a.bssid[0], a.bssid[1], a.bssid[2], a.bssid[3], a.bssid[4], a.bssid[5]);
  canvas.setCursor(6, y); canvas.print(l); y += 22;
  snprintf(l, sizeof(l), "Vendor: %s", vendorLookup(a.bssid));
  canvas.setCursor(6, y); canvas.print(l); y += 22;
  snprintf(l, sizeof(l), "Channel %d   RSSI %d dBm   %s", a.channel, a.rssi, encStr(a.enc));
  canvas.setCursor(6, y); canvas.print(l); y += 22;
  { uint32_t now = millis(); char fa[10], la[10];
    fmtAgo(now - a.firstSeen, fa, sizeof(fa));
    fmtAgo(now - a.lastSeen,  la, sizeof(la));
    snprintf(l, sizeof(l), "First seen %s ago   Last %s ago%s", fa, la, a.present ? "" : "  (gone)");
    canvas.setCursor(6, y); canvas.print(l); y += 22; }
  snprintf(l, sizeof(l), "AP %d / %d", inspSel + 1, apCount);
  canvas.setCursor(6, y); canvas.print(l); y += 10;
  canvas.drawFastHLine(0, y, CANVAS_W, INK); y += 24;

  if(!detail){ canvas.setCursor(6, y); canvas.print("OK: read beacon frame (802.11 fields)"); stickyShow(); return; }
  if(!capDone){ canvas.setCursor(6, y); canvas.print("No beacon captured - try again"); stickyShow(); return; }

  canvas.setFont(F_SMALLB); canvas.setCursor(6, y); canvas.print("--- Beacon frame ---"); y += 26;
  canvas.setFont(F_SMALL);
  uint16_t bi  = capBuf[32] | (capBuf[33] << 8);
  uint16_t cap = capBuf[34] | (capBuf[35] << 8);
  snprintf(l, sizeof(l), "Beacon interval: %u TU (~%u ms)", bi, (unsigned)(bi * 1024UL / 1000)); canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "Capability 0x%04X %s%s%s", cap,
           (cap & 0x0001) ? "ESS " : "", (cap & 0x0002) ? "IBSS " : "", (cap & 0x0010) ? "Privacy" : "");
  canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "RSSI (captured): %d dBm", capRssi); canvas.setCursor(6, y); canvas.print(l); y += 24;

  bool hasRSN = false, hasWPA = false, hasWMM = false, hasHT = false, hasVHT = false, hasHE = false;
  char country[4] = {0}; int dsCh = -1;
  int p = 36;
  while(p + 2 <= (int)capLen){
    uint8_t id = capBuf[p], ln = capBuf[p + 1]; int d = p + 2;
    if(d + ln > (int)capLen) break;
    switch(id){
      case 3:   if(ln >= 1) dsCh = capBuf[d]; break;
      case 7:   if(ln >= 2){ country[0]=capBuf[d]; country[1]=capBuf[d+1]; country[2]=0; } break;
      case 45:  hasHT  = true; break;
      case 48:  hasRSN = true; break;
      case 191: hasVHT = true; break;
      case 255: hasHE  = true; break;
      case 221:
        if(ln >= 4 && capBuf[d] == 0x00 && capBuf[d+1] == 0x50 && capBuf[d+2] == 0xF2){
          if(capBuf[d+3] == 0x01) hasWPA = true;
          else if(capBuf[d+3] == 0x02) hasWMM = true;
        }
        break;
    }
    p = d + ln;
  }
  snprintf(l, sizeof(l), "DS channel: %d   Country: %s", dsCh, country[0] ? country : "-"); canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "Security: %s%s%s", hasRSN ? "RSN(WPA2/3) " : "", hasWPA ? "WPA " : "",
           (!hasRSN && !hasWPA) ? "OPEN/WEP" : ""); canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "PHY: 802.11 b/g %s%s%s", hasHT ? "n " : "", hasVHT ? "ac " : "", hasHE ? "ax" : ""); canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "WMM/QoS: %s", hasWMM ? "yes" : "no"); canvas.setCursor(6, y); canvas.print(l); y += 24;
  stickyShow();
}
void drawMonitor(){
  char t[40]; snprintf(t, sizeof(t), "Packet monitor CH%d %s", curCh, hopOn ? "[hop]" : "[fix]");
  tb(t, "OK: reset   UP/DOWN: channel   UP+DOWN: hop");
  char l[64];

  canvas.setFont(F_MEDB);
  snprintf(l, sizeof(l), "Total: %lu", (unsigned long)cTotal);
  canvas.setCursor(6, 64); canvas.print(l);

  canvas.setFont(F_SMALL); int y = 92;
  snprintf(l, sizeof(l), "MGMT: %lu   CTRL: %lu   DATA: %lu",
           (unsigned long)cMgmt, (unsigned long)cCtrl, (unsigned long)cData);
  canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "Beacon: %lu   Deauth: %lu",
           (unsigned long)cBeacon, (unsigned long)cDeauth);
  canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "ProbeReq: %lu   ProbeResp: %lu",
           (unsigned long)cProbeReq, (unsigned long)cProbeResp);
  canvas.setCursor(6, y); canvas.print(l); y += 12;
  canvas.drawFastHLine(0, y, CANVAS_W, INK); y += 26;

  canvas.setFont(F_SMALLB);
  canvas.setCursor(6, y); canvas.print("Recent frames (src / bssid / rssi / t.s):"); y += 24;
  canvas.setFont(F_SMALL);
  for(int k = 0; k < 8; k++){
    int idx = (ringHead - 1 - k) & 7;
    LastFrame f = ring[idx];
    bool empty = true; for(int b = 0; b < 6; b++) if(f.src[b]) empty = false;
    if(empty) continue;
    snprintf(l, sizeof(l), "%02X:%02X:%02X:%02X:%02X:%02X  %02X%02X  %ddBm  %d/%d",
             f.src[0], f.src[1], f.src[2], f.src[3], f.src[4], f.src[5],
             f.bssid[4], f.bssid[5], f.rssi, f.ftype, f.fsub);
    canvas.setCursor(6, y); canvas.print(l); y += 22; if(y > 770) break;
  }
  stickyShow();
}

// ============================================================
//  Kliens-felderito
// ============================================================
void resetClients(){ clientCount = 0; memset(clients, 0, sizeof(clients)); }

// Valaszto nezet: egy AP reszletei egyszerre (mint az inspector), UP/DOWN valt
void drawClientPicker(){
  tb("Client discovery", "UP/DOWN: AP   OK: watch clients");
  if(apCount == 0){
    canvas.setFont(F_MED); canvas.setCursor(6, 72); canvas.print("No networks"); stickyShow(); return;
  }
  if(cliSel < 0) cliSel = 0;
  if(cliSel >= apCount) cliSel = apCount - 1;
  ApInfo& a = aps[cliSel];

  canvas.setFont(F_MEDB); canvas.setCursor(6, 64);
  { char name[40];
    if(a.hidden) strcpy(name, "<hidden>");
    else { strncpy(name, a.ssid, 32); name[32] = 0; if(name[0] == 0) strcpy(name, "<empty>"); }
    fitW(name, CANVAS_W - 12); canvas.print(name); }

  canvas.setFont(F_SMALL); int y = 96; char l[80];
  snprintf(l, sizeof(l), "BSSID %02X:%02X:%02X:%02X:%02X:%02X",
           a.bssid[0], a.bssid[1], a.bssid[2], a.bssid[3], a.bssid[4], a.bssid[5]);
  canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "Vendor: %s", vendorLookup(a.bssid));
  canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "Channel %d   RSSI %d dBm   %s", a.channel, a.rssi, encStr(a.enc));
  canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "AP %d / %d", cliSel + 1, apCount);
  canvas.setCursor(6, y); canvas.print(l); y += 16;
  canvas.drawFastHLine(0, y, CANVAS_W, INK); y += 24;
  canvas.setCursor(6, y); canvas.print("OK: watch this AP's clients");
  stickyShow();
}

// Kliens nezet: a kivalasztott AP klienseinek listaja
void drawClients(){
  // helyi masolat + rendezes (a sniffer taszk kozben ir a tombbe)
  ClientInfo cc[MAX_CLIENTS];
  int n = clientCount; if(n > MAX_CLIENTS) n = MAX_CLIENTS;
  for(int i = 0; i < n; i++) cc[i] = clients[i];
  for(int i = 1; i < n; i++){                      // legaktivabb (frames) elol
    ClientInfo k = cc[i]; int j = i - 1;
    while(j >= 0 && cc[j].frames < k.frames){ cc[j+1] = cc[j]; j--; }
    cc[j+1] = k;
  }

  int rowH = 44, top = 48, per = (CANVAS_H - top - 28) / rowH;
  int pages = (n + per - 1) / per; if(pages < 1) pages = 1;
  if(cliPage >= pages) cliPage = pages - 1;
  if(cliPage < 0) cliPage = 0;

  char t[48]; snprintf(t, sizeof(t), "CH %d - %d clients  %d/%d", cliChannel, n, cliPage + 1, pages);
  tb(t, "OK: reset   UP/DOWN: page   OK-hold: back");

  if(n == 0){
    canvas.setFont(F_SMALL); canvas.setCursor(6, top + 26);
    canvas.print("No clients yet - waiting for traffic..."); stickyShow(); return;
  }

  int start = cliPage * per;
  uint32_t now = millis();
  for(int r = 0; r < per; r++){
    int i = start + r; if(i >= n) break;
    int y = top + r * rowH;
    ClientInfo& c = cc[i];

    canvas.setFont(F_SMALLB);
    char mac[24]; snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
                           c.mac[0], c.mac[1], c.mac[2], c.mac[3], c.mac[4], c.mac[5]);
    canvas.setCursor(6, y + 18); canvas.print(mac);
    char rs[12]; snprintf(rs, sizeof(rs), "%d dBm", c.rssi);
    canvas.setFont(F_SMALL); rightAt(rs, CANVAS_W - 6, y + 18);

    char ago[10]; fmtAgo(now - c.lastSeen, ago, sizeof(ago));
    char d[64]; snprintf(d, sizeof(d), "%s   %lu frames   %s ago",
                         vendorLookup(c.mac), (unsigned long)c.frames, ago);
    canvas.setCursor(6, y + 36); canvas.print(d);
    canvas.drawFastHLine(0, y + rowH - 4, CANVAS_W, INK);
  }
  stickyShow();
}

void startClientSniff(){ wifiToPromisc(cliChannel); }
void clientsTick(){
  uint32_t now = millis();
  if(now - lastRefresh > REFRESH_MS){ lastRefresh = now; drawClients(); }
}

// ============================================================
//  Probe request analyzer
// ============================================================
void resetProbes(){
  proberCount = 0; memset(probers, 0, sizeof(probers));
  probeTotal = 0; probeWindowBase = 0; probeWindowStart = millis(); probeRate = 0;
}
void drawProbes(){
  // helyi masolat + rendezes (a sniffer taszk kozben ir a tombbe)
  ProbeClient pc[MAX_PROBERS];
  int n = proberCount; if(n > MAX_PROBERS) n = MAX_PROBERS;
  for(int i = 0; i < n; i++) pc[i] = probers[i];
  for(int i = 1; i < n; i++){                     // legtobbet probolo elol
    ProbeClient k = pc[i]; int j = i - 1;
    while(j >= 0 && pc[j].count < k.count){ pc[j+1] = pc[j]; j--; }
    pc[j+1] = k;
  }

  int rowH = 60, top = 66, per = (CANVAS_H - top - 28) / rowH;
  int pages = (n + per - 1) / per; if(pages < 1) pages = 1;
  if(probePage >= pages) probePage = pages - 1;
  if(probePage < 0) probePage = 0;

  char t[48]; snprintf(t, sizeof(t), "Probe req  %d/s  %d/%d", probeRate, probePage + 1, pages);
  tb(t, "OK: reset   UP/DOWN: page   OK-hold: back");

  // osszesito sor: kliensszam + broadcast/directed
  uint32_t bc = 0, dc = 0;
  for(int i = 0; i < n; i++){ bc += pc[i].broadcast; dc += pc[i].directed; }
  canvas.setFont(F_SMALL);
  char sline[64]; snprintf(sline, sizeof(sline), "%d clients   bcast %lu   directed %lu",
                           n, (unsigned long)bc, (unsigned long)dc);
  canvas.setCursor(6, 56); canvas.print(sline);

  if(n == 0){
    canvas.setCursor(6, top + 26); canvas.print("No probes yet - listening..."); stickyShow(); return;
  }

  int start = probePage * per;
  for(int r = 0; r < per; r++){
    int i = start + r; if(i >= n) break;
    int y = top + r * rowH;
    ProbeClient& c = pc[i];

    canvas.setFont(F_SMALLB);
    char mac[24]; snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
                           c.mac[0], c.mac[1], c.mac[2], c.mac[3], c.mac[4], c.mac[5]);
    canvas.setCursor(6, y + 18); canvas.print(mac);
    char rs[12]; snprintf(rs, sizeof(rs), "%d dBm", c.rssi);
    canvas.setFont(F_SMALL); rightAt(rs, CANVAS_W - 6, y + 18);

    char d[64]; snprintf(d, sizeof(d), "%s   %lu probes   b:%lu d:%lu",
                         vendorLookup(c.mac), (unsigned long)c.count,
                         (unsigned long)c.broadcast, (unsigned long)c.directed);
    canvas.setCursor(6, y + 36); canvas.print(d);

    char ss[48];
    if(c.directed > 0 && c.lastSsid[0]) snprintf(ss, sizeof(ss), "wants: %s", c.lastSsid);
    else                                snprintf(ss, sizeof(ss), "(broadcast only)");
    fitW(ss, CANVAS_W - 12);
    canvas.setCursor(6, y + 54); canvas.print(ss);

    canvas.drawFastHLine(0, y + rowH - 4, CANVAS_W, INK);
  }
  stickyShow();
}
void startProbeSniff(){ curCh = 1; lastHop = millis(); wifiToPromisc(curCh); }
void probeTick(){
  uint32_t now = millis();
  // csatorna-ugralas a hatterben (a kepernyon nem jelenik meg)
  if(now - lastHop > HOP_MS){
    lastHop = now; curCh = (curCh >= 13) ? 1 : curCh + 1;
    esp_wifi_set_channel(curCh, WIFI_SECOND_CHAN_NONE);
  }
  // periodikus frissites + probe/sec szamitas az ablakra
  if(now - lastRefresh > REFRESH_MS){
    uint32_t dt = now - probeWindowStart;
    probeRate = (dt > 0) ? (int)((uint32_t)(probeTotal - probeWindowBase) * 1000UL / dt) : 0;
    probeWindowBase = probeTotal; probeWindowStart = now;
    lastRefresh = now;
    drawProbes();
  }
}

// ============================================================
//  RSSI meter
// ============================================================
void ringPush10(int8_t v){ rssiHist10[rssiH10Head] = v; rssiH10Head = (rssiH10Head + 1) % RSSI_H10; if(rssiH10N < RSSI_H10) rssiH10N++; }
void ringPush60(int8_t v){ rssiHist60[rssiH60Head] = v; rssiH60Head = (rssiH60Head + 1) % RSSI_H60; if(rssiH60N < RSSI_H60) rssiH60N++; }
void resetRssiStats(){ rssiH10N = 0; rssiH10Head = 0; rssiH60N = 0; rssiH60Head = 0; }

// Valaszto nezet: egy AP reszletei, UP/DOWN valt (mint a Client discovery)
void drawRssiPicker(){
  tb("RSSI meter", "UP/DOWN: AP   OK: measure");
  if(apCount == 0){
    canvas.setFont(F_MED); canvas.setCursor(6, 72); canvas.print("No networks"); stickyShow(); return;
  }
  if(rssiSel < 0) rssiSel = 0;
  if(rssiSel >= apCount) rssiSel = apCount - 1;
  ApInfo& a = aps[rssiSel];

  canvas.setFont(F_MEDB); canvas.setCursor(6, 64);
  { char name[40];
    if(a.hidden) strcpy(name, "<hidden>");
    else { strncpy(name, a.ssid, 32); name[32] = 0; if(name[0] == 0) strcpy(name, "<empty>"); }
    fitW(name, CANVAS_W - 12); canvas.print(name); }

  canvas.setFont(F_SMALL); int y = 96; char l[80];
  snprintf(l, sizeof(l), "BSSID %02X:%02X:%02X:%02X:%02X:%02X",
           a.bssid[0], a.bssid[1], a.bssid[2], a.bssid[3], a.bssid[4], a.bssid[5]);
  canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "Vendor: %s", vendorLookup(a.bssid));
  canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "Channel %d   RSSI %d dBm   %s", a.channel, a.rssi, encStr(a.enc));
  canvas.setCursor(6, y); canvas.print(l); y += 24;
  snprintf(l, sizeof(l), "AP %d / %d", rssiSel + 1, apCount);
  canvas.setCursor(6, y); canvas.print(l); y += 16;
  canvas.drawFastHLine(0, y, CANVAS_W, INK); y += 24;
  canvas.setCursor(6, y); canvas.print("OK: measure this AP");
  stickyShow();
}

// Mero nezet: sav + pillanatnyi + min/max/avg + history grafikon
void drawRssi(){
  bool haveSig = (rssiLastMs != 0) && (millis() - rssiLastMs < 2000);

  char t[40]; snprintf(t, sizeof(t), "RSSI meter  %s", rssiWindow ? "1 min" : "10 s");
  tb(t, "OK: reset   UP+DOWN: 10s/1min   OK-hold: back");

  canvas.setFont(F_SMALL);
  { char s[48]; snprintf(s, sizeof(s), "AP: %s   CH %d", rssiSsid, rssiChannel);
    canvas.setCursor(6, 54); canvas.print(s); }

  // Pillanatnyi ertek
  canvas.setFont(F_MEDB);
  { char cur[24];
    if(haveSig) snprintf(cur, sizeof(cur), "%d dBm", (int)rssiLast);
    else        snprintf(cur, sizeof(cur), "NO SIGNAL");
    canvas.setCursor(20, 92); canvas.print(cur); }

  // Terero-sav
  int barX = 20, barY = 104, barW = 440, barH = 34;
  canvas.drawRect(barX, barY, barW, barH, INK);
  if(haveSig){
    int cl = rssiLast;
    if(cl < RSSI_MIN_DBM) cl = RSSI_MIN_DBM;
    if(cl > RSSI_MAX_DBM) cl = RSSI_MAX_DBM;
    int fw = (cl - RSSI_MIN_DBM) * (barW - 2) / (RSSI_MAX_DBM - RSSI_MIN_DBM);
    if(fw > 0) canvas.fillRect(barX + 1, barY + 1, fw, barH - 2, INK);
  }
  canvas.setFont(F_SMALL);
  canvas.setCursor(barX, barY + barH + 16); canvas.print("-90");
  rightAt("-30", barX + barW, barY + barH + 16);
  { int w = (int)textW("-60"); canvas.setCursor(barX + barW / 2 - w / 2, barY + barH + 16); canvas.print("-60"); }

  // Min / Max / Avg az aktiv ablakbol
  int cap  = rssiWindow ? RSSI_H60 : RSSI_H10;
  int8_t* buf = rssiWindow ? rssiHist60 : rssiHist10;
  int n    = rssiWindow ? rssiH60N : rssiH10N;
  int head = rssiWindow ? rssiH60Head : rssiH10Head;
  int mn = 127, mx = -127; long sum = 0;
  for(int i = 0; i < n; i++){ int idx = (head - n + i + cap) % cap; int v = buf[idx]; if(v < mn) mn = v; if(v > mx) mx = v; sum += v; }
  int avg = n ? (int)(sum / n) : 0;
  { char m[48];
    if(n) snprintf(m, sizeof(m), "min %d   avg %d   max %d dBm", mn, avg, mx);
    else  snprintf(m, sizeof(m), "min -   avg -   max -");
    canvas.setCursor(6, barY + barH + 44); canvas.print(m); }

  // History grafikon
  int gx = 40, gy = 220, gw = 420, gh = 470;
  canvas.drawRect(gx, gy, gw, gh, INK);
  for(int v = -30; v >= -90; v -= 20){
    int yy = gy + (RSSI_MAX_DBM - v) * gh / (RSSI_MAX_DBM - RSSI_MIN_DBM);
    if(v != -30 && v != -90) canvas.drawFastHLine(gx, yy, gw, INK);
    char lab[6]; snprintf(lab, sizeof(lab), "%d", v);
    canvas.setCursor(gx - 34, yy + 5); canvas.print(lab);
  }
  int px = 0, py = 0;
  for(int i = 0; i < n; i++){
    int idx = (head - n + i + cap) % cap;
    int v = buf[idx];
    if(v < RSSI_MIN_DBM) v = RSSI_MIN_DBM;
    if(v > RSSI_MAX_DBM) v = RSSI_MAX_DBM;
    int x = gx + (n > 1 ? i * (gw - 1) / (n - 1) : 0);
    int y = gy + (RSSI_MAX_DBM - v) * gh / (RSSI_MAX_DBM - RSSI_MIN_DBM);
    if(i > 0) canvas.drawLine(px, py, x, y, INK);
    else      canvas.fillCircle(x, y, 1, INK);
    px = x; py = y;
  }
  rightAt(rssiWindow ? "last 1 min" : "last 10 s", gx + gw, gy + gh + 16);

  // Kimenet: elso rajz = teljes alapkep, utana vegig villanasmentes reszleges.
  if(!epdPartialSession){
    epdBeginPartialSession();
  } else {
    epdPartialUpdate();
  }
}

void startRssiMeas(){ wifiToPromisc(rssiChannel); }
void rssiTick(){
  uint32_t now = millis();
  bool haveSig = (rssiLastMs != 0) && (now - rssiLastMs < 2000);
  if(haveSig){
    if(now - rssiSamp10 >= 200){ rssiSamp10 = now; ringPush10(rssiLast); }
    if(now - rssiSamp60 >= 1000){ rssiSamp60 = now; ringPush60(rssiLast); }
  }
  if(now - lastRefresh > RSSI_REFRESH_MS){ lastRefresh = now; drawRssi(); }
}

// ============================================================
//  Monitor eletciklus
// ============================================================
void resetCounters(){
  cTotal = 0; cMgmt = 0; cCtrl = 0; cData = 0;
  cBeacon = 0; cProbeReq = 0; cProbeResp = 0; cDeauth = 0;
  memset(ring, 0, sizeof(ring)); ringHead = 0;
}
void startMonitor(){ wifiToPromisc(curCh); }
void monitorTick(){
  uint32_t now = millis();
  if(hopOn && now - lastHop > HOP_MS){
    lastHop = now; curCh = (curCh >= 13) ? 1 : curCh + 1;
    esp_wifi_set_channel(curCh, WIFI_SECOND_CHAN_NONE);
  }
  if(now - lastRefresh > REFRESH_MS){ lastRefresh = now; drawMonitor(); }
}
void captureBeacon(int idx){
  if(idx < 0 || idx >= apCount) return;
  memcpy(capBssid, aps[idx].bssid, 6);
  capDone = false; capWanted = true; capLen = 0;
  wifiToPromisc(aps[idx].channel);
  uint32_t t0 = millis();
  while(!capDone && millis() - t0 < 1500) delay(10);
  capWanted = false;
  esp_wifi_set_promiscuous(false);
  wifiToSTA();
}

// ============================================================
//  Kepernyo-belepes
// ============================================================
void enterScreen(Screen s){
  screen = s;
  if(s == SCR_SCANNER){ if(apCount == 0) doScan(); scanPage = 0; drawScanner(); }
  else if(s == SCR_CHANNELS){ doScan(); drawChannels(); }
  else if(s == SCR_INSPECTOR){ if(apCount == 0) doScan(); inspSel = 0; capDone = false; drawInspector(false); }
  else if(s == SCR_MONITOR){ resetCounters(); curCh = 1; hopOn = true; startMonitor(); lastRefresh = millis(); lastHop = millis(); drawMonitor(); }
  else if(s == SCR_CLIENTS){ if(apCount == 0) doScan(); cliSel = 0; cliPage = 0; cliPicking = true; cliActive = false; drawClientPicker(); }
  else if(s == SCR_PROBES){ resetProbes(); probePage = 0; probeActive = true; lastRefresh = millis(); startProbeSniff(); drawProbes(); }
  else if(s == SCR_RSSI){ if(apCount == 0) doScan(); rssiSel = 0; rssiPicking = true; rssiActive = false; drawRssiPicker(); }
}

// ============================================================
//  Kepernyonkenti gombkezeles
// ============================================================
void handleMenu(BtnEvent e){
  if(e == EV_UP)   { menuSel = (menuSel + MENU_N - 1) % MENU_N; drawMenu(); }
  else if(e == EV_DOWN){ menuSel = (menuSel + 1) % MENU_N; drawMenu(); }
  else if(e == EV_OK)  { enterScreen((Screen)(menuSel + 1)); }
}
void handleScanner(BtnEvent e){
  int per = scannerPageRows();
  int pages = (apCount + per - 1) / per; if(pages < 1) pages = 1;
  if(e == EV_UP)   { if(scanPage > 0){ scanPage--; drawScanner(); } }
  else if(e == EV_DOWN){ if(scanPage < pages - 1){ scanPage++; drawScanner(); } }
  else if(e == EV_OK)  { doScan(); scanPage = 0; drawScanner(); }
  else if(e == EV_CHORD){ sortMode ^= 1; sortAps(); scanPage = 0; drawScanner(); }
}
void handleChannels(BtnEvent e){ if(e == EV_OK){ doScan(); drawChannels(); } }
void handleInspector(BtnEvent e){
  if(e == EV_UP)   { if(inspSel > 0){ inspSel--; capDone = false; drawInspector(false); } }
  else if(e == EV_DOWN){ if(inspSel < apCount - 1){ inspSel++; capDone = false; drawInspector(false); } }
  else if(e == EV_OK)  { captureBeacon(inspSel); drawInspector(true); }
}
void handleMonitor(BtnEvent e){
  if(e == EV_OK){ resetCounters(); }
  else if(e == EV_UP)   { hopOn = false; curCh = (curCh <= 1) ? 13 : curCh - 1; esp_wifi_set_channel(curCh, WIFI_SECOND_CHAN_NONE); }
  else if(e == EV_DOWN) { hopOn = false; curCh = (curCh >= 13) ? 1 : curCh + 1; esp_wifi_set_channel(curCh, WIFI_SECOND_CHAN_NONE); }
  else if(e == EV_CHORD){ hopOn = !hopOn; lastHop = millis(); }
  if(e != EV_NONE){ drawMonitor(); lastRefresh = millis(); }
}

void handleClients(BtnEvent e){
  if(cliPicking){
    if(e == EV_UP)   { if(cliSel > 0){ cliSel--; drawClientPicker(); } }
    else if(e == EV_DOWN){ if(cliSel < apCount - 1){ cliSel++; drawClientPicker(); } }
    else if(e == EV_OK){
      memcpy(cliBssid, aps[cliSel].bssid, 6);
      cliChannel = aps[cliSel].channel;
      resetClients(); cliPage = 0;
      cliPicking = false; cliActive = true;
      startClientSniff();
      lastRefresh = millis();
      drawClients();
    }
  } else {
    int per = (CANVAS_H - 48 - 28) / 44;
    int pages = (clientCount + per - 1) / per; if(pages < 1) pages = 1;
    if(e == EV_UP)   { if(cliPage > 0){ cliPage--; drawClients(); } }
    else if(e == EV_DOWN){ if(cliPage < pages - 1){ cliPage++; drawClients(); } }
    else if(e == EV_OK){ resetClients(); cliPage = 0; drawClients(); }
  }
}

void handleProbes(BtnEvent e){
  int per = (CANVAS_H - 66 - 28) / 60;
  int pages = (proberCount + per - 1) / per; if(pages < 1) pages = 1;
  if(e == EV_UP)   { if(probePage > 0){ probePage--; drawProbes(); } }
  else if(e == EV_DOWN){ if(probePage < pages - 1){ probePage++; drawProbes(); } }
  else if(e == EV_OK){ resetProbes(); probePage = 0; drawProbes(); }
}

void handleRssi(BtnEvent e){
  if(rssiPicking){
    if(e == EV_UP)   { if(rssiSel > 0){ rssiSel--; drawRssiPicker(); } }
    else if(e == EV_DOWN){ if(rssiSel < apCount - 1){ rssiSel++; drawRssiPicker(); } }
    else if(e == EV_OK){
      memcpy(rssiBssid, aps[rssiSel].bssid, 6);
      rssiChannel = aps[rssiSel].channel;
      const char* nm = aps[rssiSel].hidden ? "<hidden>" : (aps[rssiSel].ssid[0] ? aps[rssiSel].ssid : "<empty>");
      strncpy(rssiSsid, nm, 32); rssiSsid[32] = 0;
      resetRssiStats();
      rssiLast = 0; rssiLastMs = 0; rssiWindow = 0;
      rssiPicking = false; rssiActive = true;
      epdPartialSession = false;   // uj meres -> tiszta alapkeppel indul
      startRssiMeas();
      uint32_t now = millis(); lastRefresh = now; rssiSamp10 = now; rssiSamp60 = now;
      drawRssi();
    }
  } else {
    if(e == EV_CHORD){ rssiWindow ^= 1; drawRssi(); lastRefresh = millis(); }
    else if(e == EV_OK){ resetRssiStats(); uint32_t now = millis(); rssiSamp10 = now; rssiSamp60 = now; drawRssi(); lastRefresh = now; }
  }
}

// ============================================================
//  Deep sleep
// ============================================================
void enterDeepSleep(){
  if(wifiState == WM_PROMISC) esp_wifi_set_promiscuous(false);
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_OFF);

  ledcWriteTone(PIN_BUZZER, 0);
  ledcDetach(PIN_BUZZER);
  pinMode(PIN_BUZZER, OUTPUT); digitalWrite(PIN_BUZZER, LOW);

  digitalWrite(EPD_CS, HIGH);
  digitalWrite(EPD_PWR_EN, LOW);

  while(digitalRead(PIN_BTN_OK) == LOW || digitalRead(PIN_BTN_DOWN) == LOW ||
        digitalRead(PIN_BTN_UP) == LOW) delay(10);
  delay(100);

  rtc_gpio_pullup_en((gpio_num_t)PIN_BTN_OK);   rtc_gpio_pulldown_dis((gpio_num_t)PIN_BTN_OK);
  rtc_gpio_pullup_en((gpio_num_t)PIN_BTN_DOWN); rtc_gpio_pulldown_dis((gpio_num_t)PIN_BTN_DOWN);
  rtc_gpio_pullup_en((gpio_num_t)PIN_BTN_UP);   rtc_gpio_pulldown_dis((gpio_num_t)PIN_BTN_UP);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

  // Barmely gombra ebredjen: OK + DOWN + UP
  const uint64_t wakeMask = (1ULL << PIN_BTN_OK) | (1ULL << PIN_BTN_DOWN) | (1ULL << PIN_BTN_UP);
  esp_sleep_enable_ext1_wakeup(wakeMask, ESP_EXT1_WAKEUP_ANY_LOW);

  gpio_hold_en((gpio_num_t)PIN_PWR_HOLD);
  gpio_hold_en((gpio_num_t)PIN_PWR_LOCK);
  gpio_hold_en((gpio_num_t)PIN_CHG_EN);
  gpio_hold_en((gpio_num_t)EPD_PWR_EN);
  gpio_deep_sleep_hold_en();

  esp_deep_sleep_start();
}
void noteActivity(){ lastActivity = millis(); }

// ============================================================
//  setup / loop
// ============================================================
void setup(){
  holdSystemPower();

  pinMode(PIN_BTN_UP,   INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_OK,   INPUT_PULLUP);

  ledcAttach(PIN_BUZZER, 2500, 8);

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 100000);   // BQ27220 uzemanyagmero

  pinMode(EPD_CS, OUTPUT); pinMode(EPD_DC, OUTPUT); pinMode(EPD_RST, OUTPUT);
  pinMode(EPD_BUSY, INPUT); pinMode(EPD_PWR_EN, OUTPUT);
  digitalWrite(EPD_CS, HIGH); digitalWrite(EPD_DC, LOW);
  digitalWrite(EPD_RST, HIGH); digitalWrite(EPD_PWR_EN, HIGH);

  releasePinHolds();

  SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, EPD_CS);
  canvas.setRotation(CANVAS_ROTATION);

  WiFi.mode(WIFI_STA); WiFi.disconnect();
  wifi_country_t ctry = { "HU", 1, 13, 0, WIFI_COUNTRY_POLICY_MANUAL };
  esp_wifi_set_country(&ctry);
  wifiState = WM_STA_IDLE;

  screen = SCR_MENU;
  drawMenu();
  beep(40);
  noteActivity();
}

void loop(){
  BtnEvent e = pollButtons();

  if(e == EV_OK_LONG && screen != SCR_MENU){
    // Kliens nezetbol eloszor a valasztohoz terunk vissza (nem egybol menube)
    if(screen == SCR_CLIENTS && !cliPicking){
      esp_wifi_set_promiscuous(false);
      wifiToSTA();
      cliActive = false; cliPicking = true;
      drawClientPicker(); beep(30);
      noteActivity(); delay(120);
      return;
    }
    if(screen == SCR_RSSI && !rssiPicking){
      esp_wifi_set_promiscuous(false);
      wifiToSTA();
      rssiActive = false; rssiPicking = true;
      drawRssiPicker(); beep(30);
      noteActivity(); delay(120);
      return;
    }
    if(wifiState == WM_PROMISC) esp_wifi_set_promiscuous(false);
    probeActive = false; cliActive = false; rssiActive = false;
    wifiToSTA();
    screen = SCR_MENU; drawMenu(); beep(30);
    noteActivity();
    delay(120);
    return;
  }
  if(e == EV_OK || e == EV_CHORD) beep(20);
  if(e != EV_NONE) noteActivity();

  switch(screen){
    case SCR_MENU:      handleMenu(e);      break;
    case SCR_SCANNER:   handleScanner(e);   break;
    case SCR_CHANNELS:  handleChannels(e);  break;
    case SCR_INSPECTOR: handleInspector(e); break;
    case SCR_MONITOR:   handleMonitor(e); monitorTick(); break;
    case SCR_CLIENTS:   handleClients(e); if(!cliPicking) clientsTick(); break;
    case SCR_PROBES:    handleProbes(e); probeTick(); break;
    case SCR_RSSI:      handleRssi(e); if(!rssiPicking) rssiTick(); break;
  }

  // Alvas-idozito: 1 perc tetlenseg utan eloszor vissza a menube, majd alvas
  if(millis() - lastActivity > AWAKE_WINDOW_MS){
    if(screen != SCR_MENU){
      if(wifiState == WM_PROMISC) esp_wifi_set_promiscuous(false);
      probeActive = false; cliActive = false; rssiActive = false;
      wifiToSTA();
      screen = SCR_MENU; drawMenu();
    }
    enterDeepSleep();
  }

  delay(5);
}
