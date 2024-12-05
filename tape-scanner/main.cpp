#include "application.cpp"

int main(const int argc, char* argv[]) {
    // return Application::run(argc, argv);
    Application::convertEbcdicFileToAscii("resources/TAPE1.ebcdic", 128);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <tape-location> <collection-name>\n";
        std::cerr << "Example: " << argv[0] << " resources/TAPE1.ebcdic ZIA6.DATA\n";
        return 1;
    }

    const std::string tape_location = argv[1];
    const std::string collection_name = argv[2];

    try {
        Application::scanTapeHeaders(tape_location, collection_name);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}