//
//  main.cpp
//  SkinSegmentation
//
//  Created by Sasha Nicolas Da Rocha Pinheiro on 10/22/14.
//  Copyright (c) 2014 Sasha Nicolas Da Rocha Pinheiro. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>

#define NUM_CLASSES 5
#define ERROR_FCM 0.0005
#define FUZZIFIER 2
//comentario
using namespace std;

struct color{
    float r, g, b, y;
};
typedef struct color Color;

struct vec3 {
    float x, y, z;
};
typedef struct vec3 Vec3;

float norma(Vec3 a, Vec3 b){
    return sqrt(
            pow(a.x - b.x, 2) +
            pow(a.y - b.y, 2) +
            pow(a.z - b.z, 2)
        );
}
float normaN(vector<float> a, vector<float> b){
    float soma=0;
    for (int i=0; i<a.size(); i++) {
        soma += pow(a.at(i) - b.at(i), 2);
    }
    return sqrt( soma );
}

int main(int argc, const char * argv[]) {
    ifstream dataset_file("Skin_NonSkin.txt", ios::in);
    vector< Color > data;
    
//    vector<float> a = {3,0};
//    vector<float> b = {0,4};
//    cout << normaN(a,b) << endl;
//    return 0;
    
    //abrir o arquivo
    if(dataset_file){
        cout << "Achou o arquivo.\n";
        //listar os pontos RGB do arquivo
        
        string line;
//        int i=50000;
        while( getline(dataset_file, line))
        {
            Color cor;
            istringstream is( line );
            is >> cor.b >> cor.g >> cor.r >> cor.y;
            //cout << cor.b << " " << cor.g << " " << cor.r << " " << cor.y << endl;
            data.push_back(cor);
        }
    }else{ //if ler o aquivo
        cout << "Nao achou o arquivo\n";
        return 0;
    }//if ler o aquivo
    //*/
        
    //teste com pontos controlados
    /*for (int i=0; i<10; i++) {
        Color cor;
        cor.r = sin((36 * i)*M_PI/180);
        cor.g = cos((36 * i)*M_PI/180);
        cor.b = 0;
        data.push_back(cor);
    }
    for (int i=0; i<10; i++) {
        Color cor;
        cor.r = sin((36 * i)*M_PI/180);
        cor.g = cos((36 * i)*M_PI/180);
        cor.b = 10;
        data.push_back(cor);
    }
    for(int i = 0; i<20; i++)
    {
        
        cout << "r=" << data.at(i).r << " g=" << data.at(i).g << " b=" << data.at(i).b << endl;
        
    }//*/
    //end - teste com pontos controlados
    
    //end - arquivo carregado em data
    
    //Fuzzy c-means (FCM)
    int N = (int) data.size();
    vector< vector<float> > U; //matriz de graus de associacao (degree of membership)
    vector< vector<float> > U_anterior;
    
    //instanciar com valores aleatorios a matriz U, probabilidades de ser do cluster j
    //passo 1
    for (int i=0; i<N ; i++) {
        vector<float> novo;
        U.push_back(novo);
        for (int j=0; j < NUM_CLASSES; j++) {
            float r = ((double) rand() / (RAND_MAX));
            U.at(i).push_back(r);
        }
    }
    
    bool convergiu = false;
    int step_k = 0;
    vector< vector<float> > C;
    
    while (!convergiu) {
        cout << step_k << endl;
        C.clear();
        //passo 2 - calcular os centróides de cada NUM_CLASSES cluster
        for (int j=0; j < NUM_CLASSES; j++) {
            //calculo do denominador - soma da coluna j
            float soma_dem = 0;
            for (int i=0; i<N ; i++) {
                soma_dem += pow(U.at(i).at(j), FUZZIFIER);
            }
            //calcula os numeradores
            float soma_Cr = 0, soma_Cg = 0, soma_Cb = 0;
            for (int i=0; i<N ; i++) {
                float uij_fuzz = pow(U.at(i).at(j), FUZZIFIER);
                soma_Cr += uij_fuzz * data.at(i).r;
                soma_Cg += uij_fuzz * data.at(i).g;
                soma_Cb += uij_fuzz * data.at(i).b;
            }
            //CjR, CjG, CjB
            vector<float> centroid;
            centroid.push_back(soma_Cr/soma_dem);
            centroid.push_back(soma_Cg/soma_dem);
            centroid.push_back(soma_Cb/soma_dem);
            C.push_back(centroid);
        }
        
        cout << "Imprime os U:"<<endl;
        for(int i = 0; i<50000; i+=2000)
        {
            for(int j = 0; j<NUM_CLASSES; j++)
            {
                
                cout << U.at(i).at(j) << " ";
            }
            cout << endl;
            cout << "ponto corr: b=" << data.at(i).b << " g=" << data.at(i).g << " r=" << data.at(i).r << endl;

        }//*/
        
        //passo 3 - update de U(k), U(k+1)
        U_anterior.clear();
        U_anterior.swap(U);
        float expoente = 2./(FUZZIFIER-1);
        
        for (int i=0; i<N ; i++) {
            vector<float> novo;
            U.push_back(novo);
            for (int j=0; j < NUM_CLASSES; j++) {
                Vec3 Xi = { (float) data.at(i).r, (float) data.at(i).g, (float) data.at(i).b};
                Vec3 Cj = { (float) C.at(j).at(0), (float) C.at(j).at(1), (float) C.at(j).at(2)};
                float numerador = norma(Xi,Cj);
                float soma_dem = 0;
                for (int k=0; k < NUM_CLASSES; k++) {
                    Vec3 Ck = { (float) C.at(k).at(0), (float) C.at(k).at(1), (float) C.at(k).at(2)};
                    float denominador = norma(Xi,Ck);
                    soma_dem +=  pow( numerador/denominador, expoente);
                }
                U.at(i).push_back(1/soma_dem);
            }
        }
        
        cout << "Imprime os centros:"<<endl;
        for(int i = 0; i<NUM_CLASSES; i++)
        {
            cout << "r=" << C.at(i).at(0) << " g=" << C.at(i).at(1) << " b=" << C.at(i).at(2) << endl;
            
        }
        cout << endl;//*/
        
        //for for IF error
        bool algum_maior = false;
        for (int i=0; i<N ; i++) {
            if( normaN (U.at(i), U_anterior.at(i))  > ERROR_FCM){
                algum_maior = true;
                break;
            }
        }
        if(!algum_maior) convergiu = true;
        
        step_k++;
    }
    cout << "Imprime os centros finais:"<<endl;
    for(int i = 0; i<NUM_CLASSES; i++)
    {
        cout << "r=" << C.at(i).at(0) << " g=" << C.at(i).at(1) << " b=" << C.at(i).at(2) << endl;
        
    }
    return 0;
}




