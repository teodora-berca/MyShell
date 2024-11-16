#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

void LeaveMyShell()
{
        char* username = getenv("USER");
        printf("La revedere utilizator %s.\n", username);
        printf("Parasire MyShell.\n");
        sleep(5);
        exit(0);
}


void mycp(const char* fisier1,const char* fisier2)
{
    ssize_t n;
    char buf[10000];
    int f_c = open(fisier1, O_RDONLY);
    int f_s = open(fisier2, O_WRONLY | O_CREAT, 0644); // cream fisierul daca nu exista
    if(f_c < 0 || f_s < 0)
    {
        perror("Eroare aparuta la deschiderea fisierelor");
        close(f_c);
        close(f_s);
        return;
    }
    while((n = read(f_c, buf, sizeof(buf))) > 0)
    {
        if(write(f_s, buf, n) != n)
        {
            perror("Eroare la scrierea in fisierul destinatie");
            close(f_c);
            close(f_s);
            return;
        }
    }

    if(n < 0)
    {
        perror("Eroare la citirea din fisierul sursa");
    }
    else
    {
        printf("Fisierul '%s' a fost copiat cu succes in '%s'.\n", fisier1, fisier2);
    }
    close(f_c);
    close(f_s);
}



void myrm(const char *filePath)
{
	if(remove(filePath)!=0)
		perror("Eroare aparuta la stergerea fisierului");
	else
		printf("Fisierul '%s' a fost sters cu succes.\n", filePath);
  return;
}

void mymv(const char *sourcePath, const char* destinationPath )
{
	if(rename(sourcePath, destinationPath)!=0)
		perror("Eroare aparuta la mutarea fisierului");
	else
		printf("Fisierul a fost mutat din '%s' in '%s'.\n", sourcePath, destinationPath);
  return;
}

void mymkdir(const char *name)
{
	if((mkdir(name, 0777))==-1)
	{
		perror("Nu s-a putut crea directorul");
	}
	else
	{
		printf("Directorul '%s' a fost creat.\n",name);
	}
  return;
}


void myls(const char* director)
{
        DIR* dir;
        dir = opendir(director);
        struct dirent *d;
        if(dir==NULL)
        {
                if(errno = ENOENT) //daca nu s-a gasit un director cu acest nume
                {
                        perror("Nu exista un director cu acest nume");
                }
                else //daca eroarea apare din alt motiv
                {
                        perror("Nu s-a putut citi acest director");
                }
        }
        else
        {
                while((d=readdir(dir))!=NULL) //cat timp mai citim din director
                {
                        if(d->d_name[0] == '.') //daca gasim inregistrarile . sau ..
                                {
                                        continue; //trecem mai departe
                                }
                        else
                                printf("%s\n", d->d_name); //afisam numele fisierului
                }
        }
        closedir(dir);
}


void mysearch(const char *path, const char *searchTerm)
{
    DIR *dir = opendir(path);
    if (!dir)
    {
        fprintf(stderr, "Eroare la deschiderea directorului '%s'\n", path);
        return;
    }

    struct dirent *entry;
    int i=0;
    while ((entry = readdir(dir)) != NULL ) //cat timp se citesc fisiere din director
    {
       // ignoram intrarile '.' și '..', ca sa nu cautam recursiv in directorul curent sau parinte
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      {
                  continue;
      }

        char fullPath[1024];
        if (snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name) >= sizeof(fullPath))
        {
            fprintf(stderr, "Eroare: Calea este prea lungă pentru '%s/%s'.\n", path, entry->d_name);
            continue;
        }

        struct stat statbuf;  //prin stat obtinem informatii despre fisier
        if (stat(fullPath, &statbuf) == -1)
        {
            fprintf(stderr, "Eroare la obținerea informațiilor pentru '%s'\n", fullPath);
            continue;
        }

        //daca ce am gasit e un director si nu are numele corespunzator cautam recursiv si in acesta
        //st_mode memoreaza tipul de fisier
        if (S_ISDIR(statbuf.st_mode) && strcmp(entry->d_name, searchTerm) !=0)
        {
            mysearch(fullPath, searchTerm);
        }
        else if (strcmp(entry->d_name, searchTerm) ==0)
        {
            printf("Fisierul a fost gasit: %s\n", fullPath);
            break;
        }
    }

    closedir(dir);
    return;
}

void mycd(const char *path)
{
    if (chdir(path) != 0)
    {
        perror("Eroare la schimbarea directorului");
    }
    else
    {
        printf("Directorul curent a fost schimbat in '%s'.\n", path);
    }
    return;
}

void show_history()
{
    HIST_ENTRY **istoric;
    istoric = history_list();
    if (istoric)
    {
        for (int i = 0; istoric[i]; i++)
        {
            printf("%d: %s\n", i + history_base, istoric[i]->line);
        }
    }
}


