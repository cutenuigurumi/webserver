#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#define MAX_BACKLOG 5
#define DEFAULT_PORT "50000"
#define BUF_LINE_SIZE 10024
#define FILENAME 50
#define TIME 20
#define METHOD_LEN 5
#define PATH_LEN 1024
#define EXPLAIN_LEN 20
#define CONTENTTYPE_LEN 30
#define CHARASET_LEN 50
#define VERSION_LEN 10
#define CONTENT_LENGTH_LEN 10
#define EXTENSION_LEN 10
#define USER_INPUT_LEN 10
#define STATUSLINE_ARRAY 10
#define COOKIE_LEN 1024
#define CODE_200 0
#define CODE_303 1
#define CODE_400 2
#define CODE_403 3
#define CODE_404 4
#define EXTENSION_ARRAY 10
#define PATH_400 "/usr/local/bin/scd_education/www/400.html"
#define PATH_403 "/usr/local/bin/scd_education/www/403.html"
#define PATH_404 "/usr/local/bin/scd_education/www/404.html"
#define FILE_HEAD "/request_"
#define EXTENSION ".log"
#define CONTENT_DIR "/content/"
#define DOCUMENTROOT "/usr/local/bin/scd_education/www"
#define LOGFILETROOT "/usr/local/bin/scd_education/log"
#define DELETE_COOKIE_DATE "Fri, 31-Dec-1999 23:59:59 GMT"
#define COOKIEVALUE1 "cookie_value1"
#define COOKIEVALUE2 "cookie_value2"
#define COOKIEDELETE "/usr/local/bin/scd_education/www/cookie/logout.html"
#define LOGIN_CHECK_PATH "/usr/local/bin/scd_education/www/cookie/login_check.html"
#define CONTENT_PAGE_PATH "http://54.64.221.93:50000/cookie/content/welcome.html"
#define LOGIN_PAGE_PATH "http://54.64.221.93:50000/cookie/login.html"
#define USERNAME_TEST "admin"
#define PASSWORD_TEST "nmadmin001"

//logfileの名前を作成する関数
char* create_logfilename(char *);
//クッキーの有効期限を作成する関数
char* create_cookie_limit(char *);
//文字列を切り出して変数に格納する関数
char* strtok_string(char *, char *);
static int listen_socket (char *port);
struct statusline {
	char version[VERSION_LEN];
	int status_code;
	char explain[EXPLAIN_LEN];
};
struct extension_list {
    char extension[EXTENSION_LEN];
    char content_type[CONTENTTYPE_LEN];
	char charaset[CHARASET_LEN];
};


