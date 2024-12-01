#include <stdio.h>
#include <stdlib.h>
#define MAX 100

typedef struct ref_Information { //ref구조체
    int Process_num; //프로세스번호
    int Page_num; //페이지번호
    int Index_count; //출력을 위한 인덱스번호
    int Count; //LRU를 위한 카운터
    int Future; //OPT를 위한 것, -1 할당 ㄴㄴ 
    ref_Information *Next;
} ref_Information;

typedef struct List_head { //리스트헤더구조체
    int Process_num;
    int Frame_num;
    ref_Information *front;
    ref_Information *rear; //FIFO를 위하여...
} List_head;

void head_arr_init(List_head arr[], int num) { //리스트헤더배열 초기화
    int i;
    for(i=0 ; i < MAX ; i++) {
        arr[i].front = NULL;
        arr[i].rear = NULL;
        arr[i].Frame_num = 0;
        arr[i].Process_num = -1;
    }
}

/////////FIFO 시작
void FIFO_en(List_head* q, ref_Information P, int index) { //FIFO 삽입
    ref_Information *temp = (ref_Information*)malloc(sizeof(ref_Information));
     
    temp->Process_num = P.Process_num;
    temp->Page_num = P.Page_num;
    temp->Index_count = index;
    temp->Count = P.Count;
    temp->Next = NULL;

    if(q->Process_num == -1) { //처음 들어 온 프로세스 번호일 때
        q->front = temp;
        q->rear = temp;
        //q->Process_num = P.Process_num;
    }
    else {
        q->rear->Next = temp;
        q->rear = temp;
    }
    q->Frame_num++;
}

ref_Information FIFO_de(List_head* q) { //FIFO삭제
    ref_Information *temp = q->front;;
    ref_Information item;

    item.Count = temp->Count;
    item.Index_count = temp->Index_count;
    item.Page_num = temp->Page_num;
    item.Process_num = temp->Process_num;

    q->front = temp->Next;
    q->Frame_num--;
    free(temp);

    return item;
}

int equal_search(List_head* q, ref_Information P) { //같은 프로세스번호, 페이지번호 있나 확인 , 1있음  0없음
    ref_Information* temp;
     
    for(temp = q->front ; temp != NULL ; temp = temp->Next) {
        if(temp->Page_num == P.Page_num && temp->Process_num == P.Process_num) {
            temp->Count = P.Count;
            return 1;
        }
    }
    return 0;
}

int arr_pro_num_FIFO_search(List_head arr[], ref_Information P, int frame, int *index) { //FIFO 찾고 처리 1이면 페이지 폴트증가, 0이면 그대로
    int i;
    for(i = 0;  ; i++) {
        if(arr[i].Process_num == -1) { //프로세스번호가 처음 들어올경우
            FIFO_en(&arr[i], P, *index);
            arr[i].Process_num = P.Process_num;
            (*index)++;
            return 1;
        }
        else if(arr[i].Process_num == P.Process_num) {
            if(equal_search(&arr[i], P)) { //같은 것이 있다면
                return 0;
            }
            else {
                if(arr[i].Frame_num < frame) { //프로세스번호가 같고 프레임 수 보다 적을 때
                    FIFO_en(&arr[i], P, *index);
                    (*index)++;
                    return 1;
                }
                else if(arr[i].Frame_num >= frame){ //프로세스번호가 같고 프레임 수 보다 같거나 클 때
                    ref_Information temp;
                    temp = FIFO_de(&arr[i]);
                    FIFO_en(&arr[i], P, temp.Index_count);
                    return 1;
                }
            }
        }
    }
}
////////FIFO 끝
////////LRU 시작
int arr_pro_num_LRU_search(List_head arr[], ref_Information P, int frame, int *index) { //LRU 찾고 처리 1이면 페이지 폴트증가, 0이면 그대로
    int i;
    ref_Information *temp;
    ref_Information *temp2;

    for(i = 0 ; ; i++) {
        if(arr[i].Process_num == -1) {
            FIFO_en(&arr[i], P, *index);
            arr[i].Process_num = P.Process_num;
            (*index)++;
            return 1;
        }
        else if(arr[i].Process_num == P.Process_num) {
            if(equal_search(&arr[i], P)) { //같은 것이 있다면
                return 0;
            }
            else {
                if(arr[i].Frame_num < frame) { //프로세스번호가 같고 프레임 수 보다 적을 때
                    FIFO_en(&arr[i], P, *index);
                    (*index)++;
                    return 1;
                }
                else if(arr[i].Frame_num >= frame) {
                    for(temp = arr[i].front, temp2 = arr[i].front ; temp != NULL ; temp = temp->Next) {
                        if(temp->Count < temp2->Count) { //count가 작다면 
                            temp2 = temp;
                        }
                    }
                    temp2->Page_num = P.Page_num;
                    temp2->Count = P.Count;
                    return 1; 
                }
            }
        }
    }
}
////////LRU 끝

