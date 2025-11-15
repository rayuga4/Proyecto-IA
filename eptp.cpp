// Algoritmo Evolutivo

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

using namespace std;

struct Instancia {
    int n;
    vector<int> t;
    vector<vector<int>> td;
    vector<vector<int>> adj;
};

struct Usuario {
    int T;
    vector<int> s;
    vector<vector<int>> c;
};

struct Individuo {
    int aptitud;
    bool evaluado;
    vector<int> cromosoma;
};

struct Poblacion {
    vector<Individuo> poblacion;
    vector<int> mejorCromosoma;
    int mejorAptitud;
};

void printArray(vector<vector<int>> arr, int n, int m){
    for (int i=0;i<n;i++){
        for (int j=0;j<m;j++){
            cout<<arr[i][j]<< " ";
        }
        cout<<endl;
    }
}

Individuo generarIndAleatorio(int n, float prob = 0.6, int leng = -1){
    mt19937 rng;
    rng.seed(chrono::high_resolution_clock::now().time_since_epoch().count());
    Individuo ind;
    if (leng == -1){
        while (ind.cromosoma.size()==0){
            for (int i = 1;i<n;i++){
            uniform_real_distribution<float> dist(0.0,1.0);
                if (dist(rng)<prob){
                    ind.cromosoma.push_back(i);
                }
            }
        }
        shuffle(ind.cromosoma.begin(),ind.cromosoma.end(),rng);
    }
    return ind;
}

Individuo cruce(Individuo p1, Individuo p2){

}

int eval(Individuo ind, Instancia ins, Usuario us, int gen){
    int curr;
    int next;
    int val = 0;
    int t = 0;
    bool factibilidad = true;
    int pen = max(gen/50,10)*1500+1000;
    vector<int> tour = ind.cromosoma;
    tour.push_back(0);
    tour.insert(tour.begin(),0);
    for (int i=0;i<(tour.size()-1);i++){
        curr = tour[i];
        next = tour[i+1];
        t += ins.adj[curr][next];
        if (t<ins.td[next][0]){
            t = ins.td[next][0];
        }
        if ((t+ins.t[next])>ins.td[next][1]){
            factibilidad = false;
            break;
        }
        val = val + us.s[next] + us.c[curr][next];
        t += ins.t[next];
        if (t>us.T){
            factibilidad = false;
            break;
        }
    }
    if (factibilidad) {
        return val;
    } else {
        return val - pen;
    }
}

Individuo solve(Instancia instancia, Usuario usuario){
    cout<<"Matriz de adyacencia"<<endl;
    printArray(instancia.adj,instancia.n,instancia.n);
    cout<<"Matriz de preferencia"<<endl;
    printArray(usuario.c,instancia.n,instancia.n);
    Poblacion pob;
    pob.mejorAptitud = 0;
    for (int i=0;i<100;i++){
        Individuo ind;
        if (i<25){
            ind = generarIndAleatorio(instancia.n,0.4);
        } else if (i<50){
            ind = generarIndAleatorio(instancia.n,0.5);
        } else if (i<75){
            ind = generarIndAleatorio(instancia.n);
        } else {
            ind = generarIndAleatorio(instancia.n,0.7);
        }
        ind.aptitud = eval(ind,instancia,usuario,0);
        if (ind.aptitud>pob.mejorAptitud){
            pob.mejorAptitud = ind.aptitud;
            pob.mejorCromosoma = ind.cromosoma;
        }
        ind.evaluado = true;
        pob.poblacion.push_back(ind);
    }
    Individuo mejor;
    mejor.aptitud = pob.mejorAptitud;
    mejor.cromosoma = pob.mejorCromosoma;
    return mejor;
}

int tiempoUsado(vector<int> tour, Instancia ins){
    int curr;
    int next;
    int t = 0;
    tour.push_back(0);
    tour.insert(tour.begin(),0);
    for (int i=0;i<(tour.size()-1);i++){
        curr = tour[i];
        next = tour[i+1];
        t += ins.adj[curr][next];
        if (t<ins.td[next][0]){
            t = ins.td[next][0];
        }
//        if ((t+ins.t[next])>ins.td[next][1]){
//            return -1;
//        }
        t += ins.t[next];
    }
    return t;
}

int main(){

    // Eleccion de instancia
    cout << "Ingrese cantidad de nodos de la instancia" << endl;
    int n;
    int m;
    cin >> n;
    cout << "Ingrese numero de la instancia" << endl;
    cin >> m;
    stringstream ss;
    ss << "Instancias/nodos/" << n << "_instancia_" << m << ".txt";
    string fname = ss.str();
    int u;
    cout << "Ingrese numero de usuarios" << endl;
    cin >> u;
    stringstream ss2;
    ss2 << "Instancias/usuarios/" << n << "_instancia_" << m << "_" << u << "us.txt"; 
    string fname2 = ss2.str();
    cout << fname << endl << fname2 << endl;

    // Lectura de nodos
    ifstream nodoF(fname);
    string aux;
    getline(nodoF,aux);
    getline(nodoF,aux);
    vector<int> t(n);
    stringstream linT(aux);
    for (int i = 0;i<n;i++){
        linT >> t[i];
    }
    getline(nodoF,aux);
    vector<vector<int>> td(n,vector<int>(2));
    stringstream linTd(aux);
    for (int i = 0;i<n;i++){
        linTd >> td[i][0] >> td[i][1];
    }
    stringstream linMat;
    vector<vector<int>> adj(n,vector<int>(n));
    for (int i = 0;i<n;i++){
        getline(nodoF,aux);
        linMat.str("");
        linMat.clear();
        linMat << aux;
        for (int j = 0; j<n;j++){
            linMat >> adj[i][j];
        }
    }
    nodoF.close();
    Instancia instancia = {n,t,td,adj};

    // Lectura y procesamiento de usuarios
    stringstream ss3;
    ss3<<"sol_"<<n<<"_"<<m<<".txt";
    string fname3 = ss3.str();
    ofstream solucion(fname3);
    ifstream usuarioF(fname2);
    getline(usuarioF,aux);
    for (int i = 0;i<u;i++){
        cout<<"Usuario "<<i<<endl;
        int T;
        getline(usuarioF,aux);
        T = stoi(aux);
        getline(usuarioF,aux);
        stringstream linS(aux);
        vector<int> s(n);
        for (int i = 0;i<n;i++){
            linS >> s[i];
        }
        stringstream linC;
        vector<vector<int>> c(n,vector<int>(n));
        for (int i = 0;i<n;i++){
            getline(usuarioF,aux);
            linC.str("");
            linC.clear();
            linC << aux;
            for (int j = 0; j<n;j++){
                linC >> c[i][j];
            }
        }
        Usuario usuario = {T, s, c};
        Individuo mejor = solve(instancia, usuario);
        int tiempo = tiempoUsado(mejor.cromosoma,instancia);
        if (tiempo==-1||tiempo>usuario.T){
            cout<<"Error"<<endl;
            continue;
        }
        solucion<<mejor.aptitud<<endl;
        solucion<<usuario.T<<" "<<tiempo<<endl;
        solucion<<"1 ";
        for (int i=0;i<mejor.cromosoma.size();i++){
            solucion<<mejor.cromosoma[i]+1<<" ";
        }
        solucion<<"1"<<endl;
        cout<<"Mejor tour con valoracion de "<<mejor.aptitud<<endl;
        for (int i=0;i<mejor.cromosoma.size();i++){
            cout<<mejor.cromosoma[i]<<" ";
        }
        cout<<endl;
    }
}