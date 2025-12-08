/**
 * @file crypt.cpp
 * @brief Реализация алгоритма шифрования SEED
 *
 * Содержит реализацию классов SeedKey и SeedCryptor
 */

#include "crypt.h"
#include <random>
#include <stdexcept>
#include <algorithm>
#include <cstring>

// ============================================================================
// Константы алгоритма SEED
// ============================================================================

/**
 * @brief S-Box таблицы для алгоритма SEED
 *
 * SEED использует две S-Box таблицы (SS0/SS1 и SS2/SS3),
 * которые применяются в функции G для нелинейного преобразования
 */
static const unsigned int SS0[256] = {
    0x2989a1a8, 0x05858184, 0x16c6d2d4, 0x13c3d1d4, 0x14445054, 0x1d0d111c, 0x2c8ca0ac, 0x25052124,
    0x1d4d515c, 0x03434340, 0x18081018, 0x1e0e121c, 0x11415150, 0x3cccf0fc, 0x0acac2c8, 0x23436360,
    0x28082028, 0x04444044, 0x20002020, 0x1d8d919c, 0x20c0e0e0, 0x22c2e2e0, 0x08c8c0c8, 0x17071314,
    0x2585a1a4, 0x0f8f838c, 0x03030300, 0x3b4b7378, 0x3b8bb3b8, 0x13031110, 0x12c2d0d4, 0x2ecee2ec,
    0x30407070, 0x0c8c808c, 0x3f0f333c, 0x2888a0a8, 0x32023230, 0x1dcdd1dc, 0x36c6f2f4, 0x34447074,
    0x2ccce0ec, 0x15859194, 0x0b0b0308, 0x17475354, 0x1c4c505c, 0x1b4b5358, 0x3d8db1bc, 0x01010100,
    0x24042024, 0x1c0c101c, 0x33437370, 0x18889098, 0x10001010, 0x0cccc0cc, 0x32c2f0f4, 0x19c9d1d8,
    0x2c0c202c, 0x27c7e3e4, 0x32427270, 0x03838380, 0x1b8b9398, 0x11c1d1d0, 0x06868284, 0x09c9c1c8,
    0x20406060, 0x10405050, 0x2383a3a0, 0x2bcbe3e8, 0x0d0d010c, 0x3686b2b4, 0x1e8e929c, 0x0f4f434c,
    0x3787b3b4, 0x1a4a5258, 0x06c6c2c4, 0x38487078, 0x2686a2a4, 0x12021210, 0x2f8fa3ac, 0x15c5d1d4,
    0x21416160, 0x03c3c3c0, 0x3484b0b4, 0x01414140, 0x12425250, 0x3d4d717c, 0x0d8d818c, 0x08080008,
    0x1f0f131c, 0x19899198, 0x00000000, 0x19091118, 0x04040004, 0x13435350, 0x37c7f3f4, 0x21c1e1e0,
    0x3dcdf1fc, 0x36467274, 0x2f0f232c, 0x27072324, 0x3080b0b0, 0x0b8b8388, 0x0e0e020c, 0x2b8ba3a8,
    0x2282a2a0, 0x2e4e626c, 0x13839390, 0x0d4d414c, 0x29496168, 0x3c4c707c, 0x09090108, 0x0a0a0208,
    0x3f8fb3bc, 0x2fcfe3ec, 0x33c3f3f0, 0x05c5c1c4, 0x07878384, 0x14041014, 0x3ecef2fc, 0x24446064,
    0x1eced2dc, 0x2e0e222c, 0x0b4b4348, 0x1a0a1218, 0x06060204, 0x21012120, 0x2b4b6368, 0x26466264,
    0x02020200, 0x35c5f1f4, 0x12829290, 0x0a8a8288, 0x0c0c000c, 0x3383b3b0, 0x3e4e727c, 0x10c0d0d0,
    0x3a4a7278, 0x07474344, 0x16869294, 0x25c5e1e4, 0x26062224, 0x00808080, 0x2d8da1ac, 0x1fcfd3dc,
    0x2181a1a0, 0x30003030, 0x37073334, 0x2e8ea2ac, 0x36063234, 0x15051114, 0x22022220, 0x38083038,
    0x34c4f0f4, 0x2787a3a4, 0x05454144, 0x0c4c404c, 0x01818180, 0x29c9e1e8, 0x04848084, 0x17879394,
    0x35053134, 0x0bcbc3c8, 0x0ecec2cc, 0x3c0c303c, 0x31417170, 0x11011110, 0x07c7c3c4, 0x09898188,
    0x35457174, 0x3bcbf3f8, 0x1acad2d8, 0x38c8f0f8, 0x14849094, 0x19495158, 0x02828280, 0x04c4c0c4,
    0x3fcff3fc, 0x09494148, 0x39093138, 0x27476364, 0x00c0c0c0, 0x0fcfc3cc, 0x17c7d3d4, 0x3888b0b8,
    0x0f0f030c, 0x0e8e828c, 0x02424240, 0x23032320, 0x11819190, 0x2c4c606c, 0x1bcbd3d8, 0x2484a0a4,
    0x34043034, 0x31c1f1f0, 0x08484048, 0x02c2c2c0, 0x2f4f636c, 0x3d0d313c, 0x2d0d212c, 0x00404040,
    0x3e8eb2bc, 0x3e0e323c, 0x3c8cb0bc, 0x01c1c1c0, 0x2a8aa2a8, 0x3a8ab2b8, 0x0e4e424c, 0x15455154,
    0x3b0b3338, 0x1cccd0dc, 0x28486068, 0x3f4f737c, 0x1c8c909c, 0x18c8d0d8, 0x0a4a4248, 0x16465254,
    0x37477374, 0x2080a0a0, 0x2dcde1ec, 0x06464244, 0x3585b1b4, 0x2b0b2328, 0x25456164, 0x3acaf2f8,
    0x23c3e3e0, 0x3989b1b8, 0x3181b1b0, 0x1f8f939c, 0x1e4e525c, 0x39c9f1f8, 0x26c6e2e4, 0x3282b2b0,
    0x31013130, 0x2acae2e8, 0x2d4d616c, 0x1f4f535c, 0x24c4e0e4, 0x30c0f0f0, 0x0dcdC1cc, 0x08888088,
    0x16061214, 0x3a0a3238, 0x18481058, 0x14c4d0d4, 0x22426260, 0x29092128, 0x07070304, 0x33033330,
    0x28c8e0e8, 0x1b0b1318, 0x05050104, 0x39497178, 0x10809090, 0x2a4a6268, 0x2a0a2228, 0x1a8a9298
};

