#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

int scan_port(struct addrinfo* hints, char* target_ip, int target_port);

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("Invalid usage!\nport_scan <IP> <Port_low> <Port_high>\n");
    exit(1);
  }
  
  int start_port = atoi(argv[2]);
  int end_port = atoi(argv[3]);

  struct addrinfo hints;
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  
  for(int port = start_port; port<=end_port; port++) {
    (void)scan_port(&hints, argv[1], port);
  }

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