#include <unistd.h>
#include <stdlib.h>
int main() {
    int i = 0;
    fork();
    while (i < 4) {
        if (getpid() % 2 == 0) {
            fork();
        }
        i++;
    }
}