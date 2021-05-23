//Inkludere almindelige biblioteker
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <memory.h>
#include <cstdlib>
#include <wiringPi.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <semaphore.h>
#include <sstream>
#include <iomanip>
#include <wiringSerial.h>
#include <errno.h>
#include <cstring>
#include <fstream>

//----------------------------------------------------

//Definering af namespace
using namespace std;
using namespace cv; //benyttes til openCV


//----------------------------------------------------

//Variabler og arrays defineres
//----------------------------------------------------

//---Variabler til billedebehandling og FPGA læsning
//----------------------------------------------------
//Billede størrelse
const int m = 640; //bredde
const int n = 480; //højde

//Counters
volatile int sort_pixel_count = 0;
volatile int ild_pixel_count = 0;
volatile int min_ild_pixel = 2;
volatile int maks_sort_pixel = m*n*0.4;

//Benyttes til at navngive billede
char* bmpName;
string nameWithNumber;
string skaleretName;
string jpgName;
int billedeCounter = 0;

//Struct til at gemme RGB data for hver pixel
struct pixel{
	volatile int R, G, B;
};
pixel volatile billede[m][n];

//Gemmer pixels der indikeres som brand
struct brand{
	volatile int i, j;
};
brand volatile brandPixels[m][n];

//Kendt billedestørrelse for testbillede
const int AmoutOfByte = 1271*6;

//anvendes til at dobbelttjekke modtaget billededata
const int picsample = 10;
int rawRGB555data[AmoutOfByte][picsample];
int RGB555placeholder[picsample];
int sortedRGB555data[2*m*n];
int requrenceOfData[picsample];
int posIndex;
int temp[2*m*n];
int CorrectPositionCounter;

//Anvendes til aflæsning af FPGA
volatile int number = 0b00000000;
volatile char a[7];

bool storage = true;
bool kontrolB = true;
bool kontrolG = true;

//GPIO Pins defineres
#define PIN2 2
#define PIN4 4
#define PIN17 17
#define PIN27 27
#define PIN22 22
#define PIN23 23
#define PIN24 24
#define PIN25 25
#define DONE 8
#define RPI 7
#define TX 26
#define RX 16

//----------------------------------------------------

//--Variabler til GPS
//----------------------------------------------------
string gpsDATA;
string latt;
string lonn;
string Height;
double lattf;
double lonnf;
double heightf;
int x = 0;
double lat;
double lon;
int GPSCounter = 0;

const int gpsArraySize = 67;
string str;
ofstream myfile;
string gpsArray[gpsArraySize];
stringstream ss;
int l = 0;

//--Variabler til pthreads
//----------------------------------------------------
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock1 =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond4 = PTHREAD_COND_INITIALIZER;

//----------------------------------------------------

//Andre header filer
#include "Image.h"
#include "readFPGA.h"
#include "billedeBehandling.h"
#include "GPS.h"
#include <filesystem>
#include "threads.h"