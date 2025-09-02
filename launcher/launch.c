#include "loader.h"

int main(int argc, char** argv) {
  if(argc != 2) {
    printf("Usage: %s <ELF Executable>\n", argv[0]);
    exit(1);
  }
  load_and_run_elf(&argv[1]);
  loader_cleanup();
  return 0;
}
