#include <stdio.h>
#include <stdint.h>
#include "XTEA.c"

int main(){
    // Encryption Settings
    unsigned int rounds = 64;
    char pw[7];
    uint32_t key[4] = {0x23FB, 0x89FA, 0xD3BC, 0x18AF};
    uint32_t m_pw[6][2];

    // Input Password
    printf("Masukkan password: ");
    scanf("%s", pw);

    // Divide ASCII Code
    for (int i = 0; i < 6; i++){
        m_pw[i][0] = pw[i] / 2;
        m_pw[i][1] = pw[i] - m_pw[i][0];
    }

    // Encipher Process 
    for (int i = 0; i < 6; i++){
        encipher(rounds, m_pw[i], key);
    }

    // Hashing -> ((C0 + C1) mod C2) * ((C3 + C4) mod C5)
    int c_pw[6];

    for (int i = 0; i < 6; i++){
        c_pw[i] = m_pw[i][0] + m_pw[i][0];
    }

    int f_pw = ((c_pw[0] + c_pw[1]) % c_pw[2]) * ((c_pw[3] + c_pw[4]) % c_pw[5]);

    printf("%x", f_pw);


}