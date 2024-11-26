#include <iostream>

#include "../../application/src/Application.h"

int main(int argc, char* argv[]) {
    
    application::Application app;

    if (!app.init()) {
        std::cerr << "Unexpected error occured" << std::endl;
        return 1;
    }

    app.run();
    

    return 0;
}
