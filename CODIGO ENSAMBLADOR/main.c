/*
===============================================================================
 Name        : TP_FINAL_SDC.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

//---------------------------------------------------------------------------------------------------------------------
//--------------------------------------IMPORTACION DE LIBRERIAS O BIBLIOTECAS-----------------------------------------
#ifdef __USE_CMSIS		 		//IMPORTAMOS LIBRERIAS/BIBLIOTECAS
#include "LPC17xx.h"			//
#endif							//
                                //
#include <cr_section_macros.h>  //
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//-----------------------------------------DECLARACION DE FUNCIONES O PROTOTIPOS---------------------------------------
void ConfigDePin();				//DECLARAMOS FUNCION, LO QUE HACE CONFIGURAR LOS PINES DEL MICRO
void ConfigTimer();			    //DECLARAMOS FUNCION, LO QUE HACE CONFIGURAR INTERRUPCION DEL TIMER
void ConfigGpio();			    //DECLARAMOS FUNCION, LO QUE HACE CONFIGURAR INTERRUPCION DE GPIO
void ConfigADC();			    //DECLARAMOS FUNCION, LO QUE HACE CONFIGURAR INTERRUPCION DE ADC
void ConfigUart();				//DECLARAMOS FUNCION, LO QUE HACE CONFIGURAR INTERRUPCION POR UART
void DecodificarConversion(int conversion);	//DECLARAMOS FUNCION, LO QUE HACE ES DECODIFICAR CONVERSION BINARIA
void Enviar(uint8_t mensaje);				//DECLARACION FUNCION, LO QUE HACE ES ENVIAR UN CARACTER
void PrenderApagarSistema();
void ModoAutomaticoManual();
void AumentarBrillo();
void DisminuirBrillo();
void EstadoLampara();
void ContarTiempo();
void ConfTiempo();
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//------------------------------------------DEFINICION DE REGISTROS----------------------------------------------------
unsigned int volatile * const PINSEL0 = (unsigned int *)0x4002C000;  	//
unsigned int volatile * const PINSEL1 = (unsigned int *)0x4002C004;  	//
unsigned int volatile * const PINMODE0 = (unsigned int *)0x4002C040;	//
unsigned int volatile * const PINMODE1 = (unsigned int *)0x4002C044;	//
unsigned int volatile * const PINMODE_OD0 = (unsigned int *)0x4002C068;	//
unsigned int volatile * const PINMODE_OD1 = (unsigned int *)0x4002C06C;	//
unsigned int volatile * const FIODIR = (unsigned int *)0x2009C000;		//
unsigned int volatile * const FIOPIN = (unsigned int *)0x2009C014;		//
unsigned int volatile * const FIOSET = (unsigned int *)0x2009C018;		//
unsigned int volatile * const FIOCLR = (unsigned int *)0x2009C01C;		//
unsigned int volatile * const CTCR = (unsigned int *)0x40004070;		//
unsigned int volatile * const TCR = (unsigned int *)0x40004004;			//
unsigned int volatile * const MR0 = (unsigned int *)0x40004018;			//
unsigned int volatile * const MR1 = (unsigned int *)0x4000401C;			//
unsigned int volatile * const MR2 = (unsigned int *)0x40004020;			//
unsigned int volatile * const MCR = (unsigned int *)0x40004014;			//
unsigned int volatile * const PR = (unsigned int *)0x4000400C;			//
unsigned int volatile * const PC = (unsigned int *)0x40004010;			//
unsigned int volatile * const TC = (unsigned int *)0x40004008;			//
unsigned int volatile * const IR = (unsigned int *)0x40004000;			//
unsigned int volatile * const ISER0 = (unsigned int *)0xE000E100;		//
unsigned int volatile * const IO0INTENR = (unsigned int *)0x40028090;	//
unsigned int volatile * const IO0INTCLR = (unsigned int *)0x4002808C;	//
unsigned int volatile * const IO0INTSTATR = (unsigned int *)0x40028084;	//
unsigned int volatile * const EXTINT = (unsigned int *)0x400FC140;		//
unsigned int volatile * const PCONP = (unsigned int *)0x400FC0C4;		//
unsigned int volatile * const AD0CR = (unsigned int *)0x40034000;		//
unsigned int volatile * const AD0INTEN = (unsigned int *)0x4003400C;	//
unsigned int volatile * const AD0GDR = (unsigned int *)0x40034004;		//
unsigned int volatile * const ADDR0 = (unsigned int *)0x40034010;		//
/*unsigned int volatile * const LCR = (unsigned int *)0x4000C00C;			//
unsigned int volatile * const DLL = (unsigned int *)0x4000C000;			//
unsigned int volatile * const DLM = (unsigned int *)0x4000C004;			//ES IGUAL A IER
unsigned int volatile * const IER = (unsigned int *)0x4000C004;			//ES IGUAL A DLM
unsigned int volatile * const RBR = (unsigned int *)0x4000C000;			//
unsigned int volatile * const LSR = (unsigned int *)0x4000C014;			//
unsigned int volatile * const THR = (unsigned int *)0x4000C000;			//*/
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------DECLARACION DE VARIABLES GLOBALES-----------------------------------------
int flagModos = 0;				//VARIABLE QUE INDICA SI ESTA EN 'MODO MANUAL=1' O EN 'MODO AUTOMATICO=0'
int flagSistema = 0;			//VARIABLE QUE INDICA SI ESTA EN EL SISTEMA 'ON=1' O  EN 'OFF=0'
int brilloLed = 0; 				//VARIABLE QUE INDICA EL BRILLO DE LA LAMPARA
int Brillo[21] = {0,1250,2500,3750,5000,6250,7500,8750,10000,11250,12500,13750,15000,16250,17500,18750,20000,21250,22500,23750,23750};	//ARREGLO QUE CONTIENE LOS NIVELES DE BRILLO DEL LED
int flagEstado5 = 0;
int flagEstado6 = 0;
int HoraON = 0;
int HoraOFF = 0;
int flagTiempo = 0;
int Hora = 0;
int Minuto = 0;
int milisegundos = 0;
int segundos = 0;
int minutos = 0;
int horas = 0;
int flagTiempoCumplido = 0;
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//--------------------------------------FUNCION PRINCIPAL (RUTINA PRINCIPAL)-------------------------------------------
int main(void)					//FUNCION PRINCIPAL, ES EL HILO PRINCIPAL DONDE ESPEZARAN A EJECUTARSE LAS INSTRUCCIONES
{
	ConfigDePin();				//LLAMAMOS A LA FUNCION PARA QUE CONFIGURE LOS PINES DEL PUERTO0
	ConfigTimer();				//LLAMAMOS A LA FUNCION PARA QUE CONFIGURE EL TIMER0
	ConfigGpio();				//LLAMAMOS A LA FUNCION PARA QUE CONFIGURE EL GPIO0
	ConfigADC();				//LLAMAMOS A LA FUNCION PARA QUE CONFIGURE EL ADC0
	ConfigUart();				//LLAMAMOS A LA FUNCION, PARA QUE CONFIGURE INTERRUPCION POR UART0

	*FIOSET = (1<<0); 			//INICIALMENTE SE APAGA LED 2 (SISTEMA PRENDIDO) (LOGICA NEGATIVA)
	*FIOCLR = (1<<1); 			//INICIALMENTE SE PRENDE LED 2 (SISTEMA APAGADO) (LOGICA NEGATIVA)
	*FIOSET = (1<<16); 			//INICIALMENTE SE APAGA LED 1 (LAMPARA) (LOGICA NEGATIVA)
	*FIOSET = (1<<17); 			//INICIALMENTE SE APAGA LED 3 (PRENDIDO) (LOGICA NEGATIVA)
	*FIOSET = (1<<18); 			//INICIALMENTE SE APAGA LED 3 (APAGADO) (LOGICA NEGATIVA)

    while(1)
    {
    	*AD0CR |= (1<<24);	//EMPIEZO CONVERSION (BIT 24,25,26)
    }
    return 0 ;					//PREDETERMINADAMENTE RETORNAMOS 0 YA QUE DICHA FUNCION PRINCIPAL DEVUELVE UN ENTERO
}
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------IMPLEMENTACION DE FUNCIONES-------------------------------------------
void ConfigDePin()
{
	//LEDS (LOGICA NEGATIVA)
	*PINSEL0 &= ~(3 << 0);    	//SELECCIONAMOS EL PIN P0.0 EN GPIO0
	*PINMODE0 |= (2 << 0);	    //SELECCIONAMOS EL PIN P0.0 PARA QUE NO TENGA NI PULL UP NI PULL DOWN
	*PINMODE_OD0 &= ~(1 << 0);  //SELECCIONAMOS EL PIN P0.0 SIN OPEN DRAIN
	*FIODIR |= (1 << 0);     	//SELECCIONAMOS EL PIN P0.0 PARA QUE SEA SALIDA

	*PINSEL0 &= ~(3 << 2);    	//SELECCIONAMOS EL PIN P0.1 EN GPIO0
	*PINMODE0 |= (2 << 2);	    //SELECCIONAMOS EL PIN P0.1 PARA QUE NO TENGA NI PULL UP NI PULL DOWN
	*PINMODE_OD0 &= ~(1 << 1);  //SELECCIONAMOS EL PIN P0.1 SIN OPEN DRAIN
	*FIODIR |= (1 << 1);     	//SELECCIONAMOS EL PIN P0.1 PARA QUE SEA SALIDA

	*PINSEL1 &= ~(3 << 0);    	//SELECCIONAMOS EL PIN P0.16 EN GPIO0
	*PINMODE1 |= (2 << 0);	    //SELECCIONAMOS EL PIN P0.16 PARA QUE NO TENGA NI PULL UP NI PULL DOWN
	*PINMODE_OD0 &= ~(1 << 16); //SELECCIONAMOS EL PIN P0.16 SIN OPEN DRAIN
	*FIODIR |= (1 << 16);     	//SELECCIONAMOS EL PIN P0.16 PARA QUE SEA SALIDA

	*PINSEL1 &= ~(3 << 2);    	//SELECCIONAMOS EL PIN P0.17 EN GPIO0
	*PINMODE1 |= (2 << 2);	    //SELECCIONAMOS EL PIN P0.17 PARA QUE NO TENGA NI PULL UP NI PULL DOWN
	*PINMODE_OD0 &= ~(1 << 17); //SELECCIONAMOS EL PIN P0.17 SIN OPEN DRAIN
	*FIODIR |= (1 << 17);     	//SELECCIONAMOS EL PIN P0.17 PARA QUE SEA SALIDA

	*PINSEL1 &= ~(3 << 4);    	//SELECCIONAMOS EL PIN P0.18 EN GPIO0
	*PINMODE1 |= (2 << 4);	    //SELECCIONAMOS EL PIN P0.18 PARA QUE NO TENGA NI PULL UP NI PULL DOWN
	*PINMODE_OD0 &= ~(1 << 18); //SELECCIONAMOS EL PIN P0.18 SIN OPEN DRAIN
	*FIODIR |= (1 << 18);     	//SELECCIONAMOS EL PIN P0.18 PARA QUE SEA SALIDA

	//GPIO
	*PINSEL0 &= ~(3 << 12);    	//SELECCIONAMOS EL PIN P0.6 EN GPIO0
	*PINSEL0 &= ~(3 << 14);    	//SELECCIONAMOS EL PIN P0.7 EN GPIO0
	*PINSEL0 &= ~(3 << 16);    	//SELECCIONAMOS EL PIN P0.8 EN GPIO0
	*PINSEL0 &= ~(3 << 18);    	//SELECCIONAMOS EL PIN P0.9 EN GPIO0
	*PINMODE0 |= (3 << 12);     //SELECCIONAMOS EL PIN P0.6 PARA QUE TENGA PULL DOWN
	*PINMODE0 |= (3 << 14);     //SELECCIONAMOS EL PIN P0.7 PARA QUE TENGA PULL DOWN
	*PINMODE0 |= (3 << 16);     //SELECCIONAMOS EL PIN P0.8 PARA QUE TENGA PULL DOWN
	*PINMODE0 |= (3 << 18);     //SELECCIONAMOS EL PIN P0.9 PARA QUE TENGA PULL DOWN
	*PINMODE_OD0 &= ~(1 << 6);  //SELECCIONAMOS EL PIN P0.6 SIN OPEN DRAIN
	*PINMODE_OD0 &= ~(1 << 7);  //SELECCIONAMOS EL PIN P0.7 SIN OPEN DRAIN
	*PINMODE_OD0 &= ~(1 << 8);  //SELECCIONAMOS EL PIN P0.8 SIN OPEN DRAIN
	*PINMODE_OD0 &= ~(1 << 9);  //SELECCIONAMOS EL PIN P0.9 SIN OPEN DRAIN
	*FIODIR &= ~(1 << 6);       //SELECCIONAMOS EL PIN P0.6 PARA QUE SEA ENTRADA
	*FIODIR &= ~(1 << 7);       //SELECCIONAMOS EL PIN P0.7 PARA QUE SEA ENTRADA
	*FIODIR &= ~(1 << 8);       //SELECCIONAMOS EL PIN P0.8 PARA QUE SEA ENTRADA
	*FIODIR &= ~(1 << 9);       //SELECCIONAMOS EL PIN P0.9 PARA QUE SEA ENTRADA

	//ADC
	*PINSEL1 |= (1<<14);		//SELECCIONAMOS EL PIN P0.23 EN AD0.0
	*PINSEL1 &= ~(1<<15);		//SELECCIONAMOS EL PIN P0.23 EN AD0.0
	*PINMODE1 |= (2 << 14);     //SELECCIONAMOS EL PIN P0.23 PARA QUE NO TENGAN PULL UP, PULL DOWN
	*PINMODE_OD0 &= ~(1 << 23); //SELECCIONAMOS EL PIN P0.23 SIN OPEN DRAIN
	*FIODIR &= ~(1<<23);	    //SELECCIONAMOS EL PIN P0.23 PARA QUE SEA ENTRADA

	//UART
	*PINSEL0 |= (1<<4); 		//SELECCIONAMOS EL PIN P0.2 COMO TXD0 PARA UART0
	*PINSEL0 &= ~(1<<5);		//SELECCIONAMOS EL PIN P0.2 COMO TXD0 PARA UART0
	*PINSEL0 |= (1<<6); 		//SELECCIONAMOS EL PIN P0.3 COMO RXD0 PARA UART0
	*PINSEL0 &= ~(1<<7);		//SELECCIONAMOS EL PIN P0.3 COMO RXD0 PARA UART0
	//*PINMODE0 |= (2<<4);		//SELECCIONAMOS EL PIN P0.2 PARA QUE NO TENGA NI PULL UP, NI PULL DOWN
	//*PINMODE0 |= (2<<6);		//SELECCIONAMOS EL PIN P0.3 PARA QUE NO TENGA NI PULL UP, NI PULL DOWN
	//*PINMODE_OD0 &= ~(3<<2); 	//SELECCIONAMOS EL PIN P0.2,3 SIN OPEN DRAIN
}

