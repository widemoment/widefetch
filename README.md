# widefetch (Windows)

A small, fast, and clean **fastfetch / winfetch-style system info tool** written in C for Windows.

It shows:

* OS + kernel version
* Host + user
* Uptime
* Shell + terminal
* Resolution
* CPU + GPU
* Memory usage
* Windows-style ASCII logo
* Color palette

Once installed, you can run it from **any terminal** (CMD, PowerShell, Windows Terminal, VS Code terminal) by typing:

```
widefetch
```

---

## Requirements

* **Windows 10 / 11**

* **CMake**
  Download and install the Windows x64 installer from:
  [https://cmake.org/download](https://cmake.org/download)

  During installation, make sure you check:

  > Add CMake to the system PATH

* A C compiler (one of these):

  * **Visual Studio (recommended)** — install “Desktop development with C++”
  * OR **MinGW-w64 / MSYS2**

---

## Folder Structure

Your project should look roughly like this:

```
widefetch/
├─ build/
├─ include/
│  ├─ fmt.h
│  ├─ logo.h
│  ├─ util.h
│  └─ modules/
│     ├─ os.h
│     ├─ host.h
│     ├─ cpu.h
│     ├─ gpu.h
│     ├─ memory.h
│     └─ uptime.h
├─ src/
│  ├─ main.c
│  ├─ fmt.c
│  ├─ logo.c
│  └─ modules/
│     ├─ os.c
│     ├─ host.c
│     ├─ cpu.c
│     ├─ gpu.c
│     ├─ memory.c
│     └─ uptime.c
├─ install-widefetch.bat
├─ update-widefetch.bat
└─ CMakeLists.txt
```

---

## Installation (Easy Mode — Recommended)

This installs `widefetch` globally so it works in **every terminal** by just typing `widefetch`.

### 1) Download or clone the project

Place the folder anywhere, for example:

```
C:\Users\YourName\Desktop\widefetch
```

### 2) Open a terminal in the project folder

You can either:

* Shift + Right Click → “Open in Terminal”

Or:

```
cd C:\Users\YourName\Desktop\widefetch
```

### 3) Run the installer script

```
install-widefetch.bat
```

### 4) Open a NEW terminal window

Then run:

```
widefetch
```

You should see the Windows ASCII logo and your system information.

---

## What the Installer Does

The installer script automatically:

* Builds a **Release** version of widefetch using CMake

* Copies the executable to:

  ```
  C:\Tools\widefetch.exe
  ```

* Adds `C:\Tools` to your **User PATH**

* Makes `widefetch` work in:

  * CMD
  * PowerShell
  * Windows Terminal
  * VS Code terminal

You only need to run the installer once.

---

## Updating widefetch (After Code Changes)

If you modify the code or pull new updates, run:

```
update-widefetch.bat
```

Then open a new terminal and run:

```
widefetch
```

This rebuilds the Release version and updates the global command.

---

## Running Without Installing

If you just want to test it without installing globally, you can run it directly from the build folder:

```
build\Release\widefetch.exe
```

Or using the full path:

```
C:\Users\YourName\Desktop\widefetch\build\Release\widefetch.exe
```

---

## Uninstall

1. Delete the executable:

```
C:\Tools\widefetch.exe
```

2. Remove `C:\Tools` from PATH:

* Press `Win + R`

* Type:

  ```
  sysdm.cpl
  ```

* Go to **Advanced → Environment Variables**

* Under **User variables**, select `Path` → **Edit**

* Remove:

  ```
  C:\Tools
  ```

* Click OK on all windows

* Open a new terminal

---

## Troubleshooting

### `widefetch` is not recognized

Open a new terminal and try:

```
where widefetch
```

If nothing shows, rerun:

```
install-widefetch.bat
```

### `cmake` is not recognized

Check:

```
cmake --version
```

If this fails, reinstall CMake and make sure the PATH option was enabled during installation.

### Build fails

Make sure you have a working C compiler:

* Visual Studio: install **Desktop development with C++**
* Or MinGW/MSYS2 and ensure `gcc` is in PATH

---

## Notes

* The **Release build** is used for the global command (faster and smaller).
* The **Debug build** is useful for development and debugging.
* Windows “Fast Startup” can affect uptime. A full **Restart** resets it properly.

---

## License

This project is licensed under the **Widemoment Attribution License (WAL)**.

You are free to use, modify, and redistribute the code, but you must keep credit to the original author:

> Code written by widemoment (https://github.com/widemoment)

If you improve or modify the project:

> Code written by widemoment (https://github.com/widemoment), improved by [Your Name]
