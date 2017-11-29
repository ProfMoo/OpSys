/* producer.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shared.h"

int main()
{
  key_t key = SHM_SHARED_KEY;
  int shmid = shmget( key, sizeof( struct shared_data ),
                      IPC_CREAT | IPC_EXCL | 0660 );

  printf( "shmget() returned %d\n", shmid );

  if ( shmid == -1 )
  {
    perror( "shmget() failed" );
    return EXIT_FAILURE;
  }

  
  struct shared_data * shm;
  shm = shmat( shmid, NULL, 0 );
  if ( shm == (struct shared_data *)-1 )
  {
    perror( "shmat() failed" );
    return EXIT_FAILURE;
  }


  int item = 1;
  shm->in = shm->out = shm->count = 0;

  while ( item < 200 )
  {
    while ( shm->count == BUFFER_SIZE )
    {
      /* busy wait */
#if 1
      printf( "PRODUCER: buffer full......\n" );
      sleep( 1 );
#endif
    }

    printf( "PRODUCER: producing item %d\n", item );
    shm->buffer[ shm->in ] = item;
    item++;
    shm->in = ( shm->in + 1 ) % BUFFER_SIZE;    /* circular array */
    shm->count++;

    sleep( 1 );
  }


  /* detach from the shared memory segment */
  int rc = shmdt( shm );

  if ( rc == -1 )
  {
    perror( "shmdt() failed" );
    return EXIT_FAILURE;
  }


  /* remove the shared memory segment */
  if ( shmctl( shmid, IPC_RMID, 0 ) == -1 )
  {
    perror( "shmctl() failed" );
    return EXIT_FAILURE;
  }


  return EXIT_SUCCESS;
}
