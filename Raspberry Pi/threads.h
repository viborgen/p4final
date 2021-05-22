void* T1 (void* arg)  { // Tråd til at tage billede og behandle det.
	while(1) {
		pthread_mutex_lock(&lock1); //Mutexen lock1 låses

		//De følgende funktioner indsamler billede arrayet fra FPGA'en og behandler billedet
		definePinMode();//Definere pins
		readFPGA(); //Læser fra FPGA'en
		sortRGB555(); //Korrekter bitfejl ved at kigge det samme billede igennem bare sendt flere gange
		convertToRGB888(); //Konvertere billedet fra RGB555 til RBG888
		billedebehandling(); //Behandler billedet så det kan bruges i OPENCV
		lavBMP(); //Funktion til at skabe billede fra FPGA data
		skalerKomprimer(); //Skalere og komprimere billedet
	
		pthread_cond_signal(&cond1); //Signalere til T3 at T1 er færdig
		pthread_cond_wait(&cond3, &lock1); //Modtager et signal fra T3 om at den er færdig
		pthread_mutex_unlock(&lock1); //Mutexen bliver låst op, og tasken starter forfra
}
	return NULL;  //Til syntaksen af Pthreads
}

void* T2 (void* arg) { //Tråd til GPS delen
	while(1) {
		pthread_mutex_lock(&lock2); //Låser mutexen lock2

		laesGPS(); //Funktion der modtager lokation fra GPS

		pthread_cond_signal(&cond2); //Sender et signal til T3 om at T2 er færdig
		pthread_cond_wait(&cond4, &lock2); //Modtager et signal fra T3 om at den er færdig
		pthread_mutex_unlock(&lock2); //Mutexen lock2 bliver frigivet og tasken kan starter forfra
	}
	return NULL; //Syntaks til Pthreads
}

void* T3 (void* arg){ //Tråd til sende delen
	
	while(1){ 
		pthread_cond_wait(&cond1,&lock1); //Tasken venter på signal fra T1 om at billede er færdigbehandlet
		pthread_cond_wait(&cond2,&lock2); //Tasken venter på signal fra T2 om at GPS kordinaterne er færdigbehandlet
		
		kombiner(); //Kombinere GPS og billede samt flytter det til en seperat mappe

		pthread_cond_signal(&cond3); //Signalere til T1 at den kan fortsætte
		pthread_cond_signal(&cond4); //Signalere til T2 at den kan fortsætte
		
		//Flytter den samlede fil til en anden mappe (Illustrere at billedet bliver sendt imens T1 og T2 er startet på ny)
		copyFile("/home/pi/Desktop/p4/sammensatThreads/Storage/DataSendt/Samlet" + to_string(billedeCounter) + ".jpg","/home/pi/Desktop/p4/sammensatThreads/Storage/KomModul/KomModul" + to_string(billedeCounter) + ".jpg");	
	}
	return NULL; //Syntaks til Pthreads
}
	
