#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <telebot.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))
#define FIFO_PATH "/tmp/MI_FIFO"

int flag1=3;
int flag2=3;
int flag3=3;
int flag4=3;
telebot_handler_t handle;
telebot_user_t me;
telebot_error_e ret;
telebot_message_t message;
telebot_update_type_e update_types[] = {TELEBOT_UPDATE_TYPE_MESSAGE};
int cantidad_archivos;
int lastfile;
int firstfile;
char auxiliar_cantidad[50]={""};
int number_file2;
int lista;
telebot_update_t *updates;


/*/////////////////////////////////////////////////////////////////////////////////////////////////////*/
int atoifunction(char *character)
{
    return(atoi(character));
}

void obtener_cantidad()
{
	FILE *flast;
	int i_parcial3=0;
	char auxiliar[150]={""};
	
	
	flast=fopen("/home/pi/Desktop/Archivos/libreria /RaspberryPi-ADC-DAC-master/build/Parametros.txt","r+");
	if(flast==NULL)
	{
		printf("No se pudo abrir archivo\n");
		exit(2);
	}
	do
	{
		fgets(auxiliar,40,flast);
	} while(strstr(auxiliar,"Cantidad de archivos")==NULL);
	
    printf("auxiliar = %s\n",auxiliar);
    
	fclose(flast);
	
	while(auxiliar[23+i_parcial3]!='\0')
	{
		auxiliar_cantidad[i_parcial3]=auxiliar[23+i_parcial3];
		i_parcial3++;
	}
	cantidad_archivos=atoi(auxiliar_cantidad);
	printf("Cantidad de archivos = %s y en numero es %d\n",auxiliar_cantidad,cantidad_archivos);
}
/*/////////////////////////////////////////////////////////////////////////////////////////////////////*/
void enviar_archivo(char auxiliar[150])
{
	int l=strlen(auxiliar);
	char auxiliar2[50]={""};
	char auxiliar3[50]={""};
	char doc[]={"/home/pi/Desktop/Archivos/libreria /RaspberryPi-ADC-DAC-master/build"};
	int i_parcial=0;
	
	while(auxiliar[l-i_parcial]!='/')
	{
		auxiliar2[i_parcial]=auxiliar[l-i_parcial-2];
		i_parcial++;
	}
	l=strlen(auxiliar2);
	for(i_parcial=0;i_parcial<l-1;i_parcial++)
	{
		auxiliar3[i_parcial]=auxiliar2[l-i_parcial-2];
	}
	
	strcat(doc,auxiliar3);
    printf("Doc = %s\n",doc);
	ret=telebot_send_document(handle, message.chat->id, doc, true, NULL, "","", false, 0,"");
	
}
/*/////////////////////////////////////////////////////////////////////////////////////////////////////*/
void ultimo()
{
	//FILE *ff;
	char auxiliar[150]={""};
    //char cadena1[]={"find /home/pi/Desktop/Archivos/ -type f -name \"*00*\" -printf '%T+ %p\n' | sort -r | head -1 >resultado.txt"};
    char cadena[]={"find /home/pi/Desktop/Archivos/ -type f -wholename \"*00"};
    FILE *fl;
	FILE *name_file;
	//int i_parcial2=0;
	int number_file;
	//char auxiliar[40]={""};
	char auxiliar2[200]={""};
    //char c;
	
	fl=fopen("/home/pi/Desktop/Archivos/libreria /RaspberryPi-ADC-DAC-master/build/lastfile.txt","r+");
	if(fl==NULL)
	{
		printf("No se pudo abrir archivo\n");
		exit(3);
	}
	fgets(auxiliar,40,fl);
	fclose(fl);
	number_file=atoi(auxiliar);
	number_file--;
	
	if(number_file<0)
	{
		obtener_cantidad();
		number_file=cantidad_archivos;
	}
	
    name_file=fopen("resultado2.txt","r+");
    if(name_file==NULL)
    {
        printf("No se pudo abrir archivo\n");
        exit(4);
    }
    if(number_file==0)
    {
			system("find /home/pi/Desktop/Archivos/ -type f -name \"*000-*\" >resultado2.txt");
			fgets(auxiliar2,150,name_file);
    }
    else
    {
        sprintf(auxiliar_cantidad,"%d",number_file);
		strcat(cadena,auxiliar_cantidad);
		strcat(cadena,"-*\">resultado2.txt");
		printf("cadena = %s\n",cadena);
		system(cadena);
		fseek(name_file,0,SEEK_SET);
        fgets(auxiliar2,150,name_file);
    }
    
	enviar_archivo(auxiliar2);	
}

