from libcpp.string cimport string

cdef extern from "include/iex/worker.hpp" namespace "jmnel::iex":
    ctypedef struct iex_meta_t:
        string key
        string date
        string feed
        string link
        string protocol
        size_t download_size
        string version
        string state

    int get_ticks(iex_meta_t)

def download_ticks():

    meta = { "date" : "2018-05-10",
            "feed" : "TOPS",
            "link" : "https://www.googleapis.com/download/storage/v1/b/iex/o/data%2Ffeeds%2F20180510%2F20180510_IEXTP1_TOPS1.6.pcap.gz?generation=1525997640587651&alt=media", 
            "protocol" : "IEXTP1",
            "download_size" : 1050600619,
            "version" : "1.6",
            "key" : "2018-05-10",
            "state" : "need_download" }

    meta = { k : v.encode("utf-8") if isinstance(v, str) else v for k, v in meta.items() }

    return get_ticks(meta)