void ConfigTimer()
{
	*CTCR = 0;					//SELECCIONAMOS MODO TIMER
	*TCR = 0;					//DESHABILITAMOS EL CONTADOR DE TIMER TC
	*MR0 = 0;					//MATCHREG0 VARIABLE
	*MR1 = 25000;				//MATCHREG1 FIJO LO CARGAMOS CON 25000 PARA QUE INTERRUMPA CADA 1ms
	*MR2 = 12500;				//MATCHREG2 FIJO LO CARGAMOS CON 12500 PARA QUE INTERRUMPA CADA 0,5ms
	//*MCR = 25; 					//B’011001’ EN MCR, INTERRUMPE (MR0), INTERRUMPE (MR1), RESETEA (MR1)
	*MCR = 89; 					//B’001011001’ EN MCR, INTERRUMPE (MR0), INTERRUMPE (MR1), INTERRUMPE (MR2), RESETEA (MR1)
	*PR = 0;					//PRESCALER EN 0, EL TC AUMENTA CADA 40ns
	*PC = 0; 					//PONGO EN 0 CONTADOR DE PRESCALER
	*TC = 0; 					//PONGO EN 0 CONTADOR DE TIMER
	*ISER0 |= (1<<1);        	//HABILITO INTERRUPCION POR TIMER
	*TCR = 1; 					//HABILITAMOS EL CONTADOR DE TIMER TC
}

