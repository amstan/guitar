#ifndef ADC_H
#define ADC_H

/*! @file adc.h
 * ADC reading library
 */

#define sbiBF(port,bit)  (port |= (1<<bit))   //set bit in port
#define cbiBF(port,bit)  (port &= ~(1<<bit))  //clear bit in port

/*! Initialize the ADC with the selected ADC-channel
 * @param input port number to start adc reading
 */
void ADC_init(char input);

/*! Do a Analog to Digital Conversion
 * @returns what it read from adc
 */
int ADC_read(void);

#endif