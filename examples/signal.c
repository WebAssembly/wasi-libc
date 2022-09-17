#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile int run = 1;

void sig_handler(int signo)
{
  if (signo == SIGHUP) {
    printf("received SIGHUP\n");
  }
  if (signo == SIGINT) {
    run = 0;
    printf("received SIGINT\n");
  }
}

int main(void)
{
  if (signal(SIGINT, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");
  if (signal(SIGHUP, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGHUP\n");

  raise(SIGHUP);
  
  // A long long wait so that we can easily issue a signal to this process
  while(run) {
    sleep(1);
  }
  
  return 0;
}