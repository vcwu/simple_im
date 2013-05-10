/**
 * CS423 Project 4 Spring 2013
 * Client Server Simple IM/trivial FTP in java
 * @author victoria wu
 */

import java.util.Scanner;
import java.util.logging.Logger;

public class cs423_project4_wu {

    private static Logger LOGGER = Logger.getLogger("CS423_Project4");
        
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        //Getting serverIP, port, and listenPort.
        //--------------------------------------------
        String serverName = "134.193.128.197";
        int portNum = 3456;
        
        int listenPort = 65501;
        int backLog = 5;
        
        //specify server and port
        if(args.length == 2)    {
            serverName = args[0];
            portNum = Integer.parseInt(args[1]);           
        }
        //default to Cotter's server, port
        else if(args.length == 1)   {
            listenPort = Integer.parseInt(args[0]);
        }
        else    {  
            System.out.println("No server name and port num specified. Setting to default.");
        }
        String cmdLineArgs = String.format("Server IP: %s PortNum: %s ListenPort: %s", serverName, portNum, listenPort);
        LOGGER.info(cmdLineArgs);
        
        //Starting up, logging on.
        //--------------------------------------------        
        IM_Client cl = new IM_Client();
        
        cl.startup(serverName, portNum, listenPort, backLog);
       
        Scanner in = new Scanner(System.in);
        System.out.println("What is your username?");
        String userName = in.next();
        cl.logOn(userName);
        
        String input ="";
        Boolean userContinue = true;
        while(userContinue) {
            cl.displayMenu();
            input = in.next();
            switch(input.charAt(0)) {
                case 's': cl.sendMessage(); break;
                case 'f': cl.getFileNames(); break;
                case 'd': cl.downloadFile(); break;
                case 'q': userContinue = false; break;
                case 'b': cl.listBuddies(); break;
                default: break;
            }
        }
        
        cl.shutdown();
        System.out.println("Bye!");
    }
}
