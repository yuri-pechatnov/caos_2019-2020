// %%cpp multiplexing_reader_test.cpp

#include "multiplexing_reader_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

const int INPUTS_COUNT = 5;

int main() {
    log_printf("Start multiplexing test");
    pid_t pids[INPUTS_COUNT];
    int input_fds[INPUTS_COUNT];
    // create INPUTS_COUNT subprocesses that will write to pipes with different delays
    for (int i = 0; i < INPUTS_COUNT; ++i) {
        int fds[2];
        pipe(fds);
        input_fds[i] = fds[0];
        if ((pids[i] = fork()) == 0) {
            sleep(INPUTS_COUNT - 1 - i);
            dprintf(fds[1], "Hello from %d subprocess\n", i);
            // try with EPOLL realisation
            // sleep(10);
            // dprintf(fds[1], "Hello 2 from %d subprocess\n", i);
            exit(0);
        }
        close(fds[1]);
    }
    
    // run multiplexing reading
    read_all(input_fds, INPUTS_COUNT);
     
    int status;
    for (int i = 0; i < INPUTS_COUNT; ++i) {
        close(input_fds[i]);
        assert(waitpid(pids[i], &status, 0) != -1);
    }
    log_printf("Finish multiplexing test");
    return 0;
}

