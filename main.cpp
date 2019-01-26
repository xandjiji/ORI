#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <string>
#include <vector>
#include <algorithm>
#include <vector>


/*
---> FEITO:
	registros sem ordenacao
	registros de tamanho fixo (100 bytes)
	insercao
	busca de um registro (por uma chave)
	remocao de um registro (por uma chave) (LOGICA)
	listagem dos registros
	compactacao do arquivo
	criacao de um arquivo vazio

	
---> NAO FOI POSSIVEL FAZER:
	controle de blocos (cada bloco possui um numero inteiro de registros)
	blocos possuem 512 bytes (5 registros + 12 bytes)
*/

/*
	Os registros tem o seguinte formato:
	CHAVE[0,9]NOME[0,20]IDADE[0,2]
	
	O tamanho dos campos sao fixos e todo espaco que sobrar sera completado com '@'. Exemplo:
	123@@@@@@Juninho Oliveira@@@@13
	
	Registros com '@' no primeiro caractere é removido logicamente. Exemplo:
	@23@@@@@@Juninho Oliveira@@@@13	
*/



using namespace std;

//tamanho dos campos em bytes
const int chave_tam = 9;
const int nome_tam = 20;
const int idade_tam = 2;

//index da posicao de cada campo na linha
const int chave_index = 0;                      //0
const int nome_index = chave_index + chave_tam; //9
const int idade_index = nome_index + nome_tam;  //29

//tamanho de cada registro
const int registro_tam = chave_tam + nome_tam + idade_tam + 2; //33 bytes, pois no final sera acrescentado '\n'

//classe Pessoa
class Pessoa {
public:
     int chave;
     string nome;
     int idade;
};




//funcao para pegar uma string e carregar seus dados em um objeto Pessoa
void stringToPessoa(string linha, Pessoa& p){
	//a string sera separada em 3 strings (uma para cada campo)
	string chavestr; int chaveint;	//essa string sera convertida para int
	string nomestr;
	string idadestr; int idadeint;	//essa string sera convertida para int
	
	//pegando os dados da string de acordo com seus index
	chavestr = linha.substr(chave_index, chave_tam);
    nomestr = linha.substr(nome_index, nome_tam);
    idadestr = linha.substr(idade_index, idade_tam);
	
	//removendo as ocorrencias de '@' que sao usadas para completar os campos vazios
	chavestr.erase(remove(chavestr.begin(), chavestr.end(), '@'), chavestr.end());
	nomestr.erase(remove(nomestr.begin(), nomestr.end(), '@'), nomestr.end());
	idadestr.erase(remove(idadestr.begin(), idadestr.end(), '@'), idadestr.end());
	
	//convertendo tipos strings para tipo int
	chaveint = stoi(chavestr);
	idadeint = stoi(idadestr);
	
	//carregando os dados nos atributos do objeto Pessoa
	p.chave = chaveint;
	p.nome = nomestr;
	p.idade = idadeint;
}


//funcao para completar os espacos vazios dos campos de um objeto Pessoa com '@' (retorna uma string)
string pessoaToString(Pessoa p){
	string linha;
	string aux;
	int tamanho;
	
	
	//CHAVE
	aux = to_string(p.chave);
	tamanho = aux.length();
	tamanho = chave_tam - tamanho;
	
	for(int i = 0; i < tamanho; i++){
		aux = aux + "@";
	}
	
	linha = linha + aux;
	
	
	//NOME
	aux = p.nome;
	tamanho = aux.length();
	tamanho = nome_tam - tamanho;
	
	for(int i = 0; i < tamanho; i++){
		aux = aux + "@";
	}
	
	linha = linha + aux;
	
	//IDADE
	aux = to_string(p.idade);
	tamanho = aux.length();
	tamanho = idade_tam - tamanho;
	
	for(int i = 0; i < tamanho; i++){
		aux = aux + "@";
	}
	
	linha = linha + aux;
	
	
	return linha;
}


//cria um arquivo .dat com o nome desejado
void criaArquivo(string nome){
	nome = nome + ".dat";
	
	ofstream novoarquivo;
	novoarquivo.open(nome);
	novoarquivo.close();
}


//funcao que retorna o numero de registros em um dado arquivo
int numReg(istream& entrada){
	int tamanho = 0;
	string linha;
	
	while(getline(entrada, linha, '\n')){
		
		//IF para identificar remocao logica ('@' no primeiro caractere de cada registro)
		if(linha[0] != '@'){
			tamanho++;
		}
	}

	//volta o ponteiro do arquivo pro inicio
	entrada.clear();
	entrada.seekg(0);
	
	
	return tamanho;
}


