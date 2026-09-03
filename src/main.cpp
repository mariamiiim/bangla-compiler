#include <iostream>
#include <fstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "parser.h"

using namespace std;

int main(int argc, char* argv[])
{
#ifdef _WIN32
    // Make Windows consoles print UTF-8 (Bangla) text correctly.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    string filename = "program.bangla";

    if (argc > 1)
    {
        filename = argv[1];
    }

    ifstream file(filename, ios::binary);

    if (!file)
    {
        cout << "ERROR: Cannot open " << filename << endl;
        return 1;
    }

    string source(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>()
    );

    file.close();

    // Strip a UTF-8 byte-order-mark (EF BB BF) if the file was saved
    // with one (common when editing with Notepad on Windows). Without
    // this, the lexer sees 3 bogus bytes and reports an error on line 1
    // even though the file looks fine.
    if (source.size() >= 3 &&
        static_cast<unsigned char>(source[0]) == 0xEF &&
        static_cast<unsigned char>(source[1]) == 0xBB &&
        static_cast<unsigned char>(source[2]) == 0xBF)
    {
        source.erase(0, 3);
    }

    if (source.empty())
    {
        cout << "ERROR: " << filename << " is empty!" << endl;
        return 1;
    }

    Parser parser(source);
    parser.parseProgram();

    return 0;
}