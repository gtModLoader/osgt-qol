<div align="center">

<img width="200" src="src/res/logo.png" alt="OSGT-QOL logo, created by Corrects">

# OSGT-QOL
</div>

OSGT-QOL is a quality of life mod for Growtopia 3.02 64-bit Windows clients aimed at Oldschool Growtopia players. It features various quality of life and security changes along extending several limitations of the vanilla game client.

> [!NOTE]
> The game modification is provided as a working prototype. The core maintainers of the project are working on a rewrite in another repository. Contributions are still very much welcome, things done here will be relevant for the rewrite.

## Table of Contents
- [Cache and save.dat separation](#cache-and-savedat-separation)
- [Installation](#installation)
- [Uninstalling](#uninstalling)
- [What does this mod change?](#what-does-this-mod-change)
- [Configuration](#configuration)
- [Credits & Contributors](#credits--contributors)

## Cache and save.dat separation
If you play on more than one server or play on the live servers, you may want to clone your V3.02 Growtopia installation to not have to reinstall the game and uninstall the mod each time.

First ensure you have V3.02 installed then once you have verified your game version, press `Windows Key` + `R` and paste in `%LocalAppData%`. This will open a file explorer window, you should locate your Growtopia folder here.

Once you've found your Growtopia folder, make a copy of it (click on it once and press `Ctrl` + `C`, followed by `Ctrl` + `V`). Rename the created folder (typically `Growtopia - Copy` or similar depending on your system language) to `OSGT` or another folder of your liking, that is completely up to you. After you have cloned your install folder, you can proceed with [Installation](#installation)

If you want to make a quick way to access this install of your game along your other copies, go inside the cloned folder, hold shift and right click Growtopia.exe, select "Send to" and then "Desktop (create shortcut)".

Optionally, you can edit the newly created shortcut by selecting dinput8.dll for its icon, this will put the project logo as the shortcut logo.

## Installation
> [!WARNING]
> If you play on the live Growtopia servers or any other servers and you did not follow [Cache and save.dat separation](#cache-and-savedat-separation), you will need to uninstall the mod each time you switch between other servers that do not support V3.02.

Navigate to [Releases](https://github.com/gtModLoader/osgt-qol/releases) section and download the latest `osgt-qol-*.zip` package.

Open the downloaded zip file then drag and drop `dinput8.dll` to your game installation folder.

If you are unsure how to find your installation folder, the default install can be found by pressing `Windows Key` + `R`, then pasting in `%LocalAppData%\Growtopia` and pressing Enter.

If you followed [Cache and save.dat separation](#cache-and-savedat-separation) guide, then your game installation folder is the cloned folder you made earlier.

## Uninstalling
Open your game installation folder and delete `dinput8.dll` file.

## What does this mod change?
OSGT-QOL comes with several embedded game modifications, some of which can't be disabled, others which can be toggled by the patches.txt file (see [Configuration](#configuration)).

Core Patches (always enabled):
- CRC Integrity Check bypass - Required for code modification to work
- No DACL Modifications - Removes restrictive ACEs applied to the game, which allows for other programs/scripts to interact with the game without needing Administrator privileges
- About Menu attribution - Adds credits for the modification & lists all the loaded in user patches
- Secure Item Manager - Scans to-be-downloaded files for path traversal attempts, stopping & warning the user on detection
- Save & Log Location Fixer - Loads save.dat from current working directory and sends game logs to log.txt residing in current working directory

User-facing, most notable/impactful patches:
- Framerate Unlock patch - Adjusts the game's framerate limit to match the users display refresh rate. Additionally fixes some visuals caused by higher FPS
- Audio stuttering patch - Forces the game to stream all audio files from disk rather than try load in memory. Playback from memory is broken on Proton's AudioManager FMOD backend
- Server Switcher - You can now connect to any server that supports V3.02 without modifying hosts file with this mod
- Cache Location Fixer - Binds game's cache folder to the same folder game installation is on. Useful for people that have multiple Growtopia installations
- Live GUI rebuilder - You can switch between resolutions and fullscreen without disconnecting from the server
- Status Effects Overlay - Showcases active status effects with timers on your character just below the chat
- Music mute fix - Allows you to mute music volume without game inherting sfx volume instead for it - does come with drawback that unmuting will start the last played track from start, but it's far better than V3.02 default behaviour

User-facing, visible or impactful patches:
- Legacy Chat patch - Removes the chat update paddings, chat tabs and `[S]` symbols
- No Guild Icon - Shifts Gem Store margin up to be next to Friends List
- Customized Title Screen - Changes the eye-searing sunny weather on title screen to a cozy night weather & adds OSGT-QOL watermark
- Bubble Opacity Slider - Backport of the modern client bubble opacity feature
- Hide my GUI - Adds a new keybind (CTRL+H) to toggle the clickability of GameMenu's rightmost buttons. You can also change the opacity (up to fully transparent) within in-game settings
- Hotkey patch - Adds hotkeys number 1 through 3 to cycle through your inventory's quickbar
- Quick Toggle Space to Punch - If you like to parkour using spacebar, but also like punching with spacebar, this patch adds Ctrl+P hotkey to toggle between them
- Start Music Slider - Backport of modern client Start Music volume slider, your ears will thank you
- Old Checkboxes - Removes the odd spacing introduced in version V2.997 on checkboxes
- Da Vinci Fake ID - Masquerades any instance of old Da Vinci Wings to the newer one so it would render properly
- Legendary Wings patch - V3.02 does not render Legendary Wings at all due to an oversight, this patches the vanilla bug
- Extended Chat Limit - Increases the chat history limit from the original 125 up to 500
- Locale Switcher - Allows you to change your in-game country flag without having to restart your PC or game
- URL buttons fix - Fixes the broken GDPR logic that blocks opening any URL buttons if the game cannot phone home
- ToggleCtrlJump - Adds a toggle to remove CTRL to Jump control
- Pause Menu No AAP - Removes "Secure my Account" button in Pause Menu, given the target audience of the mod, this mod makes sense for restoring visual identity. Can be toggled on the fly in OSGT-QOL Settings.
- Handle drag fix - Fixes the misinput that causes the chat or inventory to bounce back for grab handles that happens when you release their hold too fast
- Better Log Handle DPI - Uses more sensible DPI values for desktop use rather than mobile-like scaling, most notable on 2K resolution.
- Instant world buttons - Makes the buttons in the world selection screen appear instantly instead of fading in after a while.
- Quick drop - Allows you to optionally bind Q to drop the currently selected item.
- Anchor camera to player - Always centers camera around the player including at the world borders. This is disabled by default and can be enabled in the settings in-game.

User-facing patches that don't really impact your gameplay or are made for experimenting:
- Light Optimization - The light calculation for Growtopia is rather inefficient, this makes the light calculation algorithm a lot more performant
- Allow releasing mutex - The game no longer runs into an integrity error when releasing mutex with external tools
- Items.dat CrashMeNot - Makes the game not crash on items.dat versions up to 21. This does not store the information anywhere, only seeks through it
- Handle old logon handshakes - Allows the V3.02 to accept all historical logon handshake packets
- Blood Moon Demo Weather - An experimental extension to game that adds a completely new weather to the game without replacing any existing weather.

## Configuration
Currently, if you wish to disable any specific mod, you will need to download the [patches.txt](https://raw.githubusercontent.com/gtModLoader/osgt-qol/refs/heads/main/patches.txt) file from this repository and place it in the same folder as the mod.
By default, without this file, OSGT-QOL will enable all of the built-in mods.

To disable a mod, add a `-` at the start of a mod's name (for example `+my_mod` should become `-+my_mod`)

## Recommended additions (for technical users)
When Growtopia switched to using BGFX as its rendering framework, Windows clients started using DirectX9 as opposed to OpenGL. With this transition, several visuals broke.
You may be interested in installing [dxvk](https://github.com/doitsujin/dxvk) as it resolves some of the blending issues (most notably Sunset/beach weather).

## Credits / Contributors
- cernodile - creating the contents and logic of patches used in OSGT-QOL, reverse engineering client structures and classes
- houzeyhoo - creating the underlying patch framework, build system, and other minor improvements
