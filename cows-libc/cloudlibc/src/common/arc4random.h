// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef COMMON_ARC4RANDOM_H
#define COMMON_ARC4RANDOM_H

#include <stddef.h>
#include <stdint.h>

// "Spritz -- a spongy RC4-like stream cipher and hash function"
// by Ronald L. Rivest and Jacob C.N. Schuldt.
// http://people.csail.mit.edu/rivest/pubs/RS14.pdf

#define N 256
#define Sw(n) S[(n) % N]

struct spritz {
  uint8_t S[N];
  uint8_t a;
  uint8_t i;
  uint8_t j;
  uint8_t k;
  uint8_t w;
  uint8_t z;
};

#define SPRITZ_INITIALIZER                                                    \
  {                                                                           \
    {                                                                         \
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  \
        14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  \
        28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  \
        42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  \
        56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  \
        70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  \
        84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  \
        98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, \
        112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, \
        126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, \
        140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, \
        154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, \
        168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, \
        182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, \
        196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, \
        210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, \
        224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, \
        238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, \
        252, 253, 254, 255},                                                  \
        0, 0, 0, 0, 1, 0                                                      \
  }

static inline unsigned int spritz_gcd(unsigned int a, unsigned int b) {
  while (b != 0) {
    unsigned int t = b;
    b = a % b;
    a = t;
  }
  return a;
}

static inline void spritz_swap(uint8_t *a, uint8_t *b) {
  uint8_t tmp = *a;
  *a = *b;
  *b = tmp;
}

static inline void spritz_update(struct spritz *s) {
  s->i += s->w;
  s->j = s->k + s->Sw(s->j + s->S[s->i]);
  s->k = s->i + s->k + s->S[s->j];
  spritz_swap(&s->S[s->i], &s->S[s->j]);
}

static inline void spritz_whip(struct spritz *s, unsigned int r) {
  for (unsigned int v = 0; v < r; ++v)
    spritz_update(s);
  do {
    ++s->w;
  } while (spritz_gcd(s->w, N) != 1);
}

static inline void spritz_crush(struct spritz *s) {
  for (uint8_t v = 0; v < N / 2; ++v)
    if (s->S[v] > s->S[N - 1 - v])
      spritz_swap(&s->S[v], &s->S[N - 1 - v]);
}

static inline void spritz_shuffle(struct spritz *s) {
  spritz_whip(s, 2 * N);
  spritz_crush(s);
  spritz_whip(s, 2 * N);
  spritz_crush(s);
  spritz_whip(s, 2 * N);
  s->a = 0;
}

static inline void spritz_absorb_nibble(struct spritz *s, uint8_t x) {
  if (s->a == N / 2)
    spritz_shuffle(s);
  spritz_swap(&s->S[s->a], &s->S[N / 2 + x]);
  ++s->a;
}

static inline void spritz_absorb_byte(struct spritz *s, uint8_t b) {
  spritz_absorb_nibble(s, b & 0xf);
  spritz_absorb_nibble(s, b >> 4);
}

static inline void spritz_absorb(struct spritz *s, const uint8_t *i,
                                 size_t ilen) {
  while (ilen-- > 0)
    spritz_absorb_byte(s, *i++);
}

static inline uint8_t spritz_output(struct spritz *s) {
  s->z = s->Sw(s->j + s->Sw(s->i + s->Sw(s->z + s->k)));
  return s->z;
}

static inline uint8_t spritz_drip(struct spritz *s) {
  if (s->a > 0)
    spritz_shuffle(s);
  spritz_update(s);
  return spritz_output(s);
}

#undef N
#undef Sw

#endif
