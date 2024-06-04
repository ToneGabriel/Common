#include "Merge.h"

std::string format_time(const std::chrono::system_clock::time_point& tp)
{
    std::time_t tt  = std::chrono::system_clock::to_time_t(tp);
    std::tm tm      = *std::localtime(&tt);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);

    return std::string(buffer);
}

Files sort_files(const std::string& directoryName, std::chrono::hours offset = std::chrono::hours(0))
{
    Files files;

    for (const auto& entry : fs::directory_iterator(directoryName))
    {
        auto path = entry.path();

        if (entry.is_regular_file() && (path.extension() == ".jpg" || path.extension() == ".JPG"))
            files.emplace_back(path, fs::last_write_time(path) + offset);
    }

    std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

    return files;
}

void copy_and_rename_one(const fs::path& old, const std::string& destination, const fs::file_time_type& time, int index)
{
    // Convert file_time_type to system_clock::time_point
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(time - fs::file_time_type::clock::now() + std::chrono::system_clock::now());

    // Print the time
    // std::cout << "Old File: " << old << "\n";
    // std::cout << "Creation time (adjusted): " << format_time(sctp) << "\n\n";

    // Construct new file name based on index and creation time
    std::string new_file_name = "image" + std::to_string(index) + ".jpg";
    std::string destination_path = destination + "\\" + new_file_name;
    fs::copy_file(old, destination_path);
}

void copy_and_rename(const Files& files, const std::string& destination)
{
    ProgressBar progressBar;
    int index = 1;

    for (const auto& [file_path, adjusted_time] : files)
    {
        progressBar.add_task(std::bind(copy_and_rename_one, std::ref(file_path), std::ref(destination), std::ref(adjusted_time), index++));
    }

    progressBar.run();

    std::cout << std::endl; // Move to the next line after progress bar is complete
}