// BatchMeansVSOverlapBatchMeans.cpp : Define o ponto de entrada para a aplicação de console.
//

// MM1_Project.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include "stdlib.h"

#include "Original\TimedQueue.h"
#include "Original\ServerList.h"
#include "Original\mt.h"

#include <string>
#include <chrono>
// Originais
#include <iostream>
#include <cmath>

#include <algorithm>    // std::sort

using namespace std;

MersenneTwister mt;

TimedQueue questao03();
TimedQueue fase01();
bool NBM(TimedQueue queue);
bool OBM(TimedQueue queue);

double valorTeoricoDaMedia(double lambda, double mi);
long double intervaloConfianca(vector<double> element, double waitAvg, double desvio);
TimedQueue ExecMM1_Queue(TimedQueue queue, int exponent, float interArrivalMean, float serviceRateMean, int servers, bool listQueue);
TimedQueue ExecMM1_Queue(int exponent, float interArrivalMean, float serviceRateMean, int servers, bool listQueue);

double desvioPadrao(vector<double> mediasBlocos, double waitAvg);

void obterInformacoesQueue(TimedQueue queue);

vector<double> SWcoefficients(double B);
void executaNBM();
void executaOBM();

double lambda = 5;
int mi = 10;
vector<double> _medias;

// Adiciona 1000 elementos na queue e reprocessa
TimedQueue aumentaQueue(TimedQueue original, int exponent) {
	TimedQueue queue;

	queue = ExecMM1_Queue(original, exponent, lambda, mi, 1, false);
	//printf("Tamanho atual da Queue: %f\n", queue.Size());
	return queue;
}

int main()
{
	//executaNBM();
	executaOBM();

	system("pause");
	return 0;
}

// Executa Nonoverlapping Batch Means
void executaNBM() {

	int count = 0;
	// Execucao do algoritmo NBM 100 vezes
	for (int i = 0; i< 100; i++) {
		printf("\n\n************************\n");
		printf("Nonoverlapping Batch Means\n");
		printf("Execucao da iteracao %d\n", i);
		TimedQueue queue;

		printf("\n\n************************\n");
		printf("Execucao da Fase de Eliminacao Transiente\n");
		queue = fase01();

		printf("\n************************\n");
		printf("Execucao do NBM\n");
		if (NBM(queue))
			count++;
	}

	// Tratamento dos dados
	printf("\nNumero de Acertos: %d\n", count);
	double mediaBlocos = 0;
	double desvio = 0;
	double variacaoIC = 0;
	// Calcula a media dos blocos
	for (int i = 0; i < _medias.size(); i++) {
		mediaBlocos = mediaBlocos + _medias[i];
	}
	mediaBlocos = mediaBlocos / _medias.size();

	// Calcula do Desvio Padrao
	desvio = desvioPadrao(_medias, mediaBlocos);
	variacaoIC = intervaloConfianca(_medias, mediaBlocos, desvio);

	long double h = variacaoIC;
	// Obtem gama
	long double gama = h / mediaBlocos;

	printf("Media Global: %f\n", mediaBlocos);
	printf("IC-: %f;\n", mediaBlocos - variacaoIC);
	printf("IC+: %f;\n", mediaBlocos + variacaoIC);
	printf("Precisao Relativa: %f;\n", gama);

}

// Executa Nonoverlapping Batch Means
void executaOBM() {

	int count = 0;
	// Execucao do algoritmo NBM 100 vezes
	for (int i = 0; i< 100; i++) {
		printf("\n\n************************\n");
		printf("Overlapping Batch Means");
		printf("Execucao da iteracao %d\n", i);
		TimedQueue queue;

		printf("\n\n************************\n");
		printf("Execucao da Fase de Eliminacao Transiente\n");
		queue = fase01();

		printf("\n************************\n");
		printf("Execucao do OBM\n");
		if (OBM(queue))
			count++;
	}

	// Tratamento dos dados
	printf("\nNumero de Acertos: %d\n", count);
	double mediaBlocos = 0;
	double desvio = 0;
	double variacaoIC = 0;
	// Calcula a media dos blocos
	for (int i = 0; i < _medias.size(); i++) {
		mediaBlocos = mediaBlocos + _medias[i];
	}
	mediaBlocos = mediaBlocos / _medias.size();

	// Calcula do Desvio Padrao
	desvio = desvioPadrao(_medias, mediaBlocos);
	variacaoIC = intervaloConfianca(_medias, mediaBlocos, desvio);

	long double h = variacaoIC;
	// Obtem gama
	long double gama = h / mediaBlocos;

	printf("Media Global: %f\n", mediaBlocos);
	printf("IC-: %f;\n", mediaBlocos - variacaoIC);
	printf("IC+: %f;\n", mediaBlocos + variacaoIC);
	printf("Precisao Relativa: %f;\n", gama);

}

void printQueue(TimedQueue queue) {
	for (int i = 0; i < queue.Size(); i++) {
		printf("%f, ", queue[i].waitTime);
	}
	printf("\n\n");
}



// *******************************
// Desenvolvimento NBM

