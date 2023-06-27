# **Módulo kernel escaneador de teclado com servidor TCP.**

Trabalho da disciplina de Sistemas Operacionais I (SSC0640), lecionada pelo Docente Vanderlei Bonato, para o curso de Engenharia de Computação - USP São Carlos.


Por: **Caio O. Godinho**, **Hugo H. Nakamura** e **Isaac S. Soares**.

---

**Esse código foi realizado e executado na distribuição de Linux Ubuntu 23.04. Em outros sistemas derivados, algumas execuções podem não funcionar!**

## **1. Composição do projeto.**
O projeto é composto por programas em *kernel space* e *user space*. O módulo kernel, responsável por escanear a entrada de teclado no sistema e enviar a um servidor, é inteiramente composto por C. A implementação do servidor e sua interface foi feita a partir de Java 8, utilizando as bibliotecas do JavaFX.

## **2. Requisitos**
* Arquivos headers de kernel;
* JDK Java 8 com javaFX;
* Apache ant;

## **3. Instalação dos requisitos.**

A execução do servidor depende do compilador Apache Ant e do pacote de desenvolvedor Java JDK 8. Caso você não possua instalado, basta copiar o código abaixo no terminal e executar.

```
$ sudo apt install default-jdk
$ sudo apt install ant
```

Além disso, é preciso obter as bibliotecas que lidam com a execução de programas em kernel. Para saber se essas bibliotecas estão instaladas no seu sistema, execute

```
$ uname -r
$ apt search linux-headers-$(uname -r)
```

Caso você não tenha as bibliotecas, execute

```
$ sudo apt-get install build-essential linux-headers-`uname -r`
```

## **Guia de execução.**

1. Compile todos os arquivos executando:
```
$ make
```
2. Abra o servidor executando:

```
$ make start*
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

