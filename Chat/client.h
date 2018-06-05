
/**
 * Permet de mémoriser les infos côté serveur d'un client du chat
 * */
struct client
{
    /**
     * Handle unix du named pipe
     * */
    int pipeHandle;

    /**
     * Pseudo du client
     * */
    char nickName[256];

    /**
    * Nom du pipe du client
    *  */
    char tubeName[260];
   
};

typedef struct client client;