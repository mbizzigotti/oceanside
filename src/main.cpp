#define RGFW_IMPLEMENTATION
#include "engine.h"

int main(int argc, char *argv[]) {
	Options options(argc, argv);

	if (!options.scene_filename) {
		printf("No scene provided\n");
		return 0;
	}

	Engine engine {};
	if (engine.setup(options))
		return Failed;

	return engine.run();
}
