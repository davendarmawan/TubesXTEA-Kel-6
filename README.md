# Secure GPS Tracking System: Development of an ESP32-based Tracker with XTEA-Encrypted Coordinates
> This repository is intended for storing data related to the XTEA GPS Tracking System Project by Group 6 - Problem Solving with C

### Link ke LaTeX
[Laporan Tugas Besar XTEA - Kelompok 6](https://www.overleaf.com/project/63d8d457e6db21ec23b3274a)

### Anggota Kelompok
- Maritza Humaira (13221026)
- Fathiya Amani Shabira (13221032)
- Daven Darmawan Sendjaya (13221062)

### Brief Description
**XTEA** (Extended Tiny Encryption Algorithm) merupakan sebuah metode kriptografi yang berbasiskan Feistel Block Cipher. Feistel Block Cipher bekerja dengan membagi input *plaintext* menjadi dua bagian dan mengacaknya dengan *key* yang dimasukkan pengguna. Setelah Feistel Block telah terdefinisi, maka *plaintext* akan dienkripsi berdasarkan algoritma yang digunakan, yaitu XTEA.

XTEA bekerja dengan menerima input 64-bit (8 byte) dari pengguna. Input 64-bit akan dipecah ke dalam dua bagian (*left dan right*) yang masing-masingnya berjumlah 32-bit. Lalu, sistem juga akan menerima input *key* berjumlah 128-bit yang dibagi menjadi empat buah key berjumlah 32-bit, yaitu key<sub>0</sub>, key<sub>1</sub>, key<sub>2</sub>, dan key<sub>3</sub>.

Setelah menerima input *plaintext* dan *key*, maka sistem akan mendefinisikan sebuah variabel "sum". Variabel ini digunakan dalam proses enkripsi. Proses pengacakan *plaintext* dilakukan secara bit per bit (*bitwise*) dan dilakukan secara berulang-ulang. Pengulangan terbaik dilakukan sebanyak 64 kali. Setiap pengulangan dilakukan, nilai variabel "sum" akan bertambah sebanyak *delta*, yang merupakan konstanta *golden ratio* yang bernilai (9E3779B9)<sub>16</sub>. Proses enkripsi berlangsung melalui operator *bitwise* shift left (<<), shift right (>>), AND (&), dan XOR (^), serta operasi aritmetika penjumlahan (+). 

### Cara Kerja
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