/* consumer.c */
/* consumer.c */

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
  int shmid = shmget( key, sizeof( struct shared_data ), 0660 );

  if ( shmid == -1 )
  {
    perror( "shmget() failed" );
    return EXIT_FAILURE;
  }

  printf( "CONSUMER: found shared memory segment with id %d\n", shmid );

  struct shared_data * shm;
  shm = shmat( shmid, NULL, 0 );   /* attach to the shared memory segment */
  if ( shm == (struct shared_data *)-1 )
  {
    perror( "shmat() failed" );
    return EXIT_FAILURE;
  }


  while ( 1 )
  {
    while ( shm->count == 0 )
    {
#if 1
      printf( "CONSUMER: buffer empty......\n" );
      sleep( 1 );
#endif
    }

/************* CRITICAL SECTION ************************************/
   /* note that the critical section also includes the end of the
        above while loop */
    int next_consumed = shm->buffer[ shm->out ];
    shm->out = ( shm->out + 1 ) % BUFFER_SIZE;   /* circular array */
    shm->count--;
/************* CRITICAL SECTION ************************************/

    printf( "CONSUMER: consumed %d\n", next_consumed );

    sleep( 1 );
  }




  /* detach from the shared memory segment */
  int rc = shmdt( shm );

  if ( rc == -1 )
  {
    perror( "shmdt() failed" );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