//funcao que retorna o RRN da primeira ocorrencia de um registro com a chave correspondente (em caso de falha retorna -1)
int buscaPessoa(int chave, istream& entrada){
	int RRN = -1;
	string linha;
	Pessoa p;
	bool achou = false;
	
	//while que percorre todos os registros do arquivo procurando a primeira ocorrencia que corresponde a uma dada chave
	while(getline(entrada, linha) && achou == false){
		RRN++;
		
		//if para ignorar remocoes logicas
		if(linha[0] != '@'){
			
			//carregando uma linha em um objeto Pessoa
			stringToPessoa(linha, p);
			if(p.chave == chave){
				achou = true;
			}
		}
	}
	
	//volta o ponteiro do arquivo pro inicio
	entrada.clear();
	entrada.seekg(0);
	
	
	//caso nao tenha encontrado, retorna -1
	if(achou == false){
		return -1;
	}
	
	//caso tenha encontrado, retorna o RRN
	if(achou == true){
		return RRN;
	}
}


//remove a primeira ocorrencia de uma dada chave no arquivo
void removePessoa(int chave, istream& entrada, ostream& saida){
	
	//carrega o RRN correspondente a primeira ocorrencia da chave
	int RRN = buscaPessoa(chave, entrada);
	string linha;
	
	//caso nao tenha encontrado nenhuma ocorrencia
	if(RRN == -1){
		//cout << "o arquivo nao continha registro com a chave: " << chave << endl;
	}
	else{
		//carregando a string em linha
		entrada.seekg((RRN * registro_tam));
		getline(entrada, linha);
		
		//volta o ponteiro do arquivo pro inicio
		entrada.clear();
		entrada.seekg(0);
		
		//remocao LOGICA
		linha[0] = '@';
		
		//reescreve no arquivo
		saida.seekp((RRN * registro_tam));
		saida << linha << endl;
		
		//volta o ponteiro do arquivo pro inicio
		saida.clear();
		saida.seekp(0);
		
		//cout << "a chave: " << chave << " foi removida com sucesso" << endl;
	}
}


//funcao que insere uma pessoa no arquivo (fazendo reaproveitamento de espaco)
void inserePessoa(istream& entrada, ostream& saida, Pessoa p){
	int RRN = numReg(entrada);
	string linha;
	string inserindo;
	
	//completando os espaços vazios dos campos com '@'
	inserindo = pessoaToString(p);

	//caso o arquivo esteja vazio, apenas insira no inicio
	if(RRN == 0){
		saida << inserindo << endl;
	}
	else{	
		
		//while que percorrera o arquivo inteiro e armazenara o RRN do ultimo registro removido logicamente
		int i = -1;
		while(getline(entrada, linha)){
			i++;
			if(linha[0] == '@'){
				RRN = i;
			}
		}
		
		//como mexemos no ponteiro de entrada, devemos retorna-lo ao inicio
		entrada.clear();
		entrada.seekg(0);
	
		//realiza a insercao no RRN adequado (fim do arquivo OU em um registro removido logicamente)
		saida.seekp((RRN * registro_tam));
		saida << inserindo << endl;
		
		//como mexemos no ponteiro de saida, devemos retorna-lo ao inicio
		saida.clear();
		saida.seekp(0);
	}

}


//reescreve todos os registros validos e substitui os removidos logicamente por uma linha vazia
void compacta(istream& entrada, ostream& saida, string nome){
	string linha;
	
	//vetor onde sera armazenado os registros validos
	vector<string> registros;
	
	
	//colocara no vetor "registros" todos os registros validos
	while(getline(entrada, linha)){
		if(linha[0] != '@'){
			registros.push_back(linha);
		}
	}
	
	//substitui o arquivo atual por um arquivo vazio com o mesmo nome
	criaArquivo(nome);
	
	//reescreve todos os registros validos
	for(int i = 0; i < registros.size(); i++){
		saida << registros[i] << endl;
	}
	
	//como mexemos no ponteiro de entrada, devemos retorna-lo ao inicio
	saida.clear();
	saida.seekp(0);
	
	//como mexemos no ponteiro de entrada, devemos retorna-lo ao inicio
	entrada.clear();
	entrada.seekg(0);
}


//lista todos os registros validos
void listaPessoas(istream& entrada){
	Pessoa p;
	string linha;
	
	cout << "Listagem de todos os registros: " << endl << endl;
	
	while(getline(entrada, linha)){
		if(linha[0] != '@'){
			stringToPessoa(linha, p);
			cout << "Chave: " << p.chave << endl << "Nome: " << p.nome << endl << "Idade: " << p.idade << endl << endl;
		}
	}
}



int main()
{
	
	/*
    Pessoa pessoaTeste;
	pessoaTeste.chave = 12345;
	pessoaTeste.nome = "Fulano da Silva";
	pessoaTeste.idade = 16;
	*/
	
	
    fstream entrada;
	fstream saida;
	entrada.open("teste.dat");
	saida.open("teste.dat");

	//realize as operacoes aqui
	
	entrada.close();
	saida.close();

	

	getchar();

    return 0;
}
