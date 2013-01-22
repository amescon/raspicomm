
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX 256

struct queue
{
	int arr[MAX];
	int rear,front;
};

int isQueueEmpty(struct queue *p);

/* returns 0 if successful, otherwise -1 */
int enqueue(struct queue *p, int v);

int dequeue(struct queue *p);

#ifdef __cplusplus
}
#endif