#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
void loadTheSeed()
{
    FILE *fp;
    char buff[255];

    fp = fopen("seeds/seed.txt", "r");
    fscanf(fp, "%s", buff);

    long seed = strtol(buff, NULL, 10);
    fclose(fp);

    srand(seed);

    printf("Read seed value: %s\n\n", buff);
    printf("Read seed value (converted to integer): %ld\n", seed);
}

int randIntBetween(int min, int max)
{
    return rand() % (max - min + 1) + min;
}



// TODO convert to semaphore

// threads for planes
pthread_t threads[45];

// semaphores
sem_t one;
sem_t two;
sem_t three;
sem_t four;
sem_t five;
sem_t six;

sem_t RunWay;
// struct for plane which stores the type of plane and the unique id
struct Plane
{
    enum
    {
        LARGE,
        SMALL
    } type;
    int id; //gets the id of the plane
};

// array of planes
struct Plane planes[44];

const char *const plane_size[] = {
    [0] = "Large",
    [1] = "Small"};

// plane states
enum PlaneStatus
{
    I,
    WAIT_T,
    TAKEOFF,
    FLY,
    WAIT_L,
    LAND
};

// use enum as string
const char *const condition_names[] = {
    [I] = "Idle at Terminal",
    [WAIT_T] = "Awaiting Takeoff",
    [TAKEOFF] = "Taking Off",
    [FLY] = "FLY",
    [WAIT_L] = "Awaiting to Land",
    [LAND] = "Landing"};





/**
 * function called by the plane threads which goes through the states of the planes
 * takes in a plane struct
 */