void ConfigGpio()
{
	*IO0INTENR |= (1<<6);		//HABILITAMOS P0.6 CON FLANCO DE SUBIDA
	*IO0INTENR |= (1<<7);		//HABILITAMOS P0.7 CON FLANCO DE SUBIDA
	*IO0INTENR |= (1<<8);		//HABILITAMOS P0.8 CON FLANCO DE SUBIDA
	*IO0INTENR |= (1<<9);		//HABILITAMOS P0.9 CON FLANCO DE SUBIDA
	*IO0INTCLR |= (1<<6);		//BAJAMOS BANDERA P0.6
	*IO0INTCLR |= (1<<7);		//BAJAMOS BANDERA P0.7
	*IO0INTCLR |= (1<<8);		//BAJAMOS BANDERA P0.8
	*IO0INTCLR |= (1<<9);		//BAJAMOS BANDERA P0.9
	*ISER0 |= (1<<21);			//HABILITAMOS INTERRUPCION POR EINT3
	*EXTINT |= (1<<3);			//BAJAMOS BANDERA EINT3
}

void ConfigADC()
{
	*PCONP|=(1<<12);			//HABILITAMOS PERIFERICO ADC (PCADC)
	*AD0CR |= (1<<0);			//SELECCIONO EL CANAL AD0.0 (BIT 0)
	*AD0CR |= (4<<8);			//DIVIDO POR (4+1) AL PCLK_ADC0 ES DECIR 25MHZ/5 = 5MHZ (BITS 8,9,10,11,12,13,14,15) DEBE SER MENOR O IGUAL A 13MHZ
	*AD0CR |= (1<<21);			//SELECCIONAMOS EL MODO OPERACIONAL PARA EL ADC (BIT 21)
	*ISER0 |= (1<<22);      	//HABILITAMOS INTERRUPCION POR ADC
	*AD0INTEN |=(1<<0);			//CUANDO TERMINA CONVERSION SE GENERA INTERRUPCION
}

