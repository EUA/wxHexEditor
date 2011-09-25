/*
 *  gosthash.c 
 *  21 Apr 1998  Markku-Juhani Saarinen <mjos@ssh.fi>
 * 
 *  GOST R 34.11-94, Russian Standard Hash Function
 *
 *  Copyright (c) 1998 SSH Communications Security, Finland
 *  All rights reserved.
 */

/* This implementation is placed in the public domain, according to
 * the author. --nikos
 */

#include "libdefs.h"

#ifdef ENABLE_GOST

#include "mhash_gost.h"
/*
   lookup tables : each of these has two rotated 4-bit S-Boxes 
 */

static __const mutils_word32 gost_sbox_1[256] = {

	0x72000UL, 0x75000UL, 0x74800UL, 0x71000UL, 0x76800UL,
	0x74000UL, 0x70000UL, 0x77000UL, 0x73000UL, 0x75800UL,
	0x70800UL, 0x76000UL, 0x73800UL, 0x77800UL, 0x72800UL,
	0x71800UL, 0x5A000UL, 0x5D000UL, 0x5C800UL, 0x59000UL,
	0x5E800UL, 0x5C000UL, 0x58000UL, 0x5F000UL, 0x5B000UL,
	0x5D800UL, 0x58800UL, 0x5E000UL, 0x5B800UL, 0x5F800UL,
	0x5A800UL, 0x59800UL, 0x22000UL, 0x25000UL, 0x24800UL,
	0x21000UL, 0x26800UL, 0x24000UL, 0x20000UL, 0x27000UL,
	0x23000UL, 0x25800UL, 0x20800UL, 0x26000UL, 0x23800UL,
	0x27800UL, 0x22800UL, 0x21800UL, 0x62000UL, 0x65000UL,
	0x64800UL, 0x61000UL, 0x66800UL, 0x64000UL, 0x60000UL,
	0x67000UL, 0x63000UL, 0x65800UL, 0x60800UL, 0x66000UL,
	0x63800UL, 0x67800UL, 0x62800UL, 0x61800UL, 0x32000UL,
	0x35000UL, 0x34800UL, 0x31000UL, 0x36800UL, 0x34000UL,
	0x30000UL, 0x37000UL, 0x33000UL, 0x35800UL, 0x30800UL,
	0x36000UL, 0x33800UL, 0x37800UL, 0x32800UL, 0x31800UL,
	0x6A000UL, 0x6D000UL, 0x6C800UL, 0x69000UL, 0x6E800UL,
	0x6C000UL, 0x68000UL, 0x6F000UL, 0x6B000UL, 0x6D800UL,
	0x68800UL, 0x6E000UL, 0x6B800UL, 0x6F800UL, 0x6A800UL,
	0x69800UL, 0x7A000UL, 0x7D000UL, 0x7C800UL, 0x79000UL,
	0x7E800UL, 0x7C000UL, 0x78000UL, 0x7F000UL, 0x7B000UL,
	0x7D800UL, 0x78800UL, 0x7E000UL, 0x7B800UL, 0x7F800UL,
	0x7A800UL, 0x79800UL, 0x52000UL, 0x55000UL, 0x54800UL,
	0x51000UL, 0x56800UL, 0x54000UL, 0x50000UL, 0x57000UL,
	0x53000UL, 0x55800UL, 0x50800UL, 0x56000UL, 0x53800UL,
	0x57800UL, 0x52800UL, 0x51800UL, 0x12000UL, 0x15000UL,
	0x14800UL, 0x11000UL, 0x16800UL, 0x14000UL, 0x10000UL,
	0x17000UL, 0x13000UL, 0x15800UL, 0x10800UL, 0x16000UL,
	0x13800UL, 0x17800UL, 0x12800UL, 0x11800UL, 0x1A000UL,
	0x1D000UL, 0x1C800UL, 0x19000UL, 0x1E800UL, 0x1C000UL,
	0x18000UL, 0x1F000UL, 0x1B000UL, 0x1D800UL, 0x18800UL,
	0x1E000UL, 0x1B800UL, 0x1F800UL, 0x1A800UL, 0x19800UL,
	0x42000UL, 0x45000UL, 0x44800UL, 0x41000UL, 0x46800UL,
	0x44000UL, 0x40000UL, 0x47000UL, 0x43000UL, 0x45800UL,
	0x40800UL, 0x46000UL, 0x43800UL, 0x47800UL, 0x42800UL,
	0x41800UL, 0xA000UL,  0xD000UL,  0xC800UL,  0x9000UL,
	0xE800UL,  0xC000UL,  0x8000UL,  0xF000UL,  0xB000UL,
	0xD800UL,  0x8800UL,  0xE000UL,  0xB800UL,  0xF800UL,
	0xA800UL,  0x9800UL,  0x2000UL,  0x5000UL,  0x4800UL,
	0x1000UL,  0x6800UL,  0x4000UL,  0x0UL,     0x7000UL,
	0x3000UL,  0x5800UL,  0x800UL,   0x6000UL,  0x3800UL,
	0x7800UL,  0x2800UL,  0x1800UL,  0x3A000UL, 0x3D000UL,
	0x3C800UL, 0x39000UL, 0x3E800UL, 0x3C000UL, 0x38000UL,
	0x3F000UL, 0x3B000UL, 0x3D800UL, 0x38800UL, 0x3E000UL,
	0x3B800UL, 0x3F800UL, 0x3A800UL, 0x39800UL, 0x2A000UL,
	0x2D000UL, 0x2C800UL, 0x29000UL, 0x2E800UL, 0x2C000UL,
	0x28000UL, 0x2F000UL, 0x2B000UL, 0x2D800UL, 0x28800UL,
	0x2E000UL, 0x2B800UL, 0x2F800UL, 0x2A800UL, 0x29800UL,
	0x4A000UL, 0x4D000UL, 0x4C800UL, 0x49000UL, 0x4E800UL,
	0x4C000UL, 0x48000UL, 0x4F000UL, 0x4B000UL, 0x4D800UL,
	0x48800UL, 0x4E000UL, 0x4B800UL, 0x4F800UL, 0x4A800UL,
	0x49800UL,
};
static __const mutils_word32 gost_sbox_2[256] = {

	0x3A80000UL, 0x3C00000UL, 0x3880000UL, 0x3E80000UL, 0x3D00000UL,
	0x3980000UL, 0x3A00000UL, 0x3900000UL, 0x3F00000UL, 0x3F80000UL,
	0x3E00000UL, 0x3B80000UL, 0x3B00000UL, 0x3800000UL, 0x3C80000UL,
	0x3D80000UL, 0x6A80000UL, 0x6C00000UL, 0x6880000UL, 0x6E80000UL,
	0x6D00000UL, 0x6980000UL, 0x6A00000UL, 0x6900000UL, 0x6F00000UL,
	0x6F80000UL, 0x6E00000UL, 0x6B80000UL, 0x6B00000UL, 0x6800000UL,
	0x6C80000UL, 0x6D80000UL, 0x5280000UL, 0x5400000UL, 0x5080000UL,
	0x5680000UL, 0x5500000UL, 0x5180000UL, 0x5200000UL, 0x5100000UL,
	0x5700000UL, 0x5780000UL, 0x5600000UL, 0x5380000UL, 0x5300000UL,
	0x5000000UL, 0x5480000UL, 0x5580000UL, 0xA80000UL,  0xC00000UL,
	0x880000UL,  0xE80000UL,  0xD00000UL,  0x980000UL,  0xA00000UL,
	0x900000UL,  0xF00000UL,  0xF80000UL,  0xE00000UL,  0xB80000UL,
	0xB00000UL,  0x800000UL,  0xC80000UL,  0xD80000UL,  0x280000UL,
	0x400000UL,  0x80000UL,   0x680000UL,  0x500000UL,  0x180000UL,
	0x200000UL,  0x100000UL,  0x700000UL,  0x780000UL,  0x600000UL,
	0x380000UL,  0x300000UL,  0x0UL,       0x480000UL,  0x580000UL,
	0x4280000UL, 0x4400000UL, 0x4080000UL, 0x4680000UL, 0x4500000UL,
	0x4180000UL, 0x4200000UL, 0x4100000UL, 0x4700000UL, 0x4780000UL,
	0x4600000UL, 0x4380000UL, 0x4300000UL, 0x4000000UL, 0x4480000UL,
	0x4580000UL, 0x4A80000UL, 0x4C00000UL, 0x4880000UL, 0x4E80000UL,
	0x4D00000UL, 0x4980000UL, 0x4A00000UL, 0x4900000UL, 0x4F00000UL,
	0x4F80000UL, 0x4E00000UL, 0x4B80000UL, 0x4B00000UL, 0x4800000UL,
	0x4C80000UL, 0x4D80000UL, 0x7A80000UL, 0x7C00000UL, 0x7880000UL,
	0x7E80000UL, 0x7D00000UL, 0x7980000UL, 0x7A00000UL, 0x7900000UL,
	0x7F00000UL, 0x7F80000UL, 0x7E00000UL, 0x7B80000UL, 0x7B00000UL,
	0x7800000UL, 0x7C80000UL, 0x7D80000UL, 0x7280000UL, 0x7400000UL,
	0x7080000UL, 0x7680000UL, 0x7500000UL, 0x7180000UL, 0x7200000UL,
	0x7100000UL, 0x7700000UL, 0x7780000UL, 0x7600000UL, 0x7380000UL,
	0x7300000UL, 0x7000000UL, 0x7480000UL, 0x7580000UL, 0x2280000UL,
	0x2400000UL, 0x2080000UL, 0x2680000UL, 0x2500000UL, 0x2180000UL,
	0x2200000UL, 0x2100000UL, 0x2700000UL, 0x2780000UL, 0x2600000UL,
	0x2380000UL, 0x2300000UL, 0x2000000UL, 0x2480000UL, 0x2580000UL,
	0x3280000UL, 0x3400000UL, 0x3080000UL, 0x3680000UL, 0x3500000UL,
	0x3180000UL, 0x3200000UL, 0x3100000UL, 0x3700000UL, 0x3780000UL,
	0x3600000UL, 0x3380000UL, 0x3300000UL, 0x3000000UL, 0x3480000UL,
	0x3580000UL, 0x6280000UL, 0x6400000UL, 0x6080000UL, 0x6680000UL,
	0x6500000UL, 0x6180000UL, 0x6200000UL, 0x6100000UL, 0x6700000UL,
	0x6780000UL, 0x6600000UL, 0x6380000UL, 0x6300000UL, 0x6000000UL,
	0x6480000UL, 0x6580000UL, 0x5A80000UL, 0x5C00000UL, 0x5880000UL,
	0x5E80000UL, 0x5D00000UL, 0x5980000UL, 0x5A00000UL, 0x5900000UL,
	0x5F00000UL, 0x5F80000UL, 0x5E00000UL, 0x5B80000UL, 0x5B00000UL,
	0x5800000UL, 0x5C80000UL, 0x5D80000UL, 0x1280000UL, 0x1400000UL,
	0x1080000UL, 0x1680000UL, 0x1500000UL, 0x1180000UL, 0x1200000UL,
	0x1100000UL, 0x1700000UL, 0x1780000UL, 0x1600000UL, 0x1380000UL,
	0x1300000UL, 0x1000000UL, 0x1480000UL, 0x1580000UL, 0x2A80000UL,
	0x2C00000UL, 0x2880000UL, 0x2E80000UL, 0x2D00000UL, 0x2980000UL,
	0x2A00000UL, 0x2900000UL, 0x2F00000UL, 0x2F80000UL, 0x2E00000UL,
	0x2B80000UL, 0x2B00000UL, 0x2800000UL, 0x2C80000UL, 0x2D80000UL,
	0x1A80000UL, 0x1C00000UL, 0x1880000UL, 0x1E80000UL, 0x1D00000UL,
	0x1980000UL, 0x1A00000UL, 0x1900000UL, 0x1F00000UL, 0x1F80000UL,
	0x1E00000UL, 0x1B80000UL, 0x1B00000UL, 0x1800000UL, 0x1C80000UL,
	0x1D80000UL,
};
static __const mutils_word32 gost_sbox_3[256] = {

	0x30000002UL, 0x60000002UL, 0x38000002UL, 0x8000002UL,
	0x28000002UL, 0x78000002UL, 0x68000002UL, 0x40000002UL, 
	0x20000002UL, 0x50000002UL, 0x48000002UL, 0x70000002UL, 
	0x2UL,        0x18000002UL, 0x58000002UL, 0x10000002UL, 
	0xB0000005UL, 0xE0000005UL, 0xB8000005UL, 0x88000005UL,
	0xA8000005UL, 0xF8000005UL, 0xE8000005UL, 0xC0000005UL,
	0xA0000005UL, 0xD0000005UL, 0xC8000005UL, 0xF0000005UL, 
	0x80000005UL, 0x98000005UL, 0xD8000005UL, 0x90000005UL, 
	0x30000005UL, 0x60000005UL, 0x38000005UL, 0x8000005UL, 
	0x28000005UL, 0x78000005UL, 0x68000005UL, 0x40000005UL,
	0x20000005UL, 0x50000005UL, 0x48000005UL, 0x70000005UL, 
	0x5UL,        0x18000005UL, 0x58000005UL, 0x10000005UL, 
	0x30000000UL, 0x60000000UL, 0x38000000UL, 0x8000000UL, 
	0x28000000UL, 0x78000000UL, 0x68000000UL, 0x40000000UL, 
	0x20000000UL, 0x50000000UL, 0x48000000UL, 0x70000000UL,
	0x0UL,        0x18000000UL, 0x58000000UL, 0x10000000UL, 
	0xB0000003UL, 0xE0000003UL, 0xB8000003UL, 0x88000003UL, 
	0xA8000003UL, 0xF8000003UL, 0xE8000003UL, 0xC0000003UL, 
	0xA0000003UL, 0xD0000003UL, 0xC8000003UL, 0xF0000003UL, 
	0x80000003UL, 0x98000003UL, 0xD8000003UL, 0x90000003UL,
	0x30000001UL, 0x60000001UL, 0x38000001UL, 0x8000001UL,
	0x28000001UL, 0x78000001UL, 0x68000001UL, 0x40000001UL, 
	0x20000001UL, 0x50000001UL, 0x48000001UL, 0x70000001UL, 
	0x1UL,        0x18000001UL, 0x58000001UL, 0x10000001UL, 
	0xB0000000UL, 0xE0000000UL, 0xB8000000UL, 0x88000000UL,
	0xA8000000UL, 0xF8000000UL, 0xE8000000UL, 0xC0000000UL,
	0xA0000000UL, 0xD0000000UL, 0xC8000000UL, 0xF0000000UL, 
	0x80000000UL, 0x98000000UL, 0xD8000000UL, 0x90000000UL, 
	0xB0000006UL, 0xE0000006UL, 0xB8000006UL, 0x88000006UL, 
	0xA8000006UL, 0xF8000006UL, 0xE8000006UL, 0xC0000006UL,
	0xA0000006UL, 0xD0000006UL, 0xC8000006UL, 0xF0000006UL,
	0x80000006UL, 0x98000006UL, 0xD8000006UL, 0x90000006UL, 
	0xB0000001UL, 0xE0000001UL, 0xB8000001UL, 0x88000001UL, 
	0xA8000001UL, 0xF8000001UL, 0xE8000001UL, 0xC0000001UL, 
	0xA0000001UL, 0xD0000001UL, 0xC8000001UL, 0xF0000001UL,
	0x80000001UL, 0x98000001UL, 0xD8000001UL, 0x90000001UL,
	0x30000003UL, 0x60000003UL, 0x38000003UL, 0x8000003UL, 
	0x28000003UL, 0x78000003UL, 0x68000003UL, 0x40000003UL, 
	0x20000003UL, 0x50000003UL, 0x48000003UL, 0x70000003UL, 
	0x3UL,        0x18000003UL, 0x58000003UL, 0x10000003UL,
	0x30000004UL, 0x60000004UL, 0x38000004UL, 0x8000004UL,
	0x28000004UL, 0x78000004UL, 0x68000004UL, 0x40000004UL, 
	0x20000004UL, 0x50000004UL, 0x48000004UL, 0x70000004UL, 
	0x4UL,        0x18000004UL, 0x58000004UL, 0x10000004UL, 
	0xB0000002UL, 0xE0000002UL, 0xB8000002UL, 0x88000002UL,
	0xA8000002UL, 0xF8000002UL, 0xE8000002UL, 0xC0000002UL,
	0xA0000002UL, 0xD0000002UL, 0xC8000002UL, 0xF0000002UL, 
	0x80000002UL, 0x98000002UL, 0xD8000002UL, 0x90000002UL, 
	0xB0000004UL, 0xE0000004UL, 0xB8000004UL, 0x88000004UL, 
	0xA8000004UL, 0xF8000004UL, 0xE8000004UL, 0xC0000004UL,
	0xA0000004UL, 0xD0000004UL, 0xC8000004UL, 0xF0000004UL,
	0x80000004UL, 0x98000004UL, 0xD8000004UL, 0x90000004UL, 
	0x30000006UL, 0x60000006UL, 0x38000006UL, 0x8000006UL, 
	0x28000006UL, 0x78000006UL, 0x68000006UL, 0x40000006UL, 
	0x20000006UL, 0x50000006UL, 0x48000006UL, 0x70000006UL,
	0x6UL,        0x18000006UL, 0x58000006UL, 0x10000006UL, 
	0xB0000007UL, 0xE0000007UL, 0xB8000007UL, 0x88000007UL, 
	0xA8000007UL, 0xF8000007UL, 0xE8000007UL, 0xC0000007UL, 
	0xA0000007UL, 0xD0000007UL, 0xC8000007UL, 0xF0000007UL, 
	0x80000007UL, 0x98000007UL, 0xD8000007UL, 0x90000007UL,
	0x30000007UL, 0x60000007UL, 0x38000007UL, 0x8000007UL,
	0x28000007UL, 0x78000007UL, 0x68000007UL, 0x40000007UL, 
	0x20000007UL, 0x50000007UL, 0x48000007UL, 0x70000007UL, 
	0x7UL,        0x18000007UL, 0x58000007UL, 0x10000007UL,
};
static __const mutils_word32 gost_sbox_4[256] = {

	0xE8UL,  0xD8UL,  0xA0UL,  0x88UL,  0x98UL,
	0xF8UL,  0xA8UL,  0xC8UL,  0x80UL,  0xD0UL,
	0xF0UL,  0xB8UL,  0xB0UL,  0xC0UL,  0x90UL,
	0xE0UL,  0x7E8UL, 0x7D8UL, 0x7A0UL, 0x788UL,
	0x798UL, 0x7F8UL, 0x7A8UL, 0x7C8UL, 0x780UL,
	0x7D0UL, 0x7F0UL, 0x7B8UL, 0x7B0UL, 0x7C0UL,
	0x790UL, 0x7E0UL, 0x6E8UL, 0x6D8UL, 0x6A0UL,
	0x688UL, 0x698UL, 0x6F8UL, 0x6A8UL, 0x6C8UL,
	0x680UL, 0x6D0UL, 0x6F0UL, 0x6B8UL, 0x6B0UL,
	0x6C0UL, 0x690UL, 0x6E0UL, 0x68UL,  0x58UL,
	0x20UL,  0x8UL,   0x18UL,  0x78UL,  0x28UL,
	0x48UL,  0x0UL,   0x50UL,  0x70UL,  0x38UL,
	0x30UL,  0x40UL,  0x10UL,  0x60UL,  0x2E8UL,
	0x2D8UL, 0x2A0UL, 0x288UL, 0x298UL, 0x2F8UL,
	0x2A8UL, 0x2C8UL, 0x280UL, 0x2D0UL, 0x2F0UL,
	0x2B8UL, 0x2B0UL, 0x2C0UL, 0x290UL, 0x2E0UL,
	0x3E8UL, 0x3D8UL, 0x3A0UL, 0x388UL, 0x398UL,
	0x3F8UL, 0x3A8UL, 0x3C8UL, 0x380UL, 0x3D0UL,
	0x3F0UL, 0x3B8UL, 0x3B0UL, 0x3C0UL, 0x390UL,
	0x3E0UL, 0x568UL, 0x558UL, 0x520UL, 0x508UL,
	0x518UL, 0x578UL, 0x528UL, 0x548UL, 0x500UL,
	0x550UL, 0x570UL, 0x538UL, 0x530UL, 0x540UL,
	0x510UL, 0x560UL, 0x268UL, 0x258UL, 0x220UL,
	0x208UL, 0x218UL, 0x278UL, 0x228UL, 0x248UL,
	0x200UL, 0x250UL, 0x270UL, 0x238UL, 0x230UL,
	0x240UL, 0x210UL, 0x260UL, 0x4E8UL, 0x4D8UL,
	0x4A0UL, 0x488UL, 0x498UL, 0x4F8UL, 0x4A8UL,
	0x4C8UL, 0x480UL, 0x4D0UL, 0x4F0UL, 0x4B8UL,
	0x4B0UL, 0x4C0UL, 0x490UL, 0x4E0UL, 0x168UL,
	0x158UL, 0x120UL, 0x108UL, 0x118UL, 0x178UL,
	0x128UL, 0x148UL, 0x100UL, 0x150UL, 0x170UL,
	0x138UL, 0x130UL, 0x140UL, 0x110UL, 0x160UL,
	0x1E8UL, 0x1D8UL, 0x1A0UL, 0x188UL, 0x198UL,
	0x1F8UL, 0x1A8UL, 0x1C8UL, 0x180UL, 0x1D0UL,
	0x1F0UL, 0x1B8UL, 0x1B0UL, 0x1C0UL, 0x190UL,
	0x1E0UL, 0x768UL, 0x758UL, 0x720UL, 0x708UL,
	0x718UL, 0x778UL, 0x728UL, 0x748UL, 0x700UL,
	0x750UL, 0x770UL, 0x738UL, 0x730UL, 0x740UL,
	0x710UL, 0x760UL, 0x368UL, 0x358UL, 0x320UL,
	0x308UL, 0x318UL, 0x378UL, 0x328UL, 0x348UL,
	0x300UL, 0x350UL, 0x370UL, 0x338UL, 0x330UL,
	0x340UL, 0x310UL, 0x360UL, 0x5E8UL, 0x5D8UL,
	0x5A0UL, 0x588UL, 0x598UL, 0x5F8UL, 0x5A8UL,
	0x5C8UL, 0x580UL, 0x5D0UL, 0x5F0UL, 0x5B8UL,
	0x5B0UL, 0x5C0UL, 0x590UL, 0x5E0UL, 0x468UL,
	0x458UL, 0x420UL, 0x408UL, 0x418UL, 0x478UL,
	0x428UL, 0x448UL, 0x400UL, 0x450UL, 0x470UL,
	0x438UL, 0x430UL, 0x440UL, 0x410UL, 0x460UL,
	0x668UL, 0x658UL, 0x620UL, 0x608UL, 0x618UL,
	0x678UL, 0x628UL, 0x648UL, 0x600UL, 0x650UL,
	0x670UL, 0x638UL, 0x630UL, 0x640UL, 0x610UL,
	0x660UL,
};

