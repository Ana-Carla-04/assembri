#include <iostream>
#include <string>
#include <vector>
#include <bitset> //manipular bit
#include <fstream> //para manipulacao de arquivos
#include <iomanip> //para hex,setw,setfill
using namespace std;


struct InstrucaoR {
	string nome;
	int opcode;
	int funct;
};

// tabela R
// instrução, opcod, funct
vector<InstrucaoR> tabelaR = {
	{"sll", 0, 0},
	{"srl", 0, 2},
	{"jr",  0, 8},
	{"mfhi", 0, 16},
	{"mflo", 0, 18},
	{"mult", 0, 24},
	{"multu", 0, 25},
	{"div", 0, 26},
	{"divu", 0, 27},
	{"add", 0, 32},
	{"addu", 0, 33},
	{"sub", 0, 34},
	{"subu", 0, 35},
	{"and", 0, 36},
	{"or",  0, 37},
	{"slt", 0, 42},
	{"sltu", 0, 43},
	{"mul", 28, 2}
};

struct InstrucaoI {
	string nome;
	int opcode;
};

vector<InstrucaoI> tabelaI = {
	{"beq", 4},
	{"bne", 5},
	{"addi", 8},
	{"addiu", 9},
	{"slti", 10},
	{"sltiu", 11},
	{"andi", 12},
	{"ori", 13},
	{"lui", 15},
	{"lw", 35},
	{"sw", 43}
};

struct InstrucaoJ {
	string nome;
	int opcode;
};

vector<InstrucaoJ> tabelaJ = {
	{"j", 2},
	{"jal", 3}
};

//funcao tranformar em binario
string concatenarR(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0);
string concatenarI(int opCode0, int rs0, int rt0, int immediate);
string concatenarJ(int opCode0, int address);
void salvar_Intrucao_R(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0, ofstream& arquivoB, ofstream& arquivoH);
void salvar_Intrucao_I(int opCode0, int rs0, int rt0, int immediate, ofstream& arquivoB, ofstream& arquivoH);
void salvar_Intrucao_J(int opCode0, int address, ofstream& arquivoB, ofstream& arquivoH);

//funcao tranformar em hex

int main() {

	//abrir arquivo em binario
	string arquivoBin = "dadosBin.bin";

	ofstream arquivoB(arquivoBin, ios::binary);//abre em modo binario //, ios::binary||ios::out


	//abrir arquivo em hexadecimal
	string arquivoHex = "dadosHex.hex";
	ofstream arquivoH(arquivoHex);//abre um arquivo em hexadecimal

	if (!arquivoB.is_open()) {
		return 1;
	}
	if (!arquivoH.is_open()) {
		return 1;
	}
	arquivoH << "v2.0 raw" << endl;


	string nome;
	int opCode0 = 0;
	int rs0 = 0;
	int rt0 = 0;
	int rd0 = 0;
	int shamt0 = 0;
	int func0 = 0;
	// variáveis para tipo I
	int immediate0 = 0;
	// variáveis para tipo J
	int address0 = 0;


	cin >> nome; //nome da instrução



	// procura em R
	for (auto instr : tabelaR) {
		// caso encontre a instrução, ele vai pegar o opcode e o funct e os valores dos registradores e do shamt
		if (instr.nome == nome) {
			opCode0 = instr.opcode;
			func0 = instr.funct;
			if (instr.nome == "jr") {
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do primeiro registrador
				salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
			}
			else if (instr.nome == "mfhi" || instr.nome == "mflo") {
				cin.get(); //joga fora o $
				cin >> rd0; //pegou o valor do primeiro registrador
				salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
			}
			else if (instr.nome == "mult" || instr.nome == "multu" || instr.nome == "div" || instr.nome == "divu") {
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do primeiro registrador
				cin.get(); //joga fora a virgula
				cin.get(); //joga fora o $
				cin >> rt0; //pegou o valor do segundo $
				salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
			}
			cin.get(); //joga fora o $
			cin >> rd0; //pegou o valor do primeiro registrador
			cin.get(); //joga fora a virgula
			cin.get(); //joga fora o $
			cin >> rt0; //pegou o valor do segundo $
			cin.get();//jogou a virgula fora
			cin >> shamt0; //pegou o terceiro valor
			salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoB, arquivoH);
			arquivoB.close();
			arquivoH.close();
			return 0; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
		}
	}

	// procura em I

	for (auto instr : tabelaI) {
		if (instr.nome == nome) {
			opCode0 = instr.opcode;
			if (instr.nome == "beq") {
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do primeiro registrador
				cin.get(); //joga fora a virgula
				cin.get(); //joga fora o $
				cin >> rt0; //pegou o valor do segundo $
				cin.get();//jogou a virgula fora
				cin >> immediate0; //pegou o terceiro valor
				salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;// ja achou sua instrução vaza daqui
			}
			else if (instr.nome == "bne") {
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do primeiro registrador
				cin.get(); //joga fora a virgula
				cin.get(); //joga fora o $
				cin >> rt0; //pegou o valor do segundo $
				cin.get();//jogou a virgula fora
				cin >> immediate0; //pegou o terceiro valor
				salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;// ja achou sua instrução vaza daqui
			}
			else if (instr.nome == "addi") {
				cin.get(); //joga fora o $
				cin >> rt0; //pegou o valor do primeiro registrador
				cin.get(); //joga fora a virgula
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do segundo $
				cin.get();//jogou a virgula fora
				cin >> immediate0; //pegou o terceiro valor
				salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;// ja achou sua instrução vaza daqui
			}
			else if (instr.nome == "addiu") {
				cin.get(); //joga fora o $
				cin >> rt0; //pegou o valor do primeiro registrador
				cin.get(); //joga fora a virgula
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do segundo $
				cin.get();//jogou a virgula fora
				cin >> immediate0; //pegou o terceiro valor
				salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;// ja achou sua instrução vaza daqui
			}
			else if (instr.nome == "sltiu") {
				cin.get(); //joga fora o $
				cin >> rt0; //pegou o valor do primeiro registrador
				cin.get(); //joga fora a virgula
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do segundo $
				cin.get();//jogou a virgula fora
				cin >> immediate0; //pegou o terceiro valor
				salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;// ja achou sua instrução vaza daqui
			}
			else if (instr.nome == "andi") {
				cin.get(); //joga fora o $
				cin >> rt0; //pegou o valor do primeiro registrador
				cin.get(); //joga fora a virgula
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do segundo $
				cin.get();//jogou a virgula fora
				cin >> immediate0; //pegou o terceiro valor
				salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;// ja achou sua instrução vaza daqui
			}
			else if (instr.nome == "ori") {
				cin.get(); //joga fora o $
				cin >> rt0; //pegou o valor do primeiro registrador
				cin.get(); //joga fora a virgula
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do segundo $
				cin.get();//jogou a virgula fora
				cin >> immediate0; //pegou o terceiro valor
				salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;// ja achou sua instrução vaza daqui
			}
			else if (instr.nome == "lui") {
				cin.get(); //joga fora o $
				cin >> rt0; //pegou o valor do primeiro registrador
				cin.get(); //joga fora a virgula
				cin >> rs0; //pegou o valor do segundo $
				cin.get();//jogou a virgula fora
				cin >> immediate0; //pegou o terceiro valor
				salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;// ja achou sua instrução vaza daqui
			}
			else if (instr.nome == "lw" || instr.nome == "sw") {
				cin.get();//joga fora o $
				cin >> rt0;//pegou o valor de rt
				cin.get(); //jogou fora a virgula
				cin >> immediate0;//pegou o valor de immediat
				cin.get();// jogou fora o (
				cin.get(); //jogou fora o $
				cin >> rs0; //pegou o valor de rs
				cin.get();//jogou fora o parentese
				salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;
			}


		}
	}

	// procura em J
	for (auto instr : tabelaJ) {
		if (instr.nome == nome) {
			opCode0 = instr.opcode;
			if (instr.nome == "j") {
				cin >> address0; //pegou o endereço
				salvar_Intrucao_J(opCode0, address0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;

			}
			else if (instr.nome == "jal") {
				cin >> address0; //pegou o endereço
				salvar_Intrucao_J(opCode0, address0, arquivoB, arquivoH);
				arquivoB.close();
				arquivoH.close();
				return 0;

			}
		}
	}

	arquivoB.close();
	arquivoH.close();
}

