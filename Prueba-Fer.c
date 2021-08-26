/**
 * @file mainTestAdda.c
 * @author Boris Bocquet <b.bocquet@akeoplus.com>
 * @date May, 2018
 * @brief Testing AD-DA-WS-RPI lib
 *
 * @details Usefull to perform Analog to Digital(ADC, using ADS1256 circuit) and Digital to Analog (DAC, using DAC8552).
 * @todo More code especially for DAC8552.
 */

// This Source Code Form is subject to the terms of the
// GNU Lesser General Public License V 3.0.
// If a copy of the GNU GPL was not distributed
// with this file, You can obtain one at https://www.gnu.org/licenses/lgpl-3.0.en.html

#include "AD-DA-WS-RPI/AD-DA-WS-RPI.h"
#include <sys/time.h>  				// elapsed time to microseconds; measure sample rate
#include <time.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define CS_ADC_1() printf("CS_ADC_1()\n")
#define CS_ADC_0() printf("CS_ADC_0()\n")

/**
 * @brief Simple code to get the voltage on each 8 inputs (single ended) using the ADC1256 and write the input 1 value to output 1 using DAC8552.
 * You can use the jumper of the waveshare board to connect to 5V (power supply and vref), and connect ADO to ADJ (the potentiometer) and DAC to LEDA.
 * 
 * @return int 
 */
int canal;
int frequency;
int N_post;
int N_pre;
float nivel;
FILE *fp;																		//Descriptor de archivo que usaremos para el archivo que contiene los parámetros
struct timeval start,current;													//Estructuras donde guardaremos el tiempo
uint8_t buf[4];
char auxiliar[30]={""};
int DRATE_E;

int obtener()
{
	return(atoi(auxiliar));															//Función que convierte una cadena de caracteres en un número entero
}
void frecuencia()																//Función que asigna a ADS1256_DRATE_E el valor correspondiente segun lo leído en el archivo
{
	switch(frequency)
	{
	case 30000:
	{DRATE_E=ADS1256_30000SPS;}
	break;
	case 15000:
	{DRATE_E=ADS1256_15000SPS;}
	break;
	case 7500:
	{DRATE_E=ADS1256_7500SPS;}
	break;
	case 3750:
	{DRATE_E=ADS1256_3750SPS;}
	break;
	case 2000:
	{DRATE_E=ADS1256_2000SPS;}
	break;
	case 1000:
	{DRATE_E=ADS1256_1000SPS;}
	break;
	case 500:
	{DRATE_E=ADS1256_500SPS;}
	break;
	case 100:
	{DRATE_E=ADS1256_100SPS;}
	break;
	case 60:
	{DRATE_E=ADS1256_60SPS;}
	break;
	case 50:
	{DRATE_E=ADS1256_50SPS;}
	break;
	case 30:
	{DRATE_E=ADS1256_30SPS;}
	break;
	case 25:
	{DRATE_E=ADS1256_25SPS;}
	break;
	case 15:
	{DRATE_E=ADS1256_15SPS;}
	break;
	case 10:
	{DRATE_E=ADS1256_10SPS;}
	break;
	case 5:
	{DRATE_E=ADS1256_5SPS;}
	break;
	case 2:
	{DRATE_E=ADS1256_2d5SPS;}
	break;
	}
}
void parametros()																//Función que obtiene los parámetros guardados en el archivo de configuración
{
	char parameter;
	int largo;
	int i_parcial2=0;
	while(!feof(fp))															//Mientras no se llegue hasta el final del archivo se ejecuta el loop
	{
		parameter=fgetc(fp);													//Obtenemos el primer caracter de cada fila
		fgets(auxiliar,30,fp);													//Obtenemos la oración restante
		largo=strlen(auxiliar);													//Largo de la cadena almacenada en aux
		switch(parameter)																//Según el valor del primer caracter de cada fila es el parámetro a obtener
		{
		case '1':																//Parámetro canal
		{auxiliar[0]=auxiliar[largo-3];
		canal=obtener();}
		break;
		case '2': 																//Parámetro frecuencia
		{auxiliar[5]='\0';
			while(auxiliar[12+i_parcial2]!='\0')
			{
			auxiliar[i_parcial2]=auxiliar[12+i_parcial2];
			i_parcial2++;
			}
		frequency=obtener();
		frecuencia();
		}
		break;
		case '3':																//Parámetro nivel
		{auxiliar[3]='\0';
		auxiliar[0]=auxiliar[largo-6];
		auxiliar[1]=auxiliar[largo-4];
		auxiliar[2]=auxiliar[largo-3];
		nivel=obtener()/100.0;}
		break;
		case '4':																//Parámetro cantidad de muestras post trigger
		{auxiliar[2]='\0';
		auxiliar[0]=auxiliar[largo-4];
		auxiliar[1]=auxiliar[largo-3];
		N_post=obtener();}
		break;
		case '5': 																//Parámetro cantidad de muestras pre trigger
		{auxiliar[0]=auxiliar[largo-4];
		auxiliar[1]=auxiliar[largo-3];
		N_pre=obtener();}
		break;
		}
	}
}
int lectura(int NbChannels, int AdcValues[][8], ADS1256_SCAN_MODE mode, int loop)	//Función de lectura implementada con matrices y no arreglos dinámicos
{
	
	int i;
	for (i = 0; i < NbChannels; i++)
	{
		ADS1256_WaitDRDY_LOW();														//Espera a que el pin DRDY se ponga en bajo lo que indica que la conversión anterior terminó y puede iniciar la nueva conversión 
		
		uint8_t CurChannel = i;
		
		if (mode == SINGLE_ENDED_INPUTS_8)											//Modo de 8 entradas individuales
			ADS1256_SetChannel(CurChannel);
		else
			ADS1256_SetDiffChannel(CurChannel);
		
		bsp_DelayUS(MASTER_CLOCK_PERIOD_USEC_TIMES_24);								//Tiempos de espera necesarios para que se establezcan correctamente los parámetros
		
		ADS1256_WriteCmd(CMD_SYNC);													//Comando de sincronismo
		bsp_DelayUS(MASTER_CLOCK_PERIOD_USEC_TIMES_24);
		
		ADS1256_WriteCmd(CMD_WAKEUP);												//Junto con CMD_SYNC son comandos necesarios antes realizar la medición
		bsp_DelayUS(MASTER_CLOCK_PERIOD_USEC_TIMES_24);
		
		AdcValues[loop][i] = ADS1256_ReadData();									//Lee y realiza la conversión del canal antes especificado
	}
	
	return 0;
}