static constexpr unsigned int SS1[256] = {
    0x38380830, 0xe828c8e0, 0x2c2d0d21, 0xa42686a2, 0xcc0fcfc3, 0xdc1eced2, 0xb03383b3, 0xb83888b0,
    0xac2f8fa3, 0x60204060, 0x54154551, 0xc407c7c3, 0x44044440, 0x6c2f4f63, 0x682b4b63, 0x581b4b53,
    0xc003c3c3, 0x60224262, 0x30330333, 0xb43585b1, 0x28290921, 0xa02080a0, 0xe022c2e2, 0xa42787a3,
    0xd013c3d3, 0x90118191, 0x10110111, 0x04060602, 0x1c1c0c10, 0xbc3c8cb0, 0x34360632, 0x480b4b43,
    0xec2fcfe3, 0x88088880, 0x6c2c4c60, 0xa82888a0, 0x14170713, 0xc404c4c0, 0x14160612, 0xf434c4f0,
    0xc002c2c2, 0x44054541, 0xe021c1e1, 0xd416c6d2, 0x3c3f0f33, 0x3c3d0d31, 0x8c0e8e82, 0x98188890,
    0x28280820, 0x4c0e4e42, 0xf436c6f2, 0x3c3e0e32, 0xa42585a1, 0xf839c9f1, 0x0c0d0d01, 0xdc1fcfd3,
    0xd818c8d0, 0x282b0b23, 0x64264662, 0x783a4a72, 0x24270723, 0x2c2f0f23, 0xf031c1f1, 0x70324272,
    0x40024242, 0xd414c4d0, 0x40014141, 0xc000c0c0, 0x70334373, 0x64274763, 0xac2c8ca0, 0x880b8b83,
    0xf437c7f3, 0xac2d8da1, 0x80008080, 0x1c1f0f13, 0xc80acac2, 0x2c2c0c20, 0xa82a8aa2, 0x34340430,
    0xd012c2d2, 0x080b0b03, 0xec2ecee2, 0xe829c9e1, 0x5c1d4d51, 0x94148490, 0x18180810, 0xf838c8f0,
    0x54174753, 0xac2e8ea2, 0x08080800, 0xc405c5c1, 0x10130313, 0xcc0dcdc1, 0x84068682, 0xb83989b1,
    0xfc3fcff3, 0x7c3d4d71, 0xc001c1c1, 0x30310131, 0xf435c5f1, 0x880a8a82, 0x682a4a62, 0xb03181b1,
    0xd011c1d1, 0x20200020, 0xd417c7d3, 0x00020202, 0x20220222, 0x04040400, 0x68284860, 0x70314171,
    0x04050501, 0xe021c1e1, 0x902282a2, 0x34350531, 0x381b0b33, 0xfc3ecef2, 0xe424c4e0, 0xb83b8bb3,
    0x6c2e4e62, 0x7c3e4e72, 0x78394971, 0xdc1dcdD1, 0x30320232, 0x902080a0, 0x882a8aa2, 0xc007c7c3,
    0x88098981, 0xc809c9c1, 0x94168692, 0x60214161, 0xbc3e8eb2, 0x702d4d61, 0x94178793, 0xb83d8db1,
    0x100f0f13, 0xd010c0d0, 0xac2f8fa3, 0xec2ecee2, 0x900d8d81, 0x140a0a12, 0x580a4a42, 0x78380870,
    0xfc3bcbf3, 0x58194951, 0x6c2d4d61, 0x481d4d41, 0x90158591, 0x381a0a32, 0xf033c3f3, 0x881d8d81,
    0x180f0f03, 0x28260622, 0xb82989a1, 0xac2a8aa2, 0x842383a3, 0xfc3bcbf3, 0xdc1eced2, 0x28230323,
    0x3c3c0c30, 0x70354571, 0xc005c5c1, 0x80028282, 0x500d4d41, 0xf020c0e0, 0xfc3dcdf1, 0xcc0cccc0,
    0x20230323, 0x500c4c40, 0x10100010, 0xc408c8c0, 0x240b0b23, 0x280a0a22, 0xf836c6f2, 0xcc0cccc0,
    0xe425c5e1, 0x88048480, 0x501d4d51, 0x0c0f0f03, 0xb43686b2, 0x7c3f4f73, 0x442787a3, 0x7c3c4c70,
    0x501c4c50, 0x10150511, 0x180d0d01, 0x24250521, 0x58094941, 0xe425c5e1, 0xdc19c9d1, 0xe023c3e3,
    0x70234363, 0xd414c4d0, 0x301e0e32, 0x941a8a92, 0xc006c6c2, 0x840e8e82, 0x901f8f93, 0xc004c4c0,
    0x800b8b83, 0x50104050, 0x240c0c20, 0x00000000, 0x7c294961, 0x28250521, 0xa02282a2, 0x9414c490,
    0x18140410, 0x40034343, 0x200a0a22, 0x48084840, 0xe022c2e2, 0x0c0c0c00, 0x381e0e32, 0xc809c9c1,
    0x60244464, 0x30310131, 0x94168692, 0x982b8ba3, 0x78394971, 0x60204060, 0xbc3f8fb3, 0x4c0f4f43,
    0x2c280820, 0x7c394971, 0xb83a8ab2, 0x6c294961, 0x04020202, 0xdc1bcbd3, 0x400f4f43, 0x5c1c4c50,
    0xec28c8e0, 0x982a8aa2, 0x6c294961, 0x2c2b0b23, 0x8c0d8d81, 0x80018181, 0xcc0fcfc3, 0x80048480,
    0x94128292, 0xd014c4d0, 0x882b8ba3, 0xd818c8d0, 0xb43787b3, 0x1c170713, 0x0c0e0e02, 0x30360632,
    0xf030c0f0, 0xe021c1e1, 0x1c110111, 0x60274763, 0x98188890, 0x04030303, 0x0c0a0a02, 0xe023c3e3
};

