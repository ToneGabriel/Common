#include "Config.hpp"
#include "Merge.h"
#include "ThreadPool.h"

void test(int n)
{
    std::this_thread::sleep_for(std::chrono::seconds(n));
}

int main()
{
    std::vector<std::function<void(void)>> vec;
    vec.emplace_back(std::bind(test, 3));
    vec.emplace_back(std::bind(test, 10));
    vec.emplace_back(std::bind(test, 2));
    vec.emplace_back(std::bind(test, 3));
    vec.emplace_back(std::bind(test, 5));
    vec.emplace_back(std::bind(test, 1));
    vec.emplace_back(std::bind(test, 2));
    vec.emplace_back(std::bind(test, 2));
    vec.emplace_back(std::bind(test, 1));
    vec.emplace_back(std::bind(test, 6));

    ThreadPool<5>::start_and_display(std::move(vec));


	// std::string sourceFile = "H:\\Programare\\C++\\Pictures\\data\\poze\\franta_poze_liviu";
	// std::string destFile = "H:\\Programare\\C++\\Pictures\\data\\poze\\franta_poze_narcisa";
	// size_t count = 0;

	// for (const auto& entry : fs::directory_iterator(sourceFile))
    // {
    //     auto path = entry.path();
	// 	auto name = path.string().substr(sourceFile.size() + 1);

    //     if (entry.is_regular_file() &&
	// 		(path.extension() == ".jpg" || path.extension() == ".JPG") &&
	// 		path.string().find('-') != std::string::npos)
    //     {
	// 		++count;
    // 		fs::rename(path, destFile + '\\' + name);
    //     	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	// 	}
    // }

	// std::cout << count << '\n';


    //==============================================================================


	// try
    // {
    //     Reader reader;
    //     reader.read("H:\\Programare\\C++\\Pictures\\data\\path.txt");

    //     Files sorted;
    //     Files merged;

    //     for (const auto& [offset, directory] : reader)
    //     {
    //         Files temp;
    //         sorted = std::move(sort_files(directory, std::chrono::hours(offset)));
    //         std::merge(merged.begin(), merged.end(), sorted.begin(), sorted.end(), std::back_inserter(temp));
    //         merged = std::move(temp);
    //     }

    //     copy_and_rename(merged, reader.dest());

    // } catch (const std::exception& e)
    // {
    //     std::cerr << "Error: " << e.what() << '\n';
    // }

	return 0;
}