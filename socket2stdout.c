#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#define MAX_BACKLOG 5
#define BUF_LINE_SIZE 10024
#define TIME 50
#define METHOD_LEN 5
#define PAGENAME_LEN 10
#define PATH_LEN 10024
#define EXPLAIN_LEN 20
#define CONTENTTYPE_LEN 30
#define CHARASET_LEN 50
#define VERSION_LEN 10
#define CONTENT_LENGTH_LEN 10
#define EXTENSION_LEN 10
#define USER_INPUT_LEN 10
#define COOKIE_LEN 1024
#define PORT_LEN 100
#define CODE_200 0
#define CODE_303 1
#define CODE_400 2
#define CODE_403 3
#define CODE_404 4
#define EXTENSION_ARRAY 10
#define STATUSLINE_ARRAY 10
#define FILE_HEAD "/request_"
#define EXTENSION ".log"
#define SCD_EDUCATION_PATH "/usr/local/bin/scd_education/"
#define DOCUMENTROOT SCD_EDUCATION_PATH "www"
#define LOGFILETROOT SCD_EDUCATION_PATH "log"
#define CONTENT_DIR "content/"
#define COOKIE_DIR "cookie/"
#define DELETE_COOKIE_DATE "Fri, 31-Dec-1999 23:59:59 GMT"
#define COOKIEVALUE1 "cookie_value1"
#define LOGOUT_PATH DOCUMENTROOT COOKIE_DIR "/logout.html"
#define LOGIN_CHECK_PATH DOCUMENTROOT COOKIE_DIR "login_check.html"
#define URL "http://54.64.221.93:"
#define CONTENT_PAGE_PATH COOKIE_DIR  "content/welcome.html"
#define LOGIN_PAGE_PATH COOKIE_DIR "/login.html"
#define USERNAME_TEST "admin"
#define PASSWORD_TEST "nmadmin001"

int is_check_argument_err(int);
//ステータスエラーページを返す関数
char* return_status_error_page (int, char*);
//子プロセスを成仏させる
void kill_child_process ();
//logfileの名前を作成する関数
char* create_logfilename(char *);
//クッキーの有効期限を作成する関数
char* create_cookie_limit(char *);
//文字列を切り出して変数に格納する関数
char* strtok_string(char *, char *);
//テキスト形式のresponseボディを返却する
void write_txt_http_responsebody(FILE *, FILE *);
//バイナリ形式のresponseボディを返却する
void write_binary_http_responsebody(FILE *, FILE *);
//構造体の中の拡張子と同じだったら構造体の何番目の拡張子かを返却する
int get_extension_list_array_num(char *);
//ファイルが存在するかを確認
int is_checked_file_exist(char *);

static int listen_socket (char *port);
struct statusline {
	char version[VERSION_LEN];
	int status_code;
	char explain[EXPLAIN_LEN];
	char pagename[PAGENAME_LEN];
};
struct extension_list {
    char extension[EXTENSION_LEN];
    char content_type[CONTENTTYPE_LEN];
	char charaset[CHARASET_LEN];
};

//ステータスラインの情報を格納する構造体
struct statusline str_statusline[STATUSLINE_ARRAY] = {
	{"HTTP/1.0", 200, "OK"},
	{"HTTP/1.0", 303, "See Other"},
	{"HTTP/1.0", 400, "Bad Request", "/400.html"},
	{"HTTP/1.0", 403, "Forbidden", "/403.html"},	
	{"HTTP/1.0", 404, "Not Found", "/404.html"},
};
//拡張子とcontent-typeを格納する構造体
struct extension_list str_extension_list[EXTENSION_ARRAY] = {
	{".html", "text/html;", "charset=UTF-8"},
	{".htm", "text/html;", "charset=UTF-8"},
	{".txt", "text/plain;", "charset=UTF-8"},
	{".jpg", "image/jpeg", ""},
	{".jpeg", "image/jpeg", ""},
	{".ico", "image/x-icon", ""},
	{".gif", "image/gif", ""},
	{".png", "image/png", ""}
};

