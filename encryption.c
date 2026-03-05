#include <string.h>
#include "encryption.h"

/* ── substitution_cipher ─────────────────────────────────────────────────── *
 * Shifts each printable ASCII character (32-126) forward by:
 *   (base_key[i % 50] + (salt >> (i % 8) * 4) & 0xF) % PRINTABLE_RANGE
 * The salt mixes in unique per-user randomness so the same plaintext
 * produces a different ciphertext for each user.
 * ─────────────────────────────────────────────────────────────────────────── */
void substitution_cipher(const char *src, char *dst, unsigned int salt) {
    static const int base_key[] = {7, 13, 3, 19, 11, 5, 17, 2, 23, 29};

    size_t len = strlen(src);
    for (size_t i = 0; i < len; i++) {
        int c = (unsigned char)src[i];
        if (c >= 32 && c <= 126) {
            int salt_contribution = (salt >> ((i % 8) * 4)) & 0xF;
            int shift = (base_key[i % 10] + salt_contribution) % PRINTABLE_RANGE;
            dst[i] = (char)(((c - 32 + shift) % PRINTABLE_RANGE) + 32);
        } else {
            dst[i] = (char)c;
        }
    }
    dst[len] = '\0';
    #undef PRINTABLE_RANGE

}
