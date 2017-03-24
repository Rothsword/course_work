/* bridge */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>


void sort_suit(char array[13], int no);        //sort suits of each player, des order.
void sort_4num(int array[4]);                  //sort 4 numbers, des order.
void show_suits_points(int player_no, char array[][2]);     //print out suits of each player.
char* find_max_three(char card1[2], char card2[2], char card3[2], char contract);  //find biggest card among 3 cards.
char* find_min(char array[][2], char contract);             //the leader play card.
char* second_find(char array[][2], char contract, char precard[2]);         //the first defender play card.
char* third_find(char array[][2], char contract, char precard1[2], char precard2[2]);           //the ruff player play card.
char* forth_find(char array[][2], char contract, char precard1[2], char precard2[2], char precard3[2]);         //the last player play card.
void change_to(char card[2]);           //change card in order to compare.
void change_back(char card[2]);         //change card back.


int main(int argc, char * argv[])
{
    int pid,i,p,q,s;
    int player_no = 0;
    char card_array[13][2];
    int pid_array[4];
    int flag = 0;
    int fd_p[4][2];     //pipe from parent to child.
    int fd_c[4][2];     //pipe from child to parent.
    char *letter = malloc(2*sizeof(char));
    char contract[3];
    char b[2];
    char report[3];     //report* for parent to recieve the cards played by children.
    char report2[3];
    char report3[3];
    char report4[3];
    char start[2];
    char message[3];
    char message2[3];
    char message3[3];   //message* for child recieve the cards played before.

    //printf("%s", *card_array);
    char argv_array[52][2];
    for( s = 0; s < 52; s++){
        if(scanf("%s",argv_array[s]) != 1){
            break;
        }
    }

    for (i = 0; i < 4; i++ ){
        if(pipe(fd_p[i]) < 0 || pipe(fd_c[i]) < 0){
            printf("Pipe pc error\n");
            exit(1);
        }

        pid = fork();

        if ( pid < 0)
        {
            printf("Fork Failed\n");
            exit(1);
        }

        else if ( pid == 0 )
        {
            close(fd_c[i][0]);
            close(fd_p[i][1]);
            player_no = i + 1;
            flag = 0;
            break;
        }
        else{
            close(fd_c[i][1]);
            close(fd_p[i][0]);
            pid_array[i] = pid;
            flag = 1;
        }

    }

    if(flag){          //parent process.
        //char contract[3];

        printf("Parent: the child players are ");
        for (i = 0; i < 4; i++) {
            printf("%d ", pid_array[i]);
        }
        printf("\n");


        for(i = 0; i < 4; i++){
            char x[] = "r";
            write(fd_p[i][1], x, sizeof(char));
            read(fd_c[i][0], b, sizeof(char));
            letter[0]=argv[1][0];
            letter[1]=argv[1][1];
            write(fd_p[i][1], letter, 2*sizeof(char));
        }

        printf("Parent: broadcast contract %s to all children\n", argv[1]);
        int order = 0;
        int count = 0;
        int defender = 0;
        int declaring = 0;

        for(i = 0; i<13; i++){
            char lead[] = "l";
            printf("Parent: child %d to lead in trick %d\n", order+1, ++count);
            write(fd_p[order][1], lead, sizeof(char));
            read(fd_c[order][0], report, 2*sizeof(char));
            printf("Parent: Child %d played %s\n", order+1, report);
            order = order+1;
            if(order >= 4){
                order = order-4;
            }
            char second[] = "s";
            write(fd_p[order][1], second, 2*sizeof(char));
            write(fd_p[order][1], report, 2*sizeof(char));
            read(fd_c[order][0], report2, 2*sizeof(char));
            printf("Parent: Child %d played %s\n", order+1, report2);
            order = order+1;
            if(order >= 4){
                order = order-4;
            }
            char third[] = "t";
            write(fd_p[order][1], third, 2*sizeof(char));
            write(fd_p[order][1], report, 2*sizeof(char));
            write(fd_p[order][1], report2, 2*sizeof(char));
            read(fd_c[order][0], report3, 3*sizeof(char));
            printf("Parent: Child %d played %s\n", order+1, report3);
            order = order+1;
            if(order >= 4){
                order = order-4;
            }
            char forth[] = "f";
            write(fd_p[order][1], forth, 2*sizeof(char));
            write(fd_p[order][1], report, 2*sizeof(char));
            write(fd_p[order][1], report2, 2*sizeof(char));
            write(fd_p[order][1], report3, 2*sizeof(char));
            read(fd_c[order][0], report4, 3*sizeof(char));
            printf("Parent: Child %d played %c%c\n", order+1, report4[0], report4[1]);
            char *max = malloc(2*sizeof(char));
            char *result = malloc(2*sizeof(char));
            change_to(report);
            change_to(report2);
            change_to(report3);
            max = find_max_three(report, report2, report3, argv[1][1]);
            change_to(max);
            change_to(report4);

            //parent decide the winning card.
            if(max[0] == argv[1][1]){
                if(report4[0] != argv[1][1]){
                    result[0] = argv[1][1];
                    result[1] = max[1];
                }
                else{
                    result[0] = argv[1][1];
                    result[1] = max[1] > report4[1] ? max[1] : report4[1];
                }
            }
            else{
                if(report4[0] == argv[1][1]){
                    result[0] = argv[1][1];
                    result[1] = report4[1];
                }
                else{
                    if(report4[0] == max[0]){
                        result[0] = report4[0];
                        result[1] = max[1] > report4[1] ? max[1] : report4[1];
                    }
                    else{
                        result[0] = max[0];
                        result[1] = max[1];
                    }
                }
            }

            change_back(result);
            change_back(report);
            change_back(report2);
            change_back(report3);
            change_back(report4);

            //printf("%s", result);
            if(result[0] == report[0] && result[1] == report[1]){
                if((order-2) <= 0){
                    order = order + 4;
                }
                printf("child %d win the trick\n", order-2);
                order = order-3;
            }
            if(result[0] == report2[0] && result[1] == report2[1]){
                if((order-1) <= 0){
                    order = order + 4;
                }
                printf("child %d win the trick\n", order-1);
                order = order-2;
            }
            if(result[0] == report3[0] && result[1] == report3[1]){
                if(order == 0){
                    order = order + 4;
                }
                printf("child %d win the trick\n", order);
                order = order-1;
            }
            if(result[0] == report4[0] && result[1] == report4[1]){
                if((order+1) > 4){
                    order = order - 4;
                }
                printf("child %d win the trick\n", order+1);
                order = order;
            }
            if((order+1) % 2 == 0){
                declaring++;
            }
            else defender++;
        }
        printf("Parent: declaring side wins %d tricks, defenders win %d tricks\n", declaring, defender);
        int a = argv[1][0] - 48; int b = a+6-declaring;
        if(b > 0){
            printf("Parent: contract %s defeated by %d trick\n", argv[1], b);
        }
        else printf("Parent: contract %s made with %d overtrick\n", argv[1], (-1)*b);

        for(i = 0; i < 4; i++){
            close(fd_p[i][1]);
            close(fd_c[i][0]);
        }   //close pipe.

        for ( i = 0; i < 4; i++){
            wait(NULL);
        }
        //exit(0);
    }
    else{    //child process.
        for (p = 0, q = player_no - 1; q < 53; q += 4) {
            card_array[p][0] = argv_array[q][0];
            card_array[p][1] = argv_array[q][1];
            p++;
        }

        read(fd_p[player_no-1][0], start, sizeof(char));
        show_suits_points(player_no, card_array);
        char x[] = "r";         //r for ready, tell parent he is ready.
        write(fd_c[player_no-1][1], x ,sizeof(char));
        read(fd_p[player_no-1][0], contract, 3*sizeof(char));
        for(i = 0; i < 13; i++){
            read(fd_p[player_no-1][0], message, 2*sizeof(char));
            if(message[0] == 'l'){       //l for leader
                char *card1 = malloc(2*sizeof(char));
                card1 = find_min(card_array, contract[1]);
                printf("child %d: %s\n", player_no, card1);
                write(fd_c[player_no-1][1], card1, 2*sizeof(char));
                //show_suits_points(player_no, card_array);
            }
            if(message[0] == 's'){          //s for second.
                read(fd_p[player_no-1][0], message, 2*sizeof(char));
                char *card2 = malloc(2*sizeof(char));
                card2 = second_find(card_array, contract[1], message);
                printf("child %d: %s\n", player_no, card2);
                write(fd_c[player_no-1][1], card2, 2*sizeof(char));
            }
            if(message[0] == 't'){          //t for third.
                read(fd_p[player_no-1][0], message, 2*sizeof(char));
                read(fd_p[player_no-1][0], message2, 2*sizeof(char));
                char *card3 = malloc(2*sizeof(char));
                card3 = third_find(card_array, contract[1], message, message2);
                printf("child %d: %s\n", player_no, card3);
                write(fd_c[player_no-1][1], card3, 2*sizeof(char));
            }
            if(message[0] == 'f'){          //for for forth.
                read(fd_p[player_no-1][0], message, 2*sizeof(char));
                read(fd_p[player_no-1][0], message2, 2*sizeof(char));
                read(fd_p[player_no-1][0], message3, 2*sizeof(char));
                char *card4 = malloc(2*sizeof(char));
                card4 = forth_find(card_array, contract[1], message, message2, message3);
                printf("child %d: %s\n", player_no, card4);
                write(fd_c[player_no-1][1], card4, 3*sizeof(char));
            }
        }
        close(fd_c[player_no-1][1]);
        close(fd_p[player_no-1][0]);
    }

}

