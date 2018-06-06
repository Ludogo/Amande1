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
#include "client.h"
#include "chatMessage.h"

#include "utils.h"

//tableau pour mémoriser les clients
client *ID_Tube_Client[256];
int clientCount = 0;

/**
 * gère la crétion du pipe d'envoie des message vers les clients
 * */
int CreateClientPipe(char *nickName)
{
	char tubeName[265];
	sprintf(tubeName, "/tmp/%s.fifo", nickName);

	//création du tube client
	int ret = mkfifo(tubeName, 0666);
	if (ret == 0)
	{
		//creation dynamique du nouveau client en mémoire
		client *newClient = malloc(sizeof(client));
		//mémorise les infos client
		strcpy((*newClient).nickName, nickName);
		strcpy((*newClient).tubeName, tubeName);
		//ouverture du pipe
		(*newClient).pipeHandle = open(tubeName, O_WRONLY);
		//mémorisation du client dans la liste
		ID_Tube_Client[clientCount++] = newClient;

		//TODO traiter les erreurs
	}

	return ret;
}

/**
 * nettoie les ressources
 * */
void CleanUpClient()
{
	for (int i = 0; i < clientCount; i++)
	{
		client *currentClient = ID_Tube_Client[i];
		//TODO envoyer un message de stop
		//fermeture de la fifo
		close((*currentClient).pipeHandle);
		//supprime le fichier fifo
		remove((*currentClient).tubeName);
		//supprime en mémoire
		free(currentClient);
	}
}

/**
 * Envoie un message à tous les clients
 * 
 * */
void WriteToAll(ChatMessage *message)
{
	//parcours de la liste des clients et envoie à tout le monde
	for (int i = 0; i < clientCount; i++)
	{
		client *currentClient = ID_Tube_Client[i];
		write((*currentClient).pipeHandle, message, sizeof(ChatMessage));
	}
}

// pour le serveur : il faut créer deux tube, un ou le serveur écoute, et un ou le serveur réécrie ce qu'il à entendu
int main(void)
{
	int ID_Tube_serveur;
	char Tube_serveur[] = "/tmp/serveur.fifo";

	fprintf(stderr, "You are on the server\n");

	//Création du tube serveur 1
	if ((mkfifo(Tube_serveur, 0666) != 0) && (errno != EEXIST))
	{
		fprintf(stderr, "Impossible de créer le tube serveur 1.\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Waiting for first client to connect \n");

	if ((ID_Tube_serveur = open(Tube_serveur, O_RDONLY)) == -1)
	{
		fprintf(stderr, "Impossible d'ouvrir l'entrée du tube serveur 1.\n");
		exit(EXIT_FAILURE);
	}

	while (1) //ici mettre la reception du signal de stop sinon on ne sort jamais
	{
		ChatMessage newMessage;
		//Lecture du tube de réception
		read(ID_Tube_serveur, &newMessage, sizeof(ChatMessage));
		fprintf(stderr, "%s wrote a message : %s \n", newMessage.nickName, newMessage.payLoad);

		if (strlen(newMessage.payLoad) == 0)
		{
			//nouvel utilisateur
			CreateClientPipe(newMessage.nickName);
			//envoi d'un petit message à tout le monde
			ChatMessage welcome;
			strcpy(welcome.nickName, "server");
			sprintf(welcome.payLoad, "Welcome to %s", newMessage.nickName);
			welcome.payLoadLength = strlen(welcome.payLoad);
			WriteToAll(&welcome);
		}
		else
		{
			//on envoit à tout le monde le message
			WriteToAll(&newMessage);
		}
	}

	if (close(ID_Tube_serveur) == -1)
	{
		perror("Erreur fermeture du tube serveur");
		exit(EXIT_FAILURE);
	}

	CleanUpClient();

	fprintf(stderr, "fermeture du serveur\n");

	return EXIT_SUCCESS;
}