/*/////////////////////////////////////////////////////////////////////////////////////////////////////*/
void ultimos10()
{
	FILE *fl;
	FILE *name_file;
	int i_parcial2=0;
	int number_file;
	char auxiliar[40]={""};
	char auxiliar2[200]={""};
    char c;
	
	
	fl=fopen("/home/pi/Desktop/Archivos/libreria /RaspberryPi-ADC-DAC-master/build/lastfile.txt","r+");
	if(fl==NULL)
	{
		printf("No se pudo abrir archivo\n");
		exit(3);
	}
	fgets(auxiliar,40,fl);
	fclose(fl);
	number_file=atoi(auxiliar);
	number_file--;
	
	if(number_file<0)
	{
		obtener_cantidad();
		number_file=cantidad_archivos;
	}
	
	
	
	for(i_parcial2=0;i_parcial2<10;i_parcial2++)
	{
		char cadena[]={"find /home/pi/Desktop/Archivos/ -type f -wholename \"*00"};
        
        name_file=fopen("resultado2.txt","r+");
        if(name_file==NULL)
        {
            printf("No se pudo abrir archivo\n");
            exit(4);
        }
        if(number_file==0)
		{
			system("find /home/pi/Desktop/Archivos/ -type f -name \"*000-*\" >resultado2.txt");
			
			fgets(auxiliar2,150,name_file);
			enviar_archivo(auxiliar2);	
			
			obtener_cantidad();
            sprintf(auxiliar_cantidad,"%d",cantidad_archivos);
            strcat(cadena,auxiliar_cantidad);
			strcat(cadena,"-*\">resultado2.txt");
			printf("cadena = %s\n",cadena);
            system(cadena);
            
            fseek(name_file,0,SEEK_SET);
            c=fgetc(name_file);
            printf("c =%c",c);
            if(c=='/')
            {
                printf("Si existe el archivo\n");
				number_file=cantidad_archivos;
                i_parcial2++;
            }
			else
			{
				printf("Ya se enviaron todos los archivos\n");
				break;
			}
		}
		
		sprintf(auxiliar_cantidad,"%d",number_file);
		strcat(cadena,auxiliar_cantidad);
		strcat(cadena,"-*\">resultado2.txt");
		printf("cadena = %s\n",cadena);
		system(cadena);
		fseek(name_file,0,SEEK_SET);
        fgets(auxiliar2,150,name_file);
		printf("auxiliar2 =%s\n",auxiliar2);
        enviar_archivo(auxiliar2);	
        number_file--;
        fclose(name_file);
	}
	
}
/*/////////////////////////////////////////////////////////////////////////////////////////////////////*/
void listar_archivos()
{
	FILE *fu;
	FILE *name_file2;
	int i_parcial3=0;
	int i_parcial4=0;
	char auxiliar[40]={""};
    char auxiliar2[200]={""};
	
    char oldest[]={"find /home/pi/Desktop/Archivos/ -type f -wholename \"*00"};
	//char oldest[200]={""};
    int largo;
	char c;
	char f;
    printf("\n\nlista = %d\n\n",lista);
	if(lista==-1)
	{
		fu=fopen("/home/pi/Desktop/Archivos/libreria /RaspberryPi-ADC-DAC-master/build/lastfile.txt","r+");
	
		if(fu==NULL)
		{
			printf("No se pudo abrir archivo\n");
			exit(3);
		}
		fgets(auxiliar,40,fu);
		fclose(fu);
		number_file2=atoi(auxiliar);
		
        strcat(oldest,auxiliar);
        strcat(oldest,"-*\">resultado2.txt");
        printf("oldest = %s\n",oldest);
        system(oldest);
        name_file2=fopen("resultado2.txt","r+");
        f=fgetc(name_file2);
        fclose(name_file2);
        
        if(f=='/')
		{
			printf("Si existe el archivo\n");
			firstfile=number_file2;
		}
		else
		{
			printf("No existe\n");
			firstfile=0;
		}
		printf("firstfile es igual a %d\n",firstfile);
        number_file2--;
		
		obtener_cantidad();
		if(number_file2<0)
		{
			number_file2=cantidad_archivos;
		}
		lastfile=number_file2;
        printf("\nAca entra\n");
	}
	char auxiliar4[1000]={""};
	
	for(i_parcial3=0;i_parcial3<10;i_parcial3++)
	{
		char cadena[]={"find /home/pi/Desktop/Archivos/ -type f -wholename \"*00"};
		char aux_name[400]={""};
		char aux_name2[400]={""};
        char auxiliar3[50]={""};
		name_file2=fopen("resultado2.txt","r+");
		if(name_file2==NULL)
		{
			printf("No se pudo abrir archivo\n");
			exit(4);
		}
		printf("\nAca tambien\n");
        
        if(number_file2==0)
		{
			flag4=0;
			system("find /home/pi/Desktop/Archivos/ -type f -name \"*000-*\" >resultado2.txt");
			
			fgets(auxiliar2,150,name_file2);
			largo=strlen(auxiliar2);
            printf("auxiliar 2 en 0 =%s\n",auxiliar2);
			i_parcial4=0;
            while(auxiliar2[largo-i_parcial4]!='/')
			{
				auxiliar3[i_parcial4]=auxiliar2[largo-i_parcial4-2];
				i_parcial4++;
			}
             printf("auxiliar 3 en 0 =%s\n",auxiliar3);
			largo=strlen(auxiliar3);
			for(i_parcial4=0;i_parcial4<largo-1;i_parcial4++)
			{
				aux_name2[i_parcial4]=auxiliar3[largo-i_parcial4-2];
			}
           
            strcat(aux_name2,"\n\n");
			strcat(aux_name2,auxiliar4);
			strcpy(auxiliar4,aux_name2);
            
			obtener_cantidad();
            sprintf(auxiliar_cantidad,"%d",cantidad_archivos);
			strcat(cadena,auxiliar_cantidad);
			strcat(cadena,"-*\">resultado2.txt");
			printf("cadena = %s\n",cadena);
			system(cadena);
			
			fseek(name_file2,0,SEEK_SET);
			c=fgetc(name_file2);
			if(c=='/')
			{
				printf("Si existe el archivo\n");
				number_file2=cantidad_archivos;
			}
			else
			{
				printf("Ya se enviaron todos los archivos\n");
				//lista=-1;
                break;
			}
		}
		else
		{
			/*if(number_file2==lastfile&&lista!=-1)
			{
				lista=-1;
				break;
			}*/
			sprintf(auxiliar_cantidad,"%d",number_file2);
			strcat(cadena,auxiliar_cantidad);
			strcat(cadena,"-*\">resultado2.txt");
			//printf("cadena = %s\n",cadena);
			system(cadena);
			
			fseek(name_file2,0,SEEK_SET);
			fgets(aux_name,150,name_file2);
			//printf("aux_name =%s\n",aux_name);
			i_parcial4=0;
			largo=strlen(aux_name);
			
            while(aux_name[largo-i_parcial4]!='d')
            {
                auxiliar3[i_parcial4]=aux_name[largo-i_parcial4-1];
                i_parcial4++;
            }
        
			largo=strlen(auxiliar3);
			//printf("auxiliar3 =%s\n",auxiliar3);
			i_parcial4=0;
            while(auxiliar3[i_parcial4]!='/')
            {
                aux_name2[i_parcial4]=auxiliar3[largo-i_parcial4-2];
                i_parcial4++;
            }
            
			strcat(aux_name2,"\n\n");
			strcat(aux_name2,auxiliar4);
			strcpy(auxiliar4,aux_name2);
			number_file2--;
		}
		fclose(name_file2);
        
	}
	printf("auxiliar4 =%s\n",auxiliar4);
	ret = telebot_send_message(handle, message.chat->id, auxiliar4, "HTML", false, false, 0, "");
}

