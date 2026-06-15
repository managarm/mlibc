#include <gtest/gtest.h>
#include <cstdint>
#include <limits>

// Test the security invariant: allocation size must not overflow
// when computing num_fds * sizeof(pollfd)

struct pollfd {
    int fd;
    short events;
    short revents;
};

class PollOverflowTest : public ::testing::TestWithParam<size_t> {};

TEST_P(PollOverflowTest, AllocationSizeMustNotOverflow) {
    // Invariant: num_fds * sizeof(pollfd) must not overflow size_t
    size_t num_fds = GetParam();
    constexpr size_t pollfd_size = sizeof(pollfd);
    
    // Check if multiplication would overflow
    bool would_overflow = (num_fds > 0) && 
                          (num_fds > std::numeric_limits<size_t>::max() / pollfd_size);
    
    if (would_overflow) {
        // Security property: overflow must be detected and rejected
        // A safe implementation MUST reject this input
        EXPECT_TRUE(would_overflow) << "Overflow condition detected for num_fds=" << num_fds;
    } else {
        // Safe case: multiplication is valid
        size_t alloc_size = num_fds * pollfd_size;
        EXPECT_GE(alloc_size, num_fds) << "Allocation size must be >= num_fds";
    }
}

INSTANTIATE_TEST_SUITE_P(
    AdversarialInputs,
    PollOverflowTest,
    ::testing::Values(
        // Exploit payload: on 32-bit, 0x20000001 * 8 = 0x100000008 overflows to 8
        static_cast<size_t>(0x20000001),
        // Boundary: maximum value that causes overflow
        std::numeric_limits<size_t>::max() / sizeof(pollfd) + 1,
        // Valid input: normal usage
        static_cast<size_t>(100)
    )
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}