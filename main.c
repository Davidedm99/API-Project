#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define MAX 5000
#define ALTISSIMO_NUMERO 2147483647

typedef struct command{
    int nNodes, nTop;
}Commands;

typedef struct nodeDist{
    unsigned long dist;
    bool passed;
    struct nodeDist* next;
}nodeDist;

typedef struct sNode{
    int graphindex;
    unsigned long key;
    struct sNode* next;
    struct sNode* prev;
}sNode;

//------GLOBALS--------------

Commands c;
sNode* head;
sNode* back;
int count = 0;
unsigned long sc;
unsigned long* dist;
char* ptr;
char* str;
unsigned long* adj;
unsigned long* dist;
bool* passed;
char* trash;
unsigned long cm;
int listSize = 0;
nodeDist* distHead;

//--------FUNCTIONS------------

void parser(char* in);
void addMatrix();
unsigned long minDist();
unsigned long getMin();
unsigned long dijkstra();
void sortedSNodeInsert(sNode* prev, unsigned long key, int value);
void headNodeInsert(unsigned long key, int value);
void findSNode(unsigned long key, int value);
void printTopK();
void init();
void removeLastNode();
void setInfDist();

//---------------------------------

void init(){
    adj = (unsigned long*) malloc( c.nNodes * c.nNodes * sizeof(unsigned long*));
    dist = (unsigned long*) malloc(c.nNodes * sizeof(unsigned long));
    passed = (bool*) malloc(c.nNodes * sizeof(bool));
}

void parser(char* in){

    if(*in == 'A'){
        addMatrix();
    }else if(*in == 'T'){
        if(count != 0) {
            printTopK();
        }else{
            printf("\n");
        }
    }else {
        c.nNodes = strtol(in, &ptr, 10);
        ptr = ptr + 1;
        c.nTop = strtol(ptr, &ptr, 10);
        init();
    }
}

void addMatrix(){

    for(int n = 0; n < c.nNodes; n++ ){
        trash = fgets(str, MAX, stdin);
        ptr = str;
        for(int j = 0; j < c.nNodes; j++){
            adj[n * c.nNodes + j] = strtoul(ptr, &ptr, 10);
            ptr++;
        }
    }
    findSNode(dijkstra(adj), count++);
}

unsigned long minDist(){
    unsigned long min = ALTISSIMO_NUMERO, min_index = ALTISSIMO_NUMERO;
    nodeDist* temp = distHead;

    for(int v = 0; v < c.nNodes; v++){
        if(temp->passed == false && temp->dist <= min ) {
            min = temp->dist;
            min_index = v;
        }
    }
    return min_index;
}

unsigned long getMin(){
    cm = 0;
    for (unsigned long i = 0; i < c.nNodes; i++) {
        if (dist[i] == ALTISSIMO_NUMERO) {
            dist[i] = 0;
        }
        cm += dist[i];
    }
    return cm;
}

void setInfDist(){
    nodeDist* temp;
    sc = 0;
    for(int n = 0; n < c.nNodes; n++){
        nodeDist* nd = (nodeDist*) malloc(sizeof(nodeDist));
        nd->dist = ALTISSIMO_NUMERO;
        nd->passed = false;
        nd->next = NULL;
        sc += adj[n];
        if(n == 0){
            distHead = nd;
        }
        else {
            temp->next = nd;
        }
        temp = nd;
    }
}

unsigned long dijkstra(){
    //inizializzo un array delle distanze dallo 0 a infinito e un parallelo array che monitora il passaggio
    setInfDist();
    //controllo semplicissimo sulla prima riga della matrice di adiacenza, se 0 evito dijkstra
    if(sc == 0){
        return sc;
    }
    //distanza 0 se stesso nulla , estraggo il più corto e lo segno come passato per non ri-controllarlo
    distHead->dist = 0;
    for(int n = 0; n < c.nNodes-1; n++) {
        unsigned long u = minDist();
        passed[u] = true;
        //preso il nodo attuale u che è il minore monitoro i contigui su cui vale false il passed[]
        for (int v = 0; v < c.nNodes; v++) {
            if (!passed[v] && adj[u * c.nNodes + v] && dist[u] + adj[u * c.nNodes + v] < dist[v]) {
                dist[v] = dist[u] + adj[u * c.nNodes + v];
            }
        }
    }
    //ritorno del valore dato dalla somma di tutti ivalori costituenti l'array dist[]
    return getMin();
}

void sortedSNodeInsert(sNode* prev, unsigned long key, int value){
    sNode* newSNode = (sNode *) malloc(sizeof(sNode));
    newSNode->key = key;
    newSNode->graphindex = value;
    newSNode->next = prev->next;
    newSNode->prev = prev;
    prev->next->prev = newSNode;
    prev->next = newSNode;
    listSize++;
}

void backNodeInsert(unsigned long key, int value){
    sNode* newSNode = (sNode*) malloc(sizeof(sNode));
    newSNode->key = key;
    newSNode->graphindex = value;
    newSNode->next = NULL;
    newSNode->prev = back;
    back->next = newSNode;
    back = newSNode;
    listSize++;
}

void headNodeInsert(unsigned long key, int value){
    sNode* newSNode = (sNode*) malloc(sizeof(sNode));
    newSNode->key = key;
    newSNode->graphindex = value;
    newSNode->prev = NULL;
    newSNode->next = head;
    if(head != NULL) {
        head->prev = newSNode;
    }
    head = newSNode;
    listSize++;
}

void findSNode(unsigned long key, int value){
    sNode* temp = head;
    if(temp == NULL){                                             //solo caso lista vuota, head è nodo massimo
        headNodeInsert(key, value);
        back = head;
        return;
    }
    if(key >= back->key){                                         //valore da inserire maggiore del max, in coda
        if(listSize > c.nTop) {                                  //se lista e stack pieni non inserire
            return;
        }else{                                                    //dato che somma mai > nTop allora inserisco in coda
            backNodeInsert(key, value);
        }
    }else{                                                        //valore grafo <= al max, IN TESTA O AL CENTRO
        if(temp->key > key){
            headNodeInsert(key, value);
            if(listSize > c.nTop){
                removeLastNode();
            }
        }else{
            while(key >= temp->next->key){
                temp = temp->next;
            }
            sortedSNodeInsert(temp, key, value);
            if(listSize > c. nTop){
                removeLastNode();
            }
        }
    }
}

void removeLastNode(){
    back = back->prev;
    free(back->next);
    back->next = NULL;
    listSize--;
}

void printTopK(){
    sNode* temp = head;
    for(int i = 0; i < c.nTop && temp != NULL; i++){
        if (temp->next != NULL) {                                    //nel caso in cui ci sono meno grafi della topk richiesta vengono stampati valori sballati
            printf("%d ", temp->graphindex);
        } else {
            printf("%d\n", temp->graphindex);
        }
        temp = temp->next;
    }
}

int main(){
    str = (char*) malloc(MAX * sizeof(char));
    trash = fgets(str, MAX, stdin);

    while(trash != NULL) {
        parser(str);
        trash = fgets(str, MAX, stdin);
    }
}