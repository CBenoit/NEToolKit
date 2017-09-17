message(STATUS "Found GCC ${CMAKE_CXX_COMPILER_VERSION}")

if(NETKIT_WITH_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Wdouble-promotion -Wnull-dereference -Wimplicit-fallthrough -Wmissing-include-dirs -Wswitch-bool -Wswitch-unreachable -Walloc-zero -Wduplicated-branches -Wduplicated-cond -Wfloat-equal -Wshadow -Wundef -Wexpansion-to-defined -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wdate-time -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wrestrict -Winvalid-pch -Woverlength-strings -Wzero-as-null-pointer-constant -Wsubobject-linkage -Wdelete-incomplete -Wuseless-cast")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast -Wjump-misses-init -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Woverride-init-side-effects -Wnested-externs")
    message(STATUS "GCC: enabled warnings")
endif()

if(NETKIT_WITH_SUGGESTIONS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-override -Wsuggest-attribute=const -Wsuggest-attribute=pure -Wsuggest-attribute=noreturn -Wsuggest-attribute=format -Wsuggest-final-types -Wsuggest-final-methods")
    message(STATUS "GCC: enabled suggestions")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -s -Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -O2 -DNDEBUG")

