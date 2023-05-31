# Secure GPS Tracking System: Development of an ESP32-based Tracker with XTEA-Encrypted Coordinates
> This repository is intended for data storing use, related to the XTEA GPS Tracking System Project by Group 6 - Problem Solving with C

### External Links
[XTEA GPS Tracker Project Report - Group 6](https://www.overleaf.com/project/63d8d457e6db21ec23b3274a)

### Authors
- Daven Darmawan Sendjaya (13221062@mahasiswa.itb.ac.id)
- Fathiya Amani Shabira (13221032@mahasiswa.itb.ac.id)
- Maritza Humaira (13221026@mahasiswa.itb.ac.id)
- Muhammad Ogin Hasanuddin (moginh@itb.ac.id)

### Brief Description
**XTEA** (Extended Tiny Encryption Algorithm) is a block cryptographic algorithm that can be used for encryption of sensitive data. The algorithm is based on TEA (Tiny Encryption Algorithm) with a few added loops to increase the security and strength of the cryptographic system.

XTEA works by receiving a 64-bit input (8 bytes) from user. The 64-bit input should be divided into two 32-bit inputs, which is denoted by $v_0$ and $v_1$. The cryptographic system also receives a 128-bit $key$ input, which is divided into four 32-bit keys. These 32-bit keys are denoted as $key_0$, $key_1$, $key_2$, $key_3$. 

After receiving a plaintext and a key input, system will define a variable $sum$. This variable is used in the encryption procedure. The encryption procedure is done bitwise and repeatedly (according to how many rounds that user has set). The best round is determined to be 64 rounds. For every loops, the $sum$ variable will be incremented by $delta$, which is a variable of golden ratio. This variable has a value of (9E3779B9)<sub>16</sub>. The encryption procedure runs by executing the shift left ($\ll$), shift right ($\gg$), AND (&), and XOR ($\oplus$) bitwise operators. This procedure also uses the arithmetic operator plus (+). The decryption algorithm is also executed similarly by reversing the decryption algorithm.

In this study, two ESP32s, with each device connected to a GPS Module, is used to determine the location of respective devices. The first ESP32 is placed at the convenience of the user, i.e. at his/her house. The second ESP32 is then placed on the user's vehicle. The second ESP32 is used to receive GPS coordinates (latitude and longitude) from the GPS module. This data is then transmitted over MQTT protocol. To ensure the security of data, we need cryptographic measures so that data are obscured. In this case, we use XTEA as this cryptographic system is easy to implement on a microcontroller (ESP32), with cheap and efficient resource usage.

The first ESP32 will then receive and decrypt the message sent over the MQTT protocol. This device is then calculating the distance between both devices with the Haversine formula. If the distance is detected abnormally, i.e. very far from the first ESP-32, then we can conclude that the car is stolen. Buzzer will ring as a notification to the user if distance abnormalities are detected.

### How It Works?
![System Overview](/images/system_diagram.svg "System Overview")

<figcaption align = "center"><b>Fig. 1 - System Overview</b></figcaption>

![Block Diagram](/images/block_diagram.svg "Block Diagram")

<figcaption align = "center"><b>Fig. 2 - Block Diagram of The Proposed System</b></figcaption>

#### Cryptography Settings
The encryption and decryption procedures are executed with encryption settings as follows,
- The procedure uses four 32-bit keys, which are 0x23FB, 0x89FA, 0xD3BC, dan 0x18AF
- The encryption and decryption procedures are executed 64 rounds.

The XTEA algorithm (for encryption and decryption) used on this project was written as follows,

```c
#include <stdint.h>
#include <stdio.h>

void encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;
    for (i=0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
    }
    v[0]=v0; v[1]=v1;
}

void decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*num_rounds;
    for (i=0; i < num_rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}
```

#### Step 1: Arranging ESP32s and GPS Modules
In this project, we prepared two ESP32s, with GPS modules connected to each devices. The GPS module is used to acquire GPS coordinates data when the device is connected to satellites. On the first ESP32, only GPS module is connected to the device.

On the second ESP32, we connected GPS module and buzzer. GPS module is used to acquire location data of the second ESP32. This location data would then be compared with the first location data to calculate the distance between both devices. Other than GPS module, a buzzer is also connected to the second device. This buzzer served as a notification to user if the device detects anomalies on the vehicle's distance from the first ESP32.

#### Step 2: Acquiring First GPS Coordinates
Both ESP32 acquires GPS coordinates from GPS modules that were connected to the device. Furthermore, we had to place the devices directly to the sky to ensure good connection with the satellites so they could acquire real time coordinates data.

After acquiring the location data of the first ESP32, the data should be encrypted using XTEA encryption system. The encrypted location data would then be published to the MQTT protocol. This encryption procedure ensured that the data could be published as safe as possible.

#### Step 3: Acquiring Second GPS Coordinates and Comparing the Distance
On the second ESP32, the encrypted location data from the first ESP32 was acquired by subscribing to MQTT. Before being processed, the data was decrypted to show the proper coordinate data. At the same time, the GPS module on the second ESP32 also acquired location details on the ESP32.

After getting both location data, we could compute the distance of both devices by using the Haversine formula. This formula is used to find the distance between two coordinate points on a spherical surface when latitude and longitude of the two points have been determined.

To calculate distance, firstly, we need to convert the unit degrees of latitude and longitude to radians with the formula as follows, 

Let $p_{deg}$ be a point given in degrees and $p_{rad}$ be a point given in radians, hence
```math
k_{rad} = \frac{k_{\deg} \cdot \pi}{180} \text{ radians}
```

To calculate distance between two points on a spherical surface, we can use the Haversine formula as follows

Let,
- $r$ = Radius of spherical surface
- $\phi_1$ = Latitude of point 1 (radians)
- $\phi_2$ = Latitude of point 2 (radians)
- $\lambda_1$ = Longitude of point 1 (radians)
- $\lambda_2$ = Longitude of point 2 (radians)

then the distance can be calulated as, 
```math
d = 2r \cdot \arcsin\left(\sqrt{\sin^2\left(\frac{\phi_2-\phi_1}{2}\right) + \cos(\phi_1) \cdot \cos (\phi_2) \cdot \sin^2\left(\frac{\lambda_2-\lambda_1}{2}\right) } \right)
```
Source: <https://www.geeksforgeeks.org/haversine-formula-to-find-distance-between-two-points-on-a-sphere/>

After acquiring distance between these two points, we had to set the conditions of buzzer. If the distance was less than 10 meters, then buzzer would output high pitch tone. If the distance was around 10-20 meters, the buzzer would output low pitch tone. If the distance was over 20 meters, the buzzer would no longer output any tones.

### Remarks on Folders
Each `.ino` files can be accessed on the `ESP32 Files` folder. The `ESP32 Files` folder contains additional folders which shows the usage of each `.ino` files.

The folders are as follows,
- Folder `gps_decrypt`: Algorithm for the second ESP32. This algorithm works by decrypting the location data from the first ESP32. This location data will then be compared with the first ESP32 location data acquired from MQTT. After calculating the distance data, buzzer will output tones based on the distance data.
- Folder `gps_encrypt`: Algorithm for the first ESP32. This algorithms works by acquiring location data from the GPS Module and encrypt the data using XTEA. The encrypted coordinates is then published to MQTT.
- Folder `gps_publish`: Similar to `gps_encrypt` without encryption (to calculate execution and resource usage without XTEA).
- Folder `gps_subscribe`: Similar to `gps_subscribe` without decryption (to calculate execution and resource usage without XTEA).

### Quotes on Cryptography
>  *"A cryptographic system should be secure even if everything about the system, except the key, is public knowledge.”*
>
> ― Auguste Kerckhoffs