#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "Line.h"
#include "Symbol.h"
using namespace std;

Line start_line;
Line lines[59];  //there are 59 lines in the source program
Symbol SYMTAB[58];
string INSTRUCTION_SET[59*2]={"ADD","ADDF","ADDR","AND","CLEAR",//5
	"COMP","COMPF","COMPR","DIV","DIVF","DIVR","FIX","FLOAT",//8
	"HIO","J","JEQ","JGT","JLT","JSUB","LDA","LDB","LDCH","LDF",//10
	"LDL","LDS","LDT","LDX","LPS","MUL","MULF","MULR","NORM","OR",//10
	"RD","RMO","RSUB","SHIFTL","SHIFTR","SIO","SSK","STA","STB","STCH",//10
	"STF","STI","STL","STS","STSW","STT","STX","SUB","SUBF","SUBR","SVC",//11
	"TD","TIO","TIX","TIXR","WD",//5
	
	"+ADD","+ADDF","+ADDR","+AND","+CLEAR",//5
	"+COMP","+COMPF","+COMPR","+DIV","+DIVF","+DIVR","+FIX","+FLOAT",//8
	"+HIO","+J","+JEQ","+JGT","+JLT","+JSUB","+LDA","+LDB","+LDCH","+LDF",//10
	"+LDL","+LDS","+LDT","+LDX","+LPS","+MUL","+MULF","+MULR","+NORM","+OR",//10
	"+RD","+RMO","+RSUB","+SHIFTL","+SHIFTR","+SIO","+SSK","+STA","+STB","+STCH",//10
	"+STF","+STI","+STL","+STS","+STSW","+STT","+STX","+SUB","+SUBF","+SUBR","+SVC",//11
	"+TD","+TIO","+TIX","+TIXR","+WD"};//5
int OPTAB[59*2]={0x18,0x58,0x90,0x40,0xB4,0x28,0x88,0xA0,
				0x24,0x64,0x9C,0xC4,0xC0,0xF4,0x3C,0x30,
				0x34,0x38,0x48,0x00,0x68,0x50,0x70,0x08,
				0x6C,0x74,0x04,0xD0,0x20,0x60,0x98,0xC8,
				0x44,0xD8,0xAC,0x4C,0xA4,0xA8,0xF0,0xEC,
				0x0C,0x78,0x54,0x80,0xD4,0x14,0x7C,0xE8,
				0x84,0x10,0x1C,0x5C,0x94,0xB0,0xE0,0xF8,
				0x2C,0xB8,0xDC,
						
				0x18,0x58,0x90,0x40,0xB4,0x28,0x88,0xA0,
				0x24,0x64,0x9C,0xC4,0xC0,0xF4,0x3C,0x30,
				0x34,0x38,0x48,0x00,0x68,0x50,0x70,0x08,
				0x6C,0x74,0x04,0xD0,0x20,0x60,0x98,0xC8,
				0x44,0xD8,0xAC,0x4C,0xA4,0xA8,0xF0,0xEC,
				0x0C,0x78,0x54,0x80,0xD4,0x14,0x7C,0xE8,
				0x84,0x10,0x1C,0x5C,0x94,0xB0,0xE0,0xF8,
				0x2C,0xB8,0xDC};
string FORMAT1[6]={"FIX","FLOAT","HIO","NORM","SIO","TIO",};
string FORMAT2[11]={"ADDR","CLEAR","COMPR","DIVR","MULR","RMO","SHIFTL","SHIFTR","SUBR","SVC","TIXR"};
string DIRECTIVES1[6]={"WORD","RESW","RESB","BYTE","EQU","CSECT"};//Loc YES
string DIRECTIVES2[4]={"EXTDEF","EXTREF","LTORG","END"};//Loc NO
int LOCCTR=0;
int SYMCTR=0;
string current_SECT="default";
fstream source("figure2.15.txt",ios::in);
fstream symtab("SYMTAB.txt",ios::out);
char reg_1[5];
char reg_2[5];


void op_int2bin(int n, char *ps) 
{
	int size = 8;
	int i = size -1;
	
	while(i+1) 
	{
		ps[i--] = (1 & n) + '0';
		n >>= 1;
	}
	
	ps[size] = '\0';
	//return ps;
}

