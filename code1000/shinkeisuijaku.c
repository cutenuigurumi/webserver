#include<stdio.h>
#include<stdlib.h>
#define MAX_LENGTH 4
#define MAX_WIDTH 13
#define SHUFFLE 20

int describe_card(int card[][MAX_WIDTH]);
//最初のみ使用。カードをランダムに置き換える
int put_card_randam(int card[][MAX_WIDTH]);


int main()
{
	int card_array[MAX_LENGTH][MAX_WIDTH];
	int end_flag = 0;

	put_card_randam(card_array);
	printf("神経衰弱をはじめましょう\n");
	while(end_flag == 0){
		describe_card(card_array);
		play_my_turn(card_array);
		end_flag = 1;	
	}

}

int play_my_turn(int card[][MAX_WIDTH]){

	printf("1枚目に引く、座標を指定してください(x,y)\n");
	scanf("%d %d")
	
	return 0;
}
int describe_card(int card[][MAX_WIDTH])
{
	int i = 0, j = 0;
	for(j = 0;j < 13;j++){
		printf("%2d ",j);
	}
	printf("\n");
	printf("  |-----------------------------------|\n");

	for(i = 0; i < 4; i++){
		printf("%d ", i);
		for(j = 0; j < 13; j++){
			printf("■  ");
		}
		printf("\n");
	}

}
int put_card_randam(int card[][MAX_WIDTH])
{
	int i = 0, j = 0, x1,x2,y1,y2, buffer;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 13; j++){
			card[i][j] = j+1;
		}
	}
	srand(time(0));
	
	int tmp, a,b;
	for(i = 0;i < MAX_LENGTH; i++){
		for(j = 0;j < MAX_WIDTH; j++){
			a = rand()%MAX_WIDTH;
			b = i % MAX_WIDTH;
			tmp = card[i][a];
			card[i][a] = card[i][b];
			card[i][b] = tmp;
		}
	}
/*
	printf("シャッフル後\n");
	for(i = 0; i < 4; i++){
		for(j = 0; j < 13; j++){
			printf("%d ",  card[i][j]);
		}
		printf("\n");
	}
*/	
}

int is_error_check()
{
	
	
}