static constexpr unsigned int SS2[256] = {
    0x97929792, 0x57525752, 0xd7d2d7d2, 0xd3d6d3d6, 0x50555055, 0x11101110, 0xa0a5a0a5, 0x21242124,
    0x51545154, 0x434e434e, 0x08150815, 0x12171217, 0x4145414d, 0xf0f5f0f5, 0xc2c5c2c5, 0x63666366,
    0x08250825, 0x44414441, 0x00050005, 0x919c919c, 0xe0e1e0e1, 0xe2e5e2e5, 0xc0c1c0c1, 0x13161316,
    0xa1a4a1a4, 0x838e838e, 0x03060306, 0x7b7e7b7e, 0xb3b6b3b6, 0x13121312, 0xd2d5d2d5, 0xe2e7e2e7,
    0x70717071, 0x808d808d, 0x33363336, 0xa0a1a0a1, 0x32313231, 0xd1d4d1d4, 0xf2f5f2f5, 0x70757075,
    0xe0e5e0e5, 0x91949194, 0x0b0e0b0e, 0x53565356, 0x50515051, 0x5b5e5b5e, 0xb1b4b1b4, 0x01040104,
    0x20252025, 0x10151015, 0x73767376, 0x90919091, 0x00010001, 0xc0c5c0c5, 0xf2f1f2f1, 0xd1d0d1d0,
    0x20212021, 0xe3e6e3e6, 0x72717271, 0x83868386, 0x93969396, 0xd1d4d1d4, 0x82858285, 0xc1c0c1c0,
    0x60656065, 0x50415041, 0xa3a6a3a6, 0xe3e6e3e6, 0x0d080d08, 0xb2b5b2b5, 0x929d929d, 0x434a434a,
    0xb3b4b3b4, 0x525d525d, 0xc2c1c2c1, 0x78797879, 0xa2a5a2a5, 0x12111211, 0xa3aea3ae, 0xd1d4d1d4,
    0x61646164, 0xc3c6c3c6, 0xb0b5b0b5, 0x41444144, 0x52515251, 0x71747174, 0x818c818c, 0x000d000d,
    0x131e131e, 0x919c919c, 0x00010001, 0x11181118, 0x00010001, 0x53565356, 0xf3f6f3f6, 0xe1e4e1e4,
    0xf1f4f1f4, 0x72757275, 0x232e232e, 0x23262326, 0xb0b1b0b1, 0x838e838e, 0x020d020d, 0xa3a6a3a6,
    0xa2a1a2a1, 0x626d626d, 0x93969396, 0x41484148, 0x696c696c, 0x70757075, 0x010c010c, 0x020d020d,
    0xb3beB3be, 0xe3eee3ee, 0xf3f6f3f6, 0xc1c4c1c4, 0x83868386, 0x10151015, 0xf2f5f2f5, 0x60656065,
    0xd2ddd2dd, 0x222d222d, 0x434e434e, 0x121d121d, 0x02050205, 0x21202120, 0x636e636e, 0x62656265,
    0x02010201, 0xf1f4f1f4, 0x92919291, 0x828d828d, 0x000d000d, 0xb3b6b3b6, 0x727d727d, 0xd0d1d0d1,
    0x727d727d, 0x43464346, 0x92959295, 0xe1e4e1e4, 0x22252225, 0x80818081, 0xa1aca1ac, 0xd3ded3de,
    0xa1a0a1a0, 0x30313031, 0x33363336, 0xa2ada2ad, 0x32353235, 0x11141114, 0x22212221, 0x30353035,
    0xf0f5f0f5, 0xa3a6a3a6, 0x41444144, 0x404d404d, 0x81848184, 0xe1e0e1e0, 0x80858085, 0x93969396,
    0x31343134, 0xc3c6c3c6, 0xc2cdc2cd, 0x30353035, 0x71707170, 0x11101110, 0xc3c6c3c6, 0x818c818c,
    0x71747174, 0xf3f6f3f6, 0xd2d1d2d1, 0xf0f1f0f1, 0x90959095, 0x51545154, 0x82818281, 0xc0c5c0c5,
    0xf3feF3fe, 0x41444144, 0x31343134, 0x63666366, 0xc0c1c0c1, 0xc3cec3ce, 0xd3d6d3d6, 0xb0b1b0b1,
    0x030e030e, 0x828d828d, 0x42414241, 0x23262326, 0x91909190, 0x606d606d, 0xd3d6d3d6, 0xa0a5a0a5,
    0x30353035, 0xf1f0f1f0, 0x40454045, 0xc2c1c2c1, 0x636e636e, 0x313c313c, 0x212c212c, 0x40414041,
    0xb2bdb2bd, 0x323d323d, 0xb0bdb0bd, 0xc1c4c1c4, 0xa2a1a2a1, 0xb2b1b2b1, 0x424d424d, 0x51545154,
    0x333e333e, 0xd0d5d0d5, 0x686d686d, 0x737e737e, 0x909d909d, 0xd0d1d0d1, 0x424d424d, 0x52555255,
    0x73767376, 0xa0a1a0a1, 0xe1ece1ec, 0x42454245, 0xb1b4b1b4, 0x232e232e, 0x61646164, 0xf2f1f2f1,
    0xe3e6e3e6, 0xb1b0b1b0, 0xb1b0b1b0, 0x939e939e, 0x525d525d, 0xf1f0f1f0, 0xe2e5e2e5, 0xb2b1b2b1,
    0x31303130, 0xe2e1e2e1, 0x616c616c, 0x535e535e, 0xe0e5e0e5, 0xf0f1f0f1, 0xc1ccc1cc, 0x808d808d,
    0x12151215, 0x323e323e, 0x50555055, 0xd0d5d0d5, 0x62616261, 0x21282128, 0x03060306, 0x33363336,
    0xe0e1e0e1, 0x131e131e, 0x01040104, 0x71747174, 0x90919091, 0x626d626d, 0x222d222d, 0x92919291
};