void dsp_int2bin(int n, char *ps,int format) 
{
	int size = 12;
	
	if(format==2)
		size = 4;
	else if(format==3)
		size = 12;
	else if(format==4)
		size = 20;
		
	int i = size -1;
	
	while(i+1) 
	{
		ps[i--] = (1 & n) + '0';
		n >>= 1;
	}
	
	ps[size] = '\0';
	//return ps;
}

int hex_to_int(string hex_str)
{
	stringstream ss;
	int i;
	ss << std::hex << hex_str;
	ss >> i;
	
	return i;
}

vector<string>  split(const string& str,const string& delim) 
{
	vector<string> res;
	if("" == str) return  res;
	
	string strs = str + delim; 
	size_t pos;
	size_t size = strs.size();
 
	for (int i = 0; i < size; ++i) 
	{
		pos = strs.find(delim, i); 
		if( pos < size) 
		{ 
			string s = strs.substr(i, pos - i);
			res.push_back(s);
			i = pos + delim.size() - 1;
		}
	}
	return res;	
}

int get_format(string term)
{
	if(term[0]=='+')
		return 4;
	
	for(int i=0;i<6;i++)
		if(term==FORMAT1[i])
			return 1;
			
	for(int i=0;i<11;i++)
		if(term==FORMAT2[i])
			return 2;
			
	return 3;
}

int search_symbol(string symbol)
{
	for(int i=0;i<58;i++)
		if(SYMTAB[i].symbol==symbol)
			return i+1;
	
	return 777;
}

int search_SYMTAB(string symbol)
{
	for(int i=0;i<58;i++)
		if(SYMTAB[i].symbol==symbol)
			return SYMTAB[i].location;
	
	return 777;
}

int find_symbol(string symbol)
{
	for(int i=0;i<58;i++)
		if(SYMTAB[i].symbol==symbol)
			return 1;
	
	return 0;
}

int is_directive(string term)
{
	for(int i=0;i<6;i++)
		if(term==DIRECTIVES1[i])
			return 1;
	
	for(int i=0;i<4;i++)
		if(term==DIRECTIVES2[i])
			return 2;
			
	return 777;
}

int is_valid(string term)
{
	for(int i=0;i<59*2;i++)
		if(term==INSTRUCTION_SET[i])
			return 1;
	
	return 0;
} 

int get_opcode(string term)
{
	for(int i=0;i<59*2;i++)
		if(term==INSTRUCTION_SET[i])
			return OPTAB[i];
	
	return 777;
}

void add_SYMTAB(string label,int LOCCTR,string section)
{
	SYMTAB[SYMCTR].symbol=label;
	SYMTAB[SYMCTR].location=LOCCTR;
	SYMTAB[SYMCTR].section=section;
	SYMCTR++;
}

