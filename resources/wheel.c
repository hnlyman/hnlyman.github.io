#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/// The number of primes used to make the `struct Wheel`
#define WHEEL_K 5

struct Wheel {
    size_t size;

    /// Numbers with these remainders modulo `size` are coprime to `size`.
    uint32_t* coprime_spokes;
    /// The number of elements in `spokes`
    size_t n_spokes;
};

/// Initialize a `struct Wheel` using the first `k` prime numbers.
///
/// `primes`: an array of size at least `k` whose first `k` elements are the
///           first `k` primes in order
struct Wheel wheel_init(uint32_t* primes, size_t k) {
    struct Wheel wheel;

    wheel.size = 1;
    for (size_t i=0; i<k; i++)
        wheel.size *= primes[i];

    // All the remainders mod `wheel.size` which imply divisibility by one of
    // the first `k` primes
    uint32_t *coprime_spokes = malloc( wheel.size * sizeof(uint32_t) );
    wheel.n_spokes = 0; // index of the next element to be added to
                        // `coprime_spokes`

    for (size_t i=0; i<wheel.size; i++) {
        for (size_t j=0; j<k; j++) {
            if (i % primes[j] == 0) goto continue_loop;
        }

        coprime_spokes[wheel.n_spokes] = i;
        wheel.n_spokes ++;

continue_loop: ;
    }

    wheel.coprime_spokes = malloc( wheel.n_spokes * sizeof(uint32_t) );
    memcpy(wheel.coprime_spokes, coprime_spokes, wheel.n_spokes * sizeof(uint32_t));
    free(coprime_spokes);

    return wheel;
}

/// Returns the largest `uint32_t` coprime to `wheel.size`
uint32_t wheel_last(struct Wheel wheel) {
    for (uint32_t l = 0xFFFFFFFF;; l--) {
        for(size_t i=0; i<wheel.n_spokes; i++) {
            if ( (l - wheel.coprime_spokes[i]) % wheel.size == 0 )
                return l;
        }
    }
}

void wheel_free(struct Wheel wheel) {
    free(wheel.coprime_spokes);
}

/// Returns true iff `n` is a prime number.
///
/// `primes`: an array of prime numbers in order, starting from 2, skipping none,
/// and going at least up to the square root of `n`.
bool is_prime(uint32_t n, uint32_t* primes) {
    for (size_t i=0; ; i++) {
        uint32_t p = primes[i];

        if (n % p == 0) {
            return false;
        }

        if (p >= 0xFFFF || p*p >= n) return true;
    }
}

int main() {
    uint32_t* primes = (uint32_t*) malloc(203739216 * sizeof(uint32_t));

    primes[0] = 2;
    size_t p_idx = 1; // index of the next element to be added to `primes`
    
    // get first `WHEEL_K` primes to initialize wheel
    for (uint32_t n=3; p_idx < WHEEL_K; n+=2) {
         if ( is_prime(n, primes) ) {
            primes[p_idx] = n;
            p_idx++;
        }
    }

    struct Wheel wheel = wheel_init(primes, WHEEL_K);

    // use sequential trial division to get all the primes up to `wheel.size`,
    // so we can start looking for primes with `turn = 1` in the main loop
    for (uint32_t n = 1+primes[p_idx-1]; n<wheel.size; n++) {
        if ( is_prime(n, primes) ) {
            primes[p_idx] = n;
            p_idx++;
        }       
    }

    uint32_t last = wheel_last(wheel);
    uint32_t turn = 1;
    while (true) {
        for (size_t i=0; i<wheel.n_spokes; i++) {
            uint32_t n = wheel.size * turn + wheel.coprime_spokes[i];

            if ( is_prime(n, primes) ) {
                primes[p_idx] = n;
                p_idx++;
            }       

            if (n == last)
                goto exit_loop;
        }

        turn++;
    }
exit_loop: ;

    FILE* prime_file = fopen("PRIMES", "wb");
    fwrite(primes, 4, p_idx, prime_file);

    fclose(prime_file);
    free(primes);

    return 0;
}