static constexpr unsigned int SS3[256] = {
    0x9c069c06, 0x5c025c02, 0xdc0edc0e, 0xd60ad60a, 0x54015401, 0x14051405, 0xac0aac0a, 0x24012401,
    0x5c015c01, 0x4c034c03, 0x180a180a, 0x1c0e1c0e, 0x14051405, 0xfc0ffc0f, 0xca08ca08, 0x640b640b,
    0x280a280a, 0x44014401, 0x20002000, 0x9c059c05, 0xe0020e02, 0xe2040e04, 0xc800c800, 0x1707170f,
    0xac01ac01, 0x8f038f03, 0x03000300, 0x7c037c03, 0xbc03bc03, 0x10051005, 0xd204d204, 0xec0bec0b,
    0x70047004, 0x8c028c02, 0x3c0f3c0f, 0xa808a808, 0x30023002, 0xdc05dc05, 0xf60af60a, 0x74047404,
    0xec0aec0a, 0x94059405, 0x0c030c03, 0x54035403, 0x5c045c04, 0x58035803, 0xbc05bc05, 0x04010401,
    0x24022402, 0x1c041c04, 0x74037403, 0x98089808, 0x10001000, 0xcc0ccc0c, 0xf204f204, 0xd809d809,
    0x2c022c02, 0xe70be70b, 0x72047204, 0x80038003, 0x98039803, 0xd104d104, 0x84028402, 0xc908c908,
    0x64026402, 0x54025402, 0xa40ba40b, 0xec0bec0b, 0x0c050c05, 0xb606b606, 0x9c069c06, 0x4c074c07,
    0xb407b407, 0x58025802, 0xc608c608, 0x78067806, 0xa602a602, 0x10021002, 0xac0fac0f, 0xd405d405,
    0x64016401, 0xc400c400, 0xb404b404, 0x44014401, 0x54025402, 0x7c057c05, 0x8c058c05, 0x08000800,
    0x1c071c07, 0x98059805, 0x00000000, 0x18051805, 0x04000400, 0x54035403, 0xf407f407, 0xe404e404,
    0xfc05fc05, 0x74067406, 0x2c0f2c0f, 0x24072407, 0xb000b000, 0x8c038c03, 0x0c060c06, 0xac08ac08,
    0xa400a400, 0x6c066c06, 0x94039403, 0x4c054c05, 0x6c096c09, 0x7c047c04, 0x08050805, 0x08060806,
    0xbc0fbc0f, 0xec0fec0f, 0xf400f400, 0xc404c404, 0x84038403, 0x14041404, 0xfc0efc0e, 0x64046404,
    0xdc0ddc0d, 0x2c062c06, 0x4c074c07, 0x18061806, 0x04020402, 0x24012401, 0x6c096c09, 0x64026402,
    0x02000200, 0xf404f404, 0x90029002, 0x88028802, 0x0c000c00, 0xb400b400, 0x7c067c06, 0xd000d000,
    0x78067806, 0x44034403, 0x94069406, 0xe404e404, 0x24062406, 0x80008000, 0xac05ac05, 0xdc07dc07,
    0xa400a400, 0x30003000, 0x34073407, 0xac06ac06, 0x34063406, 0x14051405, 0x22002200, 0x38003800,
    0xf404f404, 0xa407a407, 0x44054405, 0x4c044c04, 0x84018401, 0xe808e808, 0x84048404, 0x94079407,
    0x34053405, 0xcc03cc03, 0xcc06cc06, 0x3c003c00, 0x74017401, 0x14011401, 0xc400c400, 0x88058805,
    0x74057405, 0xfc03fc03, 0xd808d808, 0xf800f800, 0x94049404, 0x58095809, 0x80028002, 0xc400c400,
    0xfc07fc07, 0x48054805, 0x38053805, 0x64036403, 0xc000c000, 0xcc07cc07, 0xd406d406, 0xb800b800,
    0x0c070c07, 0x8c068c06, 0x44024402, 0x20032003, 0x90059005, 0x6c046c04, 0xd803d803, 0xa400a400,
    0x34043404, 0xf400f400, 0x48044804, 0xc200c200, 0x6c076c07, 0x3c053c05, 0x2c052c05, 0x40004000,
    0xbc06bc06, 0x3c063c06, 0xbc04bc04, 0xc100c100, 0xa808a808, 0xb808b808, 0x4c064c06, 0x54055405,
    0x3c033c03, 0xdc04dc04, 0x68046804, 0x7c077c07, 0x9c049c04, 0xd800d800, 0x48064806, 0x54065406,
    0x74077407, 0xa000a000, 0xec05ec05, 0x44064406, 0xb404b404, 0x2c032c03, 0x64056405, 0xf808f808,
    0xe400e400, 0xb808b808, 0xb000b000, 0x9c079c07, 0x5c065c06, 0xf800f800, 0xe604e604, 0xb200b200,
    0x30013001, 0xe808e808, 0x6c056c05, 0x5c075c07, 0xe400e400, 0xf000f000, 0xcc05cc05, 0x88088808,
    0x14061406, 0x38063806, 0x58055805, 0xd404d404, 0x62046204, 0x28052805, 0x04030403, 0x30033003,
    0xe800e800, 0x1c031c03, 0x04050405, 0x78057805, 0x90009000, 0x68066806, 0x28062806, 0x98029802
};

