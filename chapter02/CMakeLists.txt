add_executable(Toy toy.cpp)
target_link_libraries(Toy ${llvm_libs})

add_executable(Function function.cpp)
target_link_libraries(Function ${llvm_libs})

add_executable(FunctionBlock functionBlock.cpp)
target_link_libraries(FunctionBlock ${llvm_libs})

add_executable(Global global.cpp)
target_link_libraries(Global ${llvm_libs})

add_executable(Return return.cpp)
target_link_libraries(Return ${llvm_libs})

add_executable(FunctionArgument funcArgument.cpp)
target_link_libraries(FunctionArgument ${llvm_libs})

add_executable(Arithmetic arithmetic.cpp)
target_link_libraries(Arithmetic ${llvm_libs})

add_executable(IfElse If-Else.cpp)
target_link_libraries(IfElse ${llvm_libs})
