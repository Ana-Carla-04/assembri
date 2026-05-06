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

// Estrutura para armazenar informacoes do label
struct LabelInfo {
    string nome;
    int linhaCodigo = 0;      // n�mero da linha no codigo (instru��o)
    int enderecoMemoria = 0;  // endere�o de memoria
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




//contadores
int sll = 0, srl = 0, jr = 0, mfhi = 0, mflo = 0, mult = 0, multu = 0, Cdiv = 0;
int divu = 0, add = 0, addu = 0, sub = 0, subu = 0, Cand = 0, Cor = 0, slt = 0, sltu = 0, mul = 0;
int beq = 0, bne = 0, addi = 0, addiu = 0, slti = 0, sltiu = 0, andi = 0, ori = 0, lui = 0, lw = 0, sw = 0, j = 0, jal = 0;

// Funcoes
string concatenarR(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0);
string concatenarI(int opCode0, int rs0, int rt0, int immediate);
string concatenarJ(int opCode0, int address);
void salvar_Intrucao_R(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0, string modo, ofstream& arquivo_gerado);
void salvar_Intrucao_I(int opCode0, int rs0, int rt0, int immediate, string modo, ofstream& arquivo_gerado);
void salvar_Intrucao_J(int opCode0, int address, string modo, ofstream& arquivo_gerado);
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

