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
	FILE *fp1;
	int CantMuestras = 5;																	//Cantidad de muestras a leer por cada canal (Deberá ser un parámetro que ingrese el usuario)
	uint8_t *Channels = malloc(NChannels * sizeof(uint8_t));					//Arreglo dinámico con la cantidad de canales a leer
	uint8_t Ch;
	for (Ch = 0; Ch < NChannels; Ch++)											//Rellenamos el arreglo con los nros que representan a cada canal, de 0 a 7 (NbChannels-1). ¿Se podría hacer con 
	{																			//una simple constante que se incremente cada vez que se quiera cambiar de canal?
		*(Channels + Ch) = Ch;
	}

	double media[8]={0,0,0,0,0,0,0,0};											//Arreglo donde guardaremos la media de cada canal
	double m2[8]={0,0,0,0,0,0,0,0};												//Distancia al cuadrado de cada muestra a la media	
	double delta[8]={0,0,0,0,0,0,0,0};											//Distancia de cada muestra a la media
	double varianza[8]={0,0,0,0,0,0,0,0};										//Varianza de cada canal
	double stdev[8]={0,0,0,0,0,0,0,0};											//Desviación estandar de cada canal
	int n=0;																	//Contador de iteraciones del bucle
	
	while (1 == 1)
	{

		printf("ADC_DAC_Init\r\n");
		int Id = 0;
		int Init = ADC_DAC_Init(&Id, ADS1256_GAIN_1, ADS1256_100SPS);			//Inicializamos el ADC. En Id obtenemos el numero de identificación del chip que debería ser igual a 3.
		if (Init != 0)															//Elegimos ganancia igual a 1 y frecuencia de muestreo igual a 100 m/s
		{
			RetCode = -1;
			break;
		}
		printf("init done !\r\n");
		fp=fopen("Lectura.xls","a+");
		
		//fp=fopen("Lectura.txt","a");																							//Adjuntamos un archivo de nombre "Lectura", para escritura. Se crea si no existe
		//fp = fopen("/home/Desktop/Archivos/libreria/RaspberryPi-ADC-DAC-master/Repo/Codigo_Proyecto_Final", "Lecturas");		//Abrimos un archivo de nombre "Lectura" en la ruta especificada
		int Loop;
		
		for (Loop = 0; Loop < CantMuestras; Loop++)										
		{
			int32_t *AdcValues = NULL;																		//Arreglo dinámico donde guardaremos los valores leidos de cada canal(se rellena con los argumentos pasados en ReadAdcValues
			ADS1256_ReadAdcValues(&Channels, NChannels, SINGLE_ENDED_INPUTS_8, &AdcValues);				//Pasamos el arreglo con los canales a leer, la cant. de canales, el modo (singular o diferencial) y el arreglo 
																											//donde guardar los valores leidos
			double *volt = ADS1256_AdcArrayToMicroVolts(AdcValues, NChannels, 1000.0 / 1000000.0);			//Pasamos a volts los valores leidos (esto lo podemos realizar en un excel una vez que saquemos los datos para poder
																											//reducir los ciclos de procesamiento del procesador
			
			fprintf(fp,"0 : %f V   1 : %f V   2 : %f V   3 : %f V   4 : %f V   5 : %f V   6 : %f V   7 : %f V \r\n", 		//Imprimimos los valores del arreglo en el archivo (Probar)
				   volt[0], volt[1], volt[2], volt[3], volt[4], volt[5], volt[6], volt[7]);  
			//printf("0 : %f V   1 : %f V   2 : %f V   3 : %f V   4 : %f V   5 : %f V   6 : %f V   7 : %f V \r\n",		//Mostrar los datos por puerto serie
			//	   volt[0], volt[1], volt[2], volt[3], volt[4], volt[5], volt[6], volt[7]);

			//DAC8552_Write(channel_A, Voltage_Convert(5.0, volt[0]));										//Funcion para escribir en una salida el valor leido por el canal 0
		n++;																								////Calculo de la media, el delta y la distancia al cuadrado de cada canal////
		for (Loop = 0; Loop < NChannels; Loop++)										
		{
			delta[Loop]=volt[Loop]-media[Loop];
			
		}
		
		for (Loop = 0; Loop < NChannels; Loop++)										
		{
			media[Loop]+=delta[Loop]/n;
		}
		
		for (Loop = 0; Loop < NChannels; Loop++)										
		{
			m2[Loop]+=(delta[Loop]*(volt[Loop]-media[Loop]));
			printf("m2= %lf\n",m2[0]);
		}
		
		for (Loop = 0; Loop < NChannels; Loop++)										
		{
		varianza[Loop]=m2[Loop]/((double)n-1);
		printf("var= %lf\n",varianza[0]);
		//printf("aca llega!\r\n");
		}	
		for (Loop = 0; Loop < NChannels; Loop++)										
		{
		stdev[Loop]= sqrt(varianza[Loop]);
		}
		
		bsp_DelayUS(500000);	
		free(AdcValues);																				//Así como inicializamos los arreglos dinamicos y reservamos memoria, tambien debemos liberar esa memoria reservada
		free(volt);																		//Espera activa de 0,5 segundos = 500000 uS
		}
		fclose(fp);																							//Cerramos el descriptor de archivo
		
		printf("ADC_DAC_Close\r\n");
		int CloseCode = ADC_DAC_Close();																	//Finalizamos la comunicación SPI y ponemos al conversor en standby
		if (CloseCode != 0)
		{
			RetCode = -2;
			break;
		}
		
		MainLoop++;

		//This loop proves that you can close and re-init pacefully the librairie. Prove it several times (e.g. 3) and then finish the code.
		if (MainLoop >= 3)
			break;
			//break;
		
	}
	
	fp1=fopen("desviacion estandar.xls","a+");
	fprintf(fp1,"Desviación estandar: \n Canal 0: %f\n Canal 1: %f\n Canal 2: %f\n Canal 3: %f\n Canal 4: %f\n Canal 5: %f\n Canal 6: %f\n Canal 7: %f\n",stdev[0],stdev[1],stdev[2],stdev[3],stdev[4],stdev[5],stdev[6],stdev[7]);
	fclose(fp1);	
	printf("Test ADDA finished with returned code %d\r\n", RetCode);
	
	return RetCode;
}