/**
 * @brief Константы KC для генерации раундовых ключей SEED
 */
static constexpr unsigned int KC[16] = {
    0x9e3779b9, 0x3c6ef373, 0x78dde6e6, 0xf1bbcdcc,
    0xe3779b99, 0xc6ef3733, 0x8dde6e67, 0x1bbcdccf,
    0x3779b99e, 0x6ef3733c, 0xdde6e678, 0xbbcdccf1,
    0x779b99e3, 0xef3733c6, 0xde6e678d, 0xbcdccf1b
};

// ============================================================================
// SeedKey реализация
// ============================================================================

SeedKey::SeedKey() : _keyData{} {}

SeedKey::SeedKey(const std::array<unsigned char, KEY_SIZE>& keyData) : _keyData(keyData) {}

SeedKey::SeedKey(const std::string& filePath) : _keyData{} {
    if (!loadFromFile(filePath)) {
        throw std::runtime_error("Не удалось загрузить ключ из файла: " + filePath);
    }
}

SeedKey SeedKey::generateRandom() {
    SeedKey key;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned short> dist(0, 255);

    for (size_t i = 0; i < KEY_SIZE; ++i) {
        key._keyData[i] = static_cast<unsigned char>(dist(gen));
    }

    return key;
}

bool SeedKey::saveToFile(const std::string& filePath) const {
    std::ofstream file(filePath, std::ios::binary);
    if (!file) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(_keyData.data()), KEY_SIZE);
    return file.good();
}