//使ってない定数は削除する
int main (int argc, char *argv[]) {
    int lissock,argument_err_flag;
    char port_number[PORT_LEN], filename[PATH_LEN], return_path[PATH_LEN], parameter_path[PATH_LEN];
	/* httpリクエストの解析に使用 */
	char  *p_parameter_path, method[METHOD_LEN], *p_method, path[PATH_LEN], *p_path, version[VERSION_LEN], *p_version, content_length[CONTENT_LENGTH_LEN], *p_content_length, username[USER_INPUT_LEN], password[USER_INPUT_LEN], *p_username, *p_password, user_cookie[COOKIE_LEN], *p_user_cookie;
    FILE *response;
	
	printf("%s,%s,%s,%s,%s,%s", DOCUMENTROOT,LOGFILETROOT, LOGOUT_PATH,LOGIN_CHECK_PATH,CONTENT_PAGE_PATH, LOGIN_PAGE_PATH);
	argument_err_flag = is_check_argument_err(argc);
	if(argument_err_flag == -1){
		exit(-1);
		printf("エラー！終了します\n");
	}
	strcpy(port_number, argv[1]);
    lissock = listen_socket (port_number);
	printf("listen_socket後%d", lissock);
    for (;;) {
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof addr;
        int accsock, status_line_flag = 0, tmp_status_code = 0, extension_list_array_num = 0,error_flag = 0, int_content_length = 0, continue_flag = 0, cookie_create_check = 0, file_size = 0;
        char buf_request[BUF_LINE_SIZE],buf[BUF_LINE_SIZE], redirect_location[PATH_LEN], buf_path[PATH_LEN], cookie_expires_string[TIME];
        FILE *fp, *sockf, *write_sockf;
		char *extension;
		strcpy(filename, create_logfilename(filename));
		//ソケットへの接続を待つ動作を用意する
        accsock = accept (lissock, (struct sockaddr*) &addr, &addrlen);
        if (accsock < 0) {
			fprintf (stderr, "accept failed\n");
			continue;
        }
		//pidには子プロセスのプロセスidが返ってくる。
		int pid,status;
		pid = fork();
		printf("pid %d", pid);

		if(pid  == -1){
			printf("子プロセスの生成に失敗しました\n");
		} else if(pid > 0){
printf("親プロセス\n");
			//子プロセスがexitしていたらkill_child_processを実行
			signal (SIGCHLD, kill_child_process);
			close (accsock);
			continue;
		}
		if (WIFEXITED(status)){
			printf("exit, status=%d\n", WEXITSTATUS(status));
		} else {
		printf("不正終了\n");
		}

		if(pid == 0){
printf("子プロセスが作成されました。%d\n", pid);


			//httpリクエストを読み込むファイルディスクリプタ
			sockf = fdopen (accsock, "r");
			//httpレスポンスを返すファイルディスプリプタ
			write_sockf = fdopen(accsock, "w");
			//logを書き込むファイルディスクリプタ
			fp = fopen(filename, "w");


			//ここからリクエストを読み込み(while文の中が1リクエスト)
			while (fgets (buf, BUF_LINE_SIZE, sockf)) {
				if(status_line_flag == 0){
					//一時変数に格納
					strcpy(buf_request, buf);
					//リクエストラインの解析
					p_method = strtok(buf_request, " ");
					strcpy(method, p_method);
					p_path = strtok(NULL, " ");
					strcpy(path, p_path);
					p_version = strtok(NULL, p_path);
					strcpy(version, p_version);
					if(strstr(path, "?") != NULL){
						strcpy(buf_path, path);
						p_path = strtok(buf_path, "?");
						strcpy(path, p_path);
						p_parameter_path = strtok(NULL, " ");
						strcpy(parameter_path, p_parameter_path);
					}
					status_line_flag = 1;
				}
				if(strstr(buf, "Content-Length:") != NULL){
					p_content_length = strtok(buf, ":");
					p_content_length = strtok(NULL, ":");
					strcpy(content_length, p_content_length);
				}

				if(strstr(buf, "Cookie:") != NULL){
					p_user_cookie = strtok(buf, ":");
					p_user_cookie = strtok(NULL, "=");
					p_user_cookie = strtok(NULL, "\r\n");
					strcpy(user_cookie, p_user_cookie);
					//printf("user_cookieの値 %s\n", user_cookie);
				}
				fputs (buf, fp);
				if(strcmp(buf, "\r\n") == 0){
					if(strcmp(method, "POST") == 0){
						fgets(buf, atoi(content_length), sockf);
						fputs(buf, fp);
						p_username = strtok(buf, "&");
						p_username = strtok(NULL, "=");
						p_username = strtok(NULL, "&");
						strcpy(username, p_username);
						p_password = strtok(NULL, "&");
						p_password = strtok(NULL, "=");
						p_password = strtok(NULL, "&");
						strcpy(password, p_password);
						//	printf("username = \"%s\", password = \"%s\"", username, password);
						break;
					}
					if(strcmp(method, "GET") == 0){
printf("GETでループから抜ける直前\n");
						break;
					}
				}
			}
		
			printf("ループから抜けた\n");
			//レスポンスを返す
			if(strcmp(path, "/") == 0){
				strcpy(path, "/index.html");
			}
//printf("return_pathを返す直前%s\n", path);
			strcpy(return_path, DOCUMENTROOT);
			strcat(return_path, path);
			printf("フルパス %s\n", return_path);

			//400 badrequest ディレクトリトラバーサル対策
			if(strstr(return_path, "../") != NULL){
				tmp_status_code = CODE_404;
				strcpy(return_path,return_status_error_page(tmp_status_code, return_path));
				error_flag = 1;
			}
//printf("400処理の直後\n");

			//拡張子を取得
			extension = strstr(path, ".");
			extension_list_array_num = get_extension_list_array_num(extension);
//printf("%d", extension_list_array_num);

			if(extension_list_array_num > EXTENSION_ARRAY){
				tmp_status_code = CODE_404;
                strcpy(return_path,return_status_error_page(tmp_status_code, return_path));
				error_flag = 1;
			}
			//404の処理
			if(is_checked_file_exist(return_path) == -1){
				tmp_status_code = CODE_404;
				strcpy(return_path, return_status_error_page(tmp_status_code, return_path));
				error_flag = 1;
			}
			int binary_flag = 0, text_flag = 0;
			if((strstr(str_extension_list[extension_list_array_num].content_type, "image")) != NULL){
				response = fopen(return_path, "rb");
				if(response != NULL){
					binary_flag = 1;
					//Content-Lengthで使用する文字数カウントの処理
					fseek(response, 0L, SEEK_END);
					file_size = ftell(response);
					fseek(response, 0L, SEEK_SET);
				}
			} else {
				response = fopen(return_path, "r");
				text_flag = 1;
			}
			//login_check.htmlに来たときの処理
			if(strcmp(return_path, LOGIN_CHECK_PATH) == 0){
				//エラー処理
				if(strcmp(method, "POST") != 0){
					strcpy(redirect_location, LOGIN_PAGE_PATH);
				}
				//useridもパスワードも正しい場合
				if(strcmp(username, USERNAME_TEST) == 0 && strcmp(password, PASSWORD_TEST) == 0){
					strcpy(redirect_location, URL);
					strcat(redirect_location, port_number);
					strcat(redirect_location, CONTENT_PAGE_PATH);
printf("IDパスワード正しい%s\n", redirect_location);
				} else {
					strcpy(redirect_location, URL);
					strcat(redirect_location, port_number);
					strcpy(redirect_location, LOGIN_PAGE_PATH);
				}
				tmp_status_code = CODE_303;
			}
			//logout.htmlに来た場合の処理（login.htmlに遷移する）
			if(strcmp(return_path, LOGOUT_PATH) == 0){
				strcpy(redirect_location, URL);
				strcat(redirect_location, port_number);
				strcpy(redirect_location, LOGIN_PAGE_PATH);
				tmp_status_code = CODE_303;
			}

			//welcome.htmlに直接来た場合の処理
//printf("welcome.html前\n%s\n", return_path);
			if((strstr(return_path, CONTENT_DIR)) != NULL){
				//cookieを持っていなかったらlogin.htmlにリダイレクト
				if(strcmp(user_cookie, COOKIEVALUE1) != 0){
//printf("user_cookie value=%s, COOKIEVALUE1=%s\n", user_cookie, COOKIEVALUE1);
					strcpy(redirect_location, LOGIN_PAGE_PATH);
					tmp_status_code = CODE_303;
				}
			}
			//403 閲覧禁止の場合の処理
			if(response == NULL && error_flag == 0){
//printf("debug code:403の処理を通っていることを確認");
				strcpy(return_path,return_status_error_page(tmp_status_code, return_path));
				error_flag = 1;
				//上でパスを変更しているので開き直す
				response = fopen(return_path ,"r");
			}

			if(error_flag == 0 && tmp_status_code != CODE_303){
				tmp_status_code = CODE_200;
			}

			/* ここから下がcookieの処理	*/
			//Cookieの作成（login_check.html)
			cookie_create_check = strcmp(return_path, LOGIN_CHECK_PATH);

//printf("cookieの作成\n");
			if(cookie_create_check == 0){
				strcpy(cookie_expires_string, create_cookie_limit(cookie_expires_string));
			}

			fprintf(write_sockf, "%s %d %s\n", str_statusline[tmp_status_code].version, str_statusline[tmp_status_code].status_code, str_statusline[tmp_status_code].explain);
			fprintf(write_sockf, "Server: Apache\n");
			fprintf(write_sockf, "Accept-Ranges: bytes\n");
printf("tmp_status_code→%d\n", tmp_status_code);
			if(tmp_status_code == CODE_303){
printf("redirect\n");
				fprintf(write_sockf, "Location:%s \n", redirect_location);
			}
//printf("cookieの削除処理の前\n");
	
			fprintf(write_sockf, "Set-Cookie: CUSTOMER=%s; path=/cookie/; expires=%s", COOKIEVALUE1, cookie_expires_string);
//Cookieの削除logout.html)
			int cookie_delete_check;
			cookie_delete_check = strcmp(return_path, LOGOUT_PATH);
// printf("cookie_delete_check= %d, return_path=%s, COOKIEDELETE=%s\n", cookie_delete_check, return_path, COOKIEDELETE);
			if(cookie_delete_check == 0){
				fprintf(write_sockf, "Set-Cookie: CUSTOMER=%s; expires=%s;\n", COOKIEVALUE1, DELETE_COOKIE_DATE);
			}
		
			if(binary_flag == 1){
				fprintf(write_sockf, "Content-Length: %d\n", file_size);
   				fprintf(stdout, "Content-Length: %d\n", file_size);
			}
			//fprintf(write_sockf, "Content-Lenth: 20\n, fie_sie);
			fprintf(write_sockf, "Content-Type: %s\n", str_extension_list[extension_list_array_num].content_type);
			fprintf(write_sockf, "\n");
			//バイナリデータの処理
			if(binary_flag == 1){
				write_binary_http_responsebody(response, write_sockf);
			}
			if(text_flag = 1){
				write_txt_http_responsebody(response, write_sockf);
			}
			fclose(write_sockf);
			fclose (sockf);
			fclose (response);
			fclose(fp);
			close(accsock);
			close(lissock);
			printf("close完了\n");
			sleep(10);
			exit(0);
		}
	}
}



