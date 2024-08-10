#include <gtest/gtest.h>
#include "paeth.hpp"

TEST(TestPaeth, paeth_simple) {
    EXPECT_EQ(10, paeth_simple(10,20,30));
    EXPECT_EQ(10, paeth_simple(20,10,30));
    EXPECT_EQ(15, paeth_simple(20,10,15));
}

#ifdef __SSE4_1__
TEST(TestPaeth, paeth_before) {
    for (int i=0; i<256; i++) {
        for (int j=0; j<256; j++) {
            for (int k=0; k<256; k+=8) {
                uint8_t a[8], b[8], c[8], ans[8];
                for (int l=0; l<8; l++) {
                    a[l] = i;
                    b[l] = j;
                    c[l] = k+l;
                }
                paeth_before(a, b, c, ans);
                for (int l=0; l<8; l++) {
                    EXPECT_EQ(paeth_simple(a[l],b[l],c[l]), ans[l]);
                }
            }
        }
    }
}
TEST(TestPaeth, paeth_after) {
    for (int i=0; i<256; i++) {
        for (int j=0; j<256; j++) {
            for (int k=0; k<256; k+=8) {
                uint8_t a[8], b[8], c[8], ans[8];
                for (int l=0; l<8; l++) {
                    a[l] = i;
                    b[l] = j;
                    c[l] = k+l;
                }
                paeth_after(a, b, c, ans);
                for (int l=0; l<8; l++) {
                    EXPECT_EQ(paeth_simple(a[l],b[l],c[l]), ans[l]);
                }
            }
        }
    }
}
#endif // __SSE4_1__
