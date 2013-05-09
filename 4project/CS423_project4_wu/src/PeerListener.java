/**
 * CS423 Project 4 Spring 2013
 * Client Server Simple IM/trivial FTP in java
 * @author victoria wu
 */
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


/*
 * PeerListener
 * Listens for incoming msgs from other peers.
 */


public class PeerListener implements Runnable {
    
    private static Logger LOGGER = Logger.getLogger("CS423_Project4");
    IM_Client cl;
    
   // private PrintWriter output;
   // private Scanner input;
    
    public PeerListener(IM_Client in) {
        cl = in;
    }
    public void run()   {
        
        try {
            LOGGER.info("Running Peer Listener!");
            ServerSocket listen = cl.peerListen;
            //need better ending case
            LOGGER.info("Entering while loop");
            Socket talker = null;
            PeerConnection peer = null;
            while(true) {
                talker = listen.accept();
                peer = new PeerConnection(talker);
                Thread t = new Thread(peer);
                t.start();
                LOGGER.info("GOT a talker!!");
            }
        }   //catch socket close exceptiON?? I'm so confused D: - 
            //SocketException socket closed, when quitting
        catch (IOException ex) {
            LOGGER.log(Level.SEVERE, null, ex);
        }
   }
 
}


class PeerConnection implements Runnable    {
    
    
    private static Logger LOGGER = Logger.getLogger("CS423_Project4");
    
    private Socket incoming;
    private PrintWriter outputToSocket;
    private Scanner inputFromSocket;
    
    public PeerConnection(Socket talker) {
        incoming = talker;
    }
    
    public void run()    {
        try {
            outputToSocket = new PrintWriter(incoming.getOutputStream(), true);
            inputFromSocket = new Scanner(new BufferedReader(new InputStreamReader(incoming.getInputStream())));
            inputFromSocket.useDelimiter("#");
            
            
            System.out.println("Recieved message!");
            
            String meat = inputFromSocket.next();
            System.out.println("GOT FROM PEER: " + meat);
            
            switch(meat.charAt(0))  {
                case ('2'):
                    System.out.println("New Message!" + meat);
                    break;
                case ('5'):
                    System.out.println("File list requested.");
                    sendFileList();
                    break;
                case('6'):
                    System.out.println("File requested");
                    sendFile();
                    break;
            }
            
            
            outputToSocket.close();
            inputFromSocket.close();
            
        } catch (IOException ex)    {
            //stuff
            LOGGER.log(Level.SEVERE, null ,ex);
        }
    }
    
    /*
    * Find the files.
    * Then, make the message to get ready for sending
    */
   public String getFileListPayload() {
       
       File fileFolder = new File("../files");
       File[] listOfFiles = fileFolder.listFiles();
       int numFiles = listOfFiles.length;
       StringBuilder list = new StringBuilder();
       
       for(int i = 0; i< numFiles -1; i++)   {
           list.append(listOfFiles[i].getName());
           list.append("\n");           
       }
       list.append(listOfFiles[numFiles-1].getName());
       list.append("#");
       String payload = String.format("ack;%d\n%s", numFiles, list.toString());
       return payload;
   }
    
   public void sendFileList() {
       System.out.println("look at the pretty FILES!");
       String payload = getFileListPayload();
       
       LOGGER.log(Level.INFO, "Sending fileList{0}", payload);
       outputToSocket.print(payload);
   }
   
   public void sendFile()   {
       
   }
   
}