        // Remove espacos
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

int main(int argc, char* argv[]) {
    // =============================================
    // PRIMEIRA PASSAGEM: Encontrar labels
    // =============================================

    string nome_do_arquivo = argv[1];
    string nome_arquivo_gerado;
    ifstream arquivo(nome_do_arquivo);
    if (!arquivo.is_open()) {
        cout << "Erro: nao abriu o arquivo" << endl;
        return 1;
    }

    string modo = argv[2];




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
    ifstream arquivo2(nome_do_arquivo);

    if (!arquivo2.is_open()) {
        cout << "Erro: nao abriu o arquivo" << endl;
        return 1;
    }

    ofstream arquivo_gerado;

    if (modo == "-b" || modo == "-B") {
        nome_arquivo_gerado = nome_do_arquivo.substr(0, nome_do_arquivo.find_last_of('.')) + ".bin";
        arquivo_gerado.open(nome_arquivo_gerado, ios::binary);

    }
    else if (modo == "-h" || modo == "-H") {
        nome_arquivo_gerado = nome_do_arquivo.substr(0, nome_do_arquivo.find_last_of('.')) + ".hex";
        arquivo_gerado.open(nome_arquivo_gerado);
        arquivo_gerado << "v2.0 raw" << endl;

    }
    else {
        cout << "Modo de saida invalido. Use -b para binario ou -h para hexadecimal." << endl;
        return 1;
    }

    // agora sim faz sentido
    if (!arquivo_gerado.is_open()) {
        cout << "Erro: nao abriu arquivo de saida" << endl;
        return 1;
    }



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
    endereco = 0x00400000;
    // L linha por linha novamente
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

        cout << "Traduzindo: " << linhaParaProcessar << endl;

        // Aqui voce coloca a logica de traducao que ja tinha
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
                    if (instr.nome == "sll") {
                        sll++;
                    }
                    else {
                        srl++;
                    }
                    ss.get(); //joga fora o $
                    ss >> rd0; //pegou o valor do primeiro registrador
                    ss.get(); //joga fora a virgula
                    ss.get(); //joga fora o $
                    ss >> rt0; //pegou o valor do segundo $
                    ss.get();//jogou a virgula fora
                    ss >> shamt0; //pegou o terceiro valor
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, modo, arquivo_gerado);
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R

                }
                if (instr.nome == "jr") {
                    jr++;
                    ss.get(); //joga fora o $
                    ss >> rs0; //pegou o valor do primeiro registrador
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, modo, arquivo_gerado);
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
                else if (instr.nome == "mfhi" || instr.nome == "mflo") {
                    if (instr.nome == "mfhi") {
                        mfhi++;
                    }
                    else {
                        mflo++;
                    }
                    ss.get(); //joga fora o $
                    ss >> rd0; //pegou o valor do primeiro registrador
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, modo, arquivo_gerado);
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
                else if (instr.nome == "mult" || instr.nome == "multu" || instr.nome == "div" || instr.nome == "divu") {
                    if (instr.nome == "mult") {
                        mult++;
                    }
                    else if (instr.nome == "multu") {
                        multu++;
                    }
                    else if (instr.nome == "div") {
                        Cdiv++;
                    }
                    else {
                        divu++;
                    }
                    ss.get(); //joga fora o $
                    ss >> rs0; //pegou o valor do primeiro registrador
                    ss.get(); //joga fora a virgula
                    ss.get(); //joga fora o $
                    ss >> rt0; //pegou o valor do segundo $
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, modo, arquivo_gerado);
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
                else  if (instr.nome == "add" || instr.nome == "addu" || instr.nome == "sub" || instr.nome == "subu") {
                    if (instr.nome == "add") {
                        add++;
                    }
                    else if (instr.nome == "addu") {
                        addu++;
                    }
                    else if (instr.nome == "sub") {
                        sub++;
                    }
                    else {
                        subu++;
                    }
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


                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, modo, arquivo_gerado);
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
                else if (instr.nome == "and" || instr.nome == "or" || instr.nome == "slt" || instr.nome == "sltu" || instr.nome == "mul") {
                    if (instr.nome == "and") {
                        Cand++;
                    }
                    else if (instr.nome == "or") {
                        Cor++;
                    }
                    else if (instr.nome == "slt") {
                        slt++;
                    }
                    else if (instr.nome == "sltu") {
                        sltu++;
                    }
                    else {
                        mul++;
                    }
                    ss.get(); //joga fora o $
                    ss >> rd0; //pegou o valor do primeiro registrador
                    ss.get(); //joga fora a virgula
                    ss.get(); //joga fora o $
                    ss >> rs0; //pegou o valor do segundo $
                    ss.get();//jogou a virgula fora
                    ss.get(); //joga fora o $
                    ss >> rt0; //pegou o valor do terceiro
                    salvar_Intrucao_R(opCode0, rs0, rt0, rd0, shamt0, func0, modo, arquivo_gerado);
                    break; // aqui ele já salva a instrução e sai do programa, caso encontre a instrução na tabela R
                }
            }
        }
        // procura em I
        for (auto instr : tabelaI) {
            if (instr.nome == nomeInstr) {
                opCode0 = instr.opcode;
                if (instr.nome == "beq") {
                    beq++;
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
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = (it->second.enderecoMemoria - (endereco + 4)) / 4;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "bne") {
                    bne++;
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
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = (it->second.enderecoMemoria - (endereco + 4)) / 4;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }


                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "addi") {
                    addi++;
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
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = (it->second.enderecoMemoria - (endereco + 4)) / 4;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "addiu") {
                    addiu++;
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
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = (it->second.enderecoMemoria - (endereco + 4)) / 4;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "slti") {
                    slti++;
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
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = (it->second.enderecoMemoria - (endereco + 4)) / 4;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "sltiu") {
                    sltiu++;
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
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = (it->second.enderecoMemoria - (endereco + 4)) / 4;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "andi") {
                    andi++;
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
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = (it->second.enderecoMemoria - (endereco + 4)) / 4;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "ori") {
                    ori++;
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
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    else {
                        limpar(immediate1);
                        auto it = tabela.find(immediate1);
                        if (it != tabela.end()) {
                            immediate0 = (it->second.enderecoMemoria - (endereco + 4)) / 4;
                        }
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }

                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "lui") {
                    lui++;
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
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    else {
                        auto it = tabela.find(immediate1);

                        if (it != tabela.end()) {
                            immediate0 = (it->second.enderecoMemoria - (endereco + 4)) / 4;
                        }

                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    break;// ja achou sua instrução vaza daqui
                }
                else if (instr.nome == "lw" || instr.nome == "sw") {
                    if (instr.nome == "lw") {
                        lw++;
                    }
                    else {
                        sw++;
                    }
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
                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
                    }
                    else {
                        auto it = tabela.find(immediate1);

                        if (it != tabela.end()) {
                            immediate0 = (it->second.enderecoMemoria - (endereco + 4)) / 4;
                        }

                        salvar_Intrucao_I(opCode0, rs0, rt0, immediate0, modo, arquivo_gerado);
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
                    j++;
                    string destino;
                    ss >> destino;

                    auto it = tabela.find(destino);

                    if (it != tabela.end()) {
                        address0 = it->second.enderecoMemoria >> 2;
                    }
                    salvar_Intrucao_J(opCode0, address0, modo, arquivo_gerado);
                    break;

                }
                else if (instr.nome == "jal") {
                    jal++;
                    string destino;
                    ss >> destino;

                    auto it = tabela.find(destino);

                    if (it != tabela.end()) {
                        address0 = it->second.enderecoMemoria >> 2;
                    }
                    salvar_Intrucao_J(opCode0, address0, modo, arquivo_gerado);
                    break;
                }
            }
        }
        rs0 = rt0 = rd0 = shamt0 = immediate0 = address0 = 0;
        endereco += 4;
    }

    // =============================================
    // LEITURA DO ARQUIVO CSV DE CICLOS
    // =============================================


    // Vetor para armazenar pares (nome, quantidade) - APENAS INSTRUÇÕES QUE APARECERAM
    vector<pair<string, int>> vetorQuantidades;

    // Adicionar apenas instruções que apareceram (quantidade > 0)
    if (add > 0) vetorQuantidades.push_back({ "add", add });
    if (sub > 0) vetorQuantidades.push_back({ "sub", sub });
    if (addu > 0) vetorQuantidades.push_back({ "addu", addu });
    if (subu > 0) vetorQuantidades.push_back({ "subu", subu });
    if (addi > 0) vetorQuantidades.push_back({ "addi", addi });
    if (addiu > 0) vetorQuantidades.push_back({ "addiu", addiu });
    if (beq > 0) vetorQuantidades.push_back({ "beq", beq });
    if (bne > 0) vetorQuantidades.push_back({ "bne", bne });
    if (lw > 0) vetorQuantidades.push_back({ "lw", lw });
    if (sw > 0) vetorQuantidades.push_back({ "sw", sw });
    if (j > 0) vetorQuantidades.push_back({ "j", j });
    if (jal > 0) vetorQuantidades.push_back({ "jal", jal });
    if (mult > 0) vetorQuantidades.push_back({ "mult", mult });
    if (multu > 0) vetorQuantidades.push_back({ "multu", multu });
    if (Cdiv > 0) vetorQuantidades.push_back({ "div", Cdiv });
    if (divu > 0) vetorQuantidades.push_back({ "divu", divu });
    if (mul > 0) vetorQuantidades.push_back({ "mul", mul });
    if (Cand > 0) vetorQuantidades.push_back({ "and", Cand });
    if (Cor > 0) vetorQuantidades.push_back({ "or", Cor });
    if (slt > 0) vetorQuantidades.push_back({ "slt", slt });
    if (sltu > 0) vetorQuantidades.push_back({ "sltu", sltu });
    if (slti > 0) vetorQuantidades.push_back({ "slti", slti });
    if (sltiu > 0) vetorQuantidades.push_back({ "sltiu", sltiu });
    if (andi > 0) vetorQuantidades.push_back({ "andi", andi });
    if (ori > 0) vetorQuantidades.push_back({ "ori", ori });
    if (lui > 0) vetorQuantidades.push_back({ "lui", lui });
    if (sll > 0) vetorQuantidades.push_back({ "sll", sll });
    if (srl > 0) vetorQuantidades.push_back({ "srl", srl });
    if (jr > 0) vetorQuantidades.push_back({ "jr", jr });
    if (mfhi > 0) vetorQuantidades.push_back({ "mfhi", mfhi });
    if (mflo > 0) vetorQuantidades.push_back({ "mflo", mflo });

    // Ler o arquivo CSV
    ifstream csv("infos.csv");
    if (!csv.is_open()) {
        cout << "Erro: Não foi possível abrir o arquivo ciclos.csv" << endl;
    }
    else {
        string linha;
        int totalCiclos = 0;

        cout << "\n=== CALCULO DE CICLOS ===" << endl << endl;
        cout << "Instrucao | quantidade" << endl;
        cout << "-------------------" << endl;

        int c = 0;
        while (getline(csv, linha)) {
            if (linha.empty()) continue;

            stringstream ss(linha);
            string nomeCSV;
            int ciclos;

            getline(ss, nomeCSV, ',');  // Lê até a vírgula
            ss >> ciclos;                // Lê o número

            // Procurar no vetor de quantidades
            for (const auto& quant : vetorQuantidades) {
                if (quant.first == nomeCSV) {
                    int resultado = quant.second * ciclos;
                    totalCiclos += resultado;

                    cout << left << setw(10) << nomeCSV << " | "
                        << setw(10) << quant.second << endl;
                    c = c + resultado;
                    break;
                }
            }
        }

        int totalInstr = 0;
        for (auto& q : vetorQuantidades)
            totalInstr += q.second;

        cout << endl << "CPI: " << (float)c / totalInstr;




    }
    arquivo2.close();
    arquivo_gerado.close();

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

