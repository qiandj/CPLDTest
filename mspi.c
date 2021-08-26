#include   <stdlib.h>
#include   <stdio.h>
#include   <time.h>
#include   <conio.h>
#include   <string.h>
#include <unistd.h>
#pragma inline
#define UINT8 unsigned char
#define UINT16 unsigned short
#define BYTE  unsigned char

UINT8 IoRead8(UINT16 Port)
{
        UINT8 Value;
        _asm {
                mov dx, Port
                in al, dx
                mov Value,al
        };
        return Value;
}
void IoWrite8(UINT16 Port,UINT8 Value)
{
        _asm {
                mov dx, Port
                mov     al, Value
                out dx, al
         //       jmp $+2
         //       jmp $+2
        };
}


void ready(){
        _asm{
                mov dx, 200h
        };
       
        _asm{
         w4_spi:
                in al, dx
                and al, 80h
                cmp al, 80h
                jne w4_spi
        };
}

void opk(int num, UINT8 a1, UINT8 a2){ //write serial bytes
        IoWrite8(0x200,0x80);
        ready();
        IoWrite8(0x201,a1);
        IoWrite8(0x201,a2);
        IoWrite8(0x202,num);
        IoWrite8(0x202,0);
        IoWrite8(0x200,0x43);
        ready();
}


UINT8 op000(UINT8 v){  //read
        UINT8 x;//////////////////////////////////////////////////
        //FILE *fo = NULL;
//      fo = fopen("readout.txt", "w");
        IoWrite8(0x200,0x80);
        IoWrite8(0x201,v);
        IoWrite8(0x200,0x40);
        ready();
        x = IoRead8(0x201);
        //printf("op=000  value_out=%x\n", x);
//      if (fo != NULL)
//              fclose(fo);
        return x;
}

int rd_clk(UINT8 AD1){
        UINT8 RD;
        opk(2, 0x55, 0x57);
        RD = op000(0x56);
        RD = op000(0x56);
        printf("CLK=%x\n", RD);
        return 1;
}

int wt_clk(UINT8 AD1){
        UINT8 RD;
        opk(2, 0x55, 0x57);
                opk(2, 0x57, AD1);
        RD = op000(0x56);
        RD = op000(0x56);
        printf("SET_CLK=%x\n", RD);
        return 1;
}

int RD_256(UINT8 AD1, UINT8 AD2, UINT8 AD3) {
        UINT8 RD;
        int i = 0x100;
                FILE *fo = NULL;
        fo = fopen("RD256.bin", "wb");
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
              RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x03);     //READ CMD
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);     //discard
        RD = op000(0x56);
        RD = op000(0x56);
                
                
        opk(2, 0x55, 0x59);
        opk(2, 0x57, AD1);      //READ AD1
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
                //printf("DATA0: %02X\n", RD);
                
        opk(2, 0x55, 0x59);
        opk(2, 0x57, AD2);      //READ AD2
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
                //printf("DATA1: %02X\n", RD);
        opk(2, 0x55, 0x59);
        opk(2, 0x57, AD3);      //READ AD3
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
        //printf("DATA2: %02X\n", RD);
                
                while (i > 0){
                        opk(2, 0x55, 0x59);
                        opk(2, 0x57, 0x0);      //READ CMD
                        opk(2, 0x55, 0x5a);
                        RD = op000(0x56);
                        RD = op000(0x56);
                        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
                        }
                        opk(2, 0x55, 0x5b);
                        RD = op000(0x56);
                        RD = op000(0x56);
                        //printf("DATA3: %02X\n", RD);
                        fprintf(fo,"%c", RD);
						if(i >= 250) {
							printf("%02x", RD);
						}
                        i--;
                }
                
                opk(2, 0x55, 0x56);
        opk(2, 0x57, 0x80);
                /*
                opk(2, 0x55, 0x5a);
                RD = op000(0x56);
                RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
                RD = op000(0x56);
        }
                */
        if (fo != NULL) fclose(fo);       
        return 1;
}