void pass1() 
{
	//READ START
	string start_line_str;
	getline(source,start_line_str);
	stringstream start_ss(start_line_str);
	start_ss >> start_line.label;
	start_ss >> start_line.mnemonic;
	start_ss >> start_line.operand;
	
	//SET STARTING ADDRESS
	if(start_line.mnemonic=="START")
	{
		LOCCTR=stoi(start_line.operand);
		start_line.location=stoi(start_line.operand);
	}

//////////////////////////////////////////////////////////////////////////////////
	
	//READ EACH LINE
	for(int i=0;i<59;i++)
	{
		string tmp_str;
		getline(source,tmp_str);
		stringstream tmp_ss(tmp_str);
		
		if(tmp_str[0]=='\t' || tmp_str[1]==' ')
		{
			//lines[i].label="";
			tmp_ss >> lines[i].mnemonic;
			tmp_ss >> lines[i].operand;
		}
		else
		{
			tmp_ss >> lines[i].label;
			tmp_ss >> lines[i].mnemonic;
			tmp_ss >> lines[i].operand;
		}
		
//////////////////////////////////////////////	
		
		string comment=".";
		if(lines[i].label!=comment)
		{
			//PROCESS SYMBOL
			if(lines[i].label!=" " && search_symbol(lines[i].label)!=777)
			{
				if(SYMTAB[search_symbol(lines[i].label)-1].section=="EXT")
				{
					SYMTAB[search_symbol(lines[i].label)-1].section=current_SECT;
					SYMTAB[search_symbol(lines[i].label)-1].location=LOCCTR;
				}
				else
					cout << "ERROR: duplicate symbol." << lines[i].label << " " << lines[i].mnemonic << endl;
			}
							
			if(lines[i].label!=" " && search_symbol(lines[i].label)==777)
				add_SYMTAB(lines[i].label,LOCCTR,current_SECT);
			
			
			
			//PROCESS OP
			if(is_valid(lines[i].mnemonic))
			{
				lines[i].location=LOCCTR;
				
				switch(get_format(lines[i].mnemonic))
				{
					case 1: LOCCTR=LOCCTR+1; break;
					case 2: LOCCTR=LOCCTR+2; break;
					case 3: LOCCTR=LOCCTR+3; break;
					case 4: LOCCTR=LOCCTR+4; break;
					default: cout << "Unknown error." << endl;
				}
			}
			else if(lines[i].mnemonic=="WORD")
			{
				lines[i].location=LOCCTR;
				LOCCTR=LOCCTR+3;
			}
			else if(lines[i].mnemonic=="RESW")
			{
				lines[i].location=LOCCTR;
				LOCCTR=LOCCTR+3*stoi(lines[i].operand);
			}
			else if(lines[i].mnemonic=="RESB")
			{
				lines[i].location=LOCCTR;
				LOCCTR=LOCCTR+stoi(lines[i].operand);
			}
			else if(lines[i].mnemonic=="BYTE")
			{
				lines[i].location=LOCCTR;
				LOCCTR=LOCCTR+1;//lines[i].operand.size()-3;
			}
			else if(lines[i].mnemonic=="CSECT")
			{
				LOCCTR=0;
				lines[i].location=LOCCTR;
				SYMTAB[SYMCTR-1].location=LOCCTR;
				SYMTAB[SYMCTR-1].section=lines[i].label;
				current_SECT=lines[i].label;
			}
			else if(lines[i].mnemonic=="EQU" || lines[i].mnemonic[0]=='=')
			{
				lines[i].location=LOCCTR;
				LOCCTR=LOCCTR+3;
			}
			else if(lines[i].mnemonic=="EXTDEF")
			{
				lines[i].location=65537;
				
				string comma=",";
				vector<string> res=split(lines[i].operand,comma);
				for(int i=0;i<res.size();i++)
				{
					if(search_symbol(res[i])==777)
						add_SYMTAB(res[i],NULL,"EXT");
				}
			}
			else if(lines[i].mnemonic=="EXTREF")
			{
				lines[i].location=65537;
			}
			else if(lines[i].mnemonic=="LTORG")
			{
				lines[i].location=65537;
			}	
			else if(lines[i].mnemonic=="END")
			{
				lines[i].location=65537;
			}
		}
		else
		{
			lines[i].location=65536;
		}
	} //end for 
	
	add_SYMTAB("A",0,"REG"); //A=0
	add_SYMTAB("X",1,"REG"); //X=1
	add_SYMTAB("L",2,"REG"); //L=2
	add_SYMTAB("PC",8,"REG"); //PC=1
	add_SYMTAB("SW",9,"REG"); //SW=9
	add_SYMTAB("B",3,"REG"); //B=3
	add_SYMTAB("S",4,"REG"); //S=4
	add_SYMTAB("T",5,"REG"); //T=5
	add_SYMTAB("F",6,"REG"); //F=6
	
///////////////////////////////////////////////////////////////////////////////////	
	/*
	cout << "Loc" << "\t" << "Source statement" << "\t\t" << endl;
	cout << "---------------------------------------------------" << endl;
	printf("%04X",start_line.location);
	cout << "\t" << start_line.label << "\t" << start_line.mnemonic << "\t" << start_line.operand << endl;
	
	for(int i=0;i<59;i++)
	{
		if(lines[i].location==65536)
			continue;
		
		if(lines[i].location==65537 || lines[i].label==" ")
			cout << "    ";
		else
			printf("%04X",lines[i].location);
		
		interm << lines[i].location << "\t" << lines[i].label << "\t" << lines[i].mnemonic << "\t" << lines[i].operand << endl;
		cout << "\t" << lines[i].label << "\t" << lines[i].mnemonic << "\t" << lines[i].operand << endl;
	}
	
	symtab << "SYMBOL" << "\t" << "Loc" << "\t" << "Section" << endl;
	symtab << "---------------------------------" <<endl; 
	for(int i=0;i<SYMCTR;i++)
	{	
		symtab << SYMTAB[i].symbol << "\t" << SYMTAB[i].location << "\t" << SYMTAB[i].section << endl;
	}
		
	source.close();
	symtab.close();
	interm.close();*/
	//return 0;
}

