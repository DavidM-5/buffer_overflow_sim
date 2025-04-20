#include "PEApplication.h"

int main() {
    application::PEApplication app;

    if (!app.init()) {
        std::cerr << "PEApplication failed to initialize" << std::endl;
        return 1;
    }

    app.run();
    return 0;
}
