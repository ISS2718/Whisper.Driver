#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/socket.h>
#include <linux/slab.h>

#include "cliente.h"

struct socket * socket = NULL;

u32 criaEndereco(u8 *ip){
    u32 addr = 0;
    int i;

    for(i = 0; i<4; i++){
        addr += ip[i];
        if(i==3)
            break;
        addr <<= 8;
    }

    return addr;
}

int enviarMensagem(struct socket *localSocket, const char * mensagem, const size_t tam, unsigned long flags){
    struct msghdr msg;
    struct kvec vec;
    int len, written = 0, left = tam;

    //mm_segment_t oldmm;

    msg.msg_name = 0;
    msg.msg_namelen = 0;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = flags;

    //oldmm = get_fs();
    //set_fs(KERNEL_DS);
    
    while(1){
        vec.iov_len = left;
        vec.iov_base = (char *)mensagem + written;
        len = kernel_sendmsg(localSocket, &msg, &vec, left, left);
        if(len == -ERESTARTSYS || !(flags & MSG_DONTWAIT) && len == -EAGAIN)
            continue;
        if(len > 0){
            written += len;
            left -= len;
            if(left)
                continue;

        }
        //set_fs(oldmm);
        break;
    }
    return written ? written:len;
}

int conectarServidor(void){
    struct sockaddr_in endereco;
    unsigned char ip[5] = {127,0,0,1,'\0'};


    int len = 63;
    char reply[64];
    int erro = -1;

    DECLARE_WAIT_QUEUE_HEAD(filaEspera);

    erro = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &socket);
    if(erro < 0){
        printk(KERN_ERR "Erro ao criar o socket.\n");
        return -1;
    }
    memset(&endereco, 0, sizeof(endereco));

    endereco.sin_family = AF_INET;
    endereco.sin_port = htons(PORTA);
    endereco.sin_addr.s_addr = htonl(criaEndereco(ip));

    erro = socket->ops->connect(socket, (struct sockaddr*)&endereco, sizeof(endereco), O_RDWR);
    if(erro && erro != -EINPROGRESS){
        printk(KERN_ERR "Erro ao conectar socket.\n");
        return -1;
    }
    
    printk(KERN_INFO "Conectado ao servidor.\n");

    memset(&reply, 0, len+1);
    strcat(reply, "HOLA\n");
    enviarMensagem(socket, reply, strlen(reply), MSG_DONTWAIT);

    return 0;
}