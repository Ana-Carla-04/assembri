#include <iostream>
#include <string>
#include <vector>
#include <bitset> //manipular bit
#include <fstream> //para manipulacao de arquivos
#include <iomanip> //para hex,setw,setfill
#include <map>
#include <sstream> // PARA STRINGSTREAM
#include <cctype> // para isdigit
using namespace std;

// Estrutura para armazenar informa��es do label
struct LabelInfo {
    string nome;
    int linhaCodigo = 0;      // n�mero da linha no c�digo (instru��o)
    int enderecoMemoria = 0;  // endere�o de mem�ria
};

struct InstrucaoR {
    string nome;
    int opcode;
    int funct;
};
struct Registrador {
    string nome;
    int numero;
};

vector<Registrador> registradores = {
    {"$zero",0},

    {"$v0",2},
    {"$v1",3},

    {"$a0",4},
    {"$a1",5},
    {"$a2",6},
    {"$a3",7},

    {"$t0",8},
    {"$t1",9},
    {"$t2",10},
    {"$t3",11},
    {"$t4",12},
    {"$t5",13},
    {"$t6",14},
    {"$t7",15},

    {"$s0",16},
    {"$s1",17},
    {"$s2",18},
    {"$s3",19},
    {"$s4",20},
    {"$s5",21},
    {"$s6",22},
    {"$s7",23},

    {"$t8",24},
    {"$t9",25}
};

// tabela R
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

// Funcoes
string concatenarR(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0);
string concatenarI(int opCode0, int rs0, int rt0, int immediate);
string concatenarJ(int opCode0, int address);
void salvar_Intrucao_R(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0, ofstream& arquivoB, ofstream& arquivoH);
void salvar_Intrucao_I(int opCode0, int rs0, int rt0, int immediate, ofstream& arquivoB, ofstream& arquivoH);
void salvar_Intrucao_J(int opCode0, int address, ofstream& arquivoB, ofstream& arquivoH);
void processarInstrucao(string linha, ofstream& arquivoB, ofstream& arquivoH, map<string, LabelInfo>& tabela, int& enderecoAtual);
int lerRegistrador(string reg);
bool ehNumero(string s);

// =============================================
// FUNCAO PARA LIMPAR LINHA (REMOVER COMENTARIOS)
// =============================================
string limparLinha(string linha) {
    // Procura por # e corta a partir dele
    size_t pos = linha.find('#');
    if (pos != string::npos) {
        linha = linha.substr(0, pos);
    }

    // Remove espa�os do inicio e fim
    size_t inicio = linha.find_first_not_of(" \t");
    if (inicio == string::npos) {
        return ""; // linha vazia
    }
    size_t fim = linha.find_last_not_of(" \t");

    return linha.substr(inicio, fim - inicio + 1);
}

// =============================================
// FUNCAO PARA EXTRAIR LABEL
// =============================================
bool extrairLabel(string linha, string& nomeLabel, string& resto) {
    size_t pos = linha.find(':');

    if (pos != string::npos) {
        // Pega o nome do label
        nomeLabel = linha.substr(0, pos);

        // Remove espa�os
        size_t inicio = nomeLabel.find_first_not_of(" \t");
        size_t fim = nomeLabel.find_last_not_of(" \t");
        if (inicio != string::npos && fim != string::npos) {
            nomeLabel = nomeLabel.substr(inicio, fim - inicio + 1);
        }

        // Pega o resto (instrucao depois do label)
        resto = linha.substr(pos + 1);
        inicio = resto.find_first_not_of(" \t");
        if (inicio != string::npos) {
            resto = resto.substr(inicio);
        }
        else {
            resto = "";
        }

        return true;
    }

    resto = linha;
    return false;
}

