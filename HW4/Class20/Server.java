import java.io.*;
import java.net.*;
import java.util.*;

public class Server
{
  public static void main( String[] args )
  {
    new Server();
  }

  public Server()
  {
    System.out.println( "Server started at " + new Date() );

    try
    {
      // Create a server socket that will listen on port 9889
      ServerSocket serverSocket = new ServerSocket( 9889 );
      //                                             ^
      //                                             |
      //  This server process will listen on this port #
      //
      //  For a given machine (IP address), we have 65,536 port
      //    numbers available
      //  ftp   21         ftp://ftp.abc.com/
      //  ssh   22
      //  telnet 23             /etc/services
      //  http  80
      //  https 443        http://xyz.com:8111/woefijweofij
      //  etc.

      while ( true )
      {
        // Listen for a connection request
        Socket socket = serverSocket.accept();   // BLOCK

        System.out.println( "Client connection rcvd at " + new Date() );

        // Create data input and output streams
        //  for primitive Java data types
        DataInputStream inputFromClient =
          new DataInputStream( socket.getInputStream() );
        DataOutputStream outputToClient =
          new DataOutputStream( socket.getOutputStream() );

        // below is the application-level protocol
        // Receive radius from the client
        double radius = inputFromClient.readDouble();   // BLOCK

        // Compute area
        double area = radius * radius * Math.PI;

        // Send area back to the client
        outputToClient.writeDouble( area );

        System.out.println( "Radius received from client: " + radius );
        System.out.println( "Area found: " + area );

        socket.close();
      }
    }
    catch( IOException ex )
    {
      System.err.println( ex );
    }
  }
}