/*
 *  A macro that performs a full encryption round of GOST 28147-89.
 *  Temporary variable t assumed and variables r and l for left and right
 *  blocks
 */

#define GOST_ENCRYPT_ROUND(k1, k2) \
t = (k1) + r; \
l ^= gost_sbox_1[t & 0xff] ^ gost_sbox_2[(t >> 8) & 0xff] ^ \
gost_sbox_3[(t >> 16) & 0xff] ^ gost_sbox_4[t >> 24]; \
t = (k2) + l; \
r ^= gost_sbox_1[t & 0xff] ^ gost_sbox_2[(t >> 8) & 0xff] ^ \
gost_sbox_3[(t >> 16) & 0xff] ^ gost_sbox_4[t >> 24]; \

/*
   encrypt a block with the given key 
 */

#define GOST_ENCRYPT(key) \
GOST_ENCRYPT_ROUND(key[0], key[1]) \
GOST_ENCRYPT_ROUND(key[2], key[3]) \
GOST_ENCRYPT_ROUND(key[4], key[5]) \
GOST_ENCRYPT_ROUND(key[6], key[7]) \
GOST_ENCRYPT_ROUND(key[0], key[1]) \
GOST_ENCRYPT_ROUND(key[2], key[3]) \
GOST_ENCRYPT_ROUND(key[4], key[5]) \
GOST_ENCRYPT_ROUND(key[6], key[7]) \
GOST_ENCRYPT_ROUND(key[0], key[1]) \
GOST_ENCRYPT_ROUND(key[2], key[3]) \
GOST_ENCRYPT_ROUND(key[4], key[5]) \
GOST_ENCRYPT_ROUND(key[6], key[7]) \
GOST_ENCRYPT_ROUND(key[7], key[6]) \
GOST_ENCRYPT_ROUND(key[5], key[4]) \
GOST_ENCRYPT_ROUND(key[3], key[2]) \
GOST_ENCRYPT_ROUND(key[1], key[0]) \
t = r; \
r = l; \
l = t;