void salvar_Intrucao_R(int opCode0, int rs0, int rt0, int rd0, int shamt0, int func0, string modo, ofstream& arquivo_gerado) {

    string instrucaoBits = concatenarR(opCode0, rs0, rt0, rd0, shamt0, func0);
    unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();
    if (modo == "b" || modo == "B") {
        if (arquivo_gerado.is_open()) {
            arquivo_gerado << instrucaoBits << endl; // Adicionei endl
        }
    }
    else if (modo == "h" || modo == "H") {
        if (arquivo_gerado.is_open()) {
            arquivo_gerado << "0x" << hex << uppercase << instrucaoInt << dec << endl;
        }
    }
}

void salvar_Intrucao_I(int opCode0, int rs0, int rt0, int immediate, string modo, ofstream& arquivo_gerado) {
    string instrucaoBits = concatenarI(opCode0, rs0, rt0, immediate);
    unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();

    if (modo == "b" || modo == "B") {
        if (arquivo_gerado.is_open()) {
            arquivo_gerado << instrucaoBits << endl; // Adicionei endl
        }
    }
    else if (modo == "h" || modo == "H") {
        if (arquivo_gerado.is_open()) {
            arquivo_gerado << "0x" << hex << uppercase << instrucaoInt << dec << endl;
        }
    }
}

void salvar_Intrucao_J(int opCode0, int address, string modo, ofstream& arquivo_gerado) {
    string instrucaoBits = concatenarJ(opCode0, address);
    unsigned int instrucaoInt = bitset<32>(instrucaoBits).to_ulong();

    if (modo == "b" || modo == "B") {
        if (arquivo_gerado.is_open()) {
            arquivo_gerado << instrucaoBits << endl; // Adicionei endl
        }
    }
    else if (modo == "h" || modo == "H") {
        if (arquivo_gerado.is_open()) {
            arquivo_gerado << "0x" << hex << uppercase << instrucaoInt << dec << endl;
        }
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
    if (s.empty()) return false;
    int i = 0;
    if (s[0] == '-') i = 1;

    for (; i < s.size(); i++) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}