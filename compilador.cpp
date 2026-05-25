#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

// --- PASSO 2: DEFINIÇÃO DOS TOKENS ---
enum TokenType {
    T_NUMERO_REAL, T_CARACTERE, T_APAGAR, T_NOME_ESPACO,
    T_ESPECIAL, T_UNICODE, T_FIM_ARQUIVO, 
    T_VALOR, // Qualquer identificador ou valor associado a um comando
    T_ERRO
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

// Função auxiliar para o Analisador Léxico
Token classificarToken(const std::string& word, int line) {
    if (word == "NumeroReal") return {T_NUMERO_REAL, word, line};
    if (word == "Caractere") return {T_CARACTERE, word, line};
    if (word == "Apagar") return {T_APAGAR, word, line};
    if (word == "NomeComEspaço") return {T_NOME_ESPACO, word, line};
    if (word == "Especial") return {T_ESPECIAL, word, line};
    if (word == "Unicode") return {T_UNICODE, word, line};
    if (word == "FimDoArquivo") return {T_FIM_ARQUIVO, word, line};
    
    // Se não for nenhuma palavra reservada, classificamos como um VALOR
    return {T_VALOR, word, line};
}

// --- PASSO 3: ANALISADOR SINTÁTICO ---
void analisarSintaxe(const std::vector<Token>& tokens) {
    size_t i = 0;
    
    while (i < tokens.size()) {
        Token t = tokens[i];

        if (t.type == T_FIM_ARQUIVO) {
            std::cout << "Fim do arquivo encontrado na linha " << t.line << ".\n";
            break;
        }
        
        // Pela nossa Gramática, um comando deve ser seguido por um valor
        if (t.type >= T_NUMERO_REAL && t.type <= T_UNICODE) {
            if (i + 1 < tokens.size() && tokens[i+1].type == T_VALOR) {
                std::cout << "Comando valido: '" << t.lexeme << "' com valor '" << tokens[i+1].lexeme << "' (Linha " << t.line << ")\n";
                i += 2; // Avança o comando e o valor consumidos
            } else {
                std::cerr << "[ERRO SINTATICO] Esperado um valor apos o comando '" << t.lexeme << "' na linha " << t.line << "\n";
                return;
            }
        } else {
            std::cerr << "[ERRO SINTATICO] Token inesperado ou comando solto '" << t.lexeme << "' na linha " << t.line << "\n";
            return;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <entrada>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo '" << argv[1] << "'" << std::endl;
        return 1;
    }

    std::string line;
    bool inMultiLineComment = false;
    int lineNumber = 0; 
    std::vector<Token> listaDeTokens;

    // --- PASSO 1: PRÉ-PROCESSAMENTO E LEITURA ---
    while (std::getline(inputFile, line)) {
        lineNumber++;
        std::string processedLine = "";
        
        for (size_t i = 0; i < line.length(); ++i) {
            if (inMultiLineComment) {
                if (i + 1 < line.length() && line[i] == '*' && line[i+1] == '/') {
                    inMultiLineComment = false;
                    i++; 
                }
            } else {
                if (i + 1 < line.length() && line[i] == '/' && line[i+1] == '/') {
                    break;
                } else if (i + 1 < line.length() && line[i] == '/' && line[i+1] == '*') {
                    inMultiLineComment = true;
                    i++;
                } else {
                    processedLine += line[i];
                }
            }
        }

        std::stringstream ss(processedLine);
        std::string word;
        
        // --- PASSO 2: GERAÇÃO DE TOKENS (LEXER) ---
        while (ss >> word) {
            Token novoToken = classificarToken(word, lineNumber);
            listaDeTokens.push_back(novoToken);
        }
    }
    inputFile.close();

    // Executa a validação final
    if (!listaDeTokens.empty()) {
        analisarSintaxe(listaDeTokens);
    } else {
        std::cout << "O arquivo esta vazio ou contem apenas comentarios.\n";
    }

    return 0;
}