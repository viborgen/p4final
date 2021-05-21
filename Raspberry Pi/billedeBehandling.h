//Der produceres en BMP fil ud fra modtaget FPGA data
void lavBMP(){
    if(countSort<=(n*m)*0.4){
	Image image(m, n); //Definere billedestørrlse i Image.h
	for (int i= 0; i < n; i++){ //Der anvedes to forloops for at gennemgå matrix array
	    for (int j = 0; j < m; j++){
		image.SetColor(Color(billede[j][i].R,billede[j][i].G,billede[j][i].B), j, i); //Sender array data til Image.h
	    }	
	}
	nameWithNumber = "/home/pi/Desktop/p4/sammensatThreads/Storage/0RAW/random" + to_string(billedeCounter) + ".bmp"; //Generere et navn til at gemme billedefilen med
	bmpName = const_cast<char*>(nameWithNumber.c_str()); //Omdannes til en char så det fungerer opsætningen i Image.h
	image.Export(bmpName); //Laver bitmapfilen
    }
}

//Der undersøges for sorte- og brandpixels
void billedebehandling(){
	for (volatile int i=0; i<m; i++){
		for(volatile int j=0; j<n; j++){
			if(billede[i][j].R < 27 and billede[i][j].G < 27 and billede[i][j].B < 27){
				countSort = countSort + 1;
			}
			if(billede[i][j].R >= 159 and billede[i][j].G >= 98 and billede[i][j].B >= 36){
				if(billede[i][j].R <= 255 and billede[i][j].G <= 166 and billede[i][j].B <= 88){
					brandPixels[i][j].i = i;
					brandPixels[i][j].j = j;
					countBrand++;
				}
			}
		}
	}
	cout << "Sorte pixels: " << countSort << "\n";
	if(countSort > (n*m)*0.4){
		cout << "Billedet kan ikke bruges\n";
	}
	countSort = 0;
	cout << "Brand pixels: " << countBrand << "\n";
}

void skalerKomprimer(){
    if(countSort<=(n*m)*0.4){
		Mat gemtBillede; //Den gemte BMP fil importeres i openCV
		gemtBillede = imread(bmpName,IMREAD_COLOR); 
		resize(gemtBillede,gemtBillede,Size(192,144)); //Billedet gives nye dimentioner i openCV
		skaleretName = "/home/pi/Desktop/p4/sammensatThreads/Storage/1Skaleret/Skaleret" + to_string(billedeCounter) + ".bmp"; //Gives nummerede navne
		jpgName = "/home/pi/Desktop/p4/sammensatThreads/Storage/2Komprimeret/Komprimeret" + to_string(billedeCounter) + ".jpg";
		imwrite(jpgName, gemtBillede, {IMWRITE_JPEG_QUALITY, 30}); //jpg gemmes med 70% komprimering
		imwrite(skaleretName,gemtBillede);
    }
}


bool copyFile(string SRC, string DEST) { //Funktion til at kopiere og indsætte fil
    std::ifstream src(SRC, std::ios::binary); //Åbner filen der ønskes at flyttes
    std::ofstream dest(DEST, std::ios::binary); //Lukker filen der ønskes at flyttes
    dest << src.rdbuf(); //Skubber src.rdbuf til dest
    return src && dest;  //Retunere de to lokationer
}















