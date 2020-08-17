#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <termios.h>
#if 0
//#include <conio.h>
#endif
#include <time.h>
#include <sys/time.h>
#include "/usr/include/alsa/asoundlib.h"

pthread_mutex_t mutex;
int read_complete = 0;
int quit_commmand = false; 

#if 1 //#include <termios.h>
#define MODE (1)
int linux_kbhit(void) {
	struct termios oldt, newt;
	int ch;
        int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;

	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#if 1
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
#endif
	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#if 1
        fcntl(STDIN_FILENO, F_SETFL, oldf);
#endif

#if 1
        if(ch != EOF)
        {
                ungetc(ch, stdin);
                return 1;
        }

        return 0;
#else
        return ch;
#endif
}

int linux_getch(void) {

	int ch;
	struct termios buf, save;

	tcgetattr(0, &save);
	buf = save;

	buf.c_lflag &= ~(ICANON | ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	tcsetattr(0, TCSAFLUSH, &buf);

	ch = getchar();

	tcsetattr(0, TCSAFLUSH, &save);
	return ch;
}

        #if 1
char getKey(){
        if(linux_kbhit()) // 입력값이 있는지 확인
        {
                return getchar(); // 입력값이 있다면 char를 리턴함.
        }
        return '\0'; //입력값이 없다면 널문자 리턴함.
}
        #else
int getKey(){
        return linux_getch();
}
        #endif

#else //#include <conio.h>
char getKey(){
        if(kbhit()) // 입력값이 있는지 확인
        {
                return getch(); // 입력값이 있다면 char를 리턴함.
        }
        return '\0'; //입력값이 없다면 널문자 리턴함.
}
#endif

#if 0
// smapling 하여 버퍼에 저장하는 recording 함수
int record_sound(unsigned char *ap_rec_buffer1)
{	
	char fname[256]; // 파일이름
	FILE *p_file;	

	snd_pcm_t *p_capture_handle;    // sound 핸들
	snd_pcm_hw_params_t *p_hw_params; // sound 핸들에 적용할 파라메터
	unsigned int freq = 192000;  // 192Khz 샘플링 frequency 지정


        // mic입력의 sampling frequency로 버퍼에 저장하기 위해 sound핸들 open
        // 사운드 장치 중 "plughw:1,0"(card 1, device 0)장치를 녹음(capture) 모드로 open함.   
	if(snd_pcm_open(&p_capture_handle, "plughw:1,0", SND_PCM_STREAM_CAPTURE,0)<0){
      		perror("Cound not open output audio dev");
      		return 0;
	}	   
        
        //핸들에 적용할 파라메터의 메모리 할당         
	snd_pcm_hw_params_malloc (&p_hw_params);

        // 설정 값을 저장할 메모리에 기본(default) 값을 설정함   
	snd_pcm_hw_params_any (p_capture_handle, p_hw_params);	// set default value

	// 이 장치를 사용하여 send_pcm_readi 또는 send_pcm_writei 함수를 사용하 수 있도혹 함
	snd_pcm_hw_params_set_access (p_capture_handle, p_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);

	//샘플링 레이트는 부호를 고려하여 2바이트(16qlxm)로 설정하고 바이트 정렬방식은 little endian으로 세팅함
	snd_pcm_hw_params_set_format (p_capture_handle, p_hw_params, (snd_pcm_format_t)SND_PCM_FORMAT_S16_LE);  // signed 16 bits, little endian
	
        // sampling 주기는 192Khz로 함
	snd_pcm_hw_params_set_rate_near (p_capture_handle, p_hw_params, &freq, 0);
	
	// 모노 채널로 세팅함 
	snd_pcm_hw_params_set_channels (p_capture_handle, p_hw_params, 1); // 1 is Mono

	// 녹음 시 사용할 기준 값들을 저장한 p_hw_params를 녹음 장치에 정보 설정
        snd_pcm_hw_params (p_capture_handle, p_hw_params);

	// 녹음 장치에 설정한 기준 값의 메모리를 제거함
	snd_pcm_hw_params_free (p_hw_params);
	
	printf("\tStarted to record for 341ms...\n"); 
	
	// 녹음 시작
       	snd_pcm_prepare (p_capture_handle);

	snd_pcm_readi (p_capture_handle, ap_rec_buffer1, 65536); // 65536 개의 샘플링을 함 (중요:참고로 65536 바이트를 의미하는 것이 아님)

	printf("\tFinished to record!\n");

	// 녹음 중지
	snd_pcm_drop(p_capture_handle);

        // 녹음 장치 닫기 
	snd_pcm_close (p_capture_handle);
	
	return 1;
}

#else
// smapling 하여 버퍼에 저장하는 recording 함수
int record2_sound(unsigned char *ap_rec_buffer1, unsigned char *ap_rec_buffer2)
{	
	char fname[256]; // 파일이름
	FILE *p_file;	

	snd_pcm_t *p_capture_handle;    // sound 핸들
	snd_pcm_hw_params_t *p_hw_params; // sound 핸들에 적용할 파라메터
	unsigned int freq = 192000;  // 192Khz 샘플링 frequency 지정


        // mic입력의 sampling frequency로 버퍼에 저장하기 위해 sound핸들 open
        // 사운드 장치 중 "plughw:1,0"(card 1, device 0)장치를 녹음(capture) 모드로 open함.   
	if(snd_pcm_open(&p_capture_handle, "plughw:1,0", SND_PCM_STREAM_CAPTURE,0)<0){
      		perror("Cound not open output audio dev");
      		return 0;
	}	   
        
        //핸들에 적용할 파라메터의 메모리 할당         
	snd_pcm_hw_params_malloc (&p_hw_params);

        // 설정 값을 저장할 메모리에 기본(default) 값을 설정함   
	snd_pcm_hw_params_any (p_capture_handle, p_hw_params);	// set default value

	// 이 장치를 사용하여 send_pcm_readi 또는 send_pcm_writei 함수를 사용하 수 있도혹 함
	snd_pcm_hw_params_set_access (p_capture_handle, p_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);

	//샘플링 레이트는 부호를 고려하여 2바이트(16qlxm)로 설정하고 바이트 정렬방식은 little endian으로 세팅함
	snd_pcm_hw_params_set_format (p_capture_handle, p_hw_params, (snd_pcm_format_t)SND_PCM_FORMAT_S16_LE);  // signed 16 bits, little endian
	
        // sampling 주기는 192Khz로 함
	snd_pcm_hw_params_set_rate_near (p_capture_handle, p_hw_params, &freq, 0);
	
	// 모노 채널로 세팅함 
	snd_pcm_hw_params_set_channels (p_capture_handle, p_hw_params, 1); // 1 is Mono

	// 녹음 시 사용할 기준 값들을 저장한 p_hw_params를 녹음 장치에 정보 설정
        snd_pcm_hw_params (p_capture_handle, p_hw_params);

	// 녹음 장치에 설정한 기준 값의 메모리를 제거함
	snd_pcm_hw_params_free (p_hw_params);
	
	printf("\tStarted to record every 341ms...\n"); 

	// 녹음 시작
	snd_pcm_prepare (p_capture_handle);

	int char_key;
	while(quit_commmand != true){
#if 1
		printf("\tStarted to record for buffer 1...\n");
		snd_pcm_readi (p_capture_handle, ap_rec_buffer1, 65536); // 65536 개의 샘플링을 함 (중요:참고로 65536 바이트를 의미하는 것이 아님)
		pthread_mutex_lock(&mutex);
		read_complete |= 0x01;
		pthread_mutex_unlock(&mutex);
		printf("\tFinished to record for buffer 1...\n");
 
 	#if 1
		//pthread_mutex_lock(&mutex);
		if (quit_commmand == true){
			break;
		}
		//pthread_mutex_unlock(&mutex);
	#endif
		printf("\tStarted to record for buffer 2...\n");
		snd_pcm_readi (p_capture_handle, ap_rec_buffer2, 65536); //
		pthread_mutex_lock(&mutex);
		read_complete |= 0x02;
		pthread_mutex_unlock(&mutex);
		printf("\tFinished to record buffer 2...\n");
#endif
#if 1
		char_key = getKey();
		if (char_key == 'q'){
				break;
		}
#endif        
	}

	printf("\tFinished to record every 341ms...\n");
	// 녹음 중지
	snd_pcm_drop(p_capture_handle);
    // 녹음 장치 닫기 
	snd_pcm_close (p_capture_handle);
	
	return 1;
}
#endif

#if 0
// main()에 호출한 스레드 함수
void* thread_routine(void *arg){
        char fname[256]; // 파일이름
	FILE *p_file;
        pthread_t tid;
#if 1
	#if 0
	    gettimeofday에서 두번째 인자인 struct timezone 변수는 내부적으로 사용되지 않는다
	    
	    struct timeval {
		time_t      tv_sec;     /* seconds */
		suseconds_t tv_usec;    /* microseconds */
	    };
	#endif

	struct timeval tv; // 녹음 시간.
	struct tm *record_time; // 녹음 시간의 한국 localtime
#endif        
 
        tid = pthread_self();
 
        int i=0;
        printf("\tThread tid:%x\n",(unsigned int)tid); // 스레드의 tid 표시함.
        //printf("\tThread file name: %s\n", rfname);

        //p_file = fopen(fname, "wb"); // main()에서 입력된 파일이름으로 파일 생성.

        // 테스트 코드, 수행 안됨.
        while(false){
                printf("\tnew thread:%d\n",i);
                i++;
                sleep(1);
        }

        int charKey;
        unsigned char rec_buffer1[65536*2];  // 실제 필요한 버퍼 사이즈: 2bytes * 19200KHz * 0.341 msec => 131072 bytes.
#if 0        
        unsigned char rec_buffer2[65536*2];
#endif
#if 1
        while(true){
                if (gettimeofday(&tv, NULL)){
			printf("\t{Error] ===> gettimeofday Error\n");
			break;	
		}

                record_time = localtime(&tv.tv_sec);
		sprintf(fname, "%02d:%02d:%02d:%6ld-usec.wav", record_time->tm_hour, record_time->tm_min, record_time->tm_sec, tv.tv_usec);
                p_file = fopen(fname, "wb"); 
		printf("\tCreating the file, %s\n",fname);
		
                if(record_sound(rec_buffer1) == 1) { // sampling 하는 함수 실행.
		        fwrite((char *)rec_buffer1, 1, sizeof(rec_buffer1), p_file); // sampling된 버퍼를 파일로 저장.
		        fclose(p_file);
		        printf("\tFinished to save the file (%s) !\n", fname );

		}else{
		        // Do nothing here.
                        printf("\t{Error ===>] Save the file (%s) !\n", fname );
		}
#if 1
                charKey = getKey();
                if(charKey == 'q'){
                        break;                     
                }
#endif
        }	
#else
	while((getKey() != 'q')){
		if(record2_sound(rec_buffer1, rec_buffer2) == 1) { // sampling 하는 함수 실행.
		        fwrite((char *)rec_buffer1, 1, sizeof(rec_buffer1), p_file); // sampling된 버퍼를 파일로 저장.
		        fclose(p_file);
		        printf("\tFinished to save the file (%s) !\n", fname );

		}else{
		        // Do nothing here.
		}
	}
#endif
        pthread_exit(NULL);
        
}
#endif

unsigned char rec_buffer1[65536*2];  // 실제 필요한 버퍼 사이즈: 2bytes * 19200KHz * 0.341 msec => 131072 bytes.
#if 1        
unsigned char rec_buffer2[65536*2];
#endif

// main()에 호출한 스레드 함수
void* thread1_routine(void *arg){
	FILE *p_file;
    pthread_t tid;
	int status1 = 0;
    
        tid = pthread_self();
 
        int i=0;
        printf("\tThread tid:%x\n",(unsigned int)tid); // 스레드의 tid 표시함.
        //printf("\tThread file name: %s\n", rfname);

        // 테스트 코드, 수행 안됨.
        while(false){
                printf("\tnew thread:%d\n",i);
                i++;
                sleep(1);
        }

        int charKey;
#if 1
        //while(true){
		if (true){
            if(record2_sound(rec_buffer1, rec_buffer2) == 1) { // sampling 하는 함수 실행.
#if 0
				charKey = getKey();
				if(charKey == 'q'){
						break;                     
				}
#endif
			}
		}
#else
	while((getKey() != 'q')){
		if(record2_sound(rec_buffer1, rec_buffer2) == 1) { // sampling 하는 함수 실행.
		        fwrite((char *)rec_buffer1, 1, sizeof(rec_buffer1), p_file); // sampling된 버퍼를 파일로 저장.
		        fclose(p_file);
		        printf("\tFinished to save the file (%s) !\n", fname );

		}else{
		        // Do nothing here.
		}
	}
#endif
        pthread_exit(NULL);
        
}


void* thread2_routine(void *arg){
	char fname[256]; // 파일이름
	FILE *p_file;
    pthread_t tid;
	int status2 = 0;
#if 1
	#if 0
	    gettimeofday에서 두번째 인자인 struct timezone 변수는 내부적으로 사용되지 않는다
	    
	    struct timeval {
		time_t      tv_sec;     /* seconds */
		suseconds_t tv_usec;    /* microseconds */
	    };
	#endif

	struct timeval tv; // 녹음 시간.
	struct tm *record_time; // 녹음 시간의 한국 localtime
#endif    

	tid = pthread_self();
 
    int i=0;
    printf("\tThread tid:%x\n",(unsigned int)tid); // 스레드의 tid 표시함.

	int charKey;
	while(true){
		if(read_complete & 0x01) {// buffer1 read complete
			if (gettimeofday(&tv, NULL)){
					printf("\t{Error] ===> gettimeofday Error\n");
					break;	
			}

			record_time = localtime(&tv.tv_sec);
			sprintf(fname, "%02d:%02d:%02d:%6ld-usec.wav", record_time->tm_hour, record_time->tm_min, record_time->tm_sec, tv.tv_usec);
            p_file = fopen(fname, "wb"); 
			printf("\tCreating the file, %s using Buffer 1.\n",fname);
		
		    fwrite((char *)rec_buffer1, 1, sizeof(rec_buffer1), p_file); // sampling된 버퍼를 파일로 저장.
		    fclose(p_file);
		    printf("\tFinished to save the file (%s) using Buffer 1.\n", fname );

			pthread_mutex_lock(&mutex);
			read_complete &= ~0x01;
			pthread_mutex_unlock(&mutex);

		}
		
		if (read_complete & 0x02){
			if (gettimeofday(&tv, NULL)){
					printf("\t{Error] ===> gettimeofday Error\n");
					break;	
				}

            record_time = localtime(&tv.tv_sec);
			sprintf(fname, "%02d:%02d:%02d:%6ld-usec.wav", record_time->tm_hour, record_time->tm_min, record_time->tm_sec, tv.tv_usec);
            p_file = fopen(fname, "wb"); 
			printf("\tCreating the file, %s using Buffer 2\n",fname);
		
            fwrite((char *)rec_buffer2, 1, sizeof(rec_buffer2), p_file); // sampling된 버퍼를 파일로 저장.
		    fclose(p_file);
		    printf("\tFinished to save the file (%s) using Buffer 2.\n", fname );

			pthread_mutex_lock(&mutex);
			read_complete &= ~0x02;
			pthread_mutex_unlock(&mutex);
		}

#if 1
		charKey = getKey();
		if(charKey == 'q'){
			//pthread_mutex_lock(&mutex);
			quit_commmand = true;
			//pthread_mutex_unlock(&mutex);
			break;                     
		}
#endif
	}

	pthread_exit(NULL);
}

// "micInput" 실행 시 실행 방법 표시함.
void usage(){
        printf("[Error] Require options as belows.\n");
        printf("Recording example: ./micInput\n");
}

// 에러를 출력하는 매크로 함수
#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

//main() 함수
int main(int argc, char *argv[]) {
        int stack_size;
        int s, opt;
        pthread_t thread1;
		pthread_t thread2;
        pthread_attr_t attr;
		int status;

        // no arguement
        if (argc != 1){ 
                usage();
                exit(EXIT_FAILURE);        
        }
#if 0
        int mode;
        char *fname;

        // 2번째 argument가 저장하려는 파일 이름   
        if(argv[1][1] == 'r'){
                mode = 0; // Read:0
                fname =  &argv[2][0];
                //printf("Main file name: %s\n", fname);             
        }else{
                usage();
                exit(EXIT_FAILURE);
        }
#endif
        pthread_mutex_init(&mutex,NULL);

	// 스레드 생성 시 필요한 attribute 초기화
        s = pthread_attr_init(&attr);
        if(s != 0 )
                handle_error_en(s, "pthread_attr_init");

#if 1
         // 스레드 생성
        s = pthread_create(&thread1,&attr,thread1_routine, (void *)"thread1");
		if(s != 0)
                handle_error_en(s, "thread1_create");
		s = pthread_create(&thread2,&attr,thread2_routine, (void *)"thread2");
		if(s != 0)
                handle_error_en(s, "thread2_create");
#else
         // 스레드 생성
        s = pthread_create(&thread,&attr,thread_routine, (void *)fname);
		if(s != 0)
                handle_error_en(s, "thread_create");
#endif
        

        // 스레드 생성 후 attribute 제거 
        s = pthread_attr_destroy(&attr);
        if (s != 0)
                handle_error_en(s, "pthread_attr_destroy");
        
        int i=0;
        // Main()의 tid 출력
        printf("Main tid:%x\n", (unsigned int)pthread_self());

        //테스트 코드, 수행 안됨.
        while(false){
                printf("main:%d\n",i);
                i++;
                sleep(1);
        }
        
        // 스레드 join함
        pthread_join(thread1, (void **)&status);
        pthread_join(thread2, (void **)&status);

		pthread_mutex_destroy(&mutex);
        exit(EXIT_SUCCESS);

}