///////OPT 시작
int equal_search_OPT(List_head* q, ref_Information P) { //같은 프로세스번호, 페이지번호 있나 확인 , 1있음  0없음
    ref_Information* temp;
     
    for(temp = q->front ; temp != NULL ; temp = temp->Next) {
        if(temp->Page_num == P.Page_num && temp->Process_num == P.Process_num) {
            return 1;
        }
    }
    return 0;
}

int Future_Search(int count, int Page, ref_Information arr[], int process) {
    int i;
    for(i = count - 1 ; arr[i].Page_num != -1 && arr[i].Process_num != -1 ; i++) {
        if(arr[i].Page_num == Page && process == arr[i].Process_num) {
            return i;
        }
    }
    return 30000;
}

int arr_pro_num_OPT_search(List_head arr[], ref_Information P, int frame, int *index, ref_Information input[]) { //LRU 찾고 처리 1이면 페이지 폴트증가, 0이면 그대로
    int i;
    ref_Information *temp;
    ref_Information *temp2;

    for(i = 0 ; ; i++) {
        if(arr[i].Process_num == -1) {
            FIFO_en(&arr[i], P, *index);
            arr[i].Process_num = P.Process_num;
            (*index)++;
            return 1;
        }
        else if(arr[i].Process_num == P.Process_num) { //프로세스번호가 같다면
            if(equal_search_OPT(&arr[i], P)) { //같은 것이 있다면
                return 0;
            }
            else {
                if(arr[i].Frame_num < frame) { //프로세스번호가 같고 프레임 수 보다 적을 때
                    FIFO_en(&arr[i], P, *index);
                    (*index)++;
                    return 1;
                }

                else {
                    for(temp = arr[i].front ; temp != NULL ; temp = temp->Next) { //미래값 대입
                        temp->Future = Future_Search(P.Count, temp->Page_num, input, temp->Process_num);
                    }

                    for(temp = arr[i].front, temp2 = arr[i].front ; temp != NULL ; temp = temp->Next) {
                        if(temp->Future == temp2->Future) { //같을 경우
                            if(temp->Count < temp2->Count) { //먼저 들어 왔을 경우
                                temp2 = temp;
                            }
                        }
                        else if( ( temp->Future > temp2->Future ) ) { // 다른 경우
                            temp2 = temp;
                        }
                    }

                    temp2->Page_num = P.Page_num;
                    temp2->Count = P.Count;

                    return 1; 
                }
            }
        }
    }
}


///////OPT 끝 

void copy_arr(List_head arr[], ref_Information ref_arr[], int index) { //배열복사함수
    int i, in = 0;
    ref_Information *temp;
    for(i = 0 ; ; i++) {
        if(in == index) {
            break;
        }
        for(temp = arr[i].front ; temp != NULL ; temp = temp->Next) {
            ref_arr[temp->Index_count].Process_num = temp->Process_num;
            ref_arr[temp->Index_count].Page_num = temp->Page_num;
            ref_arr[temp->Index_count].Index_count = temp->Index_count;
            in++;
        }
    }
}

