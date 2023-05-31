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

#### Pengaturan Kriptografi
Proses enkripsi dan dekripsi dilakukan dengan pengaturan sebagai berikut,
- Program tersusun atas empat key dengan jumlah 32-bit, yaitu 0x23FB, 0x89FA, 0xD3BC, dan 0x18AF
- Operasi *encryption* dilakukan sebanyak 64 kali

Berikut merupakan potongan kode dari fungsi *encryption* dan *decryption* melalui algoritma XTEA dalam bahasa C,

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

#### Langkah 1: Hashing Password
Program akan menerima input sebuah string enam karakter sebagai *password* yang harus mengandung kriteria-kriteria berikut,
- Memiliki minimal 1 buah karakter *uppercase*
- Memiliki minimal 1 buah karakter *lowercase*
- Memiliki minimal 1 buah karakter numerik
- Karakter dalam *password* tidak boleh kurang atau lebih dari 6 karakter

*Password* akan tersusun dengan beberapa karakter *uppercase*, *lowercase*, numerik, serta karakter spesial lainnya. Lalu, setiap karakter akan dicari padanan nilai ASCII-nya. Nilai ASCII dari setiap karakter akan berbentuk *integer*.

Password akan dienkripsi menggunakan algoritma XTEA berdasarkan *keys* dan jumlah *rounds* yang telah dideskripsikan sebelumnya.

Karena XTEA memerlukan dua input *unsigned integer* 32-bit, maka praktikan harus membagi kode ASCII menjadi dua buah integer berdasarkan aturan berikut,
```math
k_1 = k/2
```
```math
k_2 = k - k_1 
```
dengan $k$ merupakan kode ASCII dari karakter, $k_1$ merupakan nilai input pertama, dan $k_2$ merupakan nilai input kedua untuk fungsi *encipher*.

Setelah pembagian kode ASCII dilakukan untuk setiap karakter, maka proses enkripsi berdasarkan algoritma XTEA langsung dijalankan. Setelah kode berhasil terenkripsi, maka kode tersebut akan di-*hashing*.

Sebelum di-*hashing*, maka nilai $k_1$ dan $k_2$ yang telah dienkripsi akan dijumlahkan kembali. Misal, hasil penjumlahan $k_1$ dan $k_2$ adalah $c$ dan *password* memiliki 6 buah karakter, oleh karena itu, hasil penjumlahan akan dinotasikan sebagai berikut,

```math
c_i, i\in\{1,2,3,4,5,6\}
```

Setelah itu, proses *hashing* akan dilakukan berdasarkan aturan berikut,

```math
((c_1 + c_2)\bmod c_3) \times ((c_4 + c_5)\bmod c_6)
```

> **Contoh Proses Hashing Password:**
>
> Input password: `Pmc%12`
>
> Password (hashed): `0x8a7b4d5e`

**Note 1**: *Password* yang telah di-*hashing* tidak dapat dikembalikan kembali ke dalam bentuk *plain text password* yang dimasukkan oleh pengguna. Hal ini dimaksudkan agar *password* hanya bisa diketahui oleh pengguna (bahkan tidak bisa diketahui oleh pihak pembuat algoritma). 

**Note 2**: Saat pengguna hendak login, password yang dimasukkan oleh pengguna akan di-*hashing* dan akan divalidasi dengan *password* yang tersimpan pada sistem (*hashed password*). Bila password sama, maka pengguna bisa mengakses layanan.

#### Langkah 2: Under Construction
Ditunggu langkah selanjutnya :)


### *Quotes on Cryptography*
>  *"A cryptographic system should be secure even if everything about the system, except the key, is public knowledge.”*
>
> ― Auguste Kerckhoffs