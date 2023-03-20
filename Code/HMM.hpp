#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <limits>
#include "Player.hpp"
#include <algorithm>
#include <random>
namespace ducks {


    class HMM{

    public:
        static const int num_state = 2;
        static const int num_Ob    = 9;
        
        float A[2][2];
        float B[2][9];
        float P[1][2];
        bool converge;
        int T;
        int *observation;
        float Probnobservation = 0;
        int nobservation = 0;
        bool firstBuild = 0;

        void Initialization(const Bird bird){
            if (firstBuild){
                delete alphas;
                delete betas;
                delete gammas;
                delete di_gammas;
            }
            
            float sum = 0;
            // A
            for(int i = 0; i<num_state;i++){
                sum = 0;
                for(int j = 0; j< num_state;j++){
                    A[i][j] = (rand()%10)+1;
                    sum += A[i][j];
                }
                for(int j = 0; j< num_state;j++){
                    A[i][j] /= sum;
                    
                }

            }
            // b
            for(int i = 0; i<num_state;i++){
                sum = 0;
                for(int j = 0; j< num_Ob;j++){
                    B[i][j] = (rand()%10)+1;
                    sum += B[i][j];
                }
                for(int j = 0; j< num_Ob;j++){
                    B[i][j] /= sum;
                    
                }

            }
            // P
            sum = 0;
            for(int i = 0; i< num_state;i++){
                P[0][i] = (rand()%10)+1;
                sum+=P[0][i];
            }
            for(int i = 0; i< num_state;i++){
                P[0][i] /= sum;
                //sum+=P[0][i];
            }
            if(bird.isDead())
            {
            for(int i=0;i<bird.getSeqLength();++i)
                if(bird.getObservation(i) == -1)
                {
                    T = i;
                    break;
                }
            }
            else{
                T = bird.getSeqLength();
            }

            //T = bird.getSeqLength();
            observation = new int[T];
            ci = new float [T];
            // Initialize alphas , betas and gammas
            alphas = new float*[T];
            betas = new float*[T];
            gammas = new float*[T];
            for(int i = 0; i<T;++i){
                ci[i] = 0.0;
                alphas[i] = new float[num_state];
                betas[i] = new float[num_state];
                gammas[i] = new float[num_state];
            }
            // Initialize di gammas
            di_gammas = new float**[T];
            for(int i = 0; i<T;i++){
                di_gammas[i] = new float*[num_state];
                for(int j = 0; j<num_state;j++){
                    di_gammas[i][j] = new float[num_state];
                    alphas[i][j] = 0;
                    betas[i][j] = 0;
                    gammas[i][j] = 0;
                }
            }
            // Initialized observation
            int ii = 0;
            for(int i = 0;i < T;i++){
                ii = i;
                if(bird.getObservation(i)!=-1){
                    observation[i] = bird.getObservation(i);
                }
                else{
                    
                    break;
                }
                

            }
            T  = ii;
        }

