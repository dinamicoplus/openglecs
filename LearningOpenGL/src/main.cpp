#include "Application.h"

int main(int argc, char** argv)
{   
    Application app{};

    while (app.isRunning())
    {
        app.updateInput();
        app.update();
        app.render();
    }

    app.close();

    return 0;
}
