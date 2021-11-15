
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<wait.h>
#include<sys/types.h>
#include<math.h>
#include<stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

pthread_cond_t cond1=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3=PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock1=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3=PTHREAD_MUTEX_INITIALIZER;

bool finished_1=false;
bool finished_2=false;
bool finished_3=false;

int to_do=1;

int run1=0;
int run2=0;
int run3=0;
double t1,t2,t3 = 0;
time_t e1,s2,e2,s3,e3;
static time_t s1;

int shmid;
int* shmPtr;

typedef struct thread_data {
   int n;
   long long int result;
   bool finished;
   double waiting_time;
   time_t start_time;
}thread_data;

void *process_c2(void *data){
    thread_data *tdata=(thread_data*)data;
    tdata->start_time=time(0);
    FILE *fp;
    fp=fopen("Number.txt","r");
    int num;
    int n=tdata->n;
    double start,end;
    double waiting_time=0;
    //printf("run2 is %d\n",run2);
    for(int i=0;i<n;i++){
        pthread_mutex_lock(&lock2);
        while(!run2) { /* We're paused */
            start=time(0);
            printf("paused,waiting,c2\n");
            pthread_cond_wait(&cond2, &lock2); /* Wait for signal */
            end=time(0);
            waiting_time=waiting_time+difftime(end,start)*1000;
        }
        pthread_mutex_unlock(&lock2);
        fscanf(fp,"%d\n",&num);
        printf("number is : %d\n",num);
    }
    tdata->finished=true;
    tdata->waiting_time=waiting_time;
    pthread_exit(NULL);
}

void *process_c1(void *data){
    thread_data *tdata=(thread_data*)data;
    tdata->start_time=time(0);
    long long int sum=0;
    // int* n1_ptr=(int*)n1;
    int n=tdata->n;
    double start,end;
    double waiting_time=0;
    // printf("run1 is %d\n",run1);
    for(int j=1;j<=n;j++){
        pthread_mutex_lock(&lock1);
        while(!run1) { /* We're paused */
            start=time(0);
            printf("paused,waiting,c1\n");
            pthread_cond_wait(&cond1, &lock1); /* Wait for signal */
            end=time(0);
            waiting_time=waiting_time+difftime(end,start)*1000;
        }
        pthread_mutex_unlock(&lock1);
        sum+=j;
    }
    // printf("sum in func is %d\n",sum);
    // pthread_exit((void*)&sum);
    tdata->result=sum;
    tdata->finished=true;
    tdata->waiting_time=waiting_time;
    pthread_exit(NULL);
}

void *process_c3(void *data){
    // while(pthread_cond_wait(&cond3, &lock3))
    thread_data *tdata=(thread_data*)data;
    tdata->start_time=time(0);

    FILE *fp;
    fp=fopen("Number.txt","r");
    int num;
    int n=tdata->n;
    long long int sum=0;
    double start,end;
    double waiting_time=0;
    for(int i=0;i<n;i++){
        pthread_mutex_lock(&lock3);
        while(!run3) { /* We're paused */
            start=time(0);
            printf("paused,waiting,c3\n");
            pthread_cond_wait(&cond3, &lock3); /* Wait for signal */
            end=time(0);
            waiting_time=waiting_time+difftime(end,start)*1000;
        }
        pthread_mutex_unlock(&lock3);
        fscanf(fp,"%d\n",&num);
        // num=i;
        sum=sum+num;
    }

    // int i=0;

    // for()

    // printf("sum in func c3 is %d\n",sum);
    tdata->result=sum;
    tdata->finished=true;
    tdata->waiting_time=waiting_time;
    pthread_exit(NULL);
}