        void Build_HMM(){
            int Ot = 0;
            float mom = 0;
            float sumup = 0;
            iter_time = 0;
            old_logProb = -100000;
            new_logProb = 0;
            flag = 1;
            converge = 0;
            //PrintOut();
            while(flag == 1){
                //PrintOut();
                // calculate alpha
                Ot = observation[0];
                ci[0] = 0.0;
                for(int i = 0;i<num_state;++i){
                    alphas[0][i] = B[i][Ot] * P[0][i];
                    ci[0] = ci[0] + alphas[0][i];
                }

                //if(ci[0]!=0){
                    ci[0] = 1.0/ci[0];
                    for(int i = 0;i<num_state;++i){
                        alphas[0][i] = alphas[0][i] * ci[0];

                    }

            


                for(int t = 1;t < T;t++){
                    Ot = observation[t];
                    ci[t] = 0.0;
                    for(int i = 0;i<num_state;++i){
                        alphas[t][i] = 0;
                        for(int j = 0;j<num_state;++j){
                           alphas[t][i] += A[j][i] * alphas[t-1][j];
                        }
                        alphas[t][i] *= B[i][Ot];
                        ci[t] += alphas[t][i];
                    }
                        ci[t] = 1.0/ci[t];
                        for(int i = 0;i<num_state;i++){
                            alphas[t][i] = alphas[t][i] * ci[t];
                        }
                }
                // calculate beta
                for(int i = 0;i<num_state;++i){
                    betas[T-1][i] = ci[T-1];
                }
                for(int t = T-2;t >= 0;--t){
                    Ot = observation[t+1];
                    
                    for(int i = 0; i<num_state;++i){
                        betas[t][i] = 0;
                        for(int j = 0; j<num_state;++j){
                            betas[t][i]+= betas[t+1][j] * B[j][Ot] * A[i][j];
                        }
                        betas[t][i]  *= ci[t];
                    }
                }
                float denom = 0;
                float numer = 0;
                // calculate gammas
                for(int t = 0;t<T-1;t++){
                    Ot = observation[t+1];
                    denom = 0;
                    for(int i = 0; i< num_state;i++){
                        for(int j = 0; j<num_state;j++){
                            denom += alphas[t][i] * A[i][j] * B[j][Ot] * betas[t+1][j];

                        }
                    }
                    for(int i = 0;i<num_state;i++){
                        gammas[t][i] = 0;
                        for(int j = 0; j<num_state;j++){
                            di_gammas[t][i][j] = (alphas[t][i] * A[i][j] * B[j][Ot] * betas[t+1][j])/denom;
                            gammas[t][i] += di_gammas[t][i][j];
                        }
                    }
                }
                // special case for gammas T-1
                for(int i = 0;i<num_state;i++){
                    gammas[T-1][i] = alphas[T-1][i];
                }
                // re-estimate_Parameters
                
                // P
                for(int i = 0; i<num_state;i++){
                    P[0][i] = gammas[0][i];
                }
                // A
                for(int i = 0; i<num_state;i++){
                    denom = 0;
                    for(int t = 0; t<T-1;t++){
                        denom = denom + gammas[t][i];
                    }
                    for(int j = 0; j<num_state;j++){
                        numer = 0;
                        for(int t = 0;t<T-1;t++){
                            numer = numer + di_gammas[t][i][j];
                        }
                        A[i][j] = numer/denom;
                    }
                }
                // B
                for(int i = 0;i<num_state;i++){
                    denom = 0;
                    for(int t = 0; t<T;t++){
                        denom = denom + gammas[t][i];
                    }
                    for(int j = 0; j<num_Ob;j++){
                        numer = 0;
                        for(int t = 0; t<T-1; t++){
                            if(observation[t]==j){
                                numer = numer + gammas[t][i];
                            }
                        }
                        B[i][j] = numer/denom;
                    }
                }
                // compute logProb
                new_logProb = 0;
                for(int t = 0; t<T;t++){
                    new_logProb = new_logProb + std::log(ci[t]);
                }
                new_logProb = - new_logProb;

            // test if the model converge
                iter_time ++;
                if(iter_time < max_iter){
                    converge = 1;
                }
                // test A
                for(int i = 0; i<num_state;i++){
                    for(int j = 0; j<num_state;j++){
                        if(std::isnan(A[i][j])){
                            converge = 0;
                            flag = 0;
                        }
                    }
                }
                // test B
                for(int i = 0; i<num_state;i++){
                    for(int j = 0; j<num_Ob;j++){
                        if(std::isnan(B[i][j])){
                            converge = 0;
                            flag = 0;
                        }
                    }
                }

                if(iter_time < max_iter || new_logProb > old_logProb){
                    old_logProb = new_logProb;
                    flag = 1;
                }
                else{
                    flag = 0;
                }
                

            }

  }



      void Prediction(const Bird b){
            //PrintOut();
            float next_state[num_state];
            float next_observation[num_Ob];
            nobservation = -1;
            Probnobservation = 0;
            if(converge){
                for(int i = 0; i<num_state;i++){
                    next_state[i] = 0;
                    for(int j = 0; j<num_state;j++){
                        next_state[i] += alphas[T-1][j] * A[j][i]; // for next state
                    }

                }
                for(int i = 0; i < num_Ob;i++){
                    next_observation[i] = 0;
                    for(int j = 0; j< num_state;j++){
                        next_observation[i] += next_state[j] * B[j][i];
                    }
                }
                //PrintOut();
                // found
                float sum = 0;
                for(int i = 0; i< num_Ob;i++){
                    //std::cerr<< next_observation[i]<<" ";
                    sum += next_observation[i];
                    if(Probnobservation < next_observation[i]){
                        Probnobservation = next_observation[i];
                        nobservation = i;
                    }
                }
                //Probnobservation /= sum;
                //std::cerr<<std::endl;

            }
            else{
                nobservation = -1;
                Probnobservation = 0;
            }

        }

