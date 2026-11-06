#include <stdio.h>
#include <unistd.h>

int main(void)
{
    while (1)
    {
        printf("DiagnosticsService Alive\n");
        sleep(2);
    }
}
