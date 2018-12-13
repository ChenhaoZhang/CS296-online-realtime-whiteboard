#define _GNU_SOURCE
#include <sched.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#define MAX_SIZE (100)

typedef struct container_status{
    pid_t pid;
    //char* ip_address;
    int assign_ip;
    int limit_pro;
    int limit_cpu;
    int cpu_freq;
}container_status;

container_status containers[MAX_SIZE];
static int size = 0;

void cleanup(int signal) {
    int status;
    while (waitpid((pid_t) (-1), 0, WNOHANG) > 0) {}
}

void usage()
{
    
    fprintf(stderr, "Usage: ./container [options] cmd [arg...]\n\n");
    fprintf(stderr, "Options can be: \n\n");
#define fps(str) fprintf(stderr, "    %s", str);
    fps("-p                          Limit the number of process\n");
    fps("-c 0 < <freq> <= 100        Limit CPU frequence\n");
    fps("-i                          Assign IP Address\n");
    exit(EXIT_FAILURE);
}

void print_containers()
{
    for(int i = 0; i < size; i++)
    {
        printf("container %d pid: %d\n", i, containers[i].pid);
    }
}
void limit_cpu_freq(pid_t pid, int freq)
{
    //fprintf(stdout, "Limit CPU Frequency\n");
    char f[10];
    snprintf(f, 10, "%d", freq);
    
    char p[10];
    snprintf(p, 10, "%d", (int)pid);
    
    char cmd[100];
    char l[100];
    strcpy(l, " --limit ");
    strcpy(cmd, "sudo cpulimit --pid ");
    strcat(cmd, p);
    strcat(cmd, l);
    strcat(cmd, f);
    strcat(cmd, " --background");
    fprintf(stdout, "cmd is %s\n", cmd);
    //system("ssh -tt host 'stty raw -echo; sudo ...' < <(cat)");
    //system("ssh -o IPQoS=throughput -tt host 'stty raw -echo; sudo cmd | cat' < <(cat)");
    system(cmd);
    /*
     pid_t c_pid = fork();
     if(c_pid == -1)
     {
     fprintf(stderr, "Limit CPU Frequency Fork Failed\n");
     }
     else if(c_pid == 0)
     {
     fprintf(stdout, "Limit CPU Frequency\n");
     system(cmd);*/
    /*
     if(execlp("sudo", "sudo", "cpulimit", "cpulimit", "--pid", p, "--limit", f, NULL) == -1)
     {
     fprintf(stderr, "Limit CPU Usage Failed: %s\n", strerror(errno));
     }
     */
    /* }
     else
     {
     int status;
     int a = waitpid(c_pid, &status, 0);
     if(a == -1)
     {
     fprintf(stderr, "Limit CPU Frequency Wait Failed\n");
     }
     }
     */
}

void limit_pro()
{
    struct rlimit r1;
    
    getrlimit (RLIMIT_NPROC, &r1);
    
    //printf("Default Hard Limit is : %lld\n", (long long int)r1.rlim_max);
    printf("Default Max # of Process is : %lld\n", (long long int)r1.rlim_cur);
    // Change the time limit
    r1.rlim_max = 20;
    r1.rlim_cur = 10;
    // Now call setrlimit() to set the
    // changed value.
    if(setrlimit (RLIMIT_NPROC, &r1) == -1)
    {
        printf("Limit # of Process Failed: %s\n", strerror(errno));
    }
    // Again get the limit and check
    getrlimit (RLIMIT_NPROC, &r1);
    
    //printf("Default Hard Limit now is : %lld\n", (long long int)r1.rlim_max);
    printf("Default Max # of Process  now is : %lld\n", (long long int)r1.rlim_cur);
    
}

void assign_ip()
{
    system("./ns-inet.sh");
}

int child(void *args)
{
    container_status* cs = (container_status*) args;
    if(cs -> limit_cpu)
    {
        //limit_cpu_freq(cs -> pid, cs -> cpu_freq);
    }
    
    if(cs -> limit_pro)
    {
        limit_pro();
    }
    
    if(cs -> assign_ip)
    {
        assign_ip();
    }
    printf("pid as seen in the child: %d\n", getpid());
    system("bash");
    
}

void handle_sigint()
{
    return;
}

int main(int argc, char*argv[])
{
    signal(SIGINT, handle_sigint);
    signal(SIGCHLD, cleanup);
    container_status cs;
    int opt;
    int limit_cpu = 0;
    int freq = 0;
    int set_ip = 0;
    while((opt = getopt(argc, argv, "c:ip")) != -1)
    {
        switch(opt)
        {
            case 'c':
                //cs.limit_cpu = 1;
                //int freq = atoi(optarg);
                freq = atoi(optarg);
                if(freq <= 0)
                {
                    usage();
                }
                //cs.cpu_freq = freq;
                limit_cpu = 1;
                printf("get freq %d\n", freq);
                break;
            case 'p':
                cs.limit_pro = 1;
                break;
            case 'i':
                cs.assign_ip = 1;
                //set_ip = 1;
                break;
            default:
                usage();
        }
        
    }
    
    int namespaces = CLONE_NEWUTS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWNS|CLONE_NEWNET;
    pid_t p = clone(child, malloc(4096) + 4096, SIGCHLD|namespaces, &cs);
    if (p == -1) {
        perror("clone");
        exit(1);
    }
    /*
     container_status child;
     child. pid = p;
     child.ip_address = "abc";
     containers[size] = child;
     size++;
     print_containers();
     */
    system("usermod -aG sudo root");
    if(limit_cpu)
    {
        limit_cpu_freq(p, freq);
    }
    
    
    printf("child pid: %d./na   \n", p);
    waitpid(p, NULL, 0);
    //printf("hi\n");
    return 0;
}


