#include<iostream>
#include <mutex>
#include <stdlib.h>
#include <time.h>
#include <thread>
using namespace std;

template <class T>
struct Node
{
    T dato;
    Node<T> *next;
    //mutex candado;
    bool flag;

    Node(T _dato){
        dato = _dato;
        flag = true;
    }
};

template <class T>
class Lista
{ 
    private:
        Node<T> *head;
        Node<T> *tail;
        mutex candadoActual;
        mutex candadoAnterior;
    
    public:
        Lista(){
            head = nullptr;
            tail = nullptr;
        }

        bool addValue(T valor){
            //candado.lock();
            cout << "Añadiendo: " << valor << endl;
            while (true)
            {           
                Node<T> *newNodo = new Node<T>(valor); 
                //newNodo->dato = valor;

                Node<T> *aux1 = head;
                Node<T> *aux2;

                // ···········································
                Node<T> *temp1 = new Node<T>(-1);
                temp1->next = aux1;
                aux2 = temp1;
                // ···········································

                //Buscamos donde deve entrar el nuevo nodo
                while ( (aux1 != nullptr) && (aux1->dato < valor)){
                    aux2 = aux1;
                    aux1 = aux1->next;
                }

                //bloqueamos actual y anterior
                candadoAnterior.lock();
                candadoActual.lock();
                
                //Primer dato
                if(aux1 == nullptr){
                    head = newNodo; // Insertamos
                    // Desbloquamos
                    candadoAnterior.unlock();
                    candadoActual.unlock();
                    return true;
                }   

                // ---------------- VERIFICACION DE THREADS ----------------
                if(aux1->flag == true && aux2->flag == true){ // verifica el flag de ambos punteros
                    if(aux1->dato == valor){
                        candadoAnterior.unlock();
                        candadoActual.unlock();
                        return false;
                    }

                    //Primer dato
                    if(aux1 == head){
                        head = newNodo;
                    }
                    else{
                        aux2->next = newNodo;
                    }

                    newNodo->next = aux1;
                    candadoAnterior.unlock();
                    candadoActual.unlock();
                    return true;
                }

                candadoAnterior.unlock();
                candadoActual.unlock();
                // ----------------------------------------------------------

                //newNodo->next = aux1;
                //candado.unlock();
                cout << "Nodo "<< valor << " insertado" << endl;
                //return true;
            }
        }

        bool deleteValue(T valor){
            //candado.lock();
            cout << "Eliminando: " << valor << endl;
            while (true)
            {           
                Node<T> *newNodo = new Node<T>(valor); 
                //newNodo->dato = valor;

                Node<T> *aux1 = head;
                Node<T> *aux2;

                // ···········································
                Node<T> *temp1 = new Node<T>(-1);
                temp1->next = aux1;
                aux2 = temp1;
                // ···········································

                //Buscamos donde deve entrar el nuevo nodo
                while ( (aux1 != nullptr) && (aux1->dato < valor)){
                    aux2 = aux1;
                    aux1 = aux1->next;
                }

                //bloqueamos actual y anterior
                candadoAnterior.lock();
                candadoActual.lock();
                
                //CASO 1
                if(aux1 == nullptr){
                    //head = newNodo; // Insertamos
                    // Desbloquamos
                    candadoAnterior.unlock();
                    candadoActual.unlock();
                    return false;
                }   

                // ---------------- VERIFICACION DE THREADS ----------------
                if(aux1->flag == true && aux2->flag == true){ // verifica el flag de ambos punteros
                    if(aux1->dato != valor){
                        candadoAnterior.unlock();
                        candadoActual.unlock();
                        return false;
                    }

                    //Primer dato
                    if(aux1 == head){
                        head = aux1->next;
                        aux2->next = head;
                        candadoAnterior.unlock();
                        candadoActual.unlock();
                    }
                    else{
                        aux2->next = newNodo;
                    }

                    //newNodo->next = aux1;
                    aux2->next = aux1->next;
                    delete aux1;

                    candadoAnterior.unlock();
                    candadoActual.unlock();
                    return true;
                }

                candadoAnterior.unlock();
                candadoActual.unlock();
                // ----------------------------------------------------------

                //newNodo->next = aux1;
                //candado.unlock();
                cout << "Nodo "<< valor << " Eliminado" << endl;
                //return true;
            }
        }

        void print(){
            Node<T> *aux = head;
            cout << "---------- Lista ------------- " << endl;
            cout << "| ";
            while (aux != nullptr)
            {
                cout << aux->dato << " | -> |";
                aux = aux->next; 
            }
            cout << endl;
        }
};

template <class T>
struct Add
{
    Lista<T> *queue;
    int valMin, valMax;
    Add(Lista<T> &_queue,int _valMin,int _valMax)
    {
        queue = &_queue;
        valMin = _valMin;
        valMax = _valMax;
    }
    
    void operator()(int n_operations)
    {
        for(int i = 0; i < n_operations; i++)
        {
            int number = valMin+rand()%(valMax);
            //cout << "Añadiendo: " << number << endl;
            queue->addValue(number);
        }
    }
};

template <class T>
struct Delete
{
    Lista<T> *queue;
    int valMin, valMax;
    Delete(Lista<T> &_queue,int _valMin,int _valMax)
    {
        queue = &_queue;
        valMin = _valMin;
        valMax = _valMax;
    }
    
    void operator()(int n_operations)
    {
        for(int i=0; i < n_operations; i++)
        {
            int number = valMin+rand()%(valMax);
            //cout << "Eliminando: " << number << endl;
            queue->deleteValue(number);
        }
    }
};


int main(){

    /*
    // Version 1
    queue.addValue(10);
    queue.addValue(20);
    queue.addValue(5);
    queue.addValue(50);
    queue.addValue(7);
    queue.addValue(12);

    queue.print();

    queue.deleteValue(50);
    queue.print();
    queue.deleteValue(5);
    queue.print();
    queue.deleteValue(20);
    queue.print();

    queue.conteinsSerch(5);
    queue.conteinsSerch(7);
    */

    /*
    //Version 2
    Lista queue;

    Add op1(queue,1,100);   
    op1(20); 
    queue.print();

    Delete op2(queue,1,100);
    op2(20);
    queue.print();
    */


    cout << "----- Usando Threads: -------- " << endl;

    srand(time(NULL));
    thread* threads[2];
    Lista<int> queue;
    
    Add<int> opt1(queue, 1,1000);
    thread first(opt1,1000);

    Delete<int> opt2(queue, 1, 1000);
    thread second(opt2, 1000);

    threads[0] = &first;
    threads[1] = &second;

    threads[0]->join();
    threads[1]->join();

    queue.print();

    return 0;
}