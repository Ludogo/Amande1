#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>



#include "utils.h"

#define TAILLE_MESSAGE  256

// pour le serveur : il faut créer deux tube, un ou le serveur écoute, et un ou le serveur réécrie ce qu'il à entendu
int main(void)
{
	int ID_Tube_client_1,ID_Tube_client_2,ID_Tube_serveur;
	int id=0;
	char chaine[TAILLE_MESSAGE];
	char Tube_client_1[] = "chat1.fifo";
	char Tube_client_2[] = "chat2.fifo";
	char Tube_serveur[] = "serveur.fifo";

	//Création du tube client 1
	if((mkfifo(Tube_client_1, 0666) != 0)&&(errno != EEXIST)) //donne toute les permissions
	{
		fprintf(stderr, "Impossible de créer le tube client 1.\n");
		exit(EXIT_FAILURE);
	}
	if((ID_Tube_client_1 = open(Tube_client_1, O_WRONLY)) == -1)
	{
		fprintf(stderr, "Impossible d'ouvrir l'entrée du tube client 1.\n");
		exit(EXIT_FAILURE);
	}
	//Création du tube client 2
	if((mkfifo(Tube_client_2, 0666) != 0)&&(errno != EEXIST))
	{
		fprintf(stderr, "Impossible de créer le tube client 2.\n");
		exit(EXIT_FAILURE);
	}
	if((ID_Tube_client_2 = open(Tube_client_2, O_WRONLY)) == -1)
	{
		fprintf(stderr, "Impossible d'ouvrir l'entrée du tube client 2.\n");
		exit(EXIT_FAILURE);
	}
	//Création du tube serveur 1
	if((mkfifo(Tube_serveur, 0666) != 0)&&(errno != EEXIST))
	{
		fprintf(stderr, "Impossible de créer le tube serveur 1.\n");
		exit(EXIT_FAILURE);
	}
	if((ID_Tube_serveur = open(Tube_serveur, O_RDONLY)) == -1)
	{
		fprintf(stderr, "Impossible d'ouvrir l'entrée du tube serveur 1.\n");
		exit(EXIT_FAILURE);
	}


	fprintf(stderr,"You are on the server\n");

	while(1)
	{
		//Lecture des tube serveur 1 et 2
		read(ID_Tube_serveur, chaine, TAILLE_MESSAGE);
		fprintf(stderr,"%s", chaine);

		//Ecriture sur les tubes clients => fork pour un fonctionnement parallele
		if (strstr(chaine,"clt2 : ")!=NULL)
			id=ID_Tube_client_1;
		if (strstr(chaine,"clt1 : ")!=NULL)
			id=ID_Tube_client_2;

		write(id, chaine, TAILLE_MESSAGE);
	}
	
	if(close(ID_Tube_serveur) == -1) 
	{
		perror("Erreur fermeture du tube serveur");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr,"fermeture du serveur\n");
	
	return EXIT_SUCCESS;
}
