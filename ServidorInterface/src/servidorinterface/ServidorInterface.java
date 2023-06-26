package servidorinterface;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

/**
 *
 * @author ikuyorih9
 */
public class ServidorInterface extends Application {
    
    @Override
    public void start(Stage stage) throws Exception {
        
        FXMLLoader loader = new FXMLLoader(getClass().getResource("ServidorInterfaceDocument.fxml"));
        Parent root = loader.load();
        
        Scene scene = new Scene(root);
        
        ServidorInterfaceDocumentController Controller = loader.getController();
        Controller.fecharServidor(stage);
        
        String css = getClass().getResource("styles.css").toExternalForm();
        scene.getStylesheets().add(css);
        
        stage.setScene(scene);
        stage.setTitle("Servidor");
        stage.setResizable(false);
        
        stage.show();
    }


    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        launch(args);
    }
    
}
