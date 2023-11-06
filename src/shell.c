#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TOKEN_DELIM " \t\r\n"
// SİL #define RESET "\e[0m"
#define BUFF_SIZE 80    //Komut satırına girilen satırın 80 karakter sınırını taşıması adına tanımlandı
#define TK_BUFF_SIZE 64
#define processnumber 30

//Renk Kodları
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"
#define reset "\e[0m"

#include "shell.h"

int showpid(){ //Execute satırında her forklanmadan sonra oluşturulan (yani kabuk tarafından üretilmiş) processlerin id'lerini göstermekte
	printf("\n%s**************************************************%s\n",YEL,reset);
	printf(" %s processes created by shell %s\n", BLU,reset);
	//printf("%d",Shellpid);
	for(int i=0;i<ProcessCount;i++) 
	 printf("%d-) %d\n",i+1,process[i]);
 	printf("\n%s**************************************************%s\n",YEL,reset);
		return 1;
		 
}

int builtin_exit() { //Kullanıcı shell'e exit yazdığında çıkış sağlanacak
	printf("%sFile was closed by user request with Exit command%s\n", RED, reset);
  return 0;
}

int builtin_help() //Kullanıcı shell'e help yazdığında çalışacak olan fonksiyon , built-in komutları listelemekte
{
	
		fprintf(stderr,"\n*******************\n"
				"Supported Commands:\n1. showpid\n2. help\n3. cd\n4. exit"
				"\n*******************\n\n");	
	return 1;
}

int builtin_cd(char **args)//Kullanıcı shell'e cd yazdığında ilgili kontroller sağlandıktan sonra mevcut konumunu değiştirmesi sağlandı.
{
	if(args[1] == NULL)
	{
		fprintf(stderr, "%sPlease enter a path to cd%s\n", RED, reset);
	}
	else
	{
		if(chdir(args[1]) > 0)//Dizin değiştirme "chdir" fonksiyonu ile yapıldı
		{
			perror("ERROR\n");			
		}
	}
	return 1;
}

char **split_line(char * line) { //Kendisine gönderilen satır parametresini parçalayan,bölen ve bu parçaları döndüren fonksiyon
	int buffsize = TK_BUFF_SIZE, position = 0; //
	char **tokens = malloc(buffsize * sizeof(char *));//Parçalar için ek bir bellek alanı oluşturuldu 
	char *token;

  if (!tokens) {//Eğer  parçalar için oluşturulan ek bellek alanında sıkıntı varsa hata döndürüldü ve fonksiyon başarısız olarak sonlandırıldı.
    fprintf(stderr, "%sAllocation error%s\n", RED, reset);
    exit(EXIT_FAILURE);
  }
  
  token = strtok(line, TOKEN_DELIM); //Burada strtok fonksiyonu ile satırlar ilgili ayırıcı karakterlere bölümlendirildi.
									 //Her ayırıcı karakter bölümünü NULL'ile sonlandırıp strtok fonksiyonunu besledik.
  while (token != NULL) {			//Daha böldüğü satırları döndürecek
    tokens[position] = token;
    position++;

    if (position >= buffsize) {
      buffsize += TK_BUFF_SIZE;
      tokens = realloc(tokens, buffsize * sizeof(char * ));

      if (!tokens) {
        fprintf(stderr, "%sAllocation error%s\n", RED, reset);
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, TOKEN_DELIM);
  }
  tokens[position] = NULL;

  return tokens;
}

char *read_line() {  //Shell'den girilen satırın okuma işlemini gerçekleştiriyor.
  int buffsize = BUFF_SIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buffsize); //Satırı dinamik bir bellek bölgesi ayırdık
  int c;
 
  if (!buffer) { //Yer ayrılmadıysa hata döndür
    fprintf(stderr, "%sAllocation error%s\n", RED, reset);
    exit(EXIT_FAILURE);
  }

  while (1) { //Her bir satırı oku ve belirli karakterlere geldiğinde \0 ekle ve bu satırı döndür
    c = getchar();
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;
	lineLength++;   
  }
}