void ConfigUart()
{
	/**PCONP |= (1<<3);			//HABILITAMOS PERIFERICO UART0 (PCUART0)
	*LCR |= 11;					//CONFIGURAMOS PARA TENER LONGITUD DE CARACTER DE 8 BITS
								//SELECCIONAMOS BIT DE STOP
								//HABILITAMOS PARA QUE HAYA PARIDAD
								//DESHABILITAMOS EL BREAK CONTROL
	*LCR |= (1<<7);				//HABILITAMOS DLAB PARA PODER ACCEDER A DLL,DLM (CONFIGURACION DE BAUD RATE)
	*DLL = 161;					//BAUD RATE ~= 9600
	*DLM = 0;					//
	*LCR &= ~(1<<7);			//DESHABILITAMOS DLAB PARA PODER ACCEDER A RBR,THR,IER
	*ISER0 |= (1<<5);			//HABILITAMOS INTERRUPCION POR UART0
	*IER = 1;					//SELECCIONAMOS FUENTE DE INTERRUPCION DEL MODULO UART0*/

	LPC_SC->PCONP |= (1<<3);
	LPC_UART0->LCR |= 11;
	LPC_UART0->LCR |= (1<<7);
	LPC_UART0->DLL = 0b10100001;
	LPC_UART0->DLM = 0;
	LPC_UART0->LCR &= 0b01111111;
	LPC_UART0->IER = 1;
	NVIC_EnableIRQ(UART0_IRQn);
}

