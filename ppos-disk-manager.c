#include <signal.h>
#include <errno.h>
#include <string.h>
#include "ppos.h"
#include "ppos-core-globals.h"
#include "disk-driver.h"
#include "ppos-disk-manager.h"

// adicione todas as variaveis globais necessarias para implementar o gerenciado do disco



void bodyDiskManager(void* arg);
void diskSignalHandler();

// função para o tratamento de erros dos sinais - usada em disk_mgr_init()
void 
clean_exit_on_sig(int sig_num)
{
    printf ("\n ERROR[Signal = %d]: %d \"%s\"", sig_num, errno, strerror(errno));
    exit(errno);
}

disk_t disk;    // Global disk strcuture

int disk_mgr_init (int *numBlocks, int *blockSize) {

    // coloque o codigo para inicializar o disco aqui

    disk.num_blocks = *numBlocks;
    disk.block_size = *blockSize;
    disk.head_position = 0;         // Starts at 0
    disk.busy = 0;                  // Starts idle 
    disk.queue = NULL;
    sem_init(&disk.sem,1);



    // o seu codigo deve terminar ate aqui. 
    // As proximas linhas dessa função não devem ser modificadas
    signal(SIGSEGV, clean_exit_on_sig);

    return 0;
}

int disk_block_read(int block, void* buffer) {
    

    return 0;
}

int disk_block_write(int block, void* buffer) {
    

    return 0;
}


// Essa função implemeneta o escalonador de requisicoes de 
// leitura/scrita do disco usado pelo gerenciador do disco
// A função implementa a política FCFS.
diskrequest_t* disk_scheduler(diskrequest_t* queue) {
     
     if (!disk.queue) //No pending requests
        return NULL;

    //  -------------------------   FCFS scheduler   -------------------------
    return disk.queue;

    //  -------------------------   SSTF scheduler  -------------------------
   /* diskrequest_t *req = disk.queue, *best = req;
    int min = abs(disk.head_position - req->block);

    diskrequest_t *r;
    for (r = req->next; r != req; r = r->next)
    {
        int seek = abs(disk.head_position - r->block);
        if (seek < min)
        {
            min = seek;
            best = r;
        }
    }
    return best; */

    //  -------------------------   CSCAN schedyler -------------------------
    /*
    diskrequest_t *req = disk.queue;
    diskrequest_t *best = NULL;
    int min_seek = __INT_MAX__, min_block = __INT_MAX__;
    diskrequest_t *min_req = NULL;

    //Finding the closest higher block and the lowest

    diskrequest_t *r;
    for (r = req->next; r != req; r = r->next)
    {
        if (r->block >= disk.head_position)
        {
            int seek = r->block - disk.head_position;
            if (seek < min_seek)
            {
                min_seek = seek;
                best = r;
            }
            
        }

        //Finding smallest block for wrap around
        if (r->block < min_block)
        {
            min_block = r->block;
            min_req = r;
        }
    }

    //IF no higher block is found, wrap around to the lowest block request
    // return best ? best : min_req; 
    if (best)
        return best;
    else 
        return min_req;
    */
    
}