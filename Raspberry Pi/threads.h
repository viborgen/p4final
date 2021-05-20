int err;
int err2;

void* T1 (void* arg)  { // Tråd til at tage billede og behandle det.
	while(1) {
		printf("Tråd 1 starter \n");
		pthread_mutex_lock(&lock1);

		definePinMode();
		readFPGA();
		sortRGB555();
		convertToRGB888();
		billedebehandling(); //Behandler billedet så det kan bruges i OPENCV
		lavBMP(); //Funktion til at skabe billede fra FPGA data
		skalerKomprimer(); //Skalere og komprimere billedet
	
		pthread_cond_signal(&cond1);
		pthread_cond_wait(&cond3, &lock1);
		pthread_mutex_unlock(&lock1);
}
	return NULL; 
}

void* T2 (void* arg) { //Tråd til GPS delen
	while(1) {
		printf("Tråd 2 starter \n");
		pthread_mutex_lock(&lock2);

		laesGPS(); //Funktion der modtager lokation fra GPS

		pthread_cond_signal(&cond2);
		pthread_cond_wait(&cond4, &lock2);		
		pthread_mutex_unlock(&lock2);
		printf("Tråd 2 slutter \n");

	}
	return NULL;
}

void* T3 (void* arg){ //Tråd til sende delen
	
	while(1){ 
		pthread_cond_wait(&cond1,&lock1);
		pthread_cond_wait(&cond2,&lock2);
		
		printf("Tråd 3 starter \n");
		kombiner(); //Kombinere GPS og billede samt flytter det til en seperat mappe
		printf("Tråd 3 slutter \n");

		pthread_cond_signal(&cond3);
		pthread_cond_signal(&cond4);
		
		printf("Flytter til send mappe \n");
		copyFile("/home/pi/Desktop/p4/sammensatThreads/Storage/DataSendt/Samlet" + to_string(billedeCounter) + ".jpg","/home/pi/Desktop/p4/sammensatThreads/Storage/KomModul/KomModul" + to_string(billedeCounter) + ".jpg");	
	}
	return NULL;
}
	
