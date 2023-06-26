#ifndef __CLIENTE_H__
#define __CLIENTE_H__

#include <linux/socket.h>

#define PORTA 8008

u32 criaEndereco(u8 * ip);
int enviarMensagem(struct socket * localSocket, const char * mensagem, const size_t tam, unsigned long flags);
int conectarServidor(void);

#endif