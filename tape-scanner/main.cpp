#include "application.cpp"

#ifndef UNIT_TESTS
int main(const int argc, char* argv[]) {
    // return Application::run(argc, argv);

    // Application::convertEbcdicFileToAscii("resources/TAPE1.ebcdic", 128);

    Application::scanTapeHeaders("resources/TAPE1.ebcdic", "ZIA6.DATA");

    return 0;
}
#endif