//
// Created by Juscelino Tanaka on 15/05/17.
//

#include "TimedQueue.h"
#include <math.h>       /* pow */  /* sqrt */

double lvlConfianca = 0.95;

/*
* Retorna um ponto crítico da distribuiçao normal
*/
double normal_dist(double p) {
	double q, z1, n, d;
	q = (p > 0.5) ? (1 - p) : p;
	double logq = log(q);
	z1 = sqrt(-2.0 * logq);
	n = (0.010328 * z1 + 0.802853) * z1 + 2.515517;
	d = ((0.001308 * z1 + 0.189269) * z1 + 1.43278) * z1 + 1;
	z1 -= n / d;
	if (p > 0.5)
		z1 = -z1;
	return z1;
}

/*
* Retorna um ponto crítico da distribuiçao de student
*/
double student_dist(int ndf, double p) {
	int i;
	double z1, z2, h[4], x = 0.0;

	z1 = fabs(normal_dist(p));
	z2 = z1 * z1;
	h[0] = 0.25 * z1 * (z2 + 1.0);
	h[1] = 0.010416667 * z1 * ((5.0 * z2 + 16.0) * z2 + 3.0);
	h[2] = 0.002604167 * z1 * (((3.0 * z2 + 19.0) * z2 + 17.0) * z2 - 15.0);
	h[3] = z1*((((79.0*z2 + 776.0)*z2 + 1482.0)*z2 - 1920.0)*z2 - 945.0);
	h[3] *= 0.000010851;
	for (i = 3; i >= 0; i--)
		x = (x + h[i]) / ((double)ndf);
	z1 += x;
	//if (p > 0.5)
	// abs() não funciona para doubles
	if (z1 < 0.0)
		z1 = -z1;
	return z1;
}

// retorna o desvio padrao dos tempos de espera
// deve ser utilizada apenas se a media ja tiver sido calculada
double TimedQueue::DesvioPadrao() {
	double desvio = 0.0;
	double aux;

	if (waitAvg > 0.0) {

		for (int i = 0; i < element.size(); ++i) {
			Element e = element[i];
			aux = e.waitTime;
			desvio = desvio + pow(aux - waitAvg, 2);
		}
		desvio = desvio / (element.size() - 1);
		desvio = sqrt(desvio);
	}

	return desvio;
}

// retorna o valor do intervalo de confianca a ser somado
// e a ser subtraido da media;
// deve ser utilizada apenas se a media ja tiver sido calculada
long double TimedQueue::IntervaloConfianca() {
	double desvio = 0.0;
	long double variacao = 0.0;

	if (waitAvg > 0.0) {

		desvio = DesvioPadrao();

		// obtendo T[n-1; alpha]
		double alpha = 1 - lvlConfianca;
		double student = student_dist(element.size() - 1, alpha);

		if ((alpha > 0) 
			&& (student > 0) 
			&& (element.size() > 0)) {
			variacao = (desvio * student) / sqrt(element.size());
		}

	}
	variacaoIC = variacao;
	return variacao;
}

// mutator operator[] ---> used to change data members;
// [PT] usado somente para acessar um item diretamente com o operador []. Igual array.
Element& TimedQueue::operator[](int index)
{
    if (index < 0 || index > element.size())
        throw out_of_range("Index out of range");

    return element[index];
}

//Accessor operator[]---> used to read data members
// [PT] mesma coisa aqui, só que apenas para leitura.
Element TimedQueue::operator[](int index) const
{
    if (index < 0 || index > element.size())
        throw out_of_range("Index out of range");

    return element[index];
}

// add an element.
// [PT] Ao adicionar um elemento precisamos saber a hora que ele chegou (arrivalTime) e
// quanto tempo ele vai passar (operationDuration). O restante das informações serão atualizadas quando um server
// chamar ele para ser atendido. So ai saberemos quanto tempo ele esperou, etc.
void TimedQueue::AddElement(int id, float arrivalTime, float operationDuration) {
    Element e;
    e.id = id;
    e.startTime = 0;
    e.endTime = 0;
    e.waitTime = 0;
    e.arrivalTime = arrivalTime;
    e.operationDuration = operationDuration;

    element.push_back(e);
}

void TimedQueue::RemoveElements(int index)
{
	element.erase(element.begin(), element.begin() + index);
}

TimedQueue::TimedQueue() {
}

// calculates the total waiting time and divides by the total of elements on queue = avg waiting time.
// [PT] a soma de todos os tempos esperados por cada elemento dividido pelo total de elementos nos dará a média de
// espera total de todos os elementos que foram atendidos.
void TimedQueue::ProcessQueue() {
    totalWaitTime = 0;

    for (int i = 0; i < element.size(); ++i) {
        Element e = element[i];

        totalWaitTime += e.waitTime;
    }
    waitAvg = totalWaitTime / Size();
}

// total elements on queue
double TimedQueue::Size() {
    return element.size();
}

// return the calculated value on ProcessQueue. Which means, if you don't call that function this will return 0;
double TimedQueue::WaitingAverage() {
    return waitAvg;
}

double TimedQueue::VariacaoIC() {
	return variacaoIC;
}

// list each element with its information. This can take so long for huge lists (i.e. 10^7).
void TimedQueue::ListQueue() {
    for (int i = 0; i < element.size(); ++i) {
        char * str;
        Element e = element[i];
        printf("e[%3d] arrival: %3.8f\t\t"
                       "start: %3.8f\t\t"
                       "wait: %3.8f\t\t"
                       "end: %3.8f\t\t"
                       "operationDuration: %3.8f\t\t"
                       "total: %3.8f\t\t"
                       "server: %d\n",
               e.id, e.arrivalTime, e.startTime, e.waitTime, e.endTime,
               e.operationDuration, e.totalTime, e.serverId );
    }
}

// set the start time of an element - from here we can calculate other related stuffs
// [PT] quando calculamos o tempo de início (não confunda com tempo de chegada - arrivalTime) nós podemos calcular
// outras informações, pois o tempo de operação já é conhecido (foi calculado um tempo aleatório lá no addElement).
// Com isso e o horário real de início (que foi a hora que o server chamou o elemento pra ser atendido) a gente sabe
// quanto ele esperou desde a chegada (waitTime) e o tempo total gasto desde a chegada até a saída (totalTime).
void Element::SetStartTime(double startTime) {
    this->startTime = startTime;
    this->endTime = startTime + operationDuration;
    this->waitTime = startTime - arrivalTime;
    this->totalTime = endTime - arrivalTime;
}
