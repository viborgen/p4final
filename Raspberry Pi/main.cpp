#include "header.h"
   
int main(){        
    pthread_t billedeThread, GPSThread, kombinerThread; //De tre tråde oprettes
    pthread_create(&billedeThread, NULL, T1, NULL);    
    pthread_create(&GPSThread, NULL, T2, NULL);
    pthread_create(&kombinerThread, NULL, T3, NULL);
    
    pthread_join(billedeThread, NULL);
    pthread_join(GPSThread, NULL);
    pthread_join(kombinerThread, NULL);
    
}
