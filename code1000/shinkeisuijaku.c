#include<stdio.h>
#include<stdlib.h>

#define MAX_LENGTH 13
#define MAX_WIDTH 4
#define MAX_CARD 52
#define SHUFFLE 20
#define ONCE_TURNED_CARD 2
#define	FINISHED_CARD 1
#define INIT_CARD 0

//カードの状態を描写する
int describe_card(int card[][MAX_LENGTH],  int information_card[][MAX_LENGTH]);
//最初のみ使用。カードをランダムに置き換える
int put_card_randam(int card[][MAX_LENGTH],int information_card[][MAX_LENGTH]);
//カードが同じ数字かどうかを判定する関数
int check_cards(int x1,int y1,int x2,int y2,int card[][MAX_LENGTH]);
//自分のターンを実行する関数
int play_my_turn(int card[][MAX_LENGTH], int information_card[][MAX_LENGTH]);
//敵のターンを実行する関数
int play_enemy_turn(int card[][MAX_LENGTH], int information_card[][MAX_LENGTH]);
//play_enemy_turnの中で敵が引くカードを作る関数
void draw_enemy_card(int *x1,int *y1,int information_card[][MAX_LENGTH]);
//二つの整数型の入力を受け付ける関数
void scanf_two_int(int *, int *);
//カードのエラーチェック
int is_check_error_number(int input1, int input2,int information_card[][MAX_LENGTH]);
//敵の引いたカードのエラーチェック
int is_check_error_number_for_enemy(int **x,int **y);
//あたっていた時の判定
void func_mark_to_card_strike(int x1,int x2,int y1,int y2,int information_card[][MAX_LENGTH]);
//外れた時の判定
void func_mark_to_card_miss(int x1,int x2,int y1,int y2,int information_card[][MAX_LENGTH]);
//重複の処理
int is_checked_overlap(int *x,int *y, int information_card[][MAX_LENGTH]);


int main()
{
	/*変数解説
	*card_array 1〜13までのカードに書いてある数値が格納されている
	*information_card 既にその場にないカード、一回ひっくり返した事のあるカード、まだ
	*ひっくり返した事の無いカード情報が格納されている
	*/
	int card_array[MAX_WIDTH][MAX_LENGTH], information_card[MAX_WIDTH][MAX_LENGTH];
	int end_flag = 0,sum = 0, my_score = 0, enemy_score = 0;

	put_card_randam(card_array, information_card);
	printf("神経衰弱をはじめましょう\n");

	do{
		while(sum <= MAX_CARD / 2){
			my_score = play_my_turn(card_array, information_card);
			enemy_score = play_enemy_turn(card_array, information_card);	
			sum = my_score + enemy_score;
		}
		if(my_score > enemy_score){ 
			printf("貴方の勝ち\n");
		} else if(my_score == enemy_score) {
			printf("引き分け\n");
		} else {
			printf("敵の勝ち\n");
		}
		printf("もう一回遊ぶ？Yes 1: No:0");
		end_flag = scanf_int();
	}while(end_flag);

	exit (0);
}

/* ----------------------------------------------------------- *
 *  play_enemy_turn:敵のターンの処理
 *  引数： 数値型の二次元配列 card[][MAX_WIDTH]:カードの番号を格納
 *  information_card[][MAX_WIDTH]:成功して既に場にないカード、一度引いたカード
 *  まだ引かれてないカードの値が格納
 *  戻り値 数値型 score得点。あたっていたら1を返す。あたっていなかったら0 
 * ----------------------------------------------------------- */