/* ----------------------------------------------------------- *
 *  is_check_argument_err コマンドライン引数の数値が正常かを判断する関数
 *  引数：argc 引数の数
 *  戻り値 エラーがあれば-1、正常なら0
 *  ----------------------------------------------------------- */
int is_check_argument_err(int argc){
	if(argc > 2){
		fprintf(stderr, "コマンドライン引数が多すぎます！\n");
		return -1;
	}
	if(argc <= 1){
		fprintf(stderr, "コマンドライン引数が少なすぎます！\n");
		return -1;
	}
	return 0;
}

/* ----------------------------------------------------------- *
 *  return_status_error_page 引数で渡されたステータスコードによって
 *  ブラウザに返すURLを作成する関数
 *  引数：count_str_statusline：ステータスコードが黄道帯の何番目に格納されているか
 *  return_path:ブラウザに返却するパス
 *  戻り値：return_path
 *  ----------------------------------------------------------- */
char* return_status_error_page (int count_str_statusline, char *return_path){

	strcpy(return_path, DOCUMENTROOT);
	strcat(return_path, str_statusline[count_str_statusline].pagename);
//printf("return_status_error_pageの中%s\n", return_path);
	return return_path;

}

/* ----------------------------------------------------------- *
 *  kill_child_process exitされた子プロセスを成仏させる
 *  引数：なし
 *  戻り値：なし
 *  ----------------------------------------------------------- */
