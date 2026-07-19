[report1.docx](https://github.com/user-attachments/files/30165706/report1.docx)
# ESP32-LED-Blink
Controlling an LED using esp32 and VS Code - PlatformIO
Prepared by: Hiba Banat|   Date: July 19, 2026
Introduction
This report documents the steps I followed to set up an embedded development environment using Visual Studio Code and the PlatformIO extension: downloading the software, creating an account, installing the extension, creating a new project, an installation issue I ran into on Windows, and the final working project — turning an LED ON and OFF while tracking its status through the Serial Monitor.
Step 1: Downloading and Installing Visual Studio Code
VS Code was downloaded from the official website code.visualstudio.com, selecting the Windows version, then installed normally through the setup installer.
 
Step 2: Installing the PlatformIO IDE Extension
After opening VS Code, I went to the Extensions panel on the sidebar and searched for PlatformIO IDE, then clicked Install. The screenshot below shows the extension installed alongside related ESP-IDF extensions, and the PlatformIO Home walkthrough guide that opens automatically after setup, together with the integrated terminal pointing to the new project folder.
 
PlatformIO IDE extension installed, with the ESP-IDF Basic Usage Guide and terminal open
Step 3: Creating a PlatformIO Account
From PlatformIO Home (the page that opens automatically after installation), I went to the Account icon and chose Sign Up to create a new account by email, or logged in if an account already existed.
Step 4: Creating a New Project
From the same PIO Home page, I clicked New Project, entered a project name, selected the board — an ESP32 Dev Module (esp32dev) — chose the Arduino framework, and picked a location to save the project. PlatformIO then generated the project configuration file platformio.ini shown below, which defines the platform, board, framework, monitor speed, and upload port used for the project.
 
platformio.ini — the generated project configuration file (board: esp32dev, framework: arduino)
The screenshot below shows the resulting project structure opened in VS Code, with the main source file src/main.cpp and the terminal pointing to the project folder.
 
Project structure and main.cpp opened in VS Code, with the integrated terminal
Step 5: The Problem I Faced During Installation (Windows-related)
 During the ESP32 setup, the development board was not recognized by the computer because the CP2102 USB-to-UART driver was missing. As a result, Windows displayed the device under Other Devices with Error Code 28, indicating that no compatible driver was installed. Consequently, Visual Studio Code (PlatformIO) was unable to detect the board, no COM port was assigned, and the firmware could not be uploaded or monitored through the Serial Monitor. 
The issue was resolved by following the configuration guide provided in the project documentation, which included installing the correct CP2102 USB-to-UART driver and configuring the development environment. After completing these steps, Windows successfully recognized the ESP32 as a serial device, assigned a valid COM port, and PlatformIO was able to upload programs and communicate with the board normally.

Project setup guide (Google Docs):
https://docs.google.com/document/d/1r----DytNP4usgfL1DBQ_7RTqCAibtwTNp1p1wUODig/edit?usp=sharing

Code Development and Version Control
The ESP32 application was developed using Visual Studio Code with the PlatformIO extension. The project was configured using the Arduino framework, and the source code was implemented in the main.cpp file. After verifying that the program compiled successfully and operated correctly on the ESP32 development board, the project was managed using Git for version control.
To upload the project to GitHub, a new repository was created and initialized locally using Git. The project files were added to the local repository, committed with an initial commit message, and linked to the remote GitHub repository. Finally, the project was pushed to GitHub, allowing the source code to be stored securely online, tracked through version control, and shared for collaboration and future updates.
 








Step 6: Description of the Project
The project turns an LED ON and OFF repeatedly (a classic "Blink"), while printing the LED's status (LED ON / LED OFF) to the Serial Monitor inside VS Code each time it changes, so the circuit's behavior can be followed live while it runs.
 

 


Step 7: The Project Code (Sketch)
The code below is the full main.cpp sketch for the project. It toggles the LED on pin 2 every second and prints the current status to the Serial Monitor at 115200 baud
Conclusion
Through this project I learned how to set up a professional embedded development environment for the ESP32 using VS Code and PlatformIO instead of the traditional Arduino IDE, how to manage a PlatformIO account and its projects, how to work through a Windows-related installation issue, and how to build and run a simple project that turns an LED on and off while monitoring its status through the Serial Monitor.
