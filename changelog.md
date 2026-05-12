# Changelog

Changelog for this public Droid Beacon Toolbox fork.

Upstream history before this fork is intentionally not repeated here. For the
original Droid Toolbox history, see Ruthsarian's project:
https://github.com/ruthsarian/droid-toolbox/

Changelog format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

- No unreleased changes.

## [0.83.3-jm1] - 2026-05-12

### Added

- Public fork documentation with Ruthsarian attribution, PlatformIO setup,
  Wi-Fi secrets guidance, feature overview, operation guide, troubleshooting
  notes, and upstream setup/resource links.
- PlatformIO configuration for the original LilyGo/TTGO T-Display ESP32.
- Local-only Wi-Fi configuration through ignored `secrets.h` and published
  `secrets.example.h`.
- Galaxy's Edge wait-times screen using Queue-Times.com data.
- Settings menu for Wi-Fi connect/status/off.
- Datapad iBeacon mode for Disneyland and Walt Disney World payloads.
- Star Wars trivia mode.
- Star Wars-themed screensavers.
- Refreshed startup splash screen.
- Aurabesh decode transition that settles into a readable English font.
- Additional beacon presets and a silent/no-reaction droid personality option.

### Changed

- Local Wi-Fi credentials are kept out of GitHub; only placeholder examples are
  published.
- Final display font is readable English, with Aurabesh used for the decode
  transition.
- Menu and status text now auto-fits within the display; menu sections use a
  shared fitted text size and long trivia/status lines wrap or shrink instead
  of clipping off-screen.
- Improved runtime performance on ESP32 hardware by caching menu/font metrics,
  avoiding startup heap allocation for menu lists, reducing temporary JSON
  parser strings, keeping the transition font resident, and using integer-only
  volume command math.
- README attribution now refers to Ruthsarian by name and recommends checking
  out the original Droid Toolbox project.
