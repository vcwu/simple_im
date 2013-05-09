
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;
import java.util.concurrent.ConcurrentHashMap;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 * CS423 Project 4 Spring 2013
 * Client Server Simple IM/trivial FTP in java
 * @author victoria wu
 * 
 * TODO: Handle error when using multiple username
 */

public class IM_Client {
   
    //LOGGER! 
    private static Logger LOGGER = Logger.getLogger("CS423_Project4");
    
    //Vars
    //--------------------------------------------
    protected ConcurrentHashMap<String, InetSocketAddress> buddyLog;
    private String username;
    
    private ServerSocket peerListen;
    private Thread peerListener_t;
    
    
    protected Socket serverTalker;
    protected Thread serverTalker_t;
    protected PrintWriter talkToServer;
    protected Scanner listenToServer;
    
    
    //Methods
    //--------------------------------------------
    public IM_Client()  {
        buddyLog = new ConcurrentHashMap();    
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
            //PEER TO PEER COMMUNICATION
            //----------------------------------------
            peerListen = new ServerSocket(peerListenPortNum, backLog);
            peerListener_t = new Thread(new PeerListener());
            peerListener_t.setName("Peer Listener");
            peerListener_t.start();
            LOGGER.info(String.format("PeerListener on port %d backlog %d", peerListenPortNum, backLog));
            
            //SERVER COMMUNICATION
            //----------------------------------------
            
            serverTalker = new Socket();
            serverTalker.connect(new InetSocketAddress(remoteIP, remotePort), 1000);
            
            talkToServer = new PrintWriter(serverTalker.getOutputStream(), true);   //I"ll need to close this...
            listenToServer = new Scanner(new BufferedReader(new InputStreamReader(serverTalker.getInputStream())));
            
            serverTalker_t = new Thread(new ServerListener(this));
            serverTalker_t.setName("Server Listener");
            serverTalker_t.start();
            LOGGER.info(String.format("Connected to server ip: %s, port %d", remoteIP, remotePort));
            
        } catch (IOException ex) {
            LOGGER.log(Level.SEVERE, null, ex);
        }
     
    }

    public void shutdown()  {
        try {
            //Gracefully shut down threads.
            //Gracefully shut down ServerTalker socket, and PeerListen socket.??
            talkToServer.close();
            serverTalker.close();
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
    
    /**
     * Send a message to a buddy.
     */
    public void sendMessage()   {
        String recipient, msg;
        System.out.println("Recipient: ");
        Scanner in = new Scanner(System.in);
        recipient = in.next();
        System.out.println("Message: ");
        msg = in.next();
        
        String outmsg = String.format("2;%s\n%s\n%s#", username, recipient, msg);
        
        if(buddyLog.containsKey(recipient)) {
            try {
                InetSocketAddress buddy = buddyLog.get(recipient);
                Socket talker = new Socket(buddy.getAddress(), buddy.getPort());
                PrintWriter out = new PrintWriter(serverTalker.getOutputStream(), true);
                out.print(outmsg);
                System.out.println("Successfully sent message to "  + recipient);
            } catch (IOException ex) {
                LOGGER.log(Level.SEVERE, null, ex);
            }
        }
        else    {
            System.out.println(recipient + " is not logged in.");
        }
        
    }
    
    /**
     * Get file names from a buddy.
     */
    public void getFileNames()  {
        
    }
    
    /**
     * Download a file from a buddy.
     */
    public void downloadFile()  {
    
    }
    public void displayMenu()   {
        System.out.println("What would you like to do?");
        System.out.println("\tSend msg to buddy (s)");
        System.out.println("\tGet file list from buddy (f)");
        System.out.println("\tDownload file from buddy (d)");
        System.out.println("\tQuit (q)");
    }
}