bool testeVonNeuman(vector<double> mediasBlocos) {
	double B = 0.05;
	vector<int> R;
	int contadorR = 0;

	double RVN = 0.0;

	for (int i = 0; i < mediasBlocos.size(); i++) {
		for (int j = 0; j < mediasBlocos.size(); j++) {
			if (i == j) {
				continue;
			}

			if (mediasBlocos[j] <= mediasBlocos[i]) {
				contadorR++;
			}
		}
		R.push_back(contadorR);
		contadorR = 0;
	}
	// Media R
	double mediaR = 0.0;
	for (int i = 0; i < R.size(); i++) {
		mediaR = mediaR + R[i];
	}
	mediaR = mediaR / R.size();

	// Numerador
	double numerador = 0.0;
	for (int i = 0; i < R.size() - 1; i++) {
		numerador = numerador + pow((R[i] - R[i + 1]), 2);
	}
	// Denominador
	double denominador = 0.0;
	for (int i = 0; i < R.size(); i++) {
		denominador = denominador + pow((R[i] - mediaR), 2);
	}

	// RVN
	RVN = numerador / denominador;
	if (RVN > 1.44)
		return true;

	return false;
}

double desvioPadrao(vector<double> mediasBlocos, double waitAvg) {
	double desvio = 0.0;
	double aux;

	if (waitAvg > 0.0) {

		for (int i = 0; i < mediasBlocos.size(); ++i) {
			aux = mediasBlocos[i];
			desvio = desvio + pow(aux - waitAvg, 2);
		}
		desvio = desvio / (mediasBlocos.size() - 1);
		desvio = sqrt(desvio);
	}

	return desvio;
}

long double intervaloConfianca(vector<double> element, double waitAvg, double desvio) {
	double lvlConfianca = 0.95;
	long double variacao = 0.0;

	if (waitAvg > 0.0) {

		// obtendo T[n-1; alpha]
		double alpha = 1 - lvlConfianca;
		double student = student_dist(element.size() - 1, alpha);
		// double student = student_dist(element.size() - 1, alpha / 2);

		if ((alpha > 0)
			&& (student > 0)
			&& (element.size() > 0)) {
			variacao = (desvio * student) / sqrt(element.size());
		}

	}
	return variacao;
}

bool batchMeans(TimedQueue queue, int N, int M, int B) {
	bool faseTeste = false;
	bool faseEstimacao = false;
	double media = 0.0;

	vector<double> mediasBlocos;

	// Inicio da Fase de Teste
	// Teste de Von Neumann
	while (!faseTeste) {
		vector<double> mediasBlocosAux;
		for (int i = 0; i < B; i++) {
			for (int j = 0; j < M; j++) {
				media = media + queue[i*M + j].waitTime;
			}
			media = media / M;
			mediasBlocosAux.push_back(media);
			media = 0;
		}
		if (testeVonNeuman(mediasBlocosAux)) {
			mediasBlocos = mediasBlocosAux;
			faseTeste = true;
			break;
		}
		else {
			//M = 2 * M;
			M = 10 + M;
			N = B * M;
			while (N > queue.Size()) {
				queue = aumentaQueue(queue, 3);
			}
		}
	}

	// Inicio da Fase de Estimacao
	while (!faseEstimacao) {
		vector<double> mediasBlocos;
		double mediaBlocos = 0.0;
		double desvio = 0.0;
		long double variacaoIC = 0.0;

		for (int i = 0; i < B; i++) {
			for (int j = 0; j < M; j++) {
				media = media + queue[i*M + j].waitTime;
			}
			media = media / M;
			mediasBlocos.push_back(media);
			media = 0;
		}

		// Calcula a media dos blocos
		for (int i = 0; i < mediasBlocos.size(); i++) {
			mediaBlocos = mediaBlocos + mediasBlocos[i];
		}
		mediaBlocos = mediaBlocos / mediasBlocos.size();

		// Calcula do Desvio Padrao
		desvio = desvioPadrao(mediasBlocos, mediaBlocos);

		variacaoIC = intervaloConfianca(mediasBlocos, mediaBlocos, desvio);

		long double h = variacaoIC;
		// Obtem gama
		long double gama = h / mediaBlocos;

		// Checa condicao de parada
		if (gama <= 0.05) {
			double icInferior = mediaBlocos - variacaoIC;
			double icSuperior = mediaBlocos + variacaoIC;

			printf("Media Global: %f\n", mediaBlocos);
			printf("IC-: %f;\n", mediaBlocos - variacaoIC);
			printf("IC+: %f;\n", mediaBlocos + variacaoIC);
			printf("Precisao Relativa: %f;\n", gama);
			printf("Valor teorico: %f;\n", valorTeoricoDaMedia(lambda, mi));
			printf("N = %d\tM = %d\tB = %d\n\n", N, M, B);
			
			_medias.push_back(mediaBlocos);

			if (icInferior < valorTeoricoDaMedia(lambda, mi)
				&& valorTeoricoDaMedia(lambda, mi) < icSuperior)
				return true;
			else
				return false;
		}
		else {
			M = 300 + M ;
			N = B * M;
			while (N > queue.Size()) {
				queue = aumentaQueue(queue, 4);
			}
		}
	}

	return false;
}