int RD_ALL(char *file_name) {
        UINT8 RD;
        int i = 0x080000;
        FILE *fo = NULL;
        fo = fopen(file_name, "wb");
                if(fo == NULL) {
                        printf("please input the filename\n");
						return 0;
                }
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
              RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x03);     //READ CMD
        /*
                opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
                */
        opk(2, 0x55, 0x5b);     //discard
        RD = op000(0x56);
        RD = op000(0x56);
                
                
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x0);      //READ AD1
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
                */
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
                //printf("DATA0: %02X\n", RD);
                
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x0);      //READ AD2
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
                */
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
                //printf("DATA1: %02X\n", RD);
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x0);      //READ AD3
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
                */
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
                RD = op000(0x56);
        //printf("DATA2: %02X\n", RD);
                
                while (i > 0){
                        opk(2, 0x55, 0x59);
                        opk(2, 0x57, 0x0);      //READ CMD
                                                /*
                        opk(2, 0x55, 0x5a);
                        RD = op000(0x56);
                        RD = op000(0x56);
                        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
                        }
                                                */
                        opk(2, 0x55, 0x5b);
                        RD = op000(0x56);
                        RD = op000(0x56);
                        //printf("DATA3: %02X\n", RD);
                                                fprintf(fo,"%c", RD);
                        i--;
                }
                opk(2, 0x55, 0x56);
                opk(2, 0x57, 0x80);
                                /*
                opk(2, 0x55, 0x5a);
                RD = op000(0x56);
                RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
                RD = op000(0x56);
        }
                */
        if (fo != NULL) fclose(fo);       
        return 1;
        
}
 
UINT8 RDID(UINT8 DATA){
                UINT8 RD;
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
                RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, DATA);     //READ CMD
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x0);      //READ CMD
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
        //printf("ID: %02X\n", RD);
                
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0x80);
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
                RD = op000(0x56);
        }
                */
        return RD;
}

int RDST(){
                UINT8 RD,ST;
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
                RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x05);     //READ CMD
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x0);      //READ CMD
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        ST = op000(0x56);
        //printf("STATUS: %02X\n", ST);
                
                opk(2, 0x55, 0x56);
        opk(2, 0x57, 0x80);
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
                RD = op000(0x56);
        }
                */
        return ST;
}

int WTEN(UINT8 DATA){ //wten 
        UINT8 RD;
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);//c4
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
                RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, DATA);     //WTEN
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
        
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0x80);
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
                RD = op000(0x56);
        }
                */
        return 1;
        
        
}


/*
int WTDIS(){
                UINT8 RD,ST;
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);//c4
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
                RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x04);     //WTEN
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
        
                opk(2, 0x55, 0x56);
        opk(2, 0x57, 0x80);
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
                RD = op000(0x56);
        }
                
                ST = RDST();
                while(ST & 0x01 != 0) {
                        ST = RDST();
                }
                
        return 1;
        
        
}
*/
int ERASECHIP(){
        UINT8 RD, ST;
        RD = WTEN(0x06);
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);//c4
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
              RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0xC7);     //
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
        
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0x80);
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
              RD = op000(0x56);
        }
                */
        ST = RDST();
        while(ST & 0x01 != 0) {
             ST = RDST();
        }
        //RD = WTDIS();
                
        return 1;
        
        
}

int ERASE(UINT8 AD1, UINT8 AD2, UINT8 AD3, UINT8 CMD) {//
        UINT8 RD, ST;
       //         int i = 10;
        RD = WTEN(0x06);
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);//c4
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
              RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, CMD);     //READ CMD
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);     //discard
        RD = op000(0x56);
        RD = op000(0x56);
                
                
        opk(2, 0x55, 0x59);
        opk(2, 0x57, AD1);      //READ AD1
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
                //printf("DATA0: %02X\n", RD);
                
        opk(2, 0x55, 0x59);
        opk(2, 0x57, AD2);      //READ AD2
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
                //printf("DATA1: %02X\n", RD);
        opk(2, 0x55, 0x59);
        opk(2, 0x57, AD3);      //READ AD3
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
                RD = op000(0x56);
        //printf("DATA2: %02X\n", RD);
                
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0x80);
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
              RD = op000(0x56);
        }
        */        
        ST = RDST();
        while(ST & 0x01 != 0) {
                ST = RDST();
        }
        //RD = WTDIS();
        return 1;
        
}