void TIMER0_IRQHandler(void)
{
	if (*IR &(1<<0)) 		//INTERRUMPIO POR MR0?
	{
		*FIOSET = (1<<16);  //APAGAR LED 1 (LAMPARA) (LOGICA NEGATIVA)
		*IR |=(1<<0);       //BAJO LA BANDERA DE MR0
	}
	if (*IR &(1<<1)) 		//INTERRUMPIO POR MR1?
	{
		*FIOCLR = (1<<16);	//PRENDER LED 1 (LAMPARA) (LOGICA NEGATIVA)
		*IR |=(1<<1);       //BAJO LA BANDERA DE MR1
	}
	if (*IR &(1<<2)) 		//INTERRUMPIO POR MR2?
	{
		ConfTiempo();
		*IR |=(1<<2);       //BAJO LA BANDERA DE MR2
	}
}

void EINT3_IRQHandler(void)
{
	if (*IO0INTSTATR &(1<<7))	//INTERRUMPIO POR P0.7?
	{
		PrenderApagarSistema();
		*IO0INTCLR |=(1<<7);	//BAJO LA BANDERA DE P0.7
	}

	if (*IO0INTSTATR &(1<<6))	//INTERRUMPIO POR P0.6?
	{
		ModoAutomaticoManual();
		*IO0INTCLR |=(1<<6);	//BAJO LA BANDERA DE P0.6
	}

	if (*IO0INTSTATR &(1<<8))	//INTERRUMPIO POR P0.8?
	{
		AumentarBrillo();
		*IO0INTCLR |=(1<<8);	//BAJO LA BANDERA DE P0.8
	}
	if (*IO0INTSTATR &(1<<9))	//INTERRUMPIO POR P0.9?
	{
		DisminuirBrillo();
		*IO0INTCLR |=(1<<9);	//BAJO LA BANDERA DE P0.9
	}
}

