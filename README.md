<div align="center">

<img width="50%" src="src/res/logo.png" alt="OSGT-QOL logo, created by Corrects">
<hr>

# osgt-qol
</div>

osgt-qol is a quality of life mod for Growtopia 3.02 64-bit Windows clients aimed at Oldschool Growtopia players. It features various quality of life and security changes along extending several limitations of the vanilla game client.

> [!NOTE]
> This repository is provided as a Proof-of-Concept. The core maintainers of the project are currently working on a rewrite. Contributions are still very much welcome, things done here will be relevant for the rewrite.

## Installation
Navigate to [Releases](https://github.com/gtModLoader/osgt-qol-poc/releases) section and download the latest `osgt-qol-*.zip` package.
Open the downloaded zip file then drag and drop `dinput8.dll` to your Growtopia installation folder.

If you are unsure how to find your installation folder, press Windows Key + R and paste in `%LocalAppData%\Growtopia` then press Enter.

## Cache and save separation
If you play more than one server or play on the live servers, you may want to clone your V3.02 Growtopia installation by pressing Windows Key + R, pasting in `%LocalAppData%`, copying Growtopia folder, pasting it and renaming the pasted folder to OSGT. You should install the dinput8.dll file only inside OSGT folder then.

If you want to make a quick way to access this install of your game along your other copies, go inside the created OSGT folder, right click Growtopia.exe, select "Send to" and then "Desktop (create shortcut)". For Windows 11, you will need to select "Show more options" to see "Send to" option.

Optionally, you can edit the newly created shortcut by selecting dinput8.dll for its icon, this will put the project logo as the shortcut logo.

## Uninstalling
Open your Growtopia installation folder and delete `dinput8.dll` file.

## Configuration
Currently, if you wish to disable any specific mod, you will need to download the [patches.txt](https://raw.githubusercontent.com/gtModLoader/osgt-qol-poc/refs/heads/main/patches.txt) file from this repository and place it in the same folder as the mod.
By default, without this file, osgt-qol will enable all of the built-in mods.

## Features
osgt-qol divides its mods into two primary categories, core patches and user patches. Core patches cannot be disabled and are always enabled.
User patches may be disabled via patches.txt file.

Core Patches:
- CRC Integrity Check bypass - Required for code modification to work
- About Menu attribution - Adds credits for the modification & lists all the loaded in user patches
- Secure Item Manager - Scans to-be-downloaded files for path traversal attempts, stopping & warning the user on detection
- Save & Log Location Fixer - Loads save.dat from current working directory and sends game logs to log.txt residing in current working directory

User Patches:
- Framerate Unlock patch - Adjusts the game's framerate limit to match the users display refresh rate. Additionally fixes some visuals caused by higher FPS
- Audio stuttering patch - Forces the game to stream all audio files from disk rather than try load in memory. Playback from memory is broken on Proton's AudioManager FMOD backend
- Legacy Chat patch - Removes the chat update paddings, chat tabs and `[S]` symbols
- No Guild Icon - Shifts Gem Store margin up to be next to Friends List
- Customized Title Screen - Changes the eye-searing sunny weather on title screen to a cozy night weather & adds OSGT-QOL watermark
- Server Switcher - You can now connect to various servers without modifying hosts file with this mod
- Cache Location Fixer - Binds game's cache folder to the same folder game installation is on. Useful for people that have multiple Growtopia installations.
- Bubble Opacity Slider - Backport of the modern client bubble opacity feature
- Hotkey patch - Adds hotkeys number 1 through 3 to cycle through your inventory's quickbar
- Start Music Slider - Backport of modern client Start Music volume slider, your ears will thank you
- Old Checkboxes - Removes the odd spacing introduced in version V2.997 on checkboxes
- Live GUI rebuilder - You can switch between resolutions and fullscreen without disconnecting from the server.
- Da Vinci Fake ID - Masquerades any instance of old Da Vinci Wings to the newer one so it would render properly.
- Legendary Wings patch - V3.02 does not render Legendary Wings at all due to an oversight, this patches the vanilla bug.
- Light Optimization - The light calculation for Growtopia is rather inefficient, this makes the light calculation algorithm a lot more performant.
- Playmod Overlay - Showcases active mods with timers on your character just below the chat.
- Allow releasing mutex - The game no longer runs into an integrity error when releasing mutex with external tools.
- Handle old logon handshakes - Allows the V3.02 to accept all historical logon handshake packets.
- Extended Chat Limit - Increases the chat history limit from the original 125 up to 500.
- Locale Switcher - Allows you to change your in-game country flag without having to restart your PC or game.

## Recommended additions
When Growtopia switched to using BGFX as its rendering framework, Windows clients started using DirectX9 as opposed to OpenGL. With this transition, several visuals broke.
You may be interested in installing [dxvk](https://github.com/doitsujin/dxvk) as it resolves some of the blending issues (most notably Sunset/beach weather).

## Credits / Contributors
- cernodile - creating the contents and logic of patches used in osgt-qol, reverse engineering client structures and classes
- houzeyhoo - creating the underlying patch framework