int main() {
    // =============================================
    // PRIMEIRA PASSAGEM: Encontrar labels
    // =============================================

    ifstream arquivo("teste.asm");
    if (!arquivo.is_open()) {
        cout << "Erro: nao abriu o arquivo" << endl;
        return 1;
    }

    // Tabela de simbolos
    map<string, LabelInfo> tabela;
    

    string linha;
    int numeroInstrucao = 0;
    int endereco = 0x00400000;

    cout << "=== PRIMEIRA PASSAGEM: ENCONTRANDO LABELS ===" << endl;

    // PRIMEIRA PASSAGEM: Apenas encontrar labels e contar instrucoes
    while (getline(arquivo, linha)) {
        string linhaLimpa = limparLinha(linha);

        if (linhaLimpa.empty()) {
            continue; // Pula linha vazia ou comentario
        }

        string nomeLabel, restoLinha;
        bool temLabel = extrairLabel(linhaLimpa, nomeLabel, restoLinha);

        if (temLabel) {
            // Guarda o label na tabela
            LabelInfo info;
            info.nome = nomeLabel;
            info.linhaCodigo = numeroInstrucao;
            info.enderecoMemoria = endereco;
            tabela[nomeLabel] = info;

            cout << "Label: " << nomeLabel << " | Linha: " << numeroInstrucao
                << " | Endereco: 0x" << hex << endereco << dec << endl;
            // Se tem instrucao depois do label, conta
            if (!restoLinha.empty()) {
                numeroInstrucao++;
                endereco += 4;
            }
        }
        else {
            // Nao tem label, so instrucao
            cout << "Instrucao linha " << numeroInstrucao << ": " << linhaLimpa << endl;
            numeroInstrucao++;
            endereco += 4;
        }
    }

    arquivo.close();

    // Mostra a tabela de simbolos
    cout << "\n=== TABELA DE SIMBOLOS ===" << endl;
    for (auto& item : tabela) {
        cout << "[" << item.first << "] linha: " << item.second.linhaCodigo
            << " end: 0x" << hex << item.second.enderecoMemoria << dec << endl;
    }

    cout << "\nTotal de instrucoes: " << numeroInstrucao << endl;

    // =============================================
    // SEGUNDA PASSAGEM: Traduzir instrucoes
    // =============================================

    // Reabre o arquivo para a segunda passagem
    ifstream arquivo2("teste.asm");
    if (!arquivo2.is_open()) {
        cout << "Erro: nao abriu o arquivo na segunda passagem" << endl;
        return 1;
    }

    // Abre arquivos de saida
    string arquivoBin = "dadosBin.bin";
    ofstream arquivoB(arquivoBin, ios::binary);

    string arquivoHex = "dadosHex.hex";
    ofstream arquivoH(arquivoHex);

    if (!arquivoB.is_open() || !arquivoH.is_open()) {
        cout << "Erro: nao abriu arquivos de saida" << endl;
        return 1;
    }

    arquivoH << "v2.0 raw" << endl;

    // VARIAVEIS PARA TRADUCAO
    string nome;
    int opCode0 = 0;
    int rs0 = 0;
    int rt0 = 0;
    int rd0 = 0;
    int shamt0 = 0;
    int func0 = 0;

    int immediate0 = 0;
    int address0 = 0;
    cout << "\n=== SEGUNDA PASSAGEM: TRADUZINDO INSTRUCOES ===" << endl;

    // L� linha por linha novamente
    while (getline(arquivo2, linha)) {
        string linhaLimpa = limparLinha(linha);

        if (linhaLimpa.empty()) {
            continue;
        }

        string nomeLabel, instrucao;
        bool temLabel = extrairLabel(linhaLimpa, nomeLabel, instrucao);

        // Se tem label mas nao tem instrucao, pula
        if (temLabel && instrucao.empty()) {
            continue;
        }

        // Se tem label e instrucao, usa a instrucao
        string linhaParaProcessar = temLabel ? instrucao : linhaLimpa;

        cout << "Traduzindo[" << endereco << "]: " << linhaParaProcessar << endl;

        // Aqui voc� coloca a logica de traducao que ja tinha
        // Mas ao inves de usar ss, usa stringstream para ler da string

        stringstream ss(linhaParaProcessar);
        string nomeInstr;
        ss >> nomeInstr; // Pega o nome da instrucao

        // procura em R
        for (auto instr : tabelaR) {
            // caso encontre a instrução, ele vai pegar o opcode e o funct e os valores dos registradores e do shamt
            if (instr.nome == nomeInstr) {
                opCode0 = instr.opcode;
                func0 = instr.funct;
                if (instr.nome == "sll" || instr.nome == "srl") {
                    ss.get(); //joga fora o $
                    ss >> rd0; //pegou o valor do primeiro registrador
                    ss.get(); //joga fora a virgula
                    ss.get(); //joga fora o $
                    ss >> rt0; //pegou o valor do segundo $
                    ss.get();//jogou a virgula fora
                    ss >> shamt0; //pegou o terceiro valor
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoB, arquivoH);
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
                if (instr.nome == "jr") {
                    ss.get(); //joga fora o $
                    ss >> rs0; //pegou o valor do primeiro registrador
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoB, arquivoH);
                    arquivoB.close();                   
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
                else if (instr.nome == "mfhi" || instr.nome == "mflo") {
                    ss.get(); //joga fora o $
                    ss >> rd0; //pegou o valor do primeiro registrador
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoB, arquivoH);                                        
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
                else if (instr.nome == "mult" || instr.nome == "multu" || instr.nome == "div" || instr.nome == "divu") {
                    ss.get(); //joga fora o $
                    ss >> rs0; //pegou o valor do primeiro registrador
                    ss.get(); //joga fora a virgula
                    ss.get(); //joga fora o $
                    ss >> rt0; //pegou o valor do segundo $
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoB, arquivoH);                                       
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
                else  if (instr.nome == "add" || instr.nome == "addu" || instr.nome == "sub" || instr.nome == "subu") {
                    string rdStr, rsStr, rtStr;

                    getline(ss, rdStr, ',');
                    getline(ss, rsStr, ',');
                    getline(ss, rtStr);

                    auto limpar = [](string& s) {
                        s.erase(0, s.find_first_not_of(" \t\r\n"));
                        s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };

                    limpar(rdStr);
                    limpar(rsStr);
                    limpar(rtStr);

                    rd0 = lerRegistrador(rdStr);
                    rs0 = lerRegistrador(rsStr);
                    rt0 = lerRegistrador(rtStr);

                    
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoB, arquivoH);
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
                else if (instr.nome == "and" || instr.nome == "or" || instr.nome == "slt" || instr.nome == "sltu" || instr.nome == "mul") {
                    ss.get(); //joga fora o $
                    ss >> rd0; //pegou o valor do primeiro registrador
                    ss.get(); //joga fora a virgula
                    ss.get(); //joga fora o $
                    ss >> rs0; //pegou o valor do segundo $
                    ss.get();//jogou a virgula fora
                    ss.get(); //joga fora o $
                    ss >> rt0; //pegou o valor do terceiro
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, arquivoB, arquivoH);
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
            }
        }
        // procura em I
        for (auto instr : tabelaI) {
            if (instr.nome == nomeInstr) {
                opCode0 = instr.opcode;
                if (instr.nome == "beq") {
                    string rsStr, rtStr, immediate1;

                    getline(ss, rsStr, ',');
                    getline(ss, rtStr, ',');
                    getline(ss, immediate1);
                    auto limpar = [](string& s) {
                        s.erase(0, s.find_first_not_of(" \t\r\n"));
                        s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };

                    limpar(rsStr);
                    limpar(rtStr);
                    rs0 = lerRegistrador(rsStr);
                    rt0 = lerRegistrador(rtStr);

                    if (ehNumero(immediate1)) {
                        immediate0 = stoi(immediate1); // Se for um número, converte diretamente
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = it->second.enderecoMemoria;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "bne") {
                    string rsStr, rtStr, immediate1;

                    getline(ss, rsStr, ',');
                    getline(ss, rtStr, ',');
                    getline(ss, immediate1);
                    auto limpar = [](string& s) {
                        s.erase(0, s.find_first_not_of(" \t\r\n"));
                        s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };

                    limpar(rsStr);
                    limpar(rtStr);
                    rs0 = lerRegistrador(rsStr);
                    rt0 = lerRegistrador(rtStr);

                    if (ehNumero(immediate1)) {
                        immediate0 = stoi(immediate1); // Se for um número, converte diretamente
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = it->second.enderecoMemoria;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }


                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "addi") {
                    string rsStr, rtStr, immediate1;

                    getline(ss, rsStr, ',');
                    getline(ss, rtStr, ',');
                    getline(ss, immediate1);
                    auto limpar = [](string& s) {
                        s.erase(0, s.find_first_not_of(" \t\r\n"));
                        s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };

                    limpar(rsStr);
                    limpar(rtStr);
                    rs0 = lerRegistrador(rsStr);
                    rt0 = lerRegistrador(rtStr);

                    if (ehNumero(immediate1)) {
                        immediate0 = stoi(immediate1); // Se for um número, converte diretamente
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = it->second.enderecoMemoria;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "addiu") {
                    string rsStr, rtStr, immediate1;

                    getline(ss, rsStr, ',');
                    getline(ss, rtStr, ',');
                    getline(ss, immediate1);
                    auto limpar = [](string& s) {
                        s.erase(0, s.find_first_not_of(" \t\r\n"));
                        s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };

                    limpar(rsStr);
                    limpar(rtStr);
                    rs0 = lerRegistrador(rsStr);
                    rt0 = lerRegistrador(rtStr);

                    if (ehNumero(immediate1)) {
                        immediate0 = stoi(immediate1); // Se for um número, converte diretamente
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = it->second.enderecoMemoria;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "sltiu") {
                    string rsStr, rtStr, immediate1;

                    getline(ss, rsStr, ',');
                    getline(ss, rtStr, ',');
                    getline(ss, immediate1);
                    auto limpar = [](string& s) {
                        s.erase(0, s.find_first_not_of(" \t\r\n"));
                        s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };

                    limpar(rsStr);
                    limpar(rtStr);
                    rs0 = lerRegistrador(rsStr);
                    rt0 = lerRegistrador(rtStr);

                    if (ehNumero(immediate1)) {
                        immediate0 = stoi(immediate1); // Se for um número, converte diretamente
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = it->second.enderecoMemoria;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "andi") {
                    string rsStr, rtStr, immediate1;

                    getline(ss, rsStr, ',');
                    getline(ss, rtStr, ',');
                    getline(ss, immediate1);
                    auto limpar = [](string& s) {
                        s.erase(0, s.find_first_not_of(" \t\r\n"));
                        s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };

                    limpar(rsStr);
                    limpar(rtStr);
                    rs0 = lerRegistrador(rsStr);
                    rt0 = lerRegistrador(rtStr);

                    if (ehNumero(immediate1)) {
                        immediate0 = stoi(immediate1); // Se for um número, converte diretamente
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = it->second.enderecoMemoria;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "ori") {
                    string rsStr, rtStr, immediate1;

                    getline(ss, rsStr, ',');
                    getline(ss, rtStr, ',');
                    getline(ss, immediate1);
                    auto limpar = [](string& s) {
                        s.erase(0, s.find_first_not_of(" \t\r\n"));
                        s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };

                    limpar(rsStr);
                    limpar(rtStr);
                    limpar(immediate1);
                    rs0 = lerRegistrador(rsStr);
                    rt0 = lerRegistrador(rtStr);

                    if (ehNumero(immediate1)) {
                        immediate0 = stoi(immediate1); // Se for um número, converte diretamente
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = it->second.enderecoMemoria;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "lui") {
                    string rsStr, rtStr, immediate1;

                    getline(ss, rtStr, ',');
                    getline(ss, immediate1);
                    auto limpar = [](string& s) {
                        s.erase(0, s.find_first_not_of(" \t\r\n"));
                        s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };
                    limpar(rtStr);
                    limpar(immediate1);
                    rt0 = lerRegistrador(rtStr);
                    if (ehNumero(immediate1)) {
                        rt0 = lerRegistrador(rtStr);
                        immediate0 = stoi(immediate1); // Se for um número, converte diretamente
                        salvar_Intrucao_I(opCode0, rs0 = 0, rt0, immediate0, arquivoB, arquivoH);
                    }
                    else {
                        auto it = tabela.find(immediate1);

                        if (it != tabela.end()) {
                            immediate0 = it->second.enderecoMemoria;
                        }
                        
                        salvar_Intrucao_I(opCode0, rs0 = 0, rt0, immediate0, arquivoB, arquivoH);
                    }
                }
                else if (instr.nome == "lw" || instr.nome == "sw") {
                    string rtStr, resto;
                    int offset;

                    getline(ss, rtStr, ',');
                    getline(ss, resto);

                    auto limpar = [](string& s) {
                        s.erase(0, s.find_first_not_of(" \t\r\n"));
                        s.erase(s.find_last_not_of(" \t\r\n") + 1);
                        };

                    limpar(rtStr);
                    limpar(resto);

                    // separa offset e registrador
                    size_t abre = resto.find('(');
                    size_t fecha = resto.find(')');

                    string immediate1 = resto.substr(0, abre);
                    string rsStr = resto.substr(abre + 1, fecha - abre - 1);

                    limpar(immediate1);
                    limpar(rsStr);

                    rt0 = lerRegistrador(rtStr);
                    rs0 = lerRegistrador(rsStr);

                    if (ehNumero(immediate1)) {
                        rt0 = lerRegistrador(rtStr);
                        immediate0 = stoi(immediate1); // Se for um número, converte diretamente
                        salvar_Intrucao_I(opCode0, rs0 = 0, rt0, immediate0, arquivoB, arquivoH);
                    }
                    else {
                        auto it = tabela.find(immediate1);

                        if (it != tabela.end()) {
                            immediate0 = it->second.enderecoMemoria;
                        }

                        salvar_Intrucao_I(opCode0, rs0 = 0, rt0, immediate0, arquivoB, arquivoH);
                    }              


                    break;
                }
            }
        }
		for (auto instr : tabela) {
            if (instr.first == nomeInstr) {
                break;
            }
        }

        // procura em J
        for (auto instr : tabelaJ) {
            if (instr.nome == nomeInstr) {
                opCode0 = instr.opcode;
                if (instr.nome == "j") {
                    string destino;
                    ss >> destino;

                    auto it = tabela.find(destino);

                    if (it != tabela.end()) {
                        address0 = it->second.enderecoMemoria >> 2;
                    }
                    salvar_Intrucao_J(opCode0, address0, arquivoB, arquivoH);                    
                    break;

                }
                else if (instr.nome == "jal") {
                    string destino;
                    ss >> destino;

                    auto it = tabela.find(destino);

                    if (it != tabela.end()) {
                        address0 = it->second.enderecoMemoria >> 2;
                    }
                    salvar_Intrucao_J(opCode0, address0, arquivoB, arquivoH);
                    break;
                }
            }
        }
    }
        arquivo2.close();
        arquivoB.close();
        arquivoH.close();
        cout << "\nArquivos gerados com sucesso!" << endl;
    }

// =============================================
// IMPLEMENTACOES DAS FUNCOES (suas funcoes originais)
// =============================================

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
    
    string instrucaoBits = concatenarR(opCode0, rs0, rt0, rd0, shamt0, func0);

    if (arquivoB.is_open()) {
        arquivoB << instrucaoBits << endl; // Adicionei endl
    }
    unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();
    if (arquivoH.is_open()) {
        arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;
    }
}

void salvar_Intrucao_I(int opCode0, int rs0, int rt0, int immediate, ofstream& arquivoB, ofstream& arquivoH) {
    string instrucaoBits = concatenarI(opCode0, rs0, rt0, immediate);

    if (arquivoB.is_open()) {
        arquivoB << instrucaoBits << endl;
    }
    unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();

    if (arquivoH.is_open()) {
        arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;
    }
}

void salvar_Intrucao_J(int opCode0, int address, ofstream& arquivoB, ofstream& arquivoH) {
    string instrucaoBits = concatenarJ(opCode0, address);

    if (arquivoB.is_open()) {
        arquivoB << instrucaoBits << endl;
    }
    unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();

    if (arquivoH.is_open()) {
        arquivoH << "0x" << hex << uppercase << instrucaoInt << dec << endl;
    }
}
int lerRegistrador(string reg) {
    if (reg.back() == ',')
        reg.pop_back();

    for (const auto& r : registradores) {
        if (r.nome == reg) {
            return r.numero;
        }
    }
    return -1;
}
bool ehNumero(string s) {
    for (char c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}