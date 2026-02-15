#ifndef GREEKCORE_RNG_H
#define GREEKCORE_RNG_H

#include <cstdint>
#include <limits>
#include <array>
#include <bit>

namespace GreekCore {

    /**
     * @brief Xoshiro256++ High Performance RNG.
     * State size: 32 bytes (Fits in AVX registers).
     * Period: 2^256 - 1.
     * Speed: ~2-4x faster than std::mt19937.
     * 
     * @cite Implementation based on:
     *       David Blackman and Sebastiano Vigna. 2021.
     *       "Scrambled Linear Pseudorandom Number Generators".
     *       ACM Trans. Math. Softw. 47, 4, Article 32 (December 2021).
     *       https://prng.di.unimi.it/
     */
    struct Xoshiro256 {
        using result_type = uint64_t;
        std::array<uint64_t, 4> s;

        // Required for std::concepts::uniform_random_bit_generator
        static constexpr result_type min() { return 0; }
        static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

        explicit Xoshiro256(uint64_t seed);
        result_type operator()();

        /**
         * @brief Converts uint64_t to double in [0, 1) range.
         * Uses bit manipulation of IEEE 754 representation.
         * (x >> 11) * 0x1.0p-53
         */
        constexpr static double to_double(uint64_t x) {
            return (x >> 11) * 0x1.0p-53;
        }
    };
}
#endif // GREEKCORE_RNG_H
