#include<stdio.h>
#include<stdlib.h>
#define REST_FINGER 2

//数値以外のものが入力された時の処理が入った数値型のscanf
int scanf_int();
//入力された数値にエラーが無いかを確認する関数
int is_check_error(int, int);
//使い方をstdoutに出力する関数
void display_how_to_play();
//自分のターンの処理をする関数
int func_my_turn(int *, int *,int *);
//相手のターンの処理をする関数
int func_enemy_turn();
//敵のあげる指の本数を作る関数
int create_enemy_number_of_fingers(int **);
//敵のターンの時に宣言する指の数を作る関数
int create_enemy_call_sum_of_fingers(int);
//宣言した数と指の数が合っていれば-1を返す
int func_which_win(int,int);

int main()
{
	/*変数解説
	 * my_finger_restとenemy_finger_rest 自分の指の残りの本数
	 * sum_finger_rest 上二つを合わせた変数
	*/
	int is_start_flag = 0,play_flag = 0,turn = 0,my_finger_rest = REST_FINGER,sum_finger_rest = 0, enemy_finger_rest = REST_FINGER,end_flag = 0, error_flag = 0; 

	printf("使い方をみますか？Yes:1 No:0\n");
	play_flag = scanf_int();
	if(play_flag != 0){
		display_how_to_play();
		printf("0 ゲームに移動 1 やめる\n");
		is_start_flag =  scanf_int();
	}
	if(is_start_flag != 0){
		printf("終了します\n");
		exit(-1);
	}
	do{
		sum_finger_rest = my_finger_rest + enemy_finger_rest;
		do{
			//偶数は自分のターン、奇数は相手のターン
			printf("いっせーの…\n");
			if(turn % 2 == 0){
				printf("あなたのターン\n");
				func_my_turn(&my_finger_rest,&enemy_finger_rest, &turn);
			}else{
				printf("敵のターン\n");
				do{
					func_enemy_turn(&my_finger_rest,&enemy_finger_rest, &turn);
				}while(error_flag == 1);
			}
			sum_finger_rest = my_finger_rest + enemy_finger_rest;
			printf("残りの指の本数：%d　あなた：%d　あいて：%d\n", sum_finger_rest, my_finger_rest,enemy_finger_rest);
			turn++;
		}while(my_finger_rest > 0 && enemy_finger_rest > 0);
		printf("もう一回遊ぶ？\n Yes 1:No:0");
		end_flag = scanf_int();
		if(end_flag == 1){
			//初期化する。これをやらないと結果が引き継がれてしまって2回戦目の手が0から始まる。
			turn = 0,my_finger_rest = REST_FINGER,sum_finger_rest = 0, enemy_finger_rest = REST_FINGER;
		}
	}while(end_flag == 1);
		exit(0);
}

/* ----------------------------------------------------------- *
 *  func_which_win:関数を呼び出した方が勝ったか、引き分けかを判定する関数
 *  引数： take_both_finger:実際に出した値
 *  call_sum_finger：ターン主が宣言した値
 *  戻り値　result 勝てば-1を返却。負ければ0を返却
 * ----------------------------------------------------------- */
int func_which_win(int take_both_finger, int call_sum_finger)
{
	int result = 0;
	if(take_both_finger == call_sum_finger){
		result = -1;
	}
	if(take_both_finger != call_sum_finger){
		printf("引き分け\n");
	}
	return result;
}

 /*変数解説 func_enemy_turnとfunc_my_turn
 * call_sum_finger いっせーの！で宣言する指の本数。ターン主が宣言
 * take_my_finger,take_enemy_finger いっせーので出す指の本数
 * take_both_finger 上の二つを合わせた数
 * sum_finger_rest 残り指の本数(my_finger_rest　+ enemy_finger_rest)
*/

/* ----------------------------------------------------------- *
 *  func_enemy_turn:敵のターンを実行する関数
 *  引数： 数値型　my_finger_rest, enemy_finger_rest:残りの指の本数
 *  turn:ターン数（奇数偶数で相手の番か否かを判定）
 *  戻り値　なし
 * ----------------------------------------------------------- */

int func_enemy_turn(int *my_finger_rest,int *enemy_finger_rest,int *turn)
{
	int take_both_finger = 0,take_enemy_finger = 0,call_sum_finger = 0,error_flag = 0, take_my_finger = 0, sum_finger_rest, tmp_score = 0;

	sum_finger_rest = *my_finger_rest + *enemy_finger_rest;
	do{
		printf("貴方の出す数値を教えてください。\n");
		take_my_finger = scanf_int();
		error_flag = is_check_error(take_my_finger, *my_finger_rest);
	}while(error_flag != 0);
	call_sum_finger = create_enemy_call_sum_of_fingers(sum_finger_rest);
	printf("せっ%d！！！", call_sum_finger);
	/*強くする処理
	 *e.g. いっせーのせ！で0と宣言しているのに、2を出したりしないようにする処理 */
	do{
		take_enemy_finger = create_enemy_number_of_fingers(&enemy_finger_rest);
	}while(take_enemy_finger > call_sum_finger);
	take_both_finger = take_enemy_finger + take_my_finger;
	printf("貴方は%d, 相手は%d", take_my_finger, take_enemy_finger);
	tmp_score = func_which_win(take_both_finger,call_sum_finger);	
	*enemy_finger_rest = *enemy_finger_rest + tmp_score;	
}
/* ----------------------------------------------------------- *
 *  func_my_turn:自分のターンを実行する関数
 *  引数： 数値型　my_finger_rest, enemy_finger_rest:残りの指の本数
 *  turn:ターン数（奇数偶数で自分の番か否かを判定）
 *  戻り値　なし
 * ----------------------------------------------------------- */