bool SeedKey::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return false;
    }

    file.read(reinterpret_cast<char*>(_keyData.data()), KEY_SIZE);
    return file.gcount() == KEY_SIZE;
}

const std::array<unsigned char, SeedKey::KEY_SIZE>& SeedKey::getData() const {
    return _keyData;
}

void SeedKey::setData(const std::array<unsigned char, KEY_SIZE>& keyData) {
    _keyData = keyData;
}

bool SeedKey::isValid() const {
    for (unsigned char byte : _keyData) {
        if (byte != 0) {
            return true;
        }
    }
    return false;
}

bool SeedKey::operator==(const SeedKey& other) const {
    return _keyData == other._keyData;
}

bool SeedKey::operator!=(const SeedKey& other) const {
    return !(*this == other);
}

// ============================================================================
// SeedCryptor реализация
// ============================================================================

SeedCryptor::SeedCryptor(const SeedKey& key) : _key(key), _roundKeys{} {
    generateRoundKeys();
}

SeedCryptor::SeedCryptor(const std::string& keyFilePath) : _key(keyFilePath), _roundKeys{} {
    generateRoundKeys();
}

const SeedKey& SeedCryptor::getKey() const {
    return _key;
}

void SeedCryptor::setKey(const SeedKey& key) {
    _key = key;
    generateRoundKeys();
}

unsigned int SeedCryptor::G(unsigned int x) {
    // Функция G использует S-Box преобразования
    unsigned char x0 = (x >> 24) & 0xFF;
    unsigned char x1 = (x >> 16) & 0xFF;
    unsigned char x2 = (x >> 8) & 0xFF;
    unsigned char x3 = x & 0xFF;

    return SS0[x0] ^ SS1[x1] ^ SS2[x2] ^ SS3[x3];
}

unsigned int SeedCryptor::F(unsigned int Ki0, unsigned int Ki1, unsigned int R0, unsigned int R1) {
    // F-функция SEED
    unsigned int T0 = R0 ^ Ki0;
    unsigned int T1 = R1 ^ Ki1;
    T1 ^= T0;
    T1 = G(T1);
    T0 += T1;
    T0 = G(T0);
    T1 += T0;
    T1 = G(T1);
    T0 += T1;
    return T0;
}

void SeedCryptor::generateRoundKeys() {
    const auto& keyData = _key.getData();

    // Извлекаем 4 32-битных слова из ключа (big-endian)
    unsigned int K[4];
    for (int i = 0; i < 4; ++i) {
        K[i] = (static_cast<unsigned int>(keyData[i * 4]) << 24) |
               (static_cast<unsigned int>(keyData[i * 4 + 1]) << 16) |
               (static_cast<unsigned int>(keyData[i * 4 + 2]) << 8) |
               static_cast<unsigned int>(keyData[i * 4 + 3]);
    }

    // Генерация 16 пар раундовых ключей
    for (int i = 0; i < 16; ++i) {
        unsigned int T0 = K[0] + K[2] - KC[i];
        unsigned int T1 = K[1] - K[3] + KC[i];

        _roundKeys[2 * i] = G(T0);
        _roundKeys[2 * i + 1] = G(T1);

        // Ротация ключа
        if (i % 2 == 0) {
            // Циклический сдвиг влево на 8 бит для K[0] и K[1]
            unsigned int temp = K[0];
            K[0] = (K[0] << 8) | (K[1] >> 24);
            K[1] = (K[1] << 8) | (temp >> 24);
        } else {
            // Циклический сдвиг вправо на 8 бит для K[2] и K[3]
            unsigned int temp = K[3];
            K[3] = (K[3] >> 8) | (K[2] << 24);
            K[2] = (K[2] >> 8) | (temp << 24);
        }
    }
}

