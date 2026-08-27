#include "core/application.h"

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    Application app("Physics Engine", SCR_WIDTH, SCR_HEIGHT);

    // Run loop
    app.run();

    return 0;
}