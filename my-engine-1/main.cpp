#include "App.h"
#include "ComInitializer.h"


int main() {
    ComInitializer comInitializer;
    comInitializer.Initialize();

    App app;
    app.Initialize();
    app.Run();

    return 0;
}