void sort_suit(char array[13], int no){
    int i,j,key;
    for(i = 0; i < no; i++){
        switch (array[i]) {
            case 'T':
                array[i] = ':';
                break;
            case 'J':
                array[i] = ';';
                break;
            case 'Q':
                array[i] = '<';
                break;
            case 'K':
                array[i] = '=';
                break;
            case 'A':
                array[i] = '>';
                break;
        }
    }

    for(i = 1; i < no; i++){
        key = array[i];
        for (j = i - 1; j >= 0 && array[j] < key; j--){
            array[j + 1] = array[j];
        }
        array[j + 1] = key;
    }

    for(i = 0; i < no; i++){
        switch (array[i]) {
            case ':':
                array[i] = 'T';
                break;
            case ';':
                array[i] = 'J';
                break;
            case '<':
                array[i] = 'Q';
                break;
            case '=':
                array[i] = 'K';
                break;
            case '>':
                array[i] = 'A';
                break;
        }
    }
}

void sort_suit_n(char array[13], int no){
    int i,j,key;
    for(i = 0; i < no; i++){
        switch (array[i]) {
            case 'T':
                array[i] = ':';
                break;
            case 'J':
                array[i] = ';';
                break;
            case 'Q':
                array[i] = '<';
                break;
            case 'K':
                array[i] = '=';
                break;
            case 'A':
                array[i] = '>';
                break;
        }
    }

    for(i = 1; i < no; i++){
        key = array[i];
        for (j = i - 1; j >= 0 && array[j] < key; j--){
            array[j + 1] = array[j];
        }
        array[j + 1] = key;
    }
}