void SeedCryptor::encryptBlock(std::array<unsigned char, BLOCK_SIZE>& block) {
    // Преобразуем блок в 4 32-битных слова (big-endian)
    unsigned int L0 = (static_cast<unsigned int>(block[0]) << 24) |
                  (static_cast<unsigned int>(block[1]) << 16) |
                  (static_cast<unsigned int>(block[2]) << 8) |
                  static_cast<unsigned int>(block[3]);
    unsigned int L1 = (static_cast<unsigned int>(block[4]) << 24) |
                  (static_cast<unsigned int>(block[5]) << 16) |
                  (static_cast<unsigned int>(block[6]) << 8) |
                  static_cast<unsigned int>(block[7]);
    unsigned int R0 = (static_cast<unsigned int>(block[8]) << 24) |
                  (static_cast<unsigned int>(block[9]) << 16) |
                  (static_cast<unsigned int>(block[10]) << 8) |
                  static_cast<unsigned int>(block[11]);
    unsigned int R1 = (static_cast<unsigned int>(block[12]) << 24) |
                  (static_cast<unsigned int>(block[13]) << 16) |
                  (static_cast<unsigned int>(block[14]) << 8) |
                  static_cast<unsigned int>(block[15]);

    // 16 раундов шифрования
    for (int i = 0; i < 16; ++i) {
        unsigned int T0 = R0;
        unsigned int T1 = R1;

        unsigned int f = F(_roundKeys[2 * i], _roundKeys[2 * i + 1], R0, R1);

        R0 = L0 ^ f;
        R1 = L1 ^ f;
        L0 = T0;
        L1 = T1;
    }

    // Финальная перестановка (L и R меняются местами)
    // Записываем результат обратно в блок (big-endian)
    block[0] = (R0 >> 24) & 0xFF;
    block[1] = (R0 >> 16) & 0xFF;
    block[2] = (R0 >> 8) & 0xFF;
    block[3] = R0 & 0xFF;
    block[4] = (R1 >> 24) & 0xFF;
    block[5] = (R1 >> 16) & 0xFF;
    block[6] = (R1 >> 8) & 0xFF;
    block[7] = R1 & 0xFF;
    block[8] = (L0 >> 24) & 0xFF;
    block[9] = (L0 >> 16) & 0xFF;
    block[10] = (L0 >> 8) & 0xFF;
    block[11] = L0 & 0xFF;
    block[12] = (L1 >> 24) & 0xFF;
    block[13] = (L1 >> 16) & 0xFF;
    block[14] = (L1 >> 8) & 0xFF;
    block[15] = L1 & 0xFF;
}

void SeedCryptor::decryptBlock(std::array<unsigned char, BLOCK_SIZE>& block) {
    // Преобразуем блок в 4 32-битных слова (big-endian)
    unsigned int L0 = (static_cast<unsigned int>(block[0]) << 24) |
                  (static_cast<unsigned int>(block[1]) << 16) |
                  (static_cast<unsigned int>(block[2]) << 8) |
                  static_cast<unsigned int>(block[3]);
    unsigned int L1 = (static_cast<unsigned int>(block[4]) << 24) |
                  (static_cast<unsigned int>(block[5]) << 16) |
                  (static_cast<unsigned int>(block[6]) << 8) |
                  static_cast<unsigned int>(block[7]);
    unsigned int R0 = (static_cast<unsigned int>(block[8]) << 24) |
                  (static_cast<unsigned int>(block[9]) << 16) |
                  (static_cast<unsigned int>(block[10]) << 8) |
                  static_cast<unsigned int>(block[11]);
    unsigned int R1 = (static_cast<unsigned int>(block[12]) << 24) |
                  (static_cast<unsigned int>(block[13]) << 16) |
                  (static_cast<unsigned int>(block[14]) << 8) |
                  static_cast<unsigned int>(block[15]);

    // 16 раундов расшифрования (в обратном порядке ключей)
    for (int i = 15; i >= 0; --i) {
        unsigned int T0 = R0;
        unsigned int T1 = R1;

        unsigned int f = F(_roundKeys[2 * i], _roundKeys[2 * i + 1], R0, R1);

        R0 = L0 ^ f;
        R1 = L1 ^ f;
        L0 = T0;
        L1 = T1;
    }

    // Финальная перестановка
    block[0] = (R0 >> 24) & 0xFF;
    block[1] = (R0 >> 16) & 0xFF;
    block[2] = (R0 >> 8) & 0xFF;
    block[3] = R0 & 0xFF;
    block[4] = (R1 >> 24) & 0xFF;
    block[5] = (R1 >> 16) & 0xFF;
    block[6] = (R1 >> 8) & 0xFF;
    block[7] = R1 & 0xFF;
    block[8] = (L0 >> 24) & 0xFF;
    block[9] = (L0 >> 16) & 0xFF;
    block[10] = (L0 >> 8) & 0xFF;
    block[11] = L0 & 0xFF;
    block[12] = (L1 >> 24) & 0xFF;
    block[13] = (L1 >> 16) & 0xFF;
    block[14] = (L1 >> 8) & 0xFF;
    block[15] = L1 & 0xFF;
}

