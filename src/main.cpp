#include "Spheremarcher.h"
#include "utils/Application.h"

int main(void)
{
    Spheremarcher marcher(1920, 1080);
    Application app((char *)"Spheremarcher", &marcher);

    app.run();

    exit(EXIT_SUCCESS);
}