#include "ADC.h"


Adc adc; //preinitiate adc

ISR(ADC_vect)
{
  adc.isrCallback();
}

void Adc::begin(byte reference)
{
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Prescaler 128.So F_adc = 16MHz/128 = 125KHz
  ADMUX  |= (reference << REFS0);                       //Reference 
  ADCSRA |= (1 << ADEN);                                //Enable ADC
  
}

void Adc::setAutoTriggerSource(byte source)
{
  if(source)
  {
    ADCSRA |= (1 << ADATE); //ADC Auto trigger enable
  }
  ADCSRB &= 0b11111000;   //Clearing trigger source bits
  ADCSRB |= source;
}

void Adc::attachInterrupt(void (*isr)())
{
  if(isr !=NULL)
  {
  	isrCallback = isr;
  }
  sei();                  //Enable Global Interrupt
  ADCSRA |= ((1 << ADIE)| (1 << ADEN));  //ADC Conversion complete interrupt enable
  
}
void Adc::detachInterrupt()
{
  ADCSRA &= ~((1 << ADEN) | (1 << ADIE));
}


void Adc::startConversion(byte channel)
{
  setChannel(channel);
  ADCSRA |= (1 << ADSC);  //start conversion
}
void Adc::setChannel(byte channel)
{
  ADMUX = (ADMUX & 0b11110000) | (channel);
}


uint16_t Adc::read(byte channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |=1<<ADSC; //Start Conversion
	while(!(ADCSRA&(1<<ADIF))); //Wait until conversion completes
	ADCSRA|= 1<<ADIF; //ADIF must be cleared(1) to trigger a new conversion next time
	return ADCW; //return ADC value
}
float Adc:readAverage(byte channel, byte N)
{
	uint32_t adcSum = 0; 
	for(byte i = 0; i< N; i++)
	{
		adcSum += read(channel);
	}

	float result = (float)adcSum/N;
	return result;
}

float Adc::getAvcc()
{
	float avcc = readAverage(0b1110,50);
	avcc = (1024.0*1.1)/avcc;
	return avcc;
}