int main (int argc, char *argv[]) {
    int lissock;
    char filename[PATH_LEN], return_path[PATH_LEN], parameter_path[PATH_LEN], *p_parameter_path, method[METHOD_LEN], *p_method, path[PATH_LEN], *p_path, version[VERSION_LEN], *p_version, content_length[CONTENT_LENGTH_LEN], *p_content_length, username[USER_INPUT_LEN], password[USER_INPUT_LEN], *p_username, *p_password, user_cookie[COOKIE_LEN], *p_user_cookie;
    FILE *response;
	struct stat str_file;
	//ステータスラインの情報を格納する構造体
	struct statusline str_statusline[STATUSLINE_ARRAY] = {
		{"HTTP/1.0", 200, "OK"},
		{"HTTP/1.0", 303, "See Other"},
		{"HTTP/1.0", 400, "Bad Request"},
		{"HTTP/1.0", 403, "Forbidden"},	
		{"HTTP/1.0", 404, "Not Found"},
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

    lissock = listen_socket (DEFAULT_PORT);
    for (;;) {
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof addr;
        int accsock, status_line_flag = 0, tmp_status_code = 0, tmp_content_type = 0,error_flag = 0, int_content_length = 0, continue_flag = 0, cookie_create_check = 0;
        char buf_request[BUF_LINE_SIZE],buf[BUF_LINE_SIZE], redirect_location[PATH_LEN], buf_path[PATH_LEN], cookie_expires_string[TIME];
        FILE *fp, *sockf, *write_sockf;


		printf("create_logfilename前\n");
		strcpy(filename, create_logfilename(filename));
		printf("create_logfilenameあと%s\n", filename);

		//ソケットへの接続を待つ動作を用意する
        accsock = accept (lissock, (struct sockaddr*) &addr, &addrlen);
        if (accsock < 0) {
			fprintf (stderr, "accept failed\n");
			continue;
        }
		//httpリクエストを読み込むファイルディスクリプタ
		sockf = fdopen (accsock, "r");
		//httpレスポンスを返すファイルディスプリプタ
		write_sockf = fdopen(accsock, "w");
		//logを書き込むファイルディスクリプタ
		fp = fopen(filename, "w");
		//ここからリクエストを読み込み(while文の中が1リクエスト)
		while (fgets (buf, BUF_LINE_SIZE, sockf)) {
			if(status_line_flag == 0){
				printf("test");
				//一時変数に格納
				strcpy(buf_request, buf);
				//リクエストラインの解析
				p_method = strtok(buf_request, " ");
				strcpy(method, p_method);
				p_path = strtok_string(buf_request, p_path);
				printf("%s", path);
                p_path = strtok(NULL, " ");
				strcpy(path, p_path);
				p_version = strtok(NULL, " ");
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
				printf("user_cookieの値 %s\n", user_cookie);
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
					printf("username = \"%s\", password = \"%s\"", username, password);
					break;
				}
				if(strcmp(method, "GET") == 0){
					printf("ループから抜ける直前\n");
					break;
				}
			}
		}
		char buffer[BUF_LINE_SIZE];
		
		//レスポンスを返す
		if(strcmp(path, "/") == 0){
			strcpy(path, "/index.html");
		}
		printf("return_pathを返す直前%s\n", path);
		strcpy(return_path, DOCUMENTROOT);
		strcat(return_path, path);
		printf("フルパス %s\n", return_path);
		//400 badrequest ディレクトリトラバーサル対策

		if(strstr(return_path, "../") != NULL){
			strcpy(return_path, PATH_400);
            tmp_status_code = CODE_400;
            error_flag = 1;
		}
		printf("400処理の直後\n");
		//拡張子を取得
		char *extension = strstr(path, ".");
		printf("%s", extension);
		//.が無かった時の処理を入れる
		int i, debug;
		for(i = 0; i < EXTENSION_ARRAY; i++){
			debug = strcmp(extension, str_extension_list[i].extension);
			printf("%d", i);
			if(debug == 0){
				tmp_content_type = i;
	            printf("デバッグ！ %s,%d\n", str_extension_list[i].extension,debug);
				break;
			}
        }
		//構造体の中に該当の拡張子が無かった時の処理を入れる。
		printf("拡張子処理の直後\n");

		if(i < EXTENSION_ARRAY){
			error_flag = 1;	
		}

        int file_size = 0;
        struct stat file;
		//ブラウザに返すファイルを開く
        //404 該当するページが無かったときの処理

        if(stat(return_path, &str_file) != 0){
			printf("debug code:404の処理を通っていることを確認");
			strcpy(return_path, PATH_404);
			tmp_status_code = CODE_404;
			//tmp_content_type = CONTENT_TYPE_HTML;
			error_flag = 1;
		}
		int binary_flag = 0, text_flag = 0;
		if((strstr(str_extension_list[tmp_content_type].content_type, "image")) != NULL){
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
			printf("login_check.htmlの処理\n");
			if(strcmp(method, "POST") != 0){
				strcpy(redirect_location, LOGIN_PAGE_PATH);
				printf("postじゃなかった！\n");
			}
			//useridもパスワードも正しい場合

			if(strcmp(username, USERNAME_TEST) == 0 && strcmp(password, PASSWORD_TEST) == 0){
				strcpy(redirect_location, CONTENT_PAGE_PATH);
				printf("userid%s,pass %s", username,password);
			} else {
				strcpy(redirect_location,LOGIN_PAGE_PATH);
				printf("userid%s,pass %s", username,password);
				printf("どっちか間違えてる\n");
			}
			tmp_status_code = CODE_303;
		}
		//welcome.htmlに直接来た場合の処理

		if(strstr(return_path, CONTENT_DIR) != NULL){
			printf("welcome.html!\n");
			//cookieを持っていなかったらlogin.htmlにリダイレクト

			if(strcmp(user_cookie, COOKIEVALUE1) != 0){
				printf("user_cookie value=%s, COOKIEVALUE1=%s\n", user_cookie, COOKIEVALUE1);
				strcpy(redirect_location, LOGIN_PAGE_PATH);
				tmp_status_code = CODE_303;
			}
		}

		//403 閲覧禁止の場合の処理

		if(response == NULL && error_flag == 0){
            printf("debug code:403の処理を通っていることを確認");
			tmp_status_code = CODE_403;
        	strcpy(return_path, PATH_403);
			error_flag = 1;
			//上でパスを変更しているので開き直す
			response = fopen(return_path ,"r");
		}

		if(error_flag == 0){
			tmp_status_code = CODE_200;
		}


		/* ここから下がcookieの処理	*/
		//Cookieの作成（login_check.html)
		cookie_create_check = strcmp(return_path, LOGIN_CHECK_PATH);

		if(cookie_create_check == 0){
			strcpy(cookie_expires_string, create_cookie_limit(cookie_expires_string));
			fprintf(write_sockf, "Set-Cookie: CUSTOMER=%s; path=/cookie/; expires=%s GMT;\n", COOKIEVALUE1, cookie_expires_string);
		}

		fprintf(write_sockf, "%s %d %s\n", str_statusline[tmp_status_code].version, str_statusline[tmp_status_code].status_code, str_statusline[tmp_status_code].explain);
        fprintf(write_sockf, "Server: Apache\n");
		fprintf(write_sockf, "Keep-Alive:timeout=2, max=100\n");
        fprintf(write_sockf, "Accept-Ranges: bytes\n");
		if(tmp_status_code == CODE_303){
			fprintf(write_sockf, "Location:%s \n", redirect_location);
		}



		//Cookieの削除logout.html)
        int cookie_delete_check;
        cookie_delete_check = strcmp(return_path, COOKIEDELETE);
        printf("cookie_delete_check= %d, return_path=%s, COOKIEDELETE=%s\n", cookie_delete_check, return_path, COOKIEDELETE);
        if(cookie_delete_check == 0){
            fprintf(write_sockf, "Set-Cookie: CUSTOMER=%s; expires=%s;\n", COOKIEVALUE1, DELETE_COOKIE_DATE);
        }
		
		if(binary_flag == 1){
	        fprintf(write_sockf, "Content-Length: %d\n", file_size);
    	    fprintf(stdout, "Content-Length: %d\n", file_size);
		}
		//fprintf(write_sockf, "Content-Lenth: 20\n, fie_sie);
      fprintf(write_sockf, "Content-Type: %s\n", str_extension_list[tmp_content_type].content_type);
        fprintf(write_sockf, "\n");
		//バイナリデータの処理
		if(binary_flag == 1){
			int size = 0, k = 0;
			size = fread(buffer, sizeof( unsigned char ), 10000, response);
			for(k = 0; k < size; k++){
				fputc(buffer[k], write_sockf);
			}
		}
		if(text_flag == 1){
			while(fgets(buffer, sizeof(buffer), response)){
				fputs(buffer, write_sockf);
			}
				printf("text_flagの中\n");
		}
		fclose(write_sockf);
		fclose (sockf);
		fclose (response);
		fclose(fp);
		printf("close完了\n");
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
 *  create_cookie_limit  cookieの有効期限を作成
 *  引数：datetime:char型の文字配列
 *  戻り値：datetime:cookieの有効期限(GMT)
 *  ----------------------------------------------------------- */
char* create_cookie_limit(char *datetime)
{

	/* ここから下がcookieの処理	*/
   	time_t now;
	struct tm *str_gmt;
	char *wday[] = {"Sun","Mon","The","Wed","Thu","Fri","Sat"}, *wmon[] = {"Jan","Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	now = time(NULL); //時刻の取得
	str_gmt = gmtime(&now);

	sprintf(datetime, "%s, %d-%s-%d %d:%d:%d GMT;\n", COOKIEVALUE1, wday[str_gmt->tm_wday], str_gmt->tm_mday+1, wmon[str_gmt->tm_mon], str_gmt->tm_year+1900, str_gmt->tm_hour, str_gmt->tm_min, str_gmt->tm_sec);
	printf("%s", datetime);
	return datetime;

}


char* strtok_string(char *buffer,char *division_string)
{
	division_string = strtok(NULL, " ");
	strcpy(buffer, division_string);
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
