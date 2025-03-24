#include"myRandom.h"

bool generate_random_bytes(uint8_t* buffer, size_t length) {
    NTSTATUS status = BCryptGenRandom(NULL, buffer, length, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return status == 0;
}

int generate_random_number(int N) {
    if (N <= 0) return -1;
    uint8_t buffer[4];
    if (!generate_random_bytes(buffer, sizeof(buffer))) {
        return -1;
    }
    uint32_t random_value = *(uint32_t*)buffer;
    return random_value % N;
}