void pass2()
{
	current_SECT="default";
	
	for(int i=0;i<58;i++)
	{
		if(lines[i].mnemonic=="CSECT")
			current_SECT=lines[i].label;
		
		string comment=".";
		if(lines[i].label!=comment)
		{
			lines[i].opcode=get_opcode(lines[i].mnemonic);
			if(lines[i].opcode!=777)
			{
				if(!lines[i].operand.empty())
				{
					if(search_symbol(lines[i].operand)!=777)
					{
						if(current_SECT==SYMTAB[search_symbol(lines[i].operand)-1].section)
							lines[i].displacement=search_SYMTAB(lines[i].operand);
						else
							lines[i].displacement=0;
					}
					else if(lines[i].operand[1]=='#')
					{
						lines[i].displacement=stoi(lines[i].operand.substr(1,1));
					}
					else if(lines[i].operand[1]=='@')
					{
						lines[i].displacement=search_SYMTAB(lines[i].operand.substr(1,6));
					}
					else
						cout << "undefined symbol: " << lines[i].label << " " << lines[i].mnemonic << " " << lines[i].operand << endl;
				}
				else
					lines[i].displacement=0;	
			}
			else if(lines[i].mnemonic=="BYTE")
			{
				string hex_str=lines[i].operand.substr(2,lines[i].operand.size()-3);
				lines[i].displacement=hex_to_int(hex_str);
			}
			else if(lines[i].mnemonic=="WORD")
			{
				lines[i].displacement=0;  //NEED MODIFICATION
			}
			
			
			
			// ASSEMBLE HERE //
				lines[i].format=get_format(lines[i].mnemonic);
				/*string comma=",";
				vector<string> res=split(lines[i].operand,comma);*/
				
				op_int2bin(lines[i].opcode,lines[i].opcode_bin);
				lines[i].set_nixbpe();
				dsp_int2bin(lines[i].displacement,lines[i].dsp_bin,3);
				
				string str_op(lines[i].opcode_bin);
				string str_nixbpe(lines[i].nixbpe_bin);
				string str_dsp(lines[i].dsp_bin);
				
				//concatnation
				lines[i].object_code=str_op.substr(0,6)+str_nixbpe+str_dsp;
				lines[i].obj=stoi(lines[i].object_code,0,2);
			// ASSEMBLE HERE //
		}
	}
}

int main() 
{
	pass1();
	pass2();
	
	cout << "Loc" << "\t" << "Source statement" << "\t\t" << endl;
	cout << "---------------------------------------------------" << endl;
	printf("%04X",start_line.location);
	cout << "\t" << start_line.label << "\t" << start_line.mnemonic << "\t" << start_line.operand << endl;
	
	for(int i=0;i<59;i++)
	{
		if(lines[i].location==65536)
			continue;
		
		if(lines[i].location==65537)
			cout << "    ";
		else
			printf("%04X",lines[i].location);
		
		
		cout << "\t" << lines[i].label << "\t" << lines[i].mnemonic << "\t" << lines[i].operand;
		if(is_directive(lines[i].mnemonic)==777)
			printf("\t\t\t%X\n",lines[i].obj);
		else if(lines[i].mnemonic=="BYTE")
			printf("\t\t\t%X\n",lines[i].displacement);
		else
			printf("\n");
	}
	
	symtab << "SYMBOL" << "\t" << "Loc" << "\t" << "Section" << endl;
	symtab << "---------------------------------" <<endl; 
	for(int i=0;i<SYMCTR;i++)
	{	
		symtab << SYMTAB[i].symbol << "\t" << SYMTAB[i].location << "\t" << SYMTAB[i].section << endl;
	}
	
		
	source.close();
	symtab.close();
	
	return 0;
}
