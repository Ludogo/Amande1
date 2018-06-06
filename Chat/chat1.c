#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "chatMessage.h"
#include <stdbool.h>
#include <pthread.h>

#define TAILLE_MESSAGE 2048

int clientPipeNameHandle;

void *ReadMessages(void *x_void_ptr)
{
	ChatMessage newMessage;
	/* increment x to 100 */
	bool *stop = (bool *)x_void_ptr;
	while (*stop != true)
	{
		read(clientPipeNameHandle, &newMessage, sizeof(ChatMessage));
		fprintf(stderr, "(%s) : %s\n", newMessage.nickName, newMessage.payLoad);
	}

	printf("reader is finished\n");

	/* the function must return something - NULL will do */
	return NULL;
}

int main(void)
{
	int ID_Tube_client_1, ID_Tube_serveur;
	char chaine_emission[TAILLE_MESSAGE];
	char Tube_serveur[] = "/tmp/serveur.fifo";

	bool isConnected = false;
	bool stop = false;
	pthread_t threadLecteur;
	char nickName[256];

	//Vérification que serveur est bien lancé
	// if ((ID_Tube_serveur = open(Tube_serveur, O_WRONLY | O_NDELAY )) != 0)
	// {
	// 	system("./serveur");
	// }

	//connexion au serveur
	if ((ID_Tube_serveur = open(Tube_serveur, O_WRONLY)) == -1)
	{
		fprintf(stderr, "Impossible d'ouvrir l'entrée du tube serveur 1.\n");
		exit(EXIT_FAILURE);
	}

	while (!strstr(chaine_emission, "/CLOSE"))
	{
		char chaine[TAILLE_MESSAGE];
		fgets(chaine, TAILLE_MESSAGE, stdin);
		//delete /N at the end of line
		strtok(chaine, "\n");
		//reset chaine emission
		memset(chaine_emission, 0, TAILLE_MESSAGE);
		sprintf(chaine_emission, "%s", chaine);

		if (strstr(chaine_emission, "/CONNECT"))
		{
			if (isConnected == false)
			{

				char clientPipeName[261];
				fprintf(stdout, "enter nick name :\n");
				fgets(nickName, 256, stdin);
				//delete /N at the end of line
				strtok(nickName, "\n");
				ChatMessage message;
				strcpy(message.nickName, nickName);
				//demande la connexion
				write(ID_Tube_serveur, &message, sizeof(ChatMessage));

				sprintf(clientPipeName, "/tmp/%s.fifo", nickName);

				//laisse le temp au serveur de faire la création
				sleep(1);

				//connexion au pipe de lecture
				if ((clientPipeNameHandle = open(clientPipeName, O_RDONLY)) == -1)
				{
					fprintf(stderr, "Enable to connect to client pipe\n");
					exit(EXIT_FAILURE);
				}
				//lance le thread de lecture
				/* create a second thread which executes inc_x(&x) */
				if (pthread_create(&threadLecteur, NULL, ReadMessages, &stop))
				{
					fprintf(stderr, "Error creating thread\n");
					return 1;
				}
				isConnected = true;
			}
			else
			{
				fprintf(stderr, "you must close before connect \n");
			}
		}
		else if (isConnected == true)
		{
			ChatMessage messageToSend;
			strcpy(messageToSend.nickName, nickName);
			strcpy(messageToSend.payLoad, chaine_emission);
			messageToSend.payLoadLength = strlen(chaine_emission);
			write(ID_Tube_serveur, &messageToSend, sizeof(ChatMessage));
		}
	}

	/* wait for the second thread to finish */
	if (pthread_join(clientPipeNameHandle, NULL))
	{

		fprintf(stderr, "Error joining thread\n");
		return 2;
	}
	if (close(clientPipeNameHandle) == -1)
	{
		perror("Erreur fermeture du tube client 1");
	}
	fprintf(stdout, "fermeture du Client 1\n");
	//system("rm -f chat1.fifo");
	return EXIT_SUCCESS;
}
