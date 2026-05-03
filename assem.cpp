#include <iostream>
#include <string>
#include <vector>
#include <bitset> //manipular bit
#include <fstream> //para manipulacao de arquivos
#include <iomanip> //para hex,setw,setfill
#include <map>
#include <sstream> // PARA STRINGSTREAM
using namespace std;

// Estrutura para armazenar informações do label
struct LabelInfo {
    string nome;
    int linhaCodigo;      // número da linha no código (instrução)
    int enderecoMemoria;  // endereço de memória
};

struct InstrucaoR {
    string nome;
    int opcode;
    int funct;
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

// =============================================
// FUNCAO PARA LIMPAR LINHA (REMOVER COMENTARIOS)
// =============================================
string limparLinha(string linha) {
    // Procura por # e corta a partir dele
    size_t pos = linha.find('#');
    if (pos != string::npos) {
        linha = linha.substr(0, pos);
    }

    // Remove espaços do inicio e fim
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

        // Remove espaços
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
    int opCode0, rs0, rt0, rd0, shamt0, func0;
    int immediate0, address0;

    // Resetando o endereco para a segunda passagem
    endereco = 0x00400000;
    numeroInstrucao = 0;

    cout << "\n=== SEGUNDA PASSAGEM: TRADUZINDO INSTRUCOES ===" << endl;

    // Lê linha por linha novamente
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

        // Aqui você coloca a logica de traducao que ja tinha
        // Mas ao inves de usar cin, usa stringstream para ler da string

        stringstream ss(linhaParaProcessar);
        string nomeInstr;
        ss >> nomeInstr; // Pega o nome da instrucao

        // Exemplo: se for add (formato R)
        if (nomeInstr == "add") {
            // Pula o $
            char c;
            ss >> c; // le o $
            ss >> rs0; // le o primeiro registrador
            ss >> c; // le a virgula
            ss >> c; // le o $
            ss >> rt0; // le o segundo registrador
            ss >> c; // le a virgula
            ss >> c; // le o $
            ss >> rd0; // le o terceiro registrador

            // Salva instrucao
            opCode0 = 0;
            func0 = 32;
            salvar_Intrucao_R(opCode0, rs0, rt0, rd0, 0, func0, arquivoB, arquivoH);
        }
        else if (nomeInstr == "addi") {
            char c;
            ss >> c; // $
            ss >> rt0; // destino
            ss >> c; // ,
            ss >> c; // $
            ss >> rs0; // origem
            ss >> c; // ,
            ss >> immediate0; // imediato

            opCode0 = 8;
            salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
        }
        else if (nomeInstr == "beq") {
            char c;
            string labelNome;

            ss >> c; // $
            ss >> rs0; // registro 1
            ss >> c; // ,
            ss >> c; // $
            ss >> rt0; // registro 2
            ss >> c; // ,
            ss >> labelNome; // nome do label

            // Calcula o offset usando a tabela de simbolos
            int enderecoLabel = tabela[labelNome].enderecoMemoria;
            int pcAtual = endereco + 4; // PC aponta para proxima instrucao
            int offset = (enderecoLabel - pcAtual) / 4;

            // Immediate de 16 bits (pode ser negativo)
            immediate0 = offset;

            opCode0 = 4;
            salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, arquivoB, arquivoH);
        }
        else if (nomeInstr == "j") {
            string labelNome;
            ss >> labelNome;

            // Calcula o endereco do jump
            int enderecoLabel = tabela[labelNome].enderecoMemoria;
            address0 = enderecoLabel / 4;

            opCode0 = 2;
            salvar_Intrucao_J(opCode0, address0, arquivoB, arquivoH);
        }
        else {
            cout << "Instrucao nao reconhecida: " << nomeInstr << endl;
        }

        // Atualiza endereco e contador
        numeroInstrucao++;
        endereco += 4;
    }

    arquivo2.close();
    arquivoB.close();
    arquivoH.close();

    cout << "\nArquivos gerados com sucesso!" << endl;

    return 0;
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