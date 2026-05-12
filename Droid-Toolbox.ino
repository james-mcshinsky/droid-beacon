/* Droid Toolbox v0.83 : ruthsarian@gmail.com
 * 
 * A program to work with droids from the Droid Depot at Galaxy's Edge.
 * 
 * Features
 *   - Scan for nearby droids
 *   - Generate location and droid beacons
 *   - Control audio produced by droids
 *   - Readable display font with Aurabesh decode transition
 * 
 * Designed to be used with a LilyGO TTGO T-Display or LilyGO T-Display-S3 which are ESP32-based modules with an LCD display, although
 * it should work with any ESP32 module and some small code changes.
 *   TTGO T-Display: https://www.lilygo.cc/products/lilygo%C2%AE-ttgo-t-display-1-14-inch-lcd-esp32-control-board
 *   T-Display-S3: https://www.lilygo.cc/products/t-display-s3
 * 
 * Required Arduino Board
 *  Arduino ESP32 core: https://github.com/espressif/arduino-esp32
 *    1. add https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json to the additional board manager URLs in the preferences window
 *    2. go to boards manager
 *    3. search for and install esp32
 *    4. when compiling, select either the ESP32 Dev Module (for T-Display) or ESP32S3 Dev Module (for T-Display-32)
 * 
 * Required Arduino Libraries
 *  TFT_eSPI: https://github.com/Bodmer/TFT_eSPI
 *  OpenFontRender: https://github.com/takkaO/OpenFontRender
 *  
 * NOTE 1: 
 *  After installing _OR_UPDATING_ the TFT_eSPI library you _MUST_ edit User_Setup_Select.h as follows 
 *     1. comment out the line "#include <User_Setup.h>" (line 22-ish)
 *     2. uncomment the line "#include <User_Setups/Setup25_TTGO_T_Display.h>" (line 61-ish) for T-Display
 *        or "#include <User_Setups/Setup206_LilyGo_T_Display_S3.h>" for the T-Display-32
 *
 *   Possible path for Windows users: %USERPROFILE%\Documents\Arduino\libraries\TFT_eSPI\User_Setup_Select.h
 *
 * NOTE 2:
 *   The T-Display-S3 may display the wrong colors. If this happens you'll also need to modify 
 *   User_Setups/Setup206_LilyGo_T_Display_S3.h in the TFT_eSPI library, locate the two lines, one commented out,
 *   that begin with "#define TFT_RGB_ORDER". Uncomment the commented-out line, and comment out the uncommented line.
 *   Then reupload the sketch and the colors should be correct.
 *
 *   #define TFT_RGB_ORDER TFT_RGB
 *
 * NOTE 3:
 *   A BLE library is included in the Arduino ESP32 core. If you have ArduinoBLE already installed you will need
 *   to uninstall it in order for this code to compile correctly. To uninstall a library locate your arduino 
 *   libraries folder and delete the ArduinoBLE folder.
 *
 * NOTE 4:
 *   It seems when the serial port is open, button 0 doesn't work. If your T-Display is connected via USB to 
 *   your computer and button 0 does not work, change the port in Arduino IDE to something else. Don't forget
 *   to change it back when you want to upload new code to the T-Display.
 *
 * TTGO T-Display Board Configuration (defaults)
 *   Board: ESP32 Dev Module
 *   Flash Size: 4MB (32Mb)
 *   Partition Scheme: Huge App (3MB No OTA/1MB SPIFFS)
 *   PSRAM: Disabled
 * 
 * T-Display-S3 Board Configuration (defaults)
 *   Board: ESP32S3 Dev Module
 *   USB CDC On Boot: Enabled
 *   Flash Size: 16MB (128Mb)
 *   Partition Scheme: 16M Flash(3M APP/9.9MB FATFS)
 *   PSRAM: OPI PSRAM
 *   Upload Mode: UART0/Hardware CDC
 *   USB Mode: CDC and JTAG
 * 
 * T-Display-S3 AMOLED (Basic V1, Basic V2, Touch)
 *   Board: ESP32S3 Dev Module
 *   USB CDC On Boot: Enabled
 *   Flash Size: 16MB (128Mb)
 *   Partition Scheme: 16M Flash(3M APP/9.9MB FATFS)
 *   PSRAM: OPI PSRAM
 *   Upload Mode: UART0/Hardware CDC
 *   USB Mode: CDC and JTAG
 * 
 * References
 *   Arduino IDE setup: https://www.youtube.com/watch?v=b8254--ibmM
 *   TTGO T-Display Github Repository: https://github.com/Xinyuan-LilyGO/TTGO-T-Display
 *   T-Display-S3 Github Repository: https://github.com/Xinyuan-LilyGO/T-Display-S3
 *   T-Display-S3 AMOLED Github Repository: https://github.com/Xinyuan-LilyGO/T-Display-S3-AMOLED
 *   LilyGo AMOLED Series Github Repository: https://github.com/Xinyuan-LilyGO/LilyGo-AMOLED-Series
 *   
 *   Misc:
 *     https://github.com/Bodmer/TFT_eSPI/blob/master/TFT_eSPI.h
 *     https://github.com/takkaO/OpenFontRender/blob/master/src/OpenFontRender.h
 *     https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/BLEScan.h
 *     https://programmer.ink/think/arduino-development-tft_espi-library-learning.html
 *     https://programmer.ink/think/color-setting-and-text-display-esp32-learning-tour-arduino-version.html.
 *     https://github.com/nkolban/esp32-snippets/blob/fe3d318acddf87c6918944f24e8b899d63c816dd/cpp_utils/BLEAdvertisedDevice.h
 *     https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
 *     https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/preferences.html
 *
 * TODO
 *   screen:
 *    brightness control for T-Display and T-Display-S3
 *      both currently run at max brightness
 *      need to PWM the backlight control PIN to control brightness
 *      GPIO38 on T-Display-S3
 *      GPIO4 on T-Display
 *   scanner:
 *     search for specific droid/location
 *        constant scan; alert when encountering the specified beacon
 *        maybe proximity alert too? getting closer or farther away?
 *     ability to scan for location beacons
 *        based on RSSI, identify current 'zone'
 *        identify nearby zones 
 *        maybe a constant-scan mode that alerts when the rare zones appear (4, 5)
 *     ability to connect to a single droid and send 'raw' commands
 *        control LEDs
 *        control motors (is this a GOOD idea? probably not...)
 *   other: 
 *     is there any value in scanning for SWGE East/West beacon (used by the Disney Play app) and identifying which location you're in based off that?
 *     ability save beacons that are defined in EXPERT mode?
 *     revisit auto shutoff. can it not require a reset to wake up?
 *     add option, through defines, to rotate display 180 degrees so buttons are on the right
 *
 */

#define USE_OFR_FONTS         // uncomment to use openFontRenderer (see notes above on how to install this library)
#define USE_NVS               // uncomment to enable use of non-volatile storage (NVS) to save preferences (last font used);
//#define LILYGO_AMOLED         // uncomment if you're using a LilyGo T-Display AMOLED device
//#define SINGLE_BUTTON_MODE    // uncomment to use a single button (BUTTON1) for both button 1 and button 2; most AMOLED devices will need this
//#define SERIAL_DEBUG_ENABLE   // uncomment to enable serial debug/monitor messages
                              // if using serial debug, you may need to select HUGE APP from the partition scheme option under tools menu

#include <driver/rtc_io.h>
#ifdef LILYGO_AMOLED
  #include <LilyGo_AMOLED.h>
#endif
#include <TFT_eSPI.h>
#include <SPI.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <math.h>

#if __has_include("secrets.h")
  #include "secrets.h"
#endif

#ifndef WIFI_SSID
  #define WIFI_SSID ""
#endif

#ifndef WIFI_PASSWORD
  #define WIFI_PASSWORD ""
#endif

#ifndef WIFI_NETWORKS
  #define WIFI_NETWORKS { { WIFI_SSID, WIFI_PASSWORD } }
#endif

// newer versions of espressif esp32 core use NimBLE conventions. these defines should allow older
// versions of the core to continue to be compatible with this code 
#ifndef BLE_GAP_CONN_MODE_NON
  #define BLE_GAP_CONN_MODE_NON ADV_TYPE_NONCONN_IND
#endif
#ifndef BLE_ADDR_RANDOM
  #define BLE_ADDR_RANDOM BLE_ADDR_TYPE_RANDOM
#endif

// stop compilation under certain combinations of ESP32 and TFT_eSPI versions
#if (ESP_ARDUINO_VERSION_MAJOR >= 3) && defined(_TFT_eSPI_ESP32H_) && !defined(GPIO_PIN_COUNT)
    #error Please downgrade your ESP32 core to 2.0 or add #include "driver/gpio.h" to TFT_eSPI_ESP32.h and TFT_eSPI_ESP32_S3.h in the TFT_eSPI library.
#endif

#ifdef USE_OFR_FONTS            // load OpenFontRenderer and some fonts from https://aurekfonts.github.io/
  #include "OpenFontRender.h"
  #include "Aurabesh.h"
#endif
 
#ifdef USE_NVS                                // if NVS is enabled, create a preferences object to store data to
  #include <Preferences.h>
  #define PREF_APP_NAME     "droid-toolbox"   // the name should not be changed
  Preferences preferences;                    // create a preferences object to store variables in non-volatile storage (NVS)
#endif

#define C565(r,g,b)                         ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)    // macro to convert RGB values to TFT_eSPI color value

// CUSTOMIZATIONS BEGIN -- These values can be changed to alter Droid Toolbox's behavior.

#define MSG_VERSION                         "v0.83.3-jm1"           // the version displayed on the splash screen at the lower right; β

#ifdef LILYGO_AMOLED
  #define DEFAULT_TEXT_SIZE                 3
  #define DEFAULT_TEXT_PADDING              15
#else
  #define DEFAULT_TEXT_SIZE                 2                       // a generic size used throughout 
  #define DEFAULT_TEXT_PADDING              10                      // necessary? perhaps just use a formula throughout the code? e.g. (tft.fontHeight() / 3)
#endif

#define DEFAULT_TEXT_COLOR                  TFT_DARKGREY            // e.g. 'turn off your droid remote'
#define DEFAULT_SELECTED_TEXT_COLOR         TFT_WHITE
#define DEFAULT_SELECTED_BORDER_COLOR       TFT_YELLOW

#define SPLASH_TEXT_SIZE                    DEFAULT_TEXT_SIZE       // title is SPLASH_TEXT_SIZE+1 size, all other text is SPLASH_TEXT_SIZE
#define SPLASH_TITLE_COLOR                  TFT_RED
#define SPLASH_SUBTITLE_COLOR               TFT_ORANGE
#define SPLASH_TEXT_COLOR                   TFT_LIGHTGREY
#define SPLASH_VERSION_COLOR                C565(64,64,64)          // TFT_VERYDARKGREY

#define MENU_SELECT_CAPTION_TEXT_SIZE       DEFAULT_TEXT_SIZE
#define MENU_SELECT_CAPTION_TEXT_PADDING    DEFAULT_TEXT_PADDING
#define MENU_SELECT_CAPTION_TEXT_COLOR      TFT_WHITE
#define MENU_SELECT_TEXT_SIZE               (DEFAULT_TEXT_SIZE*2)
#define MENU_SELECT_TEXT_PADDING            DEFAULT_TEXT_PADDING
#define MENU_SELECT_TEXT_COLOR              C565(0,64,0)
#define MENU_SELECT_SELECTED_TEXT_COLOR     TFT_GREEN
#define MENU_SELECT_SELECTED_BORDER_COLOR   TFT_BLUE

#define BEACON_MENU_SELECT_TEXT_PADDING     (DEFAULT_TEXT_PADDING - 2)  // beacon menus use MENU_SELECT_* values except for these two
#define BEACON_MENU_SELECT_TEXT_SIZE        (DEFAULT_TEXT_SIZE + 1)

#define BEACON_INTERVAL_TITLE_TEXT_SIZE     DEFAULT_TEXT_SIZE
#define BEACON_INTERVAL_TITLE_COLOR         TFT_BLUE
#define BEACON_INTERVAL_VALUE_TEXT_SIZE     (DEFAULT_TEXT_SIZE + 1)
#define BEACON_INTERVAL_VALUE_COLOR         TFT_RED
#define BEACON_INTERVAL_VALUE_COLOR_S       TFT_GREEN
#define BEACON_INTERVAL_SET_TEXT_SIZE       (DEFAULT_TEXT_SIZE + 1)
#define BEACON_INTERVAL_SET_COLOR           TFT_BROWN
#define BEACON_INTERVAL_SET_COLOR_S         TFT_YELLOW
#define BEACON_INTERVAL_TEXT_PADDING        DEFAULT_TEXT_PADDING

#define BEACON_CONTROL_TEXT_SIZE            (DEFAULT_TEXT_SIZE + 1)
#define BEACON_CONTROL_TEXT_PADDING         DEFAULT_TEXT_PADDING
#define BEACON_CONTROL_TYPE_TEXT_SIZE       DEFAULT_TEXT_SIZE
#define BEACON_CONTROL_TYPE_COLOR           TFT_BLUE
#define BEACON_CONTROL_ID_COLOR             TFT_RED
#define BEACON_CONTROL_ACTIVE_COLOR         TFT_YELLOW
#define BEACON_CONTROL_INACTIVE_COLOR       TFT_BROWN
#define BEACON_CONTROL_COUNTDOWN_COLOR      C565(64,64,64)
#define BEACON_CONTROL_COUNTDOWN_SIZE       DEFAULT_TEXT_SIZE

#define BEACON_EXPERT_TEXT_SIZE             DEFAULT_TEXT_SIZE
#define BEACON_EXPERT_TEXT_PADDING          DEFAULT_TEXT_PADDING
#define BEACON_EXPERT_LABEL_COLOR           DEFAULT_TEXT_COLOR
#define BEACON_EXPERT_VALUE_COLOR           TFT_BROWN
#define BEACON_EXPERT_VALUE_SELECTED_COLOR  TFT_YELLOW

#define ACTION_TEXT_SIZE                    (DEFAULT_TEXT_SIZE * 2)
#define ACTION_TEXT_COLOR                   TFT_ORANGE            // e.g. 'CONNECTING'
#define ACTION_RESULT_OK_TEXT_COLOR         TFT_GREEN             // e.g. 'CONNECTED'
#define ACTION_RESULT_NG_TEXT_COLOR         TFT_RED               // e.g. 'CONNECT FAILED'

#define DROID_REPORT_TEXT_SIZE              DEFAULT_TEXT_SIZE     // personality text size is +1, otherwise this value is used throughout the screen
#define DROID_REPORT_COLOR                  DEFAULT_TEXT_COLOR
#define DROID_REPORT_PERSONALITY_COLOR      TFT_RED
#define DROID_REPORT_AFFILIATION_COLOR      TFT_GREEN
#define DROID_REPORT_ADDRESS_COLOR          TFT_BLUE
#define DROID_REPORT_RSSI_COLOR             TFT_PURPLE

#define SOUNDS_TEXT_SIZE                    (DEFAULT_TEXT_SIZE + 1)
#define SOUNDS_TEXT_PADDING                 DEFAULT_TEXT_PADDING
#define SOUNDS_GROUP_COLOR                  TFT_BLUE
#define SOUNDS_TRACK_COLOR                  TFT_RED
#define SOUNDS_VALUE_COLOR                  TFT_DARKGREY
#define SOUNDS_VALUE_SELECTED_COLOR         TFT_GREEN

#define VOLUME_TEXT_SIZE                    (DEFAULT_TEXT_SIZE + 1)
#define VOLUME_TEXT_PADDING                 DEFAULT_TEXT_PADDING
#define VOLUME_LOW_COLOR                    TFT_CYAN
#define VOLUME_MED_COLOR                    TFT_YELLOW
#define VOLUME_HIGH_COLOR                   TFT_RED
#define VOLUME_MAX_COLOR                    TFT_MAGENTA
#define VOLUME_TEXT_COLOR                   TFT_DARKGREY
#define VOLUME_SELECTED_TEXT_COLOR          TFT_GREEN

#define DEFAULT_TEXT_FIT_WIDTH              (tft.getViewportWidth() * 0.8)    // used to control the available width value used to calculate the font size needed for a string of text to fit within that space

#define SLEEP_AFTER                         (5 * 60 * 1000) // how many milliseconds of inactivity before going to sleep/hibernation (5 minutes is the default setting)
#define DEFAULT_BEACON_REACTION_TIME        2               // how many minutes to wait between reactions to the beacon being broadcast; ((esp_random() % 3) + 1)
#define MAX_BEACON_CHANGE_INTERVAL          120             // this is multiplied by 10. should be no larger than 250.
#define SHORT_PRESS_INTERVAL_INC            1               // this is multiplied by 10. 
#define LONG_PRESS_INTERVAL_INC             10              // this is multiplied by 10
#define WAIT_TIMES_URL_DISNEYLAND          "https://queue-times.com/parks/16/queue_times.json"
#define WAIT_TIMES_URL_DISNEY_WORLD        "https://queue-times.com/parks/7/queue_times.json"
#define WAIT_TIMES_WIFI_TIMEOUT             15000
#define WAIT_TIMES_HTTP_TIMEOUT             15000
#define WIFI_CONNECT_TIMEOUT                8000
#define SCREENSAVER_FRAME_TIME              55
#define TRIVIA_RECENT_COUNT                 12
#define TRIVIA_LINE_MAX                     96
#define DECODE_TRANSITION_ENABLE            // briefly render new screens in Aurabesh, then decode to the normal readable font
#define AURABESH_FONT                       1
#define DEFAULT_DTB_FONT                    0
#define FIRST_USER_SELECTABLE_FONT          0
#define LAST_USER_SELECTABLE_FONT           0
#define DECODE_AURABESH_FONT                AURABESH_FONT
#define DECODE_HOLD_TIME                    450
#define DECODE_FIZZ_FRAMES                  9
#define DECODE_FIZZ_FRAME_TIME              55
#define DECODE_FIZZ_SPECKS                  90

// static strings used throughout DroidToolbox
const char ble_adv_name[]               = "DROIDTLBX";              // this is the name the toolbox's beacon will appear as, keep to 10 characters or less
const char ble_adv_name_droid[]         = "DROID";
const char msg_version[]                = MSG_VERSION;
const char msg_title[]                  = "Droid Toolbox";
const char msg_email[]                  = "ruthsarian@gmail.com";
const char msg_mod_email[]              = "JM fork";
const char msg_splash_subtitle[]        = "DROID DEPOT UTILITY";
const char msg_splash_mod[]             = "JM MOD";
const char msg_continue1[]              = "PRESS ANY BUTTON";
const char msg_continue2[]              = "TO CONTINUE";
const char msg_select[]                 = "select an option";
const char msg_select_beacon_type[]     = "select beacon type";
const char msg_select_beacon[]          = "select a beacon";
const char msg_beacon[]                 = "BEACON";
const char msg_beacon_off[]             = "OFF";
const char msg_beacon_on[]              = "ON";
const char msg_scanner[]                = "SCANNER";
const char msg_scanner_active[]         = "SCANNING";
const char msg_droid_report[]           = "DROID REPORT";
const char msg_scanner_connecting[]     = "CONNECTING";
const char msg_turn_off_remote1[]       = "TURN OFF YOUR";
const char msg_turn_off_remote2[]       = "DROID REMOTE";
const char msg_scanner_connected[]      = "CONNECTED";
const char msg_connect[]                = "CONNECT";
const char msg_failed[]                 = "FAILED";
const char msg_no_droids1[]             = "No Droids";
const char msg_no_droids2[]             = "In Area";
const char msg_unknown_int[]            = "Unknown (%d)";           // probably need to remove the printf variables from these strings
const char msg_rssi[]                   = "rssi: %ddBm";
const char msg_d_of_d[]                 = "%d of %d";
const char msg_int_minutes[]            = "%d mins";
const char msg_sounds[]                 = "SOUNDS";
const char msg_volume[]                 = "VOLUME";
const char msg_random[]                 = "RANDOM";
const char msg_droid[]                  = "DROID";
const char msg_location[]               = "LOCATION";
const char msg_rotating[]               = "ROTATING";
const char msg_beacon_settings[]        = "BEACON SETTINGS";
const char msg_activate_beacon[]        = "inactive";
const char msg_beacon_active[]          = "ACTIVE";
const char msg_expert[]                 = "EXPERT";
const char msg_yes[]                    = "YES";
const char msg_no[]                     = "NO";
const char msg_type[]                   = "TYPE";
const char msg_state[]                  = "STATE";
const char msg_group[]                  = "Group";
const char msg_track[]                  = "Track";
const char msg_play[]                   = "PLAY";
const char msg_vol_inc[]                = "VOL+";
const char msg_vol_dec[]                = "VOL-";
const char msg_set_vol[]                = "SET VOLUME";
const char msg_set_beacon_interval[]    = "SET BEACON INTERVAL";
const char msg_int_sec[]                = "%d sec";
const char msg_set_interval[]           = "START BEACON";
const char msg_datapad[]                = "DATAPAD";       // top-level menu label for the iBeacon mode
const char msg_ibeacon[]                = "IBEACON";       // beacon-type label shown on the active-beacon screen
const char msg_wait_times[]             = "WAIT TIMES";
const char msg_wait_times_active[]      = "UPDATING";
const char msg_wait_times_title[]       = "GE WAIT TIMES";
const char msg_settings[]               = "SETTINGS";
const char msg_wifi_connect[]           = "WIFI CONNECT";
const char msg_wifi_status[]            = "WIFI STATUS";
const char msg_wifi_off[]               = "WIFI OFF";
const char msg_wifi_connected[]         = "CONNECTED";
const char msg_wifi_disconnected[]      = "DISCONNECTED";
const char msg_wifi_connecting[]        = "CONNECTING";
const char msg_wifi_not_set[]           = "WIFI NOT SET";
const char msg_wifi_failed[]            = "WIFI FAILED";
const char msg_fetch_failed[]           = "FETCH FAILED";
const char msg_parse_failed[]           = "NO WAIT DATA";
const char msg_closed[]                 = "CLOSED";
const char msg_queue_times_source[]     = "Queue-Times.com";
const char msg_trivia[]                 = "TRIVIA";
const char msg_answer[]                 = "ANSWER";
const char msg_screensaver[]            = "SCREENSAVER";
const char msg_death_star[]             = "DEATH STAR";
const char msg_hyperspace[]             = "HYPERSPACE";
const char msg_twin_suns[]              = "TWIN SUNS";

typedef struct {
  const char* ssid;
  const char* password;
} wifi_network_t;

wifi_network_t known_wifi_networks[] = WIFI_NETWORKS;
#define WIFI_NETWORK_COUNT (sizeof(known_wifi_networks)/sizeof(wifi_network_t))

typedef struct {
  uint8_t id;           // personality ID, unique for each personality
  const char *name;     // the personality name; displayed on screen
  uint8_t affiliation;  // the group affiliation (see droid_affiliations[] for relevant values); only used when emulating droid beacons
  uint8_t emulatable;   // 0 = will not be included in list of droid beacons available for emulation
} personality_t;

//
// Droid Personalities
//
// Add custom personality chips to this array!
//
// the (E)mulatable flag identifies whether or not that personality will be available as a droid beacon
//
// WHY WOULD I NOT WANT A PERSONALITY TO BE EMULATABLE?
//   - to customize the droid beacon list to something smaller/manageable; removing those you don't use
//   - to hide custom personality chip identities from being emulatable
//   - because the programmer created this 'feature' for another reason, then rewrote the code making the 'feature' useless
//
personality_t droid_personalities[] = {
//  ID,   NAME,         AFF,  E
  { 0x01, "R-Series",   0x01, 1},
  { 0x02, "BB-Series",  0x01, 1},
  { 0x03, "Blue",       0x05, 1},
  { 0x04, "Gray",       0x01, 1},
  { 0x05, "Red",        0x09, 1},
  { 0x06, "Orange",     0x05, 1},
  { 0x07, "Purple",     0x01, 1},
  { 0x08, "Black",      0x09, 1},
  { 0x09, "CB-23",      0x01, 1},
  { 0x0A, "Yellow",     0x05, 1},
  { 0x0B, "C1-Series",  0x05, 1},
  { 0x0C, "D-O",        0x05, 1},
  { 0x0D, "Blue 2",     0x01, 1},
  { 0x0E, "BD Unit",    0x05, 1},
  { 0x0F, "A-LT Unit",  0x01, 1},
  { 0x10, "Drum Kit",   0x01, 1},
  // Silent / keep-awake beacon: affiliation 0x0C falls in the spec's 0x0C-0x0F "no react" range.
  // Droids will NOT trigger an audio reaction, but they DO record the interaction and reset their
  // shutdown timer, so this beacon will keep a nearby droid awake without making it talk.
  { 0xFE, "Silent",     0x0C, 1},
};

#define DROID_PERSONALITIES_SIZE sizeof(droid_personalities)/sizeof(personality_t)
personality_t** emulatable_personalities;
uint8_t emulatable_personalities_size;

typedef struct {
  const char* name;
  esp_bd_addr_t bleaddr;
} name_by_bleaddr_t;

//
// Custom Droid Names
//
// Use this array to add custom names for individual droids. This is based on bluetooth address.
// The bluetooth address is included in the scanner results
name_by_bleaddr_t named_droids[] = {
// LABEL,      6-BYTE BLEADDR
  {"Frank",    {0xc2, 0xb1, 0x05, 0x2b, 0x1f, 0x91}}, // "c2:b1:05:2b:1f:91"
};

#define NAMED_DROID_SIZE  sizeof(named_droids)/sizeof(name_by_bleaddr_t)

typedef struct {
  uint8_t id;
  const char *name;
} affiliaton_t;

//
// Droid Affiliations
//
// You could add custom droid affiliations here if you so choose, but they may not play well
// with droids. I recommend values between 0x00 and 0x0B. A value from 0x0C to 0x0F will cause 
// a droid to NOT react to the droid beacon. A value up to 0x3F could be used, but the issue
// of no reaction for values from 0x_C to 0x_F will persist. 
//
// Droids are programmed to only recognize the three base affiliations. Custom affiliation
// values will fall into one of these groups:
//     Scoundrel: 0x00 - 0x03
//    Resistance: 0x04 - 0x07
//   First Order: 0x08 - 0x0B
//   No Reaction: 0x0C - 0x0F
//
affiliaton_t droid_affiliations[] = {
// ID,    NAME
  { 0x01, "Scoundrel"   },
  { 0x05, "Resistance"  },
  { 0x09, "First Order" },
};

#define DROID_AFFILIATIONS_SIZE  sizeof(droid_affiliations)/sizeof(affiliaton_t)

typedef struct {
  uint8_t id;            // location id written into the beacon (1..7 are the known locations)
  const char *name;      // text shown in the location list
  // optional encoded-byte overrides for preset variants (alarm-style and stronger-RSSI families):
  //   react_byte    - if 0xFF, encoder writes this exact byte (0xFF = alarm); 0x00 = use Expert default
  //   rssi          - if non-zero, sets the minimum-RSSI byte directly (e.g. 0x3A = -58dBm "0x18BA01" family);
  //                   0x00 = use the Expert default (-38dBm)
  uint8_t react_byte;
  uint8_t rssi;
} location_t;

