#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100

typedef struct data {
    int arrive_time; //도착시간
    int track_num; //트랙번호
    int gap_time; //SSTF를 위한 (현 트랙 - 다음 트랙)의 절대값
    int flag; //LOOK을 위한 변수, 1이면 확인, -1이면 확인 안함
    data *rlink; //리스트를 위한 것
    data *link;
}data;

typedef struct head_data {
    data *front;
    data *rear;
    int count; //SSTF를 위한 것
    int direct; //LOOK을 위한 것, 3이 오른쪽, 2가 왼쪽
}head_data;

int abso(int a, int b) { //절대값함수
    int c;
    c = a-b;
    if(c>0)
        return c;
    else
        return -c;
}

void List_inti(head_data *d) {
    d->front = NULL;
    d->rear = NULL;
    d->count = 0;
    d->direct = 3;
}

void List_insert(head_data *d, data value) {
    data *temp = (data*)malloc(sizeof(data));

    temp->arrive_time = value.arrive_time;
    temp->flag = value.flag;
    temp->gap_time = value.gap_time;
    temp->track_num = value.track_num;

    if(d->count == 0) {
        d->front = temp;
        d->rear = temp;
        temp->link = NULL;
        temp->rlink = NULL;
    }
    else {
        d->rear->rlink = temp;
        temp->link = d->rear;
        d->rear = temp;
        temp->rlink = NULL;
    }

    d->count++;
}

void set_gap(head_data *d, int track) { //gap 구하는 함수
    data *temp;
    for(temp = d->front ; temp != NULL ; temp = temp->rlink) {
        temp->gap_time = abso(track, temp->track_num);
    }
}

int find_gap(head_data *d, int *track, int service, int time) { //제일작은 gap찾는 함수
    data *temp;
    data *temp2;
    int gap_temp = 10000, result_time;

    for(temp = d->front ; temp != NULL ; temp = temp->rlink) {
        if(temp->gap_time < gap_temp) {
            gap_temp = temp->gap_time;
            temp2 = temp;
        }
    }

    result_time = temp2->gap_time + time + service;
    *track = temp2->track_num;

    if(temp2 == d->front && temp2 != d->rear) {
        temp2->rlink->link = NULL;
        d->front = temp2->rlink;
        temp2->rlink = NULL;
    }
    else if(temp2 == d->rear && temp2 != d->front) {
        temp2->link->rlink = NULL;
        d->rear = temp2->link;
        temp2->link = NULL;
    }
    else if(temp2 == d->front && temp2 == d->rear) {
        d->front = NULL;
        d->rear = NULL;
    }
    else {
        temp2->rlink->link = temp2->link;
        temp2->link->rlink = temp2->rlink;
        temp2->rlink = NULL;
        temp2->link = NULL;
    }

    d->count--;
    free(temp2);
    return result_time;
}

int end_list(head_data *d) { //1이면 종료, 0이면 안 종료
    data *temp;

    for(temp = d->front ; temp != NULL ; temp = temp->rlink) {
        if(temp->flag == -1) {
            return 0;
        }
    }

    return 1;
}

int LOOK_MAX(head_data *d, int *track, int service) { //최대값이 있을 경우 값출력, 없을경우 -1
    int result_track, time_temp, flag = -1;
    int count = 0;
    data *temp;
    data *temp2 = (data*)malloc(sizeof(data));
     
    temp2->track_num = (*track);

    for(temp = d->front ; temp != NULL ; temp = temp->rlink) {
        if(temp->track_num >= *track  && temp->flag == -1) {
            if(temp->track_num >= temp2->track_num  && temp->flag == -1) {
                temp2 = temp;
            }
            temp->flag = 1;
            count++;
            flag = 1;
        }
    }

    if(flag == 1) {
        result_track = temp2->track_num;
        time_temp = abso(*track, result_track) + count*service;
        *track = temp2->track_num;
        return time_temp;
    }
    else {
        return -1;
    }
}

