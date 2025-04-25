#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "my_barrier.h"

#define FAMILY_MEMBERS 6
#define DAYS 5

typedef enum { MAMA, PAPA, ALICE, BOB, CHRIS, DAVE } FamilyMember;

my_barrier_t breakfast_barrier;
my_barrier_t dinner_barrier;
my_barrier_t math_study_barrier;
my_barrier_t football_barrier;
my_barrier_t day_end_barrier;

void perform_activity(const char *activity, const char *name) {
    printf("%s starts %s.\n", name, activity);
    sleep(1); 
    printf("%s ends %s.\n", name, activity);
}

void *family_routine(void *arg) {
    FamilyMember member = *(FamilyMember *)arg;
    char *name;

    switch (member) {
        case MAMA: name = "Mama"; break;
        case PAPA: name = "Papa"; break;
        case ALICE: name = "Alice"; break;
        case BOB: name = "Bob"; break;
        case CHRIS: name = "Chris"; break;
        case DAVE: name = "Dave"; break;
    }

    for (int day = 1; day <= DAYS; day++) {
        if (member == MAMA) {
            printf("\n========================\n   Day %d Begins\n========================\n", day);
        }

        sleep(1); 
        printf("%s wakes up.\n", name);

        printf(" - %s is ready for breakfast.\n", name);
        my_barrier_wait(&breakfast_barrier);
        perform_activity("having breakfast", name);

        if (member == MAMA || member == PAPA) {
            perform_activity("working", name);
        } else {
            perform_activity("studying at school", name);
        }

        if (member == ALICE || member == BOB) {
            printf(" - %s is ready to study math with %s.\n", name, (member == ALICE) ? "Bob" : "Alice");
            my_barrier_wait(&math_study_barrier);
            perform_activity("studying math", name);
        } else if (member == CHRIS || member == DAVE) {
            printf(" - %s is ready to play football with %s.\n", name, (member == CHRIS) ? "Dave" : "Chris");
            my_barrier_wait(&football_barrier);
            perform_activity("playing football", name);
        }

        printf(" - %s is ready for dinner.\n", name);
        my_barrier_wait(&dinner_barrier);
        perform_activity("having dinner", name);

        printf("%s goes to sleep.\n", name);

        my_barrier_wait(&day_end_barrier);

        if (member == MAMA) {
            printf("========================\n   Day %d Ends\n========================\n", day);
        }
    }

    free(arg);
    return NULL;
}

int main() {
    pthread_t threads[FAMILY_MEMBERS];

    my_barrier_init(&breakfast_barrier, FAMILY_MEMBERS);
    my_barrier_init(&dinner_barrier, FAMILY_MEMBERS);
    my_barrier_init(&math_study_barrier, 2);  
    my_barrier_init(&football_barrier, 2);   
    my_barrier_init(&day_end_barrier, FAMILY_MEMBERS);

    for (int i = 0; i < FAMILY_MEMBERS; i++) {
        FamilyMember *member = malloc(sizeof(FamilyMember));  
        *member = i;
        pthread_create(&threads[i], NULL, family_routine, member);
    }

    for (int i = 0; i < FAMILY_MEMBERS; i++) {
        pthread_join(threads[i], NULL);
    }

    my_barrier_destroy(&breakfast_barrier);
    my_barrier_destroy(&dinner_barrier);
    my_barrier_destroy(&math_study_barrier);
    my_barrier_destroy(&football_barrier);
    my_barrier_destroy(&day_end_barrier);

    return 0;
}