//
// Location Beacons
//
// The beacons given below are known to exist in Galaxy's Edge (except for 0x04 which has
// not yet been observed in Galaxy's Edge.) There's probably not much reason to add 'custom'
// location beacons as your droid is programmed to ignore any value at or above 0x08. However
// you could change the names of the beacons to anything you want, especially if you're using
// these beacons in an environment outside of Galaxy's Edge!
//
// NOTE: a droid does "see" a location beacon with a value of 0, however it will not react
//       therefore I do not include it in this list, but location 0 is available in the
//       expert beacon mode.
//
// The "+" entries are documented variants (0x0A04xxxx18BA01 family) where the in-park beacon
// requires the droid to be much closer (~6 inches; -58dBm) before it will react.
// The "!" entries are the alarm-style 0x0A04xxFFA601 family, with a ~21min reaction interval.
//
location_t locations[] = {
//  ID,    NAME,           REACT,  RSSI         // override bytes (0x00 = Expert default)
  { 0x01, "Marketplace",   0x00,   0x00 },
  { 0x02, "Droid Depot",   0x00,   0x00 },
  { 0x03, "Resistance",    0x00,   0x00 },
  { 0x04, "Unknown",       0x00,   0x00 },
  { 0x05, "Alert",         0x00,   0x00 },
  { 0x06, "Dok Ondar's",   0x00,   0x00 },
  { 0x07, "First Order",   0x00,   0x00 },
  // -58dBm presets (0x0A040x18BA01 family, "must be very close")
  { 0x03, "Depot+",        0x18,   0x3A },     // Disneyland near Droid Depot
  { 0x06, "Mkt+",          0x18,   0x3A },     // WDW Marketplace
  // alarm-style presets (0x0A040xFFA601 family)
  { 0x05, "Alert!",        0xFF,   0x00 },     // ~21min cooldown alarm
  { 0x07, "FO!",           0xFF,   0x00 },     // ~21min cooldown alarm
};

#define LOCATIONS_SIZE  sizeof(locations)/sizeof(location_t)

#define NUM_BEACON_PARAMS 4

// next two arrays are labels for the expert beacon screen
const char* msg_beacon_droid_param[NUM_BEACON_PARAMS] = {
  "CHIP",
  "AFFL",
  "PAIRED",
  "B DROID"
};
const char* msg_beacon_location_param[NUM_BEACON_PARAMS] = {
  "LCTN", 
  "REACT INT", 
  "MIN RSSI",
  nullptr
};

//
// Custom Fonts
// The OpenFontRenderer is used to display TTF fonts.
// https://github.com/takkaO/OpenFontRender
// 
// How To Make A Font Header File
// 1) convert to TTF (if needed)
//    https://cloudconvert.com/otf-to-ttf
// 2) reduce to only those characters we care about (if needed)
//    https://products.aspose.app/font/generator/ttf-to-ttf
//      !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
// 3) convert to header (.h) file
//    https://notisrac.github.io/FileToCArray/
//      uncheck static
//      save .h file to code tree
//      make sure font variable name in header file has no dashes in it
//      make sure font variable in header file is type const unsigned char
//      look at existing font header files for an example
// 4) add #include "header.h" line near the top where other fonts are included
// 5) add font to dtb_fonts array below
//
#ifdef USE_OFR_FONTS
  typedef struct {
    const unsigned char* data;
    uint32_t size;
    float y_scale;    // a workaround for font rendering issues; the font will be scaled by this value when rendered
    float y_offset;   // another workaround to help fix baseline alignment issues
  } dtb_font_t;

  dtb_font_t dtb_fonts[] = {
    { Aurabesh, sizeof(Aurabesh), 1.0, -0.06 },
  };

  #define NUM_FONTS (sizeof(dtb_fonts)/sizeof(dtb_font_t))
  OpenFontRender ofr;
  uint8_t dtb_font = DEFAULT_DTB_FONT;
#endif // USE_OFR_FONTS

// CUSTOMIZATIONS END -- In theory you shouldn't have to edit anything below this line.

// there has got to be a better way to manage expert beacon parameter variables
// like determinig their length on-the-fly rather than hard-coded NUM_BEACON_PARAMS
// but for now it's all about just making it work. i make it pretty later on.

// define array index values for msg_beacon_droid_param[]
#define BEACON_PARAM_DROID_ID     0
#define BEACON_PARAM_DROID_AFFL   1
#define BEACON_PARAM_DROID_PAIRED 2
#define BEACON_PARAM_DROID_BDROID 3

// define array index values for msg_beacon_location_param[]
#define BEACON_PARAM_LCTN_ID      0
#define BEACON_PARAM_LCTN_REACT   1
#define BEACON_PARAM_LCTN_RSSI    2
#define BEACON_PARAM_LCTN_PARAM4  3

typedef enum {
  DROID,
  LOCATION,
  IBEACON,    // Apple iBeacon (manufacturer ID 0x004C); used for emulating the Galaxy's Edge "datapad" location iBeacons so the Play Disney Parks app thinks you're in the land
} beacon_type_t;

//
// Datapad iBeacons
//
// Apple iBeacon manufacturer-data packets that the Play Disney Parks app uses to identify which
// Galaxy's Edge land you're standing in. Each entry is the full 25-byte manufacturer-data
// payload (little-endian Apple mfid 0x004C, then 0x02 0x15 iBeacon header, 16-byte UUID,
// 2-byte MAJOR, 2-byte MINOR, 1-byte TX power), as documented in the SWGE beacon spec.
//
// Picking an entry from the DATAPAD top-level menu emits this exact payload until you stop the
// beacon, or sleep kicks in.
//
typedef struct {
  const char* name;
  uint8_t     payload[25];
} datapad_t;

datapad_t datapads[] = {
  { "Disneyland", {
      0x4C, 0x00,                                      // mfid: 0x004C (Apple), little-endian
      0x02, 0x15,                                      // iBeacon header: type 0x02, length 0x15 (21 bytes follow)
      0xBE, 0x52, 0x02, 0xC7, 0x40, 0x17, 0x44, 0x89,  // UUID: be5202c7-4017-4489-9cb2-d73d62cd529d
      0x9C, 0xB2, 0xD7, 0x3D, 0x62, 0xCD, 0x52, 0x9D,
      0xA0, 0x8E,                                      // MAJOR: 0xA08E
      0x00, 0x71,                                      // MINOR: 0x0071
      0xC0                                             // TX power: 0xC0 (-64 dBm)
  } },
  { "Walt Disney World", {
      0x4C, 0x00,                                      // mfid: 0x004C (Apple), little-endian
      0x02, 0x15,                                      // iBeacon header
      0xBC, 0x85, 0xCE, 0xAA, 0xE2, 0xE2, 0x43, 0x5D,  // UUID: bc85ceaa-e2e2-435d-b049-9b70d5151c3b
      0xB0, 0x49, 0x9B, 0x70, 0xD5, 0x15, 0x1C, 0x3B,
      0xA0, 0x29,                                      // MAJOR: 0xA029
      0x00, 0x26,                                      // MINOR: 0x0026
      0xBA                                             // TX power: 0xBA (-70 dBm)
  } },
};

#define DATAPADS_SIZE  (sizeof(datapads)/sizeof(datapad_t))
#define IBEACON_PAYLOAD_SIZE  25
 uint8_t current_datapad_idx = 0;     // last-picked datapad entry; used by display_beacon_control to label the active iBeacon

typedef struct {
  beacon_type_t type;                 // DROID or LOCATION
  uint8_t setting[NUM_BEACON_PARAMS]; // 0 = droid:personality[1:14],  location:location[1:7]
                                      // 1 = droid:affiliation[1,5,9], location:interval[1:20]
                                      // 2 = droid:paired[0,1],        location:minimum_rssi[58:28]
                                      // 3 = droid:bdroid[0,1],        location:unused
} beacon_t;
beacon_t beacon;

// this is assuming you're compiling for T-Display-S3 using the "ESP32S3 Dev Module" or "LilyGo T-Display-S3" board.
#if (defined(ARDUINO_ESP32S3_DEV) || defined(ARDUINO_LILYGO_T_DISPLAY_S3))
  #define TDISPLAYS3
#endif

#define BUTTON1_PIN       0     // button 1 on the TTGO T-Display and T-Display-S3 is GPIO 0
#if defined(LILYGO_AMOLED)
  #define BUTTON2_PIN     -1    // button 2 on the LilyGo AMOLED; if you have a version 1 AMOLED basic change this to 21
  #define BAT_ADC_PIN     4     // battery monitor pin
#elif defined(TDISPLAYS3)
  #define BUTTON2_PIN     14    // button 2 on the T-Display-S3 is GPIO14
  #define BAT_ADC_PIN     4     // battery monitor pin
#else
  #define BUTTON2_PIN     35    // button 2 on the TTGO T-Display is GPIO 35
  #define BAT_ADC_PIN     34    // battery monitor pin
  //#undef USE_OFR_FONTS          // force T-Display to not use OFR? i don't think this is necessary.
#endif

#define BTN_UP_STATE      HIGH  // the logic level that identifies if a button is up / not currently pressed; WAKEUP_LEVEL should be set to the opposite value
#define LAZY_DEBOUNCE     10    // time to wait after a button press before considering it a good press
#define SHORT_PRESS_TIME  500   // maximum time, in milliseconds, that a button can be held before release and be considered a SHORT press
#define SINGLE_BTN_GRACE  250   // how long after a button press to wait for the next one in single button mode

// WAKEUP_BUTTON defines the button that, when pressed, will wake the toolbox up from sleep. the reset button will also do this.
// The wakeup button needs to be an RTC GPIO so that the pullup/pulldown resistors will have power while in deep sleep.
// If you do not want to use a wakeup button, comment out the #define.
//
// Lilygo T-Display variants all have button 1 on GPIO 0 which is an RTC GPIO pin.
//
// Reference: 
//   ESP32    https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/gpio.html
//   ESP32S3  https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/gpio.html
#define WAKEUP_BUTTON     BUTTON1_PIN

#define MAX_DROIDS        20    // maximum number of droids to report on
#define BLE_SCAN_TIME     5     // how many seconds to scan

#define PAYLOAD_SIZE      8     // size, in bytes, of a beacon payload
#define MSG_LEN_MAX       32
#define DROID_ADDR_LEN    20
char wifi_last_status[MSG_LEN_MAX] = "";

// Using these macros to print debug messages will make it easier to disable the printing of those messages by undefining SERIAL_DEBUG_ENABLE
#ifdef SERIAL_DEBUG_ENABLE
  #define SERIAL_BEGIN(x)       Serial.begin(x)
  #define SERIAL_PRINT(x)       Serial.print(x)
  #define SERIAL_PRINTLN(x)     Serial.println(x)
  #define SERIAL_PRINT2(x,y)    Serial.print(x,y)
  #define SERIAL_PRINTLN2(x,y)  Serial.println(x,y)
#else
  #define SERIAL_BEGIN(x)
  #define SERIAL_PRINT(x)
  #define SERIAL_PRINTLN(x)
  #define SERIAL_PRINT2(x,y)
  #define SERIAL_PRINTLN2(x,y)
#endif

const BLEUUID serviceUUID("09b600a0-3e42-41fc-b474-e9c0c8f0c801");
const BLEUUID cmdUUID("09b600b1-3e42-41fc-b474-e9c0c8f0c801");
const BLEUUID notifyUUID("09b600b0-3e42-41fc-b474-e9c0c8f0c801");  // not used, but keeping it for future reference

BLEScan* pBLEScan = nullptr;
BLEClient* pClient = nullptr;
BLERemoteService* pRemoteService = nullptr;
BLERemoteCharacteristic* pRemoteCharacteristicCmd = nullptr;
BLERemoteCharacteristic* pRemoteCharacteristicNotify = nullptr;
BLEAdvertising* pAdvertising = nullptr;
BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
BLEAdvertisementData* pAdvertisementData = nullptr;  // must be pointer so i can delete class then recreate it every time beacon changes
                                                     // should i be using smart pointers?
 
typedef struct {
  uint8_t chipid;
  uint8_t affid;
  BLEAdvertisedDevice* pAdvertisedDevice;
} droid_t;

droid_t droids[MAX_DROIDS];
uint8_t droid_count = 0;
uint8_t current_droid = 0;
uint8_t current_group = 0;
uint8_t current_track = 0;

typedef struct {
  const char* label;
  const char* url;
} wait_time_park_t;

wait_time_park_t galaxy_edge_wait_parks[] = {
  { "DL",  WAIT_TIMES_URL_DISNEYLAND   },
  { "WDW", WAIT_TIMES_URL_DISNEY_WORLD },
};

#define WAIT_TIME_PARK_DISNEYLAND    0
#define WAIT_TIME_PARK_DISNEY_WORLD  1
#define GALAXY_EDGE_PARK_COUNT       (sizeof(galaxy_edge_wait_parks)/sizeof(wait_time_park_t))

typedef struct {
  uint8_t park_index;
  const char* name;
  const char* label;
  bool found;
  bool is_open;
  int16_t wait_time;
  char last_updated[25];
} wait_time_ride_t;

wait_time_ride_t galaxy_edge_waits[] = {
  { WAIT_TIME_PARK_DISNEYLAND,   "Millennium Falcon: Smugglers Run",  "DL FAL",   false, false, -1, "" },
  { WAIT_TIME_PARK_DISNEYLAND,   "Star Wars: Rise of the Resistance", "DL RISE",  false, false, -1, "" },
  { WAIT_TIME_PARK_DISNEY_WORLD, "Millennium Falcon: Smugglers Run",  "WDW FAL",  false, false, -1, "" },
  { WAIT_TIME_PARK_DISNEY_WORLD, "Star Wars: Rise of the Resistance", "WDW RISE", false, false, -1, "" },
};

#define GALAXY_EDGE_WAIT_COUNT (sizeof(galaxy_edge_waits)/sizeof(wait_time_ride_t))
char wait_times_status[MSG_LEN_MAX] = "";

typedef struct {
  const char* question;
  const char* answer;
} trivia_t;

const trivia_t trivia_items[] = {
  { "Where was Luke Skywalker raised?", "Tatooine" },
  { "What planet was Princess Leia from?", "Alderaan" },
  { "Who was Han Solo's Wookiee co-pilot?", "Chewbacca" },
  { "What ship made the Kessel Run in under 12 parsecs?", "Millennium Falcon" },
  { "Who built C-3PO?", "Anakin Skywalker" },
  { "Who carried Leia's message to Obi-Wan?", "R2-D2" },
  { "What flaw destroyed the first Death Star?", "Thermal exhaust port" },
  { "What planet did the first Death Star destroy?", "Alderaan" },
  { "Where did Yoda train Luke?", "Dagobah" },
  { "Who told Luke, 'I am your father'?", "Darth Vader" },
  { "What was Darth Vader's birth name?", "Anakin Skywalker" },
  { "Who was the Emperor in the original trilogy?", "Palpatine" },
  { "What forest moon is home to Ewoks?", "Endor" },
  { "Where was the second Death Star battle fought?", "Endor" },
  { "What species is Jabba?", "Hutt" },
  { "Who froze Han Solo in carbonite?", "Darth Vader" },
  { "What city did Lando run?", "Cloud City" },
  { "Cloud City floats above what planet?", "Bespin" },
  { "What Rebel base was on Hoth?", "Echo Base" },
  { "What creature attacked Luke on Hoth?", "Wampa" },
  { "What animals do Rebels ride on Hoth?", "Tauntauns" },
  { "What moon held the Rebel base in A New Hope?", "Yavin 4" },
  { "Who said, 'It's a trap!'?", "Admiral Ackbar" },
  { "What species is Admiral Ackbar?", "Mon Calamari" },
  { "Who was Anakin Skywalker's mother?", "Shmi Skywalker" },
  { "Who trained Obi-Wan Kenobi?", "Qui-Gon Jinn" },
  { "Who was Queen of Naboo in Episode I?", "Padme Amidala" },
  { "What species is Jar Jar Binks?", "Gungan" },
  { "Who was Darth Maul's master?", "Darth Sidious" },
  { "What planet had the clone army?", "Kamino" },
  { "Who was the clone army template?", "Jango Fett" },
  { "What was Count Dooku's Sith name?", "Darth Tyranus" },
  { "What order turned clones against Jedi?", "Order 66" },
  { "Who commanded the Separatist droid army?", "General Grievous" },
  { "Where did Obi-Wan defeat Grievous?", "Utapau" },
  { "Where did Obi-Wan duel Anakin?", "Mustafar" },
  { "What color is Mace Windu's lightsaber?", "Purple" },
  { "What planet held the Jedi Temple?", "Coruscant" },
  { "Who adopted Leia Organa?", "Bail and Breha Organa" },
  { "Who raised Luke on Tatooine?", "Owen and Beru Lars" },
  { "What crystals power lightsabers?", "Kyber crystals" },
  { "What powers the Death Star superlaser?", "Kyber crystals" },
  { "What is Yoda's species called?", "Unknown" },
  { "Who was Ahsoka Tano's master?", "Anakin Skywalker" },
  { "What species is Ahsoka Tano?", "Togruta" },
  { "What nickname did Anakin give Ahsoka?", "Snips" },
  { "Who commanded the 501st with Ahsoka?", "Captain Rex" },
  { "Who framed Ahsoka for the Jedi Temple bombing?", "Barriss Offee" },
  { "What planet was liberated in Rebels finale?", "Lothal" },
  { "What was Kanan Jarrus's birth name?", "Caleb Dume" },
  { "Who pilots the Ghost?", "Hera Syndulla" },
  { "What is Chopper's droid designation?", "C1-10P" },
  { "What is Ezra Bridger's homeworld?", "Lothal" },
  { "What species is Grand Admiral Thrawn?", "Chiss" },
  { "What clan is Sabine Wren from?", "Clan Wren" },
  { "What are Star Wars space whales called?", "Purrgil" },
  { "What blade was created by Tarre Vizsla?", "Darksaber" },
  { "Who is Bo-Katan's sister?", "Satine Kryze" },
  { "What group did Maul rule on Mandalore?", "Shadow Collective" },
  { "What world is Maul from in canon?", "Dathomir" },
  { "Who is known as the Mandalorian?", "Din Djarin" },
  { "What was Din Djarin's gunship called?", "Razor Crest" },
  { "What is the Mandalorian creed phrase?", "This is the Way" },
  { "What child travels with Din Djarin?", "Grogu" },
  { "Where did Grogu train as a youngling?", "Jedi Temple" },
  { "What is Grogu's species officially called?", "Unknown" },
  { "Who wielded the Darksaber against Din?", "Moff Gideon" },
  { "Who refused to train Grogu on Corvus?", "Ahsoka Tano" },
  { "Who was Boba Fett's father?", "Jango Fett" },
  { "Who is Boba Fett's sharpshooter ally?", "Fennec Shand" },
  { "What species is Cad Bane?", "Duros" },
  { "What style of armor does Boba Fett wear?", "Mandalorian armor" },
  { "Where was Cassian Andor raised?", "Ferrix" },
  { "What was Cassian Andor's birth world?", "Kenari" },
  { "Who runs an antiquities shop on Coruscant?", "Luthen Rael" },
  { "What world is Mon Mothma from?", "Chandrila" },
  { "Who is the ISB supervisor in Andor?", "Dedra Meero" },
  { "What prison moon held Cassian?", "Narkina 5" },
  { "Where were the Death Star plans stolen?", "Scarif" },
  { "Who led Rogue One?", "Jyn Erso" },
  { "Who was Jyn Erso's father?", "Galen Erso" },
  { "Who directed the Death Star project in Rogue One?", "Orson Krennic" },
  { "What kind of droid was K-2SO?", "Imperial security droid" },
  { "Who says, 'I am one with the Force'?", "Chirrut Imwe" },
  { "Who led the Partisans on Jedha?", "Saw Gerrera" },
  { "Who owned the Falcon before Han?", "Lando Calrissian" },
  { "What is Chewbacca's homeworld?", "Kashyyyk" },
  { "Who leads Crimson Dawn after Dryden Vos?", "Qi'ra" },
  { "Who led the Cloud-Riders?", "Enfys Nest" },
  { "What planet was Rey scavenging on?", "Jakku" },
  { "What was Finn's stormtrooper number?", "FN-2187" },
  { "What droid carries Poe's map piece?", "BB-8" },
  { "What was Kylo Ren's birth name?", "Ben Solo" },
  { "What base destroyed the Hosnian system?", "Starkiller Base" },
  { "Where is Maz Kanata's castle?", "Takodana" },
  { "Who was Supreme Leader before Kylo Ren?", "Snoke" },
  { "What planet hid Luke in exile?", "Ahch-To" },
  { "What birds live on Ahch-To?", "Porgs" },
  { "Who was Rose Tico's sister?", "Paige Tico" },
  { "What casino city appears in The Last Jedi?", "Canto Bight" },
  { "Who performed the hyperspace ram?", "Vice Admiral Holdo" },
  { "Who is Rey's grandfather?", "Palpatine" },
  { "What Sith world appears in Episode IX?", "Exegol" },
  { "What ocean moon has Death Star ruins?", "Kef Bir" },
  { "Who is the First Order spy in Episode IX?", "General Hux" },
  { "What is Leia's blockade runner called?", "Tantive IV" },
  { "What cantina is in A New Hope?", "Mos Eisley Cantina" },
  { "Who trades droids on Tatooine?", "Jawas" },
  { "Who ride banthas on Tatooine?", "Tusken Raiders" },
  { "What are Tusken Raiders also called?", "Sand People" },
  { "What pit almost ate Luke and Han?", "Sarlacc pit" },
  { "What crime lord ruled from Tatooine?", "Jabba the Hutt" },
  { "What species has head-tails called lekku?", "Twi'leks" },
  { "What is the Sith Rule of Two tied to?", "Darth Bane" },
  { "Who hunted Jedi after Order 66?", "Inquisitors" },
  { "What was the Grand Inquisitor before?", "Jedi Temple Guard" },
  { "Who is Cal Kestis's droid companion?", "BD-1" },
  { "Who mentors Cal Kestis?", "Cere Junda" },
  { "What witches lived on Dathomir?", "Nightsisters" },
  { "What battle anchors the ABY calendar?", "Battle of Yavin" },
  { "What famous Rebel X-wing unit did Luke lead?", "Rogue Squadron" },
  { "What color was Luke's second lightsaber?", "Green" },
  { "Who was Luke's first Jedi teacher?", "Obi-Wan Kenobi" },
  { "Who completed Luke's Jedi training?", "Yoda" },
  { "What title did Padme hold after queen?", "Senator" },
  { "Who married Anakin Skywalker?", "Padme Amidala" },
  { "What clone unit used orange markings under Cody?", "212th Attack Battalion" },
  { "What clone unit served Anakin?", "501st Legion" },
  { "Who said, 'Hello there' to Grievous?", "Obi-Wan Kenobi" },
  { "Who said, 'Do or do not'?", "Yoda" },
  { "What ship is Home One?", "Mon Calamari cruiser" },
  { "What planet is home to the Gungans?", "Naboo" },
  { "What Naboo city is underwater?", "Otoh Gunga" },
  { "Who was Anakin's podracing rival?", "Sebulba" },
  { "Who lost Anakin to Watto in a bet?", "Gardulla the Hutt" },
  { "Who owned Anakin when Qui-Gon met him?", "Watto" },
  { "What species is Watto?", "Toydarian" },
  { "What planet has the arena battle in Episode II?", "Geonosis" },
  { "Who was Padme's decoy in Episode I?", "Sabe" },
  { "What Sith title did Palpatine use?", "Darth Sidious" },
  { "What weapon do Jedi younglings build on Ilum?", "Lightsabers" },
  { "What droid says 'Roger roger'?", "Battle droid" },
  { "What Separatist leader was a banker?", "San Hill" },
  { "Who killed Qui-Gon Jinn?", "Darth Maul" },
  { "Who cut Darth Maul in half on Naboo?", "Obi-Wan Kenobi" },
  { "Who killed Count Dooku?", "Anakin Skywalker" },
  { "Who killed Jabba the Hutt?", "Leia Organa" },
  { "Who wore the Boushh disguise?", "Leia Organa" },
  { "What bounty hunter captured Han for Jabba?", "Boba Fett" },
  { "What disguise did Leia wear in Jabba's palace?", "Boushh" },
  { "What kind of bounty hunter was Boushh?", "Ubese" },
  { "What did Luke lose on Bespin?", "His right hand" },
  { "Who wore Vader's reforged mask in Episode IX?", "Kylo Ren" },
  { "Who trained Rey after Luke?", "Leia Organa" },
  { "Whose jacket does Finn wear?", "Poe Dameron's" },
  { "What rank was Leia in the Resistance?", "General" },
  { "Who said, 'Never tell me the odds'?", "Han Solo" },
  { "What holographic board game is on the Falcon?", "Dejarik" },
  { "What is Luke's callsign at Yavin?", "Red Five" },
  { "Who was Luke's friend in Red Squadron?", "Biggs Darklighter" },
  { "Who was Red Leader at Yavin?", "Garven Dreis" },
  { "Who was Gold Leader at Yavin?", "Jon Vander" },
  { "What is Han Solo's homeworld?", "Corellia" },
  { "What species is Greedo?", "Rodian" },
  { "What is Obi-Wan's Tatooine alias?", "Ben Kenobi" },
  { "What droid did Owen buy before R2-D2?", "R5-D4" },
  { "Who bought C-3PO and R2-D2 from Jawas?", "Owen Lars" },
  { "What is Tarkin's first name?", "Wilhuff" },
  { "What rank did Tarkin hold?", "Grand Moff" },
  { "What was Vader's flagship at Hoth?", "Executor" },
  { "What creature lived in the asteroid cave?", "Exogorth" },
  { "What bounty hunter tracked the Falcon to Bespin?", "Boba Fett" },
  { "Who piloted the Falcon at Endor?", "Lando Calrissian" },
  { "Who was Lando's copilot at Endor?", "Nien Nunb" },
  { "What species is Nien Nunb?", "Sullustan" },
  { "What language does Chewbacca speak?", "Shyriiwook" },
  { "What Tatooine spaceport contains the cantina?", "Mos Eisley" },
  { "What is Luke's X-wing astromech called?", "R2-D2" },
  { "Who commanded the Death Star trench run?", "Garven Dreis" },
  { "What medal ceremony ends A New Hope?", "Yavin ceremony" },
  { "What monster attacks the trash compactor?", "Dianoga" },
  { "What Hoth cannon helped cover transports?", "Ion cannon" },
  { "What walkers attacked Echo Base?", "AT-ATs" },
  { "What smaller walkers fought on Hoth?", "AT-STs" },
  { "What bounty hunter is a protocol droid assassin?", "4-LOM" },
  { "What bounty hunter is paired with 4-LOM?", "Zuckuss" },
  { "What bounty hunter is a Trandoshan?", "Bossk" },
  { "What bounty hunter wears bandage-like wraps?", "Dengar" },
  { "What rancor keeper cries after the rancor dies?", "Malakili" },
  { "What droid oversees Jabba's droid torture room?", "EV-9D9" },
  { "Who was Dooku's former Jedi master?", "Yoda" },
  { "Who was Qui-Gon Jinn's Padawan?", "Obi-Wan Kenobi" },
  { "What is Naboo's capital city?", "Theed" },
  { "Who was Naboo's senator before Padme?", "Palpatine" },
  { "What Jedi name is tied to the clone army order?", "Sifo-Dyas" },
  { "Who erased Kamino from the Jedi Archives?", "Count Dooku" },
  { "What species are the Kamino cloners?", "Kaminoan" },
  { "What species is General Grievous?", "Kaleesh" },
  { "Who served Obi-Wan as clone commander?", "Commander Cody" },
  { "Who knocked Mace Windu from the window?", "Palpatine" },
  { "Who said, 'So this is how liberty dies'?", "Padme Amidala" },
  { "What prophecy was Anakin tied to?", "The Chosen One" },
  { "What was the Chosen One meant to bring?", "Balance to the Force" },
  { "What clone planet is covered in ocean?", "Kamino" },
  { "What Geonosian designed the Death Star plans?", "Poggle the Lesser" },
  { "What clone war began on Geonosis?", "The Clone Wars" },
  { "What Jedi survived Order 66 in Fallen Order?", "Cal Kestis" },
  { "What ship does Cal Kestis travel on?", "Stinger Mantis" },
  { "Who pilots the Stinger Mantis?", "Greez Dritus" },
  { "What Nightsister joins Cal's crew?", "Merrin" },
  { "Who is the Second Sister?", "Trilla Suduri" },
  { "What moon holds Fortress Inquisitorius?", "Nur" },
  { "What droid helps younglings build lightsabers?", "Huyang" },
  { "What crystals did Ahsoka purify for white sabers?", "Inquisitor kyber crystals" },
  { "Who blinded Kanan Jarrus?", "Maul" },
  { "What species is Garazeb Orrelios?", "Lasat" },
  { "What weapon does Zeb carry?", "Bo-rifle" },
  { "Who was Ezra Bridger's Jedi master?", "Kanan Jarrus" },
  { "What Sith temple world appears in Rebels?", "Malachor" },
  { "What symbol did Sabine often paint?", "Starbird" },
  { "What is Hera Syndulla's son named?", "Jacen Syndulla" },
  { "What Rebel cell ship has a detachable shuttle?", "Ghost" },
  { "What small shuttle docks with the Ghost?", "Phantom" },
  { "What Imperial agent hunted the Ghost crew?", "Agent Kallus" },
  { "Who was Kallus's Lasat rival?", "Zeb Orrelios" },
  { "What owl-like convor follows Ahsoka?", "Morai" },
  { "What world between paths appears in Rebels?", "World Between Worlds" },
  { "Who recruited Cassian into the Rebellion?", "Luthen Rael" },
  { "What alias did Cassian use on Aldhani?", "Clem" },
  { "What heist did Cassian join?", "Aldhani payroll heist" },
  { "Who says 'One way out' on Narkina 5?", "Kino Loy" },
  { "What droid belonged to Maarva Andor?", "B2EMO" },
  { "Who raised Cassian on Ferrix?", "Maarva and Clem Andor" },
  { "Who is Mon Mothma's old banker friend?", "Tay Kolma" },
  { "What prison did Cassian escape from?", "Narkina 5" },
  { "What planet held the Aldhani heist?", "Aldhani" },
  { "What Imperial bureau hunts Rebels in Andor?", "ISB" },
  { "What planet was Jyn Erso rescued from?", "Wobani" },
  { "What moon held Jedha City?", "Jedha" },
  { "Who was Chirrut Imwe's heavy-weapon friend?", "Baze Malbus" },
  { "What Imperial cargo shuttle did Rogue One steal?", "Zeta-class cargo shuttle" },
  { "What was Krennic's rank?", "Director" },
  { "What pilot defected to Saw Gerrera?", "Bodhi Rook" },
  { "What codename hid the Death Star plans?", "Stardust" },
  { "Who gave the Death Star flaw its codename?", "Galen Erso" },
  { "What planet is Galaxy's Edge set on?", "Batuu" },
  { "What outpost is in Galaxy's Edge?", "Black Spire Outpost" },
  { "Who runs Ohnaka Transport Solutions?", "Hondo Ohnaka" },
  { "What ride features Hondo Ohnaka?", "Smugglers Run" },
  { "What cantina is in Black Spire Outpost?", "Oga's Cantina" },
  { "Who is the droid DJ at Oga's Cantina?", "DJ R-3X" },
  { "What workshop builds custom lightsabers on Batuu?", "Savi's Workshop" },
  { "What Batuu shop builds custom droids?", "Droid Depot" },
  { "Who runs the Den of Antiquities?", "Dok-Ondar" },
  { "What Resistance spy is tied to Batuu?", "Vi Moradi" },
  { "What planet has the Resistance base in Episode VII?", "D'Qar" },
  { "What fighter does Poe Dameron fly?", "T-70 X-wing" },
  { "What is Poe's black X-wing called?", "Black One" },
  { "Who commands chrome First Order troopers?", "Captain Phasma" },
  { "What style is Kylo Ren's lightsaber?", "Crossguard" },
  { "Who killed Han Solo?", "Kylo Ren" },
  { "Who killed Supreme Leader Snoke?", "Kylo Ren" },
  { "What mineral planet hosts the final battle in Episode VIII?", "Crait" },
  { "What crystal foxes live on Crait?", "Vulptices" },
  { "What droid cannot translate Sith language?", "C-3PO" },
  { "What desert world hosts the Festival of Ancestors?", "Pasaana" },
  { "Who leads the riders on Kef Bir?", "Jannah" },
  { "What device leads to Exegol?", "Sith wayfinder" },
  { "What object helps Rey locate a wayfinder?", "Sith dagger" },
  { "Who gives Rey Leia's lightsaber on Ahch-To?", "Luke Skywalker" },
  { "What metal is prized in Mandalorian armor?", "Beskar" },
  { "Who leads the bounty guild on Nevarro?", "Greef Karga" },
  { "What former shock trooper helps Din Djarin?", "Cara Dune" },
  { "What planet has Grogu's seeing stone?", "Tython" },
  { "Who trained Grogu after Din found Ahsoka?", "Luke Skywalker" },
  { "Who forged Din Djarin's beskar armor?", "The Armorer" },
  { "What starfighter replaces the Razor Crest?", "N-1 starfighter" },
  { "What waters are sacred to Mandalore?", "Living Waters" },
  { "What legendary beast sleeps under Mandalore?", "Mythosaur" },
  { "What weapon did Din win from Moff Gideon?", "Darksaber" },
  { "What High Republic station aided the frontier?", "Starlight Beacon" },
  { "What era is The Acolyte set in?", "High Republic" },
  { "What twins are central to The Acolyte?", "Osha and Mae" },
  { "What Jedi Master trained Osha?", "Sol" },
  { "What Wookiee Jedi appears in The Acolyte?", "Kelnacca" },
  { "Who is the main outlaw in Star Wars Outlaws?", "Kay Vess" },
  { "What companion travels with Kay Vess?", "Nix" },
};