void kill_child_process () {
	int status;
	wait(&status);
}
/* ----------------------------------------------------------- *
 *  is_checked_file_exist ページが存在するか、しないかを判断。
 *  引数：return_path: ファイルまでのパスが格納されている文字配列
 *  戻り値：error_flag : -1ならファイルが存在しない(404)、0なら存在する。
 *  ----------------------------------------------------------- */
int is_checked_file_exist(char *return_path)
{
	struct stat file;
	struct stat str_file;
	int error_flag = 0;
	printf("is_checked_file_existの中\n");
	//ブラウザに返すファイルを開く
	//404 該当するページが無かったときの処理
	if(stat(return_path, &str_file) != 0){
		error_flag = -1;
	}
	return error_flag;
}
/* ----------------------------------------------------------- *
 *  get_extension_list_array_num
 *  構造体の中の拡張子と同じだったら構造体の何番目の拡張子かを返却する
 *  引数：extension :拡張子
 *  戻り値：i:構造体の何番目に格納されている拡張子か
 *  ----------------------------------------------------------- */
int get_extension_list_array_num(char *extension)
{
	int i = 0, debug = 0;
	while(i < EXTENSION_ARRAY){
		debug = strcmp(extension, str_extension_list[i].extension);
		if(debug == 0){
			return i;
		}
		i++;
	}
		return i;
}
/* ----------------------------------------------------------- *
 *  write_binary_http_responsebody  テキスト形式のhttpレスポンスボディを返却
 *  引数：FILE *response, FILE *write_sockf:ファイルポインタ
 *  戻り値：なし
 *  ----------------------------------------------------------- */