void mostrar_actuales()
{
	char parameters_read[1000]={""};
	FILE *fparameters;
	int nro_caracteres;
	
	fparameters=fopen("/home/pi/Desktop/Archivos/libreria /RaspberryPi-ADC-DAC-master/build/Parametros.txt","r+");
	if(fparameters==NULL)
	{
		printf("\nNo se pudo abrir el archivo Parametros.txt\n");
	}
	
	nro_caracteres=fread(parameters_read,sizeof(char),1000,fparameters);
	
	if(nro_caracteres==0)
	{
		printf("\nNo ley� ning�n caracter\n");
	}
	fclose(fparameters);
	
	ret = telebot_send_message(handle, message.chat->id, parameters_read, "HTML", false, false, 0, "");
	
}

void modificar_parametros()
{
    ret = telebot_send_message(handle, message.chat->id, "Elija el parametro a modificar:\n /Canal_de_disparo\n /Frecuencia_de_muestreo\n /Nivel_de_disparo\n /Muestras_post_trigger\n /Muestras_pre_trigger\n /Cantidad_archivos_almacenables\n" , "HTML", false, false, 0, "");
    
}

void modify_parameters(int flag_change)
{
	char auxiliar[300];
	FILE *fparameters;
	char line[1000]={""};
	char line1[200]={"1-Canal-"};
	char line2[200]={"2-Frecuencia-"};
	char line3[200]={"3-Nivel-"};
	char line4[200]={"4-Muestras posttrigger-"};
	char line5[200]={"5-Muestras pretrigger-"};	char line6[200]={"6-Cantidad de archivos-"};
	int index=0; 
    int count, offset = -1;
	int i_parcial;
	int length,length3;
	int unusable;
    char c;
    message = updates[index].message;
    strcpy(message.text,line);
    
	while(1)
	{
        
        ret=telebot_get_updates(handle, offset, 20, 0, update_types, 0, &updates, &count);
		
        message = updates[index].message;
        printf("\nmessage.text = %s\n",message.text);
        
		if(flag_change==1||flag_change==2)
		{
            c=message.text[1];
            unusable=atoifunction(&c);
			printf("\natoi tiro %d\n",unusable);
			if((unusable>=1&&unusable<=9)||c=='0')
			{break;}
		}
		else
		{
			c=message.text[0];
            unusable=atoifunction(&c);
			if((unusable>=1&&unusable<=9)||c=='0')
			{break;}
		}
            
	}
	
	fparameters=fopen("/home/pi/Desktop/Archivos/libreria /RaspberryPi-ADC-DAC-master/build/Parametros.txt","r+");
	if(fparameters==NULL)
	{
		printf("\nNo se pudo abrir el archivo Parametros.txt\n");
	}
	
	switch(flag_change)
	{
		case 1:
		{	strcpy(line,line1);
			length=strlen(message.text);
			for(i_parcial=0;i_parcial<length;i_parcial++)
			{
				message.text[i_parcial]=message.text[i_parcial+1];
			}	
		}	
		break;
		case 2:
		{	strcpy(line,line2);
			length=strlen(message.text);
			for(i_parcial=0;i_parcial<length;i_parcial++)
			{
				message.text[i_parcial]=message.text[i_parcial+1];
			}	
		}
		break;
		case 3:
		{strcpy(line,line3);}
		break;
		case 4:
		{strcpy(line,line4);}	
		break;				
		case 5:
		{strcpy(line,line5);}	
		break;
		case 6:
		{strcpy(line,line6);}	
		break;
	}
	
	strcat(line,message.text);
    strcat(line,"\n");
	
	for(i_parcial=0;i_parcial<flag_change;i_parcial++)
	{
		fgets(auxiliar,250,fparameters);
	}
	printf("\nauxiliar = %s\n",auxiliar);
	
//	length2=strlen(line);
	length3=strlen(auxiliar);
	
	/*if((length3-length2)>0)
	{
		for(i_parcial=0;i_parcial<(length3-length2);i_parcial++)
		{
			strcat(line," ");
		}
	}*/
	
    for(i_parcial=0;i_parcial<(14-flag_change);i_parcial++)
    {
        fgets(auxiliar,300,fparameters);
        length3=length3+strlen(auxiliar);
        strcat(line,auxiliar);
    }
    printf("\nline =%s\n",line);
    printf("\nlength3 =%d\n",length3);
    fseek(fparameters,-length3,SEEK_CUR);
    fputs(line,fparameters);
	
	fclose(fparameters);
	
	ret = telebot_send_message(handle, message.chat->id, "Aplicar los cambios:\n\n/Aplicar_ahora.\n\n/Tal_vez_luego." , "HTML", false, false, 0, "");
}