#define TRIVIA_COUNT (sizeof(trivia_items)/sizeof(trivia_t))
uint16_t current_trivia = 0;
uint16_t recent_trivia[TRIVIA_RECENT_COUNT];
uint8_t recent_trivia_pos = 0;

const uint8_t SWGE_LOCATION_BEACON_PAYLOAD[] = {
  0x83, 0x01,  // manufacturer's id: 0x0183
  0x0A,        // type of beacon (location beacon)
  0x04,        // length of beacon data
  0x01,        // location; also corresponds to the audio group the droid will select a sound from
  0x0C,        // minimum interval between droid reactions to the beacon; this value is multiplied by 5 to determine the interval in seconds. droids have a minimum reaction time of 60 seconds (except for BD)
  0xA6,        // expected RSSI, beacon is ignored if weaker than value specified
  0x01,        // ? 0 or 1 otherwise droid will ignore the beacon
};

const uint8_t SWGE_DROID_BEACON_PAYLOAD[] = {
  0x83, 0x01,  // manufacturer's id: 0x0183
  0x03,        // type of beacon (droid beacon)
  0x04,        // length of beacon data
  0x44,        // ??
  0x81,        // 0x01 + ( 0x80 if droid is paired with a remote)
  0x82,        // a combination of personality chip and affiliation IDs
  0x01,        // personality chip ID
};

// payload[] is the manufacturer-data buffer pushed to the BLE advertiser. Sized to fit the largest
// supported beacon (25 bytes for an Apple iBeacon); SWGE droid/location beacons only use the first 8.
// payload_len reflects how many bytes are actually in the payload for the current beacon type.
uint8_t payload[IBEACON_PAYLOAD_SIZE];
uint8_t payload_len = PAYLOAD_SIZE;

typedef enum {
  SHORT_PRESS,
  LONG_PRESS
} button_press_t;

typedef enum {
  SPLASH,                 // splash screen
  TOP_MENU,               // top menu; beacon or scanner
  BEACON_TYPE_MENU,       // display the types of beacons to pick from
  BEACON_DROID_LIST,      // display a list of droid beacons to pick from
  BEACON_LOCATION_LIST,   // display a list of location beacons to pick from
  BEACON_DATAPAD_LIST,    // display a list of datapad iBeacons to pick from (Disneyland / WDW)
  BEACON_RANDOM,          // generate a random beacon
  BEACON_ROTATING,        // the beacon rotates (changes) at some regular interval
  BEACON_SET_INTERVAL,    // set the interval between beacon rotations
  BEACON_ACTIVATE,        // display the option to activate the beacon
  BEACON_ACTIVE,          // display the currently active beacon
  BEACON_EXPERT,          // display an 'expert mode' beacon control screen
  BEACON_EXPERT_ACTIVE,   // for when the expert mode beacon is active

  SCANNER_SCANNING,
  SCANNER_RESULTS,
  SCANNER_CONNECTING,
  SCANNER_CONNECTED,
  SCANNER_CONNECT_FAILED, 

  WAIT_TIMES_LOADING,
  WAIT_TIMES_RESULTS,

  SETTINGS_MENU,
  SETTINGS_WIFI_CONNECTING,
  SETTINGS_WIFI_STATUS,
  SETTINGS_WIFI_OFF,

  TRIVIA_QUESTION,
  TRIVIA_ANSWER,

  SCREENSAVER_MENU,
  SCREENSAVER_DEATH_STAR,
  SCREENSAVER_HYPERSPACE,
  SCREENSAVER_TWIN_SUNS,

  CONNECTED_MENU,         // options available after connecting to a droid; volume or sounds

  SOUND_GROUP,
  SOUND_TRACK,
  SOUND_PLAY,
  SOUND_PLAYING,

  VOLUME_UP,
  VOLUME_DOWN,
  VOLUME_TEST,
  VOLUME_TESTING,
} system_state_t;

//
// menus consist of a string and a state
// the strings of the menu are displayed as a list
// the state of the code changes to the state of string that is selected
//
typedef struct {
  system_state_t state;
  const char* text;
} menu_item_t;

const menu_item_t top_menu[] = {
  { SCANNER_SCANNING,     msg_scanner   },
  { BEACON_TYPE_MENU,     msg_beacon    },
  { BEACON_DATAPAD_LIST,  msg_datapad   },
  { WAIT_TIMES_LOADING,   msg_wait_times },
  { TRIVIA_QUESTION,      msg_trivia     },
  { SCREENSAVER_MENU,     msg_screensaver },
  { SETTINGS_MENU,        msg_settings   }
};

#define TOP_MENU_INDEX_SCANNER     0
#define TOP_MENU_INDEX_BEACON      1
#define TOP_MENU_INDEX_DATAPAD     2
#define TOP_MENU_INDEX_WAIT_TIMES  3
#define TOP_MENU_INDEX_TRIVIA      4
#define TOP_MENU_INDEX_SCREENSAVER 5
#define TOP_MENU_INDEX_SETTINGS    6

const menu_item_t beacon_type_menu[] = {
  { BEACON_LOCATION_LIST, msg_location  },
  { BEACON_DROID_LIST,    msg_droid     },
  { BEACON_RANDOM,        msg_random    },
  { BEACON_ROTATING,      msg_rotating  },
  { BEACON_EXPERT,        msg_expert    },
};

const menu_item_t connected_menu[] = {
  { SOUND_GROUP,          msg_sounds    },
  { VOLUME_UP,            msg_volume    },
};

const menu_item_t settings_menu[] = {
  { SETTINGS_WIFI_CONNECTING, msg_wifi_connect },
  { SETTINGS_WIFI_STATUS,     msg_wifi_status  },
  { SETTINGS_WIFI_OFF,        msg_wifi_off     },
};

const menu_item_t screensaver_menu[] = {
  { SCREENSAVER_DEATH_STAR, msg_death_star },
  { SCREENSAVER_HYPERSPACE, msg_hyperspace },
  { SCREENSAVER_TWIN_SUNS,  msg_twin_suns  },
};

//
// Lists
//
// a list is an array of strings which is used to render a list of options on the display
// they are derrived from menus, beacons, locations, etc.
//
// with the introduction of custom fonts, i need to precalculate some properties of each
// list that is going to be rendered in order to speed-up rendering times.
//
// thus i've created the list object which will contain a pointer to the menu or string array 
// that contains the contents of the list along with properties that will be used when 
// rendering the list on screen.
// 

// this struct contains information for rendering lists
// ofr_font_size and row_width will be calculated, for every list, every time the system font changes. 
typedef struct {
  uint8_t  text_size;
  uint16_t text_color;
  uint8_t  text_padding;
  uint16_t selected_text_color;
  uint16_t selected_border_color;
  uint8_t  fit_text_size;
  uint16_t ofr_font_size;
  uint16_t ofr_font_height;
  uint16_t row_width;
} list_render_options_t;

// TODO: remove this global once lists are fully incorporated into the code
list_render_options_t list_options;

//
// this is the list object
//
// it will have an array of pointers to strings that make up the content of the list
// this array must be populated at runtime because we need to extract the strings from different
// object types (menus, personalities, locations, etc.)
//
// the render_options object sets how the list will be displayed (color, default font size, etc.)
// these will be set set based on #define lines under the customizations section at the top of this code.
//
typedef struct {
  const char**          items;
  uint8_t               num_items;
  list_render_options_t render_options;
} list_t;

// these #define statements are index values of the global lists[] array. they will be used to indentify an individual list within the lists[] array.
#define LIST_TOP_MENU           0
#define LIST_BEACON_TYPE_MENU   1
#define LIST_CONNECTED_MENU     2
#define LIST_LOCATIONS          3
#define LIST_PERSONALITIES      4
#define LIST_DATAPADS           5
#define LIST_SETTINGS_MENU      6
#define LIST_SCREENSAVER_MENU   7
#define NUM_LISTS               8

// lists will be loaded via the load_lists() function. 
list_t lists[NUM_LISTS];

  int8_t droid_volume = 100;          // there is no way to 'read' the current volume setting, so we'll keep track with a variable and assume it starts at full (100) volume
 uint8_t selected_item = 0;           // keep track of the currently selected option when displaying menus, options, etc.
 uint8_t beacon_rotate_interval = 6;  // this value, multiplied by 10, defines the number of seconds before the current beacon changes; when set to 0 the beacon rotation feature is disabled
uint32_t next_beacon_time = 0;        // the time, in ms, when the next beacon change will occur
 uint8_t current_location_idx = 0xFF; // tracks which entry in locations[] was picked, so duplicate-ID presets (Depot+, Mkt+, Alert!, FO!) display their own name on the active-beacon screen; 0xFF = unset

// TFT_eSPI requires special setup with LilyGo AMOLED devices
#ifdef LILYGO_AMOLED
  TFT_eSPI tft_object = TFT_eSPI();
  TFT_eSprite tft = TFT_eSprite(&tft_object);
  LilyGo_Class amoled;
#else
  TFT_eSPI tft = TFT_eSPI();    // TFT_eSPI display interface
#endif

bool tft_update = true;         // flag to indicate display needs to be updated
system_state_t state = SPLASH;  // track the current state of the toolbox
uint32_t last_activity;         // keep track of the last time the user did something; used for setting the device to sleep
bool decode_transition_active = false;

int8_t button_pins[] = {BUTTON1_PIN, BUTTON2_PIN};  // make the IO pins for buttons variables so we can change them in setup()

//
// rendering lists is done with an array of strings (the items in the list)
// these lists are coming from different variable types (menus, beacons, personalities, etc.)
//
// i need this function in order to extract pointers to the list item strings in each of those 
// objects and put them into a shared type (const char**) so all the list rendering code doesn't
// require a ton of if/else statements depending on the type of object being rendered.
//
// it's important the order of the items in the parent is shared in the list object so that 
// they can share an index variable (e.g. selected_item global)
//
// i suppose if you want to further customize how lists displays (e.g. beacons and personalities
// have different text colors) then you could alter this function to change those properties.
//
// there's probably a better way to do this, but i want to get this working
// i'm open to suggestions. ruthsarian@gmail.com
//
void list_init() {
  uint8_t i;

  // populate TOP MENU list

  // calculate num_items and allocate memory for items
  lists[LIST_TOP_MENU].num_items = sizeof(top_menu) / sizeof(menu_item_t);
  lists[LIST_TOP_MENU].items = (const char**)malloc(sizeof(char*) * lists[LIST_TOP_MENU].num_items);

  // populate items
  for (i=0; i<lists[LIST_TOP_MENU].num_items; i++) {
    lists[LIST_TOP_MENU].items[i] = top_menu[i].text;
  }

  // set render options
  lists[LIST_TOP_MENU].render_options.text_size             = MENU_SELECT_TEXT_SIZE;
  lists[LIST_TOP_MENU].render_options.text_color            = MENU_SELECT_TEXT_COLOR;
  lists[LIST_TOP_MENU].render_options.text_padding          = MENU_SELECT_TEXT_PADDING;
  lists[LIST_TOP_MENU].render_options.selected_text_color   = MENU_SELECT_SELECTED_TEXT_COLOR;
  lists[LIST_TOP_MENU].render_options.selected_border_color = MENU_SELECT_SELECTED_BORDER_COLOR;
  lists[LIST_TOP_MENU].render_options.ofr_font_size         = 0;
  lists[LIST_TOP_MENU].render_options.ofr_font_height       = 0;
  lists[LIST_TOP_MENU].render_options.row_width             = 0;

  //
  // rinse and repeat
  //

  // populate BEACON TYPE MENU list
  lists[LIST_BEACON_TYPE_MENU].num_items = sizeof(beacon_type_menu) / sizeof(menu_item_t);
  lists[LIST_BEACON_TYPE_MENU].items = (const char**)malloc(sizeof(char*) * lists[LIST_BEACON_TYPE_MENU].num_items);
  for (i=0; i<lists[LIST_BEACON_TYPE_MENU].num_items; i++) {
    lists[LIST_BEACON_TYPE_MENU].items[i] = beacon_type_menu[i].text;
  }
  lists[LIST_BEACON_TYPE_MENU].render_options.text_size             = MENU_SELECT_TEXT_SIZE;
  lists[LIST_BEACON_TYPE_MENU].render_options.text_color            = MENU_SELECT_TEXT_COLOR;
  lists[LIST_BEACON_TYPE_MENU].render_options.text_padding          = MENU_SELECT_TEXT_PADDING;
  lists[LIST_BEACON_TYPE_MENU].render_options.selected_text_color   = MENU_SELECT_SELECTED_TEXT_COLOR;
  lists[LIST_BEACON_TYPE_MENU].render_options.selected_border_color = MENU_SELECT_SELECTED_BORDER_COLOR;
  lists[LIST_BEACON_TYPE_MENU].render_options.ofr_font_size         = 0;
  lists[LIST_BEACON_TYPE_MENU].render_options.ofr_font_height       = 0;
  lists[LIST_BEACON_TYPE_MENU].render_options.row_width             = 0;

  // populate CONNECTED MENU list
  lists[LIST_CONNECTED_MENU].num_items = sizeof(connected_menu) / sizeof(menu_item_t);
  lists[LIST_CONNECTED_MENU].items = (const char**)malloc(sizeof(char*) * lists[LIST_CONNECTED_MENU].num_items);
  for (i=0; i<lists[LIST_CONNECTED_MENU].num_items; i++) {
    lists[LIST_CONNECTED_MENU].items[i] = connected_menu[i].text;
  }
  lists[LIST_CONNECTED_MENU].render_options.text_size             = MENU_SELECT_TEXT_SIZE;
  lists[LIST_CONNECTED_MENU].render_options.text_color            = MENU_SELECT_TEXT_COLOR;
  lists[LIST_CONNECTED_MENU].render_options.text_padding          = MENU_SELECT_TEXT_PADDING;
  lists[LIST_CONNECTED_MENU].render_options.selected_text_color   = MENU_SELECT_SELECTED_TEXT_COLOR;
  lists[LIST_CONNECTED_MENU].render_options.selected_border_color = MENU_SELECT_SELECTED_BORDER_COLOR;
  lists[LIST_CONNECTED_MENU].render_options.ofr_font_size         = 0;
  lists[LIST_CONNECTED_MENU].render_options.ofr_font_height       = 0;
  lists[LIST_CONNECTED_MENU].render_options.row_width             = 0;

  // populate LOCATIONS list
  lists[LIST_LOCATIONS].num_items = LOCATIONS_SIZE;
  lists[LIST_LOCATIONS].items = (const char**)malloc(sizeof(char*) * lists[LIST_LOCATIONS].num_items);
  for (i=0; i<lists[LIST_LOCATIONS].num_items; i++) {
    lists[LIST_LOCATIONS].items[i] = locations[i].name;
  }
  lists[LIST_LOCATIONS].render_options.text_size              = BEACON_MENU_SELECT_TEXT_SIZE;
  lists[LIST_LOCATIONS].render_options.text_color             = MENU_SELECT_TEXT_COLOR;
  lists[LIST_LOCATIONS].render_options.text_padding           = BEACON_MENU_SELECT_TEXT_PADDING;
  lists[LIST_LOCATIONS].render_options.selected_text_color    = MENU_SELECT_SELECTED_TEXT_COLOR;
  lists[LIST_LOCATIONS].render_options.selected_border_color  = MENU_SELECT_SELECTED_BORDER_COLOR;
  lists[LIST_LOCATIONS].render_options.ofr_font_size          = 0;
  lists[LIST_LOCATIONS].render_options.ofr_font_height        = 0;
  lists[LIST_LOCATIONS].render_options.row_width              = 0;

  // populate DROID PERSONALITIES list
  lists[LIST_PERSONALITIES].num_items = DROID_PERSONALITIES_SIZE;
  lists[LIST_PERSONALITIES].items = (const char**)malloc(sizeof(char*) * lists[LIST_PERSONALITIES].num_items);
  for (i=0; i<lists[LIST_PERSONALITIES].num_items; i++) {
    lists[LIST_PERSONALITIES].items[i] = droid_personalities[i].name;
  }
  lists[LIST_PERSONALITIES].render_options.text_size              = BEACON_MENU_SELECT_TEXT_SIZE;
  lists[LIST_PERSONALITIES].render_options.text_color             = MENU_SELECT_TEXT_COLOR;
  lists[LIST_PERSONALITIES].render_options.text_padding           = BEACON_MENU_SELECT_TEXT_PADDING;
  lists[LIST_PERSONALITIES].render_options.selected_text_color    = MENU_SELECT_SELECTED_TEXT_COLOR;
  lists[LIST_PERSONALITIES].render_options.selected_border_color  = MENU_SELECT_SELECTED_BORDER_COLOR;
  lists[LIST_PERSONALITIES].render_options.ofr_font_size          = 0;
  lists[LIST_PERSONALITIES].render_options.ofr_font_height        = 0;
  lists[LIST_PERSONALITIES].render_options.row_width              = 0;

  // populate DATAPADS (iBeacon) list
  lists[LIST_DATAPADS].num_items = DATAPADS_SIZE;
  lists[LIST_DATAPADS].items = (const char**)malloc(sizeof(char*) * lists[LIST_DATAPADS].num_items);
  for (i=0; i<lists[LIST_DATAPADS].num_items; i++) {
    lists[LIST_DATAPADS].items[i] = datapads[i].name;
  }
  lists[LIST_DATAPADS].render_options.text_size              = BEACON_MENU_SELECT_TEXT_SIZE;
  lists[LIST_DATAPADS].render_options.text_color             = MENU_SELECT_TEXT_COLOR;
  lists[LIST_DATAPADS].render_options.text_padding           = BEACON_MENU_SELECT_TEXT_PADDING;
  lists[LIST_DATAPADS].render_options.selected_text_color    = MENU_SELECT_SELECTED_TEXT_COLOR;
  lists[LIST_DATAPADS].render_options.selected_border_color  = MENU_SELECT_SELECTED_BORDER_COLOR;
  lists[LIST_DATAPADS].render_options.ofr_font_size          = 0;
  lists[LIST_DATAPADS].render_options.ofr_font_height        = 0;
  lists[LIST_DATAPADS].render_options.row_width              = 0;

  // populate SETTINGS MENU list
  lists[LIST_SETTINGS_MENU].num_items = sizeof(settings_menu) / sizeof(menu_item_t);
  lists[LIST_SETTINGS_MENU].items = (const char**)malloc(sizeof(char*) * lists[LIST_SETTINGS_MENU].num_items);
  for (i=0; i<lists[LIST_SETTINGS_MENU].num_items; i++) {
    lists[LIST_SETTINGS_MENU].items[i] = settings_menu[i].text;
  }
  lists[LIST_SETTINGS_MENU].render_options.text_size             = MENU_SELECT_TEXT_SIZE;
  lists[LIST_SETTINGS_MENU].render_options.text_color            = MENU_SELECT_TEXT_COLOR;
  lists[LIST_SETTINGS_MENU].render_options.text_padding          = MENU_SELECT_TEXT_PADDING;
  lists[LIST_SETTINGS_MENU].render_options.selected_text_color   = MENU_SELECT_SELECTED_TEXT_COLOR;
  lists[LIST_SETTINGS_MENU].render_options.selected_border_color = MENU_SELECT_SELECTED_BORDER_COLOR;
  lists[LIST_SETTINGS_MENU].render_options.ofr_font_size         = 0;
  lists[LIST_SETTINGS_MENU].render_options.ofr_font_height       = 0;
  lists[LIST_SETTINGS_MENU].render_options.row_width             = 0;

  // populate SCREENSAVER MENU list
  lists[LIST_SCREENSAVER_MENU].num_items = sizeof(screensaver_menu) / sizeof(menu_item_t);
  lists[LIST_SCREENSAVER_MENU].items = (const char**)malloc(sizeof(char*) * lists[LIST_SCREENSAVER_MENU].num_items);
  for (i=0; i<lists[LIST_SCREENSAVER_MENU].num_items; i++) {
    lists[LIST_SCREENSAVER_MENU].items[i] = screensaver_menu[i].text;
  }
  lists[LIST_SCREENSAVER_MENU].render_options.text_size             = MENU_SELECT_TEXT_SIZE;
  lists[LIST_SCREENSAVER_MENU].render_options.text_color            = MENU_SELECT_TEXT_COLOR;
  lists[LIST_SCREENSAVER_MENU].render_options.text_padding          = MENU_SELECT_TEXT_PADDING;
  lists[LIST_SCREENSAVER_MENU].render_options.selected_text_color   = MENU_SELECT_SELECTED_TEXT_COLOR;
  lists[LIST_SCREENSAVER_MENU].render_options.selected_border_color = MENU_SELECT_SELECTED_BORDER_COLOR;
  lists[LIST_SCREENSAVER_MENU].render_options.ofr_font_size         = 0;
  lists[LIST_SCREENSAVER_MENU].render_options.ofr_font_height       = 0;
  lists[LIST_SCREENSAVER_MENU].render_options.row_width             = 0;
}

