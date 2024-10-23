# osgt-qol
osgt-qol is a quality of life mod for Growtopia 3.02 64-bit Windows clients aimed at Oldschool Growtopia players. It features various quality of life and security changes.

## Installation
Navigate to [Releases](https://github.com/houzeyhoo/osgt-qol/releases) section and download the latest `osgt-qol-*.zip` package.
Open the downloaded zip file then drag and drop `dinput8.dll` to your Growtopia installation folder.

If you are unsure how to find your installation folder, press Windows Key + R and paste in `%LocalAppData%\Growtopia` then press Enter.

## Uninstalling
Open your Growtopia installation folder and delete `dinput8.dll` file.

## Configuration
Currently, if you wish to disable any specific mod, you will need to download the [patches.txt](https://raw.githubusercontent.com/houzeyhoo/osgt-qol/refs/heads/main/patches.txt) file from this repository and place it in the same folder as the mod.
By default, without this file, osgt-qol will enable all of the built-in mods.

## Features
osgt-qol divides its mods into two primary categories, core patches and user patches. Core patches cannot be disabled and are always enabled.
User patches may be disabled via patches.txt file.

Core Patches:
- CRC Integrity Check bypass - Required for code modification to work
- About Menu attribution - Adds credits for the modification & lists all the loaded in user patches
- Secure Item Manager - scans to-be-downloaded files for path traversal attempts, stopping & warning the user on detection

User Patches:
- Framerate Unlock patch - Adjusts the game's framerate limit to match the users display refresh rate. Additionally fixes some visuals caused by higher FPS
- Audio stuttering patch - Forces the game to stream all audio files from disk rather than try load in memory. Playback from memory is broken on Proton's AudioManager FMOD backend
- Legacy Chat patch - Removes the chat update paddings, chat tabs and `[S]` symbols
- No Guild Icon - Shifts Gem Store margin up to be next to Friends List
- Good Night Title Screen - Changes the eye-searing sunny weather on title screen to a cozy night weather
- Server Switcher - You can now connect to various servers without modifying hosts file with this mod
- Cache Location Fixer - Binds game's cache folder to the same folder game installation is on. Useful for people that have multiple Growtopia installations. **NOTE:** Your save.dat is still shared between other installations
- Bubble Opacity Slider - Backport of the modern client bubble opacity feature
- Hotkey patch - Adds hotkeys number 1 through 3 to cycle through your inventory's quickbar
- Start Music Slider - Backport of modern client Start Music volume slider, your ears will thank you
- Old Checkboxes - Removes the odd spacing introduced in late 2.XX versions on checkboxes

## Recommended additions
When Growtopia switched to using BGFX as its rendering framework, Windows clients started using DirectX9 as opposed to OpenGL. With this transition, several visuals broke.
You may be interested in installing [dxvk](https://github.com/doitsujin/dxvk) as it resolves some of the blending issues (most notably Sunset/beach weather).