void write_binary_http_responsebody(FILE *response, FILE *write_sockf){

	char buffer[BUF_LINE_SIZE];

	int size = 0, k = 0;
	size = fread(buffer, sizeof( unsigned char ), 10000, response);
	for(k = 0; k < size; k++){
		fputc(buffer[k], write_sockf);
	}

}
/* ----------------------------------------------------------- *
 *  write_txt_http_responsebody  バイナリ形式のhttpレスポンスボディを返却
 *  引数：FILE *response, FILE *write_sockf:ファイルポインタ
 *  戻り値：なし
 *  ----------------------------------------------------------- */
void write_txt_http_responsebody(FILE *response, FILE *write_sockf){

	char buffer[BUF_LINE_SIZE];
	while(fgets(buffer, sizeof(buffer), response)){
		fputs(buffer, write_sockf);
	}
}

/* ----------------------------------------------------------- *
 *  create_logfilename: ログファイルの場所が記載されている変数filename
 *  を返す関数
 *  引数：filenamechar型の文字配列
 *  戻り値：filename：ログファイルの場所が記載されているcharの文字配列
 *  注意点：create_cookie_limitと途中迄やっていることは一緒ですが、gmtとjstの違いがあるので
 *  別関数にしています。
 *  ----------------------------------------------------------- */
