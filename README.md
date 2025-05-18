## Updated Version (Maintained by rsna6ce)

This is an updated version of PowerMode, maintained to work with Notepad++ v8.8.1 as of May 2025. The plugin has been fixed to handle events properly and is compatible with modern versions of Notepad++.

### Changes
- Fixed event handling to support Scintilla 5.x (Notepad++ v8.6.5).
- Removed dependency on `SCN_MODIFIED` due to structure mismatch.
- Added support for copy/paste and Undo/Redo events.
- Removed debug logs and cleaned up the code.

### Installation (Updated)

#### Option 1: Download the Pre-built DLL (Recommended)
Download the DLL and enjoy! 🚀  
https://github.com/rsna6ce/PowerMode/releases  
1. Extract the downloaded `PowerMode.dll` (e.g., from `PowerMode-v1.0.0.zip`).
2. Copy it to `Notepad++/Plugins/PowerMode/`.
3. Restart Notepad++.

#### Option 2: Build from Source
1. Build the project using Visual Studio 2022.
2. Copy the generated `PowerMode.dll` (from the Debug/Release folder) to `Notepad++/Plugins/PowerMode/`.
3. Restart Notepad++.

### Compatibility
- Tested with Notepad++ v8.8.1.
- May work with older versions (e.g., v7.x), but compatibility is not guaranteed.

### Credits
This project is a fork of [dail8859/PowerMode](https://github.com/dail8859/PowerMode), originally developed by Justin Dailey.


---


# Power Mode
A completely useless Notepad++ plugin. Inspired by the [Atom package](https://atom.io/packages/activate-power-mode).

![demo](/img/demo.gif)

## Installation
Download one of the [releases](https://github.com/dail8859/PowerMode/releases) and extract the DLL. Copy this into Notepad++'s plugin directory. This will either be located in the installation directory or under `%APPDATA%\Notepad++\plugins`.

## Development
The code has been developed using MSVC 2013. Building the Release configuration will generate the DLL which can be used by Notepad++. For convenience, MSVC copies the built DLL into the Notepad++ plugin directory. 

## License
This code is released under the [GNU General Public License version 2](http://www.gnu.org/licenses/gpl-2.0.txt).

### Disclaimer
This is not intended to be useful or bug free. Pull requests are welcome, bug reports however will most likely go unfixed.