int execute(char **args) { //Kendisini gönderilen parametreleri işleyen  fonksiyon
  int cpid;
  int status;
	//**Builtin kısmı
  if (strcmp(args[0], "exit") == 0||atoi(args[0])==4) {
    return builtin_exit();
  }
    else if (strcmp(args[0], "cd") == 0||atoi(args[0])==3) {
    return builtin_cd(args);
  }
    else if (strcmp(args[0], "help") == 0||atoi(args[0])==2) {
    return builtin_help();
  }
   else if (strcmp(args[0], "showpid") == 0) {
	  return showpid(); 
	}
//*****
  cpid = fork(); //Ebevyn process fork ile kendi kopyasını oluşturmuş oldu
  
  if (cpid == 0) {//Eğer oluşturulmış processin fork değeri 0'a eşitse bu yavru processtir.
    if (execvp(args[0], args) < 0)//Gönderilen argümanların ilki her zaman komutu barındırdığından dizinin ilk elemanı ve argümanları olarak execvp'ye gönderildi
	{
		printf("%sCommand not found: %s", RED, reset);//Eğer fonksiyon sonucu 0'dan küçük ise böyle bir komutun olmadığı ifade edildi.
		printf("%s\n", args[0]);
	}
    exit(EXIT_FAILURE); //Ve execute fonksiyonu başarısız olarak sonlandırıldı.

  } else if (cpid < 0)//Eğer oluşturulmuş process'in fork değeri 0'dan küçük ise fork işlenirken hata oluşmuş demektir 
    printf(RED "Error forking"
      reset "\n");
   else { //Burada oluşturulmuş yavru processlerin id'leri bir process dizisinde tutuldu.Showpid kısmında listelenmesi adına
	  process[ProcessCount]=cpid;
    waitpid(cpid, & status, WUNTRACED);//Zombi processlerin oluşmasını engellemek adına ebevyn'in yavrunun sonlanması anına kadar bekletilmesi işlemi gerçekleştirildi.
  }
  ProcessCount++;
  return 1;
}

int args_length(char **args)//Kullanıcının girmiş olduğu argüman sayısını hesaplayan fonksiyon
{
	int i = 0;

	while(args[i] != NULL)
	{
		i++;
	}
	return i;
}

void Prompt()//Kullanıcının girmesi gereken komutlara yol gösteren ve girdiye hazır olduğunu ifade eden fonksiyon
{

//-------------------show the path-----------------------------

   char path[1024]="";
   printf(MAG "%s/: sau > "reset ,getcwd(currentPath,1024)  );
}

void Entry(){//Girişte kullanıcıya sunulan karşılama mesajını gösteren fonksiyon
	printf("\n%s **************************************%s ",YEL,reset);
	printf("\n %s***%s %s      WELCOME%s %sTO%s %sOUR%s %sSHELL%s     %s***%s\n ",YEL,reset,RED,reset,MAG,reset,YEL,reset,BLU,reset,YEL,reset);
	printf("%s**************************************%s\n\n ",YEL,reset);
}

void loop() { //Shell'in sürekliliğini döngüsel olarak sağlayan fonksiyon
  char *line;
  char **args;
  int status = 1;
  int length;
 
	process=malloc(processnumber*sizeof(int)); //Showpid' işleminde processlerin pid'leri tutan dizi
	
	if( process == NULL)
    fprintf(stdout, "Hata olustu");

	Entry(); //Karşılama mesajı verildi.
   do {//döngü başladı
	lineLength=0;
	    Prompt(); //Prompt çalıştırıldı

    line = read_line(); //Satır okundu
	if(lineLength<80){//Okunan satırın 80 karakterden büyük olması engellendi ve hata mesajı verildi
		
		args = split_line(line); //Satır parçalandı ve argümanlar args'ye atandı.
		length=args_length(args);//Argümanların sayısı öğrenildi
		
		
		if(length>9){ //Eğer argüman sayısı 9'dan büyük ise bu durumda argüman sayısının fazlalığından dolayı hata mesajı verildi
			fprintf(stderr, "%sTo many Arguments%s\n", RED, reset);
		}
		else{
			status = execute(args);//Son durumda eldeki argümanlar çalıştırılmak üzere fonskiyona yönlendirildi.
		}
		 
		free(line);//Oluşturulmuş bellek bölgeleri serbest bırakıldı çöp oluşmaması sağlandı
		free(args); 				
	}
	else{
		
		fprintf(stderr, "%sCharacter limit exceeded, Command line cannot exceed  * 80 *  characters  %s\n", RED, reset);
	}  
   } while (status); 	
	free(process);   
}

int main()
{		
  loop(); //Döngü çalıştırıldı ve işlem başlatıldı. 
  return 0;
}
