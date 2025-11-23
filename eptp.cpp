// Algoritmo Evolutivo

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <unordered_set>
#include <iterator>
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
    bool operator > (const Individuo& a) const {
        return (aptitud>a.aptitud);
    }
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
    } else {
        vector<int> available(n-1);
        iota(available.begin(),available.end(),1);
        shuffle(available.begin(),available.end(),rng);
        ind.cromosoma = vector<int>(available.begin(),available.begin()+leng);
    }
    return ind;
}

Individuo cruce(Individuo p1, Individuo p2){
    int l1 = p1.cromosoma.size();
    int l2 = p2.cromosoma.size();
    int lenMin = min(l1,l2);
    int lenMax = max(l1,l2);
    uniform_int_distribution<int> lenDist(lenMin,lenMax);
    mt19937 rng;
    rng.seed(chrono::high_resolution_clock::now().time_since_epoch().count());
    int target = lenMax;
    int hijo[target] = {0};
    uniform_int_distribution<int> pos(0,l1);
    int a = pos(rng);
    int b = pos(rng);
    unordered_set<int> usado;
    for (int i = min(a,b);i<max(a,b);i++){
        hijo[i] = p1.cromosoma[i];
        usado.insert(hijo[i]);
    }
    int currPos = 0;
    if (currPos == min(a,b)){
        currPos = max(a,b);
    }
    for (int nodo:p2.cromosoma){
        if (currPos == target-1){
            break;
        }
        if (usado.find(nodo)==usado.end()){
            hijo[currPos] = nodo;
            currPos+=1;
            if (currPos == min(a,b)){
                currPos = max(a,b);
            }
            usado.insert(nodo);
        }
    }
    if (currPos<target-1){
        for (int nodo:p1.cromosoma){
            if (currPos == target-1){
                break;
            }
            if (usado.find(nodo)==usado.end()){
                hijo[currPos] = nodo;
                currPos+=1;
                if (currPos == min(a,b)){
                    currPos = max(a,b);
                }
                usado.insert(nodo);
            }
        }
    }
    vector<int> hijoVec(hijo,hijo+sizeof(hijo)/sizeof(hijo[0]));
    Individuo aux;
    aux.cromosoma = hijoVec;
    aux.evaluado = false;
    return aux;
}