void SeedCryptor::addPadding(std::vector<unsigned char>& data) {
    // PKCS7 паддинг
    size_t paddingSize = BLOCK_SIZE - (data.size() % BLOCK_SIZE);
    if (paddingSize == 0) {
        paddingSize = BLOCK_SIZE;
    }

    for (size_t i = 0; i < paddingSize; ++i) {
        data.push_back(static_cast<unsigned char>(paddingSize));
    }
}

void SeedCryptor::removePadding(std::vector<unsigned char>& data) {
    if (data.empty()) {
        throw std::runtime_error("Данные пусты, невозможно удалить паддинг");
    }

    unsigned char paddingSize = data.back();

    if (paddingSize == 0 || paddingSize > BLOCK_SIZE || paddingSize > data.size()) {
        throw std::runtime_error("Неверный паддинг PKCS7");
    }

    // Проверяем корректность паддинга
    for (size_t i = data.size() - paddingSize; i < data.size(); ++i) {
        if (data[i] != paddingSize) {
            throw std::runtime_error("Неверный паддинг PKCS7");
        }
    }

    data.resize(data.size() - paddingSize);
}

std::array<unsigned char, SeedCryptor::BLOCK_SIZE> SeedCryptor::generateIV() {
    std::array<unsigned char, BLOCK_SIZE> iv;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned short> dist(0, 255);

    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        iv[i] = static_cast<unsigned char>(dist(gen));
    }

    return iv;
}

std::string SeedCryptor::encrypt(const std::string& plaintext) {
    std::vector<unsigned char> data(plaintext.begin(), plaintext.end());
    return encrypt(data);
}

std::string SeedCryptor::encrypt(const std::vector<unsigned char>& plainData) {
    std::vector<unsigned char> data = plainData;
    addPadding(data);

    // Генерируем IV
    auto iv = generateIV();

    // Результат: IV + шифротекст
    std::vector<unsigned char> result(iv.begin(), iv.end());

    std::array<unsigned char, BLOCK_SIZE> previousBlock = iv;

    // Шифрование в режиме CBC
    for (size_t i = 0; i < data.size(); i += BLOCK_SIZE) {
        std::array<unsigned char, BLOCK_SIZE> block;
        std::copy(data.begin() + i, data.begin() + i + BLOCK_SIZE, block.begin());

        // XOR с предыдущим блоком (или IV для первого блока)
        for (size_t j = 0; j < BLOCK_SIZE; ++j) {
            block[j] ^= previousBlock[j];
        }

        encryptBlock(block);

        result.insert(result.end(), block.begin(), block.end());
        previousBlock = block;
    }

    return std::string(result.begin(), result.end());
}

std::string SeedCryptor::decrypt(const std::vector<unsigned char>& cipherData) {
    auto decrypted = decryptToBytes(cipherData);
    return std::string(decrypted.begin(), decrypted.end());
}

std::string SeedCryptor::decrypt(const std::string& cipherData) {
    std::vector<unsigned char> data(cipherData.begin(), cipherData.end());
    return decrypt(data);
}

std::vector<unsigned char> SeedCryptor::decryptToBytes(const std::string& cipherData) {
    std::vector<unsigned char> data(cipherData.begin(), cipherData.end());
    return decryptToBytes(data);
}

std::vector<unsigned char> SeedCryptor::decryptToBytes(const std::vector<unsigned char>& cipherData) {
    if (cipherData.size() < BLOCK_SIZE * 2) {
        throw std::runtime_error("Данные слишком короткие для расшифрования");
    }

    if ((cipherData.size() - BLOCK_SIZE) % BLOCK_SIZE != 0) {
        throw std::runtime_error("Неверный размер зашифрованных данных");
    }

    // Извлекаем IV
    std::array<unsigned char, BLOCK_SIZE> iv;
    std::copy(cipherData.begin(), cipherData.begin() + BLOCK_SIZE, iv.begin());

    std::vector<unsigned char> result;
    std::array<unsigned char, BLOCK_SIZE> previousBlock = iv;

    // Расшифрование в режиме CBC
    for (size_t i = BLOCK_SIZE; i < cipherData.size(); i += BLOCK_SIZE) {
        std::array<unsigned char, BLOCK_SIZE> block;
        std::copy(cipherData.begin() + i, cipherData.begin() + i + BLOCK_SIZE, block.begin());

        std::array<unsigned char, BLOCK_SIZE> cipherBlock = block;

        decryptBlock(block);

        // XOR с предыдущим блоком шифротекста
        for (size_t j = 0; j < BLOCK_SIZE; ++j) {
            block[j] ^= previousBlock[j];
        }

        result.insert(result.end(), block.begin(), block.end());
        previousBlock = cipherBlock;
    }

    removePadding(result);
    return result;
}