void sort_4num(int array[4]){
    int i,j,key;
    for(i = 1; i < 4; i++){
        key = array[i];
        for (j = i - 1; j >= 0 && array[j] < key; j--){
            array[j + 1] = array[j];
        }
        array[j + 1] = key;
    }

}

void show_suits_points(int player_no, char array[][2]){
    int pid = getpid();
    char card_h[13];
    char card_s[13];
    char card_d[13];
    char card_c[13];
    int i, j, no_H=0, no_S=0, no_D=0, no_C=0;

    for( i = 0; i < 13; i++){
        switch (array[i][0]) {
            case 'H':
                card_h[no_H]=array[i][1];
                no_H++;
                break;
            case 'S':
                card_s[no_S]=array[i][1];
                no_S++;
                break;
            case 'D':
                card_d[no_D]=array[i][1];
                no_D++;
                break;
            case 'C':
                card_c[no_C]=array[i][1];
                no_C++;
                break;
        }
    }

    //printf("%d",no_H);
    sort_suit(card_h, no_H);
    sort_suit(card_c, no_C);
    sort_suit(card_s, no_S);
    sort_suit(card_d, no_D);

    int num_array[4];
    num_array[0] = no_D;
    num_array[1] = no_S;
    num_array[2] = no_C;
    num_array[3] = no_H;

    sort_4num(num_array);

    printf("child %d, pid %d: ", player_no, pid);

    int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;

    for(j = 0; j < 4; j++){
        printf("<");
        if (num_array[j] == no_S && flag1 == 0) {
            for (i = 0; i < no_S; i++){
                if(i != 0){
                    printf(" ");
                }
                printf("S%c", card_s[i]);
            }
            flag1 = 1;
            printf("> ");
            continue;
        }
        else if(num_array[j] == no_H && flag2 == 0){
            for (i = 0; i < no_H; i++){
                if(i != 0){
                    printf(" ");
                }
                printf("H%c", card_h[i]);
            }
            flag2 = 1;
            printf("> ");
            continue;
        }
        else if(num_array[j] == no_C && flag3 == 0){
            for (i = 0; i < no_C; i++){
                if(i != 0){
                    printf(" ");
                }
                printf("C%c", card_c[i]);
            }
            flag3 = 1;
            printf("> ");
            continue;
        }
        else if(num_array[j] == no_D && flag4 == 0){
            for (i = 0; i < no_D; i++){
                if(i != 0){
                    printf(" ");
                }
                printf("D%c", card_d[i]);
            }
            flag4 = 1;
            printf("> ");
            continue;
        }
    }
    printf("\n");

}

char* find_min(char array[][2], char contract){
    char card_h[13];
    char card_s[13];
    char card_d[13];
    char card_c[13];
    int i, no_H=0, no_S=0, no_D=0, no_C=0;

    for( i = 0; i < 13; i++){
        switch (array[i][0]) {
            case 'H':
                card_h[no_H]=array[i][1];
                no_H++;
                break;
            case 'S':
                card_s[no_S]=array[i][1];
                no_S++;
                break;
            case 'D':
                card_d[no_D]=array[i][1];
                no_D++;
                break;
            case 'C':
                card_c[no_C]=array[i][1];
                no_C++;
                break;
        }
    }

    sort_suit_n(card_h, no_H);
    sort_suit_n(card_c, no_C);
    sort_suit_n(card_s, no_S);
    sort_suit_n(card_d, no_D);


    char *min = malloc(2*sizeof(char));
    int a = 0,b = 0,c = 0,d = 0,count = 0;

    if(contract == 'N'){
        if(no_C != 0){
            a = (card_c[no_C-1]-48) * 4 - 3;
            count++;
        }
        else a = 100;
        if (no_D != 0){
            b = (card_d[no_D-1]-48) * 4 - 2;
            count++;
        }
        else b = 100;
        if (no_H != 0){
            c = (card_h[no_H-1]-48) * 4 - 1;
            count++;
        }
        else c = 100;
        if (no_S != 0){
            d = (card_s[no_S-1]-48) * 4;
            count++;
        }
        else d = 100;
    }
    else{
        switch (contract) {
            case 'H':
                if(no_H == (no_H-no_S-no_C-no_D)){
                    min[0] = 'H';
                    min[1] = card_h[no_H-1];
                    goto change;
                }
                c = 100;
                if(no_C != 0){
                    a = (card_c[no_C-1]-48) * 4 - 3;
                    count++;
                }
                else a = 100;
                if (no_D != 0){
                    b = (card_d[no_D-1]-48) * 4 - 2;
                    count++;
                }
                else b = 100;
                if (no_S != 0){
                    d = (card_s[no_S-1]-48) * 4;
                    count++;
                }
                else d = 100;
                break;
            case 'D':
                if(no_D == (no_D-no_H-no_C-no_S)){
                    min[0] = 'D';
                    min[1] = card_d[no_D-1];
                    goto change;
                }
                b = 100;
                if(no_C != 0){
                    a = (card_c[no_C-1]-48) * 4 - 3;
                    count++;
                }
                else a = 100;
                if (no_H != 0){
                    c = (card_h[no_H-1]-48) * 4 - 1;
                    count++;
                }
                else c = 100;
                if (no_S != 0){
                    d = (card_s[no_S-1]-48) * 4;
                    count++;
                }
                else d = 100;
                break;

            case 'S':
                if(no_S == (no_S-no_H-no_C-no_D)){
                    min[0] = 'S';
                    min[1] = card_s[no_S-1];
                    goto change;
                }
                d = 100;
                if(no_C != 0){
                    a = (card_c[no_C-1]-48) * 4 - 3;
                    count++;
                }
                else a = 100;
                if (no_D != 0){
                    b = (card_d[no_D-1]-48) * 4 - 2;
                    count++;
                }
                else b = 100;
                if (no_H != 0){
                    c = (card_h[no_H-1]-48) * 4 - 1;
                    count++;
                }
                else c = 100;
                break;

            case 'C':
                if(no_C == (no_C-no_H-no_S-no_D)){
                    min[0] = 'C';
                    min[1] = card_c[no_C-1];
                    goto change;
                }
                a = 100;
                if (no_D != 0){
                    b = (card_d[no_D-1]-48) * 4 - 2;
                    count++;
                }
                else b = 100;
                if (no_H != 0){
                    c = (card_h[no_H-1]-48) * 4 - 1;
                    count++;
                }
                else c = 100;
                if (no_S != 0){
                    d = (card_s[no_S-1]-48) * 4;
                    count++;
                }
                else d = 100;
                break;
            }

    }
    int array_num[4];
    array_num[0]=a;
    array_num[1]=b;
    array_num[2]=c;
    array_num[3]=d;
    sort_4num(array_num);
    int num = array_num[3];
    switch (num % 4) {
        case 0:
            min[0] = 'S';
            break;
        case 1:
            min[0] = 'C';
            break;
        case 2:
            min[0] = 'D';
            break;
        case 3:
            min[0] = 'H';
            break;
    }
    if(num % 4 != 0){
        min[1] = num/4 + 49;
    }
    else min[1] = num/4 + 48;


    change:
    change_back(min);

    for( i = 0; i < 13; i++){
        if(array[i][0] == min[0] && array[i][1] == min[1]){
            array[i][0] = 'U';
            array[i][1] = 'D';
            break;
        }
    }

    return min;
}

