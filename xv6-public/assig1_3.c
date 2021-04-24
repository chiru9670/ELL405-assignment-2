#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
	// toggle(); // This toggles the system trace on or off
	
	/* Removed a \n at the beginning of the below
        * printf string, to match output
	*/
	printf(1, "sum of 2 and 3 is: %d\n",add(2,3));
	exit();
}
