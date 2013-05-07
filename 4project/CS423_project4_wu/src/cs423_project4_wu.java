/**
 * CS423 Project 4 Spring 2013
 * Client Server Simple IM/trivial FTP in java
 * @author victoria wu
 */

import java.util.logging.Logger;

public class cs423_project4_wu {

    private static Logger LOGGER = Logger.getLogger("CS423_Project4");
        
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        String serverName = "134.193.128.197";
        String portNum = "3456";
        
        String LISTEN_PORT = "65501";
        
        
        //specify server and port
        if(args.length == 2)    {
            serverName = args[0];
            portNum = args[1];           
        }
        //default to Cotter's server, port
        else if(args.length == 1)   {
            LISTEN_PORT = args[0];
        }
        else    {  
            System.out.println("No server name and port num specified. Setting to default.");
        }
        String cmdLineArgs = String.format("Server IP: %s PortNum: %s ListenPort: %s", serverName, portNum, LISTEN_PORT);
        LOGGER.info(cmdLineArgs);
    }
}
