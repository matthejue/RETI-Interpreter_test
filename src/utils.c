#include <stdint.h>

// r = a % m <-> r + m * q = a + m * p <-> r = a - m * q (q is biggest q such that q*m <= a) = a − m * (a / m)
// 0 <= r < m, a,q € Z, m € N/0 (normal C / hardware implementation ignores 0 <= r < m)
// 7 % 3 = 7 - 3 * (7 / 3) = 1
// -7 % 3 = -7 - 3 * (-7 / 3) = -1, but r has to be positive, get next congruent element of congruence class by -1 + 3 = 2 (one can easily get negative or positive r with -m < r < m by just nr = r - m and also r = nr + m)
// irrelevant cases where on ignores m € N/0:
// 7 % -3 = 7 - (-3) * (7 / -3) = 1 (if we ignore m € N/0), but that is only for truncated division, for floor it is 7 - (-3) * (7 / -3) = 7 - (-3) * -3 = 7 - 9 = -2 (if we ignore 0 <= r < m and say -m < r < m)
// -7 % -3 = -7 - (-3) * (-7 / -3) = -1 (if we ignore m € N/0 and 0 =< r < m and thus -m < r < m), for floor it is the same: -7 - (-3) * (-7 / -3) = -7 - (-3) * 2 = -7 - (-6) = -1, -1 + 3 = 2 (if don't want to also ignore 0 =< r < m)

uint32_t mod(int32_t a, int32_t b) {
  // r = a − m x trunc(a/m)
  int32_t result = a % b;
  if (result < 0) {
    result += b;
  }
  return result;
}

int32_t max(int32_t a, int32_t b) { return (a > b) ? a : b; }

int32_t min(int32_t a, int32_t b) { return (a < b) ? a : b; }

uint32_t sign_extend_22_to_32(uint32_t num) {
  // Check if the number is negative by checking the 21st bit
  if (num & (1 << 21)) {
    // If negative, set the upper bits to 1
    num |= ~((1 << 22) - 1);
  } else {
    // If positive, ensure the upper bits are 0
    num &= (1 << 22) - 1;
  }
  return num;
}

uint32_t swap_endian_32(uint32_t value) {
  return (value >> 24) | ((value >> 8) & 0x0000FF00) |
         ((value << 8) & 0x00FF0000) | (value << 24);
}
