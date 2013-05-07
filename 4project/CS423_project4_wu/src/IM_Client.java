
import java.io.IOException;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 * CS423 Project 4 Spring 2013
 * Client Server Simple IM/trivial FTP in java
 * @author victoria wu
 */

public class IM_Client {
   
    //LOGGER! 
    private static Logger LOGGER = Logger.getLogger("CS423_Project4");
    
    //Vars
    //--------------------------------------------
    private HashMap<String, InetSocketAddress> buddyLog;
    private String username;
    
    private ServerSocket peerListen;
    private Thread peerListener_t;
    
    
    private Socket serverTalker;
    private Thread serverListener_t;
    private PrintWriter talkToServer;
    
    
    //Methods
    //--------------------------------------------
    public IM_Client()  {
        buddyLog = new HashMap();    
    }
    
    /**
     * Connect to server and set up peer listener.
     * @param remoteIP 
     * @param remotePort
     * @param peerListenPortNum
     * @param backLog 
     */
    public void startup(String remoteIP, int remotePort, int peerListenPortNum, int backLog)   {
        try {
            //Start listening for peer requests.
            peerListen = new ServerSocket(peerListenPortNum, backLog);
            peerListener_t = new Thread(new PeerListener());
            peerListener_t.start();
            LOGGER.info(String.format("PeerListener on port %d backlog %d", peerListenPortNum, backLog));
            
            //Start listening for server.
            serverTalker = new Socket();
            serverTalker.connect(new InetSocketAddress(remoteIP, remotePort), 1000);
            talkToServer = new PrintWriter(serverTalker.getOutputStream(), true);   //I"ll need to close this...
            
            serverListener_t = new Thread(new ServerListener());
            serverListener_t.start();
            LOGGER.info(String.format("Connected to server ip: %s, port %d", remoteIP, remotePort));
            
        } catch (IOException ex) {
            LOGGER.log(Level.SEVERE, null, ex);
        }
     
    }
    
    public void logOn(String name)  { 
        username = name;
            
        String msg = String.format("1;%s;%05d#", username, peerListen.getLocalPort());
        LOGGER.log(Level.INFO, "Sent: {0}", msg);
        talkToServer.printf(msg);  
    }
}