// calculate the current font height
uint16_t dtb_get_font_height() {
  static const char test_str[] = "Hy";
  #ifdef USE_OFR_FONTS
    if (dtb_font != 0) {
      return((uint16_t)(ofr.getTextHeight(test_str) & 0x0000FFFF));
    } else {
  #endif
      return((uint16_t)(tft.fontHeight() & 0x7FFF));
  #ifdef USE_OFR_FONTS
    }
  #endif
}

// calculate the current font height
uint16_t dtb_get_text_width(const char* msg) {
  static const char test_str[] = "Hy";
  if (msg == nullptr) {
    msg = test_str;
  }
  #ifdef USE_OFR_FONTS
    if (dtb_font != 0) {
      return((uint16_t)(ofr.getTextWidth(msg) & 0x0000FFFF));
    } else {
  #endif
      return((uint16_t)(tft.textWidth(msg) & 0x0000FFFF));
  #ifdef USE_OFR_FONTS
    }
  #endif
}

uint16_t dtb_get_effective_draw_width(uint32_t draw_width) {
  uint16_t viewport_width = tft.getViewportWidth();

  if (viewport_width < 1) {
    return 1;
  }
  if (draw_width > 0 && draw_width < viewport_width) {
    return (uint16_t)draw_width;
  }
  return viewport_width;
}

uint16_t dtb_get_menu_row_width_limit() {
  uint16_t viewport_width = tft.getViewportWidth();

  if (viewport_width > 4) {
    return viewport_width - 4;
  }
  return viewport_width;
}

uint8_t dtb_fit_glcd_text_size(uint8_t text_size, uint16_t width_fit, const char* str) {
  if (text_size < 1) {
    text_size = 1;
  } else if (text_size > 7) {
    text_size = 7;
  }

  if (str == nullptr || width_fit < 1) {
    tft.setTextSize(text_size);
    return text_size;
  }

  while (text_size > 1) {
    tft.setTextSize(text_size);
    if (tft.textWidth(str) <= width_fit) {
      return text_size;
    }
    text_size--;
  }

  tft.setTextSize(1);
  return 1;
}

// this function is precalculating ofr_font_size and row_width for all lists
// it should be called only one time, after the system font is set/changed.
// this front-loads all the processor-intensive calculations required to determine the
// correct font size and render area for each list.
//
// if this was done on-the-fly everytime a list was rendered then we would see obvious
// screen lag/delay every time the list was drawn on screen. so this is being done to 
// improve UI performance
//
// this function needs to know how wide the area where the font will be rendered is.
// this is typically tft.width(), but I've made it a function argument so we can do 
// things like add gutterspace on either side of a list that this function
// doesn't need to know about.
//
// the ofr font should already be loaded at this point
void list_calculate_dynamic_font_properties() {
  uint8_t curr_list, curr_item, num_items;
  uint16_t font_height = 0, ofs_tmp;
  uint16_t list_width_limit = dtb_get_menu_row_width_limit();

  #ifdef USE_OFR_FONTS
    SERIAL_PRINT("dtb_font = ");
    SERIAL_PRINTLN(dtb_font);
  #endif

  // loop through all lists
  for (curr_list=0; curr_list<NUM_LISTS; curr_list++) {

    // initialize the render options we'll be calculating in a bit
    lists[curr_list].render_options.fit_text_size = lists[curr_list].render_options.text_size;
    lists[curr_list].render_options.ofr_font_size = 0;
    lists[curr_list].render_options.row_width = 0;

    // set ofr_font_size to 0 as it will not be used in this instance
    // set the GLCD font size and calculate font_height from that
    //tft.setTextFont(1);
    tft.setTextSize(lists[curr_list].render_options.text_size);
    font_height = tft.fontHeight();

    #ifdef USE_OFR_FONTS
      if (dtb_font == 0) {
    #endif

        // Keep a single fitted GLCD size per list so all rows in a menu feel cohesive.
        for (curr_item=0; curr_item<lists[curr_list].num_items; curr_item++) {
          ofs_tmp = dtb_fit_glcd_text_size(lists[curr_list].render_options.fit_text_size, list_width_limit, lists[curr_list].items[curr_item]);
          if (ofs_tmp < lists[curr_list].render_options.fit_text_size) {
            lists[curr_list].render_options.fit_text_size = ofs_tmp;
          }
        }
        tft.setTextSize(lists[curr_list].render_options.fit_text_size);

        // find the list item with the largest width when rendered and record to row_width
        for (curr_item=0; curr_item<lists[curr_list].num_items; curr_item++) {
          if (tft.textWidth(lists[curr_list].items[curr_item]) > lists[curr_list].render_options.row_width) {
            lists[curr_list].render_options.row_width = tft.textWidth(lists[curr_list].items[curr_item]);
          }
        }

    #ifdef USE_OFR_FONTS
      } else {

        // determine the font size to use when rendering this list
        for (curr_item=0; curr_item<lists[curr_list].num_items; curr_item++) {

          // calculate the font size that would be needed to fit within the dimensions of the viewport width and tft.fontHeight()
          //
          // would caluclateBoundingBox() be a better method?
          ofs_tmp = ofr.calculateFitFontSize(list_width_limit, tft.fontHeight(), ofr.getLayout(), lists[curr_list].items[curr_item]);

          // if this value is smaller than what is currently stored, record the font size
          if (lists[curr_list].render_options.ofr_font_size < 1 || ofs_tmp < lists[curr_list].render_options.ofr_font_size) {
            lists[curr_list].render_options.ofr_font_size = ofs_tmp;
          }
        }

        // set the font size to the newly determined ofr_font_size
        ofr.setFontSize(lists[curr_list].render_options.ofr_font_size);

        // record the font height
        lists[curr_list].render_options.ofr_font_height = dtb_get_font_height();

        // now determine the list item with the largest width when rendered with the currently set font size
        for (curr_item=0; curr_item<lists[curr_list].num_items; curr_item++) {
          ofs_tmp = ofr.getTextWidth(lists[curr_list].items[curr_item]);
          if (ofs_tmp > lists[curr_list].render_options.row_width) {
            lists[curr_list].render_options.row_width = ofs_tmp;
          }
        }
      }
    #endif
  }
}

// calculate and set the font size
void dtb_set_font_size(uint8_t text_size, uint16_t width_fit, const char* str) {
	static const char test_str[] = "Hy";
  uint32_t tmp_size = 0;
  width_fit = dtb_get_effective_draw_width(width_fit);

  if (text_size > 0 && text_size < 8) {
    #ifdef USE_OFR_FONTS
      if (dtb_font == 0 && str != nullptr) {
        dtb_fit_glcd_text_size(text_size, width_fit, str);
      } else {
        tft.setTextSize(text_size);
      }
    #else
      if (str != nullptr) {
        dtb_fit_glcd_text_size(text_size, width_fit, str);
      } else {
        tft.setTextSize(text_size);
      }
    #endif
  }

  #ifdef USE_OFR_FONTS
    if (dtb_font != 0) {
      if (text_size > 7) {
        ofr.setFontSize(text_size);
      } else {

        // stupid bug; ofr.calculateFitFontSize() doesn't light a height that's below something like 16, so...
        if (text_size == 1) {
          tmp_size = tft.fontHeight() * 2;
        } else {
          tmp_size = tft.fontHeight();
        }
      
        if (str == nullptr) {
          tmp_size = ofr.calculateFitFontSize(width_fit, tmp_size, ofr.getLayout(), test_str);
        } else {
          tmp_size = ofr.calculateFitFontSize(width_fit, tmp_size, ofr.getLayout(), str);
        }

        // readajust tmp_size if we're doing the bug workaround
        if (text_size == 1) {
          tmp_size = tmp_size / 2;
        }
        if (tmp_size < 1) {
          tmp_size = 1;
        }
        ofr.setFontSize(tmp_size);
      }
    }
  #endif
}

// set alignment of ofr using the tft datum values
void set_ofr_alignment_by_datum(uint8_t d) {
  #ifdef USE_OFR_FONTS
  switch (d) {
    default:
    case L_BASELINE:
      ofr.setAlignment(Align::Left);
      break;
    case C_BASELINE:
      ofr.setAlignment(Align::Center);
      break;
    case R_BASELINE:
      ofr.setAlignment(Align::Right);
      break;
    case TL_DATUM:
      ofr.setAlignment(Align::TopLeft);
      break;
    case TC_DATUM:
      ofr.setAlignment(Align::TopCenter);
      break;
    case TR_DATUM:
      ofr.setAlignment(Align::TopRight);
      break;
    case ML_DATUM:
      ofr.setAlignment(Align::MiddleLeft);
      break;
    case MC_DATUM:
      ofr.setAlignment(Align::MiddleCenter);
      break;
    case MR_DATUM:
      ofr.setAlignment(Align::MiddleRight);
      break;
    case BL_DATUM:
      ofr.setAlignment(Align::BottomLeft);
      break;
    case BC_DATUM:
      ofr.setAlignment(Align::BottomCenter);
      break;
    case BR_DATUM:
      ofr.setAlignment(Align::BottomRight);
      break;
  }
  #endif // USE_OFR_FONTS
}

// NOTES
//   text_size can be a pixel height or it can be a GLCD multiplier. GLCD multiplier cannot be larger than 7, nor zero. 
//   draw_width is only used if it's a non-GLCD font and we're being asked to calculate the font size (based on GLCD text_size)
//
void dtb_draw_string(const char* str, int32_t draw_x, int32_t draw_y, uint32_t draw_width, uint16_t text_size, uint16_t text_color, uint8_t text_datum) {
  uint8_t height_offset = 0;
  uint16_t fit_width = dtb_get_effective_draw_width(draw_width);

  if (str == nullptr) {
    return;
  }

  // if text_size is 0 then assume the font has already been set outside of this function and it just needs to be drawn.
  if (text_size > 0) {

    // set GLCD font; not necessary, we're never setting this value to anything else in the code
    //tft.setTextFont(1);

    #ifdef USE_OFR_FONTS
      // if text_size is less than 8 assume it's a GLCD multiplier, otherwise assume it's a pixel height for use with OFR
      if (dtb_font == 0) {
    #endif

        if (draw_width > 0) {
          dtb_fit_glcd_text_size((uint8_t)(text_size & 0x00FF), fit_width, str);
        } else {
          tft.setTextSize((uint8_t)(text_size & 0x00FF));
        }

    #ifdef USE_OFR_FONTS
      } else if (text_size < 8) {
        tft.setTextSize((uint8_t)(text_size & 0x00FF));
      }
    #endif
  }

  #ifdef USE_OFR_FONTS

  // if we're using the GLCD font
  if (dtb_font == 0) {

  #endif  // USE_OFR_FONTS

    // set alignment
    tft.setTextDatum(text_datum);

    // set color
    tft.setTextColor(text_color);

    // draw the string
    tft.drawString(str, draw_x, draw_y);

  #ifdef USE_OFR_FONTS

  // else we're using a TTF font
  } else {

    // set text alignment
    set_ofr_alignment_by_datum(text_datum);

    if (text_size != 0) {

      // if text_size is less than 8 it's a GLCD font size
      // calculate and set the actual font size based on tft.fontHeight()
      if (text_size < 8) {

        if (dtb_fonts[dtb_font - 1].y_scale != 1) {
          ofr.setFontSize(ofr.calculateFitFontSize(fit_width, (uint32_t)(tft.fontHeight() * dtb_fonts[dtb_font - 1].y_scale), ofr.getLayout(), str));
        } else {
          ofr.setFontSize(ofr.calculateFitFontSize(fit_width, tft.fontHeight(), ofr.getLayout(), str));
        }

        // so this is dumb. some TTF fonts don't behave as expected with ofr.calculateFitFontSize() and the 
        // calculated font size is TOO BIG. this is a check to resolve those situations and, as a backup, set
        // the font size to tft.fontHeight(); -- it's a rough workaround, that's what i get for trying to use
        // dynamic font sizes. 
        if (dtb_get_font_height() > tft.fontHeight()) {
            ofr.setFontSize(tft.fontHeight());
        }

        // we're sizing based on the GLCD font size. our new font height will likely be smaller than the
        // GLCD font height, so we add a small offset as half the difference between the two font heights
        // to vertically center this text within the area where the GLCD font would have been
        height_offset = (tft.fontHeight() - dtb_get_font_height()) / 2;

      // else assume text_size is a pixel height
      } else {
        ofr.setFontSize(text_size);
        if (draw_width > 0 && ofr.getTextWidth(str) > fit_width) {
          ofr.setFontSize(ofr.calculateFitFontSize(fit_width, dtb_get_font_height(), ofr.getLayout(), str));
        }
      }
    }

    // adjust text position
    draw_y += height_offset;

    // further text position adjustments
    if (dtb_fonts[dtb_font - 1].y_offset != 0) {
      draw_y += (dtb_get_font_height() * dtb_fonts[dtb_font - 1].y_offset);
    }

    // draw the string
    //ofr.setBackgroundColor(TFT_BLACK);
    //ofr.setFontColor(text_color, TFT_BLACK);
    ofr.setFontColor(text_color);
    ofr.drawString(str, draw_x, draw_y, text_color, ofr.getBackgroundColor(), ofr.getLayout());
  }

  #endif  // USE_OFR_FONTS
}

// load the font specified by dtb_font
void dtb_load_font() {

  #ifdef USE_OFR_FONTS

    // load the new font
    ofr.unloadFont();
    if (dtb_font > NUM_FONTS) {
      dtb_font = DEFAULT_DTB_FONT;
    }
    if (dtb_font != 0 && ofr.loadFont(dtb_fonts[dtb_font - 1].data, dtb_fonts[dtb_font - 1].size)) {
      dtb_font = DEFAULT_DTB_FONT;
      if (dtb_font != 0 && ofr.loadFont(dtb_fonts[dtb_font - 1].data, dtb_fonts[dtb_font - 1].size)) {
        dtb_font = 0;
      }
    }

    // recalculate font properties based on new font
    list_calculate_dynamic_font_properties();
  #endif
}

personality_t* get_droid_personality(uint8_t id) {
  uint8_t i;
  for (i=0; i<DROID_PERSONALITIES_SIZE; i++) {
    if (droid_personalities[i].id == id) {
      return(&droid_personalities[i]);
    }
  }
  return nullptr;
}

affiliaton_t* get_droid_affiliation(uint8_t id) {
  uint8_t i;
  for (i=0; i<DROID_AFFILIATIONS_SIZE; i++) {
    if (droid_affiliations[i].id == id) {
      return(&droid_affiliations[i]);
    }
  }
  return nullptr;
}

location_t* get_location(uint8_t id) {
  uint8_t i;
  for (i=0; i<LOCATIONS_SIZE; i++) {
    if (locations[i].id == id) {
      return(&locations[i]);
    }
  }
  return nullptr;
}

void init_advertisement_data(const char* name) {
  if (pAdvertisementData != nullptr) {
    delete pAdvertisementData;
  }
  pAdvertisementData = new BLEAdvertisementData();
  if (name == nullptr) {
    pAdvertisementData->setName(ble_adv_name);
  } else {
    pAdvertisementData->setName(name);
  }
}

void load_payload_location_beacon_data() {
  memcpy(payload, SWGE_LOCATION_BEACON_PAYLOAD, sizeof(uint8_t) * PAYLOAD_SIZE);
  payload_len = PAYLOAD_SIZE;
}

void load_payload_droid_beacon_data() {
  memcpy(payload, SWGE_DROID_BEACON_PAYLOAD, sizeof(uint8_t) * PAYLOAD_SIZE);
  payload_len = PAYLOAD_SIZE;
}

// populate beacon struct + payload with one of the datapad iBeacons
void set_ibeacon(uint8_t idx) {
  if (idx >= DATAPADS_SIZE) {
    idx = 0;
  }
  current_datapad_idx = idx;
  beacon.type = IBEACON;
  beacon.setting[0] = idx;        // remembered for back-navigation; the rest of beacon.setting[] is unused
  SERIAL_PRINT("Creating an IBEACON: ");
  SERIAL_PRINTLN(datapads[idx].name);
}

void load_payload_ibeacon_data(uint8_t idx) {
  if (idx >= DATAPADS_SIZE) {
    idx = 0;
  }
  memcpy(payload, datapads[idx].payload, IBEACON_PAYLOAD_SIZE);
  payload_len = IBEACON_PAYLOAD_SIZE;
}

// populate beacon struct with a droid beacon
void set_droid_beacon(uint8_t personality) {
  uint8_t i;
  personality_t* p;
  affiliaton_t* a; 
  char msg[MSG_LEN_MAX];

  // get the personality
  p = get_droid_personality(personality);

  // do we have a valid personality?
  if (p != nullptr && p->emulatable != 0) {

    // create a droid beacon
    SERIAL_PRINTLN("Creating a DROID beacon.");
    beacon.type = DROID;

    // populate the beacon values
    beacon.setting[BEACON_PARAM_DROID_ID]     = p->id;
    beacon.setting[BEACON_PARAM_DROID_AFFL]   = p->affiliation;
    beacon.setting[BEACON_PARAM_DROID_PAIRED] = 1;
    beacon.setting[BEACON_PARAM_DROID_BDROID] = 0;

    // output personality name to serial
    SERIAL_PRINT("  Personality: ");
    SERIAL_PRINTLN(p->name);

    // output affiliation to serial
    SERIAL_PRINT("  Affiliation: ");
    a = get_droid_affiliation(p->affiliation);
    if (a != nullptr) {
      SERIAL_PRINTLN(a->name);
    } else {
      snprintf(msg, MSG_LEN_MAX, msg_unknown_int, p->affiliation);
      SERIAL_PRINTLN(msg);
    }
  } else {
    // pick a random droid beacon, only pick from emulatable personalities
    set_droid_beacon(emulatable_personalities[esp_random() % emulatable_personalities_size]->id);
  }
}

void set_location_beacon(uint8_t location) {
  uint8_t i;
  location_t* l;

  // get the location
  l = get_location(location);

  // do we have a valid location?
  if (l != nullptr) {

    // create a location beacon
    beacon.type = LOCATION;
    SERIAL_PRINTLN("Creating a LOCATION beacon.");

    // set the bacon location value
    beacon.setting[BEACON_PARAM_LCTN_ID] = l->id;
    SERIAL_PRINT("  Location: ");
    SERIAL_PRINTLN(l->name);

    // set reaction interval (in minutes), could go as high as 19, but keeping it low on purpose
    beacon.setting[BEACON_PARAM_LCTN_REACT] = DEFAULT_BEACON_REACTION_TIME;
    SERIAL_PRINT("  Interval: ");
    SERIAL_PRINTLN(beacon.setting[BEACON_PARAM_LCTN_REACT]);

    // set minimum RSSI for droid to react; while this value is stored as an unsigned value, think of it as a negative value in dBm; e.g. 38 = -38dBm
    beacon.setting[BEACON_PARAM_LCTN_RSSI] = 38;
    SERIAL_PRINT("  Minimum RSSI: -");
    SERIAL_PRINT(beacon.setting[BEACON_PARAM_LCTN_RSSI]);
    SERIAL_PRINTLN("dBm");

    // is this even necessary? we don't use this...
    beacon.setting[BEACON_PARAM_LCTN_PARAM4] = 0;
  } else {

    // if no valid location was found, select one of the known locations at random
    set_location_beacon(locations[esp_random() % LOCATIONS_SIZE].id);
  }
}

// Same as set_location_beacon() but selects the entry by index in locations[] so we can
// distinguish between presets that share the same location id (e.g. Resistance vs Depot+,
// Dok Ondar's vs Mkt+, Alert vs Alert!).  Applies the per-entry react_byte / rssi overrides
// when present so the alarm-style and -58dBm preset families produce the documented bytes.
void set_location_beacon_by_index(uint8_t idx) {
  if (idx >= LOCATIONS_SIZE) {
    set_location_beacon(0);  // fall back to random
    return;
  }

  current_location_idx = idx;
  location_t* l = &locations[idx];
  beacon.type = LOCATION;
  beacon.setting[BEACON_PARAM_LCTN_ID]      = l->id;
  beacon.setting[BEACON_PARAM_LCTN_REACT]   = DEFAULT_BEACON_REACTION_TIME;
  beacon.setting[BEACON_PARAM_LCTN_RSSI]    = 38;
  beacon.setting[BEACON_PARAM_LCTN_PARAM4]  = 0;

  // alarm-style preset: react_byte == 0xFF triggers the encoder's ALARM path (>= 22 mins)
  if (l->react_byte == 0xFF) {
    beacon.setting[BEACON_PARAM_LCTN_REACT] = 22;
  }
  // stronger-RSSI preset (e.g. 0x18BA01 family). 0xBA = -58dBm; we store the magnitude.
  if (l->rssi == 0x3A) {
    beacon.setting[BEACON_PARAM_LCTN_RSSI] = 58;
  }

  SERIAL_PRINTLN("Creating a LOCATION beacon (preset).");
  SERIAL_PRINT("  Location: ");
  SERIAL_PRINTLN(l->name);
}

// populate the global beacon variable with random(ish) values
void set_random_beacon() {

  SERIAL_PRINTLN("Generating a random beacon.");
  if (esp_random() % 2)  {
    set_droid_beacon(0);      // create a DROID beacon
  } else {
    set_location_beacon(0);   // create a LOCATION beacon
  }
}

// generate a random location beacon for the next rotating beacon. make sure we don't repeat locations.
void set_rotating_beacon() {
  static uint8_t old_id = 0;

  SERIAL_PRINTLN("Generating the next rotating beacon.");

  do {
    set_location_beacon(0);

  // generate a random beacon, again, if it's the same value as the previous (old) beacon
  } while (old_id == beacon.setting[BEACON_PARAM_LCTN_ID]);

  old_id = beacon.setting[BEACON_PARAM_LCTN_ID];
}

// set the advertising payload based on the data in the global beacon variable
void set_payload_from_beacon() {

  // IBEACON beacon type (Apple iBeacon for Datapad / Play Disney Parks app)
  if (beacon.type == IBEACON) {
    load_payload_ibeacon_data(beacon.setting[0]);

  // DROID beacon type
  } else if (beacon.type == DROID) {
    load_payload_droid_beacon_data();

    // set whether or not the droid appears as being paired with a remote
    payload[5] = 0x01 + (beacon.setting[BEACON_PARAM_DROID_PAIRED] > 0 ? 0x80 : 0x00);

    // set affiliation
    payload[6] = 0x80 + (beacon.setting[BEACON_PARAM_DROID_AFFL] * 2);

    // set personality chip id
    payload[7] = beacon.setting[BEACON_PARAM_DROID_ID];

  // LOCATION beacon type
  } else {
    load_payload_location_beacon_data();

    // set location
    payload[4] = beacon.setting[BEACON_PARAM_LCTN_ID];

    // set reaction interval
    //   setting is in MINUTES; encoded byte = setting * 12 (since each tick = 5s, 12 ticks = 60s).
    //   setting >= 22 emits the spec's 0xFF "alarm" interval (~21 min between droid reactions),
    //   matching the 0x0A04xxFFA601 alarm-style location beacons.
    {
      uint8_t r = beacon.setting[BEACON_PARAM_LCTN_REACT];
      if (r >= 22) {
        payload[5] = 0xFF;
      } else {
        payload[5] = r * 12;
      }
    }

    // set minimum RSSI for reaction
    payload[6] = 0x80 + beacon.setting[BEACON_PARAM_LCTN_RSSI];
  }

  // load the payload into the advertisement data

  // workaround ESP32 3.x core incompatibilities with 2.x
  // payload_len reflects the active beacon type's actual byte count (8 for SWGE droid/location, 25 for iBeacons).
  #if ESP_ARDUINO_VERSION_MAJOR >= 3
    // new code
    String payload_wstr;
    for(uint8_t i=0;i<payload_len;i++) {
      payload_wstr += (char)payload[i];
    }
    pAdvertisementData->setManufacturerData(payload_wstr);
  #else
    // old code
    pAdvertisementData->setManufacturerData(std::string(reinterpret_cast<char*>(payload), payload_len));
  #endif

  pAdvertising->setAdvertisementData(*pAdvertisementData);

  // prevent connection attempts while advertising a beacon
  pAdvertising->setAdvertisementType(BLE_GAP_CONN_MODE_NON);
  pAdvertising->setScanFilter(false, true);
}

// BLE Advertising Callback
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    uint8_t *rawdata, rawdata_len, i, pos;
    uint16_t mfid;
    char msg[MSG_LEN_MAX];

    // do not add this object if it doesn't have the name DROID or if it does not have manufacturer data
    if (advertisedDevice.getName() != "DROID" || !advertisedDevice.haveManufacturerData()) {
      return;
    }

    // exract manufacturer's id from device's manufacturer data

    // workaround ESP32 3.x core incompatibilities with 2.x
    #if ESP_ARDUINO_VERSION_MAJOR >= 3
      // new code
      unsigned char rawdata_buf[PAYLOAD_SIZE+1];  // the +1 is a hack; the returned data will include an end-of-string character, so +1 the buffer size for that
      advertisedDevice.getManufacturerData().getBytes(rawdata_buf, PAYLOAD_SIZE+1);
      rawdata = (uint8_t *)&rawdata_buf[0];
    #else
      // old code
      rawdata = (uint8_t*)advertisedDevice.getManufacturerData().data();
    #endif

    rawdata_len = advertisedDevice.getManufacturerData().length();
    mfid = rawdata[0] + (rawdata[1] << 8);

    SERIAL_PRINTLN("*");
    SERIAL_PRINTLN("*");
    SERIAL_PRINTLN("*");

    SERIAL_PRINT("rawdata_len: ");
    SERIAL_PRINTLN(rawdata_len);

    SERIAL_PRINT("data: ");
    for(i=0;i<rawdata_len;i++) {
      SERIAL_PRINT(" ");
      SERIAL_PRINT2(rawdata[i], HEX);
    }
    SERIAL_PRINTLN("");

    SERIAL_PRINTLN("*");
    SERIAL_PRINTLN("*");
    SERIAL_PRINTLN("*");

    delay(500);

    // do not add this device if it does not have a manufacturer's id of 0x0183 (Disney)
    if (rawdata_len != 8 || mfid != 0x0183) {
      return;
    }

    // the droid list is sorted from strongest to weakest signal strength (RSSI) value
    // locate where in the list this new droid will be inserted
    pos = 0;
    if (droid_count > 0) {

      // find where to insert droid into list; higher RSSI come first in list
      for (pos = 0; pos < droid_count; pos++) {
        if (droids[pos].pAdvertisedDevice->getRSSI() < advertisedDevice.getRSSI()) {
          break;
        }
      }

      // this droid's signal strength is too weak, do not insert it into the list
      if (droid_count == MAX_DROIDS && pos == MAX_DROIDS) {
        return;
      }

      // push droids with a lower RSSI down the list
      for (i = droid_count; i > pos; i--) {
        if (i < MAX_DROIDS) {
          droids[i] = droids[i - 1];                  // move droid to new position in array
          droids[i - 1].pAdvertisedDevice = nullptr;  // clear the previous position in prep for the new droid
        } else {
          delete droids[i - 1].pAdvertisedDevice;
        }
      }
    }

    SERIAL_PRINT("chipid: ");
    SERIAL_PRINTLN2(rawdata[rawdata_len - 1], HEX);

    SERIAL_PRINT("affid: ");
    SERIAL_PRINTLN2((rawdata[rawdata_len - 2] - 0x80) / 2, HEX);

    // store found droid's information
    droids[pos].chipid = rawdata[rawdata_len - 1];
    droids[pos].affid = (rawdata[rawdata_len - 2] - 0x80) / 2;

    // need to store a pointer to an AdvertisedDevice object for this device because
    // it's the only thing i can use to connect to the droid successfully

    // first check to make sure there isn't already a pointer and if there is delete it
    if (droids[pos].pAdvertisedDevice != nullptr) {
      SERIAL_PRINTLN("delete old AdvertsidedDevice object");
      delete droids[pos].pAdvertisedDevice;
    }

    // store the droid's AdvertsiedDevice object
    droids[pos].pAdvertisedDevice = new BLEAdvertisedDevice(advertisedDevice);

    SERIAL_PRINT("Droid Found -- BD_ADDR: ");
    SERIAL_PRINT(droids[pos].pAdvertisedDevice->getAddress().toString().c_str());
    SERIAL_PRINT(", RSSI: ");
    SERIAL_PRINTLN(advertisedDevice.getRSSI());

    // increment counter
    if (droid_count < MAX_DROIDS) {
      droid_count++;
    }
  }
};

