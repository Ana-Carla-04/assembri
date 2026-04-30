#include <iostream>
#include <string>
#include <vector>
#include <bitset> //manipular bit
#include <fstream> //para manipula��o de arquivos
#include <iomanip> //para hex,setw,setfill
using namespace std;

/*
lou aqui, tenta deixar o mais claro oq eu tentei fazer no codigo
minha ideia foi criar uma base de dados com as instruções R, I e J, onde cada instrução tem o nome, o opcode e a informação principal da instrução.
pra ser mais facil de entender, eu criei uma struct para cada tipo de instrução, onde cada struct tem os campos correspondentes a cada tipo de instrução,
depois disso eu criei um vetor para cada tipo de instrução, onde cada vetor é preenchido com as informações das instruções R, I e J, dessa forma fica mais
fácil de procurar a instrução e pegar as informações necessárias para montar a instrução em binário e em hexadecimal.
*/

struct InstrucaoR {
	string nome;
	int opcode;
	int funct;
};

// tabela R
// instrução, opcod, funct
vector<InstrucaoR> tabelaR = {
	{"add", 0, 32},
	{"addu", 0, 33},
	{"sub", 0, 34},
	{"and", 0, 36},
	{"or",  0, 37},
	{"nor", 0, 39},
	{"slt", 0, 42},
	{"sltu", 0, 43},
	{"sll", 0, 0},
	{"srl", 0, 2},
	{"jr",  0, 8},
	{"mfhi", 0, 16},
	{"mflo", 0, 18},
	{"mult", 0, 24},
	{"multu", 0, 25},
	{"div", 0, 26},
	{"divu", 0, 27},
	{"subu", 0, 35},
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

//fun��o tranformar em binario
string concatenarR(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0);

//fun��o tranformar em hex

int main() {

	//abrir arquivo em bin�rio
	string arquivoBin = "dadosBin.bin";

	ofstream arquivoB(arquivoBin, ios::binary);//abre em modo binario //, ios::binary||ios::out

	if (!arquivoB.is_open()) {
		return 1;
	}

	// while (arquivoT.getline)

	//abrir arquivo em hexadecimal
	string arquivoHex = "dadosHex.hex";
	ofstream arquivoH(arquivoHex);//abre um arquivo em hexadecimal
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


	cin >> nome;
	// lou aqui, não implementei no codigo pra vc ver oq eu 
	// fiz primeiro e vc decidir ser vai implementar ou n, 
	// mas a ideia é essa, a gente vai ler o nome da //
	// instrução e depois procurar na tabela R, I e J, caso
	// encontre a instrução na tabela R, ele vai pegar o 
	// opcode e o funct e os valores dos registradores e do 
	// shamt, caso encontre na tabela I ele vai pegar o 
	// opcode e os valores dos registradores 
	// e do immediate, caso encontre na tabela J ele vai
	// pegar o opcode e o valor do address, depois disso ele 
	// vai salvar a instrução no arquivo binário e 
	// no arquivo hexadecimal

	// procura em R
	for (auto instr : tabelaR) {
		// caso encontre a instrução, ele vai pegar o opcode e o funct e os valores dos registradores e do shamt
		if (instr.nome == nome) { 
			opCode0 = instr.opcode;
			func0 = instr.funct;
			if (instr.nome == "jr") {
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do primeiro registrador
				salvar_Intrução_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoBin, arquivoHex);
				return; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
			}
			else if (instr.nome == "mfhi" || instr.nome == "mflo") {
				cin.get(); //joga fora o $
				cin >> rd0; //pegou o valor do primeiro registrador
				salvar_Intrução_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoBin, arquivoHex);
				return; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
			}
			else if (instr.nome == "mult" || instr.nome == "multu" || instr.nome == "div" || instr.nome == "divu") {
				cin.get(); //joga fora o $
				cin >> rs0; //pegou o valor do primeiro registrador
				cin.get(); //joga fora a virgula
				cin.get(); //joga fora o $
				cin >> rt0; //pegou o valor do segundo $
				salvar_Intrução_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoBin, arquivoHex);
				return; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
			}

			cin.get(); //joga fora a virgula
			cin.get(); //joga fora o $
			cin >> rd0; //pegou o valor do primeiro registrador
			cin.get(); //joga fora a virgula
			cin.get(); //joga fora o $
			cin >> rt0; //pegou o valor do segundo $
			cin.get();//jogou a virgula fora
			cin >> shamt0; //pegou o terceiro valor
			salvar_Intrução_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoBin, arquivoHex);
			return; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
		}
	}

	// procura em I
	for (auto instr : tabelaI) {
		if (instr.nome == nome) {
			opCode0 = instr.opcode;
			cin.get(); //joga fora a virgula
			cin.get(); //joga fora o $
			cin >> rd0; //pegou o valor do primeiro registrador
			cin.get(); //joga fora a virgula
			cin.get(); //joga fora o $
			cin >> rt0; //pegou o valor do segundo $
			cin.get();//jogou a virgula fora
			cin >> immediate0; //pegou o terceiro valor
			salvar_Intrução_I(opCode0, rs0, rt0, immediate0, arquivoBin, arquivoHex);
			return;
		}
	}

	// procura em J
	for (auto instr : tabelaJ) {
		if (instr.nome == nome) {
			opCode0 = instr.opcode;
			cin >> address0; //pegou o endereço
			salvar_Intrução_J(opCode0, address0, arquivoBin, arquivoHex);
			return;
		}
	}

	//cout << "Aqui: " << opCode<<" "<< rd<<" "<<rt<<" "<<sa;
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



