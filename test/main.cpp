#include "test.h"

#include <iostream>


using namespace std;

lest::tests& test_specification()
{
	static lest::tests tests;
	return tests;
}

static int executeTests(int argc, char** argv)
{
	int ret = lest::run(test_specification(), argc, argv);
	if (ret == EXIT_SUCCESS)
		cout << "\nAll tests executed successfully!\n";
	else
		cout << "\nTesting FAILED!\n";

	return ret;
}

int main(int argc, char** argv)
{
	if (argc > 1 && !strcmp(argv[1], "test-prompt"))
	{
		int ret = executeTests(argc - 1, argv + 1);
		cout << "\n\nPress ENTER to exit!\n";
		cin.get();
		return ret;
	}

	return executeTests(argc, argv);
}
