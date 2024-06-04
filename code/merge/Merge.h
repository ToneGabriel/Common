#pragma once
#include <filesystem>
#include "ProgressBar.h"
#include "Reader.h"

namespace fs = std::filesystem;
using Files = std::vector<std::pair<fs::path, fs::file_time_type>>;

std::string format_time(const std::chrono::system_clock::time_point& tp);

Files sort_files(const std::string& directoryName, std::chrono::hours offset);

void copy_and_rename_one(const fs::path& old, const std::string& destination, const fs::file_time_type& time, int index);

void copy_and_rename(const Files& files, const std::string& destination);