string concatenarR(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0) {
	bitset<6> opCode(opCode0);
	bitset<5> rs(rs0);
	bitset<5> rt(rt0);
	bitset<5> rd(rd0);
	bitset<5> shamt(shamt0);
	bitset<6> funct(func0);

	return opCode.to_string() + rs.to_string() +
		rt.to_string() + rd.to_string() +
		shamt.to_string() + funct.to_string();
}

string concatenarI(int opCode0, int rs0, int rt0, int immediate) {
	bitset<6> opCode(opCode0);
	bitset<5> rs(rs0);
	bitset<5> rt(rt0);
	bitset<16> imme(immediate);

	return opCode.to_string() + rs.to_string() +
		rt.to_string() + imme.to_string();
}

string concatenarJ(int opCode0, int address) {
	bitset<6> opCode(opCode0);
	bitset<26> add(address);

	return opCode.to_string() + add.to_string();
}

void salvar_Intrucao_R(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0, ofstream& arquivoB, ofstream& arquivoH) {
	string instrucaoBits = concatenarR(opCode0, rs0, rt0, rd0, shamt0, func0); //juntar em uma string

	unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();//converte para numero inteiro

	if (arquivoB.is_open()) { //se o arquivoB foi aberto
		//colocando os valor em binario no arquivo
		arquivoB << instrucaoBits;
	}

	if (arquivoH.is_open()) { //se o arquivoH foi aberto
		// Escrever no arquivo Hexadecimal
		arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;

	}
}

void salvar_Intrucao_I(int opCode0, int rs0, int rt0, int immediate, ofstream& arquivoB, ofstream& arquivoH) {
	string instrucaoBits = concatenarI(opCode0, rs0, rt0, immediate); //juntar em uma string

	unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();//converte para numero inteiro

	if (arquivoB.is_open()) { //se o arquivoB foi aberto
		//colocando os valor em binario no arquivo
		arquivoB << instrucaoBits;
	}

	if (arquivoH.is_open()) { //se o arquivoH foi aberto
		// Escrever no arquivo Hexadecimal
		arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;

	}
}

void salvar_Intrucao_J(int opCode0, int address, ofstream& arquivoB, ofstream& arquivoH) {
	string instrucaoBits = concatenarJ(opCode0, address); //juntar em uma string

	unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();//converte para numero inteiro

	if (arquivoB.is_open()) { //se o arquivoB foi aberto
		//colocando os valor em binario no arquivo
		arquivoB << instrucaoBits;
	}

	if (arquivoH.is_open()) { //se o arquivoH foi aberto
		// Escrever no arquivo Hexadecimal
		arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;

	}


}
