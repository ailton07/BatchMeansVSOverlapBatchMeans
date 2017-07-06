//
// Created by Juscelino Tanaka on 21/05/17.
//

#include "ServerList.h"

// inits a server list with n server
// [PT] inicializa uma lista com n servers. Neste caso deve ser sempre 1.
ServerList::ServerList(int totalServers) {
    if (totalServers <= 0)
        totalServers = 1;

    for (int i = 0; i < totalServers; ++i) {
        Server s(i);
        servers.push_back(s);
    }
}

// [PT] pega o primeiro servidor da fila que está livre. Ou seja, se o próximo horário disponível dele é menor ou igual
// ao horário de chegada do elemento. Exemplo: se um elemento chegou aos 20 mas o proximo tempo disponivel do server
// é 22, entao o server ainda nao está disponível. Se todos tiverem ocupados então a gente vai pegar o que tem menor
// próximo tempo disponível - ou seja, o atendente que vai acabar primeiro dentre todos.

Server * ServerList::GetFirstAvailable(double arrivalTime) {
    Server * min = &servers[0];

    for (int i = 1; i < servers.size(); ++i) {
        if (min->nextAvailableTime < arrivalTime)
            return min;

        if (servers[i].nextAvailableTime < min->nextAvailableTime)
            min = &servers[i];
    }

    return min;
}

//Server *ServerList::GetLessTired() {
//    Server * min = &servers[0];
//
//    for (int i = 1; i < servers.size(); ++i) {
//        if (servers[i].nextAvailableTime < min->nextAvailableTime)
//            min = &servers[i];
//    }
//
//    return min;
//}


void ServerList::RunQueue(TimedQueue * queue) {

    for (int i = 0; i < queue->Size(); ++i) {
        Element &e = queue[0][i];
        Server * s = GetFirstAvailable(e.arrivalTime);
//        Server * s = GetLessTired();

        // [PT] quando temos um server precisamos saber se ele já está disponível ou teremos que esperar ele acabar o
        // atendimento.

        // [PT] Se ele tiver disponível, ele começará atender no tempo de chegada do elemento.
        if (s->nextAvailableTime < e.arrivalTime) {
            e.SetStartTime(e.arrivalTime);
            // [PT] e o tempo de operação será o horário da chegada mais o tempo de duração da operação.
            s->nextAvailableTime = e.arrivalTime + e.operationDuration;
        } else {
            // [PT] Senão ele começará a atender logo após ele ficar disponível
            e.SetStartTime(s->nextAvailableTime);
            // [PT] e o próximo horário disponível será após o tempo da execução da operação atual
            s->nextAvailableTime += e.operationDuration;
        }

        // save on element the server which served it. and increments the total served elements on server. Stats only.
        // [PT] salva no elemento que servidor atendeu ele - neste caso sempre será o 0 e incrementa o total atendido
        // pelo servidor
        e.serverId = s->id;
        s->totalExecuted++;
    }
}