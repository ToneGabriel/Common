#include "FileOperations.h"

std::vector<fs::directory_entry> sort_files_in_dir(const std::string& directoryName, FileCompareFlag compareFlag)
{
    std::vector<fs::directory_entry> ret;
    auto iter = fs::directory_iterator(directoryName);
    bool(*comp)(const fs::directory_entry&, const fs::directory_entry&);

    for (const auto& entry : iter)
        if (entry.is_regular_file())
            ret.push_back(entry);

    switch (compareFlag)
    {
        case FileCompareFlag::LastWriteTime:
            comp = [](const fs::directory_entry& first, const fs::directory_entry& second) -> bool
                    {
                        return first.last_write_time() < second.last_write_time();
                    };
            break;

        case FileCompareFlag::FileSize:
            comp = [](const fs::directory_entry& first, const fs::directory_entry& second) -> bool
                    {
                        return first.file_size() < second.file_size();
                    };
            break;

        case FileCompareFlag::FileName:
            comp = [](const fs::directory_entry& first, const fs::directory_entry& second) -> bool
                    {
                        return first.path().string() < second.path().string();
                    };

        default:
            break;
    }

    std::sort(ret.begin(), ret.end(), comp);

    return ret;
}

// this renames the files
std::vector<fs::directory_entry> merge_dirs(const std::vector<std::string>& directories,
                                            const std::string& destination,
                                            FileCompareFlag compareFlag)
{
    std::vector<fs::directory_entry> ret;
    std::vector<fs::directory_entry> sortedTemp;
    std::vector<fs::directory_entry> mergedTemp;

    for (const auto& dir : directories)
    {
        sortedTemp = std::move(sort_files_in_dir(dir, compareFlag));
        std::merge(ret.begin(), ret.end(), sortedTemp.begin(), sortedTemp.end(), std::back_inserter(mergedTemp));
        ret = std::move(mergedTemp);

        mergedTemp.clear(); // std::move() may not leave this vector empty
    }

    return ret;
}