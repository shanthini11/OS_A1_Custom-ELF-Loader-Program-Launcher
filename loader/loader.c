#include "loader.h"

Elf32_Ehdr *ehdr = NULL;       // pointer to ELF header
Elf32_Phdr *phdr = NULL;       // pointer to Program header
int fd = -1;                   // file descriptor of ELF

void loader_cleanup() { //loader cleanup function is created here to empty memory in ehdr and phdr;
    if (ehdr) free(ehdr);
    if (phdr) free(phdr);
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
    ehdr = NULL;
    phdr = NULL;
}

int validate_elf_magic() { //checks the byte code of file and sees if it matches woth elf magic number; 1 if it is a valid elf file, 0 if not a valid file
  int flag;
  if(ehdr->e_ident[EI_MAG0] == ELFMAG0 && ehdr->e_ident[EI_MAG1] == ELFMAG1 && ehdr->e_ident[EI_MAG2] == ELFMAG2 && ehdr->e_ident[EI_MAG3] == ELFMAG3){
    flag=1;
  }
  if (flag==0){
    return 0;
  }

  else{
    return 1;
  } // flag system created to throw the final verdict after comparing file with magic number of elf
}

int check_lseek_validity(int offset) { //this function helps the file descriptor to go to a given offset in either ehdr or phdr and read a section/segment from it
    int flag = lseek(fd, offset, SEEK_SET);
    if (flag == -1) {
        printf("lseek failed\n");
        return 0;  
    }

    else{
        
        return 1; //flag system to give final verdict as sucessful moving of pointer or failure
    }      
}

//this fucntion is now the main loader: 

void load_and_run_elf(char **argv) { // first step is opening the ELF file
    char *filename = argv[0];
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Error: cannot open file %s\n", filename);
        exit(1);
    }
    else{
      printf("File Descriptor: %d\n\n", fd); //if it has sucess in opening the file, it will print the pointer(FD) otherwise break and exit
    }

    ehdr = malloc(sizeof(Elf32_Ehdr)); //next step in loader is reading the elf header
    if (ehdr==NULL) {
        printf("Failed to allocate memory for ELF header\n");
        exit(1);
    }
    if (read(fd, ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
        printf("Failed to read ELF header\n");
        exit(1); //these preserve space in elf header and read the header from memory here; if it is unable to read from memory, it will parse error handling accordingly
    }


    if (validate_elf_magic()==0) { //here it validates if the elf file is valid, otherwise will throw an error and error handle
        printf("Not a valid ELF file\n");
        exit(1);
    }
    if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
        printf("Not a 32-bit ELF file\n");
        exit(1);
    }

    size_t ph_size = ehdr->e_phentsize * ehdr->e_phnum; //similarly, it reads the phdr header
    phdr = malloc(ph_size);
    if (phdr==NULL) {
        printf("Failed to allocate memory for PHDR table\n");
        exit(1);
    }
    if (check_lseek_validity(ehdr->e_phoff)==0) {
        loader_cleanup();
        exit(1);
    }
    if (read(fd, phdr, ph_size) != (ssize_t)ph_size) { //these preserve space in phdr header and read the header from memory here; if it is unable to read from memory, it will parse error handling accordingly
        printf("Error reading program headers\n");
        exit(1);
    }

    //next step is to load the segments
    void *entry_addr = NULL; //this helps keep track of the address that is currently running(entry point address)
    
    for (int i = 0; i < ehdr->e_phnum; i++){ //this goes through all phdr and processes the segments which can be loaded
        if (phdr[i].p_type == PT_LOAD){
        void *segment = mmap(NULL,phdr[i].p_memsz,PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0); //mmap is being used here to make memory for segments; it creates diff permissions that are allowed to execute within
        //map private and annoymous: creates a private mapping that is not shared; and doesnt map from a file, it reads data into it manually
        if (segment == MAP_FAILED) {
            perror("mmap failed");  //if mmap fails
            loader_cleanup();
            exit(1);
        // if PT_LOAD, then it uses mmap to allocate memory and moves pointer to p_offset it then reads the file bytes and fills the rest accordingly using .BSS
        //It also configures file handling if the mmap defination fails and exits from function
        }

        if (check_lseek_validity(phdr[i].p_offset)==0) { //segment data being read from the correct start point: p_offset 
            loader_cleanup();
            exit(1);
        }
        if (read(fd, segment, phdr[i].p_filesz) != (ssize_t)phdr[i].p_filesz) { //this reads the bytes from file and sees if the the bytes are lesser than p_memsz
            //gives error if theres error in loading this memory
            printf("Error reading segment %d\n", i);
            loader_cleanup();
            exit(1);
        }

        if (phdr[i].p_memsz > phdr[i].p_filesz) { //creating more memory for some segments than what the file can provide by adding more zeros according to file specs
            memset((char*)segment + phdr[i].p_filesz, 0,
                   phdr[i].p_memsz - phdr[i].p_filesz);
        }

        
        if (ehdr->e_entry >= phdr[i].p_vaddr && //this helps check the entry point if it lies in current segment or not (computing using formula of segment+offset=address that it is currently at)
            ehdr->e_entry < phdr[i].p_vaddr + phdr[i].p_memsz) {
            size_t offset = ehdr->e_entry - phdr[i].p_vaddr;
            entry_addr = (char*)segment + offset;
        }
    }
    else{
      continue;
    }
  }

// final step in loader: jumping tp the entry point once it finds; if no point found, error is thrown using error handling
//it captures the final return value and prints to user interface and also cleans the memory before exiting program
    if (entry_addr==NULL) {
        printf("Entrypoint not found in any PT_LOAD segment\n");
        loader_cleanup();
        exit(1);
    }

    int (*entry)() = (int (*)())entry_addr;
    int result = entry();
    printf("Fibonnaci Number: %d\n", result);

    loader_cleanup();
}