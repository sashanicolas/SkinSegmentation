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
#include <algorithm>

#include "CImg.h"

#define NUM_CLASSES 4
#define ERROR_FCM 0.0005

#define FUZZIFIER 2
//#define M_PI 3.14159265358979323846 
#define SIGMA 10.
  
using namespace std;
using namespace cimg_library;

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

void gaussianFunction(CImg<unsigned char>& image, float mi, float sigma, int corR, int corG, int corB){

	int h = image.height();
	int w = image.width();
	for (int i = 0; i < w; i++){
		float j = (1. / (sigma*sqrt(2 * M_PI)))* exp(-0.5*pow((i-mi)/sigma,2));
		int y = h - j*1000;
		image(i, y,0) = corR;
		image(i, y, 1) = corG;
		image(i, y, 2) = corB;
	}
	// resolver problema de display da Cimg
	for (int i = 0; i < w; i++){
		image(i, 0, 0) = 255;
		image(i, 0, 1) = 255;
		image(i, 0, 2) = 255;
	}
}

int main(int argc, const char * argv[]) {

    ifstream dataset_file("Skin_NonSkin.txt", ios::in);
    vector< Color > data;
        
	//Create empty images
	/*CImg<unsigned char> image_r(255, 100, 1, 3, 255);
	CImg<unsigned char> image_g(255, 100, 1, 3, 255);
	CImg<unsigned char> image_b(255, 100, 1, 3, 255);*/
	
    //abrir o arquivo
    if(dataset_file){
        cout << "Achou o arquivo.\n";
        //listar os pontos RGB do arquivo
        
        string line;
        //int i=50000;
        while( getline(dataset_file, line) )
        {
            Color cor;
            istringstream is( line );
            is >> cor.b >> cor.g >> cor.r >> cor.y;
            //cout << cor.b << " " << cor.g << " " << cor.r << " " << cor.y << endl;

			//if (cor.y == 1){ // skin only
				data.push_back(cor);
			//}

        }
    }else{ //if ler o aquivo
        cout << "Nao achou o arquivo\n";
        return 0;
    }//if ler o aquivo
    //*/

	/*
	 * Gerar asquivos de treino e teste
	 *
	random_shuffle(data.begin(), data.end());
	random_shuffle(data.begin(), data.end());
	random_shuffle(data.begin(), data.end());
	//treino
	ofstream training_data_file("training_data.txt", ios::out);
	if (training_data_file) {
		cout << "Output file training_data.txt created." << endl;
		training_data_file << "200000 3" << endl;
		for (int i = 0; i<200000; i++){
			training_data_file << data.at(i).r << " " << data.at(i).g << " " << data.at(i).g << (data.at(i).y==1?" Skin 1":" Nonskin 1") << endl;
		}
		training_data_file.close();  // on referme le fichier
	}
	else cout << "Output file training_data.txt creation FAILED." << endl;
	
	//teste
	ofstream testing_data_file("testing_data.txt", ios::out);
	if (testing_data_file) {
		cout << "Output file testing_data_file.txt created." << endl;
		testing_data_file << "" << data.size() - 200000 << " 3" << endl;
		for (int i = 200000; i<data.size(); i++){
			testing_data_file << data.at(i).r << " " << data.at(i).g << " " << data.at(i).g << (data.at(i).y == 1 ? " Skin 1" : " Nonskin 1") << endl;
		}
		testing_data_file.close();  // on referme le fichier
	}
	else cout << "Output file testing_data_file.txt creation FAILED." << endl;

	return 0;*/
        
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
    cout << "Quantidade de entradas no dataset: "<< N << endl;
	//int N = 10000;
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
        
        /*cout << "Imprime os U:"<<endl;
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
        
        /*cout << "Imprime os centros:"<<endl;
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
        
    }//*/

	//display gaussians
	//int colorC1[3] = { 0, 0, 0 }; // black
	//int colorC2[3] = { 0, 255, 0 }; // green
	//int colorC3[3] = { 0, 0, 255 }; //blue
	//int colorC4[3] = { 255, 0, 0 };// red
	//int colorC5[3] = { 255, 0, 255 }; // ? something in between
	//vector<int*> colors;
	//colors.push_back(colorC1);
	//colors.push_back(colorC2);
	//colors.push_back(colorC3);
	//colors.push_back(colorC4);
	//colors.push_back(colorC5);
	//for (int i = 0; i < NUM_CLASSES; i++){
	//	float mi = C.at(i).at(0); // r
	//	gaussianFunction(image_r, mi, SIGMA, colors[i][0], colors[i][1], colors[i][2]);
	//	mi = C.at(i).at(1); // g
	//	gaussianFunction(image_g, mi, SIGMA, colors[i][0], colors[i][1], colors[i][2]);
	//	C.at(i).at(0); // b
	//	gaussianFunction(image_b, mi, SIGMA, colors[i][0], colors[i][1], colors[i][2]);
	//}
	//(image_r, image_g, image_b).display();

	// Write new archives :
	// --> Centroid centers
	
	ofstream centroid_file("centroid_4_centers.txt", ios::out);
	if (centroid_file) {
		cout << "Output file centroid_4_centers.txt created." << endl;
		for (int i = 0; i<NUM_CLASSES; i++){
			centroid_file << C.at(i).at(0) << " " << C.at(i).at(1) << " " << C.at(i).at(2) << endl;
		}
		centroid_file.close();  // on referme le fichier
	}
	else cout << "Output file centroid_4_centers.txt creation FAILED." << endl;

	//Write results in .txt files
	vector<string> files;
    
    files.push_back("points_class_1_4_centroids.txt");
    files.push_back("points_class_2_4_centroids.txt");
    files.push_back("points_class_3_4_centroids.txt");
    files.push_back("points_class_4_4_centroids.txt");


	//if (points_class_1_file) {
		for (int i = 0; i < U.size(); i++){
			float max = 0;
			int target_class = 0;
			for (int j = 0; j < NUM_CLASSES; j++){
				if (U.at(i).at(j) > max){
					max = U.at(i).at(j);
					target_class = j;
				}
			}
			ofstream file(files.at(target_class).c_str(), ios::app);
			if (file) {
				file << data.at(i).r << " " << data.at(i).g << " " << data.at(i).b << endl;
			}
			else cout << "FAILED to write in file points_class_" << max <<"_centers.txt"<< endl;
		}
	
    
    //escrever o data.attrs
    ofstream dataAttrs("data.attrs", ios::app);
    if(dataAttrs){
        
    }
    
    return 0;
}




