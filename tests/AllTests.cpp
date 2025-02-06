#include "CppUTest/CommandLineTestRunner.h"

IMPORT_TEST_GROUP(Music_player_test_group);

int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