void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  uint8_t i, j;

  SERIAL_PRINT("Notify callback for characteristic: ");
  SERIAL_PRINT(pBLERemoteCharacteristic->getUUID().toString().c_str());
  SERIAL_PRINT(" of data length ");
  SERIAL_PRINTLN(length);
  SERIAL_PRINT("data: ");

  // SERIAL_PRINTLN((char*)pData);
  for (i = 0; i < length; i++) {

    // print hex value
    SERIAL_PRINT2(pData[i], HEX);
    SERIAL_PRINT(" ");

    // newline
    if ((i % 16 == 0 && i > 0) || (i == (length - 1))) {
      if (i == (length - 1)) {
        j = i - (i % 16);
      } else {
        j = i - 16;
      }
      SERIAL_PRINT("    [ ");
      for (; j < i; j++) {
        SERIAL_PRINT((char)pData[j]);
      }
      SERIAL_PRINTLN(" ]");
    }
  }
  SERIAL_PRINTLN();
}

void droid_disconnect() {
  // disconnect from any previous connection
  if (pClient != nullptr && pClient->isConnected()) {
    SERIAL_PRINTLN("Disconnecting from droid.");
    pClient->disconnect();
  }
}

bool droid_connect() {
  return droid_connect((const char *)nullptr);
}

bool droid_connect(const char *addr_to_connect) {
  uint8_t login_value[] = { 0x22, 0x20, 0x01 };
  uint8_t cmd_a[] = { 0x27, 0x42, 0x0f, 0x44, 0x44, 0x00, 0x1f, 0x07 };
  uint8_t cmd_b[] = { 0x27, 0x42, 0x0f, 0x44, 0x44, 0x00, 0x18, 0x00 };

  // end any current connection
  droid_disconnect();

  // attempt to connect to the droid
  if (addr_to_connect == nullptr) {
    SERIAL_PRINT("Connecting to ");
    SERIAL_PRINTLN(droids[current_droid].pAdvertisedDevice->getAddress().toString().c_str());
     if (!pClient->connect(droids[current_droid].pAdvertisedDevice)) {
      SERIAL_PRINTLN("Connection failed.");
      return false;
    }

    // store this address to NVS
    #ifdef USE_NVS
      SERIAL_PRINTLN("Saving droid BLE address.");
      preferences.putString("saved_addr", droids[current_droid].pAdvertisedDevice->getAddress().toString().c_str());
    #endif
  } else {
    SERIAL_PRINT("Manually connecting to ");
    SERIAL_PRINTLN(addr_to_connect);
    BLEAddress connect_address(addr_to_connect);
    if (!pClient->connect(connect_address, BLE_ADDR_RANDOM)) {
      SERIAL_PRINTLN("Connection failed.");
      return false;
    }
  }

  // locate the service we want to talk to
  pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    SERIAL_PRINTLN("Failed to find droid service.");
    droid_disconnect();
    return false;
  }

  // get the characteristic that receives commands
  pRemoteCharacteristicCmd = pRemoteService->getCharacteristic(cmdUUID);
  if (pRemoteCharacteristicCmd == nullptr) {
    SERIAL_PRINTLN("Failed to find droid chracteristic.");
    droid_disconnect();
    return false;
  }
  pRemoteCharacteristicCmd->registerForNotify(notifyCallback);

  // get the characteristic that receives notifications from the droid
  pRemoteCharacteristicNotify = pRemoteService->getCharacteristic(notifyUUID);
  if (pRemoteCharacteristicNotify == nullptr) {
    SERIAL_PRINTLN("Failed to find droid chracteristic.");
    droid_disconnect();
    return false;
  }
  pRemoteCharacteristicNotify->registerForNotify(notifyCallback);

  // send command to 'login'; will also disable droid's beacon until disconnect()
  pRemoteCharacteristicCmd->writeValue(login_value, sizeof(login_value));
  delay(100);
  pRemoteCharacteristicCmd->writeValue(login_value, sizeof(login_value));
  delay(100);

  // make droid play activation noise
  pRemoteCharacteristicCmd->writeValue(cmd_a, sizeof(cmd_a));
  delay(100);
  pRemoteCharacteristicCmd->writeValue(cmd_b, sizeof(cmd_b));
  delay(100);

  return true;
}

void droid_play_track() {
  static uint8_t cmd_set_group[]  = { 0x27, 0x42, 0x0f, 0x44, 0x44, 0x00, 0x1f, 0x07 };
  static uint8_t cmd_play_track[] = { 0x27, 0x42, 0x0f, 0x44, 0x44, 0x00, 0x18, 0x00 };

  if (pClient->isConnected() && pRemoteCharacteristicCmd != nullptr) {
    cmd_set_group[7] = current_group % 12;
    cmd_play_track[7] = current_track & 0xFF;

    SERIAL_PRINT("Playing track ");
    SERIAL_PRINT(current_group);
    SERIAL_PRINT("-");
    SERIAL_PRINTLN(current_track);

    pRemoteCharacteristicCmd->writeValue(cmd_set_group, sizeof(cmd_set_group));
    delay(100);
    pRemoteCharacteristicCmd->writeValue(cmd_play_track, sizeof(cmd_play_track));
    delay(100);
  }
}

void droid_play_next_track() {
  static uint8_t cmd_play_next_track[] = { 0x26, 0x42, 0x0f, 0x43, 0x44, 0x00, 0x1c };
  if (pClient->isConnected() && pRemoteCharacteristicCmd != nullptr) {
    pRemoteCharacteristicCmd->writeValue(cmd_play_next_track, sizeof(cmd_play_next_track));
    delay(100);
  }
}

void droid_set_volume() {
  static uint8_t cmd_set_volume[] = { 0x27, 0x42, 0x0f, 0x44, 0x44, 0x00, 0x0e, 0x1f };

  if (pClient->isConnected() && pRemoteCharacteristicCmd != nullptr) {
    cmd_set_volume[7] = (uint8_t)((float)droid_volume / 3.2); // where's the 3.2 come from? 
                                                              // assumed good values for volume are 0x00 - 0x1f (31).
                                                              // 100 / 31 = 3.2 (ish)
                                                              // the 0 to 100 volume scale is just cosmetic

    SERIAL_PRINT("New volume: 0x");
    SERIAL_PRINTLN2(cmd_set_volume[7], HEX);

    pRemoteCharacteristicCmd->writeValue(cmd_set_volume, sizeof(cmd_set_volume));
    delay(100);
  }
}

void ble_scan() {

  // if we have any connections, disconnect.
  droid_disconnect();

  // get ready to count some droids
  droid_count = 0;

  // workaround ESP32 3.x core incompatibilities with 2.x
  #if ESP_ARDUINO_VERSION_MAJOR >= 3
    // BLE scan
    BLEScanResults *foundDevices = pBLEScan->start(BLE_SCAN_TIME, false);

    // Report results via Serial
    SERIAL_PRINT(foundDevices->getCount());
  #else
    // BLE scan
    BLEScanResults foundDevices = pBLEScan->start(BLE_SCAN_TIME, false);

    // Report results via Serial
    SERIAL_PRINT(foundDevices.getCount());
  #endif

  SERIAL_PRINTLN(" BLE devices seen.");

  // delete results fromBLEScan buffer to release memory
  pBLEScan->clearResults();
}

void wait_times_set_status(const char* status) {
  snprintf(wait_times_status, sizeof(wait_times_status), "%s", status);
}

void wait_times_reset() {
  uint8_t i;

  for (i = 0; i < GALAXY_EDGE_WAIT_COUNT; i++) {
    galaxy_edge_waits[i].found = false;
    galaxy_edge_waits[i].is_open = false;
    galaxy_edge_waits[i].wait_time = -1;
    galaxy_edge_waits[i].last_updated[0] = '\0';
  }
}

bool wait_times_wifi_configured() {
  uint8_t i;

  for (i = 0; i < WIFI_NETWORK_COUNT; i++) {
    if (known_wifi_networks[i].ssid != nullptr &&
        strlen(known_wifi_networks[i].ssid) > 0 &&
        strcmp(known_wifi_networks[i].ssid, "your_wifi_name") != 0) {
      return true;
    }
  }

  return false;
}

uint8_t wifi_configured_network_count() {
  uint8_t i;
  uint8_t count = 0;

  for (i = 0; i < WIFI_NETWORK_COUNT; i++) {
    if (known_wifi_networks[i].ssid != nullptr &&
        strlen(known_wifi_networks[i].ssid) > 0 &&
        strcmp(known_wifi_networks[i].ssid, "your_wifi_name") != 0) {
      count++;
    }
  }

  return count;
}

void wifi_set_status(const char* status) {
  snprintf(wifi_last_status, sizeof(wifi_last_status), "%s", status);
}

void wifi_off() {
  WiFi.disconnect(false);
  WiFi.mode(WIFI_OFF);
  wifi_set_status(msg_wifi_disconnected);
}

bool wifi_connect_known() {
  uint32_t start_time;
  uint8_t i;

  if (WiFi.status() == WL_CONNECTED) {
    wifi_set_status(msg_wifi_connected);
    return true;
  }

  if (!wait_times_wifi_configured()) {
    wifi_set_status(msg_wifi_not_set);
    return false;
  }

  WiFi.mode(WIFI_STA);

  for (i = 0; i < WIFI_NETWORK_COUNT; i++) {
    if (known_wifi_networks[i].ssid == nullptr ||
        strlen(known_wifi_networks[i].ssid) == 0 ||
        strcmp(known_wifi_networks[i].ssid, "your_wifi_name") == 0) {
      continue;
    }

    WiFi.disconnect(false);
    delay(100);
    WiFi.begin(known_wifi_networks[i].ssid, known_wifi_networks[i].password);
    wifi_set_status(msg_wifi_connecting);

    start_time = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start_time < WIFI_CONNECT_TIMEOUT) {
      delay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
      wifi_set_status(msg_wifi_connected);
      return true;
    }
  }

  wifi_set_status(msg_wifi_failed);
  return false;
}

bool read_json_bool_field(const String& json, int start, int end, const char* field, bool& value) {
  String marker = String("\"") + field + "\":";
  int pos = json.indexOf(marker, start);

  if (pos < 0 || pos > end) {
    return false;
  }

  pos += marker.length();
  if (json.startsWith("true", pos)) {
    value = true;
    return true;
  }
  if (json.startsWith("false", pos)) {
    value = false;
    return true;
  }

  return false;
}

bool read_json_int_field(const String& json, int start, int end, const char* field, int16_t& value) {
  String marker = String("\"") + field + "\":";
  int pos = json.indexOf(marker, start);
  int value_end;

  if (pos < 0 || pos > end) {
    return false;
  }

  pos += marker.length();
  value_end = pos;
  while (value_end <= end && (isDigit(json[value_end]) || json[value_end] == '-')) {
    value_end++;
  }

  value = (int16_t)json.substring(pos, value_end).toInt();
  return true;
}

bool read_json_string_field(const String& json, int start, int end, const char* field, char* value, size_t value_size) {
  String marker = String("\"") + field + "\":\"";
  int pos = json.indexOf(marker, start);
  int value_end;
  size_t copy_len;

  if (pos < 0 || pos > end || value_size < 1) {
    return false;
  }

  pos += marker.length();
  value_end = json.indexOf("\"", pos);
  if (value_end < 0 || value_end > end) {
    return false;
  }

  copy_len = value_end - pos;
  if (copy_len >= value_size) {
    copy_len = value_size - 1;
  }

  memcpy(value, json.c_str() + pos, copy_len);
  value[copy_len] = '\0';
  return true;
}

bool parse_wait_time_ride(const String& json, wait_time_ride_t& ride) {
  String name_marker = String("\"name\":\"") + ride.name + "\"";
  int name_pos = json.indexOf(name_marker);
  int ride_start;
  int ride_end;

  if (name_pos < 0) {
    return false;
  }

  ride_start = json.lastIndexOf("{", name_pos);
  ride_end = json.indexOf("}", name_pos);
  if (ride_start < 0 || ride_end < 0) {
    return false;
  }

  if (!read_json_bool_field(json, ride_start, ride_end, "is_open", ride.is_open)) {
    return false;
  }
  if (!read_json_int_field(json, ride_start, ride_end, "wait_time", ride.wait_time)) {
    return false;
  }

  if (!read_json_string_field(json, ride_start, ride_end, "last_updated", ride.last_updated, sizeof(ride.last_updated))) {
    ride.last_updated[0] = '\0';
  }

  ride.found = true;
  return true;
}

bool parse_wait_times(const String& json, uint8_t park_index) {
  uint8_t i;
  bool found_any = false;

  for (i = 0; i < GALAXY_EDGE_WAIT_COUNT; i++) {
    if (galaxy_edge_waits[i].park_index != park_index) {
      continue;
    }

    if (parse_wait_time_ride(json, galaxy_edge_waits[i])) {
      found_any = true;
    }
  }

  return found_any;
}

void fetch_wait_times() {
  WiFiClientSecure client;
  HTTPClient http;
  uint8_t i;
  int http_code;
  String response;
  bool was_connected = WiFi.status() == WL_CONNECTED;
  bool found_any = false;
  bool fetch_failed = false;

  wait_times_reset();

  if (!wait_times_wifi_configured()) {
    wait_times_set_status(msg_wifi_not_set);
    return;
  }

  wait_times_set_status(msg_wait_times_active);
  if (!wifi_connect_known()) {
    wait_times_set_status(msg_wifi_failed);
    if (!was_connected) {
      wifi_off();
    }
    return;
  }

  client.setInsecure();
  client.setHandshakeTimeout(12);
  http.setTimeout(WAIT_TIMES_HTTP_TIMEOUT);

  for (i = 0; i < GALAXY_EDGE_PARK_COUNT; i++) {
    if (!http.begin(client, galaxy_edge_wait_parks[i].url)) {
      fetch_failed = true;
      continue;
    }

    http_code = http.GET();
    if (http_code != HTTP_CODE_OK) {
      SERIAL_PRINT("Wait-times HTTP status: ");
      SERIAL_PRINTLN(http_code);
      fetch_failed = true;
      http.end();
      continue;
    }

    response = http.getString();
    http.end();

    if (parse_wait_times(response, i)) {
      found_any = true;
    }
  }

  if (found_any) {
    wait_times_set_status("");
  } else if (fetch_failed) {
    wait_times_set_status(msg_fetch_failed);
  } else {
    wait_times_set_status(msg_parse_failed);
  }

  if (!was_connected) {
    wifi_off();
  }
}

void reset_screen(void) {
  tft.resetViewport();
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setCursor(0, 0);

  #ifdef LILYGO_AMOLED
    tft.fillRect(0, 0, amoled.width(), amoled.height(), TFT_BLACK);
    amoled.pushColors(0, 0, amoled.width(), amoled.height(), (uint16_t *)tft.getPointer());
  #endif
}

void flush_display(void) {
  #ifdef LILYGO_AMOLED
    amoled.pushColors(0, 0, amoled.width(), amoled.height(), (uint16_t *)tft.getPointer());
  #endif
}

//void display_list(const char **items, uint8_t num_items) {
void display_list(uint8_t list_index) {
  uint8_t rows, row_padding, max_padding, i, menu_text_size;
  uint16_t row_height, row_width, text_color, font_height, max_row_width, view_height;
  int16_t  y = 0;

  menu_text_size = lists[list_index].render_options.fit_text_size;
  if (menu_text_size < 1) {
    menu_text_size = lists[list_index].render_options.text_size;
  }

  // get the pixel height of the font
  #ifdef USE_OFR_FONTS
    if (dtb_font != 0) {
      SERIAL_PRINT("ofr_font_size: ");
      SERIAL_PRINTLN(lists[list_index].render_options.ofr_font_size);
      font_height = lists[list_index].render_options.ofr_font_height; //lists[list_index].render_options.ofr_font_size;
    } else {
  #endif
      SERIAL_PRINT("text_size: ");
      SERIAL_PRINTLN(menu_text_size);
      tft.setTextSize(menu_text_size);
      font_height = tft.fontHeight();
  #ifdef USE_OFR_FONTS
    }
  #endif

  // calculate vertical padding (aka row_padding) limits and update row_padding
  // if currently set value is outside of available limits
  // 
  // since font size can be changed via a define, or that this code might run on a screen
  // that i haven't tested with, check to make sure padding isn't so big that we can't show 
  // at least 2 menu items on the screen.
  //
  // divide the available space by 6; padding top/bottom of each item (4) + leave an extra gap at the very top and very bottom
  row_padding = lists[list_index].render_options.text_padding;
  view_height = tft.getViewportHeight();
  if (view_height <= (font_height * 2)) {
    max_padding = 0;
  } else {
    max_padding = (view_height - (font_height * 2))/6;
  }
  if (max_padding < 1) {
    row_padding = 0;
  } else if (max_padding < row_padding) {
    row_padding = max_padding;
  }

  // calculate how tall each row will be; font height + top and bottom padding + border width
  row_height = font_height + (row_padding * 2);
  if (row_height < 1) {
    row_height = 1;
  }

  SERIAL_PRINT("row_height: ");
  SERIAL_PRINTLN(row_height);
  SERIAL_PRINT("font_height: ");
  SERIAL_PRINTLN(font_height);
  SERIAL_PRINT("row_padding: ");
  SERIAL_PRINTLN(row_padding);

  // calculate how many rows will fit within the screen
  rows = view_height / row_height;
  if (rows < 1) {
    rows = 1;
  }

  // pickup the precalculated row_width
  row_width = lists[list_index].render_options.row_width;
  max_row_width = dtb_get_menu_row_width_limit();

  // add some horizontal padding, if there's room for it.
  //
  // should calculate a tft.getViewPortWidth() value earlier in this function
  // which has side gutter space subtracted ahead of time...
  if (row_width < max_row_width) {
    row_width = row_width + ((max_row_width - row_width)/4);
  }
  if (row_width > max_row_width) {
    row_width = max_row_width;
  }
  if (row_width < 1) {
    row_width = 1;
  }

  // LIST RENDERING METHODOLOGY:
  //   to give a sense of moving through a menu that's taller than the screen, i want the selected item to be
  //   in a position on the screen relative to its location in the list. so the first item is rendered at the top,
  //   the last item is rendered at the bottom, and intervening items render somewhere in the middle
  //
  //   i tested several different approaches and i found this "feels" right given the limited screen space

  // is the list larger than the screen?
  if (lists[list_index].num_items > rows && rows >= 1)  {
    uint16_t visible_span = 0;

    if (view_height > row_height) {
      visible_span = view_height - row_height;
    }

    // where should the selected item appear on screen?
    y = (visible_span / (lists[list_index].num_items - 1)) * selected_item;

    // where should the list start on (or off) screen in order to put the selected item at the 
    // previously calculated location?
    y -= (row_height * selected_item);

  // entire list will fit on the screen, vertically center it
  } else {
    uint32_t list_height = (uint32_t)row_height * lists[list_index].num_items;

    // y is set to where the top of the first menu item will be rendered
    if (list_height < view_height) {
      y = (view_height - list_height)/2;
    } else {
      y = 0;
    }
  }

  // draw the list, starting at the previously calculated position (y)
  for (i = 0; i < lists[list_index].num_items; i++) {

    // is the current list item selected?
    if (i == selected_item) {

      // draw a box around the list item
      tft.drawRect(
        ((tft.getViewportWidth() - row_width) / 2),
        y,
        row_width,
        row_height,
        lists[list_index].render_options.selected_border_color
      );

      // set the text color for selected list items
      //tft.setTextColor(list_options.selected_text_color);
      text_color = lists[list_index].render_options.selected_text_color;
    } else {

      // this item is not selected, set its text color to the normal color
      //tft.setTextColor(list_options.text_color);
      text_color = lists[list_index].render_options.text_color;
    }

    // draw the list item on the screen
    if (lists[list_index].render_options.ofr_font_size != 0) {
      dtb_draw_string(lists[list_index].items[i], tft.getViewportWidth()/2,  y + row_padding + 1, row_width, lists[list_index].render_options.ofr_font_size, text_color, TC_DATUM);   // TC_DATUM
    } else {
      dtb_draw_string(lists[list_index].items[i], tft.getViewportWidth()/2,  y + row_padding + 1, row_width, menu_text_size, text_color, TC_DATUM);       // TC_DATUM
    }

    // increment y for the next list item
    y += row_height;
  }
}

void display_captioned_menu(const char* caption, uint8_t list_index) {
  uint8_t h;

  // draw menu caption
  dtb_draw_string(caption, tft.width()/2, MENU_SELECT_CAPTION_TEXT_PADDING, dtb_get_menu_row_width_limit(), MENU_SELECT_CAPTION_TEXT_SIZE, MENU_SELECT_CAPTION_TEXT_COLOR, TC_DATUM);

  // calculate viewport dimensions for subsequent menu
  h = tft.fontHeight() + (MENU_SELECT_CAPTION_TEXT_PADDING * 2);
  tft.setViewport(0, h, tft.width(), tft.height()-h);

  // display the menu
  display_list(list_index);
}


void display_set_interval() {
  char msg[MSG_LEN_MAX];
  uint16_t y, content_height;
  uint8_t gap, bfs;

  // line 1: set beacon interval
  // line 2: %d sec
  // line 3: gap
  // line 4: start 

  // set the default font size to 1 and get the pixel height of the text at this size; this will be the base unit used to calculate the size of everything on the screen
  tft.setTextSize(1);
  bfs = tft.fontHeight();

  // calclualte a gap between the beacon title and the activate option
  gap = (bfs * BEACON_CONTROL_TEXT_SIZE) / 2;

  // calculate the height of the content to vertically align everything
  content_height = (bfs * BEACON_INTERVAL_TITLE_TEXT_SIZE) + (bfs * BEACON_INTERVAL_VALUE_TEXT_SIZE) + gap + (bfs * BEACON_INTERVAL_SET_TEXT_SIZE) + (BEACON_INTERVAL_TEXT_PADDING * 3);

  // calculate where to begin drawing text  
  y = (tft.getViewportHeight() - content_height)/2;

  // display title
  dtb_draw_string(msg_set_beacon_interval, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, BEACON_INTERVAL_TITLE_TEXT_SIZE, BEACON_INTERVAL_TITLE_COLOR, TC_DATUM);
  y += (bfs * BEACON_INTERVAL_TITLE_TEXT_SIZE) + BEACON_INTERVAL_TEXT_PADDING;

  // display current interval
  snprintf(msg, MSG_LEN_MAX, msg_int_sec, beacon_rotate_interval * 10);
  dtb_draw_string(msg, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, BEACON_INTERVAL_VALUE_TEXT_SIZE, (selected_item == 0 ? BEACON_INTERVAL_VALUE_COLOR_S : BEACON_INTERVAL_VALUE_COLOR), TC_DATUM);
  y += (bfs * BEACON_INTERVAL_VALUE_TEXT_SIZE) + gap + (BEACON_INTERVAL_TEXT_PADDING * 2) ;

  // display set interval action
  dtb_draw_string(msg_set_interval, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, BEACON_INTERVAL_SET_TEXT_SIZE, (selected_item == 1 ? BEACON_INTERVAL_SET_COLOR_S : BEACON_INTERVAL_SET_COLOR), TC_DATUM);
}

void display_beacon_control() {
  uint16_t y, content_height;
  uint8_t gap, bfs;
  char msg[MSG_LEN_MAX];
  personality_t *p;
  location_t* l;

  // line 1: beacon type
  // line 2: beacon id
  // line 3: gap
  // line 3: beacon state

  // set the default font size to 1 and get the pixel height of the text at this size; this will be the base unit used to calculate the size of everything on the screen
  tft.setTextSize(1);
  bfs = tft.fontHeight();
  
  // calclualte a gap between the beacon title and the activate option
  gap = (bfs * BEACON_CONTROL_TEXT_SIZE) / 2;

  // calculate the height of the content to vertically align everything
  content_height = (bfs * BEACON_CONTROL_TYPE_TEXT_SIZE) + (bfs * BEACON_CONTROL_TEXT_SIZE *2) + gap + (BEACON_CONTROL_TEXT_PADDING * 3);

  // calculate where to begin drawing text  
  y = (tft.getViewportHeight() - content_height)/2;

  // display beacon type
  if (beacon.type == DROID) {
    snprintf(msg, MSG_LEN_MAX, "%s %s", msg_droid, msg_beacon);
  } else if (beacon.type == IBEACON) {
    snprintf(msg, MSG_LEN_MAX, "%s", msg_datapad);
  } else {
    snprintf(msg, MSG_LEN_MAX, "%s %s", msg_location, msg_beacon);
  }
  dtb_draw_string(msg, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, BEACON_CONTROL_TYPE_TEXT_SIZE, BEACON_CONTROL_TYPE_COLOR, TC_DATUM);

  // adjust position for next line
  y += tft.fontHeight() + BEACON_CONTROL_TEXT_PADDING;

  // display beacon id
  if (beacon.type == DROID) {
    p = get_droid_personality(beacon.setting[BEACON_PARAM_DROID_ID]);
    if (p != nullptr) {
      snprintf(msg, MSG_LEN_MAX, "%s", p->name);
    } else {
      snprintf(msg, MSG_LEN_MAX, msg_unknown_int, beacon.setting[BEACON_PARAM_DROID_ID]);
    }
  } else if (beacon.type == IBEACON) {
    // show the picked datapad's friendly name (Disneyland / Walt Disney World)
    uint8_t di = beacon.setting[0];
    if (di >= DATAPADS_SIZE) di = 0;
    snprintf(msg, MSG_LEN_MAX, "%s", datapads[di].name);
  } else {
    // prefer the explicitly-picked preset's name (handles duplicate-ID entries like Depot+/Mkt+/Alert!/FO!)
    if (current_location_idx < LOCATIONS_SIZE && locations[current_location_idx].id == beacon.setting[BEACON_PARAM_LCTN_ID]) {
      snprintf(msg, MSG_LEN_MAX, "%s", locations[current_location_idx].name);
    } else {
      l = get_location(beacon.setting[BEACON_PARAM_LCTN_ID]);
      if (l != nullptr) {
        snprintf(msg, MSG_LEN_MAX, "%s", l->name);
      } else {
        snprintf(msg, MSG_LEN_MAX, msg_unknown_int, beacon.setting[BEACON_PARAM_LCTN_ID]);
      }
    }
  }
  dtb_draw_string(msg, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, BEACON_CONTROL_TEXT_SIZE, BEACON_CONTROL_ID_COLOR, TC_DATUM);

  // adjust position for next line
  y += tft.fontHeight() + (BEACON_CONTROL_TEXT_PADDING * 2) + gap;

  // display beacon state
  if (state == BEACON_ACTIVE) {
    dtb_draw_string(msg_beacon_active,   tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, BEACON_CONTROL_TEXT_SIZE, BEACON_CONTROL_ACTIVE_COLOR,   TC_DATUM);
  } else {
    dtb_draw_string(msg_activate_beacon, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, BEACON_CONTROL_TEXT_SIZE, BEACON_CONTROL_INACTIVE_COLOR, TC_DATUM);
  }
}

