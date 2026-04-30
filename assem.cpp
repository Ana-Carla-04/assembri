#include <iostream>
#include <string>
#include <bitset> //manipular bit
#include <fstream> //para manipulação de arquivos
#include <iomanip> //para hex,setw,setfill
using namespace std;


//função tranformar em binario
string concatenarR(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0);

//função trnasformar em hex

int main() {

	//abrir arquivo em binário
	string arquivoBin = "dadosBin.bin";

	ofstream arquivoB(arquivoBin, ios::binary);//abre em modo binario //, ios::binary||ios::out


	//abrir arquivo em hexadecimal
	string arquivoHex = "dadosHex.hex";
	ofstream arquivoH(arquivoHex);//abre um arquivo em hexadecimal

	if (!arquivoB.is_open() || !arquivoH.is_open()) { //se não abrir algum dos arquivos der erro
		cout << "Erro: Não foi possível abrir o arquivo!" << endl;
		return 1;
	}

	arquivoH << "v2.0 raw\n"; //adicona cabeçalho no hexadecimal

	string nome;
	int opCode0 = 0;
	int rs0 = 0;
	int rt0 = 0;
	int rd0 = 0 ;
	int shamt0 = 0;
	int func0 = 0;

	cin >> nome;

	//tabela R
	if (nome == "sll") {
		cin.get(); //joga fora a virgula
		cin.get(); //joga fora o $
		cin >> rd0; //pegou o valor do primeiro registrador
		cin.get(); //joga fora a virgula
		cin.get(); //joga fora o $
		cin >> rt0; //pegou o valor do segundo $
		cin.get();//jogou a virgula fora
		cin >> shamt0; //pegou o terceiro valor
		
		string instrucaoBits = concatenarR(opCode0, rs0,  rt0, rd0, shamt0,  func0); //juntar em uma string
		
		unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();//converte para numero inteiro
		
		
		// Escrever no arquivo Binário
		arquivoB << instrucaoBits; //mostrar no arquivoBin.bin
		cout << instrucaoBits;
		

		// Escrever no arquivo Hexadecimal
		arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;
			
		
	}
	if (nome == "srl") {
		cin.get(); //joga fora a virgula
		cin.get(); //joga fora o $
		cin >> rd0; //pegou o valor do primeiro registrador
		cin.get(); //joga fora a virgula
		cin.get(); //joga fora o $
		cin >> rt0; //pegou o valor do segundo $
		cin.get();//jogou a virgula fora
		cin >> shamt0; //pegou o terceiro valor
		
		string instrucaoBits = concatenarR(opCode0, rs0, rt0, rd0, shamt0, func0); //juntar em uma string

		unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();//converte para numero inteiro

		if (arquivoB.is_open()) { //se o arquivoB foi aberto
			// Escrever no arquivo Binário
			arquivoB << instrucaoBits; //mostrar no arquivoBin.bin
			cout << instrucaoBits;
		}

		if (arquivoH.is_open()) { //se o arquivoH foi aberto
			// Escrever no arquivo Hexadecimal
			arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;
			//mostrar no arquivoBin.bin
		}

	}

	if (nome == "srl") {
		cin.get(); //joga fora a virgula
		cin.get(); //joga fora o $
		cin >> rd0; //pegou o valor do primeiro registrador
		cin.get(); //joga fora a virgula
		cin.get(); //joga fora o $
		cin >> rt0; //pegou o valor do segundo $
		cin.get();//jogou a virgula fora
		cin >> shamt0; //pegou o terceiro valor

		string instrucaoBits = concatenarR(opCode0, rs0, rt0, rd0, shamt0, func0); //juntar em uma string

		unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();//converte para numero inteiro

		if (arquivoB.is_open()) { //se o arquivoB foi aberto
			// Escrever no arquivo Binário
			arquivoB << instrucaoBits; //mostrar no arquivoBin.bin
			cout << instrucaoBits;
		}

		if (arquivoH.is_open()) { //se o arquivoH foi aberto
			// Escrever no arquivo Hexadecimal
			arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;
			//mostrar no arquivoBin.bin
		}

	}

	if (nome == "mul") {
		cin.get(); //joga fora a virgula
		cin.get(); //joga fora o $
		cin >> rd0; //pegou o valor do primeiro registrador
		cin.get(); //joga fora a virgula
		cin.get(); //joga fora o $
		cin >> rs0; //pegou o valor do segundo $
		cin.get();//jogou a virgula fora
		cin >> rt0; //pegou o terceiro valor

		string instrucaoBits = concatenarR(opCode0, rs0, rt0, rd0, shamt0, func0); //juntar em uma string

		unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();//converte para numero inteiro

		if (arquivoB.is_open()) { //se o arquivoB foi aberto
			// Escrever no arquivo Binário
			arquivoB << instrucaoBits; //mostrar no arquivoBin.bin
			cout << instrucaoBits;
		}

		if (arquivoH.is_open()) { //se o arquivoH foi aberto
			// Escrever no arquivo Hexadecimal
			arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;
			//mostrar no arquivoBin.bin
		}

	}


	
	arquivoB.close();
	arquivoH.close();
}

string concatenarR(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0){
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