int WTSTEN(){
        UINT8 RD;
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);//c4
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
                RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x50);     //WTSTEN
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
        
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0x80);
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
                RD = op000(0x56);
        }
                */
        return 1;
        
        
}

int WTST(UINT8 DATA){
        UINT8 RD;
        RD = WTSTEN();
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);//c4
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
                RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x01);     //WTEN
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
        
                opk(2, 0x55, 0x59);
        opk(2, 0x57, DATA);     //WTEN
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
                RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
        printf("DATA: %02X\n", DATA);
                
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0x80);
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
                RD = op000(0x56);
        }
                */
        return 1;
        
        
}



int WT(UINT8 AD1, UINT8 AD2, UINT8 AD3,UINT8 DA4) {
        UINT8 RD, ST;
        int i = 1;
        RD = WTEN(0x06);
        opk(2, 0x55, 0x56);
        opk(2, 0x57, 0xc0);//c4
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x10 != 0x10) {
              RD = op000(0x56);
        }
        
        opk(2, 0x55, 0x59);
        opk(2, 0x57, 0x02);     //READ CMD
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        //opk(2, 0x55, 0x5b);     //discard
        RD = op000(0x56);
        RD = op000(0x56);
                
                
        opk(2, 0x55, 0x59);
        opk(2, 0x57, AD1);      //READ AD1
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
                //printf("DATA0: %02X\n", RD);
                
        opk(2, 0x55, 0x59);
        opk(2, 0x57, AD2);      //READ AD2
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
                //printf("DATA1: %02X\n", RD);
        opk(2, 0x55, 0x59);
        opk(2, 0x57, AD3);      //READ AD3
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
              RD = op000(0x56);
        }
        //opk(2, 0x55, 0x5b);
        RD = op000(0x56);
                RD = op000(0x56);
        //printf("DATA2: %02X\n", RD);
                
        //while (i > 0){
        opk(2, 0x55, 0x59);
        opk(2, 0x57, DA4);  //    
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x08 != 0x08) {
            RD = op000(0x56);
        }
        opk(2, 0x55, 0x5b);
        RD = op000(0x56);
        RD = op000(0x56);
                
                opk(2, 0x55, 0x56);
        opk(2, 0x57, 0x80);
                
                /*
        opk(2, 0x55, 0x5a);
        RD = op000(0x56);
        RD = op000(0x56);
        while(RD & 0x80 != 0x80) {
                RD = op000(0x56);
        }
                */
                
        ST = RDST();
        while(ST & 0x01 != 0) {
             ST = RDST();
        }
        //RD = WTDIS();        
        return 1; 
}

