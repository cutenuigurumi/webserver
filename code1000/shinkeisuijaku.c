#include<stdio.h>
#include<stdlib.h>

#define MAX_CARD 52
#define SHUFFLE 20
#define ONCE_TURNED_CARD 2
#define	FINISHED_CARD 1
#define INIT_CARD 0
#define DEFAULT_MEMORY 10

//カードの状態を描写する
int describe_card(int card[][MAX_LENGTH],  int information_card[][MAX_LENGTH]);
//最初のみ使用。カードをランダムに置き換える
int put_card_random(int card[][MAX_LENGTH],int information_card[][MAX_LENGTH]);
//カードが同じ数字かどうかを判定する関数
int check_cards(int x1,int y1,int x2,int y2,int card[][MAX_LENGTH]);
//自分のターンを実行する関数
int play_my_turn(int card[][MAX_LENGTH], int information_card[][MAX_LENGTH]);
//敵のターンを実行する関数
int play_enemy_turn(int card[][MAX_LENGTH], int information_card[][MAX_LENGTH]);
//play_enemy_turnの中で敵が引くカードを作る関数
int draw_enemy_card(int *x1,int *y1,int information_card[][MAX_LENGTH]);
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

struct card {
	//場に出ているカードの値
	int value[MAX_CARD];
	//一度ひっくり返したことがあるか、既に場に無いか、手をつけたことが無いかの情報
	int info[MAX_CARD];
};

struct enemy_info {
	int x;
	int x;
	int memory[DEFAULT_MEMORY];
	int score;
}
struct myself_info {
	int x;
	int y;
	int score;
}


int main()
{
	int end_flag = 0,sum = 0, my_score = 0, enemy_score = 0;
	struct card str_card;
	struct enemy_info str_enemy;
	struct myself_info str_myself;
	put_card_random(str_card,str_enemy);
	printf("神経衰弱をはじめましょう\n");

	do{
		while(sum <= MAX_CARD / 2){
	//		my_score = play_my_turn(str_card,str_enemy);
			str_myself.score = play_enemy_turn(str_card,str_enemy);
			str_enemy.score = play_enemy_turn(str_card,str_enemy);	
			sum = str_myself.score + str_enemy.score;
		}
		if(str_myself.score > str_enemy.score){ 
			printf("貴方の勝ち\n");
		} else if(str_myself.score == str_enemy.score) {
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
int play_enemy_turn(struct str_card, struct str_enemy){

	int i,j,k, error_flag = 0, score = 0;
	describe_card(str_card);

	do{
		printf("敵のターンです\n");
		//memoryの中の値を消す処理
		//敵のカードを引く処理
		str_enemy.x = draw_enemy_card(str_card, str_enemy);
		printf("1枚目はx=%dをめくります。\n",str_enemy.x1);
	
		//敵のカードを引く処理
		str_enemy.y = draw_enemy_card(str_card, str_enemy);
		if(str_enemy.x == str_enemy.y){
			printf("エラー！xとyに同じ座標は選べません。\n");
			error_flag = -1;
			continue;
		}
	}while(error_flag == -1);

	printf("2枚目はx=%dをめくります。\n", str_enemy.y);
	score = check_cards(str_card, str_enemy);
	if(score == 1){
		func_mark_to_card_strike(str_card, str_enemy);
		return score;
	}
	func_mark_to_card_miss(str_card, str_enemy);

}

/* ----------------------------------------------------------- *
 *  draw_enemy_card:敵がカードを1枚引く処理
 *  引数：int *x,int *y：x座標とy座標
 *  int information_card[][MAX_LENGTH]:カードの情報が格納
 *  戻り値　明示的なものは無し。(xとyが実質的な戻り値)
 * ----------------------------------------------------------- */
int draw_enemy_card(struct str_card, struct str_enemy)
{
	int x, overlap_error_flag = 0,error_flag = 0;
	do{
		x = rand() % ((MAX_CARD -1) - 0 + 1) + 0;
		error_flag = is_check_error_number_for_enemy(x, str_enemy);

		overlap_error_flag = is_checked_overlap(str_enemy,str_card);
		if(overlap_error_flag != 0){
printf("overlap\n");
			error_flag = overlap_error_flag;
		}
	}while(error_flag != 0);
	return x;

}
/* ----------------------------------------------------------- *
 *  play_my_turn:自分のターンの処理
 *  引数： 数値型の二次元配列 card[][MAX_LENGTH]:カードの番号を格納
 *  information_card[][MAX_LENGTH]:成功して既に場にないカード、一度引いたカード
 *  まだ引かれてないカードの値が格納
 *  戻り値 数値型 score得点。あたっていたら1を返す。あたっていなかったら0 
 * ----------------------------------------------------------- */
int play_my_turn(struct str_card, struct str_myself){

	int x1,y1,x2,y2,score = 0,i,j,k, error_flag = 0,once_turned = ONCE_TURNED_CARD;
	describe_card(card, information_card);

	do{
		printf("貴方のターンです\n");
		printf("1枚目に引く、座標を指定してください(x y)\n");
		scanf_two_int(&x1,&y1);
		error_flag = is_check_error_number(x1,y1,information_card);
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
	score = check_cards(str_card);
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
void func_mark_to_card_strike(struct str_card, struct str_player)
{
	printf("当たりのときの処理\n");
	str_card.info[str_player.x] = FINISHED_CARD;
	str_card.info[str_player.y] = FINISHED_CARD;
}

/* ----------------------------------------------------------- *
 *  func_mark_to_card_miss:外れた時にカードを一回ひっくり返したことがある
 *  マークにする為の処理
 *  引数：x1,x2,y1,y2 座標
 *  整数型の二次元配列 information_card:カード情報を格納
 *  戻り値　明示的なものは無し( information_card[x1][y1]information_card[x2][y2]が
 *  実質的な戻り値)
 * ----------------------------------------------------------- */

void func_mark_to_card_miss(struct str_card, struct str_player)
{
	if(str_card.info[str_player.x] != FINISHED_CARD || str_card.info[str_player.y] != FINISHED_CARD){
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
int is_check_error_number_for_enemy(int x,int y)
{
	int error_flag = 0,overlap_error_flag = 0, i,j;
//printf("is_check_error_numberの中input1=%d,input2=%d\n", **x, **y);
	if(x > MAX_WIDTH){
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

int is_checked_overlap(int x, struct str_card)
{
	int  error_flag = 0, FINISHED_CARD;
	//カードの重複チェック
	if(str_card.info[x] == finished){
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

int check_cards(struct str_card, struct str_player)
{
	printf("めくったカードは%dと%dでした\n", str_card.value[str_player.x], str_card.value[str_player.y]);
	if(str_card.value[str_player.x] == str_card.value[str_player.y]){
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

int describe_card(struct str_card, struct str_enemy_info)
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
printf("%d,", str_card.);
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


int put_card_random(int card[][MAX_LENGTH],int information_card[][MAX_LENGTH])
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
	
	inddt enemy[MAX_CARD];
}