int main()
{
    int c1_pid,c2_pid,c3_pid;
    int n1,n2,n3,time_quantum;
    scanf("%d %d %d",&n1,&n2,&n3);
    printf("Enter time quantum");
    scanf("%d",&time_quantum);
    int fds_1[2],fds_2[2],fds_3[3],fds_1_time[2],fds_2_time[2],fds_3_time[2],fds_1_wtime[2],fds_2_wtime[2],fds_3_wtime[2],fds_1_stime[2],fds_2_stime[2],fds_3_stime[2];
    pipe(fds_1);
    pipe(fds_1_time);
    pipe(fds_1_wtime);
    pipe(fds_1_stime);


    char buf_1[30],buf_3[30],buf_2[30],buf_3_2[30],buf_1_2[30],buf_2_2[30];

    shmid=shmget(2045,32,0666 | IPC_CREAT);
                shmPtr=shmat(shmid,0,0);
                *shmPtr=-1;

                *(shmPtr+1)=0;
                *(shmPtr+2)=0;
                *(shmPtr+3)=0;

    c1_pid=fork();
    if(c1_pid!=0){
        // wait(NULL);
        // close(fds_1[1]);
        // read(fds_1[0],buf_1,25);
        // printf("c1 sent %d to parent via pipe\n",atoi(buf_1));
        sleep(1);
        pipe(fds_2);
        pipe(fds_2_time);
        pipe(fds_2_wtime);
        pipe(fds_2_stime);

        c2_pid=fork();
        if(c2_pid!=0){
            // wait(NULL);
            // close(fds_2[1]);
            // read(fds_2[0],buf_2,14);
            // printf("%s\n",buf_2);
            sleep(1);
            pipe(fds_3);
            pipe(fds_3_time);
            pipe(fds_3_wtime);
            pipe(fds_3_stime);

            c3_pid=fork();
            if(c3_pid!=0){
                //parent or master process here
                // wait(NULL);

                //shared memory here
                // shmid=shmget(2045,32,0666 | IPC_CREAT);
                // shmPtr=shmat(shmid,0,0);
                // *shmPtr=-1;

                // *(shmPtr+1)=0;
                // *(shmPtr+2)=0;
                // *(shmPtr+3)=0;

                //scheduling here
                //int time_quantum=1;
                time_t start_of_all;
                sleep(5);
                start_of_all=time(0);

                printf("---------------STARTTING ALL PROCESSES-----------\n");
                *shmPtr=1;

                while((*(shmPtr+1)==0) || (*(shmPtr+2)==0) || (*(shmPtr+3)==0)){
                    if(to_do==1 && *(shmPtr+1)==0){
                        // pthread_cond_signal(&cond1);
                        printf("running c1\n");
                        sleep(time_quantum);
                    }
                    else if(to_do==2 && *(shmPtr+2)==0){
                        // pthread_cond_signal(&cond2);
                        printf("running c2\n");
                        sleep(time_quantum);
                    }
                    else if(to_do==3 && *(shmPtr+3)==0){
                        // pthread_cond_signal(&cond3);
                        printf("running c3\n");
                        sleep(time_quantum);
                    }
                //     if(*(shmPtr+1)==1){
                //         char* eptr;
                // close(fds_1[1]);
                // read(fds_1[0],buf_1,25);
                // printf("c1 sent %lld to parent via pipe\n",strtoll(buf_1,&eptr,10));

                // close(fds_1_time[1]);
                // read(fds_1_time[0],buf_1_2,25);
                // char* eptr3;
                // printf("time taken by c1 is %lf\n",strtod(buf_1_2,&eptr3));

                // close(fds_1_wtime[1]);
                // read(fds_1_wtime[0],buf_1_2,25);
                // // char* eptr3;
                // printf("waiting time of c1 is %lf\n",strtod(buf_1_2,&eptr3));
                //     *(shmPtr+1)=2;
                //     }

                    to_do=to_do+1;
                    if(to_do==4){
                        to_do=1;
                    }
                    *shmPtr=to_do;
                }



                printf("this is parent process\n");
                char* eptr;
                close(fds_1[1]);
                read(fds_1[0],buf_1,25);
                printf("c1 sent %lld to parent via pipe\n",strtoll(buf_1,&eptr,10));

                close(fds_1_time[1]);
                read(fds_1_time[0],buf_1_2,25);
                char* eptr3;
                // printf("time taken by c1 is %lld\n",strtoll(buf_1_2,&eptr3,10));
                printf("exact ta of c1 is %lf\n",difftime(strtoll(buf_1_2,&eptr3,10),start_of_all)*1000);
                close(fds_1_wtime[1]);
                read(fds_1_wtime[0],buf_1_2,25);
                // char* eptr3;
                printf("waiting time of c1 is %lf\n",strtod(buf_1_2,&eptr3));
                double wt=strtod(buf_1_2,&eptr3);

                close(fds_1_stime[1]);
                read(fds_1_stime[0],buf_1_2,25);
                long long int s_ll=strtoll(buf_1_2,&eptr3,10);
                // printf("s_ll in main is %lld\n",s_ll);
                // char* eptr3;
                printf("waiting time exact of c1 is %lf\n",wt-(difftime(start_of_all,s_ll)*1000));
                // printf("above wt is %lf\n",wt);
                // printf("remove %lf\n",difftime(start_of_all,s_ll)*1000);
                close(fds_2[1]);
                read(fds_2[0],buf_2,14);
                printf("%s\n",buf_2);

                close(fds_2_wtime[1]);
                read(fds_2_wtime[0],buf_2_2,25);
                char* eptr4;
                printf("waiting time of c2 is %lf\n",strtod(buf_2_2,&eptr4));

                wt=strtod(buf_2_2,&eptr4);

                close(fds_2_stime[1]);
                read(fds_2_stime[0],buf_2_2,25);
                s_ll=strtoll(buf_2_2,&eptr4,10);
                printf("waiting time exact of c2 is %lf\n",wt-(difftime(start_of_all,s_ll)*1000));

                close(fds_3[1]);
                read(fds_3[0],buf_3,25);
                // char* eptr;
                printf("c3 sent %lld to parent via pipe\n",strtoll(buf_3,&eptr,10));

                close(fds_3_time[1]);
                read(fds_3_time[0],buf_3_2,25);
                char* eptr2;
                printf("time taken by c3 is %lf\n",strtod(buf_3_2,&eptr2));

                close(fds_3_wtime[1]);
                read(fds_3_wtime[0],buf_3_2,25);
                // char* eptr2;
                printf("waiting time of c3 is %lf\n",strtod(buf_3_2,&eptr2));

                wt=strtod(buf_3_2,&eptr2);

                close(fds_3_stime[1]);
                read(fds_3_stime[0],buf_3_2,25);
                s_ll=strtoll(buf_3_2,&eptr2,10);
                printf("waiting time exact of c1 is %lf\n",wt-(difftime(start_of_all,s_ll)*1000));
                
                // printf("Time taken by C1 is %lf\n",t1);
                // printf("Time taken by C3 is %lf\n",t3);
            }
            else{
                //c3 process here
                // printf("enter n3: ");
                // scanf("%d",&n3);
                //n3=100000000;
                s3=time(0);
                printf("entered n3 is %d\n",n3);
                pthread_t thread_id_3;
                thread_data data;
                data.n=n3;
                data.finished=false;
                int rc=pthread_create(&thread_id_3,NULL,process_c3,(void*)&data);    
                if(rc){
                    printf("error %d in pthread_create()\n",rc);
			        exit(1);
                }

                //shared mem
                shmid=shmget(2045,32,0);
                shmPtr=shmat(shmid,0,0);

                while(!data.finished){
                    // pthread_cond_wait(&cond3, &lock3);
                    if((*shmPtr)==3){
                        pthread_mutex_lock(&lock3);
                        run3 = 1;
                        // printf("signalling c3\n");
                        pthread_cond_signal(&cond3);
                        pthread_mutex_unlock(&lock3);
                        // pthread_cond_signal(&cond3);
                        while((*shmPtr)==3){

                        }
                        // pthread_cond_wait(&cond3, &lock);
                        // run3=0;
                        pthread_mutex_lock(&lock3);
                        run3 = 0;
                        // printf("pause c3\n");
                        pthread_mutex_unlock(&lock3);
                    }
                }
                *(shmPtr+3)=1;
                // pthread_join(thread_id_3,NULL);
                printf("sum in c3 is %lld\n",data.result);
                char str[30];
                int len=snprintf(NULL,0,"%lld",data.result)+1;
                snprintf(str,len,"%lld",data.result);
                close(fds_3[0]);
                write(fds_3[1],str,len);
                e3=time(0);
                t3=difftime(e3,s3)*1000;
                // printf("Time taken for C3 process %lf\n",t3);
                char str2[25];
                int len2=snprintf(NULL,0,"%lf",t3)+1;
                snprintf(str2,len2,"%lf",t3);
                close(fds_3_time[0]);
                write(fds_3_time[1],str2,len2);

                char str3[25];
                int len3=snprintf(NULL,0,"%lf",data.waiting_time)+1;
                snprintf(str3,len3,"%lf",data.waiting_time);
                close(fds_3_wtime[0]);
                write(fds_3_wtime[1],str3,len3);

                char str4[25];
                int len4=snprintf(NULL,0,"%lld",(long long int)data.start_time)+1;
                snprintf(str4,len4,"%lld",(long long int)data.start_time);
                close(fds_3_stime[0]);
                write(fds_3_stime[1],str4,len4);
            }
        }
        else{
            //c2 process here
            //printf("enter n2: ");
            //scanf("%d",&n2);
            //n2=10000;
            s2=time(0);
            printf("entered n2 is %d\n",n2);
            pthread_t thread_id_2;
            thread_data data;
            data.n=n2;
            data.finished=false;
            sleep(3);
            int rc=pthread_create(&thread_id_2,NULL,process_c2,(void*)&data);    
            if(rc){
                printf("error %d in pthread_create()\n",rc);
			    exit(1);
            }
            //shared mem
            shmid=shmget(2045,32,0);
            shmPtr=shmat(shmid,0,0);

            while(!data.finished){
                // pthread_cond_wait(&cond3, &lock3);
                if((*shmPtr)==2){
                    pthread_mutex_lock(&lock2);
                    run2 = 1;
                    // printf("signalling c3\n");
                    pthread_cond_signal(&cond2);
                    pthread_mutex_unlock(&lock2);
                    // pthread_cond_signal(&cond3);
                    while((*shmPtr)==2){

                    }
                    // pthread_cond_wait(&cond3, &lock);
                    // run3=0;
                    pthread_mutex_lock(&lock2);
                    run2 = 0;
                    // printf("pause c3\n");
                    pthread_mutex_unlock(&lock2);
                }
            }
            *(shmPtr+2)=1;
            // pthread_join(thread_id_2,NULL);
            // printf("done printing\n");
            
            close(fds_2[0]);
            write(fds_2[1],"Done Printing",14);
            e2=time(0);
            t2=difftime(e2,s2)*1000;
            printf("Time taken for C2 process %lf\n",t2);
            char str2[25];
            int len2=snprintf(NULL,0,"%lf",t2)+1;
            snprintf(str2,len2,"%lf",t2);
            close(fds_2_time[0]);
            write(fds_2_time[1],str2,len2);

            char str3[25];
            int len3=snprintf(NULL,0,"%lf",data.waiting_time)+1;
            snprintf(str3,len3,"%lf",data.waiting_time);
            close(fds_2_wtime[0]);
            write(fds_2_wtime[1],str3,len3);

            char str4[25];
            int len4=snprintf(NULL,0,"%lld",(long long int)data.start_time)+1;
            snprintf(str4,len4,"%lld",(long long int)data.start_time);
            close(fds_2_stime[0]);
            write(fds_2_stime[1],str4,len4);
        }
    }
    else{
        //c1 process here
        // printf("enter n1: ");
        // scanf("%d",&n1);
        //n1=100000000;
        // s1=time(0);
        printf("entered n1 is %d\n",n1);
        pthread_t thread_id_1;
        thread_data data;
        data.n=n1;
        data.finished=false;
        int rc=pthread_create(&thread_id_1,NULL,process_c1,(void*)&data);    
        if(rc){
            printf("error %d in pthread_create()\n",rc);
		    exit(1);
        }
        //shared mem
        shmid=shmget(2045,32,0);
        shmPtr=shmat(shmid,0,0);

        s1=time(0);

        while(!data.finished){
            // pthread_cond_wait(&cond3, &lock3);
            if((*shmPtr)==1){
                pthread_mutex_lock(&lock1);
                printf("changing run1 to 1\n");
                run1 = 1;

                // s1=time(0);

                // printf("signalling c3\n");
                pthread_cond_signal(&cond1);
                pthread_mutex_unlock(&lock1);
                // pthread_cond_signal(&cond3);
                while((*shmPtr)==1){
                }
                // pthread_cond_wait(&cond3, &lock);
                // run3=0;
                pthread_mutex_lock(&lock1);
                run1 = 0;
                // printf("pause c3\n");
                pthread_mutex_unlock(&lock1);
            }
        }
        *(shmPtr+1)=1;
        // pthread_join(thread_id_1,NULL);
        printf("sum in c1 is %lld\n",data.result);
        char str[30];
        int len=snprintf(NULL,0,"%lld",data.result)+1;
        snprintf(str,len,"%lld",data.result);
        close(fds_1[0]);
        write(fds_1[1],str,len);
        e1=time(0);
        t1=difftime(e1,s1)*1000;
        // printf("Time taken for C1 process %lf\n",t1);
        char str2[25];
        long long int e1_ll=(long long int)e1;
        int len2=snprintf(NULL,0,"%lld",e1_ll)+1;
        snprintf(str2,len2,"%lld",e1_ll);
        close(fds_1_time[0]);
        write(fds_1_time[1],str2,len2);

        char str3[25];
        long long int s_ll=(long long int)data.start_time;
        // printf("s_ll is %lld\n",s_ll);
        int len3=snprintf(NULL,0,"%lld",s_ll)+1;
        snprintf(str3,len3,"%lld",s_ll);
        close(fds_1_stime[0]);
        write(fds_1_stime[1],str3,len3);

        char str4[25];
        int len4=snprintf(NULL,0,"%lf",data.waiting_time)+1;
        snprintf(str4,len4,"%lf",data.waiting_time);
        close(fds_1_wtime[0]);
        write(fds_1_wtime[1],str4,len4);
    }
}
