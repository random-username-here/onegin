#include "onegin.h"
#include <stdlib.h>
#include "istd/util/err.h"

const size_t reorder[][4] = {
  [GENERATED_RYTHM_SEQUENTIAL] = { 0, 1, 2, 3 },
  [GENERATED_RYTHM_CROSSOVER ] = { 0, 2, 1, 3 },
  [GENERATED_RYTHM_WRAPPED   ] = { 0, 2, 3, 1 },
};

string_view_t* generate_something_weird(
    const string_view_t* poem, size_t poem_len,
    size_t generated_len, generated_rythm_type rythm_type) {

  check$(poem != NULL, "You shall pass not-NULL array of lines as poem");
  check$(poem_len != 0, "Given poem should have nonzero length");
  check$(generated_len % 4 == 0, "Length of generated poem shall be divisible by four");

  string_view_t* generated = (string_view_t*) calloc(generated_len, sizeof(string_view_t));
  check$(generated != NULL, "Should be able to allocate memory for generated poem");

  for (size_t line = 0; line < generated_len; line += 4) {

    size_t pos1 = rand() % (poem_len - 1),
           pos2 = rand() % (poem_len - 1);
    
    const string_view_t lines[4] = {
      poem[pos1], poem[pos1+1],
      poem[pos2], poem[pos2+1]
    };

    for (size_t j = 0; j < 4; ++j)
      generated[line+j] = lines[reorder[rythm_type][j]];
  }

  return generated;
}
