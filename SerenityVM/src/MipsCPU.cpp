#include <MipsCPU.h>
#include <conio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;
MipsCPU::~MipsCPU(){}
MipsCPU::MipsCPU(): MMU( MMU_SIZE, CRTADR){
	rgf[0]=0;	//$zero
	PC=0;
	MMU.print();
}
void MipsCPU::boot(ifstream &fin)
{
	for(int i = 0; !fin.eof(); ++i){
		unsigned short byte = 0;
		byte = byte | (fin.get() << 8);
		byte = byte | (fin.get() << 0);
		MMU.sh(i,byte);
	}
}
void MipsCPU::run(){
	char c = 'r';
	while(c!='q'){		
		IR=MMU.lw(PC);
		PC+=2;						//16-bit/byte
	//R:	op:6,rs:5,rt:5,rd:5,sft:5,fun:6
	//I:	op:6,$rs:5,$rt:5,dat:16
	//J:	op:6,adr:26
		op=(IR>>26)&63;
		rs=(IR>>21)&31;
		rt=(IR>>16)&31;
		rd=(IR>>11)&31;
		sft=(IR>>6)&31;
		fun=IR & 63;
		dat=(int)(short)(IR&0xFFFF);
		adr=IR&0x3FFFFFF;

		switch(op){
		case 0:		//R-type
			switch(fun){
			case 32:	//ADD
				rgf[rd]=rgf[rs]+rgf[rt];
				operation = "ADD $rd,$rs,$rt";
				break;
			case 34:	//SUB
				operation = "SUB $rd,$rs,$rt";
				rgf[rd]=rgf[rs]-rgf[rt];
				break;
			default:
				operation = "ERROR!!!";
				cout << "\nError!\n" << endl;
				break;
			}
			break;
		case 35:	//LW
			rgf[rt]=MMU.lw(rgf[rs]+dat);
			operation = "LW rt,data(rs)";
			break;
		case 43:	//SW
			MMU.sw(rgf[rs]+dat, rgf[rt]);
			operation = "SW rt,data(rs)";
			break;
		case 33: //lh
			rgf[rt]=MMU.lh(rgf[rs]+dat);
			operation = "LH rt,data(rs)";
			break;
		case 41: //sh
			MMU.sh(rgf[rs]+dat, rgf[rt]);
			operation = "SH rt,data(rs)";
			break;
		case 4:		//BEQ
			if(rgf[rs]==rgf[rt])
				PC+=(dat<<1);
			operation = "BEQ rs,rt,data";
			break;
		case 2:		//J
			PC=(PC&0xF8000000)|(adr<<1);
			operation = "J address";
			break;
		case 8:   //addi
			rgf[rt] = rgf[rs] + dat;
			operation = "Addi rt,rs,data";
            break;
		default:
			cout << "\nError!" << endl; 
			operation = "ERROR!!!";
			break;
		}
		if(c != 'r'){
			printReg();
		}
		MMU.print();
		c = 'r';
		// c = getch();
		// printReg();
	}//end_for
}
void MipsCPU::printReg(){
		cout << endl;
		cout << operation << " opcode:" << hex << op << " rd:" << rd << " rs:" << rs << " rt:" << rt << " sft:" << sft << " data:" << dat << " address:"<< adr << endl;
		cout << "code:0x" << hex << setw(8) << setfill('0') << IR << endl;
		cout << "PC:0x" << hex << setw(8) << PC << endl;
		cout << "$zero:0x" << setw(8) << rgf[0] << "\t";
		cout << "at:0x" << setw(8) << rgf[1] << "\t";
		cout << "v0:0x" << setw(8) << rgf[2] << "\t";
		cout << "v1:0x" << setw(8) << rgf[3] << "\t" << endl;
		cout << "a0:0x" << setw(8) << rgf[4] << "\t";
		cout << "a1:0x" << setw(8) << rgf[5] << "\t";
		cout << "a2:0x" << setw(8) << rgf[6] << "\t";
		cout << "a3:0x" << setw(8) << rgf[7] << "\t" << endl;
		cout << "t0:0x" << setw(8) << rgf[8] << "\t";
		cout << "t1:0x" << setw(8) << rgf[9] << "\t";
		cout << "t2:0x" << setw(8) << rgf[10] << "\t";
		cout << "t3:0x" << setw(8) << rgf[11] << "\t" << endl;
		cout << "t4:0x" << setw(8) << rgf[12] << "\t";
		cout << "t5:0x" << setw(8) << rgf[13] << "\t";
		cout << "t6:0x" << setw(8) << rgf[14] << "\t";
		cout << "t7:0x" << setw(8) << rgf[15] << "\t" << endl;
		cout << "s0:0x" << setw(8) << rgf[16] << "\t";
		cout << "s1:0x" << setw(8) << rgf[17] << "\t";
		cout << "s2:0x" << setw(8) << rgf[18] << "\t";
		cout << "s3:0x" << setw(8) << rgf[19] << "\t" << endl;
		cout << "s4:0x" << setw(8) << rgf[20] << "\t";
		cout << "s5:0x" << setw(8) << rgf[21] << "\t";
		cout << "s6:0x" << setw(8) << rgf[22] << "\t";
		cout << "s7:0x" << setw(8) << rgf[23] << "\t" << endl;
		cout << "t8:0x" << setw(8) << rgf[24] << "\t";
		cout << "t9:0x" << setw(8) << rgf[25] << "\t";
		cout << "k0:0x" << setw(8) << rgf[26] << "\t";
		cout << "k1:0x" << setw(8) << rgf[27] << "\t" << endl;
		cout << "gp:0x" << setw(8) << rgf[28] << "\t";
		cout << "sp:0x" << setw(8) << rgf[29] << "\t";
		cout << "fp:0x" << setw(8) << rgf[30] << "\t";
		cout << "ra:0x" << setw(8) << rgf[31] << "\t" << endl;
}