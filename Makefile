qs: c
	# g++ -std=c++14 src/driver.cpp -o ./qs
	clang++ -std=c++14 -g -O3 src/driver.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o ./qs

c:
	rm -rf qs qs.dSYM
