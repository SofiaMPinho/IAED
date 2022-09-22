/*
 * File: proj1.c
 * Author: Sofia Pinho
 * Description: A program that simulates a task management system
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_DESC_ACTIV 21 /*maximum length of activity descriptions*/
#define MAX_ACTIV      10 /*max number of activities*/

#define MAX_DESC_USER 21 /*maximum length of user descriptions*/
#define MAX_USER      50 /*max number of users*/

#define MAX_DESC_TASK 51 /*maximum length of task descriptions*/
#define MAX_TASK      10000 /*max number of tasks*/

/*constants used in functions that evaluate whether input is valid or not*/
#define VALID         1
#define INVALID       0
#define TRUE          1
#define FALSE         0
/*constants used to distinguish alphabetical from start instance based sorting*/
#define ALPH          1
#define START_INST    0
/*default activities descriptions*/
#define TO_DO_DESC    "TO DO"
#define IN_P_DESC     "IN PROGRESS"
#define DONE_DESC     "DONE"

/*categories in which the project tasks are divided and that
represent a specific operation*/
typedef struct activity
{
    char desc[MAX_DESC_ACTIV];
} Activity; 

/*type used to represent and distinguish between the people 
using the simulator*/
typedef struct
{
    char desc[MAX_DESC_USER];
} User;

/*type used to represent and store all relevant information
connected to each task*/
typedef struct task
{
    int id;
    char desc[MAX_DESC_TASK];
    User user;
    Activity activ;
    int dur, start_inst;
} Task;

Activity activities[MAX_ACTIV]; /*array of activities in the system*/
User users[MAX_USER]; /*array of users in the system*/
Task tasks[MAX_TASK]; /*array of tasks in the system*/

/*global variables that allow to keep track of time and 
the number of elements in the system*/
int time = 0, num_activ = 0, num_user = 0, num_task = 0;

/*adds a new task to the system and increases the global task counter
and returns the new task id*/
int create_task(char desc[], User user, Activity activ, int dur)
{
    int id;
    
    id = num_task + 1;
    tasks[num_task].id = id;
    strcpy(tasks[num_task].desc, desc);
    tasks[num_task].activ = activ;
    tasks[num_task].user = user;
    tasks[num_task].dur = dur;
    tasks[num_task].start_inst = 0;

    num_task++;

    return id;
}

/*verifies if the input in the t command is valid*/
int t_valid_input(char desc[], int dur)
{
    int i;
    
    if(num_task >= MAX_TASK) {
        printf("too many tasks\n");
        return INVALID;
    }

    for(i = 0; i < num_task; ++i) {
        if(strcmp(desc, tasks[i].desc) == 0) {
            printf("duplicate description\n");
            return INVALID;
        }
    }
    
    if(dur <= 0) {
        printf("invalid duration\n");
        return INVALID;
    }

    return VALID;
}

/*executes the t command, creating a new task*/
void t(User user, Activity activ_to_do)
{
    int dur, id;
    char desc_task[MAX_DESC_TASK];

    scanf("%d", &dur);
    
    getchar(); /*space*/
    fgets(desc_task, MAX_DESC_TASK, stdin);
    if (desc_task[strlen(desc_task)-1] == '\n')
        desc_task[strlen(desc_task)-1] = '\0';

    if(t_valid_input(desc_task, dur) == VALID) {
        id = create_task(desc_task, user, activ_to_do, dur);

        printf("task %d\n", id);
    }    
}

/*exchanges the two values with the indexes ind1 and ind2 in the array v*/
void exch(int v[], int ind1, int ind2)
{
    int temp = v[ind1];

    v[ind1] = v[ind2];
    v[ind2] = temp;
}

/*returns TRUE if the description of the task with index id1 comes first compared to
the description of the task with index id2 in alphabetical order*/
int less_alph(int id1, int id2)
{
    return (strcmp(tasks[id1].desc, tasks[id2].desc) < 0);
}

/*returns TRUE if the task with index id1 began earlier than the task with index id2,
if they began at the same time returns TRUE if the description of the task with id1 
comes first in alphabetical order*/
int less_inst(int id1, int id2)
{
    if(tasks[id1].start_inst < tasks[id2].start_inst)
        return TRUE;

    if((tasks[id1].start_inst == tasks[id2].start_inst) && 
        less_alph(id1, id2))
        return TRUE;
    return FALSE;
}

/*partitions the ids array in two, depending on the type of sorting pretended, 
auxiliary to the sort_tasks function*/
int partition(int ids[], int left, int right, int type)
{
    int v = ids[right], i = left - 1, j = right;

    while(i < j) {
        if(type == ALPH) {
            while (less_alph(ids[++i], v));
            while (less_alph(v, ids[--j]))
                if(j == left)
                    break;
        }

        if(type == START_INST) {
            while (less_inst(ids[++i], v));
            while (less_inst(v, ids[--j]))
                if(j == left)
                    break;
        }
        
        if(i < j)
            exch(ids, i, j);
    }

    exch(ids, i, right);
    return i;
}

