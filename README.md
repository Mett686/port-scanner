# Port Scanner

A lightweight, fast network port scanner written in C. This tool probes target IP addresses or hostnames to identify open, closed, or filtered ports.

## Features
- Scans single ports or custom port ranges
- Low-level socket programming in C[cite: 1]
- Fast execution with configurable timeouts

## Build & Usage
```bash
# Compile
gcc -Wall -Wextra -O2 -o scanner main.c

# Run (adjust arguments to match your CLI flags)
./scanner <target-ip> <start-port> <end-port>
