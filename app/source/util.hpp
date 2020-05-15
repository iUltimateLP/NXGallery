/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

#pragma once
#include <cstdarg>
#include <string.h>
#include <string>

// Print a string centered to the console
static void printCentered(const char* str, ...)
{
    char buffer[1024];
    va_list vaList;
    va_start(vaList, buffer);
    vsprintf(buffer, str, vaList);
    va_end(vaList);

    int stringLength = std::string(buffer).size();
    int consoleWidth = consoleGetDefault()->windowWidth;
    int leftPadding = (consoleWidth - stringLength) / 2;

    for (int i = 0; i < leftPadding; i++)
    {
        printf(" ");
    }

    printf("%s", buffer);
}
