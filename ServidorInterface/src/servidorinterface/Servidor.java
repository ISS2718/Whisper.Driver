package servidorinterface;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Servidor {
    
    ServerSocket servidor;
    Socket socket;
    BufferedReader inCliente;
    
    
    String ip;
    int porta;
    
    public Servidor(int porta, String ip){
        try{
            this.ip = ip;
            this.porta = porta;
            servidor = new ServerSocket(porta, 0,InetAddress.getByName(ip));
            
        }
        catch(IOException e){
            System.out.println(e.getMessage());
        }
        
    }
    
    public boolean conectar(){
        try{
            System.out.println("Tentando conectar...");
            socket = servidor.accept();
            inCliente = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        }
        catch(IOException e){
            System.out.println("Erro ao conectar." + e.getCause());
            return false;
        }
        return true;
    }
    
    public String receber(){
        String mensagem = "NULL";
        try{
            mensagem = inCliente.readLine();
            System.out.println("Mensagem: "+ mensagem);
        }
        catch(IOException e){
            System.out.println("Buffer de mensagem do cliente não foi inicializado.");
        }
        return mensagem;
    }
    
    public void fechar(){
        try{
            socket.close();
            socket = null;
            servidor.close();
            servidor = null;
            inCliente.close();
        }
        catch(IOException e){
            System.out.println("Erro ao fechar servidor.");
        }
        
    }
    
    public boolean conectado(){
        if(socket != null)
            return socket.isConnected();
        else
            return false;
    }
    
    
}