int main() {
    int i;
    int frame;
    int page_fault = 0, index = 0, count = 0;
     
    List_head head_arr[MAX];
    List_head head_arr_LRU[MAX];
    List_head head_arr_OPT[MAX];
    ref_Information ref_arr[30000];
    ref_Information ref_arr_print[10000];

    FILE *file = fopen("page.inp", "rt");
    FILE *file2 = fopen("page.out", "wt");
    fscanf(file, "%d", &frame);

    head_arr_init(head_arr, frame);
    head_arr_init(head_arr_LRU, frame);
    head_arr_init(head_arr_OPT, frame);//리스트헤더배열 값의 초기화

    for(i = 0 ; ; i++) { //배열로 복사 
        count++;
        fscanf(file, "%d %d", &ref_arr[i].Process_num, &ref_arr[i].Page_num);
        ref_arr[i].Count = count;
        ref_arr[i].Future = 0;
        if(ref_arr[i].Process_num == -1 && ref_arr[i].Page_num == -1) 
            break;
    }

    for(i = 0 ; ; i++) { //FIFO, LRU 성공!
        if(ref_arr[i].Process_num == -1 && ref_arr[i].Page_num == -1) 
            break;
   
        ref_arr[i].Index_count = index;

        if(arr_pro_num_FIFO_search(head_arr, ref_arr[i], frame, &index)) {
        //if(arr_pro_num_LRU_search(head_arr, ref_arr[i], frame, &index)) {
        //if(arr_pro_num_OPT_search(head_arr, ref_arr[i], frame, &index, ref_arr)) {
            page_fault++;
        }
    }

    copy_arr(head_arr, ref_arr_print, index);

    fprintf(file2, "FIFO: %d \n", page_fault);
    for(i = 0; i < index ; i++) {
        fprintf(file2, "%d %d %d \n", ref_arr_print[i].Index_count, ref_arr_print[i].Process_num, ref_arr_print[i].Page_num);
    }

     
    page_fault = 0;
    index = 0;
    count = 0;
        for(i = 0 ; ; i++) { //LRU
        if(ref_arr[i].Process_num == -1 && ref_arr[i].Page_num == -1) 
            break;
   
        ref_arr[i].Index_count = index;

        //if(arr_pro_num_FIFO_search(head_arr, ref_arr[i], frame, &index)) {
        if(arr_pro_num_LRU_search(head_arr_LRU, ref_arr[i], frame, &index)) {
        //if(arr_pro_num_OPT_search(head_arr, ref_arr[i], frame, &index, ref_arr)) {
            page_fault++;
        }
    }

    copy_arr(head_arr_LRU, ref_arr_print, index);

    fprintf(file2, "LRU: %d \n", page_fault);
    for(i = 0; i < index ; i++) {
        fprintf(file2, "%d %d %d \n", ref_arr_print[i].Index_count, ref_arr_print[i].Process_num, ref_arr_print[i].Page_num);
    } 

     
    page_fault = 0;
    index = 0;
    count = 0;
        for(i = 0 ; ; i++) { //OPT 성공!
        if(ref_arr[i].Process_num == -1 && ref_arr[i].Page_num == -1) 
            break;
   
        ref_arr[i].Index_count = index;

        //if(arr_pro_num_FIFO_search(head_arr, ref_arr[i], frame, &index)) {
        //if(arr_pro_num_LRU_search(head_arr, ref_arr[i], frame, &index)) {
        if(arr_pro_num_OPT_search(head_arr_OPT, ref_arr[i], frame, &index, ref_arr)) {
            page_fault++;
        }
    }

    copy_arr(head_arr_OPT, ref_arr_print, index);

    fprintf(file2, "OPT: %d \n", page_fault);
    for(i = 0; i < index ; i++) {
        fprintf(file2, "%d %d %d \n", ref_arr_print[i].Index_count, ref_arr_print[i].Process_num, ref_arr_print[i].Page_num);
    }
     
    fclose(file);
    return 0;
}


