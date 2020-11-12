#include "path_finding.h"
#define max_size 1000
//♥♥♥♥♥♥♥♥♥♥♥


void find_path(Partie* p,Pos depart, Pos arriver,Entite* F){	
	int liste_nb_choix[max_size]={0};
	int nb_openlist=0;
	int nb_closelist=0;
	int nb_voisin;
	int index_min;
	int nb_tour=0;
	int way_find=0;
	Noeud liste_ouverte[max_size]={0};
	Noeud liste_fermer[max_size]={0};
	
	Noeud Noeud_depart={depart,0,distance(depart,arriver),distance(depart,arriver)+0,NULL};
	Noeud Noeud_arriver={arriver,distance(depart,arriver),0,distance(depart,arriver)+0,NULL};
	printf("\nTarget x: %d y: %d    Possition  x: %d y: %d\n",Noeud_arriver.pos.c,Noeud_arriver.pos.l,Noeud_depart.pos.c,Noeud_depart.pos.l);

	if (nb_openlist==0){
		liste_ouverte[0]=Noeud_depart;
		liste_fermer[0]=Noeud_depart;
		liste_nb_choix[0]+=1;
		nb_openlist++;
		nb_closelist++;

	}
	while(!way_find){
	
		//printf("\nnb_closelist: %d nb_openlist: %d nb_tour: %d\n",nb_closelist,nb_openlist,nb_tour );
		nb_voisin=nb_openlist;		
		for(int v=liste_nb_choix[nb_tour];v!=0;v--){
			if  ((liste_fermer[nb_closelist-v].pos.l==Noeud_arriver.pos.l) && (liste_fermer[nb_closelist-v].pos.c ==Noeud_arriver.pos.c)){
				way_find=1;
				get_path(&(liste_fermer[nb_closelist-v]),nb_tour,F);
				break;
			}
			nb_openlist+=recherche_voisin(p,&(liste_fermer[nb_closelist-v]),liste_fermer,liste_ouverte,&Noeud_depart,&Noeud_arriver,nb_openlist,nb_closelist);		
		}
		if (way_find){
			break;
		}

		nb_voisin=(nb_openlist-nb_voisin);
		index_min=find_min(liste_ouverte,nb_openlist,nb_voisin);
		for (int k=(nb_openlist-nb_voisin);k!=nb_openlist;k++){
			if ((liste_ouverte[index_min].cout_t==liste_ouverte[k].cout_t) || (liste_ouverte[index_min].cout_h==liste_ouverte[k].cout_h) || (liste_ouverte[k].cout_t<=liste_ouverte[k].parent->cout_t)){
				liste_fermer[nb_closelist]=liste_ouverte[k];
				nb_closelist+=1;
				liste_nb_choix[nb_tour+1]+=1;
			}
		}
		nb_tour+=1;
	}

}



void get_path(Noeud* last_noeud,int nb_tour,Entite* F){
	printf("\nget_path %p\n",last_noeud->parent );
	Noeud* local=last_noeud;
	int index=nb_tour-1;
	F->nombre_noeud=nb_tour;
	F->chemin_noeud[index]=last_noeud;
	while (local->parent!=NULL){
		local=local->parent;
		if (local->parent != NULL){
			F->chemin_noeud[--index]=local;
			printf("\nNOEUD SELECTED:: n°%d x: %d y: %d G: %d H: %d T:%d Parent: %p",index,F->chemin_noeud[index]->pos.c,F->chemin_noeud[index]->pos.l,F->chemin_noeud[index]->cout_g,F->chemin_noeud[index]->cout_h,F->chemin_noeud[index]->cout_t,F->chemin_noeud[index]->parent);
		}

	}
	printf("\nEND nb_tour:%d\n",nb_tour);
}

int recherche_voisin(Partie* p,Noeud* current_noeud,Noeud liste_fermer[],Noeud liste_ouverte[],Noeud* Noeud_depart,Noeud* Noeud_arriver,int nb_openlist,int nb_closelist){
	int directions[4][2] = {{-1,0},{0,1},{1,0},{0,-1}};
	int nb_voisin=0;
	for (int i=0;i!=4;i++){
		if (p->plateau[current_noeud->pos.l+directions[i][1]][current_noeud->pos.c+directions[i][0]]!='*' && !(inliste((Pos){current_noeud->pos.l+directions[i][1],current_noeud->pos.c+directions[i][0]},liste_fermer,nb_closelist))){
			liste_ouverte[nb_openlist]=(Noeud){(Pos){(current_noeud->pos.l+directions[i][1]),(current_noeud->pos.c+directions[i][0])},distance(Noeud_depart->pos,(Pos){(current_noeud->pos.l+directions[i][1]),(current_noeud->pos.c+directions[i][0])}),distance((Pos){(current_noeud->pos.l+directions[i][1]),(current_noeud->pos.c+directions[i][0])},Noeud_arriver->pos),distance(Noeud_depart->pos,(Pos){(current_noeud->pos.l+directions[i][1]),(current_noeud->pos.c+directions[i][0])})+distance((Pos){(current_noeud->pos.l+directions[i][1]),(current_noeud->pos.c+directions[i][0])},Noeud_arriver->pos),current_noeud};
			nb_openlist+=1;
			nb_voisin+=1;
		}
	}
	return nb_voisin;
}


int find_min(Noeud liste_ouverte[],int nb_openlist , int nb_voisin){
	int cout_t_min=999999;
	int pos_min;
	for (int k =(nb_openlist- nb_voisin);k!=nb_openlist;k++){
		if (liste_ouverte[k].cout_t<cout_t_min) {
			cout_t_min=liste_ouverte[k].cout_t;
			pos_min=k;
		}
	}
	return pos_min;	
}

int inliste(Pos element,Noeud liste_fermer[],int nb_closelist){
	for (int n=0;n!=nb_closelist;n++){
		if ((element.c==liste_fermer[n].pos.c)&&(element.l==liste_fermer[n].pos.l)){
			return 1;
		}
	}
	return 0;
}
int distance(Pos pointA,Pos pointB){
	int result=abs(pointB.c - pointA.c) + abs(pointB.l - pointA.l);
	return result;
}

