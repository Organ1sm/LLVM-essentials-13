add_executable(GetAddress GetAddress.cpp)
target_link_libraries(GetAddress ${llvm_libs})

add_executable(ReadMemory Read-Memory.cpp)
target_link_libraries(ReadMemory ${llvm_libs})

add_executable(WriteMemory Write-Memory.cpp)
target_link_libraries(WriteMemory ${llvm_libs})

add_executable(InsertScalarIntoVec InsertScalarToVec.cpp)
target_link_libraries(InsertScalarIntoVec ${llvm_libs})

add_executable(ExtractScalarFromVec ./ExtractScalarFromVec.cpp)
target_link_libraries(ExtractScalarFromVec ${llvm_libs})