int WTB(UINT8 ADD1, UINT8 ADD2, UINT8 ADD3, char *file_name) {
        UINT8 RD, ST;
        int i = 1, r = 0, f_eof = 0;
        int chh = 21;
                unsigned int AD1, AD2, AD3;
                FILE *fp = NULL;
                fp = fopen(file_name, "rb");
                if(fp == NULL){
                        printf("Error!!!NO Bin file!!!\n");
                        return 0; 
        }
                for(AD1 = ADD1; AD1 <= 0x07; AD1 ++  ){
                        for(AD2 = ADD2; AD2 <= 0xff; AD2 ++  ){
                                //for(AD3 = ADD3; AD3 <= 0xff; AD3 ++  ){
                                        RD = WTEN(0x06);
                                        opk(2, 0x55, 0x56);
                                        opk(2, 0x57, 0xc0);//c4
                                        opk(2, 0x55, 0x5a);
                                        RD = op000(0x56);
                                        RD = op000(0x56);
                                        while(RD & 0x10 != 0x10) {
                                                RD = op000(0x56);
                                        }
                                        
                                        opk(2, 0x55, 0x59);
                                        opk(2, 0x57, 0x02);     //READ CMD
                                        opk(2, 0x55, 0x5a);
                                        RD = op000(0x56);
                                        RD = op000(0x56);
                                        while(RD & 0x08 != 0x08) {
                                                RD = op000(0x56);
                                        }
                                                        
                                                        
                                        opk(2, 0x55, 0x59);
                                        opk(2, 0x57, AD1);      //READ AD1
                                        opk(2, 0x55, 0x5a);
                                        RD = op000(0x56);
                                        RD = op000(0x56);
                                        while(RD & 0x08 != 0x08) {
                                                RD = op000(0x56);
                                        }
                                        opk(2, 0x55, 0x5b);
                                        RD = op000(0x56);
                                        RD = op000(0x56);
                                                        
                                        opk(2, 0x55, 0x59);
                                        opk(2, 0x57, AD2);      //READ AD2
                                        opk(2, 0x55, 0x5a);
                                        RD = op000(0x56);
                                        RD = op000(0x56);
                                        while(RD & 0x08 != 0x08) {
                                                RD = op000(0x56);
                                        }
                                        opk(2, 0x55, 0x5b);
                                        RD = op000(0x56);
                                        RD = op000(0x56);
                                                        //printf("DATA1: %02X\n", RD);
                                        opk(2, 0x55, 0x59);
                                        opk(2, 0x57, 0x0);      //READ AD3
                                        opk(2, 0x55, 0x5a);
                                        RD = op000(0x56);
                                        RD = op000(0x56);
                                        while(RD & 0x08 != 0x08) {
                                                RD = op000(0x56);
                                        }
                                        //opk(2, 0x55, 0x5b);
                                        RD = op000(0x56);
                                        RD = op000(0x56);
                                        
                                                for (r = 0;  r <= 0xff; r++){
                                                        chh = fgetc(fp);
                                                        if(chh == EOF){
                                                                f_eof = 1;
                                                                break;
                                                        }
                                                        opk(2, 0x55, 0x59);
                                                        opk(2, 0x57, chh);  //    
                                                        opk(2, 0x55, 0x5a);
                                                        RD = op000(0x56);
                                                        RD = op000(0x56);
                                                        while(RD & 0x08 != 0x08) {
                                                                RD = op000(0x56);
                                                        }
                                                        opk(2, 0x55, 0x5b);
                                                        RD = op000(0x56);
                                                        RD = op000(0x56);
                                                }
                                                
                                                opk(2, 0x55, 0x56);
                                                opk(2, 0x57, 0x80);     //#CS
                                                /*
                                                opk(2, 0x55, 0x5a);
                                                RD = op000(0x56);
                                                RD = op000(0x56);
                                                while(RD & 0x80 != 0x80) {
                                                        RD = op000(0x56);
                                                }
                                                */
                                                ST = RDST();
                                                while(ST & 0x01 != 0) {
                                                        ST = RDST();
                                                }
                                                if(f_eof) {
                                                        return 1;
                                                }
                                        
                                        if (AD2 == 0xff){
                                                ADD2 = 0x0;
                                        }
                                }
                }
                fclose(fp);                  
        return 1; 
}


