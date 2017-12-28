#include <sys/socket.h>  
#include <sys/types.h>  
#include <unistd.h>   
#include <arpa/inet.h>  
#include <netinet/in.h>  
#include <errno.h>  
#include <strings.h>  
#include <ctype.h>   
               
char host_name[20];  
int port = 8000;  
void bufferHandle(char *);
void MethodParse(char *,char *);
void DataParse(char *,char *);
int main()  
{     
          
        struct sockaddr_in sin,pin;  
        int sock_descriptor,temp_sock_descriptor,address_size;  
        int i , len , on=1;  
        char buf[16384];  
      
        sock_descriptor = socket(AF_INET,SOCK_STREAM,0);  
        bzero(&sin,sizeof(sin));  
        sin.sin_family = AF_INET;  
        sin.sin_addr.s_addr = INADDR_ANY;  
        sin.sin_port = htons(port);  
        if(bind(sock_descriptor,(struct sockaddr *)&sin,sizeof(sin)) == -1)  
        {  
                perror("call to bind");  
                exit(1);  
        }  
        if(listen(sock_descriptor,100) == -1)  
        {  
                perror("call to listem");  
                exit(1);  
        }  
        printf("Accpting connections...\n");  
  
        while(1)  
        {  
                address_size = sizeof(pin);  
                temp_sock_descriptor = accept(sock_descriptor,(struct sockaddr *)&pin,&address_size);  
                if(temp_sock_descriptor == -1)  
                {  
                        perror("call to accept");  
                        exit(1);  
                }  
                if(recv(temp_sock_descriptor,buf,16384,0) == -1)  
                {  
                        perror("call to recv");  
                        exit(1);  
                }  
                inet_ntop(AF_INET,&pin.sin_addr,host_name,sizeof(host_name));  
                printf("received from client(%s):\n%s\n",host_name,buf);  
 
                bufferHandle(buf);

                char * rsp = "http server has got client message\n";

                len = strlen(rsp);  
                for(i = 0 ; i  < len ; i++)  
                {  
                        buf[i] =  toupper(rsp[i]);  
                }  
  
                if(send(temp_sock_descriptor,rsp,len+1,0) == -1)  
                {  
                        perror("call to send");  
                        exit(1);  
  
                }  
			
                close(temp_sock_descriptor);  
        }    
}

void bufferHandle(char * buf){
    
	char method[5] = {""};
    char data[100] = {""};

	MethodParse(buf,method);
	DataParse(buf,data);

	if(strcmp(method,"POST") == 0){
		printf("receive http POST request\n");
	}
	if(strcmp(method,"GET") == 0)
		printf("receive http GET request\n");
    if(data)
	  printf("data:\n%s\n",data);
}

void MethodParse(char * buf,char method[]){
	char * p = strchr(buf,' ');
	strncpy(method,buf,(int)(p-buf));
}

void DataParse(char * buf,char data[]){

	int num = 0;
	char * p = buf;
    for(;*p!='\0';p=p+1){
		if(num != 6){
            if(*p=='\n')
		      num ++;
		}else{
//		  printf("data:%s\n",p);
          strcpy(data,p);		  
		  break;
		}
	}
}
