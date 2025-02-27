#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    printf("Hello world!\n");
    fflush(stdout);

    int a;
    scanf("%d", &a);
    // fflush(stdout);

    printf("Typed: %d\n", a);
    fflush(stdout);

    return 0;
}
