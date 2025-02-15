// Copyright 2019 JD.com Inc. JD AI

#include <dabnn/bitpack.h>

#include <gtest/gtest.h>

#include <common/baseline.h>
#include <common/common_bitpack.h>
#include <common/helper.h>
#include <dabnn/mat.h>

#ifdef __aarch64__
TEST(bitpack, pack_mat_128) {
    const size_t AHEIGHT = 64;
    const size_t AWIDTH = 64;
    const size_t CHANNEL = 256;
    const size_t ALEN = AHEIGHT * AWIDTH * CHANNEL;
    float a_data[ALEN];
    fill_rand_float(a_data, ALEN);

    const bnn::Mat a(AHEIGHT, AWIDTH, CHANNEL, a_data, bnn::DataType::Float);
    bnn::Mat a_binary(AHEIGHT, AWIDTH, CHANNEL, bnn::DataType::Bit);
    bnn::Mat expected(AHEIGHT, AWIDTH, CHANNEL, bnn::DataType::Bit);
    pack_mat_128_opt(a, a_binary);

    baseline_pack_mat(a, expected);

    FORZS(i, a_binary.total(), 2) {
        // LOG(INFO) << i;
        ASSERT_EQ(bitcount(*(static_cast<uint64_t *>(a_binary) + i)) +
                      bitcount(*(static_cast<uint64_t *>(a_binary) + i + 1)),
                  bitcount(*(static_cast<uint64_t *>(expected) + i)) +
                      bitcount(*(static_cast<uint64_t *>(expected) + i + 1)));
    }
}
#endif  // __aarch64__

TEST(bitpack, pack_mat_64) {
    const size_t AHEIGHT = 64;
    const size_t AWIDTH = 64;
    const size_t CHANNEL = 256;
    const size_t ALEN = AHEIGHT * AWIDTH * CHANNEL;
    float a_data[ALEN];
    fill_rand_float(a_data, ALEN);

    const bnn::Mat a(AHEIGHT, AWIDTH, CHANNEL, a_data, bnn::DataType::Float);
    bnn::Mat a_binary(AHEIGHT, AWIDTH, CHANNEL, bnn::DataType::Bit);
    bnn::Mat expected(AHEIGHT, AWIDTH, CHANNEL, bnn::DataType::Bit);
    pack_mat_64(a, a_binary);

    baseline_pack_mat(a, expected);

    FORZS(i, a_binary.total(), 2) {
        // LOG(INFO) << i;
        ASSERT_EQ(bitcount(*(static_cast<uint64_t *>(a_binary) + i)) +
                      bitcount(*(static_cast<uint64_t *>(a_binary) + i + 1)),
                  bitcount(*(static_cast<uint64_t *>(expected) + i)) +
                      bitcount(*(static_cast<uint64_t *>(expected) + i + 1)));
    }
}

TEST(bitpack, addv_v7) {
    uint64_t data[2];
    fill_rand_uint64(data, 2);
    uint8x16_t v = vld1q_u8(reinterpret_cast<unsigned char *>(data));
    auto v1 = vcntq_u8(v);
    auto v2 = vpaddlq_u8(v1);
    auto v3 = vpaddlq_u16(v2);
    auto v4 = vpaddlq_u32(v3);

    auto res = vgetq_lane_u64(v4, 0) + vgetq_lane_u64(v4, 1);

    ASSERT_EQ(res, __builtin_popcountl(data[0]) + __builtin_popcountl(data[1]));
}
