#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <telebot.h>
#include <sys/time.h> 
#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))
struct teclado
    {
        char keyboard[1][30];
    }tcl;

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

    telebot_handler_t handle;
    if (telebot_create(&handle, token) != TELEBOT_ERROR_NONE)
    {
        printf("Telebot create failed\n");
        return -1;
    }

    telebot_user_t me;
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
    telebot_error_e ret;
    telebot_message_t message;
    telebot_update_type_e update_types[] = {TELEBOT_UPDATE_TYPE_MESSAGE};
    //telebot_reply_keyboard_markup_t teclado;
    //char teclado[2][2];
    //telebot_keyboard_button_t keyboard;
    //char *keyboard[][4]={{'A'},{'B'},{'C'},{'D'}};
    //tcl={"A"};
    //tcl.n=2;
    //tcl.keyboard={"A"};
    //struct teclado *tcl;
    
    char buffer[20]={"hola"};
    strcpy(&tcl.keyboard[0][0],buffer);
    
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
                
                if (strstr(message.text, "/temp"))
                {
                    char aux[30];
                    FILE *ft = fopen("temperatura.txt", "r+");
                    char comando[]={"vcgencmd measure_temp > temperatura.txt"};
                    system(comando);
                    fgets(aux,30,ft);
                    ret = telebot_send_message(handle, message.chat->id, aux, "HTML", false, false, updates[index].message.message_id, "");
                    fclose(ft);
                     printf("Hola amigos\n");
                }
                if (strstr(message.text, "Menú"))
                {
                    char aux2[100];
                    FILE *fm = fopen("menu.txt", "r");
                    fread(aux2,sizeof(char),46,fm);
                    ret = telebot_send_message(handle, message.chat->id, aux2, "HTML", false, false, 0, "");
                    fread(aux2,sizeof(char),95,fm);
                    ret = telebot_send_message(handle, message.chat->id, aux2, "HTML", false, false, 0, "");
                    fclose(fm);
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
                    ret = telebot_send_message(handle, message.chat->id, str, "HTML", false, false, updates[index].message.message_id, "");
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