bool oBatchMeans(TimedQueue queue, int N, int M, int B) {
	bool faseTeste = false;
	bool faseEstimacao = false;
	double media = 0.0;

	vector<double> mediasBlocos;

	// Inicio da Fase de Teste
	// Teste de Von Neumann
	while (!faseTeste) {
		vector<double> mediasBlocosAux;
		for (int i = 0; i < B; i++) {
			for (int j = 0; j < M; j++) {
				media = media + queue[i*M + j].waitTime;
			}
			media = media / M;
			mediasBlocosAux.push_back(media);
			media = 0;
		}
		if (testeVonNeuman(mediasBlocosAux)) {
			mediasBlocos = mediasBlocosAux;
			faseTeste = true;
			break;
		}
		else {
			//M = 2 * M;
			M = 10 + M;
			N = B * M;
			while (N > queue.Size()) {
				queue = aumentaQueue(queue, 3);
			}
		}
	}

	// Inicio da Fase de Estimacao
	while (!faseEstimacao) {
		vector<double> mediasBlocos;
		double mediaBlocos = 0.0;
		double desvio = 0.0;
		// Atualizando o bzinho
		double b = N - M + 1;

		// Atualizando medias de blocos
		for (int i = 0; i < b; i++) {
			for (int j = i; j < i + M; j++) {
				media = media + queue[j].waitTime;
			}
			media = media / M;
			mediasBlocos.push_back(media);
			media = 0;
		}

		// Atualizando a media dos blocos
		// Calcula a media dos blocos
		for (int i = 0; i < mediasBlocos.size(); i++) {
			mediaBlocos = mediaBlocos + mediasBlocos[i];
		}
		mediaBlocos = mediaBlocos / mediasBlocos.size();

		// Calcula do Desvio Padrao
		desvio = desvioPadrao(mediasBlocos, mediaBlocos);
		
		// Calculo da largura do intervalo de confianca
		long double h;
		double alpha = 1 - 0.95;
		double student = student_dist(1.5*(b - 1), alpha/2);

		h = (desvio * student) / sqrt((B - 1));

		// Obtem gama
		long double gama = h / mediaBlocos;

		// Checa condicao de parada
		if (gama <= 0.05) {
			double icInferior = mediaBlocos - h;
			double icSuperior = mediaBlocos + h;

			printf("Media Global: %f\n", mediaBlocos);
			printf("IC-: %f;\n", mediaBlocos - h);
			printf("IC+: %f;\n", mediaBlocos + h);
			printf("Precisao Relativa: %f;\n", gama);
			printf("Valor teorico: %f;\n", valorTeoricoDaMedia(lambda, mi));
			printf("N = %d\tM = %d\tB = %d\n\n", N, M, B);

			_medias.push_back(mediaBlocos);

			if (icInferior < valorTeoricoDaMedia(lambda, mi)
				&& valorTeoricoDaMedia(lambda, mi) < icSuperior)
				return true;
			else
				return false;
		}
		else {
			if (M < 1400)
				M = 1400 + M;
			else
				M = 500 + M;
			N = B * M;
			while (N > queue.Size()) {
				queue = aumentaQueue(queue, 4);
			}
		}
	}

	return false;
}

// Nonoverlapping Batch Means
bool NBM(TimedQueue queue) {
	int N;
	int B;
	int M;

	M = 100;
	B = 32;
	N = M * B;
	// Coleta ate pelo menos a queue ter tamnho N
	while (N > queue.Size()) {
		queue = aumentaQueue(queue, 3);
	}

	return batchMeans(queue, N, M, B);
}

// Overlapping Batch Means
bool OBM(TimedQueue queue) {
	int N;
	int B;
	int M;

	M = 100;
	B = 32;
	N = M * B;
	// Coleta ate pelo menos a queue ter tamnho N
	while (N > queue.Size()) {
		queue = aumentaQueue(queue, 3);
	}

	return oBatchMeans(queue, N, M, B);
}

// *******************************
// Desenvolvimento para Eliminacao da Fase
// Transiente

void obterInformacoesQueue(TimedQueue queue) {
	// Media da espera
	double waitingAverage = queue.WaitingAverage();
	// Intervalo de confianca
	long double variacaoIC = queue.IntervaloConfianca();

	printf("\nobterInformacoesQueue:\n");
	printf("Media de Espera: %f\n", waitingAverage);
	printf("VariacaoIC: %f\n", variacaoIC);
	// Aplicacao do intervalo de confianca na media
	printf("IC-: %f;\n", waitingAverage - variacaoIC);
	printf("IC+: %f;\n", waitingAverage + variacaoIC);
	printf("Numero de elementos da queue final: %f\n", queue.Size());
}

TimedQueue podaQueueEObtemInformacoes(TimedQueue queue, int j) {
	// Remove os J primeiros elementos
	queue.RemoveElements(j);

	// Recalcula Media
	queue.ProcessQueue();

	//obterInformacoesQueue(queue);

	return queue;
}

