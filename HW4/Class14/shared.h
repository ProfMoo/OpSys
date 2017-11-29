/* shared.h */

/* This example shows the use of shared memory amongst two processes */

#define BUFFER_SIZE 8

/* This constant will define the shared memory segment such that
    multiple processes can attach to this segment */
#define SHM_SHARED_KEY 5678

struct shared_data
{
  int buffer[BUFFER_SIZE];  /* circular array */
  int in;                   /* index of next available producer array slot */
  int out;                  /* index of next array slot to consume from */
  int count;                /* number of elements in the buffer */
};
