## LLVM tools

- Use clang with the following  options to convert it to LLVM IR.

    `> clang -emit-llvm -c -S add.c`

- LLVM assembler: `llvm-as`

    `> llvm-as add.ll -o add.bc`

- To view the content of this bitcode file, a tool such as `hexdump` can be used.

    `> hexdump -c add.bc` 

- LLVM disassembler:

  `> llvm-dis add.bc  -o add.ll`

- LLVM Linker:
  
  `> llvm-link main.bc add.bc -o output.bc`

- LLVM JIT Compiler or Interpreter
  
  `> lli output.bc`

- LLVM static compiler 
  
  `> llc output.bc -o output.s`