TimedQueue questao04DoJobForK(TimedQueue queue, int k) {
	double n = 0;
	int j = 0;  // posicao ate encontrar k passagens pela media
	int contadorTransicoes = 0;
	double media = 0.0;
	double referencia = 0;

	while (contadorTransicoes < k) {
		vector<double> observacoes;
		// n = n + (queue.Size() / 100) + 1;
		n = n + 300;
		// Incrementa tamanho da Queue
		while (n >= queue.Size())
			queue = aumentaQueue(queue, 2);

		contadorTransicoes = 0;

		// Pega N primeiro elementos
		for (int i = 0; i < n; i++) {
			observacoes.push_back(queue[i].waitTime);
			media = media + queue[i].waitTime;
			// printf("%f, ", queue[i].waitTime);
		}
		media = media / n;

		referencia = observacoes[0];

		for (int i = 1; i < n; i++) {
			if ((observacoes[i] > media
				&& referencia < media) ||
				(observacoes[i] < media
					&& referencia > media)) {
				contadorTransicoes++;
				j = i;

				if (contadorTransicoes == k)
					break;
			}
			referencia = observacoes[i];
		}
	}
	// Faz prints e retorna a queue
	return podaQueueEObtemInformacoes(queue, j);
}

TimedQueue fase01() {
	// Carregamento inicial da fila M/M/1
	// com intervalo de confiança de 95% e como
	// regra de parada a precisão relativa de 5 %
	TimedQueue queue = questao03();

	// Eliminicao do transiente
	TimedQueue auxQueue;
	// K = 20
	auxQueue = queue;
	auxQueue = questao04DoJobForK(auxQueue, 20);

	return auxQueue;
}

// *******************************
// Desenvolvimento para Questao 3

TimedQueue questao03() {
	TimedQueue queue;
	bool continuarExecucao = true;
	int queueSize;

	// Inicia os calculos para o caso inicial de uma fila
	queue = ExecMM1_Queue(2, lambda, mi, 1, false);
	queueSize = queue.Size();
	// Media da espera
	double waitingAverage = queue.WaitingAverage();
	// Intervalo de confianca
	long double variacaoIC = queue.IntervaloConfianca();

	long double h = variacaoIC;
	// Obtem gama
	long double gama = h / waitingAverage;

	// Checa condicao de parada
	if (gama <= 0.05) {
		continuarExecucao = false;
	}
	// Caso a condicao de parada nao seja alcancada
	// Repete os passos anteriores
	while (continuarExecucao) {
		queue = aumentaQueue(queue, 2);
		queueSize = queue.Size();

		waitingAverage = queue.WaitingAverage();
		variacaoIC = queue.IntervaloConfianca();

		h = variacaoIC;
		gama = h / waitingAverage;
		if (gama <= 0.05) {
			continuarExecucao = false;
			break;
		}
	}

	return queue;
}

double ExponentialRandom(double seed, double lambda) {
	return -log(seed) / lambda;
}

double valorTeoricoDaMedia(double lambda, double mi) {
	double ro = lambda / mi;
	double media = ro * (1 / mi);
	media = media / (1 - ro);

	return media;
}

TimedQueue ExecMM1_Queue(TimedQueue queue, int exponent, float interArrivalMean, float serviceRateMean, int servers, bool listQueue) {

	ServerList server(servers);

	// we need to generate a random number to make the MersenneTwister be different everytime it starts.
	// Read more about it on MersenneTwister documentation.
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();

	mt.init_genrand(seed1);

	int sampleSize = pow(10, exponent);

	double interArrivalTime;
	double serviceTime;

	double antigoTamanho = queue.Size();

	//double arrivalTime = 0;
	// obtem o ultimo arrivaltime pra utilizar como base
	double arrivalTime = queue[queue.Size() - 1].arrivalTime;

	// create queue with all elements
	for (int i = 0; i < sampleSize; ++i) {
		// random seeds will be used to generate exponential random number - which means M from "M"/M/1
		double seed = mt.genrand_real2();
		interArrivalTime = ExponentialRandom(seed, interArrivalMean);

		seed = mt.genrand_real2();
		serviceTime = ExponentialRandom(seed, serviceRateMean); // here is the 2nd M - M/"M"/1

																// the sum of inter arrival times will be giving the real arriving time of each element.
																// ex.: if first interval is 1 - arriving[0] = 1.
																// if next interval = 0.5 -> arriving[1] = 1+0.5 = 1.5.
																// if next = .75 arr[2] = 1 + 0.5 + .75 = 2.25....
		arrivalTime += interArrivalTime;
		queue.AddElement(i + antigoTamanho, arrivalTime, serviceTime);
	}

	// after we add all process to the queue then we can run the queue on a list of server - in this case 1 (M/M/"1")
	server.RunQueue(&queue);

	// after all process pass in a server we need to get their waiting average. It will be calculate here:
	queue.ProcessQueue();

	// the we return the processed waiting average time.
	return queue;
}

