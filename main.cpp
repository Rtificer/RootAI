#include <fmt/core.h>

int main()
{
    fmt::print("Hello, from RootAI!\n");
    return 0;
}


// // Graph
// struct PackedGraph {
//     static constexpr int N = 12;
//     static constexpr int BitsPerEdge = 2;
//     static constexpr int TotalBits = N * N * BitsPerEdge;
//     static constexpr int StorageBytes = (TotalBits + 7) / 8;

//     std::array<uint8_t, StorageBytes> data{};

//     constexpr void set(int from, int to, uint8_t state) noexcept {
//         const uint16_t shift = (from * N + to) * BitsPerEdge;
//         write_bits<uint8_t, StorageBytes, BitsPerEdge>(data, state, shift);
//     }

//     [[nodiscard]] constexpr uint8_t get(int from, int to) const noexcept {
//         const uint16_t shift = (from * N + to) * BitsPerEdge;
//         return read_bits<uint8_t, StorageBytes, BitsPerEdge>(data, shift);
//     }
// };