#include "junkcode.h"

namespace {
    const char* asm_size_1[] = { "nop" };
    const char* asm_size_2[] = { "push eax\npop eax","push ebx\npop ebx","push ecx\npop ecx","push edx\npop edx","push esi\npop esi","push edi\npop edi","pushad\npopad" };
    const char* asm_size_3[] = { "push eax\ninc eax\npop eax", "push eax\ndec eax\npop eax", "push ebx\ninc ebx\npop ebx", "push ebx\ndec ebx\npop ebx", "push ecx\ninc ecx\npop ecx", "push ecx\ndec ecx\npop ecx", "push edx\ninc edx\npop edx", "push edx\ndec edx\npop edx" };
    const char* asm_size_4[] = { "push eax\ninc eax\ninc eax\npop eax", "push eax\ndec eax\ndec eax\npop eax", "push eax\ninc eax\ndec eax\npop eax", "push eax\ninc eax\ndec eax\npop eax", "push ebx\ninc ebx\ninc ebx\npop ebx", "push ebx\ndec ebx\ndec ebx\npop ebx", "push ebx\ninc ebx\ndec ebx\npop ebx", "push ebx\ninc ebx\ndec ebx\npop ebx", "push ecx\ninc ecx\ninc ecx\npop ecx", "push ecx\ndec ecx\ndec ecx\npop ecx", "push ecx\ninc ecx\ndec ecx\npop ecx", "push ecx\ninc ecx\ndec ecx\npop ecx", "push edx\ninc edx\ninc edx\npop edx", "push edx\ndec edx\ndec edx\npop edx", "push edx\ninc edx\ndec edx\npop edx", "push edx\ninc edx\ndec edx\npop edx" };
}

const char* get_size1(int& rem_size)
{
    rem_size -= 1;
    return asm_size_1[0];
}

const char* get_size2(int& rem_size)
{
    rem_size -= 2;
    int choice = rand() % (sizeof(asm_size_2) / sizeof(asm_size_2[0]));
    return asm_size_2[choice];
}

const char* get_size3(int& rem_size)
{
    rem_size -= 3;
    int choice = rand() % (sizeof(asm_size_2) / sizeof(asm_size_3[0]));
    return asm_size_3[choice];
}

const char* get_size4(int& rem_size)
{
    rem_size -= 4;
    int choice = rand() % (sizeof(asm_size_2) / sizeof(asm_size_4[0]));
    return asm_size_4[choice];
}

#define App(x) junk.append(x)

std::string get_junk(int size, int &junk_counter) {
    junk_counter++;
    std::srand(time(0) * junk_counter);

    int rem_size = size;
    std::string junk;
    App("\n_asm {\n");
    while (rem_size > 0) {
        int choice = rand();
        printf("\n%d\n", choice % 12);
        if (rem_size == 1) {
            App(get_size1(rem_size));
        }
        else if (rem_size == 2) {

            if (choice % 2 == 0) {

                App(get_size1(rem_size));
            }
            else
            {
                App(get_size2(rem_size));
            }
        }
        else if (rem_size == 3) {
            if (choice % 6 == 0)
                App(get_size1(rem_size));
            else if (choice % 6 == 1 || choice % 6 == 2)
            {
                App(get_size2(rem_size));
            }
            else
            {
                App(get_size3(rem_size));
            }
        }
        else {
            if (choice % 12 == 0 || choice % 12 == 1)
                App(get_size1(rem_size));
            else if (choice % 12 == 4 || choice % 12 == 2)
            {
                App(get_size2(rem_size));
            }
            else if (choice % 12 == 5 || choice % 12 == 3 || choice % 12 == 6)
            {
                App(get_size3(rem_size));
            }
            else
            {
                App(get_size4(rem_size));
            }
        }
        const char* newline = "\n";
        junk.append(newline);
    }
    App("}\n");
    return junk;
};