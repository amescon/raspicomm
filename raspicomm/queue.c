#include "queue.h"

#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int isQueueEmpty(struct queue *p)
{
	if(p->front ==p->rear)
		return 1;
	else
		return 0;
}

/* returns 0 if successful, otherwise -1 */
int enqueue(struct queue *p, int v)
{
	int t;
	t = (p->rear+1)%MAX;
	if(t == p->front)
  {
		perror("\nQueue Overflow\n");
    return -1;
  } 
	else
	{
		p->rear=t;
		p->arr[p->rear]=v;
    return 0;
	}
}

int dequeue(struct queue *p)
{
	if(isQueueEmpty(p))
	{
		perror("\nQueue Underflow");
		exit(0);
	}
	else
	{
		p->front=(p->front + 1)%MAX;
		return(p->arr[p->front]);
	}
}
