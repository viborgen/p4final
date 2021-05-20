void definePinMode(){
	wiringPiSetupGpio();
	pinMode(PIN2, INPUT);
	pinMode(PIN4, INPUT);
	pinMode(PIN17, INPUT);
	pinMode(PIN27, INPUT);
	pinMode(PIN22, INPUT);
	pinMode(PIN23, INPUT);
	pinMode(PIN24, INPUT);
	pinMode(PIN25, INPUT);
	pinMode(RPI, OUTPUT);
	pinMode(TX, OUTPUT);
	pinMode(RX, INPUT);
	pinMode(DONE, OUTPUT);
}

//Anvendes ved start af FPGA læsning
void read(){
	digitalWrite(RPI,1); //signal der sendes fra Raspberry Pi der vælger om FPGAen skal være i Read eller Write.
	digitalWrite(TX,1); //RX-TX signal til Raspberry Pi, så der kan holdes styr på hvornår der kan modtages data.
}

//Anvenes ved slut af FPGA-læsning
void doneRead(){	
	digitalWrite(RPI,0);
	digitalWrite(TX,0);
}

//Læser FPGA
void readFPGA(){
	for (volatile int j = 0; j < picsample; j++){ //Der dannes to for-loops for at kune læse 2d array med billededata
		for(volatile int i = 0; i<AmoutOfByte; i++){
			while(digitalRead(RX) == 1){} //Der ventes på modtaget rx signal, så der ikke læses forgæves eller forældet data.
			read();
			while(digitalRead(RX) == 0){} //Samme kontrol efter read()
			a[0] = digitalRead(PIN2); //Hver pin gemmes i et array
			a[1] = digitalRead(PIN4);
			a[2] = digitalRead(PIN17);
			a[3] = digitalRead(PIN27);
			a[4] = digitalRead(PIN22);
			a[5] = digitalRead(PIN23);
			a[6] = digitalRead(PIN24);
			a[7] = digitalRead(PIN25);
			number = 0b00000000; //Nulstilles så den aldrig går igen
			for (volatile int i=0; i<8; i++){ //Alle 8 pins definere samlet et binært tal. Disse konverteres her til en integer.
				number += a[i] << i;
			}
			doneRead();
			rawRGB555data[i][j] = number; //Alle integers gemmes i rawRGB555data, så denne senere kan bruges til omkonvertering til RGB 888.
		}
		
		digitalWrite(DONE,1); //Kontrolsignal til FPGA, der angiver at alt er læst.
		digitalWrite(DONE,0);
	}
}

//For at sikre at dataen der modtages er korrekt laves et tjek der angiver hvor mange gange det samme tal på en given plads går igen.
void sortRGB555(){
	for (int m = 0; m < AmoutOfByte; m++){ //for-loops for 2d array opstilles
		for ( int n = 0; n < picsample; n++){
			RGB555placeholder[n] = rawRGB555data[m][n];
		}
		
		for(int p = 0; p < picsample; p++){
			for(int k = 0; k < picsample; k++){
				if(RGB555placeholder[p] == RGB555placeholder[k]){
					requrenceOfData[p]++;	
				}
			}	
		}
		
		for(int c = 0; c < picsample; c++){
			cout << requrenceOfData[c] << " ";
		}
		
		for( int q = 0; q< picsample; q++) {
			if(requrenceOfData[0] <= requrenceOfData[q]){
				requrenceOfData[0] = requrenceOfData[q];
				posIndex = q;
			}
		}
		
		for(int r = 0; r < picsample; r++){
			requrenceOfData[r] = 0;  // fill array with 0, so its ready for next byte to procces. 
		}
		
		for (int s = 0; s < picsample; s++){
			if (rawRGB555data[m][s] != RGB555placeholder[posIndex]){
				for(int t = m; t < AmoutOfByte; t++){
					temp[t] = rawRGB555data[t+1][s];	
					rawRGB555data[t][s] = temp[t];
				}
			}
		}
		sortedRGB555data[m] = RGB555placeholder[posIndex];
	}

	for (int d = 0; d < AmoutOfByte; d++){
		cout << sortedRGB555data[d] << " "; 
	}
}

//Konverteres fra OV7670 datastrukturv ved RGB 555 til RGB 888.
void convertToRGB888(){	
	for (volatile int i=0; i<n; i++){
		volatile int t = 0;
		for(volatile int j=0; j<1271; j=j+2){
			billede[t][i].R = ((sortedRGB555datab1[j] & 124) >> 2) * 8;
			billede[t][i].G = (((sortedRGB555datab1[j] & 3) << 3) | ((sortedRGB555data[j+1] & 224) >> 5))*8 ;
			billede[t][i].B = (sortedRGB555datab1[j+1] & 31)*8;
			t++;
		}
	}
}