int play_enemy_turn(int card[][MAX_LENGTH], int information_card[][MAX_LENGTH]){

	int x1,y1,x2,y2,score = 0,i,j,k, error_flag = 0;
	describe_card(card, information_card);

	do{
		printf("敵のターンです\n");
		//敵のカードを引く処理
		draw_enemy_card(&x1,&y1, information_card);
		printf("1枚目はx=%d,y=%dをめくります。\n", x1,y1);
	
		printf("2枚目に引く、座標を指定してください(x y)\n");
		//敵のカードを引く処理
		draw_enemy_card(&x2,&y2, information_card);
		if(x1 == y1 && x2 == y2){
			printf("エラー！xとyに同じ座標は選べません。\n");
			error_flag = -1;
			continue;
		}
	}while(error_flag == -1);

	printf("2枚目はx=%d,y=%dをめくります。\n", x2,y2);
	score = check_cards(x1, y1, x2, y2, card);
	if(score == 1){
		func_mark_to_card_strike(x1,x2,y1,y2,information_card);
		return score;
	}
	func_mark_to_card_miss(x1,x2,y1,y2,information_card);

}

/* ----------------------------------------------------------- *
 *  draw_enemy_card:敵がカードを引く処理
 *  引数：int *x,int *y：x座標とy座標
 *  int information_card[][MAX_LENGTH]:カードの情報が格納
 *  戻り値　明示的なものは無し。(xとyが実質的な戻り値)
 * ----------------------------------------------------------- */
void draw_enemy_card(int *x,int *y,int information_card[][MAX_LENGTH])
{
	int overlap_error_flag = 0,error_flag = 0;
	do{
		*x = rand() % ((MAX_WIDTH -1) - 0 + 1) + 0;
		*y = rand() % ((MAX_LENGTH -1) - 0 + 1) + 0;
		error_flag = is_check_error_number_for_enemy(&x, &y);
		overlap_error_flag = is_checked_overlap(&x,&y,information_card);
		if(overlap_error_flag != 0){
printf("overlap\n");
			error_flag = overlap_error_flag;
		}
	}while(error_flag != 0);

}
/* ----------------------------------------------------------- *
 *  play_my_turn:自分のターンの処理
 *  引数： 数値型の二次元配列 card[][MAX_LENGTH]:カードの番号を格納
 *  information_card[][MAX_LENGTH]:成功して既に場にないカード、一度引いたカード
 *  まだ引かれてないカードの値が格納
 *  戻り値 数値型 score得点。あたっていたら1を返す。あたっていなかったら0 
 * ----------------------------------------------------------- */
int play_my_turn(int card[][MAX_LENGTH], int information_card[][MAX_LENGTH]){

	int x1,y1,x2,y2,score = 0,i,j,k, error_flag = 0,once_turned = ONCE_TURNED_CARD;
	describe_card(card, information_card);

	do{
		printf("貴方のターンです\n");
		printf("1枚目に引く、座標を指定してください(x y)\n");
		scanf_two_int(&x1,&y1);
		error_flag = is_check_error_number(x1,y1,information_card);
printf("error_flag=%d", error_flag);
		if(error_flag != 0){
			continue;
		}
		printf("x=%d,y=%dをめくります。\n", x1,y1);
	
		printf("2枚目に引く、座標を指定してください(x y)\n");
		scanf_two_int(&x2,&y2);
		error_flag = is_check_error_number(x2,y2,information_card);
		if(x1 == x2 && y1 == y2){
			printf("エラー！xとyに同じ座標は選べません。\n");
			error_flag = -1;
			continue;
		}	
	}while(error_flag != 0);

	printf("x=%d,y=%dをめくります。\n", x2,y2);
	score = check_cards(x1, y1, x2, y2, card);
	if(score == 1){
		func_mark_to_card_strike(x1,x2,y1,y2,information_card);
		return score;
	}
	func_mark_to_card_miss(x1,x2,y1,y2,information_card);

}

/* ----------------------------------------------------------- *
 *  func_mark_to_card_strike:あたったカードにマークをつける処理
 *  引数：x1,x2,y1,y2 座標
 *  整数型の二次元配列 information_card:カード情報を格納
 *  戻り値　明示的なものは無し( information_card[x1][y1]information_card[x2][y2]が
 *  実質的な戻り値)
 * ----------------------------------------------------------- */