TimedQueue ExecMM1_Queue(int exponent, float interArrivalMean, float serviceRateMean, int servers, bool listQueue) {


	TimedQueue queue;
	ServerList server(servers);

	// we need to generate a random number to make the MersenneTwister be different everytime it starts.
	// Read more about it on MersenneTwister documentation.
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();

	mt.init_genrand(seed1);

	int sampleSize = pow(10, exponent);

	double interArrivalTime;
	double serviceTime;

	string s = "creating random interval arrival times for a sampling of " + to_string(sampleSize) + " elements.";

	double arrivalTime = 0;

	// create queue with all elements
	for (int i = 0; i < sampleSize; ++i) {
		// random seeds will be used to generate exponential random number - which means M from "M"/M/1
		double seed = mt.genrand_real2();
		interArrivalTime = ExponentialRandom(seed, interArrivalMean);

		seed = mt.genrand_real2();
		serviceTime = ExponentialRandom(seed, serviceRateMean); // here is the 2nd M - M/"M"/1

																// the sum of inter arrival times will be giving the real arriving time of each element.
																// ex.: if first interval is 1 - arriving[0] = 1.
																// if next interval = 0.5 -> arriving[1] = 1+0.5 = 1.5.
																// if next = .75 arr[2] = 1 + 0.5 + .75 = 2.25....
		arrivalTime += interArrivalTime;
		queue.AddElement(i, arrivalTime, serviceTime);
	}

	// after we add all process to the queue then we can run the queue on a list of server - in this case 1 (M/M/"1")
	server.RunQueue(&queue);

	// after all process pass in a server we need to get their waiting average. It will be calculate here:
	queue.ProcessQueue();

	// this is not necessary and you should never set it to true unless you want to wait forever for 10^7.
	// Writing in console is so expensive compared to just math only.
	if (listQueue) {
		// Log("Listing queue:");
		queue.ListQueue();
	}

	// the we return the processed waiting average time.
	return queue;
}

