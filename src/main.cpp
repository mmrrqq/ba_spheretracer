#include "Spheremarcher.h"
#include "utils/Application.h"

int main(void)
{
    Spheremarcher marcher(1920, 600);
    Application app((char *)"Spheremarcher", &marcher);

    app.run();
}