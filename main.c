#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#define NUM_THREADS 100

// Sharred Global variables
pthread_mutex_t mutex;
uint16_t global_port;
uint16_t end_port;
char* tartget_ip;
struct addrinfo hints;

// Tries to connect to given port
int scan_port(struct addrinfo* hints, char* target_ip, int target_port);

// Worker for scanning ports
void* scan_worker(void* arg);

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("Invalid usage!\nport_scan <IP> <Port_low> <Port_high>\n");
    exit(1);
  }
  
  tartget_ip = argv[1];
  global_port = atoi(argv[2]);
  end_port = atoi(argv[3]);

  if(end_port > 65535 || global_port > end_port) {
    printf("Invalid port range.\n");
    exit(1);
  }

  if(pthread_mutex_init(&mutex, NULL)) {
    perror("Mutex init failed");
    exit(1);
  }


  printf("Scanning %s from %d to %d using %d threads...\n", tartget_ip, global_port, end_port, NUM_THREADS);

  // Initialize socket hints
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  // Create threads
  unsigned thread_count = 0;
  pthread_t threads[NUM_THREADS];
  for (int i = 0; (i < NUM_THREADS); i++) {
    if(!pthread_create(&threads[i], NULL, scan_worker, NULL)) {
      thread_count++;
    } else {
      perror("Failed to create thread");
      break;
    }
  }

  for(int i = 0; i < thread_count; i++) {
    pthread_join(threads[i], NULL);
  }
  printf("Done, all threads exited!\n");
  return 0;
}

int scan_port(struct addrinfo* hints, char* target_ip, int target_port) {
  char port_str[6];
  snprintf(port_str, sizeof(port_str), "%d", target_port);
  
  struct addrinfo *result;
  struct addrinfo *rp;
  int socket_fd;

  int status = getaddrinfo(target_ip, port_str, hints, &result);
  if(status) {
    fprintf(stderr, "Error resolving %s:%d - %s\n", target_ip, target_port, gai_strerror(status));
    return 1;
  }

  for(rp = result; rp; rp = rp->ai_next) {

    socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if(socket_fd == -1) continue;

    if(connect(socket_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
      fprintf(stdout, "%d Open\n", target_port);
      close(socket_fd);
      break;
    }

    close(socket_fd);
  }
  freeaddrinfo(result);
  return 0;
}

void* scan_worker(void* arg) {
  char port_str[6];
  struct addrinfo *result, *rp;

  while (1) {
    uint16_t port_to_scan;

    pthread_mutex_lock(&mutex);
    if(global_port > end_port) {
      pthread_mutex_unlock(&mutex);
      break;
    }
    port_to_scan = global_port;
    global_port++;
    pthread_mutex_unlock(&mutex);

    scan_port(&hints, tartget_ip, port_to_scan);
  }
  return NULL;
}