#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 2 //スレッド数
#define MAXSIZE 700000 //要素数
int x[MAXSIZE];
int y[MAXSIZE];
int n = MAXSIZE-1;
int j = 0, k = 0,l = 0,m = 0;
int changeCounter = 0;
pthread_mutex_t mutex, mutex_start;
pthread_cond_t cond_start;
void thread(void);
clock_t start, end;

int main(void){
  int i;
  char fn[255];
  FILE *fp;
  pthread_t thread_id[MAX_THREADS];
  void *thread_return;

  strcpy(fn, getenv("PWD"));
  strcat(fn, "/data.txt");
  if( NULL == (fp = fopen(fn, "r")) ) {
          printf("Can't open the input file: %s\n\n", fn);
          exit(1);
  }

  for(i=0;i<MAXSIZE;i++){
          fscanf(fp,"%d",&x[i]);
  }
  /*for (i = 0; i < MAXSIZE; i++) {
    printf("%d",x[i]);
  }*/


  printf("program(%d) start\n", MAXSIZE);
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&mutex_start, NULL);
  pthread_cond_init(&cond_start, NULL);
  /* create new threads */
  for(i = 0; i < MAX_THREADS; i++){
      if(pthread_create(&thread_id[i], NULL, (void *)thread, NULL) < 0){
          perror("pthread_create error");
          exit(1);
      }
  }
  sleep(1); //一秒待つ
  pthread_cond_broadcast(&cond_start);
  start = clock();
  /* wait for the end of threads */
  for(i = 0; i < MAX_THREADS; i++){
      if(pthread_join(thread_id[i], &thread_return) < 0){
          perror("pthread_join error");
          exit(1);
      }
  }
  end = clock();
  printf("x=[%d]\n",x[0]);
  printf("処理時間:%f[秒]\n", (double)(end - start) / CLOCKS_PER_SEC);
  return 0;
}

void thread(void){

    printf("thread[%p] start\n", pthread_self());
    pthread_mutex_lock(&mutex_start);
    pthread_cond_wait(&cond_start, &mutex_start);
    pthread_mutex_unlock(&mutex_start);

    for(k=MAXSIZE-1;k>0;k--){ //行数分回る
      pthread_mutex_lock(&mutex); //排他処理
      for(j=0;j<n;j++){  //1行の計算
        x[j]=(x[j]+x[j+1])%10;
        //printf("%d",x[j]);
      }
      n--;//kをロックしてないのでそれを利用してfor文を回すとおかしくなるので別途用意してそれを利用する
      pthread_mutex_unlock(&mutex);
    }
    printf("thread[%p] end\n", pthread_self());
    pthread_exit(0);
}