// display the splash screen seen when the program starts
void display_splash() {
  uint16_t y, c, batt_mv;
  uint16_t w = tft.getViewportWidth();
  uint16_t h = tft.getViewportHeight();
  uint16_t footer_y;
  char msg[MSG_LEN_MAX];

  // The startup screen is tiny on the original T-Display, so keep it as a
  // compact poster instead of trying to fit full attribution strings.
  tft.setTextSize(1);
  footer_y = h - tft.fontHeight() - 3;

  // title
  y = 8;
  dtb_draw_string(msg_title, w / 2, y, w - 14, SPLASH_TEXT_SIZE + 1, SPLASH_TITLE_COLOR, TC_DATUM);
  y += tft.fontHeight() + 2;

  // short identity line
  dtb_draw_string(msg_splash_subtitle, w / 2, y, w - 18, 1, SPLASH_SUBTITLE_COLOR, TC_DATUM);

  // press any button...
  y = (h / 2) + 6;
  dtb_draw_string(msg_continue1, w / 2, y, w - 18, SPLASH_TEXT_SIZE, SPLASH_TEXT_COLOR, TC_DATUM);
  y += tft.fontHeight() + 2;
  dtb_draw_string(msg_continue2, w / 2, y, w - 18, 1, DEFAULT_TEXT_COLOR, TC_DATUM);

  // battery voltage
  batt_mv = (analogRead(BAT_ADC_PIN) * 2 * 3.3 * 1000) / 4096;
  c = SPLASH_VERSION_COLOR;
  if (batt_mv < 3400) {
    c = C565(128,0,0);        // need to charge the battery
  } else if (batt_mv < 3800) {
    c = C565(96,96,0);        // battery is getting low
  } else if (batt_mv < 4400) {
    c = C565(0,128,0);        // battery is charged
  } else {
    c = SPLASH_VERSION_COLOR; // you're probably on USB
  }

  tft.drawFastHLine(10, footer_y - 5, w - 20, SPLASH_VERSION_COLOR);

  snprintf(msg, MSG_LEN_MAX, "%s %.2fV", (batt_mv < 4400 ? "BAT" : "PWR"), (batt_mv / (float)1000));
  dtb_draw_string(msg, 2, footer_y, w / 3, 1, c, TL_DATUM);
  dtb_draw_string(msg_splash_mod, w / 2, footer_y, w / 3, 1, SPLASH_VERSION_COLOR, TC_DATUM);
  dtb_draw_string(msg_version, w - 2, footer_y, w / 3, 1, SPLASH_VERSION_COLOR, TR_DATUM);
}

void display_scanner_results() {
  char msg[MSG_LEN_MAX];
  uint16_t y = 0;
  uint8_t i;
  const char* name = nullptr;
  affiliaton_t* a = nullptr;
  personality_t* p = nullptr;

  // display header
  dtb_draw_string(msg_droid_report, tft.getViewportWidth()/2, 0, DEFAULT_TEXT_FIT_WIDTH, DROID_REPORT_TEXT_SIZE, DROID_REPORT_COLOR, TC_DATUM);

  // add a gap after the header
  if (droid_count > 0) {

    // find where to start printing droid details so that it is vertically centered
    tft.setTextSize(1);
    y = (tft.height() - ((tft.fontHeight() * (DROID_REPORT_TEXT_SIZE + 1)) + (tft.fontHeight() * DROID_REPORT_TEXT_SIZE * 3)))/2;

    // look for a known ble address first; this takes precedence over anything else
    for (i=0; i<NAMED_DROID_SIZE; i++) {
      if (memcmp(named_droids[i].bleaddr, droids[current_droid].pAdvertisedDevice->getAddress().getNative(), ESP_BD_ADDR_LEN) == 0) {
        name = named_droids[i].name;
        break;
      }
    }

    // if a ble address match was not found, grab the details of the droid based on ID
    if (name == nullptr) {

      // search known personalities
      p = get_droid_personality(droids[current_droid].chipid);

      // if a known personality was found, extract its name
      if (p != nullptr) {
        name = get_droid_personality(droids[current_droid].chipid)->name;
      }
    }

    // print personality name
    if (name != nullptr) {
      dtb_draw_string(name, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DROID_REPORT_TEXT_SIZE + 1, DROID_REPORT_PERSONALITY_COLOR, TC_DATUM);
    } else {
      snprintf(msg, MSG_LEN_MAX, msg_unknown_int, droids[current_droid].chipid);
      dtb_draw_string(msg, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DROID_REPORT_TEXT_SIZE + 1, DROID_REPORT_PERSONALITY_COLOR, TC_DATUM);
    }

    // print droid affiliation
    y += tft.fontHeight();
    a = get_droid_affiliation(droids[current_droid].affid);
    if (a != nullptr) {
      dtb_draw_string(a->name, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DROID_REPORT_TEXT_SIZE, DROID_REPORT_AFFILIATION_COLOR, TC_DATUM);
    } else {
      snprintf(msg, MSG_LEN_MAX, msg_unknown_int, droids[current_droid].affid);
      dtb_draw_string(msg, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DROID_REPORT_TEXT_SIZE, DROID_REPORT_AFFILIATION_COLOR, TC_DATUM);
    }

    // print Bluetooth MAC address
    y += tft.fontHeight();
    dtb_draw_string(droids[current_droid].pAdvertisedDevice->getAddress().toString().c_str(), tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DROID_REPORT_TEXT_SIZE, DROID_REPORT_ADDRESS_COLOR, TC_DATUM);

    // print RSSI
    y += tft.fontHeight();
    snprintf(msg, MSG_LEN_MAX, msg_rssi, droids[current_droid].pAdvertisedDevice->getRSSI());
    dtb_draw_string(msg, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DROID_REPORT_TEXT_SIZE, DROID_REPORT_RSSI_COLOR, TC_DATUM);

    // print
    snprintf(msg, MSG_LEN_MAX, msg_d_of_d, current_droid + 1, droid_count);
    dtb_draw_string(msg, tft.getViewportWidth()/2, tft.getViewportHeight(), DEFAULT_TEXT_FIT_WIDTH, DROID_REPORT_TEXT_SIZE, DROID_REPORT_COLOR, BC_DATUM);

  // display message that no droids were found
  } else {

    // set the font size
    dtb_set_font_size(DROID_REPORT_TEXT_SIZE + 1, DEFAULT_TEXT_FIT_WIDTH, msg_no_droids1);

    // calculate where to start rendering the message
    y = (tft.height()/2) - dtb_get_font_height();

    // render the message
    dtb_draw_string(msg_no_droids1, 
      tft.width()/2, y, 0, 0, ACTION_RESULT_NG_TEXT_COLOR, TC_DATUM
    );
    dtb_draw_string(msg_no_droids2, 
      tft.width()/2, y + dtb_get_font_height(), 0, 0, ACTION_RESULT_NG_TEXT_COLOR, TC_DATUM
    );
  }
}

void display_wait_times() {
  char msg[MSG_LEN_MAX];
  uint16_t y;
  uint16_t color;
  uint8_t i;
  const char* updated = nullptr;

  dtb_draw_string(msg_wait_times_title, tft.getViewportWidth()/2, 0, DEFAULT_TEXT_FIT_WIDTH, DEFAULT_TEXT_SIZE, TFT_ORANGE, TC_DATUM);

  if (state == WAIT_TIMES_LOADING) {
    dtb_draw_string(msg_wait_times_active, tft.getViewportWidth()/2, (tft.getViewportHeight()/2) - tft.fontHeight(), DEFAULT_TEXT_FIT_WIDTH, ACTION_TEXT_SIZE, ACTION_TEXT_COLOR, TC_DATUM);
  } else if (strlen(wait_times_status) > 0) {
    dtb_draw_string(wait_times_status, tft.getViewportWidth()/2, (tft.getViewportHeight()/2) - tft.fontHeight(), DEFAULT_TEXT_FIT_WIDTH, ACTION_TEXT_SIZE, ACTION_RESULT_NG_TEXT_COLOR, TC_DATUM);
  } else {
    tft.setTextSize(1);
    y = tft.fontHeight() + 10;

    for (i = 0; i < GALAXY_EDGE_WAIT_COUNT; i++) {
      if (!galaxy_edge_waits[i].found) {
        snprintf(msg, MSG_LEN_MAX, "%s --", galaxy_edge_waits[i].label);
        color = DEFAULT_TEXT_COLOR;
      } else if (!galaxy_edge_waits[i].is_open) {
        snprintf(msg, MSG_LEN_MAX, "%s %s", galaxy_edge_waits[i].label, msg_closed);
        color = ACTION_RESULT_NG_TEXT_COLOR;
      } else {
        snprintf(msg, MSG_LEN_MAX, "%s %d MIN", galaxy_edge_waits[i].label, galaxy_edge_waits[i].wait_time);
        color = ACTION_RESULT_OK_TEXT_COLOR;
      }

      dtb_draw_string(msg, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DEFAULT_TEXT_SIZE, color, TC_DATUM);
      y += tft.fontHeight() + 4;

      if (updated == nullptr && galaxy_edge_waits[i].last_updated[0] != '\0') {
        updated = galaxy_edge_waits[i].last_updated;
      }
    }

    if (updated != nullptr && strlen(updated) >= 16) {
      snprintf(msg, MSG_LEN_MAX, "UTC %.5s", updated + 11);
      dtb_draw_string(msg, tft.getViewportWidth(), tft.getViewportHeight(), tft.getViewportWidth()/2, 1, DEFAULT_TEXT_COLOR, BR_DATUM);
    }
  }

  dtb_draw_string(msg_queue_times_source, 0, tft.getViewportHeight(), tft.getViewportWidth()/2, 1, DEFAULT_TEXT_COLOR, BL_DATUM);
}

void display_wifi_settings() {
  char msg[MSG_LEN_MAX];
  uint16_t y;
  bool connected = WiFi.status() == WL_CONNECTED;

  dtb_draw_string(msg_settings, tft.getViewportWidth()/2, 0, DEFAULT_TEXT_FIT_WIDTH, DEFAULT_TEXT_SIZE, TFT_ORANGE, TC_DATUM);

  if (state == SETTINGS_WIFI_CONNECTING) {
    dtb_draw_string(msg_wifi_connecting, tft.getViewportWidth()/2, (tft.getViewportHeight()/2) - tft.fontHeight(), DEFAULT_TEXT_FIT_WIDTH, ACTION_TEXT_SIZE, ACTION_TEXT_COLOR, TC_DATUM);
  } else {
    tft.setTextSize(1);
    y = tft.fontHeight() + 18;

    if (!wait_times_wifi_configured()) {
      dtb_draw_string(msg_wifi_not_set, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DEFAULT_TEXT_SIZE + 1, ACTION_RESULT_NG_TEXT_COLOR, TC_DATUM);
      y += tft.fontHeight() + 8;
    } else if (connected) {
      dtb_draw_string(msg_wifi_connected, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DEFAULT_TEXT_SIZE + 1, ACTION_RESULT_OK_TEXT_COLOR, TC_DATUM);
      y += tft.fontHeight() + 8;

      snprintf(msg, MSG_LEN_MAX, "%s", WiFi.SSID().c_str());
      dtb_draw_string(msg, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DEFAULT_TEXT_SIZE, DEFAULT_SELECTED_TEXT_COLOR, TC_DATUM);
      y += tft.fontHeight() + 6;

      snprintf(msg, MSG_LEN_MAX, "%s", WiFi.localIP().toString().c_str());
      dtb_draw_string(msg, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DEFAULT_TEXT_SIZE, DEFAULT_TEXT_COLOR, TC_DATUM);
    } else if (strlen(wifi_last_status) > 0) {
      dtb_draw_string(wifi_last_status, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DEFAULT_TEXT_SIZE + 1, ACTION_RESULT_NG_TEXT_COLOR, TC_DATUM);
      y += tft.fontHeight() + 8;
    } else {
      dtb_draw_string(msg_wifi_disconnected, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, DEFAULT_TEXT_SIZE + 1, DEFAULT_TEXT_COLOR, TC_DATUM);
      y += tft.fontHeight() + 8;
    }

    snprintf(msg, MSG_LEN_MAX, "%d NETWORKS", wifi_configured_network_count());
    dtb_draw_string(msg, tft.getViewportWidth()/2, tft.getViewportHeight(), DEFAULT_TEXT_FIT_WIDTH, 1, DEFAULT_TEXT_COLOR, BC_DATUM);
  }
}

void trivia_init() {
  uint8_t i;

  for (i = 0; i < TRIVIA_RECENT_COUNT; i++) {
    recent_trivia[i] = 0xFFFF;
  }
}

bool trivia_is_recent(uint16_t idx) {
  uint8_t i;

  for (i = 0; i < TRIVIA_RECENT_COUNT; i++) {
    if (recent_trivia[i] == idx) {
      return true;
    }
  }

  return false;
}

void trivia_remember(uint16_t idx) {
  recent_trivia[recent_trivia_pos] = idx;
  recent_trivia_pos = (recent_trivia_pos + 1) % TRIVIA_RECENT_COUNT;
}

void trivia_pick_random() {
  uint8_t attempts;
  uint16_t idx;

  if (TRIVIA_COUNT < 1) {
    current_trivia = 0;
    return;
  }

  for (attempts = 0; attempts < 40; attempts++) {
    idx = esp_random() % TRIVIA_COUNT;
    if (!trivia_is_recent(idx)) {
      current_trivia = idx;
      trivia_remember(idx);
      return;
    }
  }

  current_trivia = (current_trivia + 1) % TRIVIA_COUNT;
  trivia_remember(current_trivia);
}

uint16_t draw_wrapped_text(const char* text, int16_t x, int16_t y, int16_t w, uint8_t text_size, uint16_t color, uint8_t max_lines) {
  char line[TRIVIA_LINE_MAX];
  char candidate[TRIVIA_LINE_MAX];
  char word[TRIVIA_LINE_MAX];
  uint8_t word_len = 0;
  uint8_t lines = 0;
  uint16_t line_height;
  size_t i;
  size_t len;

  line[0] = '\0';
  tft.setTextDatum(TL_DATUM);
  dtb_set_font_size(text_size, w, nullptr);
  tft.setTextColor(color, TFT_BLACK, true);
  line_height = dtb_get_font_height() + 2;
  len = strlen(text);

  for (i = 0; i <= len; i++) {
    char c = text[i];

    if (c != ' ' && c != '\0' && c != '\n' && word_len < sizeof(word) - 1) {
      word[word_len++] = c;
      continue;
    }

    if (word_len > 0) {
      word[word_len] = '\0';
      if (line[0] == '\0') {
        snprintf(candidate, sizeof(candidate), "%s", word);
      } else {
        snprintf(candidate, sizeof(candidate), "%s %s", line, word);
      }

      if (line[0] != '\0' && dtb_get_text_width(candidate) > w) {
        dtb_draw_string(line, x, y, w, text_size, color, TL_DATUM);
        dtb_set_font_size(text_size, w, nullptr);
        y += line_height;
        lines++;
        if (lines >= max_lines) {
          return y;
        }
        snprintf(line, sizeof(line), "%s", word);
      } else {
        snprintf(line, sizeof(line), "%s", candidate);
      }
      word_len = 0;
    }

    if (c == '\n' || c == '\0') {
      if (line[0] != '\0') {
        dtb_draw_string(line, x, y, w, text_size, color, TL_DATUM);
        dtb_set_font_size(text_size, w, nullptr);
        y += line_height;
        lines++;
        line[0] = '\0';
        if (lines >= max_lines) {
          return y;
        }
      }
    }
  }

  return y;
}

void display_trivia() {
  char msg[MSG_LEN_MAX];
  uint16_t y;

  if (current_trivia >= TRIVIA_COUNT) {
    current_trivia = 0;
  }

  dtb_draw_string((state == TRIVIA_ANSWER ? msg_answer : msg_trivia), tft.getViewportWidth() / 2, 2, DEFAULT_TEXT_FIT_WIDTH, 1, TFT_ORANGE, TC_DATUM);

  snprintf(msg, MSG_LEN_MAX, "%d/%d", current_trivia + 1, TRIVIA_COUNT);
  dtb_draw_string(msg, tft.getViewportWidth() - 2, 2, tft.getViewportWidth() / 2, 1, DEFAULT_TEXT_COLOR, TR_DATUM);

  y = 22;
  if (state == TRIVIA_ANSWER) {
    y = draw_wrapped_text(trivia_items[current_trivia].question, 8, y, tft.getViewportWidth() - 16, 1, DEFAULT_TEXT_COLOR, 2);
    y += 4;
    draw_wrapped_text(trivia_items[current_trivia].answer, 8, y, tft.getViewportWidth() - 16, DEFAULT_TEXT_SIZE, ACTION_RESULT_OK_TEXT_COLOR, 3);
  } else {
    draw_wrapped_text(trivia_items[current_trivia].question, 8, y, tft.getViewportWidth() - 16, DEFAULT_TEXT_SIZE, DEFAULT_SELECTED_TEXT_COLOR, 5);
  }
}

bool is_screensaver_state() {
  return state == SCREENSAVER_DEATH_STAR ||
         state == SCREENSAVER_HYPERSPACE ||
         state == SCREENSAVER_TWIN_SUNS;
}

void draw_screensaver_stars(uint16_t frame, uint8_t count) {
  uint8_t i;
  int16_t x;
  int16_t y;
  uint16_t c;

  for (i = 0; i < count; i++) {
    x = (int16_t)((i * 47 + frame * 3) % tft.getViewportWidth());
    y = (int16_t)((i * 29 + frame) % tft.getViewportHeight());
    c = (i % 5 == 0) ? C565(120, 160, 220) : C565(90, 90, 110);
    tft.drawPixel(x, y, c);
  }
}

void draw_death_star_screensaver(uint16_t frame) {
  const float pi = 3.1415926;
  float a = frame * 0.065;
  int16_t w = tft.getViewportWidth();
  int16_t h = tft.getViewportHeight();
  int16_t cx = w / 2;
  int16_t cy = h / 2;
  int16_t r = min(w, h) / 3;
  int16_t x1, y1, x2, y2, x3, y3;
  float lon1, lon2, lat1, lat2;
  float z;
  uint16_t shade;
  int8_t lat;
  uint8_t lon;

  reset_screen();
  draw_screensaver_stars(frame, 32);

  tft.fillCircle(cx, cy, r, C565(52, 56, 62));
  tft.drawCircle(cx, cy, r, C565(150, 160, 170));

  for (lat = -2; lat < 2; lat++) {
    lat1 = (float)lat * pi / 7.5;
    lat2 = (float)(lat + 1) * pi / 7.5;

    for (lon = 0; lon < 12; lon++) {
      lon1 = a + (float)lon * pi / 6.0;
      lon2 = a + (float)(lon + 1) * pi / 6.0;
      z = (cos(lat1) * cos(lon1) + cos(lat1) * cos(lon2) + cos(lat2) * cos(lon1)) / 3.0;

      if (z > -0.15) {
        x1 = cx + (int16_t)(r * cos(lat1) * sin(lon1));
        y1 = cy + (int16_t)(r * sin(lat1));
        x2 = cx + (int16_t)(r * cos(lat1) * sin(lon2));
        y2 = cy + (int16_t)(r * sin(lat1));
        x3 = cx + (int16_t)(r * cos(lat2) * sin(lon1));
        y3 = cy + (int16_t)(r * sin(lat2));
        shade = (uint16_t)(82 + (z * 55));
        tft.fillTriangle(x1, y1, x2, y2, x3, y3, C565(shade, shade, shade + 8));
      }
    }
  }

  for (lat = -2; lat <= 2; lat++) {
    lat1 = (float)lat * pi / 7.5;
    y1 = cy + (int16_t)(r * sin(lat1));
    x1 = (int16_t)(r * cos(lat1));
    tft.drawLine(cx - x1, y1, cx + x1, y1, C565(78, 84, 92));
  }

  for (lon = 0; lon < 8; lon++) {
    lon1 = a + (float)lon * pi / 4.0;
    z = cos(lon1);
    if (z > -0.15) {
      x1 = cx + (int16_t)(r * sin(lon1));
      tft.drawLine(x1, cy - r + 4, x1, cy + r - 4, C565(70, 76, 84));
    }
  }

  y1 = cy + (int16_t)(r * 0.12);
  tft.drawLine(cx - r + 3, y1, cx + r - 3, y1, C565(18, 22, 28));
  tft.drawLine(cx - r + 6, y1 + 3, cx + r - 6, y1 + 3, C565(100, 106, 112));

  lon1 = a + 0.8;
  z = cos(lon1);
  if (z > -0.05) {
    x1 = cx + (int16_t)(r * 0.47 * sin(lon1));
    y1 = cy - (int16_t)(r * 0.32);
    x2 = max((int16_t)7, (int16_t)(r * (0.18 + (z * 0.10))));
    tft.fillCircle(x1, y1, x2, C565(38, 42, 48));
    tft.drawCircle(x1, y1, x2, C565(160, 166, 176));
    tft.drawCircle(x1, y1, x2 / 2, C565(80, 90, 104));
    tft.drawLine(x1 - x2, y1, x1 + x2, y1, C565(78, 84, 92));
    tft.drawLine(x1, y1 - x2, x1, y1 + x2, C565(78, 84, 92));
  }

  dtb_draw_string(msg_death_star, w - 2, h - 2, w / 2, 1, C565(50, 58, 70), BR_DATUM);
}

void draw_hyperspace_screensaver(uint16_t frame) {
  const float pi = 3.1415926;
  int16_t w = tft.getViewportWidth();
  int16_t h = tft.getViewportHeight();
  int16_t cx = w / 2;
  int16_t cy = h / 2;
  float a;
  int16_t d;
  int16_t len;
  int16_t x1, y1, x2, y2;
  uint8_t i;

  reset_screen();

  for (i = 0; i < 44; i++) {
    a = (float)((i * 31 + frame * 4) % 360) * pi / 180.0;
    d = 8 + ((i * 17 + frame * 6) % 130);
    len = 8 + (d / 4);
    x1 = cx + (int16_t)(cos(a) * d);
    y1 = cy + (int16_t)(sin(a) * d * 0.58);
    x2 = cx + (int16_t)(cos(a) * (d + len));
    y2 = cy + (int16_t)(sin(a) * (d + len) * 0.58);
    tft.drawLine(x1, y1, x2, y2, (i % 4 == 0) ? C565(90, 140, 255) : TFT_WHITE);
  }

  tft.drawCircle(cx, cy, 2 + (frame % 4), C565(80, 130, 255));
  dtb_draw_string(msg_hyperspace, w - 2, h - 2, w / 2, 1, C565(50, 58, 90), BR_DATUM);
}

void draw_twin_suns_screensaver(uint16_t frame) {
  int16_t w = tft.getViewportWidth();
  int16_t h = tft.getViewportHeight();
  int16_t horizon = (h * 2) / 3;
  int16_t shift = (frame / 3) % w;
  uint8_t i;

  tft.fillScreen(C565(7, 10, 22));
  draw_screensaver_stars(frame / 3, 20);

  tft.fillCircle((w / 2) - 32, horizon - 34, 18, C565(255, 190, 72));
  tft.fillCircle((w / 2) + 6, horizon - 25, 12, C565(255, 220, 110));
  tft.fillRect(0, horizon, w, h - horizon, C565(58, 34, 24));

  for (i = 0; i < 6; i++) {
    int16_t x = (int16_t)((i * 54 - shift) % (w + 60));
    if (x < -20) {
      x += w + 60;
    }
    tft.drawLine(x, horizon + 5, x + 30, horizon + 18, C565(118, 72, 42));
  }

  tft.fillTriangle(12, horizon, 30, horizon - 28, 47, horizon, C565(22, 18, 18));
  tft.fillRect(27, horizon - 40, 6, 40, C565(18, 16, 16));
  tft.fillTriangle(w - 42, horizon, w - 24, horizon - 22, w - 8, horizon, C565(20, 17, 17));
  tft.drawLine((w / 2) - 8, horizon - 10, (w / 2) - 8, horizon, C565(18, 16, 16));
  tft.drawCircle((w / 2) - 8, horizon - 14, 4, C565(18, 16, 16));

  dtb_draw_string(msg_twin_suns, w - 2, h - 2, w / 2, 1, C565(118, 78, 42), BR_DATUM);
}

void render_screensaver(bool force = false) {
  static uint32_t last_frame_time = 0;
  static uint16_t frame = 0;

  if (!force && millis() - last_frame_time < SCREENSAVER_FRAME_TIME) {
    return;
  }

  last_frame_time = millis();
  frame++;

  switch (state) {
    case SCREENSAVER_DEATH_STAR:
      draw_death_star_screensaver(frame);
      break;

    case SCREENSAVER_HYPERSPACE:
      draw_hyperspace_screensaver(frame);
      break;

    case SCREENSAVER_TWIN_SUNS:
      draw_twin_suns_screensaver(frame);
      break;
  }

  #ifdef LILYGO_AMOLED
    amoled.pushColors(0, 0, amoled.width(), amoled.height(), (uint16_t *)tft.getPointer());
  #endif
}

void display_sounds() {
  const char tst_msg[] = "88";
  char msg[MSG_LEN_MAX];
  uint16_t x, y, w;

  // line1: group
  // line2: track
  // line3: gap
  // line4: play

  // figure out a font size to use
  dtb_set_font_size(SOUNDS_TEXT_SIZE, DEFAULT_TEXT_FIT_WIDTH/2, msg_group);

  // let's figure out how to center all this 
  snprintf(msg, MSG_LEN_MAX, "%s: ", msg_group);
  x = dtb_get_text_width(msg);
  snprintf(msg, MSG_LEN_MAX, "%s: ", msg_track);
  y = dtb_get_text_width(msg);
  if (y > x) {
    x = y;
  }

  // the vertical split is located
  x = ((tft.getViewportWidth() - (x + dtb_get_text_width(tst_msg) + 10))/2) + x;

  // find the starting point to draw text
  y = (tft.fontHeight() * 4) + SOUNDS_TEXT_PADDING;
  y = (tft.getViewportHeight() - y)/2;

  // display group label
  snprintf(msg, MSG_LEN_MAX, "%s: ", msg_group);
  dtb_draw_string(msg, x, y, 0, 0, SOUNDS_GROUP_COLOR, TR_DATUM);

  // display group value
  snprintf(msg, MSG_LEN_MAX, "%d", (current_group + 1));
  dtb_draw_string(msg, x+10, y, 0, 0, (state == SOUND_GROUP ? SOUNDS_VALUE_SELECTED_COLOR : SOUNDS_VALUE_COLOR), TL_DATUM);

  // move next line
  y += tft.fontHeight() + SOUNDS_TEXT_PADDING;

  // display track label
  snprintf(msg, MSG_LEN_MAX, "%s: ", msg_track);
  dtb_draw_string(msg, x, y, 0, 0, SOUNDS_TRACK_COLOR, TR_DATUM);

  // display track value
  snprintf(msg, MSG_LEN_MAX, "%d", (current_track + 1));
  dtb_draw_string(msg, x+10, y, 0, 0, (state == SOUND_TRACK ? SOUNDS_VALUE_SELECTED_COLOR : SOUNDS_VALUE_COLOR), TL_DATUM);

  // move down to draw play button
  y += (tft.fontHeight() * 2);

  // display play button
  dtb_draw_string(msg_play, tft.getViewportWidth()/2, y, 0, 0, (state == SOUND_PLAY ? SOUNDS_VALUE_SELECTED_COLOR : SOUNDS_VALUE_COLOR), TC_DATUM);
}