int LOOK_MIN(head_data *d, int *track, int service) { //최소값이 있을 경우 값출력, 없을경우 -1
    int result_track, time_temp, flag = -1;
    int count = 0;
    data *temp;
    data *temp2 = (data*)malloc(sizeof(data));

    temp2->track_num = (*track);

    for(temp = d->front ; temp != NULL ; temp = temp->rlink) {
        if(temp->track_num <= *track && temp->flag == -1) {
            if(temp->track_num <= temp2->track_num  && temp->flag == -1) {
                temp2 = temp;
            }
            temp->flag = 1;
            count++;
            flag = 1;
        }
    }

    if(flag == 1) {
        result_track = temp2->track_num;
        time_temp = abso(*track, result_track) + count*service;
        *track = temp2->track_num;
        return time_temp;
    }
    else {
        return -1;
    }
}

int LOOK(head_data *d, int time, int service, int *track) {
    int count = 0;
    int temp, result_time;
     
    if(d->direct == 3) {
        temp = LOOK_MAX(d, track, service);
        if(temp == -1) {
            d->direct = 2;
            result_time = time + LOOK_MIN(d, track, service);
        }
        else {
            result_time = time + temp;
        }
    }
    else {
        temp = LOOK_MIN(d, track, service);
        if(temp == -1) {
            d->direct = 3;
            result_time = time + LOOK_MAX(d, track, service);
        }
        else {
            result_time = time + temp;
        }
    }

    return result_time;
}

int main() {
    int time = 0;
    int track = 0;
    int service_time;
    data input[100];
    char command[5];

    int i, j, j_temp = 0;
    int for_break;

    head_data List_head;

    FILE *file = fopen("disk.inp", "rt");
    FILE *file2 = fopen("disk.out", "wt");

    fscanf(file, "%s %d", &command, &service_time);

    for(i = 0 ; i < MAX ; i++) {
        fscanf(file, "%d %d", &input[i].arrive_time, &input[i].track_num);
        input[i].flag = -1;
        input[i].gap_time = -1;
        //input[i].rlink = NULL;
        //input[i].link = NULL;

        if(input[i].arrive_time == -1 && input[i].track_num == -1) {
            for_break = i;
            break;
        }
    }

    if(strcmp(command, "FCFS") == 0) {
        for(i = 0 ; i < for_break ; i++) {
            if(time != 0 && time < input[i].arrive_time) {
                time = input[i].arrive_time;
                //track = input[i].track_num;
            }
            time = time + abso(track, input[i].track_num) + service_time;
            track = input[i].track_num;
        }

        //printf("%d %d\n", time, track);
        fprintf(file2, "%d %d", time, track);
    }

    else if(strcmp(command, "SSTF") == 0) {
        List_inti(&List_head);

        while(1) {
            for(j = j_temp ; input[j].arrive_time != -1 && input[j].track_num != -1 ; j++) {
                if(time >= input[j].arrive_time) {
                    List_insert(&List_head, input[j]);
                    j_temp = j+1;
                }
                else {
                    if(List_head.count == 0) {
                        time = input[j].arrive_time;
                        List_insert(&List_head, input[j]);
                        j_temp = j+1;
                    }
                    else {
                        j_temp = j;
                        break;
                    }
                }
            }

            if(input[j_temp].arrive_time == -1 && input[j_temp].track_num == -1 && List_head.count == 0) {
                break;
            }

            set_gap(&List_head, track);
            time = find_gap(&List_head, &track, service_time, time);

        }
        //printf("%d %d\n", time, track);
        fprintf(file2, "%d %d", time, track);
    }

    else {
        List_inti(&List_head);
        while(1) {
            for(j = j_temp ; input[j].arrive_time != -1 && input[j].track_num != -1 ; j++) {
                if(time >= input[j].arrive_time) {
                    List_insert(&List_head, input[j]);
                    j_temp = j+1;
                }
                else {
                    if(end_list(&List_head) == 1) {
                        //List_inti(&List_head);
                        time = input[j].arrive_time;
                        List_insert(&List_head, input[j]);
                        j_temp = j+1;
                    }
                    else {
                        j_temp = j;
                        break;
                    }
                }
            }

            if(input[j_temp].arrive_time == -1 && input[j_temp].track_num == -1 && end_list(&List_head) == 1) {
                break;
            }

            time = LOOK(&List_head, time, service_time, &track);

        }

        //printf("%d %d\n", time, track);
        fprintf(file2, "%d %d", time, track);
    }

    fclose(file);
    fclose(file2);
    return 0;
}

