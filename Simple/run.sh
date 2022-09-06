rm -rf simple-static
# make -B
g++ main.cpp -o simple-static ../build/libftd2xx.a -Wall -Wextra -lpthread -lrt

sudo ./simple-static