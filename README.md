# ESP32 Dog Meal Tracker
This project is an ESP32-based dog meal tracker system that allows the user to monitor when their dog has been fed. The system contains integration with Apple HomeKit, which allows the user to remotely control and view the dog meal tracker with their iOS device.

This meal tracker system was designed with the [**ESP32 JC2432W328C 2.8 TFT Capacitive Touch Dev Board**](https://www.amazon.com/DIYmalls-Touchscreen-ESP-WROOM-32-Development-JC2432W328C/dp/B0D5H84RDB) in mind; however, implementation with similar boards can be accomplished, although minor changes to the pin mapping, display rotation, etc., may be necessary.

## Installation
- Clone the project repository or download the source code.
- Open the source code in Arduino IDE.
- Rename `secrets_example.h` to `secrets.h`
- Change the WiFi credentials in the `secrets.h` file.
- Change `DOG_NAMES` and `HK_SWITCH_NAMES` dog names in `Globals.cpp`
- In `UI.cpp`, change icon header file names and icon names in the `drawIcons()` function to reflect the icon header files for your dog. *(View below on how to create icon header files)*
- Install the required libraries (HomeSpan, NTPClient, TimeLib, and bb_captouch).
- Connect the ESP32 board to the computer.
- Select the board from the Tools menu.
- Set the upload speed to 115200.
- Upload the code to the ESP32 board.

## Connecting to Apple HomeKit
- Once the code has finished uploading to the ESP32 board, open the serial monitor in Arduino IDE.
- If the WiFi connection is successful, the serial monitor should display the board's IP address along with the 8-digit HomeKit pairing code.
- If the HomeKit pairing code is not displayed in the serial monitor, press `P` to display the code.
- You can change the pairing code by pressing `S` followed by the 8-digit new code.
- Pressing `?` will display the help menu for additional HomeSpan commands.
- When adding the accessory to Apple HomeKit, choose `Dog Meal Tracker` as the bridge and add the four accessories associated with the bridge as switches.

## Creating Icon Header Files for Displaying Images on the Display
It is necessary to create icon header files because the ESP32 cannot directly display PNG or SVG images. Any image of that type must first be converted into a bitmap array (raw RGB565 pixel data) and stored in a header file so it can be rendered on the TFT display. The PNG is converted to SVG so the image can be resized without quality loss.
- Get PNG image.
- Convert to SVG.
- I used Figma for the following:
    - Import SVG image.
    - Change the image size to the desired size that you want displayed on the board. 
    - If the image has a transparent background, you need to specify the color. Otherwise, the TFT display will set the default background to black. 
    - Specify the background by doing the following in Figma:
        - Place a rectangle behind your image and center the image inside the rectangle.
        - Change the rectangle color so it matches the background color that will be shown behind the transparent image on the display.
        - Group the two layers and export the group as a PNG.
- The `images` directory contains all the icon header files. Choose one of the header files to rename: `oldNameIcon.h` -> `newNameIcon.h`
- In the newly renamed header file, change the 3rd line to reflect your image (for example, of size 32x32 px): `const uint16_t oldNameIcon[48 * 48]` -> `const uint16_t newNameIcon[32 * 32]`
- Open up [LCD Image Converter](https://lcd-image-converter.riuson.com/en/about/) and select `New Image`.
- In the project, select `Options → Conversion`
- Under the `Image` tab, make sure the following settings are set
    - **Format:** Color R5G6B5
    - **Block size:** 16-bit
    - **Byte order:** Little-endian
- In the bottom left corner, press `Show Preview`
- In the window that pops up, copy all the hex codes.
- Go to the icon header file and replace all the old hex codes with the new hex codes.
- Save file.

## Demo
![IMG_7415](https://github.com/user-attachments/assets/c27b49ba-6ce5-42ed-a78d-ca8c7eeae2ec)
