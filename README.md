<a href="./LIESMICH.md">==> Deutsche Version</a>   
Last update: March 21, 2024 <a name="up"></a>   
<h1>ESP32: Control of an N-track DCC module</h1>   

# Overview
This repository contains a program for an EPS32 with which you can control an N-gauge DCC module that contains a display, a turnout, a decoupler and a track that can be switched off.   
![Modul_Abstellgleis](./images/300_modul_siding_1_240321.png "Modul Abstellgleis")   
Image 1: N-gauge DCC module "Siding"_ 

This repository also contains programs that can be used to test individual circuits that are required for the overall control system. Each program contains a `README.md` file that describes this program in more detail.   

# Control of the N-track DCC module
The N-track module can be controlled in three ways:
* Direct control via the controls on the front panel   
* Control via DCC commands   
* Control via WLAN using MQTT commands   

[Back to top of page](#up)