vector<double> SWcoefficients(double B) {
	int b = (B / 2) - 1;
	vector<double> a;
	// GAMBIARRA
	for (int i = 0; i < b + 1; i++)
		a.push_back(0);

	if (B == 3)
	{
		a[0] = .7071;
	}
	else if (B == 4)
	{
		a[0] = .6872;
		a[1] = .1677;
	}
	else if (B == 5)
	{
		a[0] = .6646;
		a[1] = .2413;
	}
	else if (B == 6)
	{
		a[0] = .6431;
		a[1] = .2806;
		a[2] = .0875;
	}
	else if (B == 7)
	{
		a[0] = .6233;
		a[1] = .3031;
		a[2] = .1401;
	}
	else if (B == 8)
	{
		a[0] = .6052;
		a[1] = .3164;
		a[2] = .1743;
		a[3] = .0561;
	}
	else if (B == 9)
	{
		a[0] = .5888;
		a[1] = .3244;
		a[2] = .1976;
		a[3] = .0947;
	}
	else if (B == 10)
	{
		a[0] = .5739;
		a[1] = .3291;
		a[2] = .2141;
		a[3] = .1224;
		a[4] = .0399;
	}
	else if (B == 11)
	{
		a[0] = .5601;
		a[1] = .3315;
		a[2] = .226;
		a[3] = .1429;
		a[4] = .0695;
	}
	else if (B == 12)
	{
		a[0] = .5475;
		a[1] = .3325;
		a[2] = .2347;
		a[3] = .1586;
		a[4] = .0922;
		a[5] = .0303;
	}
	else if (B == 13)
	{
		a[0] = .5359;
		a[1] = .3325;
		a[2] = .2412;
		a[3] = .1707;
		a[4] = .1099;
		a[5] = .0539;
	}
	else if (B == 14)
	{
		a[0] = .5251;
		a[1] = .3318;
		a[2] = .246;
		a[3] = .1802;
		a[4] = .124;
		a[5] = .0727;
		a[6] = .024;
	}
	else if (B == 15)
	{
		a[0] = .515;
		a[1] = .3306;
		a[2] = .2495;
		a[3] = .1878;
		a[4] = .1353;
		a[5] = .088;
		a[6] = .0433;
	}
	else if (B == 16)
	{
		a[0] = .5056;
		a[1] = .329;
		a[2] = .2521;
		a[3] = .1939;
		a[4] = .1447;
		a[5] = .1005;
		a[6] = .0593;
		a[7] = .0196;
	}
	else if (B == 17)
	{
		a[0] = .4968;
		a[1] = .3273;
		a[2] = .254;
		a[3] = .1988;
		a[4] = .1524;
		a[5] = .1109;
		a[6] = .0725;
		a[7] = .0359;
	}
	else if (B == 18)
	{
		a[0] = .4886;
		a[1] = .3253;
		a[2] = .2553;
		a[3] = .2027;
		a[4] = .1587;
		a[5] = .1197;
		a[6] = .0837;
		a[7] = .0496;
		a[8] = .0163;
	}
	else if (B == 19)
	{
		a[0] = .4808;
		a[1] = .3232;
		a[2] = .2561;
		a[3] = .2059;
		a[4] = .1641;
		a[5] = .1271;
		a[6] = .0932;
		a[7] = .0612;
		a[8] = .0303;
	}
	else if (B == 20)
	{
		a[0] = .4734;
		a[1] = .3211;
		a[2] = .2565;
		a[3] = .2085;
		a[4] = .1686;
		a[5] = .1334;
		a[6] = .1013;
		a[7] = .0711;
		a[8] = .0422;
		a[9] = .014;
	}
	else if (B == 21)
	{
		a[0] = .4643;
		a[1] = .3185;
		a[2] = .2578;
		a[3] = .2119;
		a[4] = .1736;
		a[5] = .1399;
		a[6] = .1092;
		a[7] = .0804;
		a[8] = .053;
		a[9] = .0263;
	}
	else if (B == 22)
	{
		a[0] = .459;
		a[1] = .3156;
		a[2] = .2571;
		a[3] = .2131;
		a[4] = .1764;
		a[5] = .1443;
		a[6] = .115;
		a[7] = .0878;
		a[8] = .0618;
		a[9] = .0368;
		a[10] = .0122;
	}
	else if (B == 23)
	{
		a[0] = .4542;
		a[1] = .3126;
		a[2] = .2563;
		a[3] = .2139;
		a[4] = .1787;
		a[5] = .148;
		a[6] = .1201;
		a[7] = .0941;
		a[8] = .0696;
		a[9] = .0459;
		a[10] = .0228;
	}
	else if (B == 24)
	{
		a[0] = .4493;
		a[1] = .3098;
		a[2] = .2554;
		a[3] = .2145;
		a[4] = .1807;
		a[5] = .1512;
		a[6] = .1245;
		a[7] = .0997;
		a[8] = .0764;
		a[9] = .0539;
		a[10] = .0321;
		a[11] = .0107;
	}
	else if (B == 25)
	{
		a[0] = .445;
		a[1] = .3069;
		a[2] = .2543;
		a[3] = .2148;
		a[4] = .1822;
		a[5] = .1539;
		a[6] = .1283;
		a[7] = .1046;
		a[8] = .0823;
		a[9] = .061;
		a[10] = .0403;
		a[11] = .02;
	}
	else if (B == 26)
	{
		a[0] = .4407;
		a[1] = .3043;
		a[2] = .2533;
		a[3] = .2151;
		a[4] = .1836;
		a[5] = .1563;
		a[6] = .1316;
		a[7] = .1089;
		a[8] = .0876;
		a[9] = .0672;
		a[10] = .0476;
		a[11] = .0284;
		a[12] = .0094;
	}
	else if (B == 27)
	{
		a[0] = .4366;
		a[1] = .3018;
		a[2] = .2522;
		a[3] = .2152;
		a[4] = .1848;
		a[5] = .1584;
		a[6] = .1346;
		a[7] = .1128;
		a[8] = .0923;
		a[9] = .0728;
		a[10] = .054;
		a[11] = .0358;
		a[12] = .0178;
	}
	else if (B == 28)
	{
		a[0] = .4328;
		a[1] = .2992;
		a[2] = .251;
		a[3] = .2151;
		a[4] = .1857;
		a[5] = .1601;
		a[6] = .1372;
		a[7] = .1162;
		a[8] = .0965;
		a[9] = .0778;
		a[10] = .0598;
		a[11] = .0424;
		a[12] = .0253;
		a[13] = .0084;
	}
	else if (B == 29)
	{
		a[0] = .4291;
		a[1] = .2968;
		a[2] = .2499;
		a[3] = .215;
		a[4] = .1864;
		a[5] = .1616;
		a[6] = .1395;
		a[7] = .1192;
		a[8] = .1002;
		a[9] = .0822;
		a[10] = .065;
		a[11] = .0483;
		a[12] = .032;
		a[13] = .0159;
	}
	else if (B == 30)
	{
		a[0] = .4254;
		a[1] = .2944;
		a[2] = .2487;
		a[3] = .2148;
		a[4] = .187;
		a[5] = .163;
		a[6] = .1415;
		a[7] = .1219;
		a[8] = .1036;
		a[9] = .0862;
		a[10] = .0697;
		a[11] = .0537;
		a[12] = .0381;
		a[13] = .0227;
		a[14] = .0076;
	}
	else if (B == 31)
	{
		a[0] = .422;
		a[1] = .2921;
		a[2] = .2475;
		a[3] = .2145;
		a[4] = .1874;
		a[5] = .1641;
		a[6] = .1433;
		a[7] = .1243;
		a[8] = .1066;
		a[9] = .0899;
		a[10] = .0739;
		a[11] = .0585;
		a[12] = .0435;
		a[13] = .0289;
		a[14] = .0144;
	}
	else if (B == 32)
	{
		a[0] = .4188;
		a[1] = .2898;
		a[2] = .2463;
		a[3] = .2141;
		a[4] = .1878;
		a[5] = .1651;
		a[6] = .1449;
		a[7] = .1265;
		a[8] = .1093;
		a[9] = .0931;
		a[10] = .0777;
		a[11] = .0629;
		a[12] = .0485;
		a[13] = .0344;
		a[14] = .0206;
		a[15] = .0068;
	}
	else if (B == 33)
	{
		a[0] = .4156;
		a[1] = .2876;
		a[2] = .2451;
		a[3] = .2137;
		a[4] = .188;
		a[5] = .166;
		a[6] = .1463;
		a[7] = .1284;
		a[8] = .1118;
		a[9] = .0961;
		a[10] = .0812;
		a[11] = .0669;
		a[12] = .053;
		a[13] = .0395;
		a[14] = .0262;
		a[15] = .0131;
	}
	else if (B == 34)
	{
		a[0] = .4127;
		a[1] = .2854;
		a[2] = .2439;
		a[3] = .2132;
		a[4] = .1882;
		a[5] = .1667;
		a[6] = .1475;
		a[7] = .1301;
		a[8] = .114;
		a[9] = .0988;
		a[10] = .0844;
		a[11] = .0706;
		a[12] = .0572;
		a[13] = .0441;
		a[14] = .0314;
		a[15] = .0187;
		a[16] = .0062;
	}
	else if (B == 35)
	{
		a[0] = .4096;
		a[1] = .2834;
		a[2] = .2427;
		a[3] = .2127;
		a[4] = .1883;
		a[5] = .1673;
		a[6] = .1487;
		a[7] = .1317;
		a[8] = .116;
		a[9] = .1013;
		a[10] = .0873;
		a[11] = .0739;
		a[12] = .061;
		a[13] = .0484;
		a[14] = .0361;
		a[15] = .0239;
		a[16] = .0119;
	}
	else if (B == 36)
	{
		a[0] = .4068;
		a[1] = .2813;
		a[2] = .2415;
		a[3] = .2121;
		a[4] = .1883;
		a[5] = .1678;
		a[6] = .1496;
		a[7] = .1331;
		a[8] = .1179;
		a[9] = .1036;
		a[10] = .09;
		a[11] = .077;
		a[12] = .0645;
		a[13] = .0523;
		a[14] = .0404;
		a[15] = .0287;
		a[16] = .0172;
		a[17] = .0057;
	}
	else if (B == 37)
	{
		a[0] = .404;
		a[1] = .2794;
		a[2] = .2403;
		a[3] = .2116;
		a[4] = .1883;
		a[5] = .1683;
		a[6] = .1505;
		a[7] = .1344;
		a[8] = .1196;
		a[9] = .1056;
		a[10] = .0924;
		a[11] = .0798;
		a[12] = .0677;
		a[13] = .0559;
		a[14] = .0444;
		a[15] = .0331;
		a[16] = .022;
		a[17] = .011;
	}
	else if (B == 38)
	{
		a[0] = .4015;
		a[1] = .2774;
		a[2] = .2391;
		a[3] = .211;
		a[4] = .1881;
		a[5] = .1686;
		a[6] = .1513;
		a[7] = .1356;
		a[8] = .1211;
		a[9] = .1075;
		a[10] = .0947;
		a[11] = .0824;
		a[12] = .0706;
		a[13] = .0592;
		a[14] = .0481;
		a[15] = .0372;
		a[16] = .0264;
		a[17] = .0158;
		a[18] = .0053;
	}
	else if (B == 39)
	{
		a[0] = .3989;
		a[1] = .2755;
		a[2] = .238;
		a[3] = .2104;
		a[4] = .188;
		a[5] = .1689;
		a[6] = .152;
		a[7] = .1366;
		a[8] = .1225;
		a[9] = .1092;
		a[10] = .0967;
		a[11] = .0848;
		a[12] = .0733;
		a[13] = .0622;
		a[14] = .0515;
		a[15] = .0409;
		a[16] = .0305;
		a[17] = .0203;
		a[18] = .0101;
	}
	else if (B == 40)
	{
		a[0] = .3964;
		a[1] = .2737;
		a[2] = .2368;
		a[3] = .2098;
		a[4] = .1878;
		a[5] = .1691;
		a[6] = .1526;
		a[7] = .1376;
		a[8] = .1237;
		a[9] = .1108;
		a[10] = .0986;
		a[11] = .087;
		a[12] = .0759;
		a[13] = .0651;
		a[14] = .0546;
		a[15] = .0444;
		a[16] = .0343;
		a[17] = .0244;
		a[18] = .0146;
		a[19] = .0049;
	}
	else if (B == 41)
	{
		a[0] = .394;
		a[1] = .2719;
		a[2] = .2357;
		a[3] = .2091;
		a[4] = .1876;
		a[5] = .1693;
		a[6] = .1531;
		a[7] = .1384;
		a[8] = .1249;
		a[9] = .1123;
		a[10] = .1004;
		a[11] = .0891;
		a[12] = .0782;
		a[13] = .0677;
		a[14] = .0575;
		a[15] = .0476;
		a[16] = .0379;
		a[17] = .0283;
		a[18] = .0188;
		a[19] = .0094;
	}
	else if (B == 42)
	{
		a[0] = .3917;
		a[1] = .2701;
		a[2] = .2345;
		a[3] = .2085;
		a[4] = .1874;
		a[5] = .1694;
		a[6] = .1535;
		a[7] = .1392;
		a[8] = .1259;
		a[9] = .1136;
		a[10] = .102;
		a[11] = .0909;
		a[12] = .0804;
		a[13] = .0701;
		a[14] = .0602;
		a[15] = .0506;
		a[16] = .0411;
		a[17] = .0318;
		a[18] = .0227;
		a[19] = .0136;
		a[20] = .0045;
	}
	else if (B == 43)
	{
		a[0] = .3894;
		a[1] = .2684;
		a[2] = .2334;
		a[3] = .2078;
		a[4] = .1871;
		a[5] = .1695;
		a[6] = .1539;
		a[7] = .1398;
		a[8] = .1269;
		a[9] = .1149;
		a[10] = .1035;
		a[11] = .0927;
		a[12] = .0824;
		a[13] = .0724;
		a[14] = .0628;
		a[15] = .0534;
		a[16] = .0442;
		a[17] = .0352;
		a[18] = .0263;
		a[19] = .0175;
		a[20] = .0087;
	}
	else if (B == 44)
	{
		a[0] = .3872;
		a[1] = .2667;
		a[2] = .2323;
		a[3] = .2072;
		a[4] = .1868;
		a[5] = .1695;
		a[6] = .1542;
		a[7] = .1405;
		a[8] = .1278;
		a[9] = .116;
		a[10] = .1049;
		a[11] = .0943;
		a[12] = .0842;
		a[13] = .0745;
		a[14] = .0651;
		a[15] = .056;
		a[16] = .0471;
		a[17] = .0383;
		a[18] = .0296;
		a[19] = .0211;
		a[20] = .0126;
		a[21] = .0042;
	}
	else if (B == 45)
	{
		a[0] = .385;
		a[1] = .2651;
		a[2] = .2313;
		a[3] = .2065;
		a[4] = .1865;
		a[5] = .1695;
		a[6] = .1545;
		a[7] = .141;
		a[8] = .1286;
		a[9] = .117;
		a[10] = .1062;
		a[11] = .0959;
		a[12] = .086;
		a[13] = .0765;
		a[14] = .0673;
		a[15] = .0584;
		a[16] = .0497;
		a[17] = .0412;
		a[18] = .0328;
		a[19] = .0245;
		a[20] = .0163;
		a[21] = .0081;
	}
	else if (B == 46)
	{
		a[0] = .383;
		a[1] = .2635;
		a[2] = .2302;
		a[3] = .2058;
		a[4] = .1862;
		a[5] = .1695;
		a[6] = .1548;
		a[7] = .1415;
		a[8] = .1293;
		a[9] = .118;
		a[10] = .1073;
		a[11] = .0972;
		a[12] = .0876;
		a[13] = .0783;
		a[14] = .0694;
		a[15] = .0607;
		a[16] = .0522;
		a[17] = .0439;
		a[18] = .0357;
		a[19] = .0277;
		a[20] = .0197;
		a[21] = .0118;
		a[22] = .0039;
	}
	else if (B == 47)
	{
		a[0] = .3808;
		a[1] = .262;
		a[2] = .2291;
		a[3] = .2052;
		a[4] = .1859;
		a[5] = .1695;
		a[6] = .155;
		a[7] = .142;
		a[8] = .13;
		a[9] = .1189;
		a[10] = .1085;
		a[11] = .0986;
		a[12] = .0892;
		a[13] = .0801;
		a[14] = .0713;
		a[15] = .0628;
		a[16] = .0546;
		a[17] = .0465;
		a[18] = .0385;
		a[19] = .0307;
		a[20] = .0229;
		a[21] = .0153;
		a[22] = .0076;
	}
	else if (B == 48)
	{
		a[0] = .3789;
		a[1] = .2604;
		a[2] = .2281;
		a[3] = .2045;
		a[4] = .1855;
		a[5] = .1693;
		a[6] = .1551;
		a[7] = .1423;
		a[8] = .1306;
		a[9] = .1197;
		a[10] = .1095;
		a[11] = .0998;
		a[12] = .0906;
		a[13] = .0817;
		a[14] = .0731;
		a[15] = .0648;
		a[16] = .0568;
		a[17] = .0489;
		a[18] = .0411;
		a[19] = .0335;
		a[20] = .0259;
		a[21] = .0185;
		a[22] = .0111;
		a[23] = .0037;
	}
	else if (B == 49)
	{
		a[0] = .377;
		a[1] = .2589;
		a[2] = .2271;
		a[3] = .2038;
		a[4] = .1851;
		a[5] = .1692;
		a[6] = .1553;
		a[7] = .1427;
		a[8] = .1312;
		a[9] = .1205;
		a[10] = .1105;
		a[11] = .101;
		a[12] = .0919;
		a[13] = .0832;
		a[14] = .0748;
		a[15] = .0667;
		a[16] = .0588;
		a[17] = .0511;
		a[18] = .0436;
		a[19] = .0361;
		a[20] = .0288;
		a[21] = .0215;
		a[22] = .0143;
		a[23] = .0071;
	}
	else if (B == 50)
	{
		a[0] = .3751;
		a[1] = .2574;
		a[2] = .226;
		a[3] = .2032;
		a[4] = .1847;
		a[5] = .1691;
		a[6] = .1554;
		a[7] = .143;
		a[8] = .1317;
		a[9] = .1212;
		a[10] = .1113;
		a[11] = .102;
		a[12] = .0932;
		a[13] = .0846;
		a[14] = .0764;
		a[15] = .0685;
		a[16] = .0608;
		a[17] = .0532;
		a[18] = .0459;
		a[19] = .0386;
		a[20] = .0314;
		a[21] = .0244;
		a[22] = .0174;
		a[23] = .0104;
		a[24] = .0035;
	}
	return a;
}