void func_mark_to_card_strike(int x1,int x2,int y1,int y2,int information_card[][MAX_LENGTH])
{

	printf("当たりのときの処理\n");
	information_card[x1][y1] = FINISHED_CARD;
	information_card[x2][y2] = FINISHED_CARD;
}

/* ----------------------------------------------------------- *
 *  func_mark_to_card_miss:外れた時にカードを一回ひっくり返したことがある
 *  マークにする為の処理
 *  引数：x1,x2,y1,y2 座標
 *  整数型の二次元配列 information_card:カード情報を格納
 *  戻り値　明示的なものは無し( information_card[x1][y1]information_card[x2][y2]が
 *  実質的な戻り値)
 * ----------------------------------------------------------- */

void func_mark_to_card_miss(int x1, int x2,int y1, int y2, int information_card[][MAX_LENGTH])
{
	if(information_card[x1][y1] != FINISHED_CARD || information_card[x2][y2] != FINISHED_CARD){
		printf("外れのときの処理\n");
		information_card[x1][y1] = ONCE_TURNED_CARD;
		information_card[x2][y2] = ONCE_TURNED_CARD;
	}
}



/* ----------------------------------------------------------- *
 *  is_check_error_number_for_enemy:引いたカードのエラーチェック。(敵用)
 *  チェック内容→ 引いたカードは範囲内か
 *  引数：input1,input2:x座標とy座標
 *  戻り値　error_flag エラーがあった場合-1
 * ----------------------------------------------------------- */
int is_check_error_number_for_enemy(int **x,int **y)
{
	int error_flag = 0,overlap_error_flag = 0, i,j;
printf("is_check_error_numberの中input1=%d,input2=%d", **x, **y);
	if(**x > MAX_WIDTH){
		error_flag = -1;
	}
	if(**y > MAX_LENGTH){
		error_flag = -1;
	} 
	return error_flag;
}

/* ----------------------------------------------------------- *
 *  is_check_error_number:引いたカードのエラーチェック。
 *  チェック内容→ 引いたカードは範囲内か、既にあたりで場にないものはないか
 *  引数：input1,input2:x座標とy座標, information_card[][MAX_WIDTH]:カード情報
 *  戻り値　error_flag エラーがあった場合-1
 * ----------------------------------------------------------- */
int is_check_error_number(int input1, int input2, int information_card[][MAX_LENGTH])
{
	int error_flag = 0,overlap_error_flag = 0, i,j;
//printf("is_check_error_numberの中input1=%d,input2=%d", input1, input2);
	if(input1 > MAX_WIDTH){
		printf("エラー！xには%dより小さい値を入れて下さい\n",MAX_WIDTH);
		error_flag = -1;
	}
	if(input2 > MAX_LENGTH){
		printf("エラー！yには%dより小さい値を入れてください。\n", MAX_LENGTH);
		error_flag = -1;
	} 
	overlap_error_flag = is_checked_overlap(&input1, &input2,information_card);
printf("overlap_error_flag=%d",overlap_error_flag);
	if(overlap_error_flag != 0){
		printf("エラー！指定された場所のカードは既に選択済みです\n");
		error_flag = overlap_error_flag;
	}
	return error_flag;
}

/* ----------------------------------------------------------- *
 *  is_checked_overlap: 重複チェック
 *  引数：数値型の二次元配列　information_cardカード情報が入っている 
 *  戻り値 数値型 error_flag 重複があれば-1を返す
 * ----------------------------------------------------------- */

int is_checked_overlap(int *x,int *y, int information_card[][MAX_LENGTH])
{
printf("is_checked_overlapの中。重複\n");
	int  error_flag = 0,finished = FINISHED_CARD;
	//カードの重複チェック
	if(information_card[*x][*y] == finished){
		printf("is_checked_overlapの中。重複\n");
		error_flag = -1;
	}
	return error_flag;

}