/*sorts the tasks with ids in the v array alphabetically or in order of start instance*/
void sort_tasks(int v[], int left, int right, int type)
{
    int i;
    
    if(right <= left)
        return;
    
    i = partition(v, left, right, type);

    sort_tasks(v, left, i-1, type);
    sort_tasks(v, i+1, right, type);
}

/*lists all existing tasks in alphabetical order*/
void list_tasks()
{
    int i;
    int sorted[MAX_TASK]; /*array in which all tasks indexs will be stored and then sorted*/

    for(i = 0; i < num_task; ++i)
        sorted[i] = i;

    sort_tasks(sorted, 0, num_task - 1, ALPH); 
        
    for(i = 0; i < num_task; ++i) {
        printf("%d %s #%d %s\n", tasks[sorted[i]].id, tasks[sorted[i]].activ.desc, 
                tasks[sorted[i]].dur, tasks[sorted[i]].desc);
    }
}

/*executes the l command, listing all the existing tasks in alphabetical order
or the tasks with the ids input by the user*/
void l()
{
    char c;
    int i, id;

    if((c = getchar()) == '\n') 
        list_tasks();

    else {
        do {
            scanf("%d", &id);
            if((id > num_task) || (id <= 0))
                printf("%d: no such task\n", id);
            
            else {
                i = id - 1;
                printf("%d %s #%d %s\n", tasks[i].id, tasks[i].activ.desc, 
                        tasks[i].dur, tasks[i].desc);
            }
        } while((c = getchar()) != '\n');
    }
}

/*executes the n command, printing the current time or increasing it*/
void n()
{
    int dur;

    scanf("%d", &dur);

    if(dur < 0) {
        printf("invalid time\n");
        return;
    }

    if(dur != 0) 
        time += dur;

    printf("%d\n", time);
}

/*adds a new user to the system, auxiliary to the u function*/
void u_new_user()
{
    int i;
    User user;
    
    scanf("%s", user.desc);

    for(i = 0; i < num_user; ++i) { /*check wether the user already exists*/
        if(strcmp(user.desc, users[i].desc) == 0) {
            printf("user already exists\n");

            return;
        }
    }

    if(num_user >= MAX_USER) { 
        printf("too many users\n");

        return;
    }

    num_user++;
    
    strcpy(users[num_user - 1].desc, user.desc);
}

/*executes the u command, listing existing users or creating a new one*/
void u()
{
    int i;

    if(getchar() == '\n') {
        for(i = 0; i < num_user; ++i)
            printf("%s\n", users[i].desc); 
        }
                
    else
        u_new_user();
}

/*verifies if the task with identifier id exists in the system*/
int task_exists(int id)
{
    return ((0 < id) && (id <= num_task));
}

/*verifies if the user exists in the system*/
int user_exists(User user)
{
    int i;

    for(i = 0; i < num_user; ++i) {
        if(strcmp(user.desc, users[i].desc) == 0)
            return TRUE;
    }
    return FALSE;
}

/*verifies if the activity with description desc exists in the system*/
int activ_exists(char desc[])
{
    int i;

    for(i = 0; i < num_activ; ++i) {
        if(strcmp(desc, activities[i].desc) == 0)
            return TRUE;
    }

    return FALSE;
}

/*veryfies if the input following the m command is valid, 
auxiliary to the m function*/
int m_valid_input(int id, Activity activ, User user, Activity activ_to_do) 
{
    if(!(task_exists(id))) {
        printf("no such task\n");
        return INVALID;
    }
    /*return INVALID if trying to move to TO DO activity,
    unless moving from TO DO to TO DO to change the user*/
    if((strcmp(activ.desc, activ_to_do.desc) == 0) &&
        !((strcmp(user.desc, tasks[id-1].user.desc) != 0) &&
        (strcmp(tasks[id-1].activ.desc, activ_to_do.desc) == 0))) {
        printf("task already started\n");
        return INVALID;
    }
    if(!(user_exists(user))) {
        printf("no such user\n");
        return INVALID;
    }
    if(!(activ_exists(activ.desc))) {
        printf("no such activity\n");
        return INVALID;
    }
    return VALID;
}

/*calculate and print slack and duration if moving to DONE,
auxiliary to the m function*/
void calc_slack_dur(Activity activ, Activity activ_done, Activity activ_to_do, int id)
{
    int duration, slack;
    
    if((strcmp(activ.desc, activ_done.desc) == 0) && 
        (strcmp(tasks[id - 1].activ.desc, activ_done.desc) != 0)) {
        if(strcmp(tasks[id - 1].activ.desc, activ_to_do.desc) == 0)
            duration = 0;
        else
            duration = time - tasks[id - 1].start_inst;
        
        slack = duration - tasks[id - 1].dur;
        printf("duration=%d slack=%d\n", duration, slack);
    }
}