/*
 *  "chi" compression function. the result is stored over h
 */

static void gosthash_compress(mutils_word32 * h, mutils_word32 * m)
{
	mutils_word32 i;
	mutils_word32 l, r, t, key[8], u[8], v[8], w[8], s[8];

	mutils_memcpy(u, h, sizeof(u));
	mutils_memcpy(v, m, sizeof(u));

	for (i = 0; i < 8; i += 2) {
		w[0] = u[0] ^ v[0];	/*
					   w = u xor v 
					 */
		w[1] = u[1] ^ v[1];
		w[2] = u[2] ^ v[2];
		w[3] = u[3] ^ v[3];
		w[4] = u[4] ^ v[4];
		w[5] = u[5] ^ v[5];
		w[6] = u[6] ^ v[6];
		w[7] = u[7] ^ v[7];

		/*
		   P-Transformation 
		 */

		key[0] = (w[0] & 0x000000ff) | ((w[2] & 0x000000ff) << 8) | ((w[4] & 0x000000ff) << 16) | ((w[6] & 0x000000ff) << 24);
		key[1] = ((w[0] & 0x0000ff00) >> 8) | (w[2] & 0x0000ff00) | ((w[4] & 0x0000ff00) << 8) | ((w[6] & 0x0000ff00) << 16);
		key[2] = ((w[0] & 0x00ff0000) >> 16) | ((w[2] & 0x00ff0000) >> 8) | (w[4] & 0x00ff0000)
		    | ((w[6] & 0x00ff0000) << 8);
		key[3] = ((w[0] & 0xff000000) >> 24) | ((w[2] & 0xff000000) >> 16) | ((w[4] & 0xff000000) >> 8) | (w[6] & 0xff000000);
		key[4] = (w[1] & 0x000000ff) | ((w[3] & 0x000000ff) << 8) | ((w[5] & 0x000000ff) << 16) | ((w[7] & 0x000000ff) << 24);
		key[5] = ((w[1] & 0x0000ff00) >> 8) | (w[3] & 0x0000ff00) | ((w[5] & 0x0000ff00) << 8) | ((w[7] & 0x0000ff00) << 16);
		key[6] = ((w[1] & 0x00ff0000) >> 16) | ((w[3] & 0x00ff0000) >> 8) | (w[5] & 0x00ff0000)
		    | ((w[7] & 0x00ff0000) << 8);
		key[7] = ((w[1] & 0xff000000) >> 24) | ((w[3] & 0xff000000) >> 16) | ((w[5] & 0xff000000) >> 8) | (w[7] & 0xff000000);

		r = h[i];	/*
				   encriphering transformation 
				 */
		l = h[i + 1];
		GOST_ENCRYPT(key);

		s[i] = r;
		s[i + 1] = l;

		if (i == 6)
			break;

		l = u[0] ^ u[2];	/*
					   U = A(U) 
					 */
		r = u[1] ^ u[3];
		u[0] = u[2];
		u[1] = u[3];
		u[2] = u[4];
		u[3] = u[5];
		u[4] = u[6];
		u[5] = u[7];
		u[6] = l;
		u[7] = r;

		if (i == 2) {	/*
				   Constant C_3 
				 */
			u[0] ^= 0xff00ff00;
			u[1] ^= 0xff00ff00;
			u[2] ^= 0x00ff00ff;
			u[3] ^= 0x00ff00ff;
			u[4] ^= 0x00ffff00;
			u[5] ^= 0xff0000ff;
			u[6] ^= 0x000000ff;
			u[7] ^= 0xff00ffff;
		}

		l = v[0];	/*
				   V = A(A(V)) 
				 */
		r = v[2];
		v[0] = v[4];
		v[2] = v[6];
		v[4] = l ^ r;
		v[6] = v[0] ^ r;
		l = v[1];
		r = v[3];
		v[1] = v[5];
		v[3] = v[7];
		v[5] = l ^ r;
		v[7] = v[1] ^ r;
	}

	/*
	   12 rounds of the LFSR (computed from a product matrix) and xor in M 
	 */

	u[0] = m[0] ^ s[6];
	u[1] = m[1] ^ s[7];
	u[2] = m[2] ^ (s[0] << 16) ^ (s[0] >> 16) ^ (s[0] & 0xffff) ^ (s[1] & 0xffff) ^ (s[1] >> 16) ^ (s[2] << 16) ^ s[6] ^ (s[6] << 16) ^ (s[7] & 0xffff0000) ^ (s[7] >> 16);
	u[3] = m[3] ^ (s[0] & 0xffff) ^ (s[0] << 16) ^ (s[1] & 0xffff) ^ (s[1]
									  << 16)
	    ^ (s[1] >> 16) ^ (s[2] << 16) ^ (s[2] >> 16) ^ (s[3] << 16) ^ s[6] ^ (s[6] << 16) ^ (s[6] >> 16) ^ (s[7] & 0xffff) ^ (s[7] << 16) ^ (s[7] >> 16);
	u[4] =
	    m[4] ^ (s[0] & 0xffff0000) ^ (s[0] << 16) ^ (s[0] >> 16) ^
	    (s[1] & 0xffff0000) ^ (s[1] >> 16) ^ (s[2] << 16) ^ (s[2] >> 16) ^ (s[3] << 16) ^ (s[3] >> 16) ^ (s[4] << 16) ^ (s[6] << 16) ^ (s[6] >> 16) ^ (s[7] & 0xffff) ^ (s[7] << 16) ^ (s[7] >> 16);
	u[5] =
	    m[5] ^ (s[0] << 16) ^ (s[0] >> 16) ^ (s[0] & 0xffff0000) ^
	    (s[1] & 0xffff) ^ s[2] ^ (s[2] >> 16) ^ (s[3] << 16) ^ (s[3] >>
								    16) ^ (s[4] << 16) ^ (s[4] >> 16) ^ (s[5] << 16) ^ (s[6] << 16) ^ (s[6] >> 16) ^ (s[7] & 0xffff0000) ^ (s[7] << 16) ^ (s[7] >> 16);
	u[6] = m[6] ^ s[0] ^ (s[1] >> 16) ^ (s[2] << 16) ^ s[3] ^ (s[3] >> 16)
	    ^ (s[4] << 16) ^ (s[4] >> 16) ^ (s[5] << 16) ^ (s[5] >> 16) ^ s[6] ^ (s[6] << 16) ^ (s[6] >> 16) ^ (s[7] << 16);
	u[7] =
	    m[7] ^ (s[0] & 0xffff0000) ^ (s[0] << 16) ^ (s[1] & 0xffff) ^
	    (s[1] << 16) ^ (s[2] >> 16) ^ (s[3] << 16) ^ s[4] ^ (s[4] >> 16) ^ (s[5] << 16) ^ (s[5] >> 16) ^ (s[6] >> 16) ^ (s[7] & 0xffff) ^ (s[7] << 16) ^ (s[7] >> 16);

	/*
	   16 * 1 round of the LFSR and xor in H 
	 */

	v[0] = h[0] ^ (u[1] << 16) ^ (u[0] >> 16);
	v[1] = h[1] ^ (u[2] << 16) ^ (u[1] >> 16);
	v[2] = h[2] ^ (u[3] << 16) ^ (u[2] >> 16);
	v[3] = h[3] ^ (u[4] << 16) ^ (u[3] >> 16);
	v[4] = h[4] ^ (u[5] << 16) ^ (u[4] >> 16);
	v[5] = h[5] ^ (u[6] << 16) ^ (u[5] >> 16);
	v[6] = h[6] ^ (u[7] << 16) ^ (u[6] >> 16);
	v[7] = h[7] ^ (u[0] & 0xffff0000) ^ (u[0] << 16) ^ (u[7] >> 16) ^ (u[1] & 0xffff0000) ^ (u[1] << 16) ^ (u[6] << 16) ^ (u[7] & 0xffff0000);

	/*
	   61 rounds of LFSR, mixing up h (computed from a product matrix) 
	 */

	h[0] =
	    (v[0] & 0xffff0000) ^ (v[0] << 16) ^ (v[0] >> 16) ^ (v[1] >>
								 16) ^
	    (v[1] & 0xffff0000) ^ (v[2] << 16) ^ (v[3] >> 16) ^ (v[4] << 16) ^ (v[5] >> 16) ^ v[5] ^ (v[6] >> 16) ^ (v[7] << 16) ^ (v[7] >> 16) ^ (v[7] & 0xffff);
	h[1] =
	    (v[0] << 16) ^ (v[0] >> 16) ^ (v[0] & 0xffff0000) ^ (v[1] &
								 0xffff) ^ v[2] ^ (v[2] >> 16) ^ (v[3] << 16) ^ (v[4] >> 16) ^ (v[5] << 16) ^ (v[6] << 16) ^ v[6] ^ (v[7] & 0xffff0000) ^ (v[7] >> 16);
	h[2] =
	    (v[0] & 0xffff) ^ (v[0] << 16) ^ (v[1] << 16) ^ (v[1] >> 16) ^
	    (v[1] & 0xffff0000) ^ (v[2] << 16) ^ (v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^ (v[5] >> 16) ^ v[6] ^ (v[6] >> 16) ^ (v[7] & 0xffff) ^ (v[7] << 16) ^ (v[7] >> 16);
	h[3] = (v[0] << 16) ^ (v[0] >> 16) ^ (v[0] & 0xffff0000) ^ (v[1] & 0xffff0000)
	    ^ (v[1] >> 16) ^ (v[2] << 16) ^ (v[2] >> 16) ^ v[2] ^ (v[3] << 16) ^ (v[4] >> 16) ^ v[4] ^ (v[5] << 16) ^ (v[6] << 16) ^ (v[7] & 0xffff) ^ (v[7] >> 16);
	h[4] = (v[0] >> 16) ^ (v[1] << 16) ^ v[1] ^ (v[2] >> 16) ^ v[2] ^ (v[3] << 16) ^ (v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^ (v[5] >> 16) ^ v[5]
	    ^ (v[6] << 16) ^ (v[6] >> 16) ^ (v[7] << 16);
	h[5] = (v[0] << 16) ^ (v[0] & 0xffff0000) ^ (v[1] << 16) ^ (v[1] >> 16) ^ (v[1] & 0xffff0000) ^ (v[2] << 16) ^ v[2] ^ (v[3] >> 16) ^ v[3]
	    ^ (v[4] << 16) ^ (v[4] >> 16) ^ v[4] ^ (v[5] << 16) ^ (v[6] << 16) ^ (v[6] >> 16) ^ v[6] ^ (v[7] << 16) ^ (v[7] >> 16) ^ (v[7] & 0xffff0000);
	h[6] = v[0] ^ v[2] ^ (v[2] >> 16) ^ v[3] ^ (v[3] << 16) ^ v[4] ^ (v[4]
									  >> 16)
	    ^ (v[5] << 16) ^ (v[5] >> 16) ^ v[5] ^ (v[6] << 16) ^ (v[6] >> 16) ^ v[6] ^ (v[7] << 16) ^ v[7];
	h[7] = v[0] ^ (v[0] >> 16) ^ (v[1] << 16) ^ (v[1] >> 16) ^ (v[2] << 16) ^ (v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^ v[4] ^ (v[5] >> 16) ^ v[5]
	    ^ (v[6] << 16) ^ (v[6] >> 16) ^ (v[7] << 16) ^ v[7];
}

/*
   Clear the state of the given context structure. 
 */

void gosthash_reset(GostHashCtx * ctx)
{
	mutils_bzero(ctx->sum, 32);
	mutils_bzero(ctx->hash, 32);
	mutils_bzero(ctx->len, 32);
	mutils_bzero(ctx->partial, 32);
	ctx->partial_bytes = 0;
}

/*
   Mix in a 32-byte chunk ("stage 3") 
 */

static void gosthash_bytes(GostHashCtx * ctx, __const mutils_word8 * buf, mutils_word32 bits)
{
	mutils_word32 i;
	mutils_word32 j;
	mutils_word32 a, c, m[8];

	/*
	   convert bytes to a long words and compute the sum 
	 */

	j = 0;
	c = 0;
	for (i = 0; i < 8; i++) {
		a = ((mutils_word32) buf[j]) | (((mutils_word32) buf[j + 1]) << 8) | (((mutils_word32) buf[j + 2]) << 16) | (((mutils_word32) buf[j + 3]) << 24);
		j += 4;
		m[i] = a;
		c = a + c + ctx->sum[i];
		
		if ((a==0xFFFFFFFFUL) && (ctx->sum[i]==0xFFFFFFFFUL)) {
			ctx->sum[i] = c;
			c = 1;
		} else {
			ctx->sum[i] = c;
			c = c < a ? 1 : 0;
		}
	}

	/*
	   compress 
	 */

	gosthash_compress(ctx->hash, m);

	/*
	   a 64-bit counter should be sufficient 
	 */

	ctx->len[0] += bits;
	if (ctx->len[0] < bits)
		ctx->len[1]++;
}

/*
   Mix in len bytes of data for the given buffer. 
 */

void gosthash_update(GostHashCtx * ctx, __const mutils_word8 * buf, mutils_word32 len)
{
	mutils_word32 i, j;

	i = ctx->partial_bytes;
	j = 0;
	while (i < 32 && j < len)
		ctx->partial[i++] = buf[j++];

	if (i < 32) {
		ctx->partial_bytes = i;
		return;
	}
	gosthash_bytes(ctx, ctx->partial, 256);

	while ((j + 32) < len) {
		gosthash_bytes(ctx, &buf[j], 256);
		j += 32;
	}

	i = 0;
	while (j < len)
		ctx->partial[i++] = buf[j++];
	ctx->partial_bytes = i;
}


/*
   Compute and save the 32-byte digest. 
 */

void gosthash_final(GostHashCtx * ctx, mutils_word8 * digest)
{
	mutils_word32 i;
	mutils_word32 j;
	mutils_word32 a;

	/*
	   adjust and mix in the last chunk 
	 */

	if (ctx->partial_bytes > 0) {
		mutils_bzero(&ctx->partial[ctx->partial_bytes], 32 - ctx->partial_bytes);
		gosthash_bytes(ctx, ctx->partial, ctx->partial_bytes << 3);
	}

	/*
	   mix in the length and the sum 
	 */

	gosthash_compress(ctx->hash, ctx->len);
	gosthash_compress(ctx->hash, ctx->sum);

	/*
	   convert the output to bytes 
	 */

	j = 0;

	if (digest != NULL)
	for (i = 0; i < 8; i++) {
		a = ctx->hash[i];
		digest[j] = (mutils_word8) a;
		digest[j + 1] = (mutils_word8) (a >> 8);
		digest[j + 2] = (mutils_word8) (a >> 16);
		digest[j + 3] = (mutils_word8) (a >> 24);
		j += 4;
	}
}

#endif /* ENABLE_GOST */
