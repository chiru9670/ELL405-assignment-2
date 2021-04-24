#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char * argv[]) {
  if(argc != 3) { // 2 arguments and one for the executable call itself
    printf(2, "user_add requires exactly 2 arguments\n");
  }
  else {
    printf(1, "%d\n", add(atoi(argv[1]), atoi(argv[2])));
  }
  exit();
}
