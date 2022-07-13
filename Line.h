#include <iostream>
#include <string>
using namespace std;

class Line
{
public:
	int location=65536;
	
	string label=" ";
	string mnemonic="";
	string operand="";
	
	int opcode=0;
	int displacement=0;
	
	int format=3;
	string object_code;
	int obj;
	
	char opcode_bin[8+1];
	char nixbpe_bin[6+1]={'1','1','0','0','1','0','\0'};
	char dsp_bin[12+8+1]={'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
							'\0','\0','\0','\0','\0','\0','\0','\0',};
	
	void set_nixbpe()
	{
		if(operand[0]=='@')
			nixbpe_bin[1]='0'; //n=1 //i=0
			
		if(operand[0]=='#')
			nixbpe_bin[0]='0'; //n=0 //i=1
			
		if(mnemonic[0]=='+') //set e
			nixbpe_bin[5]='1';
	}
};
