package servidorinterface;

import java.net.URL;
import java.util.ResourceBundle;
import javafx.animation.AnimationTimer;
import javafx.event.ActionEvent;
import javafx.event.EventType;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.application.Platform;
import javafx.scene.control.Label;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;
import javafx.scene.control.ListView;
import javafx.stage.Stage;

public class ServidorInterfaceDocumentController implements Initializable {
    
    Servidor servidor;
    private boolean threadMensagem = true;
    @FXML
    private Button iniciar_button;
    
    @FXML
    private Button fechar_button;
    
    @FXML
    private TextField ip_tf;
    
    @FXML
    private TextField porta_tf;
    
    @FXML
    private ListView mensagens_lv;
    
    @FXML
    private void iniciarServidor(ActionEvent event) {
        String ip = ip_tf.getText();
        String porta = porta_tf.getText();
        
        if(!porta.isEmpty() && !ip.isEmpty()){
            System.out.println(ip);
            System.out.println(Integer.parseInt(porta));
            servidor =  new Servidor(Integer.parseInt(porta), ip);
            System.out.println("Servidor: " + servidor);
            try{
                new Thread(()->{
                    if(!servidor.conectado()){
                        adicionarItem(mensagens_lv,"Buscando conexão...");
                        boolean conectado = servidor.conectar();
                        System.out.println("Conectado: " + conectado);
                        if(conectado)
                            //mensagens_lv.getItems().add("Cliente conectado.");
                            adicionarItem(mensagens_lv, "Cliente conectado.");
                        
                        else
                            adicionarItem(mensagens_lv, "Cliente não conectado.");
                    }
                    fechar_button.disableProperty().set(false);
                    recebeMensagens.start();
                }).start();
            }
            catch(Exception e){
                
            }
            iniciar_button.disableProperty().set(true);
        }
        else
            adicionarItem(mensagens_lv, "Selecione o IP e a porta!");
        
    }
    
    @FXML
    public void fecharServidor(ActionEvent event){
        servidor.fechar();
        servidor = null;
        threadMensagem = false;
        adicionarItem(mensagens_lv, "Servidor fechado!");
        fechar_button.disableProperty().set(true);
        iniciar_button.disableProperty().set(false);
        Stage stage = (Stage) iniciar_button.getScene().getWindow();
        stage.close();
    }
    
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        
    }    
    
    public void adicionarItem(ListView list, String mensagem){
        list.getItems().add(0, mensagem);
    }
    
    Thread recebeMensagens =  new Thread(()->{
        System.out.println("ESTOU OUVINDO!!!");
        while(servidor.conectado()){
            String mensagem = servidor.receber();
            if(mensagem.compareTo("NULL") != 0)
                adicionarItem(mensagens_lv, mensagem);
            
        }
        System.out.println("NÃO ESTOU MAIS OUVINDO!!!");
    });

    
}
