#ifndef EXIT_MATCHER_H
#define EXIT_MATCHER_H

#include <stdlib.h>
#include <boost/array.hpp>

enum class ExitState {
  OFFTRACK, NL, E, X, I, T, CR, MATCH
};

class exit_matcher {
public:
  ExitState state;

  exit_matcher();

  bool try_match(boost::array<char, 8192>& buf, size_t buf_size);
private:
  void goto_state(ExitState state);
  void trans(char c);
};

#endif /* EXIT_MATCHER_H */

