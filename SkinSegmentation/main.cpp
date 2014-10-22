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

using namespace std;

struct color{
    unsigned short r, g, b, y;
};
typedef struct color Color;

#define NUM_CLASSES 5
#define ERROR_FCM 0.7
#define FUZZIFIER 2

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

int main(int argc, const char * argv[]) {
    ifstream dataset_file("Skin_NonSkin.txt", ios::in);
    vector< Color > data;
    
    //abrir o arquivo
    if(dataset_file){
        cout << "Achou o arquivo.\n";
        //listar os pontos RGB do arquivo
        
        string line;
        while( getline(dataset_file, line) )
        {
            Color cor;
            istringstream is( line );
            is >> cor.b >> cor.g >> cor.r >> cor.y;
            //cout << cor.b << " " << cor.g << " " << cor.r << " " << cor.y << endl;
            data.push_back(cor);
        }
        
        /*vector< Color >  new_;
        new_.swap(data);
        cout << data.size() << endl;
        for(int i = 0; i<new_.size();i++)
        {
            cout << new_.at(i).b << " " << new_.at(i).g << " " << new_.at(i).r << " " << new_.at(i).y << endl;
        }//*/
        
    }else{ //if ler o aquivo
        cout << "Nao achou o arquivo\n";
        return 0;
    }//if ler o aquivo
    
    //arquivo carregado em data
    
    //Fuzzy c-means (FCM)
    int N = (int) data.size();
    vector< vector<float> > U; //matriz de graus de associacao (degree of membership)
    vector< vector<float> > U_anterior;
    
    //instanciar com valores aleatorios a matriz U
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
        //passo 3 - update de U(k), U(k+1)
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
        
        //for for IF error
        bool algum_maior = false;
        for (int i=0; i<N ; i++) {
            for (int j=0; j < NUM_CLASSES; j++) {
                if(abs(U.at(i).at(j) - U_anterior.at(i).at(j)) > ERROR_FCM){
                    algum_maior = true;
                    break;
                }
            }
            if(algum_maior){
                break;
            }
        }
        if(!algum_maior) convergiu = true;
        
        step_k++;
    }
    
    return 0;
}




