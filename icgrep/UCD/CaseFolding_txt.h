#ifndef CASEFOLDING_TXT_H
#define CASEFOLDING_TXT_H
/*
 *  Copyright (c) 2014 International Characters, Inc.
 *  This software is licensed to the public under the Open Software License 3.0.
 *  icgrep is a trademark of International Characters, Inc.
 *
 *  This file is generated by casefold.py - manual edits may be lost.
 */

#include <vector>
#include <utility>
#include "re/re_re.h"
#include "re/re_cc.h"

struct FoldEntry {
    re::codepoint_t range_lo;
    int fold_offset;
    std::vector<re::interval_t> fold_pairs;
};

void caseInsensitiveInsertRange(re::CC * cc, const re::codepoint_t lo, const re::codepoint_t hi);

inline void caseInsensitiveInsert(re::CC * cc, const re::codepoint_t cp) {
    caseInsensitiveInsertRange(cc, cp, cp);
}

const int foldTableSize = 216;

const FoldEntry foldTable[foldTableSize] = {
  {0x0, 0, {}},
  {0x41, 32, {{0x4b, 0x212a}, {0x53, 0x17f}}},
  {0x5b, 0, {}},
  {0x61, -32, {{0x6b, 0x212a}, {0x73, 0x17f}}},
  {0x7b, 0, {{0xb5, 0x3bc}, {0xb5, 0x39c}}},
  {0xc0, 32, {{0xc5, 0x212b}}},
  {0xd7, 0, {}},
  {0xd8, 32, {}},
  {0xdf, 0, {{0xdf, 0x1e9e}}},
  {0xe0, -32, {{0xe5, 0x212b}}},
  {0xf7, 0, {}},
  {0xf8, -32, {}},
  {0xff, 0, {{0xff, 0x178}}},
  {0x100, 1, {}},
  {0x130, 0, {}},
  {0x132, 1, {}},
  {0x138, 0, {}},
  {0x139, 1, {}},
  {0x149, 0, {}},
  {0x14a, 1, {}},
  {0x178, 0, {{0x178, 0xff}}},
  {0x179, 1, {}},
  {0x17f, 0, {{0x17f, 0x73}, {0x17f, 0x53}, {0x180, 0x243}, {0x181, 0x253}}},
  {0x182, 1, {}},
  {0x186, 0, {{0x186, 0x254}}},
  {0x187, 1, {}},
  {0x189, 205, {}},
  {0x18b, 1, {}},
  {0x18d, 0, {{0x18e, 0x1dd}, {0x18f, 0x259}, {0x190, 0x25b}}},
  {0x191, 1, {}},
  {0x193, 0, {{0x193, 0x260}, {0x194, 0x263}, {0x195, 0x1f6}, {0x196, 0x269},
    {0x197, 0x268}}},
  {0x198, 1, {}},
  {0x19a, 0, {{0x19a, 0x23d}, {0x19c, 0x26f}, {0x19d, 0x272}, {0x19e, 0x220},
    {0x19f, 0x275}}},
  {0x1a0, 1, {}},
  {0x1a6, 0, {{0x1a6, 0x280}}},
  {0x1a7, 1, {}},
  {0x1a9, 0, {{0x1a9, 0x283}}},
  {0x1ac, 1, {}},
  {0x1ae, 0, {{0x1ae, 0x288}}},
  {0x1af, 1, {}},
  {0x1b1, 217, {}},
  {0x1b3, 1, {}},
  {0x1b7, 0, {{0x1b7, 0x292}}},
  {0x1b8, 1, {}},
  {0x1ba, 0, {}},
  {0x1bc, 1, {}},
  {0x1be, 0, {{0x1bf, 0x1f7}}},
  {0x1c4, 1, {{0x1c4, 0x1c6}}},
  {0x1c6, 0, {}},
  {0x1c5, 1, {{0x1c6, 0x1c4}, {0x1c7, 0x1c9}}},
  {0x1c9, 0, {}},
  {0x1c8, 1, {{0x1c9, 0x1c7}, {0x1ca, 0x1cc}}},
  {0x1cc, 0, {}},
  {0x1cb, 1, {{0x1cc, 0x1ca}}},
  {0x1dd, 0, {{0x1dd, 0x18e}}},
  {0x1de, 1, {}},
  {0x1f0, 0, {}},
  {0x1f1, 1, {{0x1f1, 0x1f3}}},
  {0x1f3, 0, {}},
  {0x1f2, 1, {{0x1f3, 0x1f1}}},
  {0x1f6, 0, {{0x1f6, 0x195}, {0x1f7, 0x1bf}}},
  {0x1f8, 1, {}},
  {0x220, 0, {{0x220, 0x19e}}},
  {0x222, 1, {}},
  {0x234, 0, {{0x23a, 0x2c65}}},
  {0x23b, 1, {}},
  {0x23d, 0, {{0x23d, 0x19a}, {0x23e, 0x2c66}}},
  {0x23f, 10815, {}},
  {0x241, 1, {}},
  {0x243, 0, {{0x243, 0x180}, {0x244, 0x289}, {0x245, 0x28c}}},
  {0x246, 1, {}},
  {0x250, 0, {{0x250, 0x2c6f}, {0x251, 0x2c6d}, {0x252, 0x2c70}, {0x253, 0x181},
    {0x254, 0x186}}},
  {0x256, -205, {}},
  {0x258, 0, {{0x259, 0x18f}, {0x25b, 0x190}, {0x25c, 0xa7ab}, {0x260, 0x193},
    {0x261, 0xa7ac}, {0x263, 0x194}, {0x265, 0xa78d}, {0x266, 0xa7aa},
    {0x268, 0x197}, {0x269, 0x196}, {0x26b, 0x2c62}, {0x26c, 0xa7ad},
    {0x26f, 0x19c}, {0x271, 0x2c6e}, {0x272, 0x19d}, {0x275, 0x19f},
    {0x27d, 0x2c64}, {0x280, 0x1a6}, {0x283, 0x1a9}, {0x287, 0xa7b1},
    {0x288, 0x1ae}, {0x289, 0x244}}},
  {0x28a, -217, {}},
  {0x28c, 0, {{0x28c, 0x245}, {0x292, 0x1b7}, {0x29e, 0xa7b0}, {0x345, 0x3b9},
    {0x345, 0x399}, {0x345, 0x1fbe}}},
  {0x370, 1, {}},
  {0x374, 0, {}},
  {0x376, 1, {}},
  {0x378, 0, {}},
  {0x37b, 130, {}},
  {0x37e, 0, {{0x37f, 0x3f3}, {0x386, 0x3ac}}},
  {0x388, 37, {}},
  {0x38b, 0, {{0x38c, 0x3cc}}},
  {0x38e, 63, {}},
  {0x390, 0, {}},
  {0x391, 32, {{0x392, 0x3d0}, {0x395, 0x3f5}, {0x398, 0x3d1}, {0x398, 0x3f4},
    {0x399, 0x345}, {0x399, 0x1fbe}, {0x39a, 0x3f0}, {0x39c, 0xb5},
    {0x3a0, 0x3d6}, {0x3a1, 0x3f1}}},
  {0x3a2, 0, {}},
  {0x3a3, 32, {{0x3a3, 0x3c2}, {0x3a6, 0x3d5}, {0x3a9, 0x2126}}},
  {0x3ac, 0, {{0x3ac, 0x386}}},
  {0x3ad, -37, {}},
  {0x3b0, 0, {}},
  {0x3b1, -32, {{0x3b2, 0x3d0}, {0x3b5, 0x3f5}, {0x3b8, 0x3d1}, {0x3b8, 0x3f4},
    {0x3b9, 0x345}, {0x3b9, 0x1fbe}, {0x3ba, 0x3f0}, {0x3bc, 0xb5},
    {0x3c0, 0x3d6}, {0x3c1, 0x3f1}}},
  {0x3c2, 1, {{0x3c2, 0x3a3}}},
  {0x3c4, 0, {}},
  {0x3c3, -32, {{0x3c6, 0x3d5}, {0x3c9, 0x2126}}},
  {0x3cc, 0, {{0x3cc, 0x38c}}},
  {0x3cd, -63, {}},
  {0x3cf, 0, {{0x3cf, 0x3d7}, {0x3d0, 0x3b2}, {0x3d0, 0x392}, {0x3d1, 0x3b8},
    {0x3d1, 0x398}, {0x3d1, 0x3f4}, {0x3d5, 0x3c6}, {0x3d5, 0x3a6},
    {0x3d6, 0x3c0}, {0x3d6, 0x3a0}, {0x3d7, 0x3cf}}},
  {0x3d8, 1, {}},
  {0x3f0, 0, {{0x3f0, 0x3ba}, {0x3f0, 0x39a}, {0x3f1, 0x3c1}, {0x3f1, 0x3a1},
    {0x3f2, 0x3f9}, {0x3f3, 0x37f}, {0x3f4, 0x3b8}, {0x3f4, 0x398},
    {0x3f4, 0x3d1}, {0x3f5, 0x3b5}, {0x3f5, 0x395}}},
  {0x3f7, 1, {}},
  {0x3f9, 0, {{0x3f9, 0x3f2}}},
  {0x3fa, 1, {}},
  {0x3fc, 0, {}},
  {0x3fd, -130, {}},
  {0x400, 80, {}},
  {0x410, 32, {}},
  {0x450, -80, {}},
  {0x460, 1, {}},
  {0x482, 0, {}},
  {0x48a, 1, {}},
  {0x4c0, 0, {{0x4c0, 0x4cf}}},
  {0x4c1, 1, {}},
  {0x4cf, 0, {{0x4cf, 0x4c0}}},
  {0x4d0, 1, {}},
  {0x530, 0, {}},
  {0x531, 48, {}},
  {0x557, 0, {}},
  {0x561, -48, {}},
  {0x587, 0, {}},
  {0x10a0, 7264, {}},
  {0x10c6, 0, {{0x10c7, 0x2d27}, {0x10cd, 0x2d2d}, {0x1d79, 0xa77d}, {0x1d7d, 0x2c63}}},
  {0x1e00, 1, {{0x1e60, 0x1e9b}, {0x1e61, 0x1e9b}}},
  {0x1e96, 0, {{0x1e9b, 0x1e61}, {0x1e9b, 0x1e60}, {0x1e9e, 0xdf}}},
  {0x1ea0, 1, {}},
  {0x1f00, 8, {}},
  {0x1f16, 0, {}},
  {0x1f18, -8, {}},
  {0x1f1e, 0, {}},
  {0x1f20, 8, {}},
  {0x1f46, 0, {}},
  {0x1f48, -8, {}},
  {0x1f4e, 0, {{0x1f51, 0x1f59}, {0x1f53, 0x1f5b}, {0x1f55, 0x1f5d}, {0x1f57, 0x1f5f},
    {0x1f59, 0x1f51}, {0x1f5b, 0x1f53}, {0x1f5d, 0x1f55}, {0x1f5f, 0x1f57}}},
  {0x1f60, 8, {}},
  {0x1f70, 74, {}},
  {0x1f72, 86, {}},
  {0x1f76, 100, {}},
  {0x1f78, 128, {}},
  {0x1f7a, 112, {}},
  {0x1f7c, 126, {}},
  {0x1f7e, 0, {}},
  {0x1f80, 8, {}},
  {0x1fb2, 0, {{0x1fb3, 0x1fbc}}},
  {0x1fb8, -8, {}},
  {0x1fba, -74, {}},
  {0x1fbc, 0, {{0x1fbc, 0x1fb3}, {0x1fbe, 0x3b9}, {0x1fbe, 0x345}, {0x1fbe, 0x399},
    {0x1fc3, 0x1fcc}}},
  {0x1fc8, -86, {}},
  {0x1fcc, 0, {{0x1fcc, 0x1fc3}}},
  {0x1fd0, 8, {}},
  {0x1fd2, 0, {}},
  {0x1fd8, -8, {}},
  {0x1fda, -100, {}},
  {0x1fdc, 0, {}},
  {0x1fe0, 8, {}},
  {0x1fe2, 0, {{0x1fe5, 0x1fec}}},
  {0x1fe8, -8, {}},
  {0x1fea, -112, {}},
  {0x1fec, 0, {{0x1fec, 0x1fe5}, {0x1ff3, 0x1ffc}}},
  {0x1ff8, -128, {}},
  {0x1ffa, -126, {}},
  {0x1ffc, 0, {{0x1ffc, 0x1ff3}, {0x2126, 0x3c9}, {0x2126, 0x3a9}, {0x212a, 0x6b},
    {0x212a, 0x4b}, {0x212b, 0xe5}, {0x212b, 0xc5}, {0x2132, 0x214e},
    {0x214e, 0x2132}}},
  {0x2160, 16, {}},
  {0x2180, 0, {}},
  {0x2183, 1, {}},
  {0x2185, 0, {}},
  {0x24b6, 26, {}},
  {0x24ea, 0, {}},
  {0x2c00, 48, {}},
  {0x2c2f, 0, {}},
  {0x2c30, -48, {}},
  {0x2c5f, 0, {}},
  {0x2c60, 1, {}},
  {0x2c62, 0, {{0x2c62, 0x26b}, {0x2c63, 0x1d7d}, {0x2c64, 0x27d}, {0x2c65, 0x23a},
    {0x2c66, 0x23e}}},
  {0x2c67, 1, {}},
  {0x2c6d, 0, {{0x2c6d, 0x251}, {0x2c6e, 0x271}, {0x2c6f, 0x250}, {0x2c70, 0x252}}},
  {0x2c72, 1, {}},
  {0x2c74, 0, {}},
  {0x2c75, 1, {}},
  {0x2c77, 0, {}},
  {0x2c7e, -10815, {}},
  {0x2c80, 1, {}},
  {0x2ce4, 0, {}},
  {0x2ceb, 1, {}},
  {0x2cef, 0, {}},
  {0x2cf2, 1, {}},
  {0x2cf4, 0, {}},
  {0x2d00, -7264, {}},
  {0x2d26, 0, {{0x2d27, 0x10c7}, {0x2d2d, 0x10cd}}},
  {0xa640, 1, {}},
  {0xa66e, 0, {}},
  {0xa680, 1, {}},
  {0xa69c, 0, {}},
  {0xa722, 1, {}},
  {0xa730, 0, {}},
  {0xa732, 1, {}},
  {0xa770, 0, {}},
  {0xa779, 1, {}},
  {0xa77d, 0, {{0xa77d, 0x1d79}}},
  {0xa77e, 1, {}},
  {0xa788, 0, {}},
  {0xa78b, 1, {}},
  {0xa78d, 0, {{0xa78d, 0x265}}},
  {0xa790, 1, {}},
  {0xa794, 0, {}},
  {0xa796, 1, {}},
  {0xa7aa, 0, {{0xa7aa, 0x266}, {0xa7ab, 0x25c}, {0xa7ac, 0x261}, {0xa7ad, 0x26c},
    {0xa7b0, 0x29e}, {0xa7b1, 0x287}}},
  {0xff21, 32, {}},
  {0xff3b, 0, {}},
  {0xff41, -32, {}},
  {0xff5b, 0, {}},
  {0x10400, 40, {}},
  {0x10450, 0, {}},
  {0x118a0, 32, {}},
  {0x118e0, 0, {}},
  {0x110000, 0, {}}};
#endif
