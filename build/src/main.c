#include <stdio.h>

void main (void)
{
	FILE * fd;
	FILE * script;
	FILE * luascript;
	FILE * luascript2;
	FILE * countfile;

	char countstring[100];
	char string [100];
	char comment [500];
	char text [500];
	char time [5];
	char action [500];
	char path [100];
	char destfile [20];
	int count;
	int filescount;
	char filescount_str[10];

	char c;
	int i;

	while(1)
	{

	system("mv filescount filescount2");
	
	system("rm list");
	system("ls files>>list");

	
	fd = fopen("list","r");
	script = fopen("script","w");

	count = 0;

	system("find files -type f | wc -l>>filescount");

	countfile = fopen("filescount","r");
	fscanf(countfile,"%s",filescount_str);
	fclose(countfile);

	filescount = atoi(filescount_str);
	
	while(count<filescount)
	{
		for (i=0;i<500;i++)
			text[i]='\0';
		count ++;
		fscanf(fd,"%s",string);
		printf("%s\n",string);
		sprintf(path,"files/%s",string);
		printf("path:%s\n",path);

		luascript = fopen(path,"r");
		fscanf(luascript,"--%s\n",time);
		fscanf(luascript,"--%s\n",action);
//		fscanf(luascript,"--%s",text);
		
		i=0;
		c=' ';
		if(c!='\n')
			fscanf(luascript,"%c",&c);
		if(c!='\n')
			fscanf(luascript,"%c",&c);

		while(c!='\n')
		{
			fscanf(luascript,"%c",&c);
//			printf("%c",c);
			text[i]=c;
			i++;
		}		
		text[i-1]='\0';

		printf("time:%s\naction:%s\ntext:%s\n",time,action,text);
		fclose(luascript);
			
		if(!strcmp(action,"picture"))
		{
			printf("drawing a picture...\n");
			if(atoi(time)>=1000)
			{
				fprintf(script,"cp %s test.dat\n./displayd picture.lua\nsleep %i\n",text,atoi(time)/1000);
			}
			else
			{
				fprintf(script,"cp %s test.dat\n./displayd picture.lua\nwait %s\n",text,time);
			}
		}
		else
		{
			printf("count=%i\n",count);
			printf("countstring=%i.lua",count);
			sprintf(countstring,"%i.lua",count);
			
			luascript2 = fopen(countstring,"w");
			printf("sending text...\n");
			fprintf(luascript2,"diSendText(\"*** %s \");\n",text);
			if(atoi(time)>=1000)
			{
				fprintf(script,"./displayd %s\nsleep %i\n",countstring,atoi(time)/1000);

			}else{
				fprintf(script,"./displayd %s\nwait %s\n",countstring,time);
			}
			fclose(luascript2);
		}
	}
	fclose(fd);	
	fclose(script);
	system("chmod +rwxrwxrwx script");
	system("./script");
	}
}
