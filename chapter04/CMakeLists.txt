if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

add_library(FnNamePrint SHARED FnNamePrint.cpp)
target_link_libraries(FnNamePrint ${llvm_libs})
