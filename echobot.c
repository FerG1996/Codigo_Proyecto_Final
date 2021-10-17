#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <telebot.h>
#include <sys/time.h> 
#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

int flag1=3;
int flag2=3;
int flag3=3;
telebot_handler_t handle;
telebot_user_t me;
telebot_error_e ret;
telebot_message_t message;
telebot_update_type_e update_types[] = {TELEBOT_UPDATE_TYPE_MESSAGE};
int cantidad_archivos;
int lastfile;
char auxiliar_cantidad[50]={""};

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

void ultimo()
{
	FILE *ff;
	char auxiliar[150]={""};
    char cadena1[]={"find /home/pi/Desktop/Archivos/ -type f -name \"*00*\" -printf '%T+ %p\n' | sort -r | head -1 >resultado.txt"};

	system(cadena1);
	
	ff=fopen("resultado.txt","r+");
	if(ff==NULL)
	{
		printf("No se pudo abrir archivo\n");
		exit(1);
	}
	fgets(auxiliar,150,ff);
	fclose(ff);
    
	enviar_archivo(auxiliar);	
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



int main(int argc, char *argv[])
{
    //struct teclado tcl;
    printf("Welcome to Echobot\n");
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
    
    //telebot_reply_keyboard_markup_t teclado;
    //char teclado[2][2];
    //telebot_keyboard_button_t keyboard;
    //char *keyboard[][4]={{'A'},{'B'},{'C'},{'D'}};
    //tcl={"A"};
    //tcl.n=2;
    //tcl.keyboard={"A"};
    //struct teclado *tcl;
    
    //char buffer[20]={"hola"};
    //strcpy(&tcl.keyboard[0][0],buffer);
    
    //struct teclado *tecla;

    
    while (1)
    {
        telebot_update_t *updates;
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
				}
                
				
				if(flag2==1)
				{
					ret = telebot_send_message(handle, message.chat->id, "Se muestra listado" , "HTML", false, false, 0, "");
					flag2=0;
				}
				
				if (strstr(message.text, "/Mostrar_actuales"))
				{
					ret = telebot_send_message(handle, message.chat->id, "Se muestran los parametros actuales" , "HTML", false, false, 0, "");
                    flag1=3;
                }
				
				if (strstr(message.text, "/Modificar_parametros"))
				{
					ret = telebot_send_message(handle, message.chat->id, "Elija el parametro a modificar:\n /Canal_disparo\n /Frecuencia\n /Nivel\n /Post\n /Pre\n /Cantidad\n" , "HTML", false, false, 0, "");
                    flag1=3;
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
        
       
        
        sleep(1);
        printf("\nEl mensaje es: %s\n",message.text);
        int l=strlen(message.text);
        printf("\nl=%d\n",l);
        
    }

    telebot_destroy(handle);

    return 0;
}
