#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

struct BitArray {
    size_t len;
    uint64_t* buffer;
};

/// Constructs a `struct BitArray` that can store at least `size` bits, with
/// all bits initialized to `0`.
struct BitArray bitarray_init(size_t size) {
    struct BitArray ba;

    if (size % 64 == 0) ba.len = size / 64;
    else ba.len = 1 + size / 64;

    ba.buffer = calloc(ba.len, sizeof(uint64_t));
    
    return ba;
}

/// Returns `true` iff the `i`th bit in the array is a `1`.
/// Does not perform bounds checking.
bool bitarray_get(struct BitArray ba, size_t i) {
    size_t buf_idx = i / 64;
    uint64_t offset = i % 64;
    uint64_t mask = (uint64_t) 1 << offset;

    if ( ba.buffer[buf_idx] & mask )
        return true;
    else
        return false;
}

/// Sets the `i`th bit in the array to `0` if `value` is `false` and `1`
/// otherwise.
/// Does not perform bounds checking.
void bitarray_set(struct BitArray* ba, size_t i, bool value) {
    size_t buf_idx = i / 64;
    uint64_t offset = i % 64;
    uint64_t mask = (uint64_t) 1 << offset;

    ba->buffer[buf_idx] &= ~mask;

    if (value)
        ba->buffer[buf_idx] += mask;
}

void bitarray_free(struct BitArray ba) {
    free(ba.buffer);
}

int main() {
    // We are assuming, pretty justifiably, that `size_t` is at least 32 bits.
    // `list[i] == true` means the number `i` is crossed off (marked as not
    // prime).
    struct BitArray list = bitarray_init( (size_t) 0xFFFFFFFF );

    // 0 and 1 are not prime
    bitarray_set(&list, 0, true);
    bitarray_set(&list, 1, true);

    for (uint32_t p = 2;
         // Comparing against 0xFFFF prevents overflow in `p*p`.
         p <= 0xFFFF && p*p <= 0xFFFFFFFF;
         p++
    ) {
        // Skip over `p` if it has already been marked composite.
        if (bitarray_get(list, p))
            continue;

        // Cross off all the multiples of `p` greater than `p`.
        for (size_t i=2; i <= 0xFFFFFFFF / p; i++) {
            bitarray_set(&list, i*p, true);
        }
    }

    uint32_t* primes = malloc(203739216 * sizeof(uint32_t));
    size_t p_idx = 0;

    for (size_t n=2; n<=0xFFFFFFFF; n++) {
        if ( !bitarray_get(list, n) ) {
            primes[p_idx] = (uint32_t) n;
            p_idx ++;
        }
    }

    FILE* prime_file = fopen("PRIMES", "wb");
    fwrite(primes, 4, p_idx, prime_file);

    fclose(prime_file);
    free(primes);
    bitarray_free(list);
    
    return 0;
}
