#include "GreekCore/Numerics/RNG.h"

namespace GreekCore {

    Xoshiro256::Xoshiro256(uint64_t seed) {
        // SplitMix64 initialization
        for (int i = 0; i < 4; ++i) {
            uint64_t z = (seed += 0x9e3779b97f4a7c15);
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            s[i] = z ^ (z >> 31);
        }
    }

    Xoshiro256::result_type Xoshiro256::operator()() {
        const uint64_t result = std::rotl(s[0] + s[3], 23) + s[0];
        const uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;
        s[3] = std::rotl(s[3], 45);

        return result;
    }
}
