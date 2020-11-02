set(iex_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/message.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/decoder.cpp 
    )

add_library(iex SHARED ${iex_SRC})
target_include_directories(iex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set_property(TARGET iex PROPERTY CMAKE_CXX_STANDARD 17)
set_property(TARGET iex PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)

target_link_libraries(iex ${CONAN_LIBS})
target_link_libraries(iex stdc++fs)
target_link_libraries(iex PcapPlusPlus::PcapPlusPlus)

#message(${PcapPlusPlus_INCLUDE_DIRS})

include_directories(${PcapPlusPlus_INCLUDE_DIRS})
