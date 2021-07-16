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
#include <unistd.h>
//#include <math.h>
/**
 * @brief Simple code to get the voltage on each 8 inputs (single ended) using the ADC1256 and write the input 1 value to output 1 using DAC8552.
 * You can use the jumper of the waveshare board to connect to 5V (power supply and vref), and connect ADO to ADJ (the potentiometer) and DAC to LEDA.
 * 
 * @return int 
 */
struct timeval start,current;	//Estructuras donde guardaremos el tiempo

int lectura(int NbChannels, int AdcValues[][8], ADS1256_SCAN_MODE mode, int loop)
{
	
	int i;
	for (i = 0; i < NbChannels; i++)
	{
		ADS1256_WaitDRDY_LOW();
		
		uint8_t CurChannel = i;
		
		if (mode == SINGLE_ENDED_INPUTS_8)
			ADS1256_SetChannel(CurChannel);
		else
			ADS1256_SetDiffChannel(CurChannel);
		
		bsp_DelayUS(MASTER_CLOCK_PERIOD_USEC_TIMES_24);
		
		ADS1256_WriteCmd(CMD_SYNC);
		bsp_DelayUS(MASTER_CLOCK_PERIOD_USEC_TIMES_24);
		
		ADS1256_WriteCmd(CMD_WAKEUP);
		bsp_DelayUS(MASTER_CLOCK_PERIOD_USEC_TIMES_24);
		
		AdcValues[loop][i] = ADS1256_ReadData();
	}
	
	return 0;
}

int main(void)
{
	printf("Iniciando prueba\r\n");

	int initSpi = spi_init();													//Inicializamos la comunicación SPI
        if (initSpi != 1)
	{
		printf("SPI init failed with code %d\r\n", initSpi);
		return -55;
	}
	printf("SPI initialized\r\n");

	//Prepare an array with the 8 single ended inputs to read [0,1,2,3,4,5,6,7]
	int NChannels = 8;															//Cantidad de canales a leer
	int MainLoop = 0;															//Variables de control (sacar despues)			
	int RetCode = 0;
	FILE *fp;																	//Descriptor de archivo que usaremos para guardar los datos en un archivo
//	FILE *fp1;
	
	int64_t tiempo[40000][2];													//Matriz donde guardaremos el tiempo en segundos y microsegundos
	int CantMuestras = 1000;														//Cantidad de muestras a leer por cada canal (Deberá ser un parámetro que ingrese el usuario)

	while (1 == 1)
	{

		printf("ADC_DAC_Init\r\n");
		int Id = 0;
		int Init = ADC_DAC_Init(&Id, ADS1256_GAIN_1, ADS1256_15000SPS);			//Inicializamos el ADC. En Id obtenemos el numero de identificación del chip que debería ser igual a 3.
		if (Init != 0)															//Elegimos ganancia igual a 1 y frecuencia de muestreo igual a 100 m/s
		{
			RetCode = -1;
			break;
		}
		printf("init done !\r\n");
		printf("clocks por seg = %ld\r\n",CLOCKS_PER_SEC);
		fp=fopen("Lectura.txt","a+");
		gettimeofday(&start, NULL);  
		fprintf(fp,"%s\n",asctime(localtime(&start.tv_sec)));					//Imprimimos la fecha y la hora actual
		/*start.tv_sec = 0;														//Ponemos los segundos de la estructura en 0
		start.tv_usec = 0;														//Ponemos los microsegundos en 0
		settimeofday(&start, NULL);		*/										//Seteamos la estructura con los valores colocados
		fprintf(fp,"Volts/cuenta =	5000000/8388608\nCanal 0 (uV)	Canal 1 (uV)	Canal 2 (uV)	Canal 3 (uV)	Canal 4 (uV)	Canal 5 (uV)	Canal 6 (uV)	Canal 7 (uV)	Tiempo: canal 0 (S) +	uS\r\n"); 		//Encabezado de columnas en el excel
		//fp=fopen("Lectura.txt","a");																							//Adjuntamos un archivo de nombre "Lectura", para escritura. Se crea si no existe
		//fp = fopen("/home/Desktop/Archivos/libreria/RaspberryPi-ADC-DAC-master/Repo/Codigo_Proyecto_Final", "Lecturas");		//Abrimos un archivo de nombre "Lectura" en la ruta especificada
		int Loop;
		//int cont;
		int32_t AdcValues[40000][8];															//Arreglo donde guardaremos los valores leidos de cada canal(se rellena con los argumentos pasados en ReadAdcValues
		//int32_t *AdcValues = NULL;
		gettimeofday(&start, NULL);  
		fprintf(fp,"\nTiempo inicial = %ld s	%ld us\n",start.tv_sec,start.tv_usec);
		for (Loop = 0; Loop < CantMuestras; Loop++)										
		{
			
			gettimeofday(&current, NULL);      																		// Guardamos el tiempo en la estructura start
			tiempo[Loop][0]=current.tv_sec;
			tiempo[Loop][1]=current.tv_usec;
			
			lectura(NChannels,AdcValues,SINGLE_ENDED_INPUTS_8,Loop);
			//ADS1256_ReadAdcValues(&Channels, NChannels, SINGLE_ENDED_INPUTS_8, &AdcValues);			//Pasamos el arreglo con los canales a leer, la cant. de canales, el modo (singular o diferencial) y el arreglo 
				//double *volt = ADS1256_AdcArrayToMicroVolts(AdcValues, NChannels, 1000000.0 / 1000000.0);																										//donde guardar los valores leidos
																	
		}
		
		for (Loop = 0; Loop < CantMuestras; Loop++)										
		{
				//double *volt = ADS1256_AdcArrayToMicroVolts(AdcValues, NChannels, 1000000.0 / 1000000.0);					//Pasamos a volts los valores leidos (esto lo podemos realizar en un excel una vez que saquemos los datos para poder																										//reducir los ciclos de procesamiento del procesador
				fprintf(fp,"%d	%d	%d	%d	%d	%d	%d	%d	%lld	%lld\r\n",AdcValues[Loop][0], AdcValues[Loop][1], AdcValues[Loop][2], AdcValues[Loop][3], AdcValues[Loop][4], AdcValues[Loop][5], 
					AdcValues[Loop][6], AdcValues[Loop][7], tiempo[Loop][0], tiempo[Loop][1]); 		// tiempo/(double)CLOCKS_PER_SEC
				
		}
		fclose(fp);																							//Cerramos el descriptor de archivo
		printf("aca llega!\r\n");
		printf("ADC_DAC_Close\r\n");
		int CloseCode = ADC_DAC_Close();																	//Finalizamos la comunicación SPI y ponemos al conversor en standby
		if (CloseCode != 0)
		{
			RetCode = -2;
			break;
		}
		
		MainLoop=1;

		//This loop proves that you can close and re-init pacefully the librairie. Prove it several times (e.g. 3) and then finish the code.
		if (MainLoop == 1)
			break;
			//break;
		
	}
	
	//fp1=fopen("desviacion estandar.xls","a+");
	//fprintf(fp1,"Desviación estandar: \n Canal 0: %f\n Canal 1: %f\n Canal 2: %f\n Canal 3: %f\n Canal 4: %f\n Canal 5: %f\n Canal 6: %f\n Canal 7: %f\n",stdev[0],stdev[1],stdev[2],stdev[3],stdev[4],stdev[5],stdev[6],stdev[7]);
	//fclose(fp1);	
	printf("Test ADDA finished with returned code %d\r\n", RetCode);
	
	return RetCode;
}