        float Prob_sequence(Bird bird){
            int Time = 0;
            if(bird.isDead())
            {
            for(int i=0;i<bird.getSeqLength();++i)
                if(bird.getObservation(i) == -1)
                {
                    Time = i;
                    break;
                }
            }
            else{
                Time = bird.getSeqLength();
            }

                    
            int givenObs[Time];
            float tempAlphas[Time][num_state];
            int Ot = 0;
            float mom = 0;
            float temp_ci[Time];
            float sumup = 0;
            float temp_p[num_state];

            // if converge then we can predict
            if(converge){
                // receive observations
                for(int i = 0 ; i< Time;i++){
                    givenObs[i] = bird.getObservation(i);
                }
                Ot = givenObs[0];
                temp_ci[0] = 0;
                for(int i = 0;i<num_state;i++){
                    temp_p[i] = 1.0;
                    tempAlphas[0][i] = 0;
                    tempAlphas[0][i] = B[i][Ot] * P[0][i];
                    temp_ci[0] += tempAlphas[0][i];
                }
                temp_ci[0] = 1.0/temp_ci[0];
                for(int i = 0; i<num_state;i++){
                    tempAlphas[0][i] = tempAlphas[0][i] * temp_ci[0];
                }
                for(int t = 1;t < Time;t++){
                    Ot = givenObs[t];
                    temp_ci[t] = 0;
                    for(int i = 0;i<num_state;i++){
                        tempAlphas[t][i] = 0;
                        for(int j = 0;j<num_state;j++){
                            tempAlphas[t][i] += A[j][i] * tempAlphas[t-1][j];
                        }
                        tempAlphas[t][i] *= B[i][Ot] ;        
                        temp_ci[t] += tempAlphas[t][i];
                        }
                    
                        temp_ci[t] = 1.0/temp_ci[t];
                        for(int i = 0; i<num_state;i++){
                            tempAlphas[t][i] = tempAlphas[t][i] * temp_ci[t];
                                        
                        } 

                    }
                    // sum up the tempAlphas[Time-1]
                    float Prob = 0.0;
                    for(int i = 0; i< num_state;i++){
                        Prob += tempAlphas[Time-1][i];
                    }
                     if(std::isnan(Prob)) return 0.0;
                     if(Prob == 0) return 0.0;
                    
                    return Prob / temp_ci[Time-1];
                    
                    
                    
                }
                else return 0.0;
  }

    private:
        float** gammas;
        float*** di_gammas;
        float** alphas; // T x num_state
        float** betas; // T x num_state
        float* ci; // T
        int flag = 1;
        int iter_time = 0;
        int max_iter = 300;
        float old_logProb = -10000;
        float new_logProb = 0;
        void PrintOut(){
            std::cerr<<"print out A"<<std::endl;
            for(int i=0;i<num_state;i++){
                for(int j = 0;j<num_state;j++){
                    std::cerr<<A[i][j]<<" ";
                }
                std::cerr<<std::endl;
            }
            std::cerr<<"print out B"<<std::endl;
            for(int i=0;i<num_state;i++){
                for(int j = 0;j<num_Ob;j++){
                    std::cerr<<B[i][j]<<" ";
                }
                std::cerr<<std::endl;
            }

            std::cerr<<"print out P"<<std::endl;
            for(int i=0;i<1;i++){
                for(int j = 0;j<num_state;j++){
                    std::cerr<<P[i][j]<<" ";
                }
                std::cerr<<std::endl;
            }
            std::cerr<<"print out alphas"<<std::endl;
            for(int i=0;i<10;i++){
                for(int j = 0;j<num_state;j++){
                    std::cerr<<alphas[i][j]<<" ";
                }
                std::cerr<<std::endl;
            }

            std::cerr<<"print out gammas"<<std::endl;
             for(int i=0;i<10;i++){
                 for(int j = 0;j<num_state;j++){
                     std::cerr<<gammas[i][j]<<" ";
                 }
                 std::cerr<<std::endl;
             }
        }

        


};
}
