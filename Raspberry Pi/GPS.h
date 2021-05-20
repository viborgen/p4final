void NMEA_decoder2(string data){ //Decoder der anvendes til at frasortere data der ikke skal benyttes
	int j = 0;
  	int k = 0;
  	string dataArray[14]; // er 14 da sætnings konstruktionen på et gps signal har 15 sætninger
  
  	if (data.substr(0,6) == "$GPGGA"){ // tjekker om det er positions data
		for (int i = 0 ; data[i] != '*' ; i++){ //for loop der køre indtil '' som er end bit 
			if(data[i] !=','){ // denne if sætning tæller hvor mange chars der er til næste komma
				j++;
			}
			
			else{
			dataArray[k] = data.substr(i-j,i);               // data bliver indsæt i array som er delt op i sætninger af k. 
			j = 0;                                              //Sætninger bliver definere ved i og j, hvor j har talt hvor mange char der er til et komma. 
			k++; // k tæller antal sætning der er indeholdet    //i holder hvor mange char der er talt i alt. j bliver sat til 0 igen så den tæller hvor mange chars der imellem to kommaer
			}
		}
	
		double floatDataArray[14]; //benyttes til at konvertere dataen til floats (nu doubles).
		floatDataArray[0] = stod(dataArray[2].substr(0,2));
		floatDataArray[1] = stod(dataArray[2].substr(2,8))/60.0;
		floatDataArray[2] = stod(dataArray[2].substr(13,3));
		floatDataArray[3] = stod(dataArray[2].substr(16,8))/60.0;

		lat = floatDataArray[0] + floatDataArray[1]; //latitude defineres her som en double
		lon = floatDataArray[2] + floatDataArray[3]; //longitude defineres her som en double
		latt = to_string(lat) + dataArray[2].substr(6,7); //latitude ændres til en string og retningen på kordinatet tilføles
		lonn = to_string(lon) + dataArray[2].substr(23,24); //det samme sker for longitude
		Height = dataArray[9].substr(0,4) + " m"; //Højden defineres og enheden tilføjes
	
		pData = true; // boolean der sørger for overstående data kun bliver printet én gang i void loop
 	}
}


//Funktion der læser direkte fra GPS.
void laesGPS(){
	x = 0;
  	int serial_port; 
  	char dat,buff[100],GGA_code[3];
  	unsigned char IsitGGAstring=0;
  	unsigned char GGA_index=0;
  	unsigned char is_GGA_received_completely = 0;
  
	if ((serial_port = serialOpen ("/dev/ttyS0", 9600)) < 0)		/* open serial port */
	{
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
		//return 1 ;
	}

	if (wiringPiSetup () == -1){							/* initializes wiringPi setup */
		fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
		//return 1 ;
	}
	string str;
	ofstream myfile;
	string gpsArray[67];
	stringstream ss;
	int l = 0;
	
  	while(x < 1){
		if(serialDataAvail (serial_port) )		/* check for any data available on serial port */
		  { 
			dat = serialGetchar(serial_port);		/* receive character serially */		
			if(dat == '$'){
				IsitGGAstring = 0;
				GGA_index = 0;
			}
			else if(IsitGGAstring ==1){
				gpsArray[l] = dat;
				l++;
					if(dat=='\r'){
						is_GGA_received_completely = 1;
					}
				}

			else if(GGA_code[0]=='G' && GGA_code[1]=='G' && GGA_code[2]=='A'){
				IsitGGAstring = 1;
				GGA_code[0]= 0; 
				GGA_code[0]= 0;
				GGA_code[0]= 0;		
				}

			else{
				GGA_code[0] = GGA_code[1];
				GGA_code[1] = GGA_code[2];
				GGA_code[2] = dat;
				}
		  }
		if(is_GGA_received_completely==1){
			l = 0;	//array resettes
			ss.str("");
			ss.clear();
			for(int i = 0; i<67; i++){ //laver datarray om til en string, så den kan  bruges i decoderen.
				ss << gpsArray[i];
			}
			string test;
			string strr("$GPGGA,"+ss.str());
			
			NMEA_decoder2(strr);

			cout << "\n" << str << "GPS læst\n";
			cout << "Lat: " << setprecision(9) << latt << endl;
			cout << "Lon: " << setprecision(9) << lonn  << endl;
			cout << "Højde: "<< Height << endl;
			if(countSort<=(n*m)*0.4){
				myfile.open("/home/pi/Desktop/p4/sammensatThreads/Storage/3GPSData/GPSData"+to_string(billedeCounter) +".txt",ofstream::trunc);
				if (myfile.is_open()){
					myfile.seekp (0,ios_base::end);  
					myfile << "Lat: " << setprecision(9) << latt << endl;
					myfile << "Lon: " << setprecision(9) << lonn  << endl;
					myfile << "Højde: " << Height << endl;
					myfile.close();
					cout << "GPS gemt" <<endl;
					GPSCounter ++;
				  }
				  else cout << "Unable to open file";
				is_GGA_received_completely = 0;
				x++;
			}
			else{
				delay(1000);
			}
		}
	}
}


void kombiner() { //Funktion til at indlægge GPS positionen på billedet
	if(countSort<=(n*m)*0.4){
   		copyFile("/home/pi/Desktop/p4/sammensatThreads/Storage/2Komprimeret/Komprimeret" + to_string(billedeCounter) + ".jpg","/home/pi/Desktop/p4/sammensatThreads/Storage/DataSendt/Samlet" + to_string(billedeCounter) + ".jpg");
		//Det seneste billede kopieres til en anden mappe
		Mat gemtBillede2; //En variabel til at gemme et billede oprettes
		gemtBillede2 = imread("/home/pi/Desktop/p4/sammensatThreads/Storage/DataSendt/Samlet" + to_string(billedeCounter) + ".jpg",IMREAD_COLOR); //Det flyttede billede gemmes i den oprettet variabel
		putText(gemtBillede2,latt + " " + lonn, Point(10,10), FONT_HERSHEY_PLAIN,0.7,Scalar(0,255,255),0.1,CV_AVX); //Lat og lon indsættes på billedet
		putText(gemtBillede2, Height, Point(10,20), FONT_HERSHEY_PLAIN,0.7,Scalar(0,255,255),0.1,CV_AVX); //Højden indsættes på billedet
		imwrite("/home/pi/Desktop/p4/sammensatThreads/Storage/DataSendt/Samlet" + to_string(billedeCounter) + ".jpg",gemtBillede2); //Billedet gemmes med teksten skrevet ovenop
		billedeCounter++; //Billede counter stiger med en
	}
}