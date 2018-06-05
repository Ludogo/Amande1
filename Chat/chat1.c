#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define TAILLE_MESSAGE  256

// pour le serveur : il faut créé deux tube, un ou le serveur écoute, et un ou le serveur réécrie ce qu'il à entendu
int main(void)
{
	int ID_Tube_client_1,ID_Tube_serveur;
	char chaine[TAILLE_MESSAGE];
	char chaine_emission[TAILLE_MESSAGE];
	char chaine_reception[TAILLE_MESSAGE];
	char Tube_client_1[] = "chat1.fifo";
	char Tube_serveur[] = "serveur.fifo";
	int anti_fork_bomb = 0;
	int _pid=-1;

	//Vérification que serveur est bien lancé
	if((ID_Tube_serveur = open(Tube_serveur, O_RDONLY)) == 0)
	{
		system("./serveur");
	}

	//Création du tube client 2
	if((mkfifo(Tube_client_1, 0666) != 0)&&(errno != EEXIST))
	{
		fprintf(stderr, "Impossible de créer le tube client 1.\n");
		exit(EXIT_FAILURE);
	}
	if((ID_Tube_client_1 = open(Tube_client_1, O_RDONLY)) == -1)
	{
		fprintf(stderr, "Impossible d'ouvrir l'entrée du tube client 1.\n");
		exit(EXIT_FAILURE);
	}
	//Création du tube serveur 1
	/*if((mkfifo(Tube_serveur, 0666) != 0)&&(errno != EEXIST))
	{
		fprintf(stderr, "Impossible de créer le tube serveur 2.\n");
		exit(EXIT_FAILURE);
	}*/
	if((ID_Tube_serveur = open(Tube_serveur, O_WRONLY)) == -1)
	{
		fprintf(stderr, "Impossible d'ouvrir l'entrée du tube serveur 1.\n");
		exit(EXIT_FAILURE);
	}

	while(!strstr(chaine_emission,"CLOSE"))
	{

		if(anti_fork_bomb == 0)
		{
			_pid = fork();
			anti_fork_bomb = 1;
		}

		switch (_pid)
		{
			case -1:
				fprintf(stderr, "fork failed\n");
				exit(EXIT_FAILURE);
			break;

			case 0 : //l'enfant -- lecture du tube serveur
				while(read(ID_Tube_client_1, chaine_reception, TAILLE_MESSAGE) != 0)
				{
					fprintf(stderr,"%s", chaine_reception);
				}
			break;

			default : //le parent -- écriture de l'utilisateur et envoie vers le serveur
				fgets(chaine, sizeof chaine_emission, stdin);
				sprintf(chaine_emission,"clt1 : %s",chaine);
				write(ID_Tube_serveur, chaine_emission, TAILLE_MESSAGE);
			break;
		}
	}

	if(close(ID_Tube_client_1) == -1)
	{
		perror("Erreur fermeture du tube client 1");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr,"fermeture du Client 1\n");
	system("rm -f chat1.fifo");
	return EXIT_SUCCESS;
}
