#pragma once

#include <filesystem>
#include <string>
#include <utility>

using std::pair;
using std::string;

namespace fs = std::filesystem;

namespace jmnel::iex {

    struct iex_meta_t {
        string key;
        string date;
        string feed;
        string link;
        string protocol;
        size_t download_size;
        string version;
        string state;
    };

    int get_ticks( iex_meta_t meta );

    //    fs::path get_temp_dir();
    //    fs::path download( iex_meta_t meta, fs::path dir );
    //    fs::path unzip( iex_meta_t meta );

}  // namespace jmnel::iex

//int foo();