void ADC_IRQHandler(void)
{
	int binario = (*ADDR0>>4) & (0xFFF); //DEBIDO A QUE EL VALOR SE ENCUENTRA ENTRE EL BIT 4 AL BIT 16
										 //LO QUE HACEMOS ES CAPTURAR EL VALOR Y CORRERLO PARA QUE QUEDE ENTRE EL BIT 0 AL BIT 11
	DecodificarConversion(binario);
	*AD0GDR |= (1<<30);

}

void DecodificarConversion(int conversion)
{
	int posicion = 0;			//VARIABLE QUE INDICARA LA POSICION DEL ARREGLO
	int rango = 19500;			//VARIABLE QUE CONTIENE EL RANGO DE CONVERSION
	static int CantMuestras = 0;
	static int PromMuestras = 0;

	conversion = 100*conversion;	//AGRANDAMOS BINARIO PARA QUE SE TENGA MUCHO MAS MARGEN

	if(CantMuestras == 100)
	{
		while(PromMuestras > rango)	//METODO PARA SABER QUE NIVEL DE BRILLO MOSTRAR EN LED
		{
			rango = rango + 19500;		//AUMENTAMOS RANGO
			posicion++;					//INCREMENTAMOS VARABLE
		}

		if(flagModos==0 && flagSistema==1)
		{
			*MR0 = Brillo[posicion];					//CARGAMOS MR0 PARA REALIZAR NIVEL DE BRILLO
			brilloLed = *MR0;
		}

		CantMuestras = 0;
		PromMuestras = 0;
	}
	else
	{
		CantMuestras++;
		PromMuestras = PromMuestras + conversion;
		if(CantMuestras == 100)
		{
			PromMuestras = PromMuestras/CantMuestras;
		}
	}
}