int main(int argc, char *argv[])
{
    //struct teclado tcl;
	FILE *file_main;
	char name_file[400]={""};
	char yourpid[20];
	int flag_parameters=0;
	int flag_change;
	int nread,fifo_t,err;
	int pid_m;
    
    printf("Welcome to Echobot\n");
    int pid=getpid();
    printf("Mi pid es %d\n",pid);

	
	// FIFO puede ser leida, escrita y ejecutada por:
	err = mkfifo(FIFO_PATH,0777);
	if(err == -1) 
	{printf("\nError al crear FIFO, la FIFO ya existe\n");}

	fifo_t = open(FIFO_PATH,O_RDWR, 0); //Bloqueante
	if(fifo_t == -1)
	{printf("\nError al abrir FIFO\n");}
	
	nread = read(fifo_t, yourpid, sizeof(yourpid));
	if(nread == -1) 
	{printf("\nError al leer de la FIFO\n");}
	
	pid_m=atoi(yourpid);
	printf("El pid del otro proceso es %d\n",pid_m);
	
	
    char exit[]={"exit"};
    printf("exit=%s\n",exit);
    FILE *fp = fopen("token.txt", "r");
    if (fp == NULL)
    {
        printf("Failed to open .token file\n");
        return -1;
    }

    char token[1024];
    if (fscanf(fp, "%s", token) == 0)
    {
        printf("Failed to read token\n");
        fclose(fp);
        return -1;
    }
    printf("Token: %s\n", token);
    fclose(fp);

    if (telebot_create(&handle, token) != TELEBOT_ERROR_NONE)
    {
        printf("Telebot create failed\n");
        return -1;
    }

    if (telebot_get_me(handle, &me) != TELEBOT_ERROR_NONE)
    {
        printf("Failed to get bot information\n");
        telebot_destroy(handle);
        return -1;
    }

    printf("ID: %d\n", me.id);
    printf("First Name: %s\n", me.first_name);
    printf("User Name: %s\n", me.username);

    telebot_put_me(&me);

    int index, count, offset = -1;
    
    while (1)
    {
        char cadena_main[]={"find /home/pi/Desktop/Archivos/ -type f -wholename \"*"};
        ret = telebot_get_updates(handle, offset, 20, 0, update_types, 0, &updates, &count);
        if (ret != TELEBOT_ERROR_NONE)
            continue;
        printf("Number of updates: %d\n", count);
        for (index = 0; index < count; index++)
        {
            message = updates[index].message;
            
            if (message.text)
            {
                printf("%s: %s \n", message.from->first_name, message.text);
                
				if (strstr(message.text, "Menu"))
				{
					printf("Hola amigos\n");
                    char aux2[200];
					FILE *fm = fopen("menu.txt", "r");
					fread(aux2,sizeof(char),149,fm);
					ret = telebot_send_message(handle, message.chat->id, aux2, "HTML", false, false, 0, "");
					fclose(fm);
					flag1=3;
					flag2=3;
					flag3=3;
                    
				}   
				
                if (strstr(message.text, "/Temp"))
                {
                    char aux[30];
                    FILE *ft = fopen("temperatura.txt", "r+");
                    char comando[]={"vcgencmd measure_temp > temperatura.txt"};
                    system(comando);
                    fgets(aux,30,ft);
                    ret = telebot_send_message(handle, message.chat->id, aux, "HTML", false, false, updates[index].message.message_id, "");
                    fclose(ft);
                     
                }
                
				if (strstr(message.text, "/Bajar_archivos"))
				{
					flag1=0;
				}
				
				if (strstr(message.text, "/Parametros"))
				{
					flag1=1;
				}
				
				
				if (strstr(message.text, "/Bajar_el_ultimo"))
				{
					ret = telebot_send_message(handle, message.chat->id, "Ultimo archivo" , "HTML", false, false, 0, "");
                    flag1=3;
                    ultimo();
                }
				
				if (strstr(message.text, "/Bajar_ultimos_10"))
				{
					ret = telebot_send_message(handle, message.chat->id, "Ultimo 10 archivos" , "HTML", false, false, 0, "");
                    flag1=3;
                    ultimos10();
                }
				
				if (strstr(message.text, "/Listar_archivos"))
				{
					ret = telebot_send_message(handle, message.chat->id, "Listado de archivos" , "HTML", false, false, 0, "");
					flag2=1;
                    flag1=3;
					lista=-1;
					ret = telebot_send_message(handle, message.chat->id, "Se listan los 10 archivos mas nuevos:\n", "HTML", false, false, 0, "");
					listar_archivos();
					ret = telebot_send_message(handle, message.chat->id, "Puede avanzar o retroceder con:\n/Anteriores\n/Posteriores\n", "HTML", false, false, 0, "");
					flag2=0;
				}
				
				if (strstr(message.text, "/Anteriores"))
				{
					lista--;
					listar_archivos();
				}
				
				if (strstr(message.text, "/Posteriores"))
				{
					printf("\n\nlista antes de modificar lista = %d\n\n",lista);
                    lista=lista+1;
                    printf("\n\nlista despues de modificar lista = %d\n\n",lista);
					number_file2=number_file2+19;
                    if(number_file2>lastfile&&lista==0)
					{
						printf("Ando por ac� y firstfile = %d\n",firstfile);
                        number_file2=firstfile+9;
						flag4=1;
                         printf("\n\nEntre en 1ra condicion\n\n");
					}
					
					if(number_file2>lastfile&&flag4==0)
					{
						//number_file2=lastfile-1;
                        printf("\nNumber file es %d\n",number_file2);
                        if(number_file2-19<lastfile)
                        {lista=-1;}
                        printf("\n\nEntre en 2da condicion\n\n");
					}
					
					if(number_file2>cantidad_archivos)
					{
						number_file2=number_file2-cantidad_archivos-1;
                        printf("\n\nEntre en 3ra condicion\n\n");
					}
                    printf("\n\nlista antes de listar archivos = %d\n\n",lista);
					listar_archivos();
                    
                  /*  if(number_file2==lastfile-11)
                    {
                        lista=-1;
                        //listar_archivos();
                    }*/
				}
				
				if (strstr(message.text, "/00"))
				{
                   strcat(cadena_main,message.text);
                   strcat(cadena_main,"-*\" >resultado2.txt");
                   printf("cadena main = %s\n",cadena_main);
                   system(cadena_main);
                   file_main=fopen("resultado2.txt","r+");
                    if(file_main==NULL)
                    {
                        printf("No se pudo abrir archivo\n");
                        return(6);
                    }
                    fgets(name_file,300,file_main);
                    printf("va bien\n");
                    fclose(file_main);
                    printf("name file = %s\n",name_file);
                    enviar_archivo(name_file);
                }   
                    
				if (strstr(message.text, "/Mostrar_actuales"))
				{
					ret = telebot_send_message(handle, message.chat->id, "Se muestran los parametros actuales" , "HTML", false, false, 0, "");
                    flag1=3;
					mostrar_actuales();
					ret = telebot_send_message(handle, message.chat->id, "\nDesea modificar alguno de los parametros:\n\n/Si\n\n/No\n" , "HTML", false, false, 0, "");
					flag_parameters=1;
                }
				
					if (strstr(message.text,"/Si"))
					{
                        if(flag_parameters==1)
                        {
                        //memset(message.text,'\0',1000);
                        //strcpy(message.text,"/Modificar_parametros");
						modificar_parametros();
                        flag_parameters=0;
                        }
					}
					
					if (strstr(message.text,"/No") && flag_parameters==1)
					{
						flag_parameters=0;
					}
				
				if (strstr(message.text, "/Modificar_parametros"))
				{
					  flag1=3;
                      modificar_parametros();
                }
                
					if (strstr(message.text, "/Canal_de_disparo"))
					{
						ret = telebot_send_message(handle, message.chat->id,"Elija el canal de disparo:\n/0\n\n/1\n\n/2\n\n/3\n\n/4\n\n/5\n\n/6\n\n/7\n", "HTML", false, false, 0, "");
						flag_change=1;
						modify_parameters(flag_change);
					}
				
					if (strstr(message.text, "/Frecuencia_de_muestreo"))
					{
						ret = telebot_send_message(handle, message.chat->id,"Ingrese la frecuencia de muestreo en muestras por segundo:\n/30000\n\n/15000\n\n/7500\n\n/3750\n\n/2000\n\n/1000\n\n/500\n\n/100\n\n/60\n\n/50\n\n/30\n\n/25\n\n/15\n\n/10\n\n/5\n\n/2\n\n", "HTML", false, false, 0, "");
						flag_change=2;
						modify_parameters(flag_change);
					}
					
					if (strstr(message.text, "/Nivel_de_disparo"))
					{
						ret = telebot_send_message(handle, message.chat->id,"Ingrese el nivel de disparo en el formato \"U.dc\". Por ejemplo, \"1.38\", en volts.\n", "HTML", false, false, 0, "");
						flag_change=3;
						modify_parameters(flag_change);
					}
					
					if (strstr(message.text, "/Muestras_post_trigger"))
					{
						ret = telebot_send_message(handle, message.chat->id,"Ingrese la cantidad de muestras post trigger. *Tenga en cuenta que la suma de la cantidad de muestras pre y post disparo no deben superar las 39000 muestras.*", "HTML", false, false, 0, "");
						flag_change=4;
						modify_parameters(flag_change);
					}
					
					if (strstr(message.text, "/Muestras_pre_trigger"))
					{
						ret = telebot_send_message(handle, message.chat->id,"Ingrese la cantidad de muestras pre trigger. *Tenga en cuenta que la suma de la cantidad de muestras pre y post disparo no deben superar las 39000 muestras.*", "HTML", false, false, 0, "");
						flag_change=5;
						modify_parameters(flag_change);
					}
					
					if (strstr(message.text, "/Cantidad_archivos_almacenables"))
					{
						ret = telebot_send_message(handle, message.chat->id,"Ingrese la cantidad de archivos almacenables. *Una vez alcanzada la cantidad especificada, los nuevos archivos sobrescribiran a los mas antiguos.* ", "HTML", false, false, 0, "");
						flag_change=6;
						modify_parameters(flag_change);
					}
                    
					printf("flag_change = %d\n",flag_change);
					
					if (strstr(message.text, "/Aplicar_ahora"))
					{
						if(flag_change>0&&flag_change<7)
						{
							printf("\nEnviar se�al\n");
							kill(pid_m,SIGUSR1);
							flag_change=0;
						}
					}
					if (strstr(message.text, "/Tal_vez_luego"))
					{
							flag_change=0;
					}
					
                switch(flag1)
				{
					case 0:
					{
						ret = telebot_send_message(handle, message.chat->id, "1:/Bajar_el_ultimo\n2:/Bajar_ultimos_10\n3:/Listar_archivos" , "HTML", false, false, 0, "");
					};
					break;
					case 1:
					{
						ret = telebot_send_message(handle, message.chat->id, "1:/Mostrar_actuales\n2:/Modificar_parametros" , "HTML", false, false, 0, "");
					};
                    break;
				}
				
				if (strstr(message.text, "/documento"))
                {
                   char doc[]={"/home/pi/Desktop/Archivos/libreria /RaspberryPi-ADC-DAC-master/build/005-21|09|21-20:32:47.txt"};
                   ret=telebot_send_document(handle, message.chat->id, doc, true, NULL, "","", false, updates[index].message.message_id,"");
                  
                }
                if (strstr(message.text, "/dice"))
                {
                    telebot_send_dice(handle, message.chat->id, false, 0, "");
                     char image[]={"/home/pi/Desktop/Archivos/telegrambot/telebot-master/test/test/download.jpeg"};
                    ret=telebot_send_photo(handle, message.chat->id,image,true,"","",false, updates[index].message.message_id,"");
                    if (ret != TELEBOT_ERROR_NONE)
                    {
                        printf("Failed to send photo: %d \n", ret);
                    }
                }
                else
                {
                    char str[4096];
                    if (strstr(message.text, "/start"))
                    {
                        snprintf(str, SIZE_OF_ARRAY(str), "Hello %s", message.from->first_name);
                    }
                    else
                    {
                        snprintf(str, SIZE_OF_ARRAY(str), "<i>%s</i>", message.text);
                    }
                    //ret = telebot_send_message(handle, message.chat->id, str, "HTML", false, false, updates[index].message.message_id, "");
                }
                if (ret != TELEBOT_ERROR_NONE)
                {
                    printf("Failed to send message: %d \n", ret);
                }
            }
            offset = updates[index].update_id + 1;
        }
        //telebot_put_updates(updates, count);
        if(message.text[0]=='E'&&message.text[1]=='x'&&message.text[2]=='i'&&message.text[3]=='t')
        {
            printf("\nMe voy\n");
            break;
        }
        else
        {printf("\nTodavia no me voy\n");}
        
       
        close(fifo_t);
        sleep(1);
        printf("\nEl mensaje es: %s\n",message.text);
        int l=strlen(message.text);
        printf("\nl=%d\n",l);
        
    }

    telebot_destroy(handle);

    return 0;
}
