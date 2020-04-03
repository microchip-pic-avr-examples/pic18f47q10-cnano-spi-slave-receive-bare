/**
 * \file main.c
 *
 * \brief Main source file.
 *
 (c) 2020 Microchip Technology Inc. and its subsidiaries.
    Subject to your compliance with these terms, you may use this software and
    any derivatives exclusively with Microchip products. It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/* WDT operating mode->WDT Disabled */
#pragma config WDTE = OFF
/* Low voltage programming enabled, RE3 pin is MCLR */
#pragma config LVP = ON

#include <xc.h>
#include <stdint.h>

static void CLK_Initialize(void);
static void PPS_Initialize(void);
static void PORT_Initialize(void);
static void SPI1_Initialize(void);
static uint8_t SPI1_exchangeByte(uint8_t data);

uint8_t receiveData;        /* Data that will be received */
uint8_t writeData = 1;      /* Data that will be transmitted */

static void CLK_Initialize(void)
{
    OSCCON1bits.NOSC = 6;        /* HFINTOSC Oscillator */
    
    OSCFRQbits.HFFRQ = 2;        /* HFFRQ 4 MHz */
}

static void PPS_Initialize(void)
{
    SSP1SSPPS = 0x05;             /* SS channel on RA5 */
    
    RC3PPS = 0x0F;               /* SCK channel on RC3 */
 
    SSP1DATPPS = 0x14;           /* SDI channel on RC4 */
    
    RC5PPS = 0x10;               /* SDO channel on RC5 */
}

static void PORT_Initialize(void)
{
    /* SDO as output; SDI, SCK as input */
    TRISC = 0xDF;
    
    /* SS as digital pin */
    ANSELA = 0xDF;  
    
    /* SCK, SDI, SDO as digital pins */
    ANSELC = 0xC7;         
}

static void SPI1_Initialize(void)
{
    /* Enable module, SPI Slave Mode */
    SSP1CON1 = 0x24;        
}

static uint8_t SPI1_exchangeByte(uint8_t data)
{
    SSP1BUF = data;
    
    while(!PIR3bits.SSP1IF) /* Wait until data is exchanged */
    {
        ;
    }   
    PIR3bits.SSP1IF = 0;
    
    return SSP1BUF;
}

int main(void)
{
    CLK_Initialize();
    PPS_Initialize();
    PORT_Initialize();
    SPI1_Initialize();
    
    while(1)
    {
        if(!PORTAbits.RA5)              /* SS line is LOW */
        {
            receiveData = SPI1_exchangeByte(writeData);
        }
    }
}
