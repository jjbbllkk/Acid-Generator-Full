# ACID GENERATOR

**A deterministic, musically-aware acid bassline generator.**

This is a fork of [drakh/acid-generator](https://github.com/drakh/acid-generator), heavily inspired by the [**STING by SKINNERBOX**](https://maxforlive.com/library/device/4260/sting-by-skinnerbox) Max for Live device.

Unlike standard random generators, this version focuses on **musical stability and reproducibility**. It uses seeded random number generation and weighted probability to ensure that "random" patterns feel like cohesive musical phrases rather than chaotic noise.

[![IMAGE](https://maxforlive.com/images/screenshots/?ss=sting.jpg&id=4260)](https://maxforlive.com/library/device/4260/sting-by-skinnerbox)

## New Features & Changes

### Deterministic & Musical Generation
* **Seed-Based Logic:** The generator is now fully deterministic. Every pattern is generated from a unique seed. If you find a pattern you like, it will sound exactly the same every time you load it.
* **Weighted "Musical" Probability:** The generator no longer picks notes purely at random. It uses a tonal hierarchy:
    * **Root & Fifth Priority:** The generator favors the root and fifth notes of the scale, especially on downbeats, to "anchor" the bassline.
    * **Rhythmic Stability:** Downbeats (steps 1, 5, 9, 13) are prioritized when generating rhythms to establish a strong groove.
    * **Intelligent Resolution:** "Wonky" intervals are mathematically forced to resolve to stable tones on the "One" (the first beat of the bar).

### Smarter Knobs
* **Density as a Mask:** Changing `Density` no longer regenerates a new random pattern. Instead, it acts as a "Rhythmic Gate," revealing or hiding notes from a fixed "Master Sequence." Turning it down removes the funkier off-beats first; turning it up reveals them.
* **Spread as a Pool:** Changing `Spread` controls the *pool* of allowed notes. Lower settings restrict the melody to just the Root and Fifth. Higher settings unlock the full scale. This allows you to simplify a melody without changing its fundamental rhythm.
* **Loop Length Independence:** Changing the `Pattern Length` (e.g., from 16 to 7) does not regenerate the notes. It simply loops a smaller section of the 64-step master sequence.

### Extended Audio Engine
* **New Synth Controls:** Added **Envelope Mod** (Env) and **Decay** controls to the interface, allowing for deeper, squelchier TB-303 style sound design.
* **Visual Feedback:** The piano roll now visually dims steps that are outside the active loop length.
* **Expanded Scales:** Added Pentatonic Minor/Major, Blues Minor, Whole Tone, Chromatic, and Japanese In Sen scales.

## Installation (VCV Rack)

### Pre-built (Recommended)

Download the `.vcvplugin` matching your platform from the [Releases](https://github.com/jjbbllkk/Acid-Generator-Full/releases) page:

| Platform | File |
| --- | --- |
| macOS (Apple Silicon) | `AcidGenerator-x.y.z-mac-arm64.vcvplugin` |
| macOS (Intel)         | `AcidGenerator-x.y.z-mac-x64.vcvplugin`   |
| Windows (64-bit)      | `AcidGenerator-x.y.z-win-x64.vcvplugin`   |
| Linux (64-bit)        | `AcidGenerator-x.y.z-lin-x64.vcvplugin`   |

Then either:

- **Option A (easiest):** Open VCV Rack and choose **Library → Install Plugin from File…**, then select the downloaded `.vcvplugin`. Restart Rack.
- **Option B (manual):** Place the `.vcvplugin` file in your Rack user plugins folder, then restart Rack:
  - **macOS:** `~/Library/Application Support/Rack2/plugins-mac-arm64/` (Apple Silicon) or `…/plugins-mac-x64/` (Intel)
  - **Windows:** `%LOCALAPPDATA%\Rack2\plugins-win-x64\`
  - **Linux:** `~/.local/share/Rack2/plugins-lin-x64/`

After restart, find **Acid Generator** in the module browser under the **Vulpes79** brand.

### Build from Source

1. Clone the repository into your Rack SDK plugins folder:
   ```
   cd /path/to/Rack-SDK/plugins
   git clone https://github.com/jjbbllkk/Acid-Generator-Full.git AcidGenerator
   ```
2. Build the plugin:
   ```
   cd AcidGenerator
   make dist
   ```
3. The packaged `.vcvplugin` will be in the `dist/` folder. Install it via VCV Rack's **Library → Install Plugin from File…** menu (or copy it into the appropriate `plugins-<os>-<arch>` folder shown above).

### Cross-platform builds

This repository ships a GitHub Actions workflow (`.github/workflows/build.yml`) that builds `.vcvplugin` artifacts for all four supported targets (`mac-arm64`, `mac-x64`, `win-x64`, `lin-x64`) on every push. Tagged releases (`v*`) are automatically attached to a GitHub Release.

## USAGE

* **Space:** Start/Stop playback.
* **G (or Click Smile):** Generate a completely new pattern (new Seed).
* **Knobs:**
    * **Density:** Add/remove rhythmic events.
    * **Spread:** Add/remove melodic complexity.
    * **Env / Decay / Cut / Res:** Shape the synth tone.

## Tech Stack

Created with help of [Tone.js](https://tonejs.github.io/).

[![IMAGE](https://avatars.githubusercontent.com/u/11019186?s=200&v=4)](https://tonejs.github.io/)

## Links

* [Original Repository](https://github.com/drakh/acid-generator)
* [Original Live Demo](https://drakh.github.io/acid-generator/)
