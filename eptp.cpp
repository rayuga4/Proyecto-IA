// Algoritmo Evolutivo

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

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

int eval(Individuo ind, Instancia ins, Usuario us, int gen){
    int curr;
    int next;
    int val = 0;
    int t = 0;
    bool factibilidad = true;
    int pen = max(gen/50,10)*1500+1000;
    for (int i=0;i<(ind.cromosoma.size()-1);i++){
        curr = ind.cromosoma[i];
        next = ind.cromosoma[i+1];
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

int solve(Instancia instancia, Usuario usuario){
    cout<<"Matriz de adyacencia"<<endl;
    printArray(instancia.adj,instancia.n,instancia.n);
    cout<<"Matriz de preferencia"<<endl;
    printArray(usuario.c,instancia.n,instancia.n);
    return 0;
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
    string fname;
    fname = ss.str();
    int u;
    cout << "Ingrese numero de usuarios" << endl;
    cin >> u;
    stringstream ss2;
    ss2 << "Instancias/usuarios/" << n << "_instancia_" << m << "_" << u << "us.txt"; 
    string fname2;
    fname2 = ss2.str();
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
        solve(instancia, usuario);

    }
}