/* ----------------------------------------------------------- *
 *  scanf_two_int:2つの数値型の入力を受け付ける関数。数値以外のものが
 *  入力されるとバッファをクリアして、正常な値を読み込むまで繰り返してくれる
 *  引数： なし
 *  戻り値 数値型 input:ユーザが入力した内容
 * ----------------------------------------------------------- */

void scanf_two_int(int *input1, int *input2)
{
    while(1){
        if(scanf("%d %d", input1, input2) != 2){
            printf("エラー！数値以外のものが入力されました%d,%d\n", *input1, *input2);
            scanf("%*s");
            continue;
        }
        break;
    }
}
/* ----------------------------------------------------------- *
 *  check_cards:めくった２枚のカードの数値が一致しているかを判定
 *  する関数
 *  引数： 数値型x1,x2,y1,y2 2枚のカードの座標
 *  戻り値 数値型 あたっていたら1を返す
 * ----------------------------------------------------------- */

int check_cards(int x1,int y1,int x2,int y2,int card[][MAX_LENGTH])
{
	printf("めくったカードは%dと%dでした\n", card[x1][y1], card[x2][y2]);
	if(card[x1][y1] == card[x2][y2]){
		printf("あたりです\n");
		return 1;
	}
	printf("はずれ\n");
	return 0;

}
/* ----------------------------------------------------------- *
 *  scanf_two_int:2つの数値型の入力を受け付ける関数。数値以外のものが
 *  入力されるとバッファをクリアして、正常な値を読み込むまで繰り返してくれる
 *  引数： なし
 *  戻り値 数値型 input:ユーザが入力した内容
 * ----------------------------------------------------------- */


int describe_card(int card[][MAX_LENGTH],int information_card[][MAX_LENGTH])
{
	int i = 0, j = 0;
	printf("■ まだめくっていない\n");
	printf("X 合ってた！\n");
	printf("□ 一回だけめくっている\n");

//デバッグバッグコード
printf("information\n");
for(i = 0; i < 4; i++){
printf("%d ", i);
for(j = 0; j < 13; j++){
printf("%d,", information_card[i][j]);
}
printf("\n");
}
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
			if(information_card[i][j] == FINISHED_CARD){
				printf("X  ");
				continue;
			}
			if(information_card[i][j] == ONCE_TURNED_CARD){
				printf("□  ");
				continue;
			}
			printf("■  ");
		}
		printf("\n");
	}

}

/* ----------------------------------------------------------- *
 * scanf_int:数値型の入力を受け付ける関数。数値以外のものが
 * 入力されるとバッファをクリアして、正常な値を読み込むまで繰り返してくれる
 * 引数： なし
 * 戻り値 数値型 input:ユーザが入力した内容
 * ----------------------------------------------------------- */
int scanf_int()
{
	int input = 0;
	while(1){
		if(scanf("%d", &input) != 1){
			printf("エラー！数値以外のものが入力されました\n");
			scanf("%*s");
			continue;
		}
		break;
	}
	return input;
}


int put_card_randam(int card[][MAX_LENGTH],int information_card[][MAX_LENGTH])
{
	int i = 0, j = 0, x1,x2,y1,y2, buffer;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 13; j++){
			card[i][j] = j+1;
			information_card[i][j] = INIT_CARD;
		}
	}
	srand(time(0));
	
	int tmp, a,b;
	for(i = 0;i < MAX_WIDTH; i++){
		for(j = 0;j < MAX_LENGTH; j++){
			a = rand()%MAX_LENGTH;
			b = i % MAX_LENGTH;
			tmp = card[i][a];
			card[i][a] = card[i][b];
			card[i][b] = tmp;
		}
	}

//デバッグコード
for(i = 0; i < 4; i++){
for(j = 0; j < 13; j++){
printf("%d ",  card[i][j]);
}
printf("\n");
}
	
}
