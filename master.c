#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<wait.h>
#include<sys/types.h>
#include<math.h>

typedef struct thread_data {
   int n;
   int result;
}thread_data;

void *process_c2(void *n2){
    FILE *fp;
    fp=fopen("file.txt","r");
    int num;
    int* n2_ptr=(int *)n2;
    int n=*n2_ptr;
    for(int i=0;i<n;i++){
        fscanf(fp,"%d\n",&num);
        printf("number is : %d\n",num);
    }
    pthread_exit(NULL);
}

void *process_c1(void *data){
    int sum=0;
    thread_data *tdata=(thread_data*)data;
    // int* n1_ptr=(int*)n1;
    int n=tdata->n;
    for(int j=1;j<=n;j++){
        sum+=j;
    }
    // printf("sum in func is %d\n",sum);
    // pthread_exit((void*)&sum);
    tdata->result=sum;
    pthread_exit(NULL);
}

void *process_c3(void *data){
    FILE *fp;
    fp=fopen("file.txt","r");
    int num;
    thread_data *tdata=(thread_data*)data;
    int n=tdata->n;
    int sum=0;
    for(int i=0;i<n;i++){
        fscanf(fp,"%d\n",&num);
        sum=sum+num;
    }
    // printf("sum in func c3 is %d\n",sum);
    tdata->result=sum;
    pthread_exit(NULL);
}

int main()
{
    int c1_pid,c2_pid,c3_pid;
    int n1,n2,n3;
    int fds_1[2],fds_2[2],fds_3[3];
    pipe(fds_1);
    char buf_1[30],buf_3[30],buf_2[30];
    c1_pid=fork();
    if(c1_pid!=0){
        wait(NULL);
        close(fds_1[1]);
        read(fds_1[0],buf_1,25);
        printf("c1 sent %d to parent via pipe\n",atoi(buf_1));
        pipe(fds_2);
        c2_pid=fork();
        if(c2_pid!=0){
            wait(NULL);
            close(fds_2[1]);
            read(fds_2[0],buf_2,14);
            printf("%s\n",buf_2);
            pipe(fds_3);
            c3_pid=fork();
            if(c3_pid!=0){
                //parent or master process here
                wait(NULL);
                printf("this is parent process\n");
                close(fds_3[1]);
                read(fds_3[0],buf_3,25);
                printf("c3 sent %d to parent via pipe\n",atoi(buf_3));
            }
            else{
                //c3 process here
                printf("enter n3: ");
                scanf("%d",&n3);
                printf("entered n3 is %d\n",n3);
                pthread_t thread_id_3;
                thread_data data;
                data.n=n3;
                int rc=pthread_create(&thread_id_3,NULL,process_c3,(void*)&data);    
                if(rc){
                    printf("error %d in pthread_create()\n",rc);
			        exit(1);
                }
                pthread_join(thread_id_3,NULL);
                printf("sum in c3 is %d\n",data.result);
                char str[30];
                int len=snprintf(NULL,0,"%d",data.result)+1;
                snprintf(str,len,"%d",data.result);
                close(fds_3[0]);
                write(fds_3[1],str,len);
            }
        }
        else{
            //c2 process here
            printf("enter n2: ");
            scanf("%d",&n2);
            printf("entered n2 is %d\n",n2);
            pthread_t thread_id_2;
            int rc=pthread_create(&thread_id_2,NULL,process_c2,(void*)&n2);    
            if(rc){
                printf("error %d in pthread_create()\n",rc);
			    exit(1);
            }
            pthread_join(thread_id_2,NULL);
            // printf("done printing\n");
            close(fds_2[0]);
            write(fds_2[1],"Done Printing",14);
        }
    }
    else{
        //c1 process here
        printf("enter n1: ");
        scanf("%d",&n1);
        printf("entered n1 is %d\n",n1);
        pthread_t thread_id_1;
        thread_data data;
        data.n=n1;
        
        int rc=pthread_create(&thread_id_1,NULL,process_c1,(void*)&data);    
        if(rc){
            printf("error %d in pthread_create()\n",rc);
		    exit(1);
        }
        pthread_join(thread_id_1,NULL);
        printf("sum in c1 is %d\n",data.result);
        char str[30];
        int len=snprintf(NULL,0,"%d",data.result)+1;
        snprintf(str,len,"%d",data.result);
        close(fds_1[0]);
        write(fds_1[1],str,len);
    }
}