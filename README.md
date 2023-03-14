# XTEA by Kelompok 6
> Disusun untuk memenuhi nilai Tugas Besar EL2008: Penyelesaian Masalah dengan C

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