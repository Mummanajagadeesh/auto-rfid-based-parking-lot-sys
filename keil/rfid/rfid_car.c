#include <reg51.h>
#include <string.h>

// Pin Definitions
sbit doorp = P2^0;
sbit doorn = P2^1;
sbit rs = P2^5;
sbit rw = P2^6;
sbit en = P2^7;

// Function Declarations
void lcddat (unsigned char);
void lcdcmd (unsigned char);
void lcddis (unsigned char *);
void lcd_init();
void serial_init();
void check();
void delay();
void mdelay();

// Global Variables
unsigned char rfid[13], v1; // Changed to 13 to allow null-termination

void main()
{
    doorp = doorn = 0;
    serial_init();
    lcd_init();
    
    lcddis("RFID BASED CAR");
    lcdcmd(0xC0);
    lcddis("PARKING SYSTEM");
    delay();
    delay();
    lcdcmd(0x01);
    
    while(1)
    {
        lcdcmd(0x01);
        lcddis("SWIPE YOUR CARD");

				for(v1 = 0; v1 < 12; v1++)
				{
						while(RI == 0); // Wait for UART reception
						rfid[v1] = SBUF; // Read received character
						RI = 0; // Clear Receive Interrupt Flag

						SBUF = rfid[v1]; // Echo character back for debugging
						while(TI == 0);
						TI = 0; // Clear Transmit Interrupt Flag
				}
				rfid[12] = '\0'; // Null-terminate to avoid string corruption
				check(); // Call check() only after receiving full RFID

    }
}

// Function to Check RFID
void check()
{
    static unsigned int entry_count = 0; // Tracks number of scans for each RFID
    static char last_rfid[13] = ""; // Stores the last scanned RFID
    
    // Ensure full RFID is received before checking
    if (strlen(rfid) < 12)
        return;

    lcdcmd(0x01);
    lcddis("RFID SCANNED:");
    lcdcmd(0xC0);
    lcddis(rfid); // Debugging: Show scanned RFID
    delay();

    if (strcmp(rfid, "AB1234567890") == 0 || strcmp(rfid, "BC1234567890") == 0 ||
        strcmp(rfid, "CD1234567890") == 0 || strcmp(rfid, "DE1234567890") == 0) 
    {
        // Check if the same RFID is being scanned again (to alternate entry & exit)
        if (strcmp(last_rfid, rfid) == 0)
        {
            entry_count++; // Increment scan count if the same RFID is used again
        }
        else
        {
            entry_count = 1; // Reset count for a new RFID
        }

        strcpy(last_rfid, rfid); // Store last scanned RFID
        
        lcdcmd(0x01);
        lcddis("ACCESS GRANTED");

        if (entry_count % 2 == 1) // Odd scan: Entry
        {
            lcdcmd(0xC0);
            
            // Show slot details only during entry
            if (strcmp(rfid, "AB1234567890") == 0)
                lcddis("AB - BLOCK 00");
            else if (strcmp(rfid, "BC1234567890") == 0)
                lcddis("BC - BLOCK 01");
            else if (strcmp(rfid, "CD1234567890") == 0)
                lcddis("CD - BLOCK 10");
            else if (strcmp(rfid, "DE1234567890") == 0)
                lcddis("DE - BLOCK 11");

            delay();
            lcdcmd(0x01);
            lcddis("DOOR OPENING");
            lcdcmd(0xC0);
            lcddis("ALLOW INSIDE");
        }
        else // Even scan: Exit
        {
            lcdcmd(0x01);
            lcddis("DOOR OPENING");
            lcdcmd(0xC0);
            lcddis("ALLOW EXIT");
        }

        doorp = 1; doorn = 0;
        mdelay();
        doorp = 0; doorn = 0;
        delay();

        lcdcmd(0x01);
        lcddis("DOOR CLOSING");
        doorp = 0; doorn = 1;
        mdelay();
        doorp = 0; doorn = 0;
    }
    else
    {
        lcdcmd(0x01);
        lcddis("ACCESS DENIED");
        lcdcmd(0xC0);
        lcddis("INVALID CARD");
        mdelay();
    }
}


// LCD Initialization
void lcd_init()
{
    lcdcmd(0x38);
    lcdcmd(0x01);
    lcdcmd(0x10);
    lcdcmd(0x0C);
    lcdcmd(0x80);
}

// LCD Command Function
void lcdcmd(unsigned char val)
{
    P1 = val;
    rs = 0;
    rw = 0;
    en = 1;
    delay();
    en = 0;
}

// LCD Data Function
void lcddat(unsigned char val)
{
    P1 = val;
    rs = 1;
    rw = 0;
    en = 1;
    delay();
    en = 0;
}

// LCD String Display Function
void lcddis(unsigned char *s)
{
    unsigned char w;
    for(w = 0; s[w] != '\0'; w++)
    {
        lcddat(s[w]);
    }
}

// Serial Communication Initialization
void serial_init()
{
    SCON = 0x50; // 8-bit UART Mode
    TMOD = 0x20; // Timer 1 Mode 2 (Auto-Reload)
    TH1 = -3;    // Baud Rate 9600
    TR1 = 1;     // Start Timer 1
}

// Short Delay Function
void delay()
{
    unsigned int v5;
    for(v5 = 0; v5 < 10000; v5++);
}

// Long Delay Function
void mdelay()
{
    unsigned int v6, v7;
    for(v6 = 0; v6 < 2; v6++)
    {
        for(v7 = 0; v7 < 60000; v7++);
    }
}


