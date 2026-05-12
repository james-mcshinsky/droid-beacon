# LilyGo T-Display ESP32 1.14 Install

This folder is configured for the original LilyGo/TTGO T-Display ESP32 with the 1.14 inch 135x240 ST7789 LCD.

## Upload With PlatformIO

1. Install PlatformIO, either through the VS Code extension or the CLI.
2. Open this `Droid-Toolbox-main` folder as the PlatformIO project.
3. Connect the T-Display by USB.
4. Upload the `lilygo-t-display` environment.

CLI upload:

```sh
platformio run -e lilygo-t-display -t upload
```

If `platformio` is not on your shell path, use:

```sh
python3 -m platformio run -e lilygo-t-display -t upload
```

The project `platformio.ini` already selects:

- Board: `esp32dev`
- Partition scheme: `huge_app.csv`
- Display: ST7789, 135x240
- Display pins: MOSI 19, SCLK 18, CS 5, DC 16, RST 23, BL 4
- Buttons: GPIO 0 and GPIO 35, from the sketch defaults
- Required libraries: `TFT_eSPI` and `OpenFontRender`

## Galaxy's Edge Wait Times

The `WAIT TIMES` top-level menu fetches Galaxy's Edge waits for Disneyland Park and Disney's Hollywood Studios:

- DL FAL - Disneyland Millennium Falcon: Smugglers Run
- DL RISE - Disneyland Star Wars: Rise of the Resistance
- WDW FAL - Disney World Millennium Falcon: Smugglers Run
- WDW RISE - Disney World Star Wars: Rise of the Resistance

It uses Queue-Times' Disneyland and Hollywood Studios endpoints:

```txt
https://queue-times.com/parks/16/queue_times.json
https://queue-times.com/parks/7/queue_times.json
```

Before uploading, copy `secrets.example.h` to `secrets.h` and fill in Wi-Fi:

```cpp
#define WIFI_NETWORKS { \
  { "your_wifi_name", "your_wifi_password" }, \
  { "your_phone_hotspot", "your_hotspot_password" } \
}
```

If Wi-Fi is not configured, the wait-times screen will show `WIFI NOT SET`.

The firmware also has a `SETTINGS` top-level menu with:

- `WIFI CONNECT` - tries the configured networks in order.
- `WIFI STATUS` - shows connection state, SSID, and IP address.
- `WIFI OFF` - disconnects and turns off Wi-Fi.

When the wait-times screen needs network access it will auto-connect to the same configured networks.

## Trivia

The `TRIVIA` top-level menu shows randomized Star Wars questions from an on-device question bank.

- Button 1 reveals the answer.
- Button 1 again loads another random question.
- Button 2 skips to another random question.
- Long Button 2 returns to the top-level menu.

## Screensavers

The `SCREENSAVER` top-level menu includes:

- `DEATH STAR` - rotating low-poly station.
- `HYPERSPACE` - animated star streaks.
- `TWIN SUNS` - desert horizon scene.

Press either button while a screensaver is running to return to the screensaver menu.

If upload stalls, unplug the board, hold the top-left button while reconnecting USB, then upload again.

## Arduino IDE Notes

The sketch can still be uploaded through Arduino IDE. Use these settings:

- Board: `ESP32 Dev Module`
- Flash Size: `4MB (32Mb)`
- Partition Scheme: `Huge APP (3MB No OTA/1MB SPIFFS)`
- PSRAM: `Disabled`

For Arduino IDE, TFT_eSPI still needs its library setup changed manually. In `TFT_eSPI/User_Setup_Select.h`, comment out `#include <User_Setup.h>` and enable:

```cpp
#include <User_Setups/Setup25_TTGO_T_Display.h>
```