void ExecutaComanda(char* comanda)
{

  int i = -1;
  char* cuvant;
	char* linieComanda[6];
  cuvant=strtok(comanda," "); //impartim comanda in functie de spatii
  while(cuvant!=NULL)
  {
      i=i+1;
      linieComanda[i]=cuvant;  //linieComanda contine functia si argumentele
      cuvant=strtok(NULL," ");
  }

	if(strcmp(linieComanda[0],"mycp")==0)
	{
		if(i==2)
		{
			mycp(linieComanda[1],linieComanda[2]);
		}
		else     //daca dupa comanda mycp nu se gasesc exact 2 argumente afisam mesajul
		{
			printf("mycp are nevoie de 2 argumente.\n");
		}
	}

	else if(strcmp(linieComanda[0],"myrm")==0)
	{
		if(i==1)
		{
			myrm(linieComanda[1]);
		}
		else
		{
			printf("myrm are nevoie de 1 argument.\n");
		}
	}

	else if(strcmp(linieComanda[0],"mymkdir")==0)
	{
		if(i==1)
		{
			mymkdir(linieComanda[1]);
		}
		else
		{
			printf("mymkdir are nevoie de 1 argument.\n");
		}
	}

	else if(strcmp(linieComanda[0],"mymv")==0)
	{
		if(i==2)
		{
			mymv(linieComanda[1],linieComanda[2]);
		}
		else
		{
			printf("mymv are nevoie de 2 argumente.\n");
		}
	}

  else if (strcmp(linieComanda[0], "mysearch") == 0)
    {
        if (i == 2)
        {
            mysearch(linieComanda[1], linieComanda[2]);
        }
        else
        {
            printf("mysearch are nevoie de 2 argumente.\n");
        }
    }

    else if (strcmp(linieComanda[0], "mycd") == 0)
      {
          if (i == 1)
          {
              mycd(linieComanda[1]);
          }
          else
          {
              printf("mycd are nevoie de un argument.\n");
          }
      }


      else if (strcmp(linieComanda[0], "myls") == 0)
        {
            if (i == 1)
            {
                myls(linieComanda[1]);
            }
            else
            {
                printf("myls are nevoie de un argument.\n");
            }
        }

  else if (strcmp(linieComanda[0], "myhistory") == 0)
  {
      show_history();
  }


	else if(strcmp(linieComanda[0],"LeaveMyShell")==0)    //iesim din shell
	{
		LeaveMyShell();
	}

  else printf("Comanda introdusa nu este corecta!\n");
}


void hello() //deschidere shell
{
	system("clear");
	printf("\n\n\n");
	printf("**********************************************************************\n");
	printf("****************************MyShell***********************************\n");
	char* username = getenv("USER");
	printf("UTILIZATOR:%s\n", username);
	printf("\n\n");
  printf("Lista de functionalitati:\n");
  printf("\n");
  printf("mycp [fisier1] [fisier2] -> copiază conținutul [fisier1] în [fișier2] \n");
  printf("mymkdir [director] -> creează un nou director în directorul curent cu numele [director]\n");
  printf("myrm [nume_fisier] -> șterge nume_fisier\n");
  printf("mymv [path1] [path2] -> mută fișierul de la path1 la path2\n");
  printf("myls [nume_director] -> listează toate fișierele care sunt în directorul nume_director\n");
  printf("mysearch [nume_director] [nume_fisier] -> caută fișierul [nume_fisier] în directorul [nume_director]\n");
  printf("mycd [path] -> ne mută în sistemul de fișiere la calea specificată [path]\n");
  printf("myhistory -> ne afișează comenzile introduse anterior\n");
  printf("\n");
	printf("Pentru a parasi MyShell introdu comanda LeaveMyShell.\n");
}

void IntroduComanda()    //citim input-ul
{
	char *comanda;
  char currentPath[PATH_MAX];
  //la fiecare introducere de comanda afisam si directorul curent
  if (getcwd(currentPath, sizeof(currentPath)) != NULL)
  {

       printf("\nTe afli in %s. Introdu urmatoarea comanda:\n", currentPath);
   } else {
       perror("Eroare la obtinerea directorului curent");
   }
 comanda = readline(""); // Citeste comanda folosind readline
 if (comanda && *comanda)
 { // Verifică dacă comanda nu este goală
       add_history(comanda); // Adaugă comanda în istoric
       comanda[strcspn(comanda,"\n")] = 0;
       ExecutaComanda(comanda); // Execută comanda
   }
	free(comanda); // Eliberează memoria alocată de readline
}


int main(int argv, char* argc[])
{
	hello();   //afisam mesajul de intrare
	while(1)     //cat timp nu se introduce comanda LeaveMyShell citim
	{
		IntroduComanda();
	}
	exit(0);
}