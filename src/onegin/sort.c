#include "onegin.h"

typedef char byte;

void mswap(byte* a, byte* b, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    byte t = a[i];
    a[i] = b[i];
    b[i] = t;
  }
}

void my_qsort(
    void* _buf, size_t len, size_t item_size,
    int (*cmp)(const void* a, const void* b, void* arg), void* arg) {

  byte* buf = (byte*) _buf;

  if (len <= 1)
    return;

  // Item we compare to, at the middle of the array
  byte *compared = buf + len/2 * item_size;

  // Borders
  byte *smaller_border = buf, *equal_border = buf;

  for (size_t i = 0; i < len; ++i) {

    byte* item = buf + item_size * i;

    // Do not compare item to itself
    int cmp_res = item == compared ? 0 : cmp(compared, item, arg);

    if (cmp_res > 0) {
      // 
      //          v sb            v eb  |
      // -----------------------------------
      //  1 | 2 | 3 | 3 | 3 | 3 | 4 | 5 | 1
      // -----------------------------------
      //          |                     | i
      //          '-----------------------' move one to expected place, but we have 3 at i now  
      //                          '-------' put 3 where it belongs
      // 
      mswap(item, smaller_border, item_size);
      if (smaller_border != equal_border)
        mswap(item, equal_border, item_size);
      //*((int*) smaller_border) = 43;

      // Item we are comparing to may be at smaller_border
      // now, which we moved to equal_border.
      if (smaller_border == compared)
        compared = equal_border;

      // Advance
      smaller_border += item_size;
      equal_border += item_size;

    } else if (cmp_res == 0) {

      // Move this item where it belongs
      mswap(item, equal_border, item_size);

      // It could be the item we compare to,
      // in that case it moved to equal_border.
      if (item == compared)
        compared = equal_border;

      // Advance
      equal_border += item_size;

    } else {
      // This item is greater, everything is fine with it here
    }
  }
 
  my_qsort(buf, (smaller_border - buf)/item_size, item_size, cmp, arg);
  my_qsort(equal_border, len - (equal_border - buf)/item_size, item_size, cmp, arg);
}
