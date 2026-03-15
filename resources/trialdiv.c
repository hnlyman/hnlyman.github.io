#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/// Returns `true` iff `n` is a prime number.
///
/// `primes`: an array of prime numbers in order, starting from 2, skipping
/// none, and going at least up to the square root of `n`.
bool is_prime(uint32_t n, uint32_t* primes) {
    for (size_t i=0; ; i++) {
        uint32_t p = primes[i];

        if (n % p == 0) {
            return false;
        }

        // Comparison against 0xFFFF prevents overflow in `p*p`.
        if (p >= 0xFFFF || p*p >= n) return true;
    }
}

int main() {
    uint32_t* primes = (uint32_t*) malloc(203739216 * sizeof(uint32_t));

    primes[0] = 2;
    size_t p_idx = 1; // index of the next element to be added to `primes`

    for (uint32_t n=3; n<=0xFFFFFFFF; n+=2) {
        if ( is_prime(n, primes) ) {
            primes[p_idx] = n;
            p_idx++;
        }

        // Prevents overflow in `n+=2`
        if (n == 0xFFFFFFFF) break;
    }

    FILE* prime_file = fopen("PRIMES", "wb");
    fwrite(primes, 4, p_idx, prime_file);

    fclose(prime_file);
    free(primes);

    return 0;
}
