#include<stdio.h>
#include<stdlib.h>
#define MAX_LENGTH 4
#define MAX_WIDTH 13
#define MAX_CARD 52
#define SHUFFLE 20
#define ONCE_TURNED_CARD 14
#define	FINISHED_CARD 0

//カードの状態を描写する
int describe_card(int card[][MAX_WIDTH]);
//最初のみ使用。カードをランダムに置き換える
int put_card_randam(int card[][MAX_WIDTH]);


int main()
{
	int card_array[MAX_LENGTH][MAX_WIDTH];
	int sum = 0, my_score = 0, enemy_score = 0;

	put_card_randam(card_array);
	printf("神経衰弱をはじめましょう\n");
	while(sum <= MAX_CARD){
		describe_card(card_array);
		my_score = play_my_turn(card_array);

	}

}

int play_my_turn(int card[][MAX_WIDTH], int once_turned_array[][MAX_WIDTH]){

	int x1,y1,x2,y2,score = 0;
	printf("貴方のターンです\n");
	printf("1枚目に引く、座標を指定してください(x y)\n");
	scanf("%d %d", &x1,&y1);
	printf("x=%d,y=%dをめくります。\n", x1,y1);
	
	printf("2枚目に引く、座標を指定してください(x y)\n");
	scanf("%d %d", &x2,&y2);
	printf("x=%d,y=%dをめくります。\n", x2,y2);
	score = check_cards(x1, y1, x2, y2, card);
	if(score == 1){
		cards[x1][y1] = FINISHED_CARD;
		cards[x2][y2] = FINISHED_CARD;
		return score;
	}
	
	cards[x1][y1] = ONCE_TURNED_CARD;
	once_turned_array[x2][y2] = ONCE_TURNED_CARD;
	return score;
}

int check_cards(int x1,int y1,int x2,int y2,int card[][MAX_WIDTH])
{
	printf("めくったカードは%dと%dでした", card[x1][y1], card[x2][y2]);
	if(card[x1][y1] == card[x2][y2]){
		printf("あたりです\n");
		return 1;
	}
	printf("はずれ\n");
	return 0;

}

int describe_card(int card[][MAX_WIDTH])
{
	int i = 0, j = 0;
	printf("■ まだめくっていない\n");
	printf("X 合ってた！\n");
	printf("x y →\n");
	printf("↓");
	for(j = 0;j < 13;j++){
		printf("%2d ",j);
	}
	printf("\n");
	printf("  |-----------------------------------|\n");

	for(i = 0; i < 4; i++){
		printf("%d ", i);
		for(j = 0; j < 13; j++){
			if(card[i][j] == FINISHED_CARD){
				printf("X  ");
				continue;
			}
			if(card[i][j] == ONCE_TURNED_CARD){
				printf("□  ");
				continue;
			}
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

