opt -load ../build/lib/libFnNamePrint.so -legacy-fn-print <test.ll> -dsiable-output -enable-new-pm=0 -time-passes
opt -load-pass-plugin=../build/lib/libFnNamePrint.so -passes="fn-print" -disable-output <test.ll> -time-passes