char* create_logfilename (char *filename) {

	//logファイルの作成
	char datetime[TIME];
	struct tm *str_date;
	time_t now;
	now = time(NULL); //時刻の取得
	str_date = localtime(&now);
	strftime(datetime,  TIME, "%Y%m%d%H%M%S", str_date);
    strcpy(filename,  LOGFILETROOT);
	strcat(filename, FILE_HEAD);
	strcat(filename, datetime);
	strcat(filename, EXTENSION);
	return filename;
}


/* ----------------------------------------------------------- *
 *  create_cookie_expires_string  cookieの有効期限(GMTに＋１した値)を作成
 *  引数：datetime:char型の文字配列
 *  戻り値：datetime:cookieの有効期限(GMT)
 *  ----------------------------------------------------------- */
char* create_cookie_limit(char *return_string)
{
   	time_t now;
	struct tm *str_gmt;
	char datetime[TIME];
	char *wday[] = {"Sun","Mon","The","Wed","Thu","Fri","Sat"}, *wmon[] = {"Jan","Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	now = time(NULL); //時刻の取得
	str_gmt = gmtime(&now);

	sprintf(datetime, "%s, %d-%s-%d %d:%d:%d GMT;",  wday[str_gmt->tm_wday], str_gmt->tm_mday+1, wmon[str_gmt->tm_mon], str_gmt->tm_year+1900, str_gmt->tm_hour, str_gmt->tm_min, str_gmt->tm_sec);
	sprintf(return_string, "Set-Cookie: CUSTOMER=%s; path=/cookie/; expires=%s\n", COOKIEVALUE1, datetime);
	return return_string;

}


char* strtok_string(char *buffer,char *division_string)
{
	division_string = strtok(NULL, " ");
	strcpy(buffer, division_string);
	printf("関数内でバッグ！%s,%s\n", buffer, division_string);
	return division_string;
}


static int listen_socket (char *port) {
    struct addrinfo hints, *res, *ai;
    int err;
	//アドレス情報を設定する
    memset (&hints, 0, sizeof (struct addrinfo));
	//ネットワークアドレスの種類(IPv4)を格納
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; /* Server Side */
	//getaddrinfoでstruct addrinfoにインターネットアドレスが格納される
    if ((err = getaddrinfo (NULL, port, &hints, &res)) != 0) {
        fprintf (stderr, "getaddrinfo error");
        exit (-1);
    }
    for (ai = res; ai; ai = ai->ai_next) {
        int sock;

        //fprintf (stdout, "ai_next:%i\n", ai);

		/*ソケットの作成
		 *引数はどうやって通信するかを指定している。
		 *sockにはソケットへのディスクリプタが格納される */ 
        sock = socket (ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sock < 0) {
            fprintf (stdout, "cannot create socket\n");
            continue;
        }
		/*アドレスを割り当てる*/
        if (bind (sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            close (sock);
            fprintf (stdout, "cannot bind socket\n");
            continue;
        }
		/*listen クライアント側からの接続要求を待つ*/
        if (listen (sock, MAX_BACKLOG) < 0) {
            close (sock);
            fprintf (stdout, "cannot listen socket\n");
            continue;
        }
        freeaddrinfo (res);
        return sock;
    }

    fprintf (stderr, "failed to listen socket\n");
    exit (-1);
}
