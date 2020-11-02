#include "iex/worker.hpp"

#include <curl/curl.h>
#include <curl/easy.h>

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <new>
#include <stdexcept>
#include <tuple>

#include "iex/decoder.hpp"
#include "iex/message.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios_base;
using std::ofstream;

using boost::iostreams::filtering_streambuf;

namespace jmnel::iex {

    fs::path get_temp_dir();
    fs::path download( iex_meta_t meta, fs::path const &dir );
    fs::path unzip( iex_meta_t meta, fs::path const &dir, fs::path const &path_gz );
    iex_meta_t decode( iex_meta_t meta, fs::path const &path_pcap );

    // -- get_ticks function --
    extern int get_ticks( iex_meta_t meta ) {
        const auto dir = get_temp_dir();

        const auto path_gz = download( meta, dir );
        const auto path_pcap = unzip( meta, path_gz, dir );
        meta = decode( meta, path_pcap );

        fs::remove_all( dir );

        return EXIT_SUCCESS;
    }

    // -- get_temp_dir function --
    fs::path get_temp_dir() {
        string dir_template = "/tmp/iex-XXXXXX";

        /// @todo: Check return value of system call.

        return fs::path( mkdtemp( dir_template.data() ) );
    }

    // -- curl_write function --
    size_t curl_write( void *contents, size_t size, size_t nmemb, std::string *s ) {
        const size_t new_length = size * nmemb;
        try {
            s->append( (char *)contents, new_length );
        } catch( std::bad_alloc &e ) {
            return 0;
        }
        return new_length;
    }

    // -- download function --
    fs::path download( iex_meta_t meta, fs::path const &dir ) {

        auto curl = curl_easy_init();
        if( !curl ) {
            cerr << "failed to initialize curl\n";
            std::terminate();
        }

        const auto name_gz = meta.date + ".pcap.gz";
        const auto path_gz = dir / name_gz;

        if( fs::exists( path_gz ) ) {
            throw std::runtime_error( name_gz + " already exists" );
        }

        cout << "downloading " << path_gz << endl;
        FILE *f_handle = nullptr;
        if( f_handle = fopen( path_gz.c_str(), "w" ); !f_handle ) {
            throw std::runtime_error( "unable to open file" );
        }

        curl_easy_setopt( curl,
                          CURLOPT_URL,
                          meta.link.c_str() );
        //        curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, curl_write );

        string response = "";
        curl_easy_setopt( curl, CURLOPT_WRITEDATA, f_handle );
        curl_easy_perform( curl );
        curl_easy_cleanup( curl );
        curl = nullptr;
        fclose( f_handle );
        cout << "download complete\n";
        meta.state = "need_unzip";

        return path_gz;
    }

    // -- unzip function --
    fs::path unzip( iex_meta_t meta, fs::path const &path_gz, fs::path const &dir ) {

        const auto path_pcap = dir / ( meta.date + ".pcap" );
        cout << "decompressing file " << path_gz << " -> " << path_pcap << endl;

        std::ifstream gzip_file( path_gz, ios_base::in | ios_base::binary );
        filtering_streambuf<boost::iostreams::input> in_stream;
        std::ofstream pcap_file( path_pcap, ios_base::out | ios_base::binary );
        in_stream.push( boost::iostreams::gzip_decompressor() );
        in_stream.push( gzip_file );
        boost::iostreams::copy( in_stream, pcap_file );

        gzip_file.close();
        fs::remove( path_gz );
        pcap_file.close();

        cout << "done\n";

        return path_pcap;
    }

    // -- decode function --
    iex_meta_t decode( iex_meta_t meta, fs::path const &path_pcap ) {

        auto pcap_decoder = decoder();
        if( !pcap_decoder.open_file_for_decoding( path_pcap ) ) {
            throw std::runtime_error( "failed to initialize pcap decoder" );
        }

        // Parse header message.
        std::unique_ptr<iex_message_base> message;
        auto result = pcap_decoder.get_next_message( message );

        // Decode all remaining messages in pcap dump.
        size_t count = 0;
        bool is_regular_hours = false;
        for( ; result == return_code_t::success; result = pcap_decoder.get_next_message( message ) ) {
            if( message->type() == message_type::system_event ) {
                const auto system_event = static_cast<system_event_message *>( message.get() );

                // Set flag when entering regular trading hours.
                if( system_event->m_system_event == system_event_message::code::start_of_system_hours ) {
                    is_regular_hours = true;
                }

                // Stop at close of regular trading hours.
                else if( system_event->m_system_event == system_event_message::code::end_of_regular_market_hours ) {
                    break;
                }
            } else if( is_regular_hours && message->type() == message_type::trade_report ) {
                const auto trade_report = static_cast<trade_report_message *>( message.get() );
                trade_report->count++;
            }
        }

        meta.state = "decoded";

        cout << "done, found " << count << " trade reports\n";
        fs::remove( path_pcap );

        return meta;
    }

}  // namespace jmnel::iex