void UART0_IRQHandler(void)
{
	static uint8_t flagEstados;	//INDICA EN QUE ESTADO SE ENCUENTRA LA LAMPARA DE PIE INTELIGENTE

	//flagEstados = *RBR;			//CARGAMOS MANSAJE RECIBIDO
	flagEstados = LPC_UART0->RBR;   //CARGAMOS MANSAJE RECIBIDO

	if(flagEstado5 == 0 && flagEstado6 == 0)
	{
		if(flagEstados=='1')
		{
			PrenderApagarSistema();
		}
		else if(flagEstados=='2')
		{
			ModoAutomaticoManual();
		}
		else if(flagEstados=='3')
		{
			AumentarBrillo();
		}
		else if(flagEstados=='4')
		{
			DisminuirBrillo();
		}
		else if(flagEstados=='5')
		{
			flagEstado5 = 1;
		}
		else if(flagEstados=='6')
		{
			flagEstado6 = 1;
		}
		else if(flagEstados=='7')
		{
			EstadoLampara();
		}
	}
	else
	{
		if(flagEstado5 == 1)
		{
			if(flagTiempo==0)
			{
				Hora = flagEstados;
				flagTiempo = 1;
			}
			else
			{
				Minuto = flagEstados;
				flagTiempo = 0;
				flagEstado5 = 0;
				HoraON = 1;
				HoraOFF = 0;

				milisegundos = 0;
				segundos = 0;
				minutos = 0;
				horas = 0;
			}
		}
		if(flagEstado6 == 1)
		{
			if(flagTiempo==0)
			{
				Hora = flagEstados;
				flagTiempo = 1;
			}
			else
			{
				Minuto = flagEstados;
				flagTiempo = 0;
				flagEstado6 = 0;
				HoraOFF = 1;
				HoraON = 0;

				milisegundos = 0;
				segundos = 0;
				minutos = 0;
				horas = 0;
			}
		}
	}
}

void Enviar(uint8_t mensaje)
{
	/*while((*LSR & (1<<5)) == 0)	//ENVIAR MENSAJE
	{
		*THR = mensaje;
	}*/

	while( ( LPC_UART0->LSR & (1<<5) ) == 0 );
	LPC_UART0->THR = mensaje;
}

void PrenderApagarSistema()
{
	if(flagSistema==0)
	{
		*FIOCLR = (1<<0); 	//SE PRENDE LED 2 (SISTEMA PRENDIDO) (LOGICA NEGATIVA)
		*FIOSET = (1<<1); 	//SE APAGA LED 2 (SISTEMA APAGADO) (LOGICA NEGATIVA)

		flagModos=1;

		*FIOCLR = (1<<17); 	//SE APAGA LED 3 (PRENDIDO) (LOGICA NEGATIVA)
		*FIOSET = (1<<18); 	//SE APAGA LED 3 (APAGADO) (LOGICA NEGATIVA)

		*MR0 = 23750;		//PRENDE LED
		flagSistema = 1;	//ESTADO OFF
	}
	else					//APAGAR LED
	{
		*FIOSET = (1<<0); 	//SE APAGA LED 2 (SISTEMA PRENDIDO) (LOGICA NEGATIVA)
		*FIOCLR = (1<<1); 	//SE PRENDE LED 2 (SISTEMA APAGADO) (LOGICA NEGATIVA)

		*FIOSET = (1<<17); 	//SE APAGA LED 3 (PRENDIDO) (LOGICA NEGATIVA)
		*FIOSET = (1<<18); 	//SE APAGA LED 3 (APAGADO) (LOGICA NEGATIVA)

		*MR0 = 0;			//APAGA LED
		flagSistema = 0;	//ESTADO ON
	}
	brilloLed = *MR0;
}

void ModoAutomaticoManual()
{
	if(flagSistema==1)
	{
		if(flagModos==0)
		{
			*FIOSET = (1<<18); 			//SE APAGA LED 3 (MODO AUTOMATICO) (LOGICA NEGATIVA)
			*FIOCLR = (1<<17); 			//SE PRENDE LED 3 (MODO MANUAL) (LOGICA NEGATIVA)
			flagModos=1;
		}
		else
		{
			*FIOCLR = (1<<18); 			//SE PRENDE LED 3 (MODO AUTOMATICO) (LOGICA NEGATIVA)
			*FIOSET = (1<<17); 			//SE APAGA LED 3 (MODO MANUAL) (LOGICA NEGATIVA)
			flagModos=0;
		}
	}
}

