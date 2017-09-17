message(STATUS "Found clang ")

if(MIRAI_WITH_WARNINGS)
    # TODO: check these warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -pedantic-errors -Wall -Wextra -Wold-style-cast -Woverloaded-virtual -Wfloat-equal -Wwrite-strings -Wpointer-arith -Wcast-qual -Wcast-align -Wconversion -Wshadow -Wredundant-decls -Winit-self -Wswitch-default -Wundef -Winline")
    message(STATUS "clang: enabled warnings")
endif()

if(NETKIT_WITH_SUGGESTIONS)
    # TODO: find suggestions flags for clang
    message(STATUS "clang: suggestions not yet availables for clang build")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -s -Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -O2 -DNDEBUG")

