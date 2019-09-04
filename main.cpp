#include "mbed.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "digicode.h"


//def des entrees
DigitalIn bt_o(PG_3);
DigitalIn bt_f(PD_15);
DigitalIn fdc_o(PD_13);
DigitalIn fdc_f(PD_11);
    
//les entrées du digicode
DigitalOut L0(PD_9);
DigitalOut L1(PB_15);
DigitalOut L2(PB_13);
DigitalOut L3(PB_11);
DigitalIn C0(PE_9);
DigitalIn C1(PE_11);
DigitalIn C2(PE_13);
DigitalIn C3(PE_15); 


//les leds 
DigitalOut led_portail(PH_1);
DigitalOut led_ouverture(PF_12);
DigitalOut led_fermeture(PF_14);
DigitalOut led(PG_13);
DigitalOut led2(PG_14);

//le moteur
DigitalOut p0(PG_0);
DigitalOut p1(PG_9);



//courant moteur
AnalogIn surcourant1(PA_5);
AnalogIn surcourant2(PA_3);
AnalogIn surcourantini1=surcourant1;
AnalogIn surcourantini2=surcourant2;


//infrarouge
AnalogIn infrarouge(PF_10);
AnalogIn IRini=infrarouge;




//matrice ligne qui nous donne les coordonnées des lignes et des colonnes du digicode
DigitalOut Ligne[]={L0,L1,L2,L3};
DigitalIn Colonne[]={C0,C1,C2,C3};


Tab Mdp[4]={{1,1,1,1},{0,1,0,2},{0,0,0,0},{0,1,2,3}}; //les mots de passe
Tab M={10,10,10,10}; // la valeur 10 signifie que le numéro n'a pas été inséré


//définitions des variables globales
int etat_code=1;
int res;
int numero=10; 
int m=0;
int etat=1;



// la valeur 10 signifie que le numéro n'a pas été inséré
// la valeur 11 elle signifie un mal fonctionnement	


				//les fonctions pour le digicode
int inserer_mdp (int etat_inserer, int numero){
	if(numero==11){
		return 0;
	}
  M[etat_inserer-1]= numero;
        return 0;
}   


void reset_ligne(){
	int i;
	for(i=0;i<4;i++){
		Ligne[i].write(0);
	}
}


int bouton_appuye(void){
	int i;
	for(i=0;i<4;i++){
		reset_ligne();
		Ligne[i].write(1);
		if(C0||C1||C2||C3){
			return 1;
		}
	}
	return 0;		
}



int recherche_ligne(void){
	int k;
	for(k=0;k<4;k++){
		if(Ligne[k].read()){
			return k+1;
		}
	}
	return 11;
}



int recherche_colonne(void){
	int j;
	for (j=0;j<=4;j++){
		if(Colonne[j].read()){
		return j+1;
		}
	}
	return 11;
}



int recherche_numero(int i, int j){	//i =ligne et j=colonne
	if(i<4&&j<4){
		return j+3*(i-1);
		}
	if(i==4&&j==2){
		return 0;
		}
	if(i==1&&j==4){
		return 0;
	}
	else{
			return 11;
	}
}
	


int valid_mdp(){
    int j=0;
    int i=0;
    int c=0;      // j° numéro , ieme code, et c compteur 
    for (i=0; i<4;i++){
        c=0;
				j=0;			//nécéssaire pour réini j à chaque code
        if (M[0]==Mdp[i][0]){ // le premier numéro du code est bon 
            
            for (j=0;j<4;j++){
                if (M[j]==Mdp[i][j]){
                    c+=1;
                }
                else { // permet de sortir de la boucle dés qu'un des numéros est faux
                    j=4;
                }                   
            }
            if (c==4){            // permet de sortir plus t�t
                return 1;
            }
        }
    }   
    return 11;
}
// fin fonctions pour digicode

void initialisation_securite(){
	IRini=infrarouge;
	surcourantini1=surcourant1;
	surcourantini2=surcourant2;
}

int securite(){
	unsigned int courant_reel,courant_ini;

	courant_ini=surcourantini2.read()-surcourantini1.read();
	courant_reel=surcourant2.read()-surcourant1.read();

	
	if(infrarouge.read()>1.1*IRini.read()||courant_reel>1.1*courant_ini){
		wait(0.1);
		return 1;
	}
	return 0;
	}


void sortie(int etat,int* m){
    switch (etat){
    case 1:
        *m=0;
        led_portail.write(0);
        break;
    case 2:
        *m=-1;
        led_portail.write(1);
        break;
    case 3:
        *m=0;
        led_portail.write(0);
        break;
    case 4:
        *m=1;
        led_portail.write(1);
        break;
    }
}

void sortie_moteur(){
	        if (m==1){
            p0=0;
            p1=1;            
        }
        if (m==-1){
            p1=0;
            p0=1;
        }
        if (m==0){
            p1=0;
            p0=0;
        }
			}



int main() {
    p1=0;
    p0=0;
		int c=0;    //verif du bon fonctionnement des sorties led

    while(1) {
        //calcul de l'E fut à partir de l'E pres et des entrées
        switch (etat){
    case 1:                 //  "fermé"
		if(bt_o.read()){
			etat=2;
		}
		led=0;
    led2=0;
		C0.mode(PullDown);
		C1.mode(PullDown);
		C2.mode(PullDown);
		C3.mode(PullDown);

		c=c+1;
		if(c>1000000){
			etat_code=1;
		}
			
		if (bouton_appuye()){ // si un bouton a été appuyé, passer à l'état suivant
				numero=recherche_numero(recherche_ligne(),recherche_colonne());
				led=1;
				inserer_mdp(etat_code,numero);
				etat_code=etat_code+1;
			wait(0.2);
		}
		if(etat_code==5){
			led2=1;
			led=0;
			res=valid_mdp();
      if(res==1){
				led2=1;
				led=1;
				led_ouverture=1;
				etat=2;
       }
			else{
				etat=1;
			}
      etat_code=1;
      }	
      break;


		case 2:					//	"ouverture"
			led_portail=1;
			if (bt_f==1){ // commande de la fermeture
				etat = 4;
				led_fermeture=1;
			}
			if(fdc_o==1){ 
				etat= 3;    
			}
			etat = etat;	//idem
			
			break;
		case 3:	//ouvert
			led_portail=1;
			if (bt_f.read()){//	"ouvert"
				etat = 4;	//on a changé
				led_fermeture=1;
				}
			else{
				etat=etat;
			}
			break;
			
		case 4:
			led_portail=1;
			initialisation_securite();	//	"fermeture"
			if(securite()==1||bt_o.read()){ // capteur IR/courant pour obstacle + commande l'ouverture
				etat=2;
			}
			if (fdc_f==1){
						etat = 1; //on a changé
						}
			else {
					  etat=etat; //on a changé
						}
			
			break;
		}
        sortie(etat,&m); //calcul des var de sortie en fct de l'état
				sortie_moteur(); //mise en forme du signal m sous la forme de 2 variables binaires pour
        //un sens et le sens opposé
        
        
       }
    return 0;
}





