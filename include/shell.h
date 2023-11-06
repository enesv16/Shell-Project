#ifndef SHELL_H
#define SHELL_H

static char* currentPath;//Mevcut çalışma dizinini tutuyor
static int lineLength=0;
static int ProcessCount=0;

char *read_line(); //Girilen satırı okuyan ve döndüren fonksiyon
char **split_line(char *);//Kendisine gönderilen satırı ayırıcı karakterlere göre ayırıp bunları döndüren,argümanları tutan fonksiyon
int execute(char **);//Kendisine gönderilen argümanlar doğrultusunda yapması gereken işleri tamamlayan fonksiyon
int builtin_exit( );//Shell'den çıkmayı sağlayan fonksiyon
int *process;
int showpid();
int builtin_cd(char **args);
int builtin_help();
int args_length(char **args);
void Prompt();
void Entry();
void loop();//Shell'in sürekliliğini sağlayan döngüyü barındıran fonksiyon

#endif