void display_volume() {
  char msg[MSG_LEN_MAX];
  uint16_t content_height;
  uint16_t x, y, text_color;

  // line1: volume
  // line2: gap
  // line3: vol+ / vol-
  // line4: set volume

  // find where to start rendering content vertically
  tft.setTextSize(1);
  content_height = (tft.fontHeight() * VOLUME_TEXT_SIZE * 3) + VOLUME_TEXT_PADDING + (tft.fontHeight() * (VOLUME_TEXT_SIZE + 2));
  y = (tft.getViewportHeight() - content_height) / 2;

  // set volume color based on its value
  if (droid_volume < 50) {
    text_color = VOLUME_LOW_COLOR;
  } else if (droid_volume < 80) {
    text_color = VOLUME_MED_COLOR;
  } else if (droid_volume < 100) {
    text_color = VOLUME_HIGH_COLOR;
  } else {
    text_color = VOLUME_MAX_COLOR;
  }

  // display volume
  snprintf(msg, MSG_LEN_MAX, "%d", droid_volume);
  dtb_draw_string(msg, tft.getViewportWidth()/2, y, DEFAULT_TEXT_FIT_WIDTH, VOLUME_TEXT_SIZE + 2, text_color, TC_DATUM);

  // next line
  y += tft.fontHeight();
  
  // gap
  tft.setTextSize(VOLUME_TEXT_SIZE);
  y += tft.fontHeight();

  // why am i setting size with this instead of setting it in dtb_draw_string()?
  // because i'm dumb.
  // and dtb_draw_string calculates height_offset only if font size and render width are passed to dtb_set_font_size
  // so either i set font size here and ignore height_offset
  // or i pass width and size to all of these functions
  //
  // i should probably revisit dtb_draw_string and see if this goofball limitation/issue is relevant.. 
  // it only exists to operate under the assumption that an ofr fontsize can be passed, which I don't think i'm doing.
  //
  // oh, and hi, if you're reading comments. i apologize for the code. it's long. convoluted. and probably overly complicated
  // but this is how i code; i make it work, then i figure out if it can be done better. then i figure out if it can be done cleaner.
  // i haven't gotten to the better and cleaner parts, yet.
  dtb_set_font_size(VOLUME_TEXT_SIZE, DEFAULT_TEXT_FIT_WIDTH/2.5, msg_vol_inc);

  // vol+
  text_color = (state == VOLUME_UP ? VOLUME_SELECTED_TEXT_COLOR : VOLUME_TEXT_COLOR);
  dtb_draw_string(msg_vol_inc, (tft.getViewportWidth()/2) - VOLUME_TEXT_PADDING, y, 0, 0, text_color, TR_DATUM);

  // vol-
  text_color = (state == VOLUME_DOWN ? VOLUME_SELECTED_TEXT_COLOR : VOLUME_TEXT_COLOR);
  dtb_draw_string(msg_vol_dec, (tft.getViewportWidth()/2) + VOLUME_TEXT_PADDING, y, 0, 0, text_color, TL_DATUM);

  // next line
  y += (tft.fontHeight() + VOLUME_TEXT_PADDING);

  // set volume
  text_color = (state == VOLUME_TEST ? VOLUME_SELECTED_TEXT_COLOR : VOLUME_TEXT_COLOR);
  dtb_draw_string(msg_set_vol, tft.getViewportWidth()/2, y, 0, 0, text_color, TC_DATUM);
}

uint16_t draw_beacon_expert_line(uint16_t y, const char* label, const char* value, uint16_t value_color) {
  char label_msg[MSG_LEN_MAX];
  uint16_t label_width;
  uint16_t view_width = tft.getViewportWidth();
  uint16_t line_height;

  if (view_width < 8) {
    return y;
  }

  snprintf(label_msg, MSG_LEN_MAX, "%s: ", label);
  tft.setTextSize(BEACON_EXPERT_TEXT_SIZE);
  #ifdef USE_OFR_FONTS
    if (dtb_font != 0) {
      dtb_set_font_size(BEACON_EXPERT_TEXT_SIZE, view_width, nullptr);
    }
  #endif
  line_height = dtb_get_font_height() + 2;

  label_width = dtb_get_text_width(label_msg);
  if (label_width > view_width - 8) {
    label_width = view_width / 2;
  }

  dtb_draw_string(label_msg, 0, y, label_width, BEACON_EXPERT_TEXT_SIZE, BEACON_EXPERT_LABEL_COLOR, TL_DATUM);
  dtb_draw_string(value, label_width, y, view_width - label_width, BEACON_EXPERT_TEXT_SIZE, value_color, TL_DATUM);
  return y + line_height;
}

void display_beacon_expert() {
  char msg[MSG_LEN_MAX];
  const char* pname;
  uint8_t i;
  uint16_t y = 0;
  uint16_t value_color;
  personality_t* p = nullptr;
  affiliaton_t* a = nullptr;
  location_t* l = nullptr;

  /*
  // figure out the widest label so all the labels can be right-aligned; don't need to do this but...
  uint16_t label_width = 0;
  tft.setTextSize(BEACON_EXPERT_TEXT_SIZE);
  for(i=0;i<NUM_BEACON_PARAMS;i++) {
    if (tft.textWidth(msg_beacon_droid_param[i])>label_width) {
      label_width = tft.textWidth(msg_beacon_droid_param[i]);
    }
    if (tft.textWidth(msg_beacon_location_param[i])>label_width) {
      label_width = tft.textWidth(msg_beacon_location_param[i]);
    }
  }
  SERIAL_PRINT("label_width = ");
  SERIAL_PRINTLN(label_width);
  */

  // shrink the screen to give a little gap between the content and screen edge
  tft.setViewport(10, 10, tft.width()-20, tft.height()-20);
  tft.setTextSize(BEACON_EXPERT_TEXT_SIZE);

  // print the beacon type
  if (selected_item == 0) {
    value_color = BEACON_EXPERT_VALUE_SELECTED_COLOR;
  } else {
    value_color = BEACON_EXPERT_VALUE_COLOR;
  }
  if (beacon.type == DROID) {
    snprintf(msg, MSG_LEN_MAX, "%s", msg_droid);
  } else {
    snprintf(msg, MSG_LEN_MAX, "%s", msg_location);
  }
  y = draw_beacon_expert_line(y, msg_type, msg, value_color);

  // loop through all the beacon parameters, printing out the name and value of each
  for (i=0; i<NUM_BEACON_PARAMS; i++) {

    // get current parameter name
    pname = (beacon.type == DROID) ? msg_beacon_droid_param[i] : msg_beacon_location_param[i];

    // skip parameters that have no name
    if (pname == nullptr) {
      SERIAL_PRINT("Parameter ");
      SERIAL_PRINT(i);
      SERIAL_PRINTLN(" is not present. Skipping.");
      continue;
    }

    // print the parameter value
    // how the value is displayed will depend on what type of beacon it is
    if (beacon.type == DROID) {
      switch(i) {

        case BEACON_PARAM_DROID_ID:   // PARAMETER: personality
          p = get_droid_personality(beacon.setting[i]);
          if (p != nullptr) {
            snprintf(msg, MSG_LEN_MAX, "%s", p->name);
          } else {
            snprintf(msg, MSG_LEN_MAX, msg_unknown_int, beacon.setting[i]);
          }
          break;

        case BEACON_PARAM_DROID_AFFL: // PARAMETER: affiliation
          a = get_droid_affiliation(beacon.setting[i]);
          if (a != nullptr) {
            snprintf(msg, MSG_LEN_MAX, "%s", a->name);
          } else {
            snprintf(msg, MSG_LEN_MAX, msg_unknown_int, beacon.setting[i]);
          }
          break;

        case BEACON_PARAM_DROID_PAIRED: // PARAMETER: paired
        case BEACON_PARAM_DROID_BDROID: // PARAMETER: be a droid
          if (beacon.setting[i] == 0) {
            snprintf(msg, MSG_LEN_MAX, "%s", msg_no);
          } else {
            snprintf(msg, MSG_LEN_MAX, "%s", msg_yes);
          }
          break;

        default:
          snprintf(msg, MSG_LEN_MAX, "%d", beacon.setting[i]);
          break;
      }

    // location beacon
    } else {
      switch(i) {

        case BEACON_PARAM_LCTN_ID: // PARAMETER: location
          l = get_location(beacon.setting[i]);
          if (l != nullptr) {
            snprintf(msg, MSG_LEN_MAX, "%s", l->name);
          } else {
            snprintf(msg, MSG_LEN_MAX, msg_unknown_int, beacon.setting[i]);
          }
          break;

        case BEACON_PARAM_LCTN_REACT: // PARAMETER: reaction interval in minutes
          if (beacon.setting[i] >= 22) {
            snprintf(msg, MSG_LEN_MAX, "ALARM");   // emits 0xFF interval byte
          } else {
            snprintf(msg, MSG_LEN_MAX, msg_int_minutes, beacon.setting[i]);
          }
          break;

        case BEACON_PARAM_LCTN_RSSI: // PARAMETER: minimum rssi
          snprintf(msg, MSG_LEN_MAX, "-%ddBm", beacon.setting[i]);
          break;

        default:
          snprintf(msg, MSG_LEN_MAX, "%d", beacon.setting[i]);
          break;
      }
    }

    // select color of text depending on whether or not the current parameter is selected
    if (selected_item == i+1) {
      value_color = BEACON_EXPERT_VALUE_SELECTED_COLOR;
    } else {
      value_color = BEACON_EXPERT_VALUE_COLOR;
    }

    // display parameter value
    y = draw_beacon_expert_line(y, pname, msg, value_color);
  }

  // set text color based on whether or not beacon state is currently selected
  if (selected_item == NUM_BEACON_PARAMS+1) {
    value_color = BEACON_EXPERT_VALUE_SELECTED_COLOR;
  } else {
    value_color = BEACON_EXPERT_VALUE_COLOR;
  }

  // display the beacon state value
  if (state == BEACON_EXPERT_ACTIVE) {
    snprintf(msg, MSG_LEN_MAX, "%s", msg_beacon_active);
  } else {
    snprintf(msg, MSG_LEN_MAX, "%s", msg_activate_beacon);
  }
  draw_beacon_expert_line(y, msg_state, msg, value_color);
}

void sneaky_beacon_countdown() {
  static char msg[16];
  static uint16_t last_s = 0;
  uint16_t s;

/*
  #ifdef USE_OFR_FONTS
    BgFillMethod fm;
  #endif
*/

  s = (uint16_t)((next_beacon_time - millis())/1000);

  if (s != last_s) {

    // add leading spaces so it draws over the previous number if the number width shrinks
    snprintf(msg, 16, "    %d", s);

/*
    #ifdef USE_OFR_FONTS
      if (dtb_font != 0) {
        fm = ofr.getBackgroundFillMethod();
        ofr.setBackgroundFillMethod(BgFillMethod::Block);
        dtb_draw_string(msg, tft.getViewportWidth(), tft.getViewportHeight(), tft.getViewportWidth(), BEACON_CONTROL_COUNTDOWN_SIZE, BEACON_CONTROL_COUNTDOWN_COLOR, BR_DATUM);
        ofr.setBackgroundFillMethod(fm);
      } else {
    #endif
*/

        // have to bypass dtb_draw_string() in this case as it will not fill in the background when drawing text using tft.drawString()
        // and i do not feel like adding another parameter to that function and having to update a bunch of function calls
        //
        // also, when i make a rotating beacon inactive, should i reset the timer when i make it active again? (probably)
        tft.setTextDatum(BR_DATUM);
        dtb_fit_glcd_text_size(BEACON_CONTROL_COUNTDOWN_SIZE, tft.getViewportWidth(), msg);
        tft.setTextColor(BEACON_CONTROL_COUNTDOWN_COLOR, TFT_BLACK, true);
        tft.drawString(msg, tft.getViewportWidth(), tft.getViewportHeight());

/*
    #ifdef USE_OFR_FONTS
      }
    #endif
*/

    last_s = s;
  }
}

void render_current_screen() {
  reset_screen();

  switch (state) {

    case VOLUME_UP:              // display_volume()
    case VOLUME_DOWN:
    case VOLUME_TEST:
    case VOLUME_TESTING:
      display_volume();
      break;

    case SOUND_GROUP:            // display_sounds()
    case SOUND_TRACK:
    case SOUND_PLAY:
    case SOUND_PLAYING:
      display_sounds();
      break;

    case CONNECTED_MENU:         // display_connected_menu()
      display_captioned_menu(msg_select, LIST_CONNECTED_MENU);
      break;

    case SCANNER_CONNECTED:      // display_connected()

      // display connected message
      dtb_draw_string(msg_scanner_connected, 
        tft.getViewportWidth()/2, (tft.getViewportHeight()/2) - (dtb_get_font_height()/2), DEFAULT_TEXT_FIT_WIDTH, ACTION_TEXT_SIZE, ACTION_RESULT_OK_TEXT_COLOR, TC_DATUM
      );
      break;

    case SCANNER_CONNECT_FAILED: // display_connect_failed()
      dtb_set_font_size(ACTION_TEXT_SIZE, DEFAULT_TEXT_FIT_WIDTH, msg_scanner_connected);
      dtb_draw_string(msg_connect, 
        tft.getViewportWidth()/2, (tft.getViewportHeight()/2) - dtb_get_font_height(), 0, 0, ACTION_RESULT_NG_TEXT_COLOR, TC_DATUM
      );
      dtb_draw_string(msg_failed, 
        tft.getViewportWidth()/2, (tft.getViewportHeight()/2), 0, 0, ACTION_RESULT_NG_TEXT_COLOR, TC_DATUM
      );
      break;

    case SCANNER_CONNECTING:     // display_connecting()

      // display message to turn off droid remote at top of screen
      dtb_draw_string(msg_turn_off_remote1, tft.getViewportWidth()/2, 0, DEFAULT_TEXT_FIT_WIDTH, DROID_REPORT_TEXT_SIZE, DROID_REPORT_COLOR, TC_DATUM);
      dtb_draw_string(msg_turn_off_remote2, tft.getViewportWidth()/2, tft.fontHeight(), DEFAULT_TEXT_FIT_WIDTH, 0, DROID_REPORT_COLOR, TC_DATUM);

      // display CONNECTING message
      dtb_draw_string(msg_scanner_connecting, 
        tft.getViewportWidth()/2, (tft.getViewportHeight()/2) - (dtb_get_font_height()/2), DEFAULT_TEXT_FIT_WIDTH, ACTION_TEXT_SIZE, ACTION_TEXT_COLOR, TC_DATUM
      );
      break;

    case SCANNER_RESULTS:        // display_scanner_results()
      display_scanner_results();
      break;

    case WAIT_TIMES_LOADING:
    case WAIT_TIMES_RESULTS:
      display_wait_times();
      break;

    case TRIVIA_QUESTION:
    case TRIVIA_ANSWER:
      display_trivia();
      break;

    case SETTINGS_MENU:
      display_captioned_menu(msg_settings, LIST_SETTINGS_MENU);
      break;

    case SETTINGS_WIFI_CONNECTING:
    case SETTINGS_WIFI_STATUS:
    case SETTINGS_WIFI_OFF:
      display_wifi_settings();
      break;

    case SCREENSAVER_MENU:
      display_captioned_menu(msg_screensaver, LIST_SCREENSAVER_MENU);
      break;

    case SCREENSAVER_DEATH_STAR:
    case SCREENSAVER_HYPERSPACE:
    case SCREENSAVER_TWIN_SUNS:
      render_screensaver(true);
      break;

    case SCANNER_SCANNING:       // display_scanning()

      // for wahtever reason, using TC_DATUM instead of MC_DATUM renders more vertically-centered
      dtb_set_font_size(ACTION_TEXT_SIZE, DEFAULT_TEXT_FIT_WIDTH, msg_scanner_active);
      dtb_draw_string(msg_scanner_active, 
        tft.getViewportWidth()/2, (tft.getViewportHeight()/2) - (dtb_get_font_height()/2), 0, 0, ACTION_TEXT_COLOR, TC_DATUM
      );
      break;

    case BEACON_EXPERT_ACTIVE:  // display_beacon_expert()
    case BEACON_EXPERT:
      display_beacon_expert();
      break;

    case BEACON_ACTIVATE:        // display_beacon_control()
    case BEACON_ACTIVE:
      display_beacon_control();
      break;

    case BEACON_DROID_LIST:      // display_droid_beacon_list()
      display_captioned_menu(msg_select_beacon, LIST_PERSONALITIES);
      break;

    case BEACON_LOCATION_LIST:   // display_location_beacon_list()
      display_captioned_menu(msg_select_beacon, LIST_LOCATIONS);
      break;

    case BEACON_DATAPAD_LIST:    // display_datapad_beacon_list()
      display_captioned_menu(msg_select_beacon, LIST_DATAPADS);
      break;

    case BEACON_SET_INTERVAL:    // display_set_interval()
      display_set_interval();
      break;

    case BEACON_TYPE_MENU:       // display_beacon_type_menu()
      display_captioned_menu(msg_select_beacon_type, LIST_BEACON_TYPE_MENU);
      break;

    case TOP_MENU:               // display_top_menu()
      display_captioned_menu(msg_select, LIST_TOP_MENU);
      break;

    case SPLASH:                 // display_splash()
      display_splash();
      break;
  }

  flush_display();
}

#if defined(USE_OFR_FONTS) && defined(DECODE_TRANSITION_ENABLE)
bool decode_transition_supported() {
  return DECODE_AURABESH_FONT > 0 && DECODE_AURABESH_FONT <= NUM_FONTS;
}

void draw_decode_fizz(uint8_t frame) {
  uint8_t i;
  int16_t x;
  int16_t y;
  int16_t w;
  uint16_t color;

  for (i = 0; i < DECODE_FIZZ_SPECKS; i++) {
    x = esp_random() % max((int16_t)1, (int16_t)tft.getViewportWidth());
    y = esp_random() % max((int16_t)1, (int16_t)tft.getViewportHeight());
    color = (i % 5 == 0) ? TFT_WHITE : ((i % 2 == 0) ? TFT_DARKGREY : C565(40, 120, 90));

    if ((i + frame) % 4 == 0) {
      w = 2 + (esp_random() % 18);
      tft.drawFastHLine(x, y, w, color);
    } else {
      tft.drawPixel(x, y, color);
    }
  }

  flush_display();
}

void render_decode_transition() {
  uint8_t normal_font = dtb_font;
  uint8_t frame;

  if (!decode_transition_supported()) {
    render_current_screen();
    return;
  }

  decode_transition_active = true;
  dtb_font = DECODE_AURABESH_FONT;
  dtb_load_font();
  render_current_screen();
  delay(DECODE_HOLD_TIME);

  for (frame = 0; frame < DECODE_FIZZ_FRAMES; frame++) {
    draw_decode_fizz(frame);
    delay(DECODE_FIZZ_FRAME_TIME);
  }

  dtb_font = normal_font;
  dtb_load_font();
  decode_transition_active = false;
  render_current_screen();
}
#endif

void update_display() {
  static int16_t last_rendered_state = -1;
  bool new_screen;

  if (tft_update != true) {
    return;
  }

  new_screen = (last_rendered_state != (int16_t)state);

  #if defined(USE_OFR_FONTS) && defined(DECODE_TRANSITION_ENABLE)
    if (new_screen) {
      render_decode_transition();
    } else {
      render_current_screen();
    }
  #else
    render_current_screen();
  #endif

  last_rendered_state = (int16_t)state;
  tft_update = false;
}

void button1(button_press_t press_type);  // trying to use an enum as a parameter triggers a bug in arduino. adding an explicit prototype resolves the issue.
void button1(button_press_t press_type) {

  uint8_t i;

  SERIAL_PRINTLN("Button 1 Press");

  switch (state) {

    // on splash screen
    case SPLASH:

      // only swap fonts if USE_OFR_FONTS is defined
      #ifdef USE_OFR_FONTS

        // a long button1 press cycles through user-selectable readable fonts.
        // Aurabesh is reserved for the decode transition.
        if (press_type == LONG_PRESS) {
            if (dtb_font < FIRST_USER_SELECTABLE_FONT || dtb_font >= LAST_USER_SELECTABLE_FONT) {
              dtb_font = FIRST_USER_SELECTABLE_FONT;
            } else {
              dtb_font++;
            }

            // load the new font
            dtb_load_font();

        // otherwise go to the top menu
        } else {

          // before exiting the splash screen, store the current font to NVS
          #ifdef USE_NVS
            preferences.putUChar("dtb_font", dtb_font);
          #endif
      #endif

        state = TOP_MENU;
        selected_item = 0;

      #ifdef USE_OFR_FONTS
        }
      #endif

      tft_update = true;
      break;

    case TOP_MENU:
      state = top_menu[selected_item].state;
      if (state == TRIVIA_QUESTION) {
        trivia_pick_random();
      }
      selected_item = 0;
      tft_update = true;
      break;

    case BEACON_TYPE_MENU:
      state = beacon_type_menu[selected_item].state;

      if (state == BEACON_RANDOM) {
        set_random_beacon();

        // the following if/else block is setting selected_item to the value of the randomly selected beacon
        //
        // in its respective beacon list. this is being done so when you go back (long button 2 press)
        // you'll be brought to the element in the beacon list where the randomly selected beacon is
        if (beacon.type == DROID) {
          for(i=0; i<emulatable_personalities_size; i++) {
            if (emulatable_personalities[i]->id == beacon.setting[BEACON_PARAM_DROID_ID]) {
              selected_item = i;
              break;
            }
          }
        } else {
          for(i=0; i<LOCATIONS_SIZE; i++) {
            if (locations[i].id == beacon.setting[BEACON_PARAM_LCTN_ID]) {
              selected_item = i;
              break;
            }
          }
        }
        state = BEACON_ACTIVATE;

      // initiate a rotating beacon
      } else if (state == BEACON_ROTATING) {
        state = BEACON_SET_INTERVAL;
        selected_item = 0;

      } else if (state == BEACON_EXPERT) {

        // let whatever is currently in beacon be displayed on the expert screen.
        // let's users see what the stock beacons look like. if they figure out this little trick of set a beacon, then back out to expert mode.
        //set_random_beacon();

        // expert mode only knows how to render DROID/LOCATION beacons, so if we're carrying an
        // active datapad iBeacon at entry, normalize to a default LOCATION beacon first.
        if (beacon.type == IBEACON) {
          set_location_beacon(0);
        }

        selected_item = 0;
      } else {
        selected_item = 0;
      }
      tft_update = true;
      break;

    case BEACON_SET_INTERVAL:
      if (selected_item == 0) {
        if (beacon_rotate_interval >= MAX_BEACON_CHANGE_INTERVAL) {
          beacon_rotate_interval = 6;
        } else {
          if (press_type == SHORT_PRESS) {
            beacon_rotate_interval += SHORT_PRESS_INTERVAL_INC;
          } else {
            beacon_rotate_interval += LONG_PRESS_INTERVAL_INC;
          }
          if (beacon_rotate_interval > MAX_BEACON_CHANGE_INTERVAL) {
            beacon_rotate_interval = MAX_BEACON_CHANGE_INTERVAL;
          }
        }
      } else {
        set_rotating_beacon();
        next_beacon_time = 1;
        state = BEACON_ACTIVATE;
        selected_item = 0;
        button1(SHORT_PRESS);
      }
      tft_update = true;
      break;

    case BEACON_LOCATION_LIST:
      set_location_beacon_by_index(selected_item);
      state = BEACON_ACTIVATE;
      tft_update = true;
      break;

    case BEACON_DROID_LIST:
      set_droid_beacon(emulatable_personalities[selected_item]->id);
      state = BEACON_ACTIVATE;
      tft_update = true;
      break;

    case BEACON_DATAPAD_LIST:
      set_ibeacon(selected_item);
      state = BEACON_ACTIVATE;
      tft_update = true;
      break;

    case BEACON_ACTIVATE:
      if (next_beacon_time != 0) {
        next_beacon_time = millis() + (beacon_rotate_interval * 10000); 
      }
      init_advertisement_data(nullptr);
      set_payload_from_beacon();
      pAdvertising->start();
      state = BEACON_ACTIVE;
      tft_update = true;
      break;

    case BEACON_ACTIVE:
      pAdvertising->stop();
      state = BEACON_ACTIVATE;
      tft_update = true;
      break;

    case BEACON_EXPERT:

      // change beacon type; (re)initialize the beacon object
      if (selected_item == 0) {

        if (beacon.type == DROID) {
          beacon.type = LOCATION;
          beacon.setting[BEACON_PARAM_LCTN_ID]      = 1;  // default location
          beacon.setting[BEACON_PARAM_LCTN_REACT]   = 2;  // default interval
          beacon.setting[BEACON_PARAM_LCTN_RSSI]    = 38; // default minimim rssi
          beacon.setting[BEACON_PARAM_LCTN_PARAM4]  = 0;  // unused
        } else {
          beacon.type = DROID;
          beacon.setting[BEACON_PARAM_DROID_ID]     = 1; // default personality
          beacon.setting[BEACON_PARAM_DROID_AFFL]   = 1; // default affiliation
          beacon.setting[BEACON_PARAM_DROID_PAIRED] = 1; // default paired
          beacon.setting[BEACON_PARAM_DROID_BDROID] = 0; // be a droid
        }

      // change beacon parameters
      } else if (selected_item-1 < NUM_BEACON_PARAMS) {

        // store the current beacon parameter index based on current selected_item value
        i = selected_item - 1;

        // on a long press, reset the beacon setting to 0
        if (press_type == LONG_PRESS) {
          beacon.setting[i] = 0;

        // otherwise, increment the parameter value by 1
        } else {
          beacon.setting[i]++;
        }

        // adjust parameter values as needed for a DROID beacon
        if (beacon.type == DROID) {
          switch(i) {

            // reset affiliation if affiliation goes above 0x3F
            case BEACON_PARAM_DROID_AFFL:
              if (beacon.setting[i] > 0x3F) {
                beacon.setting[i] = 0;
              }
              break;

            // these are yes/no parameters, keep their values as either 0 or 1
            case BEACON_PARAM_DROID_BDROID:
            case BEACON_PARAM_DROID_PAIRED:
              if (beacon.setting[i] > 1) {
                beacon.setting[i] = 0;
              }
              break;
          }

        // adjust parameter values as needed for a LOCATION beacon
        } else {
          switch(i) {

            // reset location if it goes above 7
            case BEACON_PARAM_LCTN_ID:
              if (beacon.setting[i] > 7) {
                beacon.setting[i] = 0;
              }
              break;

            // cap reaction interval at 22 ("ALARM" / 0xFF), wrap to 0 after that
            case BEACON_PARAM_LCTN_REACT:
              if (beacon.setting[i] > 22) {
                beacon.setting[i] = 0;
              }
              break;

            // reset minimium rssi value if it goes outside of limits
            case BEACON_PARAM_LCTN_RSSI:

              // -20dBm is about 1ft from the droid and I think a reasonable lower limit
              // -80dBm is very weak and I think a reasonable upper limit
              // but you can change these limits if you want
              if (beacon.setting[i] < 20 || beacon.setting[i] > 80) {
                beacon.setting[i] = 20;
              }
              break;
          }
        }

      // if we get to here then we're on the activate function
      } else {

        // if a droid beacon with the be a droid parameter enabled
        if (beacon.type == DROID && beacon.setting[BEACON_PARAM_DROID_BDROID] != 0) {

          // create a beacon with the name DROID so as to appear as a droid to other toolboxes
          init_advertisement_data(ble_adv_name_droid);

        // otherwise initialize beacon as normal
        } else {
          init_advertisement_data(nullptr);
        }

        // set the payload based on the beacon data
        set_payload_from_beacon();

        // start advertising the beacon
        pAdvertising->start();

        // change state to active beacon
        state = BEACON_EXPERT_ACTIVE;
      }

      // set flag to update the display
      tft_update = true;
      break;

    case BEACON_EXPERT_ACTIVE:
      pAdvertising->stop();
      state = BEACON_EXPERT;
      tft_update = true;
      break;

    case SCANNER_RESULTS:
      if (droid_count > 0) {
        if (press_type == LONG_PRESS) {
          state = SCANNER_CONNECTING;
          tft_update = true;
        } else if (droid_count > 1) {
          current_droid = (current_droid + 1) % droid_count;
          tft_update = true;
        }
      } else {
        state = SCANNER_SCANNING;
        tft_update = true;
      }
      break;

    case WAIT_TIMES_RESULTS:
      state = WAIT_TIMES_LOADING;
      tft_update = true;
      break;

    case TRIVIA_QUESTION:
      state = TRIVIA_ANSWER;
      tft_update = true;
      break;

    case TRIVIA_ANSWER:
      trivia_pick_random();
      state = TRIVIA_QUESTION;
      tft_update = true;
      break;

    case SETTINGS_MENU:
      state = settings_menu[selected_item].state;
      selected_item = 0;
      tft_update = true;
      break;

    case SETTINGS_WIFI_STATUS:
      state = SETTINGS_WIFI_CONNECTING;
      tft_update = true;
      break;

    case SCREENSAVER_MENU:
      state = screensaver_menu[selected_item].state;
      selected_item = 0;
      tft_update = true;
      break;

    case SCREENSAVER_DEATH_STAR:
    case SCREENSAVER_HYPERSPACE:
    case SCREENSAVER_TWIN_SUNS:
      state = SCREENSAVER_MENU;
      selected_item = 0;
      tft_update = true;
      break;

    case CONNECTED_MENU:
      state = connected_menu[selected_item].state;
      tft_update = true;
      break;

    case SOUND_GROUP:
      if (press_type == SHORT_PRESS) {
        current_group = (current_group + 1) % 12;
        current_track = 0;
      } else {
        state = SOUND_TRACK;
      }
      tft_update = true;
      break;

    case SOUND_TRACK:
      if (press_type == SHORT_PRESS) {
        current_track = (current_track + 1) % 99;
      } else {
        state = SOUND_PLAY;
      }
      tft_update = true;
      break;

    case SOUND_PLAY:
      SERIAL_PRINTLN("Play selected!");
      state = SOUND_PLAYING;
      tft_update = true;
      break;

    case VOLUME_UP:
      if (droid_volume > 90) {
        droid_volume = 100;
      } else {
        droid_volume += 10;
      }
      tft_update = true;
      break;

    case VOLUME_DOWN:
      if (droid_volume < 10) {
        droid_volume = 0;
      } else {
        droid_volume -= 10;
      }
      tft_update = true;
      break;

    case VOLUME_TEST:
      state = VOLUME_TESTING;
      tft_update = true;
      break;
  }
}

