#include "Run.h"

// two functions queueScene and endScene?

// FINAL GRAHPICS TODO: screen-space shadows & frustrum culling & updated rendering pipeline (deferred/forward+) & MT rendering

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		std::cout << argv[1] << std::endl;
	}

	return run();
}