int func_my_turn(int *my_finger_rest,int *enemy_finger_rest,int *turn)
{
	int take_both_finger = 0,take_enemy_finger = 0,call_sum_finger = 0,error_flag = 0, take_my_finger = 0, sum_finger_rest, tmp_score = 0;

	sum_finger_rest = *my_finger_rest + *enemy_finger_rest;
	do{
		printf("0〜%dの数値を入力してください\n", sum_finger_rest);
		call_sum_finger = scanf_int();
		error_flag = is_check_error(call_sum_finger, sum_finger_rest);
		if(error_flag == 1){
			continue;
		}
		printf("貴方の出す数値を教えてください。\n");
		take_my_finger = scanf_int();
		error_flag = is_check_error(take_my_finger, *my_finger_rest);
	}while(error_flag != 0);
	printf("せっ%d！！！", call_sum_finger);
	take_enemy_finger = create_enemy_number_of_fingers(&enemy_finger_rest);
	take_both_finger = take_enemy_finger + take_my_finger;
	printf("貴方は%d, 相手は%d", take_my_finger, take_enemy_finger);
	tmp_score = func_which_win(take_both_finger,call_sum_finger);
	*my_finger_rest = *my_finger_rest + tmp_score;
	
}

/* ----------------------------------------------------------- *
 *  scanf_int:数値型の入力を受け付ける関数。数値以外のものが
 *  入力されるとバッファをクリアして、正常な値を読み込むまで繰り返してくれる
 *  引数： なし
 *  戻り値 数値型 input:ユーザが入力した内容
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

/* ----------------------------------------------------------- *
 *  create_enemy_number_of_fingers:敵の値をランダムで作る関数
 *  引数のenemy_finger_restより大きい数字は出さない
 *  引数： enemy_finger_rest: 敵の残りの指の本数
 *  戻り値 数値型 finger:敵が出す指の本数
 * ----------------------------------------------------------- */
int create_enemy_number_of_fingers(int **enemy_finger_rest)
{
	int finger = 0;
	finger = rand() % (**enemy_finger_rest -0 + 1) + 0;
	printf("相手は%dを出しました。\n", finger);
	return finger;
}

/* ----------------------------------------------------------- *
 *  create_enemy_call_sum_of_fingers:
 *  引数のenemy_finger_restより大きい数字は出さない
 *  引数： enemy_finger_rest: 敵の残りの指の本数
 *  戻り値 数値型 finger:敵が出す指の本数
 * ----------------------------------------------------------- */
int create_enemy_call_sum_of_fingers(int sum_finger_rest)
{
	int call_finger = 0;
	call_finger = rand() % (sum_finger_rest - 0 + 1) + 0;
	return call_finger;
}
			
/* ----------------------------------------------------------- *
 *  is_check_error: 0〜指定の値(引数のmax)までの間に引数inputが
 *  収まっているか、収まっていなかったらエラーとして処理する関数
 *  引数：input:入力値、max:最大値
 *  戻り値 数値型 error_flag エラーがあれば1を返す
 * ----------------------------------------------------------- */

int is_check_error(int input, int max)
{
	int error_flag = 0;
	if(input > max){
		error_flag = 1;
	}
	if(input < 0){
		error_flag = 1;
	}
	if(error_flag == 1){
		printf("エラー！0〜%dまでの数値を入れてください。\n", max);
	}
	return error_flag;

}

/* ----------------------------------------------------------- *
 *  display_how_to_play:使い方を表示する関数
 *  引数： なし
 *  戻り値 なし
 * ----------------------------------------------------------- */
void display_how_to_play()
{
	printf("指スマの遊び方\n");
	printf("「いっせーの...せっ！」のかけ声とともに「０〜プレイヤーの残りの指の本数」のうち、好きな数字を言う。\n");
	printf("（※スタート時の残りの本数は親指２本×プレイヤー人数）\n");
	printf("それと同時にプレイヤーは任意の数値を入力します。\n");
	printf("これは、貴方が今回出す指の本数です\n");
	printf("たった指の合計数と宣言した数が一致すれば、宣言した人は片手を下げられます。\n");
	printf("先に両手とも下げられた方が勝ち\n");

}