int main(int argc,char *argv[]){
        int ret;
        UINT8 DATA, DATA1,DATA2,DATA3;
        char cmd_0[] = "id";
        char cmd_1[] = "rd256";
        char cmd_2[] = "rdall";
        char cmd_3[] = "clall";
        char cmd_4[] = "cl4k";
        char cmd_5[] = "cl32k";
        char cmd_6[] = "cl64k";
        char cmd_7[] = "rdst";
        char cmd_8[] = "wtst";
        char cmd_9[] = "wtk";
        char cmd_A[] = "wten";
        char cmd_B[] = "wr1";
        char cmd_C[] = "w1";
        char cmd_D[] = "wt";
        char cmd_F[] = "wtclk";
        char cmd_G[] = "rdclk";
                           
             /*    DATA = RDID(0x9F);
                DATA = RDID(0x9F);
                if(DATA != 0xef) {
                        printf("FAIL, check the chip and connection.");
                        return 0;
                        
                } */
        if (strcmp(argv[1], cmd_0) == 0){                   
                ret = RDID(0x9F);
                                printf("ID:%02X\n",ret);
        }else if(strcmp(argv[1], cmd_1) == 0){
                                DATA1 = strtol(argv[2], &argv[2], 16); 
                                DATA2 = strtol(argv[3], &argv[3], 16);
                                DATA3 = strtol(argv[4], &argv[4], 16);                                  
                                ret = RD_256(DATA1,DATA2,DATA3);
        }else if(strcmp(argv[1], cmd_2) == 0){              
                ret = RD_ALL(argv[2]);
        }else if(strcmp(argv[1], cmd_3) == 0){              
                ret = ERASECHIP();
        }else if(strcmp(argv[1], cmd_4) == 0){
                                DATA1 = strtol(argv[2], &argv[2], 16); 
                                DATA2 = strtol(argv[3], &argv[3], 16);
                                DATA3 = strtol(argv[4], &argv[4], 16);                  
                ret = ERASE(DATA1,DATA2,DATA3,0x20);
        }else if(strcmp(argv[1], cmd_5) == 0){              
                DATA1 = strtol(argv[2], &argv[2], 16); 
                                DATA2 = strtol(argv[3], &argv[3], 16);
                                DATA3 = strtol(argv[4], &argv[4], 16);                  
                ret = ERASE(DATA1,DATA2,DATA3,0x52);
        }else if(strcmp(argv[1], cmd_6) == 0){              
                DATA1 = strtol(argv[2], &argv[2], 16); 
                                DATA2 = strtol(argv[3], &argv[3], 16);
                                DATA3 = strtol(argv[4], &argv[4], 16);                  
                ret = ERASE(DATA1,DATA2,DATA3,0xd8);
        }else if(strcmp(argv[1], cmd_7) == 0){              
                ret = RDST();
                                printf("STATUS: %02X\n", ret);
        }else if(strcmp(argv[1], cmd_8) == 0){  
                DATA = strtol(argv[2], &argv[2], 16);           
                ret = WTST(DATA);
        }else if(strcmp(argv[1], cmd_9) == 0){
                                DATA1 = strtol(argv[2], &argv[2], 16); 
                                DATA2 = strtol(argv[3], &argv[3], 16);
                                DATA3 = strtol(argv[4], &argv[4], 16);  
                                DATA = strtol(argv[5], &argv[5], 16);                                  
                ret = WT(DATA1,DATA2,DATA3,DATA);                       
        }else if(strcmp(argv[1], cmd_A) == 0){              
                ret = WTEN(0x06);
        }else if(strcmp(argv[1], cmd_B) == 0){ 
                DATA = strtol(argv[2], &argv[2], 16);           
                ret = RDID(DATA);
				printf("status:%02X\n",ret);
        }else if(strcmp(argv[1], cmd_C) == 0){ 
                DATA = strtol(argv[2], &argv[2], 16);           
                ret = WTEN(DATA);
        }else if(strcmp(argv[1], cmd_D) == 0){
                                DATA1 = strtol(argv[2], &argv[2], 16); 
                                DATA2 = strtol(argv[3], &argv[3], 16);
                                DATA3 = strtol(argv[4], &argv[4], 16);                 
                ret = WTB(DATA1,DATA2,DATA3,argv[5]);
        }else if(strcmp(argv[1], cmd_F) == 0){ 
                DATA = strtol(argv[2], &argv[2], 16);           
                ret = wt_clk(DATA);
        }else if(strcmp(argv[1], cmd_G) == 0){         
                ret = rd_clk(DATA);
        }else {
			printf("please enter a correct command\n");
            return 1;
        }
        return 1;
}

