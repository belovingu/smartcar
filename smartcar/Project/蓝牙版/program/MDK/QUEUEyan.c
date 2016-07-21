
#include "QUEUEyan.h"
/* ==========================================================
*     ������Ա��yan
*     ��дʱ�䣺2016/5/28
*     �������ƣ�InitQueue(SqQueue *Q)
*     ����˵����SqQueue *Q ��Ҫ��յĶ���
*     ����˵������ն���

*/

int16_t InitQueue(SqQueue *Q)
{
  Q->front = 0;
  Q->rear  = 0;
  
  return CH_OK;
}
/* ==========================================================
*     ������Ա��yan
*     ��дʱ�䣺2016/5/28
*     �������ƣ�int16_t QueueLength(SqQueue Q)
*     ����˵����
*     ����˵������ȡ���г���

*/
int16_t QueueLength(SqQueue Q)
{
  return (Q.rear - Q.front+MAXSIZEQUE)%MAXSIZEQUE;
}

/* ==========================================================
*     ������Ա��yan
*     ��дʱ�䣺2016/5/28
*     �������ƣ�int16_t EnQueue(SqQueue *Q,QElemType e)
*     ����˵����1 ������
*               2 �������
*     ����˵�������Ԫ��

*/
int16_t EnQueue(SqQueue *Q,QElemTypeint e)
{
  if ((Q->rear+1)%MAXSIZEQUE == Q->front)
    return CH_ERR;
  
  Q->data[Q->rear] = e;
  Q->rear = (Q->rear + 1)%MAXSIZEQUE;
  
  return CH_OK;

}

/* ==========================================================
*     ������Ա��yan
*     ��дʱ�䣺2016/5/28
*     �������ƣ�int16_t DeQueue(SqQueue *Q,QElemTypeint *e)
*     ����˵����1 ������
*               2 ����ɾ��������
*     ����˵������������ɾ�������ض�����

*/
int16_t DeQueue(SqQueue *Q)
{
  if(Q->front == Q->rear)
    return CH_ERR;
  
//  *e=Q->data[Q->front];
  Q->front = (Q->front + 1)%MAXSIZEQUE;
  
  return CH_OK;

}


