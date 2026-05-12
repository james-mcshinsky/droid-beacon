# Droid Beacon Toolbox

An ESP32 project for exploring what small BLE-enabled display boards can do with
Galaxy's Edge Droid Depot droids. It can advertise droid/location beacons, scan
for nearby droids, connect to a droid to play personality-chip sounds, show
Galaxy's Edge wait times, run small line-waiting activities, and generally act
like a tiny Star Wars-inspired field tool.

This is a fun learning project. It is meant to be tinkered with, studied, and
modified. The code is intentionally close to the hardware so you can see how BLE
advertising, scanning, display rendering, button input, Wi-Fi, and ESP32 sleep
all fit together.

## Credits And Attribution

This repository is based on the original
[Droid Toolbox](https://github.com/ruthsarian/droid-toolbox/) by Ruthsarian.
The original project did the hard work of documenting and implementing the
Galaxy's Edge droid BLE interactions. Please check out
[Ruthsarian's original Droid Toolbox project](https://github.com/ruthsarian/droid-toolbox/)
and the rest of their work.

This fork keeps that foundation and adds a set of local experiments and quality
of life features, including PlatformIO setup, Galaxy's Edge wait times, Wi-Fi
settings, Datapad iBeacon mode, trivia, screensavers, a refreshed splash screen,
and an Aurabesh-to-readable-font decode transition.

The original MIT license is preserved in [LICENSE](LICENSE).

This is an unofficial fan project. It is not affiliated with, endorsed by, or
supported by Disney, Lucasfilm, LilyGo, or Droid Depot. Use it responsibly and at
your own risk.

## What It Can Do

- Scan for nearby Droid Depot droids over BLE.
- Show a droid report with Bluetooth address, RSSI, personality chip, and
  affiliation when known.
- Save the last connected droid address in ESP32 non-volatile storage for quick
  reconnect from the splash screen.
- Connect to a droid and play a selected audio group and track from its
  personality chip.
- Set droid volume from the device.
- Emit Galaxy's Edge location beacons.
- Emit droid/personality beacons.
- Emit Disneyland or Walt Disney World Datapad iBeacons used by Play Disney
  Parks-style location experiences.
- Rotate location beacons on a configurable interval.
- Use Expert Mode to experiment with raw beacon parameters.
- Include documented location variants such as close-range and alarm-style
  presets.
- Use a silent/no-reaction droid beacon option that can help keep a nearby droid
  awake without making it talk.
- Fetch Disneyland and Walt Disney World Galaxy's Edge wait times for:
  - Millennium Falcon: Smugglers Run
  - Star Wars: Rise of the Resistance
- Store Wi-Fi credentials in a local-only `secrets.h` file that is not committed
  to GitHub.
- Connect to Wi-Fi, show Wi-Fi status, and turn Wi-Fi off from the Settings
  menu.
- Show randomized Star Wars trivia questions and answers.
- Run themed screensavers:
  - Rotating low-poly Death Star
  - Hyperspace streaks
  - Twin suns desert horizon
- Use an Aurabesh decode animation whenever a new screen loads, then settle into
  a normal readable English font.
- Auto sleep after inactivity, except while a beacon or screensaver is active.
- Support single-button mode for boards that only have one usable button.

## Hardware

The tested PlatformIO environment in this repository targets:

- [LilyGo/TTGO T-Display ESP32](https://www.lilygo.cc/products/lilygo%C2%AE-ttgo-t-display-1-14-inch-lcd-esp32-control-board)
- 1.14 inch 135x240 ST7789 LCD
- ESP32 Dev Module board profile
- Buttons on GPIO 0 and GPIO 35

The original codebase also contains conditional support for related LilyGo
[T-Display-S3](https://www.lilygo.cc/products/t-display-s3) and AMOLED
variants, but the included `platformio.ini` currently defines the original
T-Display ESP32 environment.

You will need:

- A supported ESP32 display board.
- A USB data cable, not just a charging cable.
- PlatformIO, either through VS Code or the CLI.
- Optional: a battery for portable use.
- Optional: Wi-Fi credentials if you want wait times.

## Upstream Setup Resources

Ruthsarian's original README collects several helpful board and library links.
They are worth checking when you are adapting this project to a different board
or troubleshooting display/library setup:

- [Arduino IDE Software](https://www.arduino.cc/en/software)
- [ESP32 Core for Arduino](https://github.com/espressif/arduino-esp32)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [OpenFontRender](https://github.com/takkaO/OpenFontRender)
- [LilyGo T-Display GitHub Repository](https://github.com/Xinyuan-LilyGO/TTGO-T-Display)
  for drivers, setup notes, and sample code
- [LilyGo T-Display-S3 GitHub Repository](https://github.com/Xinyuan-LilyGO/T-Display-S3/)
- [LilyGo AMOLED Series GitHub Repository](https://github.com/Xinyuan-LilyGO/LilyGo-AMOLED-Series/)
- [Original Droid Toolbox setup video](https://www.youtube.com/watch?v=wEwg2-N3HqM)
- [Original MacOS setup follow-up video](https://www.youtube.com/watch?v=kiu0b0Uf0Nw)

## Quick Start With PlatformIO

1. Install [Visual Studio Code](https://code.visualstudio.com/) and the
   [PlatformIO extension](https://platformio.org/install/ide?install=vscode), or
   install PlatformIO Core for CLI use.
2. Clone this repository.
3. Open this folder as a PlatformIO project.
4. Connect the T-Display by USB.
5. Build the firmware:

```sh
python3 -m platformio run -e lilygo-t-display
```

6. Upload the firmware:

```sh
python3 -m platformio run -e lilygo-t-display -t upload
```

If `platformio` is on your shell path, these also work:

```sh
platformio run -e lilygo-t-display
platformio run -e lilygo-t-display -t upload
```

If upload stalls, unplug the board, hold the top-left button while reconnecting
USB, then run the upload command again.

## Wi-Fi Setup

Wi-Fi is optional. The firmware only needs Wi-Fi for wait times.

Your real Wi-Fi credentials should live in `secrets.h`, which is ignored by git
and should never be committed.

1. Copy the example file:

```sh
cp secrets.example.h secrets.h
```

2. Edit `secrets.h`:

```cpp
#pragma once

#define WIFI_NETWORKS { \
  { "your_wifi_name", "your_wifi_password" }, \
  { "your_phone_hotspot", "your_hotspot_password" } \
}
```

You can list one network or several. The firmware tries each configured network
in order.

3. Upload the firmware after editing `secrets.h`.

If Wi-Fi is not configured, the wait-times screen shows `WIFI NOT SET`. The rest
of the tool still works.

Important: this public repository includes `secrets.example.h`, but not
`secrets.h`. If you fork this project, keep that pattern. Your local board can
have your Wi-Fi, while GitHub does not.

## Button Orientation

Hold the original T-Display horizontally with the USB port on the left.

- Top-left button: Button 1
- Bottom-left button: Button 2

Button presses are handled on release.

- Short press: held for less than 500 ms
- Long press: held for 500 ms or longer

## Startup Screen

On boot, the splash screen shows the project title, firmware version, and battery
or USB power voltage. Press either button to enter the main menu.

If a droid was previously connected and saved, hold Button 2 on the splash
screen to quick-connect to that saved droid.

## Main Menu

Use Button 2 to move through menu items. Use Button 1 to select.

Top-level options include:

- `SCANNER`
- `BEACON`
- `DATAPAD`
- `WAIT TIMES`
- `TRIVIA`
- `SCREENSAVER`
- `SETTINGS`

## Scanner

Scanner mode searches for nearby droids and sorts results by signal strength.

- Button 1 short press: next detected droid
- Button 1 long press: connect to selected droid
- Button 2 short or long press: return to menu

The scan report shows:

- Known droid name or personality when available
- Affiliation
- Bluetooth address
- RSSI signal strength
- Result count

You can add custom droid names in the `named_droids[]` array inside
`Droid-Toolbox.ino`.

## Connected Droid Controls

After connecting to a droid, the connected menu offers:

- `SOUNDS`
- `VOLUME`

### Sounds

The sound player lets you choose an audio group and track.

- Button 2 short press: move between group, track, and play
- Button 1 short press on group: increment group
- Button 1 short press on track: increment track
- Button 1 on play: play the selected sound
- Button 2 long press: return to connected menu

Droid audio groups generally map to location reactions and accessory sounds.
Track counts vary by personality chip. If a track does not exist, the droid
usually does nothing.

### Volume

Volume is represented as 0 to 100 in increments of 10.

- `VOL+`: increase selected volume
- `VOL-`: decrease selected volume
- `SET VOLUME`: send the current volume to the droid

The firmware cannot read a droid's current volume, so it starts with an assumed
value.

## Beacon Modes

The beacon menu can emit several BLE beacon types.

### Location

Location beacons represent Galaxy's Edge areas such as Marketplace, Droid Depot,
Resistance, Dok-Ondar's, First Order, and alert-style zones.

### Droid

Droid beacons represent individual droid personality profiles and affiliations.

### Random

Random chooses a location or droid beacon for you.

### Rotating

Rotating mode cycles through random location beacons at a configurable interval.

- Button 1 on interval: increase interval
- Button 2: move to `START BEACON`
- Button 1 on `START BEACON`: begin rotating

### Expert

Expert Mode exposes raw beacon parameters for experimentation:

- Beacon type
- Location id
- Reaction interval
- Minimum RSSI
- Personality chip id
- Affiliation
- Paired state
- Droid-visible state

This is the mode to study how the payload changes and to experiment outside the
presets.

### Datapad

Datapad mode emits Apple iBeacon-style payloads for Disneyland or Walt Disney
World. These are separate from the Droid Depot droid/location beacon payloads.

## Wait Times

The wait-times screen fetches Galaxy's Edge attraction waits from
[Queue-Times.com](https://queue-times.com/).

It currently displays:

- `DL FAL`
- `DL RISE`
- `WDW FAL`
- `WDW RISE`

The Settings menu can connect to Wi-Fi, show status, and turn Wi-Fi off. If the
wait-times screen needs Wi-Fi and is not connected, it attempts to connect using
the networks in `secrets.h`.

## Trivia

Trivia mode shows randomized Star Wars questions from an on-device question
bank.

- Button 1: reveal answer
- Button 1 again: next question
- Button 2 short press: skip question
- Button 2 long press: return to menu

## Screensavers

Screensaver mode includes:

- `DEATH STAR`
- `HYPERSPACE`
- `TWIN SUNS`

Press either button while a screensaver is running to return to the screensaver
menu.

## Customizing The Project

Good places to experiment:

- `named_droids[]`: add names for your own droids by Bluetooth address.
- `droid_personalities[]`: add or hide personality chip entries.
- `locations[]`: rename or add location beacon presets.
- `DECODE_HOLD_TIME`, `DECODE_FIZZ_FRAMES`, `DECODE_FIZZ_FRAME_TIME`: tune the
  screen decode animation.
- `SLEEP_AFTER`: change the inactivity sleep time.
- `SINGLE_BUTTON_MODE`: enable one-button operation.
- `SERIAL_DEBUG_ENABLE`: enable serial debug output.

This is a good project for learning:

- BLE advertising
- BLE scanning
- BLE client connections
- ESP32 non-volatile preferences
- Small-screen UI rendering
- Button debouncing
- Wi-Fi HTTP requests
- Deep sleep and wakeup behavior

## Troubleshooting

### Upload stalls

Unplug the board. Hold the top-left button while reconnecting USB. Then upload
again.

### No serial device appears

Use a USB data cable. Some cables only provide power. You may also need a USB
serial driver depending on your board.

### Screen is blank or rotated

Check the TFT_eSPI setup. The PlatformIO environment in this repository passes
the required display setup flags for the original T-Display. If using Arduino
IDE or another board, make sure the correct TFT_eSPI setup is selected.

### Wait times say `WIFI NOT SET`

Create `secrets.h` from `secrets.example.h`, add at least one Wi-Fi network, and
upload the firmware again.

### Wait times say `FETCH FAILED`

Check that Wi-Fi connected, the network has internet access, and Queue-Times.com
is reachable.

### Droid will not connect

Turn off the droid's remote, keep the droid powered on, and try scanning again.

## Repository Hygiene

The following are intentionally not committed:

- `.pio/`
- `secrets.h`
- local VS Code C/C++ cache files

Before publishing your own fork, run:

```sh
git status --short
git check-ignore -v secrets.h .pio
```

`secrets.h` should be ignored. Do not commit real Wi-Fi credentials.

## References

- [Original Droid Toolbox](https://github.com/ruthsarian/droid-toolbox/)
- [Controlling Disney's Droids from Droid Depot with WebBluetooth](https://medium.com/@baptistelaget/controlling-disneys-droids-from-droid-depots-with-webbluetooth-febbabe50587)
- [Queue-Times API](https://queue-times.com/pages/api)
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
- [OpenFontRender](https://github.com/takkaO/OpenFontRender)
- [LilyGo TTGO T-Display](https://github.com/Xinyuan-LilyGO/TTGO-T-Display)
- [LilyGo T-Display-S3](https://github.com/Xinyuan-LilyGO/T-Display-S3)
