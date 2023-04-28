#include <python3.10/Python.h>
#include <iostream>
// #include <String.h>

// g++ -g applications/singleKinectTest/sample.cpp -I/usr/include/python3.10/
// g++ -g sample.cpp -I/usr/include/python3.10/
// export PYTHONPATH=/usr/include/python3.10

using namespace std;

int main() {
    wchar_t* program = Py_DecodeLocale("print('Hello world')", NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal Error\n");
        exit(1);
    }
    Py_SetProgramName(program);
    Py_Initialize();

    PyMem_RawFree(program);
    cout << "hello\n";
    return 0;
}