void AumentarBrillo()
{
	if(flagSistema==1)
	{
		if(flagModos==1)
		{
			if(brilloLed<23750)
			{
				*MR0 = *MR0 + 1250; //AL TENER UN PERIODO DE 1ms ESTO AUMENTA CADA 0.05ms (AUMENTAMOS BRILLO LED)
			}
			brilloLed = *MR0;
		}
	}
}

void DisminuirBrillo()
{
	if(flagSistema==1)
	{
		if(flagModos==1)
		{
			if(brilloLed>0)
			{
				*MR0 = *MR0 - 1250; //AL TENER UN PERIODO DE 1ms ESTO DISMINUYE CADA 0.05ms (DISMINUIMOS BRILLO LED)
			}
			brilloLed = *MR0;
		}
	}
}

void EstadoLampara()
{
	if(flagSistema==0)
	{
		Enviar('A');
	}
	else
	{
		if(brilloLed == Brillo[0])
		{
			Enviar('B');
		}
		else if(brilloLed == Brillo[1])
		{
			Enviar('c');
		}
		else if(brilloLed == Brillo[2])
		{
			Enviar('D');
		}
		else if(brilloLed == Brillo[3])
		{
			Enviar('e');
		}
		else if(brilloLed == Brillo[4])
		{
			Enviar('f');
		}
		else if(brilloLed == Brillo[5])
		{
			Enviar('G');
		}
		else if(brilloLed == Brillo[6])
		{
			Enviar('H');
		}
		else if(brilloLed == Brillo[7])
		{
			Enviar('i');
		}
		else if(brilloLed == Brillo[8])
		{
			Enviar('j');
		}
		else if(brilloLed == Brillo[9])
		{
			Enviar('K');
		}
		else if(brilloLed == Brillo[10])
		{
			Enviar('M');
		}
		else if(brilloLed == Brillo[11])
		{
			Enviar('N');
		}
		else if(brilloLed == Brillo[12])
		{
			Enviar('o');
		}
		else if(brilloLed == Brillo[13])
		{
			Enviar('P');
		}
		else if(brilloLed == Brillo[14])
		{
			Enviar('q');
		}
		else if(brilloLed == Brillo[15])
		{
			Enviar('r');
		}
		else if(brilloLed == Brillo[16])
		{
			Enviar('S');
		}
		else if(brilloLed == Brillo[17])
		{
			Enviar('t');
		}
		else if(brilloLed == Brillo[18])
		{
			Enviar('U');
		}
		else if(brilloLed == Brillo[19])
		{
			Enviar('V');
		}
		else if(brilloLed == Brillo[20])
		{
			Enviar('w');
		}
	}
}

void ConfTiempo()
{
	if(HoraON==1)
	{
		ContarTiempo();

		if(horas == Hora && minutos == Minuto)
		{
			flagSistema = 0;
			PrenderApagarSistema();

			milisegundos = 0;
			segundos = 0;
			minutos = 0;
			horas = 0;

			HoraON = 0;
		}
	}
	if(HoraOFF==1)
	{
		ContarTiempo();

		if(horas == Hora && minutos == Minuto)
		{
			flagSistema = 1;
			PrenderApagarSistema();

			milisegundos = 0;
			segundos = 0;
			minutos = 0;
			horas = 0;

			HoraOFF = 0;
		}
	}
}

void ContarTiempo()
{
	if(milisegundos==1000)
	{
		if(segundos==60)
		{
			if(minutos==60)
			{
				if(horas==24)
				{
					horas = 0;
				}
				else
				{
					horas++;
				}
				minutos = 0;
			}
			else
			{
				minutos++;
			}
			segundos = 0;
		}
		else
		{
			segundos++;
		}
		milisegundos = 0;
	}
	else
	{
		milisegundos++;
	}
}
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