void *planeManager(void *i)
{
    // possible runway combinations of big and small planes
    int bZone[2][3] = {{1, 4, 6}, {2, 3, 5},{6, 4, 1}, {5, 3, 2} };
    int sZone[6][2] = {{1, 2}, {3, 4}, {1, 4}, {2, 3}, {4, 6}, {3, 5},{2, 1}, {4, 3}, {4, 1}, {3, 2}, {6, 4}, {5, 3}};

    // state of plane when it starts
    enum PlaneStatus condition = I;

    // for choosing which zones you get
    int choseZone;

    // a = the id of the plane in the planes array
    int a = *((int *)i);

    int sizePlane;

    // chosen zone
    int chosenZone[3];

    // time var
    int t;
    // infinite loop to get everything to run
    while (1 == 1)
    {
       //Switch statement
        switch (condition)
        {
            //What happens when it is IDLe
        case I:
            t = (rand() % 10) + 1;
            printf("Plane Thread %d, size: %s, state: %s About to sleep for %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], t);
            usleep(t * 1000000);
            condition = WAIT_T;
            //What happens when it is waiting for Takeoff
        case WAIT_T:

            // chosses runway combinationand assign right/left indexes
            if (planes[a].type == 0)
            {
                choseZone = rand() % 4;// make this 4
               sizePlane = 3;
            }
            else
            {
                choseZone = rand() % 12;// make this 12
                sizePlane = 2;
            }
            // copy to new array
            for (int i = 0; i < sizePlane; i++)
            {
                if (planes[a].type == 0)
                {
                    chosenZone[i] = bZone[choseZone][i];
                }
                else
                {
                    chosenZone[i] = sZone[choseZone][i];
                }
            }
            // size of array
            int planesize = 0;

            if (planes[a].type == 0)
            {
                planesize = 3;
                printf("Plane Thread %d, size: %s, state: %s . run way order is zones %d %d %d\n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[0], chosenZone[1], chosenZone[2]);
            }
            else
            {
                planesize = 2;
                printf("Plane Thread %d, size: %s, state: %s .  run way order is zones %d %d \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[0], chosenZone[1]);
            }

            // see if able to claim immediatly
            if (sem_trywait(&RunWay) == 0)
            {
                // if able to claim go to claim area
                goto start;
            }
            printf("Plane Thread %d, size: %s, state: %s . Waiting for runway to clear \n", planes[a].id, plane_size[planes[a].type], condition_names[condition]);

        end:
            // block claim
            sem_wait(&RunWay);
        start:
            // wait on zones
            for (int i = 0; i < planesize; i++)
            {
                switch (chosenZone[i])
                {
                case 1:
                    sem_wait(&one);
                    break;
                case 2:
                    sem_wait(&two);
                    break;
                case 3:
                    sem_wait(&three);
                    break;
                case 4:
                    sem_wait(&four);
                    break;
                case 5:
                    sem_wait(&five);
                    break;
                case 6:
                    sem_wait(&six);
                    break;
                default:
                    sem_post(&RunWay);
                    goto end;
                }
            }
            // unblock claim
            sem_post(&RunWay);
            printf("Plane Thread %d, size: %s, state: %s . Runway is clear for takeoff \n", planes[a].id, plane_size[planes[a].type], condition_names[condition]);

            condition = TAKEOFF;
        case TAKEOFF:
            // time to wait in each zone
            t = (rand() % 2) + 1;

            for (int i = 0; i < planesize; i++)
            {

                switch (chosenZone[i])
                {
                case 1:
                    // sleep and unblock zone
                    printf("Plane Thread %d, size: %s, state: %s is moving down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&one);

                    break;
                case 2:
                    // sleep and unblock zone
                    printf("Plane Thread %d, size: %s, state: %s is moving down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&two);

                    break;
                case 3:
                    // sleep and unblock zone
                    printf("Plane Thread %d, size: %s, state: %s is moving down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&three);

                    break;
                case 4:
                    // sleep and unblock zone
                    printf("Plane Thread %d, size: %s, state: %s is moving down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&four);

                    break;
                case 5:
                    // sleep and unblock zone
                    printf("Plane Thread %d, size: %s, state: %s is moving down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&five);

                    break;
                case 6:
                    // sleep and unblock zone
                    printf("Plane Thread %d, size: %s, state: %s is moving down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&six);

                    break;
                }
            }

            condition = FLY;
        case FLY:
        

            // choose FLY time
            t = (rand() % 10) + 5;

            printf("\n Plane Thread %d, size: %s, state: %s flys for %d seconds \n\n", planes[a].id, plane_size[planes[a].type], condition_names[condition], t);

            t = (rand() % 10);
            usleep(t * 1000000);

            condition_names = WAIT_L;

        case WAIT_L:

            if (planes[a].type == 0)
            {
                choseZone = rand() % 4;
                
            }
            else
            {
                choseZone = rand() % 12;
                
            }

            for (int i = 0; i < 12; i++)
            {
                if (planes[a].type == 0)
                {
                    chosenZone[i] = bZone[choseZone][i];
                }
                else
                {
                    chosenZone[i] = sZone[choseZone][i];
                }
            }
            if (planes[a].type == 0)
            {
                printf("Plane Thread %d, size: %s, state: %s .My landing zones are %d %d %d\n", planes[a].id, plane_size[planes[a].type], condition_names[condition_names], chosenZone[0], chosenZone[1], chosenZone[2]);
            }
            else
            {
                printf("Plane Thread %d, size: %s, state: %s .My landing zones are %d %d \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[0], chosenZone[1]);
            }

            int printed2 = 0;

            if (sem_trywait(&RunWay) == 0)
            {

                goto startLand;
            }
            printf("Plane Thread %d, size: %s, state: %s . Waiting for runway to clear \n", planes[a].id, plane_size[planes[a].type], condition_names[condition]);

        endLand:

            sem_wait(&RunWay);
        startLand:
            for (int i = 0; i < planesize; i++)
            {
                switch (chosenZone[i])
                {
                case 1:
                    sem_wait(&one);
                    break;
                case 2:
                    sem_wait(&two);
                    break;
                case 3:
                    sem_wait(&three);
                    break;
                case 4:
                    sem_wait(&four);
                    break;
                case 5:
                    sem_wait(&five);
                    break;
                case 6:
                    sem_wait(&six);
                    break;
                default:
                    sem_post(&RunWay);
                    goto endLand;
                }
            }
            sem_post(&RunWay);
            printf("Plane Thread %d, size: %s, state: %s . My runway for Landing is clear, proceed to take off \n", planes[a].id, plane_size[planes[a].type], condition_names[condition]);

            condition = LAND;

        case LAND:
            t = rand() % 2;

            for (int i = 0; i < 3; i++)
            {
                int val;
                switch (chosenZone[i])
                {
                case 1:
                    printf("PlaneThread %d, of size: %s, state: %s is goes down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&one);

                    break;
                case 2:
                    printf("PlaneThread %d, of size: %s, state: %s is goes down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&two);

                    break;
                case 3:
                    printf("PlaneThread %d, of size: %s, state: %s is goes down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&three);

                    break;
                case 4:
                    printf("PlaneThread %d, of size: %s, state: %s is goes down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&four);
                    break;
                case 5:
                    printf("PlaneThread %d, of size: %s, state: %s is goes down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&five);

                    break;
                case 6:
                    printf("PlaneThread %d, of size: %s, state: %s is goes down the runway and will reach the next zone %d in %d seconds \n", planes[a].id, plane_size[planes[a].type], condition_names[condition], chosenZone[i], t);
                    usleep(t * 1000000);
                    sem_post(&six);

                    break;
                }
            }
            printf(" \n   Plane Thread %d, size: %s, state: %s. Done,starting again", planes[a].id, plane_size[planes[a].type], condition_names[condition]);
            condition = I;
            break;
        }
    }
}

int main()
{
    loadTheSeed();

    // init semaphore to 1 allowing blocks on all
    sem_init(&one, 0, 1);
    sem_init(&two, 0, 1);
    sem_init(&three, 0, 1);
    sem_init(&four, 0, 1);
    sem_init(&five, 0, 1);
    sem_init(&six, 0, 1);
    sem_init(&RunWay, 0, 1);

    // create plane threads

    for(int i = 0; i<15; i++){
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        struct Plane tempP;
        tempP.type = LARGE;
            tempP.id = i + 1;
            pthread_create(&threads[i], NULL, planeManager, arg);
            planes[i] = tempP;
    }

    for(int i = 0; i<30; i++){
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        struct Plane tempP;
        tempP.type = SMALL;
            tempP.id = i + 1;
            pthread_create(&threads[i], NULL, planeManager, arg);
            planes[i] = tempP;
    }
    // join threads
    for (int i = 0; i < 45; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // destroy semaphores
    sem_destroy(&one);
    sem_destroy(&two);
    sem_destroy(&three);
    sem_destroy(&four);
    sem_destroy(&five);
    sem_destroy(&six);
    sem_destroy(&RunWay);
}