Individuo mutacion(Individuo ind){
    mt19937 rng;
    rng.seed(chrono::high_resolution_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dis(0,ind.cromosoma.size()-1);
    int a = dis(rng);
    int b = dis(rng);
    while (b==a){
        b = dis(rng);
    }
    reverse(ind.cromosoma.begin()+min(a,b),ind.cromosoma.begin()+max(a,b));
    ind.evaluado = false;
    return ind;
}

vector<float> pesoRuleta(vector<Individuo> pob){
    vector<float> probs;
    vector<Individuo> auxPob = pob;
    float suma = 0.0;
    int aux = pob.size();
    int minFit = 1000000;
    for (int i=0;i<aux;i++){
        if (pob[i].aptitud<minFit){
            minFit = pob[i].aptitud;
        }
    }
    if (minFit<0){
        for (int i=0;i<aux;i++){
            pob[i].aptitud = pob[i].aptitud-minFit+500;
        }
    }
    for (int i=0;i<aux;i++){
        suma+=pob[i].aptitud;
    }
    for (int i=0;i<aux;i++){
        probs.push_back(pob[i].aptitud/suma);
    }
    return probs;
}

vector<int> selPoblacion(Poblacion pob, int total){
    vector<int> seleccionados;
    vector<float> probs = pesoRuleta(pob.poblacion);
    vector<float> probAcumulado;
    float acumulado = 0.0;
    for (float prob: probs){
        acumulado+=prob;
        probAcumulado.push_back(acumulado);
    }
    mt19937 rng;
    rng.seed(chrono::high_resolution_clock::now().time_since_epoch().count());
    uniform_real_distribution<float> dis(0.0,acumulado);
    for (int i=0;i<total;i++){
        float r = dis(rng);
        for (int j=0;j<(int)probAcumulado.size();j++){
            if (r<=probAcumulado[j]){
                seleccionados.push_back(j);
                break;
            }
        }
    }
    return seleccionados;
}

Poblacion transformacion(Poblacion pob, int total, float pc = 0.9, float pm = 0.3){
    sort(pob.poblacion.begin(),pob.poblacion.end(),greater<Individuo>());
    vector<Individuo> elite;
    int elites = (int)total*0.2;
    for (int i = 0;i<elites;i++){
        elite.push_back(pob.poblacion[i]);
    }
    vector<int> padres = selPoblacion(pob, total-elites);
    Poblacion nueva;
    mt19937 rng;
    rng.seed(chrono::high_resolution_clock::now().time_since_epoch().count());
    uniform_real_distribution<float> dis(0.0,1.0);
    for (int i=0;i<(int)(total-elites)/2;i++){
        float r = dis(rng);
        Individuo a = pob.poblacion[padres[i*2]];
        Individuo b = pob.poblacion[padres[i*2+1]];
        if (r<=pc){
            nueva.poblacion.push_back(cruce(a,b));
            nueva.poblacion.push_back(cruce(b,a));
        } else {
            nueva.poblacion.push_back(a);
            nueva.poblacion.push_back(b);
        }
    }
    for (int i = 0;i<(total-elites);i++){
        float r = dis(rng);
        if (r<=pm){
            nueva.poblacion[i] = mutacion(nueva.poblacion[i]);
        }
    }
    for (Individuo ind:elite){
        nueva.poblacion.push_back(ind);
    }
    return nueva;
}

int eval(Individuo ind, Instancia ins, Usuario us, int gen){
    int curr;
    int next;
    int val = 0;
    int t = 0;
    bool factibilidad = true;
    int pen = min(gen/10,10)*750+500;
    vector<int> tour = ind.cromosoma;
    tour.push_back(0);
    tour.insert(tour.begin(),0);
    for (int i=0;i<(int)(tour.size()-1);i++){
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
    /*cout<<"Matriz de adyacencia"<<endl;
    printArray(instancia.adj,instancia.n,instancia.n);
    cout<<"Matriz de preferencia"<<endl;
    printArray(usuario.c,instancia.n,instancia.n);*/
    Poblacion pob;
    pob.mejorAptitud = 0;
    int tamMuestra = 1000;
    int ultimoFactible = 0;
    for (int longitud=1;longitud<instancia.n;longitud++){
        if (longitud>instancia.n*0.8){
            break;
        }
        int factibles = 0;
        for (int i =0;i<tamMuestra;i++){
            Individuo ind = generarIndAleatorio(instancia.n,0,longitud);
            ind.aptitud = eval(ind,instancia,usuario,0);
            if (ind.aptitud>0){
                factibles++;
                if (ind.aptitud>pob.mejorAptitud){
                    pob.mejorAptitud = ind.aptitud;
                    pob.mejorCromosoma = ind.cromosoma;
                }
            }
        }
        cout<<"Longitud "<<longitud<<": "<<factibles<<" factible"<<endl;
        if (factibles>0){
            ultimoFactible = longitud;
        } else {
            break;
        }
    }
    int minL = max(1,ultimoFactible-1);
    int maxL = min(instancia.n,ultimoFactible+1);
    
    int total = 100;
    int maxInt = 1000;
    int intentos = 0;
    while ((int)pob.poblacion.size()<total&&intentos<maxInt){
        Individuo ind;
        int longitud = minL+ rand()%(maxL-minL+1);
        ind = generarIndAleatorio(instancia.n,0.6,longitud);
        ind.aptitud = eval(ind,instancia,usuario,0);
        if (ind.aptitud>0){
            if (ind.aptitud>pob.mejorAptitud){
                pob.mejorAptitud = ind.aptitud;
                pob.mejorCromosoma = ind.cromosoma;
            }
            ind.evaluado = true;
            pob.poblacion.push_back(ind);
        }
    }
    while ((int)pob.poblacion.size()<total){
        Individuo ind;
        int longitud = minL+ rand()%(maxL-minL+1);
        ind = generarIndAleatorio(instancia.n,0.6,longitud);
        ind.aptitud = eval(ind,instancia,usuario,0);
        if (ind.aptitud>pob.mejorAptitud){
            pob.mejorAptitud = ind.aptitud;
            pob.mejorCromosoma = ind.cromosoma;
        }
        ind.evaluado = true;
        pob.poblacion.push_back(ind);
    }
    int cont = 0;
    for (int i=0;i<100;i++){
        Poblacion nueva = transformacion(pob,total);
        bool cambio = false;
        for (int j = 0;j<total;j++){
            if (nueva.poblacion[j].evaluado == false){
                nueva.poblacion[j].aptitud = eval(nueva.poblacion[j],instancia,usuario,i);
                nueva.poblacion[j].evaluado = true;
                if (nueva.poblacion[j].aptitud>pob.mejorAptitud){
                    pob.mejorAptitud = nueva.poblacion[j].aptitud;
                    pob.mejorCromosoma = nueva.poblacion[j].cromosoma;
                    cambio = true;
                }
            }
        }
        if (!cambio){
            cont++;
        } else {
            cont = 0;
        }
        if (cont>9){
            break;
        }
        pob.poblacion = nueva.poblacion;
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
    for (int i=0;i<(int)(tour.size()-1);i++){
        curr = tour[i];
        next = tour[i+1];
        t += ins.adj[curr][next];
        if (t<ins.td[next][0]){
            t = ins.td[next][0];
        }
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
    auto globStart = chrono::system_clock::now();
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
        auto startTime = chrono::system_clock::now();
        Individuo mejor = solve(instancia, usuario);
        auto endTime = chrono::system_clock::now();
        auto gap = chrono::duration_cast<chrono::milliseconds>(endTime-startTime).count();
        int tiempo = tiempoUsado(mejor.cromosoma,instancia);
        if (tiempo==-1||tiempo>usuario.T){
            cout<<"Error"<<endl;
            continue;
        }
        solucion<<mejor.aptitud<<endl;
        solucion<<usuario.T<<" "<<tiempo<<endl;
        solucion<<"1 ";
        for (int i=0;i<(int)mejor.cromosoma.size();i++){
            solucion<<mejor.cromosoma[i]+1<<" ";
        }
        solucion<<"1"<<endl;
        cout<<"Mejor tour con valoracion de "<<mejor.aptitud<<endl;
        for (int i=0;i<(int)mejor.cromosoma.size();i++){
            cout<<mejor.cromosoma[i]<<" ";
        }
        cout<<endl;
        cout<<"Tiempo utilizado: "<<gap/1000.0<<"s"<<endl;
    }
    auto globEnd = chrono::system_clock::now();
    auto gap = chrono::duration_cast<chrono::milliseconds>(globEnd-globStart).count();
    cout<<"Instancia resuelta en "<<gap/1000.0<<"s"<<endl;
}