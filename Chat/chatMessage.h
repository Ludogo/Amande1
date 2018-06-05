
/*
Message du chat. Cette structure permet de stocker les info du protocole de com du chat
*/
struct ChatMessage
{
    /*
    pseudo du client
    */
    char nickName[256];
    /*
    taille du message
    */
    int payLoadLength;
    /*
    le message à afficher (max 2048)
    */
    char payLoad[2048];
};

typedef struct ChatMessage ChatMessage;