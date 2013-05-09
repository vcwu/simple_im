
import java.net.InetSocketAddress;

import java.util.Scanner;
import java.util.concurrent.ConcurrentHashMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * CS423 Project 4 Spring 2013
 * Client Server Simple IM/trivial FTP in java
 * @author victoria wu
 * 
 * ServerListener
 * Updates friend list as server sends status messages.
 */


public class ServerListener implements Runnable {
    
    private static Logger LOGGER = Logger.getLogger("CS423_Project4");
    IM_Client cl;
    
    public ServerListener(IM_Client in) {
        cl = in;
    }
    
    public void run()   {
        
        Scanner in = cl.listenToServer.useDelimiter("#");
        ConcurrentHashMap<String, InetSocketAddress> log = cl.buddyLog;

        String meat, name, ip;
        int users,port;
        
        Pattern userListUpdate = Pattern.compile("4;(.*)\n");
        Pattern userInfo = Pattern.compile("(.*);(.*);(.*)");
        Matcher m;
        //ending condition? Needs to end cleanly, otherwise throws exception
        while(in.hasNext()) {
            meat = in.next();
            LOGGER.log(Level.INFO, "Server recved {0}", meat);
            
            m = userListUpdate.matcher(meat);
            
            if(m.lookingAt()) {
                users = Integer.parseInt(m.group(1));
                LOGGER.log(Level.INFO, "Users: {0}", users);
                m = userInfo.matcher(meat);
                if(users > -1)   {
                    while(m.find()) {
                        name = m.group(1);
                        ip = m.group(2);
                        port = Integer.parseInt(m.group(3));

                        log.put(name, new InetSocketAddress(ip, port));
                        LOGGER.log(Level.INFO, "Added {0} to buddylog", name);
                    }
                }
                //Removing users
                else    {
                    while(m.find()) {
                        name = m.group(1);
                        log.remove(name);
                        LOGGER.log(Level.INFO, "Removed {0} from buddylog", name);
                    }
                }
            }   //end type4 message match
        }   //scanner end
        
   }
}