// lou aqui, aproveitei oq vc já tinha feito, as funções e a ideia que vc já tinha desenvolvido, só que para cada tipo de instrução, (R, I e J), cada função vai
// receber os valores dos campos da instrução e os arquivos para salvar a instrução em binário e em hexadecimal, dentro da função a gente vai concatenar os campos da instrução em uma string de 32 bits,
// depois converter essa string para um número inteiro e salvar esse número inteiro no arquivo binário e no arquivo hexadecimal, a vantagem de criar uma
// função para cada tipo de instrução é que fica mais organizado e fácil de entender o código, além disso, caso a gente queira adicionar mais instruções no futuro,
// basta adicionar mais casos nas funções correspondentes.

void salvar_Intrução_R(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0, string arquivoB, string arquivoH) {
	string instrucaoBits = concatenarR(opCode0, rs0, rt0, rd0, shamt0, func0); //juntar em uma string

	unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();//converte para numero inteiro

	if (arquivoB.is_open()) { //se o arquivoB foi aberto
		// Escrever no arquivo BIN�RIO
		//arquivoB.write(reinterpret_cast<char*>(&instrucaoInt), sizeof(instrucaoInt));

		// arquivoB << instrucaoBits; //mostrar no arquivoBin.bin
		// em vez de transformar a string em inteiro e depois escrever, podemos escrever diretamente a string no arquivo binário
		arquivoB.write(reinterpret_cast<char*>(&instrucaoInt), sizeof(instrucaoInt)); // aqui estamos escrevendo o inteiro no arquivo binário
		cout << instrucaoBits;
	}

	if (arquivoH.is_open()) { //se o arquivoH foi aberto
		// Escrever no arquivo BIN�RIO
		arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;
		//mostrar no arquivoBin.bin
	}
}


string concatenarI(int opCode0, int rs0, int rt0, int immediate) {
	bitset<6> opCode(opCode0);
	bitset<5> rs(rs0);
	bitset<5> rt(rt0);
	bitset<16> imme(immediate);

	return opCode.to_string() + rs.to_string() +
		rt.to_string() + imme.to_string()
}

void salvar_Intrução_I(int opCode0, int rs0, int rt0, int immediate, string arquivoB, string arquivoH) {
	string instrucaoBits = concatenarI(opCode0, rs0, rt0, immediate); //juntar em uma string

	unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();//converte para numero inteiro

	if (arquivoB.is_open()) { //se o arquivoB foi aberto
		// Escrever no arquivo BIN�RIO
		//arquivoB.write(reinterpret_cast<char*>(&instrucaoInt), sizeof(instrucaoInt));

		// arquivoB << instrucaoBits; //mostrar no arquivoBin.bin
		// em vez de transformar a string em inteiro e depois escrever, podemos escrever diretamente a string no arquivo binário
		arquivoB.write(reinterpret_cast<char*>(&instrucaoInt), sizeof(instrucaoInt)); // aqui estamos escrevendo o inteiro no arquivo binário
		cout << instrucaoBits;
	}

	if (arquivoH.is_open()) { //se o arquivoH foi aberto
		// Escrever no arquivo BIN�RIO
		arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;
		//mostrar no arquivoBin.bin
	}
}


string concatenarJ(int opCode0, int address) {
	bitset<6> opCode(opCode0);
	bitset<26> add(address);

	return opCode.to_string() + add.to_string()
}

void salvar_Intrução_J(int opCode0, int address, string arquivoB, string arquivoH) {
	string instrucaoBits = concatenarL(opCode0, address); //juntar em uma string

	unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();//converte para numero inteiro

	if (arquivoB.is_open()) { //se o arquivoB foi aberto
		// Escrever no arquivo BIN�RIO
		//arquivoB.write(reinterpret_cast<char*>(&instrucaoInt), sizeof(instrucaoInt));

		// arquivoB << instrucaoBits; //mostrar no arquivoBin.bin
		// em vez de transformar a string em inteiro e depois escrever, podemos escrever diretamente a string no arquivo binário
		arquivoB.write(reinterpret_cast<char*>(&instrucaoInt), sizeof(instrucaoInt)); // aqui estamos escrevendo o inteiro no arquivo binário
		cout << instrucaoBits;
	}

	if (arquivoH.is_open()) { //se o arquivoH foi aberto
		// Escrever no arquivo BIN�RIO
		arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;
		//mostrar no arquivoBin.bin
	}