/*executes the m command, moving a task from one activity to another*/
void m(Activity activ_done, Activity activ_to_do) 
{
    int id;
    Activity activ;
    User user;

    scanf("%d%s", &id, user.desc);
    getchar(); /*space*/
    
    fgets(activ.desc, MAX_DESC_ACTIV, stdin);
    if (activ.desc[strlen(activ.desc)-1] == '\n')
        activ.desc[strlen(activ.desc)-1] = '\0';
    /*return to main if the input isn't valid*/
    if((m_valid_input(id, activ, user, activ_to_do)) != VALID) 
        return;
    /*change the user if moving from TO DO to TO DO with a different 
    user from the one who input the task into the system, and return*/
    if((strcmp(user.desc, tasks[id-1].user.desc) != 0) &&
        (strcmp(activ.desc, activ_to_do.desc) == 0)) {
            strcpy(tasks[id - 1].user.desc, user.desc);
            return;
        }
    /*change the task start_inst to current time if moving from TO DO*/
    if(strcmp(tasks[id - 1].activ.desc, activ_to_do.desc) == 0)
        tasks[id - 1].start_inst = time;
    
    calc_slack_dur(activ, activ_done, activ_to_do, id);

    strcpy(tasks[id - 1].activ.desc, activ.desc); 
    strcpy(tasks[id - 1].user.desc, user.desc);
}

/*executes the d command, listing all the tasks in the activity input by the user*/
void d()
{
    char desc_activ[MAX_DESC_ACTIV];
    int i, j;
    int inds[MAX_TASK]; /*array with the index of all tasks in the input activity*/

    getchar(); /*space*/
    fgets(desc_activ, MAX_DESC_ACTIV, stdin);
    if (desc_activ[strlen(desc_activ)-1] == '\n')
        desc_activ[strlen(desc_activ)-1] = '\0';

    if(!(activ_exists(desc_activ))) {
        printf("no such activity\n");
        return;
    }        
    for(i = 0, j = 0; i < num_task; ++i)
        if(strcmp(desc_activ, tasks[i].activ.desc) == 0) {
            inds[j] = tasks[i].id - 1;
            ++j;
        }

    sort_tasks(inds, 0, j - 1, START_INST);
    for(i = 0; i < j; i++)
        printf("%d %d %s\n", inds[i] + 1, tasks[inds[i]].start_inst, 
               tasks[inds[i]].desc);                
}

/*verifies whether the srting desc has any lower case letters or not*/
int all_upper_case(char desc[])
{
    int i, comp = strlen(desc);
    char c;

    for(i = 0; i < comp; ++i) {
        c = desc[i];
        if('a' <= c && c <= 'z')
            return FALSE;
    }

    return TRUE;
}

/*verifies if the string desc is a valid activity description*/
int a_valid_desc(char desc[])
{
    if(activ_exists(desc)) {
        printf("duplicate activity\n");

        return INVALID;
    }
    if(!(all_upper_case(desc))) {
        printf("invalid description\n");

        return INVALID;
    }
    if(num_activ >= MAX_ACTIV) {
        printf("too many activities\n");

        return INVALID;
    }
    return VALID;    
}

/*adds an activity with the description desc to the system*/
void create_activ(char desc[])
{
    if(a_valid_desc(desc) == INVALID)
        return;

    strcpy(activities[num_activ].desc, desc);
    
    num_activ++;
}

/*executes the a command, listing all activities or adding a new one to the system*/
void a()
{
    char desc_activ[MAX_DESC_ACTIV];
    int i;

    if(getchar() == '\n') {
        for(i = 0; i < num_activ; ++i)
            printf("%s\n", activities[i].desc);
    }
    else {
        fgets(desc_activ, MAX_DESC_ACTIV, stdin);
        if (desc_activ[strlen(desc_activ)-1] == '\n')
            desc_activ[strlen(desc_activ)-1] = '\0';

        if(a_valid_desc(desc_activ) == INVALID)
            return;

        create_activ(desc_activ);
    }
}

/*reads user input and manipulates the management system*/
void commands(User default_user, Activity activ_to_do, Activity activ_done)
{
    char c;

    while((c = getchar()) != 'q') {
        switch(c) {
            case 't': {
                if(num_user > 0)
                    t(users[num_user - 1], activ_to_do);
                else
                    t(default_user, activ_to_do);
                break;
            } 
            case 'l': {
                l();
                break;
            }
            case 'n': { 
                n();
                break;
            }
            case 'u': {
                u();
                break;
            }
            case 'm': {
                m(activ_done, activ_to_do);
                break;
            }
            case 'd': {
                d();
                break; 
            }
            case 'a': {
                a();
                break;
            }
        }
    }
}

/*main function that initiates the default user and default activities, and calls
the function responsible for dealing with user input and manipulating the system*/
int main() 
{   
    /*used to maintain the user field empty in new tasks if no user has been created*/
    User default_user = {"\0"}; 
    Activity activ_to_do, activ_done;

    create_activ(TO_DO_DESC);
    activ_to_do = activities[num_activ - 1];
    create_activ(IN_P_DESC);
    create_activ(DONE_DESC);
    activ_done = activities[num_activ - 1];

    commands(default_user, activ_to_do, activ_done);
    
    return 0;
}