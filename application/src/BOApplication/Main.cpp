#include "BOApplication.h"

int main() {
    application::BOApplication app;

    if (!app.init()) {
        std::cerr << "BOApplication failed to initialize" << std::endl;
        return 1;
    }

    app.run();
    return 0;
}
