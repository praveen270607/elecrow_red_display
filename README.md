# Setup Guide for ESP32 with TFT Display

### Step 1: Install ESP32 Board
- Open **Board Manager** in Arduino IDE.  
- Install **ESP32 v2.0.15**.  

### Step 2: Configure Arduino IDE
Go to **Tools** and set:
- **Board:** ESP32 WROVER Module  
- **PSRAM:** Disabled  

### Step 3: Install Required Libraries
In **Library Manager**, install:
- **TFT_eSPI** (v2.5.43)  
- **lvgl** (v8.3.11)  

### Step 4: Configure TFT_eSPI
- Navigate to the `TFT_eSPI` folder inside your Arduino **libraries** folder.  
- Replace the existing `User_Setup.h` with the one provided in this repo.  

### Step 5: Configure lvgl
- Take the `lv_conf.h` file from this repo.  
- Paste it into your Arduino **libraries** folder.  

### Step 6: Upload and Run
Once all the setup is done, upload the program to your ESP32 and run it.

# Instructions

### Before Uploading the Code
- Change the Wi-Fi credentials according to the device.  
- Change the topic according to the device assembled.  

### After Uploading the Code
- Connect the display with the device and test it without full assembly.  
- Check whether the values are updated accordingly.  
