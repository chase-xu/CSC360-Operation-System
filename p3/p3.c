/*CSC360 P3, Chase Xu, V00924503, Nov.30.2019*/
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include <math.h>

/*From tutorial*/
// Super block
struct __attribute__((__packed__)) superblock_t {
 uint8_t fs_id [8];
 uint16_t block_size;
 uint32_t file_system_block_count;
 uint32_t fat_start_block;
 uint32_t fat_block_count;
 uint32_t root_dir_start_block;
 uint32_t root_dir_block_count;
};

struct __attribute__((__packed__)) dir_entry_timedate_t {
 uint16_t year;
 uint8_t month;
 uint8_t day;
 uint8_t hour;
 uint8_t minute;
 uint8_t second;
};

struct __attribute__((__packed__)) dir_entry_t {
 uint8_t status;
 uint32_t starting_block;
 uint32_t block_count;
 uint32_t size;
 struct dir_entry_timedate_t create_time;
 struct dir_entry_timedate_t modify_time;
 uint8_t filename[31];
 uint8_t unused[6];
};


void diskinfo(int argc, char* argv[]){
    // if(argc != 2){
    //     perror("wrong number of input parameters");
    //     exit(1);
    // }
    char* image = argv[1];
    /*open file*/
    int fd = open(image, O_RDWR);


    /*from lab 10*/
    struct stat buf;
    if(fstat(fd, &buf) < 0){
        perror("fs error");
        exit(1);
    }
    void* address = mmap(NULL,buf.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    struct superblock_t*  sb = (struct superblock_t*) address;
    /*printing the superblock infomation*/
    printf("Super block information:\n");
    printf("Block size: %d\n", htons(sb->block_size));
    printf("Block count: %d\n", htonl(sb->file_system_block_count));
    printf("FAT starts: %d\n", htonl(sb->fat_start_block));
    printf("FAT blocks: %d\n", htonl(sb->fat_block_count));
    printf("Root directory starts: %d\n", htonl(sb->root_dir_start_block));
    printf("Root directory blocks: %d\n\n", htonl(sb->root_dir_block_count));

    /*find range of the fat blocks*/
    int beg = htonl(sb->fat_start_block) * htons(sb->block_size);
    int end = htonl(sb->fat_block_count) * htons(sb->block_size) + beg;
    /*retreiving FAT*/
    int allocated = 0;
    int free = 0;
    int reserved = 0;
    
    /*start looping*/
    int i = beg;
    while(i < end){
        int num = 0;
        /*get the 4bytes num*/
        memcpy(&num, address + i, 4);
        num = htonl(num);
        if(num == 1){
            reserved++;
        }
        else if(num !=1 && num != 0){
            allocated++;
        }
        else{
            free++;
        }
        i+=4;
    }

    printf("FAT information:\n");
    printf("Free Block: %d\n", free);
    printf("Reserved Block: %d\n", reserved);
    printf("Allocated Blocks: %d\n", allocated);

    close(fd);
    // f.close(fd);
}

void disklist(int argc, char* argv[]){
    // if(argc != 2){
    //     perror("wrong number of input parameters");
    //     exit(1);
    // }
    // printf("In Disklist\n");
    char* image = argv[1];
    /*open file*/
    int fd = open(image, O_RDWR);
    /*from lab 10*/
    struct stat buf;
    if(fstat(fd, &buf) < 0){
        perror("fs error");
        exit(1);
    }
    void* address = mmap(NULL,buf.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    struct superblock_t*  sb = (struct superblock_t*) address;
    int start = htonl(sb->root_dir_start_block) * htons(sb->block_size);
    int count = (int) htonl(sb->root_dir_block_count);

    count = count * 512 / 64;
    for(int i = 0;i < count; i ++){
        struct dir_entry_t* dir = (struct dir_entry_t*) (address+start+i*64);
        int type = 0;
        type = dir-> status;
        // char* fl = malloc
        if(type == 3){
            printf("F ");
            printf("%10d ", htonl(dir-> size));
            printf("%30s ", (dir-> filename));
            printf("%4d/%02d/%02d %02d:%02d:%02d\n",htons(dir->modify_time.year), dir->modify_time.month,dir->modify_time.day,
            dir->modify_time.hour, dir->modify_time.minute, dir->modify_time.second);
        }
        else if(type == 5){
            printf("D ");
            printf("%10d ", htonl(dir-> size));
            printf("%30s ", (dir-> filename));
            printf("%4d/%02d/%02d %02d:%02d:%02d\n",htons(dir->modify_time.year), dir->modify_time.month,dir->modify_time.day,
            dir->modify_time.hour, dir->modify_time.minute, dir->modify_time.second);
        }
    }
    // fd.close();    fclose(f);
    close(fd);
}



void diskget(int argc, char*argv[]){
     if(argc != 4){
        perror("Number of arguments not valid!");
        exit(0);
    }
    char* file;
    // printf("In diskget\n");
    char* image = argv[1];
    file = argv[2];
    char* file_= argv[3];
    /*open file*/
    int fd = open(image, O_RDWR);
    FILE* f = fopen(file_, "wb");

    /*Need to tokenize dirctory*/
    // strtok(file,"\n");
    char* tok = strtok(file, "/");
    // printf("%s \n",tok);
    int k = 0;
	while(tok != NULL){
        // printf("%s ",tok);
        file = tok;
		tok = strtok(NULL, "/");
        k++;
	}
    // file[k++] = NULL;
    // printf("copy to file %s\n", file_);
   
    // // printf("\n");
    // printf("directory file %s\n",file);
    /*from lab 10*/
    struct stat buf;
    if(fstat(fd, &buf) < 0){
        perror("fs error");
        exit(0);
    }
    void* address = mmap(NULL,buf.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    struct superblock_t*  sb = (struct superblock_t*) address;
    int start = htonl(sb->root_dir_start_block) * htons(sb->block_size);

    struct dir_entry_t* dir;
    int found = 0;
    int i = 0;
    while(1){
        dir = (struct dir_entry_t*) (address+start+i*64);
        int type = 0;
        type = dir-> status;
        char* name = dir->filename;
        if(type == 3){
            if(strcmp(file, name) == 0){
                // printf("%s \n", (dir-> filename));
                found = 1;
                break;
            } 
        }
        i++;
    }
    if(found == 0){
        printf("File not found.\n");
        exit(0);
    }
    // printf("Found file %s\n",dir->filename);
    /*follow info of directory and find content of file*/
    int block_size = htons(sb->block_size);
    int fat_start = htonl(sb->fat_start_block)* block_size;
    int content = htonl(dir->starting_block) * htons(sb->block_size);
    
    /*where are the next content is*/
    int next = htonl(dir->starting_block);
    if(fat_start == 0xffffffff){
        perror("fffff");
        exit(0);
    }
    // printf("Pass content\n");
    int file_size = htonl(dir->size);

    // printf("Pass file size\n");
    int num_count = htonl(dir->block_count);

    // printf("pase num count\n");
    int total_block_size = num_count * block_size;

    // printf("Pass total block size.\n");
    // printf(htonl(dir->size));

    int stop = num_count;
    // printf("pass stop\n");

    if(total_block_size < file_size){
        perror("file size is smaller than block size!!!!\n");
        exit(0);
    }
    // printf("stop is %d\n", stop);
    // printf("Next is %d\n", next);
    // printf("FAT start is %d\n", htonl(sb->fat_start_block));


    int next_;
    while(stop != 0){
        if(total_block_size > file_size){
            if(stop > 1){
                fwrite(address+content, 1 , block_size, f);
                    /*find where the next block is*/
                    next_ = next;
                    memcpy(&next_, address+fat_start + next*4, 4);
                    // printf("Next is %d\n", htonl(next));
                    next = htonl(next_);
                    if(next == 0xffffffff) break;
                    // printf("Next is %d\n", (next));
            }
            else if(stop == 1){
                int remainder = file_size - (num_count-1) * htons(sb->block_size);
                fwrite(address+content, 1, remainder, f);
            }   
        }
        else if(total_block_size == file_size){
            fwrite(address+content, 1 , block_size, f);
        }
        content = next * block_size;
        stop--;
    }
    fclose(f);
    close(fd);
    // printf("reach end\n");
}

void diskput(int argc, char* argv[]){
    if(argc != 4){
        perror("Number of arguments not valid!");
        exit(0);
    }
    char* image = argv[1];
    char* file_ = argv[2];
    char* file = argv[3];
    /*open file*/
    int fd = open(image, O_RDWR);
    int f_ = open(file_, O_RDONLY);
    struct stat h;
    if(fstat(f_, &h) < 0){
        // printf("%d not found.\n", f_);
        exit(0);
    }
    int file_size = htonl(h.st_size);
    close(f_);

    FILE* f;
    f = fopen(file_, "rb");
    if(f == NULL){
        printf("%s not found.\n", file_);
        exit(0);
    }

    
    // printf("file size is %d\n",file_size);

    char* tok = strtok(file, "/");
    // printf("%s \n",tok);
    int k = 0;
	while(tok != NULL){
        // printf("%s ",tok);
        file = tok;
		tok = strtok(NULL, "/");
        k++;
	}
    // printf("file legth is %ld\n", strlen(file));
    /*from lab 10*/
    struct stat buf;
    if(fstat(fd, &buf) < 0){
        perror("fs error");
        exit(0);
    }

    
    void* address = mmap(NULL, buf.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    struct superblock_t*  sb = (struct superblock_t*) address;
    int start = htonl(sb->root_dir_start_block) * htons(sb->block_size);
    int count = htonl(sb->root_dir_block_count);

    /*create file dirctory*/
    struct dir_entry_t* dir;
    int found = 0;
    /*number of space of direct entries*/
    count = count * htonl(sb->block_size) / 64;

    int block_size = htons(sb->block_size);
    int fat_start = htonl(sb->fat_start_block)* block_size;
    int fat_blocks = htonl(sb->fat_block_count);

    /*Find empty dir*/
    if(found == 0){

        for(int i = 0; i< count; i++){
            /*get directory*/
            dir = (struct dir_entry_t*) (address+start+i*64);
            int bit;
            bit = dir->status;
            // printf("%d\n", bit);
            /*check if the 0 bit is 0*/
            // bit = bit & 0b00000001; 
            // printf("%d\n", bit);
            if(bit == 0){
                /*update directory info*/
                memset(&(dir->status), 3, 1);
                // printf("status is %d\n",dir->status);
                memcpy(&dir->size, &file_size, 4);

                memset(&dir->filename, 0, 30);
                memcpy(&dir->filename, file, strlen(file));

                // printf("filename is %s\n", dir->filename);
                // printf("%d\n",htonl(file_size ));
                // printf("%d\n",block_size );
                float r =htonl(file_size )%(block_size);
                // printf("r is %f\n",r);
                int c;
                if(r == 0) c = r;
                else c = ntohl(htonl(file_size )/(block_size)+1);
                // printf("c is %d\n",c);
                // printf("block count is %d\n", c);
                memcpy(&dir->block_count, &c, 4); 
                // printf("block count is %d\n", htonl(dir->block_count));
                // printf("Found an empty entry block\n");
                break;
            }
        }
    }


        /*start find empty block in FAT*/

    // int block_size = htons(sb->block_size);
    // int fat_start = htonl(sb->fat_start_block)* block_size;
    // int fat_blocks = htonl(sb->fat_block_count);

        int if_first = 1;
        int block_count = htonl(dir->block_count);
        int blocks = block_count;
        int last_block;

        //     int ptr;
        //     memcpy(&ptr, address+fat_start+1*4, 4);
        //     printf("ptr is %d ", htonl(ptr));
        // printf("About start %d\n", times);
        for(int i = 0; i < fat_blocks*block_size/4;i++){
            int ptr;
           
            memcpy(&ptr, address+fat_start+i*4, 4);
            /*find empty block in FAT*/
            if(htonl(ptr) == 0){
                // printf("ptr is%d\n",htonl(ptr));
                blocks--;
                char* text = malloc(sizeof(char) * block_size);
                int k = htonl(i);
                /*it is first block*/
                if(if_first == 1){
                    /*update dir*/
                    
                    memcpy(&dir->starting_block, &k, 4);
                    // printf("dir start is %d\n", ntohl(dir->starting_block));
                    /*if only used one block*/
                    if(block_count == 1){

                        /*update fat*/
                        memset((address+fat_start+i*4), 0xffffffff, 4);
                        /*copy file*/
                        fread(text, file_size, 1, f);
                        // printf("text is%s\n", text);
                        memcpy((address+i*block_size), text, file_size);
                        break;
                    }
                    else{
                         /*update fat*/
                        // memcpy(&(address+fat_start+i*4), , 4);
                        /*copy file*/
                       
                        fread(text, block_size, 1, f);

                        memcpy((address+i*block_size), text, block_size);
                        
                    }
                    if_first = 0;
                    last_block = i;
                }
                else{
                    if(blocks != 0){
                       
                        /*update fat*/
                        memcpy((address+fat_start+last_block*4), &k, 4);
                        /*copy file*/
                        
                        fread(text, file_size, 1, f);
                        memcpy((address+i*block_size), text, block_size);
                    }
                    else{
                        /*find remainder*/
                        int remainder = ntohl(file_size) % block_size ;
     
                        /*update fat*/
                        memcpy((address+fat_start+last_block*4), &k, 4);
                        memset((address+fat_start+i*4), 0xffffffff, 4);
                        /*copy file*/
                        fread(text, remainder, 1, f);
                 
                        memcpy((address+i*block_size), text, remainder);
                        break;
                    }
                    last_block = i;

                }
                
            }
            
        }


    fclose(f);
    close(fd);


}

int main(int argc, char *argv[]) {

    // diskinfo(argc, argv);
    // disklist(argc, argv);
    // diskget(argc, argv);
    // if(argc == 4)diskput(argc, argv);
    // diskput(argc, argv);
  #if defined(PART1)
      diskinfo(argc, argv);
  #elif defined(PART2)
      disklist(argc, argv);
  #elif defined(PART3)
      diskget(argc, argv);
  #elif defined(PART4)
      diskput(argc,argv);
  #else
  # error "PART[1234] must be defined"
  #endif

  return 0;
}