void button2(button_press_t press_type);  // trying to use an enum as a parameter triggers a bug in arduino. adding an explicit prototype resolves the issue.
void button2(button_press_t press_type) {

  // if NVS is enabled, create this variable that we'll use later on
  #ifdef USE_NVS
    static String saved_addr;
  #endif

  uint8_t i;

  SERIAL_PRINTLN("Button 2 Press");

  // do button 2 stuff
  switch (state) {
    case SPLASH:

      // if we're using storage
      #ifdef USE_NVS

        // trigger a connection to a hard-coded address if there's a long button press on the splash screen
        if (press_type == LONG_PRESS) {

          saved_addr = preferences.getString("saved_addr", "");
          if (saved_addr.length() == 17) {

            // update the display to let them know we're attempting a connection
            state = SCANNER_CONNECTING;
            tft_update = true;
            update_display();
            delay(1000);

            if (droid_connect(saved_addr.c_str())) {
              state = SCANNER_CONNECTED;

            // on a failed connection, display the failed connect message, then back to the splash screen
            } else {
              SERIAL_PRINTLN("Saved address connect has failed");
              state = SCANNER_CONNECT_FAILED;
              tft_update = true;
              update_display();
              delay(2000);
              state = SPLASH;
            }
            tft_update = true;
          } else {
            SERIAL_PRINTLN("No saved address found.");
            SERIAL_PRINT("saved_addr length is ");
            SERIAL_PRINTLN(saved_addr.length());
          }
        } else {

      #endif  // USE_NVS

      // use button 2 short press to change screen brightness for AMOLED devices
      #ifdef LILYGO_AMOLED
        amoled.setBrightness((uint8_t)(amoled.getBrightness() + 32));
        #ifdef USE_NVS
          preferences.putUChar("dtb_brightness", amoled.getBrightness());
        #endif
      #else

        // before exiting the splash screen, store the current font to NVS
        #if defined (USE_OFR_FONTS) && defined (USE_NVS)
          preferences.putUChar("dtb_font", dtb_font);
        #endif

        state = TOP_MENU;
        selected_item = 0;
        tft_update = true;

      #endif

      // close out the if/else block if we're using storage
      #ifdef USE_NVS
        }
      #endif
      break;

    case TOP_MENU:
      if (press_type == LONG_PRESS) {
        state = SPLASH;
        selected_item = 0;
      } else {
        selected_item++;
        if (selected_item >= sizeof(top_menu) / sizeof(menu_item_t)) {
          selected_item = 0;
        }
      }
      tft_update = true;
      break;

    case BEACON_TYPE_MENU:
      if (press_type == LONG_PRESS) {
        state = TOP_MENU;
        selected_item = 1;  // beacon
      } else {
        selected_item++;
        if (selected_item >= sizeof(beacon_type_menu) / sizeof(menu_item_t)) {
          selected_item = 0;
        }
      }
      tft_update = true;
      break;

    case BEACON_SET_INTERVAL:

      // if a back menu button press...
      if (press_type == LONG_PRESS) {

        // find the location of rotating beacon in the beacon type menu
        for (selected_item=0; selected_item < (sizeof(beacon_type_menu) / sizeof(menu_item_t)); selected_item++) {
          if (beacon_type_menu[selected_item].state == BEACON_ROTATING) {
            break;
          }
        }

        // if rotating beacon type wasn't found in the menu then just set selected_item to the first item in the list
        if (selected_item >= (sizeof(beacon_type_menu) / sizeof(menu_item_t))) {
          next_beacon_time = 0;
          selected_item = 0;
        }
        state = BEACON_TYPE_MENU;

      } else {
        selected_item = (selected_item+1) % 2;
      }
      tft_update = true;
      break;

    case BEACON_LOCATION_LIST:
      if (press_type == LONG_PRESS) {
        state = BEACON_TYPE_MENU;
        selected_item = 0;
      } else {
        selected_item++;
        if (selected_item >= LOCATIONS_SIZE) {
          selected_item = 0;
        }
      }
      tft_update = true;
      break;

    case BEACON_DROID_LIST:
      if (press_type == LONG_PRESS) {
        state = BEACON_TYPE_MENU;
        selected_item = 1;
      } else {
        selected_item++;
        if (selected_item >= emulatable_personalities_size) {
          selected_item = 0;
        }
      }
      tft_update = true;
      break;

    case BEACON_DATAPAD_LIST:
      if (press_type == LONG_PRESS) {
        // long press returns to TOP_MENU with the cursor on DATAPAD (index 2)
        state = TOP_MENU;
        selected_item = TOP_MENU_INDEX_DATAPAD;
      } else {
        selected_item++;
        if (selected_item >= DATAPADS_SIZE) {
          selected_item = 0;
        }
      }
      tft_update = true;
      break;

    case BEACON_ACTIVE:
    case BEACON_ACTIVATE:

      // stop advertising the active beacon
      if (state == BEACON_ACTIVE) {
        pAdvertising->stop();
      }

      // if a back menu button press...
      if (press_type == LONG_PRESS) {

        // if exiting a rotating beacon
        if (next_beacon_time != 0) {
          state = BEACON_SET_INTERVAL;
          next_beacon_time = 0;
          selected_item = 1;
        } else if (beacon.type == DROID) {
          state = BEACON_DROID_LIST;
        } else if (beacon.type == IBEACON) {
          state = BEACON_DATAPAD_LIST;
          selected_item = current_datapad_idx;
        } else {
          state = BEACON_LOCATION_LIST;
        }

      // change state to an inactive beacon
      } else if (state == BEACON_ACTIVE) {
        state = BEACON_ACTIVATE;

      // activate the beacon
      } else {
        if (next_beacon_time != 0) {
          next_beacon_time = millis() + (beacon_rotate_interval * 10000); 
        }
        init_advertisement_data(nullptr);
        set_payload_from_beacon();
        pAdvertising->start();
        state = BEACON_ACTIVE;
      }
      tft_update = true;
      break;

    case BEACON_EXPERT:
      if (press_type == LONG_PRESS) {
        state = BEACON_TYPE_MENU;
        selected_item = 3;
      } else {
        selected_item++;

        // skip expert beacon parameters that have no label
        i = selected_item - 1;
        if (i < NUM_BEACON_PARAMS && ((beacon.type == DROID) ? msg_beacon_droid_param[i] : msg_beacon_location_param[i]) == nullptr) {
          selected_item++;
        }

        // return to the first beacon parameter after reaching the end
        if (selected_item >= NUM_BEACON_PARAMS+2) {
          selected_item = 0;
        }
      }
      tft_update = true;
      break;

    case BEACON_EXPERT_ACTIVE:
      pAdvertising->stop();
      if (press_type == LONG_PRESS) {
        state = BEACON_TYPE_MENU;
        selected_item = 3;
      } else {
        state = BEACON_EXPERT;
      }
      tft_update = true;
      break;

    case SCANNER_RESULTS:
      if (press_type == LONG_PRESS || droid_count < 1) {
        state = TOP_MENU;
        selected_item = 0;    // scanner
        tft_update = true;
      } else {
        if (droid_count > 1) {
          if (current_droid == 0) {
            current_droid = droid_count - 1;
          } else {
            current_droid = (current_droid - 1) % droid_count;
          }
          tft_update = true;
        }
      }
      break;

    case WAIT_TIMES_LOADING:
    case WAIT_TIMES_RESULTS:
      state = TOP_MENU;
      selected_item = TOP_MENU_INDEX_WAIT_TIMES;
      tft_update = true;
      break;

    case TRIVIA_QUESTION:
    case TRIVIA_ANSWER:
      if (press_type == LONG_PRESS) {
        state = TOP_MENU;
        selected_item = TOP_MENU_INDEX_TRIVIA;
      } else {
        trivia_pick_random();
        state = TRIVIA_QUESTION;
      }
      tft_update = true;
      break;

    case SETTINGS_MENU:
      if (press_type == LONG_PRESS) {
        state = TOP_MENU;
        selected_item = TOP_MENU_INDEX_SETTINGS;
      } else {
        selected_item++;
        if (selected_item >= sizeof(settings_menu) / sizeof(menu_item_t)) {
          selected_item = 0;
        }
      }
      tft_update = true;
      break;

    case SETTINGS_WIFI_CONNECTING:
    case SETTINGS_WIFI_STATUS:
    case SETTINGS_WIFI_OFF:
      state = SETTINGS_MENU;
      selected_item = 0;
      tft_update = true;
      break;

    case SCREENSAVER_MENU:
      if (press_type == LONG_PRESS) {
        state = TOP_MENU;
        selected_item = TOP_MENU_INDEX_SCREENSAVER;
      } else {
        selected_item++;
        if (selected_item >= sizeof(screensaver_menu) / sizeof(menu_item_t)) {
          selected_item = 0;
        }
      }
      tft_update = true;
      break;

    case SCREENSAVER_DEATH_STAR:
    case SCREENSAVER_HYPERSPACE:
    case SCREENSAVER_TWIN_SUNS:
      state = SCREENSAVER_MENU;
      selected_item = 0;
      tft_update = true;
      break;

    case CONNECTED_MENU:
      if (press_type == SHORT_PRESS) {
        selected_item++;
        if (selected_item >= sizeof(connected_menu) / sizeof(menu_item_t)) {
          selected_item = 0;
        }
      } else {
        droid_disconnect();

        // if save_addr is empty then a back operation from the menu should go to scanner results
        #ifdef USE_NVS
          if (saved_addr.isEmpty()) {
        #endif

          state = SCANNER_RESULTS;

        // else we know we got here from a quick connect from the splash screen and should return to it
        #ifdef USE_NVS
          } else {
            saved_addr.clear();
            state = SPLASH;
          }
        #endif
      }
      tft_update = true;
      break;

    case SOUND_GROUP:
    case SOUND_TRACK:
    case SOUND_PLAY:
      if (press_type == SHORT_PRESS) {
        switch (state) {
          case SOUND_GROUP:
            state = SOUND_TRACK;
            break;
          case SOUND_TRACK:
            state = SOUND_PLAY;
            break;
          case SOUND_PLAY:
            state = SOUND_GROUP;
            break;
        }
      } else {
        state = CONNECTED_MENU;
        selected_item = 0;          // todo: don't hardcode this
      }
      tft_update = true;
      break;

    case VOLUME_UP:
    case VOLUME_DOWN:
    case VOLUME_TEST:
      if (press_type == SHORT_PRESS) {
        switch(state) {
          case VOLUME_UP:
            state = VOLUME_DOWN;
            break;
          case VOLUME_DOWN:
            state = VOLUME_TEST;
            break;
          case VOLUME_TEST:
            state = VOLUME_UP;
            break;
        }
      } else {
        state = CONNECTED_MENU;
        selected_item = 1;          // todo: don't hardcode this
      }
    tft_update = true;
    break;
  }
}

void button1_handler() {
  static uint32_t last_btn1_time = 0;
  static uint8_t last_btn1_state = HIGH;

  // gather current state of things
  uint8_t now_btn1_state = digitalRead(button_pins[0]);
  uint32_t now_time = millis();

  if (now_btn1_state != last_btn1_state && now_time - last_btn1_time > LAZY_DEBOUNCE) {
    if (now_btn1_state == HIGH) {
      if (now_time - last_btn1_time > SHORT_PRESS_TIME) {
        button1(LONG_PRESS);
      } else {
        button1(SHORT_PRESS);
      }
    }
    last_btn1_state = now_btn1_state;
    last_btn1_time = now_time;
    last_activity = millis();
  }
}

void button2_handler() {
  static uint32_t last_btn2_time = 0;
  static uint8_t last_btn2_state = BTN_UP_STATE;

  // gather current state of things
  uint8_t now_btn2_state = digitalRead(button_pins[1]);
  uint32_t now_time = millis();

  if (now_btn2_state != last_btn2_state && now_time - last_btn2_time > LAZY_DEBOUNCE) {
    if (now_btn2_state == BTN_UP_STATE) {
      if (now_time - last_btn2_time > SHORT_PRESS_TIME) {
        button2(LONG_PRESS);
      } else {
        button2(SHORT_PRESS);
      }
    }
    last_btn2_state = now_btn2_state;
    last_btn2_time = now_time;
    last_activity = millis();
  }
}

void button1_single_handler() {
  static uint32_t last_btn1_time = 0;
  static uint8_t last_btn1_state = BTN_UP_STATE;

  static uint8_t btn1_press_cnt = 0;
  static uint32_t btn1_grace_end = 0;
  static uint8_t btn1_long_press = 0;

  // gather current state of things
  uint8_t now_btn1_state = digitalRead(button_pins[0]);
  uint32_t now_time = millis();

  if (now_btn1_state != last_btn1_state && now_time - last_btn1_time > LAZY_DEBOUNCE) {

    // button is released
    if (now_btn1_state == BTN_UP_STATE) {
      //SERIAL_PRINTLN("BUTTON UP");

      // record if it was a short or long button press, we'll need this later on
      if (now_time - last_btn1_time > SHORT_PRESS_TIME) {
        btn1_long_press = 1;
      } else {
        btn1_long_press = 0;
      }

      // increment press counter
      btn1_press_cnt++;

      // start a counter for how long we'll wait for the next button press before we evaluate the button press
      btn1_grace_end = now_time + SINGLE_BTN_GRACE;

    // button 1 has been pressed
    } else {
      //SERIAL_PRINTLN("BUTTON DOWN");

      // stop grace counter when button is pressed
      btn1_grace_end = 0;
    }

    last_btn1_state = now_btn1_state;
    last_btn1_time = now_time;
    last_activity = millis();

  // button has been released for longer than the grace period, time to process the button press
  } else if (now_btn1_state == HIGH && btn1_grace_end > 0 && btn1_grace_end <= now_time) {
    //SERIAL_PRINT("RECORDED ");
    //SERIAL_PRINT(btn1_press_cnt);
    //SERIAL_PRINTLN(" BUTTON CLICKS");

    // was it a long or a short button press?
    if (btn1_long_press == 0) {
      //SERIAL_PRINTLN("SHORT");

      // is this a button 1 or a button 2 event?
      if (btn1_press_cnt < 2) {
        button1(SHORT_PRESS);
      } else {
        button2(SHORT_PRESS);
      }
    } else {
      //SERIAL_PRINTLN("LONG");
      if (btn1_press_cnt < 2) {
        button1(LONG_PRESS);
      } else {
        button2(LONG_PRESS);
      }
    }

    btn1_grace_end = 0;
    btn1_press_cnt = 0;
  }
}

void button_handler() {

  // give people the option to opt-in to single button mode
  #ifdef SINGLE_BUTTON_MODE
  button1_single_handler();

  // otherwise, determine if we have valid button pin values and determine
  // which button handlers (if any) to use.
  #else
  if (button_pins[0] >= 0) {
    if (button_pins[1] < 0) {
      button1_single_handler();
    } else {
      button1_handler();
      button2_handler();
    }
  }
  #endif
}

#ifdef SERIAL_DEBUG_ENABLE
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     SERIAL_PRINTLN("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     SERIAL_PRINTLN("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    SERIAL_PRINTLN("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: SERIAL_PRINTLN("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP:      SERIAL_PRINTLN("Wakeup caused by ULP program"); break;
    default:                        SERIAL_PRINTLN("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}
#endif

void setup() {
  uint8_t i;

  // init serial
  SERIAL_BEGIN(115200);

  // T-Display-S3 needs this in order to run off battery
  #if defined(TDISPLAYS3) && !defined(LILYGO_AMOLED)
    pinMode(15, OUTPUT);
    digitalWrite(15, HIGH);
  #endif

  #ifdef SERIAL_DEBUG_ENABLE
    print_wakeup_reason();
  #endif

  // setup display
  //
  // if a LilyGo AMOLED device we need to init the amoled device
  #ifdef LILYGO_AMOLED
    i = amoled.begin();
    if (i == 0) {
      while (1) {
        SERIAL_PRINT("Unable to initialize AMOLED screen!");
      }
    }
    amoled.setRotation(2);
    amoled.setBrightness(159);    // set screen brightness, 0-255. AMOLED_DEFAULT_BRIGHTNESS = 175
    tft.createSprite(amoled.width(), amoled.height());
    tft.setSwapBytes(1);

    // yeah... LilyGo AMOLED Touch... I'd like to use it eventually, but we're not quite ready for it.
    if (amoled.hasTouch()) {
      SERIAL_PRINTLN("hasTouch() is true");
      amoled.disableTouch();
    }
  #else
    tft.init();
    tft.setRotation(3);
  #endif
  reset_screen();

  // attach ofr to tft
  #ifdef USE_OFR_FONTS
    ofr.setDrawer(tft);  
  #endif

  // don't try to use two buttons as the same button
  if (button_pins[0] == button_pins[1] && button_pins[0] >= 0) {
    button_pins[1] = -1;
  }

  // set pin mode for button pins to input
  if (button_pins[0] >= 0) {
    pinMode(button_pins[0], INPUT);
  }
  if (button_pins[1] >= 0) {
    pinMode(button_pins[1], INPUT);
  }

  // if defined, enable wakeup button
  #ifdef WAKEUP_BUTTON
    // define deep sleep wakeup trigger; if commented out ESP32 goes into hibernation instead of deep sleep and only wakes up with the reset button
    esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKEUP_BUTTON, (BTN_UP_STATE == HIGH ? 0 : 1));

    // configure the wakeup button to be pulled high or low using internal resistors so button press works while in deep sleep
    if (BTN_UP_STATE == HIGH) {
      rtc_gpio_pullup_en((gpio_num_t)WAKEUP_BUTTON);
      rtc_gpio_pulldown_dis((gpio_num_t)WAKEUP_BUTTON);
    } else {
      rtc_gpio_pullup_dis((gpio_num_t)WAKEUP_BUTTON);
      rtc_gpio_pulldown_en((gpio_num_t)WAKEUP_BUTTON);
    }
  #endif

  // init bluetooth
  BLEDevice::init(ble_adv_name);

  // setup the BLE client
  pClient = BLEDevice::createClient();

  // setup BLE scanner
  pBLEScan = BLEDevice::getScan();  //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  // setup BLE advertising (beacon)
  pAdvertising = BLEDevice::getAdvertising();
  oScanResponseData.setName(ble_adv_name);
  pAdvertising->setScanResponseData(oScanResponseData);

  // initialize the sleep monitor timer
  last_activity = millis();

  // initialize the droid array
  for (i = 0; i < MAX_DROIDS; i++) {
    droids[i].pAdvertisedDevice = nullptr;
  }

  // populate emulatable_personalities array
  emulatable_personalities_size = 0;

  // how many personalities are available for emulation?
  for (i=0; i<DROID_PERSONALITIES_SIZE; i++) {
    if (droid_personalities[i].emulatable != 0) {
      emulatable_personalities_size++;
    }
  }

  // allocate memory for the pointers
  emulatable_personalities = (personality_t**)malloc(sizeof(personality_t*) * emulatable_personalities_size);

  // assign pointers to members of droid_personalities[]
  emulatable_personalities_size = 0;
  for (i=0; i<DROID_PERSONALITIES_SIZE; i++) {
    if (droid_personalities[i].emulatable != 0) {
      emulatable_personalities[emulatable_personalities_size] = &droid_personalities[i];
      emulatable_personalities_size++;
    }
  }

  // just so there's no garbage in there if it gets used before being initialized.
  set_random_beacon();

  // seed the trivia screen with a random first question and a short recent-history buffer.
  trivia_init();
  trivia_pick_random();

  // initialize the lists used for rendering menus
  list_init();
  list_calculate_dynamic_font_properties();

  // initialize NVS
  #ifdef USE_NVS
    preferences.begin(PREF_APP_NAME, false); 
    //preferences.clear();

    // load screen brightness
    #ifdef LILYGO_AMOLED
      amoled.setBrightness((uint8_t)preferences.getUChar("dtb_brightness", 159));
      //reset_screen();
    #endif

    // load stored font
    #ifdef USE_OFR_FONTS
      dtb_font = (uint8_t)preferences.getUChar("dtb_font", DEFAULT_DTB_FONT);
      if (dtb_font < FIRST_USER_SELECTABLE_FONT || dtb_font > LAST_USER_SELECTABLE_FONT) {
        dtb_font = DEFAULT_DTB_FONT;
        preferences.putUChar("dtb_font", dtb_font);
      }
      dtb_load_font();
    #endif
  #endif

  // end of setup
  SERIAL_PRINTLN("Ready!");
}

void loop() {
  static uint16_t s = 0;

  button_handler();

  switch (state) {

    case WAIT_TIMES_LOADING:
      update_display();
      fetch_wait_times();
      state = WAIT_TIMES_RESULTS;
      tft_update = true;
      last_activity = millis();
      break;

    case SETTINGS_WIFI_CONNECTING:
      update_display();
      wifi_connect_known();
      state = SETTINGS_WIFI_STATUS;
      tft_update = true;
      last_activity = millis();
      break;

    case SETTINGS_WIFI_OFF:
      wifi_off();
      state = SETTINGS_WIFI_STATUS;
      tft_update = true;
      last_activity = millis();
      break;

    case SCREENSAVER_DEATH_STAR:
    case SCREENSAVER_HYPERSPACE:
    case SCREENSAVER_TWIN_SUNS:
      render_screensaver();
      last_activity = millis();
      break;

    case SCANNER_SCANNING:
      update_display();
      ble_scan();
      current_droid = 0;
      state = SCANNER_RESULTS;
      tft_update = true;
      break;

    case SCANNER_CONNECTING:
      update_display();
      if (droid_connect()) {
        state = SCANNER_CONNECTED;
      } else {
        state = SCANNER_CONNECT_FAILED;
      }
      tft_update = true;
      break;

    case SCANNER_CONNECTED:
      delay(2000);
      state = CONNECTED_MENU;
      tft_update = true;
      break;

    case SCANNER_CONNECT_FAILED:
      delay(2000);
      state = SCANNER_RESULTS;
      tft_update = true;
      break;

    case SOUND_PLAYING:
      update_display();
      SERIAL_PRINTLN("Playing sound...");
      droid_play_track();
      delay(2000);
      state = SOUND_PLAY;
      tft_update = true;
      break;

    case VOLUME_TESTING:
      update_display();
      SERIAL_PRINTLN("Testing volume...");
      droid_set_volume();
      droid_play_track();
      //droid_play_next_track();
      delay(2000);
      state = VOLUME_TEST;
      tft_update = true;
      break;

    case BEACON_ACTIVE:

      // test if we're in rotating beacon mode
      if (next_beacon_time != 0) {

        // is it time to change the beacon?
        if (millis() >= next_beacon_time) {

          // stop the current beacon
          button1(SHORT_PRESS);

          // set a new beacon at random
          // TODO: add some code to make sure we don't repeat the same beacon twice
          set_rotating_beacon();
          next_beacon_time = millis() + (beacon_rotate_interval * 10000);

          // start the new beacon
          state = BEACON_ACTIVATE;
          button1(SHORT_PRESS);

        // print a sneaky countdown to the next beacon in the corner
        } else if (tft_update != true) {
          sneaky_beacon_countdown();
        }
      }
      break;
  }

  update_display();

  // inactivity sleep check; screen will blank when ESP32 goes to sleep
  // press reset button to wakeup
  if (millis() - last_activity > SLEEP_AFTER) {

    // do not go to sleep if the beacon is active
    if (state != BEACON_ACTIVE && state != BEACON_EXPERT_ACTIVE && !is_screensaver_state()) {
      SERIAL_PRINTLN("Going to sleep.");
      delay(100);
      esp_deep_sleep_start();
    } else {
      // TODO: dim the screen; add check to brighten it upon user action
      last_activity = millis();
    }
  }
}