char* second_find(char array[][2], char contract, char precard[2]){
    char *card = malloc(2*sizeof(char));
    char card_h[13];
    char card_s[13];
    char card_d[13];
    char card_c[13];
    int i=0, no_H=0, no_S=0, no_D=0, no_C=0;

    for( i = 0; i < 13; i++){
        switch (array[i][0]) {
            case 'H':
                card_h[no_H]=array[i][1];
                no_H++;
                break;
            case 'S':
                card_s[no_S]=array[i][1];
                no_S++;
                break;
            case 'D':
                card_d[no_D]=array[i][1];
                no_D++;
                break;
            case 'C':
                card_c[no_C]=array[i][1];
                no_C++;
                break;
        }
    }

    change_to(precard);


    sort_suit_n(card_h, no_H);
    sort_suit_n(card_c, no_C);
    sort_suit_n(card_s, no_S);
    sort_suit_n(card_d, no_D);

    switch (precard[0]) {
        case 'H':
            if(no_H != 0){
                if(card_h[0] < precard[1]){
                    card[0] = 'H';
                    card[1] = card_h[no_H-1];
                    goto change;
                }
                for( i = 0; i < no_H; i++){
                    if(card_h[no_H-1-i] > precard[1]){
                        card[0] = 'H';
                        card[1] = card_h[no_H-1-i];
                        goto change;
                    }
                }
            }
            else if(contract == 'S'){
                if(no_S != 0){
                    card[0] = 'S';
                    card[1] = card_s[no_S-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            else if(contract == 'C'){
                if(no_C != 0){
                    card[0] = 'C';
                    card[1] = card_c[no_C-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            else if(contract == 'D'){
                if(no_D != 0){
                    card[0] = 'D';
                    card[1] = card_d[no_D-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            return find_min(array, contract);
            break;

        case 'S':
            if(no_S != 0){
                if(card_s[0] < precard[1]){
                    card[0] = 'S';
                    card[1] = card_s[no_S-1];
                    goto change;
                }
                for( i = 0; i < no_S; i++){
                    if(card_s[no_S-1-i] > precard[1]){
                        card[0] = 'S';
                        card[1] = card_s[no_S-1-i];
                        goto change;
                    }
                }
            }
            else if(contract == 'H'){
                if(no_H != 0){
                    card[0] = 'H';
                    card[1] = card_h[no_H-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            else if(contract == 'C'){
                if(no_C != 0){
                    card[0] = 'C';
                    card[1] = card_c[no_C-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            else if(contract == 'D'){
                if(no_D != 0){
                    card[0] = 'D';
                    card[1] = card_d[no_D-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            return find_min(array, contract);
            break;

        case 'C':
            if(no_C != 0){
                if(card_c[0] < precard[1]){
                    card[0] = 'C';
                    card[1] = card_c[no_C-1];
                    goto change;
                }
                for( i = 0; i < no_C; i++){
                    if(card_c[no_C-1-i] > precard[1]){
                        card[0] = 'C';
                        card[1] = card_c[no_C-1-i];
                        goto change;
                    }
                }
            }
            else if(contract == 'S'){
                if(no_S != 0){
                    card[0] = 'S';
                    card[1] = card_s[no_S-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            else if(contract == 'H'){
                if(no_H != 0){
                    card[0] = 'H';
                    card[1] = card_h[no_H-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            else if(contract == 'D'){
                if(no_D != 0){
                    card[0] = 'D';
                    card[1] = card_d[no_D-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            return find_min(array, contract);
            break;

        case 'D':
            if(no_D != 0){
                if(card_d[0] < precard[1]){
                    card[0] = 'D';
                    card[1] = card_d[no_D-1];
                    goto change;
                }
                for( i = 0; i < no_D; i++){
                    if(card_d[no_D-1-i] > precard[1]){
                        card[0] = 'D';
                        card[1] = card_d[no_D-1-i];
                        goto change;
                    }
                }
            }
            else if(contract == 'S'){
                if(no_S != 0){
                    card[0] = 'S';
                    card[1] = card_s[no_S-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            else if(contract == 'C'){
                if(no_C != 0){
                    card[0] = 'C';
                    card[1] = card_c[no_C-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            else if(contract == 'H'){
                if(no_H != 0){
                    card[0] = 'H';
                    card[1] = card_h[no_H-1];
                    goto change;
                }
                else return find_min(array, contract);
            }
            return find_min(array, contract);
            break;

    }

    change:
    change_back(card);

    for( i = 0; i < 13; i++){
        if(array[i][0] == card[0] && array[i][1] == card[1]){
            array[i][0] = 'U';
            array[i][1] = 'D';
            break;
        }
    }

    return card;
}

char* third_find(char array[][2], char contract, char precard1[2], char precard2[2]){
    char *card = malloc(2*sizeof(char));
    char card_h[13];
    char card_s[13];
    char card_d[13];
    char card_c[13];
    int i=0, no_H=0, no_S=0, no_D=0, no_C=0;

    for( i = 0; i < 13; i++){
        switch (array[i][0]) {
            case 'H':
                card_h[no_H]=array[i][1];
                no_H++;
                break;
            case 'S':
                card_s[no_S]=array[i][1];
                no_S++;
                break;
            case 'D':
                card_d[no_D]=array[i][1];
                no_D++;
                break;
            case 'C':
                card_c[no_C]=array[i][1];
                no_C++;
                break;
        }
    }
    change_to(precard1);
    change_to(precard2);

    sort_suit_n(card_h, no_H);
    sort_suit_n(card_c, no_C);
    sort_suit_n(card_s, no_S);
    sort_suit_n(card_d, no_D);



    switch (precard1[0]) {
        case 'H':
            if(no_H != 0){
                if(precard2[0] == 'H'){
                    if(card_h[0] > precard1[1] && card_h[0] > precard2[1]){
                        card[0] = 'H';
                        card[1] = card_h[0];
                    }
                    else{
                        card[0] = 'H';
                        card[1] = card_h[no_H-1];
                    }
                    goto change;
                }
                else{
                    if(precard2[0] == contract){
                        card[0] = precard1[0];
                        card[1] = card_h[no_H-1];
                    }
                    else{
                        card[0] = precard1[0];
                        card[1] = card_h[0];
                    }
                    goto change;
                }
            }
            else{
                if(precard2[0] == contract){
                    if(contract == 'S'){
                        if(no_S != 0){
                            for(i = 0; i < no_S; i++){
                                if(card_s[no_S-1-i] > precard2[1]){
                                    card[0] = 'S';
                                    card[1] = card_s[no_S-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'D'){
                        if(no_D != 0){
                            for(i = 0; i < no_D; i++){
                                if(card_d[no_D-1-i] > precard2[1]){
                                    card[0] = 'D';
                                    card[1] = card_d[no_D-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'C'){
                        if(no_C != 0){
                            for(i = 0; i < no_C; i++){
                                if(card_c[no_C-1-i] > precard2[1]){
                                    card[0] = 'C';
                                    card[1] = card_c[no_C-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                }
                else{
                    if(contract == 'S'){
                        if(no_S != 0){
                            card[0] = 'S';
                            card[1] = card_s[no_S-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'D'){
                        if(no_D != 0){
                            card[0] = 'D';
                            card[1] = card_d[no_D-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'C'){
                        if(no_C != 0){
                            card[0] = 'C';
                            card[1] = card_c[no_C-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    return find_min(array, contract);
                }

            }
            break;

        case 'S':
            if(no_S != 0){
                if(precard2[0] == 'S'){
                    if(card_s[0] > precard1[1] && card_s[0] > precard2[1]){
                        card[0] = 'S';
                        card[1] = card_s[0];
                    }
                    else{
                        card[0] = 'S';
                        card[1] = card_s[no_S-1];
                    }
                    goto change;
                }
                else{
                    if(precard2[0] == contract){
                        card[0] = precard1[0];
                        card[1] = card_s[no_S-1];
                    }
                    else{
                        card[0] = precard1[0];
                        card[1] = card_s[0];
                    }
                    goto change;

                }
            }
            else{
                if(precard2[0] == contract){
                    if(contract == 'H'){
                        if(no_H != 0){
                            for(i = 0; i < no_H; i++){
                                if(card_h[no_H-1-i] > precard2[1]){
                                    card[0] = 'H';
                                    card[1] = card_h[no_H-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'D'){
                        if(no_D != 0){
                            for(i = 0; i < no_D; i++){
                                if(card_d[no_D-1-i] > precard2[1]){
                                    card[0] = 'D';
                                    card[1] = card_d[no_D-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'C'){
                        if(no_C != 0){
                            for(i = 0; i < no_C; i++){
                                if(card_c[no_C-1-i] > precard2[1]){
                                    card[0] = 'C';
                                    card[1] = card_c[no_C-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                }
                else{
                    if(contract == 'H'){
                        if(no_H != 0){
                            card[0] = 'H';
                            card[1] = card_h[no_H-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'D'){
                        if(no_D != 0){
                            card[0] = 'D';
                            card[1] = card_d[no_D-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'C'){
                        if(no_C != 0){
                            card[0] = 'C';
                            card[1] = card_c[no_C-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    return find_min(array, contract);
                }

            }
            break;

        case 'C':
            if(no_C != 0){
                if(precard2[0] == 'C'){
                    if(card_c[0] > precard1[1] && card_c[0] > precard2[1]){
                        card[0] = 'C';
                        card[1] = card_c[0];
                    }
                    else{
                        card[0] = 'C';
                        card[1] = card_c[no_C-1];
                    }
                    goto change;
                }
                else{
                    if(precard2[0] == contract){
                        card[0] = precard1[0];
                        card[1] = card_c[no_C-1];
                    }
                    else{
                        card[0] = precard1[0];
                        card[1] = card_c[0];
                    }
                    goto change;

                }
            }
            else{
                if(precard2[0] == contract){
                    if(contract == 'S'){
                        if(no_S != 0){
                            for(i = 0; i < no_S; i++){
                                if(card_s[no_S-1-i] > precard2[1]){
                                    card[0] = 'S';
                                    card[1] = card_s[no_S-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'D'){
                        if(no_D != 0){
                            for(i = 0; i < no_D; i++){
                                if(card_d[no_D-1-i] > precard2[1]){
                                    card[0] = 'D';
                                    card[1] = card_d[no_D-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'H'){
                        if(no_H != 0){
                            for(i = 0; i < no_H; i++){
                                if(card_h[no_H-1-i] > precard2[1]){
                                    card[0] = 'H';
                                    card[1] = card_h[no_H-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                }
                else{
                    if(contract == 'S'){
                        if(no_S != 0){
                            card[0] = 'S';
                            card[1] = card_s[no_S-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'D'){
                        if(no_D != 0){
                            card[0] = 'D';
                            card[1] = card_d[no_D-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'H'){
                        if(no_H != 0){
                            card[0] = 'H';
                            card[1] = card_h[no_H-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    return find_min(array, contract);
                }

            }
            break;

        case 'D':
            if(no_D != 0){
                if(precard2[0] == 'D'){
                    if(card_d[0] > precard1[1] && card_d[0] > precard2[1]){
                        card[0] = 'D';
                        card[1] = card_d[0];
                    }
                    else{
                        card[0] = 'D';
                        card[1] = card_d[no_D-1];
                    }
                    goto change;
                }
                else{
                    if(precard2[0] == contract){
                        card[0] = precard1[0];
                        card[1] = card_d[no_D-1];
                    }
                    else{
                        card[0] = precard1[0];
                        card[1] = card_d[0];
                    }
                    goto change;
                }
            }
            else{
                if(precard2[0] == contract){
                    if(contract == 'S'){
                        if(no_S != 0){
                            for(i = 0; i < no_S; i++){
                                if(card_s[no_S-1-i] > precard2[1]){
                                    card[0] = 'S';
                                    card[1] = card_s[no_S-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);

                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'H'){
                        if(no_H != 0){
                            for(i = 0; i < no_H; i++){
                                if(card_h[i] > precard2[1]){
                                    card[0] = 'H';
                                    card[1] = card_h[i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'C'){
                        if(no_C != 0){
                            for(i = 0; i < no_C; i++){
                                if(card_c[no_C-1-i] > precard2[1]){
                                    card[0] = 'C';
                                    card[1] = card_c[no_C-1-i];
                                    goto change;
                                }
                            }
                            return find_min(array, contract);
                        }
                        else return find_min(array, contract);
                    }
                }
                else{
                    if(contract == 'S'){
                        if(no_S != 0){
                            card[0] = 'S';
                            card[1] = card_s[no_S-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'H'){
                        if(no_H != 0){
                            card[0] = 'H';
                            card[1] = card_h[no_H-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    if(contract == 'C'){
                        if(no_C != 0){
                            card[0] = 'C';
                            card[1] = card_c[no_C-1];
                            goto change;
                        }
                        else return find_min(array, contract);
                    }
                    return find_min(array, contract);
                }

            }
            break;



    }

    change:
    change_back(card);

    for( i = 0; i < 13; i++){
        if(array[i][0] == card[0] && array[i][1] == card[1]){
            array[i][0] = 'U';
            array[i][1] = 'D';
            break;
        }
    }


    return card;
}

char* forth_find(char array[][2], char contract, char precard1[2], char precard2[2], char precard3[2]){
    char *card = malloc(2*sizeof(char));
    char card_h[13];
    char card_s[13];
    char card_d[13];
    char card_c[13];
    int i=0, no_H=0, no_S=0, no_D=0, no_C=0;

    for( i = 0; i < 13; i++){
        switch (array[i][0]) {
            case 'H':
                card_h[no_H]=array[i][1];
                no_H++;
                break;
            case 'S':
                card_s[no_S]=array[i][1];
                no_S++;
                break;
            case 'D':
                card_d[no_D]=array[i][1];
                no_D++;
                break;
            case 'C':
                card_c[no_C]=array[i][1];
                no_C++;
                break;
        }
    }
    change_to(precard1);
    change_to(precard2);
    change_to(precard3);

    sort_suit_n(card_h, no_H);
    sort_suit_n(card_c, no_C);
    sort_suit_n(card_s, no_S);
    sort_suit_n(card_d, no_D);

    char *max = malloc(2*sizeof(char));
    max = find_max_three(precard1, precard2, precard3, contract);
    printf("%s", max);

    switch (precard1[0]) {
        case 'H':
            if(no_H != 0){
                if(max[0] == precard2[0] && max[1] == precard2[1]){
                    card[0] = precard1[0];
                    card[1] = card_h[no_H-1];
                    goto change;
                }
                if(max[0] == precard1[0]){
                    for(i = 0; i < no_H; i++){
                        if(card_h[no_H-1-i] > max[1]){
                            card[0] = precard1[0];
                            card[1] = card_h[no_H-1-i];
                            goto change;
                        }
                    }
                    card[0] = precard1[0];
                    card[1] = card_h[no_H-1];
                    goto change;
                }
                if(max[0] != precard1[0]){
                    card[0] = precard1[0];
                    card[1] = card_h[no_H-1];
                    goto change;
                }
            }
            else{
                if(max[0] == precard2[0] && max[1] == precard2[1]){
                    return find_min(array, contract);
                }
                if(max[0] == precard1[0]){
                    if(precard1[0] == contract){
                        return find_min(array, contract);
                    }
                    else{
                        char temp[13];
                        int no=0;
                        for(i = 0; i < 13; i++){
                            if(array[i][0] == contract){
                                temp[no] = array[i][1];
                                no++;
                            }
                        }
                        sort_suit_n(temp, no);
                        if(no != 0){
                            card[0] = contract;
                            card[1] = temp[no-1];
                            goto change;
                        }
                        else{
                            return find_min(array, contract);
                        }
                    }
                }
                if(max[0] != precard1[0]){
                    char temp[13];
                    int no=0;
                    for(i = 0; i < 13; i++){
                        if(array[i][0] == contract){
                            temp[no] = array[i][1];
                            no++;
                        }
                    }
                    sort_suit_n(temp, no);
                    if(no != 0){
                        if(temp[0] > max[1]){
                            for(i=0; i<no; i++){
                                if(temp[no-1-i] > max[1]){
                                    card[0] = contract;
                                    card[1] = temp[no-1-i];
                                    goto change;
                                }
                            }
                        }
                        else return find_min(array, contract);
                    }
                    else{
                        return find_min(array, contract);
                    }

                }
            }
            break;

        case 'S':
            if(no_S != 0){
                if(max[0] == precard2[0] && max[1] == precard2[1]){
                    card[0] = precard1[0];
                    card[1] = card_s[no_S-1];
                    goto change;
                }
                if(max[0] == precard1[0]){
                    for(i = 0; i < no_S; i++){
                        if(card_s[no_S-1-i] > max[1]){
                            card[0] = precard1[0];
                            card[1] = card_s[no_S-1-i];
                            goto change;
                        }
                    }
                    card[0] = precard1[0];
                    card[1] = card_s[no_S-1];
                    goto change;
                }
                if(max[0] != precard1[0]){
                    card[0] = precard1[0];
                    card[1] = card_s[no_S-1];
                    goto change;
                }
            }
            else{
                if(max[0] == precard2[0] && max[1] == precard2[1]){
                    return find_min(array, contract);
                }
                if(max[0] == precard1[0]){
                    if(precard1[0] == contract){
                        return find_min(array, contract);
                    }
                    else{
                        char temp[13];
                        int no=0;
                        for(i = 0; i < 13; i++){
                            if(array[i][0] == contract){
                                temp[no] = array[i][1];
                                no++;
                            }
                        }
                        sort_suit_n(temp, no);
                        if(no != 0){
                            card[0] = contract;
                            card[1] = temp[no-1];
                            goto change;
                        }
                        else{
                            return find_min(array, contract);
                        }
                    }
                }
                if(max[0] != precard1[0]){
                    char temp[13];
                    int no=0;
                    for(i = 0; i < 13; i++){
                        if(array[i][0] == contract){
                            temp[no] = array[i][1];
                            no++;
                        }
                    }
                    sort_suit_n(temp, no);
                    if(no != 0){
                        if(temp[0] > max[1]){
                            for(i=0; i<no; i++){
                                if(temp[no-1-i] > max[1]){
                                    card[0] = contract;
                                    card[1] = temp[no-1-i];
                                    goto change;
                                }
                            }
                        }
                        else return find_min(array, contract);
                    }
                    else{
                        return find_min(array, contract);
                    }

                }
            }
            break;

        case 'D':
            if(no_D != 0){
                if(max[0] == precard2[0] && max[1] == precard2[1]){
                    card[0] = precard1[0];
                    card[1] = card_d[no_D-1];
                    goto change;
                }
                if(max[0] == precard1[0]){
                    for(i = 0; i < no_D; i++){
                        if(card_d[no_D-1-i] > max[1]){
                            card[0] = precard1[0];
                            card[1] = card_d[no_D-1-i];
                            goto change;
                        }
                    }
                    card[0] = precard1[0];
                    card[1] = card_d[no_D-1];
                    goto change;
                }
                if(max[0] != precard1[0]){
                    card[0] = precard1[0];
                    card[1] = card_d[no_D-1];
                    goto change;
                }
            }
            else{
                if(max[0] == precard2[0] && max[1] == precard2[1]){
                    return find_min(array, contract);
                }
                if(max[0] == precard1[0]){
                    if(precard1[0] == contract){
                        return find_min(array, contract);
                    }
                    else{
                        char temp[13];
                        int no=0;
                        for(i = 0; i < 13; i++){
                            if(array[i][0] == contract){
                                temp[no] = array[i][1];
                                no++;
                            }
                        }
                        sort_suit_n(temp, no);
                        if(no != 0){
                            card[0] = contract;
                            card[1] = temp[no-1];
                            goto change;
                        }
                        else{
                            return find_min(array, contract);
                        }
                    }
                }
                if(max[0] != precard1[0]){
                    char temp[13];
                    int no=0;
                    for(i = 0; i < 13; i++){
                        if(array[i][0] == contract){
                            temp[no] = array[i][1];
                            no++;
                        }
                    }
                    sort_suit_n(temp, no);
                    if(no != 0){
                        if(temp[0] > max[1]){
                            for(i=0; i<no; i++){
                                if(temp[no-1-i] > max[1]){
                                    card[0] = contract;
                                    card[1] = temp[no-1-i];
                                    goto change;
                                }
                            }
                        }
                        else return find_min(array, contract);
                    }
                    else{
                        return find_min(array, contract);
                    }

                }
            }
            break;

        case 'C':
            if(no_C != 0){
                if(max[0] == precard2[0] && max[1] == precard2[1]){
                    card[0] = precard1[0];
                    card[1] = card_c[no_C-1];
                    goto change;
                }
                if(max[0] == precard1[0]){
                    for(i = 0; i < no_C; i++){
                        if(card_c[no_C-1-i] > max[1]){
                            card[0] = precard1[0];
                            card[1] = card_c[no_C-1-i];
                            goto change;
                        }
                    }
                    card[0] = precard1[0];
                    card[1] = card_c[no_C-1];
                    goto change;
                }
                if(max[0] != precard1[0]){
                    card[0] = precard1[0];
                    card[1] = card_c[no_C-1];
                    goto change;
                }
            }
            else{
                if(max[0] == precard2[0] && max[1] == precard2[1]){
                    return find_min(array, contract);
                }
                if(max[0] == precard1[0]){
                    if(precard1[0] == contract){
                        return find_min(array, contract);
                    }
                    else{
                        char temp[13];
                        int no=0;
                        for(i = 0; i < 13; i++){
                            if(array[i][0] == contract){
                                temp[no] = array[i][1];
                                no++;
                            }
                        }
                        sort_suit_n(temp, no);
                        if(no != 0){
                            card[0] = contract;
                            card[1] = temp[no-1];
                            goto change;
                        }
                        else{
                            return find_min(array, contract);
                        }
                    }
                }
                if(max[0] != precard1[0]){
                    char temp[13];
                    int no=0;
                    for(i = 0; i < 13; i++){
                        if(array[i][0] == contract){
                            temp[no] = array[i][1];
                            no++;
                        }
                    }
                    sort_suit_n(temp, no);
                    if(no != 0){
                        if(temp[0] > max[1]){
                            for(i=0; i<no; i++){
                                if(temp[no-1-i] > max[1]){
                                    card[0] = contract;
                                    card[1] = temp[no-1-i];
                                    goto change;
                                }
                            }
                        }
                        else return find_min(array, contract);
                    }
                    else{
                        return find_min(array, contract);
                    }

                }
            }
            break;

    }

    change:
    change_back(card);

    for( i = 0; i < 13; i++){
        if(array[i][0] == card[0] && array[i][1] == card[1]){
            array[i][0] = 'U';
            array[i][1] = 'D';
            break;
        }
    }


    return card;
}

char* find_max_three(char card1[2], char card2[2], char card3[2], char contract){
    char *card = malloc(2*sizeof(char));
    if(card1[0] == card2[0] && card2[0] == card3[0]){
        card[0] = card1[0];
        card[1] = card1[1] > card2[1] ? card1[1] : card2[1];
        card[1] = card[1] > card3[1] ? card[1] : card3[1];
        return card;
    }
    if(card1[0] != contract){
        if(card2[0] != contract && card3[0] != contract){
            if(card2[0] == card1[0]){
                card[0] = card1[0];
                card[1] = card1[1] > card2[1] ? card1[1] : card2[1];
                return card;
            }
            if(card3[0] == card1[0]){
                card[0] = card1[0];
                card[1] = card1[1] > card3[1] ? card1[1] : card3[1];
                return card;
            }
            return card1;
        }
        if(card2[0] == contract && card3[0] != contract){
            card[0] = card2[0];
            card[1] = card2[1];
            return card;
        }
        if(card2[0] != contract && card3[0] == contract){
            card[0] = card3[0];
            card[1] = card3[1];
            return card;
        }
        if(card2[0] == contract && card3[0] == contract){
            card[0] = card2[0];
            card[1] = card2[1] > card3[1] ? card2[1] : card3[1];
            return card;
        }
    }
    else{
        if(card2[0] != contract && card3[0] != contract){
            card[0] = card1[0];
            card[1] = card1[1];
            return card;
        }
        if(card2[0] == contract && card3[0] != contract){
            card[0] = card1[0];
            card[1] = card1[1] > card2[1] ? card1[1] : card2[1];
            return card;
        }
        if(card2[0] != contract && card3[0] == contract){
            card[0] = card1[0];
            card[1] = card1[1] > card3[1] ? card1[1] : card3[1];
            return card;
        }
    }
    return card;
}

void change_to(char card[2]){
    switch (card[1]) {
        case 'T':
            card[1] = ':';
            break;
        case 'J':
            card[1] = ';';
            break;
        case 'Q':
            card[1] = '<';
            break;
        case 'K':
            card[1] = '=';
            break;
        case 'A':
            card[1] = '>';
            break;
    }
}

void change_back(char card[2]){
    switch (card[1]) {
        case ':':
            card[1] = 'T';
            break;
        case ';':
            card[1] = 'J';
            break;
        case '<':
            card[1] = 'Q';
            break;
        case '=':
            card[1] = 'K';
            break;
        case '>':
            card[1] = 'A';
            break;
    }
}
