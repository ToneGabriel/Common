#pragma once
#include "Common.h"
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

enum class FileCompareFlag
{
    LastWriteTime,
    FileSize,
    FileName
};

std::vector<fs::directory_entry> sort_files_in_dir(const std::string& directoryName, FileCompareFlag compareFlag);

std::vector<fs::directory_entry> merge_dirs(const std::vector<std::string>& directories,
                                            const std::string& destination,
                                            FileCompareFlag compareFlag);

// void change_last_write_time_in_dir( const std::string& directoryName,
//                                     const std::chrono::system_clock::time_point& newTime);






// template impl =======================================================================


// template<class Duration>
// void change_last_write_time_in_dir( const std::string& directoryName,
//                                     const std::chrono::system_clock::time_point& newTime)
// {
//     CUSTOM_ASSERT(fs::exists(directoryName), "Directory does not exist!");

//     auto iter = fs::directory_iterator(directoryName);

//     for (const auto& entry : iter)
//         fs::last_write_time(entry, fs::file_time_type(newTime));
// }