int main(void)
{
	
////////////////////DEFINICIÓN DE VARIABLES///////////////////////////////////////////////////////////
	
	int NChannels = 8;															//Cantidad de canales a leer
	int MainLoop = 0;															//Variables de control (sacar despues)			
	int RetCode = 0;
	FILE *file;																	
	int Id = 0;																	//Variable donde se guarda el identificador de la placa
	//int CantMuestras = 100;														//Cantidad de muestras a leer por cada canal (Deberá ser un parámetro que ingrese el usuario)
	int i_general=0;															
	int j_general; 
	int i_parcial=0;
	int flag_triggered=0;
	int i_referencia;
//	int Loop;
	int level_triggered=0;														//Variable donde guardaremos la combinación equivalente al nivel seleccionado
	int N_total=0;																//N será igual a la cantidad total de muestras a guardar en el archivo, por lo tanto, N=post+pre
	
////////////////////////INICIO DE PRUEBA/////////////////////////////////////////////////////////////////
	
	printf("Iniciando prueba\r\n");
	
	int initSpi = spi_init();													//Inicializamos la comunicación SPI
	if (initSpi != 1)
	{
		printf("SPI init failed with code %d\r\n", initSpi);
		return -55;
	}
	printf("SPI initialized\r\n");
	printf("ADC_DAC_Init\r\n");
	fp=fopen("Parametros.txt","r");											//Abrimos el archivo de configuración
	if(fp==NULL)
	{
		printf("No se pudo abrir archivo\n");
		exit(1);
	}
	parametros();															//Obtenemos los parámetros
	fclose(fp);																//Cerramos el archivo de configuración
	
	printf("canal=%d\n",canal);
	printf("frecuencia=%d\n",DRATE_E);
	printf("nivel=%f\n",nivel);
	printf("post=%d\n",N_post);
	printf("pre=%d\n",N_pre);
	
	level_triggered=nivel*8388608/5;										//Conversion de volts a numero combinacional 
	N_total=N_pre+N_post;
	
	/////////////CONFIGURACIÓN DEL ADC///////////////
	
	int Init = ADC_DAC_Init(&Id, ADS1256_GAIN_1, DRATE_E);					//Inicializamos el ADC. En Id obtenemos el numero de identificación del chip que debería ser igual a 3.
	if (Init != 0)															//Elegimos ganancia igual a 1 y la frecuencia de muestreo en muestras/segundo
	{
		RetCode = -1;
		return RetCode;
	}
	//Habilitación del buffer de entrada
	buf[0] = (0 << 3) | (1 << 2) | (1 << 1);
	buf[1] = 0x08;
	buf[2] = (0 << 5) | (0 << 3) | ((uint8_t) ADS1256_GAIN_1 << 0);
	CS_ADC_0();	
	ADS1256_Send8Bit(CMD_WREG | 0); 										/* Write command register, send the register address */
	ADS1256_Send8Bit(0x03);													/* Register number 4,Initialize the number  -1*/
	ADS1256_Send8Bit(buf[0]); 												/* Set the status register */
	ADS1256_Send8Bit(buf[1]); 												/* Set the input channel parameters */
	ADS1256_Send8Bit(buf[2]); 												/* Set the ADCON control register,gain */
	ADS1256_Send8Bit(buf[3]); 												/* Set the output rate */
	
	CS_ADC_1(); 															/* SPI  cs = 1 */
	
	printf("init done !\r\n");
	file=fopen("Disparo.txt","a+");									//Abrimos el archivo donde guardaremos las lecturas
	gettimeofday(&start, NULL);  											//Función para obtener la fecha y la hora del archivo 
	fprintf(file,"%s\n",asctime(localtime(&start.tv_sec)));					//Imprimimos la fecha y la hora actual
	fprintf(file,"Parámetros:\nCanal de disparo:	%d\nFrecuencia de muestreo:	%d\nNivel de disparo=	%f\nCantidad de muestras post trigger =	%d\nCantidad de muestras pre trigger =	%d\n",canal,frequency,nivel,N_post,N_pre);	
	fprintf(file,"Volts/cuenta =	5000000/8388608\nCanal 0 (uV)	Canal 1 (uV)	Canal 2 (uV)	Canal 3 (uV)	Canal 4 (uV)	Canal 5 (uV)	Canal 6 (uV)	Canal 7 (uV)	Tiempo: canal 0 (S) +	uS\r\n"); 		//Encabezado de columnas en el excel
	
////////////////////////BUCLE PRINCIPAL//////////////////////////////////////////////////////////////////
	while (1 == 1)
	{
		int64_t tiempo[1000][2]={};											//Matriz donde guardaremos el tiempo en segundos y microsegundos
		int32_t AdcValues[1000][8]={};											//Arreglo donde guardaremos los valores leidos de cada canal(se rellena con los argumentos pasados en ReadAdcValues)
///////////////////////BUCLE SECUNDARIO///////////////////////////////////////////////////////////////////
		
		while(1)
		{	
			gettimeofday(&current, NULL);      									// Guardamos el tiempo en la estructura start
			tiempo[i_general][0]=current.tv_sec;
			tiempo[i_general][1]=current.tv_usec;
			
			lectura(NChannels,AdcValues,SINGLE_ENDED_INPUTS_8,i_general);			//Pasamos el arreglo con los canales a leer, la cant. de canales, el modo (singular o diferencial) y el arreglo donde guardar los valores leidos
																			
			if(AdcValues[i_general][canal]>=level_triggered)							//Si la medición es mayor o igual al nivel especificado, levantamos la bandera flag_triggered
			{
				flag_triggered=1;
				printf("\nAca llega\n");
			}
			if(flag_triggered==1)
			{
				i_parcial++;
			}
			if(i_parcial==N_post)
			{
				break;
			}
			if(i_general==1000)
			{
				i_general=0;
			}
			else
			{
				i_general++;
			}
		}
		
		i_referencia=i_general-N_total;
		printf("\nAca tambien\n");
		if(i_referencia<0)
		{
			for(j_general=999+i_referencia;j_general<1000;j_general++)
			{
				fprintf(file,"%d	%d	%d	%d	%d	%d	%d	%d	%lld	%lld\r\n",AdcValues[j_general][0], AdcValues[j_general][1], AdcValues[j_general][2], AdcValues[j_general][3], AdcValues[j_general][4], AdcValues[j_general][5],
					AdcValues[j_general][6], AdcValues[j_general][7], tiempo[j_general][0], tiempo[j_general][1]); 		// tiempo/(double)CLOCKS_PER_SEC
			}
			for(j_general=0;j_general<i_general+1;j_general++)
			{
				fprintf(file,"%d	%d	%d	%d	%d	%d	%d	%d	%lld	%lld\r\n",AdcValues[j_general][0], AdcValues[j_general][1], AdcValues[j_general][2], AdcValues[j_general][3], AdcValues[j_general][4], AdcValues[j_general][5],
					AdcValues[j_general][6], AdcValues[j_general][7], tiempo[j_general][0], tiempo[j_general][1]); 		// tiempo/(double)CLOCKS_PER_SEC
			}
		}
		else
		{
			for(j_general=i_referencia;j_general<=i_referencia+N_total;j_general++)
			{
				fprintf(file,"%d	%d	%d	%d	%d	%d	%d	%d	%lld	%lld\r\n",AdcValues[j_general][0], AdcValues[j_general][1], AdcValues[j_general][2], AdcValues[j_general][3], AdcValues[j_general][4], AdcValues[j_general][5],
					AdcValues[j_general][6], AdcValues[j_general][7], tiempo[j_general][0], tiempo[j_general][1]); 		// tiempo/(double)CLOCKS_PER_SEC
			}
		}
		
		flag_triggered=0;
		i_parcial=0;
		i_general=0;
		
		fprintf(file,"\n\n");
		fclose(file);																							//Cerramos el descriptor de archivo
		
		printf("ADC_DAC_Close\r\n");
		int CloseCode = ADC_DAC_Close();																	//Finalizamos la comunicación SPI y ponemos al conversor en standby
		if (CloseCode != 0)
		{
			RetCode = -2;
			break;
		}
		
		MainLoop++;

		//This loop proves that you can close and re-init pacefully the librairie. Prove it several times (e.g. 3) and then finish the code.
		if (MainLoop == 1)
			break;
		
	}
	
	printf("Test ADDA finished with returned code %d\r\n", RetCode);
	
	return RetCode;
}
